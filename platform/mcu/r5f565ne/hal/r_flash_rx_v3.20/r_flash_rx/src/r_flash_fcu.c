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
* File Name    : r_flash_fcu.c
* Description  : This module implements functions common to Flash Types 3 and 4.
***********************************************************************************************************************/
/***********************************************************************************************************************
* History      : DD.MM.YYYY Version Description
*              : 05.10.2016 1.00    First Release
*              : 06.02.2017 1.10    Added support for RX65N-2M (bank/application swap).
*                                   Added code to clear ECC flag in flash_fcuram_codecopy();
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include <string.h>     // memcpy()
#include "r_flash_rx_if.h"
#include "r_flash_fcu.h"
#include "r_flash_group.h"
#if (FLASH_TYPE == FLASH_TYPE_3)
#include "r_flash_type3_if.h"
#endif

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
External functions
***********************************************************************************************************************/

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/

#ifdef FLASH_HAS_FCU

/***********************************************************************************************************************
 * Function Name: flash_init_fcu
 * Description  : This function sets the flash clock and copies FCU firmware to RAM
 *                (if supported).
 * Arguments    : none
 * Return Value : FLASH_SUCCESS -
 *                    Peripheral Initialized successfully.
 *                FLASH_ERR_TIMEOUT
 *                    Timed out trying to copy firmware to RAM
 ***********************************************************************************************************************/
flash_err_t flash_init_fcu(void)
{
    flash_err_t err = FLASH_SUCCESS;
    uint32_t fclk = MCU_CFG_FCLK_HZ;

    g_current_parameters.current_operation = FLASH_CUR_FCU_INIT;

    /* Allow Access to the Flash registers */
    FLASH.FWEPROR.BYTE = 0x01;

    /* Let the sequencer know what FCLK is running at */
    R_FLASH_Control(FLASH_CMD_CONFIG_CLOCK, &fclk);

    /* Copy the FCU firmware to FCU RAM */
#if (FLASH_TYPE == FLASH_TYPE_3)
    err = flash_fcuram_codecopy();
#endif
    return err;
}

#if (FLASH_TYPE == FLASH_TYPE_3)
/***********************************************************************************************************************
 * Function Name: flash_fcuram_codecopy
 * Description  : This function copies FCU firmware to the FCURAM.
 * Arguments    : none
 * Return Value : FLASH_SUCCESS -
 *                    Code copied successfully.
 *                FLASH_ERR_TIMEOUT
 *                  Timed out while attempting to switch to P/E mode or while trying to issue a STOP or
 *                   an ongoing flash operation timed out.
 *                FLASH_ERR_LOCKED
 *                 Switch to Read mode timed out and STOP was attempted to recover. Stop operation failed.
 *                 Peripheral in locked state.
 *                FLASH_ERR_FAILURE
 *                 Unable to enter PE mode
 ***********************************************************************************************************************/
flash_err_t flash_fcuram_codecopy(void)
{
    uint8_t * p_src;
    uint8_t * p_dest;
    volatile int32_t wait_cnt;      // Timeout counter.
    flash_err_t err = FLASH_SUCCESS;

    p_src = (uint8_t*) FCU_FIRMWARE_STORAGE_AREA;
    p_dest = (uint8_t*) FCU_RAM_AREA;

    if (FLASH.FENTRYR.WORD != 0x0000)
    {
        /*Enter Read Mode*/
        FLASH.FENTRYR.WORD = 0xAA00;

        /* Initialize timeout for FENTRYR being written. */
        wait_cnt = FLASH_FENTRYR_TIMEOUT;

        /* Read FENTRYR to ensure it has been set to 0. Note that the top byte
           of the FENTRYR register is not retained and is read as 0x00. */
        while (0x0000 != FLASH.FENTRYR.WORD)
        {
            /* Wait until FENTRYR is 0 unless timeout occurs. */
            if (wait_cnt-- <= 0)
            {
                return FLASH_ERR_TIMEOUT;
            }
        }
    }

    FLASH.FCURAME.WORD = 0xC403;      // FCURAM Access Enabled, High Speed Write Enabled

    /* Copy over the 4K from storage area to FCURAM */
    memcpy(p_dest, p_src, FCU_RAM_SIZE);

    FLASH.FCURAME.WORD = 0xC400;      // FCURAM Access Disabled


    /* Clear the ECC error flag in FCURAM */
    err = flash_pe_mode_enter(FLASH_TYPE_CODE_FLASH);
    if (err == FLASH_SUCCESS)
    {
        err = flash_stop();
        if (err == FLASH_SUCCESS)
        {
            err = flash_pe_mode_exit();
        }
    }

    return err;
}
#endif // FLASH_TYPE_3


#if (FLASH_CFG_CODE_FLASH_ENABLE == 1)
#pragma section FRAM
#endif


/***********************************************************************************************************************
 * Function Name: flash_reset
 * Description  : Function resets the Flash peripheral
 ** Arguments   : None
 * Return Value : FLASH_SUCCESS -
 *                Flash Peripheral successfully reset.
 * Notes     :   -This function will reset the peripheral by stopping any ongoing operations,
 *                clearing the DFAE and CFAE flags and changing the PE mode to Read mode.
 ***********************************************************************************************************************/
flash_err_t flash_reset(void)
{

    /* Cannot release sequencer from the command-locked state with status clear
     * or forced-stop commands if CFAE or DFAE is set. Must read those bits
     * before can set to 0.
     */
    if (FLASH.FASTAT.BIT.CFAE == 1)
    {
        FLASH.FASTAT.BIT.CFAE = 0;
    }
#ifndef FLASH_NO_DATA_FLASH
    if (FLASH.FASTAT.BIT.DFAE == 1)
    {
        FLASH.FASTAT.BIT.DFAE = 0;
    }
#endif

    /* Possible FLASH_CMD_RESET is called when no outstanding command is in progress.
     * In that case, enter pe mode so flash_stop() can write to the sequencer.
     */
    if (g_flash_state == FLASH_READY)
    {
        flash_pe_mode_enter(FLASH_TYPE_CODE_FLASH);
    }

    /*Issue a forced stop */
    flash_stop();

    /*Transition to Read mode*/
    FLASH.FENTRYR.WORD = 0xAA00;
    while (FLASH.FENTRYR.WORD != 0x0000)
        ;

    return FLASH_SUCCESS;
}


/***********************************************************************************************************************
 * Function Name: flash_stop
 * Description  : Function issue the STOP command and check the state of the Command Lock bit.
 * Arguments    : none
 * Return Value : FLASH_SUCCESS -
 *                    Stop issued successfully.
 *                FLASH_ERR_LOCKED
 *                    Peripheral in locked state
 ***********************************************************************************************************************/
flash_err_t flash_stop(void)
{

    g_current_parameters.current_operation = FLASH_CUR_STOP;

    /* Issue stop command to flash command area */
    *g_pfcu_cmd_area = (uint8_t) FLASH_FACI_CMD_FORCED_STOP;

    /* Wait for current operation to complete.*/
    while (1 != FLASH.FSTATR.BIT.FRDY)
    {
        ;
    }

    /* Check that fcu command lock bit is cleared */
    if (0 != FLASH.FASTAT.BIT.CMDLK)
    {
        return FLASH_ERR_CMD_LOCKED;
    }

    return FLASH_SUCCESS;
}


/***********************************************************************************************************************
 * Function Name: flash_pe_mode_enter
 * Description  : Function switches the peripheral to P/E mode for Code Flash or Data Flash.
 * Arguments    : flash_type_t : Specify Code Flash or Data Flash
 * Return Value : FLASH_SUCCESS -
 *                Switched successfully.
 *                FLASH_ERR_FAILURE -
 *                Unable to Switch to P/E Mode.
 *                FLASH_ERR_PARAM -
 *                Illegal parameter passed
 * NOTE         : This function must run from RAM if Code Flash non-BGO is used
 ***********************************************************************************************************************/
flash_err_t flash_pe_mode_enter(flash_type_t flash_type)
{
    flash_err_t err = FLASH_SUCCESS;

    if (flash_type == FLASH_TYPE_DATA_FLASH)
    {
        FLASH.FENTRYR.WORD = 0xAA80;        //Transition to DF P/E mode
#if (FLASH_TYPE == 4)
        while (FLASH.FENTRYR.WORD != 0x0080)
            ;
#endif

        if (FLASH.FENTRYR.WORD == 0x0080)
        {
            err = FLASH_SUCCESS;
        }
        else
        {
            err = FLASH_ERR_FAILURE;
        }
    }

    /* WARNING! Do not #if this out when CF is not enabled. Needed for flash_reset() on parts with no DF. */
    else if (flash_type == FLASH_TYPE_CODE_FLASH)
    {
        FLASH.FENTRYR.WORD = 0xAA01;            //Transition to CF P/E mode
#if (FLASH_TYPE == 4)
        while (FLASH.FENTRYR.WORD != 0x0001)
            ;
#endif

        if (FLASH.FENTRYR.WORD == 0x0001)
        {
            err = FLASH_SUCCESS;
#ifdef FLASH_HAS_SEQUENTIAL_CF_BLOCKS_LOCK
            if (g_lkbit_mode == FLASH_LOCKBIT_MODE_OVERRIDE)
            {
                FLASH.FPROTR.WORD = 0x5501;
            }
#endif
        }
        else
        {
            err = FLASH_ERR_FAILURE;
        }
    }
    else
    {
        err = FLASH_ERR_PARAM;
    }

    return err;
}


/***********************************************************************************************************************
 * Function Name: flash_pe_mode_exit
 * Description  : Function switches the peripheral from P/E mode for Code Flash or Data Flash to Read mode
 * Arguments    : None.
 * Return Value : FLASH_SUCCESS -
 *                    Switched successfully.
 *                FLASH_ERR_TIMEOUT -
 *                    Operation timed out. Ongoing flash operation failed.
 *                FLASH_ERR_LOCKED -
 *                    Peripheral in locked state. Operation failed.
 ***********************************************************************************************************************/
flash_err_t flash_pe_mode_exit(void)
{
    flash_err_t err = FLASH_SUCCESS;


    g_current_parameters.wait_cnt = FLASH_FRDY_CMD_TIMEOUT;

    err = flash_wait_frdy();

    /* Transition to Read mode */
    FLASH.FENTRYR.WORD = 0xAA00;
    while (FLASH.FENTRYR.WORD != 0x0000)
        ;


    return err;
}


/***********************************************************************************************************************
 * Function Name: flash_wait_frdy
 * Description  : Function
 * Arguments    : None.
 * Return Value : FLASH_SUCCESS -
 *                    Switched successfully.
 *                FLASH_ERR_TIMEOUT -
 *                    Operation timed out. Ongoing flash operation failed.
 *                FLASH_ERR_LOCKED -
 *                    Peripheral in locked state. Operation failed.
 ***********************************************************************************************************************/
flash_err_t flash_wait_frdy(void)
{
    flash_err_t err = FLASH_SUCCESS;
    
    
    while (1 != FLASH.FSTATR.BIT.FRDY)
    {
        if (g_current_parameters.wait_cnt-- <= 0)
        {
            flash_stop();
            err = FLASH_ERR_TIMEOUT;
        }
    }

    /* Check if Command Lock bit is set */
    if (err == FLASH_SUCCESS)
    {
        if (0 != FLASH.FASTAT.BIT.CMDLK)
        {
            err = get_cmdlk_err();
        }
    }

    return err;
}


/***********************************************************************************************************
* Function Name: flash_get_status
* Description  : Returns the current state of the flash
*                NOTE1: This function does not have to execute from in RAM. It must be in RAM though if
*                CF BGO is enabled and this function is called during a ROM P/E operation.
* Arguments    : none
* Return Value : FLASH_SUCCESS -
*                    Flash is ready to use
*                FLASH_ERR_BUSY -
*                    Flash is busy with another operation or is uninitialized
***********************************************************************************************************/
flash_err_t flash_get_status (void)
{
    /* Return flash status */
    if( g_flash_state == FLASH_READY )
    {
        return FLASH_SUCCESS;
    }
    else
    {
        return FLASH_ERR_BUSY;
    }
}


/***********************************************************************************************************************
 * Function Name: flash_erase
 * Description  : Function erases a block of Code or Data Flash
 * Arguments    : block address -
 *                    Block address to start erasing from
 *                num_blocks -
 *                    Number of blocks to erase in ascending order (descending addresses)
 * Return Value : FLASH_SUCCESS -
 *                    Block Erased successfully.
 *                FLASH_ERR_TIMEOUT -
 *                    Erase operation timed out. The function issued a STOP to reset the peripheral.
 *                FLASH_ERR_LOCKED -
 *                    Erase operation timed out and STOP was attempted to recover. Stop operation failed.
 *                    Peripheral in locked state.
 *                FLASH_ERR_LOCKBIT_SET -
 *                    Failed while trying to erase a block because lockbit was set and lockbit protection was enabled
 *                FLASH_ERR_FAILURE -
 *                    Erase operation failed for some other reason
 ***********************************************************************************************************************/
flash_err_t flash_erase(uint32_t block_address, uint32_t num_blocks)
{
#if (FLASH_CFG_CODE_FLASH_ENABLE == 1)
    uint32_t    size_boundary;
#endif
    flash_err_t err = FLASH_SUCCESS;


    /* Set Erasure Priority Mode */
    FLASH.FCPSR.WORD = 0x0001;

    /* LOOP THROUGH EACH BLOCK */
    for (g_current_parameters.current_count = 0;
      g_current_parameters.current_count < g_current_parameters.total_count;
      g_current_parameters.current_count++)
    {
        /* Init flash register and write fcu erase block command */
        FLASH.FSADDR.LONG = g_current_parameters.dest_addr;
        *g_pfcu_cmd_area = (uint8_t) FLASH_FACI_CMD_BLOCK_ERASE;
        *g_pfcu_cmd_area = (uint8_t) FLASH_FACI_CMD_FINAL;

        /* Return if in BGO mode. Processing will finish in FRDYI interrupt */
        if ((g_current_parameters.current_operation == FLASH_CUR_CF_BGO_ERASE)
         || (g_current_parameters.current_operation == FLASH_CUR_DF_BGO_ERASE))
        {
            break;
        }

        /* In blocking mode, wait for FRDY or timeout. Return if error. */
        err = flash_wait_frdy();
        if (err != FLASH_SUCCESS)
        {
            break;
        }

        /* Adjust timeout and address for next block */
        if (FLASH.FENTRYR.WORD == 0x0080)
        {
#ifndef FLASH_NO_DATA_FLASH
            /* DATA FLASH */
            g_current_parameters.dest_addr += FLASH_DF_BLOCK_SIZE;
            g_current_parameters.wait_cnt = WAIT_MAX_ERASE_DF;
#endif
        }
        else // (FLASH.FENTRYR.WORD == 0x0001)
        {
#if (FLASH_CFG_CODE_FLASH_ENABLE == 1)
            size_boundary = (uint32_t)FLASH_CF_BLOCK_7;     // for linear and hi-bank dual mode
#ifdef FLASH_IN_DUAL_BANK_MODE
            if (g_current_parameters.dest_addr <= (uint32_t)FLASH_CF_LO_BANK_HI_ADDR)
            {
                size_boundary = (uint32_t)FLASH_CF_BLOCK_45;
            }
#endif

            /* CODE FLASH */
            if (g_current_parameters.dest_addr <= size_boundary)
            {
                g_current_parameters.dest_addr -= FLASH_CF_MEDIUM_BLOCK_SIZE;
                g_current_parameters.wait_cnt = WAIT_MAX_ERASE_CF_32K;
            }
            else
            {
                g_current_parameters.dest_addr -= FLASH_CF_SMALL_BLOCK_SIZE;
                g_current_parameters.wait_cnt = WAIT_MAX_ERASE_CF_8K;
            }
#endif
        }
    }

    return err;
}

#ifndef FLASH_NO_BLANK_CHECK
/***********************************************************************************************************************
 * Function Name: flash_blankcheck
 * Description  : Function checks if the specified Data Flash address range is blank
 * Arguments    : address -
 *                    Start address to perform blank check. Actual address or entry from "flash_block_address_t" enum can be used
 *                num_bytes -
 *                    Number of bytes to perform blank check operation for.
 *                *result
 *                    Set to FLASH_RES_NOT_BLANK or FLASH_RES_BLANK.
 * Return Value : FLASH_SUCCESS -
 *                    Operation completed (blocking) or started successfully (BGO)
 *                FLASH_ERR_TIMEOUT -
 *                    Operation timed out. The function issued a STOP to reset the peripheral.
 *                FLASH_ERR_LOCKED -
 *                    Peripheral in locked state.
 *                FLASH_ERR_FAILURE -
 *                    Operation failure
 ***********************************************************************************************************************/
flash_err_t flash_blankcheck(uint32_t start_address, uint32_t num_bytes, flash_res_t *result)
{
    flash_err_t err = FLASH_SUCCESS;

    /* Init flash registers and write fcu command */
    FLASH.FBCCNT.BYTE = 0x00;       // set mode to incremental
    FLASH.FSADDR.LONG = start_address;
    FLASH.FEADDR.LONG = (start_address + num_bytes - 1);
    *g_pfcu_cmd_area = (uint8_t) FLASH_FACI_CMD_BLANK_CHECK;
    *g_pfcu_cmd_area = (uint8_t) FLASH_FACI_CMD_FINAL;

    /* In blocking mode, wait for FRDY or timeout */
    if (g_current_parameters.bgo_enabled_df == false)
    {
        err = flash_wait_frdy();
        if (err == FLASH_SUCCESS)
        {
            /* Load blank status */
            if (FLASH.FBCSTAT.BYTE == 0x01)
            {
                *result = FLASH_RES_NOT_BLANK;
            }
            else
            {
                *result = FLASH_RES_BLANK;
            }
        }
    }

    return err;
}
#endif // #ifndef FLASH_NO_BLANK_CHECK


/***********************************************************************************************************************
 * Function Name: flash_write
 * Description  : This function writes to Code or Data Flash
 * Arguments    : src_address -
 *                    Source buffer address.
 *                dest_address -
 *                    Destination address.
 *                num_bytes
 *                    Number of bytes to be written
 * Return Value : FLASH_SUCCESS -
 *                    Write completed successfully; successfully initialized in case of BGO mode.
 *                FLASH_ERR_BYTES -
 *                    Number of bytes exceeds max range or is 0 or is not a valid multiple of the minimum programming
 *                    size for the specified flash
 *                FLASH_ERR_ADDRESS -
 *                    src/dest address is an invalid Code/Data Flash block start address or
 *                    address is not aligned with the minimum programming size for the Code/Data Flash
 *                    For Code Flash programming the src address HAS to be a RAM address since ROM cannot be accessed
 *                    during Code Flash programming
 *                FLASH_ERR_BUSY -
 *                    Flash peripheral is busy with another operation or not initialized
 *                FLASH_ERR_CMD_LOCKED -
 *                    The FCU entered a command locked state and cannot process the operation.
 *                    A RESET was performed on the FCU to recover from this state.
 *                FLASH_ERR_LOCKBIT_SET
 *                    Operation failed because the lockbit was set.
 *                    A RESET was performed on the FCU to recover from this state.
 *                FLASH_ERR_FAILURE
 *                    Code Flash Write operation attempted in BGO mode. This is temporarily not supported
 *                    Operation failed for some other reason; RESET was performed on the FCU to recover from this state.
 ***********************************************************************************************************************/
flash_err_t flash_write(uint32_t src_start_address,
                        uint32_t dest_start_address,
                        uint32_t num_bytes)
{
    flash_err_t err = FLASH_SUCCESS;
    uint32_t wait_cnt;


    g_current_parameters.total_count = (num_bytes) >> 1;    // two bytes written at a time
    g_current_parameters.dest_addr = dest_start_address;
    g_current_parameters.src_addr = src_start_address;
    g_current_parameters.current_count = 0;
    if (g_current_parameters.current_operation == FLASH_CUR_CF_WRITE)
    {
        wait_cnt = WAIT_MAX_ROM_WRITE;
    }
#ifndef FLASH_NO_DATA_FLASH
    else
    {
        wait_cnt = WAIT_MAX_DF_WRITE;
    }
#endif


    /* TOTAL NUMBER OF BYTES TO WRITE LOOP */
    while (g_current_parameters.total_count > 0)
    {
        /* Setup fcu command */
        FLASH.FSADDR.LONG = g_current_parameters.dest_addr;
        *g_pfcu_cmd_area = (uint8_t) FLASH_FACI_CMD_PROGRAM;
        *g_pfcu_cmd_area = (uint8_t) g_current_parameters.fcu_min_write_cnt;

        /* MINIMUM FLASH WRITE SIZE LOOP (2 BYTES AT A TIME DUE TO FCU BUFFER SIZE) */
        while (g_current_parameters.current_count++ < g_current_parameters.fcu_min_write_cnt)
        {
            /* Copy data from source address to destination area */
            *(FCU_WORD_PTR) g_pfcu_cmd_area = *(uint16_t *) g_current_parameters.src_addr;

            while (FLASH.FSTATR.BIT.DBFULL == 1)    // wait for fcu buffer to empty
                ;

            g_current_parameters.src_addr += 2;
            g_current_parameters.dest_addr += 2;
            g_current_parameters.total_count--;
        }

        /* Issue write end command */
        *g_pfcu_cmd_area = (uint8_t) FLASH_FACI_CMD_FINAL;

        /* Reset fcu write count */
        g_current_parameters.current_count = 0;

        /* Return if in BGO mode. Processing will finish in FRDYI interrupt */
        if ((g_current_parameters.current_operation == FLASH_CUR_CF_BGO_WRITE)
         || (g_current_parameters.current_operation == FLASH_CUR_DF_BGO_WRITE))
        {
            break;
        }

        /* In blocking mode, wait for FRDY or timeout. Return if error. */
        err = flash_wait_frdy();
        if (err != FLASH_SUCCESS)
        {
            break;
        }

        /* Reload timeout for next minimum flash write size */
        g_current_parameters.wait_cnt = wait_cnt;
    }

  return err;
}

#if (FLASH_CFG_CODE_FLASH_BGO || FLASH_CFG_DATA_FLASH_BGO)
/***********************************************************************************************************************
 * Function Name: Excep_FCU_FRDYI
 * Description  : ISR for Flash Ready Interrupt
 * Arguments    : none
 * Return Value : none
 ***********************************************************************************************************************/
#pragma interrupt Excep_FCU_FRDYI(vect=VECT(FCU,FRDYI))
static void Excep_FCU_FRDYI(void)
{
#if (FLASH_CFG_CODE_FLASH_ENABLE == 1)
    uint32_t    size_boundary;
#endif

    if ((FLASH_CUR_DF_BGO_WRITE == g_current_parameters.current_operation)
     || (FLASH_CUR_CF_BGO_WRITE == g_current_parameters.current_operation))
    {
        /* If there are still bytes to write */
        if (g_current_parameters.total_count > 0)
        {
            /* Setup fcu command */
            FLASH.FSADDR.LONG = g_current_parameters.dest_addr;
            *g_pfcu_cmd_area = (uint8_t) FLASH_FACI_CMD_PROGRAM;
            *g_pfcu_cmd_area = (uint8_t) g_current_parameters.fcu_min_write_cnt;

            /* MINIMUM FLASH WRITE SIZE LOOP (2 BYTES AT A TIME DUE TO FCU BUFFER SIZE) */
            while (g_current_parameters.current_count++ < g_current_parameters.fcu_min_write_cnt)
            {
                /* Copy data from source address to destination area */
                *(FCU_WORD_PTR) g_pfcu_cmd_area =   *(uint16_t *) g_current_parameters.src_addr;

                while (FLASH.FSTATR.BIT.DBFULL == 1)    // wait for fcu buffer to empty
                    ;

                g_current_parameters.src_addr += 2;
                g_current_parameters.dest_addr += 2;
                g_current_parameters.total_count--;
            }

            /* Issue write end command */
            *g_pfcu_cmd_area = (uint8_t) FLASH_FACI_CMD_FINAL;

            /* Reset fcu write count */
            g_current_parameters.current_count = 0;

            /* Exit ISR until next FRDY interrupt to continue operation (write next min size) */
            return;
        }
        /* Done writing all bytes */
        else
        {
            g_flash_int_ready_cb_args.event = FLASH_INT_EVENT_WRITE_COMPLETE;
        }
    }


    else if ((FLASH_CUR_DF_BGO_ERASE == g_current_parameters.current_operation)
     || (FLASH_CUR_CF_BGO_ERASE == g_current_parameters.current_operation))
    {
        g_current_parameters.current_count++;
        if (g_current_parameters.current_count < g_current_parameters.total_count)
        {
            /* Increment to next block address */
#ifndef FLASH_NO_DATA_FLASH
            /* DATA FLASH */
            if (FLASH.FENTRYR.WORD == 0x0080)
            {
                g_current_parameters.dest_addr += FLASH_DF_BLOCK_SIZE;
            }
#endif

#if (FLASH_CFG_CODE_FLASH_ENABLE == 1)
            /* CODE FLASH */
            if (FLASH.FENTRYR.WORD == 0x0001)
            {
                size_boundary = (uint32_t)FLASH_CF_BLOCK_7;     // for linear and hi-bank dual mode
#ifdef FLASH_IN_DUAL_BANK_MODE
                if (g_current_parameters.dest_addr <= (uint32_t)FLASH_CF_LO_BANK_HI_ADDR)
                {
                    size_boundary = (uint32_t)FLASH_CF_BLOCK_45;
                }
#endif
                if (g_current_parameters.dest_addr <= size_boundary)
                {
                    g_current_parameters.dest_addr -= FLASH_CF_MEDIUM_BLOCK_SIZE;
                }
                else
                {
                    g_current_parameters.dest_addr -= FLASH_CF_SMALL_BLOCK_SIZE;
                }
            }
#endif

            /* Init flash register and write fcu erase block command */
            FLASH.FSADDR.LONG = g_current_parameters.dest_addr;
            *g_pfcu_cmd_area = (uint8_t) FLASH_FACI_CMD_BLOCK_ERASE;
            *g_pfcu_cmd_area = (uint8_t) FLASH_FACI_CMD_FINAL;

            /* Exit ISR until next FRDY interrupt to continue operation (erase next block) */
            return;
        }
        /* Done erasing all blocks */
        else
        {
            g_flash_int_ready_cb_args.event = FLASH_INT_EVENT_ERASE_COMPLETE;
        }
    }


#ifndef FLASH_NO_BLANK_CHECK
    else if (FLASH_CUR_DF_BGO_BLANKCHECK == g_current_parameters.current_operation)
    {
        if (FLASH.FBCSTAT.BYTE == 0x01)
        {
            g_flash_int_ready_cb_args.event = FLASH_INT_EVENT_NOT_BLANK;
        }
        else
        {
            g_flash_int_ready_cb_args.event = FLASH_INT_EVENT_BLANK;
        }
    }
#endif

#if (FLASH_HAS_SEQUENTIAL_CF_BLOCKS_LOCK && FLASH_CFG_CODE_FLASH_ENABLE)
    else if (FLASH_CUR_CF_BGO_READ_LOCKBIT == g_current_parameters.current_operation)
    {
        if (FLASH.FLKSTAT.BIT.FLOCKST == 1)
        {
            g_flash_int_ready_cb_args.event = FLASH_INT_EVENT_LOCKBIT_NON_PROTECTED;
        }
        else
        {
            g_flash_int_ready_cb_args.event = FLASH_INT_EVENT_LOCKBIT_PROTECTED;
        }
    }
    else if (FLASH_CUR_CF_BGO_WRITE_LOCKBIT == g_current_parameters.current_operation)
    {
        if (0 == FLASH.FASTAT.BIT.CMDLK)    // last block lock successful
        {
            while (++g_current_parameters.current_count < g_current_parameters.total_count)
            {
                /* Decrement to start address of next CF block */
                if (g_current_parameters.dest_addr <= (uint32_t) FLASH_CF_BLOCK_7)
                {
                    g_current_parameters.dest_addr -= FLASH_CF_MEDIUM_BLOCK_SIZE;
                }
                else
                {
                    g_current_parameters.dest_addr -= FLASH_CF_SMALL_BLOCK_SIZE;
                }

                /* Init flash registers and write fcu command */
                FLASH.FSADDR.LONG = g_current_parameters.dest_addr;
                *g_pfcu_cmd_area = (uint8_t) FLASH_FACI_CMD_LOCK_BIT_PGM;
                *g_pfcu_cmd_area = (uint8_t) FLASH_FACI_CMD_FINAL;

                /* Exit ISR until next FRDY interrupt to continue operation (lock next block) */
                return;
            }

            g_flash_int_ready_cb_args.event = FLASH_INT_EVENT_LOCKBIT_WRITTEN;
        }
        else /* writing lockbit failed */
        {
            flash_reset();
            g_flash_int_error_cb_args.event = FLASH_INT_EVENT_ERR_FAILURE;
        }
    }
#endif

#if (FLASH_HAS_CF_ACCESS_WINDOW && FLASH_CFG_CODE_FLASH_ENABLE)
    else if (FLASH_CUR_CF_ACCESSWINDOW == g_current_parameters.current_operation)
    {
        g_flash_int_ready_cb_args.event = FLASH_INT_EVENT_SET_ACCESSWINDOW;
    }
#endif

#if (FLASH_HAS_BOOT_SWAP && FLASH_CFG_CODE_FLASH_ENABLE)
    else if (FLASH_CUR_CF_TOGGLE_STARTUPAREA == g_current_parameters.current_operation)
    {
        g_flash_int_ready_cb_args.event = FLASH_INT_EVENT_TOGGLE_STARTUPAREA;
    }
#endif

#if (FLASH_HAS_APP_SWAP && FLASH_CFG_CODE_FLASH_ENABLE)
    else if (FLASH_CUR_CF_TOGGLE_BANK == g_current_parameters.current_operation)
    {
        g_flash_int_ready_cb_args.event = FLASH_INT_EVENT_TOGGLE_BANK;
    }
#endif


    if (g_current_parameters.current_operation != FLASH_CUR_STOP)
    {
        flash_pe_mode_exit();
        flash_release_state();

        if ((FIT_NO_FUNC != flash_ready_isr_handler)
         && (NULL != flash_ready_isr_handler))
        {
            flash_ready_isr_handler((void *) &g_flash_int_ready_cb_args);
        }
    }
}


/***********************************************************************************************************************
 * Function Name: Excep_FCU_FIFERR
 * Description  : ISR for Flash Error Interrupt
 * Arguments    : none
 * Return Value : none
 ***********************************************************************************************************************/
#pragma interrupt Excep_FCU_FIFERR(vect=VECT(FCU,FIFERR))
static void Excep_FCU_FIFERR(void)
{
    /* Check if Command Lock bit is set */
    if (1 == FLASH.FASTAT.BIT.CMDLK)
    {
        g_flash_int_error_cb_args.event = get_cmdlk_err_event();
    }

    /* Reset the FCU: This will stop any existing processes and exit PE mode */
    flash_reset();

    IR(FCU,FRDYI)= 0;  //Clear any pending Flash Ready interrupt request

    flash_release_state();

    if ((FIT_NO_FUNC != flash_error_isr_handler)
     && (NULL != flash_error_isr_handler))
    {
        flash_error_isr_handler((void *) &g_flash_int_error_cb_args);
    }
}

#endif // (FLASH_CFG_CODE_FLASH_BGO || FLASH_CFG_DATA_FLASH_BGO)

#if (FLASH_CFG_CODE_FLASH_ENABLE == 1)
#pragma section
#endif

#endif // FLASH_HAS_FCU
