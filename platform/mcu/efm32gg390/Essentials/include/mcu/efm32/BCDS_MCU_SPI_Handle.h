/*----------------------------------------------------------------------------*/
/*
 * Copyright (C) Bosch Connected Devices and Solutions GmbH.
 * All Rights Reserved. Confidential.
 *
 * Distribution only to people who need to know this information in
 * order to do their job.(Need-to-know principle).
 * Distribution to persons outside the company, only if these persons
 * signed a non-disclosure agreement.
 * Electronic transmission, e.g. via electronic mail, must be made in
 * encrypted form.
 */
/*----------------------------------------------------------------------------*/

/*
 *
 * @brief
 * Defines MCU spi handle
 *
 * @details
 * MCU functions do always have at least an integer parameter of type SPI_T.
 * This integer is casted by MCU to a pointer to MCU_SPI_handle_T.
 * The data pointed to by SPI_TypeDef* is managed in BSP.
 */

#ifndef BCDS_MCU_SPI_HANDLE_H_
#define BCDS_MCU_SPI_HANDLE_H_

#include "BCDS_MCU_SPI.h"

#if BCDS_FEATURE_SPI && BCDS_SPI_COUNT

#include "em_usart.h"
#include "em_dma.h"

/**
 * @brief This data type represents a function pointer which is used between
 * BSP and MCU SPI as a callback whenever an IRQ event is to be notified from
 * the BSP to the MCU SPI driver.
 *
 * @param [in] spi : Handle of the SPI whose IRQ event should be handled.
 */
typedef void (*SPI_IRQ_Callback_T)(SPI_T spi);
typedef void (*SPI_DMA_Callback_T)(SPI_T spi);

/**
 * Function of the MCU SPI IRQ RX callback handler which is
 * used by the BSP to notify MCU about RX IRQ events.
 */
void MCU_SPI_IRQ_RX_Callback(SPI_T spi);

/**
 * Function of the MCU SPI IRQ RX callback handler which is
 * used by the BSP to notify MCU about TX IRQ events.
 */
void MCU_SPI_IRQ_TX_Callback(SPI_T spi);
/**
 * Function of the MCU SPI IRQ RX callback handler which is
 * used by the BSP to notify MCU about DMA RX IRQ events.
 */
void MCU_SPI_DMA_RX_Callback(SPI_T spi);
/**
 * Function of the MCU SPI IRQ RX callback handler which is
 * used by the BSP to notify MCU about DMA TX IRQ events.
 */
void MCU_SPI_DMA_TX_Callback(SPI_T spi);

/**
 * Enumeration of driver state machine states.
 */
typedef enum MCU_SPI_State_E
{
    SPI_STATE_INIT,
    SPI_STATE_READY,
	SPI_STATE_TX,
	SPI_STATE_RX,
	SPI_STATE_TX_RX
} MCU_SPI_State_T;

/**
 * @brief predeclaration of MCU_SPI_Handle_S structure needed by MCU_SPI_Driver_S
 */
struct MCU_SPI_Handle_S;

/**
 * @brief   Structure which is used as SPI handle.
 *
 * @detail  The handle is usually created by the BSP and fetched by the
 * application layer to use the SPI interface.
 */
struct MCU_SPI_Handle_S
{
    USART_TypeDef* Instance;                    /**<    Pointer on EFM32 SPI instance created by the BSP*/
    enum BCDS_HAL_TransferMode_E TransferMode;  /**<    Identifies the transfer
                                                        mode that is currently configured for this SPI instance.
                                                        This value will be set by the BSP upon configuration of the
                                                        SPI interface.*/
    /* context specific variables */
    MCU_SPI_Callback_T AppCallback;            /**< Callback function given in MCU_SPI_Initialize to notify application */
    uint8_t TxDefaultData;                     /** default buffer to send when there is nothing to send */

    uint32_t Baudrate;  /**< SPI Baudrate */
    uint8_t Mode;	/**< SPI Mode */
    uint8_t BitOrder;   /**< SPI Bit Order is either Most Significant bit or Least Significant Bit  */

    Retcode_T (*receive)(struct MCU_SPI_Handle_S* , uint8_t * dataIn, uint32_t datalength); /**< SPI receive function pointer*/
    Retcode_T (*transfer)(struct MCU_SPI_Handle_S* spi_ptr, uint8_t * dataOut, uint8_t * dataIn, uint32_t dataLength); /**< SPI transfer function pointer */
    Retcode_T (*send)(struct MCU_SPI_Handle_S* spi_ptr, uint8_t * dataOut, uint32_t datalength); /**< SPI send */
    Retcode_T (*cancel)(struct MCU_SPI_Handle_S* spi_ptr); /**< SPI receive cancel */
    MCU_SPI_State_T State;   /**< State of the receive driver */
    uint8_t *pRxBuffPtr;     /**< Pointer to SPI Rx transfer Buffer */
    uint32_t XferSize;       /**< SPI Rx Transfer size              */
    uint32_t RxXferCount;    /**< SPI Rx Transfer Counter           */
    uint8_t *pTxBuffPtr;     /**< Pointer to SPI Tx transfer Buffer */
    uint32_t TxXferCount;    /**< SPI Tx Transfer Counter           */
    void * Link1;            /**< general purpose link register 1 (used for e.g. DMA TX handle) */
    void * Link2;            /**< general purpose link register 2 (used for e.g. DMA RX handle) */

    SPI_Owner_T ParentHandle;     /**< OS aware SPI parent handle */
};

#endif /* BCDS_FEATURE_SPI && BCDS_SPI_COUNT */
#endif /* BCDS_MCU_SPI_HANDLE_H_ */
