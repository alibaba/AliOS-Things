/***************************************************************************//**
* \file SCB3_PM.c
* \version 4.0
*
* \brief
*  This file provides the source code to the Power Management support for
*  the SCB Component.
*
* Note:
*
********************************************************************************
* \copyright
* Copyright 2013-2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "SCB3.h"
#include "SCB3_PVT.h"

#if(SCB3_SCB_MODE_I2C_INC)
    #include "SCB3_I2C_PVT.h"
#endif /* (SCB3_SCB_MODE_I2C_INC) */

#if(SCB3_SCB_MODE_EZI2C_INC)
    #include "SCB3_EZI2C_PVT.h"
#endif /* (SCB3_SCB_MODE_EZI2C_INC) */

#if(SCB3_SCB_MODE_SPI_INC || SCB3_SCB_MODE_UART_INC)
    #include "SCB3_SPI_UART_PVT.h"
#endif /* (SCB3_SCB_MODE_SPI_INC || SCB3_SCB_MODE_UART_INC) */


/***************************************
*   Backup Structure declaration
***************************************/

#if(SCB3_SCB_MODE_UNCONFIG_CONST_CFG || \
   (SCB3_SCB_MODE_I2C_CONST_CFG   && (!SCB3_I2C_WAKE_ENABLE_CONST))   || \
   (SCB3_SCB_MODE_EZI2C_CONST_CFG && (!SCB3_EZI2C_WAKE_ENABLE_CONST)) || \
   (SCB3_SCB_MODE_SPI_CONST_CFG   && (!SCB3_SPI_WAKE_ENABLE_CONST))   || \
   (SCB3_SCB_MODE_UART_CONST_CFG  && (!SCB3_UART_WAKE_ENABLE_CONST)))

    SCB3_BACKUP_STRUCT SCB3_backup =
    {
        0u, /* enableState */
    };
#endif


/*******************************************************************************
* Function Name: SCB3_Sleep
****************************************************************************//**
*
*  Prepares the SCB3 component to enter Deep Sleep.
*  The “Enable wakeup from Deep Sleep Mode” selection has an influence on this 
*  function implementation:
*  - Checked: configures the component to be wakeup source from Deep Sleep.
*  - Unchecked: stores the current component state (enabled or disabled) and 
*    disables the component. See SCB_Stop() function for details about component 
*    disabling.
*
*  Call the SCB3_Sleep() function before calling the 
*  CyPmSysDeepSleep() function. 
*  Refer to the PSoC Creator System Reference Guide for more information about 
*  power management functions and Low power section of this document for the 
*  selected mode.
*
*  This function should not be called before entering Sleep.
*
*******************************************************************************/
void SCB3_Sleep(void)
{
#if(SCB3_SCB_MODE_UNCONFIG_CONST_CFG)

    if(SCB3_SCB_WAKE_ENABLE_CHECK)
    {
        if(SCB3_SCB_MODE_I2C_RUNTM_CFG)
        {
            SCB3_I2CSaveConfig();
        }
        else if(SCB3_SCB_MODE_EZI2C_RUNTM_CFG)
        {
            SCB3_EzI2CSaveConfig();
        }
    #if(!SCB3_CY_SCBIP_V1)
        else if(SCB3_SCB_MODE_SPI_RUNTM_CFG)
        {
            SCB3_SpiSaveConfig();
        }
        else if(SCB3_SCB_MODE_UART_RUNTM_CFG)
        {
            SCB3_UartSaveConfig();
        }
    #endif /* (!SCB3_CY_SCBIP_V1) */
        else
        {
            /* Unknown mode */
        }
    }
    else
    {
        SCB3_backup.enableState = (uint8) SCB3_GET_CTRL_ENABLED;

        if(0u != SCB3_backup.enableState)
        {
            SCB3_Stop();
        }
    }

#else

    #if (SCB3_SCB_MODE_I2C_CONST_CFG && SCB3_I2C_WAKE_ENABLE_CONST)
        SCB3_I2CSaveConfig();

    #elif (SCB3_SCB_MODE_EZI2C_CONST_CFG && SCB3_EZI2C_WAKE_ENABLE_CONST)
        SCB3_EzI2CSaveConfig();

    #elif (SCB3_SCB_MODE_SPI_CONST_CFG && SCB3_SPI_WAKE_ENABLE_CONST)
        SCB3_SpiSaveConfig();

    #elif (SCB3_SCB_MODE_UART_CONST_CFG && SCB3_UART_WAKE_ENABLE_CONST)
        SCB3_UartSaveConfig();

    #else

        SCB3_backup.enableState = (uint8) SCB3_GET_CTRL_ENABLED;

        if(0u != SCB3_backup.enableState)
        {
            SCB3_Stop();
        }

    #endif /* defined (SCB3_SCB_MODE_I2C_CONST_CFG) && (SCB3_I2C_WAKE_ENABLE_CONST) */

#endif /* (SCB3_SCB_MODE_UNCONFIG_CONST_CFG) */
}


/*******************************************************************************
* Function Name: SCB3_Wakeup
****************************************************************************//**
*
*  Prepares the SCB3 component for Active mode operation after 
*  Deep Sleep.
*  The “Enable wakeup from Deep Sleep Mode” selection has influence on this 
*  function implementation:
*  - Checked: restores the component Active mode configuration.
*  - Unchecked: enables the component if it was enabled before enter Deep Sleep.
*
*  This function should not be called after exiting Sleep.
*
*  \sideeffect
*   Calling the SCB3_Wakeup() function without first calling the 
*   SCB3_Sleep() function may produce unexpected behavior.
*
*******************************************************************************/
void SCB3_Wakeup(void)
{
#if(SCB3_SCB_MODE_UNCONFIG_CONST_CFG)

    if(SCB3_SCB_WAKE_ENABLE_CHECK)
    {
        if(SCB3_SCB_MODE_I2C_RUNTM_CFG)
        {
            SCB3_I2CRestoreConfig();
        }
        else if(SCB3_SCB_MODE_EZI2C_RUNTM_CFG)
        {
            SCB3_EzI2CRestoreConfig();
        }
    #if(!SCB3_CY_SCBIP_V1)
        else if(SCB3_SCB_MODE_SPI_RUNTM_CFG)
        {
            SCB3_SpiRestoreConfig();
        }
        else if(SCB3_SCB_MODE_UART_RUNTM_CFG)
        {
            SCB3_UartRestoreConfig();
        }
    #endif /* (!SCB3_CY_SCBIP_V1) */
        else
        {
            /* Unknown mode */
        }
    }
    else
    {
        if(0u != SCB3_backup.enableState)
        {
            SCB3_Enable();
        }
    }

#else

    #if (SCB3_SCB_MODE_I2C_CONST_CFG  && SCB3_I2C_WAKE_ENABLE_CONST)
        SCB3_I2CRestoreConfig();

    #elif (SCB3_SCB_MODE_EZI2C_CONST_CFG && SCB3_EZI2C_WAKE_ENABLE_CONST)
        SCB3_EzI2CRestoreConfig();

    #elif (SCB3_SCB_MODE_SPI_CONST_CFG && SCB3_SPI_WAKE_ENABLE_CONST)
        SCB3_SpiRestoreConfig();

    #elif (SCB3_SCB_MODE_UART_CONST_CFG && SCB3_UART_WAKE_ENABLE_CONST)
        SCB3_UartRestoreConfig();

    #else

        if(0u != SCB3_backup.enableState)
        {
            SCB3_Enable();
        }

    #endif /* (SCB3_I2C_WAKE_ENABLE_CONST) */

#endif /* (SCB3_SCB_MODE_UNCONFIG_CONST_CFG) */
}


/* [] END OF FILE */
