#include "attrs.h"

#define NDS32_FMEMCPY(dst, src, size) \
	__asm__ __volatile__( \
        "move    $r3, %2             \n\t" \
        "move    $r1, %0             \n\t" \
        "move    $r2, %1             \n\t" \
		"1:\n\t" \
		"lwi.bi  $r0, [$r2], 4       \n\t" \
		"swi.bi  $r0, [$r1], 4       \n\t" \
		"addi    $r3, $r3, -4        \n\t" \
		"bgtz    $r3, 1b             \n\t" \
		::"r"(dst),"r"(src),"r"(size):"$r0", "$r1","$r2","$r3")

#ifdef XIP_MODE
/* This must be a leaf function, no child function */
void _nds32_init_mem(void) __attribute__((no_prologue, optimize("Os"))) ATTRIBUTE_SECTION_FBOOT;

void _nds32_init_mem()
{
    int size;
    ////// for DLM init.
	// do Low Level Init
	extern char __data_lmastart;
	extern char __data_start;
	extern char _edata;

	/* Copy data section to RAM */
//asm("li     $r0, 0x00100009\n"
    asm("mtsr   %0, $mr7"
        :
        : "r"(0x00100009)
        :);
	size = &_edata - &__data_start;
    NDS32_FMEMCPY(&__data_start, &__data_lmastart, size);

}

/* This must be a leaf function, no child function */
void _move_prog_in_sram(void) __attribute__((no_prologue, optimize("Os"))) ATTRIBUTE_SECTION_FBOOT;

void _move_prog_in_sram() {
	extern char __flash_psram_lmastart;
	extern char __flash_psram_start;
	extern char _eflash;
    int size = &_eflash - &__flash_psram_start;
    NDS32_FMEMCPY(&__flash_psram_start, &__flash_psram_lmastart, size);
}

void _move_fast_boot_code(void) __attribute__((no_prologue, optimize("Os"))) __attribute__((section(".magic_boot_c")));
void _move_fast_boot_code(void) {
	extern char __flash_lmastart;
	extern char __flash_start;
	extern char _fast_code_end;
    int size = &_fast_code_end - &__flash_start;
    NDS32_FMEMCPY(&__flash_start, &__flash_lmastart, size);
}
#endif

#if defined(FEATURE_RETENTION_BOOT)
/* This must be a leaf function, no child function */
void _move_retention_sram(void) __attribute__((no_prologue, optimize("Os"))) ATTRIBUTE_SECTION_FBOOT;
void _move_retention_sram() {
	extern char __retention_code_lmastart;
	extern char __retention_code_start;
	extern char __retention_code_end;
    int size = &__retention_code_end - &__retention_code_start;
    NDS32_FMEMCPY(&__retention_code_start, &__retention_code_lmastart, size);
}
#endif

