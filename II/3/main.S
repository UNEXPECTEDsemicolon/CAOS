.global main

.text
main:
    stp x29, x30, [sp, -16]!
    mov x29, sp
    sub sp, sp, 32

    mov x0, 0
    mov x1, 4096
    str x0, [sp, 8] // save N
    str x1, [sp, 16] // save arr_size
    bl realloc
    str x0, [sp] // save arr

    while_begin:
        bl getchar_unlocked // read arr[N]
        sxtb x0, w0
        cmp x0, -1
        beq while_end
        ldr x1, [sp] // get arr
        ldr x2, [sp, 8] // get N
        str x0, [x1, x2] // arr[N] = x0
        add x2, x2, 1 // ++N
        str x2, [sp, 8] // save N
        ldr x3, [sp, 16] // get arr_size
        cmp x2, x3 // if N == arr_size
        bne if_end
            lsl x3, x3, 1 // arr_size *= 2
            str x3, [sp, 16] // save new arr_size
            mov x0, x1
            mov x1, x3
            bl realloc // realloc arr with new arr_size
            str x0, [sp] // save arr
        if_end:
        b while_begin
    while_end:

    for_begin:
        ldr x2, [sp, 8] // get N
        cmp x2, 0
        beq for_end
        sub x2, x2, 1 // --N
        str x2, [sp, 8] // save N
        ldr x1, [sp] // get arr
        ldr x0, [x1, x2] // get arr[N]
        bl putchar_unlocked // print arr[N]
        b for_begin
    for_end:

    ldr x0, [sp]
    bl free // free arr
    
    mov sp, x29
    ldp x29, x30, [sp], 16
    ret
