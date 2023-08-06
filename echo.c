#include <stdio.h>
#include <string.h>
#include "echo.h"
#include <stdlib.h>

void print_echo(char **args)
{
    int i = 1;
    int max_size = 256;
    char echo_line[256] = {0};
    while (args[i] != NULL)
    {
        strcat(echo_line, args[i]);
        if (args[i++] == NULL)
        {
            strcat(echo_line, "\n");
        }
        else
            strcat(echo_line, " ");
    }
    printf("%s\n", echo_line);
    memset(echo_line, 0, max_size);
}

// char** commands = cmd_parse(cmd_line);
        // int i = 0;
        // while(commands[i] != NULL) {
        //     printf("%s, ",commands[i]);
        //     i++;
        // }