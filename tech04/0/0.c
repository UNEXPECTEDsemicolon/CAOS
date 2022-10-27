#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <linux/limits.h>
#include <sys/stat.h>
#include <stdlib.h>

int main()
{
    char filename_buff[PATH_MAX] = {};
    char *filename = filename_buff;
    uint64_t ans = 0;
    ssize_t filename_len = 0;
    while (-1 != (filename_len = getline(&filename, &filename_len, stdin)))
    {
        if (filename[filename_len - 1] == '\n')
            filename[filename_len - 1] = '\0';
        struct stat st;
        if (-1 == lstat(filename, &st))
        {
            perror("Can't call lstat");
            exit(1);
        }
        if (S_ISREG(st.st_mode))
            ans += st.st_size;
    }
    printf("%zu\n", ans);
}