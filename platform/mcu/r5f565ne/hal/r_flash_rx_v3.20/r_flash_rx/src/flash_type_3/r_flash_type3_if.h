/**********************************************************************************************************************
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
**********************************************************************************************************************/
/**********************************************************************************************************************
* File Name    : r_flash_type3_if.h
* Description  : This module implements equates/declarations specific to Flash Type 3 MCUs
**********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version Description           
*         : 02.18.2014 1.10    Support for RX64M
*         : 12.08.2016 2.00    Modified for BSPless operation.
*         : 17.11.2016 2.10    Added FLASH_FREQ_xx and FLASH_FCU_INT_xxx #defines
*         : 05.10.2016 3.00    Merged functions common to other flash types into r_flash_fcu.c and r_flash_group.c.
**********************************************************************************************************************/

#ifndef FLASH_API_IF_FCU_64M_HEADER_FILE
#define FLASH_API_IF_FCU_64M_HEADER_FILE

#include "r_flash_rx_if.h"
#if (FLASH_TYPE == 3)

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "r_flash_rx.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#define FLASH_FREQ_LO           (4000000)
#define FLASH_FREQ_HI           (60000000)

#define FLASH_FCU_INT_ENABLE    FLASH.FAEINT.BYTE = 0x90;   \
                                FLASH.FRDYIE.BYTE = 0x01;
#define FLASH_FCU_INT_DISABLE   FLASH.FAEINT.BYTE = 0x00;   \
                                FLASH.FRDYIE.BYTE = 0x00;

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Exported global variables
***********************************************************************************************************************/
extern lkbit_mode_t g_lkbit_mode;

/***********************************************************************************************************************
Exported global functions (to be accessed by other files)
***********************************************************************************************************************/
extern flash_err_t get_cmdlk_err(void);
extern void do_cmdlk_recovery(void);
extern flash_interrupt_event_t get_cmdlk_err_event(void);
extern flash_err_t flash_api_lockbit_set(flash_block_address_t block_address, uint32_t num_blocks);
extern flash_err_t flash_lockbit_read(flash_block_address_t block_address, flash_res_t *lock_state_result);

#endif // FLASH_TYPE == 3
#endif // FLASH_API_IF_FCU_64M_HEADER_FILE


