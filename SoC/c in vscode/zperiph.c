#include <stdio.h> 
#include <string.h>

#define cmd_len 64

int main(){
    char cmd[cmd_len];
    char *chp;  
    int xdata; 
    int terminat = 0;
    int regnum; 

    printf("---------------zperiphV1.0.0--------------\r\n"); 
    printf("1. x                  : to exit the application \r\n"); 
    printf("2. R <regnum>         : to read the <regnum> register \r\n"); 
    printf("3. W <regnum> <data>  : to read the <regnum> register \r\n");
    do{
        printf("\r>> "); 
        fflush(stdout);  
        fgets(cmd, cmd_len, stdin); 
        cmd[cmd_len - 1] = '\0'; 
        chp = cmd; 
        while(*chp != '\0'){
            if(*chp == '\r' || *chp == '\n'){
                *chp = '\0'; 
            }
            chp++; 
        }

        if(!strcmp(cmd, "x")){
            printf("thank you for using zperiphV1.0.0. \r\n"); 
            terminat = 1; 
        }
        else if(!strncmp(cmd, "R", 1)){
            if(sscanf(&cmd[1], "%d", &regnum) != 1){
                printf("the entered values are not suitable for calculation \r\n"); 
            }
            else{
                printf("%d has been entered \r\n", regnum);     
            }
        } else if(!strncmp(cmd, "W", 1)){
            if(sscanf(&cmd[1], "%d %d", &regnum, &xdata) != 2){
                printf("the format youu entered is not valie \r\n"); 
            } else {
                printf("the numbers entered are : \r\n"); 
                printf("regnum: %d,       data: %d \r\n", regnum, xdata); 
            }
        }

    }while(terminat == 0); 
    

    printf("hello world \r\n"); 
    return 0; 
}