#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>

int main()
{
    char *input;
    size_t size = 0;
    getline(&input, &size, stdin);
    if (strncmp(input, "GET", 3) != 0)
    {
        printf("HTTP/1.1 405 ERROR\n\n");
        free(input);
        exit(1);
    }
    const char REQUEST_METHOD[] = "GET";
    char *SCRIPT_NAME = input + 5;
    char *path_end = strchr(SCRIPT_NAME, '?');
    size_t path_len = (path_end != NULL ? path_end : strchr(SCRIPT_NAME, ' ')) - SCRIPT_NAME;
    SCRIPT_NAME[path_len] = '\0';
    struct stat st_buf;
    if (-1 == stat(SCRIPT_NAME, &st_buf))
    {
        printf("HTTP/1.1 404 ERROR\n\n");
        free(input);
        exit(1);
    }
    if (!(st_buf.st_mode & S_IXUSR))
    {
        printf("HTTP/1.1 403 ERROR\n\n");
        free(input);
        exit(1);
    }
    char *QUERY_STRING = SCRIPT_NAME + path_len + (bool)(path_end);
    size_t query_len = (*QUERY_STRING ? strchr(QUERY_STRING, ' ') - QUERY_STRING : 0);
    QUERY_STRING[query_len] = '\0';
    char *HTTP_HOST;
    size_t host_len = 0;
    getline(&HTTP_HOST, &host_len, stdin);
    HTTP_HOST += sizeof("Host: ") - 1;
    char* host_end = strchr(HTTP_HOST, '\n');
    if (host_end)
        *host_end = '\0';
    setenv("HTTP_HOST", HTTP_HOST, true);
    setenv("QUERY_STRING", QUERY_STRING, true);
    setenv("REQUEST_METHOD", REQUEST_METHOD, true);
    setenv("SCRIPT_NAME", SCRIPT_NAME, true);
    printf("HTTP/1.1 200 OK\n");
    fflush(stdout);
    execl(SCRIPT_NAME, SCRIPT_NAME, NULL);
    perror("exec");
    exit(1);
}