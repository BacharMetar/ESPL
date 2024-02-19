WRITE EQU 4
STDOUT EQU 1

section .bss
    Infile  resb 1   ; Reserve space for input file descriptor
    Outfile resb 1   ; Reserve space for output file descriptor

global _start

section .rodata
    new_line:   db 10  ; Define new_line as the ASCII value of \n = 10

section .text
_start:
    push    ebp
    mov     ebp, esp               ; Set up base pointer
    
    mov     edi, dword [esp + 4]   ; edi = argc
    mov     esi, dword [esp + 8]   ; esi = argv

    ; Skip the first argument (program name)
    add     esi, 8                 ; Move to argv[1]


print_arguments:   
    cmp     edi, 1                 ; Check if all argv are read (excluding the program name)
    jz      call_exit

    mov     ecx, esi
    call    my_strlen              ; Get the length of the argument
print_argument_loop:
    cmp     byte [ecx], 0          ; Check for null terminator
    jz      print_newline          ; If end of argument, print newline and proceed to next argument

    ; Call encode function to encode the character
    push    ecx                     ; Save the current argument pointer
    call    encode                  ; Call the encode function
    pop     ecx                     ; Restore the current argument pointer

    ; Print the encoded character to stdout
    mov     eax, WRITE              ; Syscall number for write
    mov     ebx, dword [Outfile]    ; File descriptor for stdout (using global variable)
    mov     edx, 1                  ; Length of the character
    int     0x80                    ; Invoke syscall

    inc     ecx                     ; Move to the next character in the argument
    jmp     print_argument_loop     ; Continue looping through the argument

print_newline:
    ; Print newline character to stdout
    mov     eax, WRITE
    mov     ebx, dword [Outfile]    ; File descriptor for stdout (using global variable)
    mov     ecx, new_line
    mov     edx, 1
    int     0x80                     ; Print newline
    
    ; Move to the next argument
    mov     ebx, esi                  ; Store the pointer to the current argument
.next_argument:
    inc     ebx                       ; Move to the next character
    cmp     byte [ebx], 0             ; Check for the null terminator
    jne     .next_argument            ; Continue until the end of the argument
    inc     ebx                       ; Move past the null terminator
    mov     esi, ebx                  ; Update the pointer to the next argument
    dec     edi                       ; Decrement argc
    jmp     print_arguments           ; Continue printing arguments

call_exit:
    mov     eax, 1          
    mov     ebx, 0          
    int     0x80                     ; Exit

my_strlen:
    mov     eax, 0                   ; Initialize length to 0
.cont:
    cmp     byte [esi + eax], 0      ; Check for null terminator
    jz      .done
    inc     eax                        ; Increment length
    jmp     .cont

.done:
    ret

encode:
    cmp     byte [ecx], 'A'           ; Check if character is in the range 'A' to 'z'
    jl      .not_alpha
    cmp     byte [ecx], 'z'
    jg      .not_alpha

    inc     byte [ecx]                ; Increment the character value by 1

.not_alpha:
    ret
