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

size_t files_size;
FILE **files;

void process_SIGRT(int signo)
{
    if (signo == SIGRTMIN)
    {
        for (size_t i = 0; i < files_size; ++i)
            if (NULL != files[i])
                fclose(files[i]);
        free(files);
        exit(0);
    }
    char *buff = NULL;
    size_t buff_len;
    FILE *file = files[signo - SIGRTMIN];
    if (NULL == file)
    {
        errno = ENOENT;
        perror("Wrong file index");
        free(files);
        exit(1);
    }
    if (-1 == getline(&buff, &buff_len, file))
    {
        perror("Can't read from file");
        free(files);
        exit(1);
    }
    printf("%s", buff);
    fflush(stdout);
}

int main(int argc, char **argv)
{
    files_size = SIGRTMAX - SIGRTMIN;
    files = malloc(sizeof(int *) * files_size);
    memset(files, 0, files_size);
    for (size_t i = 1; i <= argc; ++i)
    {
        char abs_path[PATH_MAX];
        realpath(argv[i], abs_path);
        files[i] = fopen(abs_path, "r");
        if (NULL == files[i])
        {
            perror("Can't open file");
            free(files);
            exit(1);
        }
    }
    struct sigaction sig_act = {};
    sig_act.sa_handler = process_SIGRT;
    sigset_t blocked_set = {};
    sigfillset(&blocked_set);
    for (int signo = SIGRTMIN; signo <= SIGRTMAX; ++signo)
    {
        sigdelset(&blocked_set, signo);
        sigaction(signo, &sig_act, NULL);
    }
    sigprocmask(SIG_BLOCK, &blocked_set, NULL);
    while (true)
        pause();
}