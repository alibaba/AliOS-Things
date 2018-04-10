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
* File Name    : r_codeflash_extra.h
* Description  : Header file for Flash Control Access window and swap control
*                processing of E2 Code Flash.
* Limitation   : none
*******************************************************************************/
/*******************************************************************************
* History      : DD.MM.YYYY Version  Description
*                17.07.2014 1.00     First Release
*                12.01.2015 1.10     Added SAS equates for RX231
*******************************************************************************/

#ifndef R_CODEFLASH_EXTRA_H_
#define R_CODEFLASH_EXTRA_H_
#include "r_flash_rx_if.h"

/******************************************************************************
Macro definitions
******************************************************************************/
/*  operation definition (FEXCR Register setting)*/
#define FEXCR_STARTUP       (0x81)
#define FEXCR_AW            (0x82)
#define FEXCR_CLEAR         (0x00)

#define DUMMY               (0xFFFFFFFF)
#define DEFAULT_AREA        (1)
#define DEFAULT_AREA_VALUE  (0xFFFF)
#define STARTUP_AREA_VALUE  (0xFEFF)

#define SASMF_ALTERNATE     (0)
#define SASMF_DEFAULT       (1)


/******************************************************************************
Typedef definitions
******************************************************************************/

/**
   @enum r_flash_command_t
   FLASH operation command values
*/
typedef enum R_FLASH_COMMAND_T
{
    R_FLASH_ACCESSWINDOW,               /**< Flash access window command */
    R_FLASH_STARTUPAREA                 /**< Flash change startup area command */
} r_flash_command_t;


/******************************************************************************
Exported global functions (to be accessed by other files)
******************************************************************************/
flash_err_t R_CF_SetAccessWindow (flash_access_window_config_t  *pAccessInfo);
flash_err_t R_CF_GetAccessWindow (flash_access_window_config_t  *pAccessInfo);

flash_err_t R_CF_ToggleStartupArea (void);
uint8_t R_CF_GetCurrentStartupArea(void);
uint8_t R_CF_GetCurrentSwapState(void);
void R_CF_SetCurrentSwapState(uint8_t value);
flash_err_t r_cf_extra_check (void);


#endif /* R_CODEFLASH_EXTRA_H_ */

/* end of file */
