#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <linux/limits.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

int check_exec(char* s)
{
    struct stat st;
    if (-1 == stat(s, &st))
        return -1;
    return S_ISREG(st.st_mode) && (st.st_mode & S_IXUSR);
}

bool check_interpreter(char* s)
{
    if (strncmp(s, "#!", 2) != 0)
        return false;
    char* end = strstr(s, " ");
    if (end != NULL)
        *end = '\0';
    return check_exec(s + 2) > 0;
}

bool check_elf(const char* s)
{
    return strncmp(s, "\177ELF", 4) == 0;
}

int main()
{
    char filepath_buff[PATH_MAX] = {};
    char first_line[PATH_MAX] = {};
    char *filepath = filepath_buff;
    ssize_t filepath_len = 0;
    while (-1 != (filepath_len = getline(&filepath, &filepath_len, stdin)))
    {
        if (filepath[filepath_len - 1] == '\n')
            filepath[filepath_len - 1] = '\0';
        if (check_exec(filepath) <= 0)
            continue;
        int fd = open(filepath, O_RDONLY);
        if (-1 == fd)
        {
            perror("Can't open file");
            close(fd);
            exit(1);
        }
        ssize_t first_line_len;
        if (-1 == (first_line_len = read(fd, first_line, sizeof(first_line) - 1)))
        {
            perror("Can't read file");
            close(fd);
            exit(1);
        }
        char *end = strstr(first_line, "\n");
        if (end == NULL)
            end = first_line + first_line_len;
        *end = '\0';
        if (!check_interpreter(first_line) && !check_elf(first_line))
        {
            printf("%s\n", filepath);
        }
        close(fd);
    }
}