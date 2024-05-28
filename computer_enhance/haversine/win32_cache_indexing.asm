

global ReadStrided_32x2

section .text

; NOTE(kstandbridge): Win64 ABI https://learn.microsoft.com/en-us/cpp/build/x64-software-conventions?view=msvc-170
; RCX the first parameter and RDX is the second parameter

;   rcx: Count (divisible by 256)
;   rdx: Data pointer
;    r8: 64-byte reads per block
;    r9: stride for the read pointer

ReadStrided_32x2:
    align 64

.outer:
    mov r10, r8     ; Reset the reads-per-block counter
    mov rax, rdx    ; Reset read pointer to the beginning of the block

.inner:
    ; Read one cache line (64 bytes on x64 - you would change this to read 128 bytes for M-series, etc.)
    vmovdqu ymm0, [rax]
    vmovdqu ymm0, [rax + 0x20]
    add rax, r9  ; Advance the read pointer by the stride
    dec r10
    jnz .inner

    dec rcx
    jnz .outer
    ret
