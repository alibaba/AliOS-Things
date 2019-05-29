/*******************************************************************************
* File Name: pin_wakeup.c  
* Version 2.20
*
* Description:
*  This file contains APIs to set up the Pins component for low power modes.
*
* Note:
*
********************************************************************************
* Copyright 2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "cytypes.h"
#include "pin_wakeup.h"

static pin_wakeup_BACKUP_STRUCT  pin_wakeup_backup = {0u, 0u, 0u};


/*******************************************************************************
* Function Name: pin_wakeup_Sleep
****************************************************************************//**
*
* \brief Stores the pin configuration and prepares the pin for entering chip 
*  deep-sleep/hibernate modes. This function applies only to SIO and USBIO pins.
*  It should not be called for GPIO or GPIO_OVT pins.
*
* <b>Note</b> This function is available in PSoC 4 only.
*
* \return 
*  None 
*  
* \sideeffect
*  For SIO pins, this function configures the pin input threshold to CMOS and
*  drive level to Vddio. This is needed for SIO pins when in device 
*  deep-sleep/hibernate modes.
*
* \funcusage
*  \snippet pin_wakeup_SUT.c usage_pin_wakeup_Sleep_Wakeup
*******************************************************************************/
void pin_wakeup_Sleep(void)
{
    #if defined(pin_wakeup__PC)
        pin_wakeup_backup.pcState = pin_wakeup_PC;
    #else
        #if (CY_PSOC4_4200L)
            /* Save the regulator state and put the PHY into suspend mode */
            pin_wakeup_backup.usbState = pin_wakeup_CR1_REG;
            pin_wakeup_USB_POWER_REG |= pin_wakeup_USBIO_ENTER_SLEEP;
            pin_wakeup_CR1_REG &= pin_wakeup_USBIO_CR1_OFF;
        #endif
    #endif
    #if defined(CYIPBLOCK_m0s8ioss_VERSION) && defined(pin_wakeup__SIO)
        pin_wakeup_backup.sioState = pin_wakeup_SIO_REG;
        /* SIO requires unregulated output buffer and single ended input buffer */
        pin_wakeup_SIO_REG &= (uint32)(~pin_wakeup_SIO_LPM_MASK);
    #endif  
}


/*******************************************************************************
* Function Name: pin_wakeup_Wakeup
****************************************************************************//**
*
* \brief Restores the pin configuration that was saved during Pin_Sleep(). This 
* function applies only to SIO and USBIO pins. It should not be called for
* GPIO or GPIO_OVT pins.
*
* For USBIO pins, the wakeup is only triggered for falling edge interrupts.
*
* <b>Note</b> This function is available in PSoC 4 only.
*
* \return 
*  None
*  
* \funcusage
*  Refer to pin_wakeup_Sleep() for an example usage.
*******************************************************************************/
void pin_wakeup_Wakeup(void)
{
    #if defined(pin_wakeup__PC)
        pin_wakeup_PC = pin_wakeup_backup.pcState;
    #else
        #if (CY_PSOC4_4200L)
            /* Restore the regulator state and come out of suspend mode */
            pin_wakeup_USB_POWER_REG &= pin_wakeup_USBIO_EXIT_SLEEP_PH1;
            pin_wakeup_CR1_REG = pin_wakeup_backup.usbState;
            pin_wakeup_USB_POWER_REG &= pin_wakeup_USBIO_EXIT_SLEEP_PH2;
        #endif
    #endif
    #if defined(CYIPBLOCK_m0s8ioss_VERSION) && defined(pin_wakeup__SIO)
        pin_wakeup_SIO_REG = pin_wakeup_backup.sioState;
    #endif
}


/* [] END OF FILE */
