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
* File Name    : vecttbl.c
* Device(s)    : RX65N
* Description  : Definition of the exception vector table, reset vector, and user boot options.
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : DD.MM.YYYY Version   Description
*         : 01.10.2016 1.00      First Release
*         : 15.05.2017 2.00      Deleted unnecessary comments.
*                                Added the following setting.
*                                - Setting of Expansion RAM.
*                                - Setting of Bank Mode Select.
*                                - Setting of Startup Bank Switch.
*                                Added the bsp startup module disable function.
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* BSP configuration. */
#include "rx_platform.h"

/* When using the user startup program, disable the following code. */
#if (BSP_CFG_STARTUP_DISABLE == 0)

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/* Defines CV, CO, CZ, CU, CX, and CE bits. */
#define FPU_CAUSE_FLAGS     (0x000000FC)

/***********************************************************************************************************************
* Function name: PowerON_Reset_PC
* Description  : The reset vector points to this function.  Code execution starts in this function after reset.
* Arguments    : none
* Return value : none
***********************************************************************************************************************/
extern void PowerON_Reset_PC(void);                           

/***********************************************************************************************************************
* Function name: excep_supervisor_inst_isr
* Description  : Supervisor Instruction Violation ISR
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
#pragma interrupt (excep_supervisor_inst_isr)
void excep_supervisor_inst_isr(void)
{
    /* If user has registered a callback for this exception then call it. */
    R_BSP_InterruptControl(BSP_INT_SRC_EXC_SUPERVISOR_INSTR, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
}

/***********************************************************************************************************************
* Function name: excep_access_isr
* Description  : Access exception ISR
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
#pragma interrupt (excep_access_isr)
void excep_access_isr(void)
{
    /* If user has registered a callback for this exception then call it. */
    R_BSP_InterruptControl(BSP_INT_SRC_EXC_ACCESS, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
}

/***********************************************************************************************************************
* Function name: excep_undefined_inst_isr
* Description  : Undefined instruction exception ISR
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
#pragma interrupt (excep_undefined_inst_isr)
void excep_undefined_inst_isr(void)
{
    /* If user has registered a callback for this exception then call it. */
    R_BSP_InterruptControl(BSP_INT_SRC_EXC_UNDEFINED_INSTR, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
}

/***********************************************************************************************************************
* Function name: excep_floating_point_isr
* Description  : Floating point exception ISR
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
#pragma interrupt (excep_floating_point_isr)
void excep_floating_point_isr(void)
{
    /* Used for reading FPSW register. */
    uint32_t temp_fpsw;

    /* If user has registered a callback for this exception then call it. */
    R_BSP_InterruptControl(BSP_INT_SRC_EXC_FPU, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);

    /* Get current FPSW. */
    temp_fpsw = (uint32_t)get_fpsw();
    /* Clear only the FPU exception flags. */
    set_fpsw(temp_fpsw & ((uint32_t)~FPU_CAUSE_FLAGS));
}

/***********************************************************************************************************************
* Function name: non_maskable_isr
* Description  : Non-maskable interrupt ISR
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
#pragma interrupt (non_maskable_isr)
void non_maskable_isr(void)
{
    /* Determine what is the cause of this interrupt. */
    if (1 == ICU.NMISR.BIT.NMIST)
    {
        /* NMI pin interrupt is requested. */
        R_BSP_InterruptControl(BSP_INT_SRC_EXC_NMI_PIN, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);

        /* Clear NMI pin interrupt flag. */
        ICU.NMICLR.BIT.NMICLR = 1;
    }

    if (1 == ICU.NMISR.BIT.OSTST)
    {
        /* Oscillation stop detection interrupt is requested. */
        R_BSP_InterruptControl(BSP_INT_SRC_OSC_STOP_DETECT, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);

        /* Clear oscillation stop detect flag. */
        ICU.NMICLR.BIT.OSTCLR = 1;
    }

    if (1 == ICU.NMISR.BIT.WDTST)
    {
        /* WDT underflow/refresh error interrupt is requested. */
        R_BSP_InterruptControl(BSP_INT_SRC_WDT_ERROR, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);

        /* Clear WDT flag. */
        ICU.NMICLR.BIT.WDTCLR = 1;
    }

    if (1 == ICU.NMISR.BIT.IWDTST)
    {
        /* IWDT underflow/refresh error interrupt is requested. */
        R_BSP_InterruptControl(BSP_INT_SRC_IWDT_ERROR, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);

        /* Clear IWDT flag. */
        ICU.NMICLR.BIT.IWDTCLR = 1;
    }

    if (1 == ICU.NMISR.BIT.LVD1ST)
    {
        /* Voltage monitoring 1 interrupt is requested. */
        R_BSP_InterruptControl(BSP_INT_SRC_LVD1, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);

        /* Clear LVD1 flag. */
        ICU.NMICLR.BIT.LVD1CLR = 1;
    }

    if (1 == ICU.NMISR.BIT.LVD2ST)
    {
        /* Voltage monitoring 1 interrupt is requested. */
        R_BSP_InterruptControl(BSP_INT_SRC_LVD2, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);

        /* Clear LVD2 flag. */
        ICU.NMICLR.BIT.LVD2CLR = 1;
    }

    if (1 == ICU.NMISR.BIT.RAMST)
    {
        if(1 == RAM.RAMSTS.BIT.RAMERR)
        {
            /* RAM Error interrupt is requested. */
            R_BSP_InterruptControl(BSP_INT_SRC_RAM, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);

            /* Clear RAM flags. */
            RAM.RAMSTS.BIT.RAMERR = 0;
        }

        if(1 == RAM.EXRAMSTS.BIT.EXRAMERR)
        {
            /* Expansion RAM Error interrupt is requested. */
            R_BSP_InterruptControl(BSP_INT_SRC_EXRAM, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);

            /* Clear Expansion RAM flags. */
            RAM.EXRAMSTS.BIT.EXRAMERR = 0;
        }
    }
}

/***********************************************************************************************************************
* Function name: undefined_interrupt_source_isr
* Description  : All undefined interrupt vectors point to this function.
*                Set a breakpoint in this function to determine which source is creating unwanted interrupts.
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
#pragma interrupt (undefined_interrupt_source_isr)
void undefined_interrupt_source_isr(void)
{
    /* If user has registered a callback for this exception then call it. */
    R_BSP_InterruptControl(BSP_INT_SRC_UNDEFINED_INTERRUPT, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
}

/***********************************************************************************************************************
* Function name: bus_error_isr
* Description  : By default, this demo code enables the Bus Error Interrupt. This interrupt will fire if the user tries 
*                to access code or data from one of the reserved areas in the memory map, including the areas covered 
*                by disabled chip selects. A nop() statement is included here as a convenient place to set a breakpoint 
*                during debugging and development, and further handling should be added by the user for their 
*                application.
* Arguments    : none
* Return value : none
***********************************************************************************************************************/
#pragma interrupt (bus_error_isr(vect=VECT(BSC,BUSERR)))
void bus_error_isr (void)
{
    /* Clear the bus error */
    BSC.BERCLR.BIT.STSCLR = 1;

    /* 
        To find the address that was accessed when the bus error occurred, read the register BSC.BERSR2.WORD.  The upper
        13 bits of this register contain the upper 13-bits of the offending address (in 512K byte units)
    */

    /* If user has registered a callback for this exception then call it. */
    R_BSP_InterruptControl(BSP_INT_SRC_BUS_ERROR, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
}

/***********************************************************************************************************************
* The following array fills in the UB codes to get into User Boot Mode, the MDEB register, and the User Boot reset
* vector.
***********************************************************************************************************************/
#pragma address __SPCCreg=0xFE7F5D40           /* SPCC register */
const unsigned long __SPCCreg = 0xffffffff;

#pragma address __TMEFreg=0xFE7F5D48           /* TMEF register */
const unsigned long __TMEFreg = BSP_CFG_TRUSTED_MODE_FUNCTION;

#pragma address __OSISreg=0xFE7F5D50           /* OSIS register (ID codes) */
const unsigned long __OSISreg[4] = {
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
};

#pragma address __TMINFreg=0xFE7F5D10          /* TMINF register */
const unsigned long __TMINFreg = 0xffffffff;

#ifdef __BIG
    #define MDE_VALUE (0xfffffff8)    /* big */
#else
    #define MDE_VALUE (0xffffffff)    /* little */
#endif

#if defined(BSP_MCU_RX65N_2MB) /* In the case of 1.5 or 2Mbyte ROM capacity. */
    #if (BSP_CFG_CODE_FLASH_BANK_MODE == 0)
        #define BANK_MODE_VALUE (0xffffff8f)    /* dual */
    #else
        #define BANK_MODE_VALUE (0xffffffff)    /* linear */
    #endif
#else
    #define BANK_MODE_VALUE (0xffffffff)    /* linear */
#endif

#pragma address __MDEreg=0xFE7F5D00             /* MDE register (Single Chip Mode) */
const unsigned long __MDEreg = (MDE_VALUE & BANK_MODE_VALUE);

#pragma address __OFS0reg=0xFE7F5D04            /* OFS0 register */
const unsigned long __OFS0reg = BSP_CFG_OFS0_REG_VALUE;

#pragma address __OFS1reg=0xFE7F5D08            /* OFS1 register */
const unsigned long __OFS1reg = BSP_CFG_OFS1_REG_VALUE;

#pragma address __FAWreg=0xFE7F5D64             /* FAW register */
const unsigned long __FAWreg = BSP_CFG_FAW_REG_VALUE;

#pragma address __ROMCODEreg=0xFE7F5D70         /* ROMCODE register */
const unsigned long __ROMCODEreg = BSP_CFG_ROMCODE_REG_VALUE;

#if defined(BSP_MCU_RX65N_2MB) /* In the case of 1.5 or 2Mbyte ROM capacity. */
    #if (BSP_CFG_CODE_FLASH_START_BANK == 0)
        /* The address range of bank 1 from FFF00000h to FFFFFFFFh and bank 0 from FFE00000h to FFEFFFFFh. */
        #define START_BANK_VALUE (0xffffffff)
    #else
        /* The address range of bank 1 from FFE00000h to FFEFFFFFh and bank 0 from FFF00000h to FFFFFFFFh. */
        #define START_BANK_VALUE (0xfffffff8)
    #endif

    #pragma address __BANKSELreg=0xFE7F5D20 
    const unsigned long __BANKSELreg = START_BANK_VALUE;
#else
    /* Do nothing. */
#endif

/***********************************************************************************************************************
* The following array fills in the exception vector table.
***********************************************************************************************************************/
#pragma section C EXCEPTVECT

void (* const Except_Vectors[])(void) =
{ 
    /* Offset from EXTB: Reserved area - must be all 0xFF */
    (void (*)(void))0xFFFFFFFF,  /* 0x00 - Reserved */
    (void (*)(void))0xFFFFFFFF,  /* 0x04 - Reserved */
    (void (*)(void))0xFFFFFFFF,  /* 0x08 - Reserved */
    (void (*)(void))0xFFFFFFFF,  /* 0x0c - Reserved */
    (void (*)(void))0xFFFFFFFF,  /* 0x10 - Reserved */
    (void (*)(void))0xFFFFFFFF,  /* 0x14 - Reserved */
    (void (*)(void))0xFFFFFFFF,  /* 0x18 - Reserved */
    (void (*)(void))0xFFFFFFFF,  /* 0x1c - Reserved */
    (void (*)(void))0xFFFFFFFF,  /* 0x20 - Reserved */
    (void (*)(void))0xFFFFFFFF,  /* 0x24 - Reserved */
    (void (*)(void))0xFFFFFFFF,  /* 0x28 - Reserved */
    (void (*)(void))0xFFFFFFFF,  /* 0x2c - Reserved */
    (void (*)(void))0xFFFFFFFF,  /* 0x30 - Reserved */
    (void (*)(void))0xFFFFFFFF,  /* 0x34 - Reserved */
    (void (*)(void))0xFFFFFFFF,  /* 0x38 - Reserved */
    (void (*)(void))0xFFFFFFFF,  /* 0x3c - Reserved */
    (void (*)(void))0xFFFFFFFF,  /* 0x40 - Reserved */
    (void (*)(void))0xFFFFFFFF,  /* 0x44 - Reserved */
    (void (*)(void))0xFFFFFFFF,  /* 0x48 - Reserved */
    (void (*)(void))0xFFFFFFFF,  /* 0x4c - Reserved */

    /* Exception vector table */
    excep_supervisor_inst_isr,         /* 0x50  Exception(Supervisor Instruction) */
    excep_access_isr,                  /* 0x54  Exception(Access exception) */
    undefined_interrupt_source_isr,    /* 0x58  Reserved */
    excep_undefined_inst_isr,          /* 0x5c  Exception(Undefined Instruction) */
    undefined_interrupt_source_isr,    /* 0x60  Reserved */
    excep_floating_point_isr,          /* 0x64  Exception(Floating Point) */
    undefined_interrupt_source_isr,    /* 0x68  Reserved */
    undefined_interrupt_source_isr,    /* 0x6c  Reserved */
    undefined_interrupt_source_isr,    /* 0x70  Reserved */
    undefined_interrupt_source_isr,    /* 0x74  Reserved */
    non_maskable_isr,                  /* 0x78  NMI */
};

/***********************************************************************************************************************
* The following array fills in the reset vector.
***********************************************************************************************************************/
#pragma section C RESETVECT

void (* const Reset_Vector[])(void) =
{
    PowerON_Reset_PC                   /* 0xfffffffc  RESET */
};

#endif /* BSP_CFG_STARTUP_DISABLE == 0 */

