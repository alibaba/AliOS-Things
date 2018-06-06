/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
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


#ifndef _HAL_SYS_H_
#define _HAL_SYS_H_

#include "cs_types.h"

#include "hal_sys_cfg.h"
/// @defgroup system HAL Sytem Driver
/// description
/// ...
/// @{



// =============================================================================
//  MACROS
// =============================================================================

// Chip product IDs

#define HAL_SYS_CHIP_PROD_ID_8808       0x8808

#define HAL_SYS_CHIP_PROD_ID_8808S      0x880A

#define HAL_SYS_CHIP_PROD_ID_8809       0x8809

#define HAL_SYS_CHIP_PROD_ID_8810       0x8810

#define HAL_SYS_CHIP_PROD_ID_8810E      0x810E

#define HAL_SYS_CHIP_PROD_ID_8820       0x8820

#define HAL_SYS_CHIP_PROD_ID_8850       0x8850


#ifdef BIG_ENDIAN

#define HAL_ENDIAN_LITTLE_32(X) (((((X) >> 24) & 0xFF) <<  0)           \
                                |((((X) >> 16) & 0xFF) <<  8)           \
                                |((((X) >>  8) & 0xFF) << 16)           \
                                |((((X) >>  0) & 0xFF) << 24))

#define HAL_ENDIAN_LITTLE_24(X) (((((X) >> 16) & 0xFF) <<  0)           \
                                |((((X) >>  8) & 0xFF) <<  8)           \
                                |((((X) >>  0) & 0xFF) << 16))

#define HAL_ENDIAN_LITTLE_16(X) (((((X) >>  8) & 0xFF) <<  0)           \
                                |((((X) >>  0) & 0xFF) <<  8))

#define HAL_ENDIAN_BIG_32(X)    (X)

#define HAL_ENDIAN_BIG_24(X)    (X)

#define HAL_ENDIAN_BIG_16(X)    (X)

#else

#define HAL_ENDIAN_BIG_32(X)    (((((X) >> 24) & 0xFF) <<  0)           \
                                |((((X) >> 16) & 0xFF) <<  8)           \
                                |((((X) >>  8) & 0xFF) << 16)           \
                                |((((X) >>  0) & 0xFF) << 24))

#define HAL_ENDIAN_BIG_24(X)    (((((X) >> 16) & 0xFF) <<  0)           \
                                |((((X) >>  8) & 0xFF) <<  8)           \
                                |((((X) >>  0) & 0xFF) << 16))

#define HAL_ENDIAN_BIG_16(X)    (((((X) >>  8) & 0xFF) <<  0)           \
                                |((((X) >>  0) & 0xFF) <<  8))

#define HAL_ENDIAN_LITTLE_32(X) (X)

#define HAL_ENDIAN_LITTLE_24(X) (X)

#define HAL_ENDIAN_LITTLE_16(X) (X)

#endif /* BIG_ENDIAN */


// =============================================================================
// HAL_SYS_CRITICAL_SECTION_ADDRESS
// -----------------------------------------------------------------------------
/// Address of the interrupt enable semaphore, used to control the critical
/// section.
// =============================================================================
#define HAL_SYS_CRITICAL_SECTION_ADDRESS  CHIP_SYS_CRITICAL_SECTION_REG


// =============================================================================
// HAL_SYS_GET_CACHED_ADDR
// -----------------------------------------------------------------------------
/// That macro is used to get the cached version of a given address
// =============================================================================
#define HAL_SYS_GET_CACHED_ADDR(addr)  (((UINT32)(addr))&0xdfffffff)


// =============================================================================
// HAL_SYS_GET_UNCACHED_ADDR
// -----------------------------------------------------------------------------
/// That macro is used to get the uncached version of a given address
// =============================================================================
#define HAL_SYS_GET_UNCACHED_ADDR(addr) (((UINT32)(addr))|0x20000000)


// =============================================================================
// HAL_SYS_GET_RA
// -----------------------------------------------------------------------------
/// That macro puts in store the current value of the RA register. It should
/// be called at the beginning of the function (before any other function call)
/// to ensure the correctness of the result.
///
/// @param store The return value of the current function
// =============================================================================
#define HAL_SYS_GET_RA(store) asm volatile("move %0, $31":"=r"((store)))


// =============================================================================
// HAL_SYS_GET_SP
// -----------------------------------------------------------------------------
/// That macro puts in store the current value of the SP register.
///
/// @param store The return value of the current function
// =============================================================================
#define HAL_SYS_GET_SP(store) asm volatile("move %0, $29":"=r"((store)))


// =============================================================================
// HAL_SYS_GET_K1
// -----------------------------------------------------------------------------
/// That macro puts in store the current value of the K1 register.
///
/// @param store The return value of the current function
// =============================================================================
#define HAL_SYS_GET_K1(store) asm volatile("move %0, $27":"=r"((store)))


// =============================================================================
// HAL_SYS_LOOP_DELAY
// -----------------------------------------------------------------------------
/// That macro delay dedicated number of cpu instruction cycles
///
/// @param delay cycles
// =============================================================================
#define HAL_SYS_LOOP_DELAY(loops)  \
{ \
   register UINT32 _loops = loops; \
    __asm__ __volatile__ ( \
    ".set\tnoreorder\n"  \
    "1:\tsubu\t%0,1\n\t" \
    "bnez\t%0,1b\n\t" \
    "nop\n" \
    ".set\treorder" \
    :"=r" (_loops) \
    :"0" (_loops)); \
}


//mips virtual memory cacheduncached offset
#define HAL_SYS_CACHED_UNCACHED_OFFSET 0x20000000
// =============================================================================
//  TYPES
// =============================================================================
#if defined(RAM_CLK_IS_39M)
#define  HAL_SYS_MEM_DEFAULT_FREQ  HAL_SYS_MEM_FREQ_39M
#elif defined(RAM_CLK_IS_52M)
#define  HAL_SYS_MEM_DEFAULT_FREQ  HAL_SYS_MEM_FREQ_52M
#elif defined(RAM_CLK_IS_78M)
#define  HAL_SYS_MEM_DEFAULT_FREQ  HAL_SYS_MEM_FREQ_78M
#elif defined(RAM_CLK_IS_104M)
#define  HAL_SYS_MEM_DEFAULT_FREQ  HAL_SYS_MEM_FREQ_104M
#elif defined(RAM_CLK_IS_125M)
#define  HAL_SYS_MEM_DEFAULT_FREQ  HAL_SYS_MEM_FREQ_125M
#elif defined(RAM_CLK_IS_139M)
#define  HAL_SYS_MEM_DEFAULT_FREQ  HAL_SYS_MEM_FREQ_139M
#elif defined(RAM_CLK_IS_156M)
#define  HAL_SYS_MEM_DEFAULT_FREQ  HAL_SYS_MEM_FREQ_156M
#elif defined(RAM_CLK_IS_178M)
#define  HAL_SYS_MEM_DEFAULT_FREQ  HAL_SYS_MEM_FREQ_178M
#elif defined(RAM_CLK_IS_208M)
#define  HAL_SYS_MEM_DEFAULT_FREQ  HAL_SYS_MEM_FREQ_208M
#elif defined(RAM_CLK_IS_250M)
#define  HAL_SYS_MEM_DEFAULT_FREQ  HAL_SYS_MEM_FREQ_250M
#elif defined(RAM_CLK_IS_312M)
#define  HAL_SYS_MEM_DEFAULT_FREQ  HAL_SYS_MEM_FREQ_312M
#else
#error please config ddr timing.
#endif

#define  HAL_SYS_MEM_LPS_FREQ   HAL_SYS_MEM_FREQ_52M

// =============================================================================
// HAL_SYS_FREQ_USER_ID_T
// -----------------------------------------------------------------------------
/// Those are the different user recognized at the system scale.
/// @todo fill
// =============================================================================
typedef enum
{
    // Resource ID to be used by the platform
    HAL_SYS_FREQ_PLATFORM_FIRST_USER = 25,
    HAL_SYS_FREQ_PAL = HAL_SYS_FREQ_PLATFORM_FIRST_USER,
    HAL_SYS_FREQ_BCPU,
    HAL_SYS_FREQ_XCV,
    HAL_SYS_FREQ_CAMS,
    HAL_SYS_FREQ_VOIS,
    HAL_SYS_FREQ_NONE_0,
    HAL_SYS_FREQ_WCPU,
    HAL_SYS_FREQ_AVRS,
    HAL_SYS_FREQ_TSK,
    HAL_SYS_FREQ_FLASH,
    // !!! Add new platform users above !!!
    HAL_SYS_FREQ_PLATFORM_USER_END,
    HAL_SYS_FREQ_PLATFORM_LAST_USER = HAL_SYS_FREQ_PLATFORM_USER_END-1,

    // Resource ID to be used by the application (MMI)
    HAL_SYS_FREQ_APP_FIRST_USER = 40,
    HAL_SYS_FREQ_APP_USER_0 = HAL_SYS_FREQ_APP_FIRST_USER,
    HAL_SYS_FREQ_APP_USER_1,
    HAL_SYS_FREQ_APP_USER_2,
    HAL_SYS_FREQ_APP_USER_3,
    HAL_SYS_FREQ_APP_USER_4,
    HAL_SYS_FREQ_APP_USER_5,
    HAL_SYS_FREQ_APP_USER_6,
    HAL_SYS_FREQ_APP_USER_7,
    HAL_SYS_FREQ_APP_USER_8,

    // !!! Add new app users above !!!
    HAL_SYS_FREQ_APP_USER_END,
    HAL_SYS_FREQ_APP_LAST_USER = HAL_SYS_FREQ_APP_USER_END-1,

    // Total number of resource IDs
    HAL_SYS_FREQ_USER_QTY
} HAL_SYS_FREQ_USER_ID_T;


// =============================================================================
// HAL_SYS_FREQ_T
// -----------------------------------------------------------------------------
/// That type defines all the possible frequencies for the system clock.
/// This type is used to define the minimum system frequency required
/// by a user selected among #HAL_SYS_FREQ_USER_ID_T.
/// Some frequencies might not be supported by some chips.
// =============================================================================
typedef enum
{
    HAL_SYS_FREQ_32K     = 32768,
    HAL_SYS_FREQ_13M     = 13000000,
    HAL_SYS_FREQ_26M     = 26000000,
    HAL_SYS_FREQ_39M     = 39000000,
    HAL_SYS_FREQ_52M     = 52000000,
    HAL_SYS_FREQ_78M     = 78000000,
    HAL_SYS_FREQ_89M     = 89142857, //624M/7
    HAL_SYS_FREQ_104M    = 104000000,
    HAL_SYS_FREQ_113M    = 113454545, //624M/5.5
    HAL_SYS_FREQ_125M    = 124800000,
    HAL_SYS_FREQ_139M    = 138666667, //624M/4.5
    HAL_SYS_FREQ_156M    = 156000000,
    HAL_SYS_FREQ_178M    = 178285714, //624M/3.5
    HAL_SYS_FREQ_208M    = 208000000,
    HAL_SYS_FREQ_250M    = 249600000, //624M/2.5
    HAL_SYS_FREQ_312M    = 312000000,
    HAL_SYS_FREQ_UNKNOWN = 0,
} HAL_SYS_FREQ_T;


// =============================================================================
// HAL_SYS_FREQ_CHANGE_HANDLER_T
// -----------------------------------------------------------------------------
/// Type for Handler called when the system frequency changes
/// as registered by function hal_SysRequestFreq()
// =============================================================================
typedef VOID (*HAL_SYS_FREQ_CHANGE_HANDLER_T)(HAL_SYS_FREQ_T);


// =============================================================================
// HAL_SYS_MEM_FREQ_T
// -----------------------------------------------------------------------------
/// That type defines all the possible frequencies for the memory clock.
/// Some frequencies might not be supported by some chips.
// =============================================================================
typedef enum
{
    HAL_SYS_MEM_FREQ_13M     = 13000000,
    HAL_SYS_MEM_FREQ_26M     = 26000000,
    HAL_SYS_MEM_FREQ_39M     = 39000000,
    HAL_SYS_MEM_FREQ_52M     = 52000000,
    HAL_SYS_MEM_FREQ_78M     = 78000000,
    HAL_SYS_MEM_FREQ_89M     = 89142857, //624M/7
    HAL_SYS_MEM_FREQ_104M    = 104000000,
    HAL_SYS_MEM_FREQ_113M    = 113454545, //624M/5.5
    HAL_SYS_MEM_FREQ_125M    = 124800000,
    HAL_SYS_MEM_FREQ_139M    = 138666667, //624M/4.5
    HAL_SYS_MEM_FREQ_156M    = 156000000,
    HAL_SYS_MEM_FREQ_178M    = 178285714, //624M/3.5
    HAL_SYS_MEM_FREQ_208M    = 208000000,
    HAL_SYS_MEM_FREQ_250M    = 249600000, //624M/2.5
    HAL_SYS_MEM_FREQ_312M    = 312000000,
} HAL_SYS_MEM_FREQ_T;


// =============================================================================
// HAL_SYS_VOC_FREQ_T
// -----------------------------------------------------------------------------
/// That type defines all the possible frequencies for the VoC clock.
/// Some frequencies might not be supported by some chips.
// =============================================================================
typedef enum
{
    HAL_SYS_VOC_FREQ_13M     = 13000000,
    HAL_SYS_VOC_FREQ_26M     = 26000000,
    HAL_SYS_VOC_FREQ_39M     = 39000000,
    HAL_SYS_VOC_FREQ_52M     = 52000000,
    HAL_SYS_VOC_FREQ_78M     = 78000000,
    HAL_SYS_VOC_FREQ_89M     = 89142857, //624M/7
    HAL_SYS_VOC_FREQ_104M    = 104000000,
    HAL_SYS_VOC_FREQ_113M    = 113454545, //624M/5.5
    HAL_SYS_VOC_FREQ_125M    = 124800000,
    HAL_SYS_VOC_FREQ_139M    = 138666667, //624M/4.5
    HAL_SYS_VOC_FREQ_156M    = 156000000,
    HAL_SYS_VOC_FREQ_178M    = 178285714, //624M/3.5
    HAL_SYS_VOC_FREQ_208M    = 208000000,
    HAL_SYS_VOC_FREQ_250M    = 249600000, //624M/2.5
    HAL_SYS_VOC_FREQ_312M    = 312000000,
} HAL_SYS_VOC_FREQ_T;


// =============================================================================
// HAL_SYS_CLOCK_OUT_ID_T
// -----------------------------------------------------------------------------
/// That type defines all the possible user Id for the clock Out Pin sharing.
// =============================================================================
typedef enum
{
    /// returned when no more IDs are availlable
    HAL_SYS_CLOCK_OUT_ID_1 = 0,
    HAL_SYS_CLOCK_OUT_ID_2,
    HAL_SYS_CLOCK_OUT_ID_3,
    HAL_SYS_CLOCK_OUT_ID_4,

    HAL_SYS_CLOCK_OUT_ID_QTY,

    HAL_SYS_CLOCK_OUT_RESOURCE_UNAVAILABLE = -1
} HAL_SYS_CLOCK_OUT_ID_T;


// =============================================================================
// HAL_SYS_CLOCK_OUT_FREQ_T
// -----------------------------------------------------------------------------
/// That type defines all the possible frequencies for the clock Out Pin.
// =============================================================================
typedef enum
{
    HAL_SYS_CLOCK_OUT_FREQ_32K  = 1,
    HAL_SYS_CLOCK_OUT_FREQ_26M
} HAL_SYS_CLOCK_OUT_FREQ_T;


// =============================================================================
// HAL_SYS_RESET_CAUSE_T
// -----------------------------------------------------------------------------
/// Describes the cause of a system reset. This type is returned by the function
/// #hal_SysGetResetCause.
// =============================================================================
typedef enum
{
    /// Normal cause, ie power up
    HAL_SYS_RESET_CAUSE_NORMAL,

    /// The reset was caused by a watchdog
    HAL_SYS_RESET_CAUSE_WATCHDOG,

    /// The reset was caused by a soft restart, triggered by the function
    /// #hal_SysRestart.
    HAL_SYS_RESET_CAUSE_RESTART,

    /// The reset was initiated from the Host Interface.
    HAL_SYS_RESET_CAUSE_HOST_DEBUG,

    /// The reset was caused by alarm, from the calendar.
    HAL_SYS_RESET_CAUSE_ALARM,
#ifdef CHIP_DIE_8909
    /// The reset was caused from the LPS.
    HAL_SYS_RESET_CAUSE_LPS,

    /// The reset was caused from the PIN.
    HAL_SYS_RESET_CAUSE_PIN,
#endif
    HAL_SYS_RESET_CAUSE_QTY

} HAL_SYS_RESET_CAUSE_T;


// =============================================================================
// HAL_SYS_CHIP_ID_T
// -----------------------------------------------------------------------------
/// Describes the cause of a system reset. This type is returned by the function
/// #hal_SysGetResetCause.
// =============================================================================
typedef enum
{
    /// Chip product ID
    HAL_SYS_CHIP_PROD_ID,

    /// Chip bond ID
    HAL_SYS_CHIP_BOND_ID,

    /// Chip metal ID
    HAL_SYS_CHIP_METAL_ID,
#ifdef CHIP_DIE_8955
    /// Chip Sub ID
    HAL_SYS_CHIP_SUB_ID,
#endif
    /// Chip ID whole
    HAL_SYS_CHIP_ID_WHOLE,

} HAL_SYS_CHIP_ID_T;


// =============================================================================
// HAL_SYS_CHIP_METAL_ID_T
// -----------------------------------------------------------------------------
/// Those are the different metal IDs.
/// @todo fill
// =============================================================================
typedef enum
{
    HAL_SYS_CHIP_METAL_ID_U00 = 0x00,
    HAL_SYS_CHIP_METAL_ID_U01,
    HAL_SYS_CHIP_METAL_ID_U02,
    HAL_SYS_CHIP_METAL_ID_U03,
    HAL_SYS_CHIP_METAL_ID_U04,
    HAL_SYS_CHIP_METAL_ID_U05,
    HAL_SYS_CHIP_METAL_ID_U06,
    HAL_SYS_CHIP_METAL_ID_U07,
    HAL_SYS_CHIP_METAL_ID_U08,
    HAL_SYS_CHIP_METAL_ID_U09,
    HAL_SYS_CHIP_METAL_ID_U0A,
    HAL_SYS_CHIP_METAL_ID_U0B,
    HAL_SYS_CHIP_METAL_ID_U0C,
    HAL_SYS_CHIP_METAL_ID_U0D,
    HAL_SYS_CHIP_METAL_ID_U0E,
    HAL_SYS_CHIP_METAL_ID_U0F,
    HAL_SYS_CHIP_METAL_ID_U10,
} HAL_SYS_CHIP_METAL_ID_T;

// =============================================================================
// HAL_CALLBACK_FUNCTION_T/HAL_CALLBACK_T
// -----------------------------------------------------------------------------
/// Generic callback function and body type
// =============================================================================
typedef void (*HAL_CALLBACK_FUNCTION_T)(void * param);

typedef struct
{
    HAL_CALLBACK_FUNCTION_T callback;
    void *param;
} HAL_CALLBACK_T;

#define HAL_RUN_CALLBACK(cb) do { if ((cb).callback != NULL) (cb).callback((cb).param); } while (0)

// =============================================================================
//  FUNCTIONS
// =============================================================================

// ============================================================================
// hal_SysSetupPLL
// ----------------------------------------------------------------------------
/// Setup PLL at early system boot time.
// ============================================================================
PUBLIC VOID hal_SysSetupPLL(VOID);


// =============================================================================
// hal_SysStartBcpu
// -----------------------------------------------------------------------------
/// Start the BCPU with a stack to at the given address.
/// @param bcpuMain Main function to run on the BCPU.
/// @param stackStartAddr Stack top for the program running on the BCPU
// =============================================================================
PUBLIC VOID hal_SysStartBcpu(VOID* bcpuMain, VOID* stackStartAddr);


// =============================================================================
// hal_SysStopBcpu
// -----------------------------------------------------------------------------
/// Stop the BCPU and leave it in reset
/// Use it for debug only as it is prefereable to
/// not send any commands to it
// =============================================================================
PUBLIC VOID hal_SysStopBcpu(VOID);


#ifdef CHIP_HAS_WCPU
// =============================================================================
// hal_SysStartWcpu
// -----------------------------------------------------------------------------
/// Start the WCPU with a stack to at the given address.
/// @param wcpuMain Main function to run on the WCPU.
/// @param stackStartAddr Stack top for the program running on the WCPU
// =============================================================================
PUBLIC VOID hal_SysStartWcpu(VOID* wcpuMain, VOID* stackStartAddr);


// =============================================================================
// hal_SysStopWcpu
// -----------------------------------------------------------------------------
/// Stop the WCPU
// =============================================================================
PUBLIC VOID hal_SysStopWcpu(VOID);
#endif

// =============================================================================
// hal_SysGetFreq
// -----------------------------------------------------------------------------
/// Get the current system clock frequency.
/// @return The current system clock frequency.
// =============================================================================
PUBLIC HAL_SYS_FREQ_T hal_SysGetFreq(VOID);


// =============================================================================
// hal_SysSetBBClock
// -----------------------------------------------------------------------------
/// Set the BB clock.
/// @param newClock New BB clock.
// =============================================================================
PUBLIC VOID hal_SysSetBBClock(HAL_SYS_FREQ_T newClock);


// =============================================================================
// hal_SysSetVocClock
// -----------------------------------------------------------------------------
/// Set the VOC clock.
/// @param newClock New VOC clock.
/// @return Old VOC clock.
// =============================================================================
PUBLIC HAL_SYS_VOC_FREQ_T hal_SysSetVocClock(HAL_SYS_VOC_FREQ_T newClock);

// =============================================================================
//  hal_SysGetVocClock
// -----------------------------------------------------------------------------
/// Get the VOC clock.
/// @return Old VOC clock.
// =============================================================================

PUBLIC HAL_SYS_VOC_FREQ_T hal_SysGetVocClock();
// =============================================================================
// hal_SysShutdown
// -----------------------------------------------------------------------------
/// A call to this function shuts the system down.
// =============================================================================
PUBLIC VOID hal_SysShutdown(VOID);


// =============================================================================
// hal_SysSoftReset
// -----------------------------------------------------------------------------
/// Does a soft reset on the chip.
/// Generally, you should NOT use this function, use hal_SysRestart() instead.
// =============================================================================
PUBLIC VOID hal_SysSoftReset(VOID);


// =============================================================================
// hal_SysRestart
// -----------------------------------------------------------------------------
/// A call to this function reboots the system.
/// First, this function does a power cycle on USB
// =============================================================================
PUBLIC VOID hal_SysRestart(VOID);


// =============================================================================
// hal_SysGetResetCause
// -----------------------------------------------------------------------------
/// Get the cause of the last reset.
/// This function only returns the reset cause. It does not proceed to any test
/// to now if the boot was due to a press on the Power On button or because
/// the charger was plugged. Those tests are to be done by the user code,
/// when this function returns HAL_SYS_RESET_CAUSE_ALARM.
/// @return The last reset cause
// =============================================================================
PUBLIC HAL_SYS_RESET_CAUSE_T hal_SysGetResetCause(VOID);


// =============================================================================
// hal_SysSleep
// -----------------------------------------------------------------------------
/// Check if the system can sleep and, in that case, make it sleep.
///
/// This function is to be called by pal_Sleep, normally in
/// the idle task. It switches the system on the 32khz clock if no user requires
/// a higher clock and if it is ready to do it, ie: the LPS is operational
// =============================================================================
PUBLIC VOID hal_SysSleep(VOID);


// =============================================================================
// hal_SysResetOut
// -----------------------------------------------------------------------------
/// A call to this function resets an external device through the reset-out
/// line, if the parameter is \c TRUE.
/// @param resetOut If \c TRUE, rises the reset-out line.
///                 If \c FALSE, pulls it down.
// =============================================================================
PUBLIC VOID hal_SysResetOut(BOOL resetOut);


// =============================================================================
// hal_SysInvalidateCache
// -----------------------------------------------------------------------------
/// This function invalidates in the cache the line corresponding to the given
/// memory area, referred by a pointer and a size.
/// It must be called with the destination buffer after a DMA copy.
/// @param buffer Pointer to the first address whose cache line must be
/// invalidated
/// @param size Size of the buffer.
// =============================================================================
PUBLIC VOID hal_SysInvalidateCache(VOID* buffer, UINT32 size);


// =============================================================================
// hal_SysGetChipId
// -----------------------------------------------------------------------------
/// That function gives the chip ID the software is running on.
/// @param part Which part of the chip ID
/// @return The requested part of the chip ID.
// =============================================================================
PUBLIC UINT32 hal_SysGetChipId(HAL_SYS_CHIP_ID_T part);


// =============================================================================
// hal_SysGetMetalId
// -----------------------------------------------------------------------------
/// That function gives the chip metal ID the software is running on.
/// @return The chip metal ID enum, which is not the bitfield in chip register.
// =============================================================================
PUBLIC HAL_SYS_CHIP_METAL_ID_T hal_SysGetMetalId(VOID);


// =============================================================================
// hal_SysGetThermal_cal
// -----------------------------------------------------------------------------
/// That function gives the  thermal_cal_value.
/// @return  thermal_cal_value.
// =============================================================================
PUBLIC  UINT16 hal_SysGetThermal_cal(VOID);


// =============================================================================
// g_halSysCsAddress
// -----------------------------------------------------------------------------
/// This global variable stores the address of the chip critical section.
///
/// *ATTENTION* It is forbidden to use  global variables, but here we e
/// exceptionally need to have one for critical performance reasons.
/// This is not an exchange variable, it is forbidden to use it directly.
///
/// hal_SysEnterCriticalSection and hal_SysExitCriticalSection are inline
/// built using that variable and must be used to access the critical
/// section.
// =============================================================================
EXPORT VOLATILE UINT32* g_bootSysCsAddress;


// =============================================================================
// hal_SysEnterCriticalSection
// -----------------------------------------------------------------------------
/// This function enters in a critical section (relative to the hardware
/// interrupts) and return a status that should be given to the
/// #hal_SysExitCriticalSection function.
///
/// @return  The \c status that should be given to the #hal_SysExitCriticalSection
/// function.
// =============================================================================
INLINE UINT32 hal_SysEnterCriticalSection(VOID)
{
    UINT32  myLocalVar = *g_bootSysCsAddress;
    // add this empty assembly line to avoid the compiler to re-order
    // the code around the critical section call
    asm volatile ("");
    return myLocalVar;
}


// =============================================================================
// hal_SysExitCriticalSection
// -----------------------------------------------------------------------------
/// This function leaves the critical section. The <CODE>status</CODE>
/// parameter is the return value of the #hal_SysEnterCriticalSection call
/// that opened the critical section.
///
/// @param status The value returned by #hal_SysEnterCriticalSection.
// =============================================================================
INLINE VOID hal_SysExitCriticalSection(UINT32 status)
{
    // add this empty assembly line to avoid the compiler to re-order
    // the code around the critical section call
    asm volatile ("");
    *g_bootSysCsAddress = status;
}


// =============================================================================
// hal_SysClkOutOpen
// -----------------------------------------------------------------------------
/// Manage the output of the clock-out, to an external device
///
/// This function requests a resource of #HAL_SYS_FREQ_26M or release it
/// to #HAL_SYS_FREQ_32K depending on the \p freq parameter value.
///
/// @param freq the required frequency
/// @return the user id to be used for #hal_SysClkOutClose
///         or #HAL_SYS_CLOCK_OUT_RESOURCE_UNAVAILABLE when the clock is already
///         in use at a different frequency.
// =============================================================================
PUBLIC HAL_SYS_CLOCK_OUT_ID_T hal_SysClkOutOpen(HAL_SYS_CLOCK_OUT_FREQ_T freq);


// =============================================================================
// hal_SysClkOutClose
// -----------------------------------------------------------------------------
/// Manage the output of the clock-out, to an external device
///
/// This function release a resource to #HAL_SYS_FREQ_32K.
///
/// @param id the user id returned by #hal_SysClkOutOpen
// =============================================================================
PUBLIC VOID hal_SysClkOutClose(HAL_SYS_CLOCK_OUT_ID_T id);


// ==========================================================================
// hal_SysAuxClkOut
// --------------------------------------------------------------------------
/// Manage the output of the 26M AUX Clock, to an external device
/// @param enable If \c TRUE, enable AuxClk output.
///               If \c FALSE, disable AuxClk output.
// ==========================================================================
PUBLIC VOID hal_SysAuxClkOut(BOOL enable);


// ==========================================================================
// hal_Sys32kClkOut
// --------------------------------------------------------------------------
/// Manage the output of the 32K Clock, to an external device
/// @param enable If \c TRUE, enable 32K clock output.
///               If \c FALSE, disable 32K clock output.
// ==========================================================================
PUBLIC VOID hal_Sys32kClkOut(BOOL enable);


// =============================================================================
// hal_SysIrqPending
// -----------------------------------------------------------------------------
/// This function checks the IRQ pending status. It should be called in critical
/// section to know if some IRQ are pending and then decide to release the
/// critical section.
/// @return \c TRUE if IRQ are pending.
///         \c FALSE Otherwise.
// =============================================================================
PUBLIC BOOL hal_SysIrqPending(VOID);


// =============================================================================
// hal_SysProcessHostMonitor
// -----------------------------------------------------------------------------
/// This function is called in the idle task or in gdb loop.
/// It is used to execute the function processing possible commands
/// from the Host.
// =============================================================================
PUBLIC VOID hal_SysProcessHostMonitor(VOID);


// =============================================================================
// hal_SysProcessIdle
// -----------------------------------------------------------------------------
/// This function is called in the idle task.
/// It is used to execute in idle the function processing possible commands
/// from the Host, and thus, for example, call the executor. etc ...
/// @return \c TRUE polling is needed
///         \c FALSE otherwise
// =============================================================================
PUBLIC BOOL hal_SysProcessIdle(VOID);


// =============================================================================
// hal_SysWaitMicrosecond
// -----------------------------------------------------------------------------
/// This function wait some time in microseconds.
/// It calculates CPU instruction cycles needed, depending on current system frequency.
/// Note that it does NOT have critical section operations.
/// @param us Microseconds to wait.
// =============================================================================
PUBLIC VOID hal_SysWaitMicrosecond(UINT32 us);


// =============================================================================
// hal_BootEnterMonitor
// -----------------------------------------------------------------------------
/// Call this function to enter the boot monitor.
// =============================================================================
PUBLIC VOID hal_BootEnterMonitor(VOID);


// =============================================================================
// hal_BootRegisterBeforeMonCallback
// -----------------------------------------------------------------------------
/// Call this function to register a callback function which is run
/// before entering the boot monitor.
// =============================================================================
PUBLIC VOID hal_BootRegisterBeforeMonCallback(VOID (*funcPtr)(VOID));


PUBLIC VOID hal_SysSwitchPll(HAL_SYS_PLL_PROFILE_T  Mode);

#ifdef CHIP_DIE_8955
PUBLIC VOID hal_SysSetSpiFlashClkToFast(VOID);
#endif

// =============================================================================
// hal_GetSramScratchAddress/hal_GetSramScratchTop
// -----------------------------------------------------------------------------
/// Get starting addres and top address of SRAM scratch memory.
///
/// SRAM scratch memory is a small piece of SRAM memory in boot bss section.
/// There are no protection for multiple usage. So, it should be used inside
/// interrupt disabled.
// =============================================================================
PUBLIC UINT32 *hal_GetSramScratchAddress(VOID);
PUBLIC UINT32 *hal_GetSramScratchTop(VOID);

// =============================================================================
// hal_CallWithStack
// -----------------------------------------------------------------------------
/// Call specified function with specified stack pointer with 8 parameters at most.
/// It is for special usage (for example, change core voltage should avoid using
/// PSRAM, then it is needed to switch stack to SRAM).
/// NOTE: stack should be the stack top pointer (largest usable address)
// =============================================================================
PUBLIC VOID hal_CallWithStack(VOID *stack,
                              VOID *function,
                              UINT32 par1,
                              UINT32 par2,
                              UINT32 par3,
                              UINT32 par4,
                              UINT32 par5,
                              UINT32 par6,
                              UINT32 par7,
                              UINT32 par8);

// =============================================================================
// hal_SysSetTcuWrapValue
// -----------------------------------------------------------------------------
/// Record tcu wrap value once it is changed, for judging whether disable trace
/// or not.
// =============================================================================
PUBLIC VOID hal_SysSetTcuWrapValue(UINT32 value);


// =============================================================================
// hal_SysSetActiveSimCount
// -----------------------------------------------------------------------------
/// Record active sim count once it is changed, for judging whether disable trace
/// or not.
// =============================================================================
PUBLIC VOID hal_SysSetActiveSimCount(UINT8 count);

/// @} // end of the system group



#endif // _HAL_SYS_H_

