
#include <stdio.h>
#include <string.h>

#define cmd_len 64


int main()
{
    int terminat = 0; 
    int xdata; 
    char cmd[cmd_len]; 
    char* chp; 

    printf("------------------zperiphV1.0.0--------------------\r\n");
    printf("x                    : to close the application \r\n"); 
    printf("R <regnum>           : to read the <regnum> register \r\n"); 
    printf("W <regnum> <data>    : to write <data> to <regnum> \r\n"); 

    do {
        printf("\r\n>> "); 
        fflush(stdout); 
        int regnum; 
        fgets(cmd, cmd_len, stdin); 
        cmd[cmd_len - 1] = '\0'; 
        chp = cmd; 
        while (*chp != '\0') {
            if (*chp == '\r' || *chp == '\n') {
                *chp = '\0'; 
            }
            chp++; 
        }
        if (!strcmp(cmd, "x")) {
            printf("Thank you for using zperiphV1.0.0 application. \r\n"); 
            terminat = 1; 
        }
        else if (!strcmp(cmd, "R")) {
            if (sscanf(cmd, "%d" &regnum) != 1) {
                printf("there was an error getting the regnum \r\n"); 
            }
        }
        

     
    } while (terminat == 0);
}
