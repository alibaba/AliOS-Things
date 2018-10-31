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
 * Defines MCU leuart handle
 *
 * @details
 * MCU functions do always have at least an integer parameter of type LEUART_T.
 * This integer is casted by MCU to a pointer to MCU_LEUART_handle_T.
 * The data pointed to by USART_TypeDef* is managed in BSP.
 */

#ifndef BCDS_MCU_LEUART_HANDLE_H_
#define BCDS_MCU_LEUART_HANDLE_H_

#include "BCDS_MCU_LEUART.h"

#if BCDS_FEATURE_LEUART && BCDS_LEUART_COUNT

#include "em_leuart.h"
#include "em_dma.h"

/**
 * @brief   Data type definitions for the LEUART IRQ/DMA callback functions.
 *
 * The IRQ callback is registered at the MCU_LEUART_Initialize function
 * by setting a member inside the structure of the leuart handle.
 *
 * It is invoked by the LEUART BSP driver to notify the MCU.
 *
 * @param[in] leuart : The handle of leuart
 *
 * @note There is no need for an event parameter because all information is
 * available through the handle of type LEUART_T.
 *
 * @see MCU_LEUART_Initialize()
 */
typedef void (*LEUART_IRQ_Callback_T)(LEUART_T leuart);
typedef void (*LEUART_DMA_Callback_T)(LEUART_T leuart);
typedef void (*LEUART_CTS_Callback_T)(LEUART_T leuart);
typedef void (*LEUART_RTS_Callback_T)(LEUART_T leuart);

typedef enum MCU_LEUART_State_E
{
    LEUART_STATE_INIT,
    LEUART_STATE_READY,
    LEUART_STATE_RUNNING
} MCU_LEUART_State_T;
/**
 * @brief predeclaration of MCU_LEUART_Handle_S structure needed by MCU_LEUART_Driver_S
 */
struct MCU_LEUART_Handle_S;

struct MCU_LEUART_Driver_S
{
    Retcode_T (*receive)(struct MCU_LEUART_Handle_S* leuart_ptr, uint8_t * data, uint32_t len); /**< UART/USART/LEUART receive */
    Retcode_T (*send)(struct MCU_LEUART_Handle_S* leuart_ptr, uint8_t * data, uint32_t len); /**< UART/USART/LEUART send */
    MCU_LEUART_Callback_T AppCallback; /**< Callback function given in MCU_UART_Initialize to notify application */
    Retcode_T (*sendCancel)(struct MCU_LEUART_Handle_S* leuart_ptr); /**< UART/USART/LEUART send cancel */
    Retcode_T (*receiveCancel)(struct MCU_LEUART_Handle_S* leuart_ptr); /**< UART/USART/LEUART receive cancel */
    Retcode_T (*countRx)(struct MCU_LEUART_Handle_S* leuart_ptr,uint32_t* count); /**< UART/USART/LEUART receive cancel */
    Retcode_T (*countTx)(struct MCU_LEUART_Handle_S* leuart_ptr,uint32_t* count); /**< UART/USART/LEUART receive cancel */
    MCU_LEUART_State_T TxState;
    MCU_LEUART_State_T RxState;
    uint32_t TxXferSize; /*!< LEUART Tx Transfer size              */
    uint32_t TxXferCount; /*!< LEUART Tx Transfer Counter           */
    uint32_t RxXferSize; /*!< LEUART Rx Transfer size              */
    uint32_t RxXferCount; /*!< LEUART Rx Transfer Counter           */
    uint8_t *pTxBuffPtr; /*!< Pointer to LEUART Tx transfer Buffer */
    uint8_t *pRxBuffPtr; /*!< Pointer to LEUART Rx transfer Buffer */

};
typedef struct MCU_LEUART_Driver_S MCU_LEUART_Driver_T;

struct MCU_LEUART_Handle_S
{
    LEUART_TypeDef*                 leuart_ptr;                     /*!< type of driver */
    enum BCDS_HAL_TransferMode_E    TransferMode;                   /*!< Set by BSP to tell MCU whether to use interrupt-mode or DMA */
    uint32_t                        FlowCtl;                        /*!< Specifies whether the flow control mode is enabled
                                                                         or disabled.
                                                                         This parameter can be a value of @ref LEUART_Hardware_Flow_Control. */
    LEUART_IRQ_Callback_T             IrqRxCallback;                  /*!< Function invoked by BSP in case of RxIRQ  */
    LEUART_IRQ_Callback_T             IrqTxCallback;                  /*!< Function invoked by BSP in case of RxIRQ  */
    LEUART_DMA_Callback_T          	DmaRxCallback;                  /*!< Function invoked by BSP in case of Rx DMA IRQ  */
    LEUART_DMA_Callback_T          	DmaTxCallback;                  /*!< Function invoked by BSP in case of Tx DMA IRQ  */
    LEUART_CTS_Callback_T             CtsCallback;                    /*!< Function invoked by BSP in case of CTS IRQ  */
    LEUART_RTS_Callback_T             RtsCallback;                    /*!< Function invoked by BSP in case of RTS IRQ  */
    void * Link1;                    /*!< General purpose link register0  */
    void * Link2;                    /*!< General purpose link register1  */
	MCU_LEUART_Driver_T _DriverCtx;
};

#endif /* BCDS_FEATURE_LEUART && BCDS_LEUART_COUNT */

#endif /* BCDS_MCU_LEUART_HANDLE_H_ */

