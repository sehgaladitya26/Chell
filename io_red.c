#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "execute.h"
#include "signal.h"
#include "errno.h"
#include <time.h>
#include "shell.h"

struct io_redirection
{
    int input;
    int in_index;
    int output;
    int out_index;
    int inout;
};
struct io_redirection io;

void io_set()
{
    io.input = 0;
    io.output = 0;
    io.in_index = -1;
    io.out_index = -1;
    io.inout = 0;
}

void execution(char **cmds)
{
    pid_t new_process;
    new_process = fork();

    if (new_process < 0)
    {
        printf("Error(execute): forking failed\n");
        return;
    }

    else if (new_process == 0)
    {
        setpgid(new_process, new_process);
        // printf("process-id is: %d and process-group-id is: %d\n", getpid(), getpgrp());
        if (tcsetpgrp(sig.shell_dscptr, getpid()) != 0) // gives terminal to child process
            perror("tcsetpgrp() error");

        signal(SIGINT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);

        if (execvp(cmds[0], cmds) < 0)
        {
            perror("Error(execute)");
            _exit(1);
        }
        _exit(0);
    }
    else
    {
        int status;
        pid_t exit_id;
        while ((exit_id = waitpid(new_process, &status, WUNTRACED)) == 0)
        {
            if (errno == ECHILD)
            {
                break;
            }
        };
        if (tcsetpgrp(sig.shell_dscptr, sig.shell_pgid) != 0) // handles terminal back to shell group id
            perror("tcsetpgrp() error");

        if (exit_id == -1)
        { // error calling waitpid
            perror("waitpid error");
            exit(EXIT_FAILURE);
        }
    }
}

int input_redirection(char *file_cmd)
{
    int success = 1;
    // printf("%s\n", cmd1[0]);
    int file = open(file_cmd, O_RDONLY, 0644);
    if (file == -1)
    {
        perror("Error(io redirection)");
        success = 0;
        return success;
    }

    int file2 = dup2(file, STDIN_FILENO);
    if (file2 < 0)
    {
        perror("Error(io redirection");
        success = 0;
        return success;
    }
    close(file);
    return success;
}

int output_redirection(char *file_cmd, char **io_cmd, int write_type)
{
    int file;
    int success = 1;
    // printf("%s\n", io_cmd[0]);
    if (write_type == 0)
    {
        file = open(file_cmd, O_WRONLY | O_CREAT, 0644);
    }
    else if (write_type == 1)
    {
        file = open(file_cmd, O_WRONLY | O_CREAT | O_APPEND, 0644);
    }
    if (file == -1)
    {
        perror("error1");
        success = 0;
        return success;
    }

    int file2 = dup2(file, STDOUT_FILENO);
    if (file2 < 0)
    {
        perror("Error(io_redirection)");
        success = 0;
        return success;
    }
    close(file);
    if (inbuilt_cmd_parser(io_cmd) == 0)
    {
        execution(io_cmd);
    }
    return success;
}

void parse_io(char **cmds)
{
    void io_set();
    int cmd_num = 0;
    int success = 1;

    char input_file[256];
    char output_file[256];
    int write_type;

    while (cmds[cmd_num] != NULL)
    {
        if (strcmp(cmds[cmd_num], "<") == 0)
        {
            io.input = 1;
            io.in_index = cmd_num;

            cmds[cmd_num] = NULL;
            strcpy(input_file, cmds[cmd_num + 1]);
            if(input_redirection(input_file) == 0) return;
        }
        else if (strcmp(cmds[cmd_num], ">") == 0)
        {
            io.output = 1;
            io.out_index = cmd_num;
            write_type = 0;

            cmds[cmd_num] = NULL;
            strcpy(output_file, cmds[cmd_num + 1]);
            if(output_redirection(output_file, cmds, write_type) == 0) return;
        }
        else if (strcmp(cmds[cmd_num], ">>") == 0)
        {
            io.output = 1;
            io.out_index = cmd_num;
            write_type = 1;

            cmds[cmd_num] = NULL;
            strcpy(output_file, cmds[cmd_num + 1]);
            output_redirection(output_file, cmds, write_type);
        }
        cmd_num++;
    }

    if (io.input == 1)
    {
        if (inbuilt_cmd_parser(cmds) == 0)
        {
            execution(cmds);
        }
    }
}