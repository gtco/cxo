%include "symbol.inc"
;%include "gdt.inc"

IMPORT default_handler_impl
IMPORT keyboard_handler_impl
IMPORT page_fault_handler_impl
IMPORT timer_handler_impl
IMPORT gp_handler_impl

IMPORT current_task
IMPORT stack_pointers
IMPORT next_task

EXPORT set_gdtr
EXPORT set_idtr
EXPORT outb
EXPORT outw
EXPORT isr_default
EXPORT isr_keyboard
EXPORT isr_page_fault
EXPORT isr_timer
EXPORT isr_gp_fault
EXPORT inb
EXPORT set_page_dir
EXPORT get_page_dir
EXPORT set_page_enable
EXPORT get_page_enable
EXPORT test_and_set
EXPORT load_task_reg
EXPORT get_data_segment
EXPORT get_code_segment
EXPORT get_stack_pointer
EXPORT get_flags

;------------------------- Declarations ---

KERNEL_CS   EQU     0x08
KERNEL_DS   EQU     0x10
KERNEL_SS   EQU     0x10
USER_CS     EQU     0x18
USER_DS     EQU     0x20

;------------------------- Code Section ---

;SECTION .text USE32

;------------------------------------------
; int set_gdtr
;------------------------------------------

set_gdtr:
    push ebp            ; set up the stack pointer
    mov ebp, esp
                        ; load arguments
    mov eax, [ebp+8]    ; arg1 by value
    lgdt[eax]
    jmp KERNEL_CS:flush ; KERNEL_CS
    nop

flush:
    mov ax,KERNEL_SS    ; KERNEL_SS
    mov ss,ax
    mov ax,KERNEL_DS    ; KERNEL_DS
    mov ds,ax           ; not segments anymore: SELECTORS
    mov es,ax
    mov ax, 0
    mov fs,ax
    mov gs,ax
    mov eax, 0
    leave
    ret

;------------------------------------------
; int set_idtr
;------------------------------------------

set_idtr:
    push ebp            ; set up the stack pointer
    mov ebp, esp
                        ; load arguments
    mov eax, [ebp+8]    ; arg1 by value
    lidt[eax]
    sti
    leave
    ret


set_ldtr:
		mov 	eax, 0        
        lldt    ax              ; load LDTR
        leave
        ret

;------------------------------------------

; void outb ( uint16 port, uint8 value)
;------------------------------------------

outb:
    push ebp
    mov ebp, esp

    mov al, [ebp+12]
    mov dx, [ebp+8]
    out dx, al

    leave
    ret

;------------------------------------------
; void outw ( uint16 port, uint16 value)
;------------------------------------------

outw:
    push ebp
    mov ebp, esp

    mov ax, [ebp+12]
    mov dx, [ebp+8]
    out dx, ax

    leave
    ret

;------------------------------------------
;------------------------------------------

isr_default:
    push    dword 0xff              ; error code
    push    dword 0xff              ; interrupt number

    push    ds
    push    es
    push    fs
    push    gs                      ; saving segment registers and
    pushad                          ; other regs because it's an ISR

    call    default_handler_impl    ; call scheduler

    popad                           ; restoring the regs
    pop     gs
    pop     fs
    pop     es
    pop     ds
    add     esp, 8
    iretd

;------------------------------------------
;------------------------------------------

isr_keyboard:
    push    dword 0xff              ; error code
    push    dword 0x21              ; interrupt number

    push    ds
    push    es
    push    fs
    push    gs                      ; saving segment registers and
    pushad                          ; other regs because it's an ISR

	; The argument passed is a pointer to a struct
	; which describes the stack layout for all interrupts.
	push	esp
    call    keyboard_handler_impl   
	add	esp, 4			; clear 1 argument (esp)

    popad                           ; restoring the regs
    pop     gs
    pop     fs
    pop     es
    pop     ds
    add     esp, 8
    iretd

;------------------------------------------
;------------------------------------------

isr_page_fault:
;   push    dword 0xff              ; error code
    push    dword 0x0E              ; interrupt number

    push    ds
    push    es
    push    fs
    push    gs                      ; saving segment registers and
    pushad                          ; other regs because it's an ISR

    call    page_fault_handler_impl ;

    popad                           ; restoring the regs
    pop     gs
    pop     fs
    pop     es
    pop     ds
    add     esp, 8
    iretd

;------------------------------------------
;------------------------------------------

isr_gp_fault:
;   push    dword 0xff              ; error code
    push    dword 0x0D              ; interrupt number
    
    push    ds
    push    es
    push    fs
    push    gs                      ; saving segment registers and
    pushad                          ; other regs because it's an ISR

    call    gp_handler_impl ;

    popad                           ; restoring the regs
    pop     gs
    pop     fs
    pop     es
    pop     ds
    add     esp, 8
    iretd
    
;------------------------------------------
;------------------------------------------

;isr_timer:
;    push    dword 0xff              ; error code
;    push    dword 0x20              ; interrupt number
;
;    push    ds
;    push    es
;    push    fs
;    push    gs                      ; saving segment registers and
;    pushad                          ; other regs because it's an ISR
;
;    call    timer_handler_impl      ; call scheduler
;
;    popad                           ; restoring the regs
;    pop     gs
;    pop     fs
;    pop     es
;    pop     ds
;    add     esp, 8
;    iretd

;------------------------------------------
; uint8 inb (uint16 port)
;------------------------------------------

inb:
    push ebp
    mov ebp, esp

    mov dx, [ebp+8]
    in al, dx

    pop ebp
    ret

;------------------------------------------
; Before enabling paging, the address of the
; page directory must be stored in CR3
;------------------------------------------

get_page_dir:
    mov eax, cr3
    ret

set_page_dir:
    push ebp
    mov ebp, esp

    mov eax, [ebp+8]
    mov cr3, eax

    pop ebp
    ret
        
;--------------------------------------------
; The paging bit of CR0(bit 31) when set to 1
; enables paging. 
;--------------------------------------------

get_page_enable:
    mov eax, cr0
    ret

set_page_enable:
    push ebp
    mov ebp, esp

    mov eax, [ebp+8]
    mov cr0,  eax

    pop ebp
    ret

;-------------------------------------------------------
; int test_and_set(int new_value, int *lock_pointer)
;-------------------------------------------------------

test_and_set:
    push ebp
    mov ebp, esp
    mov eax, [ebp+8]        ; get new_value into eax
    mov edx, [ebp+12]       ; get lock_pointer into edx
    lock                    ; next instruction is locked
    xchg eax,[edx]          ; swap eax with what is stored in [edx]
                            ; ... and don't let any other cpu touch that
                            ; ... memory location while you're swapping
    pop ebp
    ret                     ; return the old value that's in eax

load_task_reg:
        push    ebp
        mov     ebp, esp
        ltr     [ebp+8]
        mov 	eax, 0
        str		ax
        pop     ebp
        ret


get_data_segment:
    mov eax, ds
    ret
    
get_code_segment:
    mov eax, cs
    ret    

get_stack_pointer:
    mov eax, esp
    ret    

get_flags:
	pushfd
	pop	eax
	ret