#define _GNU_SOURCE

#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdbool.h>

volatile sig_atomic_t delta_flag = false;
volatile sig_atomic_t mult_flag = false;

void process_delta(int _ __attribute__((unused)))
{
    delta_flag = true;
}

void process_mult(int _ __attribute__((unused)))
{
    mult_flag = true;
}

void process_exit(int _ __attribute__((unused)))
{
    _exit(0);
}

int main()
{
    struct sigaction sigusr1_act = {};
    sigusr1_act.sa_handler = process_delta;
    sigaction(SIGUSR1, &sigusr1_act, NULL);
    struct sigaction sigusr2_act = {};
    sigusr2_act.sa_handler = process_mult;
    sigaction(SIGUSR2, &sigusr2_act, NULL);
    struct sigaction sigexit_act = {};
    sigexit_act.sa_handler = process_exit;
    sigaction(SIGTERM, &sigexit_act, NULL);
    sigaction(SIGINT, &sigexit_act, NULL);

    printf("%d\n", getpid());
    fflush(stdout);

    int val = 0;
    scanf("%d", &val);
    fflush(stdin);
    while (true)
    {
        if (delta_flag)
        {
            delta_flag = false;
            ++val;
            printf("%d\n", val);
            fflush(stdout);
        }
        if (mult_flag)
        {
            mult_flag = false;
            val *= -1;
            printf("%d\n", val);
            fflush(stdout);
        }
        pause();
    }
}