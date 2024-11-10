section .data
    sigusr1_msg db "SIGUSR1 received!", 10
    sigusr1_len equ $ - sigusr1_msg

    random_chars db "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$%^&*()", 0
    random_chars_len equ $ - random_chars - 1

section .bss
    sigaction_struct resb 152           ; struct sigaction
    oldaction_struct resb 152           ; for old action
    random_buffer resb 100              ; Buffer for random characters

section .text
    global _start

print_msg:                              
    push rbp
    mov rbp, rsp
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11

    mov rax, 1
    mov rdi, 1
    syscall

    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rbp
    ret

_start:
    ; Initialize sigaction struct
    mov qword [sigaction_struct], sigusr1_handler    ; sa_handler
    mov qword [sigaction_struct + 8], 0x04000000     ; SA_RESTART flag
    mov qword [sigaction_struct + 16], 0             ; sa_restorer
    mov rcx, 16                                      ; Clear sa_mask
    mov rdi, sigaction_struct + 24
    xor rax, rax
    rep stosq

    ; Setup SIGUSR1 handler
    mov rax, 13                         ; sys_rt_sigaction
    mov rdi, 10                         ; SIGUSR1
    mov rsi, sigaction_struct
    mov rdx, oldaction_struct
    mov r10, 8                          ; sigsetsize
    syscall

    ; Initialize sigaction struct for SIGUSR2
    mov qword [sigaction_struct], sigusr2_handler    ; sa_handler

    ; Setup SIGUSR2 handler
    mov rax, 13                         ; sys_rt_sigaction
    mov rdi, 12                         ; SIGUSR2
    mov rsi, sigaction_struct
    mov rdx, oldaction_struct
    mov r10, 8                          ; sigsetsize
    syscall

main_loop:
    mov rax, 34                         ; sys_pause
    syscall
    jmp main_loop

sigusr1_handler:
    push rbp
    mov rbp, rsp
    sub rsp, 8                          ; Align stack to 16 bytes
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11

    ; Print message
    mov rsi, sigusr1_msg
    mov rdx, sigusr1_len
    call print_msg

    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    leave
    ret

sigusr2_handler:
    push rbp
    mov rbp, rsp
    sub rsp, 8                          ; Align stack
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11

    ; Generate 100 random characters
    xor rbx, rbx                        ; Counter
.generate_loop:
    rdtsc                               ; Get timestamp counter
    xor rdx, rdx                        ; Clear high bits for div
    mov rcx, random_chars_len
    div rcx                             ; Get remainder for random index
    mov al, [random_chars + rdx]        ; Get random char
    mov [random_buffer + rbx], al       ; Store in buffer
    inc rbx
    cmp rbx, 100
    jl .generate_loop

    ; Print random characters
    mov rax, 1                          ; sys_write
    mov rdi, 1                          ; stdout
    mov rsi, random_buffer
    mov rdx, 100
    syscall

    ; Add newline
    mov byte [random_buffer], 10        ; newline character
    mov rax, 1
    mov rdi, 1
    mov rsi, random_buffer
    mov rdx, 1
    syscall

    ; Clean up stack before exit
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    leave

    ; Exit program
    mov rax, 60                         ; sys_exit
    xor rdi, rdi                        ; status code 0
    syscall
