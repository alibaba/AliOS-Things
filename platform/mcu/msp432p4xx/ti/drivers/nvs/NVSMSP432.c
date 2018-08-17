/*
 * Copyright (c) 2015-2017, Texas Instruments Incorporated
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
 */

/*
 *  ======== NVSMSP432.c ========
 */

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include <ti/devices/DeviceFamily.h>

#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/dpl/SemaphoreP.h>

#include <ti/drivers/NVS.h>
#include <ti/drivers/nvs/NVSMSP432.h>

#include DeviceFamily_constructPath(driverlib/rom.h)
#include DeviceFamily_constructPath(driverlib/rom_map.h)

#if defined(DeviceFamily_MSP432P401x)
 #include DeviceFamily_constructPath(driverlib/flash.h)
 static uint32_t getFlashSector(uint32_t address);
 static bool unprotectMemory(uint32_t startAddr, uint32_t endAddr);
 static bool protectMemory(uint32_t startAddr, uint32_t endAddr);
 #define eraseSector(a) MAP_FlashCtl_eraseSector(sectorBase)
 #define programMemory(a, b, c) MAP_FlashCtl_programMemory(a, b, c)
 #define FLASH_END_ADDRESS (0x0003ffff)
#else
 #include DeviceFamily_constructPath(driverlib/flash_a.h)
 #define protectMemory(a, b) FlashCtl_A_protectMemory(a, b)
 #define unprotectMemory(a, b) FlashCtl_A_unprotectMemory(a, b)
 #define eraseSector(a) MAP_FlashCtl_A_eraseSector(sectorBase)
 #define programMemory(a, b, c) MAP_FlashCtl_A_programMemory(a, b, c)
 #define FLASH_END_ADDRESS (0x001fffff)
#endif

/* 4-KB FLASH sector size */
#define FLASH_SECTOR_SIZE  (0x1000)
#define SECTOR_BASE_MASK   (~(FLASH_SECTOR_SIZE - 1))

static int_fast16_t checkEraseRange(NVS_Handle handle, size_t offset, size_t size);
static int_fast16_t doErase(NVS_Handle handle, size_t offset, size_t size);

extern NVS_Config NVS_config[];
extern const uint8_t NVS_count;

/* NVS function table for NVSMSP432 implementation */
const NVS_FxnTable NVSMSP432_fxnTable = {
    NVSMSP432_close,
    NVSMSP432_control,
    NVSMSP432_erase,
    NVSMSP432_getAttrs,
    NVSMSP432_init,
    NVSMSP432_lock,
    NVSMSP432_open,
    NVSMSP432_read,
    NVSMSP432_unlock,
    NVSMSP432_write
};

/*
 *  Semaphore to synchronize access to flash region.
 */
static SemaphoreP_Handle  writeSem;

/*
 *  ======== NVSMSP432_close ========
 */
void NVSMSP432_close(NVS_Handle handle)
{
    NVSMSP432_Object *object;

    object = handle->object;
    object->opened = false;
}

/*
 *  ======== NVSMSP432_control ========
 */
int_fast16_t NVSMSP432_control(NVS_Handle handle, uint_fast16_t cmd, uintptr_t arg)
{
    return (NVS_STATUS_UNDEFINEDCMD);
}

/*
 *  ======== NVSMSP432_erase ========
 */
int_fast16_t NVSMSP432_erase(NVS_Handle handle, size_t offset, size_t size)
{
    int_fast16_t status;

    SemaphoreP_pend(writeSem, SemaphoreP_WAIT_FOREVER);

    status = doErase(handle, offset, size);

    SemaphoreP_post(writeSem);

    return (status);
}

/*
 *  ======== NVSMSP432_getAttrs ========
 */
void NVSMSP432_getAttrs(NVS_Handle handle, NVS_Attrs *attrs)
{
    NVSMSP432_HWAttrs const *hwAttrs;

    hwAttrs = handle->hwAttrs;

    /* FlashSectorSizeGet() returns the size of a flash sector in bytes. */
    attrs->regionBase  = hwAttrs->regionBase;
    attrs->regionSize  = hwAttrs->regionSize;
    attrs->sectorSize  = FLASH_SECTOR_SIZE;
}

/*
 *  ======== NVSMSP432_init ========
 */
void NVSMSP432_init()
{
    unsigned int key;
    SemaphoreP_Handle sem;

    /* speculatively create a binary semaphore for thread safety */
    sem = SemaphoreP_createBinary(1);
    /* sem == NULL will be detected in 'open' */

    key = HwiP_disable();

    if (writeSem == NULL) {
        /* use the binary sem created above */
        writeSem = sem;
        HwiP_restore(key);
    }
    else {
        /* init already called */
        HwiP_restore(key);
        /* delete unused Semaphore */
        if (sem) SemaphoreP_delete(sem);
    }
}

/*
 *  ======== NVSMSP432_lock =======
 */
int_fast16_t NVSMSP432_lock(NVS_Handle handle, uint32_t timeout)
{
    if (SemaphoreP_pend(writeSem, timeout) != SemaphoreP_OK) {
        return (NVS_STATUS_TIMEOUT);
    }
    return (NVS_STATUS_SUCCESS);
}

/*
 *  ======== NVSMSP432_open =======
 */
NVS_Handle NVSMSP432_open(uint_least8_t index, NVS_Params *params)
{
    NVSMSP432_Object *object;
    NVSMSP432_HWAttrs const *hwAttrs;
    NVS_Handle handle;

    /* Confirm that 'init' has successfully completed */
    if (writeSem == NULL) {
        NVSMSP432_init();
        if (writeSem == NULL) {
            return (NULL);
        }
    }

    /* verify NVS region index */
    if (index >= NVS_count) {
        return (NULL);
    }

    handle = &NVS_config[index];
    object = NVS_config[index].object;
    hwAttrs = NVS_config[index].hwAttrs;

    SemaphoreP_pend(writeSem, SemaphoreP_WAIT_FOREVER);

    if (object->opened == true) {
        SemaphoreP_post(writeSem);
        return (NULL);
    }

    /* The block must lie in the main memory (0 - 0x40000) */
    if ((size_t)(hwAttrs->regionBase) > FLASH_END_ADDRESS) {
        SemaphoreP_post(writeSem);
        return (NULL);
    }

    /* The regionBase must be aligned on a flash page boundary */
    if ((size_t)(hwAttrs->regionBase) & (FLASH_SECTOR_SIZE - 1)) {
        SemaphoreP_post(writeSem);
        return (NULL);
    }

    /* The region cannot be smaller than a sector size */
    if (hwAttrs->regionSize < FLASH_SECTOR_SIZE) {
        SemaphoreP_post(writeSem);
        return (NULL);
    }

    /* The region size must be a multiple of sector size */
    if (hwAttrs->regionSize != (hwAttrs->regionSize & SECTOR_BASE_MASK)) {
        SemaphoreP_post(writeSem);
        return (NULL);
    }

    object->opened = true;

    SemaphoreP_post(writeSem);

    return (handle);
}

/*
 *  ======== NVSMSP432_read =======
 */
int_fast16_t NVSMSP432_read(NVS_Handle handle, size_t offset, void *buffer,
        size_t bufferSize)
{
    NVSMSP432_HWAttrs const *hwAttrs;

    hwAttrs = handle->hwAttrs;

    /* Validate offset and bufferSize */
    if (offset + bufferSize > hwAttrs->regionSize) {
        return (NVS_STATUS_INV_OFFSET);
    }

    /*
     *  Get exclusive access to the region.  We don't want someone
     *  else to erase the region while we are reading it.
     */
    SemaphoreP_pend(writeSem, SemaphoreP_WAIT_FOREVER);

    memcpy(buffer, (char *)(hwAttrs->regionBase) + offset, bufferSize);

    SemaphoreP_post(writeSem);

    return (NVS_STATUS_SUCCESS);
}

/*
 *  ======== NVSMSP432_unlock =======
 */
void NVSMSP432_unlock(NVS_Handle handle)
{
    SemaphoreP_post(writeSem);
}

/*
 *  ======== NVSMSP432_write =======
 */
int_fast16_t NVSMSP432_write(NVS_Handle handle, size_t offset, void *buffer,
                      size_t bufferSize, uint_fast16_t flags)
{
    NVSMSP432_HWAttrs const *hwAttrs;
    unsigned int size;
    uint32_t status = 0;
    int i;
    uint8_t *srcBuf, *dstBuf;
    int retval = NVS_STATUS_SUCCESS;

    hwAttrs = handle->hwAttrs;

    /* Validate offset and bufferSize */
    if (offset + bufferSize > hwAttrs->regionSize) {
        return (NVS_STATUS_INV_OFFSET);
    }

    /* Get exclusive access to the Flash region */
    SemaphoreP_pend(writeSem, SemaphoreP_WAIT_FOREVER);

    /* If erase is set, erase destination sector(s) first */
    if (flags & NVS_WRITE_ERASE) {
        size = bufferSize & SECTOR_BASE_MASK;
        if (bufferSize & (~SECTOR_BASE_MASK)) {
            size += FLASH_SECTOR_SIZE;
        }

        retval = doErase(handle, offset & SECTOR_BASE_MASK, size);
        if (retval != NVS_STATUS_SUCCESS) {
            SemaphoreP_post(writeSem);

            return (retval);
        }
    }
    else if (flags & NVS_WRITE_PRE_VERIFY) {
        /*
         *  If pre-verify, each destination byte must be able to be changed to the
         *  source byte (1s to 0s, not 0s to 1s).
         *  this is satisfied by the following test:
         *     src == (src & dst)
         */
        dstBuf = (uint8_t *)((uint32_t)(hwAttrs->regionBase) + offset);
        srcBuf = buffer;
        for (i = 0; i < bufferSize; i++) {
            if (srcBuf[i] != (srcBuf[i] & dstBuf[i])) {
                SemaphoreP_post(writeSem);
                return (NVS_STATUS_INV_WRITE);
            }
        }
    }

    srcBuf = buffer;
    dstBuf = (uint8_t *)((uint32_t)(hwAttrs->regionBase) + offset);

    unprotectMemory((uint32_t)((uint8_t *)hwAttrs->regionBase + offset),
                    (uint32_t)((uint8_t *)hwAttrs->regionBase + offset + bufferSize));

    status = programMemory((void *)srcBuf, (void *)dstBuf, bufferSize);

    protectMemory((uint32_t)((uint8_t *)hwAttrs->regionBase + offset),
                  (uint32_t)((uint8_t *)hwAttrs->regionBase + offset + bufferSize));

    if (status != true) {
        retval = NVS_STATUS_ERROR;
    }
    else if (flags & NVS_WRITE_POST_VERIFY) {
        /*
         *  Note: This validates the entire region even on erase mode.
         */
        for (i = 0; i < bufferSize; i++) {
            if (srcBuf[i] != dstBuf[i]) {
                retval = NVS_STATUS_ERROR;
                break;
            }
        }
    }

    SemaphoreP_post(writeSem);

    return (retval);
}

/*
 *  ======== checkEraseRange ========
 */
static int_fast16_t checkEraseRange(NVS_Handle handle, size_t offset, size_t size)
{
    NVSMSP432_HWAttrs const *hwAttrs = handle->hwAttrs;

    if (offset != (offset & SECTOR_BASE_MASK)) {
        return (NVS_STATUS_INV_ALIGNMENT);    /* poorly aligned start address */
    }

    if (offset >= hwAttrs->regionSize) {
        return (NVS_STATUS_INV_OFFSET);   /* offset is past end of region */
    }

    if (offset + size > hwAttrs->regionSize) {
        return (NVS_STATUS_INV_SIZE);     /* size is too big */
    }

    if (size != (size & SECTOR_BASE_MASK)) {
        return (NVS_STATUS_INV_SIZE);     /* size is not a multiple of sector size */
    }

    return (NVS_STATUS_SUCCESS);
}

/*
 *  ======== doErase ========
 */
static int_fast16_t doErase(NVS_Handle handle, size_t offset, size_t size)
{
    NVSMSP432_HWAttrs const *hwAttrs = handle->hwAttrs;
    uint32_t status = 0;
    uint32_t sectorBase;
    int_fast16_t rangeStatus;

    /* sanity test the erase args */
    rangeStatus = checkEraseRange(handle, offset, size);

    if (rangeStatus != NVS_STATUS_SUCCESS) {
        return (rangeStatus);
    }

    sectorBase = (uint32_t)hwAttrs->regionBase + offset;

    unprotectMemory(sectorBase, sectorBase + size);

    while (size) {
        status = eraseSector(sectorBase);

        if (status != true) {
            break;
        }

        sectorBase += FLASH_SECTOR_SIZE;
        size -= FLASH_SECTOR_SIZE;
    }

    protectMemory(sectorBase, sectorBase + size);

    if (status != true) {
        return (NVS_STATUS_ERROR);
    }

    return (NVS_STATUS_SUCCESS);
}

#if defined(DeviceFamily_MSP432P401x)

#define BANK0_END_ADDRESS (0x0001ffff)

#define GET_BANK(address) ((address > BANK0_END_ADDRESS) ? \
        FLASH_MAIN_MEMORY_SPACE_BANK1 : FLASH_MAIN_MEMORY_SPACE_BANK0)

static uint32_t flashSector[32] = {
    FLASH_SECTOR0,
    FLASH_SECTOR1,
    FLASH_SECTOR2,
    FLASH_SECTOR3,
    FLASH_SECTOR4,
    FLASH_SECTOR5,
    FLASH_SECTOR6,
    FLASH_SECTOR7,
    FLASH_SECTOR8,
    FLASH_SECTOR9,
    FLASH_SECTOR10,
    FLASH_SECTOR11,
    FLASH_SECTOR12,
    FLASH_SECTOR13,
    FLASH_SECTOR14,
    FLASH_SECTOR15,
    FLASH_SECTOR16,
    FLASH_SECTOR17,
    FLASH_SECTOR18,
    FLASH_SECTOR19,
    FLASH_SECTOR20,
    FLASH_SECTOR21,
    FLASH_SECTOR22,
    FLASH_SECTOR23,
    FLASH_SECTOR24,
    FLASH_SECTOR25,
    FLASH_SECTOR26,
    FLASH_SECTOR27,
    FLASH_SECTOR28,
    FLASH_SECTOR29,
    FLASH_SECTOR30,
    FLASH_SECTOR31,
};

/*
 *  ======== getFlashSector ========
 */
static uint32_t getFlashSector(uint32_t address)
{
    int index;

    index = (address & 0x1FFFF) >> 12;
    return (flashSector[index]);
}

/*
 *  ======== protectMemory ========
 */
static bool protectMemory(uint32_t startAddr, uint32_t endAddr)
{
    uint32_t sectorMask, bankNum;
    int32_t size = endAddr - startAddr;

    while (size > 0) {
        /* get sector info */
        sectorMask = getFlashSector(startAddr);
        bankNum = GET_BANK(startAddr);

        MAP_FlashCtl_protectSector(bankNum, sectorMask);

        startAddr += FLASH_SECTOR_SIZE;
        size -= FLASH_SECTOR_SIZE;
    }

    return (true);
}

/*
 *  ======== unprotectMemory ========
 */
static bool unprotectMemory(uint32_t startAddr, uint32_t endAddr)
{
    uint32_t sectorMask, bankNum;
    int32_t size = endAddr - startAddr;

    while (size > 0) {
        /* get sector info */
        sectorMask = getFlashSector(startAddr);
        bankNum = GET_BANK(startAddr);

        MAP_FlashCtl_unprotectSector(bankNum, sectorMask);

        startAddr += FLASH_SECTOR_SIZE;
        size -= FLASH_SECTOR_SIZE;
    }

    return (true);
}
#endif /* defined (DeviceFamily_MSP432P401x) */
