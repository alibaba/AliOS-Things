/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef _MEDIATEK_CLK_PD_BASIC_H_
#define _MEDIATEK_CLK_PD_BASIC_H_

#include <freertos_to_aos.h>

#define dsb() __asm__ __volatile__ ("dsb" : : : "memory")
#define isb() __asm__ __volatile__ ("isb" : : : "memory")

#define mb()    \
    do {    \
        dsb();    \
    } while (0)

#define DRV_Reg32(addr)             (*(volatile unsigned int *)(addr))
#define DRV_WriteReg32(addr, data)    \
    do {    \
        ((*(volatile unsigned int *)(addr)) = (unsigned int)data);    \
        mb();    \
    } while(0)

#endif /* _MEDIATEK_CLK_PD_BASIC_H_ */
