#include <sys/syscall.h>

extern long syscall(long number, ...);

int _start()
{
    typedef unsigned long size_t;
    const size_t data_size = 4096 * 1000;
    const size_t strs_ptrs_size = 4096 * 1000;
    const size_t strs_lens_size = 4096 * 1000;
    char *heap_begin = (char *)syscall(SYS_brk, 0);
    char *data = heap_begin;
    syscall(SYS_brk, data + data_size + strs_ptrs_size + strs_lens_size);
    size_t input_len;
    input_len = syscall(SYS_read, 0, data, data_size);
    if (data[input_len - 1] != '\n')
    {
        data[input_len++] = '\n';
    }
    char **strs_ptrs = (char **)(data + data_size);
    char *strs_lens = data + data_size + strs_ptrs_size;
    size_t strs_num = 0;
    char *last_str = data;
    for (size_t i = 0; i <= input_len; ++i)
    {
        if (i == input_len && last_str != data + i
            || data[i] == '\n')
        {
            strs_ptrs[strs_num] = last_str;
            strs_lens[strs_num++] = data + i - last_str + 1;
            last_str = data + i + 1;
        }
    }
    for (size_t i = 0; i < strs_num; ++i)
    {
        syscall(SYS_write, 1, strs_ptrs[strs_num - i - 1], strs_lens[strs_num - i - 1]);
    }
    syscall(SYS_brk, heap_begin); // free
    syscall(SYS_exit, 0);
}