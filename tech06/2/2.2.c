#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <sys/mman.h>

int main()
{
    size_t i = 0;
    size_t *mem = mmap(NULL, sizeof(i), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (MAP_FAILED == mem)
    {
        perror("mmap");
        exit(1);
    }
    *mem = i;
    bool is_root = true;
    for (; EOF != scanf("%*s"); ++i)
    {
        pid_t pid = fork();
        if (pid > 0)
        {
            waitpid(pid, NULL, 0);
            if (is_root)
                break;
            exit(0);
        }
        else if (0 == pid)
            is_root = false;
        else
        {
            perror("fork");
            exit(1);
        }
    }
    if (is_root)
        printf("%zu\n", *mem);
    else
    {
        *mem = i;
    }
}