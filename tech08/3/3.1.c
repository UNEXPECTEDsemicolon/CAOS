#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/wait.h>
#include <error.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

void close_all(const int *pipe_fds, size_t size)
{
    for (size_t i = 0; i < size; ++i)
    {
        close(pipe_fds[2 * i + 0]);
        close(pipe_fds[2 * i + 1]);
    }
}

pid_t launch(const char *cmd, int fd_in, int fd_out, const int *pipe_fds, size_t size)
{
    pid_t pid = fork();
    if (0 == pid)
    {
        if (fd_in != STDIN_FILENO)
        {
            if (-1 == dup2(fd_in, STDIN_FILENO))
                return -1;
        }
        if (fd_out != STDOUT_FILENO)
        {
            if (-1 == dup2(fd_out, STDOUT_FILENO))
                return -1;
        }
        close_all(pipe_fds, size);
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
    int out_pipe_fds[argc - 1][2];
    pid_t childs[argc - 1];
    for (size_t i = 1; i < argc; ++i)
    {
        pipe(out_pipe_fds[i-1]);
        childs[i] = launch(argv[i],
                           (i == 1 ? STDIN_FILENO : in_pipe_fd[0]),
                           (i == argc - 1 ? STDOUT_FILENO : out_pipe_fds[i - 1][1]),
                           (const int *)out_pipe_fds, i);
        memcpy(in_pipe_fd, out_pipe_fds[i-1], sizeof(out_pipe_fds[i-1]));
    }
    close_all((const int *)out_pipe_fds, argc - 1);
    for (size_t i = argc; i > 0; --i)
        waitpid(childs[i-1], NULL, 0);
}
