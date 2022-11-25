
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
#include <stdint.h>

void handler(int signo, siginfo_t *info, void *ucontext __attribute__((unused)))
{
    union sigval val = info->si_value;
    uint32_t N = val.sival_int;
    if (0 == N)
        _exit(0);
    val.sival_int = --N;
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
    while (true)
        pause();
}