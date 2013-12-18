;------------------------- Code Section ---
%include "symbol.inc"

;SECTION .text USE32

%macro REG_SAVE 0
	;save all registers in the kernel-level stack of the process and switch to the kernel stack
	cld
	pushad
	push ds
	push es
	push fs
	push gs
	mov ax,0x10 ;load kernel segments
	mov ds,ax
	mov es,ax
	mov fs,ax
	mov gs,ax
	mov eax,[g_current] ;put the adress of the struct of CURRENT PROCESS in eax.(the CONTENT of pointer p)
	mov [eax],esp ;save esp in the location of esp in the CURRENT PROCESS-STRUCT.
;	lea eax,[kstackend] ; switch to the kernel's own stack.
;	mov esp,eax
%endmacro

%macro REG_RESTORE_MASTER 0
	cmp	[g_reschedule], dword 0 ; See if we need to choose a new thread to run.
	je	.restore
	mov ebx,esp ; -*-
	mov eax,[g_current] ;put adress of struct of current process in eax.
	mov esp,[eax] ;restore adress of esp.
;	mov ebx,[eax+8];put content of the k-stack field into ebx.
	mov [g_tss+4],ebx ;update system tss.
	mov [g_reschedule], dword 0
;	mov al,0x20
;	out 0x20,al
.restore:
	pop gs
	pop fs
	pop es
	pop ds
	popad
	iretd
%endmacro

;and here is an example isr.
[EXTERN timer_handler_impl]
[EXTERN g_reschedule]
[EXTERN g_current]
[EXTERN g_tss]
[GLOBAL isr_switch]
isr_switch:
	REG_SAVE
	call timer_handler_impl
	REG_RESTORE_MASTER