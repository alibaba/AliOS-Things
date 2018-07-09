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
* Copyright (C) 2014 Renesas Electronics Corporation. All rights reserved.
**********************************************************************************************************************/
/**********************************************************************************************************************
* File Name    : r_flash_type2_if.h
* Description  : This file defines the flash API functions for FCU enabled MCU's.
*
**********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version Description           
*         : 02.18.2014 1.10    Support for RX200, RX600
*         : 12.08.2016 2.00    Modified for BSPless operation.
**********************************************************************************************************************/

#ifndef FLASH_API_IF_FCU_HEADER_FILE
#define FLASH_API_IF_FCU_HEADER_FILE

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* Fixed width integer support. */
#include <stdint.h>
/* bool support */
#include <stdbool.h>
#include "r_flash_rx.h"          // Include this here or flash_err_t doesn't get resolved

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Exported global variables
***********************************************************************************************************************/

/***********************************************************************************************************************
Exported global functions (to be accessed by other files)
***********************************************************************************************************************/
flash_err_t flash_api_open(void);
flash_err_t flash_api_write(uint32_t buffer_addr, uint32_t flash_addr, uint32_t bytes);
flash_err_t flash_api_erase(flash_block_address_t block_start_address, uint32_t num_blocks);
flash_err_t flash_api_blankcheck(uint32_t address, uint32_t num_bytes, flash_res_t *result);
flash_err_t flash_api_control(flash_cmd_t cmd,  void  *pcfg);
uint32_t flash_get_romBlock_size (uint32_t addr, rom_block_info_t* pblock_info);
bool flash_get_romBlock_info (uint32_t block_number, rom_block_info_t* pblock_info);
flash_err_t flash_getStatus (void);

/* These functions are only used when BGO (non-blocking) mode is enabled */
#if ((FLASH_CFG_DATA_FLASH_BGO == 1) || (FLASH_CFG_CODE_FLASH_BGO == 1))
/* Callback function to call when flash erase is finished */
void FlashEraseDone(void);
/* Callback function to call when flash write is finished */
void FlashWriteDone(void);
/* Function to take care of flash errors */
void FlashError(void);
/* Callback function to call when flash blank check is finished. 'result'
   argument is 0 if block was blank and 1 if it was not */
void FlashBlankCheckDone(uint8_t result);
#endif


#endif /* FLASH_API_IF_FCU_HEADER_FILE */


