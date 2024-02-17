section .text
global _start

_start:
    ; Print "hello world" to stdout
    mov     edx, 11            ; Length of the string
    mov     ecx, hello_world   ; Address of the string
    mov     ebx, 1             ; File descriptor for stdout
    mov     eax, 4             ; SYS_WRITE system call number
    int     0x80               ; Call the kernel

    ; Print newline
    mov     edx, 1             ; Length of the newline character
    mov     ecx, newline       ; Address of the newline character
    mov     ebx, 1             ; File descriptor for stdout
    mov     eax, 4             ; SYS_WRITE system call number
    int     0x80               ; Call the kernel

    ; Exit the program
    mov     eax, 1             ; SYS_EXIT system call number
    xor     ebx, ebx           ; Exit code 0
    int     0x80               ; Call the kernel

section .data
hello_world db 'hello world', 0x0A  ; String to print, followed by newline character
newline db 0x0A                     ; Newline character
