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

#include "test_util.hpp"

#include <ctype.h>

void DumpBuffer(const char *aTextMessage, const uint8_t *aBuffer, uint16_t aBufferLength)
{
    enum
    {
        kBytesPerLine = 16, // Number of bytes per line.
    };

    char     charBuff[kBytesPerLine + 1];
    uint16_t counter;
    uint8_t  byte;

    printf("\n%s - len = %u\n    ", aTextMessage ? aTextMessage : "Buffer", aBufferLength);

    counter = 0;

    while (aBufferLength--)
    {
        byte = *aBuffer++;
        printf("%02X ", byte);
        charBuff[counter] = isprint(byte) ? static_cast<char>(byte) : '.';
        counter++;

        if (counter == kBytesPerLine)
        {
            charBuff[counter] = 0;
            printf("    %s\n    ", charBuff);
            counter = 0;
        }
    }

    charBuff[counter] = 0;

    while (counter++ < kBytesPerLine)
    {
        printf("   ");
    }

    printf("    %s\n", charBuff);
}
