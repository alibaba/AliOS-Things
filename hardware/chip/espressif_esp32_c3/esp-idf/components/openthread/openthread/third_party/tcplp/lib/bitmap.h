/*
 *  Copyright (c) 2018, Sam Kumar <samkumar@cs.berkeley.edu>
 *  Copyright (c) 2018, University of California, Berkeley
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

#ifndef BITMAP_H_
#define BITMAP_H_

#include <stdint.h>
#include <stdlib.h>

/* Computes the number of bytes required to store the specified number of bits. */
#define BITS_TO_BYTES(bits) (((bits) >> 3) + (((bits) & 0x7) ? 1 : 0))

/* Initializes a bitmap to all zeros. */
void bmp_init(uint8_t* buf, size_t numbytes);

/* Sets the specified range of bits. START is the index
   of the first bit to be set. LEN is the number of bits
   to be set. */
void bmp_setrange(uint8_t* buf, size_t start, size_t len);

/* Clears the specified range of bits. START is the index
   of the first bit to be cleared. LEN is the number of bits
   to be cleared. */
void bmp_clrrange(uint8_t* buf, size_t start, size_t len);

/* Counts the number of set bits in BUF starting at START. BUF has length
   BUFLEN, in bytes. Counts the number of set bits until it either (1) finds
   a bit that isn't set, in which case it returns the number of set bits,
   (2) it has counted at least LIMIT bits, in which case it returns a number
   greater than or equal to LIMIT, or (3) reaches the end of the buffer, in
   which case it returns exactly the number of set bits it found. */
size_t bmp_countset(uint8_t* buf, size_t buflen, size_t start, size_t limit);

/* Returns 1 if the bitmap is all zeros, and 0 otherwise. */
int bmp_isempty(uint8_t* buf, size_t buflen);

/* Prints out the bitmap in hexadecimal (used for debugging). */
void bmp_print(uint8_t* buf, size_t buflen);

#endif
