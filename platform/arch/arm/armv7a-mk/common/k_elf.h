#ifndef K_ELF_FORMAT_H
#define K_ELF_FORMAT_H

/**********************************************************
 32-Bit Data Types
 Name           Size Align Purpose
 Elf32_Addr     4    4     Unsignedprogramaddress
 Elf32_Half     2    2     Unsignedmediuminteger
 Elf32_Off      4    4     Unsignedfileoffset
 Elf32_Sword    4    4     Signedlargeinteger
 Elf32_Word     4    4     Unsignedlargeinteger
 unsigned char  1    1     Unsignedsmallinteger
**********************************************************/
typedef uint32_t    Elf32_Addr;
typedef uint16_t    Elf32_Half;
typedef uint32_t    Elf32_Off;
typedef int32_t     Elf32_Sword;
typedef uint32_t    Elf32_Word;

/**********************************************************
 ELF Header
**********************************************************/
#define EI_NIDENT   16
typedef struct {
    unsigned char e_ident[EI_NIDENT];   /* mark the file as an object file */
    Elf32_Half  e_type;         /* the object file type. */
    Elf32_Half  e_machine;      /* the required architecture for an individual file */
    Elf32_Word  e_version;      /* the object file version */
    Elf32_Addr  e_entry;        /* the virtual address to which the system first transfers control */
    Elf32_Off   e_phoff;        /* the program header table's file offset in bytes */
    Elf32_Off   e_shoff;        /* the section header table's file offset in bytes */
    Elf32_Word  e_flags;        /* processor-specific ﬂags associated with the file */
    Elf32_Half  e_ehsize;       /* the ELF header's size in bytes */
    Elf32_Half  e_phentsize;    /* a program header's size in bytes */
    Elf32_Half  e_phnum;        /* the number of entries in the program header table */
    Elf32_Half  e_shentsize;    /* a section header's size in bytes */
    Elf32_Half  e_shnum;        /* the number of entries in the section header table */
    Elf32_Half  e_shstrndx;     /* the section header table index of string table */
} Elf32_Ehdr;


/*
e_ident[ ] Identification Indexes:
Name       Value  Purpose
EI_MAG0    0      File identification
EI_MAG1    1      File identification
EI_MAG2    2      File identification
EI_MAG3    3      File identification
EI_CLASS   4      File class
EI_DATA    5      Data encoding
EI_VERSION 6      File version
EI_PAD     7      Start of padding bytes
EI_NIDENT  16     Size of e_ident[]
*/
#define EI_MAG0     0
#define EI_MAG1     1
#define EI_MAG2     2
#define EI_MAG3     3
#define EI_CLASS    4
#define EI_DATA     5

/* e_ident, EI_MAG0 to EI_MAG3, hold a "magic number"
Name     Value  Position
ELFMAG0  0x7f   e_ident[EI_MAG0]
ELFMAG1  ’E’    e_ident[EI_MAG1]
ELFMAG2  ’L’    e_ident[EI_MAG2]
ELFMAG3  ’F’    e_ident[EI_MAG3]
*/
#define ELFMAG0     0x7f
#define ELFMAG1     'E'
#define ELFMAG2     'L'
#define ELFMAG3     'F'

/* e_ident, EI_CLASS, identifies the file’s class, or capacity.
Name          Value  Meaning
ELFCLASSNONE  0      Invalid class
ELFCLASS32    1      32-bit objects
ELFCLASS64    2      64-bit objects
*/
#define ELFCLASSNONE    0
#define ELFCLASS32      1
#define ELFCLASS64      2

/* e_ident, EI_DATA, speciﬁes the data encoding
Name          Value  Meaning
ELFDATANONE   0      Invalid data encoding
ELFDATA2LSB   1      Little endian
ELFDATA2MSB   2      Big endian
*/
#define ELFDATANONE     0
#define ELFDATA2LSB     1
#define ELFDATA2MSB     2

/* e_type, identifies the object file type.
Name       Value   Meaning
ET_NONE    0       No file type
ET_REL     1       Relocatable file
ET_EXEC    2       Executable file
ET_DYN     3       Shared object file
ET_CORE    4       Core file
ET_LOPROC  0xff00  Processor-specific
ET_HIPROC  0xffff  Processor-specific
*/
#define ET_NONE     0
#define ET_REL      1
#define ET_EXEC     2
#define ET_DYN      3
#define ET_CORE     4
#define ET_NUM      5
#define ET_LOPROC   0xff00
#define ET_HIPROC   0xffff

/* e_machine, specifies the required architecture for an individual file
Name       Value   Meaning
EM_NONE    0       No machine
EM_ARM     40      ARM
*/
#define EM_NONE     0
#define EM_ARM      40

/* e_flags, ARM-specific e_flags
EF_ARM_ABI_FLOAT_HARD
EF_ARM_ABI_FLOAT_SOFT
TODO: check the Floating point compatibility
*/

/**********************************************************
 Program Header
**********************************************************/
typedef struct {
    Elf32_Word  p_type;
    Elf32_Off   p_offset;       /* the segment's offset in the file image*/
    Elf32_Addr  p_vaddr;        /* the segment's virtual address */
    Elf32_Addr  p_paddr;        /* the segment's physical address */
    Elf32_Word  p_filesz;       /* the segment's size in the file image */
    Elf32_Word  p_memsz;        /* the segment's size in the memory */
    Elf32_Word  p_flags;        /* ﬂags relevant to the segment */
    Elf32_Word  p_align;        /* which the segments are aligned in memory and in the file */
} Elf32_Phdr;

/* p_type, segment types
*/
#define PT_NULL         0           /* unused */
#define PT_LOAD         1           /* loadable segment, described by p_filesz and p_memsz */
#define PT_DYNAMIC      2           /* dynamic linking information */
#define PT_INTERP       3           /* interpreter */
#define PT_NOTE         4           /* auxiliary information */
#define PT_SHLIB        5           /* reserved */
#define PT_PHDR         6           /* the location and size of the program header table */
#define PT_ARM_ARCHEXT  0x70000000  /* Platform architecture compatibility information */
#define PT_ARM_UNWIND   0x70000001  /* Exception unwind tables  */

/**********************************************************
 Section Header
**********************************************************/
typedef struct {
    Elf32_Word  sh_name;        /* the name of the section */
    Elf32_Word  sh_type;        /* the type of the section */
    Elf32_Word  sh_flags;       /* describe miscellaneous attributes */
    Elf32_Addr  sh_addr;        /* the section's virtual address, If the section will appear in the memory */
    Elf32_Off   sh_offset;      /* the section's offset in the file image */
    Elf32_Word  sh_size;        /* the section's size in the memory */
    Elf32_Word  sh_link;        /* the section header table index link */
    Elf32_Word  sh_info;        /* extra information, whose interpretation depends on the section type */
    Elf32_Word  sh_addralign;   /* address alignment constraints */
    Elf32_Word  sh_entsize;     /* the size in bytes of each entry */
} Elf32_Shdr;


#endif /* K_ELF_FORMAT_H */
