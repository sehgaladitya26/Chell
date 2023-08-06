#ifndef EXECUTE_H
#define EXECUTE_H

#define MAX_PROCESS 256

struct process {
    char process_name[256];
    char process_command[256];
    int process_id;
    int process_init;
    int process_stat;
    int job_num;
    struct process *next;
};
// struct process pro[MAX_PROCESS];

//void sig_handler(int signum, struct process* head);
void delete_process(struct process **head_ref, int pid);
void stop_process(struct process **head_ref, int id);
void execute_bg(struct process** head_ref, char** cmds);
void execute_fg(struct process **head_ref, char **cmds);
void jobs(struct process **head_ref);
void process_sig(char **cmds, struct process **head_ref);
void fg(char **cmds, struct process **head_ref);
void bg(char **cmds, struct process **head_ref);

#endif