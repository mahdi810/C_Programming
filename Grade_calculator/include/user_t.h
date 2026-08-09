#pragma once

#include <stdbool.h>
#include <sodium.h>
#include <string.h>
#include <stdio.h>
#include "date_util.h"

#define NAME_LEN 128
#define ROLL_NUMBER_LEN 64
#define MAX_USER 10
#define MAX_ATTEMPT 3
#define MAX_SUBJECTS 40
#define MAX_EXAMS 30
#define MAX_PROJECTS 30

#define BUDGET_DESC_LEN 64
#define CURRENCY_LEN 8
#define MAX_BUDGET_ACCOUNTS 5
#define MAX_TRANSACTIONS 60
#define MAX_DEBTS 30
#define MAX_DEBT_PAYMENTS 20
#define MAX_FAMILY_TRANSFERS 60
#define MAX_WORK_SHIFTS 120

#define USER_FILE "user.dat"

// a single subject entry: name, German-scale grade, and its ECTS credits.
typedef struct
{
    char name[NAME_LEN];
    double grade;
    unsigned int ects;
} subject_t;

typedef struct
{
    unsigned int count;
    subject_t subjects[MAX_SUBJECTS];
} subject_list_t;

// a single exam or project entry.
typedef struct
{
    char title[NAME_LEN];
    char subject[NAME_LEN];
    date_t date;
    bool done;
} task_t;

// a student's profile. Only students have one - the admin account is
// pure administration, not a student, and never gets a profile.
typedef struct
{
    char full_name[NAME_LEN];
    char roll_number[ROLL_NUMBER_LEN];
    char program[NAME_LEN];
    unsigned int semester;
    date_t enrollment_date;
} student_profile_t;

// a single income or expense entry within one budget account.
typedef enum
{
    TRANSACTION_INCOME,
    TRANSACTION_EXPENSE
} transaction_type_t;

typedef struct
{
    char description[BUDGET_DESC_LEN];
    double amount;
    transaction_type_t type;
    date_t date;
} transaction_t;

// a budget account belongs to one currency (e.g. a "Cash EUR" account, a
// "Bank USD" account) - a student can hold several, each tracked separately.
typedef struct
{
    char name[BUDGET_DESC_LEN];
    char currency[CURRENCY_LEN];
    double opening_balance;
    unsigned int transaction_count;
    transaction_t transactions[MAX_TRANSACTIONS];
} budget_account_t;

// a debt is tracked independently of any account - it's an obligation,
// not yet a completed transaction. Debts can be paid off gradually: each
// installment is recorded so the full payment history is visible.
typedef enum
{
    DEBT_I_OWE,
    DEBT_OWED_TO_ME
} debt_direction_t;

typedef struct
{
    double amount;
    date_t date;
} debt_payment_t;

typedef struct
{
    char counterparty[BUDGET_DESC_LEN];
    char note[BUDGET_DESC_LEN];
    double original_amount;
    double remaining_amount;
    char currency[CURRENCY_LEN];
    debt_direction_t direction;
    date_t due_date;
    bool settled;
    unsigned int payment_count;
    debt_payment_t payments[MAX_DEBT_PAYMENTS];
} debt_t;

// money sent to family, tracked on its own so totals per recipient are
// visible even though (optionally) it's also posted as an expense
// against one of the student's own budget accounts.
typedef struct
{
    char recipient[BUDGET_DESC_LEN];
    char note[BUDGET_DESC_LEN];
    double amount;
    char currency[CURRENCY_LEN];
    date_t date;
} family_transfer_t;

typedef struct
{
    unsigned int count;
    family_transfer_t transfers[MAX_FAMILY_TRANSFERS];
} family_transfer_log_t;

typedef struct
{
    unsigned int account_count;
    budget_account_t accounts[MAX_BUDGET_ACCOUNTS];
    unsigned int debt_count;
    debt_t debts[MAX_DEBTS];
    family_transfer_log_t family_transfers;
} budget_t;

// a single work shift: the date and clock-in/clock-out times.
typedef struct
{
    date_t date;
    time_of_day_t start;
    time_of_day_t end;
    char note[BUDGET_DESC_LEN];
} work_shift_t;

typedef struct
{
    unsigned int count;
    work_shift_t shifts[MAX_WORK_SHIFTS];
} work_log_t;

// everything encrypted at rest in a student's vault: profile, subjects,
// exams, projects, budget and work log, all bundled into one payload.
typedef struct
{
    student_profile_t profile;
    subject_list_t subjects;
    unsigned int exam_count;
    task_t exams[MAX_EXAMS];
    unsigned int project_count;
    task_t projects[MAX_PROJECTS];
    budget_t budget;
    work_log_t work;
} student_data_t;

// the admin's key-escrow box: an X25519 keypair whose private key is
// itself encrypted under a key derived from the admin's own password.
// The public key lets anyone (even without the admin's password) seal
// a copy of a user's data key to the admin, while only the admin (by
// supplying their password to unlock the private key) can open it.
typedef struct
{
    unsigned char public_key[crypto_box_PUBLICKEYBYTES];
    unsigned char encrypted_secret_key[crypto_box_SECRETKEYBYTES + crypto_secretbox_MACBYTES];
    unsigned char secret_key_nonce[crypto_secretbox_NONCEBYTES];
    unsigned char kdf_salt[crypto_pwhash_SALTBYTES];
} admin_keybox_t;

// making the user structure
typedef struct
{
    char username[NAME_LEN];
    char password[crypto_pwhash_STRBYTES];
    unsigned int attempts_remaining;
    bool is_admin;

    // encrypted vault (profile + subjects + exams + projects), stored at
    // rest encrypted with a random per-user data key (DK). DK is never
    // stored in the clear - it exists only twice, wrapped
    // ("key-escrowed"): once under a key derived from this user's own
    // password, and once sealed to the admin's public key. That means
    // only this user (with their password) or the admin (with their
    // password) can ever recover it.
    unsigned char data_cipher[sizeof(student_data_t) + crypto_secretbox_MACBYTES];
    unsigned char data_nonce[crypto_secretbox_NONCEBYTES];

    unsigned char dk_wrap_user[crypto_secretbox_KEYBYTES + crypto_secretbox_MACBYTES];
    unsigned char dk_wrap_user_nonce[crypto_secretbox_NONCEBYTES];
    unsigned char user_kdf_salt[crypto_pwhash_SALTBYTES];

    unsigned char dk_wrap_admin[crypto_secretbox_KEYBYTES + crypto_box_SEALBYTES];
} user_t;

// making the user database structure
typedef struct
{
    user_t users[MAX_USER];
    unsigned int count;
    admin_keybox_t admin_box;
    bool admin_box_ready;
} user_dbt_t;

// initializing the user database
void user_db_init(user_dbt_t *user_db);

// account function
int user_create(user_dbt_t *db, const char *username, const char *password);
user_t *user_login(user_dbt_t *user_db, const char *username, const char *password); // it returns user_t* because after the login, it should return which user is currently logged in.
void user_logout(user_t **current_user);
bool user_delete_by_username(user_dbt_t *db, const char *username);

// helper functions
user_t *find_user_in_db(user_dbt_t *user_db, const char *username);
bool is_user_admin(const user_t *user);
bool is_user_db_full(const user_dbt_t *user_db);
void print_database(user_dbt_t *db);
void print_user_summary(const user_dbt_t *db);
