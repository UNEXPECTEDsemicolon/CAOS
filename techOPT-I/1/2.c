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
#include <dirent.h>
#include <strings.h>

#define FUSE_USE_VERSION 30 // API version 3.0
#include <fuse.h>

#define max(x, y) ((x) >= (y) ? (x) : (y))
#define min(x, y) ((x) <= (y) ? (x) : (y))

typedef struct vector
{
    uint8_t *data;
    size_t size;
    size_t cap;
} vector;

#define vector_init(this, n, type)                           \
    (this).data = realloc(NULL, (max(n, 1)) * sizeof(type)); \
    (this).size = 0;                                         \
    (this).cap = (max(n, 1))

#define vector_push_back(this, elem, type)                                           \
    if ((this).size == (this).cap)                                                   \
    {                                                                                \
        (this).cap *= 2;                                                             \
        if (NULL == ((this).data = realloc((this).data, (this).cap * sizeof(type)))) \
        {                                                                            \
            perror("Can't realloc array of filenames");                              \
            /*safe_exit(1);*/                                                        \
        }                                                                            \
    }                                                                                \
    memcpy(((type *)((this).data)) + (this).size++, &(elem), sizeof(type))

#define vector_at(this, i, type) ((type *)((this).data))[i]

#define vector_back(this, type) vector_at((this), (this).size - 1, type)

#define vector_delete(this, type) free((type *)((this).data))

typedef struct Node
{
    char path[PATH_MAX];
    char real_path[PATH_MAX];
    struct Node *parent;
    vector childs;
    struct stat file_stat;
} Node;

Node root = {.path = ""};

Node *find(Node* node, const char *path)
{
    const char *filename = max(path, rindex(path, '/') + 1);
    for (size_t i = 0; i < node->childs.size; ++i)
    {
        Node *cur_node = &vector_at(node->childs, i, Node);
        char *cur_node_name = max(node->path, rindex(path, '/') + 1);
        if (strncmp(filename, cur_node_name, NAME_MAX) == 0)
            return cur_node;
    }
    return NULL;
}

void path_join(const char *parent, const char *filename, char* result)
{
    sprintf(result, "%s/%s", parent, filename);
}

int merge(Node *parent, const char *real_path, struct stat *file_stat, bool expand_dirs)
{
    if (!S_ISDIR(file_stat->st_mode) && !S_ISREG(file_stat->st_mode))
    {
        perror("Error file type");
        return -1;
    }

    Node *old = find(parent, real_path);
    if (old && !S_ISDIR(file_stat->st_mode) && (file_stat->st_mtime > old->file_stat.st_mtime))
    {
        old->file_stat = *file_stat;
    }
    else
    {
        if (!old)
        {
            if (S_ISDIR(file_stat->st_mode) && !expand_dirs)
                old = parent;
            else
            {
                char local_path[PATH_MAX] = {};
                char temp[PATH_MAX] = {};
                strncpy(temp, real_path, PATH_MAX);
                path_join(parent->path, basename(temp), local_path);
                Node new_node = {.parent = parent, .file_stat = *file_stat};
                strncpy(new_node.path, local_path, PATH_MAX);
                strncpy(new_node.real_path, real_path, PATH_MAX);
                vector_init(new_node.childs, 0, Node);
                vector_push_back(parent->childs, new_node, Node);
                old = &vector_back(parent->childs, Node);
            }
        }
        if (S_ISDIR(file_stat->st_mode))
        {
            DIR *dir = opendir(real_path); //TODO: error handling
            errno = 0;
            struct dirent *dir_info;
            while ((dir_info = readdir(dir)))
            {
                if (!strcmp(dir_info->d_name, ".") || !strcmp(dir_info->d_name, ".."))
                    continue;
                char child_path[PATH_MAX] = {};
                path_join(real_path, dir_info->d_name, child_path);
                struct stat st;
                stat(child_path, &st);
                merge(old, child_path, &st, true);
                // printf("%s: %s\n", path, dir_info->d_name);
            }
            if (errno)
            {
                perror("Can't read directory");
                return -1;
            }
            closedir(dir);
        }
    }
    return 0;
}

// #define safe_exit(return_code)      \
//     vector_delete(files, FileInfo); \
//     fclose(file);                   \
//     exit(return_code)



int my_stat(const char *path/*check if absolute*/, struct stat *st, struct fuse_file_info *fi)
{
    Node *node = find(&root, path, false);
    if (NULL == node)
    {
        return -ENOENT;
    }
    *st = node->file_stat;
    st->st_mode = (st->st_mode & S_IFMT) | (S_ISDIR(st->st_mode) ? 0555 : 0444);
    return 0;
}

int my_readdir(const char *path, void *out, fuse_fill_dir_t filler, off_t off,
               struct fuse_file_info *fi, enum fuse_readdir_flags flags)
{
    Node *node = find(&root, path, false);
    if (NULL == node || !S_ISDIR(node->file_stat.st_mode))
    {
        return -ENOENT;
    }

    filler(out, ".", NULL, 0, 0);
    filler(out, "..", NULL, 0, 0);

    for (size_t i = 0; i < node->childs.size; ++i)
    {
        filler(out, basename(vector_at(node->childs, i, Node).path), NULL, 0, 0);
    }

    return 0;
}

typedef struct OpenedFile
{
    int fd;
    struct stat *file_stat;
} OpenedFile;

vector opened_files;

int my_open(const char *path, struct fuse_file_info *fi)
{
    Node *node = find(&root, path, false);
    if (NULL == node || S_ISDIR(node->file_stat.st_mode))
    {
        return -ENOENT;
    }

    if (O_RDONLY != (fi->flags & O_ACCMODE) || (node->file_stat.st_mode & S_IRUSR))
    {
        return -EACCES;
    }
    OpenedFile new_file;
    new_file.fd = open(node->real_path, O_RDONLY);
    new_file.file_stat = &(node->file_stat);
    vector_push_back(opened_files, new_file, OpenedFile);
    fi->fh = (uint64_t)(&vector_back(opened_files, OpenedFile));
    return 0;
}

int my_read(const char *path, char *out, size_t size, off_t off,
            struct fuse_file_info *fi)
{
    OpenedFile* cur_file = (OpenedFile*)(fi->fh);
    if (off > cur_file->file_stat->st_size)
        return 0;

    if (off + size > cur_file->file_stat->st_size)
        size = cur_file->file_stat->st_size - off;

    if (-1 == lseek(cur_file->fd, off, SEEK_SET))
    {
        perror("lseek error");
        return -errno;
    }
    if (-1 == read(cur_file->fd, out, size))
    {
        perror("Can't read file");
        return -errno;
    }

    return size;
}

int my_close(const char *path, struct fuse_file_info *fi)
{
    return close(((OpenedFile *)(fi->fh))->fd);
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
    char src_backup[PATH_MAX * 10];
    strcpy(src_backup, src);
    root.parent = &root;
    vector_init(root.childs, 0, Node);
    root.file_stat.st_mode = 0555 | S_IFDIR;
    root.file_stat.st_nlink = 2;

    vector_init(opened_files, 0, OpenedFile);

    char *begin = src_backup;
    char *end = begin + strlen(begin);
    char *sep = strstr(begin, ":");
    if (!sep)
        sep = end;
    while (begin != end)
    {
        *sep = '\0';
        struct stat st;
        if (-1 == stat(begin, &st))
        {
            //error
        }
        merge(&root, begin, &st, false);
        if (sep == end)
            break;
        begin = sep + 1;
        sep = strstr(begin, ":");
        if (!sep)
            sep = end;
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

    vector_delete(root.childs, Node);
    vector_delete(opened_files, OpenedFile);

    // safe_exit(0);

    return ret;
}