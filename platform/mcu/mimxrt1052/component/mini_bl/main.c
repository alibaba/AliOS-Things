/*
 * The Clear BSD License
 * Copyright 2018 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 * that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
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

#include <stdlib.h>
#include <stdint.h>
#include "fsl_mini_bl.h"
#include "fsl_common.h"
#include "fsl_flexspi_nor_boot.h"
#include "board.h"
#include "fsl_iomuxc.h"

#define UPDATE_MAX_TIME 3

static const ivt * const p_ivt = (void*)FSL_MINI_BL_IVT_ADDR;
static const BOOT_DATA_T * const p_bootData = (void*)FSL_MINI_BL_BOOT_DATA_ADDR;

void main(void)
{
    uint8_t time;

    /* Step 1: Check whether there is dirty section to copy. */
    for(time = 0; time < UPDATE_MAX_TIME; time++)
    {
        if (kStatus_Success == MINI_BL_UpdateSection())
        {
            break;
        }
    }

    if (UPDATE_MAX_TIME <= time)
    {
        NVIC_SystemReset();
    }

    /* Step 2: Check whether the image is valid. */
    if (p_ivt->hdr == IVT_HEADER)
    {
        /* Copy the image to run position. */
        if (p_bootData->start != FSL_MINI_BL_FLASH_CONFIG_ADDR)
        {
            memcpy((void*)(p_bootData->start), (void *)FSL_MINI_BL_FLASH_CONFIG_ADDR, p_bootData->size);
        }

        SCB_InvalidateICache();
        __disable_irq();

        SCB->VTOR = p_ivt->entry;

        __DSB();
        __ISB();

        /* Jump to the image. */
        __set_MSP(((uint32_t*)(p_ivt->entry))[0]);
        __set_PSP(((uint32_t*)(p_ivt->entry))[0]);

        void (*imageEntry)(void) = (void (*)(void))((uint32_t*)(p_ivt->entry))[1];
        imageEntry();
    }

    while (1)
    {
    }
}
