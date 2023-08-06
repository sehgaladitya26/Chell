#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "execute.h"
#include <sys/types.h>
#include <errno.h>
#include <time.h>

#include "pinfo.h"
#include "signal.h"

void delete_process(struct process **head_ref, int pid)
{
    struct process *temp;
    if ((*head_ref)->process_id == pid)
    {
        temp = *head_ref;
        *head_ref = (*head_ref)->next;
        free(temp);
    }
    else
    {
        struct process *current = *head_ref;
        while (current->next != NULL)
        {
            if (current->next->process_id == pid) // to find the prev process to link with next
            {
                temp = current->next;
                current->next = current->next->next;
                free(temp);
            }
            else
                current = current->next;
        }
    }
}

void stop_process(struct process **head_ref, int id)
{
    int i = 0;
    printf("%d\n", id);
    struct process *current = *head_ref;
    while (current->process_id != id)
    {
        if (current->next == NULL)
        {
            printf("Critical Error(execute): background process not found!");
            return;
        }
        current = current->next;
        // if (current->process_id == id) // to find the prev process to link with next
        // {
        //     if (current->process_init == 1){
        //         current->process_init = 0;
        //     }

        //     printf("pid is: %d, init: %d\n", current->process_id, current->process_init);
        // }
        // else
        //     current = current->next;
    }
    if (current->process_init == 1)
    {
        current->process_init = 0;
    }

    printf("pid is: %d, init: %d\n", current->process_id, current->process_init);
}

void create_process(struct process **head_ref, char *name, int id, int stat, char* commands)
{
    int i = 0;
    struct process *new_node = (struct process *)malloc(sizeof(struct process));
    struct process *last = *head_ref;
    strcpy(new_node->process_name, name);
    strcpy(new_node->process_command, commands);
    new_node->process_id = id;
    new_node->process_stat = stat;
    new_node->process_init = 1;
    new_node->next = NULL;

    if (*head_ref == NULL)
    {
        *head_ref = new_node;
        new_node->job_num = 1;
    }
    else
    {
        while (last->next != NULL)
        {
            last = last->next;
            i++;
        }
        new_node->job_num = i + 2;
        last->next = new_node;
        i++;
    }

    printf("[%d] %d\n", new_node->job_num, new_node->process_id);
    return;
}

void execute_bg(struct process **head_ref, char **cmds)
{
    int new_process = fork();
    if (new_process < 0)
    {
        printf("Error(execute): forking failed\n");
        return;
    }
    else if (new_process == 0)
    {
        setpgid(new_process, new_process);

        signal(SIGINT, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        signal(SIGTTIN, SIG_DFL);
        signal(SIGTTOU, SIG_DFL);
        signal(SIGCHLD, SIG_DFL);

        if (execvp(cmds[0], cmds) == -1)
        {
            printf("\nError(execute): Incorrect Command\n");
            return;
        }
        return;
    }
    else
    {
        char commands[256] = {0};
        int k = 0;
        while(cmds[k] != NULL){
            strcat(commands,cmds[k]);
            strcat(commands, " ");
            k++;
        }
        create_process(head_ref, cmds[0], new_process, 1, commands);
        memset(commands, 0, 256);
        // printf("Process ID: [%d]\n", new_process);
    }
}

void execute_fg(struct process **head_ref, char **cmds)
{
    // printf("shell: %d\n\n", shell_process);
    //  printf("shell_group_id: %d\n", (int)shell_process_group);
    time_t start, end;
    pid_t new_process;
    new_process = fork();

    // printf("Parent process started at %s", ctime(&when));
    start = time(NULL);
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
            printf("Error(execute): Incorrect Command\n");
            _exit(1);
        }
        _exit(0);
    }
    else
    {
        int status;
        pid_t exit_id;
        char commands[256] = {0};
        int k = 0;
        while(cmds[k] != NULL){
            strcat(commands,cmds[k]);
            strcat(commands, " ");
            k++;
        }
        create_process(head_ref, cmds[0], new_process, 1, commands);
        memset(commands, 0, 256);
        while ((exit_id = waitpid(new_process, &status, WUNTRACED)) == 0)
        {
            if (errno == ECHILD)
            {
                break;
            }
        }
        if (tcsetpgrp(sig.shell_dscptr, sig.shell_pgid) != 0) // handles terminal back to shell group id
            perror("tcsetpgrp() error");

        if (exit_id == -1)
        { // error calling waitpid
            perror("waitpid error");
            exit(EXIT_FAILURE);
        }

        if (!WIFSTOPPED(status))
            delete_process(head_ref, new_process);
        else
        {
            fprintf(stderr, "\nProcess stopped!!\n");
            stop_process(head_ref, new_process);
        }

        end = time(NULL);
        long int time_taken = (long int)(end - start);
        if (time_taken >= 1)
            printf("[%s] executed for %ld seconds\n", cmds[0], time_taken);
    }
}

void jobs(struct process **head_ref)
{
    int num = 0;
    // printf("here\n");
    struct process *current = *head_ref;
    // create a temporary struct
    while (current != NULL)
    {
        // printf("here\n");
        if(current->process_init == 1){
            printf("[%d] Running %s [%d]\n", current->job_num, current->process_command, current->process_id);
        }
        else{
            printf("[%d] Stopped %s [%d]\n", current->job_num, current->process_command, current->process_id);
        }
        current = current->next;
    }
}

void process_sig(char **cmds, struct process **head_ref)
{ // running perfectly
    struct process *current = *head_ref;
    int job_id = atoi(cmds[1]);
    int sig_id = atoi(cmds[2]);
    int init_flag = 0;

    if (sig_id > 64 || sig_id < 0)
    {
        printf("Error(sig): Signal does not exist\n");
        return;
    }

    while (current != NULL)
    {
        if (current->job_num == job_id)
        {
            kill(current->process_id, sig_id); // error handling over here (done)
            if (sig_id == 20)
            {
                current->process_init = 0;
            }
            init_flag = 1;
            break;
        }
        current = current->next;
    }
    if (init_flag == 0)
    {
        printf("Error(sig): No job ID found to send signal\n");
    }
}

void fg(char **cmds, struct process **head_ref)
{
    struct process *current = *head_ref;
    int job_id = atoi(cmds[1]);
    int init_flag = 0;

    while (current != NULL)
    {
        if (current->job_num == job_id)
        {
            init_flag = 1;
            break;
        }
        else
        {
            current = current->next;
        }
    }
    if (init_flag == 1)
    {
        pid_t new_process = current->process_id;
        if (tcsetpgrp(sig.shell_dscptr, new_process) != 0) // gives terminal to child process
            perror("tcsetpgrp() error");

        if (killpg(new_process, SIGCONT) < 0)
            printf("Error(fg)\n");

        int status;
        waitpid(new_process, &status, WUNTRACED);             // add option for stopping the process!!
        if (tcsetpgrp(sig.shell_dscptr, sig.shell_pgid) != 0) // handles terminal back to shell group id
            perror("tcsetpgrp() error");

        delete_process(head_ref, new_process);
    }
}

void bg(char **cmds, struct process **head_ref)
{
    struct process *current = *head_ref;
    int job_id = atoi(cmds[1]);
    int init_flag = 0;

    while (current != NULL)
    {
        if (current->job_num == job_id)
        {
            init_flag = 1;
            break;
        }
        else
        {
            current = current->next;
        }
    }
    if (init_flag == 1)
    {
        pid_t new_process = current->process_id;
        if (current->process_init == 0)
        {
            current->process_init = 1;
            if (kill(new_process, SIGTTIN) < 0)
                printf("Error(fg)\n");
            if (kill(new_process, SIGCONT) < 0)
                printf("Error(fg)\n");
        }
        else {
            printf("Process already running\n");
        }
    }
}