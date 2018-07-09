/********************************************************************************************************************
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
********************************************************************************************************************/
/*******************************************************************************************************************
* File Name : r_flash_type3.c
* Description  : This module implements functions specific to Flash Type 3 MCUs
********************************************************************************************************************/
/*******************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 02.18.2014 1.10    Support for RX64M
*           19.12.2014 1.20    Replaced some equate names for FLASH_TYPE_3.
*           20.05.2016 1.30    Added BGO write/erase/blankcheck interrupt operation
*                              Fixed FLASH_CMD_LOCKBIT_DISABLE/ENABLE operation.
*           25.05.2016 1.31    Added BGO lockbit read/write interrupt operation
*           13.06.2016 1.40    Fixed timeouts on flash erase and write.
*           10.08.2016 1.50    Cleaned up flash_fcuram_codecopy() function.
*                              Removed setting of flash lock state just before switch in flash_api_control()
*                                which caused CMD_GET_STATUS to always return BUSY.
*                              Added #if to exclude ISR code when not in BGO mode.
*           12.08.2016 2.00    Modified for BSPless operation.
*           17.11.2016 2.10    Fixed flash_reset() so enters PE mode before issuing a flash_stop()
*                                when in idle state.
*           18.11.2016 3.00    Merged functions common to other flash types into r_flash_fcu.c and r_flash_group.c.
********************************************************************************************************************/

/********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
********************************************************************************************************************/
#include "r_flash_rx_if.h"

#if (FLASH_TYPE == FLASH_TYPE_3)
#include "r_flash_rx.h"
#include "r_flash_fcu.h"
#include "r_flash_group.h"
#include "r_flash_type3_if.h"

/******************************************************************************
 Typedef definitions
******************************************************************************/

/***********************************************************************************************************************
 Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
 Private global variables and functions
***********************************************************************************************************************/
lkbit_mode_t g_lkbit_mode = FLASH_LOCKBIT_MODE_NORMAL;

#if (FLASH_CFG_CODE_FLASH_ENABLE == 1)
static flash_err_t flash_lockbit_write(flash_block_address_t block_address, uint32_t num_blocks);

#pragma section FRAM
#endif

/***********************************************************************************************************************
* Function Name: get_cmdlk_err
* Description  : This function identifies the error when FLASH.FASTAT.BIT.CMDLK is set and performs recovery steps.
* Arguments    : None
* Return Value : FLASH_ERR_LOCKBIT_SET -
*                    Operation failed due to lockbit being set
*                FLASH_ERR_FAILURE -
*                    Failure caused by something other than lockbit set
*                FLASH_ERR_CMD_LOCKED -
*                    Flash hardware locked
***********************************************************************************************************************/
flash_err_t get_cmdlk_err(void)
{
    flash_err_t         err;

    /* THIS FUNCTION IS ONLY CALLED AFTER FRDY FLAG IS SET (OPERATION BELIEVED TO BE COMPLETE) */

    /* FPESTAT only valid if FRDY=1 and PRGERR or ERSERR is set */
    if ((0x0001 & FLASH.FPESTAT.WORD)       // error due to lockbit on write or erase
     && ((FLASH.FSTATR.BIT.PRGERR) || (FLASH.FSTATR.BIT.ERSERR)))
    {
        err = FLASH_ERR_LOCKBIT_SET;
    }
    else if ((0x0002 & FLASH.FPESTAT.WORD)  // error due to non-lockbit error
     && ((FLASH.FSTATR.BIT.PRGERR) || (FLASH.FSTATR.BIT.ERSERR)))
    {
        err = FLASH_ERR_FAILURE;
    }
    else
    {
        err = FLASH_ERR_CMD_LOCKED;
    }

    do_cmdlk_recovery();
    return err;
}

/***********************************************************************************************************************
* Function Name: do_cmdlk_recovery
* Description  : This function performs steps to recover from a command-locked state.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void do_cmdlk_recovery(void)
{

    /* THIS FUNCTION IS ONLY CALLED AFTER FRDY FLAG IS SET (OPERATION BELIEVED TO BE COMPLETE) */

    if (FLASH.FSTATR.BIT.ILGLERR)
    {
        if (FLASH.FASTAT.BIT.DFAE == 1)         // tried to access invalid address
        {
            FLASH.FASTAT.BIT.DFAE = 0;          // can only write 0 after 1 is read
        }
        else if (FLASH.FASTAT.BIT.CFAE == 1)    // tried to access invalid address
        {
            FLASH.FASTAT.BIT.CFAE = 0;          // can only write 0 after 1 is read
        }
    }

    if ((FLASH.FSTATR.BIT.FCUERR == 1)
     || (FLASH.FSTATR.BIT.FRDTCT == 1)
     || (FLASH.FSTATR.BIT.FLWEERR == 1)
     || ((FLASH.FSTATR.BIT.FCUERR == 0) && (FLASH.FSTATR.BIT.FRDTCT == 0) && (FLASH.FSTATR.BIT.FLWEERR == 0)))
    {
        flash_stop();
    }
}


#if (FLASH_CFG_DATA_FLASH_BGO || (FLASH_CFG_CODE_FLASH_ENABLE && FLASH_CFG_CODE_FLASH_BGO))

/***********************************************************************************************************************
* Function Name: get_cmdlk_err_event
* Description  : This function identifies the error when FLASH.FASTAT.BIT.CMDLK is set and converts it to
*                an interrupt event for the callback function.
* Arguments    : None
* Return Value : FLASH_INT_EVENT_ERR_DF_ACCESS -
*                    Data flash access error
*                FLASH_INT_EVENT_ERR_CF_ACCESS -
*                    Code flash access error
*                FLASH_INT_EVENT_ERR_LOCKBIT_SET -
*                    Operation failed due to lockbit being set
*                FLASH_INT_EVENT_ERR_FAILURE -
*                    Failure caused by something other than lockbit set
*                FLASH_INT_EVENT_ERR_CMD_LOCKED -
*                    Flash hardware locked
***********************************************************************************************************************/
flash_interrupt_event_t get_cmdlk_err_event(void)
{
    flash_interrupt_event_t event;

    if (1 == FLASH.FASTAT.BIT.DFAE)         // tried to access invalid address
    {
        event = FLASH_INT_EVENT_ERR_DF_ACCESS;
    }
    else if (1 == FLASH.FASTAT.BIT.CFAE)    // tried to access invalid address
    {
        event = FLASH_INT_EVENT_ERR_CF_ACCESS;
    }
    else if ((0x0001 & FLASH.FPESTAT.WORD)
        && ((FLASH.FSTATR.BIT.PRGERR) || (FLASH.FSTATR.BIT.ERSERR)))
    {
        event = FLASH_INT_EVENT_ERR_LOCKBIT_SET;
    }
    else if ((0x0002 & FLASH.FPESTAT.WORD)
        && ((FLASH.FSTATR.BIT.PRGERR) || (FLASH.FSTATR.BIT.ERSERR)))
    {
        event = FLASH_INT_EVENT_ERR_FAILURE;
    }
    else
    {
        event = FLASH_INT_EVENT_ERR_CMD_LOCKED;
    }

    do_cmdlk_recovery();
    return event;
}
#endif // (FLASH_CFG_DATA_FLASH_BGO || (FLASH_CFG_CODE_FLASH_ENABLE && FLASH_CFG_CODE_FLASH_BGO))

#if (FLASH_CFG_CODE_FLASH_ENABLE == 1)
/***********************************************************************************************************************
* Function Name: flash_api_lockbit_set
* Description  : Function will set the lockbit for the specified number of code flash blocks.
* Arguments    : block_address -
*                    Starting address of code flash block to set the lockbit for.
*                num_blocks -
*                    Number of bytes to set the lockbit for.
* Return Value : FLASH_ERR_BLANK -
*                    Specified Data Flash area is blank
*                FLASH_ERR_BYTES -
*                    Number of bytes exceeds max range (1-65535) or is not a multiple of 4
*                FLASH_ERR_ADRRESS -
*                    Start address is an invalid Data Flash Address
*                FLASH_ERR_LOCKED -
*                    The FCU is in a command locked state and cannot process the operation.The function performed a
*                    RESET on the peripheral to recover from this state.
*                FLASH_ERR_FAILURE
*                    Failed to enter PE mode
***********************************************************************************************************************/
flash_err_t flash_api_lockbit_set(flash_block_address_t block_address, uint32_t num_blocks)
{
    flash_err_t err;

#if (FLASH_CFG_PARAM_CHECKING_ENABLE == 1)
    if (((block_address > FLASH_CF_BLOCK_0) || (block_address <= FLASH_CF_BLOCK_INVALID )))
    {
        return FLASH_ERR_ADDRESS;
    }

    if ((num_blocks > FLASH_NUM_BLOCKS_CF) || (num_blocks <= 0))
    {
        return FLASH_ERR_BLOCKS;
    }

    err = check_cf_block_total(block_address, num_blocks);
    if (err != FLASH_SUCCESS)
    {
        return err;     // block_address + num_blocks out of range
    }
#endif

    /* Lock flash driver and set state to LOCK_BIT */
    if (FLASH_SUCCESS != flash_lock_state(FLASH_LOCK_BIT))
    {
        return FLASH_ERR_BUSY;
    }

    /* Transition to CF P/E mode to issue LOCKBIT READ command */
    err = flash_pe_mode_enter(FLASH_TYPE_CODE_FLASH);
    if (FLASH_SUCCESS != err)
    {
        return(err);
    }

    /* Set the current operation */
    g_current_parameters.current_operation = FLASH_CUR_LOCKBIT_SET;

    /* Start the LockBit Set operation */
    err = flash_lockbit_write(block_address, num_blocks);
    if (FLASH_SUCCESS != err)
    {
        flash_reset();
    }

    /* Current operation is completed. Exit from PE mode and return status */
    flash_pe_mode_exit();
    flash_release_state();

    return err;
}


/***********************************************************************************************************************
 * Function Name: flash_lockbit_read
 * Description  : This function reads the lockbit setting for any block.
 * Arguments    : block_address
 *                    Address of block to check
 *                *lock_state
 *                    Set to FLASH_RES_LOCKBIT_STATE_NON_PROTECTED or FLASH_RES_LOCKBIT_STATE_PROTECTED
 * Return Value : FLASH_SUCCESS -
 *                    Operation completed successfully
 *                FLASH_ERR_ADDRESS -
 *                    Invalid block address
 *                FLASH_ERR_FAILURE
 *                    Could net enter/exit PE mode
 *                FLASH_ERR_TIMEOUT -
 *                    Operation timed out
 *                FLASH_ERR_CMD_LOCKED
 *                    Peripheral hardware in locked state. A reset was issued to recover from this state
 ***********************************************************************************************************************/
flash_err_t flash_lockbit_read(flash_block_address_t block_address, flash_res_t *lock_state)
{
    flash_err_t err = FLASH_SUCCESS;

#if (FLASH_CFG_PARAM_CHECKING_ENABLE == 1)
    uint32_t cf_block_size;

    /* Check for valid address range */
    if (((block_address > FLASH_CF_BLOCK_0) || (block_address <= FLASH_CF_BLOCK_INVALID )))
    {
        return FLASH_ERR_ADDRESS;
    }

    /* Check for block boundary */
    cf_block_size = (block_address >= FLASH_CF_BLOCK_7) ? FLASH_CF_SMALL_BLOCK_SIZE : FLASH_CF_MEDIUM_BLOCK_SIZE;
    if ((block_address & (cf_block_size - 1)) != 0)
    {
        return FLASH_ERR_ADDRESS;
    }
#endif

    /* Lock flash driver and set state to LOCK_BIT */
    if (FLASH_SUCCESS != flash_lock_state(FLASH_LOCK_BIT))
    {
        return FLASH_ERR_BUSY;
    }


    /* Transition to CF P/E mode to issue LOCKBIT READ command */
    err = flash_pe_mode_enter(FLASH_TYPE_CODE_FLASH);
    if (FLASH_SUCCESS != err)
    {
        return(err);
    }

    if (g_current_parameters.bgo_enabled_cf == true)
    {
        g_current_parameters.current_operation = FLASH_CUR_CF_BGO_READ_LOCKBIT;
    }

    /* Init flash register and write fcu read lockbit command */
    FLASH.FSADDR.LONG = block_address;
    *g_pfcu_cmd_area = (uint8_t) FLASH_FACI_CMD_LOCK_BIT_READ;
    *g_pfcu_cmd_area = (uint8_t) FLASH_FACI_CMD_FINAL;

    /* In blocking mode, wait for FRDY or timeout */
    if (g_current_parameters.bgo_enabled_cf == false)
    {
        g_current_parameters.wait_cnt = FLASH_FRDY_CMD_TIMEOUT;
        err = flash_wait_frdy();
        if (err == FLASH_SUCCESS)
        {
            /* Load lockbit setting */
            if (0x01 == FLASH.FLKSTAT.BYTE)
            {
                *lock_state = FLASH_RES_LOCKBIT_STATE_NON_PROTECTED;
            }
            else
            {
                *lock_state = FLASH_RES_LOCKBIT_STATE_PROTECTED;
            }
        }
    }

    flash_pe_mode_exit();
    flash_release_state();
    return err;
}


/***********************************************************************************************************************
 * Function Name: flash_lockbit_write
 * Description  : Function to write(set) the lockbit for any block.
 * Arguments    : block_address -
 *                    Starting block to lock
 *                num_blocks -
 *                    Number of blocks to lock in ascending order (descending addresses)
 * Return Value : FLASH_SUCCESS
 *                    Lockbit set for the specified block(s)
 *                FLASH_ERR_TIMEOUT -
 *                    Operation timed out
 *                FLASH_ERR_LOCKBIT_SET -
 *                    Programming error (should never happen)
 *                FLASH_ERR_CMD_LOCKED -
 *                    Flash hardware locked (should never happen)
 ***********************************************************************************************************************/
flash_err_t flash_lockbit_write(flash_block_address_t block_address, uint32_t num_blocks)
{
    flash_err_t err = FLASH_SUCCESS;
    volatile uint32_t wait_cnt = FLASH_FRDY_CMD_TIMEOUT;


    if (g_current_parameters.bgo_enabled_cf == true)
    {
        g_current_parameters.current_operation = FLASH_CUR_CF_BGO_WRITE_LOCKBIT;
    }

    g_current_parameters.dest_addr = block_address;
    g_current_parameters.total_count = num_blocks;

    /* Loop through each block address and set lockbit */
    for (g_current_parameters.current_count = 0;
         g_current_parameters.current_count < g_current_parameters.total_count;
         g_current_parameters.current_count++)
    {
        /* Init flash register and write fcu command */
        FLASH.FSADDR.LONG = g_current_parameters.dest_addr;
        *g_pfcu_cmd_area = (uint8_t) FLASH_FACI_CMD_LOCK_BIT_PGM;
        *g_pfcu_cmd_area = (uint8_t) FLASH_FACI_CMD_FINAL;

        /* Return if in BGO mode. Processing loop will finish in FRDYI interrupt */
        if (g_current_parameters.bgo_enabled_cf == true)
        {
            break;
        }

        /* In blocking mode. Wait until FRDY is 1 unless timeout occurs. */
        err = flash_wait_frdy();
        if (err != FLASH_SUCCESS)
        {
            break;
        }

        /* Decrement to start address of next CF block */
        if (g_current_parameters.dest_addr <= (uint32_t) FLASH_CF_BLOCK_7)
        {
            g_current_parameters.dest_addr -= FLASH_CF_MEDIUM_BLOCK_SIZE;
        }
        else
        {
            g_current_parameters.dest_addr -= FLASH_CF_SMALL_BLOCK_SIZE;
        }
    }

    return err;
}
#endif

#if (FLASH_CFG_CODE_FLASH_ENABLE == 1)
#pragma section //end section FRAM
#endif

#endif  // FLASH_TYPE_3
