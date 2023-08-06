#ifndef SIGNAL_H
#define SIGNAL_H

void init_signal();
void sig_handler(int signum);

struct signal
{
    int shell_dscptr;
    int shell_stdin;
    int shell_stdout;
    int shell_pid;
    int shell_pgid;
};
struct signal sig;

#endif