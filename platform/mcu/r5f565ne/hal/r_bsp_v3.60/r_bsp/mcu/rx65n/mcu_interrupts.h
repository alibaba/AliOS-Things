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
* File Name    : mcu_interrupts.h
* Description  : This module allows for callbacks to be registered for certain interrupts.
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version   Description
*         : 01.10.2016 1.00      First Release
*         : 15.05.2017 2.00      Added the following enumeration constant.
*                                - BSP_INT_SRC_EXRAM
*                                - BSP_INT_SRC_BL1_RIIC1_TEI1
*                                - BSP_INT_SRC_BL1_RIIC1_EEI1
*                                - BSP_INT_SRC_AL1_GLCDC_VPOS
*                                - BSP_INT_SRC_AL1_GLCDC_GR1UF
*                                - BSP_INT_SRC_AL1_GLCDC_GR2UF
*                                - BSP_INT_SRC_AL1_DRW2D_DRW_IRQ
***********************************************************************************************************************/

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
/* Available return codes. */
typedef enum
{
    BSP_INT_SUCCESS = 0,
    BSP_INT_ERR_NO_REGISTERED_CALLBACK,     //There is not a registered callback for this interrupt source
    BSP_INT_ERR_INVALID_ARG,                //Illegal argument input
    BSP_INT_ERR_UNSUPPORTED,                //Operation is not supported by this API
    BSP_INT_ERR_GROUP_STILL_ENABLED         //Not all group interrupts were disabled so group interrupt was not disabled
} bsp_int_err_t;

/* Available interrupts to register a callback for. */
typedef enum
{
    BSP_INT_SRC_EXC_SUPERVISOR_INSTR = 0, //Occurs when privileged instruction is executed in User Mode
    BSP_INT_SRC_EXC_UNDEFINED_INSTR,      //Occurs when MCU encounters an unknown instruction
    BSP_INT_SRC_EXC_NMI_PIN,              //NMI Pin interrupt
    BSP_INT_SRC_EXC_FPU,                  //FPU exception
    BSP_INT_SRC_EXC_ACCESS,               //Access exception
    BSP_INT_SRC_OSC_STOP_DETECT,          //Oscillation stop is detected
    BSP_INT_SRC_WDT_ERROR,                //WDT underflow/refresh error has occurred
    BSP_INT_SRC_IWDT_ERROR,               //IWDT underflow/refresh error has occurred
    BSP_INT_SRC_LVD1,                     //Voltage monitoring 1 interrupt
    BSP_INT_SRC_LVD2,                     //Voltage monitoring 2 interrupt
    BSP_INT_SRC_UNDEFINED_INTERRUPT,      //Interrupt has triggered for a vector that user did not write a handler for
    BSP_INT_SRC_BUS_ERROR,                //Bus error: illegal address access or timeout
    BSP_INT_SRC_RAM,                      //RAM error interrupt
    BSP_INT_SRC_EXRAM,                    //EXRAM error interrupt

    /* BE0 Group Interrupts are all CAN and therefore will be taken care of by the CAN module. */

    /* BL0 Group Interrupts. */
    BSP_INT_SRC_BL0_SCI0_TEI0,
    BSP_INT_SRC_BL0_SCI0_ERI0,
    BSP_INT_SRC_BL0_SCI1_TEI1,
    BSP_INT_SRC_BL0_SCI1_ERI1,
    BSP_INT_SRC_BL0_SCI2_TEI2,
    BSP_INT_SRC_BL0_SCI2_ERI2,
    BSP_INT_SRC_BL0_SCI3_TEI3,
    BSP_INT_SRC_BL0_SCI3_ERI3,
    BSP_INT_SRC_BL0_SCI4_TEI4,
    BSP_INT_SRC_BL0_SCI4_ERI4,
    BSP_INT_SRC_BL0_SCI5_TEI5,
    BSP_INT_SRC_BL0_SCI5_ERI5,
    BSP_INT_SRC_BL0_SCI6_TEI6,
    BSP_INT_SRC_BL0_SCI6_ERI6,
    BSP_INT_SRC_BL0_SCI7_TEI7,
    BSP_INT_SRC_BL0_SCI7_ERI7,
    BSP_INT_SRC_BL0_SCI12_TEI12,
    BSP_INT_SRC_BL0_SCI12_ERI12,
    BSP_INT_SRC_BL0_SCI12_SCIX0,
    BSP_INT_SRC_BL0_SCI12_SCIX1,
    BSP_INT_SRC_BL0_SCI12_SCIX2,
    BSP_INT_SRC_BL0_SCI12_SCIX3,
    BSP_INT_SRC_BL0_QSPI_QSPSSLI,
    BSP_INT_SRC_BL0_CAC_FERRI,
    BSP_INT_SRC_BL0_CAC_MENDI,
    BSP_INT_SRC_BL0_CAC_OVFI,
    BSP_INT_SRC_BL0_DOC_DOPCI,
    BSP_INT_SRC_BL0_PDC_PCFEI,
    BSP_INT_SRC_BL0_PDC_PCERI,

    /* BL1 Group Interrupts. */
    BSP_INT_SRC_BL1_SDHI_CDETI,
    BSP_INT_SRC_BL1_SDHI_CACI,
    BSP_INT_SRC_BL1_SDHI_SDACI,
    BSP_INT_SRC_BL1_MMCIF_CDETIO,
    BSP_INT_SRC_BL1_MMCIF_ERRIO,
    BSP_INT_SRC_BL1_MMCIF_ACCIO,
    BSP_INT_SRC_BL1_POE3_OEI1,
    BSP_INT_SRC_BL1_POE3_OEI2,
    BSP_INT_SRC_BL1_POE3_OEI3,
    BSP_INT_SRC_BL1_POE3_OEI4,
    BSP_INT_SRC_BL1_RIIC0_TEI0,
    BSP_INT_SRC_BL1_RIIC0_EEI0,
    BSP_INT_SRC_BL1_RIIC2_TEI2,
    BSP_INT_SRC_BL1_RIIC2_EEI2,
    BSP_INT_SRC_BL1_S12AD0_S12CMPAI,
    BSP_INT_SRC_BL1_S12AD0_S12CMPBI,
    BSP_INT_SRC_BL1_S12AD1_S12CMPAI1,
    BSP_INT_SRC_BL1_S12AD1_S12CMPBI1,
    BSP_INT_SRC_BL1_SCI8_TEI8,
    BSP_INT_SRC_BL1_SCI8_ERI8,
    BSP_INT_SRC_BL1_SCI9_TEI9,
    BSP_INT_SRC_BL1_SCI9_ERI9,
    BSP_INT_SRC_BL1_RIIC1_TEI1,
    BSP_INT_SRC_BL1_RIIC1_EEI1,

    /* BL2 Group Interrupts. */
    BSP_INT_SRC_BL2_SDSI_SDIOI,

    /* AL0 Group Interrupts. */
    BSP_INT_SRC_AL0_SCI10_TEI10,
    BSP_INT_SRC_AL0_SCI10_ERI10,
    BSP_INT_SRC_AL0_SCI11_TEI11,
    BSP_INT_SRC_AL0_SCI11_ERI11,
    BSP_INT_SRC_AL0_RSPI0_SPII0,
    BSP_INT_SRC_AL0_RSPI0_SPEI0,
    BSP_INT_SRC_AL0_RSPI1_SPII1,
    BSP_INT_SRC_AL0_RSPI1_SPEI1,
    BSP_INT_SRC_AL0_RSPI2_SPII2,
    BSP_INT_SRC_AL0_RSPI2_SPEI2,

    /* AL1 Group Interrupts. */
    BSP_INT_SRC_AL1_EDMAC0_EINT0,
    BSP_INT_SRC_AL1_GLCDC_VPOS,
    BSP_INT_SRC_AL1_GLCDC_GR1UF,
    BSP_INT_SRC_AL1_GLCDC_GR2UF,
    BSP_INT_SRC_AL1_DRW2D_DRW_IRQ,

    BSP_INT_SRC_TOTAL_ITEMS               //DO NOT MODIFY! This is used for sizing the interrupt callback array.
} bsp_int_src_t;

/* Available commands for R_BSP_InterruptControl() function. */
typedef enum
{
    BSP_INT_CMD_CALL_CALLBACK = 0,        //Calls registered callback function if one exists
    BSP_INT_CMD_INTERRUPT_ENABLE,         //Enables a given interrupt (Available for NMI pin, FPU, and Bus Error)
    BSP_INT_CMD_INTERRUPT_DISABLE,        //Disables a given interrupt (Available for FPU, and Bus Error)
    BSP_INT_CMD_GROUP_INTERRUPT_ENABLE,   //Enables a group interrupt when a group interrupt source is given. The
                                          //pdata argument should give the IPL to be used using the bsp_int_ctrl_t
                                          //type. If a group interrupt is enabled multiple times with different IPL
                                          //levels it will use the highest given IPL.
    BSP_INT_CMD_GROUP_INTERRUPT_DISABLE,  //Disables a group interrupt when a group interrupt source is given.
                                          //This will only disable a group interrupt when all interrupt
                                          //sources for that group are already disabled.
} bsp_int_cmd_t;

/* Type to be used for pdata argument in Control function. */
typedef union
{
    uint32_t ipl;                         //Used when enabling an interrupt to set that interrupt's priority level
} bsp_int_ctrl_t;

/* Easy to use typedef for callback functions. */
typedef void (*bsp_int_cb_t)(void *);

/* This structure is the common one that is passed as the 'void *' argument to callback functions when an
 * exception occurs.
 */
typedef struct
{
    bsp_int_src_t vector;         //Which vector caused this interrupt
} bsp_int_cb_args_t;

/***********************************************************************************************************************
Exported global variables
***********************************************************************************************************************/

/***********************************************************************************************************************
Exported global functions (to be accessed by other files)
***********************************************************************************************************************/
bsp_int_err_t R_BSP_InterruptWrite(bsp_int_src_t vector,  bsp_int_cb_t callback);
bsp_int_err_t R_BSP_InterruptRead(bsp_int_src_t vector, bsp_int_cb_t * callback);
bsp_int_err_t R_BSP_InterruptControl(bsp_int_src_t vector, bsp_int_cmd_t cmd, void * pdata);

void bsp_interrupt_open(void); //r_bsp internal function. DO NOT CALL.

