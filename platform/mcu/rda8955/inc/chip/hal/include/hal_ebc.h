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


#ifndef _HAL_EBC_H_
#define _HAL_EBC_H_

#include "cs_types.h"
#include "hal_sys.h"

/// @defgroup ebc HAL EBC Driver
///
/// This document describes the characteristics of the EBC Driver
/// and how to use it via its Hardware Abstraction Layer API.
///
/// The EBC (External Bus Controler) controls external chips connected on the
/// memory bus as well as the RAM and Flash for the system. This driver provides
/// a basic API to enable or disable Chips Select others than the ones used for
/// RAM and Flash.
///
/// All settings are configured at the opening of the given CS, as paramters
/// given to the #hal_EbcCsOpen function.
///
/// @par EBC operations
/// This driver provides an interface to configure the timing for accessing a
/// specific chip select.
/// - @b Chip @b Select @b Choice \n
///   The API defines the following ID to choose a chip select: (enum
///   #HAL_EBC_CS_T):\n
///     - #HAL_EBC_CS2
///     - #HAL_EBC_CS3
///     - #HAL_EBC_CS4
///     .
///     \n
///   Each corresponds to a hardware Chip Select, accessible through a memory
///   range :
///   <TABLE BORDER >
///   <TR>
///       <TH>Chip Select </TH>
///       <TH>Address Range</TH>
///       <TH>Note</TH>
///   </TR>
///   <TR>
///       <TD> CS2 </TD>
///       <TD> 0x00800000 to 0x008FFFFF </TD>
///       <TD> Always available, can be mulitplexed with something </TD>
///   </TR>
///   <TR>
///       <TD> CS3 </TD>
///       <TD> 0x00900000 to 0x009FFFFF </TD>
///       <TD> Always available, can be mulitplexed with something </TD>
///   </TR>
///   <TR>
///       <TD> CS4 </TD>
///       <TD> 0x00A00000 to 0x00A7FFFF </TD>
///       <TD> Always available, can be mulitplexed with something </TD>
///   </TR>
///   </TABLE>
///   \n
/// - @b Address @b Physical @b mapping \n
///   Physical address pins MXX to MXX represents the address bus. The pin MXX
///   to MXX represents the data bus.\n
///    @todo Update with pinout
///   As the data bus is 16bit wide, the address bus select halfwords. Address 0
///   refers to the first halfword, Address 1 to the second halfword and so on.
///   But the internal address is a byte address: address 0 refer to the first
///   byte, address 1 to the second byte and so on.\n
///   \n
///   So to address the second halfword from the software point of view, the
///   address 2 need to be used. For the third, the address is 4, and so on.
///   Moreover when accessing one Chip Select with words (32bits), the EBC will
///   automatically access the two consecutive (and word aligned) halfwords.
///   For instance, an access to the word at address 4 will address halfword at
///   address 4 (physical address 2) and the halfword at address 6 (physical
///   address 3).\n
///
/// @{
///


// ============================================================================
// TYPES
// ============================================================================

// ============================================================================
// HAL_MEM_SC_T
// ----------------------------------------------------------------------------
/// Id to choose the hardware general purpose semaphores.
// ============================================================================
typedef enum
{
    HAL_MEM_SC_AP_ISPI = 0,
    HAL_MEM_SC_WCPU_TRACE = 1,
    HAL_MEM_SC_AP_VOC = 2,

    // Add Others above ...
    // The max ID is 31

    HAL_MEM_SC_QTY = 32,
} HAL_MEM_SC_T;


// ============================================================================
// HAL_EBC_CS_T
// ----------------------------------------------------------------------------
/// Id to choose the Chip Selects available for peripherals
/// on the EBC.
// ============================================================================
typedef enum
{
    HAL_EBC_FLASH=0,
    HAL_EBC_SRAM,

    /// 0x00800000 to 0x008FFFFF
    HAL_EBC_CS2,

    /// 0x00900000 to 0x009FFFFF
    HAL_EBC_CS3,

    /// 0x00A00000 to 0x00A7FFFF
    HAL_EBC_CS4,

    HAL_EBC_CS_QTY
} HAL_EBC_CS_T;


// ============================================================================
// HAL_EBC_TIME_CFG_T
// ----------------------------------------------------------------------------
/// Timing configuration for a Chip Select
// ============================================================================
typedef struct
{
    /// Read access Latency Half cycle Mode:
    /// When 1 reduces the cycle latency by half a cycle
    /// between 2 consecutive half word access.
    UINT32 rdLatHalfMode:1;

    /// Read access Latency: Access time for read.
    /// The time is this value+1
    UINT32 rdLat:5;

    UINT32 :2;

    /// Chip select to Output enable Setup half mode:
    /// 1 add one half cycle to the time
    UINT32 oedlyHalfMode:1;

    /// Chip select to Output enable Setup time.
    UINT32 oedly:3;

    UINT32 :1;

    /// Page mode Latency value: Page mode access time (for read or write page
    /// mode). Access time in clock cycle = this value + 1.
    /// Note: if Half cycle mode is enabled it also applies to the page mode.
    UINT32 pageLat:3;

    /// Write access Latency Half cycle Mode:
    /// When 1,  reduce the cycle latency by half a cycle between 2 consecutive
    /// half word access.
    UINT32 wrLatHalfMode:1;

    /// Write access Latency: Access time for write.
    /// Access time in clock cycle is the value of this field +1.
    UINT32 wrLat:5;

    UINT32 :2;

    /// Chip select to Write enable Setup half mode:
    /// 1 add one half cycle to the time
    UINT32 wedlyHalfMode:1;

    /// Chip select to Write enable Setup time:
    /// The time in clock cycle is the value of this field.
    UINT32 wedly:3;

    /// Relaxation delay between sequence of access (all read or all write) to
    /// the same chip select and any new sequence or idle state.
    /// The delay in number of clock cycle is the value of this field.
    UINT32 relax:3;

    /// Force Relaxation cycles to be inserted between any access.
    /// (same page or same burst consecutive access are considered a
    /// single access)
    UINT32 forceRelax:1;
} HAL_EBC_TIME_CFG_T;


// ============================================================================
// HAL_EBC_MODE_CFG_T
// ----------------------------------------------------------------------------
/// Mode configuration for chip select
// ============================================================================
typedef struct
{
    /// Enable ...
    UINT32 csEn:1;

    /// PChip Select polarity: 1 active High, '0' active Low.
    /// This bit is only valid by CS5 and CS6, for others it is always active
    /// low (0).
    UINT32 polarity:1;

    /// 1 use Address/Data Mux for 16 lower address bits.
    UINT32 admuxMode:1;

    UINT32 :1;

    /// Write allowed ?
    UINT32 writeAllow:1;

    /// 1 to use BE as WE for each byte (to use two 8bits chips)
    UINT32 wbeMode:1;

    /// 0: BE active after OEDly/WEDly
    /// 1: BE active during full access
    UINT32 bedlyMode:1;

    UINT32 :1;

    /// 1 to use the wait input to delay end of a read access.
    /// (RdLat min 4)
    UINT32 waitMode:1;

    /// 1 to use the wait input to delay end of a write access.
    /// (WrLAt min 4)
    UINT32 writeWaitMode:1;

    UINT32 :2;

    /// Page Size, value:page size
    /// 0: 4 half words
    /// 1: 8 half words
    /// 2: 16 half words
    /// 3: 32 half words
    /// 4: 64 half words
    /// 5: 128 half words
    /// 6: 256 half words
    /// 7: Full page
    UINT32 pageSize:3;

    UINT32 :1;

    // Page mode :1 enable page mode for read access:
    /// only address changes, chip select keeps enabled while address
    /// keeps in a page (defined by PageSize).
    UINT32 pageMode:1;

    /// 1 enable page mode for write access.
    UINT32 writePageMode:1;

    UINT32 :2;

    /// 1 enable burst mode for read access.
    /// Use PageSize. Require the use of the M_ADV and M_CLK pins.
    UINT32 burstMode:1;

    /// 1 enable burst mode for write access.
    UINT32 writeBurstMode:1;

    UINT32 :2;

    /// 1 the M_ADV pin is low only during the first cycle of an asynchronous
    /// access (for asynchronous modes on burst capable chips)
    UINT32 advAsync:1;

    /// 1 the M_WE is valid only during the M_ADV pulse.
    UINT32 advwePulse:1;

    /// Address/Data Mux timing
    /// 0   normal  ADV signal as in burst mode
    /// 1   F1      ADV signal fall a half cycle after CS for 1 cycle
    /// 2   F1_5    ADV signal fall a half cycle after CS for 1.5 cycle
    /// 3   F2      ADV signal fall a half cycle after CS for 2 cycle
    UINT32 admuxDly:2;

    /// 1 the Write Burst mode PageSize is replaced by 1 to do synchronous
    /// single write access. (while PageSize is still used for
    /// Read burst access)
    UINT32 writeSingle:1;

    /// 1 the data remains driven during Relaxation time.
    UINT32 writeHold:1;

    UINT32:2;

} HAL_EBC_MODE_CFG_T;


// ============================================================================
// HAL_EBC_CS_CFG_T
// ----------------------------------------------------------------------------
/// Structure for CS configuration.
/// It describes both the timings and mode configuration
/// for a chip on a given Chip Select
// ============================================================================
typedef struct
{
    /// Timings configuration
    HAL_EBC_TIME_CFG_T time ;
    /// Mode configuration
    HAL_EBC_MODE_CFG_T mode ;
} HAL_EBC_CS_CFG_T;


// ============================================================================
// HAL_EBC_FLSH_PHYSADD_T
// ----------------------------------------------------------------------------
/// This is the type for the physical address for the flash.
/// A physical address is used here to make it simpler to program
/// the Remap register since the offsets are already generally calculated
/// in the calling functions.
// ============================================================================
typedef UINT32 HAL_EBC_FLSH_PHYSADD_T;


// ============================================================================
// HAL_EBC_BURSTCALLBACK_T
// ----------------------------------------------------------------------------
/// This is the type for the callback function used to configure BurstMode combo
/// @param csBase the chip select base
/// @param pCRE the CRE register accessor
// ============================================================================
typedef VOID HAL_EBC_BURSTCALLBACK_T(volatile REG16* csBase, REG32* pCRE);



// ============================================================================
// FUNCTIONS
// ============================================================================


#ifdef CHIP_HAS_AP
// ============================================================================
// hal_MemScTake
// ----------------------------------------------------------------------------
/// Try to take a hardware general purpose semaphore.
///
/// @param id The semaphore ID
/// @return 0 if failure; otherwise success
// ============================================================================
PUBLIC UINT32 hal_MemScTake(HAL_MEM_SC_T id);


// ============================================================================
// hal_MemScRelease
// ----------------------------------------------------------------------------
/// Release a hardware general purpose semaphore.
///
/// @param id The semaphore ID
// ============================================================================
PUBLIC VOID hal_MemScRelease(HAL_MEM_SC_T id);


// ============================================================================
// hal_MemScGetStatus
// ----------------------------------------------------------------------------
/// Get the status of all hardware general purpose semaphores.
///
/// @return status
// ============================================================================
PUBLIC UINT32 hal_MemScGetStatus(VOID);


// ============================================================================
// hal_MemScGetStatusById
// ----------------------------------------------------------------------------
/// Get the status of a hardware general purpose semaphore.
///
/// @param id The semaphore ID
/// @return status
// ============================================================================
PUBLIC UINT32 hal_MemScGetStatusById(HAL_MEM_SC_T id);


#else // !CHIP_HAS_AP
// ============================================================================
// hal_EbcFlashOpen
// ----------------------------------------------------------------------------
/// Enable the CS for FLASH. That chip select is named like this for its
/// common use, but can in fact be plugged to a RAM.
/// This function must be called before any other EBC driver function,
/// as the Flash Chip is the device imposing the frequency on the memory
/// bridge. This optimal frequency is set as a parameter and the corresponding
/// EBC settings are also specified.
/// The Chip Select used is not a parameter of the function as it is always
/// #HAL_EBC_FLASH (ie. CS0)
///
/// @param optimalFreq Optimal Frequency to use the chip plugged on the
/// #HAL_EBC_FLASH chip select.
/// @param csConfig Configuration for the chip select. The \c csEn enable bit
/// of the mode configuration must be set to 1 !
/// @param cbFunc callback function for enabling the burst mode on sram
/// or NULL if not needed. Note: cbFunc must not be in flash
/// @return Base address of the configured space
// ============================================================================
PUBLIC VOID* hal_EbcFlashOpen(HAL_SYS_FREQ_T optimalFreq, CONST HAL_EBC_CS_CFG_T* csConfig, HAL_EBC_BURSTCALLBACK_T* cbFunc);
#endif // !CHIP_HAS_AP


// ============================================================================
// hal_EbcSramOpen
// ----------------------------------------------------------------------------
/// Enable a CS for SRAM. The chip selects for
/// FLASH have been set before by calling #hal_EbcFlashOpen.
/// The enabling of a CS returns the base address of the configured space.
/// The settings are given at the opening of the peripheral on the given
/// chip select, and must support the external maximal clock as it has been
/// set by the configuring of the CS0 (Flash) chipo select.
///
/// @param csConfig Configuration for the chip select. The \c csEn enable bit
/// of the mode configuration must be set to 1 there if the chip select
/// of the opened peripheral have to be enabled !
/// @param cbFunc callback function for enabling the burst mode on sram
/// or NULL if not needed. Note: cbFunc must not be in sram
/// @return Base address of the configured space
// ============================================================================
PUBLIC VOID* hal_EbcSramOpen(CONST HAL_EBC_CS_CFG_T* csConfig, HAL_EBC_BURSTCALLBACK_T* cbFunc);


// ============================================================================
// hal_EbcCsOpen
// ----------------------------------------------------------------------------
/// Enable a CS other than the one for FLASH. The chip selects for
/// FLASH have been set before by calling #hal_EbcFlashOpen.
/// The enabling of a CS returns the base address of the configured space.
/// The settings are given at the opening of the peripheral on the given
/// chip select, and must support the external maximal clock as it has been
/// set by the configuring of the CS0 (Flash) chipo select.
///
/// @param cs Chip Select to Enable. (CS2 or above)
/// @param csConfig Configuration for the chip select. The \c csEn enable bit
/// of the mode configuration must be set to 1 there if the chip select
/// of the opened peripheral have to be enabled !
/// @return Base address of the configured space
// ============================================================================
PUBLIC VOID* hal_EbcCsOpen(HAL_EBC_CS_T cs, CONST HAL_EBC_CS_CFG_T* csConfig);


// ============================================================================
// hal_EbcCsClose
// ----------------------------------------------------------------------------
/// Disables a CS (except FLASH and SRAM).
/// The address space previously allocated for this chip select becomes
/// unavailable after a call to this function
///
/// @param cs The Chip Select to Disable
// ============================================================================
PUBLIC VOID hal_EbcCsClose(HAL_EBC_CS_T cs);


// ============================================================================
// hal_EbcFlashWriteEnable
// ----------------------------------------------------------------------------
/// Enables or disables writing for the flash memory accessible through the
/// Chip Select 0.
/// This function is normally reserved for the implementation of flash driver
///
/// @param enable If \c TRUE, writing on the CS0 is allowed \n
///               If \c FALSE, the CS0 is read-only
// ============================================================================
PUBLIC VOID hal_EbcFlashWriteEnable(BOOL enable);


// ============================================================================
// hal_EbcFlashWriteEnabled
// ----------------------------------------------------------------------------
/// Test if the flash write is enable
///
/// @return If \c TRUE, writing on the CS0 is allowed \n
///               If \c FALSE, the CS0 is read-only
// ============================================================================
PUBLIC BOOL hal_EbcFlashWriteEnabled(VOID);


// ============================================================================
// hal_EbcGetFrequency
// ----------------------------------------------------------------------------
/// Gets the EBC frequency. This frequency is *different* from the system clock,
/// but the possible values being the same, we use the same type.
///
/// @return The EBC frequency
// ============================================================================
PUBLIC HAL_SYS_MEM_FREQ_T hal_EbcGetFrequency(VOID);


// ============================================================================
// hal_EbcConfigRemap
// ----------------------------------------------------------------------------
/// Configures the flash remap bits.  When the flash is accessed above 12M,
/// the physical flash address is passed to this function to configure the
/// remap bits.
// ============================================================================
PUBLIC VOID hal_EbcConfigRemap(HAL_EBC_FLSH_PHYSADD_T physAddr);


// ============================================================================
// hal_EbcGetCsAddress
// ----------------------------------------------------------------------------
/// Return the address of a given Chip Select.
///
/// @param cs Chip Select
/// @return Base address of the configured space
// ============================================================================
PUBLIC VOID* hal_EbcGetCsAddress(HAL_EBC_CS_T cs);


// ============================================================================
// hal_EbcCsSetup
// ----------------------------------------------------------------------------
/// Configure the CS settings according to the system clock frequency
/// This function configures the CS time and mode settings.
///
///     @param sys_freq System frequency to which adjust
///     the CS settings
// ============================================================================
PROTECTED VOID hal_EbcCsSetup(HAL_SYS_MEM_FREQ_T sys_freq);


///  @} <- End of the EBC group


#endif // _HAL_EBC_H_
