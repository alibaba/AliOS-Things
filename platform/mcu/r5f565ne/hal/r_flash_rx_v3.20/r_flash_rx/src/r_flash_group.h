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
* File Name    : r_flash_group.h
* Description  : This is a private header file used internally by the FLASH module. It should not be modified or
*                included by the user in their application.
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*           05.10.2016 1.00    First Release
***********************************************************************************************************************/

#ifndef RX_FLASH_GROUP_HEADER_FILE
#define RX_FLASH_GROUP_HEADER_FILE

#include "r_flash_rx_if.h"
#if (FLASH_TYPE != 2)

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Exported global variables
***********************************************************************************************************************/
extern void (*flash_ready_isr_handler)(void *);        // Function pointer for Flash Ready ISR
extern void (*flash_error_isr_handler)(void *);        // Function pointer for Flash Error ISR
extern flash_int_cb_args_t g_flash_int_ready_cb_args;  // Callback argument structure for flash READY interrupt
extern flash_int_cb_args_t g_flash_int_error_cb_args;  // Callback argument structure for flash ERROR interrupt
extern current_param_t g_current_parameters;

/***********************************************************************************************************************
Exported global functions (to be accessed by other files)
***********************************************************************************************************************/
extern flash_err_t r_flash_open(void);
extern flash_err_t r_flash_erase(flash_block_address_t block_start_address, uint32_t num_blocks);
extern flash_err_t check_cf_block_total(flash_block_address_t block_start_address, uint32_t num_blocks);
#ifndef FLASH_NO_BLANK_CHECK
extern flash_err_t r_flash_blankcheck(uint32_t address, uint32_t num_bytes, flash_res_t *result);
#endif
extern flash_err_t r_flash_write(uint32_t src_address, uint32_t dest_address, uint32_t num_bytes);
extern flash_err_t r_flash_control(flash_cmd_t cmd,  void  *pcfg);

#endif  // #if (FLASH_TYPE != 2)
#endif  // RX_FLASH_GROUP_HEADER_FILE

