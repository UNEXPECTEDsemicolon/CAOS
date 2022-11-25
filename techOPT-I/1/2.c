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
            perror("Can't realloc array");                                           \
            exit(1);                                                                 \
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
    if (!strcmp(path, "."))
        return node;
    if (!strcmp(path, ".."))
        return node->parent;
    if (node == &root && !strcmp(path, "/"))
        return node;
    for (size_t i = 0; i < node->childs.size; ++i)
    {
        Node *cur_node = &vector_at(node->childs, i, Node);
        if (!strcmp(path, cur_node->path))
            return cur_node;
        if (!strncmp(path, cur_node->path, strlen(cur_node->path)))
        {
            Node* res = find(cur_node, path);
            if (res)
                return res;
        }
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

    char local_path[PATH_MAX] = {};
    path_join(parent->path, basename(real_path), local_path);
    Node *old = find(parent, local_path);
    if (old && !S_ISDIR(file_stat->st_mode) && (file_stat->st_mtime > old->file_stat.st_mtime))
    {
        old->file_stat = *file_stat;
        strcpy(old->real_path, real_path);
    }
    else
    {
        if (!old)
        {
            if (S_ISDIR(file_stat->st_mode) && !expand_dirs)
                old = parent;
            else
            {
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
            DIR *dir = opendir(real_path);
            if (!dir)
            {
                perror("Can't open directory");
                return -1;
            }
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

int my_stat(const char *path, struct stat *st, struct fuse_file_info *fi)
{
    Node *node = find(&root, path);
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
    Node *node = find(&root, path);
    if (NULL == node || !S_ISDIR(node->file_stat.st_mode))
    {
        return -ENOENT;
    }

    filler(out, ".", NULL, 0, 0);
    filler(out, "..", NULL, 0, 0);

    for (size_t i = 0; i < node->childs.size; ++i)
    {
        Node* child = &vector_at(node->childs, i, Node);
        filler(out, basename(child->path), &(child->file_stat), 0, 0);
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
    Node *node = find(&root, path);
    if (NULL == node || S_ISDIR(node->file_stat.st_mode))
    {
        return -ENOENT;
    }

    if (O_RDONLY != (fi->flags & O_ACCMODE) || !(node->file_stat.st_mode & S_IRUSR))
    {
        return -EACCES;
    }
    OpenedFile new_file;
    if (-1 == (new_file.fd = open(node->real_path, O_RDONLY)))
    {
        int res = -errno;
        perror("Can't open");
        return res;
    }
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
        int res = -errno;
        perror("lseek error");
        return res;
    }
    if (-1 == read(cur_file->fd, out, size))
    {
        int res = -errno;
        perror("Can't read file");
        return res;
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

void delete_nodes(Node *node)
{
    for (size_t i = 0; i < node->childs.size; ++i)
    {
        delete_nodes(&vector_at(node->childs, i, Node));
    }
    vector_delete(node->childs, Node);
}

void open_filesystem(char* src)
{
    root.parent = &root;
    vector_init(root.childs, 0, Node);
    root.file_stat.st_mode = 0555 | S_IFDIR;
    root.file_stat.st_nlink = 2;

    vector_init(opened_files, 0, OpenedFile);

    char *begin = src;
    char *end = begin + strlen(begin);
    char *sep = strchr(begin, ':');
    if (!sep)
        sep = end;
    while (begin != end)
    {
        *sep = '\0';
        struct stat st;
        char abs_begin[PATH_MAX];
        realpath(begin, abs_begin);
        if (-1 == stat(abs_begin, &st))
        {
            perror("stat");
            delete_nodes(&root);
            exit(1);
        }
        if (-1 == merge(&root, abs_begin, &st, false))
        {
            delete_nodes(&root);
            exit(1);
        }
        if (sep == end)
            break;
        begin = sep + 1;
        sep = strchr(begin, ':');
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

    delete_nodes(&root);
    vector_delete(opened_files, OpenedFile);

    return ret;
}