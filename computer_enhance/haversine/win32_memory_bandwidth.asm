global MOVAllBytesASM

section .text

; NOTE(kstandbridge): Win64 ABI https://learn.microsoft.com/en-us/cpp/build/x64-software-conventions?view=msvc-170
; RCX the first parameter and RDX is the second parameter

MOVAllBytesASM:
    xor rax, rax
    align 64
.loop:
    vmovdqu ymm0, [rdx + r9 + 0]
    vmovdqu ymm0, [rdx + r9 + 32]
    vmovdqu ymm0, [rdx + r9 + 64]
    vmovdqu ymm0, [rdx + r9 + 96]
    add rax, 128
    add r9, 128
    and r9, r8
    cmp rax, rcx
    jb .loop
    ret
