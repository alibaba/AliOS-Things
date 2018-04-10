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
* File Name    : r_codeflash.c
* Description  : Flash Control Processing of E2 Code Flash.
* Limitation   : none
*******************************************************************************/
/*******************************************************************************
* History      : DD.MM.YYYY Version  Description
*                17.07.2014 1.00     First Release
*                12.11.2014 1.10     As per Japan: Added FLASH.FASR.BIT.EXS
*                                    clearing to multiple functions. Added
*                                    loop timeout and fixed PRGERR check in
*                                    R_CF_Write_Check().
*                19.12.2014 1.20     Replaced some equate names and modified 
*                                    for FLASH_TYPE_1.
*                12.01.2015 1.30     Modified for RX231 support.
*                11.11.2015 1.40     Modified for RX24t support
*                                      (see defined(BSP_MCU_RX24_ALL))
*                02.08.2016 2.00     Modified for BSPless flash
*                17.11.2016 2.10     Moved main FLASH_TYPE #if to encompass
*                                      more include files.
*                02.08.2017 2.20     Removed #include "r_mcu_config.h". Now in
*                                    targets.h (r_flash_rx_if.h includes)
*******************************************************************************/

/******************************************************************************
Includes   <System Includes> , “Project Includes”
******************************************************************************/
#include "r_flash_rx_if.h"
#if (FLASH_TYPE == FLASH_TYPE_1)
#include "r_flash_common.h"
#include "r_codeflash.h"
#include "r_flash_type1_if.h"
#include "r_flash_group.h"

/******************************************************************************
Private global variables and functions
******************************************************************************/
#if (FLASH_CFG_CODE_FLASH_ENABLE == 1)
static r_codeflash_data_t  code_flash_info;

static void r_cf_write_fpmcr (uint8_t value);
extern void r_flash_delay_us (unsigned long us, unsigned long khz);

#pragma section FRAM

/*******************************************************************************
* Outline      : Transition to P/E mode
* Header       : r_codeflash.h
* Function Name: R_CF_Enter_PE_Mode
* Description  : Executes the sequence to enter P/E mode.
* Arguments    : none
* Return Value : none
*******************************************************************************/
void R_CF_Enter_PE_Mode(void)
{
    FLASH.FENTRYR.WORD = FENTRYR_CODEFLASH_PE_MODE;

    r_cf_write_fpmcr(DISCHARGE_1);

    /* Wait for 2us over (tDIS) */
    r_flash_delay_us(WAIT_TDIS, ICLK_KHZ);

    if (OPCCR_HIGH_SPEED_MODE == SYSTEM.OPCCR.BIT.OPCM)
    {
        r_cf_write_fpmcr(DISCHARGE_2);
        r_cf_write_fpmcr(CODEFLASH_PE_MODE);

        /* Wait for 5us over (tMS) */
        r_flash_delay_us(WAIT_TMS_HIGH, ICLK_KHZ);
    }
    else
    {
        r_cf_write_fpmcr(DISCHARGE_2 | LVPE_MODE);
        r_cf_write_fpmcr(CODEFLASH_PE_MODE | LVPE_MODE);

        /* Wait for 3us over (tMS) */
        r_flash_delay_us(WAIT_TMS_MID, ICLK_KHZ);
    }

    FLASH.FISR.BIT.PCKA = FCLK_MHZ - 1;
}

/*******************************************************************************
* Outline      : Transition to read mode
* Header       : r_codeflash.h
* Function Name: R_CF_Enter_Read_Mode
* Description  : Executes the sequence to enter read mode.
* Arguments    : none
* Return Value : none
*******************************************************************************/
void R_CF_Enter_Read_Mode(void)
{
    r_cf_write_fpmcr(DISCHARGE_2);

    /* Wait for 2us over (tDIS) */
    r_flash_delay_us(WAIT_TDIS, ICLK_KHZ);

    r_cf_write_fpmcr(DISCHARGE_1);
    r_cf_write_fpmcr(READ_MODE);

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
* Header       : r_codeflash.h
* Function Name: R_CF_Write
* Description  : Programs the specified data to the Code Flash data. Address validation
*                has already been performed by the caller.
* Arguments    : psrc_addr   : Start address of the (RAM) area which stores the programming data
*              : dest_addr   : Flash Start address which will be written
*              : byte_length : Number of bytes to write
* Return Value : None
*******************************************************************************/
void R_CF_Write (uint32_t *psrc_addr, const uint32_t dest_addr, const uint32_t byte_length )
{
    code_flash_info.start_addr = (uint32_t)psrc_addr;                 // Ram Source for data to write
    code_flash_info.end_addr   = dest_addr;                           // Flash Start address which will be written
    code_flash_info.write_cnt = byte_length / FLASH_CF_MIN_PGM_SIZE;  //  Number of units to write

    /* Select User Area */
    FLASH.FASR.BIT.EXS = 0;
    
    /* Pass in Read form address */
    R_CF_Write_Operation (psrc_addr,  dest_addr);
}

/*******************************************************************************
* Outline      : Programming
* Header       : r_codeflash.h
* Function Name: R_CF_Write_Operation
* Description  : Programs the specified data to the Code Flash data.
* Arguments    : block_start_addr   : Source address for data to be written
*              : block_end_addr     : End address (read form) for writing
* Return Value : none
*******************************************************************************/
void R_CF_Write_Operation (const uint32_t *psrc_addr, const uint32_t dest_addr)
{
    uint32_t dest_addr_idx;

    dest_addr_idx = dest_addr - CODEFLASH_ADDR_OFFSET;  /* Conversion to the P/E address from the read address */

#if defined(MCU_RX23_ALL) || defined (MCU_RX24_ALL)
    /* Write start address setting */
    FLASH.FSARH = (uint16_t)(dest_addr_idx >> 16);
    FLASH.FSARL = (uint16_t)(dest_addr_idx & 0xFFFF);

    FLASH.FWB1 = (uint16_t)(*psrc_addr >> 16);
    FLASH.FWB0 = (uint16_t)(*psrc_addr & 0xFFFF);

    psrc_addr ++;

    FLASH.FWB3 = (uint16_t)(*psrc_addr >> 16);
    FLASH.FWB2 = (uint16_t)(*psrc_addr & 0xFFFF);
#else
    /* Write start address setting */
    FLASH.FSARH = (uint8_t)((dest_addr_idx >> 16) & 0x0F);
    FLASH.FSARL = (uint16_t)(dest_addr_idx & 0xFFFF);

    /* Write data setting */
    FLASH.FWBH = (uint16_t)(*psrc_addr >> 16);
    FLASH.FWBL = (uint16_t)(*psrc_addr & 0xFFFF);
#endif

    /* Execute Write command */
    FLASH.FCR.BYTE = FCR_WRITE;
}

/*******************************************************************************
* Outline      : Verifying write command execution
* Header       : r_codeflash.h
* Function Name: R_CF_Write_Check
* Description  : Waits for the write command to be completed
               : and verifies the result of the command execution.
* Arguments    : none
* Return Value : FLASH_SUCCESS       - Command executed successfully
*              : FLASH_ERR_BUSY      - Command being executed
*              : FLASH_ERR_TIMEOUT   - Command timed out
*              : FLASH_ERR_FAILURE   - Command failed for some reason
*******************************************************************************/
flash_err_t R_CF_Write_Check (void)
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
        flash_reset();
        status = FLASH_ERR_FAILURE;
    }
    else
    {
        code_flash_info.start_addr += FLASH_CF_MIN_PGM_SIZE;
        code_flash_info.end_addr += FLASH_CF_MIN_PGM_SIZE;
        code_flash_info.write_cnt--;
        
        /* timeout counter reset */
        g_current_parameters.wait_cnt = WAIT_MAX_ROM_WRITE;

        if (code_flash_info.write_cnt)
        {
            R_CF_Write_Operation((const uint32_t *)code_flash_info.start_addr, code_flash_info.end_addr);
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
* Header       : r_codeflash.h
* Function Name: R_CF_Erase
* Description  : Performs block erasing between specified addresses.
* Arguments    : start_addr   : Start address for erasing
*              : num          : End address for erasing
* Return Value : none
*******************************************************************************/
void R_CF_Erase (const uint32_t start_addr, const uint32_t num_blocks)
{
    uint32_t block_start_addr;
    uint32_t block_end_addr;

    block_start_addr = start_addr - CODEFLASH_ADDR_OFFSET;  /* Conversion to the P/E address from the read address */
    block_end_addr   = ((start_addr + (num_blocks * FLASH_CF_BLOCK_SIZE)) - CODEFLASH_ADDR_OFFSET) - 1;

    /* Select User Area */
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
* Header       : r_codeflash
* Function Name: R_CF_Erase_Check
* Description  : Waits for the erase command to be completed
               : and verifies the result of the command execution.
* Arguments    : none
* Return Value : FLASH_SUCCESS       - Command executed successfully
*              : FLASH_ERR_BUSY      - BUSY status
*              : FLASH_ERR_TIMEOUT   - Erase command timed out
*              : FLASH_ERR_FAILURE   - Command failed for some reason
*******************************************************************************/
flash_err_t R_CF_Erase_Check (void)
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

    /* 0 != is correct */
    while (0 != FLASH.FSTATR1.BIT.FRDY)
    {
        /* Check that execute command is completed. */
    }

    if ((0 != FLASH.FSTATR0.BIT.ILGLERR) || (0 != FLASH.FSTATR0.BIT.ERERR))
    {
        flash_reset();
        return FLASH_ERR_FAILURE;
    }

    return FLASH_SUCCESS;

}


/*******************************************************************************
* Outline      : Blank check
* Header       : r_codeflash.h
* Function Name: R_CF_BlankCheck
* Description  : Performs blank check for the area between specified addresses.
* Arguments    : start_addr   : Start address for blank check
*              : end_addr     : End address for blank check
* Return Value : none
*******************************************************************************/
void R_CF_BlankCheck (const uint32_t start_addr, const uint32_t end_addr)
{
    uint32_t start_addr_idx;
    uint32_t end_addr_idx;

    start_addr_idx = start_addr - CODEFLASH_ADDR_OFFSET;    /* Conversion to the P/E address from the read address */
    end_addr_idx   = end_addr - CODEFLASH_ADDR_OFFSET;      /* Conversion to the P/E address from the read address */

    /* Select User Area */
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
* Header       : r_codeflash.h
* Function Name: R_CF_BlankCheck_Check
* Description  : Waits for the blank check to be completed.
* Arguments    : none
* Return Value : FLASH_SUCCESS       - Command executed successfully, area is blank
*              : FLASH_ERR_BUSY      - Command being executed
*              : FLASH_ERR_TIMEOUT   - Command timed out
*              : FLASH_ERR_FAILURE   - Command failed for some reason or area
*                                      is not blank
*******************************************************************************/
flash_err_t R_CF_BlankCheck_Check (void)
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

    if ((0 != FLASH.FSTATR0.BIT.ILGLERR) || (0 != FLASH.FSTATR0.BIT.BCERR))
    {
        flash_reset();
        return FLASH_ERR_FAILURE;
    }

    return FLASH_SUCCESS;
}


/*******************************************************************************
* Outline      : Setting the FPMCR register
* Header       : r_codeflash.h
* Function Name: r_cf_write_fpmcr
* Description  : Specifies the argument set to the FPMCR register by the program sequence.
* Arguments    : value     : Setting value for the FPMCR register
* Return Value : none
*******************************************************************************/
static void r_cf_write_fpmcr (uint8_t value)
{
    FLASH.FPR        = 0xA5;
    FLASH.FPMCR.BYTE = value;
    FLASH.FPMCR.BYTE = (uint8_t)~value;
    FLASH.FPMCR.BYTE = value;

    if(value == FLASH.FPMCR.BYTE)
    {
        nop();
    }

}

#pragma section /* end FLASH_SECTION_ROM */
#endif

#endif

/* end of file */
