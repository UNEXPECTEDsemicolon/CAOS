#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <error.h>

int main()
{
    char buff[100];
    write(STDOUT_FILENO, "Begin while 2\n", sizeof("Begin while 2\n") - 1);
    size_t len;
    const char msg[] = "Get: ";
    while ((len = read(STDIN_FILENO, buff, sizeof(buff))) > 0)
    {
        if (errno)
        {
            perror("");
            // break;
        }
        write(STDOUT_FILENO, msg, sizeof(msg)-1);
        write(STDOUT_FILENO, buff, len);
    }
    write(STDOUT_FILENO, "Exit 2\n", sizeof("Exit 2\n") - 1);
}