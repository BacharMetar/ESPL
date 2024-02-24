WRITE EQU 4
STDOUT EQU 1
STDERR EQU 2


global _start

section .rodata
    new_line: db 10  ;define new_line as ascii val of \n = 10

section .text
_start:
main:
    push    ebp
    mov     ebp, esp               ; Set up base pointer
    
    mov     edi, dword [esp + 4]   ; edi = argc
    mov     esi, dword [esp + 8]   ; esi = argv

print_arguments:   
    cmp     edi, 0                 ; Check if all argv are read
    jz      call_exit

    mov     ecx, esi
    push    ecx
    call    my_strlen

    mov     edx, eax               ; edx = word length
    mov     eax, WRITE
    mov     ebx, STDERR
    mov     ecx, esi    
    int     0x80                    ; Print the argument

    add     esi, edx               ; Move to the next argument
    inc     esi
    dec     edi                     ; Decrement argc
    
    ; Print new line
    mov     eax, WRITE
    mov     ebx, STDERR
    mov     ecx, new_line
    mov     edx, 1
    int     0x80                     ; Print newline
    
    jmp     print_arguments

call_exit:
    mov     eax, 1          
    mov     ebx, 0          
    int     0x80                     ; Exit

my_strlen:
    mov     eax, 0                  ; Initialize length to 0
cont:
    cmp     BYTE [ecx], 0     ; Check for null terminator
    jz      done
    inc     ecx                      ; Increment length
    inc     eax  
    jmp     cont

done:
    ret
