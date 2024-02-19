WRITE EQU 4
STDOUT EQU 1

section .bss
    Infile  resb 1   ; Reserve space for input file descriptor
    Outfile resb 1   ; Reserve space for output file descriptor

section .rodata
    new_line:   db 10  ; Define new_line as the ASCII value of \n = 10

section .text
global _start

_start:
    ; Open the output file
    mov eax, 5                 ; Syscall number for open
    mov ebx, output_file       ; Address of the filename string
    mov ecx, 65                ; Flags for opening the file (O_WRONLY | O_CREAT | O_TRUNC, 0644)
    mov edx, 0644              ; Permissions for the file
    int 0x80                   ; Invoke syscall
    mov dword [Outfile], eax   ; Save the file descriptor

    ; Error handling for file opening can be implemented here

    ; Start processing the arguments
    push    ebp
    mov     ebp, esp           ; Set up base pointer

    mov     edi, dword [esp + 4]   ; edi = argc
    mov     esi, dword [esp + 8]   ; esi = argv

    ; Skip the first argument (program name)
    add     esi, 8             ; Move to argv[1]

print_arguments:   
    cmp     edi, 1             ; Check if all argv are read (excluding the program name)
    jz      call_exit

    mov     ecx, esi
    call    my_strlen          ; Get the length of the argument

print_argument_loop:
    cmp     byte [ecx], 0      ; Check for null terminator
    jz      print_newline      ; If end of argument, print newline and proceed to next argument

    ; Call encode function to encode the character
    push    ecx                ; Save the current argument pointer
    call    encode             ; Call the encode function
    pop     ecx                ; Restore the current argument pointer

    ; Write the encoded character to the output file
    mov     eax, WRITE         ; Syscall number for write
    mov     ebx, dword [Outfile] ; File descriptor for output file
    mov     edx, 1             ; Length of the character
    int     0x80               ; Invoke syscall

    inc     ecx                ; Move to the next character in the argument
    jmp     print_argument_loop     ; Continue looping through the argument

print_newline:
    ; Print newline character to the output file
    mov     eax, WRITE         ; Syscall number for write
    mov     ebx, dword [Outfile] ; File descriptor for output file
    mov     ecx, new_line
    mov     edx, 1
    int     0x80               ; Print newline

    ; Move to the next argument
    mov     ebx, esi           ; Store the pointer to the current argument
.next_argument:
    inc     ebx                ; Move to the next character
    cmp     byte [ebx], 0      ; Check for the null terminator
    jne     .next_argument     ; Continue until the end of the argument
    inc     ebx                ; Move past the null terminator
    mov     esi, ebx           ; Update the pointer to the next argument
    dec     edi                ; Decrement argc
    jmp     print_arguments    ; Continue printing arguments

call_exit:
    ; Close the output file
    mov     eax, 6             ; Syscall number for close
    mov     ebx, dword [Outfile] ; File descriptor for output file
    int     0x80               ; Invoke syscall

    ; Exit the program
    mov     eax, 1             ; Syscall number for exit
    xor     ebx, ebx           ; Exit status 0
    int     0x80               ; Invoke syscall

my_strlen:
    mov     eax, 0             ; Initialize length to 0
.cont:
    cmp     byte [esi + eax], 0  ; Check for null terminator
    jz      .done
    inc     eax                   ; Increment length
    jmp     .cont

.done:
    ret

encode:
    cmp     byte [ecx], 'A'      ; Check if character is in the range 'A' to 'z'
    jl      .not_alpha
    cmp     byte [ecx], 'z'
    jg      .not_alpha

    inc     byte [ecx]           ; Increment the character value by 1

.not_alpha:
    ret

section .data
    output_file db 'output.txt', 0  ; Output file name
