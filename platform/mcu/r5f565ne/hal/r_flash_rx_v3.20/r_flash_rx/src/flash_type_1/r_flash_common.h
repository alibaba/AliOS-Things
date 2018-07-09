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
* File Name    : r_flash_common.h
* Description  : Header file for Flash common data
*                processing of Flash.
* Limitation   : none
*******************************************************************************/
/*******************************************************************************
* History      : DD.MM.YYYY Version  Description
*                17.07.2014 1.00     First Release
*                19.12.2014 1.10     Removed redundant equates.
*                12.01.2015 1.20     Updated for RX231
*                11.11.2015 1.40     Modified for RX24t support
*                                    (see defined(BSP_MCU_RX24_ALL))
*                22.07.2016 2.00      Modified for BSPless flash.
*                02.08.2017 2.10     Removed #include "r_mcu_config.h". Now in
*                                    targets.h (r_flash_rx_if.h includes)
*******************************************************************************/
#ifndef R_FLASH_COMMON_H_
#define R_FLASH_COMMON_H_

/******************************************************************************
Includes <System Includes> , "Project Includes"
******************************************************************************/
#include "r_flash_rx_config.h"
//#include "r_mcu_config.h"


/******************************************************************************
Typedef definitions
******************************************************************************/
/*==== Wait Process definition ====*/
#define WAIT_TDIS       (3)
#define WAIT_TMS_MID    (4)
#define WAIT_TMS_HIGH   (6)
#define WAIT_TDSTOP     (6)

#define MHZ             (1000000)
#define KHZ             (1000)
#define FCLK_MHZ        ((MCU_CFG_FCLK_HZ + 999999) / MHZ)
#define ICLK_KHZ        (MCU_CFG_ICLK_HZ / KHZ)


/*==== Flash information ====*/
/* Used for CodeFlash */
#define CODEFLASH_READ_BASE_END_ADDR        (0xFFFFFFFF)

#if defined(MCU_RX23_ALL) || defined(MCU_RX24_ALL)
#define CODEFLASH_WRITE_BASE_END_ADDR       (0xFC1FFFFF)
#else
#define CODEFLASH_WRITE_BASE_END_ADDR       (0x0007FFFF)
#endif
#define CODEFLASH_ADDR_OFFSET    (CODEFLASH_READ_BASE_END_ADDR - CODEFLASH_WRITE_BASE_END_ADDR)

/* Used for DataFlash */
#define DATAFLASH_READ_BASE_END_ADDR        (0x00101FFF)

#if defined(MCU_RX23_ALL) || defined(MCU_RX24_ALL)
#define DATAFLASH_WRITE_BASE_END_ADDR       (0xFE001FFF)
#else
#define DATAFLASH_WRITE_BASE_END_ADDR       (0x000F2FFF)
#endif
#define DATAFLASH_ADDR_OFFSET    (DATAFLASH_READ_BASE_END_ADDR - DATAFLASH_WRITE_BASE_END_ADDR)




#endif /* R_FLASH_COMMON_H_ */

/* end of file */
