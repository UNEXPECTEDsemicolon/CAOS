#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/wait.h>
#include <error.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

pid_t launch(const char *cmd, int fd_in, int fd_out)
{
    pid_t pid = fork();
    if (0 == pid)
    {
        if (fd_in != STDIN_FILENO)
        {
            if (-1 == dup2(fd_in, STDIN_FILENO))
                return -1;
            printf("fd_id=%d\n", fd_in);
            close(fd_in);
            close(4);
            close(5);
            close(6);
        }
        if (fd_out != STDOUT_FILENO)
        {
            if (-1 == dup2(fd_out, STDOUT_FILENO))
                return -1;
            printf("fd_out=%d\n", fd_out);
            close(fd_out);
            close(3);
            close(5);
            close(6);
        }
        execlp(cmd, cmd, NULL);
        return -1;
    }
    return pid;
}

int main(int argc, const char *argv[])
{
    if (argc < 2)
    {
        errno = E2BIG;
        perror("Wrong args number!");
        exit(1);
    }
    int in_pipe_fd[2];
    int out_pipe_fd[argc-1][2];
    pid_t last_child;
    for (size_t i = 1; i < argc; ++i)
    {
        pipe(out_pipe_fd[i-1]);
        printf("i=%zu: out_pipe_fd = {%d, %d}\n", i, out_pipe_fd[i-1][0], out_pipe_fd[i-1][1]);
        last_child = launch(argv[i],
                            (i == 1 ? STDIN_FILENO : in_pipe_fd[0]),
                            (i == argc - 1 ? STDOUT_FILENO : out_pipe_fd[i-1][1]));
        memcpy(in_pipe_fd, out_pipe_fd[i-1], sizeof(out_pipe_fd[i-1]));
    }
    for (size_t i = 0; i < argc - 1; ++i)
    {
        close(out_pipe_fd[i][0]);
        close(out_pipe_fd[i][1]);
    }
    printf("Before wait\n");
    waitpid(last_child, NULL, 0);
}
