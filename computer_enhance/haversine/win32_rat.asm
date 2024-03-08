global RATAdd
global RATMovAdd

section .text

; NOTE(kstandbridge): Win64 ABI https://learn.microsoft.com/en-us/cpp/build/x64-software-conventions?view=msvc-170
; RCX the first parameter and RDX is the second parameter


RATAdd:
align 64
    mov rax, 1000000000
.loop:
    add rcx, 1
    add rcx, 1
    dec rax
    jnz .loop
    ret

RATMovAdd:
align 64
    mov rax, 1000000000
.loop:
    mov rcx, rax
    add rcx, 1
    mov rcx, rax
    add rcx, 1
    dec rax
    jnz .loop
    ret