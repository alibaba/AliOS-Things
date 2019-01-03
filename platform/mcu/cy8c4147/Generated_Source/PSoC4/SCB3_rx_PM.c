/*******************************************************************************
* File Name: SCB3_rx.c  
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
#include "SCB3_rx.h"

static SCB3_rx_BACKUP_STRUCT  SCB3_rx_backup = {0u, 0u, 0u};


/*******************************************************************************
* Function Name: SCB3_rx_Sleep
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
*  \snippet SCB3_rx_SUT.c usage_SCB3_rx_Sleep_Wakeup
*******************************************************************************/
void SCB3_rx_Sleep(void)
{
    #if defined(SCB3_rx__PC)
        SCB3_rx_backup.pcState = SCB3_rx_PC;
    #else
        #if (CY_PSOC4_4200L)
            /* Save the regulator state and put the PHY into suspend mode */
            SCB3_rx_backup.usbState = SCB3_rx_CR1_REG;
            SCB3_rx_USB_POWER_REG |= SCB3_rx_USBIO_ENTER_SLEEP;
            SCB3_rx_CR1_REG &= SCB3_rx_USBIO_CR1_OFF;
        #endif
    #endif
    #if defined(CYIPBLOCK_m0s8ioss_VERSION) && defined(SCB3_rx__SIO)
        SCB3_rx_backup.sioState = SCB3_rx_SIO_REG;
        /* SIO requires unregulated output buffer and single ended input buffer */
        SCB3_rx_SIO_REG &= (uint32)(~SCB3_rx_SIO_LPM_MASK);
    #endif  
}


/*******************************************************************************
* Function Name: SCB3_rx_Wakeup
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
*  Refer to SCB3_rx_Sleep() for an example usage.
*******************************************************************************/
void SCB3_rx_Wakeup(void)
{
    #if defined(SCB3_rx__PC)
        SCB3_rx_PC = SCB3_rx_backup.pcState;
    #else
        #if (CY_PSOC4_4200L)
            /* Restore the regulator state and come out of suspend mode */
            SCB3_rx_USB_POWER_REG &= SCB3_rx_USBIO_EXIT_SLEEP_PH1;
            SCB3_rx_CR1_REG = SCB3_rx_backup.usbState;
            SCB3_rx_USB_POWER_REG &= SCB3_rx_USBIO_EXIT_SLEEP_PH2;
        #endif
    #endif
    #if defined(CYIPBLOCK_m0s8ioss_VERSION) && defined(SCB3_rx__SIO)
        SCB3_rx_SIO_REG = SCB3_rx_backup.sioState;
    #endif
}


/* [] END OF FILE */
