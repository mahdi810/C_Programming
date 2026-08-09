#include <stdio.h>
#include <string.h>
#include <math.h>
#include "include/helper.h"

#define cmdlen 128
void cleanCMD(char *cmd)
{
    char *chptr;
    cmd[cmdlen - 1] = '\0';
    chptr = cmd;
    while (*chptr != '\0')
    {
        if (*chptr == '\r' || *chptr == '\n')
        {
            *chptr = '\0';
            break;
        }
        chptr++;
    }
}

void print_help()
{
    printf("h     : print this help menu. \n");
    printf("x     : to quite the program. \n");
    printf("range : to find the range for intxx.xx and uintxx.xx \n");
}

int main()
{
    // this program finds the range of int and uint
    char cmd[cmdlen];
    int terminate = 0;

    int number, fraction;
    double min, max;

    print_help();
    do
    {
        printf(">> ");
        fflush(stdout);
        fgets(cmd, cmdlen, stdin);
        cleanCMD(cmd);

        if (!strcmp(cmd, "x"))
        {
            terminate = 1;
        }
        else if (!strcmp(cmd, "h"))
        {
            print_help();
        }
        else if (!strcmp(cmd, "range"))
        {
            printf("starting the algorithm. \n");
            printf("enter the data format (intxx.xx or uintxx.xx) \n");
            fgets(cmd, cmdlen, stdin);
            cleanCMD(cmd);
            if (!strncmp(cmd, "int", 2))
            {
                printf("int part. \n");
                sscanf(&cmd[3], "%d.%d", &number, &fraction);
                printf("number : %d, fraction : %d \n", number, fraction);
                max = (double)((pow(2, number - 1) - 1)) * (double)(pow(2, -fraction));
                min = (double)(-(pow(2, number - 1))) * (double)(pow(2, -fraction));
                printf("the range is : %0.16f -------- 0 -------%0.16f \n", min, max);
            }
            else if (!strncmp(cmd, "uint", 3))
            {
                printf("uint part. \n");
                sscanf(&cmd[4], "%d.%d", &number, &fraction);
                printf("number : %d, fraction : %d \n", number, fraction);
                max = (double)((pow(2, number) - 1)) * (double)(pow(2, -fraction));
                min = 0;
                printf("the range is : %0.16f -------- 0 -------%0.16f \n", min, max);
            }
            else
            {
                printf("invalid entry. \n");
            }
        }
        else if (strlen(cmd) > 0)
        {
            printf("invalid entry. \n");
        }
    } while (terminate == 0);
    return 0;
}