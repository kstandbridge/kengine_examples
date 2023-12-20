global MOVAllBytesASM
global NOPAllBytesASM
global CMPAllBytesASM
global DECAllBytesASM

section .text

; NOTE(kstandbridge): Win64 ABI https://learn.microsoft.com/en-us/cpp/build/x64-software-conventions?view=msvc-170
; RCX the first parameter and RDX is the second parameter

MOVAllBytesASM:
    xor rax, rax
.loop:
    mov [rdx + rax], al
    inc rax
    cmp rax, rcx
    jb .loop
    ret

NOPAllBytesASM:
    xor rax, rax
.loop:
    db 0x0f, 0x1f, 0x00 ; NOTE(kstandbridge): NOP byte sequence https://www.felixcloutier.com/x86/nop
    inc rax
    cmp rax, rcx
    jb .loop
    ret

CMPAllBytesASM:
    xor rax, rax
.loop:
    inc rax
    cmp rax, rcx
    jb .loop
    ret

DECAllBytesASM:
.loop:
    dec rcx
    jnz .loop
    ret