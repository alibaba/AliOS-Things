/***************************************************************************//**
* \file SCB3_SPI_UART_PVT.h
* \version 4.0
*
* \brief
*  This private file provides constants and parameter values for the
*  SCB Component in SPI and UART modes.
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

#if !defined(CY_SCB_SPI_UART_PVT_SCB3_H)
#define CY_SCB_SPI_UART_PVT_SCB3_H

#include "SCB3_SPI_UART.h"


/***************************************
*     Internal Global Vars
***************************************/

#if (SCB3_INTERNAL_RX_SW_BUFFER_CONST)
    extern volatile uint32  SCB3_rxBufferHead;
    extern volatile uint32  SCB3_rxBufferTail;
    
    /**
    * \addtogroup group_globals
    * @{
    */
    
    /** Sets when internal software receive buffer overflow
     *  was occurred.
    */  
    extern volatile uint8   SCB3_rxBufferOverflow;
    /** @} globals */
#endif /* (SCB3_INTERNAL_RX_SW_BUFFER_CONST) */

#if (SCB3_INTERNAL_TX_SW_BUFFER_CONST)
    extern volatile uint32  SCB3_txBufferHead;
    extern volatile uint32  SCB3_txBufferTail;
#endif /* (SCB3_INTERNAL_TX_SW_BUFFER_CONST) */

#if (SCB3_INTERNAL_RX_SW_BUFFER)
    extern volatile uint8 SCB3_rxBufferInternal[SCB3_INTERNAL_RX_BUFFER_SIZE];
#endif /* (SCB3_INTERNAL_RX_SW_BUFFER) */

#if (SCB3_INTERNAL_TX_SW_BUFFER)
    extern volatile uint8 SCB3_txBufferInternal[SCB3_TX_BUFFER_SIZE];
#endif /* (SCB3_INTERNAL_TX_SW_BUFFER) */


/***************************************
*     Private Function Prototypes
***************************************/

void SCB3_SpiPostEnable(void);
void SCB3_SpiStop(void);

#if (SCB3_SCB_MODE_SPI_CONST_CFG)
    void SCB3_SpiInit(void);
#endif /* (SCB3_SCB_MODE_SPI_CONST_CFG) */

#if (SCB3_SPI_WAKE_ENABLE_CONST)
    void SCB3_SpiSaveConfig(void);
    void SCB3_SpiRestoreConfig(void);
#endif /* (SCB3_SPI_WAKE_ENABLE_CONST) */

void SCB3_UartPostEnable(void);
void SCB3_UartStop(void);

#if (SCB3_SCB_MODE_UART_CONST_CFG)
    void SCB3_UartInit(void);
#endif /* (SCB3_SCB_MODE_UART_CONST_CFG) */

#if (SCB3_UART_WAKE_ENABLE_CONST)
    void SCB3_UartSaveConfig(void);
    void SCB3_UartRestoreConfig(void);
#endif /* (SCB3_UART_WAKE_ENABLE_CONST) */


/***************************************
*         UART API Constants
***************************************/

/* UART RX and TX position to be used in SCB3_SetPins() */
#define SCB3_UART_RX_PIN_ENABLE    (SCB3_UART_RX)
#define SCB3_UART_TX_PIN_ENABLE    (SCB3_UART_TX)

/* UART RTS and CTS position to be used in  SCB3_SetPins() */
#define SCB3_UART_RTS_PIN_ENABLE    (0x10u)
#define SCB3_UART_CTS_PIN_ENABLE    (0x20u)


/***************************************
* The following code is DEPRECATED and
* must not be used.
***************************************/

/* Interrupt processing */
#define SCB3_SpiUartEnableIntRx(intSourceMask)  SCB3_SetRxInterruptMode(intSourceMask)
#define SCB3_SpiUartEnableIntTx(intSourceMask)  SCB3_SetTxInterruptMode(intSourceMask)
uint32  SCB3_SpiUartDisableIntRx(void);
uint32  SCB3_SpiUartDisableIntTx(void);


#endif /* (CY_SCB_SPI_UART_PVT_SCB3_H) */


/* [] END OF FILE */
