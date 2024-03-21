global Read_x1
global Read_x2
global Read_x3
global Read_x4
global Read_1x2
global Read_4x2
global Read_8x2
global Read_16x2
global Read_32x2
global Read_32x4
global Read_32x6

section .text

; NOTE(kstandbridge): Win64 ABI https://learn.microsoft.com/en-us/cpp/build/x64-software-conventions?view=msvc-170
; RCX the first parameter and RDX is the second parameter

Read_x1:
    align 64
.loop:
    mov rax, [rdx]
    sub rcx, 1
    jnle .loop
    ret

Read_x2:
    align 64
.loop:
    mov rax, [rdx]
    mov rax, [rdx]
    sub rcx, 2
    jnle .loop
    ret

Read_x3:
    align 64
.loop:
    mov rax, [rdx]
    mov rax, [rdx]
    mov rax, [rdx]
    sub rcx, 3
    jnle .loop
    ret

Read_x4:
    align 64
.loop:
    mov rax, [rdx]
    mov rax, [rdx]
    mov rax, [rdx]
    mov rax, [rdx]
    sub rcx, 4
    jnle .loop
    ret

Read_1x2:
    align 64
.loop:
    mov al, [rdx]
    mov al, [rdx]
    sub rcx, 2
    jnle .loop
    ret

Read_4x2:
    xor rax, rax
    align 64
.loop:
    mov r8d, [rdx ]
    mov r8d, [rdx + 4]
    add rax, 8
    cmp rax, rcx
    jb .loop
    ret

Read_8x2:
    xor rax, rax
    align 64
.loop:
    mov r8, [rdx ]
    mov r8, [rdx + 8]
    add rax, 16
    cmp rax, rcx
    jb .loop
    ret

Read_16x2:
    xor rax, rax
    align 64
.loop:
    vmovdqu xmm0, [rdx]
    vmovdqu xmm1, [rdx + 16]
    add rax, 32
    cmp rax, rcx
    jb .loop
    ret

Read_32x2:
    xor rax, rax
    align 64
.loop:
    vmovdqu ymm0, [rdx]
    vmovdqu ymm1, [rdx + 32]
    add rax, 64
    cmp rax, rcx
    jb .loop
    ret

Read_32x4:
    xor rax, rax
    align 64
.loop:
    vmovdqu ymm0, [rdx]
    vmovdqu ymm1, [rdx + 32]
    vmovdqu ymm0, [rdx + 64]
    vmovdqu ymm1, [rdx + 96]
    add rax, 128
    cmp rax, rcx
    jb .loop
    ret

Read_32x6:
    xor rax, rax
    align 64
.loop:
    vmovdqu ymm0, [rdx]
    vmovdqu ymm1, [rdx + 32]
    vmovdqu ymm0, [rdx + 64]
    vmovdqu ymm1, [rdx + 96]
    vmovdqu ymm0, [rdx + 128]
    vmovdqu ymm1, [rdx + 160]
    add rax, 192
    cmp rax, rcx
    jb .loop
    ret
