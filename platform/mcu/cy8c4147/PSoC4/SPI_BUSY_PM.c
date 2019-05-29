/*******************************************************************************
* File Name: SPI_BUSY.c  
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
#include "SPI_BUSY.h"

static SPI_BUSY_BACKUP_STRUCT  SPI_BUSY_backup = {0u, 0u, 0u};


/*******************************************************************************
* Function Name: SPI_BUSY_Sleep
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
*  \snippet SPI_BUSY_SUT.c usage_SPI_BUSY_Sleep_Wakeup
*******************************************************************************/
void SPI_BUSY_Sleep(void)
{
    #if defined(SPI_BUSY__PC)
        SPI_BUSY_backup.pcState = SPI_BUSY_PC;
    #else
        #if (CY_PSOC4_4200L)
            /* Save the regulator state and put the PHY into suspend mode */
            SPI_BUSY_backup.usbState = SPI_BUSY_CR1_REG;
            SPI_BUSY_USB_POWER_REG |= SPI_BUSY_USBIO_ENTER_SLEEP;
            SPI_BUSY_CR1_REG &= SPI_BUSY_USBIO_CR1_OFF;
        #endif
    #endif
    #if defined(CYIPBLOCK_m0s8ioss_VERSION) && defined(SPI_BUSY__SIO)
        SPI_BUSY_backup.sioState = SPI_BUSY_SIO_REG;
        /* SIO requires unregulated output buffer and single ended input buffer */
        SPI_BUSY_SIO_REG &= (uint32)(~SPI_BUSY_SIO_LPM_MASK);
    #endif  
}


/*******************************************************************************
* Function Name: SPI_BUSY_Wakeup
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
*  Refer to SPI_BUSY_Sleep() for an example usage.
*******************************************************************************/
void SPI_BUSY_Wakeup(void)
{
    #if defined(SPI_BUSY__PC)
        SPI_BUSY_PC = SPI_BUSY_backup.pcState;
    #else
        #if (CY_PSOC4_4200L)
            /* Restore the regulator state and come out of suspend mode */
            SPI_BUSY_USB_POWER_REG &= SPI_BUSY_USBIO_EXIT_SLEEP_PH1;
            SPI_BUSY_CR1_REG = SPI_BUSY_backup.usbState;
            SPI_BUSY_USB_POWER_REG &= SPI_BUSY_USBIO_EXIT_SLEEP_PH2;
        #endif
    #endif
    #if defined(CYIPBLOCK_m0s8ioss_VERSION) && defined(SPI_BUSY__SIO)
        SPI_BUSY_SIO_REG = SPI_BUSY_backup.sioState;
    #endif
}


/* [] END OF FILE */
