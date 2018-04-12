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
* File Name : r_flash_type4.c
* Description  : This module implements functions specific to Flash Type 4 MCUs
********************************************************************************************************************/
/*******************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 11.02.2016 1.00    Initial version
*         : 12.08.2016 2.00    Modified for BSPless operation
*         : 17.11.2016 2.10    Fixed flash_reset() so enters PE mode before issuing a flash_stop()
*                                when in idle state.
*         : 18.11.2016 3.00    Merged functions common to other flash types into r_flash_fcu.c and r_flash_group.c.
*           06.02.2017 3.10    Added support for RX65N-2M (bank/application swap).
*           11.08.2017 3.20    Modified flash_toggle_banksel_reg() and flash_write_faw_reg() so always poll
*                                for FRDY even when in BGO/interrupt mode.
********************************************************************************************************************/

/********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
********************************************************************************************************************/
#include "r_flash_rx_if.h"
#if (FLASH_TYPE == FLASH_TYPE_4)
#include "r_flash_rx.h"
#include "r_flash_fcu.h"
#include "r_flash_group.h"
#include "r_flash_type4_if.h"

/******************************************************************************
Typedef definitions
******************************************************************************/

/***********************************************************************************************************************
 Macro definitions
 ***********************************************************************************************************************/


/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/
#if (FLASH_CFG_CODE_FLASH_ENABLE == 1)

static flash_err_t flash_write_faw_reg(fawreg_t faw);


#if (FLASH_IN_DUAL_BANK_MODE)
#pragma section FRAM2


flash_err_t flash_toggle_banksel_reg()
{
    flash_err_t err = FLASH_SUCCESS;
    uint32_t    banksel_val;


    err = flash_pe_mode_enter(FLASH_TYPE_CODE_FLASH);
    if (FLASH_SUCCESS != err)
    {
        return(err);
    }

    g_current_parameters.current_operation = FLASH_CUR_CF_TOGGLE_BANK;


    /* Read and toggle bank select value */
    banksel_val = *((uint32_t *)BANKSEL_ADDR);
    banksel_val ^= BANKSWP_MASK;

#ifdef __BIG    // Big endian - swap words
    uint32_t    swapped;
    swapped = (banksel_val << 16) | ((banksel_val >> 16) & 0x0000FFFF);
    banksel_val = swapped;
#endif

    /* Write new value */
    /* See table "Address Used by Configuration Set Command" in Flash Memory Manual */
    FLASH.FSADDR.BIT.FSADDR = 0x00FF5D20;       // FSADDR reg specific addr for BANKSEL register
    *g_pfcu_cmd_area = (uint8_t) 0x40;          // Control area command
    *g_pfcu_cmd_area = (uint8_t) 0x08;          // 8 words

    *((uint16_t *)g_pfcu_cmd_area) = (uint16_t) (banksel_val & 0x000FFFF);         // data for 0x00FF5D20
    *((uint16_t *)g_pfcu_cmd_area) = (uint16_t) ((banksel_val >> 16) & 0x000FFFF); // data for 0x00FF5D22
    *((uint16_t *)g_pfcu_cmd_area) = 0xFFFF;    // data for 0x00FF5D24
    *((uint16_t *)g_pfcu_cmd_area) = 0xFFFF;    // data for 0x00FF5D26
    *((uint16_t *)g_pfcu_cmd_area) = 0xFFFF;    // data for 0x00FF5D28
    *((uint16_t *)g_pfcu_cmd_area) = 0xFFFF;    // data for 0x00FF5D2A
    *((uint16_t *)g_pfcu_cmd_area) = 0xFFFF;    // data for 0x00FF5D2C
    *((uint16_t *)g_pfcu_cmd_area) = 0xFFFF;    // data for 0x00FF5D2E
    *g_pfcu_cmd_area = (uint8_t) 0xD0;          // end command

    /* NOTE: Always poll for completion even when in BGO/interrupt mode. This is
     * because the banksel register is "tied" to bank 0. We do not want to return
     * from this function (which is executing in RAM) until the operation
     * completes. Otherwise, if the app happens to be executing in bank 0, the
     * illegal situation in effect arises of writing to the same bank that you
     * are executing from.
     */
    g_current_parameters.wait_cnt = FLASH_FRDY_CMD_TIMEOUT;
    while (1 != FLASH.FSTATR.BIT.FRDY)
    {
        /* Wait until FRDY is 1 unless timeout occurs. */
        if (g_current_parameters.wait_cnt-- <= 0)
        {
            /* if FRDY is not set to 1 after max timeout, issue the stop command*/
            err = flash_stop();
            return err;
        }
    }

    flash_pe_mode_exit();

    return err;
}

#endif // FLASH_IN_DUAL_BANK_MODE

#pragma section FRAM
#endif // #if (FLASH_CFG_CODE_FLASH_ENABLE == 1)

/***********************************************************************************************************************
* Function Name: get_cmdlk_err
* Description  : This function identifies the error when FLASH.FASTAT.BIT.CMDLK is set.
* Arguments    : None
* Return Value : FLASH_ERR_SECURITY -
*                    Operation failed due to access window protection
*                FLASH_ERR_FAILURE -
*                    Failure caused by something other than lockbit set
*                FLASH_ERR_CMD_LOCKED -
*                    Flash hardware locked
***********************************************************************************************************************/
flash_err_t get_cmdlk_err(void)
{
    flash_err_t err;


    if ((FLASH.FASTAT.BIT.CFAE) || (FLASH.FSTATR.BIT.ILGLERR))
    {
        err = FLASH_ERR_ADDRESS;            // tried to access invalid address
    }
    else if (FLASH.FSTATR.BIT.SECERR)       // error due to access window locking
    {
        err = FLASH_ERR_SECURITY;
    }
    else if ((FLASH.FSTATR.BIT.PRGERR) || (FLASH.FSTATR.BIT.ERSERR))
    {
        err = FLASH_ERR_FAILURE;
    }
    else
    {
        err = FLASH_ERR_CMD_LOCKED;
    }

    /* Recovery procedure from CMDLK is to issue a forced stop */
    flash_stop();
    return err;
}


#if (FLASH_CFG_DATA_FLASH_BGO || FLASH_CFG_CODE_FLASH_BGO)

/***********************************************************************************************************************
* Function Name: get_cmdlk_err_event
* Description  : This function identifies the error when FLASH.FASTAT.BIT.CMDLK is set and converts it to
*                an interrupt event for the callback function.
* Arguments    : None
* Return Value : FLASH_INT_EVENT_ERR_DF_ACCESS -
*                    Tried to access invalid data flash address
*                FLASH_INT_EVENT_ERR_CF_ACCESS -
*                    Tried to access invalid code falsh address
*                FLASH_INT_EVENT_ERR_SECURITY -
*                    Operation failed due to access window protection
*                FLASH_INT_EVENT_ERR_FAILURE -
*                    General write or erase failure.
*                FLASH_INT_EVENT_ERR_CMD_LOCKED -
*                    Flash peripheral hardware locked
***********************************************************************************************************************/
flash_interrupt_event_t get_cmdlk_err_event(void)
{
    flash_interrupt_event_t event;

#ifndef FLASH_NO_DATA_FLASH
    if (FLASH.FASTAT.BIT.DFAE)
    {
        event = FLASH_INT_EVENT_ERR_DF_ACCESS;  // tried to access invalid address
    }
#endif
    if ((FLASH.FASTAT.BIT.CFAE) || (FLASH.FSTATR.BIT.ILGLERR))
    {
        event = FLASH_INT_EVENT_ERR_CF_ACCESS;  // tried to access invalid address
    }
    else if (FLASH.FSTATR.BIT.SECERR)           // access window protected
    {
        event = FLASH_INT_EVENT_ERR_SECURITY;
    }
    else if ((FLASH.FSTATR.BIT.PRGERR) || (FLASH.FSTATR.BIT.ERSERR))
    {
        event = FLASH_INT_EVENT_ERR_FAILURE;
    }
    else
    {
        event = FLASH_INT_EVENT_ERR_CMD_LOCKED;
    }

    /* Recovery procedure from CMDLK is to issue a forced stop */
    flash_stop();
    return event;
}
#endif // (FLASH_CFG_DATA_FLASH_BGO || FLASH_CFG_CODE_FLASH_BGO)


#if (FLASH_CFG_CODE_FLASH_ENABLE == 1)
/*******************************************************************************
* Function Name: R_CF_GetCurrentStartupArea
* Description  : Return which startup area (Default or Alternate) is active
*                at reset.
* Arguments    : None
* Return Value : startup_area_flag - 0 ==> Alternate area
*                                    1 ==> Default area
*******************************************************************************/
uint8_t R_CF_GetCurrentStartupArea(void)
{
    fawreg_t    faw;

    faw.LONG = FLASH.FAWMON.LONG;

    return(faw.BIT.BTFLG);
}


/*******************************************************************************
* Function Name: R_CF_ToggleStartupArea
* Description  : Flips rhe startup area effective at reset.
* Arguments    : None
* Return Value : FLASH_SUCCESS -
*                   Switched successfully.
*                FLASH_ERR_FAILURE -
*                   PE mode enter/exit failed
*******************************************************************************/
flash_err_t R_CF_ToggleStartupArea (void)
{
    flash_err_t err;
    fawreg_t    faw;

    g_current_parameters.current_operation = FLASH_CUR_CF_TOGGLE_STARTUPAREA;

    faw.LONG = FLASH.FAWMON.LONG;

    faw.BIT.BTFLG ^= 1;

    err = flash_write_faw_reg(faw);

    return err;
}


/*******************************************************************************
* Function Name: R_CF_GetCurrentSwapState
* Description  : Return which startup area active at the moment (not guaranteed
*                through reset)
* Arguments    : None
* Return Value : FLASH_SAS_SWAPFLG -
*                    The start-up area temporarily set according to the swap flag
*                FLASH_SAS_DEFAULT -
*                    The start-up area temporarily set to the default area
*                FLASH_SAS_ALTERNATE -
*                    The start-up area temporarily set to the alternate area
*******************************************************************************/
uint8_t R_CF_GetCurrentSwapState(void)
{

    return(FLASH.FSUACR.BIT.SAS);
}


/*******************************************************************************
* Function Name: R_CF_SetCurrentSwapState
* Description  : Temporarily set the start-up area (does not determine area at reset)
* Arguments    : value -
*                    FLASH_SAS_SWAPFLG (start-up area temporarily set according to the swap flag)
*                    FLASH_SAS_DEFAULT (start-up area temporarily set to the default area)
*                    FLASH_SAS_ALTERNATE (start-up area temporarily set to the alternate area)
*                    FLASH_SAS_SWITCH_AREA (Command to temporarily switch to the other startup area)
* Return Value : None
*******************************************************************************/
void R_CF_SetCurrentSwapState(uint8_t value)
{
    uint8_t     sas_flag;
    uint16_t    reg_value;


    if (FLASH_SAS_SWITCH_AREA == value)                     // switch startup areas
    {
        if (FLASH_SAS_SWAPFLG == FLASH.FSUACR.BIT.SAS)      // switch based upon FAWMON.BTFLG reset area
        {
            if (1 == FLASH.FAWMON.BIT.BTFLG)                // 1 = using default area
            {
                sas_flag = FLASH_SAS_ALTERNATE;
            }
            else
            {
                sas_flag = FLASH_SAS_DEFAULT;
            }
        }
        else                                                // switch based upon current state
        {
            if (FLASH_SAS_ALTERNATE == FLASH.FSUACR.BIT.SAS)
            {
                sas_flag = FLASH_SAS_DEFAULT;
            }
            else
            {
                sas_flag = FLASH_SAS_ALTERNATE;
            }
        }
    }
    else
    {
        sas_flag = value;       /* to set SAS to desired area */
    }

    reg_value = 0x6600 | (uint16_t)sas_flag;
    FLASH.FSUACR.WORD = reg_value;


    while(sas_flag != FLASH.FSUACR.BIT.SAS)
    {
        /* Confirm that the written value can be read correctly. */
    }

}


/*******************************************************************************
* Function Name: R_CF_SetAccessWindow
* Description  : Specifies the setting for the access window.
* Arguments    : pAccessInfo->start_addr -
*                   start address of Access Window Setting
*              : pAccessInfo->end_addr -
*                   end address of Access Window Setting. This should be one
*                   beyond the actual last byte to allow write access for.
*
* Return Value : FLASH_SUCCESS -
*                    Command executed successfully
*                FLASH_ERR_ADDRESS -
*                    Invalid address argument
*                FLASH_ERR_FAILURE -
*                   PE mode enter/exit failed
*******************************************************************************/
flash_err_t R_CF_SetAccessWindow (flash_access_window_config_t  *pAccessInfo)
{
    flash_err_t err = FLASH_SUCCESS;
    fawreg_t    faw;


    g_current_parameters.current_operation = FLASH_CUR_CF_ACCESSWINDOW;

    faw.LONG = FLASH.FAWMON.LONG;

    faw.BIT.FAWS = (pAccessInfo->start_addr & 0x00FFE000) >> 13;
    faw.BIT.FAWE = (pAccessInfo->end_addr & 0x00FFE000) >> 13;

    err = flash_write_faw_reg(faw);

    return err;
}


/*******************************************************************************
* Function Name: flash_write_faw_reg
* Description  : Writes the contents of the argument to the FAW register.
* Arguments    : start_addr : start address of Access Window Setting
*              : end_addr   : end address of Access Window Setting. This should be one
*                             beyond the actual last byte to allow write access for.
*                             here as required by the spec.
* Return Value : FLASH_SUCCESS -
*                    Command executed successfully
*                FLASH_ERR_FAILURE -
*                   PE mode enter/exit failed
*******************************************************************************/
flash_err_t flash_write_faw_reg (fawreg_t   faw)
{
    flash_err_t err = FLASH_SUCCESS;

    err = flash_pe_mode_enter(FLASH_TYPE_CODE_FLASH);
    if (FLASH_SUCCESS != err)
    {
        return(err);
    }

    faw.BIT.FSPR = 1;          // p/e enabled (allow rewrite of flash; 0 locks chip forever)

#ifdef __BIG    // Big endian - swap words
    uint32_t    swapped;
    swapped = (faw.LONG << 16) | ((faw.LONG >> 16) & 0x0000FFFF);
    faw.LONG = swapped;
#endif

    /* See table "Address Used by Configuration Set Command" in Flash Memory Manual */
    FLASH.FSADDR.BIT.FSADDR = 0x00FF5D60;       // FSADDR reg specific addr for FAW register
    *g_pfcu_cmd_area = (uint8_t) 0x40;          // Control area command
    *g_pfcu_cmd_area = (uint8_t) 0x08;          // 8 words

    *((uint16_t *)g_pfcu_cmd_area) = 0xFFFF;    // data for 0x00FF5D60
    *((uint16_t *)g_pfcu_cmd_area) = 0xFFFF;    // data for 0x00FF5D62
    *((uint16_t *)g_pfcu_cmd_area) = (uint16_t) (faw.LONG & 0x000FFFF);         // data for 0x00FF5D64
    *((uint16_t *)g_pfcu_cmd_area) = (uint16_t) ((faw.LONG >> 16) & 0x000FFFF); // data for 0x00FF5D66
    *((uint16_t *)g_pfcu_cmd_area) = 0xFFFF;    // data for 0x00FF5D68
    *((uint16_t *)g_pfcu_cmd_area) = 0xFFFF;    // data for 0x00FF5D6A
    *((uint16_t *)g_pfcu_cmd_area) = 0xFFFF;    // data for 0x00FF5D6C
    *((uint16_t *)g_pfcu_cmd_area) = 0xFFFF;    // data for 0x00FF5D6E
    *g_pfcu_cmd_area = (uint8_t) 0xD0;          // end command

    /* NOTE: Always poll for completion even when in BGO/interrupt mode. This is
     * because the faw register is "tied" to code flash. We do not want to return
     * from this function (which is executing in RAM) until the operation
     * completes. Otherwise, if the app happens to be executing in code flash
     * (FLASH_CFG_CODE_FLASH_RUN_FROM_ROM = 1), the illegal situation in effect
     * arises of writing to the same area that you are executing from.
     */
    g_current_parameters.wait_cnt = FLASH_FRDY_CMD_TIMEOUT;
    while (1 != FLASH.FSTATR.BIT.FRDY)
    {
        /* Wait until FRDY is 1 unless timeout occurs. */
        if (g_current_parameters.wait_cnt-- <= 0)
        {
            /* if FRDY is not set to 1 after max timeout, issue the stop command*/
            err = flash_stop();
            return err;
        }
    }

    flash_pe_mode_exit();

    return err;
}


/*******************************************************************************
* Function Name: R_CF_GetAccessWindow
* Description  : Return the read address form of the current access window area setting
* Arguments    : pAccessInfo -
*                    Pointer to structure to load address range into
* Return Value : FLASH_SUCCESS
*******************************************************************************/
flash_err_t R_CF_GetAccessWindow (flash_access_window_config_t  *pAccessInfo)
{

    pAccessInfo->start_addr = ((FLASH.FAWMON.BIT.FAWS << 13) | 0xFF000000);
    pAccessInfo->end_addr = ((FLASH.FAWMON.BIT.FAWE << 13) | 0xFF000000);

    return FLASH_SUCCESS;
}

#pragma section // end FRAM
#endif // FLASH_CFG_CODE_FLASH_ENABLE

#endif // FLASH_TYPE == 4
