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
* File Name    : mcu_interrupts.c
* Description  : This module allows for callbacks to be registered for certain interrupts.
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version   Description
*         : 01.10.2016 1.00      First Release
*         : 15.05.2017 2.00      Added callback processing of the following group interrupt sources.
*                                - RIIC1_EEI1
*                                - RIIC1_TEI1
*                                - GLCDC_VPOS
*                                - GLCDC_GR1UF
*                                - GLCDC_GR2UF
*                                - DRW2D_DRW_IRQ
*                                Changed calling order of callback function in group interrupts.
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* Used for accessing CPU registers. */
#include <machine.h>
/* Access to r_bsp. */
#include "rx_platform.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/* This macro is used to suppress compiler messages about a parameter not being used in a function. The nice thing
 * about using this implementation is that it does not take any extra RAM or ROM.
 */
#define INTERNAL_NOT_USED(p)        ((void)(p))
/* Let FPSW EV, EO, EZ, EU, EX=1 (FPU exceptions enabled.) */
#define FPU_EXCEPTIONS_ENABLE       (0x00007C00)

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Exported global variables (to be accessed by other files)
***********************************************************************************************************************/
 
/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/
/* This array holds callback functions. */
static void (* g_bsp_vectors[BSP_INT_SRC_TOTAL_ITEMS])(void * pdata);

bsp_int_err_t bsp_interrupt_enable_disable(bsp_int_src_t vector, bool enable);
bsp_int_err_t bsp_interrupt_group_enable_disable(bsp_int_src_t vector, bool enable, uint32_t ipl);

/***********************************************************************************************************************
* Function Name: bsp_interrupt_open
* Description  : Initialize callback function array.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void bsp_interrupt_open (void)
{
    uint32_t i;

    for (i = 0; i < BSP_INT_SRC_TOTAL_ITEMS; i++)
    {
        g_bsp_vectors[i] = FIT_NO_FUNC;
    }

    /* Initialize mapped interrupts. */
    bsp_mapped_interrupt_open();
}

/***********************************************************************************************************************
* Function Name: R_BSP_InterruptWrite
* Description  : Registers a callback function for supported interrupts. If FIT_NO_FUNC, NULL, or
*                any other invalid function address is passed for the callback argument then any previously registered
*                callbacks are unregistered. Use of FIT_NO_FUNC is preferred over NULL since access to the address
*                defined by FIT_NO_FUNC will cause a bus error which is easy for the user to catch. NULL typically
*                resolves to 0 which is a valid address on RX MCUs.
* Arguments    : vector -
*                    Which interrupt to register a callback for.
*                callback -
*                    Pointer to function to call when interrupt occurs.
* Return Value : BSP_INT_SUCCESS -
*                    Callback registered
*                BSP_INT_ERR_INVALID_ARG -
*                    Invalid function address input, any previous function has been unregistered
***********************************************************************************************************************/
bsp_int_err_t R_BSP_InterruptWrite (bsp_int_src_t vector,  bsp_int_cb_t callback)
{
    bsp_int_err_t err;

    err = BSP_INT_SUCCESS;

    /* Check for valid address. */
    if (((uint32_t)callback == (uint32_t)NULL) || ((uint32_t)callback == (uint32_t)FIT_NO_FUNC))
    {
        g_bsp_vectors[vector] = FIT_NO_FUNC;
    }
    else
    {
        g_bsp_vectors[vector] = callback;
    }

    return err;
}

/***********************************************************************************************************************
* Function Name: R_BSP_InterruptRead
* Description  : Returns the callback function address for an interrupt.
* Arguments    : vector -
*                    Which interrupt to read the callback for.
*                callback -
*                    Pointer of where to store callback address.
* Return Value : BSP_INT_SUCCESS -
*                    Callback was registered and address has been stored in 'callback' parameter.
*                BSP_INT_ERR_NO_REGISTERED_CALLBACK -
*                    No valid callback has been registered for this interrupt source.
***********************************************************************************************************************/
bsp_int_err_t R_BSP_InterruptRead (bsp_int_src_t vector, bsp_int_cb_t * callback)
{
    bsp_int_err_t err;

    err = BSP_INT_SUCCESS;

    /* Check for valid address. */
    if (((uint32_t)g_bsp_vectors[vector] == (uint32_t)NULL) || ((uint32_t)g_bsp_vectors[vector] == (uint32_t)FIT_NO_FUNC))
    {
        err = BSP_INT_ERR_NO_REGISTERED_CALLBACK;
    }
    else
    {
        *callback = g_bsp_vectors[vector];
    }

    return err;
}

/***********************************************************************************************************************
* Function Name: R_BSP_InterruptControl
* Description  : Executes specified command.
* Arguments    : vector -
*                    Which vector is being used.
*                cmd -
*                    Which command to execute.
*                pdata -
*                    Pointer to data to use with command.
*                    Currently only used for BSP_INT_CMD_GROUP_INTERRUPT_ENABLE, and points
*                    to a bsp_int_ctrl_t *. Other commands should supply FIT_NO_PTR.
* Return Value : BSP_INT_SUCCESS -
*                    Command executed successfully.
*                BSP_INT_ERR_NO_REGISTERED_CALLBACK -
*                    No valid callback has been registered for this interrupt source.
*                BSP_INT_ERR_INVALID_ARG -
*                    Invalid command sent in.
*                BSP_INT_ERR_GROUP_STILL_ENABLED -
*                    Not all group interrupts were disabled so group interrupt was not disabled.
***********************************************************************************************************************/
bsp_int_err_t R_BSP_InterruptControl (bsp_int_src_t vector, bsp_int_cmd_t cmd, void * pdata)
{
    bsp_int_err_t       err;
    bsp_int_cb_args_t   cb_args;

    err = BSP_INT_SUCCESS;

    switch (cmd)
    {
        case (BSP_INT_CMD_CALL_CALLBACK):
            if (((uint32_t)g_bsp_vectors[vector] != (uint32_t)NULL) &&
                ((uint32_t)g_bsp_vectors[vector] != (uint32_t)FIT_NO_FUNC))
            {
                /* Fill in callback info. */
                cb_args.vector = vector;

                g_bsp_vectors[vector](&cb_args);
            }
            else
            {
                err = BSP_INT_ERR_NO_REGISTERED_CALLBACK;
            }
        break;

        case (BSP_INT_CMD_INTERRUPT_ENABLE):
            err = bsp_interrupt_enable_disable(vector, true);
        break;

        case (BSP_INT_CMD_INTERRUPT_DISABLE):
            err = bsp_interrupt_enable_disable(vector, false);
        break;

        case (BSP_INT_CMD_GROUP_INTERRUPT_ENABLE):
            err = bsp_interrupt_group_enable_disable(vector, true, ((bsp_int_ctrl_t *)pdata)->ipl);
        break;

        case (BSP_INT_CMD_GROUP_INTERRUPT_DISABLE):
            err = bsp_interrupt_group_enable_disable(vector, false, 0);
        break;

        default:
            err = BSP_INT_ERR_INVALID_ARG;
        break;
    }

    return err;
}

/***********************************************************************************************************************
* Function Name: bsp_interrupt_enable_disable
* Description  : Either enables or disables an interrupt.
* Arguments    : vector -
*                    Which vector to enable or disable.
*                enable -
*                    Whether to enable or disable the interrupt.
* Return Value : BSP_INT_SUCCESS -
*                    Interrupt enabled or disabled.
*                BSP_INT_ERR_UNSUPPORTED -
*                    API does not support enabling/disabling for this vector.
***********************************************************************************************************************/
bsp_int_err_t bsp_interrupt_enable_disable (bsp_int_src_t vector, bool enable)
{
    uint32_t      temp_fpsw;
    bsp_int_err_t err = BSP_INT_SUCCESS;

    switch (vector)
    {
        case (BSP_INT_SRC_BUS_ERROR):

            if (true == enable)
            {
                /* Enable the bus error interrupt to catch accesses to illegal/reserved areas of memory */
                /* Clear any pending interrupts */
                IR(BSC,BUSERR) = 0;
                /* Make this the highest priority interrupt (adjust as necessary for your application */
                IPR(BSC,BUSERR) = 0x0F;
                /* Enable the interrupt in the ICU*/
                IEN(BSC,BUSERR) = 1;
                /* Enable illegal address interrupt in the BSC */
                BSC.BEREN.BIT.IGAEN = 1;
                /* Enable timeout detection enable. */
                BSC.BEREN.BIT.TOEN = 1;
            }
            else
            {
                /* Disable the bus error interrupt. */
                /* Disable the interrupt in the ICU*/
                IEN(BSC,BUSERR) = 0;
                /* Disable illegal address interrupt in the BSC */
                BSC.BEREN.BIT.IGAEN = 0;
                /* Disable timeout detection enable. */
                BSC.BEREN.BIT.TOEN = 0;
            }

        break;

        case (BSP_INT_SRC_EXC_FPU):

            /* Get current FPSW. */
            temp_fpsw = (uint32_t)get_fpsw();

            if (true == enable)
            {
                /* Set the FPU exception flags. */
                set_fpsw(temp_fpsw | ((uint32_t)FPU_EXCEPTIONS_ENABLE));
            }
            else
            {
                /* Clear only the FPU exception flags. */
                set_fpsw(temp_fpsw & ((uint32_t)~FPU_EXCEPTIONS_ENABLE));
            }

        break;

        case (BSP_INT_SRC_EXC_NMI_PIN):

            if (true == enable)
            {
                /* Enable NMI pin interrupt (cannot undo!) */
                ICU.NMIER.BIT.NMIEN = 1;
            }
            else
            {
                /* NMI pin interrupts cannot be disabled after being enabled. */
                err = BSP_INT_ERR_UNSUPPORTED;
            }

        break;

        default:
            err = BSP_INT_ERR_UNSUPPORTED;
        break;
    }

    return err;
}

/***********************************************************************************************************************
* Function Name: bsp_interrupt_group_enable_disable
* Description  : Either enables or disables a group interrupt. If a group interrupt is called multiple times to be
*                enabled then it will use the highest given IPL. A group interrupt will only be disabled when all
*                interrupt sources for that group are already disabled.
* Arguments    : vector -
*                    An interrupt source inside the group that is to be enabled/disabled.
*                enable -
*                    Whether to enable or disable the interrupt.
*                ipl -
*                    If enabling a group interrupt, what IPL to use.
* Return Value : BSP_INT_SUCCESS -
*                    Interrupt enabled or disabled.
*                BSP_INT_ERR_INVALID_ARG -
*                    Invalid IPL or vector
*                BSP_INT_ERR_GROUP_STILL_ENABLED -
*                    Not all group interrupts were disabled so group interrupt was not disabled.
***********************************************************************************************************************/
bsp_int_err_t bsp_interrupt_group_enable_disable (bsp_int_src_t vector, bool enable, uint32_t ipl)
{
    bsp_int_err_t err = BSP_INT_SUCCESS;

#if BSP_CFG_PARAM_CHECKING_ENABLE == 1
    /* If interrupt is going to be enabled, verify that IPL is valid. */
    if ((true == enable) && ((ipl == BSP_MCU_IPL_MIN) || (ipl > BSP_MCU_IPL_MAX)))
    {
        return BSP_INT_ERR_INVALID_ARG;
    }
#endif

    if      ((vector >= BSP_INT_SRC_BL0_SCI0_TEI0) && (vector <= BSP_INT_SRC_BL0_PDC_PCERI))
    {
        /* Group BL0. */
        if (true == enable)
        {
            IEN(ICU, GROUPBL0) = 0;
            IR(ICU, GROUPBL0)  = 0;
            IPR(ICU, GROUPBL0) = (uint8_t)(ipl > IPR(ICU, GROUPBL0) ? ipl : IPR(ICU, GROUPBL0));
            IEN(ICU, GROUPBL0) = 1;
        }
        else
        {
            /* Check to make sure all interrupt sources are already disabled for this group. */
            if (0 == ICU.GENBL0.LONG)
            {
                IEN(ICU, GROUPBL0) = 0;
                IPR(ICU, GROUPBL0) = 0;
            }
            else
            {
                err = BSP_INT_ERR_GROUP_STILL_ENABLED;
            }
        }
    }
    else if ((vector >= BSP_INT_SRC_BL1_SDHI_CDETI) && (vector <= BSP_INT_SRC_BL1_RIIC1_EEI1))
    {
        /* Group BL1. */
        if (true == enable)
        {
            IEN(ICU, GROUPBL1) = 0;
            IR(ICU, GROUPBL1)  = 0;
            IPR(ICU, GROUPBL1) = (uint8_t)(ipl > IPR(ICU, GROUPBL1) ? ipl : IPR(ICU, GROUPBL1));
            IEN(ICU, GROUPBL1) = 1;
        }
        else
        {
            /* Check to make sure all interrupt sources are already disabled for this group. */
            if (0 == ICU.GENBL1.LONG)
            {
                IEN(ICU, GROUPBL1) = 0;
                IPR(ICU, GROUPBL1) = 0;
            }
            else
            {
                err = BSP_INT_ERR_GROUP_STILL_ENABLED;
            }
        }
    }
    else if (vector == BSP_INT_SRC_BL2_SDSI_SDIOI)
    {
        /* Group BL2. */
        if (true == enable)
        {
            IEN(ICU, GROUPBL2) = 0;
            IR(ICU, GROUPBL2)  = 0;
            IPR(ICU, GROUPBL2) = (uint8_t)(ipl > IPR(ICU, GROUPBL2) ? ipl : IPR(ICU, GROUPBL2));
            IEN(ICU, GROUPBL2) = 1;
        }
        else
        {
            /* Check to make sure all interrupt sources are already disabled for this group. */
            if (0 == ICU.GENBL2.LONG)
            {
                IEN(ICU, GROUPBL2) = 0;
                IPR(ICU, GROUPBL2) = 0;
            }
            else
            {
                err = BSP_INT_ERR_GROUP_STILL_ENABLED;
            }
        }
    }
    else if ((vector >= BSP_INT_SRC_AL0_SCI10_TEI10) && (vector <= BSP_INT_SRC_AL0_RSPI2_SPEI2))
    {
        /* Group AL0. */
        if (true == enable)
        {
            IEN(ICU, GROUPAL0) = 0;
            IR(ICU, GROUPAL0)  = 0;
            IPR(ICU, GROUPAL0) = (uint8_t)(ipl > IPR(ICU, GROUPAL0) ? ipl : IPR(ICU, GROUPAL0));
            IEN(ICU, GROUPAL0) = 1;
        }
        else
        {
            /* Check to make sure all interrupt sources are already disabled for this group. */
            if (0 == ICU.GENAL0.LONG)
            {
                IEN(ICU, GROUPAL0) = 0;
                IPR(ICU, GROUPAL0) = 0;
            }
            else
            {
                err = BSP_INT_ERR_GROUP_STILL_ENABLED;
            }
        }
    }
    else if ((vector >= BSP_INT_SRC_AL1_EDMAC0_EINT0) && (vector <= BSP_INT_SRC_AL1_DRW2D_DRW_IRQ))
    {
        /* Group AL1. */
        if (true == enable)
        {
            IEN(ICU, GROUPAL1) = 0;
            IR(ICU, GROUPAL1)  = 0;
            IPR(ICU, GROUPAL1) = (uint8_t)(ipl > IPR(ICU, GROUPAL1) ? ipl : IPR(ICU, GROUPAL1));
            IEN(ICU, GROUPAL1) = 1;
        }
        else
        {
            /* Check to make sure all interrupt sources are already disabled for this group. */
            if (0 == ICU.GENAL1.LONG)
            {
                IEN(ICU, GROUPAL1) = 0;
                IPR(ICU, GROUPAL1) = 0;
            }
            else
            {
                err = BSP_INT_ERR_GROUP_STILL_ENABLED;
            }
        }
    }
    else
    {
        /* Vector given was not part of a group. */
        err = BSP_INT_ERR_INVALID_ARG;
    }

    return err;
}


/***********************************************************************************************************************
* Function Name: group_bl0_handler_isr
* Description  : Interrupt handler for Group BL0 interrupts. The way this code works is that for each possible interrupt
*                in this group the following will be performed:
*                1) Test to see if an interrupt is requested for this source
*                2) If an interrupt is requested then the registered callback is called (if one is registered)
*                NOTE: The interrupt request flag must be cleared in the peripheral.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
#pragma interrupt group_bl0_handler_isr(vect=VECT(ICU,GROUPBL0))
void group_bl0_handler_isr (void)
{
    if (1 == ICU.GRPBL0.BIT.IS1)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_BL0_SCI0_ERI0, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    if (1 == ICU.GRPBL0.BIT.IS0)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_BL0_SCI0_TEI0, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    if (1 == ICU.GRPBL0.BIT.IS3)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_BL0_SCI1_ERI1, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    if (1 == ICU.GRPBL0.BIT.IS2)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_BL0_SCI1_TEI1, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    if (1 == ICU.GRPBL0.BIT.IS5)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_BL0_SCI2_ERI2, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    if (1 == ICU.GRPBL0.BIT.IS4)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_BL0_SCI2_TEI2, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    if (1 == ICU.GRPBL0.BIT.IS7)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_BL0_SCI3_ERI3, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    if (1 == ICU.GRPBL0.BIT.IS6)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_BL0_SCI3_TEI3, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    if (1 == ICU.GRPBL0.BIT.IS9)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_BL0_SCI4_ERI4, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    if (1 == ICU.GRPBL0.BIT.IS8)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_BL0_SCI4_TEI4, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    if (1 == ICU.GRPBL0.BIT.IS11)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_BL0_SCI5_ERI5, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    if (1 == ICU.GRPBL0.BIT.IS10)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_BL0_SCI5_TEI5, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    if (1 == ICU.GRPBL0.BIT.IS13)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_BL0_SCI6_ERI6, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    if (1 == ICU.GRPBL0.BIT.IS12)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_BL0_SCI6_TEI6, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    if (1 == ICU.GRPBL0.BIT.IS15)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_BL0_SCI7_ERI7, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    if (1 == ICU.GRPBL0.BIT.IS14)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_BL0_SCI7_TEI7, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    if (1 == ICU.GRPBL0.BIT.IS17)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_BL0_SCI12_ERI12, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    if (1 == ICU.GRPBL0.BIT.IS16)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_BL0_SCI12_TEI12, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    if (1 == ICU.GRPBL0.BIT.IS18)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_BL0_SCI12_SCIX0, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    if (1 == ICU.GRPBL0.BIT.IS19)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_BL0_SCI12_SCIX1, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    if (1 == ICU.GRPBL0.BIT.IS20)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_BL0_SCI12_SCIX2, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    if (1 == ICU.GRPBL0.BIT.IS21)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_BL0_SCI12_SCIX3, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    if (1 == ICU.GRPBL0.BIT.IS24)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_BL0_QSPI_QSPSSLI, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    if (1 == ICU.GRPBL0.BIT.IS26)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_BL0_CAC_FERRI, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    if (1 == ICU.GRPBL0.BIT.IS27)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_BL0_CAC_MENDI, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    if (1 == ICU.GRPBL0.BIT.IS28)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_BL0_CAC_OVFI, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    if (1 == ICU.GRPBL0.BIT.IS29)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_BL0_DOC_DOPCI, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    if (1 == ICU.GRPBL0.BIT.IS31)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_BL0_PDC_PCERI, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    if (1 == ICU.GRPBL0.BIT.IS30)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_BL0_PDC_PCFEI, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }
}

/***********************************************************************************************************************
* Function Name: group_bl1_handler_isr
* Description  : Interrupt handler for Group BL1 interrupts. The way this code works is that for each possible interrupt
*                in this group the following will be performed:
*                1) Test to see if an interrupt is requested for this source
*                2) If an interrupt is requested then the registered callback is called (if one is registered)
*                NOTE: The interrupt request flag must be cleared in the peripheral.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
#pragma interrupt group_bl1_handler_isr(vect=VECT(ICU,GROUPBL1))
void group_bl1_handler_isr (void)
{
    if (1 == ICU.GRPBL1.BIT.IS3)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_BL1_SDHI_CDETI, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    if (1 == ICU.GRPBL1.BIT.IS4)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_BL1_SDHI_CACI, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    if (1 == ICU.GRPBL1.BIT.IS5)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_BL1_SDHI_SDACI, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    if (1 == ICU.GRPBL1.BIT.IS6)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_BL1_MMCIF_CDETIO, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    if (1 == ICU.GRPBL1.BIT.IS7)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_BL1_MMCIF_ERRIO, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    if (1 == ICU.GRPBL1.BIT.IS8)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_BL1_MMCIF_ACCIO, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    if (1 == ICU.GRPBL1.BIT.IS9)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_BL1_POE3_OEI1, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    if (1 == ICU.GRPBL1.BIT.IS10)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_BL1_POE3_OEI2, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    if (1 == ICU.GRPBL1.BIT.IS11)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_BL1_POE3_OEI3, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    if (1 == ICU.GRPBL1.BIT.IS12)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_BL1_POE3_OEI4, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    if (1 == ICU.GRPBL1.BIT.IS14)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_BL1_RIIC0_EEI0, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    if (1 == ICU.GRPBL1.BIT.IS13)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_BL1_RIIC0_TEI0, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    if (1 == ICU.GRPBL1.BIT.IS16)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_BL1_RIIC2_EEI2, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    if (1 == ICU.GRPBL1.BIT.IS15)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_BL1_RIIC2_TEI2, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    if (1 == ICU.GRPBL1.BIT.IS20)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_BL1_S12AD0_S12CMPAI, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    if (1 == ICU.GRPBL1.BIT.IS21)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_BL1_S12AD0_S12CMPBI, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    if (1 == ICU.GRPBL1.BIT.IS22)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_BL1_S12AD1_S12CMPAI1, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    if (1 == ICU.GRPBL1.BIT.IS23)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_BL1_S12AD1_S12CMPBI1, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    if (1 == ICU.GRPBL1.BIT.IS25)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_BL1_SCI8_ERI8, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    if (1 == ICU.GRPBL1.BIT.IS24)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_BL1_SCI8_TEI8, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    if (1 == ICU.GRPBL1.BIT.IS27)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_BL1_SCI9_ERI9, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    if (1 == ICU.GRPBL1.BIT.IS26)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_BL1_SCI9_TEI9, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    if (1 == ICU.GRPBL1.BIT.IS29)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_BL1_RIIC1_EEI1, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    if (1 == ICU.GRPBL1.BIT.IS28)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_BL1_RIIC1_TEI1, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

}

/***********************************************************************************************************************
* Function Name: group_bl2_handler_isr
* Description  : Interrupt handler for Group BL1 interrupts. The way this code works is that for each possible interrupt
*                in this group the following will be performed:
*                1) Test to see if an interrupt is requested for this source
*                2) If an interrupt is requested then the registered callback is called (if one is registered)
*                NOTE: The interrupt request flag must be cleared in the peripheral.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
#pragma interrupt group_bl2_handler_isr(vect=VECT(ICU,GROUPBL2))
void group_bl2_handler_isr (void)
{
    if (1 == ICU.GRPBL2.BIT.IS0)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_BL2_SDSI_SDIOI, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

}

/***********************************************************************************************************************
* Function Name: group_al0_handler_isr
* Description  : Interrupt handler for Group AL0 interrupts. The way this code works is that for each possible interrupt
*                in this group the following will be performed:
*                1) Test to see if an interrupt is requested for this source
*                2) If an interrupt is requested then the registered callback is called (if one is registered)
*                NOTE: The interrupt request flag must be cleared in the peripheral.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
#pragma interrupt group_al0_handler_isr(vect=VECT(ICU,GROUPAL0))
void group_al0_handler_isr (void)
{
    if (1 == ICU.GRPAL0.BIT.IS9)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_AL0_SCI10_ERI10, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    if (1 == ICU.GRPAL0.BIT.IS8)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_AL0_SCI10_TEI10, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    if (1 == ICU.GRPAL0.BIT.IS13)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_AL0_SCI11_ERI11, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    if (1 == ICU.GRPAL0.BIT.IS12)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_AL0_SCI11_TEI11, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

   if (1 == ICU.GRPAL0.BIT.IS17)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_AL0_RSPI0_SPEI0, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    if (1 == ICU.GRPAL0.BIT.IS16)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_AL0_RSPI0_SPII0, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    if (1 == ICU.GRPAL0.BIT.IS19)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_AL0_RSPI1_SPEI1, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    if (1 == ICU.GRPAL0.BIT.IS18)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_AL0_RSPI1_SPII1, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    if (1 == ICU.GRPAL0.BIT.IS21)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_AL0_RSPI2_SPEI2, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    if (1 == ICU.GRPAL0.BIT.IS20)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_AL0_RSPI2_SPII2, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }
}

/***********************************************************************************************************************
* Function Name: group_al1_handler_isr
* Description  : Interrupt handler for Group AL1 interrupts. The way this code works is that for each possible interrupt
*                in this group the following will be performed:
*                1) Test to see if an interrupt is requested for this source
*                2) If an interrupt is requested then the registered callback is called (if one is registered)
*                NOTE: The interrupt request flag must be cleared in the peripheral.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
#pragma interrupt group_al1_handler_isr(vect=VECT(ICU,GROUPAL1))
void group_al1_handler_isr (void)
{
    if (1 == ICU.GRPAL1.BIT.IS4)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_AL1_EDMAC0_EINT0, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    if (1 == ICU.GRPAL1.BIT.IS9)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_AL1_GLCDC_GR1UF, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    if (1 == ICU.GRPAL1.BIT.IS10)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_AL1_GLCDC_GR2UF, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    if (1 == ICU.GRPAL1.BIT.IS8)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_AL1_GLCDC_VPOS, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    if (1 == ICU.GRPAL1.BIT.IS11)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_AL1_DRW2D_DRW_IRQ, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

}

