#include "multiboot.h"
#include "common.h"
#include "console.h"

void print_mbi(struct multiboot_info *mbi)
{
  /* Print out the flags.  */
  kprintf ("flags = 0x%x\n", (unsigned) mbi->flags);

  /* Are mem_* valid?  */
  if (check_flag (mbi->flags, 0))
    kprintf ("mem_lower = %uKB, mem_upper = %uKB\n",
	    (unsigned) mbi->mem_lower, (unsigned) mbi->mem_upper);

  /* Is boot_device valid?  */
  if (check_flag (mbi->flags, 1))
    kprintf ("boot_device = 0x%x\n", (unsigned) mbi->boot_device);

  /* Is the command line passed?  */
  if (check_flag (mbi->flags, 2))
    kprintf ("cmdline = %s\n", (char *) mbi->cmdline);

  /* Are mods_* valid?  */
  if (check_flag (mbi->flags, 3))
	{
	  module_t *mod;
	  int i;

	  kprintf ("mods_count = %d, mods_addr = 0x%x\n",
		  (int) mbi->mods_count, (int) mbi->mods_addr);
	  for (i = 0, mod = (module_t *) mbi->mods_addr;
	   i < mbi->mods_count;
	   i++, mod += sizeof (module_t))
		kprintf (" mod_start = 0x%x, mod_end = 0x%x, string = %s\n",
		(unsigned) mod->mod_start,
		(unsigned) mod->mod_end,
		(char *) mod->string);
	}

  /* Bits 4 and 5 are mutually exclusive!  */
  if (check_flag (mbi->flags, 4) && check_flag (mbi->flags, 5))
    {
      kprintf ("Both bits 4 and 5 are set.\n");
      return;
    }

  /* Is the symbol table of a.out valid?  */
  if (check_flag (mbi->flags, 4))
    {
      aout_symbol_table_t *aout_sym = &(mbi->u.aout_sym);

      kprintf ("aout_symbol_table: tabsize = 0x%0x, "
	      "strsize = 0x%x, addr = 0x%x\n",
	      (unsigned) aout_sym->tabsize,
	      (unsigned) aout_sym->strsize,
	      (unsigned) aout_sym->addr);
    }

  /* Is the section header table of ELF valid?  */
  if (check_flag (mbi->flags, 5))
    {
      int m;
	  elf32_shdr_t *elf_shdr;
      elf_section_header_table_t *elf_sec = &(mbi->u.elf_sec);

      kprintf ("elf_sec: num = %u, size = 0x%x,"
	      " addr = 0x%x, shndx = 0x%x\n",
	      (unsigned) elf_sec->num, (unsigned) elf_sec->size,
	      (unsigned) elf_sec->addr, (unsigned) elf_sec->shndx);



      for (m = 0; m < elf_sec->num; m++)
	  {
    	   elf_shdr = (elf32_shdr_t *)
    	   	((unsigned long) elf_sec->addr + (elf_sec->size*m));


	   kprintf("0x0%x 0x0%x 0x0%x 0x0%x \n",

			elf_shdr->sh_name,
			elf_shdr->sh_type,
			elf_shdr->sh_addr,
			elf_shdr->sh_size

	   	);


		}

    }

  /* Are mmap_* valid?  */
  if (check_flag (mbi->flags, 6))
    {
      memory_map_t *mmap;

      kprintf ("mmap_addr = 0x%x, mmap_length = 0x%x\n",
	      (unsigned) mbi->mmap_addr, (unsigned) mbi->mmap_length);
      for (mmap = (memory_map_t *) mbi->mmap_addr;
	   (unsigned long) mmap < mbi->mmap_addr + mbi->mmap_length;
	   mmap = (memory_map_t *) ((unsigned long) mmap
				    + mmap->size + sizeof (mmap->size)))
				    {
		/* mmap->type=1 indicates ordinary RAM, all other values reserved */
		if (mmap->type == 1)
	kprintf (" size = 0x%x, base_addr = 0x%x%x,"
		" length = 0x%x%x, type = 0x%x\n",
		(unsigned) mmap->size,
		(unsigned) mmap->base_addr_high,
		(unsigned) mmap->base_addr_low,
		(unsigned) mmap->length_high,
		(unsigned) mmap->length_low,
		(unsigned) mmap->type);
		}
    }
    else
    {
		kprintf("No memory map present.\n");
	}
}
