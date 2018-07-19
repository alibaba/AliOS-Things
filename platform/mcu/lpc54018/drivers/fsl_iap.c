/*
 * The Clear BSD License
 * Copyright 2018 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 *  that the following conditions are met:
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

#include "fsl_iap.h"

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.iap"
#endif

#define HZ_TO_KHZ_DIV 1000

/*******************************************************************************
 * Definitions
 ******************************************************************************/


/*******************************************************************************
 * Prototypes
 ******************************************************************************/

static status_t translate_iap_status(uint32_t status)
{
    /* Translate IAP return code to sdk status code */
    if (status == kStatus_Success)
    {
        return status;
    }
    else
    {
        return MAKE_STATUS(kStatusGroup_IAP, status);
    }
}

/*******************************************************************************
 * Variables
 ******************************************************************************/


/*******************************************************************************
 * Code
 ******************************************************************************/

status_t IAP_ReadPartID(uint32_t *partID)
{
    uint32_t command[5], result[5];

    command[0] = kIapCmd_IAP_ReadPartId;
    iap_entry(command, result);
    *partID = result[1];

    return translate_iap_status(result[0]);
}

status_t IAP_ReadBootCodeVersion(uint32_t *bootCodeVersion)
{
    uint32_t command[5], result[5];

    command[0] = kIapCmd_IAP_Read_BootromVersion;
    iap_entry(command, result);
    bootCodeVersion[0] = result[1];
    bootCodeVersion[1] = result[2];

    return translate_iap_status(result[0]);
}

void IAP_ReinvokeISP(uint8_t ispType, uint32_t *status)
{
    uint32_t command[5], result[5];
    uint8_t ispParameterArray[8];

    command[0] = kIapCmd_IAP_ReinvokeISP;
    memset(ispParameterArray, 0, sizeof(uint8_t) * 8);
    ispParameterArray[1] = ispType;
    ispParameterArray[7] = ispParameterArray[0] ^ ispParameterArray[1] ^ ispParameterArray[2] ^ ispParameterArray[3] ^
                           ispParameterArray[4] ^ ispParameterArray[5] ^ ispParameterArray[6];
    command[1] = (uint32_t)ispParameterArray;
    iap_entry(command, result);
    *status = translate_iap_status(result[0]);
}

status_t IAP_ReadUniqueID(uint32_t *uniqueID)
{
    uint32_t command[5], result[5];

    command[0] = kIapCmd_IAP_ReadUid;
    iap_entry(command, result);
    uniqueID[0] = result[1];
    uniqueID[1] = result[2];
    uniqueID[2] = result[3];
    uniqueID[3] = result[4];

    return translate_iap_status(result[0]);
}

status_t IAP_PrepareSectorForWrite(uint32_t startSector, uint32_t endSector)
{
    uint32_t command[5], result[4];

    command[0] = kIapCmd_IAP_PrepareSectorforWrite;
    command[1] = startSector;
    command[2] = endSector;
    iap_entry(command, result);

    return translate_iap_status(result[0]);
}

status_t IAP_CopyRamToFlash(uint32_t dstAddr, uint32_t *srcAddr, uint32_t numOfBytes, uint32_t systemCoreClock)
{
    uint32_t command[5], result[4];

    command[0] = kIapCmd_IAP_CopyRamToFlash;
    command[1] = dstAddr;
    command[2] = (uint32_t)srcAddr;
    command[3] = numOfBytes;
    command[4] = systemCoreClock / HZ_TO_KHZ_DIV;
    iap_entry(command, result);

    return translate_iap_status(result[0]);
}

status_t IAP_EraseSector(uint32_t startSector, uint32_t endSector, uint32_t systemCoreClock)
{
    uint32_t command[5], result[4];

    command[0] = kIapCmd_IAP_EraseSector;
    command[1] = startSector;
    command[2] = endSector;
    command[3] = systemCoreClock / HZ_TO_KHZ_DIV;
    iap_entry(command, result);

    return translate_iap_status(result[0]);
}

status_t IAP_ErasePage(uint32_t startPage, uint32_t endPage, uint32_t systemCoreClock)
{
    uint32_t command[5], result[4];

    command[0] = kIapCmd_IAP_ErasePage;
    command[1] = startPage;
    command[2] = endPage;
    command[3] = systemCoreClock / HZ_TO_KHZ_DIV;
    iap_entry(command, result);

    return translate_iap_status(result[0]);
}

status_t IAP_BlankCheckSector(uint32_t startSector, uint32_t endSector)
{
    uint32_t command[5], result[4];

    command[0] = kIapCmd_IAP_BlankCheckSector;
    command[1] = startSector;
    command[2] = endSector;
    iap_entry(command, result);

    return translate_iap_status(result[0]);
}

status_t IAP_Compare(uint32_t dstAddr, uint32_t *srcAddr, uint32_t numOfBytes)
{
    uint32_t command[5], result[4];

    command[0] = kIapCmd_IAP_Compare;
    command[1] = dstAddr;
    command[2] = (uint32_t)srcAddr;
    command[3] = numOfBytes;
    iap_entry(command, result);

    return translate_iap_status(result[0]);
}

status_t IAP_ExtendedFlashSignatureRead(uint32_t startPage, uint32_t endPage, uint32_t numOfState, uint32_t *signature)
{
    uint32_t command[5], result[5];

    command[0] = kIapCmd_IAP_ExtendedReadSignature;
    command[1] = startPage;
    command[2] = endPage;
    command[3] = numOfState;
    command[4] = 0;
    iap_entry(command, result);
    signature[0] = result[4];
    signature[1] = result[3];
    signature[2] = result[2];
    signature[3] = result[1];

    return translate_iap_status(result[0]);
}

status_t IAP_ReadEEPROMPage(uint32_t pageNumber, uint32_t *dstAddr, uint32_t systemCoreClock)
{
    uint32_t command[5], result[5];

    command[0] = kIapCmd_IAP_ReadEEPROMPage;
    command[1] = pageNumber;
    command[2] = (uint32_t)dstAddr;
    command[3] = systemCoreClock / HZ_TO_KHZ_DIV;
    iap_entry(command, result);

    return translate_iap_status(result[0]);
}

status_t IAP_WriteEEPROMPage(uint32_t pageNumber, uint32_t *srcAddr, uint32_t systemCoreClock)
{
    uint32_t command[5], result[5];

    command[0] = kIapCmd_IAP_WriteEEPROMPage;
    command[1] = pageNumber;
    command[2] = (uint32_t)srcAddr;
    command[3] = systemCoreClock / HZ_TO_KHZ_DIV;
    iap_entry(command, result);

    return translate_iap_status(result[0]);
}
