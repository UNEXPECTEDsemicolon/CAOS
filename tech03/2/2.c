#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <linux/limits.h>

// #define MAX_PATH 11 // log_10(2^32) + 1
// #define BUFF_CNT 1048576 // 4MB
#define BUFF_CNT 2 // 4MB
// #define BUFF_CNT 4
#define BUFF_SIZE (BUFF_CNT * sizeof(int))

#define min(x, y) ((x) < (y) ? (x) : (y))

#define get_path(i)           \
    char path[PATH_MAX] = {}; \
    sprintf(path, "./temp/%zu", (i) / BUFF_CNT)

#define get_new_path(i)           \
    char new_path[PATH_MAX] = {}; \
    sprintf(new_path, "./temp/%zu_n", (i) / BUFF_CNT)

#define safe_exit(return_code)      \
    if (return_code)                \
        perror("Unexpected error"); \
    close(fd);                      \
    exit(return_code)

int buff_read_1[BUFF_CNT];
int buff_read_2[BUFF_CNT];
int buff_write[BUFF_CNT];

typedef struct
{
    int id;
    int fd;
    int* buffer;
} FileInfo;

void read_commit(FileInfo* file)
{
    if (file->id != -1)
    {
        close(file->fd);
    }
}

void write_commit(FileInfo* file)
{
    if (file->id != -1)
    {
        write(file->fd, file->buffer, BUFF_SIZE);
        close(file->fd);
    }
}

int get_int_at(FileInfo* file, size_t i)
{
    if (file->id != i / BUFF_CNT)
    {
        read_commit(file);
        file->id = i / BUFF_CNT;
        get_path(i);
        file->fd = open(path, O_RDONLY);
        read(file->fd, file->buffer, BUFF_SIZE);
    }
    return file->buffer[i % BUFF_CNT];
}

void put_int_at(FileInfo *file, size_t i, int x)
{
    if (file->id != i / BUFF_CNT)
    {
        write_commit(file);
        file->id = i / BUFF_CNT;
        get_new_path(i);
        file->fd = open(new_path, O_WRONLY | O_CREAT, 0666);
    }
    file->buffer[i % BUFF_CNT] = x;
}

void finalize_write(size_t n)
{
    for (size_t id = 0; id * BUFF_CNT < n; ++id)
    {
        get_new_path(id);
        get_path(id);
        rename(new_path, path);
    }
}

// void remove_tmp(size_t n)
// {
//     for (size_t id = 0; id * BUFF_CNT < n; ++id)
//     {
//         get_new_path(id);
//         get_path(id);
//         rename(new_path, path);
//     }
// }

void merge(size_t l1, size_t r1, size_t l2, size_t r2, size_t n)
{
    size_t i = l1;
    size_t j = l2;
    FileInfo inp_file1 = {-1, -1, buff_read_1};
    FileInfo inp_file2 = {-1, -1, buff_read_2};
    FileInfo out_file = {-1, -1, buff_write};
    while (i <= r1 && j <= r2)
    {
        int x1 = get_int_at(&inp_file1, i);
        int x2 = get_int_at(&inp_file2, j);
        if (x1 <= x2)
        {
            put_int_at(&out_file, (i++) + j - l2, x1);
        }
        else
        {
            put_int_at(&out_file, i + (j++) - l2, x2);
        }
    }
    while (i <= r1)
    {
        int x1 = get_int_at(&inp_file1, i);
        put_int_at(&out_file, (i++) + j - l2, x1);
    }
    while (j <= r2)
    {
        int x2 = get_int_at(&inp_file2, j);
        put_int_at(&out_file, i + (j++) - l2, x2);
    }
    read_commit(&inp_file1);
    read_commit(&inp_file2);
    write_commit(&out_file);
    finalize_write(n);
}

int main(int argc, char **argv)
{
    int fd = open(argv[1], O_RDONLY);
    if (fd == -1)
    {
        perror("Can't open input file");
        exit(1);
    }
    size_t n = lseek(fd, 0, SEEK_END) / sizeof(int);
    if (n <= 0)
    {
        safe_exit(-n);
    }
    if (-1 == lseek(fd, 0, SEEK_SET))
    {
        safe_exit(1);
    }
    size_t temp; // TODO: delete
    for (size_t id = 0; temp = read(fd, buff_write, BUFF_SIZE); ++id)
    {
        get_path(id * BUFF_CNT);
        int fd_write = open(path, O_WRONLY | O_CREAT, 0666);
        write(fd_write, buff_write, BUFF_SIZE);
        close(fd_write);
    }
    close(fd);
    for (size_t k = 0; 1ull << k <= n; ++k)
    {
        size_t delta = 1ull << k;
        for (size_t i = 0; i + delta < n; i += 2 * delta)
        {
            merge(i, i + delta - 1, i + delta, min(i + 2 * delta - 1, n-1), n);
        }
    }
    fd = open(argv[1], O_WRONLY);

    FileInfo file = {-1, -1, buff_write};
    size_t i;
    for (size_t id = 0; (i = id * BUFF_CNT) < n; ++id)
    {
        get_int_at(&file, i);
        write(fd, buff_write, min(BUFF_CNT, n - i) * sizeof(int));
    }
    read_commit(&file);

    return 0;
}