; tower_asm.asm - tiny x86_64 NASM program
; Reads up to 255 bytes from stdin and writes a response to stdout based on content.
; Linux x86_64 syscalls.

section .bss
    buf resb 256

section .data
    att_msg db "ATTACH_ACCEPT",10
    att_len equ $-att_msg
    pong_msg db "PONG",10
    pong_len equ $-pong_msg
    unk_msg db "UNKNOWN_COMMAND",10
    unk_len equ $-unk_msg
    det_msg db "DETACH_ACK",10
    det_len equ $-det_msg

section .text
    global _start

_start:
    ; read syscall: read(0, buf, 255)
    mov rax, 0
    mov rdi, 0
    mov rsi, buf
    mov rdx, 255
    syscall
    ; rax = bytes read
    cmp rax, 0
    jle _exit

    ; check for ATTACH_REQUEST (simple substring search)
    mov rcx, rax          ; length
    mov rdi, buf          ; pointer
    ; naive checks: look for 'ATTACH_REQUEST' or 'PING' or 'DETACH'
    ; check ATTACH_REQUEST
    mov rsi, att_check
    mov rdx, att_check_len
    call contains
    cmp rax, 1
    je send_attach

    ; check PING
    mov rsi, ping_check
    mov rdx, ping_check_len
    call contains
    cmp rax, 1
    je send_pong

    ; check DETACH
    mov rsi, det_check
    mov rdx, det_check_len
    call contains
    cmp rax, 1
    je send_detach

    ; default unknown
    mov rax, 1
    mov rdi, 1
    mov rsi, unk_msg
    mov rdx, unk_len
    syscall
    jmp _exit

send_attach:
    mov rax, 1
    mov rdi, 1
    mov rsi, att_msg
    mov rdx, att_len
    syscall
    jmp _exit

send_pong:
    mov rax, 1
    mov rdi, 1
    mov rsi, pong_msg
    mov rdx, pong_len
    syscall
    jmp _exit

send_detach:
    mov rax, 1
    mov rdi, 1
    mov rsi, det_msg
    mov rdx, det_len
    syscall
    jmp _exit

_exit:
    mov rax, 60
    xor rdi, rdi
    syscall

; ----------------------------------------------------------------
; contains: check if pattern (rsi,rdx) is contained in buffer (rdi,rcx)
; returns rax=1 if yes, 0 otherwise
; clobbers rbx, r8-r11
contains:
    push rbp
    mov rbp, rsp
    mov r8, rdi    ; buf ptr
    mov r9, rcx    ; buf len
    mov r10, rsi   ; pattern ptr
    mov r11, rdx   ; pattern len

    cmp r11, 0
    je .notfound

    xor rax, rax   ; default 0
    mov rbx, 0     ; offset

.loop:
    cmp rbx, r9
    jge .notfound
    ; if remaining < pattern len -> not found
    mov rax, r9
    sub rax, rbx
    cmp rax, r11
    jb .notfound
    ; compare bytes
    mov rdi, r8
    add rdi, rbx
    mov rcx, r11
    mov rsi, r10
    repe cmpsb
    ; after cmpsb ZF set if equal for entire length
    je .found
    ; else advance
    mov rax, 0
    inc rbx
    jmp .loop

.found:
    mov rax, 1
    jmp .done
.notfound:
    xor rax, rax
.done:
    pop rbp
    ret

section .rodata
att_check db "ATTACH_REQUEST",0
att_check_len equ $-att_check
ping_check db "PING",0
ping_check_len equ $-ping_check
det_check db "DETACH",0
det_check_len equ $-det_check
