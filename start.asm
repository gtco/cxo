%include "symbol.inc"

IMPORT kernel_init
IMPORT kernel_stack

EXPORT kernel_entry

;SECTION .text USE32

F_PAGE_ALIGN equ 1<<0
F_MEM_FIELDS equ 1<<1

MAGIC        equ 0x1BADB002
FLAGS        equ F_PAGE_ALIGN | F_MEM_FIELDS
CHECKSUM     equ -(MAGIC + FLAGS)

kernel_entry:
    mov esp, kernel_stack
    push ebx
    call kernel_init

; multiboot header

    align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM

