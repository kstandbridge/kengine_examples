
global DoubleLoopRead_32x8

section .text

; NOTE(kstandbridge): Win64 ABI https://learn.microsoft.com/en-us/cpp/build/x64-software-conventions?view=msvc-170
; RCX the first parameter and RDX is the second parameter

;   rcx: Count (divisible by 256)
;   rdx: Data pointer
;    r8: InnerLoopCount

DoubleLoopRead_32x8:
    align 64

.outer:
    mov r9, r8      ; Reset counter
    mov rax, rdx    ; Reset read pointer

.inner:
    ; Read 256 bytes
    vmovdqu ymm0, [rax]
    vmovdqu ymm0, [rax + 0x20]
    vmovdqu ymm0, [rax + 0x40]
    vmovdqu ymm0, [rax + 0x60]
    vmovdqu ymm0, [rax + 0x80]
    vmovdqu ymm0, [rax + 0xa0]
    vmovdqu ymm0, [rax + 0xc0]
    vmovdqu ymm0, [rax + 0xe0]
    add rax, 0x100  ; Advance the read pointer by 256 bytes
    dec r9
    jnz .inner

    dec rcx
    jnz .outer
    ret
