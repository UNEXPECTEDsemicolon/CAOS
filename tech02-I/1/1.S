.global sum
.text

sum:
    // x0=x0, x1=N, x2=X
    for_begin:
        cmp x1, 0
        beq for_end
        sub x1, x1, 1 
        ldrw w3, [x2, x1, lsl 2] // x3 = X[i]
        add w0, w0, w3
        b for_begin
    for_end:

    ret
