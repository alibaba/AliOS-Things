/* Created by plibgen $Revision: 1.31 $ */

#ifndef _DEVCON_P32MZ2048EFM124_H
#define _DEVCON_P32MZ2048EFM124_H

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

    DEVCON_ID_0 = 0,
    DEVCON_NUMBER_OF_MODULES = 1

} DEVCON_MODULE_ID;

typedef enum {

    DEVCON_CFGLOCK_NONE

} DEVCON_CFGLOCK;

/* Section 2 - Feature variant inclusion */

#define PLIB_TEMPLATE PLIB_INLINE
#include "../templates/devcon_AlternateClock_Default.h"
#include "../templates/devcon_DeviceRegsLockUnlock_PIC32MZ.h"
#include "../templates/devcon_OTPConfigLockUnlock_Unsupported.h"
#include "../templates/devcon_USB_PHY_SleepModeSet_Default.h"
#include "../templates/devcon_ECCModes_Default.h"
#include "../templates/devcon_JTAGEnable_Default.h"
#include "../templates/devcon_AnalogChargePumpControl_Default.h"
#include "../templates/devcon_TraceOutput_Default.h"
#include "../templates/devcon_JTAGUsesTDO_Default.h"
#include "../templates/devcon_IgnoreDebugFreeze_Unsupported.h"
#include "../templates/devcon_DeviceVerAndId_Default.h"
#include "../templates/devcon_SysLockUnlock_Default.h"
#include "../templates/devcon_MPLL_Unsupported.h"
#include "../templates/devcon_HSUARTControl_Unsupported.h"
#include "../templates/devcon_BootSelection_Unsupported.h"

/* Section 3 - PLIB dispatch function definitions */

PLIB_INLINE_API void PLIB_DEVCON_AlternateClockEnable(DEVCON_MODULE_ID index, DEVCON_ALT_CLOCK_TARGET targetAltClock)
{
     DEVCON_AlternateClockEnable_Default(index, targetAltClock);
}

PLIB_INLINE_API void PLIB_DEVCON_AlternateClockDisable(DEVCON_MODULE_ID index, DEVCON_ALT_CLOCK_TARGET targetAltClock)
{
     DEVCON_AlternateClockDisable_Default(index, targetAltClock);
}

PLIB_INLINE_API bool PLIB_DEVCON_ExistsAlternateClock(DEVCON_MODULE_ID index)
{
     return DEVCON_ExistsAlternateClock_Default(index);
}

PLIB_INLINE_API void PLIB_DEVCON_DeviceRegistersLock(DEVCON_MODULE_ID index, DEVCON_REGISTER_SET registersToLock)
{
     DEVCON_DeviceRegistersLock_PIC32MZ(index, registersToLock);
}

PLIB_INLINE_API void PLIB_DEVCON_DeviceRegistersUnlock(DEVCON_MODULE_ID index, DEVCON_REGISTER_SET registersToLock)
{
     DEVCON_DeviceRegistersUnlock_PIC32MZ(index, registersToLock);
}

PLIB_INLINE_API bool PLIB_DEVCON_ExistsDeviceRegsLockUnlock(DEVCON_MODULE_ID index)
{
     return DEVCON_ExistsDeviceRegsLockUnlock_PIC32MZ(index);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_DEVCON_OTPConfigLock(DEVCON_MODULE_ID index, DEVCON_CFGLOCK lockType)
{
     DEVCON_OTPConfigLock_Unsupported(index, lockType);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_DEVCON_OTPConfigUnlock(DEVCON_MODULE_ID index)
{
     DEVCON_OTPConfigUnlock_Unsupported(index);
}

PLIB_INLINE_API bool PLIB_DEVCON_ExistsOTPConfigLockUnlock(DEVCON_MODULE_ID index)
{
     return DEVCON_ExistsOTPConfigLockUnlock_Unsupported(index);
}

PLIB_INLINE_API void PLIB_DEVCON_USBPHYSleepModeSet(DEVCON_MODULE_ID index, DEVCON_USB_SLEEP_MODE sleepOrRun)
{
     DEVCON_USBPHYSleepModeSet_Default(index, sleepOrRun);
}

PLIB_INLINE_API bool PLIB_DEVCON_ExistsUSB_PHY_SleepModeSet(DEVCON_MODULE_ID index)
{
     return DEVCON_ExistsUSB_PHY_SleepModeSet_Default(index);
}

PLIB_INLINE_API void PLIB_DEVCON_FlashErrCorrectionModeSet(DEVCON_MODULE_ID index, DEVCON_ECC_CONFIG flashECC)
{
     DEVCON_FlashErrCorrectionModeSet_Default(index, flashECC);
}

PLIB_INLINE_API bool PLIB_DEVCON_ExistsECCModes(DEVCON_MODULE_ID index)
{
     return DEVCON_ExistsECCModes_Default(index);
}

PLIB_INLINE_API void PLIB_DEVCON_JTAGPortEnable(DEVCON_MODULE_ID index)
{
     DEVCON_JTAGPortEnable_Default(index);
}

PLIB_INLINE_API void PLIB_DEVCON_JTAGPortDisable(DEVCON_MODULE_ID index)
{
     DEVCON_JTAGPortDisable_Default(index);
}

PLIB_INLINE_API bool PLIB_DEVCON_ExistsJTAGEnable(DEVCON_MODULE_ID index)
{
     return DEVCON_ExistsJTAGEnable_Default(index);
}

PLIB_INLINE_API void PLIB_DEVCON_AnalogIOChargePumpEnable(DEVCON_MODULE_ID index)
{
     DEVCON_AnalogIOChargePumpEnable_Default(index);
}

PLIB_INLINE_API void PLIB_DEVCON_AnalogIOChargePumpDisable(DEVCON_MODULE_ID index)
{
     DEVCON_AnalogIOChargePumpDisable_Default(index);
}

PLIB_INLINE_API bool PLIB_DEVCON_ExistsAnalogChargePumpControl(DEVCON_MODULE_ID index)
{
     return DEVCON_ExistsAnalogChargePumpControl_Default(index);
}

PLIB_INLINE_API void PLIB_DEVCON_TraceOutputEnable(DEVCON_MODULE_ID index)
{
     DEVCON_TraceOutputEnable_Default(index);
}

PLIB_INLINE_API void PLIB_DEVCON_TraceOutputDisable(DEVCON_MODULE_ID index)
{
     DEVCON_TraceOutputDisable_Default(index);
}

PLIB_INLINE_API bool PLIB_DEVCON_ExistsTraceOutput(DEVCON_MODULE_ID index)
{
     return DEVCON_ExistsTraceOutput_Default(index);
}

PLIB_INLINE_API void PLIB_DEVCON_2WireJTAGEnableTDO(DEVCON_MODULE_ID index)
{
     DEVCON_2WireJTAGEnableTDO_Default(index);
}

PLIB_INLINE_API void PLIB_DEVCON_2WireJTAGDisableTDO(DEVCON_MODULE_ID index)
{
     DEVCON_2WireJTAGDisableTDO_Default(index);
}

PLIB_INLINE_API bool PLIB_DEVCON_ExistsJTAGUsesTDO(DEVCON_MODULE_ID index)
{
     return DEVCON_ExistsJTAGUsesTDO_Default(index);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_DEVCON_IgnoreDebugFreezeEnable(DEVCON_MODULE_ID index, DEVCON_DEBUG_PERIPHERAL myPeripheral)
{
     DEVCON_IgnoreDebugFreezeEnable_Unsupported(index, myPeripheral);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_DEVCON_IgnoreDebugFreezeDisable(DEVCON_MODULE_ID index, DEVCON_DEBUG_PERIPHERAL myPeripheral)
{
     DEVCON_IgnoreDebugFreezeDisable_Unsupported(index, myPeripheral);
}

PLIB_INLINE_API bool PLIB_DEVCON_ExistsIgnoreDebugFreeze(DEVCON_MODULE_ID index)
{
     return DEVCON_ExistsIgnoreDebugFreeze_Unsupported(index);
}

PLIB_INLINE_API uint8_t PLIB_DEVCON_DeviceVersionGet(DEVCON_MODULE_ID index)
{
     return DEVCON_DeviceVersionGet_Default(index);
}

PLIB_INLINE_API uint32_t PLIB_DEVCON_DeviceIdGet(DEVCON_MODULE_ID index)
{
     return DEVCON_DeviceIdGet_Default(index);
}

PLIB_INLINE_API bool PLIB_DEVCON_ExistsDeviceVerAndId(DEVCON_MODULE_ID index)
{
     return DEVCON_ExistsDeviceVerAndId_Default(index);
}

PLIB_INLINE_API void PLIB_DEVCON_SystemUnlock(DEVCON_MODULE_ID index)
{
     DEVCON_SystemUnlock_Default(index);
}

PLIB_INLINE_API void PLIB_DEVCON_SystemLock(DEVCON_MODULE_ID index)
{
     DEVCON_SystemLock_Default(index);
}

PLIB_INLINE_API bool PLIB_DEVCON_ExistsSystemLockUnlock(DEVCON_MODULE_ID index)
{
     return DEVCON_ExistsSystemLockUnlock_Default(index);
}

PLIB_INLINE_API bool _PLIB_UNSUPPORTED PLIB_DEVCON_MPLLIsReady(DEVCON_MODULE_ID index)
{
     return DEVCON_MPLLIsReady_Unsupported(index);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_DEVCON_MPLLEnable(DEVCON_MODULE_ID index)
{
     DEVCON_MPLLEnable_Unsupported(index);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_DEVCON_MPLLDisable(DEVCON_MODULE_ID index)
{
     DEVCON_MPLLDisable_Unsupported(index);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_DEVCON_MPLLODiv1Set(DEVCON_MODULE_ID index, DEVCON_MPLL_OUTPUT_DIVIDER bits)
{
     DEVCON_MPLLODiv1Set_Unsupported(index, bits);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_DEVCON_MPLLODiv2Set(DEVCON_MODULE_ID index, DEVCON_MPLL_OUTPUT_DIVIDER bits)
{
     DEVCON_MPLLODiv2Set_Unsupported(index, bits);
}

PLIB_INLINE_API bool _PLIB_UNSUPPORTED PLIB_DEVCON_MPLLVregIsReady(DEVCON_MODULE_ID index)
{
     return DEVCON_MPLLVregIsReady_Unsupported(index);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_DEVCON_MPLLVregEnable(DEVCON_MODULE_ID index)
{
     DEVCON_MPLLVregEnable_Unsupported(index);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_DEVCON_MPLLVregDisable(DEVCON_MODULE_ID index)
{
     DEVCON_MPLLVregDisable_Unsupported(index);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_DEVCON_MPLLMultiplierSet(DEVCON_MODULE_ID index, uint8_t value)
{
     DEVCON_MPLLMultiplierSet_Unsupported(index, value);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_DEVCON_MPLLVrefSet(DEVCON_MODULE_ID index, DEVCON_MPLL_VREF_CONTROL vref)
{
     DEVCON_MPLLVrefSet_Unsupported(index, vref);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_DEVCON_MPLLInputDivSet(DEVCON_MODULE_ID index, uint8_t value)
{
     DEVCON_MPLLInputDivSet_Unsupported(index, value);
}

PLIB_INLINE_API bool PLIB_DEVCON_ExistsMPLL(DEVCON_MODULE_ID index)
{
     return DEVCON_ExistsMPLL_Unsupported(index);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_DEVCON_HSUARTEnable(DEVCON_MODULE_ID index)
{
     DEVCON_HSUARTEnable_Unsupported(index);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_DEVCON_HSUARTDisable(DEVCON_MODULE_ID index)
{
     DEVCON_HSUARTDisable_Unsupported(index);
}

PLIB_INLINE_API bool PLIB_DEVCON_ExistsHSUARTControl(DEVCON_MODULE_ID index)
{
     return DEVCON_ExistsHSUARTControl_Unsupported(index);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_DEVCON_BootIPFSelect(DEVCON_MODULE_ID index)
{
     DEVCON_BootIPFSelect_Unsupported(index);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_DEVCON_BootExtSelect(DEVCON_MODULE_ID index)
{
     DEVCON_BootExtSelect_Unsupported(index);
}

PLIB_INLINE_API bool PLIB_DEVCON_ExistsBootSelection(DEVCON_MODULE_ID index)
{
     return DEVCON_ExistsBootSelection_Unsupported(index);
}

#endif
