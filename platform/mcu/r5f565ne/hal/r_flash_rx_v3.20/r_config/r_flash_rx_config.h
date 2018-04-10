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
 * Copyright (C) 2014 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 * File Name    : r_flash_rx_config_reference.h
 * Description  : Configures the FLASH API module for RX200 and RX600 Series MCU's.
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*           12.04.2014 1.00    First Release
*           22.12.2014 1.10    Added flash type usage comments.
*           25.06.2015 1.20    Added FLASH_CFG_CODE_FLASH_RUN_FROM_ROM.
*         : 12.10.2016 2.00    Modified for BSPless operation (added FLASH_CFG_USE_FIT_BSP).
***********************************************************************************************************************/
#ifndef FLASH_CONFIG_HEADER_FILE
#define FLASH_CONFIG_HEADER_FILE

/* Set the following value to 0 when building without using the FIT BSP Module */
#define FLASH_CFG_USE_FIT_BSP       (1)


/***********************************************************************************************************************
 Configuration Options
 ***********************************************************************************************************************/
/* SPECIFY WHETHER TO INCLUDE CODE FOR API PARAMETER CHECKING
 * Setting to BSP_CFG_PARAM_CHECKING_ENABLE utilizes the system default setting
 * Setting to 1 includes parameter checking; 0 compiles out parameter checking
 */
#define FLASH_CFG_PARAM_CHECKING_ENABLE     (1)


/******************************************************************************
 ENABLE CODE FLASH PROGRAMMING
******************************************************************************/
/* If you are only using data flash, set this to 0.
 * Setting to 1 includes code to program the ROM area. When programming ROM,
 * code must be executed from RAM, except under certain restrictions for flash
 * type 3 (see section 2.14 in App Note). See section 2.13 in the App Note for
 * details on how to set up code and the linker to execute code from RAM.
 */
#define FLASH_CFG_CODE_FLASH_ENABLE (1)


/******************************************************************************
 ENABLE BGO/NON-BLOCKING DATA FLASH OPERATIONS
******************************************************************************/
/* Setting this to 0 forces data flash API function to block until completed.
 * Setting to 1 places the module in BGO (background operations) mode. In BGO 
 * mode, data flash operations return immediately after the operation has been 
 * started. Notification of the operation completion is done via the callback 
 * function. 
 */
#define FLASH_CFG_DATA_FLASH_BGO   (0)


/******************************************************************************
 ENABLE BGO/NON-BLOCKING CODE FLASH (ROM) OPERATIONS
******************************************************************************/
/* Setting this to 0 forces ROM API function to block until completed.
 * Setting to 1 places the module in BGO (background operations) mode. In BGO 
 * mode, ROM operations return immediately after the operation has been started.
 * Notification of the operation completion is done via the callback function. 
 * When reprogramming ROM, THE RELOCATABLE VECTOR TABLE AND CORRESPONDING
 * INTERRUPT ROUTINES MUST BE IN RAM.  
 * See sections 2.16 Usage Notes in the App Note.
 */
#define FLASH_CFG_CODE_FLASH_BGO   (0)


/******************************************************************************
 ENABLE CODE FLASH SELF-PROGRAMMING
******************************************************************************/
/* Set this to 0 when programming code flash while executing in RAM.
 * Set this to 1 when programming code flash while executing from another
 * segment in ROM (possible only with RX64M, RX71M, RX65N-2 groups).
 * See section 2.14 in the App Note.
 */
#define FLASH_CFG_CODE_FLASH_RUN_FROM_ROM   (0)


/******************************************************************************
 SET IPL OF FLASH READY INTERRUPT
******************************************************************************/
#define FLASH_CFG_FLASH_READY_IPL   (5)     // Flash type 2 only


/******************************************************************************
 ENABLE OR DISABLE LOCK BIT PROTECTION
******************************************************************************/
/* Each erasure block has a corresponding lock bit that can be used to
 * protect that block from being programmed/erased after the lock bit is
 * set. The use of lock bits can be used or ignored.
 * Setting this to 1 will cause lock bits to be ignored and programs/erases to a
 * block will not be limited.
 * Setting this to 0 will cause lock bits to be used as the user configures through
 * the Control command. This only applies to ROM as the DF does not have lock bits.
 */
#define FLASH_CFG_IGNORE_LOCK_BITS  (1)     // Flash type 2 only


#endif /* FLASH_CONFIG_HEADER_FILE */
