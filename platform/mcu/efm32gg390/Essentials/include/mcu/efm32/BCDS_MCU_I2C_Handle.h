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
* Defines MCU I2C handle
*
* @details
* MCU functions do always have at least an integer parameter of type I2C_T.
* This integer is casted by MCU to a pointer to MCU_I2C_handle_T.
* The data pointed to by USART_TypeDef* is managed in BSP.
*/

#ifndef BCDS_MCU_I2C_HANDLE_H_
#define BCDS_MCU_I2C_HANDLE_H_

#include "BCDS_MCU_I2C.h"

#if BCDS_FEATURE_I2C && BCDS_I2C_COUNT

#include "em_i2c.h"
#include "em_dma.h"

/**
* @brief   Data type definition for the I2C IRQ callback function.
*
* The IRQ callback is registered at the MCU_I2C_Initialize function
* by setting a member inside the structure of the I2C handle.
*
* It is invoked by the I2C BSP driver to notify the MCU.
*
* @param[in] I2C : The handle of I2C
*
* @note There is no need for an event parameter because all information is
* available through the handle of type I2C_T.
*
* @see MCU_I2C_Initialize()
*/
typedef void (*I2C_IRQ_Callback_T)(I2C_T I2C);
typedef void (*I2C_DMA_Callback_T)(I2C_T I2C);

struct MCU_I2C_Driver_S
{
    MCU_I2C_Callback_T AppCallback;  /**< Callback function given in MCU_I2C_Initialize to notify application */
    I2C_TransferSeq_TypeDef I2C_seq; /**< I2C Transfer Sequence descriptor (from emlib) */
    uint8_t RegisterAddr;            /*!< scratch buffer to hold the register address for reading/writing */
    uint8_t *pTxBuffPtr;             /*!< Pointer to I2C Tx transfer Buffer  */
    uint32_t TxXferSize;             /*!< I2C Tx Transfer size               */
    uint32_t TxXferCount;            /*!< I2C Tx Transfer Counter            */
    uint8_t *pRxBuffPtr;             /*!< Pointer to I2C Rx transfer Buffer  */
    uint32_t RxXferSize;             /*!< I2C Rx Transfer size               */
    uint32_t RxXferCount;            /*!< I2C Rx Transfer Counter            */
    bool     Initialized;            /*!< driver initialization state        */
};
typedef struct MCU_I2C_Driver_S MCU_I2C_Driver_T;

/**
* @brief   Structure used as I2C handle.
* @detail  A pointer to this structure is wrapped in I2C_T for interface functions.
*/
struct MCU_I2C_Handle_S
{
    I2C_TypeDef* instance;                      /*!< type of driver */
    uint32_t I2cClockFreq;                      /*!< I2C clock frequency*/
    enum BCDS_HAL_TransferMode_E TransferMode; /**< Set by BSP to tell MCU whether to use interrupt-mode or DMA */
    I2C_IRQ_Callback_T IRQCallback;    /**< Function invoked by BSP in case IRQ and interrupt mode */
    I2C_DMA_Callback_T DmaRxCallback;  /**< Function invoked by BSP in case IRQ and Rx DMA mode */
    I2C_DMA_Callback_T DmaTxCallback;  /**< Function invoked by BSP in case IRQ and Tx DMA mode  */
    void * Link1;                      /**< general purpose link register 1 (used for e.g. DMA TX handle) */
    void * Link2;                      /**< general purpose link register 2 (used for e.g. DMA RX handle) */
    MCU_I2C_Driver_T   _DriverCtx;     /**< context pointer to driver context, private member */

    SWHandle_T ParentHandle;     /**< OS aware I2C parent handle */
};

#endif /* BCDS_FEATURE_I2C && BCDS_I2C_COUNT */

#endif /* BCDS_MCU_I2C_HANDLE_H_ */

