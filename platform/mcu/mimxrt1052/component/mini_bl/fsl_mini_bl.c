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
#include "crc.h"
#include "fsl_flexspi_nor_boot.h"
#include "board.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define MINI_BL_RETRY_NUM 3

#define MINI_BL_SWAP_SIZE 1024

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void MINI_BL_LoadSectionHeader(void);

static status_t MINI_BL_StoreSectionHeader(void);

static bool MINI_BL_IsDCacheEnabled(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/
static fsl_mini_bl_section_header_crc_t* p_sectionHeader = (fsl_mini_bl_section_header_crc_t *)FSL_MINI_BL_SECTION_HEADER_ADDR;

static fsl_mini_bl_section_header_crc_t s_sectionHeaderRam;

static uint8_t s_miniBlSwap[MINI_BL_SWAP_SIZE];

/*******************************************************************************
 * Code
 ******************************************************************************/
static bool MINI_BL_IsDCacheEnabled(void)
{
    if ((SCB->CCR & (uint32_t)SCB_CCR_DC_Msk) == SCB_CCR_DC_Msk)
    {
        return true;
    }
    else
    {
        return false;
    }
}

static void MINI_BL_LoadSectionHeader(void)
{
    flashOps.read(FSL_MINI_BL_SECTION_HEADER_ADDR, &s_sectionHeaderRam, sizeof(s_sectionHeaderRam));
}

static status_t MINI_BL_StoreSectionHeader(void)
{
    /* Calculate the new CRC value. */
    //s_sectionHeaderRam.crc = utils_crc16(&(s_sectionHeaderRam.header), sizeof(fsl_mini_bl_section_header_t));

    MINI_BL_EraseFlash((uint32_t)p_sectionHeader, sizeof(s_sectionHeaderRam));

    return MINI_BL_WriteFlash((uint32_t)p_sectionHeader, &s_sectionHeaderRam, sizeof(s_sectionHeaderRam));
}

status_t MINI_BL_ReadFlash(uint32_t addr, void *data, uint32_t size)
{
    return flashOps.read(addr, data, size);
}

status_t MINI_BL_EraseFlash(uint32_t addr, uint32_t size)
{
    status_t status;
    bool isDCacheEnabled = MINI_BL_IsDCacheEnabled();

    if (isDCacheEnabled)
    {
        SCB_DisableDCache();
    }

    status = flashOps.erase(addr, size);

    if (isDCacheEnabled)
    {
        SCB_EnableDCache();
    }

    return status;
}

status_t MINI_BL_WriteFlash(uint32_t addr, void *data, uint32_t size)
{
    uint32_t i, j;

    bool isDCacheEnabled = MINI_BL_IsDCacheEnabled();

    if (isDCacheEnabled)
    {
        SCB_DisableDCache();
    }

    for (i=0; i<MINI_BL_RETRY_NUM; i++)
    {
        flashOps.write(addr, data, size);

        /* Verify result */
        for (j=0; j<size; j++)
        {
            if (((const uint8_t *)data)[j] != ((const uint8_t *)addr)[j])
            {
                break;
            }
        }

        /* Verify pass */
        if (j == size)
        {
            break;
        }
    }

    if (isDCacheEnabled)
    {
        SCB_EnableDCache();
    }

    if (MINI_BL_RETRY_NUM == i)
    {
        return kStatus_Fail;
    }
    else
    {
        return kStatus_Success;
    }
}

status_t MINI_BL_UpdateSection(void)
{
    status_t status;
    uint32_t sizeLeft;
    uint32_t sizeToProgram;
    uint32_t addrToProgram;
    uint32_t addrToLoad;

    MINI_BL_LoadSectionHeader();

    //if (s_sectionHeaderRam.crc !=
    //        utils_crc16(&(s_sectionHeaderRam.header), sizeof(fsl_mini_bl_section_header_t)))
    //{
        /* Don't need to update */
    //    return kStatus_Success;
    //}

    if ((s_sectionHeaderRam.header.flags & FSL_MINI_BL_SECTION_DIRTY) == FSL_MINI_BL_SECTION_DIRTY)
    {
        MINI_BL_EraseFlash(s_sectionHeaderRam.header.destAddr, s_sectionHeaderRam.header.size);

        sizeLeft = s_sectionHeaderRam.header.size;
        addrToProgram = s_sectionHeaderRam.header.destAddr;
        addrToLoad = s_sectionHeaderRam.header.srcAddr;

        /* Copy the section. */
        while (sizeLeft > 0)
        {
            sizeToProgram = sizeLeft > MINI_BL_SWAP_SIZE ? MINI_BL_SWAP_SIZE : sizeLeft;

            memcpy(s_miniBlSwap, (void*)addrToLoad, sizeToProgram);

            status = MINI_BL_WriteFlash(addrToProgram, s_miniBlSwap, sizeToProgram);

            if (kStatus_Success != status)
            {
                /* Image program failed. */
                return status;
            }

            sizeLeft -= sizeToProgram;
            addrToLoad += sizeToProgram;
            addrToProgram += sizeToProgram;
        }

        s_sectionHeaderRam.header.flags &= ~FSL_MINI_BL_SECTION_DIRTY;

        /* Clear the dirty flag. */
        return MINI_BL_StoreSectionHeader();
    }
    else
    {
        return kStatus_Success;
    }
}
