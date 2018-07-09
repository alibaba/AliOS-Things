/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2016 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 * File Name    : r_flash_utils.c
 * Description  : This module implements the RX11x specific sections of the FLASH API
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*           12.07.2014 1.00    First Release
*           19.12.2014 1.10    Removed unused functions. Check for FLASH_TYPE_1.
*           12.01.2015 1.20    Modified for RX231 support
*           01.09.2015 1.30    Modified to use FLASH_NO_DATA_FLASH for RX23T and RX110.
*           12.08.2016 2.00    Modified for BSPless operation.
*           15.11.2016 2.10    Added function flash_stop().
*           05.10.2016 3.00    Moved flash_interrupt_config() to r_flash_group.c.
***********************************************************************************************************************/

/***********************************************************************************************************************
 Macro definitions
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 Typedef definitions
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 Private global variables and functions
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 ***********************************************************************************************************************/
/* Includes board and MCU related header files. */
#include "r_flash_rx_if.h"
#if (FLASH_TYPE == FLASH_TYPE_1)

/* Configuration for this package. */
#include "r_flash_rx_config.h"

/* Private header file for this package. */
#include "r_flash_type1_if.h"

#include "r_flash_common.h"
#include "r_dataflash.h"
#include "r_codeflash.h"
#include "r_codeflash_extra.h"

/***********************************************************************************************************************
 Macro definitions
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 Typedef definitions
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 Private global variables and functions
 ***********************************************************************************************************************/
#if ((FLASH_CFG_CODE_FLASH_ENABLE == 1) && (FLASH_CFG_CODE_FLASH_BGO == 1)) || (FLASH_CFG_DATA_FLASH_BGO == 1)
extern void (*flash_ready_isr_handler)(void *);          /* Function pointer for Flash Ready ISR */
#endif

#if (FLASH_CFG_CODE_FLASH_ENABLE == 1)
/*All the functions below need to be placed in RAM if Code Flash programming is to be supported */
#pragma section FRAM
#endif

/***********************************************************************************************************************
 * Function Name: flash_pe_mode_enter
 * Description  : Function switches the peripheral to P/E mode for Code Flash or Data Flash.
 *              : And Flash Ready Interrupt enable (BGO mode)
 * Arguments    : flash_type_t : Specify Code Flash or Data Flash
 * Return Value : FLASH_SUCCESS -
 *                Switched successfully.
 *                FLASH_ERR_FAILURE -
 *                Unable to Switch to P/E Mode.
 *                FLASH_ERR_PARAM -
 *                Illegal parameter passed
 ***********************************************************************************************************************/
flash_err_t flash_pe_mode_enter(flash_type_t flash_type)
{
    flash_err_t err = FLASH_SUCCESS;

    if (flash_type == FLASH_TYPE_DATA_FLASH)
    {
#ifndef FLASH_NO_DATA_FLASH     /* RX110/23T has no DF */
        R_DF_Enter_PE_Mode();           /* Sets PCKA clock */
#if (FLASH_CFG_DATA_FLASH_BGO == 1)
        /* Enable Flash Ready Interrupt */
        IR(FCU,FRDYI) = 0;
        IEN(FCU,FRDYI)= 1;
#endif
#endif
    }
#if (FLASH_CFG_CODE_FLASH_ENABLE == 1)
    else if (flash_type == FLASH_TYPE_CODE_FLASH)
    {
        R_CF_Enter_PE_Mode();

#if (FLASH_CFG_CODE_FLASH_BGO == 1)
        /* Enable Flash Ready Interrupt */
        IR(FCU,FRDYI) = 0;
        IEN(FCU,FRDYI)= 1;
#endif
    }
#endif
    else
    {
        err = FLASH_ERR_PARAM;
    }

    return err;
}


/***********************************************************************************************************************
 * Function Name: flash_erase
 * Description  : Function erases a block of Code or Data Flash
 ** Arguments   : uint32_t block address
 * Return Value : FLASH_SUCCESS -
 *                Block Erased successfully.
 *                FLASH_ERR_TIMEOUT
 *                Erase operation timed out.
 *                FLASH_ERR_FAILURE
 *                Erase operation failed for some other reason
 ***********************************************************************************************************************/
flash_err_t flash_erase(uint32_t block_address, uint32_t num_blocks)
{
    flash_err_t err = FLASH_SUCCESS;

    if (FLASH.FENTRYR.WORD == 0x0080)
    {
#ifndef FLASH_NO_DATA_FLASH     /* RX110/23T has no DF */
        /* Write the actual registers for first block */
        R_DF_Erase(block_address, num_blocks);  /* Set the start and end and do first block */

        /* If configured for Blocking mode then don't return until the entire operation is complete */
#if (FLASH_CFG_DATA_FLASH_BGO == 0)
        do
        {
            err = R_DF_Erase_Check();
        }
        while (FLASH_ERR_BUSY == err);
#endif
#endif
    }
#if (FLASH_CFG_CODE_FLASH_ENABLE == 1)
    else if (FLASH.FENTRYR.WORD == 0x0001)
    {
        /* Write the actual registers for first block */
        R_CF_Erase(block_address, num_blocks);  /* Set the start and end and do first block */

        /* If configured for Blocking mode then don't return until the entire operation is complete */
#if (FLASH_CFG_CODE_FLASH_BGO == 0)
        do
        {
            err = R_CF_Erase_Check();
        }
        while (FLASH_ERR_BUSY == err);
#endif
    }
#endif
    else
    {
        /* should never get here */
        return FLASH_ERR_FAILURE;
    }

    return err;
}


/***********************************************************************************************************************
 * Function Name: flash_write
 * Description  : Function writes of Code or Data Flash
 ** Arguments   : uint32_t src_address - the RAM area to take the data from
 **               uint32_t dest_address - the Flash address to write the data to
 **               uint32_t num - the count of bytes to write
 * Return Value : FLASH_SUCCESS -
 *                Block Erased successfully.
 *                FLASH_ERR_TIMEOUT
 *                Erase operation timed out.
 *                FLASH_ERR_FAILURE
 *                Erase operation failed for some other reason
 ***********************************************************************************************************************/
flash_err_t flash_write(uint32_t src_address, uint32_t dest_address, uint32_t num)
{
    flash_err_t err = FLASH_SUCCESS;

    if (FLASH.FENTRYR.WORD == 0x0080)
    {
#ifndef FLASH_NO_DATA_FLASH     /* RX110/23T has no DF */
        /* Write the actual registers for first block */
        R_DF_Write((uint32_t *)src_address, dest_address, num);  /* Set the start and end and do first write */

        /* If configured for Blocking mode then don't return until the entire operation is complete */
#if (FLASH_CFG_DATA_FLASH_BGO == 0)
        do
        {
            err = R_DF_Write_Check();
        }
        while (FLASH_ERR_BUSY == err);
#endif
#endif
     }
#if (FLASH_CFG_CODE_FLASH_ENABLE == 1)
    else if (FLASH.FENTRYR.WORD == 0x0001)
    {
        R_CF_Write((uint32_t *)src_address, dest_address, num);  /* Set the start and end and do first write */

        /* If configured for Blocking mode then don't return until the entire operation is complete */
#if (FLASH_CFG_CODE_FLASH_BGO == 0)
        do
        {
            err = R_CF_Write_Check();
        }
        while (FLASH_ERR_BUSY == err);
#endif
    }
#endif
    else
    {
        /* should never get here */
        return FLASH_ERR_FAILURE;
    }
    return err;
}

/***********************************************************************************************************************
 * Function Name: flash_blankcheck
 * Description  : Function checks if the specified Data Flash address range is blank
 * Arguments    : uint32_t address -
 *                 Start address to perform blank check. Actual address or entry from "flash_block_address_t" enum can be used
 *                uint32_t num_blocks -
 *                 Number of bytes to perform blank check operation for.
 *                flash_res_t *result
 *                 Returns the result of the blank check operation. This field is valid only in non-BGO mode
 *                  operation
 * Return Value : FLASH_ERR_TIMEOUT
 *                Operation timed out.
 *                FLASH_SUCCESS
 *                Operation started successfully (BGO/polling)
 *                Operation completed (Blocking)
 ***********************************************************************************************************************/
flash_err_t flash_blankcheck(uint32_t start_address, uint32_t num_bytes, flash_res_t *result)
{
    flash_err_t err = FLASH_SUCCESS;
    (void)&err;        /* Prevent compiler warning for not using 'err' AND not generate any code  */

    if (FLASH.FENTRYR.WORD == 0x0080)
    {
#ifndef FLASH_NO_DATA_FLASH     /* RX110/23T has no DF */
        R_DF_BlankCheck (start_address, (start_address + num_bytes) - 1);     /* We want to blank check 0 - num_bytes-1 */

#if (FLASH_CFG_DATA_FLASH_BGO == 0)
        do
        {
            err = R_DF_BlankCheck_Check();
        }
        while (FLASH_ERR_BUSY == err);
        
        /* If we blocked until done then we have the result here */
        if (FLASH_SUCCESS == err)
        {
            *result = FLASH_RES_BLANK;
        }
        else if (FLASH_ERR_FAILURE == err)
        {
            *result = FLASH_RES_NOT_BLANK;
            err = FLASH_SUCCESS;
        }
        else
        {
            /* timeout occurs */
        }
#endif
#endif
    }
#if (FLASH_CFG_CODE_FLASH_ENABLE == 1)
    else if (FLASH.FENTRYR.WORD == 0x0001)
    {
        R_CF_BlankCheck (start_address, start_address+num_bytes-1);   /* We want to blank check 0 - num_bytes-1 */

#if (FLASH_CFG_CODE_FLASH_BGO == 0)
        do
        {
            err = R_CF_BlankCheck_Check();
        }
        while (FLASH_ERR_BUSY == err);

        /* If we blocked until done then we have the result here */
        if (FLASH_SUCCESS == err)
        {
            *result = FLASH_RES_BLANK;
        }
        else if (FLASH_ERR_FAILURE == err)
        {
            *result = FLASH_RES_NOT_BLANK;
            err = FLASH_SUCCESS;
        }
        else
        {
            /* timeout occurs */
        }
#endif
    }
#endif

    return err;
}


/***********************************************************************************************************************
 * Function Name: flash_pe_mode_exit
 * Description  : Exit PE mode to either DF or CF Read
 *              : And Flash Ready Interrupt disable (BGO mode)
 **********************************************************************************************************************/
flash_err_t flash_pe_mode_exit()
{

    if (FLASH.FENTRYR.WORD == 0x0080)
    {
#ifndef FLASH_NO_DATA_FLASH     /* RX110/23T has no DF */
        R_DF_Enter_Read_Mode();
#endif
    }
#if (FLASH_CFG_CODE_FLASH_ENABLE == 1)
    else if (FLASH.FENTRYR.WORD == 0x0001)
    {
        R_CF_Enter_Read_Mode();
    }
#endif

#if ((FLASH_CFG_CODE_FLASH_ENABLE == 1) && (FLASH_CFG_CODE_FLASH_BGO == 1) || (FLASH_CFG_DATA_FLASH_BGO == 1))
    /* Enable Flash Ready Interrupt */
    IEN(FCU,FRDYI)= 0;
#endif

    return(FLASH_SUCCESS);
}


/***********************************************************************************************************************
 * Function Name: flash_stop
 * Description  : Function issues the STOP command
 * Arguments    : None
 * Return Value : None
 ***********************************************************************************************************************/
void flash_stop(void)
{

    FLASH.FCR.BIT.STOP = 1;
    while (FLASH.FSTATR1.BIT.FRDY == 0)     // wait for FRDY
        ;

    FLASH.FCR.BYTE = 0;
}


/***********************************************************************************************************************
 * Function Name: flash_reset
 * Description  : Function resets the Flash peripheral
 * Arguments    : None
 * Return Value : FLASH_SUCCESS -
 *                Flash circuit successfully reset.
 ***********************************************************************************************************************/
flash_err_t flash_reset() 
{
    /* Flash control circuit. */
    FLASH.FRESETR.BIT.FRESET = 1;
    FLASH.FRESETR.BIT.FRESET = 0;
    return FLASH_SUCCESS;
}



#pragma section /* end FLASH_SECTION_ROM */
#endif
/* end of file */
