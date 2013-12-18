#include "elf.h"

void debug(Elf32_Ehdr *hdr)
{
	if ( hdr->e_ident[EI_CLASS] == ELFCLASS32 && hdr->e_ident[EI_DATA] == ELFDATA2LSB)
	{

	}
}