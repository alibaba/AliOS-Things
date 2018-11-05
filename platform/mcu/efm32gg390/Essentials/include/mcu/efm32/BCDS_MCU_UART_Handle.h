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
 * Defines MCU uart handle
 *
 * @details
 * MCU functions do always have at least an integer parameter of type UART_T.
 * This integer is casted by MCU to a pointer to MCU_UART_handle_T.
 * The data pointed to by USART_TypeDef* is managed in BSP.
 */

#ifndef BCDS_MCU_UART_HANDLE_H_
#define BCDS_MCU_UART_HANDLE_H_

#include "BCDS_MCU_UART.h"

#if BCDS_FEATURE_UART && BCDS_UART_COUNT

#include "em_usart.h"
#include "em_leuart.h"
#include "em_dma.h"

/**
 * @brief   Data type definitions for the UART IRQ/DMA callback functions.
 *
 * The IRQ callback is registered at the MCU_UART_Initialize function
 * by setting a member inside the structure of the uart handle.
 *
 * It is invoked by the UART BSP driver to notify the MCU.
 *
 * @param[in] uart : The handle of uart
 *
 * @note There is no need for an event parameter because all information is
 * available through the handle of type UART_T.
 *
 * @see MCU_UART_Initialize()
 */
typedef void (*UART_IRQ_Callback_T)(UART_T uart);
typedef void (*UART_DMA_Callback_T)(UART_T uart);
typedef void (*UART_CTS_Callback_T)(UART_T uart);
typedef void (*UART_RTS_Callback_T)(UART_T uart);

typedef enum MCU_UART_State_E
{
    UART_STATE_INIT,
    UART_STATE_READY,
    UART_STATE_RUNNING

} MCU_UART_State_T;
/**
 * @brief predeclaration of MCU_UART_Handle_S structure needed by MCU_UART_Driver_S
 */
struct MCU_UART_Handle_S;

struct MCU_UART_Driver_S
{
    Retcode_T (*receive)(struct MCU_UART_Handle_S* uart_ptr, uint8_t * data, uint32_t len); /**< UART/USART/LEUART receive */
    Retcode_T (*send)(struct MCU_UART_Handle_S* uart_ptr, uint8_t * data, uint32_t len); /**< UART/USART/LEUART send */
    MCU_UART_Callback_T AppCallback; /**< Callback function given in MCU_UART_Initialize to notify application */
    Retcode_T (*sendCancel)(struct MCU_UART_Handle_S* uart_ptr); /**< UART/USART/LEUART send cancel */
    Retcode_T (*receiveCancel)(struct MCU_UART_Handle_S* uart_ptr); /**< UART/USART/LEUART receive cancel */
    Retcode_T (*countRx)(struct MCU_UART_Handle_S* uart_ptr,uint32_t* count); /**< UART/USART/LEUART receive cancel */
    Retcode_T (*countTx)(struct MCU_UART_Handle_S* uart_ptr,uint32_t* count); /**< UART/USART/LEUART receive cancel */
    MCU_UART_State_T TxState;
    MCU_UART_State_T RxState;
    uint32_t TxXferSize; /*!< UART Tx Transfer size              */
    uint32_t TxXferCount; /*!< UART Tx Transfer Counter           */
    uint32_t RxXferSize; /*!< UART Rx Transfer size              */
    uint32_t RxXferCount; /*!< UART Rx Transfer Counter           */
    uint8_t *pTxBuffPtr; /*!< Pointer to UART Tx transfer Buffer */
    uint8_t *pRxBuffPtr; /*!< Pointer to UART Rx transfer Buffer */

};
typedef struct MCU_UART_Driver_S MCU_UART_Driver_T;

struct MCU_UART_Handle_S
{
    USART_TypeDef* Uart_ptr; /*!< type of driver */
    uint32_t Index; /*!< Index of the MCU UART interface. Assigned by MCU UART
     upon initialization. */
    enum BCDS_HAL_TransferMode_E TransferMode; /*!< Set by BSP to tell MCU whether to use interrupt-mode or DMA */
    uint32_t FlowCtl; /*!< Specifies whether the flow control mode is enabled
     or disabled.
     This parameter can be a value of @ref UART_Hardware_Flow_Control. */
    uint32_t Baudrate; /*!< baudrate for UART  */
    uint8_t Parity;
    uint8_t Stopbits;
    UART_IRQ_Callback_T IrqRxCallback; /*!< Function invoked by BSP in case of RxIRQ  */
    UART_IRQ_Callback_T IrqTxCallback; /*!< Function invoked by BSP in case of RxIRQ  */
    UART_DMA_Callback_T DmaRxCallback; /*!< Function invoked by BSP in case of Rx DMA IRQ  */
    UART_DMA_Callback_T DmaTxCallback; /*!< Function invoked by BSP in case of Tx DMA IRQ  */
    UART_CTS_Callback_T CtsCallback; /*!< Function invoked by BSP in case of CTS IRQ  */
    UART_RTS_Callback_T RtsCallback; /*!< Function invoked by BSP in case of RTS IRQ  */
    void * Link1;                    /*!< General purpose link register0  */
    void * Link2;                    /*!< General purpose link register1  */

    MCU_UART_Driver_T _DriverCtx;

    SWHandle_T ParentHandle;     /**< OS aware UART parent handle */
};

#endif /* BCDS_FEATURE_UART && UART_COUNT */

#endif /* BCDS_MCU_UART_HANDLE_H_ */

