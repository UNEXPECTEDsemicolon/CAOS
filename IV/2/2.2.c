#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <linux/limits.h>

#define BUFF_CNT 1048576 // 4MB
#define BUFF_SIZE (BUFF_CNT * sizeof(int))

#define min(x, y) ((x) < (y) ? (x) : (y))

#define CHECK_OK3(func, fd1, fd2, fd3, msg) \
    if (-1 == (func))                       \
    {                                       \
        perror(msg);                        \
        if ((fd1) >= 0)                     \
            close(fd1);                     \
        if ((fd2) >= 0)                     \
            close(fd2);                     \
        if ((fd3) >= 0)                     \
            close(fd3);                     \
        exit(1);                            \
    }

#define CHECK_OK2(func, fd1, fd2, msg) \
    CHECK_OK3(func, fd1, fd2, -1, msg)

#define CHECK_OK1(func, fd, msg) \
    CHECK_OK2(func, fd, -1, msg)

#define CHECK_OK0(func, msg) \
    CHECK_OK1(func, -1, msg)

#define SAFE_CALL(func) \
    if (-1 == (func))   \
        return -1

int buff_read_1[BUFF_CNT];
int buff_read_2[BUFF_CNT];
int buff_write[BUFF_CNT];

typedef struct
{
    int id;
    int fd;
    int* buffer;
    size_t size;
    size_t av_size;
} FileInfo;

int reopen_at(FileInfo* file, size_t i)
{
    file->id = i / BUFF_CNT;
    SAFE_CALL(lseek(file->fd, file->id * BUFF_SIZE, SEEK_SET));
    file->av_size = file->size - file->id * BUFF_SIZE;
    SAFE_CALL(read(file->fd, file->buffer, min(file->av_size, BUFF_SIZE)));
    return 0;
}

int write_commit(FileInfo* file)
{
    if (file->id != -1)
    {
        SAFE_CALL(write(file->fd, file->buffer, min(file->av_size, BUFF_SIZE)));
        file->id = -1;
    }
    return 0;
}

int get_int_at(FileInfo* file, size_t i, int* x)
{
    if (file->id != i / BUFF_CNT)
    {
        SAFE_CALL(reopen_at(file, i));
    }
    *x = file->buffer[i % BUFF_CNT];
    return 0;
}

int put_int_at(FileInfo *file, size_t i, int x)
{
    if (file->id != i / BUFF_CNT)
    {
        SAFE_CALL(write_commit(file));
        SAFE_CALL(reopen_at(file, i));
    }
    file->buffer[i % BUFF_CNT] = x;
    return 0;
}

int merge(size_t l1, size_t r1, size_t l2, size_t r2, size_t n,
           FileInfo *inp_file1, FileInfo *inp_file2, FileInfo* out_file)
{
    size_t i = l1;
    size_t j = l2;
    while (i <= r1 && j <= r2)
    {
        int x1, x2;
        SAFE_CALL(get_int_at(inp_file1, i, &x1));
        SAFE_CALL(get_int_at(inp_file2, j, &x2));
        if (x1 <= x2)
        {
            SAFE_CALL(put_int_at(out_file, (i++) + j - l2, x1));
        }
        else
        {
            SAFE_CALL(put_int_at(out_file, i + (j++) - l2, x2));
        }
    }
    while (i <= r1)
    {
        int x1;
        SAFE_CALL(get_int_at(inp_file1, i, &x1));
        SAFE_CALL(put_int_at(out_file, (i++) + j - l2, x1));
    }
    while (j <= r2)
    {
        int x2;
        SAFE_CALL(get_int_at(inp_file2, j, &x2));
        SAFE_CALL(put_int_at(out_file, i + (j++) - l2, x2));
    }
    return 0;
}

int main(int argc, char **argv)
{
    int fd = open(argv[1], O_RDONLY);
    CHECK_OK0(fd, "Can't open input file");
    size_t n = lseek(fd, 0, SEEK_END) / sizeof(int);
    CHECK_OK1(n, fd, "Unexpected error");
    CHECK_OK1(lseek(fd, 0, SEEK_SET), fd, "Unexpected error");

    for (size_t k = 0; 1ull << k <= n; ++k)
    {
        int fd1 = open(argv[1], O_RDONLY);
        CHECK_OK1(fd1, fd1, "Can't open input file");
        int fd2 = open(argv[1], O_RDONLY);
        CHECK_OK2(fd2, fd1, fd2, "Can't open input file");
        FileInfo inp_file1 = {-1, fd1, buff_read_1, n * sizeof(int), n * sizeof(int)};
        FileInfo inp_file2 = {-1, fd2, buff_read_2, n * sizeof(int), n * sizeof(int)};
        char temp_path[] = "temp";
        int fd_temp = open(temp_path, O_RDWR | O_CREAT, 0666);
        CHECK_OK2(fd_temp, fd1, fd2, "Can't create temp file");
        FileInfo out_file = {-1, fd_temp, buff_write, n * sizeof(int), n * sizeof(int)};
        size_t delta = 1ull << k;
        for (size_t i = 0; i < n; i += 2 * delta)
        {
            CHECK_OK3(merge(i, min(i + delta - 1, n - 1), min(i + delta, n), min(i + 2 * delta - 1, n - 1),
                            n, &inp_file1, &inp_file2, &out_file),
                      fd1, fd2, fd_temp, "Unexpected error");
        }
        CHECK_OK3(write_commit(&out_file), fd1, fd2, fd_temp, "Unexpected error");
        close(fd_temp);
        close(fd1);
        close(fd2);
        CHECK_OK0(rename(temp_path, argv[1]), "Can't rename temp file");
    }
    return 0;
}
