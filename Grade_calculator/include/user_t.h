#ifndef USER_H
#define USER_H

#include <stdbool.h>
#include <sodium.h>

#define cmd_len 128

typedef struct
{
    char username[cmd_len];
    char password[crypto_pwhash_STRBYTES];
    unsigned int attempt_remainig;
    bool is_admin;
    bool is_logged_in;
} user_t;

#endif