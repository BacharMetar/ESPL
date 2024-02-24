READ EQU 3
WRITE EQU 4
OPEN EQU 5
CLOSE EQU 6
STDIN EQU 0
STDOUT EQU 1
STDERR EQU 2
O_RDONLY EQU 0
O_WRONLY EQU 1
O_CREAT EQU 64
BUFFER_SIZE EQU 256

global _start

section .bss
    UserInput: resb BUFFER_SIZE ; Global variable for user input
    UserInputDummy:

section .rodata
    new_line:   db 10  ; Define new_line as the ASCII value of \n = 10
    filename_error: db "Error, given filename does not exist", 0

section .data
    Infile: dd STDIN   ; Global variable for input file descriptor
    Outfile: dd STDOUT   ; Global variable for for output file descriptor

section .text
_start:
main:
    push    ebp
    mov     ebp, esp               ; Set up base pointer
    mov     edi, dword [esp + 4]   ; edi = argc
    mov     esi, dword [esp + 8]   ; esi = argv

    ; Skip the first argument (program name)
    ;add     esi, 8                 ; Move to argv[1] (shouldn't work correctly since the program name's length varies)
.skip_first_arg:
    inc     esi                       ; Move to the next character
    cmp     byte [esi], 0             ; Check for the null terminator
    jne     .skip_first_arg           ; Continue until the end of the argument
    inc     esi                       ; Move past the null terminator
    jmp     print_arguments           ; Continue printing arguments


print_arguments:   
    cmp     edi, 1                 ; Check if all argv are read (excluding the program name)
    jz      get_input              ; Continue to the next part of the program - get user input and print the encoded chars

    pushad
    push    esi
    push    STDERR
    call    my_println                  ; Print argv[i] to stderr
    add     esp, 8                      ; Remove pushed parameters from stack
    popad

    pushad
    push    esi
    call    check_input_filename        ; Check if argv[i] starts with "-i" and if so, saves the filename into Infile
    add     esp, 4
    popad
    pushad
    push    esi
    call    check_output_filename       ; Check if argv[i] starts with "-o" and if so, saves the filename into Outfile
    add     esp, 4
    popad

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


my_print:
    mov     edi, dword [esp + 4]   ; edi = file_descriptor
    mov     esi, dword [esp + 8]   ; esi = string_to_print
    mov     ecx, esi
    call    my_strlen              ; Get the length of the argument

    ; Print the character to file_descriptor
    mov     edx, eax                ; Length of the character
    mov     eax, WRITE              ; Syscall number for write
    mov     ebx, edi                ; File descriptor from argument
    int     0x80                    ; Invoke syscall
    ret

my_println:
    mov     edi, dword [esp + 4]   ; edi = file_descriptor
    mov     esi, dword [esp + 8]   ; esi = string_to_print
    mov     ecx, esi
    call    my_strlen              ; Get the length of the argument

    ; Print the character to file_descriptor
    mov     edx, eax                ; Length of the character
    mov     eax, WRITE              ; Syscall number for write
    mov     ebx, edi                ; File descriptor from argument
    int     0x80                    ; Invoke syscall
print_newline:
    ; Print newline character to file_descriptor
    mov     eax, WRITE
    mov     ebx, edi                ; File descriptor from argument
    mov     ecx, new_line
    mov     edx, 1
    int     0x80                     ; Print newline
    ret


call_exit:
    call    close_files
    mov     eax, 1          
    mov     ebx, 0          
    int     0x80                     ; Exit with zero exit code

call_exit_error:
    call    close_files
    mov     eax, 1          
    mov     ebx, 1          
    int     0x80                     ; Exit with non-zero exit code

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

check_input_filename:
    mov     esi, dword [esp + 4]   ; esi = input_filename
    cmp     word [esi], '-'+(256*'i')  ; equivalently check if arg starts with "-i"
    je      save_input_filename
    ret

save_input_filename:
    ; Infile = open(argv[i]+2, "r")
    mov     eax, OPEN
    mov     ebx, esi
    add     ebx, 2 ; ebx = argv[i]+2
    mov     ecx, O_RDONLY
    int     0x80
    cmp     eax, 0
    jl      invalid_filename
    mov     [Infile], eax               ; Save file descriptor of input file
    ret

invalid_filename:
    ; Print error to stderr and exit with error code 1
    push    filename_error
    push    STDERR
    call    my_println
    add     esp, 8
    jmp     call_exit_error

check_output_filename:
    mov     esi, dword [esp + 4]   ; esi = input_filename
    cmp     word [esi], '-'+(256*'o')  ; equivalently check if arg starts with "-o"
    je      save_output_filename
    ret

save_output_filename:
    ; Outfile = open(argv[i]+2, "w")
    mov     eax, OPEN
    mov     ebx, esi
    add     ebx, 2 ; ebx = argv[i]+2
    mov     ecx, O_WRONLY
    or      ecx, O_CREAT
    int     0x80
    cmp     eax, 0
    jl      invalid_filename
    mov     [Outfile], eax               ; Save file descriptor of output file
    ret

close_files:
    mov     eax, CLOSE
    mov     ebx, [Infile]
    int     0x80
    mov     eax, CLOSE
    mov     ebx, [Outfile]
    int     0x80
    ret;

get_input:
    ; Clear input buffer
    mov     eax, UserInput
clear_input_next:           ; Clear the input buffer to be all zeros
    cmp     eax, UserInputDummy
    je      read_input      ; Jump to the next part, read the input
    mov     [eax], byte 0
    inc     eax
    jmp     clear_input_next
read_input:                 ; read(Infile, UserInput, BUFFER_SIZE)
    mov     eax, READ
    mov     ebx, [Infile]
    mov     ecx, UserInput
    mov     edx, BUFFER_SIZE
    int     0x80
    cmp     eax, 0          ; If end-of-file detected, jump to exit
    je      call_exit
    mov     ecx, UserInput
char_loop:
    cmp     byte [ecx], 0           ; Check for null terminator
    je      get_input               ; If end of string, continue to the next input

    ; Call encode function to encode the character in ecx
    call    encode                  ; Call the encode function

    ; Print the encoded character to Outfile
    mov     eax, WRITE              ; Syscall number for write
    mov     ebx, dword [Outfile]    ; File descriptor for Outfile (using global variable)
    mov     edx, 1                  ; Length of the character
    int     0x80                    ; Invoke syscall

    inc     ecx                     ; Move to the next character in the input
    jmp     char_loop               ; Loop back to continue with the next character
