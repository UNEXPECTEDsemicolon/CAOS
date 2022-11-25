#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

void print(size_t i, bool end)
{
    char buff[20];
    size_t size = sprintf(buff, end ? "%zu\n" : "%zu ", i);
    write(STDOUT_FILENO, buff, size);
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        errno = EINVAL;
        perror("Invalid args");
    }
    size_t N = atol(argv[1]);
    print(1, 1 == N);
    pid_t pid;
    for (size_t i = 2; i <= N; ++i)
    {
        pid = fork();
        if (pid != 0)
        {
            if (-1 != pid)
                waitpid(pid, NULL, 0);
            else
                perror("fork");
            break;
        }
        print(i, i == N);
    }
}