section .data
    STDIN  equ 0
    STDOUT equ 1
    STDERR equ 2
    O_RDONLY equ 0             ; Open for reading only
    O_CREAT  equ 0100          ; Create the file if it does not exist
    O_TRUNC  equ 01000         ; Truncate the file to zero length if it exists

section .bss
    Infile  resb 256   ; Reserve space for input file name
    Infd    resd 1     ; Reserve space for input file descriptor

section .text
global _start

_start:
    ; Set up base pointer
    push    ebp
    mov     ebp, esp               ; Set up base pointer
    
    ; Initialize input file descriptor to default (stdin)
    mov     dword [Infile], STDIN

    ; Parse command line arguments
    mov     esi, dword [ebp + 8]   ; argv
    mov     ecx, dword [ebp + 12]  ; argc
    add     esi, 4                  ; Skip program name

parse_args_loop:
    ; Check if there are no more arguments to parse
    cmp     ecx, 0
    je      open_file

    ; Check if the argument is an option flag like '-i'
    cmp     byte [esi], '-'         ; Check if argument starts with '-'
    jne     next_arg                ; If not, it's not an option, skip it

    ; Check for input file argument
    cmp     byte [esi + 1], 'i'     ; Check if argument is '-i'
    jne     next_arg                ; If it is not, skip it

    ; Get input file name
    mov     eax, dword [esi + 2]    ; Move to next character after '-i'
    mov     ebx, Infile
    call    copy_filename
    jmp     next_arg                ; Jump to next argument after processing

next_arg:
    ; Move to the next argument
    add     esi, 4
    dec     ecx
    jmp     parse_args_loop         ; Continue parsing arguments

open_file:
    ; Open input file
    mov     eax, 5                  ; Syscall number for open
    mov     ebx, dword [Infile]     ; Pointer to input file name
    mov     ecx, O_RDONLY          ; Flags: O_RDONLY
    int     0x80                    ; Invoke syscall
    mov     dword [Infd], eax       ; Save input file descriptor

    ; Check if file opened successfully
    cmp     eax, 0
    jl      open_error              ; Jump to open_error if file open failed

    ; File opened successfully, continue your program logic here

    ; Exit the program
    mov     eax, 1
    xor     ebx, ebx
    int     0x80

open_error:
    ; Handle file open error here

copy_filename:
    ; Function to copy file name from source to destination
    ; ecx = source, ebx = destination
    .copy_loop:
        mov     dl, byte [eax]
        mov     byte [ebx], dl
        inc     eax
        inc     ebx
        cmp     dl, 0
        jne     .copy_loop
    ret
