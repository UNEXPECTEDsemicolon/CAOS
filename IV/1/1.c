#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>

#define safe_exit(return_code)      \
    if (return_code)                \
        perror("Unexpected error"); \
    close(fd);                      \
    exit(return_code)

typedef struct
{
    int value;
    uint32_t next_pointer;
} Item;

int main(int argc, char **argv)
{
    int fd = open(argv[1], O_RDONLY);
    if (fd == -1)
    {
        perror("Can't open input file");
        exit(1);
    }
    size_t file_size = lseek(fd, 0, SEEK_END);
    if (file_size <= 0)
    {
        safe_exit(-file_size);
    }
    if (-1 == lseek(fd, 0, SEEK_SET))
    {
        safe_exit(1);
    }
    Item node;
    do
    {
        if (-1 == read(fd, &node.value, sizeof(node.value)))
        {
            safe_exit(1);
        }
        printf("%d\n", node.value);
        if (-1 == read(fd, &node.next_pointer, sizeof(node.next_pointer)))
        {
            safe_exit(1);
        }
        if (-1 == lseek(fd, node.next_pointer, SEEK_SET))
        {
            safe_exit(1);
        }
    } while (node.next_pointer != 0);
}