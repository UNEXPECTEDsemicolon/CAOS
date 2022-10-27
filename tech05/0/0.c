#include <sys/mman.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv)
{
    if (argc < 3)
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
    size_t str_len = strlen(argv[2]);
    if (filesize < str_len)
        exit(0);

    char *mem = mmap(NULL,
                     filesize,
                     PROT_READ,
                     MAP_SHARED,
                     fd,
                     0);
    close(fd);

    if (MAP_FAILED == mem)
    {
        perror("mmap");
        exit(1);
    }

    for (size_t i = 0; i < filesize - str_len + 1; ++i)
    {
        if (memcmp(mem + i, argv[2], str_len) == 0)
            printf("%zu ", i);
    }

    printf("\n");

    munmap(mem, filesize);
}