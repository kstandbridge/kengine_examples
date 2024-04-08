global Read_32x8

section .text

; NOTE(kstandbridge): Win64 ABI https://learn.microsoft.com/en-us/cpp/build/x64-software-conventions?view=msvc-170
; RCX the first parameter and RDX is the second parameter

;   rcx: Count (divisible by 256)
;   rdx: Data pointer
;    r8: Mask

Read_32x8:
    xor r9, r9
    mov rax, rdx
    align 64

.loop:
    ; Read 256 bytes
    vmovdqu ymm0, [rax]
    vmovdqu ymm0, [rax + 0x20]
    vmovdqu ymm0, [rax + 0x40]
    vmovdqu ymm0, [rax + 0x60]
    vmovdqu ymm0, [rax + 0x80]
    vmovdqu ymm0, [rax + 0xa0]
    vmovdqu ymm0, [rax + 0xc0]
    vmovdqu ymm0, [rax + 0xe0]

    ; Advance and mask the read offset
    add r9, 0x100
    and r9, r8

    ; Update the read base pointer to point to the new offset
    mov rax, rdx
    add rax, r9

    ; Repeat
    sub rcx, 0x100
    jnz .loop

    ret
