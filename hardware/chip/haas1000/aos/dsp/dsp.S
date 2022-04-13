/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifdef CHIP_HAAS1000

#define TO_STRING_A(s)                  # s
#define TO_STRING(s)                    TO_STRING_A(s)

.globl __dsp_code_start
.globl __dsp_code_end
.section .rodata_dsp_code, "a", %progbits
.balign 4

__dsp_code_start:
	.incbin TO_STRING(../../hardware/chip/haas1000/aos/dsp/dsp.bin.lzma)
__dsp_code_end:

#endif
