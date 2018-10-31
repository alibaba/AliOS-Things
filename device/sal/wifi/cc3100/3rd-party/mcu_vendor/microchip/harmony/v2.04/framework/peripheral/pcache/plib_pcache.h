/*******************************************************************************
  Prefetch Cache Peripheral Library Interface Header

  Company:      Microchip Technology Inc.

  File Name:    plib_pcache.h

  Summary:
    Defines the Prefetch Cache Peripheral Library Interface

  Description:
    This header file contains the function prototypes and definitions of
    the data types and constants that make up the interface to the Prefetch
    Cache Peripheral Library for Microchip microcontrollers. The
    definitions in this file are for the Prefetch Cache module.
*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright 2013-2015 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/
// DOM-IGNORE-END

#ifndef _PLIB_PCACHE_H
#define _PLIB_PCACHE_H


// *****************************************************************************
// *****************************************************************************
// Section: Includes
// *****************************************************************************
// *****************************************************************************
/* See Bottom of file for implementation of header include files.
*/


// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Constants and Data Types
// *****************************************************************************
// *****************************************************************************

// DOM-IGNORE-BEGIN

// *****************************************************************************
/* Number of Cache Lines

  Summary:
    Defines the number of available Prefetch Cache lines.

  Description:
    This definition specifies the number of available Prefetch Cache lines.

  Remarks:
    The actual number of cache lines is processor specific and is defined in
    the processor-specific header files (see processor.h).
*/

#define _PLIB_PCACHE_NUM_LINES PLIB_PCACHE_NUM_LINES


// *****************************************************************************
/* Number of Words per Cache Line

  Summary:
    Defines the number of words per Prefetch Cache line.

  Description:
    This definition specifies the number of words per Prefetch Cache line.

  Remarks:
    The actual number of words per Prefetch Cache line is processor specific and
    is defined in the processor-specific header files (see processor.h).
*/

#define _PLIB_PCACHE_NUM_WORDS_PER_LINE PLIB_PCACHE_NUM_WORDS_PER_LINE


// *****************************************************************************
/* Maximum Value for Single Bit Error Counter

  Summary:
    Defines the maximum value for the Single-bit Error Correction (SEC) counter.

  Description:
    This definition specifies the maximum value for the SEC counter.

  Remarks:
    The actual maximum value of the SEC counter is processor specific and is 
    defined in the processor-specific header files see processor.h).
*/

#define _PLIB_PCACHE_MAX_SEC_COUNT PLIB_PCACHE_MAX_SEC_COUNT


#include "peripheral/pcache/processor/pcache_processor.h"

// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Peripheral Library Interface Routines
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void PLIB_PCACHE_WaitStateSet( PCACHE_MODULE_ID index, uint32_t clocks )

  Summary:
    Sets the Prefetch Cache access time.

  Description:
    This function sets the Prefetch Cache access time in terms of SYSCLK wait.
    states.

  Precondition:
    None.

  Parameters:
    index  - Identifier for the device instance to be configured
    clocks - Identifies the wait state in terms of number of SYSCLK cycles

  Returns:
    None.

  Example:
    <code>
    PLIB_PCACHE_WaitStateSet(PCACHE_ID_0, 0);
    </code>

  Remarks:
    At reset, this value is set to seven wait states.

    This function implements an operation of the WaitState feature.
    This feature may not be available on all devices. Please refer to the 
    specific device data sheet to determine availability or use 
    PLIB_RTCC_ExistsWaitState in your application to
    to automatically determine whether this feature is available.
*/

void PLIB_PCACHE_WaitStateSet(PCACHE_MODULE_ID index, uint32_t clocks);


/*******************************************************************************
  Function:
    uint32_t PLIB_PCACHE_WaitStateGet( PCACHE_MODULE_ID index )

  Summary:
    Gets the Prefetch Cache access time.

  Description:
    This function gets the Prefetch Cache access time in terms of SYSCLK wait.
    states.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance to be read

  Returns:
    Number of clock cycles.

  Example:
    <code>
    uint32_t wait;
    wait = PLIB_PCACHE_WaitStateGet(PCACHE_ID_0);
    </code>

  Remarks:
    At reset, this value is set to seven wait states.

    This function implements an operation of the WaitState feature.
    This feature may not be available on all devices. Please refer to the 
    specific device data sheet to determine availability or use 
    PLIB_RTCC_ExistsWaitState in your application to
    to automatically determine whether this feature is available.
*/

uint32_t PLIB_PCACHE_WaitStateGet(PCACHE_MODULE_ID index);


/*******************************************************************************
  Function:
    void PLIB_PCACHE_PrefetchEnableSet( PCACHE_MODULE_ID index,
                                        PLIB_PCACHE_PREFETCH_ENABLE region )

  Summary:
    Sets the predictive Prefetch state for the Prefetch Cache module.

  Description:
    This function sets the predictive Prefetch state for the Prefetch Cache 
    module.

  Precondition:
    None.

  Parameters:
    index  - Identifier for the device instance to be configured
    region - PLIB_PCACHE_PREFETCH_ENABLE

  Returns:
    None.

  Example:
    <code>
    PLIB_PCACHE_PrefetchEnableSet(PCACHE_ID_0, \
                                  PLIB_PCACHE_PREFETCH_ENABLE_ALL);
    </code>

  Remarks:
    At reset, the Prefetch Cache module is disabled.

    This function implements an operation of the PrefetchEnable feature.
    This feature may not be available on all devices. Please refer to the 
    specific device data sheet to determine availability or use 
    PLIB_RTCC_ExistsPrefetchEnable in your application to
    to automatically determine whether this feature is available.
*/

void PLIB_PCACHE_PrefetchEnableSet(PCACHE_MODULE_ID index, PLIB_PCACHE_PREFETCH_ENABLE region);


/*******************************************************************************
  Function:
    PLIB_PCACHE_PREFETCH_ENABLE  PLIB_PCACHE_PrefetchEnableGet( PCACHE_MODULE_ID index )

  Summary:
    Gets the predictive Prefetch state for the Prefetch Cache module.

  Description:
    This function gets the predictive Prefetch state for the Prefetch Cache 
    module.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance to be read

  Returns:
    PLIB_PCACHE_PREFETCH_ENABLE

  Example:
    <code>
    PLIB_PCACHE_PREFETCH_ENABLE region;
    region = PLIB_PCACHE_PrefetchEnableGet(PCACHE_ID_0);
    </code>

  Remarks:
    At reset, the Prefetch Cache module is disabled and this function will return
    PLIB_PCACHE_DISABLE.

    This function implements an operation of the PrefetchEnable feature.
    This feature may not be available on all devices. Please refer to the 
    specific device data sheet to determine availability or use 
    PLIB_RTCC_ExistsPrefetchEnable in your application to
    to automatically determine whether this feature is available.
*/

PLIB_PCACHE_PREFETCH_ENABLE PLIB_PCACHE_PrefetchEnableGet(PCACHE_MODULE_ID index);


/*******************************************************************************
  Function:
    void PLIB_PCACHE_DataCacheEnableSet( PCACHE_MODULE_ID index,
                                            PLIB_PCACHE_DATA_ENABLE dcache_en )

  Summary:
    Sets the data cache enable bits.

  Description:
    This function sets the data cache enable bits.

  Precondition:
    None.

  Parameters:
    index     - Identifier for the device instance to be configured
    dcache_en - PLIB_PCACHE_DATA_ENABLE

  Returns:
    None.

  Example:
    <code>
    PLIB_PCACHE_DataCacheEnableSet(PCACHE_ID_0, PLIB_PCACHE_DATA_4LINE);
    </code>

  Remarks:
    Data caching is disabled at reset.

    This function implements an operation of the DataCacheEnable feature.
    This feature may not be available on all devices. Please refer to the 
    specific device data sheet to determine availability or use 
    PLIB_RTCC_ExistsDataCacheEnable in your application to
    to automatically determine whether this feature is available.
*/

void PLIB_PCACHE_DataCacheEnableSet(PCACHE_MODULE_ID index, PLIB_PCACHE_DATA_ENABLE dcache_en);


/*******************************************************************************
  Function:
    PLIB_PCACHE_DATA_ENABLE PLIB_PCACHE_DataCacheEnableGet( PCACHE_MODULE_ID index )

  Summary:
    Gets the data cache enable bits.

  Description:
    This function gets the data cache enable bits.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance to be read

  Returns:
    PLIB_PCACHE_DATA_ENABLE

  Example:
    <code>
    PLIB_PCACHE_DATA_ENABLE dcache_en;
    dcache_en = PLIB_PCACHE_DataCacheEnableGet(PCACHE_ID_0);
    </code>

  Remarks:
    At reset, data caching is disabled, and this function will return
    PLIB_PCACHE_DATA_DISABLE.

    This function implements an operation of the DataCacheEnable feature.
    This feature may not be available on all devices. Please refer to the 
    specific device data sheet to determine availability or use 
    PLIB_RTCC_ExistsDataCacheEnable in your application to
    to automatically determine whether this feature is available.
*/

PLIB_PCACHE_DATA_ENABLE PLIB_PCACHE_DataCacheEnableGet(PCACHE_MODULE_ID index);


/*******************************************************************************
  Function:
    void PLIB_PCACHE_FlashSECIntEnable( PCACHE_MODULE_ID index )

  Summary:
    Enables an interrupt on SEC.

  Description:
    This function enables an interrupt on SEC.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance to be enabled

  Returns:
    None

  Example:
    <code>
    PLIB_PCACHE_FlashSECIntEnable(PCACHE_ID_0);
    </code>

  Remarks:
    At reset the SEC interrupt is disabled.

    This function implements an operation of the FlashSECInt feature.
    This feature may not be available on all devices. Please refer to the 
    specific device data sheet to determine availability or use 
    PLIB_RTCC_ExistsFlashSECInt in your application to
    to automatically determine whether this feature is available.
*/

void PLIB_PCACHE_FlashSECIntEnable(PCACHE_MODULE_ID index);


/*******************************************************************************
  Function:
    void PLIB_PCACHE_FlashSECIntDisable( PCACHE_MODULE_ID index )

  Summary:
    Disables an interrupt on SEC.

  Description:
    This function disables an interrupt on SEC.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance to be disabled

  Returns:
    None.

  Example:
    <code>
    PLIB_PCACHE_FlashSECIntDisable(PCACHE_ID_0);
    </code>

  Remarks:
    At reset the SEC interrupt is disabled.

    This function implements an operation of the FlashSECInt feature.
    This feature may not be available on all devices. Please refer to the 
    specific device data sheet to determine availability or use 
    PLIB_RTCC_ExistsFlashSECInt in your application to
    to automatically determine whether this feature is available.
*/

void PLIB_PCACHE_FlashSECIntDisable(PCACHE_MODULE_ID index);


/*******************************************************************************
  Function:
    bool PLIB_PCACHE_FlashDEDStatusGet( PCACHE_MODULE_ID index )

  Summary:
    Determines if a bus exception was caused by a double-bit error.

  Description:
    This function determines if a bus exception was caused by a double-bit
    error.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance to be read

  Returns:
    - true  - A double-bit error was detected
    - false - A double-bit error was not detected

  Example:
    <code>
    bool dedStatus;
    dedStatus = PLIB_PCACHE_FlashDEDStatusGet(PCACHE_ID_0);
    </code>

  Remarks:
    The Double-bit Error Detected (DED) status is set by hardware.

    This function implements an operation of the FlashDEDStatus feature.
    This feature may not be available on all devices. Please refer to the 
    specific device data sheet to determine availability or use 
    PLIB_RTCC_ExistsFlashDEDStatus in your application to
    to automatically determine whether this feature is available.
*/

bool PLIB_PCACHE_FlashDEDStatusGet(PCACHE_MODULE_ID index);


/*******************************************************************************
  Function:
    void PLIB_PCACHE_FlashDEDStatusClear( PCACHE_MODULE_ID index )

  Summary:
    Clears a bus exception caused by a double-bit error.

  Description:
    This function clears a bus exception caused by a double-bit error.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance to be cleared

  Returns:
    None.

  Example:
    <code>
    PLIB_PCACHE_FlashDEDStatusClear(PCACHE_ID_0);
    </code>

  Remarks:
    The DED status is set by hardware.

    This function implements an operation of the FlashDEDStatus feature.
    This feature may not be available on all devices. Please refer to the 
    specific device data sheet to determine availability or use 
    PLIB_RTCC_ExistsFlashDEDStatus in your application to
    to automatically determine whether this feature is available.
*/

void PLIB_PCACHE_FlashDEDStatusClear(PCACHE_MODULE_ID index);


/*******************************************************************************
  Function:
    bool PLIB_PCACHE_FlashSECStatusGet( PCACHE_MODULE_ID index )

  Summary:
    Determines if a SEC event triggered a Prefetch Cache interrupt.

  Description:
    This function determines if a SEC event triggered a Prefetch Cache interrupt.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance to be read

  Returns:
    - true  - A single-bit error was corrected
    - false - A double-bit error was not corrected

  Example:
    <code>
    bool secStatus;
    secStatus = PLIB_PCACHE_FlashSECStatusGet(PCACHE_ID_0);
    </code>

  Remarks:
    The SEC bit is set when a single-bit error is corrected and the SEC
    counter is zero. If the Flash SEC interrupt is enabled, an error event is 
    reported to the CPU by a Prefetch Cache interrupt event.

    This function implements an operation of the FlashSECStatus feature.
    This feature may not be available on all devices. Please refer to the 
    specific device data sheet to determine availability or use 
    PLIB_RTCC_ExistsFlashSECStatus in your application to
    to automatically determine whether this feature is available.
*/

bool PLIB_PCACHE_FlashSECStatusGet(PCACHE_MODULE_ID index);


/*******************************************************************************
  Function:
    void PLIB_PCACHE_FlashSECStatusSet( PCACHE_MODULE_ID index )

  Summary:
    Sets the single-bit error corrected status to true.

  Description:
    This function sets the single-bit error corrected status to true.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    PLIB_PCACHE_FlashSECStatusSet(PCACHE_ID_0);
    </code>

  Remarks:
    This function is provided for code testing and debug purposes. Setting the
    SEC status while the SEC count is zero and the SEC interrupt is enabled
    will trigger a Prefetch Cache interrupt to the CPU.

    This function implements an operation of the FlashSECStatus feature.
    This feature may not be available on all devices. Please refer to the 
    specific device data sheet to determine availability or use 
    PLIB_RTCC_ExistsFlashSECStatus in your application to
    to automatically determine whether this feature is available.
*/

void PLIB_PCACHE_FlashSECStatusSet(PCACHE_MODULE_ID index);


/*******************************************************************************
  Function:
    void PLIB_PCACHE_FlashSECStatusClear( PCACHE_MODULE_ID index )

  Summary:
    Sets the single-bit error corrected status to false.

  Description:
    This function sets the single-bit error corrected status to false.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance to be cleared

  Returns:
    None.

  Example:
    <code>
    PLIB_PCACHE_FlashSECStatusClear(PCACHE_ID_0);
    </code>

  Remarks:
    The SEC status must be cleared in response to a Prefetch Cache SEC interrupt.

    This function implements an operation of the FlashSECStatus feature.
    This feature may not be available on all devices. Please refer to the 
    specific device data sheet to determine availability or use 
    PLIB_RTCC_ExistsFlashSECStatus in your application to
    to automatically determine whether this feature is available.
*/

void PLIB_PCACHE_FlashSECStatusClear(PCACHE_MODULE_ID index);


/*******************************************************************************
  Function:
    void PLIB_PCACHE_FlashSECCountSet( PCACHE_MODULE_ID index, uint8_t count )

  Summary:
    Sets the number of single-bit error corrections that must occur before the
    SEC status is set to true.

  Description:
    This function sets the number of single-bit error corrections that must
    occur before the SEC status is set to true.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance to be configured
    count - Number of SEC events to occur before SEC status is set to true

  Returns:
    None.

  Example:
    <code>
    PLIB_PCACHE_FlashSECCountSet(PCACHE_ID_0, 255);
    </code>

  Remarks:
    This function implements an operation of the FlashSECCount feature.
    This feature may not be available on all devices. Please refer to the 
    specific device data sheet to determine availability or use 
    PLIB_RTCC_ExistsFlashSECCount in your application to
    to automatically determine whether this feature is available.
*/

void PLIB_PCACHE_FlashSECCountSet(PCACHE_MODULE_ID index, uint8_t count);


/*******************************************************************************
  Function:
    uint8_t PLIB_PCACHE_FlashSECCountGet( PCACHE_MODULE_ID index )

  Summary:
    Returns the current value of the Flash SEC counter.

  Description:
    This function returns the current value of the Flash SEC counter.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance to be read

  Returns:
    * count - Number of SEC events to occur before the SEC status is set to true

  Example:
    <code>
    uint8_t count;
    count = PLIB_PCACHE_FlashSECCountGet(PCACHE_ID_0);
    </code>

  Remarks:
    This function implements an operation of the FlashSECCount feature.
    This feature may not be available on all devices. Please refer to the 
    specific device data sheet to determine availability or use 
    PLIB_RTCC_ExistsFlashSECCount in your application to
    to automatically determine whether this feature is available.
*/

uint8_t PLIB_PCACHE_FlashSECCountGet(PCACHE_MODULE_ID index);


/*******************************************************************************
  Function:
    void PLIB_PCACHE_InvalidateOnPFMProgramAll( PCACHE_MODULE_ID index )

  Summary:
    Sets Prefetch Cache to invalidate all data and instruction lines on a PFM
    program cycle.

  Description:
    This function sets the Prefetch Cache to invalidate all data and
    instruction lines on a PFM program cycle.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    PLIB_PCACHE_InvalidateOnPFMProgramAll(PCACHE_ID_0);
    </code>

  Remarks:
    This function implements an operation of the InvalidateOnPFMProgram feature.
    This feature may not be available on all devices. Please refer to the 
    specific device data sheet to determine availability or use 
    PLIB_RTCC_ExistsInvalidateOnPFMProgram in your application to
    to automatically determine whether this feature is available.
*/

void PLIB_PCACHE_InvalidateOnPFMProgramAll(PCACHE_MODULE_ID index);


/*******************************************************************************
  Function:
    void PLIB_PCACHE_InvalidateOnPFMProgramUnlocked( PCACHE_MODULE_ID index )

  Summary:
    Sets Prefetch Cache to invalidate all unlocked data and instruction lines
    on a PFM program cycle.

  Description:
    This function sets the Prefetch Cache to invalidate all unlocked data and
    instruction lines on a PFM program cycle.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    PLIB_PCACHE_InvalidateOnPFMProgramUnlocked(PCACHE_ID_0);
    </code>

  Remarks:
    This is the default state at reset.

    This function implements an operation of the InvalidateOnPFMProgram feature.
    This feature may not be available on all devices. Please refer to the 
    specific device data sheet to determine availability or use 
    PLIB_RTCC_ExistsInvalidateOnPFMProgram in your application to
    to automatically determine whether this feature is available.
*/

void PLIB_PCACHE_InvalidateOnPFMProgramUnlocked(PCACHE_MODULE_ID index);


/*******************************************************************************
  Function:
    void PLIB_PCACHE_CacheLineSelect( PCACHE_MODULE_ID index, \
                                      uint32_t cache_line )

  Summary:
    Enables write access to the cache line specified by cache_line.

  Description:
    This function enables write access to the cache line specified by
    cache_line.

  Precondition:
    None.

  Parameters:
    index      - Identifier for the device instance to be configured
    cache_line - Specifies the cache line to enable write access

  Returns:
    None.

  Example:
    <code>
    PLIB_PCACHE_CacheLineSelect(PCACHE_ID_0, 0);
    </code>

  Remarks:
    At reset, all cache lines are read-only.

   This function implements an operation of the CacheLineSelect feature.
    This feature may not be available on all devices. Please refer to the 
    specific device data sheet to determine availability or use 
    PLIB_RTCC_ExistsCacheLineSelect in your application to
    to automatically determine whether this feature is available.
*/

void PLIB_PCACHE_CacheLineSelect(PCACHE_MODULE_ID index, uint32_t cache_line);


/*******************************************************************************
  Function:
    void PLIB_PCACHE_CacheLineDeselect( PCACHE_MODULE_ID index, \
    uint32_t cache_line )

  Summary:
    Disables write access to the cache line specified by cache_line.

  Description:
    This function disables write access to the cache line specified by
    cache_line.

  Precondition:
    None.

  Parameters:
    index      - Identifier for the device instance to be configured
    cache_line - Specifies the cache line to disable write access

  Returns:
    None.

  Example:
    <code>
    PLIB_PCACHE_CacheLineDeselect(PCACHE_ID_0, 0);
    </code>

  Remarks:
    This function implements an operation of the CacheLineSelect feature.
    This feature may not be available on all devices. Please refer to the 
    specific device data sheet to determine availability or use 
    PLIB_RTCC_ExistsCacheLineSelect in your application to
    to automatically determine whether this feature is available.
*/

void PLIB_PCACHE_CacheLineDeselect(PCACHE_MODULE_ID index, uint32_t cache_line);


/*******************************************************************************
  Function:
    void PLIB_PCACHE_CacheLineInst( PCACHE_MODULE_ID index )

  Summary:
    Sets cache line previously write-enabled by PLIB_PCACHE_CacheLineSelect
    to instruction type.

  Description:
    This function sets the cache line previously write-enabled by
    PLIB_PCACHE_CacheLineSelect to instruction type.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    PLIB_PCACHE_CacheLineInst(PCACHE_ID_0);
    </code>

  Remarks:
    This function implements an operation of the CacheLineType feature.
    This feature may not be available on all devices. Please refer to the 
    specific device data sheet to determine availability or use 
    PLIB_RTCC_ExistsCacheLineType in your application to
    to automatically determine whether this feature is available.
*/

void PLIB_PCACHE_CacheLineInst(PCACHE_MODULE_ID index);


/*******************************************************************************
  Function:
    void PLIB_PCACHE_CacheLineData( PCACHE_MODULE_ID index )

  Summary:
    Sets cache line previously write-enabled by PLIB_PCACHE_CacheLineSelect
    to data type.

  Description:
    This function sets the cache line previously enabled by
    PLIB_PCACHE_CacheLineSelect to data type.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    PLIB_PCACHE_CacheLineData(PCACHE_ID_0);
    </code>

  Remarks:
    This function implements an operation of the CacheLineType feature.
    This feature may not be available on all devices. Please refer to the 
    specific device data sheet to determine availability or use 
    PLIB_RTCC_ExistsCacheLineType in your application to
    to automatically determine whether this feature is available.
*/

void PLIB_PCACHE_CacheLineData(PCACHE_MODULE_ID index);

/*******************************************************************************
  Function:
    bool PLIB_PCACHE_CacheLineIsInst( PCACHE_MODULE_ID index )

  Summary:
    Returns true if cache line previously write-enabled by
    PLIB_PCACHE_CacheLineSelect is set to instruction type.

  Description:
    This function returns true if the cache line previously enabled by
    PLIB_PCACHE_CacheLineSelect is set to instruction type.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance to be read

  Returns:
    - true  - The cache line is instruction
    - false - The cache line is data

  Example:
    <code>
    bool CacheLineType;
    CacheLineType = PLIB_PCACHE_CacheLineIsInst(PCACHE_ID_0);
    </code>

  Remarks:
    This function implements an operation of the CacheLineType feature.
    This feature may not be available on all devices. Please refer to the 
    specific device data sheet to determine availability or use 
    PLIB_RTCC_ExistsCacheLineType in your application to
    to automatically determine whether this feature is available.
*/

bool PLIB_PCACHE_CacheLineIsInst(PCACHE_MODULE_ID index);


/*******************************************************************************
  Function:
    void PLIB_PCACHE_CacheLineLock( PCACHE_MODULE_ID index )

  Summary:
    Locks cache line previously write-enabled by PLIB_PCACHE_CacheLineSelect.

  Description:
    This function locks the cache line previously write-enabled by
    PLIB_PCACHE_CacheLineSelect.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    PLIB_PCACHE_CacheLineLock(PCACHE_ID_0);
    </code>

  Remarks:
    This function implements an operation of the CacheLineLock feature.
    This feature may not be available on all devices. Please refer to the 
    specific device data sheet to determine availability or use 
    PLIB_RTCC_ExistsCacheLineLock in your application to
    to automatically determine whether this feature is available.
*/

void PLIB_PCACHE_CacheLineLock(PCACHE_MODULE_ID index);


/*******************************************************************************
  Function:
    void PLIB_PCACHE_CacheLineUnlock( PCACHE_MODULE_ID index )

  Summary:
    Unlocks cache line previously write-enabled by
    PLIB_PCACHE_CacheLineSelect.

  Description:
    This function unlocks the cache line previously write-enabled by
    PLIB_PCACHE_CacheLineSelect.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    PLIB_PCACHE_CacheLineUnlock(PCACHE_ID_0);
    </code>

  Remarks:
    This function implements an operation of the CacheLineLock feature.
    This feature may not be available on all devices. Please refer to the 
    specific device data sheet to determine availability or use 
    PLIB_RTCC_ExistsCacheLineLock in your application to
    to automatically determine whether this feature is available.
*/

void PLIB_PCACHE_CacheLineUnlock(PCACHE_MODULE_ID index);

/*******************************************************************************
  Function:
    bool PLIB_PCACHE_CacheLineIsLocked( PCACHE_MODULE_ID index )

  Summary:
    Returns true if cache line previously write-enabled by
    PLIB_PCACHE_CacheLineSelect is locked.

  Description:
    This function returns true if the cache line previously write-enabled by
    PLIB_PCACHE_CacheLineSelect is locked.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance to be configured

  Returns:
    - true  - The cache line is locked
    - false - The cache line is unlocked

  Example:
    <code>
    bool CacheLineLocked;
    CacheLineLocked = PLIB_PCACHE_CacheLineIsLocked(PCACHE_ID_0);
    </code>

  Remarks:
   This function implements an operation of the CacheLineLock feature.
    This feature may not be available on all devices. Please refer to the 
    specific device data sheet to determine availability or use 
    PLIB_RTCC_ExistsCacheLineLock in your application to
    to automatically determine whether this feature is available.
*/

bool PLIB_PCACHE_CacheLineIsLocked(PCACHE_MODULE_ID index);


/*******************************************************************************
  Function:
    void PLIB_PCACHE_CacheLineValid( PCACHE_MODULE_ID index )

  Summary:
    Validates cache line previously write-enabled by
    PLIB_PCACHE_CacheLineSelect.

  Description:
    This function validates the cache line previously write-enabled by
    PLIB_PCACHE_CacheLineSelect.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    PLIB_PCACHE_CacheLineValid(PCACHE_ID_0);
    </code>

  Remarks:
    This function implements an operation of the CacheLineValid feature.
    This feature may not be available on all devices. Please refer to the 
    specific device data sheet to determine availability or use 
    PLIB_RTCC_ExistsCacheLineValid in your application to
    to automatically determine whether this feature is available.
*/

void PLIB_PCACHE_CacheLineValid(PCACHE_MODULE_ID index);


/*******************************************************************************
  Function:
    void PLIB_PCACHE_CacheLineInvalid( PCACHE_MODULE_ID index )

  Summary:
    Invalidates cache line previously write-enabled by
    PLIB_PCACHE_CacheLineSelect.

  Description:
    This function invalidates the cache line previously write-enabled by
    PLIB_PCACHE_CacheLineSelect.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    PLIB_PCACHE_CacheLineInvalid(PCACHE_ID_0);
    </code>

  Remarks:
    This function implements an operation of the CacheLineValid feature.
    This feature may not be available on all devices. Please refer to the 
    specific device data sheet to determine availability or use 
    PLIB_RTCC_ExistsCacheLineValid in your application to
    to automatically determine whether this feature is available.
*/

void PLIB_PCACHE_CacheLineInvalid(PCACHE_MODULE_ID index);


/*******************************************************************************
  Function:
    bool PLIB_PCACHE_CacheLineIsValid( PCACHE_MODULE_ID index )

  Summary:
    Returns true if cache line previously write-enabled by
    PLIB_PCACHE_CacheLineSelect is valid.

  Description:
    This function returns true if the cache line previously write-enabled by
    PLIB_PCACHE_CacheLineSelect is valid.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance to be configured

  Returns:
    - true  - The cache line is valid
    - false - The cache line is invalid

  Example:
    <code>
    bool CacheLineValid;
    CacheLineValid = PLIB_PCACHE_CacheLineIsValid(PCACHE_ID_0);
    </code>

  Remarks:
    This function implements an operation of the CacheLineValid feature.
    This feature may not be available on all devices. Please refer to the 
    specific device data sheet to determine availability or use 
    PLIB_RTCC_ExistsCacheLineValid in your application to
    to automatically determine whether this feature is available.
*/

bool PLIB_PCACHE_CacheLineIsValid(PCACHE_MODULE_ID index);


/*******************************************************************************
  Function:
    void PLIB_PCACHE_CacheLineAddrSet( PCACHE_MODULE_ID index, uint32_t addr )

  Summary:
    Sets the TAG address in the cache line previously write-enabled by
    PLIB_PCACHE_CacheLineSelect.

  Description:
    This function Sets the TAG address in the cache line previously write-
    enabled by PLIB_PCACHE_CacheLineSelect.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance to be configured
    addr  - Address to program into the cache line TAG register

  Returns:
    None.

  Example:
    <code>
    uint32_t addr = 0x1FC01234;
    PLIB_PCACHE_CacheLineAddrSet(PCACHE_ID_0, addr);
    </code>

  Remarks:
    This function implements an operation of the CacheLineAddr feature.
    This feature may not be available on all devices. Please refer to the 
    specific device data sheet to determine availability or use 
    PLIB_RTCC_ExistsCacheLineAddr in your application to
    to automatically determine whether this feature is available.
*/

void PLIB_PCACHE_CacheLineAddrSet(PCACHE_MODULE_ID index, uint32_t addr);


/*******************************************************************************
  Function:
    uint32_t PLIB_PCACHE_CacheLineAddrGet( PCACHE_MODULE_ID index )

  Summary:
    Gets the TAG address in the cache line previously write-enabled by
    PLIB_PCACHE_CacheLineSelect.

  Description:
    This function gets the TAG address in the cache line previously 
	write-enabled by PLIB_PCACHE_CacheLineSelect.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance to be read

  Returns:
    The address in the cache line TAG register.

  Example:
    <code>
    uint32_t addr;
    addr = PLIB_PCACHE_CacheLineAddrGet(PCACHE_ID_0);
    </code>

  Remarks:
    This function implements an operation of the CacheLineAddr feature.
    This feature may not be available on all devices. Please refer to the 
    specific device data sheet to determine availability or use 
    PLIB_RTCC_ExistsCacheLineAddr in your application to
    to automatically determine whether this feature is available.
*/

uint32_t PLIB_PCACHE_CacheLineAddrGet(PCACHE_MODULE_ID index);


/*******************************************************************************
  Function:
    void PLIB_PCACHE_CacheLineFlashTypeBoot( PCACHE_MODULE_ID index )

  Summary:
    Set the cache line tag for the line previously write-enabled by
    PLIB_PCACHE_CacheLineSelect as residing in Boot Flash.

  Description:
    This function sets the cache line tag for the line previously write-enabled
    by PLIB_PCACHE_CacheLineSelect as residing in Boot Flash.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    PLIB_PCACHE_CacheLineFlashTypeBoot(PCACHE_ID_0);
    </code>

  Remarks:
    This function implements an operation of the CacheLineFlashType feature.
    This feature may not be available on all devices. Please refer to the 
    specific device data sheet to determine availability or use 
    PLIB_RTCC_ExistsCacheLineFlashType in your application to
    to automatically determine whether this feature is available.
*/

void PLIB_PCACHE_CacheLineFlashTypeBoot(PCACHE_MODULE_ID index);


/*******************************************************************************
  Function:
    void PLIB_PCACHE_CacheLineFlashTypeInst( PCACHE_MODULE_ID index )

  Summary:
    Set the cache line tag for the line previously write-enabled by
    PLIB_PCACHE_CacheLineSelect as residing in Instruction Flash.

  Description:
    This function sets the cache line tag for the line previously write-enabled
    by PLIB_PCACHE_CacheLineSelect as residing in Instruction Flash.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    PLIB_PCACHE_CacheLineFlashTypeInst(PCACHE_ID_0);
    </code>

  Remarks:
    This function implements an operation of the CacheLineFlashType feature.
    This feature may not be available on all devices. Please refer to the 
    specific device data sheet to determine availability or use 
    PLIB_RTCC_ExistsCacheLineFlashType in your application to
    to automatically determine whether this feature is available.
*/

void PLIB_PCACHE_CacheLineFlashTypeInst(PCACHE_MODULE_ID index);


/*******************************************************************************
  Function:
    bool PLIB_PCACHE_CacheLineFlashTypeIsInst( PCACHE_MODULE_ID index )

  Summary:
    Returns true if the cache line tag for the line previously write-enabled by
    PLIB_PCACHE_CacheLineSelect shows the line is residing in Instruction Flash.

  Description:
    This function returns true if the cache line tag for the line previously
    write-enabled by PLIB_PCACHE_CacheLineSelect shows the line is residing
    in Instruction Flash.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance to be configured

  Returns:
    - true  - The cache line resides in Instruction Flash
    - false - The cache line resides in Boot Flash

  Example:
    <code>
    bool LineFlashType;
    LineFlashType = PLIB_PCACHE_CacheLineFlashTypeIsInst(PCACHE_ID_0);
    </code>

  Remarks:
    This function implements an operation of the CacheLineFlashType feature.
    This feature may not be available on all devices. Please refer to the 
    specific device data sheet to determine availability or use 
    PLIB_RTCC_ExistsCacheLineFlashType in your application to
    to automatically determine whether this feature is available.
*/

bool PLIB_PCACHE_CacheLineFlashTypeIsInst(PCACHE_MODULE_ID index);


/*******************************************************************************
  Function:
    void PLIB_PCACHE_CacheLineMaskSet( PCACHE_MODULE_ID index, uint32_t mask )

  Summary:
    Sets the cache tag mask to force a match on set bits on the cache line
    previously write-enabled by PLIB_PCACHE_CacheLineSelect.

  Description:
    This function sets the cache tag mask to force a match on set bits on the
    cache line previously write-enabled by PLIB_PCACHE_CacheLineSelect.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance to be configured
    mask  - Address mask (bits set to '1' will force a match; valid bits: <15:5>)

  Returns:
    None.

  Example:
    <code>
    uint32_t mask = 0x0C0;
    PLIB_PCACHE_CacheLineMaskSet(PCACHE_ID_0, mask);
    </code>

  Remarks:
    This function implements an operation of the CacheLineMask feature.
    This feature may not be available on all devices. Please refer to the 
    specific device data sheet to determine availability or use 
    PLIB_RTCC_ExistsCacheLineMask in your application to
    to automatically determine whether this feature is available.
*/

void PLIB_PCACHE_CacheLineMaskSet(PCACHE_MODULE_ID index, uint32_t mask);


/*******************************************************************************
  Function:
    uint32_t PLIB_PCACHE_CacheLineMaskGet( PCACHE_MODULE_ID index )

  Summary:
    Returns the current state of the cache tag mask for the cache line
    previously write-enabled by PLIB_PCACHE_CacheLineSelect.

  Description:
    This function returns the current state of the cache tag mask for the cache
    line previously write-enabled by PLIB_PCACHE_CacheLineSelect.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance to be read

  Returns:
    * Address mask - Bits set to '1' will force a match (valid bits: <15:5>).

  Example:
    <code>
    uint32_t mask;
    mask = PLIB_PCACHE_CacheLineMaskGet(PCACHE_ID_0);
    </code>

  Remarks:
    This function implements an operation of the CacheLineMask feature.
    This feature may not be available on all devices. Please refer to the 
    specific device data sheet to determine availability or use 
    PLIB_RTCC_ExistsCacheLineMask in your application to
    to automatically determine whether this feature is available.
*/

uint32_t PLIB_PCACHE_CacheLineMaskGet(PCACHE_MODULE_ID index);


/*******************************************************************************
  Function:
    uint32_t PLIB_PCACHE_WordRead( PCACHE_MODULE_ID index, uint32_t word )

  Summary:
    Reads the word specified by word from cache line previously write-enabled by
    PLIB_PCACHE_CacheLineSelect.

  Description:
    This function reads the word specified by word from the cache line
    previously write-enabled by PLIB_PCACHE_CacheLineSelect.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance to be read
    word  - Location of the word in the data array to be read

  Returns:
    A 32-bit unsigned word from the cache data array.

  Example:
    <code>
    uint32_t word;
    word = PLIB_PCACHE_WordRead(PCACHE_ID_0, 0);
    </code>

  Remarks:
    This function implements an operation of the Word feature.
    This feature may not be available on all devices. Please refer to the 
    specific device data sheet to determine availability or use 
    PLIB_RTCC_ExistsWord in your application to
    to automatically determine whether this feature is available.
*/

uint32_t PLIB_PCACHE_WordRead(PCACHE_MODULE_ID index, uint32_t word);


/*******************************************************************************
  Function:
    void PLIB_PCACHE_WordWrite( PCACHE_MODULE_ID index, uint32_t word, \
                                uint32_t data )

  Summary:
    Writes the word (specified by word parameter) with data (specified by data
    parameter) to cache line previously write-enabled by
    PLIB_PCACHE_CacheLineSelect.

  Description:
    This function writes the word (specified by word parameter) with data
    (specified by data parameter) to the cache line previously write-enabled by
    PLIB_PCACHE_CacheLineSelect.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance to be written.
    word  - Location of the word in the data array to be written
    data  - 32-bit unsigned word to write to cache data array

  Returns:
    None.

  Example:
    <code>
    uint32_t word = 0;
    uint32_t data = 0xDEADBEEF;
    PLIB_PCACHE_WordWrite(PCACHE_ID_0, word, data);
    </code>

  Remarks:
    This function implements an operation of the Word feature.
    This feature may not be available on all devices. Please refer to the 
    specific device data sheet to determine availability or use 
    PLIB_RTCC_ExistsWord in your application to
    to automatically determine whether this feature is available.
*/

void PLIB_PCACHE_WordWrite(PCACHE_MODULE_ID index, uint32_t word, uint32_t data);


/*******************************************************************************
  Function:
    uint32_t PLIB_PCACHE_LeastRecentlyUsedStateRead( PCACHE_MODULE_ID index )

  Summary:
    Reads the state of the cache Least Recently Used (LRU) encoding bits.

  Description:
    This function reads the state of the cache LRU encoding
    bits.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance to be read.

  Returns:
    The state of the LRU encoding bits represented as a 25-bit unsigned integer.

  Example:
    <code>
    uint32_t lru_state;
    lru_state = PLIB_PCACHE_LeastRecentlyUsedStateRead(PCACHE_ID_0);
    </code>

  Remarks:
    This function implements an operation of the LeastRecentlyUsedState feature.
    This feature may not be available on all devices. Please refer to the 
    specific device data sheet to determine availability or use 
    PLIB_RTCC_ExistsLeastRecentlyUsedState in your application to
    to automatically determine whether this feature is available.
*/

uint32_t PLIB_PCACHE_LeastRecentlyUsedStateRead(PCACHE_MODULE_ID index);


/*******************************************************************************
  Function:
    uint32_t PLIB_PCACHE_CacheHitRead( PCACHE_MODULE_ID index )

  Summary:
    Reads the number of cache hits.

  Description:
    This function reads and returns the number of cache hits.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance to be read

  Returns:
    The number of cache hits.

  Example:
    <code>
    uint32_t cache_hits;
    cache_hits = PLIB_PCACHE_CacheHitRead(PCACHE_ID_0);
    </code>

  Remarks:
    This function implements an operation of the CacheHit feature.
    This feature may not be available on all devices. Please refer to the 
    specific device data sheet to determine availability or use 
    PLIB_RTCC_ExistsCacheHit in your application to
    to automatically determine whether this feature is available.
*/

uint32_t PLIB_PCACHE_CacheHitRead(PCACHE_MODULE_ID index);


/*******************************************************************************
  Function:
    void PLIB_PCACHE_CacheHitWrite( PCACHE_MODULE_ID index, uint32_t data )

  Summary:
    Sets the number of cache hits.

  Description:
    This function sets the number of cache hits.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance to be written
    data  - Number of cache hits to write

  Returns:
    None.

  Example:
    <code>
    PLIB_PCACHE_CacheHitWrite(PCACHE_ID_0, 0xF00);
    </code>

  Remarks:
    This function implements an operation of the CacheHit feature.
    This feature may not be available on all devices. Please refer to the 
    specific device data sheet to determine availability or use 
    PLIB_RTCC_ExistsCacheHit in your application to
    to automatically determine whether this feature is available.
*/

void PLIB_PCACHE_CacheHitWrite(PCACHE_MODULE_ID index, uint32_t data);


/*******************************************************************************
  Function:
    uint32_t PLIB_PCACHE_CacheMissRead( PCACHE_MODULE_ID index )

  Summary:
    Reads the number of cache misses.

  Description:
    This function reads and returns the number of cache misses.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance to be read.

  Returns:
    The number of cache misses.

  Example:
    <code>
    uint32_t cache_misses;
    cache_misses = PLIB_PCACHE_CacheMissRead(PCACHE_ID_0);
    </code>

  Remarks:
    This function implements an operation of the CacheMiss feature.
    This feature may not be available on all devices. Please refer to the 
    specific device data sheet to determine availability or use 
    PLIB_RTCC_ExistsCacheMiss in your application to
    to automatically determine whether this feature is available.
*/

uint32_t PLIB_PCACHE_CacheMissRead(PCACHE_MODULE_ID index);


/*******************************************************************************
  Function:
    void PLIB_PCACHE_CacheMissWrite( PCACHE_MODULE_ID index, uint32_t data )

  Summary:
    Sets the number of cache misses.

  Description:
    This function sets the number of cache misses.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance to be written
    data  - Number of cache misses to write

  Returns:
    None.

  Example:
    <code>
    PLIB_PCACHE_CacheMissWrite(PCACHE_ID_0, 0xF00);
    </code>

  Remarks:
    This function implements an operation of the CacheMiss feature.
    This feature may not be available on all devices. Please refer to the 
    specific device data sheet to determine availability or use 
    PLIB_RTCC_ExistsCacheMiss in your application to
    to automatically determine whether this feature is available.
*/

void PLIB_PCACHE_CacheMissWrite(PCACHE_MODULE_ID index, uint32_t data);


/*******************************************************************************
  Function:
    uint32_t PLIB_PCACHE_PrefetchAbortRead( PCACHE_MODULE_ID index )

  Summary:
    Reads the number of prefetch aborts.

  Description:
    This function reads and returns the number of prefetch aborts.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance to be read

  Returns:
    The number of prefetch aborts.

  Example:
    <code>
    uint32_t pfabt;
    pfabt = PLIB_PCACHE_PrefetchAbortRead(PCACHE_ID_0);
    </code>

  Remarks:
    This function implements an operation of the PrefetchAbort feature.
    This feature may not be available on all devices. Please refer to the 
    specific device data sheet to determine availability or use 
    PLIB_RTCC_ExistsPrefetchAbort in your application to
    to automatically determine whether this feature is available.
*/

uint32_t PLIB_PCACHE_PrefetchAbortRead(PCACHE_MODULE_ID index);


/*******************************************************************************
  Function:
    void PLIB_PCACHE_PrefetchAbortWrite( PCACHE_MODULE_ID index, uint32_t data )

  Summary:
    Sets the number of prefetch aborts.

  Description:
    This function Sets the number of prefetch aborts.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance to be written
    data  - Number of prefetch aborts

  Returns:
    None.

  Example:
    <code>
    PLIB_PCACHE_PrefetchAbortWrite(PCACHE_ID_0, 0xF00);
    </code>

  Remarks:
    This function implements an operation of the PrefetchAbort feature.
    This feature may not be available on all devices. Please refer to the 
    specific device data sheet to determine availability or use 
    PLIB_RTCC_ExistsPrefetchAbort in your application to
    to automatically determine whether this feature is available.
*/

void PLIB_PCACHE_PrefetchAbortWrite(PCACHE_MODULE_ID index, uint32_t data);

/*******************************************************************************
  Function:
    void PCACHE_CacheEnable( PCACHE_MODULE_ID index, PCACHE_CACHE_TYPE cache_type );

  Summary:
    Enables Instruction Cache.

  Description:
    This function Enables Instruction Cache.

  Precondition:
    None.

  Parameters:
    index     - Identifier for the device instance to be configured
	cache_type - Identifier for Peripheral, Data and Instruction Cache 
  Returns:
    None.

  Example:
    <code>
    PLIB_PCACHE_CacheEnable( PCACHE_ID_0, PLIB_PCACHE_PERIPHERAL_CACHE );
    </code>

  Remarks:
    Instruction cache is enabled.

    This function implements an operation of the CacheEnable feature.
    This feature may not be available on all devices. Please refer to the 
    specific device data sheet to determine availability or use 
    PLIB_PCACHE_ExistsCacheEnable in your application to
    to automatically determine whether this feature is available.
*/

void PLIB_PCACHE_CacheEnable(PCACHE_MODULE_ID index, PCACHE_CACHE_TYPE cache_type);

/*******************************************************************************
  Function:
    void PCACHE_CacheDisable( PCACHE_MODULE_ID index, PCACHE_CACHE_TYPE cache_type );

  Summary:
    Disable Cache.

  Description:
    This function Disables Cache.

  Precondition:
    None.

  Parameters:
    index     - Identifier for the device instance to be configured
	cache_type - Identifier for Peripheral, Data and Instruction Cache 
	
  Returns:
    None.

  Example:
    <code>
    PLIB_PCACHE_CacheDisable( PCACHE_ID_0, PLIB_PCACHE_PERIPHERAL_CACHE  );
    </code>

  Remarks:
  Cache is Disabled.

    This function implements an operation of the CacheDisable feature.
    This feature may not be available on all devices. Please refer to the 
    specific device data sheet to determine availability or use 
    PLIB_PCACHE_ExistsCacheEnable in your application to
    to automatically determine whether this feature is available.
*/

void PLIB_PCACHE_CacheDisable(PCACHE_MODULE_ID index, PCACHE_CACHE_TYPE cache_type);

/*******************************************************************************
  Function:
    bool PCACHE_CacheIsEnabled( PCACHE_MODULE_ID index, PCACHE_CACHE_TYPE cache_type );

  Summary:
    Checks Cache is Enabled or Not.

  Description:
    This function helps to check Cache is Enable or Disable.

  Precondition:
    None.

  Parameters:
    index     - Identifier for the device instance to be configured
	cache_type - Identifier for Peripheral, Data and Instruction Cache 
	
  Returns:
    - true  -  Cache is Enabled
    - false -  Cache is Disabled

  Example:
    <code>
    PLIB_PCACHE_CacheIsEnabled( PCACHE_ID_0, PLIB_PCACHE_PERIPHERAL_CACHE );
    </code>

  Remarks:
    Cache is Enable/Disable.

    This function implements an operation of the CacheIsEnable feature.
    This feature may not be available on all devices. Please refer to the 
    specific device data sheet to determine availability or use 
    PLIB_PCACHE_ExistsCacheEnable in your application to
    to automatically determine whether this feature is available.
*/

bool PLIB_PCACHE_CacheIsEnabled(PCACHE_MODULE_ID index, PCACHE_CACHE_TYPE cache_type);

/*******************************************************************************
  Function:
    void PCACHE_ForceInvalidateCacheEnable( PCACHE_MODULE_ID index, PCACHE_CACHE_TYPE cache_type );

  Summary:
    Force Invalidates Cache/Invalidate Busy.

  Description:
    This function Invalidates Cache/Invalidate Busy.

  Precondition:
    None.

  Parameters:
    index     - Identifier for the device instance to be configured
	cache_type - Identifier for Peripheral, Data and Instruction Cache 
	
  Returns:
    None.

  Example:
    <code>
    PLIB_PCACHE_ForceInvalidateCacheEnable( PCACHE_ID_0, PLIB_PCACHE_PERIPHERAL_CACHE );
    </code>

  Remarks:
    This function implements an operation to force Invalidate Cache.
    This feature may not be available on all devices. Please refer to the 
    specific device data sheet to determine availability or use 
    PLIB_PCACHE_ExistsInvalidateCache in your application to
    to automatically determine whether this feature is available.
*/

void PLIB_PCACHE_ForceInvalidateCacheEnable(PCACHE_MODULE_ID index, PCACHE_CACHE_TYPE cache_type);

/*******************************************************************************
  Function:
    bool PCACHE_ForceInvalidateCacheIsEnabled( PCACHE_MODULE_ID index, PCACHE_CACHE_TYPE cache_type );

  Summary:
    Force Invalidates Cache/Invalidate Busy feature is set or not.

  Description:
    This function checks Force Invalidates Cache/Invalidate Busy feature is 
	set or not.

  Precondition:
    None.

  Parameters:
    index     - Identifier for the device instance to be configured
	cache_type - Identifier for Peripheral, Data and Instruction Cache 

  Returns:
    - true  - Force invalidate cache/invalidate busy feature is enabled.
    - false - Cache Invalidation follows ICHECOH/invalid complete feature is enabled.

  Example:
    <code>
    PLIB_PCACHE_ForceInvalidateCacheIsEnabled( PCACHE_ID_0,PLIB_PCACHE_PERIPHERAL_CACHE );
    </code>

  Remarks:
    This function checks Force Invalidates Cache/Invalidate Busy feature is set
	or not. This feature may not be available on all devices. Please refer to the 
    specific device data sheet to determine availability or use 
    PLIB_PCACHE_ExistsInvalidateCache in your application to
    to automatically determine whether this feature is available.
*/

bool PLIB_PCACHE_ForceInvalidateCacheIsEnabled(PCACHE_MODULE_ID index, PCACHE_CACHE_TYPE cache_type);

/*******************************************************************************
  Function:
    void PCACHE_AutoInvalidateCacheEnable( PCACHE_MODULE_ID index, PCACHE_CACHE_TYPE cache_type );

  Summary:
    Automatically invalidate cache on a programming event.

  Description:
    This function Automatically invalidates cache on a programming event.

  Precondition:
    None.

  Parameters:
    index     - Identifier for the device instance to be configured
	cache_type - Identifier for Peripheral, Data and Instruction Cache 	

  Returns:
    None.

  Example:
    <code>
    PLIB_PCACHE_AutoInvalidateCacheEnable( PCACHE_ID_0,PLIB_PCACHE_PERIPHERAL_CACHE );
    </code>

  Remarks:
    This function implements an operation to Automatically Invalidate 
	Instruction Cache on a programming event.
    This feature may not be available on all devices. Please refer to the 
    specific device data sheet to determine availability or use 
    PLIB_PCACHE_ExistsInvalidateCache in your application to
    to automatically determine whether this feature is available.
*/

void PLIB_PCACHE_AutoInvalidateCacheEnable(PCACHE_MODULE_ID index, PCACHE_CACHE_TYPE cache_type);

/*******************************************************************************
  Function:
    void PCACHE_AutoInvalidateCacheDisable( PCACHE_MODULE_ID index, PCACHE_CACHE_TYPE cache_type );

  Summary:
    Do not Automatically invalidate cache on a programming event.

  Description:
    This function will not let Automatically invalidate cache on a 
	programming event.

  Precondition:
    None.

  Parameters:
    index     - Identifier for the device instance to be configured
	cache_type - Identifier for Peripheral, Data and Instruction Cache 

  Returns:
    None.

  Example:
    <code>
    PLIB_PCACHE_AutoInvalidateCacheDisable( PCACHE_ID_0,PLIB_PCACHE_PERIPHERAL_CACHE );
    </code>

  Remarks:
    This function will not let Automatically invalidate Instruction cache on a 
	programming event.
    This feature may not be available on all devices. Please refer to the 
    specific device data sheet to determine availability or use 
    PLIB_PCACHE_ExistsInvalidateCache in your application to
    to automatically determine whether this feature is available.
*/

void PLIB_PCACHE_AutoInvalidateCacheDisable(PCACHE_MODULE_ID index, PCACHE_CACHE_TYPE cache_type);

/*******************************************************************************
  Function:
    bool PCACHE_AutoInvalidateCacheIsEnabled( PCACHE_MODULE_ID index, PCACHE_CACHE_TYPE cache_type );

  Summary:
    Checks whether Cache Automatically gets Invalidate or not.

  Description:
    This function Checks whether Cache Automatically gets Invalidate 
	or not.

  Precondition:
    None.

  Parameters:
    index     - Identifier for the device instance to be configured
	cache_type - Identifier for Peripheral, Data and Instruction Cache 	

  Returns:
    - true  - Automatically invalidate cache on a programming event feature is
	          enabled.
    - false - Do not automatically invalidate cache on a programming event feature 
			  is enabled.

  Example:
    <code>
    PLIB_PCACHE_AutoInvalidateCacheIsEnabled( PCACHE_ID_0,PLIB_PCACHE_PERIPHERAL_CACHE );
    </code>

  Remarks:
    This function checks Auto Invalidate Cache feature is set or not.
	This feature may not be available on all devices. Please refer to the 
    specific device data sheet to determine availability or use 
    PLIB_PCACHE_ExistsInvalidateCache in your application to
    to automatically determine whether this feature is available.
*/

bool PLIB_PCACHE_AutoInvalidateCacheIsEnabled(PCACHE_MODULE_ID index, PCACHE_CACHE_TYPE cache_type);

/*******************************************************************************
  Function:
    void PCACHE_PFMAddressWaitStateEnable( PCACHE_MODULE_ID index );

  Summary:
    Add one more Wait State to flash address setup (suggested for higher system clock frequencies).

  Description:
    This function Add one more Wait State to flash address setup.

  Precondition:
    None.

  Parameters:
    index     - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    PLIB_PCACHE_PFMAddressWaitStateEnable( PCACHE_ID_0 );
    </code>

  Remarks:
    This function Add one more Wait State to flash address setup.
    This feature may not be available on all devices. Please refer to the 
    specific device data sheet to determine availability or use 
    PLIB_PCACHE_ExistsPFMAddressWaitStateEnable in your application to
    to automatically determine whether this feature is available.
*/

void PLIB_PCACHE_PFMAddressWaitStateEnable(PCACHE_MODULE_ID index);

/*******************************************************************************
  Function:
    void PCACHE_PFMAddressWaitStateDisable( PCACHE_MODULE_ID index );

  Summary:
    Add no Wait States to the flash address setup (suggested for lower system 
	clock frequencies to achieve higher performance).

  Description:
    This function Add no Wait States to the flash address setup.

  Precondition:
    None.

  Parameters:
    index     - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    PLIB_PCACHE_PFMAddressWaitStateDisable( PCACHE_ID_0 );
    </code>

  Remarks:
    This function Add no Wait States to the flash address setup.
    This feature may not be available on all devices. Please refer to the 
    specific device data sheet to determine availability or use 
    PLIB_PCACHE_ExistsPFMAddressWaitStateEnable in your application to
    to automatically determine whether this feature is available.
*/

void PLIB_PCACHE_PFMAddressWaitStateDisable(PCACHE_MODULE_ID index);

/*******************************************************************************
  Function:
    bool PCACHE_PFMAddressWaitStateIsEnabled( PCACHE_MODULE_ID index );

  Summary:
    Checks Program Flash Memory Address Wait State Enable feature Exists or not

  Description:
    This function checks Program Flash Memory Address Wait State Enable feature
	Exists or not

  Precondition:
    None.

  Parameters:
    index     - Identifier for the device instance to be configured

  Returns:
	- true  - Add one more Wait State to flash address setup feature is enabled.
    - false - Add No Wait State to flash address setup feature is enabled

  Example:
    <code>
    PLIB_PCACHE_PFMAddressWaitStateIsEnabled( PCACHE_ID_0 );
    </code>

  Remarks:
    This function checks Program Flash Memory Address Wait State Enable feature
	is available or not.
    This feature may not be available on all devices. Please refer to the 
    specific device data sheet to determine availability or use 
    PLIB_PCACHE_ExistsPFMAddressWaitStateEnable in your application to
    to automatically determine whether this feature is available.
*/

bool PLIB_PCACHE_PFMAddressWaitStateIsEnabled(PCACHE_MODULE_ID index);

/*******************************************************************************
  Function:
    void PCACHE_CachePerformanceCountersEnable( PCACHE_MODULE_ID index );

  Summary:
    Cache performance counters gets Enable.

  Description:
    This function enables Cache performance counters.

  Precondition:
    None.

  Parameters:
    index     - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    PLIB_PCACHE_CachePerformanceCountersEnable( PCACHE_ID_0 );
    </code>

  Remarks:
    This function enables Cache performance counters.
    This feature may not be available on all devices. Please refer to the 
    specific device data sheet to determine availability or use 
    PLIB_PCACHE_ExistsCachePerformanceCountersEnable in your application to
    to automatically determine whether this feature is available.
*/

void PLIB_PCACHE_CachePerformanceCountersEnable(PCACHE_MODULE_ID index);

/*******************************************************************************
  Function:
    void PCACHE_CachePerformanceCountersDisable( PCACHE_MODULE_ID index );

  Summary:
    Cache performance counters gets Disable.

  Description:
    This function Disables Cache performance counters.

  Precondition:
    None.

  Parameters:
    index     - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    PLIB_PCACHE_CachePerformanceCountersDisable( PCACHE_ID_0 );
    </code>

  Remarks:
    This function Disables Cache performance counters.
    This feature may not be available on all devices. Please refer to the 
    specific device data sheet to determine availability or use 
    PLIB_PCACHE_ExistsCachePerformanceCountersEnable in your application to
    to automatically determine whether this feature is available.
*/

void PLIB_PCACHE_CachePerformanceCountersDisable(PCACHE_MODULE_ID index);

/*******************************************************************************
  Function:
    bool PCACHE_CachePerformanceCountersIsEnabled( PCACHE_MODULE_ID index );

  Summary:
    Checks if Cache performance counters is Enable/Disable.

  Description:
    This function checks if Cache performance counters is Enable/Disable.

  Precondition:
    None.

  Parameters:
    index     - Identifier for the device instance to be configured

  Returns:
    - true  - If Cache Performance counters is Enabled.
    - false - If Cache Performance counters is Disabled.

  Example:
    <code>
    PLIB_PCACHE_CachePerformanceCountersIsEnabled( PCACHE_ID_0 );
    </code>

  Remarks:
    This function checks if Cache performance counters is Enable/Disable.
    This feature may not be available on all devices. Please refer to the 
    specific device data sheet to determine availability or use 
    PLIB_PCACHE_ExistsCachePerformanceCountersEnable in your application to
    to automatically determine whether this feature is available.
*/

bool PLIB_PCACHE_CachePerformanceCountersIsEnabled(PCACHE_MODULE_ID index);

// *****************************************************************************
// *****************************************************************************
// Section: PCACHE Peripheral Library Exists Functions
// *****************************************************************************
// *****************************************************************************
/* The following functions indicate the existence of the features on the device.
*/

//******************************************************************************
/* Function:
    PLIB_PCACHE_ExistsCachePerformanceCountersEnable( PCACHE_MODULE_ID index )

  Summary:
    Identifies Cache Performance Counters Enable feature is available or not.

  Description:
    This interface identifies Cache Performance Counters Enable feature is available
	or not.
	When this interface returns true, these functions are 
    supported on the device:
    - PLIB_PCACHE_CachePerformanceCountersEnable
    - PLIB_PCACHE_CachePerformanceCountersDisable
	- PLIB_PCACHE_CachePerformanceCountersIsEnabled

  Preconditions:
    None.

  Parameters:
    index    - Identifier for the device instance

  Returns:
    - true   - Cache Performance Counters Enable feature is
			   available on this device.
    - false  - Cache Performance Counters Enable feature is
			   not available on this device.

  Remarks:
    None.
*/

bool PLIB_PCACHE_ExistsCachePerformanceCountersEnable( PCACHE_MODULE_ID index );

//******************************************************************************
/* Function:
    PLIB_PCACHE_ExistsPFMAddressWaitStateEnable( PCACHE_MODULE_ID index )

  Summary:
    Identifies Program Flash Memory Address Wait State Enable feature is available
	or not.

  Description:
    This interface identifies Program Flash Memory Address Wait State Enable feature
	is available or not.
	When this interface returns true, these functions are 
    supported on the device:
    - PLIB_PCACHE_PFMAddressWaitStateEnable
    - PLIB_PCACHE_PFMAddressWaitStateDisable
	- PLIB_PCACHE_PFMAddressWaitStateIsEnabled

  Preconditions:
    None.

  Parameters:
    index    - Identifier for the device instance

  Returns:
    - true   - Program Flash Memory Address Wait State Enable feature is
			   available on this device
    - false  - Program Flash Memory Address Wait State Enable feature is
			   not available on this device

  Remarks:
    None.
*/

bool PLIB_PCACHE_ExistsPFMAddressWaitStateEnable( PCACHE_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_PCACHE_ExistsCacheEnable( PCACHE_MODULE_ID index )

  Summary:
    Identifies that the Cache is available or not.

  Description:
    This interface identifies that the Instruction Cache is available or not.
	When this interface returns true, these functions are 
    supported on the device:
    - PLIB_PCACHE_CacheEnable
    - PLIB_PCACHE_CacheDisable
	- PLIB_PCACHE_CacheIsEnabled

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   -  Cache is available on this device
    - false  -  Cache is not available on this device

  Remarks:
    None.
*/

bool PLIB_PCACHE_ExistsCacheEnable( PCACHE_MODULE_ID index );

//******************************************************************************
/* Function:
    PLIB_PCACHE_ExistsInvalidateCache( PCACHE_MODULE_ID index )

  Summary:
    Identifies InvalidateCache feature exists on Prefetch Cache module.

  Description:
    This interface identifies InvalidateCache feature exists on Prefetch Cache module.
	When this interface returns true, these functions are 
    supported on the device:
	- PLIB_PCACHE_ForceInvalidateCacheEnable
	- PLIB_PCACHE_ForceInvalidateCacheIsEnabled
	- PLIB_PCACHE_AutoInvalidateCacheEnable
	- PLIB_PCACHE_AutoInvalidateCacheDisable
	- PLIB_PCACHE_AutoInvalidateCacheIsEnabled

  Preconditions:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - true   - InvalidateCache feature is available on this device
    - false  - InvalidateCache feature is not available on this device

  Remarks:
    None.
*/

bool PLIB_PCACHE_ExistsInvalidateCache( PCACHE_MODULE_ID index );

//******************************************************************************
/* Function:
    PLIB_PCACHE_ExistsWaitState( PCACHE_MODULE_ID index )

  Summary:
    Identifies that the WaitState feature exists on the Prefetch Cache module.

  Description:
    This interface identifies that the WaitState feature is available on the
    Prefetch Cache module. When this interface returns true, these functions are 
    supported on the device:
    - PLIB_PCACHE_WaitStateSet
    - PLIB_PCACHE_WaitStateGet

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The WaitState feature is supported on the device
    - false  - The WaitState feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_PCACHE_ExistsWaitState( PCACHE_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_PCACHE_ExistsPrefetchEnable( PCACHE_MODULE_ID index )

  Summary:
    Identifies that the PrefetchEnable feature exists on the Prefetch Cache module.

  Description:
    This interface identifies that the PrefetchEnable feature is available on the
    Prefetch Cache module. When this interface returns true, these functions are 
    supported on the device:
    - PLIB_PCACHE_PrefetchEnableSet
    - PLIB_PCACHE_PrefetchEnableGet

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The PrefetchEnable feature is supported on the device
    - false  - The PrefetchEnable feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_PCACHE_ExistsPrefetchEnable( PCACHE_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_PCACHE_ExistsDataCacheEnable( PCACHE_MODULE_ID index )

  Summary:
    Identifies that the DataCacheEnable feature exists on the Prefetch Cache module.

  Description:
    This interface identifies that the DataCacheEnable feature is available on the
    Prefetch Cache module. When this interface returns true, these functions are 
    supported on the device:
    - PLIB_PCACHE_DataCacheEnableSet
    - PLIB_PCACHE_DataCacheEnableGet

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The DataCacheEnable feature is supported on the device
    - false  - The DataCacheEnable feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_PCACHE_ExistsDataCacheEnable( PCACHE_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_PCACHE_ExistsFlashSECInt( PCACHE_MODULE_ID index )

  Summary:
    Identifies that the FlashSECInt feature exists on the Prefetch Cache module.

  Description:
    This interface identifies that the FlashSECInt feature is available on the
    Prefetch Cache module. When this interface returns true, these functions are 
    supported on the device:
    - PLIB_PCACHE_FlashSECIntEnable
    - PLIB_PCACHE_FlashSECIntDisable

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The FlashSECInt feature is supported on the device
    - false  - The FlashSECInt feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_PCACHE_ExistsFlashSECInt( PCACHE_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_PCACHE_ExistsFlashDEDStatus( PCACHE_MODULE_ID index )

  Summary:
    Identifies that the FlashDEDStatus feature exists on the Prefetch Cache module.

  Description:
    This interface identifies that the FlashDEDStatus feature is available on the
    Prefetch Cache module. When this interface returns true, these functions are 
    supported on the device:
    - PLIB_PCACHE_FlashDEDStatusGet
    - PLIB_PCACHE_FlashDEDStatusClear

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The FlashDEDStatus feature is supported on the device
    - false  - The FlashDEDStatus feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_PCACHE_ExistsFlashDEDStatus( PCACHE_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_PCACHE_ExistsFlashSECStatus( PCACHE_MODULE_ID index )

  Summary:
    Identifies that the FlashSECStatus feature exists on the Prefetch Cache module.

  Description:
    This interface identifies that the FlashSECStatus feature is available on the
    Prefetch Cache module. When this interface returns true, these functions are 
    supported on the device:
    - PLIB_PCACHE_FlashSECStatusGet
    - PLIB_PCACHE_FlashSECStatusSet
    - PLIB_PCACHE_FlashSECStatusClear

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The FlashSECStatus feature is supported on the device
    - false  - The FlashSECStatus feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_PCACHE_ExistsFlashSECStatus( PCACHE_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_PCACHE_ExistsFlashSECCount( PCACHE_MODULE_ID index )

  Summary:
    Identifies that the FlashSECCount feature exists on the Prefetch Cache module.

  Description:
    This interface identifies that the FlashSECCount feature is available on the
    Prefetch Cache module. When this interface returns true, these functions are 
    supported on the device:
    - PLIB_PCACHE_FlashSECCountSet
    - PLIB_PCACHE_FlashSECCountGet

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The FlashSECCount feature is supported on the device
    - false  - The FlashSECCount feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_PCACHE_ExistsFlashSECCount( PCACHE_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_PCACHE_ExistsInvalidateOnPFMProgram( PCACHE_MODULE_ID index )

  Summary:
    Identifies that the InvalidateOnPFMProgram feature exists on the Prefetch 
    Cache module.

  Description:
    This interface identifies that the InvalidateOnPFMProgram feature is available on the
    Prefetch Cache module. When this interface returns true, these functions are 
    supported on the device:
    - PLIB_PCACHE_InvalidateOnPFMProgramAll
    - PLIB_PCACHE_InvalidateOnPFMProgramUnlocked

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The InvalidateOnPFMProgram feature is supported on the device
    - false  - The InvalidateOnPFMProgram feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_PCACHE_ExistsInvalidateOnPFMProgram( PCACHE_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_PCACHE_ExistsCacheLine( PCACHE_MODULE_ID index )

  Summary:
    Identifies that the CacheLineSelect feature exists on the Prefetch Cache 
    module.

  Description:
    This interface identifies that the CacheLineSelect feature is available on the
    Prefetch Cache module. When this interface returns true, these functions are 
    supported on the device:
    - PLIB_PCACHE_CacheLineSelect
    - PLIB_PCACHE_CacheLineDeselect

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The CacheLineSelect feature is supported on the device
    - false  - The CacheLineSelect feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_PCACHE_ExistsCacheLine( PCACHE_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_PCACHE_ExistsCacheLineType( PCACHE_MODULE_ID index )

  Summary:
    Identifies that the CacheLineType feature exists on the Prefetch Cache module.

  Description:
    This interface identifies that the CacheLineType feature is available on the
    Prefetch Cache module. When this interface returns true, these functions are 
    supported on the device:
    - PLIB_PCACHE_CacheLineData
    - PLIB_PCACHE_CacheLineInst
    - PLIB_PCACHE_CacheLineIsInst

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The CacheLineType feature is supported on the device
    - false  - The CacheLineType feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_PCACHE_ExistsCacheLineType( PCACHE_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_PCACHE_ExistsCacheLineLock( PCACHE_MODULE_ID index )

  Summary:
    Identifies that the CacheLineLock feature exists on the Prefetch Cache module.

  Description:
    This interface identifies that the CacheLineLock feature is available on the
    Prefetch Cache module. When this interface returns true, these functions are 
    supported on the device:
    - PLIB_PCACHE_CacheLineLock
    - PLIB_PCACHE_CacheLineUnlock
    - PLIB_PCACHE_CacheLineIsLocked

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The CacheLineLock feature is supported on the device
    - false  - The CacheLineLock feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_PCACHE_ExistsCacheLineLock( PCACHE_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_PCACHE_ExistsCacheLineValid( PCACHE_MODULE_ID index )

  Summary:
    Identifies that the CacheLineValid feature exists on the Prefetch Cache module.

  Description:
    This interface identifies that the CacheLineValid feature is available on the
    Prefetch Cache module. When this interface returns true, these functions are 
    supported on the device:
    - PLIB_PCACHE_CacheLineValid
    - PLIB_PCACHE_CacheLineInvalid
    - PLIB_PCACHE_CacheLineIsValid

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The CacheLineValid feature is supported on the device
    - false  - The CacheLineValid feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_PCACHE_ExistsCacheLineValid( PCACHE_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_PCACHE_ExistsCacheLineAddr( PCACHE_MODULE_ID index )

  Summary:
    Identifies that the CacheLineAddr feature exists on the Prefetch Cache module.

  Description:
    This interface identifies that the CacheLineAddr feature is available on the 
    Prefetch Cache module. When this interface returns true, these functions are 
    supported on the device:
    - PLIB_PCACHE_CacheLineAddrSet
    - PLIB_PCACHE_CacheLineAddrGet

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The CacheLineAddr feature is supported on the device
    - false  - The CacheLineAddr feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_PCACHE_ExistsCacheLineAddr( PCACHE_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_PCACHE_ExistsCacheLineFlashType( PCACHE_MODULE_ID index )

  Summary:
    Identifies that the CacheLineFlashType feature exists on the Prefetch Cache 
    module.

  Description:
    This interface identifies that the CacheLineFlashType feature is available on 
    the Prefetch Cache module. When this interface returns true, these functions 
    are supported on the device:
    - PLIB_PCACHE_CacheLineFlashTypeBoot
    - PLIB_PCACHE_CacheLineFlashTypeInst
    - PLIB_PCACHE_CacheLineFlashTypeIsInst

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The CacheLineFlashType feature is supported on the device
    - false  - The CacheLineFlashType feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_PCACHE_ExistsCacheLineFlashType( PCACHE_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_PCACHE_ExistsCacheLineMask( PCACHE_MODULE_ID index )

  Summary:
    Identifies that the CacheLineMask feature exists on the Prefetch Cache module.

  Description:
    This interface identifies that the CacheLineMask feature is available on the 
    Prefetch Cache module. When this interface returns true, these functions are 
    supported on the device:
    - PLIB_PCACHE_CacheLineMaskSet
    - PLIB_PCACHE_CacheLineMaskGet

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The CacheLineMask feature is supported on the device
    - false  - The CacheLineMask feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_PCACHE_ExistsCacheLineMask( PCACHE_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_PCACHE_ExistsWord( PCACHE_MODULE_ID index )

  Summary:
    Identifies that the Word feature exists on the Prefetch Cache module.

  Description:
    This interface identifies that the Word feature is available on the Prefetch 
    Cache module. When this interface returns true, these functions are supported 
    on the device:
    - PLIB_PCACHE_WordRead
    - PLIB_PCACHE_WordWrite

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The Word feature is supported on the device
    - false  - The Word feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_PCACHE_ExistsWord( PCACHE_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_PCACHE_ExistsLeastRecentlyUsedState( PCACHE_MODULE_ID index )

  Summary:
    Identifies that the LeastRecentlyUsedState feature exists on the Prefetch 
    Cache module.

  Description:
    This interface identifies that the LeastRecentlyUsedState feature is available 
    on the Prefetch Cache module. When this interface returns true, this function
    is supported on the device:
    - PLIB_PCACHE_LeastRecentlyUsedStateRead

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The LeastRecentlyUsedState feature is supported on the device
    - false  - The LeastRecentlyUsedState feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_PCACHE_ExistsLeastRecentlyUsedState( PCACHE_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_PCACHE_ExistsCacheHit( PCACHE_MODULE_ID index )

  Summary:
    Identifies that the CacheHit feature exists on the Prefetch Cache module.

  Description:
    This interface identifies that the CacheHit feature is available on the 
    Prefetch Cache module. When this interface returns true, these functions are 
    supported on the device:
    - PLIB_PCACHE_CacheHitRead
    - PLIB_PCACHE_CacheHitWrite

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The CacheHit feature is supported on the device
    - false  - The CacheHit feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_PCACHE_ExistsCacheHit( PCACHE_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_PCACHE_ExistsCacheMiss( PCACHE_MODULE_ID index )

  Summary:
    Identifies that the CacheMiss feature exists on the Prefetch Cache module.

  Description:
    This interface identifies that the CacheMiss feature is available on the 
    Prefetch Cache module. When this interface returns true, these functions are 
    supported on the device:
    - PLIB_PCACHE_CacheMissRead
    - PLIB_PCACHE_CacheMissWrite

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The CacheMiss feature is supported on the device
    - false  - The CacheMiss feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_PCACHE_ExistsCacheMiss( PCACHE_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_PCACHE_ExistsPrefetchAbort( PCACHE_MODULE_ID index )

  Summary:
    Identifies that the PrefetchAbort feature exists on the Prefetch Cache module.

  Description:
    This interface identifies that the PrefetchAbort feature is available on the 
    Prefetch Cache module. When this interface returns true, these functions are 
    supported on the device:
    - PLIB_PCACHE_PrefetchAbortRead
    - PLIB_PCACHE_PrefetchAbortWrite

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The PrefetchAbort feature is supported on the device
    - false  - The PrefetchAbort feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_PCACHE_ExistsPrefetchAbort( PCACHE_MODULE_ID index );

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif /*_PLIB_PCACHE_H*/

/******************************************************************************
 End of File
*/
