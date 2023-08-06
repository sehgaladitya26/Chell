#ifndef CD_H
#define CD_H

#define MAX_LEN 256

void change_dir_ls(char* next_dir);
void parse_cd(char* next_dir);
void setup_routine();

struct directory {
    char curr_dir[MAX_LEN];
    char home_dir[MAX_LEN];
    char curr_dir_mod[MAX_LEN];
    char prev_curr_dir[MAX_LEN];
    int home_len;
    int valid_prev;
};
struct directory dir;

#endif