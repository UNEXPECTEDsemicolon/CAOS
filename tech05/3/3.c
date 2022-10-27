#include <sys/mman.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

typedef struct __attribute__((__packed__))
{
    size_t size : sizeof(size_t) * 8 - 1;
    bool is_free : 1;
} BlockHeader; // if not header.is_free then header.size == (size_t)(header)

static struct
{
    void *mem;
    size_t filesize;
    void *end;
    size_t total_used;
} info;

extern void myalloc_initialize(int fd)
{
    sizeof(BlockHeader);
    struct stat statbuf = {};
    if (-1 == fstat(fd, &statbuf))
    {
        perror("Can't get file stat");
        exit(1);
    }
    info.filesize = statbuf.st_size;
    if (info.filesize < sizeof(BlockHeader))
    {
        errno = ENOMEM;
        perror("File too small");
        exit(1);
    }
    info.mem = mmap(NULL,
                    info.filesize,
                    PROT_READ | PROT_WRITE,
                    MAP_SHARED,
                    fd,
                    0);
    if (MAP_FAILED == info.mem)
    {
        perror("mmap");
        exit(1);
    }
    info.end = info.mem + info.filesize;
    info.total_used = 0;
    BlockHeader start_header = {.is_free = true, .size = info.filesize - sizeof(BlockHeader)};
    *(BlockHeader *)(info.mem) = start_header;
}

extern void myalloc_finalize()
{
    munmap(info.mem, info.filesize);
}

extern void *my_malloc(size_t size)
{
    if (size > info.filesize - info.total_used)
    {
        errno = ENOMEM;
        return NULL;
    }
    if (size == 0)
        return NULL;
    void *pos = info.mem;
    BlockHeader header;
    bool is_block_ok;
    do
    {
        header = *(BlockHeader *)(pos);
        pos += sizeof(header) + header.size;
        if (header.is_free && pos != info.end)
        {
            BlockHeader next_header = *(BlockHeader *)(pos);
            while (next_header.is_free && pos != info.end)
            {
                header.size += sizeof(next_header) + next_header.size;
                pos += sizeof(next_header) + next_header.size;
                next_header = *(BlockHeader *)(pos);
            }
        }
        is_block_ok = header.is_free && ((ssize_t)header.size - (ssize_t)size >= 0);
    } while (!is_block_ok && info.end != pos);

    if (is_block_ok)
    {
        if (header.size - size > sizeof(header))
        {
            BlockHeader new_header = {.is_free = true, .size = header.size - size - sizeof(header)};
            *(BlockHeader *)(pos - header.size + size) = new_header;
        }
        else
            size = header.size;
        info.total_used += size + sizeof(header);
        pos -= header.size;
        header.size = size;
        header.is_free = false;
        *(BlockHeader *)(pos - sizeof(header)) = header;
        return (void *)(pos);
    }
    else
    {
        errno = ENOMEM;
        return NULL;
    }
}

extern void my_free(void *ptr)
{
    if (ptr == NULL)
        return;
    if (ptr < info.mem || ptr >= info.end)
    {
        errno = EFAULT;
        perror("Can't free given ptr");
        exit(1);
    }
    BlockHeader *header = (BlockHeader *)(ptr - sizeof(BlockHeader));
    header->is_free = true;
    info.total_used -= header->size + sizeof(BlockHeader);
}
