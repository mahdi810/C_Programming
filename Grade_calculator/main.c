#include <stdio.h>
#include "include/user_t.h"
#include "include/file_op.h"
#include "include/pass.h"
#include "include/command.h"

typedef struct
{
    char name[NAME_LEN];
    double grade;
    unsigned int ects;
} subject_t;

typedef enum
{
    SELECTION_MENU,
    LOGIN,
    CREATE_USER,
    RUNNING,
    LOGOUT,
    SHUTDOWN,
    NORMAL_USER,
    ADMIN_USER,
    TEST
} state_t;

void print_menu()
{
    printf("1. LOGIN. \n");
    printf("2. CREATE_USER. \n");
    printf("3. RUNNING. \n");
    printf("4. LOGOUT. \n");
    printf("5. SHUTDOWN. \n");
    printf("6. TEST. \n");
    printf("7. ADMIN_USER. \n");
}

int main()
{
    user_dbt_t user_db1;     // user database
    user_db_init(&user_db1); // initializing the user database.
    user_t user;
    user_t *current_user;
    FILE *fd = fopen(USER_FILE, "wb");

    char cmd[NAME_LEN];
    state_t state = SELECTION_MENU;
    printf("-----------------Grade Calculator with Extensive Password Protection-----------------\n");

    if (sodium_init() < 0)
    {
        printf("faile fot initialize the sodium. \n");
        return 1;
    }
    printf("sodium successfully initialized. \n");

    while (state != SHUTDOWN)
    {
        switch (state)
        {
        case SELECTION_MENU:
            print_menu();
            fgets(cmd, CMD_LEN, stdin);
            cmd_clean(cmd);
            if (!strcmp(cmd, "shutdown"))
            {
                state = SHUTDOWN;
            }
            else if (!strcmp(cmd, "login"))
            {
                state = LOGIN;
            }
            else if (!strcmp(cmd, "create_user"))
            {
                state = CREATE_USER;
            }
            else if (!strcmp(cmd, "test"))
            {
                state = TEST;
            }
            else if (!strcmp(cmd, "admin_user"))
            {
                state = ADMIN_USER;
            }
            else
            {
                printf("invalid command");
            }
            break;

        case LOGIN:
            printf("this is the login section. \n");
            // first load the file and check if the file exist
            open_file_and_update_user_db(fd, &user_db1);

            printf("please enter the username. \n");
            fgets(cmd, CMD_LEN, stdin);
            cmd_clean(cmd);
            if (sscanf(cmd, "%s", &user.username) != 1)
            {
                printf("invalid username. \n");
                break;
            }

            printf("please enter the password. \n");
            fgets(cmd, CMD_LEN, stdin);
            cmd_clean(cmd);
            if (sscanf(cmd, "%s", &user.password) != 1)
            {
                printf("invalid password. \n");
                break;
            }

            // now i have to search for the user in the database and check if the password is correct.
            current_user = find_user_in_db(&user_db1, user.username);
            if (current_user != NULL)
            {
                printf("logic successfull. \n");
                state = RUNNING;
            }
            else
            {
                printf("user was not found. \n");
                state = SELECTION_MENU;
            }

            // of course that the password is hased and should be compared with the hashed password in the database.
            // another imporvement should be added that the hashed password password should be salted and that is not done until now.

            break;

        case CREATE_USER:
            printf("this is the create user section. \n");
            // first load the file and check if the file exist
            open_file_and_update_user_db(fd, &user_db1);

            if (user_db1.count == 0)
            {
                printf("this is the first user creation, and this user will have admin privilliges. \n");
                printf("please enter the username. \n");
                fgets(cmd, CMD_LEN, stdin);
                cmd_clean(cmd);
                strncpy(user.username, cmd, NAME_LEN);
                user.username[NAME_LEN - 1] = '\0';

                // getting the password
                printf("enter the master password. \n");
                fgets(cmd, CMD_LEN, stdin);
                cmd_clean(cmd);

                // hashing the password and storing it in the user.password
                if (hash_password(cmd, user.password) == false)
                {
                    printf("hashing password failed. \n");
                }

                // adding the user to the database
                user_create(&user_db1, user.username, user.password);

                // saving the database to the file
                update_file_and_save(fd, &user_db1);
            }
            else
            {
                // getting the username
                printf("please enter the username. \n");
                fgets(cmd, CMD_LEN, stdin);
                cmd_clean(cmd);
                strncpy(user.username, cmd, NAME_LEN);
                user.username[NAME_LEN - 1] = '\0';

                // getting the password
                printf("enter the password. \n");
                fgets(cmd, CMD_LEN, stdin);
                cmd_clean(cmd);
                strncpy(user.password, cmd, CMD_LEN);
                user.password[CMD_LEN - 1] = '\0';

                // adding the user to the database.
                user_create(&user_db1, user.username, user.password);
                print_database(&user_db1);
            }

            state = SELECTION_MENU;
            break;

        case RUNNING:
            printf("hi welcome to the program. \n");
            fgets(cmd, CMD_LEN, stdin);
            break;

        case LOGOUT:
            break;

        case NORMAL_USER:
            break;

        case ADMIN_USER:
            // here the admin user privillages will be implemented.
            // if the user is an admin user, then he should be able to see all the users in the database and be able to delete them, but there should be the privacyy of the users and the admin user should not be able to see the passwords of the users, but he should be able to reset the password of the users.
            // the admin user should be able to see the grades of the users, but he should not be able to change them, but he should be able to reset the grades of the users.

            break;

        case TEST:
            user_create(&user_db1, "mahdi1", "master_password");
            print_database(&user_db1);
            printf("test successfull \n");
            fgets(cmd, CMD_LEN, stdin);

            state = SELECTION_MENU;
            break;

        default:
            break;
        }
    }

    return 0;
}