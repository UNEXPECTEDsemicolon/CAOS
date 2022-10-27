#include <sys/syscall.h>
#define buff_size 4096

.intel_syntax noprefix

.data
    heap_begin: .quad 0
    data: .quad 0
    data_cap: .quad buff_size
    data_len: .quad 0
    new_data_len: .quad 0
    strs_ptrs_cap: .quad buff_size
    strs_ptrs: .quad 0
    strs_ptrs_size: .quad 0
    last_str: .quad 0
    i: .quad 0
    return_code: .quad 0

.global _start
.text

_start:
    mov rax, SYS_brk
    mov rdi, 0
    syscall
    cmp rax, -1
    je err_exit

    mov heap_begin, rax

    mov data, rax
    mov rsi, data_cap
    lea rdi, [rax + rsi]
    mov rax, SYS_brk
    syscall
    cmp rax, -1
    je err_exit

    while_begin:
        mov rdi, data
        mov rsi, data_len
        lea rsi, [rdi + rsi]
        mov rax, SYS_read
        mov rdi, 0
        mov rdx, buff_size
        syscall
        cmp rax, -1
        je err_free_and_exit

        cmp rax, 0
        je while_end

        mov new_data_len, rax
        add data_len, rax

        mov rax, data_cap
        sub rax, data_len
        cmp rax, buff_size
        jge if1_end

            mov rax, data_cap
            shl rax, 1
            mov data_cap, rax

            mov rdi, data
            add rdi, data_cap
            mov rax, SYS_brk
            syscall
            cmp rax, -1
            je err_free_and_exit
        if1_end:
        jmp while_begin
    while_end:

    mov rdi, data
    mov rsi, data_len
    cmpq data_len, 0
    je if2_end
    cmpb [rdi + rsi - 1], '\n'
    je if2_end
        movb [rdi + rsi], '\n'
        incq data_len
    if2_end:

    mov rdi, data
    add rdi, data_cap
    mov strs_ptrs, rdi

    mov rax, strs_ptrs_cap
    shl rax, 3
    add rdi, rax
    mov rax, SYS_brk
    syscall
    cmp rax, -1
    je err_free_and_exit

    mov rax, data
    mov last_str, rax

    for1_begin:
        mov rdi, i
        cmp rdi, data_len
        jg for1_end

        mov rsi, data
        mov rdi, i
        mov rax, data_len
        cmpq rdi, rax
        je if3_end
        cmpb [rsi + rdi], '\n'
        jne if3_end
            mov rax, last_str
            mov rdi, strs_ptrs_size
            mov rsi, strs_ptrs
            mov [rsi + rdi * 8], rax
            incq strs_ptrs_size

            mov rax, data
            add rax, i
            inc rax
            mov last_str, rax
        if3_end:

        mov rax, strs_ptrs_size
        cmp rax, strs_ptrs_cap
        jne if4_end
            mov rax, strs_ptrs_cap
            shl rax, 1
            mov strs_ptrs_cap, rax
            shl rax, 3
            
            mov rdi, strs_ptrs
            add rdi, rax
            mov rax, SYS_brk
            syscall
            cmp rax, -1
            je err_free_and_exit
        if4_end:

        incq i
        jmp for1_begin
    for1_end:

    mov rax, last_str
    mov rdi, strs_ptrs_size
    mov rsi, strs_ptrs
    mov [rsi + rdi * 8], rax

    movq i, 0
    for2_begin:
        mov rdi, i
        cmpq rdi, strs_ptrs_size
        jge for2_end

        mov rax, strs_ptrs
        mov rdi, strs_ptrs_size
        sub rdi, i
        mov rsi, [rax + (rdi - 1) * 8]
        mov rdx, [rax + rdi * 8]
        sub rdx, rsi

        mov rax, SYS_write
        mov rdi, 1
        syscall
        cmp rax, -1
        je err_free_and_exit

        incq i
        jmp for2_begin
    for2_end:

    jmp free_and_exit

    err_free_and_exit:
        movq return_code, 1

    free_and_exit:
        mov rax, SYS_brk
        mov rdi, heap_begin
        syscall
        jmp exit

    err_exit:
        movq return_code, 1

    exit:
        mov rax, SYS_exit
        mov rdi, return_code
        syscall
