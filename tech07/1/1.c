#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

int main()
{
    char *input;
    size_t size = 0;
    getline(&input, &size, stdin);
    int fd = open("temp.c", O_WRONLY | O_CREAT, 0666);
    dprintf(fd, "#include <stdio.h>\nint main() { printf(\"%%d\\n\", (%s)); }", input);
    close(fd);
    free(input);
    execlp("bash", "bash", "-c", "gcc -o temp temp.c && ./temp; rm -f temp temp.c", NULL);
    perror("exec");
    exit(1);
}