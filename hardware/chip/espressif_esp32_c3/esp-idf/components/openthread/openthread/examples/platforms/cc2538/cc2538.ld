/*
 *  Copyright (c) 2016, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   GCC linker script for CC2538.
 */

_512k_bytes             = (512*1024);
_256k_bytes             = (256*1024);
_128k_bytes             = (128*1024);
_FLASH_page_size        = 2048;

/*
 * Change for your chip, default is 512k chips
 */
_FLASH_size_bytes       = _512k_bytes;
_FLASH_n_pages          = (_FLASH_size_bytes / _FLASH_page_size);
/* reduce the usable size by: the CCA + settings Page A & B, total 3 pages */
_FLASH_usable_size      = (_FLASH_size_bytes - (3 * _FLASH_page_size));
_FLASH_start            = 0x00200000;
_FLASH_end              = (_FLASH_start + _FLASH_size_bytes);

/*
 * The CCA (Customer Configuration Area) is always the last page.
 * See: http://www.ti.com/lit/ug/swru319c/swru319c.pdf
 * table 8-2 for more details.
 */
_FLASH_cca_page         = (_FLASH_end - (1 * _FLASH_page_size));

/*
 * OpenThread NV storage goes in the settings page.
 * OpenThread requires at least 2 adjacent pages, call them A and B.
 */
_FLASH_settings_pageB   = (_FLASH_end - (2 * _FLASH_page_size));
_FLASH_settings_pageA   = (_FLASH_end - (3 * _FLASH_page_size));

MEMORY
{
  /* would like to use SYMBOLS (from above)here but we cannot
   * GCC version 4.9 does not support symbolic expressions here.
   * But later versions do support the feature.
   */
  FLASH (rx) :     ORIGIN = 0x00200000,     LENGTH = 0x0007c000
  FLASH_CCA (rx) : ORIGIN = 0x0027FFD4,     LENGTH = 0x2c
  SRAM (rwx) :     ORIGIN = 0x20000000,     LENGTH = 32K
}
/*
 * To safty check what would have been the SYMBOL values
 * we use these ASSERTS to verify things are still good.
 */
ASSERT( _FLASH_start       == 0x00200000, "invalid flash start address for cc2538")
ASSERT( _FLASH_cca_page    == 0x0027f800, "invalid cca start address for cc2538")
ASSERT( _FLASH_usable_size == 0x0007e800, "Invalid usable size for this config")



ENTRY(flash_cca_lock_page)
SECTIONS
{
    .text : ALIGN(4)
    {
        _text = .;
        *(.vectors)
        *(.text*)
        *(.rodata*)
        KEEP(*(.init))
        KEEP(*(.fini))
        _etext = .;
    } > FLASH= 0

    .init_array :
    {
        _init_array = .;
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array*))
        _einit_array = .;
    } > FLASH

    .ARM.exidx : ALIGN(4)
    {
        *(.ARM.exidx*)
    } > FLASH

    .data : ALIGN(4)
    {
        _data = .;
        *(.data*)
        _edata = .;
    } > SRAM AT > FLASH
    _ldata = LOADADDR(.data);

    .bss : ALIGN(4)
    {
        _bss = .;
        *(.bss*)
        *(COMMON)
        _ebss = .;
    } > SRAM

    _heap = .;
    end = .;

    .stack : ALIGN(4)
    {
        *(.stack)
    } > SRAM

    .flashcca :
    {
        KEEP(*(.flash_cca))
    } > FLASH_CCA
}
