/***************************************************************************//**
* \file CyFlash.c
* \version 5.70
*
* \brief Provides an API for the FLASH.
*
* \note This code is endian agnostic.
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

#include "CyFlash.h"


/*******************************************************************************
* Cypress identified a defect with the Flash write functionality of the
* PSoC 4000, PSoC 4000U, PSoC 4100, and PSoC 4200 devices. The 
* CySysFlashWriteRow() function now checks the data to be written and, if 
* necessary, modifies it to have a non-zero checksum. After writing to Flash, 
* the modified data is replaced (Flash program) with 
* the correct (original) data.
*******************************************************************************/
#define CY_FLASH_CHECKSUM_WORKAROUND    (CY_PSOC4_4000 || CY_PSOC4_4100 || CY_PSOC4_4200 || CY_PSOC4_4000U)

#if (CY_IP_FM || ((!CY_PSOC4_4000) && CY_IP_SPCIF_SYNCHRONOUS) || (!CY_IP_FM) && CY_PSOC4_4000)
    static CY_SYS_FLASH_CLOCK_BACKUP_STRUCT cySysFlashBackup;
#endif /* (CY_IP_FM || ((!CY_PSOC4_4000) && CY_IP_SPCIF_SYNCHRONOUS) || (!CY_IP_FM) && CY_PSOC4_4000) */

static cystatus CySysFlashClockBackup(void);
static cystatus CySysFlashClockRestore(void);
#if(CY_IP_SPCIF_SYNCHRONOUS)
    static cystatus CySysFlashClockConfig(void);
#endif  /* (CY_IP_SPCIF_SYNCHRONOUS) */


/*******************************************************************************
* Function Name: CySysFlashWriteRow
****************************************************************************//**
*
* Erases a row of Flash and programs it with the new data.
*
* The IMO must be enabled before calling this function. The operation of the
* flash writing hardware is dependent on the IMO.
*
* For PSoC 4000, PSoC 4100 BLE and PSoC 4200 BLE devices (PSoC 4100 BLE and
* PSoC 4200 BLE devices with 256K of Flash memory are not affected), this API
* will automatically modify the clock settings for the device. Writing to
* flash requires that changes be made to the IMO and HFCLK settings. The
* configuration is restored before returning. This will impact the operation
* of most of the hardware in the device.
*
* For PSoC 4000 devices this API will automatically modify the clock settings
* for the device. Writing to flash requires that changes be made to the IMO
* and HFCLK settings. The configuration is restored before returning. HFCLK
* will have several frequency changes during the operation of this API between
* a minimum frequency of the current IMO frequency divided by 8 and a maximum
* frequency of 12 MHz. This will impact the operation of most of the hardware
* in the device.
*
* \param rowNum The flash row number. The number of the flash rows is defined by
* the \ref CY_FLASH_NUMBER_ROWS macro for the selected device. Refer to the
* device datasheet for the details.
* \note The target flash array is calculated based on the specified flash row.
*
* \param rowData Array of bytes to write. The size of the array must be equal to
* the flash row size. The flash row size for the selected device is defined by
* the \ref CY_FLASH_SIZEOF_ROW macro. Refer to the device datasheet for the
* details.
*
* \return \ref group_flash_status_codes
*
*******************************************************************************/
uint32 CySysFlashWriteRow(uint32 rowNum, const uint8 rowData[])
{
    volatile uint32 retValue = CY_SYS_FLASH_SUCCESS;
    volatile uint32 clkCnfRetValue = CY_SYS_FLASH_SUCCESS;
    volatile uint32 parameters[(CY_FLASH_SIZEOF_ROW + CY_FLASH_SRAM_ROM_DATA) / sizeof(uint32)];
    uint8  interruptState;

    #if (CY_FLASH_CHECKSUM_WORKAROUND)
        uint32 needChecksumWorkaround = 0u;
        uint32 savedIndex = 0u;
        uint32 savedValue = 0u;
        uint32 checksum = 0u;
        uint32 bits = 0u;
        uint32 i;
    #endif  /* (CY_FLASH_CHECKSUM_WORKAROUND) */

    if ((rowNum < CY_FLASH_NUMBER_ROWS) && (rowData != 0u))
    {
        /* Copy data to be written into internal variable */
        (void)memcpy((void *)&parameters[2u], rowData, CY_FLASH_SIZEOF_ROW);

        #if (CY_FLASH_CHECKSUM_WORKAROUND)

            for (i = 2u; i < ((CY_FLASH_SIZEOF_ROW / sizeof(uint32)) + 2u); i++)
            {
                uint32 tmp = parameters[i];
                if (tmp != 0u)
                {
                    checksum += tmp;
                    bits |= tmp;
                    savedIndex = i;
                }
            }

            needChecksumWorkaround = ((checksum == 0u) && (bits != 0u)) ? 1u : 0u;
            if (needChecksumWorkaround != 0u)
            {
                savedValue = parameters[savedIndex];
                parameters[savedIndex] = 0u;
            }
        #endif  /* (CY_FLASH_CHECKSUM_WORKAROUND) */

        /* Load Flash Bytes */
        parameters[0u] = (uint32) (CY_FLASH_GET_MACRO_FROM_ROW(rowNum)        << CY_FLASH_PARAM_MACRO_SEL_OFFSET) |
                         (uint32) (CY_FLASH_PAGE_LATCH_START_ADDR             << CY_FLASH_PARAM_ADDR_OFFSET     ) |
                         (uint32) (CY_FLASH_KEY_TWO(CY_FLASH_API_OPCODE_LOAD) << CY_FLASH_PARAM_KEY_TWO_OFFSET  ) |
                         CY_FLASH_KEY_ONE;
        parameters[1u] = CY_FLASH_SIZEOF_ROW - 1u;

        CY_FLASH_CPUSS_SYSARG_REG = (uint32) &parameters[0u];
        CY_FLASH_CPUSS_SYSREQ_REG = CY_FLASH_CPUSS_REQ_START | CY_FLASH_API_OPCODE_LOAD;
        CY_NOP;
        retValue = CY_FLASH_API_RETURN;

        if(retValue == CY_SYS_FLASH_SUCCESS)
        {
            /***************************************************************
            * Mask all the exceptions to guarantee that Flash write will
            * occur in the atomic way. It will not affect system call
            * execution (flash row write) since it is executed in the NMI
            * context.
            ***************************************************************/
            interruptState = CyEnterCriticalSection();

            clkCnfRetValue = CySysFlashClockBackup();

        #if(CY_IP_SPCIF_SYNCHRONOUS)
            if(clkCnfRetValue == CY_SYS_FLASH_SUCCESS)
            {
                retValue = CySysFlashClockConfig();
            }
        #endif  /* (CY_IP_SPCIF_SYNCHRONOUS) */

            if(retValue == CY_SYS_FLASH_SUCCESS)
            {
                /* Write Row */
                parameters[0u]  = (uint32) (((uint32) CY_FLASH_KEY_TWO(CY_FLASH_API_OPCODE_WRITE_ROW) <<  CY_FLASH_PARAM_KEY_TWO_OFFSET) | CY_FLASH_KEY_ONE);
                parameters[0u] |= (uint32)(rowNum << 16u);

                CY_FLASH_CPUSS_SYSARG_REG = (uint32) &parameters[0u];
                CY_FLASH_CPUSS_SYSREQ_REG = CY_FLASH_CPUSS_REQ_START | CY_FLASH_API_OPCODE_WRITE_ROW;
                CY_NOP;
                retValue = CY_FLASH_API_RETURN;
            }

            #if (CY_FLASH_CHECKSUM_WORKAROUND)

                if ((retValue == CYRET_SUCCESS) && (needChecksumWorkaround != 0u))
                {
                    (void)memset((void *)&parameters[2u], 0, CY_FLASH_SIZEOF_ROW);
                    parameters[savedIndex] = savedValue;

                    /* Load Flash Bytes */
                    parameters[0u] = (uint32) (CY_FLASH_GET_MACRO_FROM_ROW(rowNum)        << CY_FLASH_PARAM_MACRO_SEL_OFFSET) |
                                     (uint32) (CY_FLASH_PAGE_LATCH_START_ADDR             << CY_FLASH_PARAM_ADDR_OFFSET     ) |
                                     (uint32) (CY_FLASH_KEY_TWO(CY_FLASH_API_OPCODE_LOAD) << CY_FLASH_PARAM_KEY_TWO_OFFSET  ) |
                                     CY_FLASH_KEY_ONE;
                    parameters[1u] = CY_FLASH_SIZEOF_ROW - 1u;

                    CY_FLASH_CPUSS_SYSARG_REG = (uint32) &parameters[0u];
                    CY_FLASH_CPUSS_SYSREQ_REG = CY_FLASH_CPUSS_REQ_START | CY_FLASH_API_OPCODE_LOAD;
                    CY_NOP;
                    retValue = CY_FLASH_API_RETURN;


                    if(retValue == CY_SYS_FLASH_SUCCESS)
                    {
                        /* Program Row */
                        parameters[0u]  =
                            (uint32) (((uint32) CY_FLASH_KEY_TWO(CY_FLASH_API_OPCODE_PROGRAM_ROW) <<
                                                    CY_FLASH_PARAM_KEY_TWO_OFFSET) | CY_FLASH_KEY_ONE);
                        parameters[0u] |= (uint32)(rowNum << 16u);

                        CY_FLASH_CPUSS_SYSARG_REG = (uint32) &parameters[0u];
                        CY_FLASH_CPUSS_SYSREQ_REG = CY_FLASH_CPUSS_REQ_START | CY_FLASH_API_OPCODE_PROGRAM_ROW;
                        CY_NOP;
                        retValue = CY_FLASH_API_RETURN;
                    }
                }
            #endif  /* (CY_FLASH_CHECKSUM_WORKAROUND) */

            if(clkCnfRetValue == CY_SYS_FLASH_SUCCESS)
            {
                clkCnfRetValue = CySysFlashClockRestore();

                if(clkCnfRetValue != CY_SYS_FLASH_SUCCESS)
                {
                    retValue = clkCnfRetValue;
                }
            }

            CyExitCriticalSection(interruptState);
        }
    }
    else
    {
        retValue = CY_SYS_FLASH_INVALID_ADDR;
    }

    return (retValue);
}



#if (CY_IP_FLASH_PARALLEL_PGM_EN && (CY_IP_FLASH_MACROS > 1u))
/*******************************************************************************
* Function Name: CySysFlashStartWriteRow
****************************************************************************//**
*
* Initiates a write to a row of Flash. A call to this API is non-blocking. 
* Use CySysFlashResumeWriteRow() to resume flash writes and 
* CySysFlashGetWriteRowStatus() to ascertain status of the write operation.
*
* The devices require HFCLK to be sourced by 48 MHz IMO during flash write.
* This API will modify IMO configuration; it can be later restored to original
* configuration by calling \ref CySysFlashGetWriteRowStatus().
*
* \note The non-blocking operation does not return success status 
* CY_SYS_FLASH_SUCCESS until the last \ref CySysFlashResumeWriteRow API 
* is complete. The CPUSS_SYSARG register will be reflecting the SRAM address 
* during an ongoing non-blocking operation.
*    
* \param rowNum The flash row number. The number of the flash rows is defined by
* the \ref CY_FLASH_NUMBER_ROWS macro for the selected device. Refer to the
* device datasheet for the details.
* \note The target flash array is calculated based on the specified flash row.
*
* \param rowData Array of bytes to write. The size of the array must be equal to
* the flash row size. The flash row size for the selected device is defined by
* the \ref CY_FLASH_SIZEOF_ROW macro. Refer to the device datasheet for the
* details.
*
* \return \ref group_flash_status_codes
*
*******************************************************************************/
uint32 CySysFlashStartWriteRow(uint32 rowNum, const uint8 rowData[])
{
    volatile uint32 retValue = CY_SYS_FLASH_SUCCESS;
    volatile uint32 parameters[(CY_FLASH_SIZEOF_ROW + CY_FLASH_SRAM_ROM_DATA) / sizeof(uint32)];
    uint8  interruptState;

#if(CY_IP_SPCIF_SYNCHRONOUS)
    volatile uint32 clkCnfRetValue = CY_SYS_FLASH_SUCCESS;
#endif  /* (CY_IP_SPCIF_SYNCHRONOUS) */    

    if ((rowNum < CY_FLASH_NUMBER_ROWS) && (rowData != 0u))
    {
        /* Copy data to be written into internal variable */
        (void)memcpy((void *)&parameters[2u], rowData, CY_FLASH_SIZEOF_ROW);

        /* Load Flash Bytes */
        parameters[0u] = (uint32) (CY_FLASH_GET_MACRO_FROM_ROW(rowNum)        << CY_FLASH_PARAM_MACRO_SEL_OFFSET) |
                         (uint32) (CY_FLASH_PAGE_LATCH_START_ADDR             << CY_FLASH_PARAM_ADDR_OFFSET     ) |
                         (uint32) (CY_FLASH_KEY_TWO(CY_FLASH_API_OPCODE_LOAD) << CY_FLASH_PARAM_KEY_TWO_OFFSET  ) |
                         CY_FLASH_KEY_ONE;
        parameters[1u] = CY_FLASH_SIZEOF_ROW - 1u;

        CY_FLASH_CPUSS_SYSARG_REG = (uint32) &parameters[0u];
        CY_FLASH_CPUSS_SYSREQ_REG = CY_FLASH_CPUSS_REQ_START | CY_FLASH_API_OPCODE_LOAD;
        CY_NOP;
        retValue = CY_FLASH_API_RETURN;

        if(retValue == CY_SYS_FLASH_SUCCESS)
        {
            /***************************************************************
            * Mask all the exceptions to guarantee that Flash write will
            * occur in the atomic way. It will not affect system call
            * execution (flash row write) since it is executed in the NMI
            * context.
            ***************************************************************/
            interruptState = CyEnterCriticalSection();

        #if(CY_IP_SPCIF_SYNCHRONOUS)
            clkCnfRetValue = CySysFlashClockBackup();
            
            if(clkCnfRetValue == CY_SYS_FLASH_SUCCESS)
            {
                retValue = CySysFlashClockConfig();
            }
        #else
            (void)CySysFlashClockBackup();
        #endif  /* (CY_IP_SPCIF_SYNCHRONOUS) */

            if(retValue == CY_SYS_FLASH_SUCCESS)
            {
                /* Non-blocking Write Row */
                parameters[0u]  = (uint32) (((uint32) CY_FLASH_KEY_TWO(CY_FLASH_API_OPCODE_NON_BLOCKING_WRITE_ROW) <<
                                                        CY_FLASH_PARAM_KEY_TWO_OFFSET) | CY_FLASH_KEY_ONE);
                parameters[0u] |= (uint32)(rowNum << 16u);

                CY_FLASH_CPUSS_SYSARG_REG = (uint32) &parameters[0u];
                CY_FLASH_CPUSS_SYSREQ_REG = CY_FLASH_CPUSS_REQ_START | CY_FLASH_API_OPCODE_NON_BLOCKING_WRITE_ROW;
                CY_NOP;
                retValue = CY_FLASH_API_RETURN;
            }

            CyExitCriticalSection(interruptState);
        }
    }
    else
    {
        retValue = CY_SYS_FLASH_INVALID_ADDR;
    }

    return (retValue);
}


/*******************************************************************************
* Function Name: CySysFlashGetWriteRowStatus
****************************************************************************//**
*
* Returns the current status of the flash write operation.
*
** \note The non-blocking operation does not return success status 
* CY_SYS_FLASH_SUCCESS until the last \ref CySysFlashResumeWriteRow API 
* is complete. The CPUSS_SYSARG register will be reflecting the SRAM address 
* during an ongoing non-blocking operation.
* Calling this API before starting a non-blocking write row operation 
* using the \ref CySysFlashStartWriteRow() API will cause improper operation.
*
* \return \ref group_flash_status_codes
*
*******************************************************************************/
uint32 CySysFlashGetWriteRowStatus(void)
{
    volatile uint32 retValue = CY_SYS_FLASH_SUCCESS;

    CY_NOP;
    retValue = CY_FLASH_API_RETURN;

    (void) CySysFlashClockRestore();

    return (retValue);
}


/*******************************************************************************
* Function Name: CySysFlashResumeWriteRow
****************************************************************************//**
*
* This API must be called, once the SPC interrupt is triggered to complete the
* non-blocking operation. It is advised not to prolong calling this API for
* more than 25 ms.
*
* The non-blocking write row API \ref CySysFlashStartWriteRow() requires that
* this API be called 3 times to complete the write. This can be done by
* configuring SPCIF interrupt and placing a call to this API.
*
* For CM0 based device, a non-blocking call to program a row of macro 0
* requires the user to set the CPUSS_CONFIG.VECS_IN_RAM bit so that the
* interrupt vector for the SPC is fetched from the SRAM rather than the FLASH.
*
* For CM0+ based device, if the user wants to keep the vector table in flash
* when performing non-blocking flash write then they need to make sure the
* vector table is placed in the flash macro which is not getting programmed by
* configuring the VTOR register.
*
* \note The non-blocking operation does not return success status 
* CY_SYS_FLASH_SUCCESS until the last Resume API is complete. 
* The CPUSS_SYSARG register will be reflecting the SRAM address during an 
* ongoing non-blocking operation.
*
* \return \ref group_flash_status_codes
*
*******************************************************************************/
uint32 CySysFlashResumeWriteRow(void)
{
    volatile uint32 retValue = CY_SYS_FLASH_SUCCESS;
    static volatile uint32 parameters[1u];

    /* Resume */
    parameters[0u] = (uint32) (((uint32) CY_FLASH_KEY_TWO(CY_FLASH_API_OPCODE_RESUME_NON_BLOCKING) <<
                                         CY_FLASH_PARAM_KEY_TWO_OFFSET) | CY_FLASH_KEY_ONE);
    CY_FLASH_CPUSS_SYSARG_REG = (uint32) &parameters[0u];
    CY_FLASH_CPUSS_SYSREQ_REG = CY_FLASH_CPUSS_REQ_START | CY_FLASH_API_OPCODE_RESUME_NON_BLOCKING;
    
    CY_NOP;
    retValue = CY_FLASH_API_RETURN;
    
    return (retValue);
}

#endif /* (CY_IP_FLASH_PARALLEL_PGM_EN && (CY_IP_FLASH_MACROS > 1u)) */


/*******************************************************************************
* Function Name: CySysFlashSetWaitCycles
****************************************************************************//**
*
* Sets the number of clock cycles the cache will wait before it samples data
* coming back from Flash. This function must be called before increasing the
* SYSCLK clock frequency. It can optionally be called after lowering SYSCLK
* clock frequency in order to improve the CPU performance.
*
* \param freq The System clock frequency in MHz.
*
* \note Invalid frequency will be ignored in Release mode and the CPU will be
* halted if project is compiled in Debug mode.
*
*******************************************************************************/
void CySysFlashSetWaitCycles(uint32 freq)
{
    uint8  interruptState;

    interruptState = CyEnterCriticalSection();

    #if (CY_IP_CPUSS)

        if ( freq <= CY_FLASH_SYSCLK_BOUNDARY_MHZ )
        {
            CY_SYS_CLK_SELECT_REG &= (uint32)(~CY_FLASH_WAIT_STATE_EN);
        }
        else
        {
            CY_SYS_CLK_SELECT_REG |= CY_FLASH_WAIT_STATE_EN;
        }
    #else
        #if (CY_IP_CPUSS_FLASHC_PRESENT)
            /* CY_IP_FM and CY_IP_FS */
            if (freq <= CY_FLASH_CTL_WS_0_FREQ_MAX)
            {
                CY_FLASH_CTL_REG = (CY_FLASH_CTL_REG & ~CY_FLASH_CTL_WS_MASK) | CY_FLASH_CTL_WS_0_VALUE;
            } else
            if (freq <= CY_FLASH_CTL_WS_1_FREQ_MAX)
            {
                CY_FLASH_CTL_REG = (CY_FLASH_CTL_REG & ~CY_FLASH_CTL_WS_MASK) | CY_FLASH_CTL_WS_1_VALUE;
            } else
            #if (CY_IP_FMLT || CY_IP_FSLT)
                if (freq <= CY_FLASH_CTL_WS_2_FREQ_MAX)
                {
                    CY_FLASH_CTL_REG = (CY_FLASH_CTL_REG & ~CY_FLASH_CTL_WS_MASK) | CY_FLASH_CTL_WS_2_VALUE;
                }
                else
            #endif  /* (CY_IP_FMLT || CY_IP_FSLT) */
        #endif /* (CY_IP_CPUSS_FLASHC_PRESENT) */
        {
            /* Halt CPU in debug mode if frequency is invalid */
            CYASSERT(0u != 0u);
        }

    #endif  /* (CY_IP_CPUSS) */

    CyExitCriticalSection(interruptState);
}


#if (CY_SFLASH_XTRA_ROWS)
/*******************************************************************************
* Function Name: CySysSFlashWriteUserRow
****************************************************************************//**
*
* Writes data to a row of SFlash user configurable area.
*
* This API is applicable for PSoC 4100 BLE, PSoC 4200 BLE, PSoC 4100M,
* PSoC 4200M, and PSoC 4200L family of devices.
*
* \param rowNum The flash row number. The flash row number. The number of the
* flash rows is defined by the CY_SFLASH_NUMBER_USERROWS macro for the selected
* device. Valid range is 0-3. Refer to the device TRM for details.
*
* \param rowData Array of bytes to write. The size of the array must be equal to
* the flash row size. The flash row size for the selected device is defined by
* the \ref CY_SFLASH_SIZEOF_USERROW macro. Refer to the device TRM for the
* details.
*
* \return \ref group_flash_status_codes
*
*******************************************************************************/
uint32 CySysSFlashWriteUserRow(uint32 rowNum, const uint8 rowData[])
{
    volatile uint32 retValue = CY_SYS_FLASH_SUCCESS;
    volatile uint32 clkCnfRetValue = CY_SYS_FLASH_SUCCESS;
    volatile uint32 parameters[(CY_FLASH_SIZEOF_ROW + CY_FLASH_SRAM_ROM_DATA)/4u];
    uint8  interruptState;


    if ((rowNum < CY_SFLASH_NUMBER_USERROWS) && (rowData != 0u))
    {
        /* Load Flash Bytes */
        parameters[0u] = (uint32) (CY_FLASH_GET_MACRO_FROM_ROW(rowNum)        << CY_FLASH_PARAM_MACRO_SEL_OFFSET) |
                         (uint32) (CY_FLASH_PAGE_LATCH_START_ADDR             << CY_FLASH_PARAM_ADDR_OFFSET     ) |
                         (uint32) (CY_FLASH_KEY_TWO(CY_FLASH_API_OPCODE_LOAD) << CY_FLASH_PARAM_KEY_TWO_OFFSET  ) |
                         CY_FLASH_KEY_ONE;
        parameters[1u] = CY_FLASH_SIZEOF_ROW - 1u;

        (void)memcpy((void *)&parameters[2u], rowData, CY_FLASH_SIZEOF_ROW);
        CY_FLASH_CPUSS_SYSARG_REG = (uint32) &parameters[0u];
        CY_FLASH_CPUSS_SYSREQ_REG = CY_FLASH_CPUSS_REQ_START | CY_FLASH_API_OPCODE_LOAD;
        CY_NOP;
        retValue = CY_FLASH_API_RETURN;

        if(retValue == CY_SYS_FLASH_SUCCESS)
        {
            /***************************************************************
            * Mask all the exceptions to guarantee that Flash write will
            * occur in the atomic way. It will not affect system call
            * execution (flash row write) since it is executed in the NMI
            * context.
            ***************************************************************/
            interruptState = CyEnterCriticalSection();

            clkCnfRetValue = CySysFlashClockBackup();

        #if(CY_IP_SPCIF_SYNCHRONOUS)
            if(clkCnfRetValue == CY_SYS_FLASH_SUCCESS)
            {
                retValue = CySysFlashClockConfig();
            }
        #endif  /* (CY_IP_SPCIF_SYNCHRONOUS) */

            if(retValue == CY_SYS_FLASH_SUCCESS)
            {
                /* Write User Sflash Row */
                parameters[0u]  = (uint32) (((uint32) CY_FLASH_KEY_TWO(CY_FLASH_API_OPCODE_WRITE_SFLASH_ROW) <<  CY_FLASH_PARAM_KEY_TWO_OFFSET) | CY_FLASH_KEY_ONE);
                parameters[1u] = (uint32) rowNum;

                CY_FLASH_CPUSS_SYSARG_REG = (uint32) &parameters[0u];
                CY_FLASH_CPUSS_SYSREQ_REG = CY_FLASH_CPUSS_REQ_START | CY_FLASH_API_OPCODE_WRITE_SFLASH_ROW;
                CY_NOP;
                retValue = CY_FLASH_API_RETURN;
            }

            if(clkCnfRetValue == CY_SYS_FLASH_SUCCESS)
            {
                clkCnfRetValue = CySysFlashClockRestore();
            }
            CyExitCriticalSection(interruptState);
        }
    }
    else
    {
        retValue = CY_SYS_FLASH_INVALID_ADDR;
    }

    return (retValue);
}
#endif /* (CY_SFLASH_XTRA_ROWS) */


/*******************************************************************************
* Function Name: CySysFlashClockBackup
****************************************************************************//**
*
* Backups the device clock configuration.
*
* \return The same as \ref CySysFlashWriteRow().
*
*******************************************************************************/
static cystatus CySysFlashClockBackup(void)
{
    cystatus retValue = CY_SYS_FLASH_SUCCESS;
#if(!CY_IP_FM)
    #if !(CY_PSOC4_4000)
        #if (CY_IP_SPCIF_SYNCHRONOUS)
            volatile uint32   parameters[2u];
        #endif /* (CY_IP_SPCIF_SYNCHRONOUS) */
    #endif  /* !(CY_PSOC4_4000) */
#endif  /* (!CY_IP_FM) */

#if(CY_IP_FM)

    /***************************************************************
    * Preserve IMO configuration that could be changed during
    * system call execution (Cypress ID #150448).
    ***************************************************************/
    cySysFlashBackup.imoConfigReg = CY_SYS_CLK_IMO_CONFIG_REG;

#else   /* (CY_IP_FMLT) */

    #if (CY_PSOC4_4000)

        /***************************************************************************
        * Perform firmware clock settings backup for the PSOC4 4000 devices (the
        * corresponding system call is not available).
        ***************************************************************************/

        /***************************************************************************
        * The registers listed below are modified by CySysFlashClockConfig().
        *
        * The registers to be saved:
        * - CY_SYS_CLK_IMO_CONFIG_REG - IMO enable state.
        * - CY_SYS_CLK_SELECT_REG     - HFCLK source, divider, pump source. Save
        *                               entire register as it can be directly
        *                               written on restore (no special
        *                               requirements).
        * - CY_SYS_CLK_IMO_SELECT_REG - Save IMO frequency.
        *
        * The registers not to be saved:
        * - CY_SYS_CLK_IMO_TRIM1_REG  - No need to save. Function of frequency.
        *                               Restored by CySysClkWriteImoFreq().
        * - CY_SYS_CLK_IMO_TRIM3_REG  - No need to save. Function of frequency.
        *                               Restored by CySysClkWriteImoFreq().
        * - REG_CPUSS_FLASH_CTL       - Flash wait cycles. Unmodified due to system
        *                               clock 16 MHz limit.
        ***************************************************************************/

        cySysFlashBackup.clkSelectReg = CY_SYS_CLK_SELECT_REG;
        cySysFlashBackup.clkImoEna    = CY_SYS_CLK_IMO_CONFIG_REG & CY_SYS_CLK_IMO_CONFIG_ENABLE;
        cySysFlashBackup.clkImoFreq   = CY_SYS_CLK_IMO_MIN_FREQ_MHZ + (CY_SYS_CLK_IMO_SELECT_REG << 2u);

    #else

        #if (CY_IP_SPCIF_SYNCHRONOUS)
            /* FM-Lite Clock Backup System Call */
            parameters[0u] =
                (uint32) ((CY_FLASH_KEY_TWO(CY_FLASH_API_OPCODE_CLK_BACKUP) <<  CY_FLASH_PARAM_KEY_TWO_OFFSET) |
                            CY_FLASH_KEY_ONE);
            parameters[1u] = (uint32) &cySysFlashBackup.clockSettings[0u];
            CY_FLASH_CPUSS_SYSARG_REG = (uint32) &parameters[0u];
            CY_FLASH_CPUSS_SYSREQ_REG = CY_FLASH_CPUSS_REQ_START | CY_FLASH_API_OPCODE_CLK_BACKUP;
            CY_NOP;
            retValue = CY_FLASH_API_RETURN;
        #endif /* (CY_IP_SPCIF_SYNCHRONOUS) */

    #endif  /* (CY_PSOC4_4000) */

#endif  /* (CY_IP_FM) */

    return (retValue);
}


#if(CY_IP_SPCIF_SYNCHRONOUS)
/*******************************************************************************
* Function Name: CySysFlashClockConfig
****************************************************************************//**
*
* Configures the device clocks for the flash writing.
*
* \return The same as \ref CySysFlashWriteRow().
*
*******************************************************************************/
static cystatus CySysFlashClockConfig(void)
{
    cystatus retValue = CY_SYS_FLASH_SUCCESS;

    /***************************************************************************
    * The FM-Lite IP uses the IMO at 48MHz for the pump clock and SPC timer
    * clock. The PUMP_SEL and HF clock must be set to IMO before calling Flash
    * write or erase operation.
    ***************************************************************************/
#if (CY_PSOC4_4000)

    /***************************************************************************
    * Perform firmware clock settings setup for the PSOC4 4000 devices (the
    * corresponding system call is not reliable):
    * - The IMO frequency should be 48 MHz
    * - The IMO should be source for the HFCLK
    * - The IMO should be the source for the charge pump clock
    *
    * Note The structure members used below are initialized by
    * the CySysFlashClockBackup() function.
    ***************************************************************************/
    if ((cySysFlashBackup.clkImoFreq != 48u) ||
        ((cySysFlashBackup.clkSelectReg & CY_SYS_CLK_SELECT_DIRECT_SEL_MASK) != CY_SYS_CLK_HFCLK_IMO) ||
        (((cySysFlashBackup.clkSelectReg >> CY_SYS_CLK_SELECT_PUMP_SEL_SHIFT) & CY_SYS_CLK_SELECT_PUMP_SEL_MASK) !=
            CY_SYS_CLK_SELECT_PUMP_SEL_IMO))
    {
        /***********************************************************************
         Set HFCLK divider to divide-by-4 to ensure that System clock frequency
        * is within the valid limit (16 MHz for the PSoC4 4000).
        ***********************************************************************/
        CySysClkWriteHfclkDiv(CY_SYS_CLK_HFCLK_DIV_4);

        /* The IMO frequency should be 48 MHz */
        if (cySysFlashBackup.clkImoFreq != 48u)
        {
            CySysClkWriteImoFreq(48u);
        }
        CySysClkImoStart();

        /* The IMO should be source for the HFCLK */
        CySysClkWriteHfclkDirect(CY_SYS_CLK_HFCLK_IMO);

        /* The IMO should be the source for the charge pump clock */
        CY_SYS_CLK_SELECT_REG =  (CY_SYS_CLK_SELECT_REG &
            ((uint32)~(uint32)(CY_SYS_CLK_SELECT_PUMP_SEL_MASK << CY_SYS_CLK_SELECT_PUMP_SEL_SHIFT))) |
            ((uint32)((uint32)1u << CY_SYS_CLK_SELECT_PUMP_SEL_SHIFT));
    }

#else

    /* FM-Lite Clock Configuration */
    CY_FLASH_CPUSS_SYSARG_REG =
        (uint32) ((CY_FLASH_KEY_TWO(CY_FLASH_API_OPCODE_CLK_CONFIG) <<  CY_FLASH_PARAM_KEY_TWO_OFFSET) |
                    CY_FLASH_KEY_ONE);
    CY_FLASH_CPUSS_SYSREQ_REG = CY_FLASH_CPUSS_REQ_START | CY_FLASH_API_OPCODE_CLK_CONFIG;
    CY_NOP;
    retValue = CY_FLASH_API_RETURN;

#endif  /* (CY_PSOC4_4000) */

    return (retValue);
}
#endif  /* (CY_IP_SPCIF_SYNCHRONOUS) */


/*******************************************************************************
* Function Name: CySysFlashClockRestore
****************************************************************************//**
*
* Restores the device clock configuration.
*
* \return The same as \ref CySysFlashWriteRow().
*
*******************************************************************************/
static cystatus CySysFlashClockRestore(void)
{
    cystatus retValue = CY_SYS_FLASH_SUCCESS;
#if(!CY_IP_FM)
    #if !(CY_PSOC4_4000)
        #if (CY_IP_SPCIF_SYNCHRONOUS)
            volatile uint32   parameters[2u];
        #endif /* (CY_IP_SPCIF_SYNCHRONOUS) */
    #endif  /* !(CY_PSOC4_4000) */
#endif  /* (!CY_IP_FM) */

#if(CY_IP_FM)

    /***************************************************************
    * Restore IMO configuration that could be changed during
    * system call execution (Cypress ID #150448).
    ***************************************************************/
    CY_SYS_CLK_IMO_CONFIG_REG = cySysFlashBackup.imoConfigReg;

#else

    #if (CY_PSOC4_4000)

        /***************************************************************************
        * Perform firmware clock settings restore for the PSOC4 4000 devices (the
        * corresponding system call is not available).
        ***************************************************************************/

        /* Restore clock settings */
        if ((cySysFlashBackup.clkImoFreq != 48u) ||
            ((cySysFlashBackup.clkSelectReg & CY_SYS_CLK_SELECT_DIRECT_SEL_MASK) != CY_SYS_CLK_HFCLK_IMO) ||
            (((cySysFlashBackup.clkSelectReg >> CY_SYS_CLK_SELECT_PUMP_SEL_SHIFT) & CY_SYS_CLK_SELECT_PUMP_SEL_MASK) !=
                CY_SYS_CLK_SELECT_PUMP_SEL_IMO))
        {
            /* Restore IMO frequency if needed */
            if (cySysFlashBackup.clkImoFreq != 48u)
            {
                CySysClkWriteImoFreq(cySysFlashBackup.clkImoFreq);
            }

            /* Restore HFCLK clock source */
            CySysClkWriteHfclkDirect(cySysFlashBackup.clkSelectReg & CY_SYS_CLK_SELECT_DIRECT_SEL_MASK);

            /* Restore HFCLK divider and source for pump  */
            CY_SYS_CLK_SELECT_REG = cySysFlashBackup.clkSelectReg;

            /* Stop IMO if needed */
            if (0u == cySysFlashBackup.clkImoEna)
            {
                CySysClkImoStop();
            }
        }

    #else

        #if (CY_IP_SPCIF_SYNCHRONOUS)
            /* FM-Lite Clock Restore */
            parameters[0u] =
                (uint32) ((CY_FLASH_KEY_TWO(CY_FLASH_API_OPCODE_CLK_RESTORE) <<  CY_FLASH_PARAM_KEY_TWO_OFFSET) |
                            CY_FLASH_KEY_ONE);
            parameters[1u] = (uint32) &cySysFlashBackup.clockSettings[0u];
            CY_FLASH_CPUSS_SYSARG_REG = (uint32) &parameters[0u];
            CY_FLASH_CPUSS_SYSREQ_REG = CY_FLASH_CPUSS_REQ_START | CY_FLASH_API_OPCODE_CLK_RESTORE;
            CY_NOP;
            retValue = CY_FLASH_API_RETURN;
        #endif /* (CY_IP_SPCIF_SYNCHRONOUS) */

    #endif  /* (CY_PSOC4_4000) */

#endif  /* (CY_IP_FM) */

    return (retValue);
}


/* [] END OF FILE */
