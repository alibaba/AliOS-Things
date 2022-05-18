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

/* BITMAP */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bitmap.h"

void bmp_init(uint8_t* buf, size_t numbytes) {
    memset(buf, 0x00, numbytes);
}

#define _bmp_getrangeinfo(buf, start, len, first_bit_id, first_byte_ptr, last_bit_id, last_byte_ptr) \
    do { \
        first_bit_id = (start & 0x7); \
        first_byte_ptr = buf + (start >> 3); \
        last_bit_id = (len & 0x7) + first_bit_id; \
        last_byte_ptr = first_byte_ptr + (len >> 3) + (last_bit_id >> 3); \
        last_bit_id &= 0x7; \
    } while (0)

void bmp_setrange(uint8_t* buf, size_t start, size_t len) {
    uint8_t first_bit_id;
    uint8_t* first_byte_set;
    uint8_t last_bit_id;
    uint8_t* last_byte_set;
    uint8_t first_byte_mask, last_byte_mask;
    _bmp_getrangeinfo(buf, start, len, first_bit_id, first_byte_set,
                      last_bit_id, last_byte_set);

    first_byte_mask = (uint8_t) (0xFF >> first_bit_id);
    last_byte_mask = (uint8_t) (0xFF << (8 - last_bit_id));

    /* Set the bits. */
    if (first_byte_set == last_byte_set) {
        *first_byte_set |= (first_byte_mask & last_byte_mask);
    } else {
        *first_byte_set |= first_byte_mask;
        memset(first_byte_set + 1, 0xFF, (size_t) (last_byte_set - first_byte_set - 1));
        *last_byte_set |= last_byte_mask;
    }
}

void bmp_clrrange(uint8_t* buf, size_t start, size_t len) {
    uint8_t first_bit_id;
    uint8_t* first_byte_clear;
    uint8_t last_bit_id;
    uint8_t* last_byte_clear;
    uint8_t first_byte_mask, last_byte_mask;
    _bmp_getrangeinfo(buf, start, len, first_bit_id, first_byte_clear,
                      last_bit_id, last_byte_clear);

    first_byte_mask = (uint8_t) (0xFF << (8 - first_bit_id));
    last_byte_mask = (uint8_t) (0xFF >> last_bit_id);

    /* Clear the bits. */
    if (first_byte_clear == last_byte_clear) {
        *first_byte_clear &= (first_byte_mask | last_byte_mask);
    } else {
        *first_byte_clear &= first_byte_mask;
        memset(first_byte_clear + 1, 0x00, (size_t) (last_byte_clear - first_byte_clear - 1));
        *last_byte_clear &= last_byte_mask;
    }
}

size_t bmp_countset(uint8_t* buf, size_t buflen, size_t start, size_t limit) {
    uint8_t first_bit_id;
    uint8_t first_byte;
    uint8_t ideal_first_byte;
    size_t numset;
    uint8_t curr_byte;
    size_t curr_index = start >> 3;
    first_bit_id = start & 0x7;
    first_byte = *(buf + curr_index);

    numset = 8 - first_bit_id; // initialize optimistically, assuming that the first byte will have all 1's in the part we care about
    ideal_first_byte = (uint8_t) (0xFF >> first_bit_id);
    first_byte &= ideal_first_byte;
    if (first_byte == ideal_first_byte) {
        // All bits in the first byte starting at first_bit_id are set
        for (curr_index = curr_index + 1; curr_index < buflen && numset < limit; curr_index++) {
            curr_byte = buf[curr_index];
            if (curr_byte == (uint8_t) 0xFF) {
                numset += 8;
            } else {
                while (curr_byte & (uint8_t) 0x80) { // we could add a numset < limit check here, but it probably isn't worth it
                    curr_byte <<= 1;
                    numset++;
                }
                break;
            }
        }
    } else {
        // The streak ends within the first byte
        do {
            first_byte >>= 1;
            ideal_first_byte >>= 1;
            numset--;
        } while (first_byte != ideal_first_byte);
    }
    return numset;
}

int bmp_isempty(uint8_t* buf, size_t buflen) {
    uint8_t* bufend = buf + buflen;
    while (buf < bufend) {
        if (*(buf++)) {
            return 0;
        }
    }
    return 1;
}

void bmp_print(uint8_t* buf, size_t buflen) {
    size_t i;
    for (i = 0; i < buflen; i++) {
        printf("%02X", buf[i]);
    }
    printf("\n");
}
