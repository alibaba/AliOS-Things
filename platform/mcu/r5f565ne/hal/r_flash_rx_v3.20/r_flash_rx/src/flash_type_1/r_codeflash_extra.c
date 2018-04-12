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
* File Name    : r_codeflash_extra.c
* Description  : Flash Control Access window and swap control
*                processing of Flash.
* Limitation   : none
*******************************************************************************/
/*******************************************************************************
* History      : DD.MM.YYYY Version  Description
*                17.07.2014 1.00     First Release
*                12.11.2014 1.10     Removed FLASH.DFLCTL.BIT.DFLEN operations in
*                                    in R_CF_SetAccessWindow() and r_cf_set_startup_area(),
*                                    and FLASH.FASR.BIT.EXS clearing in 
*                                    R_CF_SetAccessWindow and R_CF_GetCurrentStartupArea
*                                    as per Japan.
*                19.12.2014 1.20     Replaced some equate names and modified 
*                                    for FLASH_TYPE_1.
*                12.01.2015 1.30     Modified for RX231 support.
*                11.11.2015 1.40     Modified for RX24t support
*                                    (see defined(BSP_MCU_RX24_ALL))
*                02.08.2016 2.00     Modified for BSPless flash.
*                02.08.2017 2.10     Removed #include "r_mcu_config.h". Now in
*                                    targets.h (r_flash_rx_if.h includes)
*******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_flash_rx_if.h"
#if (FLASH_TYPE == FLASH_TYPE_1)
#include "r_flash_common.h"
#include "r_codeflash_extra.h"
#include "r_flash_type1_if.h"
#include "r_flash_group.h"


/******************************************************************************
Private global variables and functions
******************************************************************************/
#if (FLASH_CFG_CODE_FLASH_ENABLE == 1)

static void r_cf_extra_operation (const uint32_t start_addr_startup_value, const uint32_t end_addr, r_flash_command_t command);
static flash_err_t r_cf_set_startup_area (uint32_t value);

#pragma section FRAM

/*******************************************************************************
* Outline      : Setting the access window
* Header       : r_codeflash_extra.h
* Function Name: R_CF_SetAccessWindow
* Description  : Specifies the setting for the access window.
* Arguments    : start_addr : start address of Access Window Setting
               : end_addr   : end address of Access Window Setting. This should be one
                              beyond the actual last byte to allow write access for.
                              here as required by the spec.
* Return Value : FLASH_SUCCESS            - Command executed successfully
*              : FLASH_ERR_ACCESSW        - AccessWindow setting error
*******************************************************************************/
flash_err_t R_CF_SetAccessWindow (flash_access_window_config_t  *pAccessInfo)
{
    flash_err_t      err = FLASH_SUCCESS;
    uint32_t start_addr_idx;
    uint32_t end_addr_idx;


    err = flash_pe_mode_enter(FLASH_TYPE_CODE_FLASH);
    if (FLASH_SUCCESS != err)
    {
        return(err);
    }

    g_current_parameters.current_operation = FLASH_CUR_CF_ACCESSWINDOW;
    
    start_addr_idx = (pAccessInfo->start_addr) - CODEFLASH_ADDR_OFFSET;    /* Conversion to the P/E address from the read address */
    end_addr_idx   = (pAccessInfo->end_addr) - CODEFLASH_ADDR_OFFSET;      /* Conversion to the P/E address from the read address */

    /* == Select Extra Area == */
    FLASH.FASR.BIT.EXS = 1;

    r_cf_extra_operation(start_addr_idx, end_addr_idx, (r_flash_command_t)R_FLASH_ACCESSWINDOW);

#if (FLASH_CFG_CODE_FLASH_BGO == 0)
    do
    {
        err = r_cf_extra_check();
    }
    while (FLASH_ERR_BUSY == err);

    flash_pe_mode_exit();
#endif

    return err;

}

/*******************************************************************************
* Outline      : Getting the access window
* Header       : r_codeflash_extra.h
* Function Name: R_CF_GetAccessWindow
* Description  : Return the read address form of the current access window area setting
* Arguments    : none
* Return Value : FLASH_SUCCESS
*******************************************************************************/
flash_err_t R_CF_GetAccessWindow (flash_access_window_config_t  *pAccessInfo)
{
    pAccessInfo->start_addr = (((FLASH.FAWSMR << 10) + CODEFLASH_ADDR_OFFSET) | 0xFC000000);
    pAccessInfo->end_addr = (((FLASH.FAWEMR << 10) + CODEFLASH_ADDR_OFFSET) | 0xFC000000);

    return FLASH_SUCCESS;
}


/*******************************************************************************
* Outline      : Switching the start-up program
* Header       : r_codeflash_extra.h
* Function Name: R_CF_ToggleStartupArea
* Description  : Check the current start-up area setting
*              : and specifies the area currently not used as the start-up area.
* Arguments    : none
* Return Value : FLASH_SUCCESS -
*                Switched successfully.
*                FLASH_ERR_FAILURE -
*                Unable to Switch to P/E Mode.
*                FLASH_ERR_PARAM -
*                Illegal parameter passed
*******************************************************************************/
flash_err_t R_CF_ToggleStartupArea (void)
{
    uint8_t startup_area_flag;
    uint16_t reg_fscmr;
    flash_err_t err = FLASH_SUCCESS;

    err = flash_pe_mode_enter(FLASH_TYPE_CODE_FLASH);
    if (FLASH_SUCCESS != err)
    {
        return(err);
    }

    g_current_parameters.current_operation = FLASH_CUR_CF_TOGGLE_STARTUPAREA;
    
    reg_fscmr = FLASH.FSCMR.WORD;
    startup_area_flag  = (uint8_t)((reg_fscmr >> 8) & 0x01);

    if (DEFAULT_AREA == startup_area_flag)
    {
        r_cf_set_startup_area(STARTUP_AREA_VALUE);
    }
    else
    {
        r_cf_set_startup_area(DEFAULT_AREA_VALUE);
    }

    reg_fscmr = FLASH.FSCMR.WORD;

#if (FLASH_CFG_CODE_FLASH_BGO == 0)
    flash_pe_mode_exit();
#endif

    return err;
}

/*******************************************************************************
* Outline      : Setting for switching the start-up program
* Header       : r_codeflash_extra.h
* Function Name: r_cf_set_startup_area
* Description  : Set the start-up area information that is specified by the argument.
* Arguments    : value : Setting information for the FWBH register
* Return Value : FLASH_SUCCESS -  Command executed successfully
*              : FLASH_ERR_ACCESSW - Start-up program switching error
*******************************************************************************/
static flash_err_t r_cf_set_startup_area (uint32_t value)
{
    flash_err_t err = FLASH_SUCCESS;

    /* Select Extra Area */
    FLASH.FASR.BIT.EXS = 1;

    r_cf_extra_operation(value, DUMMY, R_FLASH_STARTUPAREA);

#if (FLASH_CFG_CODE_FLASH_BGO == 0)
    do
    {
        err = r_cf_extra_check();
    }
    while (FLASH_ERR_BUSY == err);
#endif

    return err;

}

/*******************************************************************************
* Outline      : Return which startup area (Default or Alternate) is active
* Header       : r_codeflash_extra.h
* Function Name: R_CF_GetCurrentStartupArea
* Description  : Return which startup area (Default or Alternate) is active
* Arguments    : none
* Return Value : startup_area_flag - 0 ==> Alternate area
*                                    1 ==> Default area
*******************************************************************************/
uint8_t R_CF_GetCurrentStartupArea(void)
{
    uint8_t startup_area_flag;
    uint16_t reg_fscmr;

    reg_fscmr = FLASH.FSCMR.WORD;
    startup_area_flag  = (uint8_t)((reg_fscmr >> 8) & 0x01);
    return(startup_area_flag);
}


/*******************************************************************************
* Outline      : Return which startup area (Default or Alternate)
* Header       : r_codeflash_extra.h
* Function Name: R_CF_GetCurrentSwapState
* Description  : Return which startup area (Default or Alternate) is active
* Arguments    : none
* Return Value : startup_area_select - 0 ==> The start-up area is selected
*                                            according to the start-up area
*                                            settings of the extra area
*                                      2 ==> The start-up area is switched to
*                                            the default area temporarily.
*                                      3 ==> The start-up area is switched to
*                                            the alternate area temporarily.
*******************************************************************************/
uint8_t R_CF_GetCurrentSwapState(void)
{
    uint8_t startup_area_select;

    startup_area_select = FLASH.FISR.BIT.SAS;
    return(startup_area_select);
}

/*******************************************************************************
* Outline      : Switching the start-up area
* Header       : r_codeflash_extra.h
* Function Name: R_CF_SetCurrentSwapState
* Description  : Setting for switching the start-up area
* Arguments    : value for SAS bits; switch startup area if value = SAS_SWITCH_AREA
* Return Value : none
*******************************************************************************/
void R_CF_SetCurrentSwapState(uint8_t value)
{
    uint8_t sas_flag;


    if (FLASH_SAS_SWITCH_AREA == value)                 // switch startup areas
    {
        if(FLASH_SAS_EXTRA == FLASH.FISR.BIT.SAS)       // switch based upon FISR.SAS reset area
        {
            if(SASMF_DEFAULT == FLASH.FSCMR.BIT.SASMF)
            {
                sas_flag = FLASH_SAS_ALTERNATE;
            }
            else
            {
                sas_flag = FLASH_SAS_DEFAULT;
            }
        }
        else
        {
            if(FLASH_SAS_ALTERNATE == FLASH.FISR.BIT.SAS)   // switch based upon current state
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

    flash_pe_mode_enter(FLASH_TYPE_CODE_FLASH);

    FLASH.FISR.BIT.SAS = sas_flag;

    while(sas_flag != FLASH.FISR.BIT.SAS)
    {
        /* Confirm that the written value can be read correctly. */
    }

    flash_pe_mode_exit();
}

/*******************************************************************************
* Outline      : Command processing for the extra area
* Header       : r_codeflash_extra.h
* Function Name: r_cf_extra_operation
* Description  : Issues a command to the extra area.
* Arguments    : start_addr_startup_value : start Address of the AccessWindow, or StartupAreaflag setting value
*              : end_addr : end Address of AccessWindow Setting, or Dummy value (start-up area information program)
*              : command  : select from R_FLASH_ACCESSWINDOW or R_FLASH_STARTUPAREA
* Return Value : none
*******************************************************************************/
static void r_cf_extra_operation (const uint32_t start_addr_startup_value, const uint32_t end_addr, r_flash_command_t command)
{

    if (R_FLASH_ACCESSWINDOW == command)
    {
#if defined(MCU_RX23_ALL) || defined(MCU_RX24_ALL)
        FLASH.FWB1 = (uint16_t)((end_addr >> 10) & 0xFFF);
        FLASH.FWB0 = (uint16_t)((start_addr_startup_value >> 10) & 0xFFF);
#else
        /* Access Window start address setting */
        FLASH.FWBH = (uint16_t)(end_addr >> 10);
        FLASH.FWBL = (uint16_t)(start_addr_startup_value >> 10);
#endif
        /* Execute Access Window command */
        FLASH.FEXCR.BYTE = FEXCR_AW;
    }
    else
    {
#if defined(MCU_RX23_ALL) || defined(MCU_RX24_ALL)
        FLASH.FWB3 = (uint16_t)(0xFFFF);
        FLASH.FWB2 = (uint16_t)(0xFFFF);
        FLASH.FWB1 = (uint16_t)(0xFFFF);
        FLASH.FWB0 = (uint16_t)(start_addr_startup_value);
#else
        /* Startup Area Flag value setting */
        FLASH.FWBH = (uint16_t)(0xFFFF);
        FLASH.FWBL = (uint16_t)(start_addr_startup_value);
#endif
        /* Execute Startup Area Flag command */
        FLASH.FEXCR.BYTE = FEXCR_STARTUP;
    }

}

/*******************************************************************************
* Outline      : Verifying the execution result for the extra area
* Header       : r_codeflash_extra.h
* Function Name: r_cf_extra_check
* Description  : Waits for completing of the command execution
*              : and verifies the result of the execution.
* Arguments    : none
* Return Value : FLASH_SUCCESS          - Command executed successfully
*              : FLASH_ERR_BUSY         - Command being executed
*              : FLASH_ERR_ACCESSW      - Extra area command error
*******************************************************************************/
flash_err_t r_cf_extra_check (void)
{

    /* Check EXFREADY Flag bit*/
    if (1 != FLASH.FSTATR1.BIT.EXRDY)
    {
        return FLASH_ERR_BUSY;
    }

    /* Clear FEXCR register */
    FLASH.FEXCR.BYTE = FEXCR_CLEAR;

    while (0 != FLASH.FSTATR1.BIT.EXRDY)
    {
        /* Check that execute command is completed. */
    }

    if ((0 != FLASH.FSTATR0.BIT.EILGLERR) || (0 != FLASH.FSTATR0.BIT.PRGERR))
    {
        flash_reset();
        return FLASH_ERR_ACCESSW;
    }

    return FLASH_SUCCESS;
}

#pragma section /*end FRAM */

#endif
#endif

/* end of file */
