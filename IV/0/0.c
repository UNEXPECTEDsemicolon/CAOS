#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>

#define safe_exit(return_code) \
    if (return_code) perror("Unexpected error"); \
    close(f_in); \
    close(f_out_dig); \
    close(f_out_etc); \
    exit(return_code) \

int main(int argc, char** argv)
{
    int f_in = open(argv[1], O_RDONLY);
    if (f_in == -1)
    {
        perror("Can't open input file");
        exit(1);
    }
    int f_out_dig = open(argv[2], O_WRONLY | O_CREAT, 0666);
    if (f_out_dig == -1)
    {
        perror("Can't create digit output file");
        close(f_in);
        exit(2);
    }
    int f_out_etc = open(argv[3], O_WRONLY | O_CREAT, 0666);
    if (f_out_etc == -1)
    {
        perror("Can't create etc output file");
        close(f_in);
        close(f_out_dig);
        exit(2);
    }
    const size_t buff_cap = 4096;
    char buffer[buff_cap];
    char buffer_out_dig[buff_cap];
    char buffer_out_etc[buff_cap];
    size_t buff_len;
    size_t buffer_out_dig_len = 0;
    size_t buffer_out_etc_len = 0;
    while (buff_len = read(f_in, buffer, buff_cap))
    {
        if (-1 == buff_len)
        {
            safe_exit(3);
        }
        for (size_t i = 0; i < buff_len; ++i)
        {
            if (isdigit(buffer[i]))
            {
                buffer_out_dig[buffer_out_dig_len++] = buffer[i];
            }
            else
            {
                buffer_out_etc[buffer_out_etc_len++] = buffer[i];
            }
            if (buffer_out_dig_len == buff_cap)
            {
                if (-1 == write(f_out_dig, buffer_out_dig, buff_cap))
                {
                    safe_exit(3);
                }
                buffer_out_dig_len = 0;
            }
            if (buffer_out_etc_len == buff_cap)
            {
                if (-1 == write(f_out_etc, buffer_out_etc, buff_cap))
                {
                    safe_exit(3);
                }
                buffer_out_etc_len = 0;
            }
        }
    }
    if (-1 == write(f_out_dig, buffer_out_dig, buffer_out_dig_len))
    {
        safe_exit(3);
    }
    if (-1 == write(f_out_etc, buffer_out_etc, buffer_out_etc_len))
    {
        safe_exit(3);
    }
    safe_exit(0);
}