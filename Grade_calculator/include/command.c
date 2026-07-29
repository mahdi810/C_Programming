#include "command.h"

void cmd_clean(char *cmd)
{
    char *chptr;
    cmd[CMD_LEN - 1] = '\0';
    chptr = cmd;
    while (*chptr != '\0')
    {
        if (*chptr == '\r' || *chptr == '\n' || *chptr == '\0')
        {
            *chptr = '\0';
            break;
        }
        chptr++;
    }
}