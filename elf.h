#ifndef _ELF_H_
#define _ELF_H_


#define EI_NIDENT 16

// e_ident
#define EI_MAG0			0
#define EI_MAG1			1
#define EI_MAG2			2
#define EI_MAG3			3
#define EI_CLASS  		4
#define EI_DATA	  		5

#define ELFCLASS32		1
#define ELFDATA2LSB		1

// e_type
#define ET_NONE		0
#define ET_REL 		1
#define ET_EXEC		2
#define ET_DYN 		3
#define ET_CORE		4
#define ET_LOPROC	0xff00
#define ET_HIPROC	0xffff

// e_machine
#define EM_386		3

// e_version
#define EV_CURRENT	1

typedef unsigned int Elf32_Addr;
typedef unsigned short Elf32_Half;
typedef unsigned int Elf32_Off;
typedef int Elf32_Sword;
typedef unsigned int Elf32_Word;

typedef struct {

	unsigned char e_ident[EI_NIDENT]; // elf identity marker
	Elf32_Half e_type ;				  // object file type
	Elf32_Half e_machine ;			  // architecture
	Elf32_Word e_version ;			  // object version
	Elf32_Addr e_entry ;			  // virtual address of entry point
	Elf32_Off  e_phoff ;			  // program hdr table offset in bytes
	Elf32_Off  e_shoff ;			  // section hdr table offset in bytes
	Elf32_Word e_flags ;			  // processor specific flags, 0 for ia32
	Elf32_Half e_ehsize ;			  // elf header size in bytes
	Elf32_Half e_phentsize ;		  // size in bytes of one entry in program hdr table
	Elf32_Half e_phnum;				  // number of entries in program hdr table
	Elf32_Half e_shentsize ; 		  // size in bytes of one entry in section hdr table
	Elf32_Half e_shnum;				  // number of entries in section hdr table
	Elf32_Half e_shstrndx ;			  // section hdr table index of section name string table

} Elf32_Ehdr ;

typedef struct {
	Elf32_Word sh_name;			// name
	Elf32_Word sh_type;			// contents and semantics
	Elf32_Word sh_flags;		// miscellaneous attributes
	Elf32_Addr sh_addr;			// section's address, if the section will appear in the memory image of a process
	Elf32_Off sh_offset;		// byte offset from the beginning of the file to the first byte in the section
	Elf32_Word sh_size;
	Elf32_Word sh_link;
	Elf32_Word sh_info;
	Elf32_Word sh_addralign;
	Elf32_Word sh_entsize ;

} Elf32_Shdr ;

#endif


void debug(Elf32_Ehdr *hdr);