#include <sys/syscall.h>

extern long syscall(long number, ...);

int _start()
{
    typedef unsigned long size_t;
    char *heap_begin = (char *)syscall(SYS_brk, 0);
    char *data = heap_begin;
    const size_t buff_size = 4096;
    size_t data_cap = buff_size;
    syscall(SYS_brk, data + data_cap);
    size_t data_len = 0, new_data_len = 0;
    while (new_data_len = syscall(SYS_read, 0, data + data_len, buff_size))
    {
        data_len += new_data_len;
        if (data_cap - data_len < buff_size)
            syscall(SYS_brk, data + (data_cap *= 2));
    }
    if (data_len && data[data_len - 1] != '\n')
    {
        data[data_len++] = '\n';
    }
    size_t strs_ptrs_cap = buff_size;
    char **strs_ptrs = (char **)(data + data_cap);
    syscall(SYS_brk, strs_ptrs + strs_ptrs_cap);
    size_t strs_ptrs_size = 0;
    char *last_str = data;
    for (size_t i = 0; i <= data_len; ++i)
    {
        if (data[i] == '\n')
        {
            strs_ptrs[strs_ptrs_size++] = last_str;
            last_str = data + i + 1;
        }
        if (strs_ptrs_size == strs_ptrs_cap)
        {
            syscall(SYS_brk, strs_ptrs + (strs_ptrs_cap *= 2));
        }
    }
    strs_ptrs[strs_ptrs_size] = last_str;
    for (size_t i = 0; i < strs_ptrs_size; ++i)
    {
        syscall(SYS_write, 1, strs_ptrs[strs_ptrs_size - i - 1], strs_ptrs[strs_ptrs_size - i] - strs_ptrs[strs_ptrs_size - i - 1]);
    }
    syscall(SYS_brk, heap_begin);
    syscall(SYS_exit, 0);
}