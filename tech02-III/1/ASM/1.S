#include <sys/syscall.h>
#define buff_size 4096

.intel_syntax noprefix

.global _start
.text

_start:
    while_begin:
        mov rax, SYS_read
        mov rdi, 0
        lea rsi, [buffer]
        mov rdx, buff_size
        syscall // read <buff_size> chars to buffer
        cmpb al, -1
        je err_exit
        cmp rax, 0
        je while_end
        mov rdx, rax
        mov rax, SYS_write
        mov rdi, 1
        syscall // write given chars
        cmpb al, -1
        je err_exit
        jmp while_begin
    while_end:
    xor rdi, rdi
    jmp exit
    err_exit:
    mov rdi, 1
    exit:
        mov rax, SYS_exit
        syscall // exit

.data
    buffer: .zero buff_size*8
