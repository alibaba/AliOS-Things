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
* File Name    : lowlvl.c
* Version      : 2.00
* Description  : Functions to support stream I/O to the E1 virtual Console
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 01.10.2016 1.00     First Release
*         : 15.05.2017 2.00     Added the bsp startup module disable function.
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* r_bsp access. */
#include "rx_platform.h"

/* When using the user startup program, disable the following code. */
#if (BSP_CFG_STARTUP_DISABLE == 0)

#if BSP_CFG_USER_CHARPUT_ENABLED != 0
/* If user has indicated they want to provide their own charput function then this is the prototype. */
void BSP_CFG_USER_CHARPUT_FUNCTION(uint32_t output_char);
#endif

#if BSP_CFG_USER_CHARGET_ENABLED != 0
/* If user has indicated they want to provide their own charget function then this is the prototype. */
uint32_t BSP_CFG_USER_CHARGET_FUNCTION(void);
#endif

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#define E1_DBG_PORT (*(volatile struct st_dbg     __evenaccess *)0x84080)
#define TXFL0EN     0x00000100          // debug tx flow control bit
#define RXFL0EN     0x00001000          // debug RX flow control bit

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
struct st_dbg
{
    uint32_t   TX_DATA;     // Debug Virtual Console TX data
    char       wk1[12];     // spacer
    uint32_t   RX_DATA;     // Debug Virtual Console RX data
    char       wk2[44];     // spacer
    uint32_t   DBGSTAT;     // Debug Virtual Console Status
};

/***********************************************************************************************************************
Exported global variables (to be accessed by other files)
***********************************************************************************************************************/

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name: charput
* Description  : Outputs a character on a serial port
* Arguments    : character to output
* Return Value : none
***********************************************************************************************************************/
void charput (uint32_t output_char)
{
    /* If user has provided their own charput() function, then call it. */
#if BSP_CFG_USER_CHARPUT_ENABLED == 1
    BSP_CFG_USER_CHARPUT_FUNCTION(output_char);
#else
    /* Wait for transmit buffer to be empty */
    while(0 != (E1_DBG_PORT.DBGSTAT & TXFL0EN));

    /* Write the character out */
    E1_DBG_PORT.TX_DATA = output_char;
#endif
}

/***********************************************************************************************************************
* Function Name: charget
* Description  : Gets a character on a serial port
* Arguments    : none
* Return Value : received character
***********************************************************************************************************************/
uint32_t charget (void)
{
    /* If user has provided their own charget() function, then call it. */
#if BSP_CFG_USER_CHARGET_ENABLED == 1
    return BSP_CFG_USER_CHARGET_FUNCTION();
#else
    /* Wait for rx buffer buffer to be ready */
    while(0 == (E1_DBG_PORT.DBGSTAT & RXFL0EN));

    /* Read data, send back up */
    return E1_DBG_PORT.RX_DATA;
#endif
}

#endif /* BSP_CFG_STARTUP_DISABLE == 0 */

