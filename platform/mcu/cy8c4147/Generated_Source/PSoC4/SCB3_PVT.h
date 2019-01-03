/***************************************************************************//**
* \file .h
* \version 4.0
*
* \brief
*  This private file provides constants and parameter values for the
*  SCB Component.
*  Please do not use this file or its content in your project.
*
* Note:
*
********************************************************************************
* \copyright
* Copyright 2013-2017, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_SCB_PVT_SCB3_H)
#define CY_SCB_PVT_SCB3_H

#include "SCB3.h"


/***************************************
*     Private Function Prototypes
***************************************/

/* APIs to service INTR_I2C_EC register */
#define SCB3_SetI2CExtClkInterruptMode(interruptMask) SCB3_WRITE_INTR_I2C_EC_MASK(interruptMask)
#define SCB3_ClearI2CExtClkInterruptSource(interruptMask) SCB3_CLEAR_INTR_I2C_EC(interruptMask)
#define SCB3_GetI2CExtClkInterruptSource()                (SCB3_INTR_I2C_EC_REG)
#define SCB3_GetI2CExtClkInterruptMode()                  (SCB3_INTR_I2C_EC_MASK_REG)
#define SCB3_GetI2CExtClkInterruptSourceMasked()          (SCB3_INTR_I2C_EC_MASKED_REG)

#if (!SCB3_CY_SCBIP_V1)
    /* APIs to service INTR_SPI_EC register */
    #define SCB3_SetSpiExtClkInterruptMode(interruptMask) \
                                                                SCB3_WRITE_INTR_SPI_EC_MASK(interruptMask)
    #define SCB3_ClearSpiExtClkInterruptSource(interruptMask) \
                                                                SCB3_CLEAR_INTR_SPI_EC(interruptMask)
    #define SCB3_GetExtSpiClkInterruptSource()                 (SCB3_INTR_SPI_EC_REG)
    #define SCB3_GetExtSpiClkInterruptMode()                   (SCB3_INTR_SPI_EC_MASK_REG)
    #define SCB3_GetExtSpiClkInterruptSourceMasked()           (SCB3_INTR_SPI_EC_MASKED_REG)
#endif /* (!SCB3_CY_SCBIP_V1) */

#if(SCB3_SCB_MODE_UNCONFIG_CONST_CFG)
    extern void SCB3_SetPins(uint32 mode, uint32 subMode, uint32 uartEnableMask);
#endif /* (SCB3_SCB_MODE_UNCONFIG_CONST_CFG) */


/***************************************
*     Vars with External Linkage
***************************************/

#if (SCB3_SCB_IRQ_INTERNAL)
#if !defined (CY_REMOVE_SCB3_CUSTOM_INTR_HANDLER)
    extern cyisraddress SCB3_customIntrHandler;
#endif /* !defined (CY_REMOVE_SCB3_CUSTOM_INTR_HANDLER) */
#endif /* (SCB3_SCB_IRQ_INTERNAL) */

extern SCB3_BACKUP_STRUCT SCB3_backup;

#if(SCB3_SCB_MODE_UNCONFIG_CONST_CFG)
    /* Common configuration variables */
    extern uint8 SCB3_scbMode;
    extern uint8 SCB3_scbEnableWake;
    extern uint8 SCB3_scbEnableIntr;

    /* I2C configuration variables */
    extern uint8 SCB3_mode;
    extern uint8 SCB3_acceptAddr;

    /* SPI/UART configuration variables */
    extern volatile uint8 * SCB3_rxBuffer;
    extern uint8   SCB3_rxDataBits;
    extern uint32  SCB3_rxBufferSize;

    extern volatile uint8 * SCB3_txBuffer;
    extern uint8   SCB3_txDataBits;
    extern uint32  SCB3_txBufferSize;

    /* EZI2C configuration variables */
    extern uint8 SCB3_numberOfAddr;
    extern uint8 SCB3_subAddrSize;
#endif /* (SCB3_SCB_MODE_UNCONFIG_CONST_CFG) */

#if (! (SCB3_SCB_MODE_I2C_CONST_CFG || \
        SCB3_SCB_MODE_EZI2C_CONST_CFG))
    extern uint16 SCB3_IntrTxMask;
#endif /* (! (SCB3_SCB_MODE_I2C_CONST_CFG || \
              SCB3_SCB_MODE_EZI2C_CONST_CFG)) */


/***************************************
*        Conditional Macro
****************************************/

#if(SCB3_SCB_MODE_UNCONFIG_CONST_CFG)
    /* Defines run time operation mode */
    #define SCB3_SCB_MODE_I2C_RUNTM_CFG     (SCB3_SCB_MODE_I2C      == SCB3_scbMode)
    #define SCB3_SCB_MODE_SPI_RUNTM_CFG     (SCB3_SCB_MODE_SPI      == SCB3_scbMode)
    #define SCB3_SCB_MODE_UART_RUNTM_CFG    (SCB3_SCB_MODE_UART     == SCB3_scbMode)
    #define SCB3_SCB_MODE_EZI2C_RUNTM_CFG   (SCB3_SCB_MODE_EZI2C    == SCB3_scbMode)
    #define SCB3_SCB_MODE_UNCONFIG_RUNTM_CFG \
                                                        (SCB3_SCB_MODE_UNCONFIG == SCB3_scbMode)

    /* Defines wakeup enable */
    #define SCB3_SCB_WAKE_ENABLE_CHECK       (0u != SCB3_scbEnableWake)
#endif /* (SCB3_SCB_MODE_UNCONFIG_CONST_CFG) */

/* Defines maximum number of SCB pins */
#if (!SCB3_CY_SCBIP_V1)
    #define SCB3_SCB_PINS_NUMBER    (7u)
#else
    #define SCB3_SCB_PINS_NUMBER    (2u)
#endif /* (!SCB3_CY_SCBIP_V1) */

#endif /* (CY_SCB_PVT_SCB3_H) */


/* [] END OF FILE */
