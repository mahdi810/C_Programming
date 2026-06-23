#include <stdio.h>
#include <sodium.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

#ifdef _WIN32
#include <conio.h>
#include <sys/stat.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

// #defintions
#define cmd_len 128
#define MAX_USER 32
#define USER_FILE "user.txt"
#define SALT_BYTES crypto_pwhash_SALTBYTES
#define KEY_BYTES crypto_secretbox_KEYBYTES

typedef struct
{
    char username[64];
    char password[crypto_pwhash_STRBYTES];
    bool is_admin;
    bool is_logged_in;
    unsigned int attempt_no;
} user_t;

void cmd_clean(char *cmd)
{
    char *chp = cmd;
    while (*chp != '\0')
    {
        if (*chp == '\r' || *chp == '\n')
        {
            *chp = '\0';
            break;
        }
        chp++;
    }
}

void get_password(char *password, size_t max_len)
{
    size_t i = 0;
#ifdef _WIN32
    char ch;
    while (i < max_len - 1)
    {
        ch = _getch();
        if (ch == '\r' || ch == '\n')
        {
            ch = '\0';
            break;
        }
        else if (ch == '\b')
        {
            if (i > 0)
            {
                i--;
                printf("\b \b");
            }
        }
        else
        {
            password[i++] = ch;
            _putch('*');
        }
        fflush(stdout);
        password[i] = '\0';
    }
    _putch('\r');
    _putch('\n');
#else
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ECHO); /* disable echo, keep line buffering (ICANON) */
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    if (fgets(buffer, max_len, stdin) != NULL)
    {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n')
        {
            buffer[len - 1] = '\0';
        }
    }
    else
    {
        buffer[0] = '\0';
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt); /* restore echo */
    printf("\n");
#endif
}

int add_user(user_t user, user_t *user_pool, int *index)
{
    if (*index > MAX_USER)
    {
        printf("It is not possible to add new user, the maximum number is reached. \n");
        return 1;
    }
    else
    {
        for (int i = 0; i <= *index; i++)
        {
            if (!strcmp(user.username, user_pool[i].username))
            {
                printf("the username already exists. \n");
                return 1;
            }
        }
        user_pool[(*index)++] = user;
        return 0;
    }
}

void print_login_info()
{
    printf("1. login \n");
    printf("2. create account \n");
    printf("3. print all the user. \n\n");
    printf("4. to exit the program \n");

    // this will later deleted.
}

void print_help()
{
    printf("help : to show this help message \n");
    printf("exit : to exit the program \n");
    printf("start : to start the algorithm \n");
}

void print_user_data(user_t user)
{
    printf("username : %s \n", user.username);
    printf("password (hashed) : %s \n", user.password);
    printf("is_admin : %d \n", user.is_admin);
    printf("is_logged_in : %d \n", user.is_logged_in);
    printf("remaining attempts : %u \n\n", user.attempt_no);
}

static int derive_key(unsigned char *key, unsigned char *master_pass, const unsigned char *salt)
{
    return crypto_pwhash(key, KEY_BYTES, (const char *)master_pass, strlen((const char *)master_pass), salt, crypto_pwhash_OPSLIMIT_INTERACTIVE, crypto_pwhash_MEMLIMIT_INTERACTIVE, crypto_pwhash_ALG_DEFAULT);
}

void save_user_data_to_file(unsigned char *master_pass, user_t *user_pool, unsigned int no_of_user)
{
    // generating the salt
    unsigned char salt[SALT_BYTES];
    randombytes_buf(salt, SALT_BYTES);

    // getting an encryptiong key from the master password and the salt.
    unsigned char key[KEY_BYTES];
    if (derive_key(key, master_pass, salt) != 0)
    {
        printf("failed to derive the key: %s \n", strerror(errno));
        return;
    }

    // making a plain text buffer to hold the user data.
    // and copying the user data to the plain text buffer.
    unsigned char plain_text[sizeof(unsigned int) + sizeof(user_t) * MAX_USER];
    unsigned int plain_text_len = sizeof(no_of_user) + sizeof(user_t) * no_of_user;
    memcpy(plain_text, &no_of_user, sizeof(no_of_user));
    memcpy(plain_text + sizeof(no_of_user), user_pool, sizeof(user_t) * no_of_user);

    // generating the nonce for the encryption.
    unsigned char nonce[crypto_secretbox_NONCEBYTES];
    randombytes_buf(nonce, sizeof(nonce));

    // encrypting the plain text buffer using the key and the nonce.
    unsigned char cipher_text[sizeof(unsigned int) + sizeof(user_t) * MAX_USER + crypto_secretbox_MACBYTES];
    size_t cipher_len = plain_text_len + crypto_secretbox_MACBYTES;
    crypto_secretbox_easy(cipher_text, plain_text, plain_text_len, nonce, key);

    // writing the salt, nonce and the cipher text to a file.
    FILE *fd;
    fd = fopen(USER_FILE, "wb"); // opens the file, creates if not exist and make it ready for binary writing
    if (fd == NULL)
    {
        printf("could not open the file for saving user data : %s \n", strerror(errno));
        return;
    }
    else
    {
        fwrite(salt, 1, sizeof(salt), fd);
        fwrite(nonce, 1, sizeof(nonce), fd);
        fwrite(cipher_text, 1, cipher_len, fd);
        fclose(fd);

        printf("%s \n", salt);
        printf("%s \n", nonce);
        printf("%s \n", cipher_text);

#ifndef _WIN32
        chmod(USER_FILE, S_IRUSR | S_IWUSR);
#endif
    }

    // erasing sensitive information
    sodium_memzero(key, sizeof(key));
    sodium_memzero(plain_text, sizeof(plain_text));
}

static int load_user_data_from_file(unsigned char *master_pass, user_t *user_pool, unsigned int *no_of_user)
{
    // open the file
    FILE *fd = fopen(USER_FILE, "rb");
    if (fd == NULL)
    {
        printf("the stored file couldnot be opened : %s \n", strerror(errno));
        return 1;
    }
    else
    {
        // seek to the end of the file to get the total size
        if (fseek(fd, 0, SEEK_END) != 0)
        {
            printf("seek failed : %s \n", strerror(errno));
            return -1;
        }

        // getting the current position of the file.
        unsigned long int file_size = ftell(fd);

        // go back to the start of the file
        rewind(fd);

        // check if the file is large enough to contain everything
        size_t header_size = SALT_BYTES + crypto_secretbox_NONCEBYTES;
        if ((size_t)file_size < header_size)
        {
            printf("data is too small, or corrupted : %s \n", strerror(errno));
            fclose(fd);
            return 1;
        }

        unsigned char salt[SALT_BYTES];
        unsigned char nonce[crypto_secretbox_NONCEBYTES];
        if (fread(salt, 1, SALT_BYTES, fd) != SALT_BYTES)
        {
            printf("failed to read the salt : %s \n", strerror(errno));
            fclose(fd);
            return 1;
        }

        if (fread(nonce, 1, crypto_secretbox_NONCEBYTES, fd) != crypto_secretbox_NONCEBYTES)
        {
            printf("failed to read the nonce : %s \n", strerror(errno));
            fclose(fd);
            return 1;
        }

        size_t cipher_len2 = (size_t)file_size - header_size;

        // preparing the buffer text for reading the cipher text
        unsigned char cipher_text[sizeof(unsigned int) + sizeof(user_t) * MAX_USER + crypto_secretbox_MACBYTES];

        // check the length
        if (cipher_len2 > sizeof(cipher_text))
        {
            printf("the file is larger than expected : %s \n", strerror(errno));
            fclose(fd);
            return 1;
        }

        // reading the cipher text
        if (fread(cipher_text, 1, cipher_len2, fd) != cipher_len2)
        {
            printf("failed to read the cipher text : %s \n", strerror(errno));
            fclose(fd);
            return 1;
        }

        // after reading closing the fd
        fclose(fd);

        // deriving the key
        unsigned char key[KEY_BYTES];
        if (derive_key(key, (unsigned char *)master_pass, salt) != 0)
        {
            printf("failed to derive the key : %s \n", strerror(errno));
            return 1;
        }

        // creating the plain text for decryption
        unsigned char plain_text_for_decryption[sizeof(unsigned int) + sizeof(user_t) * MAX_USER];

        // decrypting the cipher text
        if (crypto_secretbox_open_easy(plain_text_for_decryption, cipher_text, cipher_len2, nonce, key) != 0)

        {
            printf("failed to decrypt the file : %s \n", strerror(errno));
            sodium_memzero(key, sizeof(key));
            return 1;
        }

        if (*no_of_user > MAX_USER)
        {
            printf("the decryption is invalid : %s \n", strerror(errno));
            sodium_memzero(key, sizeof(key));
            sodium_memzero(plain_text_for_decryption, sizeof(plain_text_for_decryption));
            return 1;
        }

        memcpy(user_pool, plain_text_for_decryption + sizeof(unsigned int), sizeof(user_t) * (*no_of_user));

        sodium_memzero(key, sizeof(key));
        sodium_memzero(plain_text_for_decryption, sizeof(plain_text_for_decryption));

        return 0;
    }
}

int main(void)
{
    if (sodium_init() < 0)
        return 1;

    int terminate = 0;
    char cmd[cmd_len];
    user_t user_pool[MAX_USER] = {0};
    unsigned int no_of_user = 0;
    unsigned char master_password[128] = "master_pass";

    // print_login_info();
    do
    {
        print_login_info();
        printf(">> ");
        fflush(stdout);
        fgets(cmd, cmd_len, stdin);
        cmd_clean(cmd);
        int choice;
        if (sscanf(cmd, "%d", &choice) != 1)
        {
            printf("you did not enter a valid input. \n");
        }
        else
        {
            if (choice == 1)
            {
                printf("login \n");
                if (!load_user_data_from_file(master_password, user_pool, &no_of_user))
                {
                    printf("the loading was not sucessfull : %s \n", strerror(errno));
                    return 1;
                }
            }
            else if (choice == 2)
            {
                char plain_password[128];
                printf("create account \n");
                user_t new_user;
                printf("You are about the create a new user. \n");
                printf("enter your user name : ");
                fgets(cmd, cmd_len, stdin);
                cmd_clean(cmd);
                if (strlen(cmd) == 0)
                {
                    printf("username cannot be empty. \n");
                    return 1;
                }
                sscanf(cmd, "%s", &new_user.username);

                printf("enter you password : ");
                get_password(&plain_password, sizeof(new_user.password));

                if (strlen(plain_password) == 0)
                {
                    printf("password cannot be empty. \n");
                    return 1;
                }

                if (crypto_pwhash_str(new_user.password, plain_password, strlen(plain_password), crypto_pwhash_OPSLIMIT_INTERACTIVE, crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0)
                {
                    printf("hashing the password failed, not enough memory. \n");
                    sodium_memzero(plain_password, sizeof(plain_password));
                    continue;
                }
                sodium_memzero(plain_password, sizeof(plain_password));

                new_user.attempt_no = 3;
                new_user.is_admin = false;
                new_user.is_logged_in = false;

                if (!add_user(new_user, &user_pool, &no_of_user))
                {
                    printf("Account successfully created. \n\n");
                    print_user_data(new_user);
                }
            }
            else if (choice == 3)
            {
                printf("print all the user names \n");
                printf("all the usernames existing. \n");
                for (int i = 0; i < no_of_user; i++)
                {
                    print_user_data(user_pool[i]);
                }
            }
            else if (choice == 4)
            {
                terminate = 1;
            }
        }
    } while (terminate == 0);

    save_user_data_to_file(master_password, user_pool, no_of_user);
    printf("the data has been saved ");

    printf("hello world \n");

    return 0;
}