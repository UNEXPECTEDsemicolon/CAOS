#define _GNU_SOURCE

#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdbool.h>

volatile sig_atomic_t sigint_flag = 0;
volatile sig_atomic_t sigterm_flag = 0;

void process_sigint(int _ __attribute__((unused)))
{
    sigint_flag = 1;
}

void process_sigterm(int _ __attribute__((unused)))
{
    sigterm_flag = 1;
}

int main()
{
    struct sigaction sigint_act = {};
    sigint_act.sa_handler = process_sigint;
    sigaction(SIGINT, &sigint_act, NULL);
    struct sigaction sigterm_act = {};
    sigterm_act.sa_handler = process_sigterm;
    sigaction(SIGTERM, &sigterm_act, NULL);

    printf("%d\n", getpid());
    fflush(stdout);

    size_t cnt = 0;
    while (true)
    {
        if (sigint_flag)
        {
            sigint_flag = 0;
            ++cnt;
        }
        if (sigterm_flag)
        {
            printf("%zu\n", cnt);
            return 0;
        }
        pause();
    }
}