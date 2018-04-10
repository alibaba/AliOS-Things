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
* File Name : r_flash_type1.c
* Description  : This module implements the Flash API layer functions for the RX1xx and RX23x
********************************************************************************************************************/
/*******************************************************************************************************************
* History : DD.MM.YYYY Version Description
*           12.11.2014 1.10    Support for RX110, RX111, RX113
*                              Changed WAIT_MAX_ROM_WRITE to WAIT_MAX_ERASE_DF_1K in flash_api_erase().
*           19.12.2014 1.20    Replaced some equate names, modified for FLASH_TYPE_1, changed some wait_cnt settings.
*           12.01.2015 1.30    Updates for RX231
*           01.09.2015 1.40    Modified to use FLASH_NO_DATA_FLASH for RX23T and RX110.
*           13.11.2015 1.50    Added FLASH_CMD_CACHE_xxx for RX24T.
*           22.07.2016 2.00    Modified for BSPless flash.
*           17.11.2016 2.10    Added flash_stop() before issuing a FLASH_CMD_RESET.
*                              Added DF and CF block boundary check to flash_api_erase().
*                              Added CF block boundary check to FLASH_CMD_ACCESSWINDOW_SET.
*                              Added non-NULL argument check for FLASH_CMD_SWAPSTATE_SET and FLASH_CMD_SWAPSTATE_GET.
*                              Added valid SAS value check for FLASH_CMD_SWAPSTATE_SET.
*                              Added check for BUSY in flash_api_control() when in BGO mode.
*                              Added check in Open() for another operation in progress when in BGO mode.
*           18.11.2016 3.00    Removed functions common to other MCUs for new merged source code.
*           02.08.2017 3.10    Removed #include "r_mcu_config.h". Now in targets.h (r_flash_rx_if.h includes)
********************************************************************************************************************/

/********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
********************************************************************************************************************/
/* Includes board and MCU related header files. */
#include "r_flash_rx_if.h"
#if (FLASH_TYPE == FLASH_TYPE_1)
#include <machine.h>

/* Private header file for this package. */
#include "r_flash_type1_if.h"
#include "r_flash_common.h"
#include "r_flash_rx.h"
#include "r_dataflash.h"
#include "r_codeflash.h"
#include "r_codeflash_extra.h"
#include "r_flash_group.h"

/*********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

/*********************************************************************************************************************
 Typedef definitions
 *********************************************************************************************************************/

/***********************************************************************************************************************
 Private global variables and functions
***********************************************************************************************************************/

/*********************************************************************************************************************
 External variables and functions
 *********************************************************************************************************************/


#if (FLASH_CFG_CODE_FLASH_ENABLE == 1)
#pragma section FRAM
#endif

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


/*******************************************************************************
* Outline      : Intrinsic function to specify the number of loops
* Header       : none
* Function Name: r_df_delay
* Description  : Wait processing that loops at a fixed five cycles.
* Arguments    : R1 : Waiting loop counter
* Return Value : none
*******************************************************************************/
#pragma inline_asm r_flash_delay
static void r_flash_delay  (unsigned long loop_cnt)
{
    BRA     ?+
    NOP
?:
    NOP
    SUB     #01H,R1
    BNE     ?-

}

/*******************************************************************************
* Outline      : Function that specifies the execution time
* Header       : none
* Function Name: r_flash_delay_us
* Description  : The number of loops is calculated based on the execution time (Î¼s)
*              : and the sytem clock (ICLK) frequency, and the intrinsic function
*              : that specifies the number of loops is called.
* Arguments    : us  : Execution time
               : khz : ICLK frequency when calling the function
* Return Value : none
*******************************************************************************/
void r_flash_delay_us (unsigned long us, unsigned long khz)
{

    signed long loop_cnt; /* Argument of R_DELAY function */

    /* Calculation of a loop count */
    loop_cnt = us * khz;
    loop_cnt = (loop_cnt / WAIT_DIV_LOOP_CYCLE );      /* Division about cycle of 1 loop */
    loop_cnt = loop_cnt - WAIT_OVERHEAD_COUNT;         /* Overhead is reduced from a loop count. */

    /* R_DELAY function is performed when loop_cnt is 1 or more. */
    if(loop_cnt > 0)
    {
        r_flash_delay((unsigned long)loop_cnt);
    }
}

#if ((FLASH_CFG_CODE_FLASH_ENABLE == 1) && (FLASH_CFG_CODE_FLASH_BGO == 1)) || (FLASH_CFG_DATA_FLASH_BGO == 1)
/******************************************************************************
* Function Name: Excep_FCU_FRDYI
* Description  : ISR that is called when FCU is done with flash operation
*                NOTE: This function MUST execute from RAM only when
*                      FLASH_CFG_CODE_FLASH_BGO is enabled.
* Arguments    : none
* Return Value : none
******************************************************************************/
#pragma interrupt Excep_FCU_FRDYI(vect=VECT(FCU, FRDYI))
static void Excep_FCU_FRDYI(void)
{
    flash_err_t err = FLASH_SUCCESS;

#if ((FLASH_CFG_CODE_FLASH_ENABLE == 1) && (FLASH_CFG_CODE_FLASH_BGO == 1))

    if (FLASH_CUR_CF_BGO_ERASE  == g_current_parameters.current_operation)
    {
        err = R_CF_Erase_Check();
        if (FLASH_SUCCESS == err)
        {
            g_flash_int_ready_cb_args.event = FLASH_INT_EVENT_ERASE_COMPLETE;
        }
        else
        {
            g_flash_int_ready_cb_args.event = FLASH_INT_EVENT_ERR_FAILURE;
        }
    }
    else if (FLASH_CUR_CF_BGO_WRITE  == g_current_parameters.current_operation)
    {
        err = R_CF_Write_Check();
        if (FLASH_SUCCESS == err)
        {
            g_flash_int_ready_cb_args.event = FLASH_INT_EVENT_WRITE_COMPLETE;
        }
        else if ((FLASH_ERR_FAILURE == err) || (FLASH_ERR_TIMEOUT == err))
        {
            g_flash_int_ready_cb_args.event = FLASH_INT_EVENT_ERR_FAILURE;
        }
        else
        {
            /* Nothing to do */
        }
    }
    else if (FLASH_CUR_CF_BGO_BLANKCHECK == g_current_parameters.current_operation)
    {
        err = R_CF_BlankCheck_Check();
        if (FLASH_SUCCESS == err)
        {
            g_flash_int_ready_cb_args.event = FLASH_INT_EVENT_BLANK;
        }
        else
        {
            g_flash_int_ready_cb_args.event = FLASH_INT_EVENT_NOT_BLANK;
        }
    }
    else if (FLASH_CUR_CF_ACCESSWINDOW == g_current_parameters.current_operation)
    {
        err = r_cf_extra_check();
        if (FLASH_SUCCESS == err)
        {
            g_flash_int_ready_cb_args.event = FLASH_INT_EVENT_SET_ACCESSWINDOW;
        }
        else
        {
            g_flash_int_ready_cb_args.event = FLASH_INT_EVENT_ERR_FAILURE;
        }
    }
    else if (FLASH_CUR_CF_TOGGLE_STARTUPAREA == g_current_parameters.current_operation)
    {
        err = r_cf_extra_check();
        if (FLASH_SUCCESS == err)
        {
            g_flash_int_ready_cb_args.event = FLASH_INT_EVENT_TOGGLE_STARTUPAREA;
        }
        else
        {
            g_flash_int_ready_cb_args.event = FLASH_INT_EVENT_ERR_FAILURE;
        }
    }
    else
    {
        /* Nothing to do */
    }
#endif  // #if ((FLASH_CFG_CODE_FLASH_ENABLE == 1) && (FLASH_CFG_CODE_FLASH_BGO == 1))


#ifndef FLASH_NO_DATA_FLASH     /* RX110/23T has no DF */
#if (FLASH_CFG_DATA_FLASH_BGO == 1)
    if (FLASH_CUR_DF_BGO_ERASE == g_current_parameters.current_operation)
    {
        err = R_DF_Erase_Check();
        if (FLASH_SUCCESS == err)
        {
            g_flash_int_ready_cb_args.event = FLASH_INT_EVENT_ERASE_COMPLETE;
        }
        else
        {
            g_flash_int_ready_cb_args.event = FLASH_INT_EVENT_ERR_FAILURE;
        }
    }
    else if (FLASH_CUR_DF_BGO_WRITE  == g_current_parameters.current_operation)
    {
        err = R_DF_Write_Check();
        if (FLASH_SUCCESS == err)
        {
            g_flash_int_ready_cb_args.event = FLASH_INT_EVENT_WRITE_COMPLETE;
        }
        else if ((FLASH_ERR_FAILURE == err) || (FLASH_ERR_TIMEOUT == err))
        {
            g_flash_int_ready_cb_args.event = FLASH_INT_EVENT_ERR_FAILURE;
        }
        else
        {
            /* Nothing to do */
        }
    }
    else if (FLASH_CUR_DF_BGO_BLANKCHECK  == g_current_parameters.current_operation)
    {
        err = R_DF_BlankCheck_Check();
        if (FLASH_SUCCESS == err)
        {
            g_flash_int_ready_cb_args.event = FLASH_INT_EVENT_BLANK;
        }
        else
        {
            g_flash_int_ready_cb_args.event = FLASH_INT_EVENT_NOT_BLANK;
        }
    }
    else
    {
        /* Nothing to do */
    }
#endif  // FLASH_CFG_DATA_FLASH_BGO
#endif  // #ifndef FLASH_NO_DATA_FLASH

    if (FLASH_ERR_BUSY != err)
    {
        /* Release lock and Set current state to Idle */
        flash_pe_mode_exit();
        flash_release_state();
        g_current_parameters.current_operation = FLASH_CUR_IDLE;

        /* call back function execute */
        flash_ready_isr_handler((void *) &g_flash_int_ready_cb_args);
    }
}

#endif  // ((FLASH_CFG_CODE_FLASH_ENABLE == 1) && (FLASH_CFG_CODE_FLASH_BGO == 1)) || (FLASH_CFG_DATA_FLASH_BGO == 1)
#endif  // (FLASH_TYPE == FLASH_TYPE_1)

#pragma section /* end FLASH_SECTION_ROM */

/* end of file */
