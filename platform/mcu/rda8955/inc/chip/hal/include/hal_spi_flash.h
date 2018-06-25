/* Copyright (C) 2017 RDA Technologies Limited and/or its affiliates("RDA").
* All rights reserved.
*
* This software is supplied "AS IS" without any warranties.
* RDA assumes no responsibility or liability for the use of the software,
* conveys no license or title under any patent, copyright, or mask work
* right to the product. RDA reserves the right to make changes in the
* software without notification.  RDA also make no representation or
* warranty that such application will be suitable for the specified use
* without further testing or modification.
*/

#ifndef _HAL_SPI_FLASH_H_
#define _HAL_SPI_FLASH_H_

#include "cs_types.h"
#include "hal_sys.h"

// =============================================================================
// hal_SpiFlashLock/Unlock
// -----------------------------------------------------------------------------
/// Apply/Release access to spi flash.
// =============================================================================
void hal_SpiFlashLock(void);
void hal_SpiFlashUnlock(void);

// =============================================================================
// hal_SpiFlashOpen
// -----------------------------------------------------------------------------
/// Initialize SPI FLASH (configure FLASH clock, read ID and etc). Will
/// configure clock, SPI controller and reset FLASH.
// =============================================================================
void hal_SpiFlashOpen(void);

// =============================================================================
// hal_SpiFlashGetID
// -----------------------------------------------------------------------------
/// Get FLASH ID (it will be read once at Init).
/// id[23:16]: capacity
/// id[15:8]: memory type
/// id[7:0]: manufacturer
// =============================================================================
uint32_t hal_SpiFlashGetID(void);

// =============================================================================
// hal_SpiFlashSetLockCallback/hal_SpiFlashSetUnlockCallback/hal_SpiFlashSetCheckIrq
// -----------------------------------------------------------------------------
/// Set FLASH multi-task lock/unlock callback, and whether to check pending IRQ
/// during wait WIP finish. Usually, they will be called once after OS is
/// started and before multi-task FLASH operations.
// =============================================================================
void hal_SpiFlashSetLockCallback(HAL_CALLBACK_FUNCTION_T lock, void *param);
void hal_SpiFlashSetUnlockCallback(HAL_CALLBACK_FUNCTION_T unlock, void *param);
void hal_SpiFlashSetCheckIrq(bool checkirq);

// =============================================================================
// hal_SpiFlashMapUncacheAddress/hal_SpiFlashOffset
// -----------------------------------------------------------------------------
/// Map between FLASH offset and CPU address.
// =============================================================================
void *hal_SpiFlashMapUncacheAddress(uint32_t flashAddress);
uint32_t hal_SpiFlashOffset(void *address);

// =============================================================================
// hal_SpiFlashWrite
// -----------------------------------------------------------------------------
/// Write data to FLASH. To get correct result, it is needed to erase FLASH
/// before write.
///
/// Optionally, it can check IRQ during wait. When there are pending IRQ,
/// SUSPEND Will be called before enable IRQ, and when the task of function
/// is scheduled again, it will call RESUME.
///
/// It will return false only when parameters are invalid.
// =============================================================================
bool hal_SpiFlashWrite(uint32_t flashAddress, const void *data, unsigned size);

// =============================================================================
// hal_SpiFlashErase
// -----------------------------------------------------------------------------
/// Erase the FLASH range. The range should be sector aligned. It will return
/// false only when parameters are invalid.
// =============================================================================
bool hal_SpiFlashErase(uint32_t flashAddress, unsigned size);

// =============================================================================
// hal_SpiFlashChipErase
// -----------------------------------------------------------------------------
/// May be it only needed for RAMRUN
// =============================================================================
void hal_SpiFlashChipErase(void);

// =============================================================================
// hal_SpiFlashSleep/hal_SpiFlashWakeup/hal_SpiFlashReset
// -----------------------------------------------------------------------------
/// Sleep will save FLASH power consumption.
// =============================================================================
void hal_SpiFlashSleep(void);
void hal_SpiFlashWakeup(void);
void hal_SpiFlashReset(void);

// =============================================================================
// hal_SpiFlashSectorErase/hal_SpiFlashBlockErase
// -----------------------------------------------------------------------------
/// Most likely, they won't be called. There are no parameter check, and no
/// multi-task lock.
// =============================================================================
void hal_SpiFlashSectorErase(uint32_t flashAddress);
void hal_SpiFlashBlockErase(uint32_t flashAddress);

// =============================================================================
// hal_SpiFlashGetPageSize/Sector/Block/Capacity
// -----------------------------------------------------------------------------
/// Get size information
// =============================================================================
unsigned hal_SpiFlashGetPageSize(void);
unsigned hal_SpiFlashGetSectorSize(void);
unsigned hal_SpiFlashGetBlockSize(void);
unsigned hal_SpiFlashGetSize(void);

// =============================================================================
// hal_SpiFlashReadStatusRegister/hal_SpiFlashWriteStatusRegister
// -----------------------------------------------------------------------------
/// Read and Write SR1 and SR2.
// =============================================================================
uint16_t hal_SpiFlashReadStatusRegister(void);
void hal_SpiFlashWriteStatusRegister(uint16_t val, uint16_t mask);

// =============================================================================
// hal_SpiFlashReadStatusRegisterEx/hal_SpiFlashWriteStatusRegisterEx/hal_SpiFlashWriteVolatileStatusRegisterEx
// -----------------------------------------------------------------------------
/// Read and write SR1/SR2/SR3. Not all FLASH support this. 'n' should be 1/2/3.
// =============================================================================
uint8_t hal_SpiFlashReadStatusRegisterEx(uint8_t n);
void hal_SpiFlashWriteStatusRegisterEx(uint8_t n, uint8_t val);
void hal_SpiFlashWriteVolatileStatusRegisterEx(uint8_t n, uint8_t val);

// =============================================================================
// hal_SpiFlashSecurityErase/Write/Read
// -----------------------------------------------------------------------------
/// Operations for security register. Security register feature won't be checked.
/// Caller should provide address[23:0] according to datasheet.
// =============================================================================
void hal_SpiFlashSecurityErase(uint32_t address);
void hal_SpiFlashSecurityWrite(uint32_t address, const void *data, unsigned size);
void hal_SpiFlashSecurityRead(uint32_t address, void *data, unsigned size);

#endif
