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
* File Name    : mcu_clocks.c
* Description  : Contains clock specific routines
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version   Description
*         : 01.10.2016 1.00      First Release
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "rx_platform.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#define CKSEL_LOCO            0X0   // SCKCR3.CKSEL register setting for LOCO
#define CKSEL_HOCO            0X1   // SCKCR3.CKSEL register setting for HOCO
#define CKSEL_MAIN_OSC        0X2   // SCKCR3.CKSEL register setting for MAIN OSC
#define CKSEL_SUBCLOCK        0X3   // SCKCR3.CKSEL register setting for SUB-CLOCK OSC
#define CKSEL_PLL             0X4   // SCKCR3.CKSEL register setting for PLL

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Exported global variables (to be accessed by other files)
***********************************************************************************************************************/

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name: get_iclk_freq_hz (RX65N)
* Description  : Return the current ICLK frequency in Hz.  Called by R_BSP_GetIClkFreqHz().
*
*                The system clock source can be changed at any time via SYSTEM.SCKCR3.BIT.CKSEL, so in order to
*                determine the ICLK frequency we need to first find the current system clock source and then,
*                in some cases where the clock source can be configured for multiple frequencies, calculate the
*                frequency at which it is currently running.
*
* Arguments    : None
*
* Return Value : uint32_t - the iclk frequency in Hz
*
***********************************************************************************************************************/
uint32_t get_iclk_freq_hz(void)
{
    #define NORMALIZE_X10  10   // used to avoid floating point arithmetic

    uint32_t sysClockSrcFreq;
    uint32_t pll_multiplier;
    uint32_t pll_source_freq;
    uint32_t hoco_frequency[3] = {16000000, 18000000, 20000000}; // 16/18/20 MHz
    uint8_t  cksel = (uint8_t)SYSTEM.SCKCR3.BIT.CKSEL;  // Read the system clock select value

    switch (cksel)
    {
        case CKSEL_LOCO:
            sysClockSrcFreq = BSP_LOCO_HZ;
            break;

        case CKSEL_HOCO:
            sysClockSrcFreq = hoco_frequency[SYSTEM.HOCOCR2.BIT.HCFRQ];
            break;

        case CKSEL_MAIN_OSC:
            sysClockSrcFreq = BSP_CFG_XTAL_HZ;
            break;

        case CKSEL_SUBCLOCK:
            sysClockSrcFreq = BSP_SUB_CLOCK_HZ;
            break;

        case CKSEL_PLL:
            /* The RX65N have two possible sources for the PLL */

            pll_multiplier = (uint32_t)(((SYSTEM.PLLCR.BIT.STC + 1) * NORMALIZE_X10) / 2);

            pll_source_freq = BSP_CFG_XTAL_HZ; // Default to the MAIN OSC as the PLL source
            if (SYSTEM.PLLCR.BIT.PLLSRCSEL == 0x1) // If 1 then the HOCO is the PLL source
            {
                pll_source_freq = hoco_frequency[SYSTEM.HOCOCR2.BIT.HCFRQ];
            }

            sysClockSrcFreq = (uint32_t)((pll_source_freq / ((SYSTEM.PLLCR.BIT.PLIDIV + 1) * NORMALIZE_X10)) * pll_multiplier);

            break;

        default:
            sysClockSrcFreq = BSP_CFG_XTAL_HZ; // Should never arrive here. Use the Main OSC freq as a default...
    }

    /* Finally, divide the system clock source frequency by the currently set ICLK divider to get the ICLK frequency */
    return (sysClockSrcFreq / (1 << SYSTEM.SCKCR.BIT.ICK));
}
