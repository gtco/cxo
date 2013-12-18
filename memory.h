#ifndef MEMORY_H
#define MEMORY_H

#define PAGE_SIZE	4096
// Minimum number of bytes reserved by morecore
#define NALLOC 		1024

// Page table privileges
#define	PRIV_PRESENT	0x001
#define	PRIV_WRITABLE	0x002
#define	PRIV_USER		0x004

// Align data structure and malloc(), and free()
// from The C Programming Language, Second Edition
// by Brian W. Kernighan and Dennis M. Ritchie.

typedef long Align;

union header {
    struct {
	union header	*ptr;
	unsigned 	size;
    } s;
    Align x;
};

typedef union header Header;

static inline void invalidate_page(void* page)
{
	asm volatile ("invlpg (%0)": :"r" (page));
}

// Return the page directory index for an address
static inline unsigned long PDI(unsigned long addr)
{
	return ((addr >> 22) & 0x3FF);
}

// Return the page table index for an address
static inline unsigned long PTI(unsigned long addr)
{
	return ((addr >> 12) & 0x3FF);
}

// Initialize and enable paging
void init_paging(unsigned long total_pages);

// Map a page table into the page directory
int map_page(unsigned long *dir, unsigned priv, unsigned long virt, unsigned long phys);

// Print page table info
void dump_page_tables(unsigned long *page_dir);

// Initialize memory allocation
void allocator_init(unsigned long sys_mem);

// Allocate a page
unsigned long *page_malloc();

// Extend the kernel's heap
char *sbrk(int size);

// Allocate a block of memory
void *malloc(unsigned nbytes);

// Request more memory from the kernel
Header *morecore(unsigned nu);

// Free a previously allocated block
void free(char *ap);

extern unsigned long *g_page_directory;

#endif
