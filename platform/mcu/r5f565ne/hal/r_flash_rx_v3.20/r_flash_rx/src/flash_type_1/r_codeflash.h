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
* File Name    : r_codeflash.h
* Description  : Header file for Flash Control Processing of E2 Code Flash.
* Limitation   : none
*******************************************************************************/
/*******************************************************************************
* History      : DD.MM.YYYY Version  Description
*                17.07.2014 1.00     First Release
*                12.01.2015 1.10     Modified for RX231.
*******************************************************************************/

#ifndef R_CODEFLASH_H_
#define R_CODEFLASH_H_

#include "r_flash_rx_if.h"
/******************************************************************************
Macro definitions
******************************************************************************/

/*  flash mode definition (FENTRYR Register setting)*/
#define FENTRYR_CODEFLASH_PE_MODE   (0xAA01)
#define FENTRYR_READ_MODE           (0xAA00)

/*  flash mode definition (FPMCR Register setting)*/
#define DISCHARGE_1                 (0x12)
#define DISCHARGE_2                 (0x92)
#define CODEFLASH_PE_MODE           (0x82)
#define READ_MODE                   (0x08)
#define LVPE_MODE                   (0x40)

/*  operation definition (FCR Register setting)*/
#define FCR_WRITE                   (0x81)
#define FCR_ERASE                   (0x84)
#define FCR_BLANKCHECK              (0x83)
#define FCR_CLEAR                   (0x00)

#define OPCCR_HIGH_SPEED_MODE       (0x00)

/******************************************************************************
Typedef definitions
******************************************************************************/
typedef struct R_CODEFLASH_DATA_T
{
    uint32_t start_addr;    /* start address (Erase) or Ram Source for Write, Dest for read */
    uint32_t end_addr;      /* end address (Erase), or Flash Start address which will be read/written */
    uint32_t write_cnt;     /* units remaining to do */
} r_codeflash_data_t;


/******************************************************************************
Exported global variables
******************************************************************************/
void R_CF_Enter_PE_Mode(void);
void R_CF_Enter_Read_Mode(void);
void R_CF_Erase (const uint32_t start_addr, const uint32_t num_blocks);
void R_CF_Erase_Operation (const uint32_t start_addr, const uint32_t end_addr);
flash_err_t R_CF_Erase_Check (void);
void R_CF_Write (uint32_t *psrc_addr, const uint32_t dest_addr, const uint32_t byte_length );
void R_CF_Write_Operation (const uint32_t *psrc_addr, const uint32_t dest_addr);
flash_err_t R_CF_Write_Check (void);
void R_CF_BlankCheck(uint32_t start_addr, uint32_t end_addr);
flash_err_t R_CF_BlankCheck_Check (void);


#endif /* R_CODEFLASH_H_ */

/* end of file */
