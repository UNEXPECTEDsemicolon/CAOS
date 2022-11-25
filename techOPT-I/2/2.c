#define _GNU_SOURCE
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <stddef.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <linux/limits.h>
#include <fcntl.h>
#include <dirent.h>
#include <strings.h>
#include <limits.h>
#include <zip.h>

#define FUSE_USE_VERSION 30 // API version 3.0
#include <fuse.h>

size_t get_depth(const char* path)
{
    size_t depth = 0;
    for (const char *begin = path - 1; (begin = strchr(begin + 1, '/')) != NULL; ++depth);
    size_t path_len = strlen(path);
    return depth;
}

zip_t* archieve;

int my_stat(const char *path, struct stat *st, struct fuse_file_info *fi)
{
    ++path;
    printf("my_stat: path=%s\n", path);
    if (path[0] == '\0')
    {
        st->st_mode = S_IFDIR;
        st->st_mode = (st->st_mode & S_IFMT) | 0555;
        return 0;
    }
    zip_stat_t stat_buf;
    zip_stat_init(&stat_buf);
    if (-1 == zip_stat(archieve, path, 0, &stat_buf))
    {
        zip_stat_init(&stat_buf);
        char valid_path[PATH_MAX] = {};
        sprintf(valid_path, "%s/", path);
        if (-1 == zip_stat(archieve, valid_path, 0, &stat_buf))
            return -ENOENT;
    }
    st->st_mtime = stat_buf.mtime;
    st->st_size = stat_buf.size;
    bool is_dir = stat_buf.name[strlen(stat_buf.name) - 1] == '/';
    st->st_mode = is_dir ? S_IFDIR : S_IFREG;
    st->st_mode = (st->st_mode & S_IFMT) | (S_ISDIR(st->st_mode) ? 0555 : 0444);
    return 0;
}

int my_readdir(const char *path, void *out, fuse_fill_dir_t filler, off_t off,
               struct fuse_file_info *fi, enum fuse_readdir_flags flags)
{
    ++path;
    printf("my_readdir: path=%s\n", path);
    char valid_path[PATH_MAX] = {};
    sprintf(valid_path, "%s/", path);
    zip_int64_t file_ind = zip_name_locate(archieve, valid_path, 0);
    bool is_root = false;
    if (path[0] == '\0')
    {
        file_ind = 0;
        is_root = true;
        valid_path[0] = '\0';
    }
    if (-1 == file_ind)
    {
        return -ENOENT;
    }

    filler(out, ".", NULL, 0, 0);
    filler(out, "..", NULL, 0, 0);

    size_t parent_depth = get_depth(valid_path);
    for (zip_int64_t i = file_ind + !is_root; ; ++i)
    {
        const char *child = zip_get_name(archieve, i, 0);
        if (child == NULL || strstr(child, valid_path) != child)
            break;
        char valid_child[PATH_MAX] = {};
        size_t child_len = strlen(child);
        strncpy(valid_child, child, child_len - (child_len > 0 && child[child_len - 1] == '/'));
        if (get_depth(valid_child) == parent_depth)
            filler(out, basename(valid_child), NULL, 0, 0);
    }

    return 0;
}

int my_open(const char *path, struct fuse_file_info *fi)
{
    if (O_RDONLY != (fi->flags & O_ACCMODE))
    {
        return -EACCES;
    }
    ++path;
    zip_file_t *fd = zip_fopen(archieve, path, ZIP_RDONLY);
    if (NULL == fd)
    {
        char valid_path[PATH_MAX] = {};
        sprintf(valid_path, "%s/", path);
        fd = zip_fopen(archieve, valid_path, ZIP_RDONLY);
        if (NULL == fd)
            return -ENOENT;
    }
    fi->fh = (uint64_t)(fd);
    return 0;
}

int my_read(const char *path, char *out, size_t size, off_t off,
            struct fuse_file_info *fi)
{
    zip_file_t *fd = (zip_file_t *)(fi->fh);
    if (off)
        zip_fseek(fd, off, SEEK_SET);
    return zip_fread(fd, out, size);
}

int my_close(const char *path, struct fuse_file_info *fi)
{
    return zip_fclose((zip_file_t *)(fi->fh));
}

struct fuse_operations operations = {
    .readdir = my_readdir,
    .getattr = my_stat,
    .open = my_open,
    .read = my_read,
    .release = my_close,
};

void open_filesystem(char* src)
{
    char abs_src[PATH_MAX];
    realpath(src, abs_src);
    int error_code;
    archieve = zip_open(abs_src, ZIP_RDONLY, &error_code);
    if (error_code != ZIP_ER_OK)
    {
        zip_error_t error;
        zip_error_init_with_code(&error, error_code);
        fprintf(stderr, "%s\n", zip_error_strerror(&error));
        zip_error_fini(&error);
        exit(1);
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

    zip_close(archieve);

    return ret;
}