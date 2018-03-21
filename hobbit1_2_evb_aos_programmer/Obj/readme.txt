Csky GNU Linker

======================================================================

Section Cross References



======================================================================

Removing Unused input sections from the image. 

	Removing .data(Obj/crt0.o), (0 bytes).
	Removing .data(Obj/main.o), (0 bytes).
	Removing .bss(Obj/main.o), (0 bytes).
	Removing .data(Obj/__rt_entry.o), (0 bytes).
	Removing .bss(Obj/__rt_entry.o), (0 bytes).

5 unused seciton(s) (total 0 bytes) removed from the image.

======================================================================

Image Symbol Table 

	Local Symbols

	Symbol Name                        Value        Type   Size  Section
	__rt_entry.S                       0x00000000     df        0  *ABS*
	main.c                             0x00000000     df        0  *ABS*
	$d                                 0x60008040               0  .text
	$t                                 0x60008040               0  .text
	$d                                 0x60008060               0  .text
	$d                                 0x60008070               0  .text
	$t                                 0x60008070               0  .text
	$t                                 0x60008074               0  .text
	__bss_initialization               0x60008074               0  .text
	__rom_decompression                0x6000808a               0  .text
	$d                                 0x60008100               0  .text
	Stack_Mem                          0x60008180               0  .bss

	Global Symbols

	Symbol Name                        Value        Type   Size  Section
	__e_ram_bss_2                      0x00000000    w          0  *UND*
	__e_ram_bss_3                      0x00000000    w          0  *UND*
	__e_ram_bss_4                      0x00000000    w          0  *UND*
	__e_ram_bss_5                      0x00000000    w          0  *UND*
	__e_ram_data_1                     0x00000000    w          0  *UND*
	__e_ram_data_2                     0x00000000    w          0  *UND*
	__e_ram_data_3                     0x00000000    w          0  *UND*
	__e_ram_data_4                     0x00000000    w          0  *UND*
	__e_ram_data_5                     0x00000000    w          0  *UND*
	__e_rom                            0x00000000    w          0  *UND*
	__s_ram_bss_2                      0x00000000    w          0  *UND*
	__s_ram_bss_3                      0x00000000    w          0  *UND*
	__s_ram_bss_4                      0x00000000    w          0  *UND*
	__s_ram_bss_5                      0x00000000    w          0  *UND*
	__s_ram_data_1                     0x00000000    w          0  *UND*
	__s_ram_data_2                     0x00000000    w          0  *UND*
	__s_ram_data_3                     0x00000000    w          0  *UND*
	__s_ram_data_4                     0x00000000    w          0  *UND*
	__s_ram_data_5                     0x00000000    w          0  *UND*
	ckcpu_vsr_table                    0x60008000               0  .text
	__stext                            0x60008040               0  .text
	Reset_Handler                      0x60008040    w          0  .text
	__bkpt_label                       0x60008056               0  .text
	Default_handler                    0x6000805a               0  .text
	Access_Error_Handler               0x6000805a    w          0  .text
	Auto_FINT_Handler                  0x6000805a    w          0  .text
	Auto_INT_Handler                   0x6000805a    w          0  .text
	Breakpoint_Exception_Handler       0x6000805a    w          0  .text
	Divided_By_Zero_Handler            0x6000805a    w          0  .text
	Idly4_Error_Handler                0x6000805a    w          0  .text
	Illegal_Handler                    0x6000805a    w          0  .text
	Misaligned_Access_Handler          0x6000805a    w          0  .text
	Privlege_Violation_Handler         0x6000805a    w          0  .text
	Reserved_FP_Handler                0x6000805a    w          0  .text
	Reserved_HAI_Handler               0x6000805a    w          0  .text
	TLB_Data_Empty_Handler             0x6000805a    w          0  .text
	TLB_Ins_Empty_Handler              0x6000805a    w          0  .text
	Trace_Exection_Handler             0x6000805a    w          0  .text
	Unrecoverable_Error_Handler        0x6000805a    w          0  .text
	main                               0x60008070      F        4  .text
	__main                             0x6000809c    w          0  .text
	__etext                            0x60008160               0  .text
	__data_start                       0x60008160               0  .data
	__erodata                          0x60008160               0  .data
	__sdata                            0x60008160               0  .data
	__srodata                          0x60008160               0  .data
	data_start                         0x60008160               0  .data
	__edata                            0x60008180               0  .data
	__s_ram_bss_1                      0x60008180               0  .bss
	__sbss                             0x60008180               0  .bss
	__e_ram_bss_1                      0x60008280               0  .bss
	__end                              0x60008280               0  .bss
	__initial_sp                       0x60008280               0  .bss
	end                                0x60008280               0  .bss

	(w:Weak    d:Deubg    F:Function    f:File name    O:Zero)


======================================================================

Memory Map of the image

    Image Entry point : 0x60008040

    Region MEM (Base: 0x60008000, Size: 0x00000280, Max: 0x00008000)

    Base Addr    Size         Type   Attr      Idx   Section Name       Object
    0x60008040   0x00000030   Code   RO         16   .text          	Obj/crt0.o
    0x60008070   0x00000004   Code   RO         21   .text          	Obj/main.o
    0x60008074   0x000000e4   Code   RO         32   .text          	Obj/__rt_entry.o
    0x60008158   0x00000008   PAD
    0x60008160   0x00000004   LD_GEN RW
    0x60008164   0x0000001c   PAD
    0x60008180   0x00000100   Zero   RW         18   .bss           	Obj/crt0.o

    Region *default* (Base: 0x00000000, Size: 0x00000059, Max: 0xffffffff)

    Base Addr    Size         Type   Attr      Idx   Section Name       Object


======================================================================

Image component sizes

       Code    RO Data    RW Data    ZI Data      Debug   Object Name

          0          0          0          0          0   linker stubs
         48          0          0        256          0   Obj/crt0.o
          4          0          0          0        449   Obj/main.o
        228          0          0          0          0   Obj/__rt_entry.o
	------------------------------------------------------------
        280          0          0        256        449   Object Totals
          8          0          0          0          0   Pad
          0          0          0          0          0   LD_GEN

	------------------------------------------------------------

======================================================================


       Code    RO Data    RW Data    ZI Data      Debug
        288          0          0        256        449   Grand Totals
        288          0          0        256        449   Elf Image Totals
        288          0          0          0          0   ROM Totals

======================================================================

Total	RO	Size (Code + RO Data)				288 (  0.28kB)
Total	RW	Size (RW Data + ZI Data)			256 (  0.25kB)
Total	ROM	Size (Code + RO Data + RW Data)		288 (  0.28kB)

======================================================================
