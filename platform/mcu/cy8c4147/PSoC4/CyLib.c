/***************************************************************************//**
* \file CyLib.c
* \version 5.70
*
* \brief Provides a system API for the Clocking, Interrupts, SysTick, and
* Voltage Detect.
*
* \note Documentation of the API's in this file is located in the PSoC 4 System
* Reference Guide provided with PSoC Creator.
*
********************************************************************************
* \copyright
* Copyright 2010-2018, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "CyLib.h"

/* CySysClkWriteImoFreq() || CySysClkImoEnableWcoLock() */
#if ((CY_IP_SRSSV2 && CY_IP_FMLT) || CY_IP_IMO_TRIMMABLE_BY_WCO)
    #include "CyFlash.h"
#endif /* (CY_IP_SRSSV2 && CY_IP_FMLT) */

/* Do not use these definitions directly in your application */
uint32 cydelayFreqHz  = CYDEV_BCLK__SYSCLK__HZ;
uint32 cydelayFreqKhz = (CYDEV_BCLK__SYSCLK__HZ + CY_DELAY_1K_MINUS_1_THRESHOLD) / CY_DELAY_1K_THRESHOLD;
uint8  cydelayFreqMhz = (uint8)((CYDEV_BCLK__SYSCLK__HZ + CY_DELAY_1M_MINUS_1_THRESHOLD) / CY_DELAY_1M_THRESHOLD);
uint32 cydelay32kMs   = CY_DELAY_MS_OVERFLOW * ((CYDEV_BCLK__SYSCLK__HZ + CY_DELAY_1K_MINUS_1_THRESHOLD) /
                        CY_DELAY_1K_THRESHOLD);


static cySysTickCallback CySysTickCallbacks[CY_SYS_SYST_NUM_OF_CALLBACKS];
static void CySysTickServiceCallbacks(void);

#if (CY_IP_PLL)
    static uint32 CySysClkPllGetBypassMode(uint32 pll);
    static cystatus CySysClkPllConfigChangeAllowed(uint32 pll);
#endif /* (CY_IP_PLL) */


/***************************************************************************//**
* Indicates whether or not the SysTick has been initialized. The variable is
* initialized to 0 and set to 1 the first time CySysTickStart() is called.
*
* This allows the component to restart without reinitialization after the first
* call to the CySysTickStart() routine.
*
* If reinitialization of the SysTick is required, call CySysTickInit() before
* calling CySysTickStart(). Alternatively, the SysTick can be reinitialized by
* calling the CySysTickInit() and CySysTickEnable() functions.
*******************************************************************************/
uint32 CySysTickInitVar = 0u;


#if(CY_IP_SRSSV2)
        /* Conversion between CySysClkWriteImoFreq() parameter and register's value */
        const uint8 cyImoFreqMhz2Reg[CY_SYS_CLK_IMO_FREQ_TABLE_SIZE] = {
            /*  3 MHz */ 0x03u,  /*  4 MHz */ 0x04u,  /*  5 MHz */ 0x05u,  /*  6 MHz */ 0x06u,
            /*  7 MHz */ 0x07u,  /*  8 MHz */ 0x08u,  /*  9 MHz */ 0x09u,  /* 10 MHz */ 0x0Au,
            /* 11 MHz */ 0x0Bu,  /* 12 MHz */ 0x0Cu,  /* 13 MHz */ 0x0Eu,  /* 14 MHz */ 0x0Fu,
            /* 15 MHz */ 0x10u,  /* 16 MHz */ 0x11u,  /* 17 MHz */ 0x12u,  /* 18 MHz */ 0x13u,
            /* 19 MHz */ 0x14u,  /* 20 MHz */ 0x15u,  /* 21 MHz */ 0x16u,  /* 22 MHz */ 0x17u,
            /* 23 MHz */ 0x18u,  /* 24 MHz */ 0x19u,  /* 25 MHz */ 0x1Bu,  /* 26 MHz */ 0x1Cu,
            /* 27 MHz */ 0x1Du,  /* 28 MHz */ 0x1Eu,  /* 29 MHz */ 0x1Fu,  /* 30 MHz */ 0x20u,
            /* 31 MHz */ 0x21u,  /* 32 MHz */ 0x22u,  /* 33 MHz */ 0x23u,  /* 34 MHz */ 0x25u,
            /* 35 MHz */ 0x26u,  /* 36 MHz */ 0x27u,  /* 37 MHz */ 0x28u,  /* 38 MHz */ 0x29u,
            /* 39 MHz */ 0x2Au,  /* 40 MHz */ 0x2Bu,  /* 41 MHz */ 0x2Eu,  /* 42 MHz */ 0x2Fu,
            /* 43 MHz */ 0x30u,  /* 44 MHz */ 0x31u,  /* 45 MHz */ 0x32u,  /* 46 MHz */ 0x33u,
            /* 47 MHz */ 0x34u,  /* 48 MHz */ 0x35u };
#endif /* (CY_IP_SRSSV2) */

#if (CY_IP_IMO_TRIMMABLE_BY_WCO)
        /* Conversion between IMO frequency and WCO DPLL max offset steps */
        const uint8 cyImoFreqMhz2DpllOffset[CY_SYS_CLK_IMO_FREQ_WCO_DPLL_TABLE_SIZE] = {
            /* 26 MHz */  238u, /* 27 MHz */  219u, /* 28 MHz */  201u, /* 29 MHz */  185u,
            /* 30 MHz */  170u, /* 31 MHz */  155u, /* 32 MHz */  142u, /* 33 MHz */  130u,
            /* 34 MHz */  118u, /* 35 MHz */  107u, /* 36 MHz */   96u, /* 37 MHz */  86u,
            /* 38 MHz */   77u, /* 39 MHz */   68u, /* 40 MHz */   59u, /* 41 MHz */  51u,
            /* 42 MHz */   44u, /* 43 MHz */   36u, /* 44 MHz */   29u, /* 45 MHz */  23u,
            /* 46 MHz */   16u, /* 47 MHz */   10u, /* 48 MHz */   4u };
#endif /* (CY_IP_IMO_TRIMMABLE_BY_WCO) */

/* Stored CY_SYS_CLK_IMO_TRIM4_REG when modified for USB lock */
#if (CY_IP_IMO_TRIMMABLE_BY_USB && CY_IP_SRSSV2)
    uint32 CySysClkImoTrim4 = 0u;
    uint32 CySysClkImoTrim5 = 0u;
#endif /* (CY_IP_IMO_TRIMMABLE_BY_USB && CY_IP_SRSSV2) */

/* Stored PUMP_SEL configuration during disable (IMO output by default) */
uint32 CySysClkPumpConfig = CY_SYS_CLK_PUMP_ENABLE;

/*******************************************************************************
* Function Name: CySysClkImoStart
****************************************************************************//**
*
* Enables the IMO.
*
* For PSoC 4100M / PSoC 4200M / PSoC 4000S / PSoC 4100S / PSoC Analog 
* Coprocessor devices, this function will also enable WCO lock if selected in 
* the Design Wide Resources tab.
*
* For PSoC 4200L devices, this function will also enable USB lock if selected
* in the Design Wide Resources tab.
*
*******************************************************************************/
void CySysClkImoStart(void)
{
    CY_SYS_CLK_IMO_CONFIG_REG |= CY_SYS_CLK_IMO_CONFIG_ENABLE;

    #if (CY_IP_IMO_TRIMMABLE_BY_WCO)
        #if (CYDEV_IMO_TRIMMED_BY_WCO == 1u)
            CySysClkImoEnableWcoLock();
        #endif  /* (CYDEV_IMO_TRIMMED_BY_WCO == 1u) */
    #endif  /* (CY_IP_IMO_TRIMMABLE_BY_WCO) */


    #if (CY_IP_IMO_TRIMMABLE_BY_USB)
        #if (CYDEV_IMO_TRIMMED_BY_USB == 1u)
            CySysClkImoEnableUsbLock();
        #endif  /* (CYDEV_IMO_TRIMMED_BY_USB == 1u) */
    #endif  /* (CY_IP_IMO_TRIMMABLE_BY_USB) */

}


/*******************************************************************************
* Function Name: CySysClkImoStop
****************************************************************************//**
*
* Disables the IMO.
*
* For PSoC 4100M / PSoC 4200M / PSoC 4200L / PSoC 4000S / PSoC 4100S / 
* PSoC Analog Coprocessor devices, this function will also disable WCO lock.
*
* For PSoC PSoC 4200L devices, this function will also disable USB lock.
*
*******************************************************************************/
void CySysClkImoStop(void)
{
    #if (CY_IP_IMO_TRIMMABLE_BY_WCO)
        CySysClkImoDisableWcoLock();
    #endif  /* (CY_IP_IMO_TRIMMABLE_BY_WCO) */

    #if (CY_IP_IMO_TRIMMABLE_BY_USB)
        CySysClkImoDisableUsbLock();
    #endif  /* (CY_IP_IMO_TRIMMABLE_BY_USB) */

    CY_SYS_CLK_IMO_CONFIG_REG &= ( uint32 ) ( ~( uint32 )CY_SYS_CLK_IMO_CONFIG_ENABLE);
}

#if (CY_IP_IMO_TRIMMABLE_BY_WCO)

    /*******************************************************************************
    * Function Name: CySysClkImoEnableWcoLock
    ****************************************************************************//**
    *
    * Enables the IMO to WCO lock feature. This function works only if the WCO is
    * already enabled. If the WCO is not enabled then this function returns
    * without enabling the lock feature.
    *
    * It takes up to 20 ms for the IMO to stabilize. The delay is implemented with
    * CyDelay() function. The delay interval is measured based on the system
    * frequency defined by PSoC Creator at build time. If System clock frequency
    * is changed in runtime, the CyDelayFreq() with the appropriate parameter
    * should be called.
    *
    * For PSoC 4200L devices, note that the IMO can lock to either WCO or USB
    * but not both.
    *
    * This function is applicable for PSoC 4100M / PSoC 4200M / PSoC 4000S /
    * PSoC 4100S / PSoC Analog Coprocessor / PSoC 4200L.
    *
    *******************************************************************************/
    void CySysClkImoEnableWcoLock(void)
    {
        #if(CY_IP_SRSSV2)
            uint32 i;
        #endif  /* (CY_IP_SRSSV2) */

        uint32 freq;
        uint8  interruptState;
        uint32 regTmp;
        uint32 lfLimit = 0u;
        volatile uint32 flashCtlReg;

        if (0u != CySysClkWcoEnabled())
        {
            interruptState = CyEnterCriticalSection();

            /* Set oscillator interface control port to WCO */
            #if (CY_IP_IMO_TRIMMABLE_BY_WCO && CY_IP_IMO_TRIMMABLE_BY_USB)
                CY_SYS_CLK_OSCINTF_CTL_REG =
                    (CY_SYS_CLK_OSCINTF_CTL_REG & (uint32) ~CY_SYS_CLK_OSCINTF_CTL_PORT_SEL_MASK) |
                    CY_SYS_CLK_OSCINTF_CTL_PORT_SEL_WCO;
            #endif /* (CY_IP_IMO_TRIMMABLE_BY_WCO && CY_IP_IMO_TRIMMABLE_BY_USB) */

            /* Get current IMO frequency based on the register value */
            #if(CY_IP_SRSSV2)
                freq = CY_SYS_CLK_IMO_MIN_FREQ_MHZ;
                for(i = 0u; i < CY_SYS_CLK_IMO_FREQ_TABLE_SIZE; i++)
                {
                    if ((uint8) (CY_SYS_CLK_IMO_TRIM2_REG & CY_SYS_CLK_IMO_FREQ_BITS_MASK) == cyImoFreqMhz2Reg[i])
                    {
                        freq = i + CY_SYS_CLK_IMO_FREQ_TABLE_OFFSET;
                        break;
                    }
                }
            #else
                /* Calculate frequency by shifting register field value and adding constant. */
                #if(CY_IP_SRSSLT)
                    freq = (((uint32) ((CY_SYS_CLK_IMO_SELECT_REG & ((uint32) CY_SYS_CLK_IMO_SELECT_FREQ_MASK)) << 
                                        CY_SYS_CLK_IMO_SELECT_FREQ_SHIFT) + CY_SYS_CLK_IMO_MIN_FREQ_MHZ) >> 
                                      ((CY_SYS_CLK_SELECT_REG >> CY_SYS_CLK_SELECT_HFCLK_DIV_SHIFT) & 
                                       (uint32) CY_SYS_CLK_SELECT_HFCLK_DIV_MASK));
                #else
                    freq = ((uint32) ((CY_SYS_CLK_IMO_SELECT_REG & ((uint32) CY_SYS_CLK_IMO_SELECT_FREQ_MASK)) <<
                                       CY_SYS_CLK_IMO_SELECT_FREQ_SHIFT) + CY_SYS_CLK_IMO_MIN_FREQ_MHZ);
                #endif  /* (CY_IP_SRSSLT) */

            #endif  /* (CY_IP_SRSSV2) */

            /* For the WCO locking mode, the IMO gain needs to be CY_SYS_CLK_IMO_TRIM4_GAIN */
            #if(CY_IP_SRSSV2)
                if ((CY_SYS_CLK_IMO_TRIM4_REG & CY_SYS_CLK_IMO_TRIM4_GAIN_MASK) == 0u)
                {
			         CY_SYS_CLK_IMO_TRIM4_REG = (CY_SYS_CLK_IMO_TRIM4_REG & (uint32) ~CY_SYS_CLK_IMO_TRIM4_GAIN_MASK) |
			                  				     CY_SYS_CLK_IMO_TRIM4_WCO_GAIN;
                }
            #endif /* (CY_IP_SRSSV2) */

            regTmp  = CY_SYS_CLK_WCO_DPLL_REG & ~(CY_SYS_CLK_WCO_DPLL_MULT_MASK |
                                                  CY_SYS_CLK_WCO_CONFIG_DPLL_LF_IGAIN_MASK |
                                                  CY_SYS_CLK_WCO_CONFIG_DPLL_LF_PGAIN_MASK |
                                                  CY_SYS_CLK_WCO_CONFIG_DPLL_LF_LIMIT_MASK);

            /* Set multiplier to determine IMO frequency in multiples of the WCO frequency */
            regTmp |= (CY_SYS_CLK_WCO_DPLL_MULT_VALUE(freq) & CY_SYS_CLK_WCO_DPLL_MULT_MASK);

            /* Set DPLL Loop Filter Integral and Proportional Gains Setting */
            regTmp |= (CY_SYS_CLK_WCO_CONFIG_DPLL_LF_IGAIN | CY_SYS_CLK_WCO_CONFIG_DPLL_LF_PGAIN);

            /* Set maximum allowed IMO offset */
            if (freq < CY_SYS_CLK_IMO_FREQ_WCO_DPLL_SAFE_POINT)
            {
                regTmp |= (CY_SYS_CLK_WCO_CONFIG_DPLL_LF_LIMIT_MAX << CY_SYS_CLK_WCO_CONFIG_DPLL_LF_LIMIT_SHIFT);
            }
            else
            {
                lfLimit = (uint32) CY_SFLASH_IMO_TRIM_REG(freq - CY_SYS_CLK_IMO_MIN_FREQ_MHZ) +
                    cyImoFreqMhz2DpllOffset[freq - CY_SYS_CLK_IMO_FREQ_WCO_DPLL_TABLE_OFFSET];

                lfLimit = (lfLimit > CY_SYS_CLK_WCO_CONFIG_DPLL_LF_LIMIT_MAX) ?
                    CY_SYS_CLK_WCO_CONFIG_DPLL_LF_LIMIT_MAX : lfLimit;

                regTmp |= (lfLimit << CY_SYS_CLK_WCO_CONFIG_DPLL_LF_LIMIT_SHIFT);
            }

            CY_SYS_CLK_WCO_DPLL_REG = regTmp;

            flashCtlReg = CY_FLASH_CTL_REG;
            CySysFlashSetWaitCycles(CY_SYS_CLK_IMO_MAX_FREQ_MHZ);
            CY_SYS_CLK_WCO_CONFIG_REG |= CY_SYS_CLK_WCO_CONFIG_DPLL_ENABLE;
            CyDelay(CY_SYS_CLK_WCO_IMO_TIMEOUT_MS);
            CY_FLASH_CTL_REG = flashCtlReg;

            CyExitCriticalSection(interruptState);
        }
    }


    /*******************************************************************************
    * Function Name: CySysClkImoDisableWcoLock
    ****************************************************************************//**
    *
    * Disables the IMO to WCO lock feature.
    *
    * For PSoC 4200L devices, note that the IMO can lock to either WCO or USB
    * but not both.
    *
    * This function is applicable for PSoC 4100M / PSoC 4200M / PSoC 4000S /
    * PSoC 4100S / PSoC Analog Coprocessor / PSoC 4200L.
    *
    *******************************************************************************/
    void CySysClkImoDisableWcoLock(void)
    {
        CY_SYS_CLK_WCO_CONFIG_REG &= (uint32) ~CY_SYS_CLK_WCO_CONFIG_DPLL_ENABLE;
    }


    /*******************************************************************************
    * Function Name: CySysClkImoGetWcoLock
    ****************************************************************************//**
    *
    * Reports the IMO to WCO lock enable state.
    *
    * This function is applicable for PSoC 4100M / PSoC 4200M / PSoC 4000S /
    * PSoC 4100S / PSoC Analog Coprocessor / PSoC 4200L.
    *
    * \return 1 if IMO to WCO lock is enabled.
    * \return 0 if IMO to WCO lock is disabled.
    *
    *******************************************************************************/
    uint32 CySysClkImoGetWcoLock(void)
    {
        return ((0u != (CY_SYS_CLK_WCO_CONFIG_REG & CY_SYS_CLK_WCO_CONFIG_DPLL_ENABLE)) ?
                (uint32) 1u :
                (uint32) 0u);
    }

#endif /* (CY_IP_IMO_TRIMMABLE_BY_WCO) */


#if (CY_IP_IMO_TRIMMABLE_BY_USB)

    /*******************************************************************************
    * Function Name: CySysClkImoEnableUsbLock
    ****************************************************************************//**
    *
    * Enables the IMO to USB lock feature.
    *
    * This function must be called before CySysClkWriteImoFreq().
    *
    * This function is called from CySysClkImoStart() function if USB lock
    * selected in the Design Wide Resources tab.
    *
    * This is applicable for PSoC 4200L family of devices only. For PSoC 4200L
    * devices, the IMO can lock to either WCO or USB, but not both.
    *
    *******************************************************************************/
    void CySysClkImoEnableUsbLock(void)
    {
        #if(CY_IP_SRSSV2)
            uint32 i;
            
            /* Check for new trim algorithm */
            uint32 CySysClkUsbCuSortTrim = ((CY_SFLASH_S1_TESTPGM_OLD_REV < (CY_SFLASH_S1_TESTPGM_REV_REG & 
                                                                            CY_SFLASH_S1_TESTPGM_REV_MASK)) ? 1u : 0u);

            /* Get current IMO frequency based on the register value */
            uint32 freq = CY_SYS_CLK_IMO_MIN_FREQ_MHZ;

            for(i = 0u; i < CY_SYS_CLK_IMO_FREQ_TABLE_SIZE; i++)
            {
                if ((uint8) (CY_SYS_CLK_IMO_TRIM2_REG & CY_SYS_CLK_IMO_FREQ_BITS_MASK) == cyImoFreqMhz2Reg[i])
                {
                    freq = i + CY_SYS_CLK_IMO_FREQ_TABLE_OFFSET;
                    break;
                }
            }
        #endif  /* (CY_IP_SRSSV2) */
        
        /* Set oscillator interface control port to USB */
        #if (CY_IP_IMO_TRIMMABLE_BY_WCO && CY_IP_IMO_TRIMMABLE_BY_USB)
            CY_SYS_CLK_OSCINTF_CTL_REG = (CY_SYS_CLK_OSCINTF_CTL_REG & (uint32) ~CY_SYS_CLK_OSCINTF_CTL_PORT_SEL_MASK) |
                                          CY_SYS_CLK_OSCINTF_CTL_PORT_SEL_USB;
        #endif /* (CY_IP_IMO_TRIMMABLE_BY_WCO && CY_IP_IMO_TRIMMABLE_BY_USB) */

        #if(CY_IP_SRSSV2)
           
            /* Save CY_SYS_CLK_IMO_TRIM4_REG and set IMO gain for USB lock */
            CySysClkImoTrim4 = CY_SYS_CLK_IMO_TRIM4_REG;
            
            if(0u != CySysClkUsbCuSortTrim)
            {
                CySysClkImoTrim5 = CY_PWR_BG_TRIM5_REG;
                
                CY_SYS_CLK_IMO_TRIM4_REG = (CySysClkImoTrim4 & (uint32) ~CY_SYS_CLK_IMO_TRIM4_GAIN_MASK) |
                                            CY_SFLASH_USBMODE_IMO_GAIN_TRIM_REG;
                CY_PWR_BG_TRIM5_REG = CY_SFLASH_USBMODE_IMO_TEMPCO_REG;
                
            }
            else
            {
                CY_SYS_CLK_IMO_TRIM4_REG = (CySysClkImoTrim4 & (uint32) ~CY_SYS_CLK_IMO_TRIM4_GAIN_MASK) |
                                            CY_SYS_CLK_IMO_TRIM4_USB_GAIN;

            }

            if (48u == freq)
            {
                CY_SYS_CLK_IMO_TRIM1_REG = (0u != CySysClkUsbCuSortTrim) ? 
                                           (uint32)CY_SFLASH_CU_IMO_TRIM_USBMODE_48_REG :
                                           (uint32)CY_SFLASH_IMO_TRIM_USBMODE_48_REG;
            }
            else if (24u == freq)
            {
                CY_SYS_CLK_IMO_TRIM1_REG = (0u != CySysClkUsbCuSortTrim) ? 
                                           (uint32)CY_SFLASH_CU_IMO_TRIM_USBMODE_24_REG :
                                           (uint32)CY_SFLASH_IMO_TRIM_USBMODE_24_REG;
            }
            else
            {
                /* Do nothing */
            }
            
        #endif  /* (CY_IP_SRSSV2) */

        CY_SYS_CLK_USBDEVv2_CR1_REG |= CY_SYS_CLK_USBDEVv2_CR1_ENABLE_LOCK;
    }


    /*******************************************************************************
    * Function Name: CySysClkImoDisableUsbLock
    ****************************************************************************//**
    *
    * Disables the IMO to USB lock feature.
    *
    * This function is called from CySysClkImoStop() function if USB lock selected
    * in the Design Wide Resources tab.
    *
    * This is applicable for PSoC 4200L family of devices only. For PSoC 4200L
    * devices, the IMO can lock to either WCO or USB, but not both.
    *
    *******************************************************************************/
    void CySysClkImoDisableUsbLock(void)
    {
        #if(CY_IP_SRSSV2)
            uint32 i;
            
            /* Check for new trim algorithm */
            uint32 CySysClkUsbCuSortTrim = ((CY_SFLASH_S1_TESTPGM_OLD_REV < (CY_SFLASH_S1_TESTPGM_REV_REG & 
                                                                            CY_SFLASH_S1_TESTPGM_REV_MASK)) ? 1u : 0u);

            /* Get current IMO frequency based on the register value */
            uint32 freq = CY_SYS_CLK_IMO_MIN_FREQ_MHZ;;

            for(i = 0u; i < CY_SYS_CLK_IMO_FREQ_TABLE_SIZE; i++)
            {
                if ((uint8) (CY_SYS_CLK_IMO_TRIM2_REG & CY_SYS_CLK_IMO_FREQ_BITS_MASK) == cyImoFreqMhz2Reg[i])
                {
                    freq = i + CY_SYS_CLK_IMO_FREQ_TABLE_OFFSET;
                    break;
                }
            }           
        #endif  /* (CY_IP_SRSSV2) */
        
        CY_SYS_CLK_USBDEVv2_CR1_REG &= (uint32) ~CY_SYS_CLK_USBDEVv2_CR1_ENABLE_LOCK;

        #if(CY_IP_SRSSV2)
            /* Restore CY_SYS_CLK_IMO_TRIM4_REG */
            CY_SYS_CLK_IMO_TRIM4_REG = ((CY_SYS_CLK_IMO_TRIM4_REG & (uint32) ~CY_SYS_CLK_IMO_TRIM4_GAIN_MASK) |
                                        (CySysClkImoTrim4 & CY_SYS_CLK_IMO_TRIM4_GAIN_MASK));

            if(0u != CySysClkUsbCuSortTrim)
            {
                CY_PWR_BG_TRIM5_REG = CySysClkImoTrim5;
            }
            
            CY_SYS_CLK_IMO_TRIM1_REG = CY_SFLASH_IMO_TRIM_REG(freq - CY_SYS_CLK_IMO_FREQ_TABLE_OFFSET);
            
        #endif  /* (CY_IP_SRSSV2) */
    }


    /*******************************************************************************
    * Function Name: CySysClkImoGetUsbLock
    ****************************************************************************//**
    *
    * Reports the IMO to USB lock enable state.
    *
    * This is applicable for PSoC 4200L family of devices only. For PSoC 4200L
    * devices, the IMO can lock to either WCO or USB, but not both.
    *
    * \return 1 if IMO to USB lock is enabled.
    * \return 0 if IMO to USB lock is disabled.
    *
    *******************************************************************************/
    uint32 CySysClkImoGetUsbLock(void)
    {
        return ((0u != (CY_SYS_CLK_USBDEVv2_CR1_REG & CY_SYS_CLK_USBDEVv2_CR1_ENABLE_LOCK)) ?
                (uint32) 1u :
                (uint32) 0u);
    }
#endif  /* (CY_IP_IMO_TRIMMABLE_BY_USB) */



/*******************************************************************************
* Function Name: CySysClkWriteHfclkDirect
****************************************************************************//**
*
* Selects the direct source for the HFCLK.
*
* The new source must be running and stable before calling this function.
*
* PSoC 4000:
* The SYSCLK has a maximum speed of 16 MHz, so HFCLK and SYSCLK dividers should
* be selected in a way to not to exceed 16 MHz for the System clock.
*
* If the SYSCLK clock frequency increases during device operation, call
* CySysFlashSetWaitCycles() with the appropriate parameter to adjust the number
* of clock cycles the cache will wait before sampling data comes back from
* Flash. If the SYSCLK clock frequency decreases, you can call
* CySysFlashSetWaitCycles() to improve the CPU performance. See
* CySysFlashSetWaitCycles() description for more information.
*
* Do not select PLL as the source for HFCLK if PLL output frequency exceeds
* maximum permissible value for HFCLK.
*
* \param clkSelect One of the available HFCLK direct sources.
* CY_SYS_CLK_HFCLK_IMO     IMO.
* CY_SYS_CLK_HFCLK_EXTCLK  External clock pin.
* CY_SYS_CLK_HFCLK_ECO     External crystal oscillator. Applicable for
*                          PSoC 4100 BLE / PSoC 4200 BLE / PSoC 4200L /
*                          4100S with ECO.
* CY_SYS_CLK_HFCLK_PLL0    PLL#0. Applicable for PSoC 4200L /
*                          4100S with PLL.
* CY_SYS_CLK_HFCLK_PLL1    PLL#1. Applicable for PSoC 4200L.
*
*******************************************************************************/
void CySysClkWriteHfclkDirect(uint32 clkSelect)
{
    uint8  interruptState;
    uint32 tmpReg;
    
    #if (CY_IP_SRSSLT && CY_IP_PLL)
        uint8 i = 0u;
    #endif /* (CY_IP_SRSSLT && CY_IP_PLL) */

    interruptState = CyEnterCriticalSection();

#if (CY_IP_SRSSLT && CY_IP_PLL)
    if ((CY_SYS_CLK_HFCLK_PLL0 == clkSelect) || (CY_SYS_CLK_HFCLK_ECO == clkSelect))
    {
        tmpReg = CY_SYS_CLK_SELECT_REG & ~CY_SYS_CLK_SELECT_DIRECT_SEL_MASK;
        tmpReg |= CY_SYS_CLK_HFCLK_IMO;
        CY_SYS_CLK_SELECT_REG = tmpReg;        
        
        /* SRSSLT block does not have registers to select PLL. It is part of EXCO */ 
        tmpReg = CY_SYS_ECO_CLK_SELECT_REG & ~CY_SYS_ECO_CLK_SELECT_ECO_PLL_MASK;
        tmpReg |= ((clkSelect & CY_SYS_CLK_SELECT_HFCLK_SEL_PLL_MASK) >> CY_SYS_CLK_SELECT_HFCLK_PLL_SHIFT);
        CY_SYS_ECO_CLK_SELECT_REG = tmpReg;
        
        /* Generate clock sequence to change clock source in CY_SYS_ECO_CLK_SELECT_REG */
        CY_SYS_EXCO_PGM_CLK_REG |= CY_SYS_EXCO_PGM_CLK_ENABLE_MASK;
        
        for(i = 0u; i < CY_SYS_EXCO_PGM_CLK_SEQ_GENERATOR; i++)
        {
            CY_SYS_EXCO_PGM_CLK_REG |= CY_SYS_EXCO_PGM_CLK_CLK_ECO_MASK;
            CY_SYS_EXCO_PGM_CLK_REG &= ~CY_SYS_EXCO_PGM_CLK_CLK_ECO_MASK;
        }
        
        CY_SYS_EXCO_PGM_CLK_REG &= ~CY_SYS_EXCO_PGM_CLK_ENABLE_MASK;    
    }
#endif /* (CY_IP_SRSSLT && CY_IP_PLL) */

    tmpReg = CY_SYS_CLK_SELECT_REG & ~(CY_SYS_CLK_SELECT_DIRECT_SEL_MASK |
                                       CY_SYS_CLK_SELECT_HFCLK_SEL_MASK);

#if (CY_IP_SRSSV2 && CY_IP_PLL)
    if ((CY_SYS_CLK_HFCLK_PLL0 == clkSelect) || (CY_SYS_CLK_HFCLK_PLL1 == clkSelect))
    {
        tmpReg |= (clkSelect & CY_SYS_CLK_SELECT_HFCLK_SEL_MASK);
    }
    else
#endif /* (CY_IP_SRSSV2 && CY_IP_PLL) */
    {
        tmpReg |= (clkSelect & CY_SYS_CLK_SELECT_DIRECT_SEL_MASK);
    }   
    
    CY_SYS_CLK_SELECT_REG = tmpReg;
    
    CyExitCriticalSection(interruptState);
}


/*******************************************************************************
* Function Name: CySysEnablePumpClock
****************************************************************************//**
*
* Enables / disables the pump clock.
*
* \param enable  
* CY_SYS_CLK_PUMP_DISABLE - Disables the pump clock
* CY_SYS_CLK_PUMP_ENABLE - Enables and restores the operating source of 
* the pump clock.
*
* \sideeffect
*  Enabling/disabling the pump clock does not guarantee glitch free operation 
*  when changing the IMO parameters or clock divider settings.
*
*******************************************************************************/
void CySysEnablePumpClock(uint32 enable)
{
    #if(CY_IP_SRSSV2)
        if (0u != (CY_SYS_CLK_PUMP_ENABLE & enable))
        {
            CY_SYS_CLK_IMO_CONFIG_REG |= (CySysClkPumpConfig << CY_SYS_CLK_IMO_CONFIG_PUMP_SEL_SHIFT);
        }
        else
        {
            CySysClkPumpConfig = (CY_SYS_CLK_IMO_CONFIG_REG >> CY_SYS_CLK_IMO_CONFIG_PUMP_SEL_SHIFT) & 
                                  CY_SYS_CLK_IMO_CONFIG_PUMP_SEL_MASK;
            CY_SYS_CLK_IMO_CONFIG_REG &= ~(CY_SYS_CLK_IMO_CONFIG_PUMP_SEL_MASK << CY_SYS_CLK_IMO_CONFIG_PUMP_SEL_SHIFT);
        }
    #else /* CY_IP_SRSSLT */
        if (0u != (CY_SYS_CLK_PUMP_ENABLE & enable))
        {
            CY_SYS_CLK_SELECT_REG |= (CySysClkPumpConfig << CY_SYS_CLK_SELECT_PUMP_SEL_SHIFT);
        }
        else
        {
            CySysClkPumpConfig = (CY_SYS_CLK_SELECT_REG >> CY_SYS_CLK_SELECT_PUMP_SEL_SHIFT) & 
                                  CY_SYS_CLK_SELECT_PUMP_SEL_MASK;
            CY_SYS_CLK_SELECT_REG &= ~(CY_SYS_CLK_SELECT_PUMP_SEL_MASK << CY_SYS_CLK_SELECT_PUMP_SEL_SHIFT);
        }
    #endif  /* (CY_IP_SRSSV2) */
}


/*******************************************************************************
* Function Name: CySysClkGetSysclkSource
****************************************************************************//**
*
* Returns the source of the System clock.
*
* \return The same as \ref CySysClkWriteHfclkDirect() function parameters.
*
*******************************************************************************/
uint32 CySysClkGetSysclkSource(void)
{
    uint8  interruptState;
    uint32 sysclkSource;

    interruptState = CyEnterCriticalSection();

#if (CY_IP_SRSSV2 && CY_IP_PLL)
    if ((CY_SYS_CLK_SELECT_REG & CY_SYS_CLK_SELECT_HFCLK_SEL_MASK) != 0u)
    {
        sysclkSource = (CY_SYS_CLK_SELECT_REG & CY_SYS_CLK_SELECT_HFCLK_SEL_MASK);
    }
    else
#endif /* (CY_IP_SRSSV2 && CY_IP_PLL) */
    {
        sysclkSource = (CY_SYS_CLK_SELECT_REG & CY_SYS_CLK_SELECT_DIRECT_SEL_MASK);

    #if (CY_IP_SRSSLT && CY_IP_PLL)
        sysclkSource |= (((uint32)(CY_SYS_ECO_CLK_SELECT_REG & CY_SYS_ECO_CLK_SELECT_ECO_PLL_MASK)) <<
                         CY_SYS_CLK_SELECT_HFCLK_PLL_SHIFT);
    #endif /* (CY_IP_SRSSLT && CY_IP_PLL) */
        
    }

    CyExitCriticalSection(interruptState);

    return (sysclkSource);
}


/*******************************************************************************
* Function Name: CySysClkWriteSysclkDiv
****************************************************************************//**
*
* Selects the prescaler divide amount for SYSCLK from HFCLK.
*
* PSoC 4000: The SYSCLK has the speed of 16 MHz, so HFCLK and SYSCLK dividers
* should be selected in a way, not to exceed 16 MHz for SYSCLK.
*
* PSoC 4100 \ PSoC 4100 BLE \ PSoC 4100M: The SYSCLK has the speed of 24 MHz,
* so HFCLK and SYSCLK dividers should be selected in a way, not to exceed 24 MHz
* for SYSCLK.
*
* If the SYSCLK clock frequency increases during the device operation, call
* \ref CySysFlashSetWaitCycles() with the appropriate parameter to adjust the
* number of clock cycles the cache will wait before sampling data comes back
* from Flash. If the SYSCLK clock frequency decreases, you can call
* \ref CySysFlashSetWaitCycles() to improve the CPU performance. See
* \ref CySysFlashSetWaitCycles() description for more information.
*
* \param divider Power of 2 prescaler selection
*  CY_SYS_CLK_SYSCLK_DIV1        SYSCLK = HFCLK / 1
*  CY_SYS_CLK_SYSCLK_DIV2        SYSCLK = HFCLK / 2
*  CY_SYS_CLK_SYSCLK_DIV4        SYSCLK = HFCLK / 4
*  CY_SYS_CLK_SYSCLK_DIV8        SYSCLK = HFCLK / 8
*  CY_SYS_CLK_SYSCLK_DIV16       SYSCLK = HFCLK / 16  (N/A for 4000 Family)
*  CY_SYS_CLK_SYSCLK_DIV32       SYSCLK = HFCLK / 32  (N/A for 4000 Family)
*  CY_SYS_CLK_SYSCLK_DIV64       SYSCLK = HFCLK / 64  (N/A for 4000 Family)
*  CY_SYS_CLK_SYSCLK_DIV128      SYSCLK = HFCLK / 128 (N/A for 4000 Family)
*
*******************************************************************************/
void CySysClkWriteSysclkDiv(uint32 divider)
{
    uint8  interruptState;

    interruptState = CyEnterCriticalSection();

    CY_SYS_CLK_SELECT_REG = ((uint32)(((uint32)divider & CY_SYS_CLK_SELECT_SYSCLK_DIV_MASK) <<
                                    CY_SYS_CLK_SELECT_SYSCLK_DIV_SHIFT)) |
                            (CY_SYS_CLK_SELECT_REG & ((uint32)(~(uint32)(CY_SYS_CLK_SELECT_SYSCLK_DIV_MASK <<
                                    CY_SYS_CLK_SELECT_SYSCLK_DIV_SHIFT))));

    CyExitCriticalSection(interruptState);
}


/*******************************************************************************
* Function Name: CySysClkWriteImoFreq
****************************************************************************//**
*
* Sets the frequency of the IMO.
*
* PSoC 4000: The SYSCLK has the speed of 16 MHz, so HFCLK and SYSCLK dividers
* should be selected in a way, not to exceed 16 MHz for SYSCLK.
*
* PSoC 4100 \ PSoC 4100 BLE \ PSoC 4100M: The SYSCLK has the speed of 24 MHz,
* so HFCLK and SYSCLK dividers should be selected in a way, not to exceed 24 MHz
* for SYSCLK.
*
* For PSoC 4200M and PSoC 4200L device families, if WCO lock feature is enabled
* then this API will disable the lock, write the new IMO frequency and then
* re-enable the lock.
*
* For PSoC 4200L device families, this function enables the USB lock when 24 or
* 48 MHz passed as a parameter if the USB lock option is enabled in Design Wide
* Resources tab or CySysClkImoEnableUsbLock() was called before. Note the USB
* lock is disabled during IMO frequency change.
*
* The CPU is halted if new frequency is invalid and project is compiled
* in debug mode.
*
* If the SYSCLK clock frequency increases during the device operation, call
* \ref CySysFlashSetWaitCycles() with the appropriate parameter to adjust the
* number of clock cycles the cache will wait before sampling data comes back
* from Flash. If the SYSCLK clock frequency decreases, you can call
* \ref CySysFlashSetWaitCycles() to improve the CPU performance. See
* \ref CySysFlashSetWaitCycles() description for more information.
*
* PSoC 4000: The System Clock (SYSCLK) has maximum speed of 16 MHz, so HFCLK
* and SYSCLK dividers should be selected in a way, to not to exceed 16 MHz for
* the System clock.
*
* \param freq All PSoC 4 families excluding the following: Valid range [3-48]
* with step size equals 1. PSoC 4000: Valid values are 24, 32, and 48.
* PSoC 4000S / PSoC 4100S / PSoC Analog Coprocessor: Valid range [24-48] with 
* step size equals 4.
*
*******************************************************************************/
#if(CY_IP_SRSSV2)
    void CySysClkWriteImoFreq(uint32 freq)
    {
        #if (CY_IP_FMLT)
            volatile uint32   parameters[2u];
            volatile uint32   regValues[4u];
        #else
            uint8  bgTrim4;
            uint8  bgTrim5;
            uint8  newImoTrim2Value;
            uint8  currentImoTrim2Value;
        #endif /* (CY_IP_FM) */

        #if (CY_IP_IMO_TRIMMABLE_BY_WCO)
            uint32 wcoLock = 0u;
        #endif  /* (CY_IP_IMO_TRIMMABLE_BY_WCO) */

        #if (CY_IP_IMO_TRIMMABLE_BY_USB)
            uint32 usbLock = 0u;
        #endif  /* (CY_IP_IMO_TRIMMABLE_BY_USB) */

        uint8  interruptState;


        interruptState = CyEnterCriticalSection();

        #if (CY_IP_IMO_TRIMMABLE_BY_WCO)
            if(0u != CySysClkImoGetWcoLock())
            {
                wcoLock = 1u;
                CySysClkImoDisableWcoLock();
            }
        #endif  /* (CY_IP_IMO_TRIMMABLE_BY_WCO) */


        #if (CY_IP_IMO_TRIMMABLE_BY_USB)

            #if (CYDEV_IMO_TRIMMED_BY_USB == 0u)
                if(0u != CySysClkImoGetUsbLock())
                {
            #endif  /* (CYDEV_IMO_TRIMMED_BY_USB == 0u) */

                if ((24u == freq) || (48u == freq))
                {
                    usbLock = 1u;
                    CySysClkImoDisableUsbLock();
                }

            #if (CYDEV_IMO_TRIMMED_BY_USB == 0u)
                }
            #endif  /* (CYDEV_IMO_TRIMMED_BY_USB == 0u) */

        #endif  /* (CYDEV_IMO_TRIMMED_BY_USB == 0u) */


        #if (CY_IP_FMLT)

            /* FM-Lite Clock Restore */
            regValues[0u] = CY_SYS_CLK_IMO_CONFIG_REG;
            regValues[1u] = CY_SYS_CLK_SELECT_REG;
            regValues[2u] = cyImoFreqMhz2Reg[freq - CY_SYS_CLK_IMO_FREQ_TABLE_OFFSET];
            regValues[3u] = CY_FLASH_CTL_REG;

            parameters[0u] =
                (uint32) ((CY_FLASH_KEY_TWO(CY_FLASH_API_OPCODE_CLK_RESTORE) <<  CY_FLASH_PARAM_KEY_TWO_OFFSET) |
                            CY_FLASH_KEY_ONE);
            parameters[1u] = (uint32) &regValues[0u];

            CY_FLASH_CPUSS_SYSARG_REG = (uint32) &parameters[0u];
            CY_FLASH_CPUSS_SYSREQ_REG = CY_FLASH_CPUSS_REQ_START | CY_FLASH_API_OPCODE_CLK_RESTORE;
            (void) CY_FLASH_CPUSS_SYSARG_REG;

        #else /* (CY_IP_FM) */

        if ((freq >= CY_SYS_CLK_IMO_MIN_FREQ_MHZ) && (freq <= CY_SYS_CLK_IMO_MAX_FREQ_MHZ))
        {
            if(freq <= CY_SFLASH_IMO_MAXF0_REG)
            {
                bgTrim4 = CY_SFLASH_IMO_ABS0_REG;
                bgTrim5 = CY_SFLASH_IMO_TMPCO0_REG;
            }
            else if(freq <= CY_SFLASH_IMO_MAXF1_REG)
            {
                bgTrim4 = CY_SFLASH_IMO_ABS1_REG;
                bgTrim5 = CY_SFLASH_IMO_TMPCO1_REG;
            }
            else if(freq <= CY_SFLASH_IMO_MAXF2_REG)
            {
                bgTrim4 = CY_SFLASH_IMO_ABS2_REG;
                bgTrim5 = CY_SFLASH_IMO_TMPCO2_REG;
            }
            else if(freq <= CY_SFLASH_IMO_MAXF3_REG)
            {
                bgTrim4 = CY_SFLASH_IMO_ABS3_REG;
                bgTrim5 = CY_SFLASH_IMO_TMPCO3_REG;
            }
            else
            {
                bgTrim4 = CY_SFLASH_IMO_ABS4_REG;
                bgTrim5 = CY_SFLASH_IMO_TMPCO4_REG;
            }

            /* Get IMO_TRIM2 value for the new frequency */
            newImoTrim2Value = cyImoFreqMhz2Reg[freq - CY_SYS_CLK_IMO_FREQ_TABLE_OFFSET];


            /****************************************************************************
            * The IMO can have a different trim per frequency. To avoid possible corner
            * cases where a trim change can exceed the maximum frequency, the trim must
            * be applied at a frequency that is low enough.
            *
            * Comparing IMO_TRIM2 values for the current and new frequencies, since
            * IMO_TRIM2 value as a function of IMO frequency is a strictly increasing
            * function and is time-invariant.
            ***************************************************************************/
            if ((newImoTrim2Value >= CY_SYS_CLK_IMO_BOUNDARY_FREQ_TRIM2) && (freq >= CY_SYS_CLK_IMO_BOUNDARY_FREQ_MHZ))
            {
                /* Set boundary IMO frequency: safe for IMO above 48 MHZ trimming */
                CY_SYS_CLK_IMO_TRIM2_REG = (uint32) cyImoFreqMhz2Reg[CY_SYS_CLK_IMO_TEMP_FREQ_MHZ -
                                                                     CY_SYS_CLK_IMO_FREQ_TABLE_OFFSET];

                CyDelayCycles(CY_SYS_CLK_IMO_FREQ_TIMEOUT_CYCLES);

                currentImoTrim2Value = CY_SYS_CLK_IMO_TEMP_FREQ_TRIM2;
            }
            else
            {
                currentImoTrim2Value = (uint8) (CY_SYS_CLK_IMO_TRIM2_REG & CY_SYS_CLK_IMO_FREQ_BITS_MASK);
            }


            /***************************************************************************
            * A trim change needs to be allowed to settle (within 5us) before the Freq
            * can be changed to a new frequency.
            *
            * Comparing IMO_TRIM2 values for the current and new frequencies, since
            * IMO_TRIM2 value as a function of IMO frequency is a strictly increasing
            * function and is time-invariant.
            ***************************************************************************/
            if (newImoTrim2Value < currentImoTrim2Value)
            {
                /* Set new IMO frequency */
                CY_SYS_CLK_IMO_TRIM2_REG = cyImoFreqMhz2Reg[freq - CY_SYS_CLK_IMO_FREQ_TABLE_OFFSET];
                CyDelayCycles(CY_SYS_CLK_IMO_FREQ_TIMEOUT_CYCLES);
            }

            /* Set trims for the new IMO frequency */
            CY_SYS_CLK_IMO_TRIM1_REG = (uint32) CY_SFLASH_IMO_TRIM_REG(freq - CY_SYS_CLK_IMO_FREQ_TABLE_OFFSET);
            CY_PWR_BG_TRIM4_REG  = bgTrim4;
            CY_PWR_BG_TRIM5_REG  = bgTrim5;
            CyDelayUs(CY_SYS_CLK_IMO_TRIM_TIMEOUT_US);

            if (newImoTrim2Value > currentImoTrim2Value)
            {
                /* Set new IMO frequency */
                CY_SYS_CLK_IMO_TRIM2_REG = cyImoFreqMhz2Reg[freq - CY_SYS_CLK_IMO_FREQ_TABLE_OFFSET];
                CyDelayCycles(CY_SYS_CLK_IMO_FREQ_TIMEOUT_CYCLES);
            }
        }
        else
        {
            /* Halt CPU in debug mode if new frequency is invalid */
            CYASSERT(0u != 0u);
        }

        #endif /* (CY_IP_FMLT) */

        #if (CY_IP_IMO_TRIMMABLE_BY_WCO)
            if (1u == wcoLock)
            {
                CySysClkImoEnableWcoLock();
            }
        #endif  /* (CY_IP_IMO_TRIMMABLE_BY_WCO) */

        #if (CY_IP_IMO_TRIMMABLE_BY_USB)
            if (1u == usbLock)
            {
                CySysClkImoEnableUsbLock();
            }
        #endif  /* (CY_IP_IMO_TRIMMABLE_BY_USB) */

        CyExitCriticalSection(interruptState);
    }

#else

    void CySysClkWriteImoFreq(uint32 freq)
    {
        uint8  interruptState;
        uint8  imoTrim1Value;

        #if (CY_IP_IMO_TRIMMABLE_BY_WCO)
            uint32 wcoLock = 0u;
        #endif  /* (CY_IP_IMO_TRIMMABLE_BY_WCO) */        
        
        #if (CY_IP_IMO_TRIMMABLE_BY_USB)
            uint32 usbLock = 0u;
        #endif  /* (CY_IP_IMO_TRIMMABLE_BY_USB) */

        #if (CY_PSOC4_4000)
            if ((freq == 24u) || (freq == 32u) || (freq == 48u))
        #elif (CY_CCG3)
            if ((freq == 24u) || (freq == 36u) || (freq == 48u))
        #else
            if ((freq == 24u) || (freq == 28u) || (freq == 32u) ||
                (freq == 36u) || (freq == 40u) || (freq == 44u) ||
                (freq == 48u))
        #endif /* (CY_PSOC4_4000) */
        {
            interruptState = CyEnterCriticalSection();

            #if (CY_IP_IMO_TRIMMABLE_BY_WCO)
                if(0u != CySysClkImoGetWcoLock())
                {
                    wcoLock = 1u;
                    CySysClkImoDisableWcoLock();
                }
            #endif  /* (CY_IP_IMO_TRIMMABLE_BY_WCO) */

            #if (CY_IP_IMO_TRIMMABLE_BY_USB)

                #if (CYDEV_IMO_TRIMMED_BY_USB == 0u)
                    if(0u != CySysClkImoGetUsbLock())
                    {
                #endif  /* (CYDEV_IMO_TRIMMED_BY_USB == 0u) */

                    if (48u == freq)
                    {
                        usbLock = 1u;
                        CySysClkImoDisableUsbLock();
                    }

                #if (CYDEV_IMO_TRIMMED_BY_USB == 0u)
                    }
                #endif  /* (CYDEV_IMO_TRIMMED_BY_USB == 0u) */

            #endif  /* (CYDEV_IMO_TRIMMED_BY_USB == 0u) */


            /* Set IMO to 24 MHz - CLK_IMO_SELECT.FREQ = 0 */
            CY_SYS_CLK_IMO_SELECT_REG &= ((uint32) ~CY_SYS_CLK_IMO_SELECT_FREQ_MASK);


            /* Apply coarse trim */
            #if (CY_IP_IMO_TRIMMABLE_BY_USB)
                if ((1u == usbLock) && (48u == freq))
                {
                    imoTrim1Value = CY_SFLASH_IMO_TRIM_USBMODE_48_REG;
                }
                else if ((1u == usbLock) && (24u == freq))
                {
                    imoTrim1Value = CY_SFLASH_IMO_TRIM_USBMODE_24_REG;
                }
                else
                {
                    imoTrim1Value = (uint8) CY_SFLASH_IMO_TRIM_REG(freq - CY_SYS_CLK_IMO_MIN_FREQ_MHZ);
                }
            #else
                imoTrim1Value = (uint8) CY_SFLASH_IMO_TRIM_REG(freq - CY_SYS_CLK_IMO_MIN_FREQ_MHZ);
            #endif  /* (CY_IP_IMO_TRIMMABLE_BY_USB) */
            CY_SYS_CLK_IMO_TRIM1_REG = (uint32) imoTrim1Value;

            /* Zero out fine trim */
            CY_SYS_CLK_IMO_TRIM2_REG = CY_SYS_CLK_IMO_TRIM2_REG & ((uint32) ~CY_SYS_CLK_IMO_TRIM2_FSOFFSET_MASK);

            /* Apply TC trim */
            CY_SYS_CLK_IMO_TRIM3_REG = (CY_SYS_CLK_IMO_TRIM3_REG & ((uint32) ~CY_SYS_CLK_IMO_TRIM3_VALUES_MASK)) |
                (CY_SFLASH_IMO_TCTRIM_REG(freq - CY_SYS_CLK_IMO_MIN_FREQ_MHZ) & CY_SYS_CLK_IMO_TRIM3_VALUES_MASK);

            CyDelayCycles(CY_SYS_CLK_IMO_TRIM_DELAY_CYCLES);

            if (freq > CY_SYS_CLK_IMO_MIN_FREQ_MHZ)
            {
                /* Select nearby intermediate frequency */
                CY_SYS_CLK_IMO_SELECT_REG = (CY_SYS_CLK_IMO_SELECT_REG & ((uint32) ~CY_SYS_CLK_IMO_SELECT_FREQ_MASK)) |
                    (((freq - 4u - CY_SYS_CLK_IMO_MIN_FREQ_MHZ) >> 2u) & CY_SYS_CLK_IMO_SELECT_FREQ_MASK);

                CyDelayCycles(CY_SYS_CLK_IMO_TRIM_DELAY_CYCLES);

                /* Make small step to final frequency */
                /* Select nearby intermediate frequency */
                CY_SYS_CLK_IMO_SELECT_REG = (CY_SYS_CLK_IMO_SELECT_REG & ((uint32) ~CY_SYS_CLK_IMO_SELECT_FREQ_MASK)) |
                    (((freq - CY_SYS_CLK_IMO_MIN_FREQ_MHZ) >> 2u) & CY_SYS_CLK_IMO_SELECT_FREQ_MASK);
            }

            #if (CY_IP_IMO_TRIMMABLE_BY_WCO)
                if (1u == wcoLock)
                {
                    CySysClkImoEnableWcoLock();
                }
            #endif  /* (CY_IP_IMO_TRIMMABLE_BY_WCO) */
        
        
            #if (CY_IP_IMO_TRIMMABLE_BY_USB)
                if (1u == usbLock)
                {
                    CySysClkImoEnableUsbLock();
                }
            #endif  /* (CY_IP_IMO_TRIMMABLE_BY_USB) */

            CyExitCriticalSection(interruptState);
        }
        else
        {
            /* Halt CPU in debug mode if new frequency is invalid */
            CYASSERT(0u != 0u);
        }
    }

#endif /* (CY_IP_SRSSV2) */


#if(CY_IP_SRSSLT)
    /*******************************************************************************
    * Function Name: CySysClkWriteHfclkDiv
    ****************************************************************************//**
    *
    * Selects the pre-scaler divider value for HFCLK from IMO.
    *
    * The HFCLK predivider allows the device to divide the HFCLK selection mux
    * input before use as HFCLK. The predivider is capable of dividing the HFCLK by
    * powers of 2 between 1 and 8.
    *
    * PSoC 4000: The SYSCLK has the speed of 16 MHz, so HFCLK and SYSCLK dividers
    * should be selected in a way, not to exceed 16 MHz for SYSCLK.
    *
    * If the SYSCLK clock frequency increases during the device operation, call
    * \ref CySysFlashSetWaitCycles() with the appropriate parameter to adjust the
    * number of clock cycles the cache will wait before sampling data comes back
    * from Flash. If the SYSCLK clock frequency decreases, you can call
    * \ref CySysFlashSetWaitCycles() to improve the CPU performance. See
    * \ref CySysFlashSetWaitCycles() description for more information.
    *
    * \param \ref CY_SYS_CLK_HFCLK_DIV_NODIV    Transparent mode (w/o dividing)
    * \param \ref CY_SYS_CLK_HFCLK_DIV_2        Divide selected clock source by 2
    * \param \ref CY_SYS_CLK_HFCLK_DIV_4        Divide selected clock source by 4
    * \param \ref CY_SYS_CLK_HFCLK_DIV_8        Divide selected clock source by 8
    *
    *******************************************************************************/
    void CySysClkWriteHfclkDiv(uint32 divider)
    {
        uint8  interruptState;

        interruptState = CyEnterCriticalSection();

        CY_SYS_CLK_SELECT_REG = ((CY_SYS_CLK_SELECT_REG & ((uint32) (~(CY_SYS_CLK_SELECT_HFCLK_DIV_MASK <<
                                        CY_SYS_CLK_SELECT_HFCLK_DIV_SHIFT)))) |
                    ((uint32)((divider & CY_SYS_CLK_SELECT_HFCLK_DIV_MASK) << CY_SYS_CLK_SELECT_HFCLK_DIV_SHIFT)));

        CyExitCriticalSection(interruptState);
    }
#endif /* (CY_IP_SRSSLT) */


#if (CY_IP_ECO)

    /*******************************************************************************
    * Function Name: CySysClkEcoStart
    ****************************************************************************//**
    *
    * Starts the External Crystal Oscillator (ECO). Refer to the device datasheet
    * for the ECO startup time.
    *
    * The timeout interval is measured based on the system frequency defined by
    * PSoC Creator at build time. If System clock frequency is changed in
    * runtime, the \ref CyDelayFreq() with the appropriate parameter should be
    * called.
    *
    * PSoC 4100 BLE / PSoC 4200 BLE: The WCO must be enabled prior to enabling ECO.
    *
    * \param timeoutUs Timeout in microseconds.
    *
    * If zero is specified, the function does not wait for timeout and returns
    * CYRET_SUCCESS. If non-zero is specified, the function waits for the timeout.
    *
    * \return \ref CYRET_SUCCESS Completed successfully. The ECO is oscillating and
    * amplitude reached 60% and it does not mean 24 MHz crystal is within 50 ppm.
    *
    * \return \ref CYRET_TIMEOUT Timeout occurred. If the crystal is not oscillating
    * or amplitude didn't reach 60% after specified amount of time, CYRET_TIMEOUT
    * is returned.
    *
    * \return \ref CYRET_BAD_PARAM One or more invalid parameters.
    *
    *******************************************************************************/
    cystatus CySysClkEcoStart(uint32 timeoutUs)
    {
        cystatus returnStatus = CYRET_SUCCESS;

        #if (CY_IP_ECO_BLESS)
            /* Enable the RF oscillator band gap */
            CY_SYS_XTAL_BLESS_RF_CONFIG_REG |= CY_SYS_XTAL_BLESS_RF_CONFIG_RF_ENABLE;

            /* Update trimming register */
            CY_SYS_XTAL_BLERD_BB_XO_REG = CY_SYS_XTAL_BLERD_BB_XO_TRIM;

            /* Enable the Crystal */
            CY_SYS_XTAL_BLERD_DBUS_REG |= CY_SYS_XTAL_BLERD_DBUS_XTAL_ENABLE;
        
        #elif (CY_IP_ECO_BLESSV3)
            uint32 regConfig;
            uint32 intrRegMaskStore = 0u;
            
            if (0u != (CY_SYS_BLESS_MT_CFG_REG & (CY_SYS_BLESS_MT_CFG_ENABLE_BLERD <<  CYFLD_BLE_BLESS_ENABLE_BLERD__OFFSET)))
            {
                CY_SYS_BLESS_MT_CFG_REG |= (CY_SYS_BLESS_MT_CFG_DPSLP_ECO_ON  <<  CYFLD_BLE_BLESS_DPSLP_ECO_ON__OFFSET);
            }
            else
            {
                /* Init BLE core */
                CY_SYS_BLESS_MT_DELAY_CFG_REG = CY_SYS_BLESS_MT_DELAY_CFG_INIT;
                CY_SYS_BLESS_MT_DELAY_CFG2_REG = CY_SYS_BLESS_MT_DELAY_CFG2_INIT;
                CY_SYS_BLESS_MT_DELAY_CFG3_REG = CY_SYS_BLESS_MT_DELAY_CFG3_INIT;
            
                /* RCB init */
                regConfig = CY_SYS_RCB_CTRL_REG;
                regConfig &= CY_SYS_RCB_CTRL_CLEAR;
                regConfig |= CY_SYS_RCB_CTRL_INIT;
                CY_SYS_RCB_CTRL_REG = regConfig;
                
                intrRegMaskStore = CY_SYS_BLESS_INTR_MASK_REG;
                if(0u != (CY_SYS_BLESS_BLERD_ACTIVE_INTR_MASK & intrRegMaskStore))
                {
                    CY_SYS_BLESS_INTR_MASK_REG &= ~CY_SYS_BLESS_BLERD_ACTIVE_INTR_MASK;
                }
                
                /* Enable BLE core */
                regConfig = CY_SYS_BLESS_MT_CFG_REG;
                regConfig &= CY_SYS_BLESS_MT_CFG_CLEAR;
                regConfig |= CY_SYS_BLESS_MT_CFG_INIT;
                CY_SYS_BLESS_MT_CFG_REG = regConfig;

                while(0u == ((CY_SYS_BLESS_BLERD_ACTIVE_INTR_STAT & CY_SYS_BLESS_INTR_STAT_REG)))
                {
                    /* Wait until BLERD55 moves to active state */
                }

                if(0u != (CY_SYS_BLESS_BLERD_ACTIVE_INTR_MASK & intrRegMaskStore))
                {
                    CY_SYS_BLESS_INTR_MASK_REG |= CY_SYS_BLESS_BLERD_ACTIVE_INTR_MASK;
                }
                
                /* Send write commands to RBUS */
                CY_SYS_RCB_TX_FIFO_WR_REG = CY_SYS_RCB_RBUS_FREQ_NRST_SET;
                CY_SYS_RCB_TX_FIFO_WR_REG = CY_SYS_RCB_RBUS_DIG_CLK_SET;
                
                #if (CY_SYS_BLE_CLK_ECO_FREQ_32MHZ == CYDEV_ECO_CLK_MHZ)
                    CY_SYS_RCB_TX_FIFO_WR_REG = CY_SYS_RCB_RBUS_FREQ_XTAL_DIV_SET;
                    CY_SYS_RCB_TX_FIFO_WR_REG = (CY_SYS_RCB_RBUS_RF_DCXO_CFG_SET | CY_SYS_RCB_RBUS_IB_VAL); 
                #else
                    CY_SYS_RCB_TX_FIFO_WR_REG = CY_SYS_RCB_RBUS_FREQ_XTAL_NODIV_SET;
                #endif
                
                intrRegMaskStore = CY_SYS_BLESS_INTR_MASK_REG;
                if(0u != (CY_SYS_RCB_INTR_RCB_DONE & intrRegMaskStore))
                {
                    CY_SYS_BLESS_INTR_MASK_REG &= ~(CY_SYS_RCB_INTR_RCB_DONE | CY_SYS_RCB_INTR_RCB_RX_FIFO_NOT_EMPTY);
                }
                
                /* Send read commands to RBUS */
                CY_SYS_RCB_TX_FIFO_WR_REG = (CY_SYS_RCB_RBUS_RD_CMD | 
                                            (CY_SYS_RCB_RBUS_RF_DCXO_CFG_SET & ~CY_SYS_RCB_RBUS_VAL_MASK));
                                            
                while (0u == (CY_SYS_RCB_INTR_RCB_RX_FIFO_NOT_EMPTY & CY_SYS_RCB_INTR_REG))
                {
                    /* Wait until RX_FIFO_NOT_EMPTY state */
                }

                CY_SYS_RCB_INTR_REG |= CY_SYS_RCB_INTR_RCB_DONE;
                
                regConfig = CY_SYS_RCB_RX_FIFO_RD_REG & CY_SYS_RCB_RBUS_TRIM_MASK;
                
                /* Send write commands to RBUS */
                CY_SYS_RCB_TX_FIFO_WR_REG = (CY_SYS_RCB_RBUS_RF_DCXO_CFG_SET | regConfig | CY_SYS_RCB_RBUS_TRIM_VAL);
                
                while (0u == (CY_SYS_RCB_INTR_RCB_DONE & CY_SYS_RCB_INTR_REG))
                {
                    /* Wait until RCB_DONE state */
                }
                
                /* Clear Interrupt */
                CY_SYS_RCB_INTR_REG = CY_SYS_RCB_INTR_CLEAR;
                
                if(0u != ((CY_SYS_RCB_INTR_RCB_DONE | CY_SYS_RCB_INTR_RCB_RX_FIFO_NOT_EMPTY) & intrRegMaskStore))
                {
                    CY_SYS_BLESS_INTR_MASK_REG |= intrRegMaskStore;
                }
                
            }
        #else /* CY_IP_ECO_SRSSV2 || CY_IP_ECO_SRSSLT */
            CY_SYS_CLK_ECO_CONFIG_REG |= CY_SYS_CLK_ECO_CONFIG_ENABLE;
            CyDelayUs(CY_SYS_CLK_ECO_CONFIG_CLK_EN_TIMEOUT_US);
            CY_SYS_CLK_ECO_CONFIG_REG |= CY_SYS_CLK_ECO_CONFIG_CLK_EN;
        #endif /* (CY_IP_ECO_BLESS) */

        if(timeoutUs > 0u)
        {
            returnStatus = CYRET_TIMEOUT;

            for( ; timeoutUs > 0u; timeoutUs--)
            {
                CyDelayUs(1u);

                if(0u != CySysClkEcoReadStatus())
                {
                    returnStatus = CYRET_SUCCESS;
                    break;
                }
            }

        }

        return(returnStatus);
    }


    /*******************************************************************************
    * Function Name: CySysClkEcoStop
    ****************************************************************************//**
    *
    * Stops the megahertz crystal.
    *
    * If ECO is disabled when it is sourcing HFCLK, the CPU will halt. In addition,
    * for PSoC 4100 BLE / PSoC 4200 BLE devices, the BLE sub-system will stop
    * functioning.
    *
    *******************************************************************************/
    void CySysClkEcoStop(void)
    {
        #if (CY_IP_WCO_BLESS)
            /* Disable the RF oscillator band gap */
            CY_SYS_XTAL_BLESS_RF_CONFIG_REG &= (uint32) ~CY_SYS_XTAL_BLESS_RF_CONFIG_RF_ENABLE;

            /* Disable the Crystal */
            CY_SYS_XTAL_BLERD_DBUS_REG &= (uint32) ~CY_SYS_XTAL_BLERD_DBUS_XTAL_ENABLE;
        #elif (CY_IP_ECO_BLESSV3)
            CY_SYS_BLESS_MT_CFG_REG &= ~(CY_SYS_BLESS_MT_CFG_DPSLP_ECO_ON  <<  CYFLD_BLE_BLESS_DPSLP_ECO_ON__OFFSET);
        #else
            CY_SYS_CLK_ECO_CONFIG_REG &= (uint32) ~(CY_SYS_CLK_ECO_CONFIG_ENABLE | CY_SYS_CLK_ECO_CONFIG_CLK_EN);
        #endif /* (CY_IP_WCO_BLESS) */
    }


    /*******************************************************************************
    * Function Name: CySysClkEcoReadStatus
    ****************************************************************************//**
    *
    * Reads the status bit for the megahertz crystal.
    *
    * For PSoC 4100 BLE / PSoC 4200 BLE devices, the status bit is the
    * XO_AMP_DETECT bit in FSM register.
    *
    * For PSoC 4200L / 4100S with ECO devices, the error status bit is the 
    * WATCHDOG_ERROR bit in ECO_STATUS register.
    *
    * \return PSoC 4100 BLE/PSoC 4200 BLE: Non-zero indicates that ECO output
    * reached 50 ppm and is oscillating in valid range.
    *
    * \return PSoC 4200L / 4100S with ECO: Non-zero indicates that ECO is running.
    *
    *******************************************************************************/
    uint32 CySysClkEcoReadStatus(void)
    {
        uint32 returnValue;

        #if (CY_IP_WCO_BLESS)
            returnValue = CY_SYS_XTAL_BLERD_FSM_REG & CY_SYS_XTAL_BLERD_FSM_XO_AMP_DETECT;
        #elif (CY_IP_ECO_BLESSV3)
            returnValue = (CY_SYS_BLESS_MT_STATUS_REG & CY_SYS_BLESS_MT_STATUS_CURR_STATE_MASK) >> CYFLD_BLE_BLESS_MT_CURR_STATE__OFFSET;

            returnValue =  ((CY_SYS_BLESS_MT_STATUS_BLERD_IDLE == returnValue) || 
                            (CY_SYS_BLESS_MT_STATUS_SWITCH_EN  == returnValue) ||
                            (CY_SYS_BLESS_MT_STATUS_ACTIVE  ==  returnValue) ||
                            (CY_SYS_BLESS_MT_STATUS_ISOLATE == returnValue));
        #else
            returnValue = (0u != (CY_SYS_CLK_ECO_STATUS_REG & CY_SYS_CLK_ECO_STATUS_WATCHDOG_ERROR)) ? 0u : 1u;
        #endif /* (CY_IP_WCO_BLESS) */

        return (returnValue);
    }

    #if (CY_IP_ECO_BLESS || CY_IP_ECO_BLESSV3)
        /*******************************************************************************
        * Function Name: CySysClkWriteEcoDiv
        ****************************************************************************//**
        *
        * Selects value for the ECO divider.
        *
        * The ECO must not be the HFCLK clock source when this function is called.
        * The HFCLK source can be changed to the other clock source by call to the
        * CySysClkWriteHfclkDirect() function. If the ECO sources the HFCLK this
        * function will not have any effect if compiler in release mode, and halt the
        * CPU when compiler in debug mode.
        *
        * If the SYSCLK clock frequency increases during the device operation, call
        * CySysFlashSetWaitCycles() with the appropriate parameter to adjust the number
        * of clock cycles the cache will wait before sampling data comes back from
        * Flash. If the SYSCLK clock frequency decreases, you can call
        * CySysFlashSetWaitCycles() to improve the CPU performance. See
        * CySysFlashSetWaitCycles() description for more information.
        *
        * \param divider Power of 2 divider selection.
        * - \ref CY_SYS_CLK_ECO_DIV1
        * - \ref CY_SYS_CLK_ECO_DIV2
        * - \ref CY_SYS_CLK_ECO_DIV4
        * - \ref CY_SYS_CLK_ECO_DIV8
        *
        *******************************************************************************/
        void CySysClkWriteEcoDiv(uint32 divider)
        {
            uint8  interruptState;

            if (CY_SYS_CLK_HFCLK_ECO != (CY_SYS_CLK_SELECT_REG & CY_SYS_CLK_SELECT_DIRECT_SEL_MASK))
            {
                interruptState = CyEnterCriticalSection();

                CY_SYS_CLK_XTAL_CLK_DIV_CONFIG_REG =  (divider & CY_SYS_CLK_XTAL_CLK_DIV_MASK) |
                                                      (CY_SYS_CLK_XTAL_CLK_DIV_CONFIG_REG & ((uint32) ~CY_SYS_CLK_XTAL_CLK_DIV_MASK));

                CyExitCriticalSection(interruptState);
            }
            else
            {
                /* Halt CPU in debug mode if ECO sources HFCLK */
                CYASSERT(0u != 0u);
            }
        }

    #else

        /*******************************************************************************
        * Function Name: CySysClkConfigureEcoTrim
        ****************************************************************************//**
        *
        * Selects trim setting values for ECO. This API is available only for PSoC
        * 4200L / 4100S with ECO devices only.
        *
        * The following parameters can be trimmed for ECO. The affected registers are
        * ECO_TRIM0 and ECO_TRIM1.
        *
        * Watchdog trim - This bit field sets the error threshold below the steady
        * state amplitude level.
        *
        * Amplitude trim - This bit field is to set the crystal drive level when
        * ECO_CONFIG.AGC_EN = 1. WARNING: use care when setting this field because
        * driving a crystal beyond its rated limit can permanently damage the crystal.
        *
        * Filter frequency trim - This bit field sets LPF frequency trim and affects
        * the 3rd harmonic content.
        *
        * Feedback resistor trim - This bit field sets the feedback resistor trim and
        * impacts the oscillation amplitude.
        *
        * Amplifier gain trim - This bit field sets the amplifier gain trim and affects
        * the startup time of the crystal.
        *
        * Use care when setting the amplitude trim field because driving a crystal
        * beyond its rated limit can permanently damage the crystal.
        *
        * \param wDTrim: Watchdog trim
        * - \ref CY_SYS_CLK_ECO_WDTRIM0      Error threshold is 0.05 V
        * - \ref CY_SYS_CLK_ECO_WDTRIM1      Error threshold is 0.10 V
        * - \ref CY_SYS_CLK_ECO_WDTRIM2      Error threshold is 0.15 V
        * - \ref CY_SYS_CLK_ECO_WDTRIM3      Error threshold is 0.20 V
        *
        * \param aTrim: Amplitude trim
        * - \ref CY_SYS_CLK_ECO_ATRIM0       Amplitude is 0.3 Vpp
        * - \ref CY_SYS_CLK_ECO_ATRIM1       Amplitude is 0.4 Vpp
        * - \ref CY_SYS_CLK_ECO_ATRIM2       Amplitude is 0.5 Vpp
        * - \ref CY_SYS_CLK_ECO_ATRIM3       Amplitude is 0.6 Vpp
        * - \ref CY_SYS_CLK_ECO_ATRIM4       Amplitude is 0.7 Vpp
        * - \ref CY_SYS_CLK_ECO_ATRIM5       Amplitude is 0.8 Vpp
        * - \ref CY_SYS_CLK_ECO_ATRIM6       Amplitude is 0.9 Vpp
        * - \ref CY_SYS_CLK_ECO_ATRIM7       Amplitude is 1.0 Vpp
        *
        * \param fTrim: Filter frequency trim
        * - \ref CY_SYS_CLK_ECO_FTRIM0       Crystal frequency > 30 MHz
        * - \ref CY_SYS_CLK_ECO_FTRIM1       24 MHz < Crystal frequency <= 30 MHz
        * - \ref CY_SYS_CLK_ECO_FTRIM2       17 MHz < Crystal frequency <= 24 MHz
        * - \ref CY_SYS_CLK_ECO_FTRIM3       Crystal frequency <= 17 MHz
        *
        * \param rTrim: Feedback resistor trim
        * - \ref CY_SYS_CLK_ECO_RTRIM0       Crystal frequency > 30 MHz
        * - \ref CY_SYS_CLK_ECO_RTRIM1       24 MHz < Crystal frequency <= 30 MHz
        * - \ref CY_SYS_CLK_ECO_RTRIM2       17 MHz < Crystal frequency <= 24 MHz
        * - \ref CY_SYS_CLK_ECO_RTRIM3       Crystal frequency <= 17 MHz
        *
        * \param gTrim: Amplifier gain trim. Calculate the minimum required gm
        * (trans-conductance value). Divide the calculated gm value by 4.5 to
        * obtain an integer value 'result'. For more information please refer
        * to the device TRM.
        * - \ref CY_SYS_CLK_ECO_GTRIM0       If result = 1
        * - \ref CY_SYS_CLK_ECO_GTRIM1       If result = 0
        * - \ref CY_SYS_CLK_ECO_GTRIM2       If result = 2
        * - \ref CY_SYS_CLK_ECO_GTRIM2       If result = 3
        *
        *******************************************************************************/
        void CySysClkConfigureEcoTrim(uint32 wDTrim, uint32 aTrim, uint32 fTrim, uint32 rTrim, uint32 gTrim)
        {
            uint8  interruptState;
            uint32 regTmp;

            interruptState = CyEnterCriticalSection();

            regTmp  = CY_SYS_CLK_ECO_TRIM0_REG & ~(CY_SYS_CLK_ECO_TRIM0_WDTRIM_MASK | CY_SYS_CLK_ECO_TRIM0_ATRIM_MASK);
            regTmp |= ((uint32) (wDTrim << CY_SYS_CLK_ECO_TRIM0_WDTRIM_SHIFT) & CY_SYS_CLK_ECO_TRIM0_WDTRIM_MASK);
            regTmp |= ((uint32) (aTrim << CY_SYS_CLK_ECO_TRIM0_ATRIM_SHIFT) & CY_SYS_CLK_ECO_TRIM0_ATRIM_MASK);
            CY_SYS_CLK_ECO_TRIM0_REG = regTmp;

            regTmp  = CY_SYS_CLK_ECO_TRIM1_REG & ~(CY_SYS_CLK_ECO_TRIM1_FTRIM_MASK |
                                                   CY_SYS_CLK_ECO_TRIM1_RTRIM_MASK |
                                                   CY_SYS_CLK_ECO_TRIM1_GTRIM_MASK);
            regTmp |= ((uint32) (fTrim << CY_SYS_CLK_ECO_TRIM1_FTRIM_SHIFT) & CY_SYS_CLK_ECO_TRIM1_FTRIM_MASK);
            regTmp |= ((uint32) (rTrim << CY_SYS_CLK_ECO_TRIM1_RTRIM_SHIFT) & CY_SYS_CLK_ECO_TRIM1_RTRIM_MASK);
            regTmp |= ((uint32) (gTrim << CY_SYS_CLK_ECO_TRIM1_GTRIM_SHIFT) & CY_SYS_CLK_ECO_TRIM1_GTRIM_MASK);

            CY_SYS_CLK_ECO_TRIM1_REG = regTmp;

            CyExitCriticalSection(interruptState);
        }


        /*******************************************************************************
        * Function Name: CySysClkConfigureEcoDrive
        ****************************************************************************//**
        *
        * Selects trim setting values for ECO based on crystal parameters. Use care
        * when setting the driveLevel parameter because driving a crystal beyond its
        * rated limit can permanently damage the crystal.
        *
        * This API is available only for PSoC 4200L / 4100S with ECO devices only.
        *
        * \param freq Frequency of the crystal in kHz.
        * \param cLoad Crystal load capacitance in pF.
        * \param esr Equivalent series resistance of the crystal in ohm.
        *  maxAmplitude: maximum amplitude level in mV. Calculate as
        *  ((sqrt(driveLevel in uW / 2 / esr))/(3.14 * freq * cLoad)) * 10^9.
        *
        *  The Automatic Gain Control (AGC) is disabled when the specified maximum
        *  amplitude level equals or above 2. In this case the amplitude is not
        *  explicitly controlled and will grow until it saturates to the supply rail
        *  (1.8V nom). WARNING: use care when disabling AGC because driving a crystal
        *  beyond its rated limit can permanently damage the crystal.
        *
        * \return \ref CYRET_SUCCESS ECO configuration completed successfully.
        * \return \ref CYRET_BAD_PARAM One or more invalid parameters.
        *
        *******************************************************************************/
        cystatus CySysClkConfigureEcoDrive(uint32 freq, uint32 cLoad, uint32 esr, uint32 maxAmplitude)
        {
            cystatus returnStatus = CYRET_SUCCESS;

            uint32 wDTrim;
            uint32 aTrim;
            uint32 fTrim;
            uint32 rTrim;
            uint32 gTrim;

            uint32 gmMin;


            if ((maxAmplitude < CY_SYS_CLK_ECO_MAX_AMPL_MIN_mV) ||
                (freq < CY_SYS_CLK_ECO_FREQ_KHZ_MIN) || (freq > CY_SYS_CLK_ECO_FREQ_KHZ_MAX))
            {
                returnStatus = CYRET_BAD_PARAM;
            }
            else
            {
                /* Calculate amplitude trim */
                aTrim = (maxAmplitude < CY_SYS_CLK_ECO_TRIM_BOUNDARY) ? ((maxAmplitude/100u) - 4u) : 7u;

                if (maxAmplitude < CY_SYS_CLK_ECO_AMPL_FOR_ATRIM0)
                {
                    aTrim = CY_SYS_CLK_ECO_ATRIM0;
                }
                else if (maxAmplitude < CY_SYS_CLK_ECO_AMPL_FOR_ATRIM1)
                {
                    aTrim = CY_SYS_CLK_ECO_ATRIM1;
                }
                else if (maxAmplitude < CY_SYS_CLK_ECO_AMPL_FOR_ATRIM2)
                {
                    aTrim = CY_SYS_CLK_ECO_ATRIM2;
                }
                else if (maxAmplitude < CY_SYS_CLK_ECO_AMPL_FOR_ATRIM3)
                {
                    aTrim = CY_SYS_CLK_ECO_ATRIM3;
                }
                else if (maxAmplitude < CY_SYS_CLK_ECO_AMPL_FOR_ATRIM4)
                {
                    aTrim = CY_SYS_CLK_ECO_ATRIM4;
                }
                else if (maxAmplitude < CY_SYS_CLK_ECO_AMPL_FOR_ATRIM5)
                {
                    aTrim = CY_SYS_CLK_ECO_ATRIM5;
                }
                else if (maxAmplitude < CY_SYS_CLK_ECO_AMPL_FOR_ATRIM6)
                {
                    aTrim = CY_SYS_CLK_ECO_ATRIM6;
                }
                else
                {
                    aTrim = CY_SYS_CLK_ECO_ATRIM7;
                }

                /* Calculate Watchdog trim. */
                wDTrim = (maxAmplitude < CY_SYS_CLK_ECO_TRIM_BOUNDARY) ? ((maxAmplitude/200u) - 2u) : 3u;

                /* Calculate amplifier gain trim. */
                gmMin = (uint32) (((((CY_SYS_CLK_ECO_GMMIN_COEFFICIENT * freq * cLoad) / 1000) * ((freq * cLoad * esr) / 1000)) / 100u) / 4500000u);
                if (gmMin > 3u)
                {
                    returnStatus = CYRET_BAD_PARAM;
                    gTrim = 0u;
                }
                else if (gmMin > 1u)
                {
                    gTrim = gmMin;
                }
                else
                {
                    gTrim = (gmMin == 1u) ? 0u : 1u;
                }

                /* Calculate feedback resistor trim */
                if (freq > CY_SYS_CLK_ECO_FREQ_FOR_FTRIM0)
                {
                    rTrim = CY_SYS_CLK_ECO_FTRIM0;
                }
                else if (freq > CY_SYS_CLK_ECO_FREQ_FOR_FTRIM1)
                {
                    rTrim = CY_SYS_CLK_ECO_FTRIM1;
                }
                else if (freq > CY_SYS_CLK_ECO_FREQ_FOR_FTRIM2)
                {
                    rTrim = CY_SYS_CLK_ECO_FTRIM2;
                }
                else
                {
                    rTrim = CY_SYS_CLK_ECO_FTRIM3;
                }

                /* Calculate filter frequency trim */
                fTrim = rTrim;

                CySysClkConfigureEcoTrim(wDTrim, aTrim, fTrim, rTrim, gTrim);

                /* Automatic Gain Control (AGC) enable */
                if (maxAmplitude < 2u)
                {
                    /* The oscillation amplitude is controlled to the level selected by amplitude trim */
                    CY_SYS_CLK_ECO_CONFIG_REG |= CY_SYS_CLK_ECO_CONFIG_AGC_EN;
                }
                else
                {
                    /* The amplitude is not explicitly controlled and will grow until it saturates to the
                    * supply rail (1.8V nom).
                    */
                    CY_SYS_CLK_ECO_CONFIG_REG &= (uint32) ~CY_SYS_CLK_ECO_CONFIG_AGC_EN;
                }
            }

            return (returnStatus);
        }

    #endif /* CY_IP_ECO_BLESS */

#endif /* (CY_IP_ECO) */


#if (CY_IP_PLL)
    /*******************************************************************************
    * Function Name: CySysClkPllStart
    ****************************************************************************//**
    *
    * Enables the PLL. Optionally waits for it to become stable. Waits at least
    * 250 us or until it is detected that the PLL is stable.
    *
    * Clears the unlock occurred status bit by calling CySysClkPllGetUnlockStatus(),
    * once the PLL is locked if the wait parameter is 1).
    *
    * This API is available only for PSoC 4200L / 4100S with PLL devices.
    *
    * \param PLL:
    * 0   PLL#0
    * 1   PLL#1 (available only for PSoC 4200L)
    *
    * \param wait:
    * 0 - Return immediately after configuration.
    * 1 - Wait for PLL lock or timeout. This API shall use the CyDelayUs() to
    * implement the timeout feature.
    *
    * \return CYRET_SUCCESS Completed successfully.
    * \return CYRET_TIMEOUT The timeout occurred without detecting a stable clock.
    * If the input source of the clock is jittery, then the lock indication may
    * not occur. However, after the timeout has expired, the generated PLL clock can
    * still be used.
    * \return CYRET_BAD_PARAM - Either the PLL or wait parameter is invalid.
    *
    *******************************************************************************/
    cystatus CySysClkPllStart(uint32 pll, uint32 wait)
    {
        uint32 counts = CY_SYS_CLK_PLL_MAX_STARTUP_US;
        uint8  interruptState;
        cystatus returnStatus = CYRET_SUCCESS;

        if((pll < CY_IP_PLL_NR) && (wait <= 1u))
        {
            interruptState = CyEnterCriticalSection();

            /* Isolate PLL outputs */
            CY_SYS_CLK_PLL_BASE.pll[pll].config &= (uint32) ~CY_SYS_CLK_PLL_CONFIG_ISOLATE;

            /* Enable PLL */
            CY_SYS_CLK_PLL_BASE.pll[pll].config |= CY_SYS_CLK_PLL_CONFIG_ENABLE;

            CyExitCriticalSection(interruptState);

            /* De-isolate >= CY_SYS_CLK_PLL_MIN_STARTUP_US after PLL enabled */
            CyDelayUs(CY_SYS_CLK_PLL_MIN_STARTUP_US);
            interruptState = CyEnterCriticalSection();
            CY_SYS_CLK_PLL_BASE.pll[pll].config |= CY_SYS_CLK_PLL_CONFIG_ISOLATE;
            CyExitCriticalSection(interruptState);

            if(wait != 0u)
            {
                returnStatus = CYRET_TIMEOUT;

                while(0u != counts)
                {

                    if(0u != CySysClkPllGetLockStatus(pll))
                    {
                        returnStatus = CYRET_SUCCESS;
                        (void) CySysClkPllGetUnlockStatus(pll);
                        break;
                    }

                    CyDelayUs(1u);
                    counts--;
                }
            }
        }
        else
        {
            returnStatus = CYRET_BAD_PARAM;
        }

        return (returnStatus);
    }


    /*******************************************************************************
    * Function Name: CySysClkPllGetLockStatus
    ****************************************************************************//**
    *
    * Returns non-zero if the output of the specified PLL output is locked.
    *
    * This API is available only for PSoC 4200L / 4100S with PLL devices.
    *
    * PLL:
    *  0   PLL#0
    *  1   PLL#1 (available only for PSoC 4200L)
    *
    * \return A non-zero value when the specified PLL is locked.
    *
    *******************************************************************************/
    uint32 CySysClkPllGetLockStatus(uint32 pll)
    {
        uint8  interruptState;
        uint32 returnStatus;

        CYASSERT(pll < CY_IP_PLL_NR);

        interruptState = CyEnterCriticalSection();

        /* PLL is locked if reported so for two consecutive read. */
        returnStatus = CY_SYS_CLK_PLL_BASE.pll[pll].status & CY_SYS_CLK_PLL_STATUS_LOCKED;
        if(0u != returnStatus)
        {
            returnStatus = CY_SYS_CLK_PLL_BASE.pll[pll].status & CY_SYS_CLK_PLL_STATUS_LOCKED;
        }

        CyExitCriticalSection(interruptState);

        return (returnStatus);
    }

    /*******************************************************************************
    * Function Name: CySysClkPllStop
    ****************************************************************************//**
    *
    * Disables the PLL.
    *
    * Ensures that either PLL is not the source of HFCLK before it is disabled,
    * otherwise, the CPU will halt.
    *
    * This API is available only for PSoC 4200L / 4100S with PLL devices.
    *
    * PLL:
    *  0   PLL#0
    *  1   PLL#1 (available only for PSoC 4200L)
    *
    *******************************************************************************/
    void CySysClkPllStop(uint32 pll)
    {
        uint8  interruptState;

        if (pll < CY_IP_PLL_NR)
        {
            interruptState = CyEnterCriticalSection();
            CY_SYS_CLK_PLL_BASE.pll[pll].config &= (uint32) ~(CY_SYS_CLK_PLL_CONFIG_ISOLATE | CY_SYS_CLK_PLL_CONFIG_ENABLE);
            CyExitCriticalSection(interruptState);
        }
    }


    /*******************************************************************************
    * Function Name: CySysClkPllSetPQ
    ****************************************************************************//**
    *
    * Sets feedback (P) and reference the (Q) divider value. This API also sets the
    * programmable charge pump current value. Note that the PLL has to be disabled
    * before calling this API. If this function is called while any PLL is sourcing,
    * the SYSCLK will return an error.
    *
    * The PLL must not be the system clock source when calling this function. The
    * PLL output will glitch during this function call.
    *
    * This API is available only for PSoC 4200L / 4100S with PLL devices.
    *
    * \param PLL:
    *  0   PLL#0
    *  1   PLL#1 (available only for PSoC 4200L)
    *
    * \param feedback The P divider. Range 4 - 259. Control bits for the feedback
    * divider.
    *
    * \param reference The Q divider. Range 1 - 64. Divide by the reference.
    *
    * \param current Charge the pump current in uA. The 2 uA for output frequencies
    * of 67 MHz or less, and 3 uA for higher output frequencies. The default
    * value is 2 uA.
    *
    * \return CYRET_SUCCESS Completed successfully.
    * \return CYRET_BAD_PARAM The parameters are out of range or the specified PLL
    * sources the system clock.
    *
    *******************************************************************************/
    cystatus CySysClkPllSetPQ(uint32 pll, uint32 feedback, uint32 reference, uint32 current)
    {
        uint32   regTmp;
        cystatus tmp;
        uint8    interruptState;
        cystatus returnStatus = CYRET_BAD_PARAM;

        interruptState = CyEnterCriticalSection();

        tmp = CySysClkPllConfigChangeAllowed(pll);

        if ((pll < CY_IP_PLL_NR) &&
            (feedback  >= CY_SYS_CLK_PLL_CONFIG_FEEDBACK_DIV_MIN)  && (feedback  <= CY_SYS_CLK_PLL_CONFIG_FEEDBACK_DIV_MAX)  &&
            (reference >= CY_SYS_CLK_PLL_CONFIG_REFERENCE_DIV_MIN) && (reference <= CY_SYS_CLK_PLL_CONFIG_REFERENCE_DIV_MAX) &&
            (current   >= CY_SYS_CLK_PLL_CONFIG_ICP_SEL_MIN )      && (current   <= CY_SYS_CLK_PLL_CONFIG_ICP_SEL_MAX) &&
            (CYRET_SUCCESS == tmp))
        {
            /* Set new feedback, reference and current values */
            regTmp  = CY_SYS_CLK_PLL_BASE.pll[pll].config & (uint32) ~(CY_SYS_CLK_PLL_CONFIG_FEEDBACK_DIV_MASK  |
                                                                       CY_SYS_CLK_PLL_CONFIG_REFERENCE_DIV_MASK |
                                                                       CY_SYS_CLK_PLL_CONFIG_ICP_SEL_MASK);

            regTmp |= ((feedback << CY_SYS_CLK_PLL_CONFIG_FEEDBACK_DIV_SHIFT) & CY_SYS_CLK_PLL_CONFIG_FEEDBACK_DIV_MASK);
            regTmp |= (((reference - 1u) << CY_SYS_CLK_PLL_CONFIG_REFERENCE_DIV_SHIFT) & CY_SYS_CLK_PLL_CONFIG_REFERENCE_DIV_MASK);
            regTmp |= ((current << CY_SYS_CLK_PLL_CONFIG_ICP_SEL_SHIFT) & CY_SYS_CLK_PLL_CONFIG_ICP_SEL_MASK);

            CY_SYS_CLK_PLL_BASE.pll[pll].config = regTmp;

            returnStatus = CYRET_SUCCESS;
        }

        CyExitCriticalSection(interruptState);

        return (returnStatus);
    }


    /*******************************************************************************
    * Function Name: CySysClkPllSetBypassMode
    ****************************************************************************//**
    *
    * Sets the bypass mode for the specified PLL.
    *
    * The PLL must not be the system clock source when calling this function.
    * The PLL output will glitch during this function call.
    *
    * When the PLL's reference input is higher than HFCLK frequency the device may 
    * lock due to incorrect flash wait cycle configuration and bypass switches from 
    * PLL output to the reference input. See description of 
    * CySysFlashSetWaitCycles() for more information.
    *
    * This API is available only for PSoC 4200L / 4100S with PLL devices.
    *
    * \param PLL:
    *  0   PLL#0
    *  1   PLL#1 (available only for PSoC 4200L)
    *
    * \param bypass: The bypass mode.
    * CY_SYS_PLL_BYPASS_AUTO - Automatic usage of the lock indicator. When unlocked,
    * automatically selects PLL the reference input (bypass mode). When locked,
    * automatically selects the PLL output.
    *
    * CY_SYS_PLL_BYPASS_PLL_REF - Selects the PLL reference input (bypass mode).
    * Ignores the lock indicator.
    *
    * CY_SYS_PLL_BYPASS_PLL_OUT - Selects the PLL output.  Ignores the lock indicator.
    *
    *******************************************************************************/
    void CySysClkPllSetBypassMode(uint32 pll, uint32 bypass)
    {
        uint32 regTmp;
        uint8  interruptState;

        interruptState = CyEnterCriticalSection();

        if ((pll < CY_IP_PLL_NR) && (bypass <= CY_SYS_PLL_BYPASS_PLL_OUT))
        {
            regTmp  = CY_SYS_CLK_PLL_BASE.pll[pll].config & (uint32) ~CY_SYS_CLK_PLL_CONFIG_BYPASS_SEL_MASK;
            regTmp |=  (uint32)(bypass << CY_SYS_CLK_PLL_CONFIG_BYPASS_SEL_SHIFT);
            CY_SYS_CLK_PLL_BASE.pll[pll].config = regTmp;
        }

        CyExitCriticalSection(interruptState);
    }


    /*******************************************************************************
    * Function Name: CySysClkPllGetBypassMode
    ****************************************************************************//**
    *
    *  Gets the bypass mode for the specified PLL.
    *  This API is available only for PSoC 4200L / 4100S with PLL devices.
    *
    *  \param PLL:
    *   0   PLL#0
    *   1   PLL#1 (available only for PSoC 4200L)
    *
    *  \param bypass: Bypass mode.
    *   The same as the parameter of the CySysClkPllSetBypassMode().
    *
    *******************************************************************************/
    static uint32 CySysClkPllGetBypassMode(uint32 pll)
    {
        uint32 returnValue;
        uint8  interruptState;

        CYASSERT(pll < CY_IP_PLL_NR);

        interruptState = CyEnterCriticalSection();

        returnValue = CY_SYS_CLK_PLL_BASE.pll[pll].config & CY_SYS_CLK_PLL_CONFIG_BYPASS_SEL_MASK;
        returnValue =  returnValue >> CY_SYS_CLK_PLL_CONFIG_BYPASS_SEL_SHIFT;

        CyExitCriticalSection(interruptState);

        return (returnValue);
    }


    /*******************************************************************************
    * Function Name: CySysClkPllConfigChangeAllowed
    ****************************************************************************//**
    *
    *  The function returns non-zero value if the specified PLL sources the System
    *  clock and the PLL is not in the bypass mode.
    *
    *  This API is available only for PSoC 4200L / 4100S with PLL devices.
    *
    *  \param PLL:
    *   0   PLL#0
    *   1   PLL#1 (available only for PSoC 4200L)
    *
    * \return Non-zero value when the specified PLL sources the System clock and
    * the PLL is not in the bypass mode.
    *
    *******************************************************************************/
    static cystatus CySysClkPllConfigChangeAllowed(uint32 pll)
    {
        uint32   pllBypassMode;
        uint32   sysclkSource;
        cystatus returnValue = CYRET_INVALID_STATE;

        sysclkSource  = CySysClkGetSysclkSource();
        pllBypassMode = CySysClkPllGetBypassMode(pll);

        if ((CY_SYS_PLL_BYPASS_PLL_REF == pllBypassMode)             ||
            ((CY_SYS_CLK_HFCLK_PLL0 != sysclkSource) && (0u == pll))
        #if (CY_IP_SRSSV2)
            || ((CY_SYS_CLK_HFCLK_PLL1 != sysclkSource) && (1u == pll))
        #endif /* (CY_IP_SRSSV2) */
            )
        {
            returnValue = CYRET_SUCCESS;
        }

        return (returnValue);
    }


    /*******************************************************************************
    * Function Name: CySysClkPllGetUnlockStatus
    ****************************************************************************//**
    *
    *  Returns a non-zero value if the specified PLL output was unlocked.
    *  The unlock status is an indicator that the PLL has lost a lock at least once
    *  during its operation. The unlock status is cleared once it is read using
    *  this API.
    *
    *  This API is available only for PSoC 4200L / 4100S with PLL devices.
    *
    *  \param PLL:
    *   0   PLL#0
    *   1   PLL#1 (available only for PSoC 4200L)
    *
    * \return Non-zero value when the specified PLL was unlocked.
    *
    *******************************************************************************/
    uint32 CySysClkPllGetUnlockStatus(uint32 pll)
    {
        uint32 returnStatus = 0u;
        uint8  interruptState;

        interruptState = CyEnterCriticalSection();

        returnStatus = CY_SYS_CLK_PLL_BASE.pll[pll].test & CY_SYS_CLK_PLL_TEST_UNLOCK_OCCURRED_MASK;
        CY_SYS_CLK_PLL_BASE.pll[pll].test |= CY_SYS_CLK_PLL_TEST_UNLOCK_OCCURRED_MASK;

        CyExitCriticalSection(interruptState);

        return (returnStatus);
    }


    /*******************************************************************************
    * Function Name: CySysClkPllSetFrequency
    ****************************************************************************//**
    *
    * Configures either PLL#0 or PLL#1 for the requested input/output frequencies.
    * The input frequency is the frequency of the source to the PLL. The source is
    * set using the CySysClkPllSetSource() function.
    *
    * The PLL must not be the system clock source when calling this function. The
    * PLL output will glitch during this function call.
    *
    * This API is available only for PSoC 4200L / 4100S with PLL devices.
    *
    * \param pll:
    *  0   PLL#0
    *  1   PLL#1 (available only for PSoC 4200L)
    *
    * \param inputFreq The reference frequency in KHz. The valid range is from 1000 to 49152 KHz.
    *
    * \param pllFreq The target frequency in KHz. The valid range is from 22500 to 49152 KHz.
    *
    * \param divider The output clock divider for the PLL:
    * CY_SYS_PLL_OUTPUT_DIVPASS    Pass Through
    * CY_SYS_PLL_OUTPUT_DIV2       Divide by 2
    * CY_SYS_PLL_OUTPUT_DIV4       Divide by 4
    * CY_SYS_PLL_OUTPUT_DIV8       Divide by 8
    *
    * \param freqTol The tolerance in ppm, 10 ppm is equal to 0.001%.
    *
    * \return CYRET_SUCCESS The PLL was successfully configured for the requested
    * frequency.
    *
    * \return CYRET_BAD_PARAM The PLL was not able to successfully configure for the
    * requested frequency.
    *
    *******************************************************************************/
    cystatus CySysClkPllSetFrequency(uint32 pll, uint32 inputFreq, uint32 pllFreq, uint32 divider, uint32 freqTol)
    {
        uint32 qMin;
        uint32 qMax;

        uint32 qVal = CY_SYS_CLK_PLL_INVALID;
        uint32 pVal = CY_SYS_CLK_PLL_INVALID;

        uint32 q;
        uint32 p;

        uint32 fvco;
        int32 ferr;

        cystatus tmp;
        cystatus returnStatus = CYRET_BAD_PARAM;


        tmp = CySysClkPllConfigChangeAllowed(pll);

        if ((pll < CY_IP_PLL_NR) &&
            (inputFreq  >= CY_SYS_CLK_PLL_INPUT_FREQ_MIN )  && (inputFreq  <= CY_SYS_CLK_PLL_INPUT_FREQ_MAX) &&
            (pllFreq >= CY_SYS_CLK_PLL_OUTPUT_FREQ_MIN ) && (pllFreq <= CY_SYS_CLK_PLL_OUTPUT_FREQ_MAX) &&
            (divider <= CY_SYS_PLL_OUTPUT_DIV8) &&
            (CYRET_SUCCESS == tmp))
        {

            /* Minimum feed forward loop divisor */
            qMin = (inputFreq + (CY_SYS_CLK_PLL_FPFDMAX - 1u)) / CY_SYS_CLK_PLL_FPFDMAX;
            qMin = (qMin < CY_SYS_CLK_PLL_QMINIP) ? CY_SYS_CLK_PLL_QMINIP : qMin;

            /* Maximum feed forward loop divisor */
            qMax = inputFreq / CY_SYS_CLK_PLL_FPFDMIN;
            qMax = (qMax > CY_SYS_CLK_PLL_QMAXIP) ? CY_SYS_CLK_PLL_QMAXIP : qMax;

            if (qMin <= qMax)
            {
                for(q = qMin; q <= qMax; q++)
                {
                    /* Solve for the feedback divisor value */

                    /* INT((pllFreq * q ) / inputFreq), where INT is normal rounding  */
                    p = ((pllFreq * q) + (inputFreq / 2u)) / inputFreq;

                    /* Calculate the actual VCO frequency (FVCO) */
                    fvco = ((inputFreq * p) / q);

                    /* Calculate the frequency error (FERR) */
                    ferr = ((1000000 * ((int32) fvco - (int32) pllFreq))/ (int32) pllFreq);

                    /* Bound check the frequency error and decide next action */
                    if ((( -1 * (int32) freqTol) <= ferr) && (ferr <= (int32) freqTol))
                    {
                        qVal = q;
                        pVal = p;
                        break;
                    }
                }


                if ((pVal != CY_SYS_CLK_PLL_INVALID) && (qVal != CY_SYS_CLK_PLL_INVALID))
                {
                    if (CySysClkPllSetPQ(pll, pVal, qVal, CY_SYS_CLK_PLL_CURRENT_DEFAULT) == CYRET_SUCCESS)
                    {
                        returnStatus = CySysClkPllSetOutputDivider(pll, divider);
                    }
                }
            }

        }

        return (returnStatus);
    }

    /*******************************************************************************
    * Function Name: CySysClkPllSetSource
    ****************************************************************************//**
    *
    *  Sets the input clock source to the PLL. The PLL must be disabled before
    *  calling this function.
    *
    *  This API is available only for PSoC 4200L / 4100S with PLL devices.
    *
    *  \param PLL:
    *  0   PLL#0
    *  1   PLL#1 (available only for PSoC 4200L)
    *
    * \param source:
    *  CY_SYS_PLL_SOURCE_IMO        IMO
    *  CY_SYS_PLL_SOURCE_EXTCLK     External Clock (available only for PSoC 4200L)
    *  CY_SYS_PLL_SOURCE_ECO        ECO
    *  CY_SYS_PLL_SOURCE_DSI0       DSI_OUT[0] (available only for PSoC 4200L)
    *  CY_SYS_PLL_SOURCE_DSI1       DSI_OUT[1] (available only for PSoC 4200L)
    *  CY_SYS_PLL_SOURCE_DSI2       DSI_OUT[2] (available only for PSoC 4200L)
    *  CY_SYS_PLL_SOURCE_DSI3       DSI_OUT[3] (available only for PSoC 4200L)
    *
    *******************************************************************************/
    void CySysClkPllSetSource(uint32 pll, uint32 source)
    {
        uint32 regTmp;
        uint8  interruptState;
        
        #if (CY_IP_SRSSLT)
            uint8 i = 0u;
        #endif /* (CY_IP_SRSSLT) */

        interruptState = CyEnterCriticalSection();

        if (pll < CY_IP_PLL_NR)
        {
            #if(CY_IP_SRSSV2)
                regTmp = CY_SYS_CLK_SELECT_REG & (uint32) ~CY_SYS_CLK_SELECT_PLL_MASK(pll);
                regTmp |= ((source << CY_SYS_CLK_SELECT_PLL_SHIFT(pll)) & CY_SYS_CLK_SELECT_PLL_MASK(pll));
                CY_SYS_CLK_SELECT_REG = regTmp;
            #else
                regTmp = CY_SYS_ECO_CLK_SELECT_REG & (uint32) ~CY_SYS_ECO_CLK_SELECT_PLL0_MASK;
                regTmp |= ((source << CY_SYS_ECO_CLK_SELECT_PLL0_SHIFT) & CY_SYS_ECO_CLK_SELECT_PLL0_MASK);
                CY_SYS_ECO_CLK_SELECT_REG = regTmp;
                
                /* Generate clock sequence to change clock source in CY_SYS_ECO_CLK_SELECT_REG */
                CY_SYS_EXCO_PGM_CLK_REG |= CY_SYS_EXCO_PGM_CLK_ENABLE_MASK;
                
                for(i = 0u; i < CY_SYS_EXCO_PGM_CLK_SEQ_GENERATOR; i++)
                {
                    CY_SYS_EXCO_PGM_CLK_REG |= CY_SYS_EXCO_PGM_CLK_CLK_ECO_MASK;
                    CY_SYS_EXCO_PGM_CLK_REG &= ~CY_SYS_EXCO_PGM_CLK_CLK_ECO_MASK;
                }
                
                CY_SYS_EXCO_PGM_CLK_REG &= ~CY_SYS_EXCO_PGM_CLK_ENABLE_MASK;

            #endif /* (CY_IP_SRSSV2) */
        }

        CyExitCriticalSection(interruptState);
    }

    
    /*******************************************************************************
    * Function Name: CySysClkPllSetOutputDivider
    ****************************************************************************//**
    *
    *  Sets the output clock divider for the PLL.
    *
    *  The PLL must not be the System Clock source when calling this function. The
    *  PLL output will glitch during this function call.
    *
    *  This API is available only for PSoC 4200L / 4100S with PLL devices.
    *
    *  \param PLL:
    *   0   PLL#0
    *   1   PLL#1 (available only for PSoC 4200L)
    *
    * \param divider:
    *  CY_SYS_PLL_OUTPUT_DIVPASS    Pass through
    *  CY_SYS_PLL_OUTPUT_DIV2       Divide by 2
    *  CY_SYS_PLL_OUTPUT_DIV4       Divide by 4
    *  CY_SYS_PLL_OUTPUT_DIV8       Divide by 8
    *
    * \return \ref CYRET_SUCCESS    Completed successfully.
    * \return \ref CYRET_BAD_PARAM  The parameters are out of range or the
    * specified PLL sources the System clock.
    *
    *******************************************************************************/
    cystatus CySysClkPllSetOutputDivider(uint32 pll, uint32 divider)
    {
        uint32   tmpReg;
        uint8    interruptState;
        cystatus returnStatus = CYRET_BAD_PARAM;
        cystatus tmp;


        interruptState = CyEnterCriticalSection();

        tmp = CySysClkPllConfigChangeAllowed(pll);

        if ((pll < CY_IP_PLL_NR) && (CYRET_SUCCESS == tmp) && (divider <= CY_SYS_PLL_OUTPUT_DIV8))
        {
            tmpReg  = CY_SYS_CLK_PLL_BASE.pll[pll].config & (uint32) ~(CY_SYS_CLK_PLL_CONFIG_OUTPUT_DIV_MASK);
            tmpReg |= ((divider << CY_SYS_CLK_PLL_CONFIG_OUTPUT_DIV_SHIFT) & CY_SYS_CLK_PLL_CONFIG_OUTPUT_DIV_MASK);

            CY_SYS_CLK_PLL_BASE.pll[pll].config = tmpReg;

            returnStatus = CYRET_SUCCESS;
        }

        CyExitCriticalSection(interruptState);

        return (returnStatus);
    }
#endif /* (CY_IP_PLL) */


#if(CY_IP_SRSSV2)

    /*******************************************************************************
    * Function Name: CySysLvdEnable
    ****************************************************************************//**
    *
    *  Enables the output of the low-voltage monitor when Vddd is at or below the
    *  trip point, configures the device to generate an interrupt, and sets the
    *  voltage trip level.
    *
    *  \param threshold: Threshold selection for Low Voltage Detect circuit.
    *  Threshold variation is +/- 2.5% from these typical voltage choices.
    *         Define                     Voltage threshold
    *    CY_LVD_THRESHOLD_1_75_V       1.7500 V
    *    CY_LVD_THRESHOLD_1_80_V       1.8000 V
    *    CY_LVD_THRESHOLD_1_90_V       1.9000 V
    *    CY_LVD_THRESHOLD_2_00_V       2.0000 V
    *    CY_LVD_THRESHOLD_2_10_V       2.1000 V
    *    CY_LVD_THRESHOLD_2_20_V       2.2000 V
    *    CY_LVD_THRESHOLD_2_30_V       2.3000 V
    *    CY_LVD_THRESHOLD_2_40_V       2.4000 V
    *    CY_LVD_THRESHOLD_2_50_V       2.5000 V
    *    CY_LVD_THRESHOLD_2_60_V       2.6000 V
    *    CY_LVD_THRESHOLD_2_70_V       2.7000 V
    *    CY_LVD_THRESHOLD_2_80_V       2.8000 V
    *    CY_LVD_THRESHOLD_2_90_V       2.9000 V
    *    CY_LVD_THRESHOLD_3_00_V       3.0000 V
    *    CY_LVD_THRESHOLD_3_20_V       3.2000 V
    *    CY_LVD_THRESHOLD_4_50_V       4.5000 V
    *
    *******************************************************************************/
    void CySysLvdEnable(uint32 threshold)
    {
        /* Prevent propagating a false interrupt */
        CY_LVD_PWR_INTR_MASK_REG   &= (uint32) ~CY_LVD_PROPAGATE_INT_TO_CPU;

        /* Set specified threshold */
        CY_LVD_PWR_VMON_CONFIG_REG = (CY_LVD_PWR_VMON_CONFIG_REG & ~CY_LVD_PWR_VMON_CONFIG_LVD_SEL_MASK) |
                ((threshold << CY_LVD_PWR_VMON_CONFIG_LVD_SEL_SHIFT) & CY_LVD_PWR_VMON_CONFIG_LVD_SEL_MASK);

        /* Enable the LVD. This may cause a false LVD event. */
        CY_LVD_PWR_VMON_CONFIG_REG |= CY_LVD_PWR_VMON_CONFIG_LVD_EN;

        /* Wait for the circuit to stabilize */
        CyDelayUs(CY_LVD_STABILIZE_TIMEOUT_US);

        /* Clear the false event */
        CySysLvdClearInterrupt();

        /* Unmask the interrupt */
        CY_LVD_PWR_INTR_MASK_REG   |= CY_LVD_PROPAGATE_INT_TO_CPU;
    }


    /*******************************************************************************
    * Function Name: CySysLvdDisable
    ****************************************************************************//**
    *
    *  Disables the low voltage detection. A low voltage interrupt is disabled.
    *
    *******************************************************************************/
    void CySysLvdDisable(void)
    {
        CY_LVD_PWR_INTR_MASK_REG   &= ~CY_LVD_PROPAGATE_INT_TO_CPU;
        CY_LVD_PWR_VMON_CONFIG_REG &= ~CY_LVD_PWR_VMON_CONFIG_LVD_EN;
    }


    /*******************************************************************************
    * Function Name: CySysLvdGetInterruptSource
    ****************************************************************************//**
    *
    *  Gets the low voltage detection interrupt status (without clearing).
    *
    * \return
    *  Interrupt request value:
    *    CY_SYS_LVD_INT - Indicates an Low Voltage Detect interrupt
    *
    *******************************************************************************/
    uint32 CySysLvdGetInterruptSource(void)
    {
        return (CY_LVD_PWR_INTR_REG & CY_SYS_LVD_INT);
    }


    /*******************************************************************************
    * Function Name: CySysLvdClearInterrupt
    ****************************************************************************//**
    *
    *  Clears the low voltage detection interrupt status.
    *
    * \return
    *  None
    *
    *******************************************************************************/
    void CySysLvdClearInterrupt(void)
    {
        CY_LVD_PWR_INTR_REG = CY_SYS_LVD_INT;
    }

#endif /* (CY_IP_SRSSV2) */


/*******************************************************************************
* Function Name: CySysGetResetReason
****************************************************************************//**
*
*  Reports the cause for the latest reset(s) that occurred in the system. All
*  the bits in the RES_CAUSE register assert when the corresponding reset cause
*  occurs and must be cleared by the firmware. These bits are cleared by the
*  hardware only during XRES, POR, or a detected brown-out.
*
*  \param reason: bits in the RES_CAUSE register to clear.
*   CY_SYS_RESET_WDT       - WDT caused a reset
*   CY_SYS_RESET_PROTFAULT - Occured protection violation that requires reset
*   CY_SYS_RESET_SW        - Cortex-M0 requested a system reset.
*
* \return
*  Status. Same enumerated bit values as used for the reason parameter.
*
*******************************************************************************/
uint32 CySysGetResetReason(uint32 reason)
{
    uint32 returnStatus;

    reason &= (CY_SYS_RESET_WDT | CY_SYS_RESET_PROTFAULT | CY_SYS_RESET_SW);
    returnStatus = CY_SYS_RES_CAUSE_REG &
                  (CY_SYS_RESET_WDT | CY_SYS_RESET_PROTFAULT | CY_SYS_RESET_SW);
    CY_SYS_RES_CAUSE_REG = reason;

    return (returnStatus);
}


/*******************************************************************************
* Function Name: CyDisableInts
****************************************************************************//**
*
*  Disables all interrupts.
*
* \return
*  32 bit mask of previously enabled interrupts.
*
*******************************************************************************/
uint32 CyDisableInts(void)
{
    uint32 intState;

    /* Get current interrupt state. */
    intState = CY_INT_CLEAR_REG;

    /* Disable all interrupts. */
    CY_INT_CLEAR_REG = CY_INT_CLEAR_DISABLE_ALL;

    return (intState);
}


/*******************************************************************************
* Function Name: CyEnableInts
****************************************************************************//**
*
*  Enables interrupts to a given state.
*
*  \param mask The 32 bit mask of interrupts to enable.
*
*******************************************************************************/
void CyEnableInts(uint32 mask)
{
    CY_INT_ENABLE_REG = mask;
}


/*******************************************************************************
* Function Name: CyIntSetSysVector
****************************************************************************//**
*
*  Sets the interrupt vector of the specified system interrupt number. These
*  interrupts are for SysTick, PendSV and others.
*
*  \param number: System interrupt number:
*    CY_INT_NMI_IRQN        - Non Maskable Interrupt
*    CY_INT_HARD_FAULT_IRQN - Hard Fault Interrupt
*    CY_INT_SVCALL_IRQN     - SV Call Interrupt
*    CY_INT_PEND_SV_IRQN    - Pend SV Interrupt
*    CY_INT_SYSTICK_IRQN    - System Tick Interrupt
*
*  \param address Pointer to an interrupt service routine.
*
* \return The old ISR vector at this location.
*
*******************************************************************************/
cyisraddress CyIntSetSysVector(uint8 number, cyisraddress address)
{
    cyisraddress oldIsr;
    cyisraddress *ramVectorTable = (cyisraddress *) CY_INT_VECT_TABLE;

    CYASSERT(number < CY_INT_IRQ_BASE);

    /* Save old Interrupt service routine. */
    oldIsr = ramVectorTable[number];

    /* Set new Interrupt service routine. */
    ramVectorTable[number] = address;

    return(oldIsr);
}


/*******************************************************************************
* Function Name: CyIntGetSysVector
****************************************************************************//**
*
*   Gets the interrupt vector of the specified system interrupt number. These
*   interrupts are for SysTick, PendSV and others.
*
*  \param number: System interrupt number:
*    CY_INT_NMI_IRQN        - Non Maskable Interrupt
*    CY_INT_HARD_FAULT_IRQN - Hard Fault Interrupt
*    CY_INT_SVCALL_IRQN     - SV Call Interrupt
*    CY_INT_PEND_SV_IRQN    - Pend SV Interrupt
*    CY_INT_SYSTICK_IRQN    - System Tick Interrupt
*
* \return Address of the ISR in the interrupt vector table.
*
*******************************************************************************/
cyisraddress CyIntGetSysVector(uint8 number)
{
    cyisraddress *ramVectorTable = (cyisraddress *) CY_INT_VECT_TABLE;

    CYASSERT(number < CY_INT_IRQ_BASE);

    return(ramVectorTable[number]);
}


/*******************************************************************************
* Function Name: CyIntSetVector
****************************************************************************//**
*
*  Sets the interrupt vector of the specified interrupt number.
*
*  \param number Valid range [0-31].  Interrupt number
*  \param address Pointer to an interrupt service routine
*
* \return Previous interrupt vector value.
*
*******************************************************************************/
cyisraddress CyIntSetVector(uint8 number, cyisraddress address)
{
    cyisraddress oldIsr;
    cyisraddress *ramVectorTable = (cyisraddress *) CY_INT_VECT_TABLE;

    CYASSERT(number < CY_NUM_INTERRUPTS);

    /* Save old Interrupt service routine. */
    oldIsr = ramVectorTable[CY_INT_IRQ_BASE + number];

    /* Set new Interrupt service routine. */
    ramVectorTable[CY_INT_IRQ_BASE + number] = address;

    return(oldIsr);
}


/*******************************************************************************
* Function Name: CyIntGetVector
****************************************************************************//**
*
*  Gets the interrupt vector of the specified interrupt number.
*
*  \param number: Valid range [0-31].  Interrupt number
*
* \return Address of the ISR in the interrupt vector table.
*
*******************************************************************************/
cyisraddress CyIntGetVector(uint8 number)
{
    cyisraddress *ramVectorTable = (cyisraddress *) CY_INT_VECT_TABLE;

    CYASSERT(number < CY_NUM_INTERRUPTS);

    return (ramVectorTable[CY_INT_IRQ_BASE + number]);
}

/*******************************************************************************
* Function Name: CyIntSetPriority
****************************************************************************//**
*
*  Sets the priority of the interrupt.
*
*  \param priority: Priority of the interrupt. 0 - 3, 0 being the highest.
*  \param number: The number of the interrupt, 0 - 31.
*
*******************************************************************************/
void CyIntSetPriority(uint8 number, uint8 priority)
{
    uint8 interruptState;
    uint32 shift;
    uint32 value;

    CYASSERT(priority <= CY_MIN_PRIORITY);
    CYASSERT(number < CY_NUM_INTERRUPTS);

    shift = CY_INT_PRIORITY_SHIFT(number);

    interruptState = CyEnterCriticalSection();

    value = CY_INT_PRIORITY_REG(number);
    value &= (uint32)(~((uint32)(CY_INT_PRIORITY_MASK << shift)));
    value |= ((uint32)priority << shift);
    CY_INT_PRIORITY_REG(number) = value;

    CyExitCriticalSection(interruptState);
}


/*******************************************************************************
* Function Name: CyIntGetPriority
****************************************************************************//**
*
*  Gets the priority of the interrupt.
*
*  \param number: The number of the interrupt, 0 - 31.
*
* \return
*  Priority of the interrupt. 0 - 3, 0 being the highest.
*
*******************************************************************************/
uint8 CyIntGetPriority(uint8 number)
{
    uint8 priority;

    CYASSERT(number < CY_NUM_INTERRUPTS);

    priority = (uint8) (CY_INT_PRIORITY_REG(number) >> CY_INT_PRIORITY_SHIFT(number));

    return (priority & (uint8) CY_INT_PRIORITY_MASK);
}


/*******************************************************************************
* Function Name: CyIntEnable
****************************************************************************//**
*
*  Enables the specified interrupt number.
*
*  \param number: Valid range [0-31].  Interrupt number
*
*******************************************************************************/
void CyIntEnable(uint8 number)
{
    CY_INT_ENABLE_REG = ((uint32) 0x01u << (CY_INT_ENABLE_RANGE_MASK & number));
}


/*******************************************************************************
* Function Name: CyIntGetState
****************************************************************************//**
*
*  Gets the enable state of the specified interrupt number.
*
*  \param number: Valid range [0-31].  Interrupt number.
*
* \return
*  Enable status: 1 if enabled, 0 if disabled
*
*******************************************************************************/
uint8 CyIntGetState(uint8 number)
{
    /* Get state of interrupt. */
    return ((0u != (CY_INT_ENABLE_REG & ((uint32) 0x01u << (CY_INT_ENABLE_RANGE_MASK & number)))) ? 1u : 0u);
}


/*******************************************************************************
* Function Name: CyIntDisable
****************************************************************************//**
*
*   Disables the specified interrupt number.
*
*   \param number: Valid range [0-31].  Interrupt number.
*
*******************************************************************************/
void CyIntDisable(uint8 number)
{
    CY_INT_CLEAR_REG = ((uint32) 0x01u << (CY_INT_ENABLE_RANGE_MASK & number));
}

/*******************************************************************************
* Function Name: CyIntSetPending
****************************************************************************//**
*
*  Forces the specified interrupt number to be pending.
*
*  \param number: Valid range [0-31].  Interrupt number.
*
*******************************************************************************/
void CyIntSetPending(uint8 number)
{
    CY_INT_SET_PEND_REG = ((uint32) 0x01u << (CY_INT_ENABLE_RANGE_MASK & number));
}


/*******************************************************************************
* Function Name: CyIntClearPending
****************************************************************************//**
*
*  Clears any pending interrupt for the specified interrupt number.
*
*  \param number: Valid range [0-31].  Interrupt number.
*
*******************************************************************************/
void CyIntClearPending(uint8 number)
{
    CY_INT_CLR_PEND_REG = ((uint32) 0x01u << (CY_INT_ENABLE_RANGE_MASK & number));
}


/*******************************************************************************
* Function Name: CyHalt
****************************************************************************//**
*
*  Halts the CPU.
*
*  \param reason: Value to be used during debugging.
*
*******************************************************************************/
void CyHalt(uint8 reason)
{
    if(0u != reason)
    {
        /* To remove unreferenced local variable warning */
    }

    #if defined (__ARMCC_VERSION)
        __breakpoint(0x0);
    #elif defined(__GNUC__) || defined (__ICCARM__)
        __asm("    bkpt    1");
    #elif defined(__C51__)
        CYDEV_HALT_CPU;
    #endif  /* (__ARMCC_VERSION) */
}


/*******************************************************************************
* Function Name: CySoftwareReset
****************************************************************************//**
*
*  Forces a software reset of the device.
*
*******************************************************************************/
void CySoftwareReset(void)
{
    /***************************************************************************
    * Setting the system reset request bit. The vector key value must be written
    * to the register, otherwise the register write is unpredictable.
    ***************************************************************************/
    CY_SYS_AIRCR_REG = (CY_SYS_AIRCR_REG & (uint32)(~CY_SYS_AIRCR_VECTKEY_MASK)) |
                            CY_SYS_AIRCR_VECTKEY | CY_SYS_AIRCR_SYSRESETREQ;
}


/*******************************************************************************
* Function Name: CyDelay
****************************************************************************//**
*
*  Blocks for milliseconds.
*
*  \param milliseconds: number of milliseconds to delay.
*
*******************************************************************************/
void CyDelay(uint32 milliseconds)
{
    while (milliseconds > CY_DELAY_MS_OVERFLOW)
    {
        /* This loop prevents overflow.
         * At 100MHz, milliseconds * cydelayFreqKhz overflows at about 42 seconds
         */
        CyDelayCycles(cydelay32kMs);
        milliseconds -= CY_DELAY_MS_OVERFLOW;
    }

    CyDelayCycles(milliseconds * cydelayFreqKhz);
}


/*******************************************************************************
* Function Name: CyDelayUs
****************************************************************************//**
*  Blocks for microseconds.
*
*  \param microseconds: number of microseconds to delay.
*
*******************************************************************************/
void CyDelayUs(uint16 microseconds)
{
    CyDelayCycles((uint32)microseconds * cydelayFreqMhz);
}


/*******************************************************************************
* Function Name: CyDelayFreq
****************************************************************************//**
*  Sets clock frequency for CyDelay.
*
*  \param freq: Frequency of bus clock in Hertz.
*
*******************************************************************************/
void CyDelayFreq(uint32 freq)
{
    if (freq != 0u)
    {
        cydelayFreqHz = freq;
    }
    else
    {
        cydelayFreqHz = CYDEV_BCLK__SYSCLK__HZ;
    }

    cydelayFreqMhz = (uint8)((cydelayFreqHz + CY_DELAY_1M_MINUS_1_THRESHOLD) / CY_DELAY_1M_THRESHOLD);
    cydelayFreqKhz = (cydelayFreqHz + CY_DELAY_1K_MINUS_1_THRESHOLD) / CY_DELAY_1K_THRESHOLD;
    cydelay32kMs   = CY_DELAY_MS_OVERFLOW * cydelayFreqKhz;
}


/*******************************************************************************
* Function Name: CySysTick_Start
****************************************************************************//**
*
*  Starts the system timer (SysTick): configures SysTick to generate interrupt
*  every 1 ms and enables the interrupt.
*
*  There are components (LIN, CapSense Gesture) that relies on the default 
*  interval (1 ms). And that changing the interval will negatively impact 
*  their functionality.
*
* \sideeffect
*  Clears SysTick count flag if it was set.
*
*******************************************************************************/
void CySysTickStart(void)
{
    if (0u == CySysTickInitVar)
    {
        CySysTickInit();
        CySysTickInitVar = 1u;
    }

    CySysTickEnable();
}


/*******************************************************************************
* Function Name: CySysTickInit
****************************************************************************//**
*
*  Initializes the callback addresses with pointers to NULL, associates the
*  SysTick system vector with the function that is responsible for calling
*  registered callback functions, configures SysTick timer to generate interrupt
*  every 1 ms.
*
*  The 1 ms interrupt interval is configured based on the frequency determined
*  by PSoC Creator at build time. If System clock frequency is changed in
*  runtime, the CyDelayFreq() with the appropriate parameter should be called.
*
* \sideeffect
*  Clears SysTick count flag if it was set.
*
*******************************************************************************/
void CySysTickInit(void)
{
    uint32 i;

    for (i = 0u; i<CY_SYS_SYST_NUM_OF_CALLBACKS; i++)
    {
        CySysTickCallbacks[i] = (void *) 0;
    }

    (void) CyIntSetSysVector(CY_INT_SYSTICK_IRQN, &CySysTickServiceCallbacks);

    #if(CY_SYSTICK_LFCLK_SOURCE)
        CySysTickSetClockSource(CY_SYS_SYST_CSR_CLK_SRC_SYSCLK);
    #endif /* (CY_SYSTICK_LFCLK_SOURCE) */

    CySysTickSetReload(cydelayFreqHz/1000u);
    CySysTickClear();
}


/*******************************************************************************
* Function Name: CySysTickEnable
****************************************************************************//**
*
*  Enables the SysTick timer and its interrupt.
*
* \sideeffect
*  Clears SysTick count flag if it was set
*
*******************************************************************************/
void CySysTickEnable(void)
{
    CySysTickEnableInterrupt();
    CY_SYS_SYST_CSR_REG |= CY_SYS_SYST_CSR_ENABLE;
}


/*******************************************************************************
* Function Name: CySysTickStop
****************************************************************************//**
*
*  Stops the system timer (SysTick).
*
* \sideeffect
*  Clears SysTick count flag if it was set
*
*******************************************************************************/
void CySysTickStop(void)
{
    CY_SYS_SYST_CSR_REG &= ((uint32) ~(CY_SYS_SYST_CSR_ENABLE));
}


/*******************************************************************************
* Function Name: CySysTickEnableInterrupt
****************************************************************************//**
*
*  Enables the SysTick interrupt.
*
* \sideeffect
*  Clears SysTick count flag if it was set
*
*******************************************************************************/
void CySysTickEnableInterrupt(void)
{
    CY_SYS_SYST_CSR_REG |= CY_SYS_SYST_CSR_ENABLE_INT;
}


/*******************************************************************************
* Function Name: CySysTickDisableInterrupt
****************************************************************************//**
*
*  Disables the SysTick interrupt.
*
* \sideeffect
*  Clears SysTick count flag if it was set
*
*******************************************************************************/
void CySysTickDisableInterrupt(void)
{
    CY_SYS_SYST_CSR_REG &= ((uint32) ~(CY_SYS_SYST_CSR_ENABLE_INT));
}


/*******************************************************************************
* Function Name: CySysTickSetReload
****************************************************************************//**
*
*  Sets value the counter is set to on startup and after it reaches zero. This
*  function do not change or reset current sysTick counter value, so it should
*  be cleared using CySysTickClear() API.
*
*  \param value: Valid range [0x0-0x00FFFFFF]. Counter reset value
*
*******************************************************************************/
void CySysTickSetReload(uint32 value)
{
    CY_SYS_SYST_RVR_REG = (value & CY_SYS_SYST_RVR_CNT_MASK);
}


/*******************************************************************************
* Function Name: CySysTickGetReload
****************************************************************************//**
*
*  Gets value the counter is set to on startup and after it reaches zero.
*
* \return
*  Counter reset value.
*
*******************************************************************************/
uint32 CySysTickGetReload(void)
{
    return(CY_SYS_SYST_RVR_REG & CY_SYS_SYST_RVR_CNT_MASK);
}


/*******************************************************************************
* Function Name: CySysTickGetValue
****************************************************************************//**
*
*  Gets current SysTick counter value.
*
* \return
*  Current SysTick counter value
*
*******************************************************************************/
uint32 CySysTickGetValue(void)
{
    return(CY_SYS_SYST_CVR_REG & CY_SYS_SYST_CVR_CNT_MASK);
}


#if(CY_SYSTICK_LFCLK_SOURCE)
    /*******************************************************************************
    * Function Name: CySysTickSetClockSource
    ****************************************************************************//**
    *
    *  Sets the clock source for the SysTick counter.
    *
    *  Clears SysTick count flag if it was set. If clock source is not ready this
    *  function call will have no effect. After changing clock source to the low
    *  frequency clock the counter and reload register values will remain unchanged
    *  so time to the interrupt will be significantly longer and vice versa.
    *
    *  The changing of SysTick clock source and\or its frequency will change 
    *  the interrupt interval and that CySysTickSetReload() should be 
    *  called to compensate this change.    
    *
    *  The function is not available on PSoC 4000, PSoC 4100, and PSoC 42000
    *  devices. The SysTick timer clocked by the System clock on these devices.
    *
    *  \param clockSource Clock source for SysTick counter
    *         Define                     Clock Source
    *   CY_SYS_SYST_CSR_CLK_SRC_SYSCLK     SysTick is clocked by the CPU clock.
    *   CY_SYS_SYST_CSR_CLK_SRC_LFCLK      SysTick is clocked by the low frequency
    *                                      clock. (ILO 100 KHz for PSoC 5LP, and
    *                                      LFCLK for PSoC 4).
    *******************************************************************************/
    void CySysTickSetClockSource(uint32 clockSource)
    {
        if (clockSource == CY_SYS_SYST_CSR_CLK_SRC_SYSCLK)
        {
            CY_SYS_SYST_CSR_REG |= (uint32)(CY_SYS_SYST_CSR_CLK_SRC_SYSCLK << CY_SYS_SYST_CSR_CLK_SOURCE_SHIFT);
        }
        else
        {
            CY_SYS_SYST_CSR_REG &= ((uint32) ~((uint32)(CY_SYS_SYST_CSR_CLK_SRC_SYSCLK << CY_SYS_SYST_CSR_CLK_SOURCE_SHIFT)));
        }
    }

    
    /*******************************************************************************
    * Function Name: CySysTickGetClockSource
    ****************************************************************************//**
    *
    *  Returns the current clock source of the SysTick counter.
    *
    *  \return 
    *   CY_SYS_SYST_CSR_CLK_SRC_SYSCLK     SysTick is clocked by CPU clock.
    *   CY_SYS_SYST_CSR_CLK_SRC_LFCLK      SysTick is clocked by the low frequency
    *                                      clock. (ILO 100 KHz for PSoC 5LP, and
    *                                      LFCLK for PSoC 4).
    *******************************************************************************/
    uint32 CySysTickGetClockSource(void)
    {
        return ((CY_SYS_SYST_CSR_REG >> CY_SYS_SYST_CSR_CLK_SOURCE_SHIFT) & CY_SYS_SYST_CSR_CLK_SRC_SYSCLK );
    }
    
#endif /* (CY_SYSTICK_LFCLK_SOURCE) */


/*******************************************************************************
* Function Name: CySysTickGetCountFlag
****************************************************************************//**
*
*  The count flag is set once SysTick counter reaches zero.
*  The flag cleared on read.
*
* \return
    *  Returns non-zero value if flag is set, otherwise zero is returned.
*
*
* \sideeffect
*  Clears SysTick count flag if it was set.
*
*******************************************************************************/
uint32 CySysTickGetCountFlag(void)
{
    return ((CY_SYS_SYST_CSR_REG >> CY_SYS_SYST_CSR_COUNTFLAG_SHIFT) & 0x01u);
}


/*******************************************************************************
* Function Name: CySysTickClear
****************************************************************************//**
*
*  Clears the SysTick counter for well-defined startup.
*
*******************************************************************************/
void CySysTickClear(void)
{
    CY_SYS_SYST_CVR_REG = 0u;
}


/*******************************************************************************
* Function Name: CySysTickSetCallback
****************************************************************************//**
*
*  This function allows up to five user-defined interrupt service routine
*  functions to be associated with the SysTick interrupt. These are specified
*  through the use of pointers to the function.
*
*  To set a custom callback function without the overhead of the system provided
*  one, use CyIntSetSysVector(CY_INT_SYSTICK_IRQN, cyisraddress <address>),
*  where <address> is address of the custom defined interrupt service routine.
*  Note: a custom callback function overrides the system defined callback
*  functions.
*
*  \param number: The number of the callback function addresses to be set. The valid
*          range is from 0 to 4.
*
*  void(*CallbackFunction(void): A pointer to the function that will be
*                                associated with the SysTick ISR for the
*                                specified number.
*
* \return
*  Returns the address of the previous callback function.
*  The NULL is returned if the specified address in not set.
*
* \sideeffect
*  The registered callback functions will be executed in the interrupt.
*
*******************************************************************************/
cySysTickCallback CySysTickSetCallback(uint32 number, cySysTickCallback function)
{
    cySysTickCallback retVal;

    retVal = CySysTickCallbacks[number];
    CySysTickCallbacks[number] = function;
    return (retVal);
}


/*******************************************************************************
* Function Name: CySysTickGetCallback
****************************************************************************//**
*
*  The function get the specified callback pointer.
*
*  \param number: The number of callback function address to get. The valid
*          range is from 0 to 4.
*
* \return
*  Returns the address of the specified callback function.
*  The NULL is returned if the specified address in not initialized.
*
*******************************************************************************/
cySysTickCallback CySysTickGetCallback(uint32 number)
{
    return ((cySysTickCallback) CySysTickCallbacks[number]);
}


/*******************************************************************************
* Function Name: CySysTickServiceCallbacks
****************************************************************************//**
*
*  System Tick timer interrupt routine
*
*******************************************************************************/
static void CySysTickServiceCallbacks(void)
{
    uint32 i;

    /* Verify that tick timer flag was set */
    if (1u == CySysTickGetCountFlag())
    {
        for (i=0u; i < CY_SYS_SYST_NUM_OF_CALLBACKS; i++)
        {
            if (CySysTickCallbacks[i] != (void *) 0)
            {
                (void)(CySysTickCallbacks[i])();
            }
        }
    }
}


/*******************************************************************************
* Function Name: CyGetUniqueId
****************************************************************************//**
*
*  Returns the 64-bit unique ID of the device. The uniqueness of the number is
*  guaranteed for 10 years due to the die lot number having a cycle life of 10
*  years and even after 10 years, the probability of getting two identical
*  numbers is very small.
*
*  \param uniqueId: The pointer to a two element 32-bit unsigned integer array. Returns
*  the 64-bit unique ID of the device by loading them into the integer array
*  pointed to by uniqueId.
*
*******************************************************************************/
void CyGetUniqueId(uint32* uniqueId)
{
#if(CY_PSOC4)
    uniqueId[0u]  =  (uint32)(* (reg8 *) CYREG_SFLASH_DIE_LOT0  );
    uniqueId[0u] |= ((uint32)(* (reg8 *) CYREG_SFLASH_DIE_LOT1  ) <<  8u);
    uniqueId[0u] |= ((uint32)(* (reg8 *) CYREG_SFLASH_DIE_LOT2  ) << 16u);
    uniqueId[0u] |= ((uint32)(* (reg8 *) CYREG_SFLASH_DIE_WAFER ) << 24u);

    uniqueId[1u]  =  (uint32)(* (reg8 *) CYREG_SFLASH_DIE_X     );
    uniqueId[1u] |= ((uint32)(* (reg8 *) CYREG_SFLASH_DIE_Y     ) <<  8u);
    uniqueId[1u] |= ((uint32)(* (reg8 *) CYREG_SFLASH_DIE_SORT  ) << 16u);
    uniqueId[1u] |= ((uint32)(* (reg8 *) CYREG_SFLASH_DIE_MINOR ) << 24u);
#else
    uniqueId[0u]  =  (uint32) CY_GET_XTND_REG8((void CYFAR *) (CYREG_FLSHID_CUST_TABLES_LOT_LSB   ));
    uniqueId[0u] |= ((uint32) CY_GET_XTND_REG8((void CYFAR *) (CYREG_FLSHID_CUST_TABLES_LOT_MSB   )) <<  8u);
    uniqueId[0u] |= ((uint32) CY_GET_XTND_REG8((void CYFAR *) (CYREG_MLOGIC_REV_ID                )) << 16u);
    uniqueId[0u] |= ((uint32) CY_GET_XTND_REG8((void CYFAR *) (CYREG_FLSHID_CUST_TABLES_WAFER_NUM )) << 24u);

    uniqueId[1u]  =  (uint32) CY_GET_XTND_REG8((void CYFAR *) (CYREG_FLSHID_CUST_TABLES_X_LOC     ));
    uniqueId[1u] |= ((uint32) CY_GET_XTND_REG8((void CYFAR *) (CYREG_FLSHID_CUST_TABLES_Y_LOC     )) <<  8u);
    uniqueId[1u] |= ((uint32) CY_GET_XTND_REG8((void CYFAR *) (CYREG_FLSHID_CUST_TABLES_WRK_WK    )) << 16u);
    uniqueId[1u] |= ((uint32) CY_GET_XTND_REG8((void CYFAR *) (CYREG_FLSHID_CUST_TABLES_FAB_YR    )) << 24u);
#endif  /* (CY_PSOC4) */
}


#if (CY_IP_DMAC_PRESENT)
    /*******************************************************************************
    * Function Name: CySysSetRamAccessArbPriority
    ****************************************************************************//**
    *
    *  Sets RAM access priority between CPU and DMA. The RAM_CTL register is
    *  configured to set the priority. Please refer to the device TRM for more
    *  details.
    *
    *  This API is applicable for PSoC 4200M / PSoC 4200L / 4100S with 
    *  DMA devices only.
    *
    *  \param source:
    *   CY_SYS_ARB_PRIORITY_CPU              CPU has priority (Default)
    *   CY_SYS_ARB_PRIORITY_DMA              DMA has priority
    *   CY_SYS_ARB_PRIORITY_ROUND            Round robin
    *   CY_SYS_ARB_PRIORITY_ROUND_STICKY     Round robin sticky
    *
    *******************************************************************************/
    void CySysSetRamAccessArbPriority(uint32 source)
    {
        uint32 regTmp;

        regTmp  = CY_SYS_CPUSS_RAM_CTL_REG & ~CY_SYS_CPUSS_RAM_CTL_ARB_MASK;
        regTmp |= ((uint32) (source << CY_SYS_CPUSS_RAM_CTL_ARB_SHIFT) & CY_SYS_CPUSS_RAM_CTL_ARB_MASK);
        CY_SYS_CPUSS_RAM_CTL_REG = regTmp;
    }


    /*******************************************************************************
    * Function Name: CySysSetFlashAccessArbPriority
    ****************************************************************************//**
    *
    *  Sets flash access priority between CPU and DMA. The FLASH_CTL register is
    *  configured to set the priority. Please refer to the device TRM for more
    *  details.
    *
    *  This API is applicable for PSoC 4200M / PSoC 4200L / 4100S with 
    *  DMA devices only.
    *
    *  \param source:
    *   CY_SYS_ARB_PRIORITY_CPU              CPU has priority (Default)
    *   CY_SYS_ARB_PRIORITY_DMA              DMA has priority
    *   CY_SYS_ARB_PRIORITY_ROUND            Round robin
    *   CY_SYS_ARB_PRIORITY_ROUND_STICKY     Round robin sticky
    *
    *******************************************************************************/
    void CySysSetFlashAccessArbPriority(uint32 source)
    {
        uint32 regTmp;

        regTmp  = CY_SYS_CPUSS_FLASH_CTL_REG & ~CY_SYS_CPUSS_FLASH_CTL_ARB_MASK;
        regTmp |= ((uint32) (source << CY_SYS_CPUSS_FLASH_CTL_ARB_SHIFT) & CY_SYS_CPUSS_FLASH_CTL_ARB_MASK);
        CY_SYS_CPUSS_FLASH_CTL_REG = regTmp;
    }


    /*******************************************************************************
    * Function Name: CySysSetDmacAccessArbPriority
    ****************************************************************************//**
    *
    *  Sets DMAC slave interface access priority between CPU and DMA. The DMAC_CTL
    *  register is configured to set the priority. Please refer to the device TRM
    *  for more details.
    *
    *  This API is applicable for PSoC 4200M / PSoC 4200L / 4100S with 
    *  DMA devices only.
    *
    *  \param source:
    *   CY_SYS_ARB_PRIORITY_CPU              CPU has priority (Default)
    *   CY_SYS_ARB_PRIORITY_DMA              DMA has priority
    *   CY_SYS_ARB_PRIORITY_ROUND            Round robin
    *   CY_SYS_ARB_PRIORITY_ROUND_STICKY     Round robin sticky
    *
    *******************************************************************************/
    void CySysSetDmacAccessArbPriority(uint32 source)
    {
        uint32 regTmp;

        regTmp  = CY_SYS_CPUSS_DMAC_CTL_REG & ~CY_SYS_CPUSS_DMAC_CTL_ARB_MASK;
        regTmp |= ((uint32) (source << CY_SYS_CPUSS_DMAC_CTL_ARB_SHIFT) & CY_SYS_CPUSS_DMAC_CTL_ARB_MASK);
        CY_SYS_CPUSS_DMAC_CTL_REG = regTmp;
    }


    /*******************************************************************************
    * Function Name: CySysSetPeripheralAccessArbPriority
    ****************************************************************************//**
    *
    *  Sets slave peripheral interface access priority between CPU and DMA.
    *  The SL_CTL register is configured to set the priority. Please refer to the
    *  device TRM for more details.
    *
    *  This API is applicable for PSoC 4200M / PSoC 4200L / 4100S with 
    *  DMA devices only.
    *
    *  \param  interfaceNumber: the slave interface number. Please refer to the
    *  device TRM for more details.
    *  \param source:
    *   CY_SYS_ARB_PRIORITY_CPU              CPU has priority (Default)
    *   CY_SYS_ARB_PRIORITY_DMA              DMA has priority
    *   CY_SYS_ARB_PRIORITY_ROUND            Round robin
    *   CY_SYS_ARB_PRIORITY_ROUND_STICKY     Round robin sticky
    *
    *******************************************************************************/
    void CySysSetPeripheralAccessArbPriority(uint32 interfaceNumber, uint32 source)
    {
        uint32 regTmp;

        if (interfaceNumber == 0u)
        {
            regTmp  = CY_SYS_CPUSS_SL_CTL0_REG & ~CY_SYS_CPUSS_SL_CTL_ARB_MASK;
            regTmp |= ((uint32) (source << CY_SYS_CPUSS_SL_CTL_ARB_SHIFT) & CY_SYS_CPUSS_SL_CTL_ARB_MASK);
            CY_SYS_CPUSS_SL_CTL0_REG = regTmp;
        } else
    #if (CY_IP_SL_NR >= 2)
        if (interfaceNumber == 1u)
        {
            regTmp  = CY_SYS_CPUSS_SL_CTL1_REG & ~CY_SYS_CPUSS_SL_CTL_ARB_MASK;
            regTmp |= ((uint32) (source << CY_SYS_CPUSS_SL_CTL_ARB_SHIFT) & CY_SYS_CPUSS_SL_CTL_ARB_MASK);
            CY_SYS_CPUSS_SL_CTL1_REG = regTmp;
        } else
    #endif /* (CY_IP_SL_NR >= 1) */
    #if (CY_IP_SL_NR >= 3)
        if (interfaceNumber == 2u)
        {
            regTmp  = CY_SYS_CPUSS_SL_CTL2_REG & ~CY_SYS_CPUSS_SL_CTL_ARB_MASK;
            regTmp |= ((uint32) (source << CY_SYS_CPUSS_SL_CTL_ARB_SHIFT) & CY_SYS_CPUSS_SL_CTL_ARB_MASK);
            CY_SYS_CPUSS_SL_CTL2_REG = regTmp;
        } else
    #endif /* (CY_IP_SL_NR >= 1) */
        {
            /* Halt CPU in debug mode if interface is invalid */
            CYASSERT(0u != 0u);
        }
    }

#endif /* (CY_IP_DMAC_PRESENT) */


#if (CY_IP_PASS)
    /*******************************************************************************
    * Function Name: CySysPrbSetGlobalVrefSource
    ****************************************************************************//**
    *
    *  Selects the source of the global voltage reference.
    *
    *  \note The global voltage reference uses one of the available programmable
    *  voltage reference lines.
    *
    *  This API is applicable for PSoC 4 devices that support the programmable
    *  reference block. Please refer to the device TRM for more details.
    *
    *  This API affects the voltage values available in
    *  \ref CySysPrbSetGlobalVrefVoltage().
    *
    *  This API is applicable for PSoC 4200M, PSoC 4200L and PSoC Analog 
    *  Coprocessor devices only.
    *
    *  \param source:
    *   CY_SYS_VREF_SOURCE_BG   Sets bandgap as the source of the global voltage
    *   reference.
    *   CY_SYS_VREF_SOURCE_VDDA Sets VDDA as the source of the global voltage
    *   reference.
    *
    *******************************************************************************/
    #ifdef CyDesignWideVoltageReference_PRB_REF
        void CySysPrbSetGlobalVrefSource(uint32 source)
        {
            CY_SET_REG32_FIELD(CYREG_PASS_PRB_REF, CYFLD_PASS_VREF_SUP_SEL, source);
        }
    #endif

    /*******************************************************************************
    * Function Name: CySysPrbSetBgGain
    ****************************************************************************//**
    *
    * Selects the gain of bandgap reference buffer. Note that this API is effective
    * only when the bandgap is set as the source of global voltage reference.
    *
    * \note This API affects the voltage values available in \ref
    * CySysPrbSetGlobalVrefVoltage() API.
    *
    * This API is applicable for PSoC 4 devices that support the programmable
    * reference block. Please refer to the device TRM for more details.
    *
    * \param gain:
    * CY_SYS_VREF_BG_GAINx1  Gain is 1.
    * CY_SYS_VREF_BG_GAINx2  Gain is 2.
    *
    *******************************************************************************/
    void CySysPrbSetBgGain(uint32 gain)
    {
        CY_SET_REG32_FIELD(CYREG_PASS_PRB_CTRL, CYFLD_PASS_VBGR_BUF_GAIN, gain);
    }


    /*******************************************************************************
    * Function Name: CySysPrbSetGlobalVrefVoltage
    ****************************************************************************//**
    *
    * Selects the value of global voltage reference. Set the source of the global
    * voltage reference and bandgap buffer gain (if applicable) before calling this
    * API.
    *
    * This API is applicable for PSoC 4 devices that support the programmable
    * reference block. Please refer to the device TRM for more details.
    *
    * \param voltageTap The range is from 1 to 16 that corresponds to:
    * Source is bandgap (x1): 0.08 V to 1.20 V in steps of 0.07 V approximately.
    * Source is bandgap (x2): 0.16 V to 2.40 V in steps of 0.14 V approximately.
    * Source is Vdda: 0.21 V to 3.30 in steps of 0.21 V approximately. The Vdda
    * is equal to 3.3 V. Voltage value will change according to value of Vdda.
    *
    *   voltageTap  | If bandgap (x1), V| If bandgap (x2), V| If Vdda
    *   ------------|-------------------|-------------------|-------------------
    *       1       |       0.08        |       0.16        |       0.21
    *       2       |       0.15        |       0.30        |       0.41
    *       3       |       0.23        |       0.46        |       0.62
    *       4       |       0.30        |       0.60        |       0.83
    *       5       |       0.38        |       0.76        |       1.03
    *       6       |       0.45        |       0.90        |       1.24
    *       7       |       0.53        |       1.06        |       1.44
    *       8       |       0.60        |       1.20        |       1.65
    *       9       |       0.68        |       1.36        |       1.86
    *       10      |       0.75        |       1.50        |       2.06
    *       11      |       0.83        |       1.66        |       2.27
    *       12      |       0.90        |       1.80        |       2.48
    *       13      |       0.98        |       1.96        |       2.68
    *       14      |       1.05        |       2.10        |       2.89
    *       15      |       1.13        |       2.26        |       3.09
    *       16      |       1.20        |       2.40        |       3.30
    *
    *******************************************************************************/
    #ifdef CyDesignWideVoltageReference_PRB_REF
        void CySysPrbSetGlobalVrefVoltage(uint32 voltageTap)
        {
            CY_SET_REG32_FIELD(CYREG_PASS_PRB_REF, CYFLD_PASS_VREF_SEL, voltageTap);
        }
    #endif


    /*******************************************************************************
    * Function Name: CySysPrbEnableDeepsleepVddaRef
    ****************************************************************************//**
    *
    * Enables the Vdda reference in deep sleep mode. The Vdda reference is by
    * default disabled when entering deep sleep mode.
    *
    * This API is applicable for PSoC 4 devices that support the programmable
    * reference block. Please refer to the device TRM for more details.
    *
    *******************************************************************************/
    void CySysPrbEnableDeepsleepVddaRef(void)
    {
        CY_SET_REG32_FIELD(CYREG_PASS_PRB_CTRL, CYFLD_PASS_DEEPSLEEP_ON, 1u);
    }


    /*******************************************************************************
    * Function Name: CySysPrbDisableDeepsleepVddaRef
    ****************************************************************************//**
    *
    * Disables the Vdda reference in deep sleep mode. The Vdda reference is by
    * default disabled when entering deep sleep mode.
    *
    * This API is applicable for PSoC 4 devices that support the programmable
    * reference block. Please refer to the device TRM for more details.
    *
    *******************************************************************************/
    void CySysPrbDisableDeepsleepVddaRef(void)
    {
        CY_CLEAR_REG32_FIELD(CYREG_PASS_PRB_CTRL, CYFLD_PASS_DEEPSLEEP_ON);
    }


    /*******************************************************************************
    * Function Name: CySysPrbEnableVddaRef
    ****************************************************************************//**
    *
    * Enables the Vdda reference. The Vdda reference is by default not enabled.
    *
    * This API is applicable for PSoC 4 devices that support the programmable
    * reference block. Please refer to the device TRM for more details.
    *
    *******************************************************************************/
    void CySysPrbEnableVddaRef(void)
    {
        CY_SET_REG32_FIELD(CYREG_PASS_PRB_CTRL, CYFLD_PASS_VDDA_ENABLE, 1u);
    }


    /*******************************************************************************
    * Function Name: CySysPrbDisableVddaRef
    ****************************************************************************//**
    *
    * Disables the Vdda reference. The Vdda reference is by default not enabled.
    *
    * This API is applicable for PSoC 4 devices that support the programmable
    * reference block. Please refer to the device TRM for more details.
    *
    *******************************************************************************/
    void CySysPrbDisableVddaRef(void)
    {
        CY_CLEAR_REG32_FIELD(CYREG_PASS_PRB_CTRL, CYFLD_PASS_VDDA_ENABLE);
    }


    /*******************************************************************************
    * Function Name: CySysPrbSetBgBufferTrim
    ****************************************************************************//**
    *
    * Sets the trim for the bandgap reference buffer.
    *
    * \note Affects all bandgap sourced references.
    *
    * \param bgTrim The trim value from -32 to 31. Step size is approximately 1 mV.
    *
    * This API is applicable for PSoC 4 devices that support the programmable
    * reference block. Please refer to the device TRM for more details.
    *
    *******************************************************************************/
    void CySysPrbSetBgBufferTrim(int32 bgTrim)
    {
        uint8 tmp = 0u;

        if (bgTrim >= 0)
        {
            CY_SET_REG32_FIELD(CYREG_PASS_PRB_TRIM, CYFLD_PASS_VBGR_BUF_TRIM, (uint32) bgTrim);
        }
        else
        {
            tmp = (uint8)((int32) bgTrim * (int8) (-1));   /* Make positive */
            tmp = (uint8) ~tmp + 1u;                      /* Two's complement */
            tmp |= (uint8) CY_SYS_VREF_BG_BUFFER_TRIM_SIGN_BIT;

            CY_SET_REG32_FIELD(CYREG_PASS_PRB_TRIM, CYFLD_PASS_VBGR_BUF_TRIM, tmp);
        }
    }


    /*******************************************************************************
    * Function Name: CySysPrbGetBgBufferTrim
    ****************************************************************************//**
    *
    * Returns the current trim of the bandgap reference buffer.
    *
    * This API is applicable for PSoC 4 devices that support the programmable
    * reference block. Please refer to the device TRM for more details.
    *
    *******************************************************************************/
    int32 CySysPrbGetBgBufferTrim(void)
    {
        uint8 tmp;
        int32 returnValue;

        tmp = (uint8) CY_GET_REG32_FIELD(CYREG_PASS_PRB_TRIM, CYFLD_PASS_VBGR_BUF_TRIM);
        if ((tmp & CY_SYS_VREF_BG_BUFFER_TRIM_SIGN_BIT) != 0u)
        {
            tmp = ((uint8) ~tmp) + 1u;          /* Make positive */
            returnValue = (int32) tmp * (-1);   /* Make negative */
        }
        else
        {
            returnValue = (int32) tmp;
        }

        return (returnValue);
    }
    
    
#endif /* (CY_IP_PASS) */


/* [] END OF FILE */
