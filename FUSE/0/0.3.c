#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <stddef.h>
#include <stdbool.h>
#include <unistd.h>
#include <bits/posix1_lim.h>

#include "chained-hash-table-c/chained_hash.h"

#define FUSE_USE_VERSION 30 // API version 3.0
#include <fuse.h>

// contents of file
static const char DummyData[] = "Hello, World!\n";
static 

// callback function to be called after 'stat' system call
int my_stat(const char *path, struct stat *st, struct fuse_file_info *fi)
{
    // check if accessing root directory
    if (0 == strcmp("/", path))
    {
        st->st_mode = 0555 | S_IFDIR; // file type - dir, access read only
        st->st_nlink = 2;             // at least 2 links: '.' and parent
        return 0;                     // success!
    }
    if (0 != strcmp("/a.txt", path) && 0 != strcmp("/b.txt", path))
    {
        return -ENOENT; // error: we have no files other than a.txt and b.txt
    }

    st->st_mode = S_IFREG | 0444;    // file type - regular, access read only
    st->st_nlink = 1;                // one link to file
    st->st_size = sizeof(DummyData); // bytes available
    return 0;                        // success!
}

// callback function to be called after 'readdir' system call
int my_readdir(const char *path, void *out, fuse_fill_dir_t filler, off_t off,
               struct fuse_file_info *fi, enum fuse_readdir_flags flags)
{
    if (0 != strcmp(path, "/"))
    {
        return -ENOENT; // we do not have subdirectories
    }

    // two mandatory entries: the directory itself and its parent
    filler(out, ".", NULL, 0, 0);
    filler(out, "..", NULL, 0, 0);

    // directory contents
    filler(out, "a.txt", NULL, 0, 0);
    filler(out, "b.txt", NULL, 0, 0);

    return 0; // success
}

// callback function to be called after 'open' system call
int my_open(const char *path, struct fuse_file_info *fi)
{
    if (0 != strcmp("/a.txt", path) && 0 != strcmp("/b.txt", path))
    {
        return -ENOENT; // we have only two files in out filesystem
    }
    if (O_RDONLY != (fi->flags & O_ACCMODE))
    {
        return -EACCES; // file system is read-only, so can't write
    }
    return 0; // success!
}

// callback function to be called after 'read' system call
int my_read(const char *path, char *out, size_t size, off_t off,
            struct fuse_file_info *fi)
{
    // 'read' might be called with arbitary arguments, so check them
    if (off > sizeof(DummyData))
        return 0;
    // reading might be called within some non-zero offset
    if (off + size > sizeof(DummyData))
        size = sizeof(DummyData) - off;
    const void *data = DummyData + off;
    // copy contents into the buffer to be filled by 'read' system call
    memcpy(out, data, size);
    // return value is bytes count (0 or positive) or an error (negative)
    return size;
}

// register functions as callbacks
struct fuse_operations operations = {
    .readdir = my_readdir,
    .getattr = my_stat,
    .open = my_open,
    .read = my_read,
};

int open_filesystem(const char* path)
{
    if (strnlen(path, PATH_MAX+1) == PATH_MAX+1)
    {
        //log
        return -1;
    }
    char real_path[PATH_MAX];
    if (realpath(path, real_path) == NULL)
    {
        //log 
        return -1;
    }
    int f = open(real_path, O_RDONLY);
    if (f == -1)
    {
        //log
        return -1;
    }
    size_t file_size = lseek(f, 0, SEEK_END);
    lseek(f, 0, SEEK_SET);
    if ((ssize_t)file_size < 0)
    {
        //log
        return -1;
    }
}

int main(int argc, char *argv[])
{
    // initialize modificable array {argc, argv}
    struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

    // struct to be filled by options parsing
    typedef struct
    {
        char *src;
    } my_options_t;

    my_options_t my_options;
    memset(&my_options, 0, sizeof(my_options));

    // options specifications
    struct fuse_opt opt_specs[] = {
        // pattern: match --src then string
        // the string value to be written to my_options_t.src
        {"--src %s", offsetof(my_options_t, src), 0},
        // end-of-array: all zeroes value
        {NULL, 0, 0}};

    // parse command line arguments, store matched by 'opt_specs'
    // options to 'my_options' value and remove them from {argc, argv}
    fuse_opt_parse(&args, &my_options, opt_specs, NULL);

    if (my_options.src)
    {
        open_filesystem(my_options.src);
    }

    // pass rest options but excluding --src and --help to mount.fuse3
    int ret = fuse_main(args.argc, args.argv, &operations, NULL);

    return ret;
}