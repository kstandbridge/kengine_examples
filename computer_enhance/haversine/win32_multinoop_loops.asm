global NOP3x1AllBytes
global NOP1x3AllBytes
global NOP1x9AllBytes

section .text

; NOTE(kstandbridge): Win64 ABI https://learn.microsoft.com/en-us/cpp/build/x64-software-conventions?view=msvc-170
; RCX the first parameter and RDX is the second parameter

NOP3x1AllBytes:
    xor rax, rax
.loop:
    db 0x0f, 0x1f, 0x00 ; NOTE(kstandbridge): NOP byte sequence https://www.felixcloutier.com/x86/nop
    inc rax
    cmp rax, rcx
    jb .loop
    ret

NOP1x3AllBytes:
    xor rax, rax
.loop:
    nop
    nop
    nop
    inc rax
    cmp rax, rcx
    jb .loop
    ret


NOP1x9AllBytes:
    xor rax, rax
.loop:
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    inc rax
    cmp rax, rcx
    jb .loop
    ret

