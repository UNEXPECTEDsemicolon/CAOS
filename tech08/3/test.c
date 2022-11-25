#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
    char buff[100];
    write(STDOUT_FILENO, "Begin while\n", sizeof("Begin while\n") - 1);
    size_t len;
    const char msg[] = "Get!\n";
    while ((len = read(STDIN_FILENO, buff, sizeof(buff))) > 0)
        write(STDOUT_FILENO, buff, len);
    write(STDOUT_FILENO, "Exit\n", sizeof("Exit\n") - 1);
    // fflush(stdout);
    // close(STDOUT_FILENO);
}