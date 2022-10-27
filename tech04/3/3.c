#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <linux/limits.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <libgen.h>
#include <string.h>

int main()
{
    char filepath_buff[PATH_MAX] = {};
    char *filepath = filepath_buff;
    ssize_t filepath_len = 0;
    while (-1 != (filepath_len = getline(&filepath, &filepath_len, stdin)))
    {
        if (filepath[filepath_len - 1] == '\n')
            filepath[filepath_len - 1] = '\0';
        struct stat st;
        if (-1 == lstat(filepath, &st))
        {
            perror("Can't call lstat");
            exit(1);
        }
        if (S_ISLNK(st.st_mode))
        {
            char real_path[PATH_MAX] = {};
            if (NULL == realpath(filepath, real_path))
            {
                perror("Can't read symlink");
                exit(1);
            }
            printf("%s\n", real_path);
        }
        if (S_ISREG(st.st_mode))
        {
            char linkpath[PATH_MAX] = {};
            char filename_buff[PATH_MAX] = {};
            char* filename = filename_buff;
            strcpy(filename, filepath);
            filename = basename(filename);
            sprintf(linkpath, "link_to_%s", filename);
            if (-1 == symlink(filepath, linkpath))
            {
                perror("Can't create symlink");
                printf("%s\n", linkpath);
                exit(1);
            }
        }
    }
}