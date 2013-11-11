section .data
    palle dd 0.1337

section .text
    global asm_raw_noise_3d

    asm_raw_noise_3d:
    movss xmm0,[palle]
    ret