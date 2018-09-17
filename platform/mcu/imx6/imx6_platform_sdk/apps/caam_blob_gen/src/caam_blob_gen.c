/*
 * Copyright (c) 2011-2012, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "sdk.h"
#include "platform_init.h"
#include "caam/caam.h"

/* These defines could be customized */
#define DEK_PLAINTEXT_ADDR  0x10700000
#define DEK_BLOB_ADDR       0x10701000

/* This is the required header added to the DEK blob for encrypted boot */
uint8_t wrapped_key_hdr[8] = {0x81, 0x00, 0x48, 0x41, 0x66, 0x55, 0x10, 0x00};

/*!
 * CAAM blob generator for encrypted boot.
 *
 * @return  0
 */
int32_t caam_blob_gen(void)
{
    uint8_t i, sel;
    uint32_t key_addr, ret = SUCCESS;

    printf("Start CAAM blob generator:\n");

    do {
        printf("\n  To wrapped the DEK into a blob:\n");
        printf("  1 - with provisioned DEK at 0x%X.\n",DEK_PLAINTEXT_ADDR);
        printf("  x - to exit to SDP mode.\n\n");

        do {
            sel = getchar();
        } while (sel == NONE_CHAR);

        if (sel == 'x') {
            printf("\nTest exit.\n");
            break;
        }

        printf("-- Executing option %d -- \n\n",sel-48);
        if (sel == '1')
        {
            /* Set the key address to the one of the DEK in DDR loaded
             * through the manufacturing tool.
             */
            key_addr = DEK_PLAINTEXT_ADDR;
        }

        caam_open();

        /* Print the DEK that is going to be used */
        for(i=0;i<16;i++)
            printf("%02X ",((uint8_t *)key_addr)[i]);
        printf("\n\n");

        /* Generate the blob to encapsulate the DEK */
        ret = caam_gen_blob((uint32_t)key_addr, (uint32_t)(DEK_BLOB_ADDR+8));
        if(ret != SUCCESS)
            printf("Error during blob decap operation: 0x%d\n",ret);

        /* Copy the header into the DEK blob buffer */
        memcpy((uint32_t *)DEK_BLOB_ADDR, wrapped_key_hdr,
                    sizeof(wrapped_key_hdr));

        /* Print the generated DEK blob */
        printf("DEK blob is available at 0x%08X and is equal to:\n",DEK_BLOB_ADDR);
        for(i=0;i<(8+64);i++)
            printf("%02X ",((uint8_t *)DEK_BLOB_ADDR)[i]);
        printf("\n\n");
        printf("Note: the first 8 Bytes are the required header and wrp_dat of"
                " the wrapped key structure!\n\n");

    } while(1);

    return 0;
}
void main(void)
{
    platform_init();

    // Run the blob generator
    caam_blob_gen();

    // enter SDP mode when exiting
    jump_to_sdp();
}
