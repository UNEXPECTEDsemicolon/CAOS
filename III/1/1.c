#include <sys/syscall.h>
#include <unistd.h>

extern long syscall(long number, ...);


int _start()
{
    const unsigned long buff_size = 4096;
    char buff[buff_size];
    unsigned long input_len;
    while (input_len = syscall(SYS_read, STDIN_FILENO, buff, buff_size))
        syscall(SYS_write, STDOUT_FILENO, buff, input_len);
    syscall(SYS_exit, 0);
}