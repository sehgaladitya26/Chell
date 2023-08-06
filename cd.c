#include<stdio.h>
#include<unistd.h> 
#include<string.h>
#include<stdlib.h>
#include "cd.h"
#include "shell.h"

#define MAX_LEN 256

void setup_routine();
void current_dir();
int change_dir(char* next_dir);
void print_cd();
void parse_cd(char* next_dir);

// create an initial setup routine function!!
void setup_routine() {
    current_dir();
    dir.curr_dir_mod[0] = '~';
    strcpy(dir.prev_curr_dir, dir.curr_dir);
    strcpy(dir.home_dir, dir.curr_dir);
    dir.home_len = (int)strlen(dir.home_dir);
    dir.valid_prev = 0;
    
    strcpy(history_dir,dir.curr_dir);
    strcpy(temp_dir,dir.curr_dir);
    strcat(history_dir,"/history.txt");
    strcat(temp_dir, "/temp.txt");
}

void current_dir() {
    // printing current working directory
    getcwd(dir.curr_dir, sizeof(dir.curr_dir));
}

void change_dir_ls(char* next_dir) {
    if(chdir(next_dir) != -1) getcwd(dir.curr_dir, sizeof(dir.curr_dir));
    else perror("Error(ls)");
}

int change_dir(char* next_dir) {
    //printf("change dir: %s\n",next_dir);
    if(chdir(next_dir) != -1)
    {
        if(dir.valid_prev == 0) strcpy(dir.prev_curr_dir, dir.curr_dir);
        dir.valid_prev = 0;
        getcwd(dir.curr_dir, sizeof(dir.curr_dir));
        return 1;
    }
    else {
        perror("Error(cd)");
        return 0;
    }
}

// main function to parse cd command -> send it change dir
void parse_cd(char* next_dir) {
    
    int success_flag = 0;
    if (next_dir == NULL) {
        success_flag = change_dir(dir.home_dir);
    }
    else if(strcmp(next_dir,"-") == 0)
    {
        success_flag = change_dir(dir.prev_curr_dir);
        printf("%s\n",dir.curr_dir);
    }
    else if(next_dir[0] == '~') {
        success_flag = change_dir(dir.home_dir);

        // if there are arguments after ~
        if(sizeof(next_dir) > sizeof("~/") && next_dir[1] == '/') {
            char temp[256];
            sprintf(temp,"%.*s",(int)(strlen(next_dir)-strlen("~/")), next_dir + 2);
            dir.valid_prev = 1;
            success_flag = change_dir(temp);
        }
    }
    else {
        success_flag = change_dir(next_dir);
    }
    if(success_flag == 1) 
    {
        print_cd();
    }
}

void print_cd() {
    char temp[256];
    strcpy(dir.curr_dir_mod,"~");

    if(strstr(dir.curr_dir,dir.home_dir) == NULL) {
        strcpy(dir.curr_dir_mod,dir.curr_dir);
    }
    else {
        sprintf(temp,"%.*s",(int)(strlen(dir.curr_dir)-dir.home_len), dir.curr_dir + dir.home_len);
        strcat(dir.curr_dir_mod,temp);
    }
}

// int main()
// {
//     char name[256];

//     // for pwd
//     setup_routine();
//     printf("Home Directory: %s\n\n",dir.home_dir);

//     parse_cd("hello world/hi");
//     printf("%s\n",dir.curr_dir_mod); // should be hello world one
//     parse_cd(NULL);
//     printf("%s\n",dir.curr_dir_mod); // should be home one
//     parse_cd("~/init");
//     printf("%s\n",dir.curr_dir_mod); // should be home one
//     parse_cd("..");
//     printf("%s\n",dir.curr_dir_mod); // should be home one
//     parse_cd("~/init");
//     printf("%s\n",dir.curr_dir_mod); // should be home one
//     return 0;
// }