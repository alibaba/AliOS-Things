/**
 * \file            lwesp_unicode.c
 * \brief           Unicode support
 */

/*
 * Copyright (c) 2022 Tilen MAJERLE
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file is part of LwESP - Lightweight ESP-AT parser library.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 * Version:         v1.1.2-dev
 */
#include "lwesp/lwesp_private.h"
#include "lwesp/lwesp_unicode.h"

/**
 * \brief           Decode single character for unicode (UTF-8 only) format
 * \param[in,out]   s: Pointer to unicode decode control structure
 * \param[in]       c: UTF-8 character sequence to test for device
 * \return          \ref lwespOK Function succedded, there is a valid UTF-8 sequence
 * \return          \ref lwespINPROG Function continues well but expects some more data to finish sequence
 * \return          \ref lwespERR Error in UTF-8 sequence
 */
lwespr_t
lwespi_unicode_decode(lwesp_unicode_t* s, uint8_t c) {
    if (s->r == 0) {                            /* Are we expecting a first character? */
        s->t = 0;                               /* Reset sequence */
        s->ch[0] = c;                           /* Save current character */
        if (c < 0x80) {                         /* One byte only in UTF-8 representation */
            s->r = 0;                           /* Remaining bytes */
            s->t = 1;
            return lwespOK;
        }
        if ((c & 0xE0) == 0xC0) {               /* 1 additional byte in a row = 110x xxxx */
            s->r = 1;
        } else if ((c & 0xF0) == 0xE0) {        /* 2 additional bytes in a row = 1110 xxxx */
            s->r = 2;
        } else if ((c & 0xF8) == 0xF0) {        /* 3 additional bytes in a row = 1111 0xxx */
            s->r = 3;
        } else {
            return lwespERR;                    /* Error parsing unicode byte */
        }
        s->t = s->r + 1;                        /* Number of bytes is 1 byte more than remaining in sequence */
        return lwespINPROG;                     /* Return in progress status */
    } else if ((c & 0xC0) == 0x80) {            /* Next character in sequence */
        --s->r;                                 /* Decrease character */
        s->ch[s->t - s->r - 1] = c;             /* Save character to array */
        if (s->r == 0) {                        /* Did we finish? */
            return lwespOK;                     /* Return OK, we are ready to proceed */
        }
        return lwespINPROG;                     /* Still in progress */
    }
    return lwespERR;                            /* An error, unknown UTF-8 character entered */
}
