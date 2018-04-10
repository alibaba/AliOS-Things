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
* File Name    : r_flash_fcu.h
* Description  : This is a private header file used internally by the FLASH module. It should not be modified or
*                included by the user in their application.
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*           05.10.2016 1.00    First Release
*           07.03.2017 1.10    Set value for FLASH_FRDY_CMD_TIMEOUT for flash type 4 (was not in early flash manual).
***********************************************************************************************************************/

#ifndef RX_FLASH_FCU_HEADER_FILE
#define RX_FLASH_FCU_HEADER_FILE

#ifdef FLASH_HAS_FCU

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#define FCU_FIRMWARE_STORAGE_AREA       (0xFEFFF000)
#define FCU_RAM_AREA                    (0x007F8000)
#define FCU_RAM_SIZE                    (4096)
//#define FAW_REG_ADDR                    (0xFE7F5D64)

/*FACI Commands*/
#define FLASH_FACI_CMD_PROGRAM              (0xE8)
#define FLASH_FACI_CMD_PROGRAM_CF           (0x80)
#define FLASH_FACI_CMD_PROGRAM_DF           (0x02)
#define FLASH_FACI_CMD_BLOCK_ERASE          (0x20)
#define FLASH_FACI_CMD_PE_SUSPEND           (0xB0)
#define FLASH_FACI_CMD_PE_RESUME            (0xD0)
#define FLASH_FACI_CMD_STATUS_CLEAR         (0x50)
#define FLASH_FACI_CMD_FORCED_STOP          (0xB3)
#define FLASH_FACI_CMD_BLANK_CHECK          (0x71)
#define FLASH_FACI_CMD_CONFIG_SET_1         (0x40)
#define FLASH_FACI_CMD_CONFIG_SET_2         (0x08)
#define FLASH_FACI_CMD_LOCK_BIT_PGM         (0x77)
#define FLASH_FACI_CMD_LOCK_BIT_READ        (0x71)
#define FLASH_FACI_CMD_FINAL                (0xD0)

/* The number of loops to wait for FENTRYR timeout. */
#define FLASH_FENTRYR_TIMEOUT   (4)

#if (FLASH_TYPE == FLASH_TYPE_3)
/* The maximum timeout for commands is 100usec when FCLK is 16 MHz i.e. 1600 FCLK cycles.
 * Assuming worst case of ICLK at 120 MHz and FCLK at 4 MHz, and optimization set to max such that
 * each count decrement loop takes only 5 cycles, then ((120/4)*1600)/5 = 9600 */
#define FLASH_FRDY_CMD_TIMEOUT          (9600)

#elif (FLASH_TYPE == FLASH_TYPE_4)
/* The maximum timeout for commands is...185ms (Fig 6.17 flash mem manual Config Set Cmd Timeout) */
#define FLASH_FRDY_CMD_TIMEOUT  ((int32_t)(185000 * (60.0/(MCU_CFG_FCLK_HZ/1000000)))*(MCU_CFG_ICLK_HZ/1000000))
#endif


/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Exported global variables
***********************************************************************************************************************/

/***********************************************************************************************************************
Exported global functions (to be accessed by other files)
***********************************************************************************************************************/
extern flash_err_t flash_init_fcu(void);
extern flash_err_t flash_fcuram_codecopy(void);
extern flash_err_t flash_reset(void);
extern flash_err_t flash_stop(void);
extern flash_err_t flash_pe_mode_enter(flash_type_t flash_type);
extern flash_err_t flash_pe_mode_exit(void);
extern flash_err_t flash_wait_frdy(void);
extern flash_err_t flash_get_status (void);
extern flash_err_t flash_erase(uint32_t block_address, uint32_t num_blocks);
extern flash_err_t flash_blankcheck(uint32_t start_address, uint32_t num_bytes, flash_res_t *result);
extern flash_err_t flash_write(uint32_t src_start_address, uint32_t dest_start_address, uint32_t num_bytes);


#endif // FLASH_HAS_FCU
#endif // RX_FLASH_FCU_HEADER_FILE 

/* end of file */
