#include "user_t.h"
#include "command.h"
#include "file_op.h"
#include "pass.h"
#include "vault.h"

// this function cleans the memory reserved for the user database.
void user_db_init(user_dbt_t *user_db)
{
    if (user_db == NULL)
    {
        return;
    }
    user_db->count = 0;
    user_db->admin_box_ready = false;
    memset(&user_db->admin_box, 0, sizeof(user_db->admin_box));
    for (int i = 0; i < MAX_USER; i++)
    {
        memset(&user_db->users[i], 0, sizeof(user_t));
        user_db->users[i].attempts_remaining = MAX_ATTEMPT;
        user_db->users[i].is_admin = false;
    }
}

// account function
int user_create(user_dbt_t *db, const char *username, const char *password)
{
    if ((db == NULL) || (username == NULL) || (password == NULL))
    {
        return -1;
    }
    user_t new_user;
    memset(&new_user, 0, sizeof(new_user));

    if (db->count >= MAX_USER)
    {
        printf("the database is full, contact admin for removing one of the old users. \n");
        return 1;
    }

    // checking if the user already exist
    for (unsigned int i = 0; i < db->count; i++)
    {
        if (strcmp(username, db->users[i].username) == 0)
        {
            printf("the user already exists, please enter a unique username. \n");
            return 1;
        }
    }

    // strcpy(new_user.username, username); //this is unsafe.
    strncpy(new_user.username, username, NAME_LEN - 1);
    new_user.username[NAME_LEN - 1] = '\0';

    // hashing the password
    if (crypto_pwhash_str(new_user.password, password, strlen(password), crypto_pwhash_OPSLIMIT_INTERACTIVE, crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0)
    {
        printf("the password hashing failed. \n");
        return -1;
    }

    new_user.attempts_remaining = MAX_ATTEMPT;
    new_user.is_admin = (db->count == 0);

    if (db->count == 0)
    {
        // this is the first user: also set up the admin key-escrow box,
        // protected by this same password.
        if (!admin_keybox_create(&db->admin_box, password))
        {
            printf("failed to initialize the admin key vault. \n");
            return -1;
        }
        db->admin_box_ready = true;
    }

    if (!db->admin_box_ready || !user_vault_init(&new_user, password, &db->admin_box))
    {
        printf("failed to initialize the user's grade vault. \n");
        return -1;
    }

    db->users[db->count++] = new_user;
    return 0;
}

user_t *user_login(user_dbt_t *user_db, const char *username, const char *password) // it returns user_t*
{
    if ((user_db == NULL) || (username == NULL) || (password == NULL))
    {
        printf("invlid username / password / database pointer. \n");
        return NULL;
    }

    user_t *user = find_user_in_db(user_db, username);
    if (user == NULL)
    {
        printf("the user was not found in the database. \n");
        return NULL;
    }

    // checking if the attempt is remaining
    if (user->attempts_remaining == 0)
    {
        printf("you have entered wrong password 3 time, for reseting the attempt contact the admin. \n");
        return NULL;
    }

    // checking if the password is right
    if (crypto_pwhash_str_verify(user->password, password, strlen(password)) != 0)
    {
        user->attempts_remaining--;
        printf("you entered the wrong password. %u attempt(s) remaining. \n", user->attempts_remaining);
        return NULL;
    }
    else
    {
        printf("login successfull. \n");
        return user;
    }
}

void user_logout(user_t **current_user)
{
    if (current_user == NULL)
    {
        return;
    }
    else
    {
        *current_user = NULL;
    }
}

// helper functions
user_t *find_user_in_db(user_dbt_t *user_db, const char *username)
{
    if ((user_db == NULL) || (username == NULL))
    {
        printf("invalid database or username. \n");
        return NULL;
    }
    for (unsigned int i = 0; i < user_db->count; i++)
    {
        if (!strncmp(user_db->users[i].username, username, NAME_LEN - 1))
        {
            printf("the user has been found. \n");
            return &user_db->users[i];
        }
    }
    printf("the user was no found in the database. \n");
    return NULL;
}

bool is_user_admin(const user_t *user)
{
    return user->is_admin;
}

bool is_user_db_full(const user_dbt_t *user_db)
{
    if (user_db == NULL)
    {
        printf("invalid input. \n");
        return true;
    }
    if (user_db->count == MAX_USER)
    {
        printf("the database is full. \n");
        return true;
    }
    else
    {
        return false;
    }
}

void print_database(user_dbt_t *db)
{
    for (unsigned int i = 0; i < db->count; i++)
    {
        printf("username : %s \n", db->users[i].username);
        printf("password : %s \n", db->users[i].password);
        printf("attempts remaining : %d \n", db->users[i].attempts_remaining);
        printf("is_admin : %d \n\n", db->users[i].is_admin);
    }
}

// lists accounts without exposing password hashes - suitable for admin use.
void print_user_summary(const user_dbt_t *db)
{
    if (db == NULL || db->count == 0)
    {
        printf("no users in the database. \n");
        return;
    }
    for (unsigned int i = 0; i < db->count; i++)
    {
        printf("%u. %-20s %s attempts remaining: %u \n", i + 1, db->users[i].username,
               db->users[i].is_admin ? "[admin]" : "", db->users[i].attempts_remaining);
    }
}

bool user_delete_by_username(user_dbt_t *db, const char *username)
{
    user_t *found = find_user_in_db(db, username);
    if (found == NULL)
    {
        return false;
    }

    unsigned int idx = (unsigned int)(found - db->users);
    for (unsigned int i = idx; i + 1 < db->count; i++)
    {
        db->users[i] = db->users[i + 1];
    }
    db->count--;
    memset(&db->users[db->count], 0, sizeof(user_t));
    db->users[db->count].attempts_remaining = MAX_ATTEMPT;
    return true;
}
