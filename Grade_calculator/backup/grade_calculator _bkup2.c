#include <stdio.h>
#include <sodium.h>
#include <stdbool.h>
#include <time.h>

#include "include/user_t.h"
#include "include/subject_t.h"

#define MAX_USER 10
#define USER_FILE "user.dat"

// subject db
subject_db_t subject_db;
unsigned int no_of_subject = 0;

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

void print_help()
{
    printf("creat  : to create new user. \n");
    printf("login  : to login to the system. \n");
    printf("forgot : in case you forget your password. \n");
    printf("x      : to exit the program. \n");
    printf("h      : to show this help menu. \n");
}

static int add_user_to_pool(user_t *user_pool_t, user_t *user_x, unsigned int index)
{
    // to add the userx to the user_pool_t
    user_pool_t[index] = *user_x;
}

void print_user_pool(user_t *user_pool_t, unsigned int no_of_elements)
{
    for (int i = 0; i < no_of_elements; i++)
    {
        printf("username : %s \n", user_pool_t[i].username);
        printf("password : %s \n", user_pool_t[i].password);
        printf("attemps_remaining : %u \n", user_pool_t[i].attempt_remainig);
        printf("is_admin : %d \n", user_pool_t[i].is_admin);
        printf("is_logged_in : %d \n\n", user_pool_t[i].is_logged_in);
    }
}

void add_user_pool_to_file(user_t *user_pool_t, unsigned int index)
{
    FILE *fd = fopen(USER_FILE, "wb");
    if (fd == NULL)
    {
        printf("the file could not be opened. \n");
        return;
    }

    // adding the number of users to the file
    fwrite(&index, sizeof(index), 1, fd);
    for (int i = 0; i < index; i++)
    {
        if (!fwrite(&user_pool_t[i], sizeof(user_t), 1, fd))
        {
            printf("the %d th strcut could not be saved to the file. \n");
            fclose(fd);
        }
    }
    fclose(fd);
}

void load_user_data_from_file(user_t *user_pool_t, unsigned int *no_of_user)
{
    // opening the file
    FILE *fd = fopen(USER_FILE, "rb");
    if (fd == NULL)
    {
        printf("couldn't open the file. \n");
        return;
    }

    // getting the number of the users.
    *no_of_user = 0;
    fread(no_of_user, sizeof(unsigned int), 1, fd);

    for (int i = 0; i < *no_of_user; i++)
    {
        if (!fread(&user_pool_t[i], sizeof(user_t), 1, fd))
        {
            printf("could not read the %d th struct from the file. \n");
            fclose(fd);
            return;
        }
    }
    fclose(fd);
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

int main()
{
    char cmd[cmd_len];
    unsigned int terminate = 0;

    user_t user_pool[MAX_USER];
    unsigned int no_of_user = 0;

    unsigned char master_password[crypto_pwhash_STRBYTES];

    print_help();
    do
    {
        printf(">> ");
        fflush(stdout);
        fgets(cmd, cmd_len, stdin);
        cmd_clean(&cmd);
        if (!strcmp(cmd, "x"))
        {
            terminate = 1;
        }
        else if (!strcmp(cmd, "h"))
        {
            print_help();
        }
        else if (!strcmp(cmd, "test"))
        {
            printf("tests will be done. \n");

            user_t user1 = {
                .username = "mahdi1",
                .password = "hashed_pass",
                .attempt_remainig = 3,
                .is_admin = false,
                .is_logged_in = false};

            user_t user2 = {
                .username = "mahdi2",
                .password = "hashed_pass2",
                .attempt_remainig = 3,
                .is_admin = false,
                .is_logged_in = false};

            // addint the user1 to the index 0 of the user_pool
            add_user_to_pool(&user_pool, &user1, no_of_user);
            printf("the user has been added to the pool \n");
            no_of_user++;

            add_user_to_pool(&user_pool, &user2, no_of_user);
            printf("the user has been added to the pool \n");
            no_of_user++;

            // printing the userpool
            print_user_pool(&user_pool, no_of_user);

            // addting the data of the user_pool to the file
            add_user_pool_to_file(user_pool, no_of_user);
            printf("the user pool has been saved to the file. \n\n");

            printf("PHASE 2 OF THE TEST. \n");
            // loading the data from the file
            load_user_data_from_file(&user_pool, &no_of_user);

            // printing the userpool
            print_user_pool(&user_pool, no_of_user);
            printf("test successful. \n");
        }
        else if (!strcmp(cmd, "create"))
        {
            unsigned char plain_password[cmd_len];

            FILE *fd = fopen(USER_FILE, "rb");
            if (fd == NULL)
            {
                printf("the user file could not be openend. \n");
                printf("or file does not exist. \n");
                no_of_user = 0;
            }
            else
            {
                fread(&no_of_user, sizeof(no_of_user), 1, fd);
                fclose(fd);
            }

            if (no_of_user == 0)
            {
                user_t user1;
                printf("You are the first user, so you are admin. \n");
                printf("set a master password, this will be used for admin privilleges. \n\n");
                printf("enter your username : ");
                fgets(cmd, cmd_len, stdin);
                cmd_clean(cmd);
                if (strlen(cmd) == 0)
                {
                    printf("username cannot be empty. \n");
                }
                sscanf(cmd, "%s", &user1.username);

                printf("enter the master password. \n");
                get_password(master_password, cmd_len);
                if (strlen(master_password) == 0)
                {
                    printf("du bist so dumm! , das master kennwort sollte nicht leer sein. \n");
                }
                printf("write down your master password, this is the only time you are going to see the master password. \n");
                printf("master password : %s \n", master_password);

                // hashing the password and saving it to the user1 structure
                if (crypto_pwhash_str(user1.password, &master_password, strlen(master_password), crypto_pwhash_OPSLIMIT_INTERACTIVE, crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0)
                {
                    printf("failed to hash the password. \n");
                    return 1;
                }
                sodium_memzero(master_password, strlen(master_password));

                user1.attempt_remainig = 3;
                user1.is_admin = true;
                user1.is_logged_in = false;

                // update the no_of_user and the user_pool
                add_user_to_pool(&user_pool, &user1, no_of_user);
                no_of_user++;

                // adding the no_of_user to the file, adding the user_pool to the file
                add_user_pool_to_file(&user_pool, no_of_user);

                no_of_user++;
            }
            else
            {
                user_t user2;
                printf("enter your username : ");
                fgets(cmd, cmd_len, stdin);
                cmd_clean(cmd);
                if (strlen(cmd) == 0)
                {
                    printf("username cannot be empty. \n");
                }
                sscanf(cmd, "%s", &user2.username);

                printf("enter the password. \n");
                get_password(plain_password, cmd_len);
                if (strlen(plain_password) == 0)
                {
                    printf("du bist so dumm! , das master kennwort sollte nicht leer sein. \n");
                }

                // hashing the password and saving it to the user1 structure
                if (crypto_pwhash_str(user2.password, &plain_password, strlen(plain_password), crypto_pwhash_OPSLIMIT_INTERACTIVE, crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0)
                {
                    printf("failed to hash the password. \n");
                    return 1;
                }
                sodium_memzero(plain_password, strlen(plain_password));

                user2.attempt_remainig = 3;
                user2.is_admin = false;
                user2.is_logged_in = false;

                // making sure the the user_pool has all the previous users, and adding the new user to the user_pool
                load_user_data_from_file(&user_pool, &no_of_user);
                add_user_to_pool(&user_pool, &user2, no_of_user);
                no_of_user++;

                // saving the user_pool to the file.
                add_user_pool_to_file(user_pool, no_of_user);

                // // printing the user_pool
                // print_user_pool(&user_pool, no_of_user);
                // printf("these are the users in the user_pool. \n");

                // printing the data from the userfile
                load_user_data_from_file(&user_pool, &no_of_user);
                print_user_pool(&user_pool, no_of_user);
                printf("these are the users in the user_file. \n");
            }
        }
        else if (!strcmp(cmd, "login"))
        {

            load_user_data_from_file(&user_pool, &no_of_user);
            if (no_of_user == 0)
            {
                printf("no users in the system, please create a user first. \n");
                continue;
            }
            else
            {
                printf("please enter your username : ");
                fgets(cmd, cmd_len, stdin);
                cmd_clean(cmd);
                for (int i = 0; i < no_of_user; i++)
                {
                    if (!strcmp(cmd, user_pool[i].username))
                    {
                        printf("please enter the password. \n");
                        get_password(cmd, cmd_len);
                        cmd_clean(cmd);

                        // verifying the entered password with the hashed password in the user_pool
                        if (crypto_pwhash_str_verify(user_pool[i].password, cmd, strlen(cmd)) == 0)
                        {
                            printf("login successful. \n");
                            user_pool[i].is_logged_in = true;
                            add_user_pool_to_file(user_pool, no_of_user);

                            // user interaction function
                            if (user_pool[i].is_admin == true)
                            {
                                // admin user interaction
                                printf("admin user access \n");
                            }
                            else
                            {
                                // normal user interaction
                                if (user_pool[i].is_logged_in == true)
                                {
                                    printf("please enter the number of subjects you want to add : ");
                                    int num_subject;
                                    fgets(cmd, cmd_len, stdin);
                                    cmd_clean(cmd);
                                    if (sscanf(cmd, "%d", &num_subject) != 1 || num_subject <= 0 || num_subject > MAX_SUBJECT)
                                    {
                                        printf("invalid number of subjects. \n");
                                        continue;
                                    }
                                    else
                                    {
                                        load_subject_db_from_file(&subject_db, &no_of_subject);
                                        for (int i = no_of_subject; i < num_subject + no_of_subject; i++)
                                        {
                                            // getting the subject data from the user
                                            subject_t subject1;
                                            printf("please enter the subject name : ");
                                            fgets(cmd, cmd_len, stdin);
                                            cmd_clean(cmd);
                                            sscanf(cmd, "%s", &subject1.name);
                                            printf("please enter the grade : ");
                                            fgets(cmd, cmd_len, stdin);
                                            cmd_clean(cmd);
                                            sscanf(cmd, "%lf", &subject1.grade);
                                            printf("please enter the ECTS : ");
                                            fgets(cmd, cmd_len, stdin);
                                            cmd_clean(cmd);
                                            sscanf(cmd, "%u", &subject1.ects);

                                            // adding the subject to db
                                            add_subject_to_db(&subject_db, &subject1, no_of_subject);
                                            no_of_subject++;
                                        }

                                        // adding the subject db to the file
                                        add_subject_db_to_file(&subject_db, no_of_subject);
                                        printf("the subjects have been added to the db and saved to the file. \n");
                                    }

                                    // getting the data from the file and calculating the gpa and total ects
                                    load_subject_db_from_file(&subject_db, &no_of_subject);
                                    double gpa = 0.0;
                                    unsigned int total_ects = 0;
                                    calc_sem_data(&subject_db, no_of_subject, &gpa, &total_ects);
                                    print_final_results(&subject_db, no_of_subject, gpa, total_ects);
                                }
                            }
                        }
                        else
                        {
                            printf("login failed. \n");
                            user_pool[i].attempt_remainig--;
                            if (user_pool[i].attempt_remainig == 0)
                            {
                                printf("you have used all your attempts, please contact the admin. \n");
                            }
                            else
                            {
                                printf("you have %u attempts remaining. \n", user_pool[i].attempt_remainig);
                                add_user_pool_to_file(user_pool, no_of_user);
                            }
                        }
                    }
                    else
                    {
                        printf("the user does not exist / check the spelling. \n");
                    }
                }
            }
        }
    } while (terminate == 0);

    return 0;
}