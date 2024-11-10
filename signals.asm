section .data
    debug_start db "Program started", 10
    debug_start_len equ $ - debug_start

    debug_sig1 db "Setting up SIGUSR1", 10
    debug_sig1_len equ $ - debug_sig1

    debug_sig2 db "Setting up SIGUSR2", 10
    debug_sig2_len equ $ - debug_sig2

    debug_loop db "Entering pause loop", 10
    debug_loop_len equ $ - debug_loop

    sigusr1_msg db "SIGUSR1 received!", 10
    sigusr1_len equ $ - sigusr1_msg

    debug_sigusr1 db "SIGUSR1 handler entered", 10
    debug_sigusr1_len equ $ - debug_sigusr1

    debug_sigusr2 db "SIGUSR2 handler entered", 10
    debug_sigusr2_len equ $ - debug_sigusr2

    random_chars db "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$%^&*()", 0
    random_chars_len equ $ - random_chars - 1

section .bss
    sigaction_struct resb 152           ; struct sigaction
    oldaction_struct resb 152           ; for old action
    random_buffer resb 100              ; Buffer for random characters

section .text
    global _start

print_msg:                              ; Helper function for debug messages
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
    ; Print start message
    mov rsi, debug_start
    mov rdx, debug_start_len
    call print_msg

    ; Initialize sigaction struct
    mov qword [sigaction_struct], sigusr1_handler    ; sa_handler
    mov qword [sigaction_struct + 8], 0x04000000     ; SA_RESTART flag
    mov qword [sigaction_struct + 16], 0             ; sa_restorer
    mov rcx, 16                                      ; Clear sa_mask
    mov rdi, sigaction_struct + 24
    xor rax, rax
    rep stosq

    ; Print SIGUSR1 setup message
    mov rsi, debug_sig1
    mov rdx, debug_sig1_len
    call print_msg

    ; Setup SIGUSR1 handler
    mov rax, 13                         ; sys_rt_sigaction
    mov rdi, 10                         ; SIGUSR1
    mov rsi, sigaction_struct
    mov rdx, oldaction_struct
    mov r10, 8                          ; sigsetsize
    syscall

    ; Initialize sigaction struct for SIGUSR2
    mov qword [sigaction_struct], sigusr2_handler    ; sa_handler
    
    ; Print SIGUSR2 setup message
    mov rsi, debug_sig2
    mov rdx, debug_sig2_len
    call print_msg

    ; Setup SIGUSR2 handler
    mov rax, 13                         ; sys_rt_sigaction
    mov rdi, 12                         ; SIGUSR2
    mov rsi, sigaction_struct
    mov rdx, oldaction_struct
    mov r10, 8                          ; sigsetsize
    syscall

    ; Print entering loop message
    mov rsi, debug_loop
    mov rdx, debug_loop_len
    call print_msg

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

    ; Print debug message
    mov rsi, debug_sigusr1
    mov rdx, debug_sigusr1_len
    call print_msg

    ; Print actual message
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

    ; Print debug message
    mov rsi, debug_sigusr2
    mov rdx, debug_sigusr2_len
    call print_msg

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
