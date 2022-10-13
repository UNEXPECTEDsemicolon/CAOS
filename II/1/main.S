.global main

.text
main:
    stp x29, x30, [sp, -16]!
    mov x29, sp
    sub sp, sp, 16

    adr x0, IN_FORMAT_STR
    sub x1, x29, 4
    sub x2, x29, 8
    bl scanf

    adr x0, OUT_FORMAT_STR
    ldr w1, [x29, -4]
    ldr w2, [x29, -8]
    add w1, w1, w2
    bl printf

    mov sp, x29
    ldp x29, x30, [sp], 16
    ret

.data
IN_FORMAT_STR: .string "%d %d"
OUT_FORMAT_STR: .string "%d\n"
