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
* File Name    : resetprg.c
* Device(s)    : RX65N
* Description  : Defines post-reset routines that are used to configure the MCU prior to the main program starting. 
*                This is were the program counter starts on power-up or reset.
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : DD.MM.YYYY Version   Description
*         : 01.10.2016 1.00      First Release
*         : 15.05.2017 2.00      Deleted unnecessary comments.
*                                Applied the following technical update.
*                                - TN-RX*-A164A - Added initialization procedure when the realtime clock
*                                                 is not to be used.
*                                Changed the sub-clock oscillator settings.
*                                Added the bsp startup module disable function.
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* Defines MCU configuration functions used in this file */
#include    <_h_c_lib.h>

/* This macro is here so that the stack will be declared here. This is used to prevent multiplication of stack size. */
#define     BSP_DECLARE_STACK
/* Define the target platform */
#include    "rx_platform.h"

/* When using the user startup program, disable the following code. */
#if (BSP_CFG_STARTUP_DISABLE == 0)

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/* If the user chooses only 1 stack then the 'U' bit will not be set and the CPU will always use the interrupt stack. */
#if (BSP_CFG_USER_STACK_ENABLE == 1)
    #define PSW_init  (0x00030000)
#else
    #define PSW_init  (0x00010000)
#endif
#define FPSW_init (0x00000000)  /* Currently nothing set by default. */

#define ROMWT_FREQ_THRESHOLD_01 50000000         // ICLK > 50MHz requires ROMWT register update
#define ROMWT_FREQ_THRESHOLD_02 100000000        // ICLK > 100MHz requires ROMWT register update
/***********************************************************************************************************************
Pre-processor Directives
***********************************************************************************************************************/
/* Set this as the entry point from a power-on reset */
#pragma entry PowerON_Reset_PC

/***********************************************************************************************************************
External function Prototypes
***********************************************************************************************************************/
/* Functions to setup I/O library */
extern void _INIT_IOLIB(void);
extern void _CLOSEALL(void);

#if BSP_CFG_USER_WARM_START_CALLBACK_PRE_INITC_ENABLED != 0
/* If user is requesting warm start callback functions then these are the prototypes. */
void BSP_CFG_USER_WARM_START_PRE_C_FUNCTION(void);
#endif

#if BSP_CFG_USER_WARM_START_CALLBACK_POST_INITC_ENABLED != 0
/* If user is requesting warm start callback functions then these are the prototypes. */
void BSP_CFG_USER_WARM_START_POST_C_FUNCTION(void);
#endif

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/
/* Power-on reset function declaration */
void PowerON_Reset_PC(void);

/* Main program function declaration */
void main(void);
static void operating_frequency_set(void);
static void clock_source_select(void);

/***********************************************************************************************************************
* Function name: PowerON_Reset_PC
* Description  : This function is the MCU's entry point from a power-on reset.
*                The following steps are taken in the startup code:
*                1. The User Stack Pointer (USP) and Interrupt Stack Pointer (ISP) are both set immediately after entry 
*                   to this function. The USP and ISP stack sizes are set in the file bsp_config.h.
*                   Default sizes are USP=4K and ISP=1K.
*                2. The interrupt vector base register is set to point to the beginning of the relocatable interrupt 
*                   vector table.
*                3. The MCU is setup for floating point operations by setting the initial value of the Floating Point 
*                   Status Word (FPSW).
*                4. The MCU operating frequency is set by configuring the Clock Generation Circuit (CGC) in
*                   operating_frequency_set.
*                5. Calls are made to functions to setup the C runtime environment which involves initializing all 
*                   initialed data, zeroing all uninitialized variables, and configuring STDIO if used
*                   (calls to _INITSCT and _INIT_IOLIB).
*                6. Board-specific hardware setup, including configuring I/O pins on the MCU, in hardware_setup.
*                7. Global interrupts are enabled by setting the I bit in the Program Status Word (PSW), and the stack 
*                   is switched from the ISP to the USP.  The initial Interrupt Priority Level is set to zero, enabling 
*                   any interrupts with a priority greater than zero to be serviced.
*                8. The processor is optionally switched to user mode.  To run in user mode, set the macro 
*                   BSP_CFG_RUN_IN_USER_MODE above to a 1.
*                9. The bus error interrupt is enabled to catch any accesses to invalid or reserved areas of memory.
*
*                Once this initialization is complete, the user's main() function is called.  It should not return.
* Arguments    : none
* Return value : none
***********************************************************************************************************************/
void PowerON_Reset_PC(void)
{
    /* Stack pointers are setup prior to calling this function - see comments above */    
    
    /* Initialize the Interrupt Table Register */
    set_intb((void *)__sectop("C$VECT"));

    /* Initialize the Exception Table Register */
    set_extb((void *)__sectop("EXCEPTVECT"));

    /* Initialize FPSW for floating-point operations */
#ifdef __ROZ
#define FPU_ROUND 0x00000001  /* Let FPSW RMbits=01 (round to zero) */
#else 
#define FPU_ROUND 0x00000000  /* Let FPSW RMbits=00 (round to nearest) */
#endif 
#ifdef __DOFF 
#define FPU_DENOM 0x00000100  /* Let FPSW DNbit=1 (denormal as zero) */
#else 
#define FPU_DENOM 0x00000000  /* Let FPSW DNbit=0 (denormal as is) */
#endif 

    set_fpsw(FPSW_init | FPU_ROUND | FPU_DENOM);

    /* Switch to high-speed operation */
    operating_frequency_set();

    /* If the warm start Pre C runtime callback is enabled, then call it. */
#if BSP_CFG_USER_WARM_START_CALLBACK_PRE_INITC_ENABLED == 1
     BSP_CFG_USER_WARM_START_PRE_C_FUNCTION();
#endif

    /* Initialize C runtime environment */
    _INITSCT();

    /* If the warm start Post C runtime callback is enabled, then call it. */
#if BSP_CFG_USER_WARM_START_CALLBACK_POST_INITC_ENABLED == 1
     BSP_CFG_USER_WARM_START_POST_C_FUNCTION();
#endif

#if BSP_CFG_IO_LIB_ENABLE == 1
    /* Comment this out if not using I/O lib */
    _INIT_IOLIB();
#endif

    /* Initialize MCU interrupt callbacks. */
    bsp_interrupt_open();

    /* Initialize register protection functionality. */
    bsp_register_protect_open();

    /* Configure the MCU and board hardware */
    hardware_setup();

    /* Change the MCU's user mode from supervisor to user */
    nop();
    set_psw(PSW_init);      
#if BSP_CFG_RUN_IN_USER_MODE==1
    chg_pmusr() ;
#endif

    /* Enable the bus error interrupt to catch accesses to illegal/reserved areas of memory */
    R_BSP_InterruptControl(BSP_INT_SRC_BUS_ERROR, BSP_INT_CMD_INTERRUPT_ENABLE, FIT_NO_PTR);

    /* Call the main program function (should not return) */
    main();
    
#if BSP_CFG_IO_LIB_ENABLE == 1
    /* Comment this out if not using I/O lib - cleans up open files */
    _CLOSEALL();
#endif

    /* Infinite loop is intended here. */    
    while(1)
    {
        /* Infinite loop. Put a breakpoint here if you want to catch an exit of main(). */
    }
}

/***********************************************************************************************************************
* Function name: operating_frequency_set
* Description  : Configures the clock settings for each of the device clocks
* Arguments    : none
* Return value : none
***********************************************************************************************************************/
static void operating_frequency_set (void)
{
    /* Used for constructing value to write to SCKCR register. */
    uint32_t temp_clock = 0;

    /* 
    Default settings:
    Clock Description              Frequency
    ----------------------------------------
    Input Clock Frequency............  12 MHz
    PLL frequency (x10).............. 120 MHz
    Internal Clock Frequency......... 120 MHz
    Peripheral Clock Frequency A..... 120 MHz
    Peripheral Clock Frequency B.....  60 MHz
    Peripheral Clock Frequency C.....  60 MHz
    Peripheral Clock Frequency D.....  60 MHz
    Flash IF Clock Frequency.........  60 MHz
    External Bus Clock Frequency..... 120 MHz
    USB Clock Frequency..............  24 MHz */

    /* Protect off. */
    SYSTEM.PRCR.WORD = 0xA50B;

    /* Select the clock based upon user's choice. */
    clock_source_select();

    /* Figure out setting for FCK bits. */
#if   BSP_CFG_FCK_DIV == 1
    /* Do nothing since FCK bits should be 0. */
#elif BSP_CFG_FCK_DIV == 2
    temp_clock |= 0x10000000;
#elif BSP_CFG_FCK_DIV == 4
    temp_clock |= 0x20000000;
#elif BSP_CFG_FCK_DIV == 8
    temp_clock |= 0x30000000;
#elif BSP_CFG_FCK_DIV == 16
    temp_clock |= 0x40000000;
#elif BSP_CFG_FCK_DIV == 32
    temp_clock |= 0x50000000;
#elif BSP_CFG_FCK_DIV == 64
    temp_clock |= 0x60000000;
#else
    #error "Error! Invalid setting for BSP_CFG_FCK_DIV in r_bsp_config.h"
#endif

    /* Figure out setting for ICK bits. */
#if   BSP_CFG_ICK_DIV == 1
    /* Do nothing since ICK bits should be 0. */
#elif BSP_CFG_ICK_DIV == 2
    temp_clock |= 0x01000000;
#elif BSP_CFG_ICK_DIV == 4
    temp_clock |= 0x02000000;
#elif BSP_CFG_ICK_DIV == 8
    temp_clock |= 0x03000000;
#elif BSP_CFG_ICK_DIV == 16
    temp_clock |= 0x04000000;
#elif BSP_CFG_ICK_DIV == 32
    temp_clock |= 0x05000000;
#elif BSP_CFG_ICK_DIV == 64
    temp_clock |= 0x06000000;
#else
    #error "Error! Invalid setting for BSP_CFG_ICK_DIV in r_bsp_config.h"
#endif

    /* Figure out setting for BCK bits. */
#if   BSP_CFG_BCK_DIV == 1
    /* Do nothing since BCK bits should be 0. */
#elif BSP_CFG_BCK_DIV == 2
    temp_clock |= 0x00010000;
#elif BSP_CFG_BCK_DIV == 4
    temp_clock |= 0x00020000;
#elif BSP_CFG_BCK_DIV == 8
    temp_clock |= 0x00030000;
#elif BSP_CFG_BCK_DIV == 16
    temp_clock |= 0x00040000;
#elif BSP_CFG_BCK_DIV == 32
    temp_clock |= 0x00050000;
#elif BSP_CFG_BCK_DIV == 64
    temp_clock |= 0x00060000;
#else
    #error "Error! Invalid setting for BSP_CFG_BCK_DIV in r_bsp_config.h"
#endif

    /* Configure PSTOP1 bit for BCLK output. */
#if BSP_CFG_BCLK_OUTPUT == 0    
    /* Set PSTOP1 bit */
    temp_clock |= 0x00800000;
#elif BSP_CFG_BCLK_OUTPUT == 1
    /* Clear PSTOP1 bit */
    temp_clock &= ~0x00800000;
#elif BSP_CFG_BCLK_OUTPUT == 2
    /* Clear PSTOP1 bit */
    temp_clock &= ~0x00800000;
    /* Set BCLK divider bit */
    SYSTEM.BCKCR.BIT.BCLKDIV = 1;
#else
    #error "Error! Invalid setting for BSP_CFG_BCLK_OUTPUT in r_bsp_config.h"
#endif

    /* Configure PSTOP0 bit for SDCLK output. */
#if BSP_CFG_SDCLK_OUTPUT == 0    
    /* Set PSTOP0 bit */
    temp_clock |= 0x00400000;
#elif BSP_CFG_SDCLK_OUTPUT == 1
    /* Clear PSTOP0 bit */
    temp_clock &= ~0x00400000;
#else
    #error "Error! Invalid setting for BSP_CFG_SDCLK_OUTPUT in r_bsp_config.h"
#endif

    /* Figure out setting for PCKA bits. */
#if   BSP_CFG_PCKA_DIV == 1
    /* Do nothing since PCKA bits should be 0. */
#elif BSP_CFG_PCKA_DIV == 2
    temp_clock |= 0x00001000;
#elif BSP_CFG_PCKA_DIV == 4
    temp_clock |= 0x00002000;
#elif BSP_CFG_PCKA_DIV == 8
    temp_clock |= 0x00003000;
#elif BSP_CFG_PCKA_DIV == 16
    temp_clock |= 0x00004000;
#elif BSP_CFG_PCKA_DIV == 32
    temp_clock |= 0x00005000;
#elif BSP_CFG_PCKA_DIV == 64
    temp_clock |= 0x00006000;
#else
    #error "Error! Invalid setting for BSP_CFG_PCKA_DIV in r_bsp_config.h"
#endif

    /* Figure out setting for PCKB bits. */
#if   BSP_CFG_PCKB_DIV == 1
    /* Do nothing since PCKB bits should be 0. */
#elif BSP_CFG_PCKB_DIV == 2
    temp_clock |= 0x00000100;
#elif BSP_CFG_PCKB_DIV == 4
    temp_clock |= 0x00000200;
#elif BSP_CFG_PCKB_DIV == 8
    temp_clock |= 0x00000300;
#elif BSP_CFG_PCKB_DIV == 16
    temp_clock |= 0x00000400;
#elif BSP_CFG_PCKB_DIV == 32
    temp_clock |= 0x00000500;
#elif BSP_CFG_PCKB_DIV == 64
    temp_clock |= 0x00000600;
#else
    #error "Error! Invalid setting for BSP_CFG_PCKB_DIV in r_bsp_config.h"
#endif

    /* Figure out setting for PCKC bits. */
#if   BSP_CFG_PCKC_DIV == 1
    /* Do nothing since PCKA bits should be 0. */
#elif BSP_CFG_PCKC_DIV == 2
    temp_clock |= 0x00000010;
#elif BSP_CFG_PCKC_DIV == 4
    temp_clock |= 0x00000020;
#elif BSP_CFG_PCKC_DIV == 8
    temp_clock |= 0x00000030;
#elif BSP_CFG_PCKC_DIV == 16
    temp_clock |= 0x00000040;
#elif BSP_CFG_PCKC_DIV == 32
    temp_clock |= 0x00000050;
#elif BSP_CFG_PCKC_DIV == 64
    temp_clock |= 0x00000060;
#else
    #error "Error! Invalid setting for BSP_CFG_PCKC_DIV in r_bsp_config.h"
#endif

    /* Figure out setting for PCKD bits. */
#if   BSP_CFG_PCKD_DIV == 1
    /* Do nothing since PCKD bits should be 0. */
#elif BSP_CFG_PCKD_DIV == 2
    temp_clock |= 0x00000001;
#elif BSP_CFG_PCKD_DIV == 4
    temp_clock |= 0x00000002;
#elif BSP_CFG_PCKD_DIV == 8
    temp_clock |= 0x00000003;
#elif BSP_CFG_PCKD_DIV == 16
    temp_clock |= 0x00000004;
#elif BSP_CFG_PCKD_DIV == 32
    temp_clock |= 0x00000005;
#elif BSP_CFG_PCKD_DIV == 64
    temp_clock |= 0x00000006;
#else
    #error "Error! Invalid setting for BSP_CFG_PCKD_DIV in r_bsp_config.h"
#endif

    /* Set SCKCR register. */
    SYSTEM.SCKCR.LONG = temp_clock;

    /* Re-init temp_clock to use to set SCKCR2. */
    temp_clock = 0;

    /* Figure out setting for UCK bits. */
#if   BSP_CFG_UCK_DIV == 2
    temp_clock |= 0x00000011;
#elif BSP_CFG_UCK_DIV == 3
    temp_clock |= 0x00000021;
#elif BSP_CFG_UCK_DIV == 4
    temp_clock |= 0x00000031;
#elif BSP_CFG_UCK_DIV == 5
    temp_clock |= 0x00000041;
#else
    #error "Error! Invalid setting for BSP_CFG_UCK_DIV in r_bsp_config.h"
#endif

    /* Set SCKCR2 register. */
    SYSTEM.SCKCR2.WORD = (uint16_t)temp_clock;

    /* Choose clock source. Default for r_bsp_config.h is PLL. */
    SYSTEM.SCKCR3.WORD = ((uint16_t)BSP_CFG_CLOCK_SOURCE) << 8;

#if (BSP_CFG_CLOCK_SOURCE != 0)
    /* We can now turn LOCO off since it is not going to be used. */
    SYSTEM.LOCOCR.BYTE = 0x01;
#endif

#if (BSP_CFG_ROM_CACHE_ENABLE == 1)
    FLASH.ROMCIV.WORD = 0x0001;
    while (FLASH.ROMCIV.WORD != 0x0000)
    {
        /* wait for bit to set */
    }
    
    FLASH.ROMCE.WORD = 0x0001;
    while (FLASH.ROMCE.WORD != 0x0001)
    {
        /* wait for bit to set */
    }
#endif

    /* Protect on. */
    SYSTEM.PRCR.WORD = 0xA500;
}

/***********************************************************************************************************************
* Function name: clock_source_select
* Description  : Enables and disables clocks as chosen by the user. This function also implements the delays
*                needed for the clocks to stabilize.
* Arguments    : none
* Return value : none
***********************************************************************************************************************/
static void clock_source_select (void)
{
    volatile uint8_t i;
    volatile uint8_t dummy;
    volatile uint8_t tmp;

    /* Main clock will be not oscillate in software standby or deep software standby modes. */
    SYSTEM.MOFCR.BIT.MOFXIN = 0;

    /* Set the oscillation source of the main clock oscillator. */
    SYSTEM.MOFCR.BIT.MOSEL = BSP_CFG_MAIN_CLOCK_SOURCE;

    /* Use HOCO if HOCO is chosen or if PLL is chosen with HOCO as source. */
#if (BSP_CFG_CLOCK_SOURCE == 1) || ((BSP_CFG_CLOCK_SOURCE == 4) && (BSP_CFG_PLL_SRC == 1))
    /* HOCO is chosen. Start it operating if it is not already operating. */
    if (1 == SYSTEM.HOCOCR.BIT.HCSTP)
    {
        /* Turn on power to HOCO. */
        SYSTEM.HOCOPCR.BYTE = 0x00;

        /* Stop HOCO. */
        SYSTEM.HOCOCR.BYTE = 0x01;

        while(1 == SYSTEM.OSCOVFSR.BIT.HCOVF)
        {
            /* The delay period needed is to make sure that the HOCO has stopped. */
        }

        /* Set HOCO frequency. */
        #if   (BSP_CFG_HOCO_FREQUENCY == 0)
        SYSTEM.HOCOCR2.BYTE = 0x00;         //16MHz
        #elif (BSP_CFG_HOCO_FREQUENCY == 1)
        SYSTEM.HOCOCR2.BYTE = 0x01;         //18MHz
        #elif (BSP_CFG_HOCO_FREQUENCY == 2)
        SYSTEM.HOCOCR2.BYTE = 0x02;         //20MHz
        #else
            #error "Error! Invalid setting for BSP_CFG_HOCO_FREQUENCY in r_bsp_config.h"
        #endif

        /* HOCO is chosen. Start it operating. */
        SYSTEM.HOCOCR.BYTE = 0x00;

        if(0x00 ==  SYSTEM.HOCOCR.BYTE)
        {
            /* Dummy read and compare. cf."5. I/O Registers", "(2) Notes on writing to I/O registers" in User's manual. 
               This is done to ensure that the register has been written before the next register access. The RX has a 
               pipeline architecture so the next instruction could be executed before the previous write had finished. */
            nop();
        }
    }

    while(0 == SYSTEM.OSCOVFSR.BIT.HCOVF)
    {
        /* The delay period needed is to make sure that the HOCO has stabilized. */    
    }
#else
    /* If HOCO is already operating, it doesn't stop.  */
    if (1 == SYSTEM.HOCOCR.BIT.HCSTP)
    {
        /* Turn off power to HOCO. */
        SYSTEM.HOCOPCR.BYTE = 0x01;
    }
    else
    {
        while(0 == SYSTEM.OSCOVFSR.BIT.HCOVF)
        {
            /* The delay period needed is to make sure that the HOCO has stabilized. */
        }
    }
#endif

    /* Use Main clock if Main clock is chosen or if PLL is chosen with Main clock as source. */
#if (BSP_CFG_CLOCK_SOURCE == 2) || ((BSP_CFG_CLOCK_SOURCE == 4) && (BSP_CFG_PLL_SRC == 0))
    /* Main clock oscillator is chosen. Start it operating. */
    
    /* If the main oscillator is >10MHz then the main clock oscillator forced oscillation control register (MOFCR) must
       be changed. */
    if (BSP_CFG_XTAL_HZ > 20000000)
    {
        /* 20 - 24MHz. */
        SYSTEM.MOFCR.BIT.MODRV2 = 0;
    }
    else if (BSP_CFG_XTAL_HZ > 16000000)
    {
        /* 16 - 20MHz. */
        SYSTEM.MOFCR.BIT.MODRV2 = 1;
    }
    else if (BSP_CFG_XTAL_HZ > 8000000)
    {
        /* 8 - 16MHz. */
        SYSTEM.MOFCR.BIT.MODRV2 = 2;
    }
    else
    {
        /* 8MHz. */
        SYSTEM.MOFCR.BIT.MODRV2 = 3;
    }

    /* Set the oscillation stabilization wait time of the main clock oscillator. */
#if (BSP_CFG_MAIN_CLOCK_SOURCE == 0) /* Resonator */
    SYSTEM.MOSCWTCR.BYTE = BSP_CFG_MOSC_WAIT_TIME;
#elif (BSP_CFG_MAIN_CLOCK_SOURCE == 1) /* External oscillator input */
    SYSTEM.MOSCWTCR.BYTE = 0x00;
#else
    #error "Error! Invalid setting for BSP_CFG_MAIN_CLOCK_SOURCE in r_bsp_config.h"
#endif 

    /* Set the main clock to operating. */
    SYSTEM.MOSCCR.BYTE = 0x00;

    if(0x00 ==  SYSTEM.MOSCCR.BYTE)
    {
        /* Dummy read and compare. cf."5. I/O Registers", "(2) Notes on writing to I/O registers" in User's manual. 
           This is done to ensure that the register has been written before the next register access. The RX has a 
           pipeline architecture so the next instruction could be executed before the previous write had finished. */    
        nop();
    }

    while(0 == SYSTEM.OSCOVFSR.BIT.MOOVF)
    {        
        /* The delay period needed is to make sure that the Main clock has stabilized. */
    }
#else
    /* Set the main clock to stopped. */
    SYSTEM.MOSCCR.BYTE = 0x01;

    /* Dummy read and compare. cf."5. I/O Registers", "(2) Notes on writing to I/O registers" in User's manual. */
    if(0x01 ==  SYSTEM.MOSCCR.BYTE)
    {
        /* Dummy read and compare. cf."5. I/O Registers", "(2) Notes on writing to I/O registers" in User's manual. 
           This is done to ensure that the register has been written before the next register access. The RX has a 
           pipeline architecture so the next instruction could be executed before the previous write had finished. */    
        nop();
    }
#endif

    /* Sub-clock setting. */
    
    /* Cold start setting */
    if (0 == SYSTEM.RSTSR1.BIT.CWSF)
    {
        /* Stop the sub-clock oscillator */
        /* RCR4 - RTC Control Register 4
        b7:b1    Reserved - The write value should be 0.
        b0       RCKSEL   - Count Source Select - Sub-clock oscillator is selected. */
        RTC.RCR4.BIT.RCKSEL = 0;

        /* dummy read four times */
        for (i = 0; i < 4; i++)
        {
            dummy = RTC.RCR4.BYTE;
        }

        if (0 != RTC.RCR4.BIT.RCKSEL)
        {
            /* Confirm that the written */
            nop();
        }

        /* RCR3 - RTC Control Register 3
        b7:b4    Reserved - The write value should be 0.
        b3:b1    RTCDV    - Sub-clock oscillator Drive Ability Control.
        b0       RTCEN    - Sub-clock oscillator is stopped. */
        RTC.RCR3.BIT.RTCEN = 0;

        /* dummy read four times */
        for (i = 0; i < 4; i++)
        {
            dummy = RTC.RCR3.BYTE;
        }

        if (0 != RTC.RCR3.BIT.RTCEN)
        {
            /* Confirm that the written */
            nop();
        }

        /* SOSCCR - Sub-Clock Oscillator Control Register
        b7:b1    Reserved - The write value should be 0.
        b0       SOSTP    - Sub-clock oscillator Stop - Sub-clock oscillator is stopped. */
        SYSTEM.SOSCCR.BYTE = 0x01;

        if (0x01 != SYSTEM.SOSCCR.BYTE)
        {
            /* Dummy read and compare. cf."5. I/O Registers", "(2) Notes on writing to I/O registers" in User's manual. 
               This is done to ensure that the register has been written before the next register access. The RX has a 
               pipeline architecture so the next instruction could be executed before the previous write had finished. */    
            nop();
        }

        while (0 != SYSTEM.OSCOVFSR.BIT.SOOVF)
        {        
            /* The delay period needed is to make sure that the sub-clock has stopped. */
        }

#if (BSP_CFG_CLOCK_SOURCE == 3) || (BSP_CFG_RTC_ENABLE == 1)
        /* Set the drive capacity of the sub-clock oscillator */
        #if   (BSP_CFG_SOSC_DRV_CAP == 0) /* Standard CL */
            tmp = 0x06;
        #elif (BSP_CFG_SOSC_DRV_CAP == 1) /* Low CL */
            tmp = 0x01;
        #else
            #error "Error! Invalid setting for BSP_CFG_SOSC_DRV_CAP in r_bsp_config.h"
        #endif 

        RTC.RCR3.BIT.RTCDV = tmp;

        /* dummy read four times */
        for (i = 0; i < 4; i++)
        {
            dummy = RTC.RCR3.BYTE;
        }

        if (tmp != RTC.RCR3.BIT.RTCDV)
        {
            /* Confirm that the written */
            nop();
        }

        /* Set wait time until the sub-clock oscillator stabilizes */
        /* SOSCWTCR - Sub-Clock Oscillator Wait Control Register
        b7:b5    Reserved - The write value should be 0.
        b4:b0    SSTS - Sub-Clock Oscillator Waiting Time - Waiting time is 2.044ms(the maximum value of fLOCO). */
        SYSTEM.SOSCWTCR.BYTE = BSP_CFG_SOSC_WAIT_TIME;

        /* Operate the Sub-clock oscillator */
        SYSTEM.SOSCCR.BYTE = 0x00;

        if (0x00 != SYSTEM.SOSCCR.BYTE)
        {
            /* Dummy read and compare. cf."5. I/O Registers", "(2) Notes on writing to I/O registers" in User's manual. 
               This is done to ensure that the register has been written before the next register access. The RX has a 
               pipeline architecture so the next instruction could be executed before the previous write had finished. */    
            nop();
        }

        while (1 != SYSTEM.OSCOVFSR.BIT.SOOVF)
        {
            /* The delay period needed is to make sure that the sub-clock  has stabilized. */
        }
#endif

#if (BSP_CFG_RTC_ENABLE == 1)
        /* ---- Set wait time until the sub-clock oscillator stabilizes ---- */
        SYSTEM.SOSCWTCR.BYTE = 0x00;

        /* ---- Operate the sub-clock oscillator ---- */
        RTC.RCR3.BIT.RTCEN = 1;

        /* dummy read four times */
        for (i = 0; i < 4; i++)
        {
             dummy = RTC.RCR3.BIT.RTCEN;
        }

        if (1 != RTC.RCR3.BIT.RTCEN)
        {
             /* Confirm that the written value can be read correctly. */
             nop();
        }
#endif

#if (BSP_CFG_CLOCK_SOURCE == 3) || (BSP_CFG_RTC_ENABLE == 1)
        /* Wait for six the sub-clock cycles */
        /* 6 count of sub-clock : (1000000/32768)*6=183.10546875us
           In the case of LOCO frequency is 264kHz : 183.10546875/(1000000/264000)=48.33984375cycle
           (48.33984375+2)*(1000000/240000)=209.7493489583333us ("+2" is overhead cycle) */
        R_BSP_SoftwareDelay((uint32_t)210, BSP_DELAY_MICROSECS);
#endif

#if (BSP_CFG_CLOCK_SOURCE == 3) && (BSP_CFG_RTC_ENABLE == 0)
        /* Stop prescaler and counter */
        /* RCR2 - RTC Control Register 2
        b7  CNTMD - Count Mode Select - The calendar count mode.
        b6  HR24  - Hours Mode - The RTC operates in 24-hour mode.
        b5  AADJP - Automatic Adjustment Period Select - The RADJ.ADJ[5:0] setting value is adjusted from 
                                                           the count value of the prescaler every 10 seconds.
        b4  AADJE - Automatic Adjustment Enable - Automatic adjustment is enabled.
        b3  RTCOE - RTCOUT Output Enable - RTCOUT output enabled.
        b2  ADJ30 - 30-Second Adjustment - 30-second adjustment is executed.
        b1  RESET - RTC Software Reset - The prescaler and the target registers for RTC software reset are initialized.
        b0  START - start - Prescaler is stopped. */
        RTC.RCR2.BYTE &= 0x7E;

        while (0 != RTC.RCR2.BIT.START)
        {
            /* Confirm that the written value can be read correctly. */
        }

        /* RTC Software Reset */
        RTC.RCR2.BIT.RESET = 1;

        while (0 != RTC.RCR2.BIT.RESET)
        {
            /* Confirm that the written value can be read correctly. */
        }

        /* An alarm interrupt request is disabled */
        /* RCR1 - RTC Control Register 1
        b7:b4  PES   - Periodic Interrupt Select - These bits specify the period for the periodic interrupt.
        b3     RTCOS - RTCOUT Output Select - RTCOUT outputs 1 Hz.
        b2     PIE   - Periodic Interrupt Enable - A periodic interrupt request is disabled.
        b1     CIE   - Carry Interrupt Enable - A carry interrupt request is disabled.
        b0     AIE   - Alarm Interrupt Enable - An alarm interrupt request is disabled. */
        RTC.RCR1.BYTE &= 0xF8;

        /* Wait for one the sub-clock cycles */
        /* 1 count of sub-clock : (1000000/32768)=30.517578125us
           In the case of LOCO frequency is 264kHz : 30.517578125/(1000000/264000)=8.056640625cycle
           (8.056640625+2)*(1000000/240000)=41.902669270833us ("+2" is overhead cycle) */
        R_BSP_SoftwareDelay((uint32_t)42, BSP_DELAY_MICROSECS);

        if (0x00 != (RTC.RCR1.BYTE & 0x07))
        {
            /* Confirm that the written value can be read correctly. */
            nop();
        }
#endif
    }
    /* Warm start setting */
    else
    {
#if (BSP_CFG_CLOCK_SOURCE == 3) || ((BSP_CFG_CLOCK_SOURCE != 3) && (BSP_CFG_RTC_ENABLE == 0))
        /* SOSCCR - Sub-Clock Oscillator Control Register
        b7:b1    Reserved - The write value should be 0.
        b0       SOSTP    - Sub-clock oscillator Stop - Sub-clock oscillator is stopped. */
        SYSTEM.SOSCCR.BYTE = 0x01;

        if (0x01 != SYSTEM.SOSCCR.BYTE)
        {
            /* Dummy read and compare. cf."5. I/O Registers", "(2) Notes on writing to I/O registers" in User's manual. 
               This is done to ensure that the register has been written before the next register access. The RX has a 
               pipeline architecture so the next instruction could be executed before the previous write had finished. */    
            nop();
        }

        while (0 != SYSTEM.OSCOVFSR.BIT.SOOVF)
        {
            /* Confirm that the Sub clock stopped. */
        }
#endif

#if (BSP_CFG_CLOCK_SOURCE == 3)
        /* Set wait time until the sub-clock oscillator stabilizes */
        /* SOSCWTCR - Sub-Clock Oscillator Wait Control Register
        b7:b5    Reserved - The write value should be 0.
        b4:b0    SSTS - Sub-Clock Oscillator Waiting Time - Waiting time is 2.044ms(the maximum value of fLOCO). */
        SYSTEM.SOSCWTCR.BYTE = BSP_CFG_SOSC_WAIT_TIME;

        /* Operate the Sub-clock oscillator */
        SYSTEM.SOSCCR.BYTE = 0x00;

        if (0x00 != SYSTEM.SOSCCR.BYTE)
        {
            /* Dummy read and compare. cf."5. I/O Registers", "(2) Notes on writing to I/O registers" in User's manual. 
               This is done to ensure that the register has been written before the next register access. The RX has a 
               pipeline architecture so the next instruction could be executed before the previous write had finished. */    
            nop();
        }
#endif

#if (BSP_CFG_CLOCK_SOURCE == 3) || (BSP_CFG_RTC_ENABLE == 1)
        while (1 != SYSTEM.OSCOVFSR.BIT.SOOVF)
        {
            /* The delay period needed is to make sure that the sub-clock  has stabilized. */
        }
#endif

#if (BSP_CFG_RTC_ENABLE == 1)
        /* ---- Set wait time until the sub-clock oscillator stabilizes ---- */
        SYSTEM.SOSCWTCR.BYTE = 0x00;
#endif
    }

#if (BSP_CFG_CLOCK_SOURCE == 4)

    /* Set PLL Input Divisor. */
    SYSTEM.PLLCR.BIT.PLIDIV = BSP_CFG_PLL_DIV - 1;

    #if (BSP_CFG_PLL_SRC == 0)
    /* Clear PLL clock source if PLL clock source is Main clock.  */
    SYSTEM.PLLCR.BIT.PLLSRCSEL = 0;
    #else
    /* Set PLL clock source if PLL clock source is HOCO clock.  */
    SYSTEM.PLLCR.BIT.PLLSRCSEL = 1;
    #endif

    /* Set PLL Multiplier. */
    SYSTEM.PLLCR.BIT.STC = ((uint8_t)((float)BSP_CFG_PLL_MUL * 2.0)) - 1;

    /* Set the PLL to operating. */
    SYSTEM.PLLCR2.BYTE = 0x00;

    while(0 == SYSTEM.OSCOVFSR.BIT.PLOVF)
    {
        /* The delay period needed is to make sure that the PLL has stabilized. */
    }

#else
    /* Set the PLL to stopped. */
    SYSTEM.PLLCR2.BYTE = 0x01;
#endif

    /* LOCO is saved for last since it is what is running by default out of reset. This means you do not want to turn
       it off until another clock has been enabled and is ready to use. */
#if (BSP_CFG_CLOCK_SOURCE == 0)
    /* LOCO is chosen. This is the default out of reset. */
    SYSTEM.LOCOCR.BYTE = 0x00;
#else
    /* LOCO is not chosen but it cannot be turned off yet since it is still being used. */
#endif

    /* Make sure a valid clock was chosen. */
#if (BSP_CFG_CLOCK_SOURCE > 4) || (BSP_CFG_CLOCK_SOURCE < 0)
    #error "ERROR - Valid clock source must be chosen in r_bsp_config.h using BSP_CFG_CLOCK_SOURCE macro."
#endif

    /* RX65N has a ROMWT register which controls the cycle waiting for access to code flash memory.
       It is set as zero coming out of reset. 
       When setting ICLK to [50 MHz < ICLK <= 100 MHz], set the ROMWT.ROMWT[1:0] bits to 01b. 
       When setting ICLK to [100 MHz < ICLK <= 120 MHz], set the ROMWT.ROMWT[1:0] bits to 10b. */
    if (BSP_ICLK_HZ > ROMWT_FREQ_THRESHOLD_02)
    {
        SYSTEM.ROMWT.BYTE = 0x02;
    }
    else if (BSP_ICLK_HZ > ROMWT_FREQ_THRESHOLD_01)
    {
        SYSTEM.ROMWT.BYTE = 0x01;
    }
    else
    {
        /* Do nothing. */
    }

}

#endif /* BSP_CFG_STARTUP_DISABLE == 0 */

