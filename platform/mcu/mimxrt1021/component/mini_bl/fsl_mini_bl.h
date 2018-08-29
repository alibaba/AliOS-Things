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

#ifndef _FSL_MINI_BL_H_
#define _FSL_MINI_BL_H_

#include "fsl_common.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief Flash operations used for MINI BL
 *
 * Application should define board level flash operations.
 */
typedef struct _fsl_mini_bl_flash_ops
{
    status_t (*read)(uint32_t addr, void *data, uint32_t size);
    status_t (*write)(uint32_t addr, const void *data, uint32_t size);
    status_t (*erase)(uint32_t addr, uint32_t size);
} fsl_mini_bl_flash_ops_t;

/* Define this at board level. */
extern const fsl_mini_bl_flash_ops_t flashOps;

/*! @brief Header to control image section.
 *
 * During MCU boot, mini bl checkes section header, if the flag FSL_MINI_BL_SECTION_DIRTY 
 * is set, then it copy the data from srcAddr to destAddr, then clear the flag
 * FSL_MINI_BL_SECTION_DIRTY.
 */
typedef struct
{
    uint32_t srcAddr;
    uint32_t destAddr;
    uint32_t size;
    uint32_t flags;
    #define FSL_MINI_BL_SECTION_DIRTY (1 << 0)
} fsl_mini_bl_section_header_t;

typedef struct _fsl_mini_bl_section_header
{
    fsl_mini_bl_section_header_t header;

    uint16_t crc; /* CRC value of the header. */
    uint16_t reserved;
} fsl_mini_bl_section_header_crc_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

status_t MINI_BL_ReadFlash(uint32_t addr, void *data, uint32_t size);

status_t MINI_BL_EraseFlash(uint32_t addr, uint32_t size);

status_t MINI_BL_WriteFlash(uint32_t addr, void *data, uint32_t size);

status_t MINI_BL_UpdateSection(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

/*! @} */

#endif /* _FSL_MINI_BL_H_ */
