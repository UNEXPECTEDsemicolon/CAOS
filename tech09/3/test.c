#define _GNU_SOURCE

#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <stdbool.h>
#include <fcntl.h>
#include <errno.h>
#include <error.h>
#include <string.h>
#include <limits.h>

void handler(int signo, siginfo_t *info, void *ucontext __attribute__((unused)))
{
    union sigval val = info->si_value;
    sigqueue(info->si_pid, signo, val);
}

int main(int argc, char **argv)
{
    struct sigaction sig_act = {};
    sig_act.sa_flags = SA_SIGINFO;
    sig_act.sa_sigaction = handler;
    sigset_t blocked_set = {};
    sigaction(SIGRTMIN, &sig_act, NULL);
    sigfillset(&blocked_set);
    sigdelset(&blocked_set, SIGRTMIN);
    sigprocmask(SIG_BLOCK, &blocked_set, NULL);
    union sigval val = {};
    val.sival_int = 5;
    int pid;
    scanf("%d", &pid);
    sigqueue(pid, SIGRTMIN, val);
    while (true)
        pause();
}