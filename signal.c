#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include "signal.h"
#include "execute.h"

void init_signal()
{
    sig.shell_pid = getpid();
    sig.shell_pgid = getpgid(sig.shell_pid);
    setpgid(sig.shell_pid, sig.shell_pgid);

    sig.shell_dscptr = STDERR_FILENO;
    sig.shell_stdin = STDIN_FILENO;
    sig.shell_stdout = STDOUT_FILENO;

    signal(SIGTTOU, SIG_IGN); // to let parent process to get access to terminal and set it's pgid to shell descriptor
    signal(SIGINT, SIG_IGN);  /* To ignore Ctrl c */
    signal(SIGTSTP, SIG_IGN); /* To ignore Ctrl z */
    signal(SIGQUIT, SIG_IGN); /* To ignore Ctrl \ */
    signal(SIGTTIN, SIG_IGN); /* To ignore background processes */
}