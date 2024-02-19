WRITE EQU 4
STDOUT EQU 1
O_CREAT equ 64
O_WRONLY equ 1
O_TRUNC equ 512


section .bss
    Infile  resb 1   ; Reserve space for input file descriptor
    Outfile resb 1   ; Reserve space for output file descriptor
    Filename resb 255 ; Reserve space for filename

section .data
    prefix_string db "-a", 0          ; Prefix string "-a" with null terminator

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

parse_arguments:   
    cmp     edi, 1                 ; Check if all argv are read (excluding the program name)
    jz      call_exit

    ; Check if the argument starts with "-a"
    mov     ecx, esi
    call    check_argument_prefix

    ; If the argument starts with "-a", it's a filename
    cmp     eax, 1
    je      handle_filename

    ; Encode and print the argument
    mov     ecx, esi
    call    print_argument
    jmp     move_to_next_argument

handle_filename:
    ; Copy the rest of the argument after "-a" into the Filename buffer
    mov     ecx, esi
    add     ecx, 2                 ; Move past the "-a" prefix
    mov     ebx, Filename
    call    copy_argument_with_null ; Copy the argument including null terminator


    ; Open the file for writing
    mov     eax, 5                  ; Syscall number for open
    mov     ebx, Filename           ; Filename to open
    mov     ecx, O_CREAT | O_WRONLY | O_TRUNC  ; File mode
    int     0x80                    ; Invoke syscall
    mov     dword [Outfile], eax    ; Store the file descriptor for writing

    jmp     move_to_next_argument

print_argument:
    ; Print the encoded argument to stdout
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
    mov     ebx, STDOUT             ; File descriptor for stdout
    mov     edx, 1                  ; Length of the character
    int     0x80                    ; Invoke syscall

    inc     ecx                     ; Move to the next character in the argument
    jmp     print_argument_loop     ; Continue looping through the argument

print_newline:
    ; Print newline character to stdout
    mov     eax, WRITE
    mov     ebx, STDOUT             ; File descriptor for stdout
    mov     ecx, new_line
    mov     edx, 1
    int     0x80                     ; Print newline
    ret

move_to_next_argument:
    ; Move to the next argument
    mov     ebx, esi                  ; Store the pointer to the current argument
.next_argument:
    inc     ebx                       ; Move to the next character
    cmp     byte [ebx], 0             ; Check for the null terminator
    jne     .next_argument            ; Continue until the end of the argument
    inc     ebx                       ; Move past the null terminator
    mov     esi, ebx                  ; Update the pointer to the next argument
    dec     edi                       ; Decrement argc
    jmp     parse_arguments           ; Continue parsing arguments

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

check_argument_prefix:
    ; Check if the argument starts with "-a"
    mov     eax, 0                    ; Initialize result to 0
    mov     ebx, ecx
    mov     edi, prefix_string        ; Address of "-a"
    mov     ecx, 2                    ; Compare first 2 characters only
    repe    cmpsb                     ; Compare strings
    jne     .prefix_not_found
    mov     eax, 1                    ; Set result to 1 if prefix is found
    ret

.prefix_not_found:
    ret

copy_argument:
    ; Copy the argument to the Filename buffer
    .copy_loop:
        lodsb                          ; Load byte from source address (esi) into AL and increment esi
        stosb                          ; Store byte from AL into destination address (edi) and increment edi
        cmp     al, 0                  ; Check for null terminator
        jne     .copy_loop             ; If not null terminator, continue copying
    ret


