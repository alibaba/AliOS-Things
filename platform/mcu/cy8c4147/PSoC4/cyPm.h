/***************************************************************************//**
* \file cyPm.h
* \version 5.70
*
* \brief Provides the function definitions for the power management API.
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

#if !defined(CY_BOOT_CYPM_H)
#define CY_BOOT_CYPM_H

#include "cytypes.h"
#include "cypins.h"


/**
* \addtogroup group_power_management Power Management API
* @{

\brief PSoC 4 devices support the following power modes (in order of high to low power consumption): Active, Sleep,
Deep Sleep, Hibernate, and Stop. Active, Sleep and Deep-Sleep are standard ARM defined power modes, supported by the
ARM CPUs. Hibernate/Stop are even lower power modes that are entered from firmware just like Deep-Sleep, but on wakeup
the CPU (and all peripherals) goes through a full reset.

There is a full range of power modes supported by PSoC devices to control power consumption and the amount of available
resources. See the following table for the supported power modes.

Mode        | PSoC 4000              | Rest Devices           |
----------- | ---------------------- | ---------------------- |
Active      |          Y             |       Y                |
Sleep       |          Y             |       Y                |
Deep Sleep  |          Y             |       Y                |
Hibernate   |          Y             |       Y                |
Stop        |                        |       Y                |

For the ARM-based devices (PSoC 4), an interrupt is required for the CPU to wake up. The Power Management implementation
assumes that wakeup time is configured with a separate component (component-based wakeup time configuration) for an
interrupt to be issued on terminal count.

All pending interrupts should be cleared before the device is put into low power mode, even if they are masked.

The Power Management API is provided in the CyPm.c and CyPm.h files.


\section group_power_management_implementation Implementation
For PSoC 4100, PSoC 4000U and PSoC 4200 devices, the software should set EXT_VCCD bit in the PWR_CONTROL register when 
Vccd is shorted to Vddd on the board. This impacts the chip internal state transitions where it is necessary to know 
whether Vccd is connected or floating to achieve minimum current in low power modes. Note Setting this bit turns off 
the active regulator and will lead to a system reset unless both Vddd and Vccd pins are supplied externally. Refer to 
the device TRM for more information.

It is safe to call PM APIs from the ISR. The wakeup conditions for Sleep and DeepSleep low power modes are illustrated
in the following table.

Interrupts State  |  Condition                      | Wakeup    |   ISR Execution   |
------------------|---------------------------------|-----------|------------------ |
Unmasked          | IRQ priority > current level    | Yes       |   Yes             |
Unmasked          | IRQ priority ≤ current level    | No        |   No              |
Masked            | IRQ priority > current level    | Yes       |   No              |
Masked            | IRQ priority ≤ current level    | No        |   No              |


\section group_power_management_clocks Clock Configuration
For PSoC 4100 BLE and PSoC 4200 BLE devices, the HFCLK source should be set to IMO before switching the device into low
power mode. The IMO should be enabled (by calling CySysClkImoStart(), if it is not) and HFCLK source should be changed
to IMO by calling CySysClkWriteHfclkDirect(CY_SYS_CLK_HFCLK_IMO).

If the System clock frequency is increased by switching to the IMO, the CySysFlashSetWaitCycles() function with an
appropriate parameter should be called beforehand. Also, it can optionally be called after lowering the System clock
frequency in order to improve CPU performance. See CySysFlashSetWaitCycles() description for the details.





*/
void CySysPmSleep(void);
void CySysPmDeepSleep(void);

#if(CY_IP_SRSSV2)
    void CySysPmHibernate(void);
    void CySysPmFreezeIo(void);
    void CySysPmUnfreezeIo(void);
    uint32 CySysPmGetResetReason(void);
    void CySysPmStop(void);
    void CySysPmSetWakeupPolarity(uint32 polarity);
#else
    void CySysPmSetWakeupHoldoff(uint32 hfclkFrequencyMhz);
#endif /* (CY_IP_SRSSV2) */

/** @} group_power_management */


/*******************************************************************************
* The ARM compilers have the __wfi() intrinsic that inserts a WFI instruction
* into the instruction stream generated by the compiler. The GCC compiler has to
* execute assembly language instruction.
*******************************************************************************/
#if defined(__ARMCC_VERSION)    /* Instristic for Keil compilers */
    #define CY_PM_WFI       __wfi()
#else   /* ASM for GCC & IAR */
    #define CY_PM_WFI       __asm volatile ("WFI \n")
#endif /* __ARMCC_VERSION */

#if(CY_IP_SRSSV2)

    /* CySysPmSetWakeupPolarity() */
    #define CY_PM_STOP_WAKEUP_ACTIVE_LOW        ((uint32)(0x0u))    /**< Logical zero will wakeup the chip */
    #define CY_PM_STOP_WAKEUP_ACTIVE_HIGH       ((uint32)(0x1u))    /**< Logical one will wakeup the chip */
    #define CY_PM_STOP_WAKEUP_POLARITY          (CY_PM_STOP_WAKEUP_ACTIVE_LOW)

    /* CySysPmGetResetReason() */
    #define CY_PM_RESET_REASON_UNKN             (0u)    /**< Unknown reset reason. */
    #define CY_PM_RESET_REASON_XRES             (1u)    /**< Transition from OFF/XRES to RESET */
    #define CY_PM_RESET_REASON_WAKEUP_HIB       (2u)    /**< Transition/wakeup from HIBERNATE to RESET */
    #define CY_PM_RESET_REASON_WAKEUP_STOP      (3u)    /**< Transition/wakeup from STOP to RESET */

#endif /* (CY_IP_SRSSV2) */


/***************************************
*              Registers
***************************************/

/* Power Mode Control */
#define CY_PM_PWR_CONTROL_REG               (*(reg32 *) CYREG_PWR_CONTROL)
#define CY_PM_PWR_CONTROL_PTR               ( (reg32 *) CYREG_PWR_CONTROL)

/* CPU System Control Register */
#if (CY_IP_CPUSS_CM0)
    #define CY_PM_CPU_SCR_REG                   (*(reg32 *) CYREG_CM0_SCR)
    #define CY_PM_CPU_SCR_PTR                   ( (reg32 *) CYREG_CM0_SCR)
#else /* CY_IP_CPUSS_CM0PLUS */
    #define CY_PM_CPU_SCR_REG                   (*(reg32 *) CYREG_CM0P_SCR)
    #define CY_PM_CPU_SCR_PTR                   ( (reg32 *) CYREG_CM0P_SCR)
#endif /* (CY_IP_CPUSS_CM0) */

/* Power System Key & Delay Register */
#define CY_PM_PWR_KEY_DELAY_REG             (*(reg32 *) CYREG_PWR_KEY_DELAY)
#define CY_PM_PWR_KEY_DELAY_PTR             ( (reg32 *) CYREG_PWR_KEY_DELAY)


#if(CY_IP_SRSSV2)
    /* Hibernate wakeup value for PWR_KEY_DELAY */
    #define CY_SFLASH_HIB_KEY_DELAY_REG     (*(reg16 *) CYREG_SFLASH_HIB_KEY_DELAY)
    #define CY_SFLASH_HIB_KEY_DELAY_PTR     ( (reg16 *) CYREG_SFLASH_HIB_KEY_DELAY)
#endif  /* (CY_IP_SRSSV2) */

/* Deep Sleep wakeup value for PWR_KEY_DELAY */
#define CY_SFLASH_DPSLP_KEY_DELAY_REG       (*(reg16 *) CYREG_SFLASH_DPSLP_KEY_DELAY)
#define CY_SFLASH_DPSLP_KEY_DELAY_PTR       ( (reg16 *) CYREG_SFLASH_DPSLP_KEY_DELAY)

/* Power Stop Mode Register */
#if(CY_IP_SRSSV2)
    #define CY_PM_PWR_STOP_REG              (*(reg32 *) CYREG_PWR_STOP)
    #define CY_PM_PWR_STOP_PTR              ( (reg32 *) CYREG_PWR_STOP)
#endif /* (CY_IP_SRSSV2) */

#if (CY_PSOC4_4100 || CY_PSOC4_4200 || CY_PSOC4_4000U)
    /* CPU Subsystem Configuration */
    #define CY_PM_CPUSS_CONFIG_REG             (*(reg32 *) CYREG_CPUSS_CONFIG)
    #define CY_PM_CPUSS_CONFIG_PTR             ( (reg32 *) CYREG_CPUSS_CONFIG)
#endif /* (CY_PSOC4_4100 || CY_PSOC4_4200 || CY_PSOC4_4000U) */


/***************************************
*       Register Constants
***************************************/

/* CM0 System Control Register Constants */
#define CY_PM_CPU_SCR_SLEEPDEEP             ((uint32)(0x04u))

#if(CY_IP_SRSSV2)
    /* Power Mode Control Constants */
    #define CY_PM_PWR_CONTROL_HIBERNATE         (0x80000000u)

    /* Power Mode Stop Constants */
    #define CY_PM_PWR_STOP_POLARITY_SHIFT       (16u)
    #define CY_PM_PWR_STOP_POLARITY             ((uint32)((uint32)1u << CY_PM_PWR_STOP_POLARITY_SHIFT))
    #define CY_PM_PWR_STOP_FREEZE_SHIFT         (17u)
    #define CY_PM_PWR_STOP_FREEZE               ((uint32)((uint32)1u << CY_PM_PWR_STOP_FREEZE_SHIFT))
    #define CY_PM_PWR_STOP_UNLOCK_SHIFT         (8u)
    #define CY_PM_PWR_STOP_UNLOCK_MASK          ((uint32)((uint32)0xFFu << CY_PM_PWR_STOP_UNLOCK_SHIFT))
    #define CY_PM_PWR_STOP_UNLOCK               ((uint32)((uint32)0x3Au << CY_PM_PWR_STOP_UNLOCK_SHIFT))
    #define CY_PM_PWR_STOP_STOP_SHIFT           (31u)
    #define CY_PM_PWR_STOP_STOP                 ((uint32)((uint32)1u << CY_PM_PWR_STOP_STOP_SHIFT))
    #define CY_PM_PWR_STOP_TOKEN_MASK           ((uint32)(0xFFu))
    #define CY_PM_PWR_STOP_TOKEN_XRES           ((uint32)(0x00u))
    #define CY_PM_PWR_STOP_TOKEN_HIB            ((uint32)(0xF1u))
    #define CY_PM_PWR_STOP_TOKEN_STOP           ((uint32)(0xF2u))
#else
    #define CY_PM_PWR_KEY_DELAY_REG_DEFAULT     ((uint32) 248u)
    #define CY_PM_PWR_KEY_DELAY_FREQ_DEFAULT    (48u)
#endif /* (CY_IP_SRSSV2) */

#if (CY_PSOC4_4100 || CY_PSOC4_4200 || CY_PSOC4_4000U)
    /* 0 - normal operation, 1 - Flash Accelerator in bypass mode */
    #define CY_PM_CPUSS_CONFIG_FLSH_ACC_BYPASS      ((uint32) 0x02u)
#endif /* (CY_PSOC4_4100 || CY_PSOC4_4200 || CY_PSOC4_4000U) */


#if (CY_IP_SRSSV2)
    #if (CY_IP_HOBTO_DEVICE)
        /*******************************************************************************
        * Function Name: CySysPmHibPinsDisableInputBuf
        ****************************************************************************//**
        *
        *  Disable the input buffer for all the port. This is required before Hibernate
        *  mode entry as the operation of the input buffer is not guaranteed if VCCD
        *  drops down to 1.0 V.
        *
        *******************************************************************************/
        static CY_INLINE void CySysPmHibPinsDisableInputBuf(void)
        {
            #ifdef CYREG_GPIO_PRT0_PC
                CY_CLEAR_REG32_FIELD(CYREG_GPIO_PRT0_PC, CYFLD_GPIO_PRT_PORT_IB_MODE_SEL);
            #endif /* CYREG_GPIO_PRT0_PC */

            #ifdef CYREG_GPIO_PRT1_PC
                CY_CLEAR_REG32_FIELD( CYREG_GPIO_PRT1_PC, CYFLD_GPIO_PRT_PORT_IB_MODE_SEL);
            #endif /* CYREG_GPIO_PRT1_PC */

            #ifdef CYREG_GPIO_PRT2_PC
                CY_CLEAR_REG32_FIELD( CYREG_GPIO_PRT2_PC, CYFLD_GPIO_PRT_PORT_IB_MODE_SEL);
            #endif /* CYREG_GPIO_PRT2_PC */

            #ifdef CYREG_GPIO_PRT3_PC
                CY_CLEAR_REG32_FIELD( CYREG_GPIO_PRT3_PC, CYFLD_GPIO_PRT_PORT_IB_MODE_SEL);
            #endif /* CYREG_GPIO_PRT3_PC */

            #ifdef CYREG_GPIO_PRT4_PC
                CY_CLEAR_REG32_FIELD( CYREG_GPIO_PRT4_PC, CYFLD_GPIO_PRT_PORT_IB_MODE_SEL);
            #endif /* CYREG_GPIO_PRT4_PC */

            #ifdef CYREG_GPIO_PRT5_PC
                CY_CLEAR_REG32_FIELD( CYREG_GPIO_PRT5_PC, CYFLD_GPIO_PRT_PORT_IB_MODE_SEL);
            #endif /* CYREG_GPIO_PRT5_PC */

            #ifdef CYREG_GPIO_PRT6_PC
                CY_CLEAR_REG32_FIELD( CYREG_GPIO_PRT6_PC, CYFLD_GPIO_PRT_PORT_IB_MODE_SEL);
            #endif /* CYREG_GPIO_PRT6_PC */

            #ifdef CYREG_GPIO_PRT7_PC
                CY_CLEAR_REG32_FIELD( CYREG_GPIO_PRT7_PC, CYFLD_GPIO_PRT_PORT_IB_MODE_SEL);
            #endif /* CYREG_GPIO_PRT7_PC */

            #ifdef CYREG_GPIO_PRT8_PC
                CY_CLEAR_REG32_FIELD( CYREG_GPIO_PRT8_PC, CYFLD_GPIO_PRT_PORT_IB_MODE_SEL);
            #endif /* CYREG_GPIO_PRT8_PC */

            #ifdef CYREG_GPIO_PRT9_PC
                CY_CLEAR_REG32_FIELD( CYREG_GPIO_PRT9_PC, CYFLD_GPIO_PRT_PORT_IB_MODE_SEL);
            #endif /* CYREG_GPIO_PRT9_PC */

            #ifdef CYREG_GPIO_PRT10_PC
                CY_CLEAR_REG32_FIELD(CYREG_GPIO_PRT10_PC, CYFLD_GPIO_PRT_PORT_IB_MODE_SEL);
            #endif /* CYREG_GPIO_PRT10_PC */

            #ifdef CYREG_GPIO_PRT11_PC
                CY_CLEAR_REG32_FIELD(CYREG_GPIO_PRT11_PC, CYFLD_GPIO_PRT_PORT_IB_MODE_SEL);
            #endif /* CYREG_GPIO_PRT11_PC */

            #ifdef CYREG_GPIO_PRT12_PC
                CY_CLEAR_REG32_FIELD(CYREG_GPIO_PRT12_PC, CYFLD_GPIO_PRT_PORT_IB_MODE_SEL);
            #endif /* CYREG_GPIO_PRT12_PC */

            #ifdef CYREG_GPIO_PRT13_PC
                CY_CLEAR_REG32_FIELD(CYREG_GPIO_PRT13_PC, CYFLD_GPIO_PRT_PORT_IB_MODE_SEL);
            #endif /* CYREG_GPIO_PRT13_PC */

            #ifdef CYREG_GPIO_PRT14_PC
                CY_CLEAR_REG32_FIELD(CYREG_GPIO_PRT14_PC, CYFLD_GPIO_PRT_PORT_IB_MODE_SEL);
            #endif /* CYREG_GPIO_PRT14_PC */

            #ifdef CYREG_GPIO_PRT15_PC
                CY_CLEAR_REG32_FIELD(CYREG_GPIO_PRT15_PC, CYFLD_GPIO_PRT_PORT_IB_MODE_SEL);
            #endif /* CYREG_GPIO_PRT15_PC */
        }
    #endif /* (CY_IP_HOBTO_DEVICE) */
#endif  /* (CY_IP_SRSSV2) */


#if (CY_IP_CPUSS_CM0)
    #define CY_PM_CM0_SCR_REG                   (CY_PM_CPU_SCR_REG)
    #define CY_PM_CM0_SCR_PTR                   (CY_PM_CPU_SCR_PTR)
    #define CY_PM_CM0_SCR_SLEEPDEEP             (CY_PM_CPU_SCR_SLEEPDEEP)
#endif /* (CY_IP_CPUSS_CM0) */


#endif  /* CY_BOOT_CYPM_H */


/* [] END OF FILE */
