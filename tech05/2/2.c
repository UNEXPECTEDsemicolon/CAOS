#include <sys/mman.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        perror("Invalid arguments");
        exit(1);
    }
    int fd = open(argv[1], O_RDWR | O_CREAT, 0666);

    if (-1 == fd)
    {
        perror("Can't open file");
        exit(1);
    }

    size_t n = atol(argv[2]);
    size_t w = atol(argv[3]);
    ssize_t line_len = n * w + 1;
    size_t filesize = n * line_len;

    if (-1 == ftruncate(fd, filesize))
    {
        perror("Can't set file size");
        exit(1);
    }

    if (filesize == 0)
        exit(0);

    char *mem = mmap(NULL,
                     filesize,
                     PROT_READ | PROT_WRITE,
                     MAP_SHARED,
                     fd,
                     0);
    close(fd);

    memset(mem, '\0', filesize);

    if (MAP_FAILED == mem)
    {
        perror("mmap");
        exit(1);
    }

    ssize_t left = 0, right = n - 1, top = 0, bottom = n - 1;
    char format_str[20] = {};
    sprintf(format_str, "%%%zuzu", w);
    size_t val = 1, n_sqr = n * n;
    while (val <= n_sqr)
    {
        for (ssize_t i = left; i <= right; ++i)
            sprintf(mem + top * line_len + i * w, format_str, val++);
        ++top;
        for (ssize_t i = top; i <= bottom; ++i)
            sprintf(mem + i * line_len + right * w, format_str, val++);
        --right;
        for (ssize_t i = right; i >= left; --i)
            sprintf(mem + bottom * line_len + i * w, format_str, val++);
        --bottom;
        for (ssize_t i = bottom; i >= top; --i)
            sprintf(mem + i * line_len + left * w, format_str, val++);
        ++left;
    }
    for (ssize_t i = 1; i <= n; ++i)
    {
        mem[line_len * i - 1] = '\n';
    }
    for (size_t i = 0; i < filesize; ++i)
    {
        if (mem[i] == '\0')
            mem[i] = ' ';
    }

    munmap(mem, filesize);
}