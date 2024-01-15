global ConditionalNOP

section .text

; NOTE(kstandbridge): Win64 ABI https://learn.microsoft.com/en-us/cpp/build/x64-software-conventions?view=msvc-170
; RCX the first parameter and RDX is the second parameter

ConditionalNOP:
    xor rax, rax
.loop:
    mov r10, [rdx + rax]
        inc rax
        test r10, 1
    jnz .skip
        nop
    .skip:
        cmp rax, rcx
        jb .loop
        ret
