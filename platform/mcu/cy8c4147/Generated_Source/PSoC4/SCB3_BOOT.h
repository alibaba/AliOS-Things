/***************************************************************************//**
* \file SCB3_BOOT.h
* \version 4.0
*
* \brief
*  This file provides constants and parameter values of the bootloader
*  communication APIs for the SCB Component.
*
* Note:
*
********************************************************************************
* \copyright
* Copyright 2014-2017, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_SCB_BOOT_SCB3_H)
#define CY_SCB_BOOT_SCB3_H

#include "SCB3_PVT.h"

#if (SCB3_SCB_MODE_I2C_INC)
    #include "SCB3_I2C.h"
#endif /* (SCB3_SCB_MODE_I2C_INC) */

#if (SCB3_SCB_MODE_EZI2C_INC)
    #include "SCB3_EZI2C.h"
#endif /* (SCB3_SCB_MODE_EZI2C_INC) */

#if (SCB3_SCB_MODE_SPI_INC || SCB3_SCB_MODE_UART_INC)
    #include "SCB3_SPI_UART.h"
#endif /* (SCB3_SCB_MODE_SPI_INC || SCB3_SCB_MODE_UART_INC) */


/***************************************
*  Conditional Compilation Parameters
****************************************/

/* Bootloader communication interface enable */
#define SCB3_BTLDR_COMM_ENABLED ((CYDEV_BOOTLOADER_IO_COMP == CyBtldr_SCB3) || \
                                             (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Custom_Interface))

/* Enable I2C bootloader communication */
#if (SCB3_SCB_MODE_I2C_INC)
    #define SCB3_I2C_BTLDR_COMM_ENABLED     (SCB3_BTLDR_COMM_ENABLED && \
                                                            (SCB3_SCB_MODE_UNCONFIG_CONST_CFG || \
                                                             SCB3_I2C_SLAVE_CONST))
#else
     #define SCB3_I2C_BTLDR_COMM_ENABLED    (0u)
#endif /* (SCB3_SCB_MODE_I2C_INC) */

/* EZI2C does not support bootloader communication. Provide empty APIs */
#if (SCB3_SCB_MODE_EZI2C_INC)
    #define SCB3_EZI2C_BTLDR_COMM_ENABLED   (SCB3_BTLDR_COMM_ENABLED && \
                                                         SCB3_SCB_MODE_UNCONFIG_CONST_CFG)
#else
    #define SCB3_EZI2C_BTLDR_COMM_ENABLED   (0u)
#endif /* (SCB3_EZI2C_BTLDR_COMM_ENABLED) */

/* Enable SPI bootloader communication */
#if (SCB3_SCB_MODE_SPI_INC)
    #define SCB3_SPI_BTLDR_COMM_ENABLED     (SCB3_BTLDR_COMM_ENABLED && \
                                                            (SCB3_SCB_MODE_UNCONFIG_CONST_CFG || \
                                                             SCB3_SPI_SLAVE_CONST))
#else
        #define SCB3_SPI_BTLDR_COMM_ENABLED (0u)
#endif /* (SCB3_SPI_BTLDR_COMM_ENABLED) */

/* Enable UART bootloader communication */
#if (SCB3_SCB_MODE_UART_INC)
       #define SCB3_UART_BTLDR_COMM_ENABLED    (SCB3_BTLDR_COMM_ENABLED && \
                                                            (SCB3_SCB_MODE_UNCONFIG_CONST_CFG || \
                                                             (SCB3_UART_RX_DIRECTION && \
                                                              SCB3_UART_TX_DIRECTION)))
#else
     #define SCB3_UART_BTLDR_COMM_ENABLED   (0u)
#endif /* (SCB3_UART_BTLDR_COMM_ENABLED) */

/* Enable bootloader communication */
#define SCB3_BTLDR_COMM_MODE_ENABLED    (SCB3_I2C_BTLDR_COMM_ENABLED   || \
                                                     SCB3_SPI_BTLDR_COMM_ENABLED   || \
                                                     SCB3_EZI2C_BTLDR_COMM_ENABLED || \
                                                     SCB3_UART_BTLDR_COMM_ENABLED)


/***************************************
*        Function Prototypes
***************************************/

#if defined(CYDEV_BOOTLOADER_IO_COMP) && (SCB3_I2C_BTLDR_COMM_ENABLED)
    /* I2C Bootloader physical layer functions */
    void SCB3_I2CCyBtldrCommStart(void);
    void SCB3_I2CCyBtldrCommStop (void);
    void SCB3_I2CCyBtldrCommReset(void);
    cystatus SCB3_I2CCyBtldrCommRead       (uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);
    cystatus SCB3_I2CCyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);

    /* Map I2C specific bootloader communication APIs to SCB specific APIs */
    #if (SCB3_SCB_MODE_I2C_CONST_CFG)
        #define SCB3_CyBtldrCommStart   SCB3_I2CCyBtldrCommStart
        #define SCB3_CyBtldrCommStop    SCB3_I2CCyBtldrCommStop
        #define SCB3_CyBtldrCommReset   SCB3_I2CCyBtldrCommReset
        #define SCB3_CyBtldrCommRead    SCB3_I2CCyBtldrCommRead
        #define SCB3_CyBtldrCommWrite   SCB3_I2CCyBtldrCommWrite
    #endif /* (SCB3_SCB_MODE_I2C_CONST_CFG) */

#endif /* defined(CYDEV_BOOTLOADER_IO_COMP) && (SCB3_I2C_BTLDR_COMM_ENABLED) */


#if defined(CYDEV_BOOTLOADER_IO_COMP) && (SCB3_EZI2C_BTLDR_COMM_ENABLED)
    /* Bootloader physical layer functions */
    void SCB3_EzI2CCyBtldrCommStart(void);
    void SCB3_EzI2CCyBtldrCommStop (void);
    void SCB3_EzI2CCyBtldrCommReset(void);
    cystatus SCB3_EzI2CCyBtldrCommRead       (uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);
    cystatus SCB3_EzI2CCyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);

    /* Map EZI2C specific bootloader communication APIs to SCB specific APIs */
    #if (SCB3_SCB_MODE_EZI2C_CONST_CFG)
        #define SCB3_CyBtldrCommStart   SCB3_EzI2CCyBtldrCommStart
        #define SCB3_CyBtldrCommStop    SCB3_EzI2CCyBtldrCommStop
        #define SCB3_CyBtldrCommReset   SCB3_EzI2CCyBtldrCommReset
        #define SCB3_CyBtldrCommRead    SCB3_EzI2CCyBtldrCommRead
        #define SCB3_CyBtldrCommWrite   SCB3_EzI2CCyBtldrCommWrite
    #endif /* (SCB3_SCB_MODE_EZI2C_CONST_CFG) */

#endif /* defined(CYDEV_BOOTLOADER_IO_COMP) && (SCB3_EZI2C_BTLDR_COMM_ENABLED) */

#if defined(CYDEV_BOOTLOADER_IO_COMP) && (SCB3_SPI_BTLDR_COMM_ENABLED)
    /* SPI Bootloader physical layer functions */
    void SCB3_SpiCyBtldrCommStart(void);
    void SCB3_SpiCyBtldrCommStop (void);
    void SCB3_SpiCyBtldrCommReset(void);
    cystatus SCB3_SpiCyBtldrCommRead       (uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);
    cystatus SCB3_SpiCyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);

    /* Map SPI specific bootloader communication APIs to SCB specific APIs */
    #if (SCB3_SCB_MODE_SPI_CONST_CFG)
        #define SCB3_CyBtldrCommStart   SCB3_SpiCyBtldrCommStart
        #define SCB3_CyBtldrCommStop    SCB3_SpiCyBtldrCommStop
        #define SCB3_CyBtldrCommReset   SCB3_SpiCyBtldrCommReset
        #define SCB3_CyBtldrCommRead    SCB3_SpiCyBtldrCommRead
        #define SCB3_CyBtldrCommWrite   SCB3_SpiCyBtldrCommWrite
    #endif /* (SCB3_SCB_MODE_SPI_CONST_CFG) */

#endif /* defined(CYDEV_BOOTLOADER_IO_COMP) && (SCB3_SPI_BTLDR_COMM_ENABLED) */

#if defined(CYDEV_BOOTLOADER_IO_COMP) && (SCB3_UART_BTLDR_COMM_ENABLED)
    /* UART Bootloader physical layer functions */
    void SCB3_UartCyBtldrCommStart(void);
    void SCB3_UartCyBtldrCommStop (void);
    void SCB3_UartCyBtldrCommReset(void);
    cystatus SCB3_UartCyBtldrCommRead       (uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);
    cystatus SCB3_UartCyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);

    /* Map UART specific bootloader communication APIs to SCB specific APIs */
    #if (SCB3_SCB_MODE_UART_CONST_CFG)
        #define SCB3_CyBtldrCommStart   SCB3_UartCyBtldrCommStart
        #define SCB3_CyBtldrCommStop    SCB3_UartCyBtldrCommStop
        #define SCB3_CyBtldrCommReset   SCB3_UartCyBtldrCommReset
        #define SCB3_CyBtldrCommRead    SCB3_UartCyBtldrCommRead
        #define SCB3_CyBtldrCommWrite   SCB3_UartCyBtldrCommWrite
    #endif /* (SCB3_SCB_MODE_UART_CONST_CFG) */

#endif /* defined(CYDEV_BOOTLOADER_IO_COMP) && (SCB3_UART_BTLDR_COMM_ENABLED) */

/**
* \addtogroup group_bootloader
* @{
*/

#if defined(CYDEV_BOOTLOADER_IO_COMP) && (SCB3_BTLDR_COMM_ENABLED)
    #if (SCB3_SCB_MODE_UNCONFIG_CONST_CFG)
        /* Bootloader physical layer functions */
        void SCB3_CyBtldrCommStart(void);
        void SCB3_CyBtldrCommStop (void);
        void SCB3_CyBtldrCommReset(void);
        cystatus SCB3_CyBtldrCommRead       (uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);
        cystatus SCB3_CyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);
    #endif /* (SCB3_SCB_MODE_UNCONFIG_CONST_CFG) */

    /* Map SCB specific bootloader communication APIs to common APIs */
    #if (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_SCB3)
        #define CyBtldrCommStart    SCB3_CyBtldrCommStart
        #define CyBtldrCommStop     SCB3_CyBtldrCommStop
        #define CyBtldrCommReset    SCB3_CyBtldrCommReset
        #define CyBtldrCommWrite    SCB3_CyBtldrCommWrite
        #define CyBtldrCommRead     SCB3_CyBtldrCommRead
    #endif /* (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_SCB3) */

#endif /* defined(CYDEV_BOOTLOADER_IO_COMP) && (SCB3_BTLDR_COMM_ENABLED) */

/** @} group_bootloader */

/***************************************
*           API Constants
***************************************/

/* Timeout unit in milliseconds */
#define SCB3_WAIT_1_MS  (1u)

/* Return number of bytes to copy into bootloader buffer */
#define SCB3_BYTES_TO_COPY(actBufSize, bufSize) \
                            ( ((uint32)(actBufSize) < (uint32)(bufSize)) ? \
                                ((uint32) (actBufSize)) : ((uint32) (bufSize)) )

/* Size of Read/Write buffers for I2C bootloader  */
#define SCB3_I2C_BTLDR_SIZEOF_READ_BUFFER   (64u)
#define SCB3_I2C_BTLDR_SIZEOF_WRITE_BUFFER  (64u)

/* Byte to byte time interval: calculated basing on current component
* data rate configuration, can be defined in project if required.
*/
#ifndef SCB3_SPI_BYTE_TO_BYTE
    #define SCB3_SPI_BYTE_TO_BYTE   (160u)
#endif

/* Byte to byte time interval: calculated basing on current component
* baud rate configuration, can be defined in the project if required.
*/
#ifndef SCB3_UART_BYTE_TO_BYTE
    #define SCB3_UART_BYTE_TO_BYTE  (174u)
#endif /* SCB3_UART_BYTE_TO_BYTE */

#endif /* (CY_SCB_BOOT_SCB3_H) */


/* [] END OF FILE */
