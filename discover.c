#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include "discover.h"
#include "cd.h"

#define RED "\e[1;91m"
#define BUFF_SIZE 256

// add -al and -la as well
const size_t buffer = BUFF_SIZE;

struct flag_check
{
    int f_flag;
    int d_flag;
};

struct discover // Add multiple dir checker variable
{
    char gtarget_dir[256];
    char gtarget_file[256];
    int file_found;
    int dir_present;
    int file_present;
};

// create struct for permissions
struct flag_check flag;
struct discover target;

void discover_set()
{
    flag.d_flag = 0;
    flag.f_flag = 0;
    target.dir_present = 0;
    target.file_found = 0;
    target.file_present = 0;
}

void tokenize(char *cmd_line, char *delim)
{
    int i = 0;
    char *tokens = strtok(cmd_line, delim);
    char **commands = malloc(buffer * (sizeof(char *)));
    while (tokens != NULL)
    {
        commands[i++] = tokens;
        tokens = strtok(NULL, delim);
    }
    commands[i] = NULL;
    strcpy(target.gtarget_file, commands[0]);
}

void flag_printer(char *path, int type, int present)
{
    if (flag.d_flag == 1 && flag.f_flag == 0)
    {
        if (type == 0 && present == 1)
            printf("%s\n", path); // type = 0: directory
    }
    else if (flag.d_flag == 0 && flag.f_flag == 1)
    {
        if (type == 1 && present == 1)
            printf("%s\n", path); // type = 0: directory
    }
    else if (flag.d_flag == 0 && flag.f_flag == 0 && target.file_present == 1)
    {
        if(present == 1) printf("%s\n", path); // type = 0: directory
    }
    else
    {
        printf("%s\n", path);
    }
}

void print_fd(char *path)
{
    int found = 0;
    char fullpath[1024];
    struct dirent **dir_curr;
    int num;
    if ((num = scandir(path, &dir_curr, NULL, alphasort)) < 0)
    {
        switch (errno)
        {
        case EACCES:
            perror("Error(ls)");
            break;
        case ENOENT:
            perror("Error(ls)"); // printf("Directory does not exist\n");
            break;
        case ENOTDIR:
            printf("%s\n", path);
            break;
        case EBADF:
            printf("Error(ls)");
            break;
        }
        return;
    }

    int j = 0;
    while (j < num)
    {
        // if (target.file_found == 1 && flag.d_flag == 0 && flag.f_flag == 0)
        //     return;

        if (dir_curr[j]->d_name[0] == '.')
        {
            free(dir_curr[j]);
            j++;
            continue; // if -a is not there
        }

        if (dir_curr[j]->d_type == 4)
        {
            int present = 0;
            sprintf(fullpath, "%s/%s", path, dir_curr[j]->d_name);
            if (strcmp(target.gtarget_file, dir_curr[j]->d_name) == 0)
                present = 1;
            flag_printer(fullpath, 0, present);
            print_fd(fullpath);
        }

        else
        {
            int present = 0;
            sprintf(fullpath, "%s/%s", path, dir_curr[j]->d_name);
            if (strcmp(target.gtarget_file, dir_curr[j]->d_name) == 0) 
                present = 1;
            flag_printer(fullpath, 1, present);
            // if(target.file_found == 1) return;
        }

        free(dir_curr[j]);
        j++;
    }
    free(dir_curr);
    return;
}

void parse_fd(char **cmd)
{
    discover_set();
    int i = 0;
    while (cmd[i] != NULL)
    {
        if (i > 4)
        {
            printf("Error(discover): too many arguments\n");
            return;
        }

        if (cmd[i][0] == '-')
        {
            if (strcmp(cmd[i], "-f") == 0)
            {
                flag.f_flag = 1;
            }
            else if (strcmp(cmd[i], "-d") == 0)
            {
                flag.d_flag = 1;
            }
            else
            {
                printf("Error(discover): '%s': unknown flag used\n", cmd[i]);
                return;
            }
        }

        else if (strcmp(cmd[i], "discover") == 0)
        {
            i++;
            continue;
        }
        else if (strchr(cmd[i], '"'))
        {
            target.file_present = 1;
            tokenize(cmd[i], "\"");
        }
        else
        {
            if (strcmp(cmd[i], "~") == 0)
            {
                strcpy(target.gtarget_dir, dir.home_dir); // update tilde (if possible)
            }
            else
            {
                strcpy(target.gtarget_dir, cmd[i]);
            }
            target.dir_present = 1;
        }
        i++;
    }

    if (target.dir_present == 0)
        strcpy(target.gtarget_dir, ".");
    print_fd(target.gtarget_dir);

    memset(target.gtarget_file, 0, BUFF_SIZE);
    memset(target.gtarget_dir, 0, BUFF_SIZE);
}

// printf("\e[1;91m%s\e[0;97m\n",dir_curr[j]->d_name);