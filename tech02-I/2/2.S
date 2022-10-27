.global solve
.text

solve:
    // x0 = A, x1 = B, x2 = C, x3 = D
    mov x4, 0 // cur x
    mov x6, 255 // ans
    sxtw x1, w1
    sxtw x2, w2
    sxtw x3, w3
    for_begin:
        cmp x4, 254
        bgt for_end
        mov x5, x0 // x5 = A*x^3 + B*x^2 + C*x + D
        madd x5, x5, x4, x1
        madd x5, x5, x4, x2
        madd x5, x5, x4, x3
        cmp x5, 0
        bne if_end
        if_begin:
            cmp x4, x6
            bge if_end
            mov x6, x4
        if_end:
        add x4, x4, 1
        b for_begin
    for_end:
    mov x0, x6
    ret
