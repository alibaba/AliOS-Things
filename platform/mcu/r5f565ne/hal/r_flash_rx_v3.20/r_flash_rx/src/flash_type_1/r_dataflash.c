/*******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized. This
* software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software. By using this software,
* you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2015 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/*******************************************************************************
* File Name    : r_dataflash.c
* Description  : Flash Control Processing of E2 Data Flash.
* Limitation   : none
*******************************************************************************/
/*******************************************************************************
* History:  DD.MM.YYYY Version  Description
*           17.07.2014 1.00     First Release
*           12.11.2014 1.10     As per Japan: Removed call to
*                               R_DF_Enable_DataFlashAccess(). Added
*                               FLASH.FASR.BIT.EXS clearing to multiple
*                               functions. Removed RX110 no data flash
*                               warning. Fixed PRGERR check in R_DF_Write_Check().
*           19.12.2014 1.20     Replaced some equate names and modified
*                               for FLASH_TYPE_1.
*           12.01.2015 1.30     Modified for RX231 support
*           01.09.2015 1.40     Modified to use FLASH_NO_DATA_FLASH for RX23T and RX110.
*                               Fixed Big Endian address error in R_DF_Write_Operation()
*           11.11.2015 1.50     Modified for RX24t support
*                               (see defined(BSP_MCU_RX24_ALL))
*           02.08.2016 2.00     Modified for BSPless flash.
*           02.08.2017 2.10     Removed #include "r_mcu_config.h". Now in
*                               targets.h (r_flash_rx_if.h includes)
*******************************************************************************/

/******************************************************************************
Includes   <System Includes> , “Project Includes”
******************************************************************************/
#include "r_flash_rx_if.h"
#if (FLASH_TYPE == FLASH_TYPE_1)
#ifndef FLASH_NO_DATA_FLASH
#include "r_flash_common.h"
#include "r_dataflash.h"
#include "r_flash_type1_if.h"
#include "r_flash_group.h"

/******************************************************************************
Macro definitions
******************************************************************************/


/******************************************************************************
Private global variables and functions
******************************************************************************/
static r_dataflash_data_t  data_flash_info;

static void r_df_write_fpmcr (uint8_t value);
extern void r_flash_delay_us (unsigned long us, unsigned long khz);


/*******************************************************************************
* Outline      : E2 Data Flash Access Enable
* Header       : r_dataflash.h
* Function Name: R_DF_Enable_DataFlashAccess
* Description  : Enable the E2 Data Flash Access and wait for the DataFlash STOP recovery time
* Arguments    : none
* Return Value : none
*******************************************************************************/
void R_DF_Enable_DataFlashAccess(void)
{
#ifndef FLASH_NO_DATA_FLASH

    /* E2 DataFlash Access enable */
    FLASH.DFLCTL.BIT.DFLEN = 1;

    if(1 == FLASH.DFLCTL.BIT.DFLEN)
    {
        nop();
    }

    /* Wait for 5us over (tDSTOP) */
    r_flash_delay_us(WAIT_TDSTOP, ICLK_KHZ);
#endif
}

/*******************************************************************************
* Outline      : Transition to P/E mode
* Header       : r_dataflash.h
* Function Name: R_DF_Enter_PE_Mode
* Description  : Executes the sequence to enter P/E mode.
* Arguments    : none
* Return Value : None
*******************************************************************************/
void R_DF_Enter_PE_Mode(void)
{

    FLASH.FENTRYR.WORD = FENTRYR_DATAFLASH_PE_MODE;

    if (OPCCR_HIGH_SPEED_MODE == SYSTEM.OPCCR.BIT.OPCM)
    {
        r_df_write_fpmcr(DATAFLASH_PE_MODE);
    }
    else
    {
        r_df_write_fpmcr(DATAFLASH_PE_MODE | LVPE_MODE);
    }

    FLASH.FISR.BIT.PCKA = FCLK_MHZ - 1;
}

/*******************************************************************************
* Outline      : Transition to read mode
* Header       : r_dataflash.h
* Function Name: R_DF_Enter_Read_Mode
* Description  : Executes the sequence to enter read mode.
* Arguments    : None
* Return Value : None
*******************************************************************************/
void R_DF_Enter_Read_Mode(void)
{

    r_df_write_fpmcr(READ_MODE);

    /* Wait for 5us over (tMS) */
    r_flash_delay_us(WAIT_TMS_HIGH, ICLK_KHZ);


    FLASH.FENTRYR.WORD = FENTRYR_READ_MODE;

    while (0x0000 != FLASH.FENTRYR.WORD)
    {
        /* Confirm that the written value can be read correctly. */
    }
}

/*******************************************************************************
* Outline      : Programming
* Header       : r_dataflash.h
* Function Name: R_DF_Write
* Description  : Programs the specified data to the E2 Data Flash data. Address validation
*                has already been performed by the caller.
* Arguments    : psrc_addr   : Start address of the (RAM) area which stores the programming data
*              : dest_addr   : Flash Start address which will be written
*              : byte_length : Number of bytes to write
* Return Value : None
*******************************************************************************/
void R_DF_Write(uint32_t *psrc_addr, const uint32_t dest_addr, const uint32_t byte_length )
{
    data_flash_info.start_addr = (uint32_t)psrc_addr;                   /* Ram Source for data to write */
    data_flash_info.end_addr   = dest_addr;                             /* Flash Start address which will be written */
    data_flash_info.write_cnt = byte_length / FLASH_DF_MIN_PGM_SIZE;    /* Number of bytes to write */

    FLASH.FASR.BIT.EXS = 0;

    /* Pass in Read form address */
    R_DF_Write_Operation (psrc_addr, dest_addr);
}

/*******************************************************************************
* Outline      : Programming
* Header       : r_dataflash.h
* Function Name: R_DF_Write_Operation
* Description  : Programs the specified data to the E2 Data Flash data.
* Arguments    : block_start_addr   : Start address (read form) for erasing
*              : block_end_addr     : End address (read form) for erasing
* Return Value : none
*******************************************************************************/
void R_DF_Write_Operation (const uint32_t *psrc_addr,  const uint32_t dest_addr)
{
    uint32_t dest_addr_idx;
    uint8_t *write_data;                            // 2015/9/9 fix for Big Endian
    write_data = (uint8_t *)psrc_addr;              // 2015/9/9

    dest_addr_idx = dest_addr - DATAFLASH_ADDR_OFFSET;  /* Conversion to the P/E address from the read address */

#if defined(MCU_RX23_ALL) || defined(MCU_RX24_ALL)
    /* Write start address setting */
    FLASH.FSARH = (uint16_t)(dest_addr_idx >> 16);
    FLASH.FSARL = (uint16_t)(dest_addr_idx & 0xFFFF);

    FLASH.FWB3 = (uint16_t)0x0000;
    FLASH.FWB2 = (uint16_t)0x0000;
    FLASH.FWB1 = (uint16_t)0x0000;
    //FLASH.FWB0 = (uint8_t)(*psrc_addr & 0x00FF);  // 2015/9/9
    FLASH.FWB0 = (uint16_t)(*write_data);           // 2015/9/9

#else
    /* Write start address setting */
    FLASH.FSARH = (uint8_t)((dest_addr_idx >> 16) & 0x0F);
    FLASH.FSARL = (uint16_t)(dest_addr_idx & 0xFFFF);

    /* Write data setting */
    FLASH.FWBH = (uint16_t)0x0000;
    //FLASH.FWBL = (uint8_t)(*psrc_addr & 0x00FF);  // 2015/9/9
    FLASH.FWBL = (uint16_t)(*write_data);           // 2015/9/9

#endif

    /* Execute Write command */
    FLASH.FCR.BYTE = FCR_WRITE;
}

/*******************************************************************************
* Outline      : Verifying write command execution
* Header       : r_dataflash.h
* Function Name: R_DF_Write_Check
* Description  : Waits for the write command to be completed
               : and verifies the result of the command execution.
* Arguments    : none
* Return Value : FLASH_SUCCESS       - Command executed successfully
*              : FLASH_ERR_BUSY      - Command being executed
*              : FLASH_ERR_TIMEOUT   - Command timed out
*              : FLASH_ERR_FAILURE   - Command failed for some reason
*******************************************************************************/
flash_err_t R_DF_Write_Check (void)
{
    flash_err_t status;

    /* Check FREADY Flag bit*/
    if (1 != FLASH.FSTATR1.BIT.FRDY)
    {
        /* Check that execute command is completed. */
        /* Wait until FRDY is 0 unless timeout occurs. */
        g_current_parameters.wait_cnt--;

        if (g_current_parameters.wait_cnt <= 0)
        {
          /* if FRDY is not set to 1 after max timeout, return error*/
           return FLASH_ERR_TIMEOUT;
        }

        return FLASH_ERR_BUSY;
    }

    /* Clear FCR register */
    FLASH.FCR.BYTE = FCR_CLEAR;

    while (0 != FLASH.FSTATR1.BIT.FRDY)
    {
        /* Check that execute command is completed. */
    }

    if ((0 != FLASH.FSTATR0.BIT.ILGLERR) || (0 != FLASH.FSTATR0.BIT.PRGERR))
    {
        r_df_reset();
        status = FLASH_ERR_FAILURE;
    }
    else
    {
        data_flash_info.start_addr += FLASH_DF_MIN_PGM_SIZE;
        data_flash_info.end_addr += FLASH_DF_MIN_PGM_SIZE;
        data_flash_info.write_cnt--;

        /* timeout counter reset */
        g_current_parameters.wait_cnt = WAIT_MAX_DF_WRITE;

        if (data_flash_info.write_cnt)
        {
            R_DF_Write_Operation((const uint32_t *)data_flash_info.start_addr, data_flash_info.end_addr);
            status = FLASH_ERR_BUSY;
        }
        else
        {
            status = FLASH_SUCCESS;
        }
    }

    return status;
}


/*******************************************************************************
* Outline      : Erasing
* Header       : r_dataflash.h
* Function Name: R_DF_Erase
* Description  : Performs block erasing between specified addresses. Address validation
*                has already been performed by the caller.
* Arguments    : start_addr   : Start address for erasing
*              : num_blocks   : # of blocks to erase
* Return Value : none
*******************************************************************************/
void R_DF_Erase (uint32_t start_addr, uint32_t num_blocks)
{
    uint32_t block_start_addr;
    uint32_t block_end_addr;

    block_start_addr = start_addr - DATAFLASH_ADDR_OFFSET;  /* Conversion to the P/E address from the read address */
    block_end_addr   = ((start_addr + (num_blocks * FLASH_DF_BLOCK_SIZE)) - DATAFLASH_ADDR_OFFSET) - 1;

    FLASH.FASR.BIT.EXS = 0;

#if defined(MCU_RX23_ALL) || defined(MCU_RX24_ALL)
    /* Erase start address setting */
    FLASH.FSARH = (uint16_t)(block_start_addr >> 16);
    FLASH.FSARL = (uint16_t)(block_start_addr & 0xFFFF);

    /* Erase end address setting */
    FLASH.FEARH = (uint16_t)(block_end_addr >> 16);
    FLASH.FEARL = (uint16_t)(block_end_addr & 0xFFFF);
#else
    /* Erase start address setting */
    FLASH.FSARH = (uint8_t)((block_start_addr >> 16) & 0x0F);
    FLASH.FSARL = (uint16_t)(block_start_addr & 0xFFFF);

    /* Erase end address setting */
    FLASH.FEARH = (uint8_t)((block_end_addr >> 16) & 0x0F);
    FLASH.FEARL = (uint16_t)(block_end_addr & 0xFFFF);
#endif

    /* Execute Erase command */
    FLASH.FCR.BYTE = FCR_ERASE;
}


/*******************************************************************************
* Outline      : Verifying erase command execution
* Header       : r_dataflash.h
* Function Name: R_DF_Erase_Check
* Description  : Waits for the erase command to be completed
               : and verifies the result of the command execution.
* Arguments    : none
* Return Value : FLASH_SUCCESS       - Command executed successfully
*              : FLASH_ERR_BUSY      - BUSY status
*              : FLASH_ERR_TIMEOUT   - Erase command timed out
*              : FLASH_ERR_FAILURE   - Command failed for some reason
*******************************************************************************/
flash_err_t R_DF_Erase_Check (void)
{

    /* Check FREADY Flag bit*/
    if (1 != FLASH.FSTATR1.BIT.FRDY)
    {
        /* Check that execute command is completed. */
        /* Wait until FRDY is 0 unless timeout occurs. */
        g_current_parameters.wait_cnt--;

        if (g_current_parameters.wait_cnt <= 0)
        {
          /* if FRDY is not set to 1 after max timeout, return error*/
           return FLASH_ERR_TIMEOUT;
        }

        return FLASH_ERR_BUSY;
    }

    /* Clear FCR register */
    FLASH.FCR.BYTE = FCR_CLEAR;

    while (0 != FLASH.FSTATR1.BIT.FRDY)
    {
        /* Check that execute command is completed. */
    }

    if ((0 != FLASH.FSTATR0.BIT.ILGLERR) || (0 != FLASH.FSTATR0.BIT.ERERR))
    {
        r_df_reset();
        return FLASH_ERR_FAILURE;
    }

    return FLASH_SUCCESS;
}


/*******************************************************************************
* Outline      : Blank check
* Header       : r_dataflash.h
* Function Name: R_DF_BlankCheck
* Description  : Performs blank check for the area between specified addresses.
* Arguments    : start_addr   : Start address for blank check
*              : end_addr     : End address for blank check
* Return Value : FLASH_SUCCESS     - The checked addresses are all blank
*              : FLASH_ERR_FAILURE - Blank check error
*******************************************************************************/
void R_DF_BlankCheck(uint32_t start_addr, uint32_t end_addr)
{
    uint32_t start_addr_idx;
    uint32_t end_addr_idx;

    start_addr_idx = start_addr - DATAFLASH_ADDR_OFFSET;  /* Conversion to the P/E address from the read address */
    end_addr_idx   = end_addr - DATAFLASH_ADDR_OFFSET;    /* Conversion to the P/E address from the read address */

    FLASH.FASR.BIT.EXS = 0;


#if defined(MCU_RX23_ALL) || defined(MCU_RX24_ALL)
    /* BlankCheck start address setting */
    FLASH.FSARH = (uint16_t)(start_addr_idx >> 16);
    FLASH.FSARL = (uint16_t)(start_addr_idx & 0xFFFF);

    /* BlankCheck end address setting */
    FLASH.FEARH = (uint16_t)(end_addr_idx >> 16);
    FLASH.FEARL = (uint16_t)(end_addr_idx & 0xFFFF);

#else
    /* BlankCheck start address setting */
    FLASH.FSARH = (uint8_t)((start_addr_idx >> 16) & 0x0F);
    FLASH.FSARL = (uint16_t)(start_addr_idx & 0xFFFF);

    /* BlankCheck end address setting */
    FLASH.FEARH = (uint8_t)((end_addr_idx >> 16) & 0x0F);
    FLASH.FEARL = (uint16_t)(end_addr_idx & 0xFFFF);

#endif

    /* Execute BlankCheck command */
    FLASH.FCR.BYTE = FCR_BLANKCHECK;

}

/*******************************************************************************
* Outline      : Verifying blank check command execution
* Header       : r_dataflash.h
* Function Name: R_DF_BlankCheck_Check
* Description  : Performs blank check for the area between specified addresses.
* Arguments    : none
* Return Value : FLASH_SUCCESS       - Command executed successfully, area is blank
*              : FLASH_ERR_BUSY      - Command being executed
*              : FLASH_ERR_TIMEOUT   - Command timed out
*              : FLASH_ERR_FAILURE   - Command failed for some reason or area
*                                      is not blank
*******************************************************************************/
flash_err_t R_DF_BlankCheck_Check (void)
{
    /* Check FREADY Flag bit*/
    if (1 != FLASH.FSTATR1.BIT.FRDY)
    {
        /* Check that execute command is completed. */
        /* Wait until FRDY is 0 unless timeout occurs. */
        g_current_parameters.wait_cnt--;

        if (g_current_parameters.wait_cnt <= 0)
        {
          /* if FRDY is not set to 1 after max timeout, return error*/
           return FLASH_ERR_TIMEOUT;
        }

        return FLASH_ERR_BUSY;
    }

    /* Clear FCR register */
    FLASH.FCR.BYTE = FCR_CLEAR;

    while (0 != FLASH.FSTATR1.BIT.FRDY)
    {
        /* Check that execute command is completed. */
    }

    if (0 != FLASH.FSTATR0.BIT.ILGLERR)
    {
        r_df_reset();
        return FLASH_ERR_FAILURE;
    }

    if (0 != FLASH.FSTATR0.BIT.BCERR)       /* Tested Flash Area is not blank */
    {
        r_df_reset();
        return FLASH_ERR_FAILURE;           /* Not really an error... */
    }

    return FLASH_SUCCESS;
}


/*******************************************************************************
* Outline      : Setting the FPMCR register
* Header       : r_dataflash.h
* Function Name: r_df_write_fpmcr
* Description  : Specifies the argument set to the FPMCR register by the program sequence.
* Arguments    : value     : Setting value for the FPMCR register
* Return Value : none
*******************************************************************************/
static void r_df_write_fpmcr (uint8_t value)
{
    FLASH.FPR = 0xA5;
    FLASH.FPMCR.BYTE = value;
    FLASH.FPMCR.BYTE = (uint8_t)~value;
    FLASH.FPMCR.BYTE = value;

    if(value == FLASH.FPMCR.BYTE)
    {
        nop();
    }

}

/*******************************************************************************
* Outline      : Flash reset
* Header       : r_dataflash.h
* Function Name: r_df_reset
* Description  : Resets the flash control circuit.
* Arguments    : none
* Return Value : none
*******************************************************************************/
void r_df_reset (void)
{
    /* Reset Flash */
    FLASH.FRESETR.BIT.FRESET = 1;
    FLASH.FRESETR.BIT.FRESET = 0;
}

#endif
#endif

/* end of file */
