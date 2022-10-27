#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <stddef.h>
#include <stdbool.h>
#include <unistd.h>
#include <bits/posix1_lim.h>
#include <libgen.h>
#include <stdio.h>
#include <stdint.h>
#include <linux/limits.h>
#include <fcntl.h>

#define FUSE_USE_VERSION 30 // API version 3.0
#include <fuse.h>

typedef struct vector
{
    uint8_t *data;
    size_t size;
    size_t cap;
} vector;

#define vector_init(this, n, type)                   \
    (this).data = realloc(NULL, (n) * sizeof(type)); \
    (this).size = 0;                                 \
    (this).cap = (n)

#define vector_push_back(this, elem, type)                 \
    if ((this).size == (this).cap)                         \
    {                                                      \
        if (NULL == realloc((this).data, (this).cap *= 2)) \
        {                                                  \
            perror("Can't realloc array of filenames");    \
            safe_exit(1);                                  \
        }                                                  \
    }                                                      \
    memcpy(((type *)((this).data)) + (this).size++, &(elem), sizeof(type))

#define vector_at(this, i, type) ((type *)((this).data))[i]

#define vector_delete(this, type) free((type *)((this).data))

vector files;
FILE *file;

typedef struct FileInfo
{
    char path[PATH_MAX];
    size_t size;
    size_t begin;
} FileInfo;

#define safe_exit(return_code)      \
    vector_delete(files, FileInfo); \
    fclose(file);                   \
    exit(return_code)

FileInfo* find(const char* path)
{
    char filename_buff[PATH_MAX];
    char* filename = filename_buff;
    strcpy(filename, path);
    filename = basename(filename);
    for (size_t i = 0; i < files.size; ++i)
    {
        FileInfo* cur_file = &(vector_at(files, i, FileInfo));
        if (strcmp(cur_file->path, filename) == 0)
            return cur_file;
    }
    return NULL;
}

int my_stat(const char *path, struct stat *st, struct fuse_file_info *fi)
{
    if (0 == strcmp("/", path))
    {
        st->st_mode = 0555 | S_IFDIR;
        st->st_nlink = 2;
        return 0;
    }
    
    FileInfo *cur_file = find(path);
    if (NULL == cur_file)
    {
        return -ENOENT;
    }

    st->st_mode = S_IFREG | 0444;
    st->st_nlink = 1;
    st->st_size = cur_file->size;
    return 0;
}

int my_readdir(const char *path, void *out, fuse_fill_dir_t filler, off_t off,
               struct fuse_file_info *fi, enum fuse_readdir_flags flags)
{
    if (0 != strcmp(path, "/"))
    {
        return -ENOENT;
    }

    filler(out, ".", NULL, 0, 0);
    filler(out, "..", NULL, 0, 0);

    for (size_t i = 0; i < files.size; ++i)
    {
        filler(out, basename(vector_at(files, i, FileInfo).path), NULL, 0, 0);
    }

    return 0;
}

int my_open(const char *path, struct fuse_file_info *fi)
{
    FileInfo *cur_file = find(path);
    if (NULL == cur_file)
    {
        return -ENOENT;
    }

    if (O_RDONLY != (fi->flags & O_ACCMODE))
    {
        return -EACCES;
    }
    fi->fh = (uint64_t)cur_file;
    return 0;
}

int my_read(const char *path, char *out, size_t size, off_t off,
            struct fuse_file_info *fi)
{
    FileInfo *cur_file = (FileInfo *)(fi->fh);

    if (off > cur_file->size)
        return 0;
    
    if (off + size > cur_file->size)
        size = cur_file->size - off;

    if (-1 == lseek(fileno(file), cur_file->begin + off, SEEK_SET))
    {
        perror("lseek error");
        return -errno;
    }
    if (-1 == read(fileno(file), out, size))
    {
        perror("Can't read src file");
        return -errno;
    }

    return size;
}

struct fuse_operations operations = {
    .readdir = my_readdir,
    .getattr = my_stat,
    .open = my_open,
    .read = my_read,
};

void open_filesystem(const char* src)
{
    file = fopen(src, "r");
    if (NULL == file)
    {
        perror("Can't open src file");
        exit(1);
    }

    size_t offset;

    char line_buffer[PATH_MAX * 2] = {};
    char *line = line_buffer;
    size_t line_len;
    line_len = getline(&line, &line_len, file);
    offset += line_len;

    size_t n;
    if (0 >= sscanf(line, "%zu", &n))
    {
        perror("Wrong src file format");
        fclose(file);
        exit(1);
    }

    vector_init(files, n, FileInfo);

    for (size_t i = 0; i < n; ++i)
    {
        FileInfo new_file;
        line_len = getline(&line, &line_len, file);
        if (0 >= sscanf(line, "%s %zu", new_file.path, &(new_file.size)))
        {
            perror("Wrong src file format");
            safe_exit(1);
        }
        vector_push_back(files, new_file, FileInfo);
        offset += line_len;
    }
    ++offset;
    for (size_t i = 0; i < n; ++i)
    {
        FileInfo *cur_file = &(vector_at(files, i, FileInfo));
        cur_file->begin = offset;
        offset += cur_file->size;
    }
}

int main(int argc, char *argv[])
{
    struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

    typedef struct
    {
        char *src;
    } my_options_t;

    my_options_t my_options;
    memset(&my_options, 0, sizeof(my_options));

    struct fuse_opt opt_specs[] = {
        {"--src %s", offsetof(my_options_t, src), 0},
        {NULL, 0, 0}};

    fuse_opt_parse(&args, &my_options, opt_specs, NULL);

    if (my_options.src)
    {
        open_filesystem(my_options.src);
    }

    int ret = fuse_main(args.argc, args.argv, &operations, NULL);

    safe_exit(0);

    return ret;
}