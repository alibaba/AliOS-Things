/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _IO_H
#define _IO_H

#include <stddef.h>
#include <stdint.h>

#define BIT(nr)                 (1UL << (nr))

#define get_bvalue(addr)    (*((volatile unsigned char  *)(addr)))
#define put_bvalue(addr, v) (*((volatile unsigned char  *)(addr)) = (unsigned char)(v))
#define get_hvalue(addr)    (*((volatile unsigned short *)(addr)))
#define put_hvalue(addr, v) (*((volatile unsigned short *)(addr)) = (unsigned short)(v))
#define get_wvalue(addr)    (*((volatile unsigned long  *)(addr)))
#define put_wvalue(addr, v) (*((volatile unsigned long  *)(addr)) = (unsigned long)(v))

#define set_bit(addr, v)    (*((volatile unsigned char  *)(addr)) |=  (unsigned char)(v))
#define clr_bit(addr, v)    (*((volatile unsigned char  *)(addr)) &= ~(unsigned char)(v))
#define set_bbit(addr, v)   (*((volatile unsigned char  *)(addr)) |=  (unsigned char)(v))
#define clr_bbit(addr, v)   (*((volatile unsigned char  *)(addr)) &= ~(unsigned char)(v))
#define set_hbit(addr, v)   (*((volatile unsigned short *)(addr)) |=  (unsigned short)(v))
#define clr_hbit(addr, v)   (*((volatile unsigned short *)(addr)) &= ~(unsigned short)(v))
#define set_wbit(addr, v)   (*((volatile unsigned long  *)(addr)) |=  (unsigned long)(v))
#define clr_wbit(addr, v)   (*((volatile unsigned long  *)(addr)) &= ~(unsigned long)(v))

#define readb(addr)         (*((volatile unsigned char  *)(addr)))
#define readw(addr)         (*((volatile unsigned short *)(addr)))
#define readl(addr)         (*((volatile unsigned long  *)(addr)))
#define writeb(v, addr)     (*((volatile unsigned char  *)(addr)) = (unsigned char)(v))
#define writew(v, addr)     (*((volatile unsigned short *)(addr)) = (unsigned short)(v))
#define writel(v, addr)     (*((volatile unsigned long  *)(addr)) = (unsigned long)(v))

#define ARRAY_SIZE(n) (sizeof(n)/sizeof(n[0]))

/**
 * sr32 - clear & set a value in a bit range for a 32 bit address
 */
static __inline void sr32(unsigned int addr, unsigned int start_bit, unsigned int num_bits, unsigned int value)
{
    unsigned int tmp, msk = (1 << num_bits) - 1;
    tmp = readl(addr) & ~(msk << start_bit);
    tmp |= value << start_bit;
    writel(tmp, addr);
}

#endif /* _IO_H */
