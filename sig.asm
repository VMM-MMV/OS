section .data
  sigusr1_msg db "SIGUSR1 received!"
  sigusr1_len equ $ - sigusr1_msg
  newline db 10 

section .bss
  sigaction_struct resb 152           ; struct sigaction
  oldaction_struct resb 152           ; for old action
  random_buffer resb 100              ; Buffer for random characters

section .text
  global _start

print_msg:
  push rax
  push rdi

  mov rax, 1
  mov rdi, 1
  syscall

  pop rdi
  pop rax
  ret

print_newline:
  push rsi
  push rdx

  mov rsi, newline
  mov rdx, 1 
  call print_msg

  pop rdx
  pop rsi
  ret

println_msg:
  call print_msg
  call print_newline
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
  mov rsi, sigusr1_msg
  mov rdx, sigusr1_len
  call println_msg
  ret

sigusr2_handler:
  xor rbx, rbx                        ; Counter

  .generate_loop:
    rdtsc                               ; Get timestamp counter
    xor rdx, rdx                        ; Clear high bits for division
    mov rcx, 26                        
    div rcx
    add dl, 65                         
    mov [random_buffer + rbx], dl      
    inc rbx
    cmp rbx, 100
    jl .generate_loop

    mov rsi, random_buffer
    mov rdx, 100
    call println_msg
    ret

