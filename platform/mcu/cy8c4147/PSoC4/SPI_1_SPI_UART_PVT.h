/***************************************************************************//**
* \file SPI_1_SPI_UART_PVT.h
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

#if !defined(CY_SCB_SPI_UART_PVT_SPI_1_H)
#define CY_SCB_SPI_UART_PVT_SPI_1_H

#include "SPI_1_SPI_UART.h"


/***************************************
*     Internal Global Vars
***************************************/

#if (SPI_1_INTERNAL_RX_SW_BUFFER_CONST)
    extern volatile uint32  SPI_1_rxBufferHead;
    extern volatile uint32  SPI_1_rxBufferTail;
    
    /**
    * \addtogroup group_globals
    * @{
    */
    
    /** Sets when internal software receive buffer overflow
     *  was occurred.
    */  
    extern volatile uint8   SPI_1_rxBufferOverflow;
    /** @} globals */
#endif /* (SPI_1_INTERNAL_RX_SW_BUFFER_CONST) */

#if (SPI_1_INTERNAL_TX_SW_BUFFER_CONST)
    extern volatile uint32  SPI_1_txBufferHead;
    extern volatile uint32  SPI_1_txBufferTail;
#endif /* (SPI_1_INTERNAL_TX_SW_BUFFER_CONST) */

#if (SPI_1_INTERNAL_RX_SW_BUFFER)
    extern volatile uint8 SPI_1_rxBufferInternal[SPI_1_INTERNAL_RX_BUFFER_SIZE];
#endif /* (SPI_1_INTERNAL_RX_SW_BUFFER) */

#if (SPI_1_INTERNAL_TX_SW_BUFFER)
    extern volatile uint8 SPI_1_txBufferInternal[SPI_1_TX_BUFFER_SIZE];
#endif /* (SPI_1_INTERNAL_TX_SW_BUFFER) */


/***************************************
*     Private Function Prototypes
***************************************/

void SPI_1_SpiPostEnable(void);
void SPI_1_SpiStop(void);

#if (SPI_1_SCB_MODE_SPI_CONST_CFG)
    void SPI_1_SpiInit(void);
#endif /* (SPI_1_SCB_MODE_SPI_CONST_CFG) */

#if (SPI_1_SPI_WAKE_ENABLE_CONST)
    void SPI_1_SpiSaveConfig(void);
    void SPI_1_SpiRestoreConfig(void);
#endif /* (SPI_1_SPI_WAKE_ENABLE_CONST) */

void SPI_1_UartPostEnable(void);
void SPI_1_UartStop(void);

#if (SPI_1_SCB_MODE_UART_CONST_CFG)
    void SPI_1_UartInit(void);
#endif /* (SPI_1_SCB_MODE_UART_CONST_CFG) */

#if (SPI_1_UART_WAKE_ENABLE_CONST)
    void SPI_1_UartSaveConfig(void);
    void SPI_1_UartRestoreConfig(void);
#endif /* (SPI_1_UART_WAKE_ENABLE_CONST) */


/***************************************
*         UART API Constants
***************************************/

/* UART RX and TX position to be used in SPI_1_SetPins() */
#define SPI_1_UART_RX_PIN_ENABLE    (SPI_1_UART_RX)
#define SPI_1_UART_TX_PIN_ENABLE    (SPI_1_UART_TX)

/* UART RTS and CTS position to be used in  SPI_1_SetPins() */
#define SPI_1_UART_RTS_PIN_ENABLE    (0x10u)
#define SPI_1_UART_CTS_PIN_ENABLE    (0x20u)


/***************************************
* The following code is DEPRECATED and
* must not be used.
***************************************/

/* Interrupt processing */
#define SPI_1_SpiUartEnableIntRx(intSourceMask)  SPI_1_SetRxInterruptMode(intSourceMask)
#define SPI_1_SpiUartEnableIntTx(intSourceMask)  SPI_1_SetTxInterruptMode(intSourceMask)
uint32  SPI_1_SpiUartDisableIntRx(void);
uint32  SPI_1_SpiUartDisableIntTx(void);


#endif /* (CY_SCB_SPI_UART_PVT_SPI_1_H) */


/* [] END OF FILE */
