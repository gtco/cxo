# makefile

# i386-elf cross compiler and binutils.
CC_PREFIX :=i586-elf-

# The C compiler used to compile the kernel.
CC := $(CC_PREFIX)gcc
LD := $(CC_PREFIX)ld

EXTRA_CFLAGS :=
CFLAGS := -O -Wall $(EXTRA_CFLAGS)

#NEWLIB_INCL := -I../newlib-1.10.0/include 
#LIBC := ../newlib-1.10.0/lib/libc.a
LIBC :=

NASM := nasm
NASMFLAGS := -f elf 
##-DNEED_UNDERSCORE

# Kernel source and object files.
C_SRCS := kernel.c      \
	console.c       \
	library.c       \
	interrupt.c \
	keyboard.c  \
	memory.c        \
	multiboot.c \
	lock.c \
	process.c \
	gmem.c gtree.c glist.c gslist.c

ASM_SRCS := start.asm i386.asm switch.asm

OBJS := $(ASM_SRCS:.asm=.o) $(C_SRCS:.c=.o) 

# Perl (needs to be version 5 or later).
PERL = perl

# Utility scripts
PAD := $(PERL) pad
NUMSECS := $(PERL) numsecs
ZEROFILE := $(PERL) zerofile
CAT := $(PERL) pcat
RM :=  rm -f

# Kernel entry point.
ENTRY = kernel_init

# ----------------------------------------------------------------------
# Compilation rules
# ----------------------------------------------------------------------

.SUFFIXES:
.SUFFIXES: .c .asm .o

.c.o:
	$(CC) $(CFLAGS) -c $<

.asm.o:
	$(NASM) $(NASMFLAGS) -o $*.o $<


# ----------------------------------------------------------------------
# Targets
# ----------------------------------------------------------------------

all : info kernel.elf

info:
	./build_info.sh

# Create a binary image of the kernel code and data.
# The file produced can be loaded into memory as-is and executed.
# The -S option strips out the symbol table.

kernel.elf storage.txt : $(OBJS) 
	$(LD) -M -o kernel.elf -T kernel.ls $(OBJS) $(LIBC) \
	> storage.txt

clean :
	$(RM) kernel.exe kernel.elf *.o \
	storage.txt 



