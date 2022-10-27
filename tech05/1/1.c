#include <sys/mman.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct
{
    int value;
    uint32_t next_pointer;
} Item;

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        perror("Invalid arguments");
        exit(1);
    }
    int fd = open(argv[1], O_RDONLY);

    if (-1 == fd)
    {
        perror("Can't open file");
        exit(1);
    }

    struct stat statbuf = {};
    if (-1 == fstat(fd, &statbuf))
    {
        perror("Can't get file stat");
        close(fd);
        exit(1);
    }

    size_t filesize = statbuf.st_size;
    if (filesize == 0)
        exit(0);
    if (filesize % sizeof(Item) != 0)
    {
        perror("Wrong data");
        exit(1);
    }

    char *mem = mmap(NULL, filesize, PROT_READ, MAP_SHARED, fd, 0);
    close(fd);

    if (MAP_FAILED == mem)
    {
        perror("mmap");
        exit(1);
    }

    Item node;
    char* pos = mem;
    do
    {
        node.value = *(int *)(pos);
        printf("%d\n", node.value);
        node.next_pointer = *(uint32_t *)(pos + sizeof(node.value));
        pos = mem + node.next_pointer;
    } while (node.next_pointer != 0);

    munmap(mem, filesize);
}