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
* Copyright (C) 2017 Renesas Electronics Corporation. All rights reserved.    
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : r_bsp.h
* H/W Platform : RSKRX65N_2MB
* Description  : Has the header files that should be included for this platform.
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : DD.MM.YYYY Version   Description
*         : 15.05.2017 1.00      First Release
***********************************************************************************************************************/

#ifndef BSP_BOARD_RSKRX65N_2MB
#define BSP_BOARD_RSKRX65N_2MB

/* Make sure that no other platforms have already been defined. Do not touch this! */
#ifdef  PLATFORM_DEFINED
#error  "Error - Multiple platforms defined in platform.h!"
#else
#define PLATFORM_DEFINED
#endif

/***********************************************************************************************************************
INCLUDE APPROPRIATE MCU AND BOARD FILES
***********************************************************************************************************************/
#include    "mcu/all/r_bsp_common.h"
#include    "r_bsp_config.h"
//#include    "r_bsp_interrupt_config.h"
#include    "mcu/rx65n/register_access/iodefine.h"
#include    "mcu/rx65n/mcu_info.h"
//#include    "mcu/rx65n/mcu_locks.h"
//#include    "mcu/rx65n/locking.h"
#include    "mcu/rx65n/cpu.h"
//#include    "mcu/rx65n/mcu_init.h"
#include    "mcu/rx65n/mcu_interrupts.h"
#include    "mcu/rx65n/mcu_mapped_interrupts_private.h"
#include    "mcu/rx65n/mcu_mapped_interrupts.h"
//#include    "mcu/rx65n/mcu_startup.h"
//#include    "board/rskrx65n_2mb/rskrx65n_2mb.h"
//#include    "board/rskrx65n_2mb/hwsetup.h"
//#include    "board/rskrx65n_2mb/lowsrc.h"
//#include    "board/rskrx65n_2mb/vecttbl.h"

#endif /* BSP_BOARD_RSKRX65N_2MB */

