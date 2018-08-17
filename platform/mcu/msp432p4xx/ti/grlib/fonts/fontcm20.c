/* --COPYRIGHT--,BSD
 * Copyright (c) 2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
//*****************************************************************************
//
// This file is generated by ftrasterize; DO NOT EDIT BY HAND!
//
//*****************************************************************************

#include <ti/grlib/grlib.h>

//*****************************************************************************
//
// Details of this font:
//     Style: cm
//     Size: 20 point
//     Bold: no
//     Italic: no
//     Memory usage: 2600 bytes
//
//*****************************************************************************

//*****************************************************************************
//
// The compressed data for the 20 point Cm font.
//
//*****************************************************************************
static const uint8_t g_pucCm20Data[2398] =
{
    5,   9,   0,  24,  96,  18,   3,  49,  33,  33,  33,  33,
    33,  33,  33,  33,  33,  33,  33, 129,  33, 240,  32,  16,
    9, 240, 209,  49,  51,  34,  65,  49,  65,  49,  49,  49,
    129,   0,  15,  38,  16,   0,   7,  49, 161,  65, 161,  49,
    161,  65, 161,  65, 161,  49, 110,  97,  65, 161,  65, 161,
    49, 126,  81,  65, 161,  65, 161,  49, 177,  49, 161,  65,
    161,  65, 161, 240, 224,  41,   9, 193, 116,  65,  17,  17,
    49,  33,  33,  33,  33,  33,  33,  33,  33,  33,  33,  99,
    116,  97,  18,  81,  33,  33,  33,  33,  33,  33,  33,  33,
    33,  33,  33,  33,  17,  68, 113, 240, 240, 176,  55,  16,
    34, 113,  81,  33,  82,  65,  50,  50,  81,  69,  97,  65,
    49,  97,  65,  33, 113,  65,  33, 113,  65,  17, 145,  33,
    33, 162,  33,  50, 145,  49,  33, 129,  33,  65,  97,  49,
    65,  97,  49,  65,  81,  65,  65,  66,  65,  65,  65,  97,
    33, 210,   0,   8,  64,  43,  15, 240,  51, 193,  33, 161,
    49, 161,  49, 161,  49, 161,  33, 177,  17, 195,  68,  81,
    97,  99,  81,  81,  33,  65,  81,  65,  33,  97,  81,  17,
    97,  97,  65,  34,  65,  17,  34,  53,  51,   0,   9,  96,
    11,   5, 240,  17,  51,  65,  65,  49,   0,   9,  16,  23,
    6, 225,  81,  65,  81,  81,  65,  81,  81,  81,  81,  81,
    81,  81,  81,  97,  81,  81,  97,  81,  97,  32,  22,   6,
    193,  97,  97,  81,  81,  97,  81,  81,  81,  81,  81,  81,
    81,  81,  65,  81,  81,  65,  65, 176,  20,   9, 193, 129,
    81,  33,  33,  49,  17,  17,  83,  85,  49,  33,  33,  81,
    129,   0,  14,  16,  21,  15,   0,   8,  33, 225, 225, 225,
    225, 225, 225, 141, 129, 225, 225, 225, 225, 225,   0,   8,
    64,  11,   4,   0,   7,  65,  50,  49,  49,  33,  49, 112,
    8,   7,   0,  10,  69,   0,   8,  16,   8,   3, 240, 240,
    241,  33, 240,  32,  24,   9, 241, 129, 113, 129, 129, 113,
    129, 129, 113, 129, 129, 129, 113, 129, 129, 113, 129, 129,
    113, 129, 129, 128,  32,   9, 240, 227,  81,  49,  49,  65,
    49,  81,  33,  81,  33,  81,  33,  81,  33,  81,  33,  81,
    33,  81,  33,  81,  49,  49,  65,  49,  83,   0,   6,  16,
    20,   7, 240, 129,  67,  97,  97,  97,  97,  97,  97,  97,
    97,  97,  97,  97,  69, 240, 240, 112,  25,   9, 240, 227,
    66,  49,  49,  81,  33,  81,  33,  81, 129, 113, 129, 113,
    113, 113, 113, 113,  81,  39,   0,   5, 112,  26,   9, 240,
    227,  66,  49,  49,  81,  33,  81, 113, 129,  68, 145, 145,
    129,  33,  81,  33,  81,  33,  65,  68,   0,   6,  16,  28,
    10, 240, 240,  81, 145, 130, 113,  17,  97,  33,  97,  33,
    81,  49,  81,  49,  65,  65,  65,  65,  72, 113, 145, 117,
    0,   6,  64,  26,   9, 240, 209, 132,  81, 129, 129,  18,
    82,  33, 145, 129, 129,  34,  65,  34,  65,  33,  81,  49,
    49,  83,   0,   6,  16,  31,   9, 240, 243,  81,  49,  49,
    65,  33, 129,  19,  66,  49,  49,  81,  33,  81,  33,  81,
    33,  81,  33,  81,  33,  81,  49,  49,  83,   0,   6,  16,
    21,  10, 240,  97, 145, 151,  33,  81, 145, 129, 129, 145,
    145, 129, 145, 145, 145, 145, 145,   0,   7,  31,   9, 240,
    227,  81,  49,  49,  81,  33,  81,  49,  65,  50,  33,  83,
    81,  34,  49,  66,  33,  81,  33,  81,  33,  81,  49,  50,
    67,   0,   6,  16,  30,   9, 240, 227,  81,  49,  49,  65,
    49,  81,  33,  81,  33,  81,  33,  81,  49,  35,  67,  17,
    129, 113,  49,  65,  49,  49,  83,   0,   6,  32,  10,   3,
    240, 145,  33, 240,  33,  33, 240,  32,  14,   4, 240, 240,
    34,  34, 240, 114,  34,  49,  49,  33,  49, 112,  18,   3,
    240,  49,  33,  81,  33,  33,  33,  33,  33,  33,  33,  33,
    33,  33,  33,  80,  11,  15,   0,  16, 125,   0,   5, 125,
    0,  15,  32,  22,   8,   0,   6,  34,  98, 241, 113, 113,
    113,  97, 113,  97,  97, 113,  65,  33,  65,  34,  49,  52,
    176,  24,   8, 163,  65,  49,  33,  65,  33,  65,  33,  65,
    113,  97, 113,  97, 113, 113, 113, 240, 114,  98, 240, 240,
    224,  52,  15, 240,  68, 161,  66, 113, 113,  81,  36,  49,
    65,  18,  33,  49,  49,  33,  66,  33,  33,  17,  97,  33,
    33,  17,  97,  33,  33,  17,  97,  33,  33,  17,  97,  33,
    33,  33,  81,  33,  49,  18,  50,  33,  49,  36,  34,  81,
    241,  98, 118,   0,  10,  35,  15, 240, 240,  98, 193,  17,
    193,  17, 193,  18, 161,  34, 161,  49, 161,  50, 129,  66,
    129,  81, 136, 113,  82,  97, 113,  97, 113,  97, 114,  52,
    84,   0,   9,  80,  32,  13, 240, 240, 153,  97,  82,  81,
    98,  65,  98,  65,  97,  81,  81, 103,  97,  97,  81, 113,
    65, 113,  65, 113,  65, 113,  65,  97,  57,   0,   8,  80,
    32,  14,   0,   5, 100,  49,  66,  66,  17,  50, 114,  49,
    130,  33, 161,  33, 161,  33, 209, 209, 209, 161,  49, 145,
    50, 113,  82,  81, 133,   0,   9,  48,  33,  14, 240, 240,
    201, 113,  82,  97, 113,  81, 113,  81, 129,  65, 129,  65,
    129,  65, 129,  65, 129,  65, 129,  65, 113,  81, 113,  81,
    82,  73,   0,   9,  48,  31,  14, 240, 240, 203,  81, 113,
    81, 113,  81, 209,  65, 129,  65, 134, 129,  65, 129,  65,
    129, 129,  65, 129,  65, 129,  65, 113,  59,   0,   9,  16,
    27,  13, 240, 240, 155,  65, 113,  65, 113,  65, 193,  65,
    113,  65, 118, 113,  65, 113,  65, 113, 193, 193, 193, 165,
    0,   9,  16,  31,  14,   0,   5, 100, 130,  65,  17,  66,
    98,  65, 129,  49, 145,  49, 209, 209, 209, 116,  33, 145,
    65, 129,  81, 113,  82,  82, 117,   0,   9,  48,  33,  14,
    240, 240, 197,  37,  65,  97,  97,  97,  97,  97,  97,  97,
    97,  97, 104,  97,  97,  97,  97,  97,  97,  97,  97,  97,
    97,  97,  97,  69,  37,   0,   9,  20,   7, 240, 101,  65,
    97,  97,  97,  97,  97,  97,  97,  97,  97,  97,  97,  69,
    240, 240, 112,  22,   9, 240, 244, 113, 129, 129, 129, 129,
    129, 129, 129, 129, 129,  50,  49,  50,  34,  68,   0,   6,
    16,  36,  15, 240, 240, 245,  68,  65, 113,  97,  97, 113,
    81, 129,  50, 145,  33, 177,  17,  17, 162,  34, 145,  66,
    129,  81, 129,  82, 113,  98,  97, 113,  69,  68,   0,   9,
    80,  25,  12, 240, 240, 101, 145, 177, 177, 177, 177, 177,
    177, 177, 177,  97,  65,  97,  65,  97,  65,  82,  41,   0,
    7, 112,  56,  17,   0,   6,  52, 131,  66, 114,  97,  17,
    81,  17,  97,  17,  81,  17,  97,  17,  81,  17,  97,  33,
    49,  33,  97,  33,  49,  33,  97,  33,  49,  33,  97,  49,
    17,  49,  97,  49,  17,  49,  97,  49,  17,  49,  97,  65,
    65,  97,  65,  65,  68,  49,  52,   0,  10, 112,  43,  15,
    240, 240, 244,  84,  67,  81,  97,  17,  81,  97,  18,  65,
    97,  33,  65,  97,  34,  49,  97,  49,  49,  97,  65,  33,
    97,  66,  17,  97,  81,  17,  97,  83,  97,  98,  97,  98,
    68,  97,   0,   9, 112,  33,  14,   0,   5, 100, 130,  66,
    82,  98,  65, 129,  49, 161,  33, 161,  33, 161,  33, 161,
    33, 161,  33, 161,  49, 129,  66,  98,  82,  66, 132,   0,
    9,  64,  26,  12, 240, 240, 104,  97,  81,  81,  97,  65,
    97,  65,  97,  65,  81,  86,  97, 177, 177, 177, 177, 177,
    149,   0,   8,  48,  41,  14,   0,   5, 101, 129,  81,  97,
    113,  65, 129,  49, 161,  33, 161,  33, 161,  33, 161,  33,
    161,  49, 145,  49,  66,  33,  81,  33,  17,  33,  99,  34,
    133, 209,  33, 177,  17, 179, 178, 240,  32,  34,  15, 240,
    240, 248, 145,  81, 129,  97, 113,  97, 113,  97, 113,  81,
    134, 145,  66, 129,  81, 129,  81, 129,  81, 129,  82, 113,
    82,  33,  37,  67,   0,   9,  96,  29,  10, 240, 240,  35,
    97,  50,  49,  81,  49,  81,  49, 146, 148, 147, 146,  33,
    97,  33,  97,  33,  97,  35,  49,  49,  20,   0,   6,  96,
    27,  14, 240, 240, 219,  49,  65,  65,  33,  81,  65,  33,
    81,  65, 129, 209, 209, 209, 209, 209, 209, 209, 209, 181,
    0,   9,  48,  34,  15, 240, 240, 245,  53,  65, 113,  97,
    113,  97, 113,  97, 113,  97, 113,  97, 113,  97, 113,  97,
    113,  97, 113,  97, 113, 113,  81, 130,  50, 148,   0,  10,
    32,  32,  15, 240, 240, 244,  99,  50, 129,  81, 113,  98,
    97, 113,  81, 129,  81, 130,  65, 145,  49, 162,  33, 177,
    17, 193,  17, 195, 209, 225,   0,  10,  48,  52,  21,   0,
    7, 116,  68,  67,  50,  97, 113,  81,  98,  81,  97,  81,
    17,  81,  98,  65,  17,  81, 113,  65,  18,  49, 129,  49,
    49,  49, 130,  33,  49,  49, 145,  33,  50,  17, 161,  17,
    81,  17, 163,  81,  17, 178,  82, 193, 113, 193, 113,   0,
    14,  32,  15, 240, 240, 244,  83,  81,  97, 129,  81, 130,
    49, 162,  17, 194, 225, 226, 178,  17, 177,  49, 145,  66,
    113,  98,  97, 113,  67, 100,   0,   9,  80,  28,  15, 240,
    240, 244,  99,  65, 129,  82,  97, 113,  81, 130,  49, 162,
    33, 177,  17, 195, 209, 225, 225, 225, 225, 197,   0,  10,
    16,  26,  12, 240, 240, 106,  34,  97,  49,  98, 161, 161,
    162, 161, 161, 161, 177, 161,  97,  49, 113,  49,  98,  42,
    0,   7,  96,  24,   5,  83,  33,  65,  65,  65,  65,  65,
    65,  65,  65,  65,  65,  65,  65,  65,  65,  65,  65,  65,
    65,  67,  32,  16,   9, 240, 209,  49,  49,  49,  65,  49,
    66,  35,  49,  49,   0,  16,  16,  24,   5,  83,  65,  65,
    65,  65,  65,  65,  65,  65,  65,  65,  65,  65,  65,  65,
    65,  65,  65,  65,  65,  35,  32,   9,   5, 240,  17,  49,
    17,   0,  10, 112,   7,   3, 145,  33,   0,   6,  80,  11,
    5, 240,  17,  49,  65,  67,  49,   0,   9,  16,  25,  11,
    0,  11,  20, 113,  49,  97,  49, 116,  97,  49,  81,  65,
    81,  65,  33,  33,  50,  33,  51,  34,   0,   7,  32,  28,
    10, 240, 242, 145, 145, 145, 145, 145,  19,  82,  49,  65,
    81,  49,  81,  49,  81,  49,  81,  49,  81,  50,  49,  99,
    0,   6,  96,  19,   8,   0,   8,  35,  65,  49,  33,  65,
    33, 113, 113, 114, 113,  49,  67, 240, 240, 208,  30,  10,
    240, 240,  82, 145, 145, 145, 145,  83,  17,  65,  50,  49,
    81,  49,  81,  49,  81,  49,  81,  49,  81,  65,  50,  83,
    18,   0,   6,  64,  20,   9,   0,   9,  35,  81,  49,  49,
    81,  39,  33, 129, 129,  81,  49,  49,  83,   0,   6,  16,
    23,   8, 179,  81,  17,  65,  33,  65, 113, 113, 113,  85,
    81, 113, 113, 113, 113, 113, 113, 100, 240, 240, 208,  25,
    11,   0,  11,  54,  65,  49,  97,  49,  97,  49,  97,  49,
    100, 113, 161, 181,  66,  81,  33, 113,  34,  82,  69, 240,
    29,  10, 240, 242, 145, 145, 145, 145, 145,  19,  82,  49,
    65,  65,  65,  65,  65,  65,  65,  65,  65,  65,  65,  65,
    51,  35,   0,   6,  64,  17,   5, 240,  17,  65, 240,  50,
    65,  65,  65,  65,  65,  65,  65,  51, 240, 192,  22,   7,
    240, 146,  82, 240, 163,  97,  97,  97,  97,  97,  97,  97,
    97,  97,  33,  49,  33,  33,  66, 176,  30,  11, 240, 240,
    50, 161, 161, 161, 161, 161,  51,  65,  49,  97,  33, 113,
    17, 130,  17, 113,  49,  97,  49,  97,  65,  67,  36,   0,
    7,  16,  18,   5, 242,  65,  65,  65,  65,  65,  65,  65,
    65,  65,  65,  65,  65,  51, 240, 192,  34,  15,   0,  15,
    2,  19,  35,  82,  50,  49,  65,  65,  65,  65,  65,  65,
    65,  65,  65,  65,  65,  65,  65,  65,  65,  65,  65,  65,
    51,  35,  35,   0,   9,  80,  25,  10,   0,  10,   2,  19,
    82,  49,  65,  65,  65,  65,  65,  65,  65,  65,  65,  65,
    65,  65,  51,  35,   0,   6,  64,  23,   9,   0,   9,  35,
    81,  49,  49,  81,  33,  81,  33,  81,  33,  81,  33,  81,
    49,  49,  83,   0,   6,  16,  28,  10,   0,  10,   2,  19,
    82,  49,  65,  81,  49,  81,  49,  81,  49,  81,  49,  66,
    50,  49,  65,  19,  81, 145, 145, 131, 240,  32,  26,  10,
    0,  10,  35,  97,  50,  49,  81,  49,  81,  49,  81,  49,
    81,  49,  81,  65,  50,  83,  17, 145, 145, 145, 131, 192,
    18,   7,   0,   7,   2,  18,  50,  17,  49,  97,  97,  97,
    97,  97,  83, 240, 240, 144,  22,   9,   0,   9,  20,  17,
    33,  66,  33,  81,  34, 148,  49,  81,  33,  81,  34,  50,
    33,  20,   0,   6,  22,   9, 240, 240, 129, 129, 129, 114,
    103,  65, 129, 129, 129, 129,  49,  65,  49,  65,  49,  83,
    0,   6,  25,  10,   0,  10,   2,  50,  65,  65,  65,  65,
    65,  65,  65,  65,  65,  65,  65,  65,  65,  50,  83,  18,
    0,   6,  64,  23,  11,   0,  11,   3,  51,  49,  81,  81,
    49,  97,  49, 113,  17, 129,  17, 129,  17, 145, 161,   0,
    7,  80,  33,  15,   0,  15,   4,  19,  35,  50,  49,  65,
    81,  49,  49,  97,  33,  17,  33,  98,  17,  17,  33, 114,
    33,  17, 130,  50, 145,  50, 145,  49,   0,  10,  16,  23,
    12,   0,  12,   4,  51,  65,  65, 113,  33, 146, 162, 145,
    18, 113,  49, 113,  65,  67,  52,   0,   7,  96,  27,  11,
    0,  11,   3,  51,  49,  81,  81,  49,  97,  49,  97,  33,
    129,  17, 130, 161, 161, 145, 113,  33, 113,  17, 131, 240,
    64,  19,   9,   0,   9,   7,  33,  65, 113, 129, 113, 113,
    113,  65,  33,  81,  39,   0,   5, 112,   8,  11,   0,  13,
    105,   0,  15,  48,   9,  21,   0,  26,  47,   4,   0,  29,
    16,  12,   8, 240, 161,  49,  33,  49,  49,  49,   0,  16,
    48,   9,   8, 240, 162,  81,  35,   0,  17,  32,
};

//*****************************************************************************
//
// The font definition for the 20 point Cm font.
//
//*****************************************************************************
const Graphics_Font g_sFontCm20 =
{
    //
    // The format of the font.
    //
    FONT_FMT_PIXEL_RLE,

    //
    // The maximum width of the font.
    //
    18,

    //
    // The height of the font.
    //
    22,

    //
    // The baseline of the font.
    //
    17,

    //
    // The offset to each character in the font.
    //
    {
        0,    5,   23,   39,   77,  118,  173,  216,
        227,  250,  272,  292,  313,  324,  332,  340,
        364,  396,  416,  441,  467,  495,  521,  552,
        573,  604,  634,  644,  658,  676,  687,  709,
        733,  785,  820,  852,  884,  917,  948,  975,
        1006, 1039, 1059, 1081, 1117, 1142, 1198, 1241,
        1274, 1300, 1341, 1375, 1404, 1431, 1465, 1497,
        1549, 1581, 1609, 1635, 1659, 1675, 1699, 1708,
        1715, 1726, 1751, 1779, 1798, 1828, 1848, 1871,
        1896, 1925, 1942, 1964, 1994, 2012, 2046, 2071,
        2094, 2122, 2148, 2166, 2188, 2210, 2235, 2258,
        2291, 2314, 2341, 2360, 2368, 2377, 2389,
    },

    //
    // A pointer to the actual font data
    //
    g_pucCm20Data
};
