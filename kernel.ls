/* kernel.ls */

kernel_stack = 0x00104000; 
scode = 0xc0000000;

ENTRY (kernel_entry)

SECTIONS
{ 
    . = kernel_stack;
        
    .text :
    { 
        stext = .;
        *(.text)
        etext = .;        
    }   
	.rodata :
	{
		srodata = .;
		*(.rodata)
		erodata = .;
	}
    .data ALIGN (0x1000) :
    { 
        sdata = .;    
        *(.data)
        edata = .;
    }
	.bss :
	{
		sbss = .;
		stack = .;
		. += 0x1000;
		stack_end = .;

		*(.bss)
		*(COMMON)
		. = ALIGN(0x1000);
		ebss = .;
	}
	end = .;    
} 
