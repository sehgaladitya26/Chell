#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

#include "ls.h"
#include "cd.h"

#define RED "\e[1;91m"

char og_dir[256];

// add -al and -la as well

int alphasort(const struct dirent **a, const struct dirent **b)
{ // Updating alphasort
    return (strcasecmp((*a)->d_name, (*b)->d_name));
}

int alphasort2(const struct dirent **a, const struct dirent **b)
{ // Updating alphasort
    return (strcmp((*a)->d_name, (*b)->d_name));
}

struct flag_check
{
    int l_flag;
    int a_flag;
    int l_index;
    int a_index;
    int dir_num;
};

// struct max_len
// {
//     int link;
//     int size;
// };
// struct max_len file_size = {.link = 0, . size = 0};

// create struct for permissions
struct flag_check flag;

void flag_set()
{
    flag.a_flag = 0;
    flag.l_flag = 0;
    flag.a_index = -1;
    flag.l_index = -1;
    flag.dir_num = 0;
}

void print_l(char *path)
{
    struct dirent **dir_curr;
    struct stat l_stat;
    int total = 0;
    int num;
    if ((num = scandir(path, &dir_curr, NULL, alphasort2)) < 0)
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

    strcpy(og_dir, dir.curr_dir);
    change_dir_ls(path);
    int j = 0;
    while (j < num)
    {
        if (dir_curr[j]->d_name[0] == '.' && flag.a_flag == 0)
        {
            j++; continue;
        }
        else {
            stat(dir_curr[j]->d_name, &l_stat);
            total += l_stat.st_blocks;
            j++;
        }
    }

    j = 0;
    if (flag.dir_num > 1)
        printf("%s:\n", path);

    printf("total %d\n", total / 2);
    while (j < num)
    {
        if (dir_curr[j]->d_name[0] == '.' && flag.a_flag == 0)
        {
            j++; continue;
        }

        stat(dir_curr[j]->d_name, &l_stat);
        printf("%s", (S_ISDIR(l_stat.st_mode)) ? "d" : "-");
        printf("%s", (l_stat.st_mode & S_IRUSR) ? "r" : "-");
        printf("%s", (l_stat.st_mode & S_IWUSR) ? "w" : "-");
        printf("%s", (l_stat.st_mode & S_IXUSR) ? "x" : "-");
        printf("%s", (l_stat.st_mode & S_IRGRP) ? "r" : "-");
        printf("%s", (l_stat.st_mode & S_IWGRP) ? "w" : "-");
        printf("%s", (l_stat.st_mode & S_IXGRP) ? "x" : "-");
        printf("%s", (l_stat.st_mode & S_IROTH) ? "r" : "-");
        printf("%s", (l_stat.st_mode & S_IWOTH) ? "w" : "-");
        printf("%s", (l_stat.st_mode & S_IXOTH) ? "x" : "-");

        printf(" %3ld ", l_stat.st_nlink);

        struct passwd *usr;
        if ((usr = getpwuid(l_stat.st_uid)) != NULL)
            printf("%s", usr->pw_name);
        else
            printf("?");

        printf(" ");

        struct group *gr;
        if ((gr = getgrgid(l_stat.st_uid)) != NULL)
            printf("%s", gr->gr_name);
        else
            printf("?");

        printf(" ");
        // total += l_stat.st_blocks;

        printf("%10ld", l_stat.st_size);

        printf(" ");

        char date[36];
        strftime(date, 36, "%b %d %H:%M", localtime(&l_stat.st_mtime));
        printf("%s", date);

        printf(" ");

        if (dir_curr[j]->d_type == 4)
            printf("\e[1;91m%s\e[0;97m  ", dir_curr[j]->d_name);

        else {
            if(l_stat.st_mode & S_IXUSR || l_stat.st_mode & S_IXGRP || l_stat.st_mode & S_IXOTH) printf("\e[32;1m%s\e[0;97m  ", dir_curr[j]->d_name);
            else printf("%s  ", dir_curr[j]->d_name);
        }
        free(dir_curr[j]);
        j++;
        printf("\n");
    }
    free(dir_curr);
    change_dir_ls(og_dir);
    for(int a = 0; a < 256; a++) og_dir[a] = '\0';
    //printf("\n");
}

void print_ls(char *path)
{ // ls tilde is remaining, ls . -a is remaining //done
    // DIR *directory;
    struct dirent **dir_curr;
    struct stat l_stat;
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

    strcpy(og_dir, dir.curr_dir);
    change_dir_ls(path);
    int j = 0;
    if (flag.dir_num > 1)
        printf("%s:\n", path);
    while (j < num)
    {
        stat(dir_curr[j]->d_name, &l_stat);
        if (dir_curr[j]->d_name[0] == '.' && flag.a_flag == 0)
        {
            free(dir_curr[j]);
            j++;
            continue; // if -a is not there
        }

        if (dir_curr[j]->d_type == 4)
            printf("\e[1;91m%s\e[0;97m  ", dir_curr[j]->d_name);

        else {
            if(l_stat.st_mode & S_IXUSR || l_stat.st_mode & S_IXGRP || l_stat.st_mode & S_IXOTH) printf("\e[32;1m%s\e[0;97m  ", dir_curr[j]->d_name);
            else printf("\e[0;97m%s\e[0;97m  ", dir_curr[j]->d_name);
        }

        free(dir_curr[j]);
        j++;
    }
    free(dir_curr);
    change_dir_ls(og_dir);
    for(int a = 0; a < 256; a++) og_dir[a] = '\0';
    printf("\n");
}

void ls(char **path)
{
    if (flag.dir_num != 0)
    {
        int i = 1;
        while (path[i] != NULL)
        { // change it to some init flag for only ls
            if (i == flag.l_index || i == flag.a_index)
            {
                i++;
                continue;
            }
            if (strcmp(path[i], "~") == 0)
                if (flag.l_flag == 0)
                    print_ls(dir.home_dir);
                else
                    print_l(dir.home_dir);
            else if (flag.l_flag == 0)
                print_ls(path[i]);
            else
                print_l(path[i]);
            if (path[++i] != NULL)
                printf("\n");
        }
    }
    else
    {
        if (flag.l_flag == 0)
            print_ls(".");
        else
            print_l(".");
    }
}

void parse_ls(char **cmd)
{
    flag_set();
    int i = 0;
    char *ls_line;
    while (cmd[i] != NULL)
    {
        if (cmd[i][0] == '-')
        {
            if (strcmp(cmd[i], "-l") == 0)
            {
                flag.l_flag = 1;
                flag.l_index = i;
            }
            else if (strcmp(cmd[i], "-a") == 0)
            {
                flag.a_flag = 1;
                flag.a_index = i;
            }
            else if (strcmp(cmd[i], "-la") == 0 || strcmp(cmd[i], "al") == 0)
            {
                flag.a_flag = 1;
                flag.a_index = i;
                flag.l_flag = 1;
                flag.l_index = i;
            }
            else
            {
                printf("Error(discover): '%s': unknown flag used\n", cmd[i]);
                return;
            }
        }
        else if (strcmp(cmd[i], "ls") == 0)
        {
            i++;
            continue;
        }
        else
            flag.dir_num++;
        i++;
    }
    // collect all directories function
    ls(cmd);
}

// int main(int argc, char *argv[])
// {
//     ls_parse("ls .");
// }

// DIR *dp;
//     struct dirent *dirp;

//     if (argc != 2) {
//         fprintf(stderr, "Usage: ./program directory_name\n");
//         exit(EXIT_FAILURE);
//     }

//     errno = 0;
//     if ((dp = opendir(argv[1])) == NULL) {
//         switch (errno) {
//             case EACCES: printf("Permission denied\n"); break;
//             case ENOENT: printf("Directory does not exist\n"); break;
//             case ENOTDIR: printf("'%s' is not a directory\n", argv[1]); break;
//         }
//         exit(EXIT_FAILURE);
//     }

//     errno = 0;
//     while ((dirp = readdir(dp)) != NULL)
//         printf("%s\n", dirp->d_name);

//     if (errno != 0) {
//         if (errno == EBADF)
//             printf("Invalid directory stream descriptor\n");
//         else
//             perror("readdir");
//     } else {
//         printf("End-of-directory reached\n");
//     }

//     if (closedir(dp) == -1)
//         perror("closedir");

//     exit(EXIT_SUCCESS);
// }