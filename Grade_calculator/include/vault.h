#ifndef VAULT_H
#define VAULT_H

#include "user_t.h"

// Generates the admin's key-escrow keypair, protecting the private key
// with a key derived from the admin's own password. Called once, when
// the first (admin) user is created.
bool admin_keybox_create(admin_keybox_t *box, const char *admin_password);

// Encrypts and stores plain into user's vault, key-escrowed to both the
// user's own password and the admin's public key. Used both to create a
// brand new (empty) vault and to save edited student data.
bool user_vault_save(user_t *user, const char *password, const admin_keybox_t *admin_box, const student_data_t *plain);

// Convenience wrapper: seals a fresh, empty student data vault for a new user.
bool user_vault_init(user_t *user, const char *password, const admin_keybox_t *admin_box);

// Decrypts a user's own student data using their own password.
bool user_vault_open_as_owner(const user_t *user, const char *password, student_data_t *out_plain);

// Decrypts any user's student data using the admin's password (escrow recovery).
bool user_vault_open_as_admin(const user_t *user, const admin_keybox_t *admin_box, const char *admin_password, student_data_t *out_plain);

// Resets a user's login password. Requires the admin's own password to
// unlock the escrow copy of the user's data key so their existing
// grades survive the reset instead of becoming unrecoverable.
bool admin_reset_user_password(user_t *user, const admin_keybox_t *admin_box, const char *admin_password, const char *new_password);

#endif
