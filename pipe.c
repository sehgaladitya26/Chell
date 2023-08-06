#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <wait.h>
#include "pipe.h"
#include "shell.h"

struct pipe_commands
{
    char *pipe_toks[1024];
    int num;
};
struct pipe_commands pipe_cmds;

void parse_pipe(char *cmds)
{
    pipe_cmds.num = tokenize_cmd(cmds, pipe_cmds.pipe_toks, "|");
    // if num < 2 then error

    int fd[2];
    pid_t new_process;
    int fd_prev = 0;

    for (int i = 0; i < pipe_cmds.num; i++)
    {
        char **commands = cmd_parse(pipe_cmds.pipe_toks[i]);
        int check = 0;

        if (i == pipe_cmds.num - 1)
            check = 1;

        pipe(fd);

        new_process = fork();
        if (new_process < 0)
        {
            printf("Error(execute): forking failed\n");
            return;
        }
        else if (new_process == 0)
        {
            dup2(fd_prev, 0);
            if (check == 0)
            {
                dup2(fd[1], 1);
            }
            close(fd[0]);
            if (execvp(commands[0], commands) != 0)
            {
                perror("Error(dup)");
            }
        }
        else
        {
            waitpid(new_process, NULL, WUNTRACED);
            close(fd[1]);
            fd_prev = fd[0];
        }
        free(commands);
    }
}

