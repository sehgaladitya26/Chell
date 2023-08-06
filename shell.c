#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <time.h>

#include "getName.h"
#include "cd.h"
#include "echo.h"
#include "ls.h"
#include "discover.h"
#include "execute.h"
#include "pinfo.h"
#include "signal.h"
#include "history.h"
#include "io_red.h"
#include "pipe.h"

#define CMD_SIZE 256
#define CMD_NUMS 20

struct commands
{
    char *cmd_line;
    char *tok_cmd[1024]; // max 50 tok commands
    char *cmds[1024];    // max 50 in-built commands // try to chnage it to **
};
struct commands cmd;

struct process *head = NULL;

void sig_handler(int signum)
{
    // if(signum = SIGINT) {
    //     signal(SIGINT, SIG_IGN);
    // }
    int status, pid, i = 0;
    struct process *current = head;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        while (current->process_id != pid)
        {
            if (current->next == NULL)
            {
                printf("Critical Error(execute): background process not found!");
                exit(1);
            }
            current = current->next;
        }
        char process_name[256];
        int process_id = current->process_id;
        strcpy(process_name, current->process_name);

        if (WIFEXITED(status))
        {
            printf("\n[%s with pid = %d exited normally with status %d]\n", process_name, process_id, WIFEXITED(status));
            delete_process(&head, pid);
        }
        else
        {
            if (WTERMSIG(status) == 20) {
                current->process_init = 0;
            }
            else {
                printf("\n[%s with pid %d has exited abnormally with status %d]\n", process_name, process_id, WTERMSIG(status));
                delete_process(&head, pid);
            }
        }
        fflush(stdin);
        fflush(stdout);
    }
}

int inbuilt_cmd_parser(char **args)
{
    int temp = 1;
    if (strcmp(args[0], "cd") == 0)
    {
        if (args[2] == NULL) parse_cd(args[1]);
            //printf("Error(cd): too many arguments\n");
    }
    else if (strcmp(args[0], "pwd") == 0)
    {
        printf("%s\n", dir.curr_dir);
    }
    else if (strcmp(args[0], "echo") == 0)
    {
        print_echo(args);
    }
    else if (strcmp(args[0], "ls") == 0)
    {
        parse_ls(args);
    }
    else if (strcmp(args[0], "pinfo") == 0)
    {
        char *pid = args[1];
        if (pid == NULL)
            pinfo(sig.shell_pid);
        else
            pinfo(atoi(pid));
    }
    else if (strcmp(args[0], "discover") == 0)
    {
        parse_fd(args);
    }
    else if (strcmp(args[0], "history") == 0)
    {
        history();
    }
    else if (strcmp(args[0], "jobs") == 0)
    {
        jobs(&head);
    }
    else if (strcmp(args[0], "sig") == 0)
    {
        process_sig(args, &head);
    }
    else if (strcmp(args[0], "fg") == 0)
    {
        if(args[1] != NULL) fg(args, &head);
        else printf("Too few Arguments\n");
    }
    else if (strcmp(args[0], "bg") == 0)
    {
        if(args[1] != NULL) bg(args, &head);
        else printf("Too few Arguments\n");
    }
    else if (strcmp(args[0], "clear") == 0)
    {
        printf("\e[1;1H\e[2J");
    }
    else if (strcmp(args[0], "exit") == 0)
    {
        exit(0);
    }
    else if (strcmp(args[0], "continue") == 0)
    {
        temp = 1;
    }
    else
    {
        // printf("Error: unrecognized built-in command\n");
        temp = 0;
    }
    return temp;
}

// tokenize_cmd

char **cmd_parse(char *cmd)
{
    int i = 0;
    char **commands = malloc(CMD_NUMS * sizeof(char *)); // dynamic 2D array for all commands

    char *tokens = strtok(cmd, " \t\r\n\v\f");

    while (tokens != NULL)
    {
        commands[i++] = tokens;
        tokens = strtok(NULL, " \t\r\n\v\f");
    }
    commands[i] = NULL;
    return commands;
}

int tokenize_cmd(char *cmd_line, char **commands, char *delim)
{
    int i = 0;
    char *tokens = strtok(cmd_line, delim);
    while (tokens != NULL)
    {
        commands[i++] = tokens;
        tokens = strtok(NULL, delim);
    }
    commands[i] = NULL;
    return i;
}

int main()
{
    const char *alias = getusrName();
    signal(SIGCHLD, sig_handler);
    setup_routine();
    init_signal();

    size_t cmd_size = CMD_SIZE;
    size_t bytes;

    while (1)
    {
        int x = dup(0);
        int y = dup(1);
        int io_redirect_flag = 0; // to check for io_redirection
        // reads sentence given
        cmd.cmd_line = (char *)malloc(cmd_size * sizeof(char));
        if (cmd.cmd_line == NULL)
        {
            perror("Error");
            exit(1);
        }

        // prints username, system and current directory
        printf("%s\e[1;94m%s\e[0;97m$ ", alias, dir.curr_dir_mod);
        bytes = getline(&cmd.cmd_line, &cmd_size, stdin);
        cmd.cmd_line[strlen(cmd.cmd_line) - 1] = ' ';
        if (bytes == -1)
        {
            printf("\n"); // change it exit function in inbuilt
            exit(0);
        }
        read_history(cmd.cmd_line);

        // checking for multiple commands
        int cmd_num = tokenize_cmd(cmd.cmd_line, cmd.cmds, ";");
        for (int i = 0; i < cmd_num; i++)
        {
            if (strchr(cmd.cmds[i], '|'))
            {
                parse_pipe(cmd.cmds[i]);
            }
            else if (strchr(cmd.cmds[i], '&'))
            {
                if (strchr(cmd.cmds[i], '<') || strchr(cmd.cmds[i], '>') || strstr(cmd.cmds[i], ">>"))
                {
                    io_redirect_flag = 1;
                }

                int tok_num = tokenize_cmd(cmd.cmds[i], cmd.tok_cmd, "&");

                // sending all bg-processes
                for (int j = 0; j < tok_num; j++)
                {
                    if (strcmp(cmd.tok_cmd[j], " ") == 0)
                        continue;

                    char **commands = cmd_parse(cmd.tok_cmd[j]);

                    if (io_redirect_flag == 1)
                    {
                        parse_io(commands);
                        dup2(x, 0);
                        dup2(y, 1);
                        close(x);
                        close(y);
                    }

                    else
                    {
                        int bg_cmd = inbuilt_cmd_parser(commands);
                        if (bg_cmd == 0 && j != tok_num - 1)
                        {
                            // send to bg-parser
                            execute_bg(&head, commands);
                        }
                        else if (bg_cmd == 0)
                        {
                            printf("here55\n");
                            execute_fg(&head, commands);
                        }
                    }
                    free(commands);
                }
            }
            else // Add IO_redirection here as well
            {
                if (strcmp(cmd.cmds[i], " ") == 0)
                {
                    continue;
                }
                if (strchr(cmd.cmds[i], '<') || strchr(cmd.cmds[i], '>') || strstr(cmd.cmds[i], ">>"))
                {
                    io_redirect_flag = 1;
                }

                char **commands = cmd_parse(cmd.cmds[i]);

                if (io_redirect_flag == 1)
                {
                    parse_io(commands);
                    dup2(x, 0);
                    dup2(y, 1);
                    close(x);
                    close(y);
                }
                else
                {
                    // send to built-in command parser
                    int temp = inbuilt_cmd_parser(commands);
                    if (temp == 0)
                        execute_fg(&head, commands);
                    free(commands);
                }
            }
        }
        free(cmd.cmd_line);
    }
    return 0;
}