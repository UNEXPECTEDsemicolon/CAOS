#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>

int main()
{
    pid_t pid;
    size_t proc_cnt = 1;
    while ((pid = fork()) == 0)
        ++proc_cnt;
    if (-1 == pid)
        printf("%zu\n", proc_cnt);
    else
        waitpid(pid, NULL, 0);
}