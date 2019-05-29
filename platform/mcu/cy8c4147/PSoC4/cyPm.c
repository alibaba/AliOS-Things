/***************************************************************************//**
* \file cyPm.c
* \version 5.70
*
* \brief Provides an API for the power management.
*
* \note Documentation of the API's in this file is located in the System
* Reference Guide provided with PSoC Creator.
*
********************************************************************************
* \copyright
* Copyright 2011-2018, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "cyPm.h"
#include "CyLib.h"
#include "CyFlash.h"


/*******************************************************************************
* Function Name: CySysPmSleep
****************************************************************************//**
*
* Puts the part into the Sleep state. This is a CPU-centric power mode.
* It means that the CPU has indicated that it is in the sleep mode and
* its main clock can be removed. It is identical to Active from a peripheral
* point of view. Any enabled interrupts can cause wakeup from the Sleep mode.
*
*******************************************************************************/
void CySysPmSleep(void)
{
    uint8 interruptState;

    interruptState = CyEnterCriticalSection();

    /* CPU enters Sleep mode upon execution of WFI */
    CY_PM_CPU_SCR_REG &= (uint32) (~CY_PM_CPU_SCR_SLEEPDEEP);

    /* Sleep and wait for interrupt */
    CY_PM_WFI;

    CyExitCriticalSection(interruptState);
}


/*******************************************************************************
* Function Name: CySysPmDeepSleep
****************************************************************************//**
*
* Puts the part into the Deep Sleep state. If the firmware attempts to enter
* this mode before the system is ready (that is, when
* PWR_CONTROL.LPM_READY = 0), then the device will go into the Sleep mode
* instead and automatically enter the originally intended mode when the
* holdoff expires.
*
* The wakeup occurs when an interrupt is received from a DeepSleep or
* Hibernate peripheral. For more details, see a corresponding
* peripheral's datasheet.
*
*******************************************************************************/
void CySysPmDeepSleep(void)
{
    uint8 interruptState;
    #if(CY_IP_SRSSV2)
        volatile uint32 clkSelectReg;
    #endif /* (CY_IP_SRSSV2) */
    
    #if(CY_IP_ECO_SRSSLT)
        volatile uint32 pllResoreFlag = 0u;
    #endif /* (CY_IP_ECO_SRSSLT) */
    
    interruptState = CyEnterCriticalSection();

    #if(CY_IP_ECO_SRSSLT)
        if(0u != (CY_SYS_ECO_CLK_SELECT_REG & CY_SYS_ECO_CLK_SELECT_ECO_PLL_MASK))
        {
            pllResoreFlag = 1u;
            
            /* Set default state = IMO for HFCLK_SEL bit mask */
            CY_SYS_CLK_SELECT_REG &= (uint32)(~CY_SYS_CLK_SELECT_DIRECT_SEL_MASK);
        }
    #endif /* (CY_IP_ECO_SRSSLT) */

    #if(CY_IP_SRSSV2)
        /* Device enters DeepSleep mode when CPU asserts SLEEPDEEP signal */
        CY_PM_PWR_CONTROL_REG &= (uint32) (~CY_PM_PWR_CONTROL_HIBERNATE);
    #endif /* (CY_IP_SRSSV2) */

    #if (CY_IP_CPUSS && CY_IP_SRSSV2)
        CY_PM_CPUSS_CONFIG_REG |= CY_PM_CPUSS_CONFIG_FLSH_ACC_BYPASS;
    #endif /* (CY_IP_CPUSS && CY_IP_SRSSV2) */

    /* Adjust delay to wait for references to settle on wakeup from Deep Sleep */
    CY_PM_PWR_KEY_DELAY_REG = CY_SFLASH_DPSLP_KEY_DELAY_REG;

    /* CPU enters DeepSleep/Hibernate mode upon execution of WFI */
    CY_PM_CPU_SCR_REG |= CY_PM_CPU_SCR_SLEEPDEEP;

    #if(CY_IP_SRSSV2)
        /* Preserve system clock configuration and
        * reduce sysclk to <=12 MHz (Cypress ID #158710, #179888).
        */
        clkSelectReg = CY_SYS_CLK_SELECT_REG;
        CySysClkWriteSysclkDiv(CY_SYS_CLK_SYSCLK_DIV4);
    #endif /* (CY_IP_SRSSV2) */
    
    /* Sleep and wait for interrupt */
    CY_PM_WFI;

    #if(CY_IP_SRSSV2)
        /* Restore system clock configuration */
        CY_SYS_CLK_SELECT_REG = clkSelectReg;
    #endif /* (CY_IP_SRSSV2) */
    
    #if (CY_IP_CPUSS && CY_IP_SRSSV2)
        CY_PM_CPUSS_CONFIG_REG &= (uint32) (~CY_PM_CPUSS_CONFIG_FLSH_ACC_BYPASS);
    #endif /* (CY_IP_CPUSS && CY_IP_SRSSV2) */

    #if(CY_IP_ECO_SRSSLT)
        if(0u != pllResoreFlag)
        {
            CySysClkWriteHfclkDirect(CY_SYS_CLK_HFCLK_PLL0);
        }
    #endif /* (CY_IP_ECO_SRSSLT) */    

    CyExitCriticalSection(interruptState);
}


#if(CY_IP_SRSSV2)

    /*******************************************************************************
    * Function Name: CySysPmHibernate
    ****************************************************************************//**
    *
    * Puts the part into the Hibernate state. Only SRAM and UDBs are retained;
    * most internal supplies are off. Wakeup is possible from a pin or a hibernate
    * comparator only.
    *
    * It is expected that the firmware has already frozen the IO-Cells using
    * CySysPmFreezeIo() function before the call to this function. If this is
    * omitted, the IO-cells will be frozen in the same way as they are
    * in the Active to Deep Sleep transition, but will lose their state on wake up
    * (because of the reset occurring at that time).
    *
    * Because all the CPU state is lost, the CPU will start up at the reset vector.
    * To save the firmware state through the Hibernate low power mode, a
    * corresponding variable should be defined with CY_NOINIT attribute. It
    * prevents data from being initialized to zero on startup. The interrupt
    * cause of the hibernate peripheral is retained, such that it can be either
    * read by the firmware or cause an interrupt after the firmware has booted and
    * enabled the corresponding interrupt. To distinguish the wakeup from
    * the Hibernate mode and the general Reset event, the
    * \ref CySysPmGetResetReason() function could be used.
    *
    *******************************************************************************/
    void CySysPmHibernate(void)
    {
        uint8 interruptState;

        interruptState = CyEnterCriticalSection();

        #if (CY_IP_HOBTO_DEVICE)
            /* Disable input buffers for all ports */
            CySysPmHibPinsDisableInputBuf();
        #endif /* (CY_IP_HOBTO_DEVICE) */

        /* Device enters Hibernate mode when CPU asserts SLEEPDEEP signal */
        CY_PM_PWR_CONTROL_REG |= CY_PM_PWR_CONTROL_HIBERNATE;

        /* Adjust delay to wait for references to settle on wakeup from hibernate */
        CY_PM_PWR_KEY_DELAY_REG = CY_SFLASH_HIB_KEY_DELAY_REG;

        /* CPU enters DeepSleep/Hibernate mode upon execution of WFI */
        CY_PM_CPU_SCR_REG |= CY_PM_CPU_SCR_SLEEPDEEP;

        /* Save token that will retain through a STOP/WAKEUP sequence
         * thus could be used by CySysPmGetResetReason() to differentiate
         * WAKEUP from a general RESET event.
         */
        CY_PM_PWR_STOP_REG = (CY_PM_PWR_STOP_REG & (uint32)(~CY_PM_PWR_STOP_TOKEN_MASK)) | CY_PM_PWR_STOP_TOKEN_HIB;

        /* Sleep and wait for interrupt. Wakeup from Hibernate is performed
         * through RESET state, causing a normal Boot procedure to occur.
         * The WFI instruction doesn't put the core to sleep if its wake condition
         * is true when the instruction is executed.
         */
        CY_PM_WFI;

        CyExitCriticalSection(interruptState);
    }


    /*******************************************************************************
    * Function Name: CySysPmStop
    ****************************************************************************//**
    *
    * Puts the part into the Stop state. All internal supplies are off;
    * no state is retained.
    *
    * Wakeup from Stop is performed by toggling the wakeup pin, causing
    * a normal boot procedure to occur. To configure the wakeup pin,
    * the Digital Input Pin component should be placed on the schematic,
    * assigned to the wakeup pin, and resistively pulled up or down to the inverse
    * state of the wakeup polarity. To distinguish the wakeup from the Stop mode
    * and the general Reset event, \ref CySysPmGetResetReason() function could be
    * used.  The wakeup pin is active low by default. The wakeup pin polarity
    * could be changed with the \ref CySysPmSetWakeupPolarity() function.
    *
    * This function freezes IO cells implicitly. It is not possible to enter
    * the STOP mode before freezing the IO cells. The IO cells remain frozen after
    * awake from the Stop mode until the firmware unfreezes them after booting
    * explicitly with \ref CySysPmUnfreezeIo() function call.
    *
    *******************************************************************************/
    void CySysPmStop(void)
    {
        (void) CyEnterCriticalSection();

        /* Update token to indicate Stop mode transition. Preserve only polarity. */
        CY_PM_PWR_STOP_REG = (CY_PM_PWR_STOP_REG & CY_PM_PWR_STOP_POLARITY) | CY_PM_PWR_STOP_TOKEN_STOP;

        /* Freeze IO-Cells to save IO-Cell state */
        CySysPmFreezeIo();

        /* Initiates transition to Stop state */
        CY_PM_PWR_STOP_REG = CY_PM_PWR_STOP_REG | CY_PM_PWR_STOP_STOP;

        /* Depending on the clock frequency and internal timing delays,
         * the final AHB transaction may or may not complete. To guard against
         * accidentally executing an unintended instruction, it is recommended
         * to add 2 NOP cycles after the final write to the STOP register.
         */
        CY_NOP;
        CY_NOP;

        /* Should never get to this WFI instruction */
        CY_PM_WFI;

        /* Wakeup from Stop is performed by toggling of Wakeup pin,
         * causing a normal Boot procedure to occur. No need to exit
         * from the critical section.
         */
    }


    /*******************************************************************************
    * Function Name: CySysPmSetWakeupPolarity
    ****************************************************************************//**
    *
    * Wake up from the stop mode is performed by toggling the wakeup pin,
    * causing a normal boot procedure to occur. This function assigns
    * the wakeup pin active level. Setting the wakeup pin to this level will cause
    * the wakeup from stop mode. The wakeup pin is active low by default.
    *
    * \param polarity
    * - \ref CY_PM_STOP_WAKEUP_ACTIVE_LOW Logical zero will wakeup the chip
    * - \ref CY_PM_STOP_WAKEUP_ACTIVE_HIGH Logical one will wakeup the chip
    *
    *******************************************************************************/
    void CySysPmSetWakeupPolarity(uint32 polarity)
    {
        uint8 interruptState;

        interruptState = CyEnterCriticalSection();

        CY_PM_PWR_STOP_REG = (CY_PM_STOP_WAKEUP_ACTIVE_LOW != polarity) ?
                            (CY_PM_PWR_STOP_REG | CY_PM_PWR_STOP_POLARITY) :
                            (CY_PM_PWR_STOP_REG & (uint32) (~CY_PM_PWR_STOP_POLARITY));

        CyExitCriticalSection(interruptState);
    }


    /*******************************************************************************
    * Function Name: CySysPmGetResetReason
    ****************************************************************************//**
    *
    * Retrieves the last reset reason - transition from OFF/XRES/STOP/HIBERNATE to
    * the RESET state. Note that waking up from STOP using XRES will be perceived
    * as a general RESET.
    *
    * \return CY_PM_RESET_REASON_UNKN          Unknown reset reason.
    * \return CY_PM_RESET_REASON_XRES          Transition from OFF/XRES to RESET
    * \return CY_PM_RESET_REASON_WAKEUP_HIB    Transition/wakeup from HIBERNATE to RESET
    * \return CY_PM_RESET_REASON_WAKEUP_STOP   Transition/wakeup from STOP to RESET
    *
    *******************************************************************************/
    uint32 CySysPmGetResetReason(void)
    {
        uint32 reason = CY_PM_RESET_REASON_UNKN;

        switch(CY_PM_PWR_STOP_REG & CY_PM_PWR_STOP_TOKEN_MASK)
        {
        /* Power up, XRES */
        case CY_PM_PWR_STOP_TOKEN_XRES:
            reason = CY_PM_RESET_REASON_XRES;
            break;

        /* Wakeup from Hibernate */
        case CY_PM_PWR_STOP_TOKEN_HIB:
            reason = CY_PM_RESET_REASON_WAKEUP_HIB;
            break;

        /* Wakeup from Stop (through WAKEUP pin assert) */
        case CY_PM_PWR_STOP_TOKEN_STOP:
            reason = CY_PM_RESET_REASON_WAKEUP_STOP;
            break;

        /* Unknown reason */
        default:
            break;
        }

        return (reason);
    }


    /*******************************************************************************
    * Function Name: CySysPmFreezeIo
    ****************************************************************************//**
    *
    * Freezes IO-Cells directly to save the IO-Cell state on wake up from the
    * Hibernate or Stop state. It is not required to call this function before
    * entering the Stop mode, since \ref CySysPmStop() function freezes IO-Cells
    * implicitly.
    *
    * This API is not available for PSoC 4000 family of devices.
    *
    *******************************************************************************/
    void CySysPmFreezeIo(void)
    {
        uint8 interruptState;

        interruptState = CyEnterCriticalSection();

        /* Check FREEZE state to avoid recurrent IO-Cells freeze attempt,
         * since the second call to this function will cause accidental switch
         * to the STOP mode (the system will enter STOP mode immediately after
         * writing to STOP bit since both UNLOCK and FREEZE have been set correctly
         * in a previous call to this function).
         */
        if (0u == (CY_PM_PWR_STOP_REG & CY_PM_PWR_STOP_FREEZE))
        {
            /* Preserve last reset reason and disable overrides the next freeze command by peripherals */
            CY_PM_PWR_STOP_REG = CY_PM_PWR_STOP_STOP | CY_PM_PWR_STOP_FREEZE | CY_PM_PWR_STOP_UNLOCK |
                                (CY_PM_PWR_STOP_REG & (CY_PM_PWR_STOP_TOKEN_MASK | CY_PM_PWR_STOP_POLARITY));

            /* If reading after writing, read this register three times to delay
            *  enough time for internal settling.
            */
            (void) CY_PM_PWR_STOP_REG;
            (void) CY_PM_PWR_STOP_REG;

            /* Second write causes the freeze of IO-Cells to save IO-Cell state */
            CY_PM_PWR_STOP_REG = CY_PM_PWR_STOP_REG;
        }

        CyExitCriticalSection(interruptState);
    }


    /*******************************************************************************
    * Function Name: CySysPmUnfreezeIo
    ****************************************************************************//**
    *
    * The IO-Cells remain frozen after awake from Hibernate or Stop mode until
    * the firmware unfreezes them after booting. The call of this function
    * unfreezes IO-Cells explicitly.
    *
    * If the firmware intent is to retain the data value on the port, then the
    * value must be read and re-written to the data register before calling this
    * API. Furthermore, the drive mode must be re-programmed.  If this is not done,
    * the pin state will change to default state the moment the freeze is removed.
    *
    * This API is not available for PSoC 4000 family of devices.
    *
    *******************************************************************************/
    void CySysPmUnfreezeIo(void)
    {
        uint8 interruptState;

        interruptState = CyEnterCriticalSection();

        /* Preserve last reset reason and wakeup polarity. Then, unfreeze I/O:
         * write PWR_STOP.FREEZE=0, .UNLOCK=0x3A, .STOP=0, .TOKEN
         */
        CY_PM_PWR_STOP_REG = CY_PM_PWR_STOP_UNLOCK |
                            (CY_PM_PWR_STOP_REG & (CY_PM_PWR_STOP_TOKEN_MASK | CY_PM_PWR_STOP_POLARITY));

        /* If reading after writing, read this register three times to delay
        *  enough time for internal settling.
        */
        (void) CY_PM_PWR_STOP_REG;
        (void) CY_PM_PWR_STOP_REG;

        /* Lock STOP mode: write PWR_STOP.FREEZE=0, UNLOCK=0x00, STOP=0, .TOKEN */
        CY_PM_PWR_STOP_REG &= (CY_PM_PWR_STOP_TOKEN_MASK | CY_PM_PWR_STOP_POLARITY);

        CyExitCriticalSection(interruptState);
    }

#else

    /*******************************************************************************
    * Function Name: CySysPmSetWakeupHoldoff
    ****************************************************************************//**
    *
    * Sets the Deep Sleep wakeup time by scaling the hold-off to the HFCLK
    * frequency.
    *
    * This function must be called before increasing HFCLK clock frequency. It can
    * optionally be called after lowering HFCLK clock frequency in order to improve
    * Deep Sleep wakeup time.
    *
    * It is functionally acceptable to leave the default hold-off setting, but
    * Deep Sleep wakeup time may exceed the specification.
    *
    * This function is applicable only for the 4000 device family.
    *
    * \param hfclkFrequencyMhz The HFCLK frequency in MHz.
    *
    *******************************************************************************/
    void CySysPmSetWakeupHoldoff(uint32 hfclkFrequencyMhz)
    {
        CY_PM_PWR_KEY_DELAY_REG = ((((uint32)(CY_PM_PWR_KEY_DELAY_REG_DEFAULT << 16u) /
            CY_PM_PWR_KEY_DELAY_FREQ_DEFAULT) * hfclkFrequencyMhz) >> 16u) + 1u;
    }

#endif /* (CY_IP_SRSSV2) */

/* [] END OF FILE */
