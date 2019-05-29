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

#if !defined(CY_SCB_PVT_SPI_1_H)
#define CY_SCB_PVT_SPI_1_H

#include "SPI_1.h"


/***************************************
*     Private Function Prototypes
***************************************/

/* APIs to service INTR_I2C_EC register */
#define SPI_1_SetI2CExtClkInterruptMode(interruptMask) SPI_1_WRITE_INTR_I2C_EC_MASK(interruptMask)
#define SPI_1_ClearI2CExtClkInterruptSource(interruptMask) SPI_1_CLEAR_INTR_I2C_EC(interruptMask)
#define SPI_1_GetI2CExtClkInterruptSource()                (SPI_1_INTR_I2C_EC_REG)
#define SPI_1_GetI2CExtClkInterruptMode()                  (SPI_1_INTR_I2C_EC_MASK_REG)
#define SPI_1_GetI2CExtClkInterruptSourceMasked()          (SPI_1_INTR_I2C_EC_MASKED_REG)

#if (!SPI_1_CY_SCBIP_V1)
    /* APIs to service INTR_SPI_EC register */
    #define SPI_1_SetSpiExtClkInterruptMode(interruptMask) \
                                                                SPI_1_WRITE_INTR_SPI_EC_MASK(interruptMask)
    #define SPI_1_ClearSpiExtClkInterruptSource(interruptMask) \
                                                                SPI_1_CLEAR_INTR_SPI_EC(interruptMask)
    #define SPI_1_GetExtSpiClkInterruptSource()                 (SPI_1_INTR_SPI_EC_REG)
    #define SPI_1_GetExtSpiClkInterruptMode()                   (SPI_1_INTR_SPI_EC_MASK_REG)
    #define SPI_1_GetExtSpiClkInterruptSourceMasked()           (SPI_1_INTR_SPI_EC_MASKED_REG)
#endif /* (!SPI_1_CY_SCBIP_V1) */

#if(SPI_1_SCB_MODE_UNCONFIG_CONST_CFG)
    extern void SPI_1_SetPins(uint32 mode, uint32 subMode, uint32 uartEnableMask);
#endif /* (SPI_1_SCB_MODE_UNCONFIG_CONST_CFG) */


/***************************************
*     Vars with External Linkage
***************************************/

#if (SPI_1_SCB_IRQ_INTERNAL)
#if !defined (CY_REMOVE_SPI_1_CUSTOM_INTR_HANDLER)
    extern cyisraddress SPI_1_customIntrHandler;
#endif /* !defined (CY_REMOVE_SPI_1_CUSTOM_INTR_HANDLER) */
#endif /* (SPI_1_SCB_IRQ_INTERNAL) */

extern SPI_1_BACKUP_STRUCT SPI_1_backup;

#if(SPI_1_SCB_MODE_UNCONFIG_CONST_CFG)
    /* Common configuration variables */
    extern uint8 SPI_1_scbMode;
    extern uint8 SPI_1_scbEnableWake;
    extern uint8 SPI_1_scbEnableIntr;

    /* I2C configuration variables */
    extern uint8 SPI_1_mode;
    extern uint8 SPI_1_acceptAddr;

    /* SPI/UART configuration variables */
    extern volatile uint8 * SPI_1_rxBuffer;
    extern uint8   SPI_1_rxDataBits;
    extern uint32  SPI_1_rxBufferSize;

    extern volatile uint8 * SPI_1_txBuffer;
    extern uint8   SPI_1_txDataBits;
    extern uint32  SPI_1_txBufferSize;

    /* EZI2C configuration variables */
    extern uint8 SPI_1_numberOfAddr;
    extern uint8 SPI_1_subAddrSize;
#endif /* (SPI_1_SCB_MODE_UNCONFIG_CONST_CFG) */

#if (! (SPI_1_SCB_MODE_I2C_CONST_CFG || \
        SPI_1_SCB_MODE_EZI2C_CONST_CFG))
    extern uint16 SPI_1_IntrTxMask;
#endif /* (! (SPI_1_SCB_MODE_I2C_CONST_CFG || \
              SPI_1_SCB_MODE_EZI2C_CONST_CFG)) */


/***************************************
*        Conditional Macro
****************************************/

#if(SPI_1_SCB_MODE_UNCONFIG_CONST_CFG)
    /* Defines run time operation mode */
    #define SPI_1_SCB_MODE_I2C_RUNTM_CFG     (SPI_1_SCB_MODE_I2C      == SPI_1_scbMode)
    #define SPI_1_SCB_MODE_SPI_RUNTM_CFG     (SPI_1_SCB_MODE_SPI      == SPI_1_scbMode)
    #define SPI_1_SCB_MODE_UART_RUNTM_CFG    (SPI_1_SCB_MODE_UART     == SPI_1_scbMode)
    #define SPI_1_SCB_MODE_EZI2C_RUNTM_CFG   (SPI_1_SCB_MODE_EZI2C    == SPI_1_scbMode)
    #define SPI_1_SCB_MODE_UNCONFIG_RUNTM_CFG \
                                                        (SPI_1_SCB_MODE_UNCONFIG == SPI_1_scbMode)

    /* Defines wakeup enable */
    #define SPI_1_SCB_WAKE_ENABLE_CHECK       (0u != SPI_1_scbEnableWake)
#endif /* (SPI_1_SCB_MODE_UNCONFIG_CONST_CFG) */

/* Defines maximum number of SCB pins */
#if (!SPI_1_CY_SCBIP_V1)
    #define SPI_1_SCB_PINS_NUMBER    (7u)
#else
    #define SPI_1_SCB_PINS_NUMBER    (2u)
#endif /* (!SPI_1_CY_SCBIP_V1) */

#endif /* (CY_SCB_PVT_SPI_1_H) */


/* [] END OF FILE */
