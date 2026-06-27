#pragma once

#include <stdbool.h>
#include <sodium.h>
#include <string.h>
#include <stdio.h>

#define NAME_LEN 128
#define MAX_USER 10
#define MAX_ATTEMPT 3

// making the user structure
typedef struct
{
    char username[NAME_LEN];
    char password[crypto_pwhash_STRBYTES];
    unsigned int attempts_remaining;
    bool is_admin;
} user_t;

// making the user database structure
typedef struct
{
    user_t users[MAX_USER];
    unsigned int count;
} user_dbt_t;

// initializing the user database
void user_db_init(user_dbt_t *user_db);

// account function
int user_create(user_dbt_t *db, const char *username, const char *password);
user_t *user_login(user_dbt_t *user_db, const char *username, const char *password); // it returns user_t* because after the login, it should return which user is currently logged in.
void user_logout(user_t **current_user);

// helper functions
user_t *find_user_in_db(user_dbt_t *user_db, const char *username);
bool is_user_admin(const user_t *user);
bool is_user_db_full(const user_dbt_t *user_db);
void print_database(user_dbt_t *db);
