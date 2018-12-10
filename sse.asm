%define pixels r9
%define count r10
%define dest r11

align 16
c1_1: dd 0.272, 0.349, 0.393, 0.272
align 16
c2_1: dd 0.534, 0.686, 0.769, 0.534
align 16
c3_1: dd 0.131, 0.168, 0.189, 0.131

align 16
c1_2: dd 0.349, 0.393, 0.272, 0.349
align 16
c2_2: dd 0.686, 0.769, 0.534, 0.686
align 16
c3_2: dd 0.168, 0.189, 0.131, 0.168

align 16
c1_3: dd 0.393, 0.272, 0.349, 0.393
align 16
c2_3: dd 0.769, 0.534, 0.686, 0.769
align 16
c3_3: dd 0.189, 0.131, 0.168, 0.189

align 16
max_values: dd 255, 255, 255, 255

global sepia_asm_inplace


sepia_asm_inplace:

    push r12
    push r13
    push r14

    mov pixels, rdi
    mov count, rsi

    and count, 0xfffffffc
    mov dest, rdx

    movaps xmm6, [max_values]

    test count, count
    jz .end
.loop:

;1
    pxor xmm0, xmm0
    pinsrb xmm0, byte[pixels], 0
    shufps xmm0, xmm0, 0b00000000
    pinsrb xmm0, byte[pixels + 3], 0
    cvtdq2ps xmm0, xmm0

    pxor xmm1, xmm1
    pinsrb xmm1, byte[pixels+ 1], 0
    shufps xmm1, xmm1, 0b00000000
    pinsrb xmm1, byte[pixels + 4], 0
    cvtdq2ps xmm1, xmm1

    pxor xmm2, xmm2
    pinsrb xmm2, byte[pixels + 2], 0
    shufps xmm2, xmm2, 0b00000000
    pinsrb xmm2, byte[pixels + 5], 0
    cvtdq2ps xmm2, xmm2


    movaps xmm3, [c1_1]
    movaps xmm4, [c2_1]
    movaps xmm5, [c3_1]

    mulps xmm0, xmm3
    mulps xmm1, xmm4
    mulps xmm2, xmm5

    addps xmm0, xmm1
    addps xmm0, xmm2

    cvtps2dq xmm0, xmm0

    pminsd xmm0, xmm6

    pextrb [dest], xmm0, 0
    pextrb [dest + 1], xmm0, 4
    pextrb [dest + 2], xmm0, 8
    pextrb [dest + 3], xmm0, 12

    add pixels, 3

    add dest, 4
;2
    pxor xmm0, xmm0
    pinsrb xmm0, [pixels], 0
    pinsrb xmm0, [pixels + 3], 8
    shufps xmm0, xmm0, 0b10100000
    cvtdq2ps xmm0, xmm0

    pxor xmm1, xmm1
    pinsrb xmm1, [pixels + 1], 0
    pinsrb xmm1, [pixels + 4], 8
    shufps xmm1, xmm1, 0b10100000
    cvtdq2ps xmm1, xmm1

    pxor xmm2, xmm2
    pinsrb xmm2, [pixels + 2], 0
    pinsrb xmm2, [pixels + 5], 8
    shufps xmm2, xmm2, 0b10100000
    cvtdq2ps xmm2, xmm2



    movaps xmm3, [c1_2]
    movaps xmm4, [c2_2]
    movaps xmm5, [c3_2]

    mulps xmm0, xmm3
    mulps xmm1, xmm4
    mulps xmm2, xmm5

    addps xmm0, xmm1
    addps xmm0, xmm2

    cvtps2dq xmm0, xmm0

    pminsd xmm0, xmm6

    pextrb [dest], xmm0, 0
    pextrb [dest + 1], xmm0, 4
    pextrb [dest + 2], xmm0, 8
    pextrb [dest + 3], xmm0, 12

    add pixels, 3
    add dest, 4

;3
    pxor xmm0, xmm0
    pinsrb xmm0, byte[pixels], 0
    pinsrb xmm0, byte[pixels + 3], 4
    shufps xmm0, xmm0, 0b01010100
    cvtdq2ps xmm0, xmm0

    pxor xmm1, xmm1
    pinsrb xmm1, byte[pixels + 1], 0
    pinsrb xmm1, byte[pixels + 4], 4
    shufps xmm1, xmm1, 0b01010100
    cvtdq2ps xmm1, xmm1

    pxor xmm2, xmm2
    pinsrb xmm2, byte[pixels + 2], 0
    pinsrb xmm2, byte[pixels + 5], 4
    shufps xmm2, xmm2, 0b01010100
    cvtdq2ps xmm2, xmm2


    movaps xmm3, [c1_3]
    movaps xmm4, [c2_3]
    movaps xmm5, [c3_3]

    mulps xmm0, xmm3
    mulps xmm1, xmm4
    mulps xmm2, xmm5

    addps xmm0, xmm1
    addps xmm0, xmm2

    cvtps2dq xmm0, xmm0

    pminsd xmm0, xmm6
    pextrb [dest], xmm0, 0
    pextrb [dest + 1], xmm0, 4
    pextrb [dest + 2], xmm0, 8
    pextrb [dest + 3], xmm0, 12


    add pixels, 6
    add dest, 4


    sub count, 4
    jnz .loop
.end:
    pop r14
    pop r13
    pop r12
    ret