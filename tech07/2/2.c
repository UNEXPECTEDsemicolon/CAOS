#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

int main()
{
    char *input1;
    size_t size1 = 0;
    getline(&input1, &size1, stdin);
    if (strncmp(input1, "GET", 3) != 0)
    {
        printf("HTTP/1.1 403 ERROR\n\n");
        free(input1);
        exit(1);
    }
    char *path = input1 + 4;
    size_t path_len = strchr(path, '?') - path;
    path[path_len] = '\0';
    struct stat st_buf;
    if (-1 == stat(path, &st_buf))
    {
        printf("HTTP/1.1 403 ERROR\n\n");
        free(input1);
        exit(1);
    }
    if (!(st_buf.st_mode & S_IXUSR))
    {
        printf("HTTP/1.1 403 ERROR\n\n");
        free(input1);
        exit(1);
    }
    char *envs = malloc(size1);
    // for (char *ptr = path + path_len + 1; sscanf(ptr, "%s=%s"))



    int fd = open("temp.c", O_WRONLY | O_CREAT, 0666);
    // dprintf(fd, "#include <stdio.h>\nint main() { printf(\"%%d\\n\", (%s)); }", input);
    close(fd);
    // free(input);
    execlp("bash", "bash", "-c", "gcc -o temp temp.c && ./temp; rm -f temp temp.c", NULL);
    perror("exec");
    exit(1);
}