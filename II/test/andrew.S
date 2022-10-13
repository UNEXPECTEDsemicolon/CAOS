.global main
.text

main:
  stp x29, x30, [sp, -16]! 
  mov x29, sp

  loop:
    bl getchar_unlocked

    cmp x0, -1
    beq exit 
    
    cmp x0, 59 // '9'
    bhi loop
    
    cmp x0, 47 // '0'
    bls loop

    bl putchar_unlocked
    b loop

  exit:
  ldp x29, x30, [sp], 16
  mov x0, 0
  ret