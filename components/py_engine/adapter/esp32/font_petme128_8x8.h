/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef MICROPY_INCLUDED_STM32_FONT_PETME128_8X8_H
#define MICROPY_INCLUDED_STM32_FONT_PETME128_8X8_H

static const uint8_t font_petme128_8x8[] = {
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // 32=
    0x00,0x00,0x00,0x4f,0x4f,0x00,0x00,0x00, // 33=!
    0x00,0x07,0x07,0x00,0x00,0x07,0x07,0x00, // 34="
    0x14,0x7f,0x7f,0x14,0x14,0x7f,0x7f,0x14, // 35=#
    0x00,0x24,0x2e,0x6b,0x6b,0x3a,0x12,0x00, // 36=$
    0x00,0x63,0x33,0x18,0x0c,0x66,0x63,0x00, // 37=%
    0x00,0x32,0x7f,0x4d,0x4d,0x77,0x72,0x50, // 38=&
    0x00,0x00,0x00,0x04,0x06,0x03,0x01,0x00, // 39='
    0x00,0x00,0x1c,0x3e,0x63,0x41,0x00,0x00, // 40=(
    0x00,0x00,0x41,0x63,0x3e,0x1c,0x00,0x00, // 41=)
    0x08,0x2a,0x3e,0x1c,0x1c,0x3e,0x2a,0x08, // 42=*
    0x00,0x08,0x08,0x3e,0x3e,0x08,0x08,0x00, // 43=+
    0x00,0x00,0x80,0xe0,0x60,0x00,0x00,0x00, // 44=,
    0x00,0x08,0x08,0x08,0x08,0x08,0x08,0x00, // 45=-
    0x00,0x00,0x00,0x60,0x60,0x00,0x00,0x00, // 46=.
    0x00,0x40,0x60,0x30,0x18,0x0c,0x06,0x02, // 47=/
    0x00,0x3e,0x7f,0x49,0x45,0x7f,0x3e,0x00, // 48=0
    0x00,0x40,0x44,0x7f,0x7f,0x40,0x40,0x00, // 49=1
    0x00,0x62,0x73,0x51,0x49,0x4f,0x46,0x00, // 50=2
    0x00,0x22,0x63,0x49,0x49,0x7f,0x36,0x00, // 51=3
    0x00,0x18,0x18,0x14,0x16,0x7f,0x7f,0x10, // 52=4
    0x00,0x27,0x67,0x45,0x45,0x7d,0x39,0x00, // 53=5
    0x00,0x3e,0x7f,0x49,0x49,0x7b,0x32,0x00, // 54=6
    0x00,0x03,0x03,0x79,0x7d,0x07,0x03,0x00, // 55=7
    0x00,0x36,0x7f,0x49,0x49,0x7f,0x36,0x00, // 56=8
    0x00,0x26,0x6f,0x49,0x49,0x7f,0x3e,0x00, // 57=9
    0x00,0x00,0x00,0x24,0x24,0x00,0x00,0x00, // 58=:
    0x00,0x00,0x80,0xe4,0x64,0x00,0x00,0x00, // 59=;
    0x00,0x08,0x1c,0x36,0x63,0x41,0x41,0x00, // 60=<
    0x00,0x14,0x14,0x14,0x14,0x14,0x14,0x00, // 61==
    0x00,0x41,0x41,0x63,0x36,0x1c,0x08,0x00, // 62=>
    0x00,0x02,0x03,0x51,0x59,0x0f,0x06,0x00, // 63=?
    0x00,0x3e,0x7f,0x41,0x4d,0x4f,0x2e,0x00, // 64=@
    0x00,0x7c,0x7e,0x0b,0x0b,0x7e,0x7c,0x00, // 65=A
    0x00,0x7f,0x7f,0x49,0x49,0x7f,0x36,0x00, // 66=B
    0x00,0x3e,0x7f,0x41,0x41,0x63,0x22,0x00, // 67=C
    0x00,0x7f,0x7f,0x41,0x63,0x3e,0x1c,0x00, // 68=D
    0x00,0x7f,0x7f,0x49,0x49,0x41,0x41,0x00, // 69=E
    0x00,0x7f,0x7f,0x09,0x09,0x01,0x01,0x00, // 70=F
    0x00,0x3e,0x7f,0x41,0x49,0x7b,0x3a,0x00, // 71=G
    0x00,0x7f,0x7f,0x08,0x08,0x7f,0x7f,0x00, // 72=H
    0x00,0x00,0x41,0x7f,0x7f,0x41,0x00,0x00, // 73=I
    0x00,0x20,0x60,0x41,0x7f,0x3f,0x01,0x00, // 74=J
    0x00,0x7f,0x7f,0x1c,0x36,0x63,0x41,0x00, // 75=K
    0x00,0x7f,0x7f,0x40,0x40,0x40,0x40,0x00, // 76=L
    0x00,0x7f,0x7f,0x06,0x0c,0x06,0x7f,0x7f, // 77=M
    0x00,0x7f,0x7f,0x0e,0x1c,0x7f,0x7f,0x00, // 78=N
    0x00,0x3e,0x7f,0x41,0x41,0x7f,0x3e,0x00, // 79=O
    0x00,0x7f,0x7f,0x09,0x09,0x0f,0x06,0x00, // 80=P
    0x00,0x1e,0x3f,0x21,0x61,0x7f,0x5e,0x00, // 81=Q
    0x00,0x7f,0x7f,0x19,0x39,0x6f,0x46,0x00, // 82=R
    0x00,0x26,0x6f,0x49,0x49,0x7b,0x32,0x00, // 83=S
    0x00,0x01,0x01,0x7f,0x7f,0x01,0x01,0x00, // 84=T
    0x00,0x3f,0x7f,0x40,0x40,0x7f,0x3f,0x00, // 85=U
    0x00,0x1f,0x3f,0x60,0x60,0x3f,0x1f,0x00, // 86=V
    0x00,0x7f,0x7f,0x30,0x18,0x30,0x7f,0x7f, // 87=W
    0x00,0x63,0x77,0x1c,0x1c,0x77,0x63,0x00, // 88=X
    0x00,0x07,0x0f,0x78,0x78,0x0f,0x07,0x00, // 89=Y
    0x00,0x61,0x71,0x59,0x4d,0x47,0x43,0x00, // 90=Z
    0x00,0x00,0x7f,0x7f,0x41,0x41,0x00,0x00, // 91=[
    0x00,0x02,0x06,0x0c,0x18,0x30,0x60,0x40, // 92='\'
    0x00,0x00,0x41,0x41,0x7f,0x7f,0x00,0x00, // 93=]
    0x00,0x08,0x0c,0x06,0x06,0x0c,0x08,0x00, // 94=^
    0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0, // 95=_
    0x00,0x00,0x01,0x03,0x06,0x04,0x00,0x00, // 96=`
    0x00,0x20,0x74,0x54,0x54,0x7c,0x78,0x00, // 97=a
    0x00,0x7f,0x7f,0x44,0x44,0x7c,0x38,0x00, // 98=b
    0x00,0x38,0x7c,0x44,0x44,0x6c,0x28,0x00, // 99=c
    0x00,0x38,0x7c,0x44,0x44,0x7f,0x7f,0x00, // 100=d
    0x00,0x38,0x7c,0x54,0x54,0x5c,0x58,0x00, // 101=e
    0x00,0x08,0x7e,0x7f,0x09,0x03,0x02,0x00, // 102=f
    0x00,0x98,0xbc,0xa4,0xa4,0xfc,0x7c,0x00, // 103=g
    0x00,0x7f,0x7f,0x04,0x04,0x7c,0x78,0x00, // 104=h
    0x00,0x00,0x00,0x7d,0x7d,0x00,0x00,0x00, // 105=i
    0x00,0x40,0xc0,0x80,0x80,0xfd,0x7d,0x00, // 106=j
    0x00,0x7f,0x7f,0x30,0x38,0x6c,0x44,0x00, // 107=k
    0x00,0x00,0x41,0x7f,0x7f,0x40,0x00,0x00, // 108=l
    0x00,0x7c,0x7c,0x18,0x30,0x18,0x7c,0x7c, // 109=m
    0x00,0x7c,0x7c,0x04,0x04,0x7c,0x78,0x00, // 110=n
    0x00,0x38,0x7c,0x44,0x44,0x7c,0x38,0x00, // 111=o
    0x00,0xfc,0xfc,0x24,0x24,0x3c,0x18,0x00, // 112=p
    0x00,0x18,0x3c,0x24,0x24,0xfc,0xfc,0x00, // 113=q
    0x00,0x7c,0x7c,0x04,0x04,0x0c,0x08,0x00, // 114=r
    0x00,0x48,0x5c,0x54,0x54,0x74,0x20,0x00, // 115=s
    0x04,0x04,0x3f,0x7f,0x44,0x64,0x20,0x00, // 116=t
    0x00,0x3c,0x7c,0x40,0x40,0x7c,0x3c,0x00, // 117=u
    0x00,0x1c,0x3c,0x60,0x60,0x3c,0x1c,0x00, // 118=v
    0x00,0x1c,0x7c,0x30,0x18,0x30,0x7c,0x1c, // 119=w
    0x00,0x44,0x6c,0x38,0x38,0x6c,0x44,0x00, // 120=x
    0x00,0x9c,0xbc,0xa0,0xa0,0xfc,0x7c,0x00, // 121=y
    0x00,0x44,0x64,0x74,0x5c,0x4c,0x44,0x00, // 122=z
    0x00,0x08,0x08,0x3e,0x77,0x41,0x41,0x00, // 123={
    0x00,0x00,0x00,0xff,0xff,0x00,0x00,0x00, // 124=|
    0x00,0x41,0x41,0x77,0x3e,0x08,0x08,0x00, // 125=}
    0x00,0x02,0x03,0x01,0x03,0x02,0x03,0x01, // 126=~
    0xaa,0x55,0xaa,0x55,0xaa,0x55,0xaa,0x55, // 127
};

#endif // MICROPY_INCLUDED_STM32_FONT_PETME128_8X8_H
