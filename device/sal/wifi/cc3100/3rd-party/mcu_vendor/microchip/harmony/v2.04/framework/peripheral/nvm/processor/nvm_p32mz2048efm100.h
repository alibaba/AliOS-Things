/* Created by plibgen $Revision: 1.31 $ */

#ifndef _NVM_P32MZ2048EFM100_H
#define _NVM_P32MZ2048EFM100_H

/* Section 1 - Enumerate instances, define constants, VREGs */

#include <xc.h>
#include <stdbool.h>

#include "peripheral/peripheral_common_32bit.h"

/* Default definition used for all API dispatch functions */
#ifndef PLIB_INLINE_API
    #define PLIB_INLINE_API extern inline
#endif

/* Default definition used for all other functions */
#ifndef PLIB_INLINE
    #define PLIB_INLINE extern inline
#endif

typedef enum {

    NVM_ID_0 = _NVM_BASE_ADDRESS,
    NVM_NUMBER_OF_MODULES = 1

} NVM_MODULE_ID;

typedef enum {

    NVM_UNLOCK_KEY1 = 0xAA996655,
    NVM_UNLOCK_KEY2 = 0x556699AA

} NVM_UNLOCK_KEYS;

typedef enum {

    NVM_ROW_SIZE = 0x800,
    NVM_PAGE_SIZE = 0x4000

} NVM_ROW_PAGE_SIZE;

typedef enum {

    WORD_PROGRAM_OPERATION = 0x1,
    ROW_PROGRAM_OPERATION = 0x3,
    PAGE_ERASE_OPERATION = 0x4,
    FLASH_ERASE_OPERATION = 0x7,
    UPPER_FLASH_REGION_ERASE_OPERATION = 0x6,
    LOWER_FLASH_REGION_ERASE_OPERATION = 0x5,
    QUAD_WORD_PROGRAM_OPERATION = 0x2,
    NO_OPERATION = 0x0

} NVM_OPERATION_MODE;

typedef enum {

    LOWER_BOOT_ALIAS_AREA = 0x8,
    UPPER_BOOT_ALIAS_AREA = 0x0

} NVM_BOOT_MEMORY_AREA;

typedef enum {

    LOWER_BOOT_ALIAS_PAGE4 = 0xC,
    LOWER_BOOT_ALIAS_PAGE3 = 0xB,
    LOWER_BOOT_ALIAS_PAGE2 = 0xA,
    LOWER_BOOT_ALIAS_PAGE1 = 0x9,
    LOWER_BOOT_ALIAS_PAGE0 = 0x8,
    UPPER_BOOT_ALIAS_PAGE4 = 0x4,
    UPPER_BOOT_ALIAS_PAGE3 = 0x3,
    UPPER_BOOT_ALIAS_PAGE2 = 0x2,
    UPPER_BOOT_ALIAS_PAGE1 = 0x1,
    UPPER_BOOT_ALIAS_PAGE0 = 0x0

} NVM_BOOT_MEMORY_PAGE;

typedef enum {

    NVM_FLASH_SWAP_UNLOCKED = 0x0,
    NVM_FLASH_SWAP_LOCKED = 0x1,
    NVM_FLASH_SWAP_LOCKED_UNTIL_RESET = 0x3

} NVM_FLASH_SWAP_LOCK_TYPE;

typedef enum {

    EEPROM_OPERATION_MODE_NONE

} EEPROM_OPERATION_MODE;

typedef enum {

    EEPROM_ERROR_NONE

} EEPROM_ERROR;

typedef enum {

    EEPROM_CALIBRATION_REG_NONE

} EEPROM_CALIBRATION_REG;

/* Section 2 - Feature variant inclusion */

#define PLIB_TEMPLATE PLIB_INLINE
#include "../templates/nvm_WriteErrorStatus_Default.h"
#include "../templates/nvm_MemoryModificationControl_Default.h"
#include "../templates/nvm_OperationMode_Default.h"
#include "../templates/nvm_AddressModifyControl_Default.h"
#include "../templates/nvm_ProvideData_Default.h"
#include "../templates/nvm_WriteOperation_Default.h"
#include "../templates/nvm_KeySequence_Default.h"
#include "../templates/nvm_SourceAddress_Default.h"
#include "../templates/nvm_LowVoltageStatus_Unsupported.h"
#include "../templates/nvm_LowVoltageError_Default.h"
#include "../templates/nvm_FlashBankRegionSelect_Default.h"
#include "../templates/nvm_BootFlashBankRegion_Default.h"
#include "../templates/nvm_ProvideQuadData_Default.h"
#include "../templates/nvm_LockPFMSelect_Default.h"
#include "../templates/nvm_FlashWPMemoryRangeProvide_Default.h"
#include "../templates/nvm_LockBootSelect_Default.h"
#include "../templates/nvm_BootPageWriteProtect_Default.h"
#include "../templates/nvm_SwapLockControl_Default.h"
#include "../templates/nvm_EEPROMEnableControl_Unsupported.h"
#include "../templates/nvm_EEPROMStopInIdleControl_Unsupported.h"
#include "../templates/nvm_EEPROMOperationModeControl_Unsupported.h"
#include "../templates/nvm_EEPROMAddressControl_Unsupported.h"
#include "../templates/nvm_EEPROMDataControl_Unsupported.h"
#include "../templates/nvm_EEPROMKeySequence_Unsupported.h"
#include "../templates/nvm_EEPROMEnableOperationControl_Unsupported.h"
#include "../templates/nvm_EEPROMStartOperationControl_Unsupported.h"
#include "../templates/nvm_EEPROMLongWriteStatus_Unsupported.h"
#include "../templates/nvm_EEPROMOperationAbortControl_Unsupported.h"
#include "../templates/nvm_EEPROMErrorStatus_Unsupported.h"
#include "../templates/nvm_EEPROMCalibrationData_Unsupported.h"
#include "../templates/nvm_EEPROMWaitStates_Unsupported.h"

/* Section 3 - PLIB dispatch function definitions */

PLIB_INLINE_API bool PLIB_NVM_ExistsWriteErrorStatus(NVM_MODULE_ID index)
{
     return NVM_ExistsWriteErrorStatus_Default(index);
}

PLIB_INLINE_API bool PLIB_NVM_WriteOperationHasTerminated(NVM_MODULE_ID index)
{
     return NVM_WriteOperationHasTerminated_Default(index);
}

PLIB_INLINE_API bool PLIB_NVM_ExistsMemoryModificationControl(NVM_MODULE_ID index)
{
     return NVM_ExistsMemoryModificationControl_Default(index);
}

PLIB_INLINE_API void PLIB_NVM_MemoryModifyEnable(NVM_MODULE_ID index)
{
     NVM_MemoryModifyEnable_Default(index);
}

PLIB_INLINE_API void PLIB_NVM_MemoryModifyInhibit(NVM_MODULE_ID index)
{
     NVM_MemoryModifyInhibit_Default(index);
}

PLIB_INLINE_API bool PLIB_NVM_ExistsOperationMode(NVM_MODULE_ID index)
{
     return NVM_ExistsOperationMode_Default(index);
}

PLIB_INLINE_API void PLIB_NVM_MemoryOperationSelect(NVM_MODULE_ID index, NVM_OPERATION_MODE operationmode)
{
     NVM_MemoryOperationSelect_Default(index, operationmode);
}

PLIB_INLINE_API bool PLIB_NVM_ExistsAddressModifyControl(NVM_MODULE_ID index)
{
     return NVM_ExistsAddressModifyControl_Default(index);
}

PLIB_INLINE_API void PLIB_NVM_FlashAddressToModify(NVM_MODULE_ID index, uint32_t address)
{
     NVM_FlashAddressToModify_Default(index, address);
}

PLIB_INLINE_API bool PLIB_NVM_ExistsProvideData(NVM_MODULE_ID index)
{
     return NVM_ExistsProvideData_Default(index);
}

PLIB_INLINE_API void PLIB_NVM_FlashProvideData(NVM_MODULE_ID index, uint32_t data)
{
     NVM_FlashProvideData_Default(index, data);
}

PLIB_INLINE_API bool PLIB_NVM_ExistsWriteOperation(NVM_MODULE_ID index)
{
     return NVM_ExistsWriteOperation_Default(index);
}

PLIB_INLINE_API uint32_t PLIB_NVM_FlashRead(NVM_MODULE_ID index, uint32_t address)
{
     return NVM_FlashRead_Default(index, address);
}

PLIB_INLINE_API void PLIB_NVM_FlashWriteStart(NVM_MODULE_ID index)
{
     NVM_FlashWriteStart_Default(index);
}

PLIB_INLINE_API void PLIB_NVM_FlashEraseStart(NVM_MODULE_ID index)
{
     NVM_FlashEraseStart_Default(index);
}

PLIB_INLINE_API bool PLIB_NVM_FlashWriteCycleHasCompleted(NVM_MODULE_ID index)
{
     return NVM_FlashWriteCycleHasCompleted_Default(index);
}

PLIB_INLINE_API bool PLIB_NVM_ExistsKeySequence(NVM_MODULE_ID index)
{
     return NVM_ExistsKeySequence_Default(index);
}

PLIB_INLINE_API void PLIB_NVM_FlashWriteKeySequence(NVM_MODULE_ID index, uint32_t keysequence)
{
     NVM_FlashWriteKeySequence_Default(index, keysequence);
}

PLIB_INLINE_API bool PLIB_NVM_ExistsSourceAddress(NVM_MODULE_ID index)
{
     return NVM_ExistsSourceAddress_Default(index);
}

PLIB_INLINE_API void PLIB_NVM_DataBlockSourceAddress(NVM_MODULE_ID index, uint32_t address)
{
     NVM_DataBlockSourceAddress_Default(index, address);
}

PLIB_INLINE_API bool PLIB_NVM_ExistsLowVoltageStatus(NVM_MODULE_ID index)
{
     return NVM_ExistsLowVoltageStatus_Unsupported(index);
}

PLIB_INLINE_API bool _PLIB_UNSUPPORTED PLIB_NVM_LowVoltageEventIsActive(NVM_MODULE_ID index)
{
     return NVM_LowVoltageEventIsActive_Unsupported(index);
}

PLIB_INLINE_API bool PLIB_NVM_ExistsLowVoltageError(NVM_MODULE_ID index)
{
     return NVM_ExistsLowVoltageError_Default(index);
}

PLIB_INLINE_API bool PLIB_NVM_LowVoltageIsDetected(NVM_MODULE_ID index)
{
     return NVM_LowVoltageIsDetected_Default(index);
}

PLIB_INLINE_API bool PLIB_NVM_ExistsFlashBankRegionSelect(NVM_MODULE_ID index)
{
     return NVM_ExistsFlashBankRegionSelect_Default(index);
}

PLIB_INLINE_API void PLIB_NVM_ProgramFlashBank1LowerRegion(NVM_MODULE_ID index)
{
     NVM_ProgramFlashBank1LowerRegion_Default(index);
}

PLIB_INLINE_API void PLIB_NVM_ProgramFlashBank2LowerRegion(NVM_MODULE_ID index)
{
     NVM_ProgramFlashBank2LowerRegion_Default(index);
}

PLIB_INLINE_API bool PLIB_NVM_ProgramFlashBank2IsLowerRegion(NVM_MODULE_ID index)
{
     return NVM_ProgramFlashBank2IsLowerRegion_Default(index);
}

PLIB_INLINE_API bool PLIB_NVM_ExistsBootFlashBankRegion(NVM_MODULE_ID index)
{
     return NVM_ExistsBootFlashBankRegion_Default(index);
}

PLIB_INLINE_API void PLIB_NVM_BootFlashBank1LowerRegion(NVM_MODULE_ID index)
{
     NVM_BootFlashBank1LowerRegion_Default(index);
}

PLIB_INLINE_API void PLIB_NVM_BootFlashBank2LowerRegion(NVM_MODULE_ID index)
{
     NVM_BootFlashBank2LowerRegion_Default(index);
}

PLIB_INLINE_API bool PLIB_NVM_BootFlashBank2IsLowerRegion(NVM_MODULE_ID index)
{
     return NVM_BootFlashBank2IsLowerRegion_Default(index);
}

PLIB_INLINE_API bool PLIB_NVM_ExistsProvideQuadData(NVM_MODULE_ID index)
{
     return NVM_ExistsProvideQuadData_Default(index);
}

PLIB_INLINE_API void PLIB_NVM_FlashProvideQuadData(NVM_MODULE_ID index, uint32_t* data)
{
     NVM_FlashProvideQuadData_Default(index, data);
}

PLIB_INLINE_API bool PLIB_NVM_ExistsLockPFMSelect(NVM_MODULE_ID index)
{
     return NVM_ExistsLockPFMSelect_Default(index);
}

PLIB_INLINE_API void PLIB_NVM_LockProgramFlashMemory(NVM_MODULE_ID index)
{
     NVM_LockProgramFlashMemory_Default(index);
}

PLIB_INLINE_API bool PLIB_NVM_IsProgramFlashMemoryLocked(NVM_MODULE_ID index)
{
     return NVM_IsProgramFlashMemoryLocked_Default(index);
}

PLIB_INLINE_API bool PLIB_NVM_ExistsFlashWPMemoryRangeProvide(NVM_MODULE_ID index)
{
     return NVM_ExistsFlashWPMemoryRangeProvide_Default(index);
}

PLIB_INLINE_API void PLIB_NVM_FlashWriteProtectMemoryAreaRange(NVM_MODULE_ID index, uint32_t address)
{
     NVM_FlashWriteProtectMemoryAreaRange_Default(index, address);
}

PLIB_INLINE_API bool PLIB_NVM_ExistsLockBootSelect(NVM_MODULE_ID index)
{
     return NVM_ExistsLockBootSelect_Default(index);
}

PLIB_INLINE_API void PLIB_NVM_LockBootMemory(NVM_MODULE_ID index, NVM_BOOT_MEMORY_AREA memoryArea)
{
     NVM_LockBootMemory_Default(index, memoryArea);
}

PLIB_INLINE_API bool PLIB_NVM_IsBootMemoryLocked(NVM_MODULE_ID index, NVM_BOOT_MEMORY_AREA memoryArea)
{
     return NVM_IsBootMemoryLocked_Default(index, memoryArea);
}

PLIB_INLINE_API bool PLIB_NVM_ExistsBootPageWriteProtect(NVM_MODULE_ID index)
{
     return NVM_ExistsBootPageWriteProtect_Default(index);
}

PLIB_INLINE_API void PLIB_NVM_BootPageWriteProtectionEnable(NVM_MODULE_ID index, NVM_BOOT_MEMORY_PAGE bootPage)
{
     NVM_BootPageWriteProtectionEnable_Default(index, bootPage);
}

PLIB_INLINE_API void PLIB_NVM_BootPageWriteProtectionDisable(NVM_MODULE_ID index, NVM_BOOT_MEMORY_PAGE bootPage)
{
     NVM_BootPageWriteProtectionDisable_Default(index, bootPage);
}

PLIB_INLINE_API bool PLIB_NVM_IsBootPageWriteProtected(NVM_MODULE_ID index, NVM_BOOT_MEMORY_PAGE bootPage)
{
     return NVM_IsBootPageWriteProtected_Default(index, bootPage);
}

PLIB_INLINE_API bool PLIB_NVM_ExistsSwapLockControl(NVM_MODULE_ID index)
{
     return NVM_ExistsSwapLockControl_Default(index);
}

PLIB_INLINE_API void PLIB_NVM_FlashSwapLockSelect(NVM_MODULE_ID index, NVM_FLASH_SWAP_LOCK_TYPE lockType)
{
     NVM_FlashSwapLockSelect_Default(index, lockType);
}

PLIB_INLINE_API NVM_FLASH_SWAP_LOCK_TYPE PLIB_NVM_FlashSwapLockStatusGet(NVM_MODULE_ID index)
{
     return NVM_FlashSwapLockStatusGet_Default(index);
}

PLIB_INLINE_API bool PLIB_NVM_ExistsEEPROMEnableControl(NVM_MODULE_ID index)
{
     return NVM_ExistsEEPROMEnableControl_Unsupported(index);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_NVM_EEPROMEnable(NVM_MODULE_ID index)
{
     NVM_EEPROMEnable_Unsupported(index);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_NVM_EEPROMDisable(NVM_MODULE_ID index)
{
     NVM_EEPROMDisable_Unsupported(index);
}

PLIB_INLINE_API bool _PLIB_UNSUPPORTED PLIB_NVM_EEPROMIsReady(NVM_MODULE_ID index)
{
     return NVM_EEPROMIsReady_Unsupported(index);
}

PLIB_INLINE_API bool PLIB_NVM_ExistsEEPROMStopInIdleControl(NVM_MODULE_ID index)
{
     return NVM_ExistsEEPROMStopInIdleControl_Unsupported(index);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_NVM_EEPROMStopInIdleEnable(NVM_MODULE_ID index)
{
     NVM_EEPROMStopInIdleEnable_Unsupported(index);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_NVM_EEPROMStopInIdleDisable(NVM_MODULE_ID index)
{
     NVM_EEPROMStopInIdleDisable_Unsupported(index);
}

PLIB_INLINE_API bool _PLIB_UNSUPPORTED PLIB_NVM_EEPROMStopInIdleIsEnabled(NVM_MODULE_ID index)
{
     return NVM_EEPROMStopInIdleIsEnabled_Unsupported(index);
}

PLIB_INLINE_API bool PLIB_NVM_ExistsEEPROMOperationModeControl(NVM_MODULE_ID index)
{
     return NVM_ExistsEEPROMOperationModeControl_Unsupported(index);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_NVM_EEPROMOperationSelect(NVM_MODULE_ID index, EEPROM_OPERATION_MODE mode)
{
     NVM_EEPROMOperationSelect_Unsupported(index, mode);
}

PLIB_INLINE_API bool PLIB_NVM_ExistsEEPROMAddressControl(NVM_MODULE_ID index)
{
     return NVM_ExistsEEPROMAddressControl_Unsupported(index);
}

PLIB_INLINE_API bool _PLIB_UNSUPPORTED PLIB_NVM_EEPROMAddress(NVM_MODULE_ID index, uint32_t address)
{
     return NVM_EEPROMAddress_Unsupported(index, address);
}

PLIB_INLINE_API bool PLIB_NVM_ExistsEEPROMDataControl(NVM_MODULE_ID index)
{
     return NVM_ExistsEEPROMDataControl_Unsupported(index);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_NVM_EEPROMDataToWrite(NVM_MODULE_ID index, uint32_t data)
{
     NVM_EEPROMDataToWrite_Unsupported(index, data);
}

PLIB_INLINE_API uint32_t _PLIB_UNSUPPORTED PLIB_NVM_EEPROMRead(NVM_MODULE_ID index)
{
     return NVM_EEPROMRead_Unsupported(index);
}

PLIB_INLINE_API bool PLIB_NVM_ExistsEEPROMKeySequence(NVM_MODULE_ID index)
{
     return NVM_ExistsEEPROMKeySequence_Unsupported(index);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_NVM_EEPROMKeySequenceWrite(NVM_MODULE_ID index, uint32_t keysequence)
{
     NVM_EEPROMKeySequenceWrite_Unsupported(index, keysequence);
}

PLIB_INLINE_API bool PLIB_NVM_ExistsEEPROMEnableOperationControl(NVM_MODULE_ID index)
{
     return NVM_ExistsEEPROMEnableOperationControl_Unsupported(index);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_NVM_EEPROMWriteEnable(NVM_MODULE_ID index)
{
     NVM_EEPROMWriteEnable_Unsupported(index);
}

PLIB_INLINE_API bool _PLIB_UNSUPPORTED PLIB_NVM_EEPROMWriteIsEnabled(NVM_MODULE_ID index)
{
     return NVM_EEPROMWriteIsEnabled_Unsupported(index);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_NVM_EEPROMReadEnable(NVM_MODULE_ID index)
{
     NVM_EEPROMReadEnable_Unsupported(index);
}

PLIB_INLINE_API bool _PLIB_UNSUPPORTED PLIB_NVM_EEPROMReadIsEnabled(NVM_MODULE_ID index)
{
     return NVM_EEPROMReadIsEnabled_Unsupported(index);
}

PLIB_INLINE_API bool PLIB_NVM_ExistsEEPROMStartOperationControl(NVM_MODULE_ID index)
{
     return NVM_ExistsEEPROMStartOperationControl_Unsupported(index);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_NVM_EEPROMReadStart(NVM_MODULE_ID index)
{
     NVM_EEPROMReadStart_Unsupported(index);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_NVM_EEPROMWriteStart(NVM_MODULE_ID index)
{
     NVM_EEPROMWriteStart_Unsupported(index);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_NVM_EEPROMEraseStart(NVM_MODULE_ID index)
{
     NVM_EEPROMEraseStart_Unsupported(index);
}

PLIB_INLINE_API bool _PLIB_UNSUPPORTED PLIB_NVM_EEPROMOperationHasCompleted(NVM_MODULE_ID index)
{
     return NVM_EEPROMOperationHasCompleted_Unsupported(index);
}

PLIB_INLINE_API bool PLIB_NVM_ExistsEEPROMLongWriteStatus(NVM_MODULE_ID index)
{
     return NVM_ExistsEEPROMLongWriteStatus_Unsupported(index);
}

PLIB_INLINE_API bool _PLIB_UNSUPPORTED PLIB_NVM_EEPROMNextWriteCycleIsLong(NVM_MODULE_ID index)
{
     return NVM_EEPROMNextWriteCycleIsLong_Unsupported(index);
}

PLIB_INLINE_API bool PLIB_NVM_ExistsEEPROMOperationAbortControl(NVM_MODULE_ID index)
{
     return NVM_ExistsEEPROMOperationAbortControl_Unsupported(index);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_NVM_EEPROMOperationAbort(NVM_MODULE_ID index)
{
     NVM_EEPROMOperationAbort_Unsupported(index);
}

PLIB_INLINE_API bool PLIB_NVM_ExistsEEPROMErrorStatus(NVM_MODULE_ID index)
{
     return NVM_ExistsEEPROMErrorStatus_Unsupported(index);
}

PLIB_INLINE_API EEPROM_ERROR _PLIB_UNSUPPORTED PLIB_NVM_EEPROMErrorGet(NVM_MODULE_ID index)
{
     return NVM_EEPROMErrorGet_Unsupported(index);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_NVM_EEPROMErrorClear(NVM_MODULE_ID index)
{
     NVM_EEPROMErrorClear_Unsupported(index);
}

PLIB_INLINE_API bool PLIB_NVM_ExistsEEPROMCalibrationData(NVM_MODULE_ID index)
{
     return NVM_ExistsEEPROMCalibrationData_Unsupported(index);
}

PLIB_INLINE_API uint32_t _PLIB_UNSUPPORTED PLIB_NVM_EEPROMReadCalibrationData(NVM_MODULE_ID index, EEPROM_CALIBRATION_REG regIndex)
{
     return NVM_EEPROMReadCalibrationData_Unsupported(index, regIndex);
}

PLIB_INLINE_API bool PLIB_NVM_ExistsEEPROMWaitStates(NVM_MODULE_ID index)
{
     return NVM_ExistsEEPROMWaitStates_Unsupported(index);
}

PLIB_INLINE_API uint8_t _PLIB_UNSUPPORTED PLIB_NVM_EEPROMWaitStatesGet(NVM_MODULE_ID index)
{
     return NVM_EEPROMWaitStatesGet_Unsupported(index);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_NVM_EEPROMWaitStatesSet(NVM_MODULE_ID index, uint8_t eews)
{
     NVM_EEPROMWaitStatesSet_Unsupported(index, eews);
}

#endif
