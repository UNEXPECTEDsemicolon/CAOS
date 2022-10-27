#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <windows.h>
#include <winbase.h>

#define safe_exit(return_code)      \
    if (return_code)                \
        perror("Unexpected error"); \
    CloseHandle(fd);                \
    exit(return_code)

typedef struct
{
    int value;
    DWORD next_pointer;
} Item;

int main(int argc, char **argv)
{
    OFSTRUCT buff;
    HANDLE fd = (OpenFile(argv[1], &buff, OF_READ));
    if (fd == INVALID_HANDLE_VALUE)
    {
        perror("Can't open input file");
        exit(1);
    }
    DWORD file_size = GetFileSize(fd, NULL);
    if (file_size == INVALID_FILE_SIZE)
    {
        safe_exit(1);
    }
    if (file_size == 0)
    {
        safe_exit(0);
    }
    Item node;
    do
    {
        if (!ReadFile(fd, &node.value, sizeof(node.value), NULL, NULL))
        {
            safe_exit(1);
        }
        if (!ReadFile(fd, &node.next_pointer, sizeof(node.next_pointer), NULL, NULL))
        {
            safe_exit(1);
        }
        printf("%d\n", node.value);
        LARGE_INTEGER new_pos;
        new_pos.LowPart = node.next_pointer;
        if (!SetFilePointerEx(fd, new_pos, NULL, FILE_BEGIN))
        {
            safe_exit(1);
        }
    } while (node.next_pointer != 0);
}