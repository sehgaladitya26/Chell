#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "pinfo.h"
#include "cd.h"
#include "execute.h"

size_t buff = 256;

struct pinfo_process {
    int pid;
    char* status;
    int mem;
    char* path;
};
struct pinfo_process info;

void exec_path_print(char* buff){

    if(strstr(buff,dir.home_dir) == 0) {
        printf("executable path: %s\n",buff);
    }
    else {
        printf("executable path: ~");
        for(int i = dir.home_len; i < (int)strlen(buff); i++){
            printf("%c",buff[i]);
        }
        printf("\n");
    }
}

void tokenize_proc(char *cmd_line, char *delim)
{
    int i = 0;
    char *tokens = strtok(cmd_line, delim);
    char** commands = malloc(buff*(sizeof(char*)));
    while (tokens != NULL)
    {
        commands[i++] = tokens;
        tokens = strtok(NULL, delim);
    }
    commands[i] = NULL;
    info.status = commands[2];
    info.mem = atoi(commands[22]);
}


void pinfo(int pid) {

    printf("gpid: %d\n",getpid());
    char filename[1000];
    sprintf(filename, "/proc/%d/stat", pid);
    int f = open(filename, O_RDONLY);
    if(!f) {
        printf("Error(pinfo): no such process exists\n");
        return;
    }

    char buff[1000];
    char* state;
    int ppid;
    read(f,buff,1000);
    tokenize_proc(buff, " ");

    printf("pid: %d\n", pid);

    if (pid == getpgrp()) printf("process status: %s+\n", info.status);
    else printf("process status: %s\n", info.status);

    // if (pid == shell_pid) printf("process status: %s+\n", info.status);
    // else printf("process status: %s\n", info.status);

    printf("memory: %d\n", info.mem);
    close(f);

    sprintf(filename, "/proc/%d/exe", pid);
    int length = readlink(filename, buff, 1000);
    if(length == -1) {
        printf("Error(pinfo): cannot find executable path\n");
        return;
    }
    buff[length] = '\0';  
    exec_path_print(buff);
}

// void main(int argc, char **argv) {
//     shell_pid = getpid();
//     printf("pid = %d\n", shell_pid);

//     char* pid = argv[1];
//     if(pid == NULL) pinfo(shell_pid);
//     else pinfo(atoi(pid));

//     int hello;
//     scanf("%d",&hello);
// }

// printf("comm = %s\n", comm);
//     printf("state = %c\n", state);
//     printf("parent pid = %d\n", ppid);

 //fscanf(f, "%d %s %c %d", &unused, comm, &state, &ppid);
 //getpgid(pid) == pid