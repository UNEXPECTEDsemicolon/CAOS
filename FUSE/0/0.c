#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

#define FUSE_USE_VERSION 30 // API version 3.0
#include <fuse.h>

// contents to be accessed by reading files
static const char DummyData[] = "Hello, World!\n";

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

struct fuse_operations operations = {
    .readdir = my_readdir, // callback function pointer for 'readdir'
    .getattr = my_stat,    // callback function pointer for 'stat'
};

int main(int argc, char *argv[])
{
    // arguments to be preprocessed before passing to /sbin/mount.fuse3
    struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

    // run daemon
    int ret = fuse_main(
        args.argc, args.argv, // arguments to be passed to /sbin/mount.fuse3
        &operations,          // pointer to callback functions
        NULL                  // optional pointer to user-defined data
    );
    return ret;
}