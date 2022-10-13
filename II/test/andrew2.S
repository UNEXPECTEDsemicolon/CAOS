.global main
.text

main:
  stp x29, x30, [sp, -16]!
  mov x29, sp
  mov x19, 0 // buffer = nullptr
  mov x20, 0 // size = 0
  mov x21, 1 // capacity = 1
  mov x0, 0
  mov x1, x21
  bl realloc
  mov x19, x0

  loop_input:
    bl getchar_unlocked

    // EOF
    cmp x0, -1
    beq loop_output
    
    // store new symbol and increment size
    str x0, [x19, x20]
    add x20, x20, 1

    // size < capacity ?
    cmp x20, x21
    blt loop_input
    
    lsl x21, x21, 1
    mov x0, x19
    mov x1, x21
    bl realloc
    mov x19, x0
    b loop_input
    
    
  loop_output:
    cmp x20, 0
    beq exit
    sub x20, x20, 1
    ldr x0, [x19, x20]
    bl putchar_unlocked
    b loop_output

  exit:
    // free buffer
    mov x0, x19
    bl free
    mov sp, x29
    ldp x29, x30, [sp], 16
    mov x0, 0
    ret