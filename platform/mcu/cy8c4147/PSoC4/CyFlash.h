/***************************************************************************//**
* \file CyFlash.h
* \version 5.70
*
* \brief Provides the function definitions for the FLASH.
*
* \note Documentation of the API's in this file is located in the System
* Reference Guide provided with PSoC Creator.
*
********************************************************************************
* \copyright
* Copyright 2010-2018, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_BOOT_CYFLASH_H)
#define CY_BOOT_CYFLASH_H

#include "CyLib.h"

/**
* \addtogroup group_flash

\brief Flash memory in PSoC devices provides nonvolatile storage for user
firmware, user configuration data, and bulk data storage. See the device
datasheet and TRM for more information on Flash architecture.

\section section_flash_protection Flash Protection
PSoC devices include a flexible flash-protection model that prevents access
and visibility to on-chip flash memory. The device offers the ability to
assign one of two protection levels to each row of flash: unprotected and
full protection. The required protection level can be selected using the Flash
Security tab of the PSoC Creator DWR file. Flash protection levels can only be
changed by performing a complete flash erase. The Flash programming APIs will
fail to write a row with Full Protection level. For more information on
protection model, refer to the Flash Security Editor section in the PSoC
Creator Help.

\section section_flash_working_with Working with Flash
Flash programming operations are implemented as system calls. System calls are
executed out of SROM in the privileged mode of operation. Users have no access
to read or modify the SROM code. The CPU requests the system call by writing
the function opcode and parameters to the System Performance Controller (SPC)
input registers, and then requesting the SROM to execute the function. Based
on the function opcode, the SPC executes the corresponding system call from
SROM and updates the SPC status register. The CPU should read this status
register for the pass/fail result of the function execution. As part of
function execution, the code in SROM interacts with the SPC interface to do
the actual flash programming operations.

It can take as many as 20 milliseconds to write to flash. During this time,
the device should not be reset, or unexpected changes may be made to portions
of the flash. Reset sources include XRES pin, software reset, and watchdog.
Make sure that these are not inadvertently activated. Also, the low voltage
detect circuits should be configured to generate an interrupt instead of a
reset.

The flash can be read either by the cache controller or the SPC. Flash write
can be performed only by the SPC. Both the SPC and cache cannot simultaneously
access flash memory. If the cache controller tries to access flash at the same
time as the SPC, then it must wait until the SPC completes its flash access
operation. The CPU, which accesses the flash memory through the cache
controller, is therefore also stalled in this circumstance. If a CPU code
fetch has to be done from flash memory due to a cache miss condition, then the
cache would have to wait until the SPC completes the flash write operation.
Thus the CPU code execution will also be halted till the flash write is
complete. Flash is directly mapped into memory space and can be read directly.

\note Flash write operations on PSoC 4000 devices modify the clock settings of
the device during the period of the write operation.
Refer to the \ref CySysFlashWriteRow() API documentation for details.

* @{
*/

uint32 CySysFlashWriteRow       (uint32 rowNum, const uint8 rowData[]);
#if (CY_SFLASH_XTRA_ROWS)
    uint32 CySysSFlashWriteUserRow  (uint32 rowNum, const uint8 rowData[]);
#endif /* (CY_SFLASH_XTRA_ROWS) */
void   CySysFlashSetWaitCycles  (uint32 freq);

#if (CY_IP_FLASH_PARALLEL_PGM_EN && (CY_IP_FLASH_MACROS > 1u))
    uint32 CySysFlashStartWriteRow(uint32 rowNum, const uint8 rowData[]);
    uint32 CySysFlashGetWriteRowStatus(void);
    uint32 CySysFlashResumeWriteRow(void);
#endif /* (CY_IP_FLASH_PARALLEL_PGM_EN && (CY_IP_FLASH_MACROS > 1u)) */

/** @} group_flash */


#define CY_FLASH_BASE           (CYDEV_FLASH_BASE)              /**< The base pointer of the Flash memory.*/
#define CY_FLASH_SIZE           (CYDEV_FLASH_SIZE)              /**< The size of the Flash memory. */
#define CY_FLASH_NUMBER_ARRAYS  (CYDEV_FLASH_SIZE / CYDEV_FLS_SECTOR_SIZE)  /**< The size of Flash array. */
#define CY_FLASH_SIZEOF_ARRAY   (CYDEV_FLS_SECTOR_SIZE)                     /**< The size of the Flash row. */
#define CY_FLASH_NUMBER_ROWS    (CYDEV_FLASH_SIZE / CYDEV_FLS_ROW_SIZE)     /**< The number of Flash row. */
#define CY_FLASH_SIZEOF_ROW     (CYDEV_FLS_ROW_SIZE)                        /**< The number of Flash arrays. */

#if (CY_SFLASH_XTRA_ROWS)
    #define CY_SFLASH_USERBASE  (CYREG_SFLASH_MACRO_0_FREE_SFLASH0) /**< The base pointer of the user SFlash memory. */
    #define CY_SFLASH_SIZE      (CYDEV_SFLASH_SIZE)                 /**< The size of the SFlash memory. */
    #define CY_SFLASH_SIZEOF_USERROW        (CYDEV_FLS_ROW_SIZE)    /**< The size of the SFlash row. */
    #define CY_SFLASH_NUMBER_USERROWS       (4u)                    /**< The number of SFlash row. */
#endif /* (CY_SFLASH_XTRA_ROWS) */


/**
* \addtogroup group_flash_status_codes Flash API status codes
* \ingroup group_flash
* @{
*/

/** Completed successfully. */
#define CY_SYS_FLASH_SUCCESS            (0x00u)
/** Specified flash row address is invalid. The row id or byte address provided is outside of the available memory. */
#define CY_SYS_FLASH_INVALID_ADDR       (0x04u)
/** Specified flash row is protected. */
#define CY_SYS_FLASH_PROTECTED          (0x05u)
/** Resume Completed. All non-blocking calls have completed. The resume/abort function cannot be called until the
next non-blocking. */
#define CY_SYS_FLASH_RESUME_COMPLETED   (0x07u)
/** \brief Pending Resume. A non-blocking was initiated and must be completed by calling the resume API, before any other
function may be called. */
#define CY_SYS_FLASH_PENDING_RESUME     (0x08u)
/** System Call Still In Progress. A resume or non-blocking is still in progress. The SPC ISR must fire before
attempting the next resume. */
#define CY_SYS_FLASH_CALL_IN_PROGRESS   (0x09u)
/** Invalid Flash Clock. Products using CY_IP_SRSSLT must set the IMO to 48MHz and the HF clock source to the IMO clock
before Write/Erase operations. */
#define CY_SYS_FLASH_INVALID_CLOCK      (0x12u)
/** @} group_flash_status_codes */

#define CY_SYS_SFLASH_SUCCESS            (CY_SYS_FLASH_SUCCESS)
#define CY_SYS_SFLASH_INVALID_ADDR       (CY_SYS_FLASH_INVALID_ADDR)
#define CY_SYS_SFLASH_PROTECTED          (CY_SYS_FLASH_PROTECTED)

/* CySysFlashSetWaitCycles() - implementation definitions */
#define CY_FLASH_WAIT_STATE_EN          (( uint32 )(( uint32 )0x01u << 18u))
#define CY_FLASH_SYSCLK_BOUNDARY_MHZ    (24u)
#if (CY_IP_CPUSS_FLASHC_PRESENT)
    /* CySysFlashSetWaitCycles() */
    #if(CY_IP_FM || CY_IP_FS)
        #define CY_FLASH_CTL_WS_0_FREQ_MIN      (0u)
        #define CY_FLASH_CTL_WS_0_FREQ_MAX      (24u)

        #define CY_FLASH_CTL_WS_1_FREQ_MIN      (24u)
        #define CY_FLASH_CTL_WS_1_FREQ_MAX      (48u)
    #else /* (CY_IP_FMLT || CY_IP_FSLT) */
        #define CY_FLASH_CTL_WS_0_FREQ_MIN      (0u)
        #define CY_FLASH_CTL_WS_0_FREQ_MAX      (16u)

        #define CY_FLASH_CTL_WS_1_FREQ_MIN      (16u)
        #define CY_FLASH_CTL_WS_1_FREQ_MAX      (32u)

        #define CY_FLASH_CTL_WS_2_FREQ_MIN      (32u)
        #define CY_FLASH_CTL_WS_2_FREQ_MAX      (48u)
    #endif  /* (CY_IP_FM || CY_IP_FS) */

    #define CY_FLASH_CTL_WS_MASK        ((uint32) 0x03u)
    #define CY_FLASH_CTL_WS_0_VALUE     (0x00u)
    #define CY_FLASH_CTL_WS_1_VALUE     (0x01u)
    #if(CY_IP_FMLT || CY_IP_FSLT)
        #define CY_FLASH_CTL_WS_2_VALUE (0x02u)
    #endif  /* (CY_IP_FMLT || CY_IP_FSLT) */
#endif  /* (CY_IP_CPUSS_FLASHC_PRESENT) */


#define CY_FLASH_KEY_ONE                (0xB6u)
#define CY_FLASH_KEY_TWO(x)             ((uint32) (((uint16) 0xD3u) + ((uint16) (x))))

#define CY_FLASH_PAGE_LATCH_START_ADDR  ((uint32) (0x00u))
#define CY_FLASH_ROW_NUM_MASK           (0x100u)
#define CY_FLASH_CPUSS_REQ_START        (( uint32 )(( uint32 )0x1u << 31u))

/* Opcodes */
#define CY_FLASH_API_OPCODE_LOAD                        (0x04u)
#define CY_FLASH_API_OPCODE_WRITE_ROW                   (0x05u)
#define CY_FLASH_API_OPCODE_NON_BLOCKING_WRITE_ROW      (0x07u)
#define CY_FLASH_API_OPCODE_RESUME_NON_BLOCKING         (0x09u)

#define CY_FLASH_API_OPCODE_PROGRAM_ROW                 (0x06u)
#define CY_FLASH_API_OPCODE_WRITE_SFLASH_ROW            (0x18u)

#define CY_FLASH_API_OPCODE_CLK_CONFIG                  (0x15u)
#define CY_FLASH_API_OPCODE_CLK_BACKUP                  (0x16u)
#define CY_FLASH_API_OPCODE_CLK_RESTORE                 (0x17u)

/* SROM API parameters offsets */
#define CY_FLASH_PARAM_KEY_TWO_OFFSET   (8u)
#define CY_FLASH_PARAM_ADDR_OFFSET      (16u)
#define CY_FLASH_PARAM_MACRO_SEL_OFFSET (24u)

#if (CY_IP_FLASH_MACROS == 2u)
    /*  Macro #0: rows 0x00-0x1ff, Macro #1: rows 0x200-0x3ff */
    #define CY_FLASH_GET_MACRO_FROM_ROW(row)         ((uint32)(((row) > 0x1ffu) ? 1u : 0u))
#else
    /* Only macro # 0 is available */
    #define CY_FLASH_GET_MACRO_FROM_ROW(row)         ((uint32)(((row) != 0u)    ? 0u : 0u))
#endif  /* (CY_IP_FLASH_MACROS == 2u) */

#if(CY_IP_FMLT)
    /* SROM size greater than 4k */
    #define CY_FLASH_IS_BACKUP_RESTORE   (CYDEV_SROM_SIZE > 0x00001000u)
#endif  /* (CY_IP_FMLT) */


#if(CY_IP_SRSSV2)
    #define CY_FLASH_CLOCK_BACKUP_SIZE      (4u)
#else /* CY_IP_SRSSLT */
    #define CY_FLASH_CLOCK_BACKUP_SIZE      (6u)
#endif  /* (CY_IP_SRSSV2) */


typedef struct cySysFlashClockBackupStruct
{
#if(CY_IP_FM)
        uint32   imoConfigReg;
#else   /* (CY_IP_FMLT) */
    #if (CY_PSOC4_4000)
        uint32 clkSelectReg;
        uint32 clkImoEna;
        uint32 clkImoFreq;
    #else

        #if(CY_IP_SRSSV2)
            uint32 clkImoPump;
        #endif /* (CY_IP_SRSSV2) */

        #if (CY_IP_SPCIF_SYNCHRONOUS)
            uint32 clockSettings[CY_FLASH_CLOCK_BACKUP_SIZE];       /* FM-Lite Clock Backup */
        #endif /* (CY_IP_SPCIF_SYNCHRONOUS) */

    #endif  /* (CY_PSOC4_4000) */

#endif  /* (CY_IP_FM) */
} CY_SYS_FLASH_CLOCK_BACKUP_STRUCT;


/* SYSARG control register */
#define CY_FLASH_CPUSS_SYSARG_REG       (*(reg32 *) CYREG_CPUSS_SYSARG)
#define CY_FLASH_CPUSS_SYSARG_PTR       ( (reg32 *) CYREG_CPUSS_SYSARG)

/* SYSCALL control register */
#define CY_FLASH_CPUSS_SYSREQ_REG       (*(reg32 *) CYREG_CPUSS_SYSREQ)
#define CY_FLASH_CPUSS_SYSREQ_PTR       ( (reg32 *) CYREG_CPUSS_SYSREQ)

#if (CY_IP_CPUSS_FLASHC_PRESENT)
    /* SYSARG control register */
    #define CY_FLASH_CTL_REG            (*(reg32 *) CYREG_CPUSS_FLASH_CTL)
    #define CY_FLASH_CTL_PTR            ( (reg32 *) CYREG_CPUSS_FLASH_CTL)
#endif  /* (CY_IP_CPUSS_FLASHC_PRESENT) */


#define CY_FLASH_API_RETURN             (((CY_FLASH_CPUSS_SYSARG_REG & 0xF0000000u) == 0xF0000000u) ? \
                                         (CY_FLASH_CPUSS_SYSARG_REG & 0x000000FFu) : \
                                         (((CY_FLASH_CPUSS_SYSARG_REG & 0xF0000000u) == 0xA0000000u) ? \
                                            CYRET_SUCCESS : (CY_FLASH_CPUSS_SYSARG_REG & 0x000000FFu)))


/*******************************************************************************
* Thne following code is OBSOLETE and must not be used starting with cy_boot
* 4.20.
*
* If the obsoleted macro definitions intended for use in the application use the
* following scheme, redefine your own versions of these definitions:
*    #ifdef <OBSOLETED_DEFINE>
*        #undef  <OBSOLETED_DEFINE>
*        #define <OBSOLETED_DEFINE>      (<New Value>)
*    #endif
*
* Note: Redefine obsoleted macro definitions with caution. They might still be
*       used in the application and their modification might lead to unexpected
*       consequences.
*******************************************************************************/
#define CY_FLASH_SRAM_ROM_KEY1          (( uint32 )0x00u)
#define CY_FLASH_SRAM_ROM_PARAM2        (CY_FLASH_SRAM_ROM_KEY1 + 0x04u)
#define CY_FLASH_SRAM_ROM_DATA          (CY_FLASH_SRAM_ROM_KEY1 + 0x08u)

#define CY_FLASH_SROM_CMD_RETURN_MASK   (0xF0000000u)
#define CY_FLASH_SROM_CMD_RETURN_SUCC   (0xA0000000u)
#define CY_FLASH_SROM_KEY1              (( uint32 )0xB6u)
#define CY_FLASH_SROM_KEY2_LOAD         (( uint32 )0xD7u)
#define CY_FLASH_SROM_KEY2_WRITE        (( uint32 )0xD8u)
#define CY_FLASH_SROM_LOAD_CMD          ((CY_FLASH_SROM_KEY2_LOAD << 8u) | CY_FLASH_SROM_KEY1)
#define CY_FLASH_LOAD_BYTE_OPCODE       (( uint32 )0x04u)
#define CY_FLASH_WRITE_ROW_OPCODE       (( uint32 )0x05u)


#endif  /* (CY_BOOT_CYFLASH_H) */


/* [] END OF FILE */
