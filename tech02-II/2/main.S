.global main

.text
main:
    stp x29, x30, [sp, -16]!
    
    while_begin:
        bl getchar_unlocked
        sxtb x0, w0
        cmp x0, -1
        beq while_end
        adr x1, DIGITS
        mov x2, 0
        for_begin:
            cmp x2, 10
            beq for_end
            ldr x3, [x1, x2]
            sxtb x3, w3
            cmp x0, x3
            beq print_char
            add x2, x2, 1
            b for_begin
        for_end:
        b while_begin
        print_char:
            bl putchar_unlocked
            b for_end
    while_end:
    
    ldp x29, x30, [sp], 16
    ret

.data
DIGITS: .string "0123456789"
