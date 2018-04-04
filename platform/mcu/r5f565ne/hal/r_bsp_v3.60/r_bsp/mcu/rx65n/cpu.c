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
* File Name    : cpu.c
* Description  : This module implements CPU specific functions. An example is enabling/disabling interrupts. 
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 01.10.2016 1.00     First Release
*         : 15.05.2017 2.00     Changed comments of the PRCR register.
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* Platform support. */
#include "rx_platform.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/* Key code for writing PRCR register. */
#define BSP_PRV_PRCR_KEY        (0xA500)

/***********************************************************************************************************************
Error checking
***********************************************************************************************************************/
#if !defined(__RENESAS__)
    #error "ERROR - Need to add compiler specific code in cpu.c"
#endif

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Exported global variables (to be accessed by other files)
***********************************************************************************************************************/
 
/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/
/* Used for holding reference counters for protection bits. */
static volatile uint16_t g_protect_counters[BSP_REG_PROTECT_TOTAL_ITEMS];
/* Masks for setting or clearing the PRCR register. Use -1 for size because PWPR in MPC is used differently. */
static const    uint16_t g_prcr_masks[BSP_REG_PROTECT_TOTAL_ITEMS-1] = 
{
    0x0001,         /* PRC0. */
    0x0002,         /* PRC1. */
    0x0008,         /* PRC3. */
};

/***********************************************************************************************************************
* Function Name: R_BSP_InterruptsDisable
* Description  : Globally disable interrupts.
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
void R_BSP_InterruptsDisable (void)
{
#if defined(__RENESAS__)
    /* Use the compiler intrinsic function to clear the I flag. This function is available with for the Renesas RX 
       compiler. This may need to be changed for other compilers. */
    clrpsw_i();
#endif
} 

/***********************************************************************************************************************
* Function Name: R_BSP_InterruptsEnable
* Description  : Globally enable interrupts.
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
void R_BSP_InterruptsEnable (void)
{
#if defined(__RENESAS__)
    /* Use the compiler intrinsic function to set the I flag. This function is available with for the Renesas RX 
       compiler. This may need to be changed for other compilers. */
    setpsw_i();
#endif
} 

/***********************************************************************************************************************
* Function Name: R_BSP_CpuInterruptLevelRead
* Description  : Reads the processor interrupt priority level.
* Arguments    : none
* Return Value : The current processor IPL
***********************************************************************************************************************/
uint32_t R_BSP_CpuInterruptLevelRead (void)
{
#if defined(__RENESAS__)
    /* Use the compiler intrinsic function to read the CPU IPL. This function is available with for the Renesas RX 
       compiler. This may need to be changed for other compilers. */
    return (uint32_t)get_ipl();
#endif
}

/***********************************************************************************************************************
* Function Name: R_BSP_CpuInterruptLevelWrite
* Description  : Writes the processor interrupt priority level.
* Arguments    : level -
*                    The level to set the processor's IPL to.
* Return Value : true -
*                    The level was set successfully.
*                false -
*                    Invalid level input. 
***********************************************************************************************************************/
bool R_BSP_CpuInterruptLevelWrite (uint32_t level)
{
#if (BSP_CFG_PARAM_CHECKING_ENABLE == 1)
    /* Check for valid level. */
    if (level > BSP_MCU_IPL_MAX)
    {
        return false;
    }
#endif

#if defined(__RENESAS__)
    /* Use the compiler intrinsic function to set the CPU IPL. This function is available with for the Renesas RX 
       compiler. This may need to be changed for other compilers. */
    set_ipl((signed long)level);
#endif

    return true;
}

/***********************************************************************************************************************
* Function Name: R_BSP_RegisterProtectEnable
* Description  : Enables register protection. Registers that are protected cannot be written to. Register protection is 
*                enabled by using the Protect Register (PRCR) and the MPC's Write-Protect Register (PWPR).
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
void R_BSP_RegisterProtectEnable (bsp_reg_protect_t regs_to_protect)
{
    /* Is it safe to disable write access? */
    if (0 != g_protect_counters[regs_to_protect])
    {
        /* Decrement the protect counter */
        g_protect_counters[regs_to_protect]--;
    }

    /* Is it safe to disable write access? */
    if (0 == g_protect_counters[regs_to_protect])
    {
        if (BSP_REG_PROTECT_MPC != regs_to_protect)
        {
            /* Enable protection using PRCR register. */
            /* When writing to the PRCR register the upper 8-bits must be the correct key. Set lower bits to 0 to 
               disable writes. 
               b15:b8 PRKEY - Write 0xA5 to upper byte to enable writing to lower byte
               b7:b4  Reserved (set to 0)
               b3     PRC3  - Enables writing to the registers related to the LVD: LVCMPCR, LVDLVLR, LVD1CR0, LVD1CR1, 
                              LVD1SR, LVD2CR0, LVD2CR1, LVD2SR.
               b2     Reserved (set to 0)
               b1     PRC1  - Enables writing to the registers related to operating modes, low power consumption, the 
                              clock generation circuit, and software reset: SYSCR0, SYSCR1, SBYCR, MSTPCRA, MSTPCRB,
                              MSTPCRC, MSTPCRD, OPCCR, RSTCKCR, DPSBYCR, DPSIER0, DPSIER1, DPSIER2, DPSIER3,
                              DPSIFR0, DPSIFR1, DPSIFR2, DPSIFR3, DPSIEGR0, DPSIEGR1, DPSIEGR2, DPSIEGR3,
                              MOSCWTCR, SOSCETCR, MOFCR, HOCOPCR, SWRR.
               b0     PRC0  - Enables writing to the registers related to the clock generation circuit: SCKCR, SCKCR2,
                              SCKCR3, PLLCR, PLLCR2, BCKCR, MOSCCR, SOSCCR, LOCOCR, ILOCOCR, HOCOCR, HOCOCR2, OSTDCR,
                              OSTDSR.
            */
            SYSTEM.PRCR.WORD = (uint16_t)((SYSTEM.PRCR.WORD | BSP_PRV_PRCR_KEY) & (~g_prcr_masks[regs_to_protect]));
        }
        else
        {
            /* Enable protection for MPC using PWPR register. */
            /* Enable writing of PFSWE bit. It could be assumed that the B0WI bit is still cleared from a call to 
               protection disable function, but it is written here to make sure that the PFSWE bit always gets 
               cleared. */
            MPC.PWPR.BIT.B0WI = 0;     
            /* Disable writing to PFS registers. */ 
            MPC.PWPR.BIT.PFSWE = 0;    
            /* Disable writing of PFSWE bit. */
            MPC.PWPR.BIT.B0WI = 1;     
        }
    }
}

/***********************************************************************************************************************
* Function Name: R_BSP_RegisterProtectDisable
* Description  : Disables register protection. Registers that are protected cannot be written to. Register protection is
*                disabled by using the Protect Register (PRCR) and the MPC's Write-Protect Register (PWPR).
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
void R_BSP_RegisterProtectDisable (bsp_reg_protect_t regs_to_unprotect)
{
    /* If this is first entry then disable protection. */
    if (0 == g_protect_counters[regs_to_unprotect])
    {
        if (BSP_REG_PROTECT_MPC != regs_to_unprotect)
        {
            /* Enable protection using PRCR register. */
            /* When writing to the PRCR register the upper 8-bits must be the correct key. Set lower bits to 1 to 
               enable writes. 
               b15:b8 PRKEY - Write 0xA5 to upper byte to enable writing to lower byte
               b7:b4  Reserved (set to 0)
               b3     PRC3  - Enables writing to the registers related to the LVD: LVCMPCR, LVDLVLR, LVD1CR0, LVD1CR1, 
                              LVD1SR, LVD2CR0, LVD2CR1, LVD2SR.
               b2     Reserved (set to 0)
               b1     PRC1  - Enables writing to the registers related to operating modes, low power consumption, the 
                              clock generation circuit, and software reset: SYSCR0, SYSCR1, SBYCR, MSTPCRA, MSTPCRB,
                              MSTPCRC, MSTPCRD, OPCCR, RSTCKCR, DPSBYCR, DPSIER0, DPSIER1, DPSIER2, DPSIER3,
                              DPSIFR0, DPSIFR1, DPSIFR2, DPSIFR3, DPSIEGR0, DPSIEGR1, DPSIEGR2, DPSIEGR3,
                              MOSCWTCR, SOSCETCR, MOFCR, HOCOPCR, SWRR.
               b0     PRC0  - Enables writing to the registers related to the clock generation circuit: SCKCR, SCKCR2,
                              SCKCR3, PLLCR, PLLCR2, BCKCR, MOSCCR, SOSCCR, LOCOCR, ILOCOCR, HOCOCR, HOCOCR2, OSTDCR,
                              OSTDSR.
            */
            SYSTEM.PRCR.WORD = (uint16_t)((SYSTEM.PRCR.WORD | BSP_PRV_PRCR_KEY) | g_prcr_masks[regs_to_unprotect]);
        }
        else
        {
            /* Disable protection for MPC using PWPR register. */
            /* Enable writing of PFSWE bit. */
            MPC.PWPR.BIT.B0WI = 0;
            /* Enable writing to PFS registers. */ 
            MPC.PWPR.BIT.PFSWE = 1;
        }
    }

    /* Increment the protect counter */
    g_protect_counters[regs_to_unprotect]++;
}

/***********************************************************************************************************************
* Function Name: bsp_register_protect_open
* Description  : Initializes variables needed for register protection functionality.
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
void bsp_register_protect_open (void)
{
    uint32_t i;

    /* Initialize reference counters to 0. */
    for (i = 0; i < BSP_REG_PROTECT_TOTAL_ITEMS; i++)
    {
        g_protect_counters[i] = 0;
    }
}


