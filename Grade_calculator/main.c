#include <stdio.h>
#include "include/user_t.h"

#define CMD_LEN 128
#define USER_FILE "user.dat"

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
    ADMIN_USER
} state_t;

void print_menu()
{
    printf("1. LOGIN. \n");
    printf("2. CREATE_USER. \n");
    printf("3. RUNNING. \n");
    printf("4. LOGOUT. \n");
    printf("5. SHUTDOWN. \n");
}

void cmd_clean(char *cmd)
{
    cmd[NAME_LEN - 1] = '\0';
    while (*cmd != '\0')
    {
        if (*cmd == '\r' || *cmd == '\n')
        {
            *cmd = '\0';
        }
        cmd++;
    }
}

int main()
{
    user_dbt_t user_db1;
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
            else if (!strcmp(cmd, "running"))
            {
                state = RUNNING;
            }
            break;

        case LOGIN:
            printf("this is the login section. \n");
            break;

        case CREATE_USER:
            // first load the file and check if the file exist
            FILE *fd = fopen(USER_FILE, "wb");
            if (fd == NULL)
            {
                printf("the file does not exist or corrupt. \n");
                user_db1.count = 0;
            }
            if (!fread(&user_db1, sizeof(user_dbt_t), 1, fd) != 1)
            {
                printf("there is no file, a file will be created. \n");
            }
            if (user_db1.count == 0)
            {
                printf("this is the first user creation, and this user will have admin privilliges. \n");
            }

            fclose(fd);
            break;

        case RUNNING:
            break;

        case LOGOUT:
            break;

        case NORMAL_USER:
            break;

        case ADMIN_USER:
            break;

        default:
            break;
        }
    }

    return 0;
}