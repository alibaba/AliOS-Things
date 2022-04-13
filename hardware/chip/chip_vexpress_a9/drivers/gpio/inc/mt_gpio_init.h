/*
 * Copyright (c) 2019 MediaTek Inc.
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
#ifndef __MT_GPIO_INIT_H
#define __MT_GPIO_INIT_H

struct val_regs {
    unsigned int val;
    unsigned int set;
    unsigned int rst;
    unsigned int _align1;
};
/*----------------------------------------------------------------------------*/
struct  gpio_regs {
    struct val_regs din[4]; 	   /*0x0000 ~ 0x003F: 4*16 bytes*/
    uint8_t  rsv02[96];	   /*0x0040 ~ 0x009f: 6*16 bytes*/
    struct val_regs dout[4];		/*0x00A0 ~ 0x00DF: 4*16 bytes*/
    uint8_t	rsv01[96];		/*0x00E0 ~ 0x013f: 6*16 bytes*/
    struct val_regs dir[4];		/*0x0140 ~ 0X017F: 4*16 bytes*/
    uint8_t 	rsv00[96];		/*0x0180 ~ 0x01dF: 6*16 bytes*/
    struct val_regs mode[12];		/*0x01e0 ~ 0x029F: 12*16 bytes*/
    uint8_t	rsv03[368];		/*0x02A0 ~ 0x040F: 23*16 bytes*/
    struct val_regs ies[2];		/*0x0410 ~ 0x042F: 2*16 bytes*/
    uint8_t	rsv06[64];		/*0x0430 ~ 0x046F: 4*16 bytes*/
    struct val_regs smt[2];		/*0x0470 ~ 0x048F: 2*16 bytes*/
    uint8_t	rsv07[640];		/*0x0490 ~ 0x070F: 40*16  bytes*/
    struct val_regs drv_mode[6]; 	   /*0x0710 ~ 0x076F:  6*16 bytes*/
    uint8_t  rsv010[240]; 	   /*0x0770 ~ 0x085F: 15*16 bytes*/
    struct val_regs pullen[4];		/*0x0860 ~ 0x089F: 4*16 bytes*/
    uint8_t	rsv04[96];		/*0x08a0 ~ 0x08ff: 6*16 bytes*/
    struct val_regs pullsel[4];		/*0x0900 ~ 0x093F: 4*16 bytes*/
};

#endif
