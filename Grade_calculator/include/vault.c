#include "vault.h"

static bool derive_key(unsigned char *out_key, size_t out_len, const char *password, const unsigned char *salt)
{
    return crypto_pwhash(out_key, out_len, password, strlen(password), salt,
                          crypto_pwhash_OPSLIMIT_INTERACTIVE, crypto_pwhash_MEMLIMIT_INTERACTIVE,
                          crypto_pwhash_ALG_DEFAULT) == 0;
}

static bool admin_keybox_unlock(const admin_keybox_t *box, const char *admin_password, unsigned char *out_sk)
{
    unsigned char kek[crypto_secretbox_KEYBYTES];
    if (!derive_key(kek, sizeof(kek), admin_password, box->kdf_salt))
    {
        return false;
    }

    bool ok = crypto_secretbox_open_easy(out_sk, box->encrypted_secret_key,
                                          sizeof(box->encrypted_secret_key),
                                          box->secret_key_nonce, kek) == 0;
    sodium_memzero(kek, sizeof(kek));
    return ok;
}

static bool seal_data(user_t *user, const unsigned char *dk, const student_data_t *plain)
{
    randombytes_buf(user->data_nonce, sizeof(user->data_nonce));
    return crypto_secretbox_easy(user->data_cipher, (const unsigned char *)plain, sizeof(student_data_t),
                                  user->data_nonce, dk) == 0;
}

static bool open_data(const user_t *user, const unsigned char *dk, student_data_t *out_plain)
{
    return crypto_secretbox_open_easy((unsigned char *)out_plain, user->data_cipher,
                                       sizeof(user->data_cipher), user->data_nonce, dk) == 0;
}

bool admin_keybox_create(admin_keybox_t *box, const char *admin_password)
{
    if (box == NULL || admin_password == NULL)
    {
        return false;
    }

    unsigned char sk[crypto_box_SECRETKEYBYTES];
    if (crypto_box_keypair(box->public_key, sk) != 0)
    {
        return false;
    }

    randombytes_buf(box->kdf_salt, sizeof(box->kdf_salt));

    unsigned char kek[crypto_secretbox_KEYBYTES];
    if (!derive_key(kek, sizeof(kek), admin_password, box->kdf_salt))
    {
        sodium_memzero(sk, sizeof(sk));
        return false;
    }

    randombytes_buf(box->secret_key_nonce, sizeof(box->secret_key_nonce));
    bool ok = crypto_secretbox_easy(box->encrypted_secret_key, sk, sizeof(sk), box->secret_key_nonce, kek) == 0;

    sodium_memzero(sk, sizeof(sk));
    sodium_memzero(kek, sizeof(kek));
    return ok;
}

bool user_vault_save(user_t *user, const char *password, const admin_keybox_t *admin_box, const student_data_t *plain)
{
    if (user == NULL || password == NULL || admin_box == NULL || plain == NULL)
    {
        return false;
    }

    unsigned char dk[crypto_secretbox_KEYBYTES];
    randombytes_buf(dk, sizeof(dk));

    if (!seal_data(user, dk, plain))
    {
        sodium_memzero(dk, sizeof(dk));
        return false;
    }

    // wrap DK under a key derived from the user's own password
    randombytes_buf(user->user_kdf_salt, sizeof(user->user_kdf_salt));
    unsigned char kek_user[crypto_secretbox_KEYBYTES];
    if (!derive_key(kek_user, sizeof(kek_user), password, user->user_kdf_salt))
    {
        sodium_memzero(dk, sizeof(dk));
        return false;
    }
    randombytes_buf(user->dk_wrap_user_nonce, sizeof(user->dk_wrap_user_nonce));
    if (crypto_secretbox_easy(user->dk_wrap_user, dk, sizeof(dk), user->dk_wrap_user_nonce, kek_user) != 0)
    {
        sodium_memzero(dk, sizeof(dk));
        sodium_memzero(kek_user, sizeof(kek_user));
        return false;
    }
    sodium_memzero(kek_user, sizeof(kek_user));

    // seal DK to the admin's public key (the escrow copy)
    bool ok = crypto_box_seal(user->dk_wrap_admin, dk, sizeof(dk), admin_box->public_key) == 0;

    sodium_memzero(dk, sizeof(dk));
    return ok;
}

bool user_vault_init(user_t *user, const char *password, const admin_keybox_t *admin_box)
{
    student_data_t empty;
    memset(&empty, 0, sizeof(empty));
    bool ok = user_vault_save(user, password, admin_box, &empty);
    sodium_memzero(&empty, sizeof(empty));
    return ok;
}

bool user_vault_open_as_owner(const user_t *user, const char *password, student_data_t *out_plain)
{
    if (user == NULL || password == NULL || out_plain == NULL)
    {
        return false;
    }

    unsigned char kek_user[crypto_secretbox_KEYBYTES];
    if (!derive_key(kek_user, sizeof(kek_user), password, user->user_kdf_salt))
    {
        return false;
    }

    unsigned char dk[crypto_secretbox_KEYBYTES];
    bool ok = crypto_secretbox_open_easy(dk, user->dk_wrap_user, sizeof(user->dk_wrap_user),
                                          user->dk_wrap_user_nonce, kek_user) == 0;
    sodium_memzero(kek_user, sizeof(kek_user));
    if (!ok)
    {
        return false;
    }

    ok = open_data(user, dk, out_plain);
    sodium_memzero(dk, sizeof(dk));
    return ok;
}

bool user_vault_open_as_admin(const user_t *user, const admin_keybox_t *admin_box, const char *admin_password, student_data_t *out_plain)
{
    if (user == NULL || admin_box == NULL || admin_password == NULL || out_plain == NULL)
    {
        return false;
    }

    unsigned char admin_sk[crypto_box_SECRETKEYBYTES];
    if (!admin_keybox_unlock(admin_box, admin_password, admin_sk))
    {
        return false;
    }

    unsigned char dk[crypto_secretbox_KEYBYTES];
    bool ok = crypto_box_seal_open(dk, user->dk_wrap_admin, sizeof(user->dk_wrap_admin),
                                    admin_box->public_key, admin_sk) == 0;
    sodium_memzero(admin_sk, sizeof(admin_sk));
    if (!ok)
    {
        return false;
    }

    ok = open_data(user, dk, out_plain);
    sodium_memzero(dk, sizeof(dk));
    return ok;
}

bool admin_reset_user_password(user_t *user, const admin_keybox_t *admin_box, const char *admin_password, const char *new_password)
{
    if (user == NULL || admin_box == NULL || admin_password == NULL || new_password == NULL)
    {
        return false;
    }

    student_data_t plain;
    if (!user_vault_open_as_admin(user, admin_box, admin_password, &plain))
    {
        return false;
    }

    if (crypto_pwhash_str(user->password, new_password, strlen(new_password),
                           crypto_pwhash_OPSLIMIT_INTERACTIVE, crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0)
    {
        sodium_memzero(&plain, sizeof(plain));
        return false;
    }

    bool ok = user_vault_save(user, new_password, admin_box, &plain);
    sodium_memzero(&plain, sizeof(plain));
    return ok;
}
