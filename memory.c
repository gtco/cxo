#include "memory.h"
#include "console.h"
#include "common.h"

// Globals

// Beginning of page allocation array
unsigned char *g_page_map = 0;
// Length of page allocation array
unsigned long g_page_map_len = 0;
// Beginning of page table directory
unsigned long *g_page_directory = 0;
static Header base;
static Header *freep = 0;
unsigned long kernel_sbrk = 0xF0000000;
// Code segments declared in the kernel linker script
extern unsigned char scode[], sbss[], stack[], stack_end[], page_dir[], ebss[];

// Initialize and enable paging
void init_paging(unsigned long total_pages)
{
	unsigned long* page_table = 0;
	unsigned long address = 0, i = 0, j = 0;
	int table_count = total_pages / 1024;

	g_page_directory = page_malloc();

	for (j = 0; j < table_count; j++)
	{
		page_table = page_malloc();

		for(i=0; i < 1024; i++)
		{
 			// Attribute set to: supervisor level, read/write, present(011 in binary)
			page_table[i] = address | 3;
			address = address + 4096;
		}

		g_page_directory[j] = (unsigned long) page_table;
		g_page_directory[j] = g_page_directory[j] | 3;

//		kprintf(" Identity Mapped (addr=0x%x) ", address);
	}

	// fill the rest of the page directory
	for(i=table_count; i<1023; i++)
	{
		// Attribute set to: supervisor level, read/write, not present(010 in binary)
		g_page_directory[i] = 0 | 2;
	};

	// Store the page directory address into CR3
	set_page_dir(g_page_directory);
	// Set the paging bit in CR0 to 1
	set_page_enable(get_page_enable() | 0x80000000);
}

int map_page(unsigned long *dir, unsigned priv, unsigned long virt, unsigned long phys)
{
	unsigned dirent, tabent;
	unsigned long *page_table;

	// Locate where the address is located in page directory
	dirent = PDI(virt);

	if(dir[dirent] == 0x02)
	{
		// If the page is not present, add a new page to avoid a page fault
		page_table = page_malloc();
		dir[dirent] = ((unsigned long) page_table & 0xFFFFF000L) | priv;
	}
	else
	{
		// Use existing page table
		page_table = (unsigned long *)(dir[dirent] & 0xFFFFF000L);
	}

	// Map page
	tabent = PTI(virt);
	priv &= 0xFFF;
	page_table[tabent] = (phys & 0xFFFFF000L) | priv;

//	kprintf(" Map Page: dirent=%u, tabent=%u, virt=0x%x, phys=0x%x ",
//		dirent, tabent, virt, phys);

	return 0;
}

void dump_page_tables(unsigned long *page_dir)
{
	unsigned long temp, *page_tab;
	unsigned long pde, pte;

	kprintf("page directory at 0x0%x\n", page_dir);

	/* RAM */
	for(pde = 0; pde < 512; pde++)
	{
		if ((page_dir[pde] & 0xFFFFF000L) == 0)
		{
			continue;
		}
		temp = page_dir[pde];
		kprintf("page dir[%u]=0x0%x\n", pde, temp);
		page_tab = (unsigned long *)(temp & -4096);

		/* show only entries 0, 1, and 1023 in the page dir (if they're valid) */
		for(pte = 0; pte < 1; pte++)
		{
			if(page_tab[pte] == 0)
			{
				continue;
			}
			temp = page_tab[pte];
			kprintf("  page tab[%u]=0x0%x\n", pte, temp);
		}
		for(pte = 1023; pte < 1024; pte++)
		{
			if (page_tab[pte] == 0)
			{
				continue;
			}
			temp = page_tab[1023];
			kprintf("  page tab[%u]=0x0%x\n", pte, temp);
		}
	}

	/* kernel memory */
	for(pde = 512; pde < 1024; pde++)
	{
		if ((page_dir[pde] & 0xFFFFF000L) == 0)
		{
			continue;
		}
		temp = page_dir[pde];
		kprintf("page dir[%u]=0x0%x\n", pde, temp);
		page_tab = (unsigned long *)(temp & -4096);
		// show all valid entries
		for(pte = 0; pte < 1024; pte++)
		{
			if(page_tab[pte] == 0)
			{
				continue;
			}
			temp = page_tab[pte];
			kprintf("  page tab[%u]=0x0%x\n", pte, temp);
		}
	}
}

void allocator_init(unsigned long sys_mem)
{
	int i = 0;

	unsigned long total_pages = (sys_mem / PAGE_SIZE);
	unsigned long reserved = 0, reserved_pages = 0;
	unsigned long reserved_entries = 0, partial_entries = 0;

	// Place the memory allocation array after the kernel code
	g_page_map = (unsigned char *) ebss + PAGE_SIZE;
	// 1 bit per page (0=free, 1=allocated)
	g_page_map_len = total_pages / 8;

	reserved = (unsigned long) g_page_map + g_page_map_len;
	reserved_pages = (reserved / PAGE_SIZE) + 1;
	reserved_entries = reserved_pages / 8;
	partial_entries = (reserved_pages % 8) + 1;

	TEXT_COLOR = BRIGHT_GREEN;

	kprintf("Memory Allocator (total=%u, ", total_pages);
	kprintf("reserved=%u, ", reserved_pages);
	kprintf("start=0x0%x, ", g_page_map);
	kprintf("end=0x0%x)\n", reserved);

	TEXT_COLOR = WHITE;

	// set map entries to zero
	for (i = 0; i < g_page_map_len; i++)
	{
		g_page_map[i] = 0x0;
	}

	// mark kernel memory used ( everything < kernel_end)
	for (i = 0; i < reserved_entries; i++)
	{
		g_page_map[i] = 0xff;
	}

	// each entry contains 8 pages, only part of the last entry
	// contains used pages
	for (i = 0; i < partial_entries; i++)
	{
		g_page_map[reserved_entries] = ((g_page_map[reserved_entries] << 1) + 1);
	}

	//
	// setup paging
	//
	init_paging(total_pages);
}

// Allocate a page
unsigned long *page_malloc(void)
{
	int i = 0, j = 0;
	unsigned long *page = 0;

	// Search array for a free page (page bit == 0)
	while(g_page_map[i / 8] & (1 << (i % 8)))
	{
		i++;
	}

	// Convert index to page
	page = (unsigned long *) (i*PAGE_SIZE);
	// Mark page as allocated
	j = (i - (i % 8)) / 8;
	g_page_map[j] = ((g_page_map[j] << 1) + 1);

//	kprintf(" Page Allocator (page=%u, address=0x0%x) ", i, page);

	return page;
}

void page_free(unsigned long addr)
{
	// @todo implement page free
}

// Extend the kernel's heap
char *sbrk(int size)
{
	//int original_size = 0;
	char *cp;
	unsigned long allocated;
	unsigned long physical_mem;

	int original_size = size;
	size = (size + PAGE_SIZE - 1) & -PAGE_SIZE;

	cp = (char*)kernel_sbrk;

	for (allocated = 0; allocated < size; allocated += PAGE_SIZE)
	{
		physical_mem = (unsigned long) page_malloc();

		if (physical_mem == 0)
		{
			return 0;
		}

		map_page(g_page_directory, (PRIV_USER | PRIV_WRITABLE | PRIV_PRESENT),
			kernel_sbrk, physical_mem);

//		invalidate_page((void*)kernel_sbrk);

		kernel_sbrk += PAGE_SIZE;

//		kprintf("%x ", physical_mem);
	}

//	kprintf("at %u\n", cp);

//	kprintf(" Extend Heap %u (%u bytes, addr=0x0%x) ", original_size, size, kernel_sbrk);
	return cp;
}

// Allocate memory within the kernel's heap
void* malloc(unsigned nbytes)
{
    Header *p, *prevp;
//    static Header *morecore(unsigned);
    unsigned nunits, block_size;

    nunits = (nbytes+sizeof(Header)-1)/sizeof(Header) + 1;

    if ((prevp = freep) == (void *)0)
    {
		base.s.ptr = freep = prevp = &base;
		base.s.size = 0;
    }

    for (p = prevp->s.ptr; ; prevp = p, p = p->s.ptr)
    {
		block_size = p->s.size;

		if (p->s.size >= nunits)
		{
			if (p->s.size == nunits)
			{
				prevp->s.ptr = p->s.ptr;
			}
			else
			{
				p->s.size -= nunits;
				p += p->s.size;
				p->s.size = nunits;
			}

			freep = prevp;

			kprintf(" Alloc (Free %u, Units %u, 0x0%x) ",
				block_size, nunits, (void*) (p+1));

			return (void*) (p+1);
		}

		if ( p == freep)
		{
			if ((p = morecore(nunits)) == (void *)0)
			{
				return (void *)0;
			}
		}
    }
}

// Request more memory from the kernel
Header *morecore(unsigned nu)
{
    char *cp;
    Header *up;

    if (nu < NALLOC)
    {
		nu = NALLOC;
	}

    cp = sbrk(nu * sizeof(Header));

    if (cp == (char *) -1)
    {
		return (void *) 0;
	}

    up = (Header*) cp;
    up->s.size = nu;

    kprintf(" Core (%u bytes, 0x0%x) ", up->s.size, up);

    free((char*)(up+1));

    return freep;
}

// Free a previously allocated block
void free(char *ap)
{
    Header *bp, *p;
    int i = 0;

	// Point to block header
    bp = (Header*) ap - 1;

    kprintf(" Free (Block header 0x0%x) ", bp);

    for (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    {
		if (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
		{
		    break;
		}

		++i;
	}

    if (bp + bp->s.size == p->s.ptr)
    {
		// Join upper
		bp->s.size += p->s.ptr->s.size;
		bp->s.ptr = p->s.ptr->s.ptr;
    }
    else
    {
		bp->s.ptr = p->s.ptr;
	}

	if ( p + p->s.size == bp )
	{
		// Join lower
		p->s.size += bp->s.size;
		p->s.ptr = bp->s.ptr;
	}
	else
	{
		p->s.ptr = bp;
	}

    freep = p;
}

