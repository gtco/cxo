#ifndef COMMON_H
#define COMMON_H

#ifdef M$
#define PACK #pragma pack(push,1)\n
#define UNPACK #pragma pack(pop)\n
#else
#define PACK __attribute__((__packed__))
#define UNPACK
#endif

#define GDT_SIZE	8

#ifndef	NULL
#define	NULL	((void*) 0)
#endif

#ifndef	FALSE
#define	FALSE	(0)
#endif

#ifndef	TRUE
#define	TRUE	(!FALSE)
#endif

typedef unsigned char boolean_t;

// Platform specific type definitions
typedef signed char	int8;
typedef signed short int16;
typedef signed int int32;
typedef signed long long int64;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;

extern const unsigned short KERNEL_CS;
extern const unsigned short KERNEL_SS;
extern const unsigned short KERNEL_DS;
extern const unsigned short USER_CS;
extern const unsigned short USER_DS;

/*
PACK

typedef struct descriptor_s {

  unsigned int limit;
  unsigned int base_low;

  unsigned char base_mid;
  unsigned char access;
  unsigned char attribs;
  unsigned char base_hi;

} descriptor_t;

typedef struct stack_addr_s  {
  unsigned long offset;
  unsigned long selector;
} stack_addr_t;

UNPACK
*/

static inline unsigned check_flag(unsigned long flags, unsigned bit)
{
	return ((flags) & (1 << (bit)));
}

extern unsigned char scode[], sbss[], stack[], stack_end[], page_dir[], ebss[], kernel_stack[];

//
// Assembly function declarations
//
int set_gdtr(unsigned char *ptr);
int set_idtr(unsigned char *ptr);

void outb ( unsigned short port, unsigned char value);
void outw ( unsigned short port, unsigned short value);
unsigned char inb (unsigned short port);

void isr_default(void);
void isr_keyboard(void);
void isr_timer(void);
void isr_page_fault(void);
void isr_gp_fault(void);
void isr_task_switch(void);

void set_page_enable(unsigned long flag);
unsigned long get_page_enable();

void set_page_dir(unsigned long *page_directory);
unsigned long get_page_dir();

int test_and_set(int new_value, int *lock_pointer);

unsigned int load_task_reg(unsigned int s);

unsigned long get_code_segment();
unsigned long get_data_segment();

unsigned long get_stack_pointer();

unsigned long get_flags();

#endif

/*
00 cf 98 00 00 00 ff ff

8=1byte
			 a
			 v
--base--  gxol lim  pdp   tta  --base--						 --limit--
0000 0000 1100 1111 1001 1000  0000 0000 0000 0000 0000 0000 1111 1111 1111 1111

3         2			1				   0 3					 1				   0
1	      3         5                    1  				 6

41000000

%macro desc 3
w_a_n_p_b
%if (%3) & (~(%3)>>2) & 0x400   ;Gate
	dw	%1
	dw	%2
	dw	(%3)+D_PRESENT
	dw	(%1) >> 16
%else                           ;Not a gate
	dw	%2
	dw	%1
	db	(%1) >> 16
	db	((%3)+D_PRESENT) >> 8
	db	(%3) + ((%2) >> 16)
	db	(%1) >> 24
%endif
%endmacro


code32	desc	0, codeLEN-1, D_CODE + D_BIG
data32	desc	0, stack_top, D_DATA + D_BIG + D_WRITE
*/

