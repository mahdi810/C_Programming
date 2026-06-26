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
    printf("3. print all the user. \n");
    printf("4. to exit the program \n\n");

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

void save_user_data_to_file(user_t *userx)
{
    // opening a file
    FILE *fd = fopen(USER_FILE, "wb");
    if (fd == NULL)
    {
        printf("the file could not be opened : %s \n", strerror(errno));
        return 1;
    }

    // writing to a file
    if (fwrite(userx, sizeof(user_t), 1, fd) != 1)
    {
        printf("error writing the data to the file : %s \n", strerror(errno));
        fclose(fd);
        return;
    }
    printf("data written successfully. \n");
    fclose(fd);
}

static int load_user_data_from_file(user_t *user_pool)
{
    // opening the file
    FILE *fd = fopen(USER_FILE, "rb");
    if (fd == NULL)
    {
        printf("error opening the file : %s \n", strerror(errno));
        return 1;
    }
    fread(&user_pool, sizeof(user_pool), 1, fd);
    printf("data has been successfully read from the file. \n");

    return 0;
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
                // this is a test case
                /*
                no_of_user = 6;
                printf("the number of user is : %u \n", no_of_user);
                save_user_data_to_file(&user_pool, no_of_user);
                user_t test_user = {.username = "mahdi",
                                    .password = "hashed_pass",
                                    .attempt_no = 3,
                                    .is_admin = true,
                                    .is_logged_in = false};
                save_user_data_to_file(&test_user,1);
                printf("the no_of_user and the strut was saved to file \n\n");

                user_t test_out_user;
                unsigned int out_no_of_user;
                unsigned int user_no_test;
                load_user_data_from_file(&user_pool, &user_no_test);
                load_user_data_from_file(&test_out_user, &out_no_of_user);
                printf("the test output should be : %u, and the output is : %u \n", no_of_user, user_no_test);
                print_user_data(test_out_user);
                */
                load_user_data_from_file(&user_pool);
                printf("the no of user is : %u \n", no_of_user);

                int i = 0;
                while (user_pool[i].username != 0)
                {
                    print_user_data(user_pool[i]);
                }
            }
            else if (choice == 2)
            {
                // char plain_password[128];
                // printf("create account \n");
                // user_t new_user;
                // printf("You are about the create a new user. \n");
                // printf("enter your user name : ");
                // fgets(cmd, cmd_len, stdin);
                // cmd_clean(cmd);
                // if (strlen(cmd) == 0)
                // {
                //     printf("username cannot be empty. \n");
                //     return 1;
                // }
                // sscanf(cmd, "%s", &new_user.username);

                // printf("enter you password : ");
                // get_password(&plain_password, sizeof(new_user.password));

                // if (strlen(plain_password) == 0)
                // {
                //     printf("password cannot be empty. \n");
                //     return 1;
                // }

                // if (crypto_pwhash_str(new_user.password, plain_password, strlen(plain_password), crypto_pwhash_OPSLIMIT_INTERACTIVE, crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0)
                // {
                //     printf("hashing the password failed, not enough memory. \n");
                //     sodium_memzero(plain_password, sizeof(plain_password));
                //     continue;
                // }
                // sodium_memzero(plain_password, sizeof(plain_password));

                // new_user.attempt_no = 3;
                // new_user.is_admin = false;
                // new_user.is_logged_in = false;

                // if (!add_user(new_user, &user_pool, &no_of_user))
                // {
                //     printf("Account successfully created. \n\n");
                //     print_user_data(new_user);
                // }

                user_t user1 = {.username = "mahdi1",
                                .password = "hashed_pass1",
                                .attempt_no = 3,
                                .is_admin = true,
                                .is_logged_in = false};

                user_t user2 = {.username = "mahdi2",
                                .password = "hashed_pass2",
                                .attempt_no = 3,
                                .is_admin = true,
                                .is_logged_in = false};

                add_user(user1, &user_pool, 0);
                add_user(user1, &user_pool, 1);
                printf("the users added in the user pool. \n");
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

    printf("the no of user is : %u \n", no_of_user);
    printf("the data has been saved ");

    printf("hello world \n");

    return 0;
}