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

/**
 * @file
 * @brief Contains the realization of the MCU I2C interface for STM32
 */
#include "BCDS_Essentials.h"
#undef BCDS_MODULE_ID
#define BCDS_MODULE_ID BCDS_ESSENTIALS_MODULE_ID_I2C

#include "BCDS_MCU_I2C.h"
#include "BCDS_MCU_I2C_Handle.h"

#include "stm32l4xx_hal.h"

#if BCDS_FEATURE_I2C && BCDS_I2C_COUNT

/* The STM32Cube Library provides a HAL driver for I2C I/O operations.
 * However depending on the configured transfer mode (Polling, Interrupt or DMA)
 * the developer has to call different functions (see stm32l4xx_hal_i2c.h).
 * Only the function signatures for IRQ and DMA transfer are the same. The
 * polling functions are introducing an additional parameter for a timeout.
 *
 * The plan is to provide function pointers on these functions (at least for
 * IRQ and DMA mode) and to set them according to the configuration of the I2C
 * interface upon initialization.
 */

/**
 * @brief Definition of values for HAL_I2C_IsDeviceReady function
 * Trials: Number of trials
 * Timeout: Timeout duration
 */
#define TRIALS                  10
#define TIMEOUT_MS              10 /* Timeout in ms */

enum
{
    TRANSFER_WAIT, TRANSFER_COMPLETE, TRANSFER_ERROR
};

volatile uint8_t wTransferState = TRANSFER_WAIT;


/* Function prototypes */
void MCU_BSP_I2C_EV_Callback(I2C_T i2c);
void MCU_BSP_I2C_ER_Callback(I2C_T i2c);
void MCU_BSP_I2C_DMA_TX_Callback(I2C_T i2c);
void MCU_BSP_I2C_DMA_RX_Callback(I2C_T i2c);

/** @brief I2C IRQ callback handler which is called back by the BSP I2C IRQ
 * handlers for event interrupt request
 * @details This handler is called by the BSP upon occurrence of any event
 * interrupt request of the I2C. This relates to communication event only.
 * Error related interrupt request are handled in MCU_BSP_I2C_ER_Callback().
 * @param i2c is the BSP handle for the interface
 */
void MCU_BSP_I2C_EV_Callback(I2C_T i2c)
{
    struct MCU_I2C_S* pI2C = (struct MCU_I2C_S*) i2c;
    if (pI2C)
    {
        /* Now call the IRQ Handler of the STM32 HAL driver layer.
         * This function will evaluate the event flags of the interrupt and
         * delegate the events to the according STM32 HAL callbacks
         */
        /* Get the STM handle from the BSP handle */
        I2C_HandleTypeDef* hdl = &pI2C->hi2c;
        /* Check if Master receive has not been invoked before and if there is
         * data in the receive register*/
        if ( __HAL_I2C_GET_FLAG(hdl, I2C_FLAG_RXNE) &&
                (hdl->State != HAL_I2C_STATE_MASTER_BUSY_RX) &&
                (NULL != pI2C->AppLayerCallback)
                )
        {
            /* Application did not invoke receive, but there is data to receive.
             * We have to signal this to the application.
             */
            struct MCU_I2C_Event_S event = { 0, 0, 0, 0 };
            /* Set the error bit in the event and signal it to application */
            event.RxReady = 1;
            pI2C->AppLayerCallback((I2C_T) pI2C, event);
        }
        else
        {
            HAL_I2C_EV_IRQHandler(hdl);
        }
    }
}

/** @brief I2C IRQ callback handler which is called back by the BSP I2C IRQ
 * handlers for error interrupt request
 * @details This handler is called by the BSP upon occurrence of any error
 * interrupt request of the I2C. This relates to error events only.
 * Non-Error related interrupt request are handled in MCU_BSP_I2C_EV_Callback().
 * @param i2c is the BSP handle for the interface
 */
void MCU_BSP_I2C_ER_Callback(I2C_T i2c)
{
	struct MCU_I2C_S* pI2C = (struct MCU_I2C_S*) i2c;
    if (pI2C)
    {
        /* Now call the IRQ Handler of the STM32 HAL driver layer.
         * This function will evaluate the event flags of the interrupt and
         * delegate the events to the according STM32 HAL callbacks
         */
        HAL_I2C_ER_IRQHandler(&pI2C->hi2c);
    }
}

/**
 * @brief I2C DMA Rx callback handler which is called back by the BSP
 * @details This functions gets called by the BSP whenever it gets a Rx
 * interrupt request from the DMA channel assigned to a I2C resource that is
 * configured to use DMA transfer mode.
 * The function is usually assigned to the BSP-MCU handler when initializing
 * the MCU I2C interface and its transfer mode is set to DMA.
 * @param i2c is the BSP handle for the interface whose DMA channel Rx request
 * should be handled.
 */
void MCU_BSP_I2C_DMA_RX_Callback(I2C_T i2c)
{
	struct MCU_I2C_S* pI2C = (struct MCU_I2C_S*) i2c;
    if (pI2C)
    {
        /* Now call the DMA Handler of the STM32 HAL driver layer.
         * This function will evaluate the event flags of the interrupt and
         * delegate the events to the according STM32 HAL callbacks
         */
        HAL_DMA_IRQHandler(pI2C->hi2c.hdmarx);
    }
}

/**
 * @brief I2C DMA Tx callback handler which is called back by the BSP
 * @details This functions gets called by the BSP whenever it gets a Tx
 * interrupt request from the DMA channel assigned to a I2C resource that is
 * configured to use DMA transfer mode.
 * The function is usually assigned to the BSP-MCU handler when initializing
 * the MCU I2C interface and its transfer mode is set to DMA.
 * @param i2c is the BSP handle for the interface whose DMA channel Tx request
 * should be handled.
 */
void MCU_BSP_I2C_DMA_TX_Callback(I2C_T i2c)
{
	struct MCU_I2C_S* pI2C = (struct MCU_I2C_S*) i2c;
    if (pI2C)
    {
        /* Now call the DMA Handler of the STM32 HAL driver layer.
         * This function will evaluate the event flags of the interrupt and
         * delegate the events to the according STM32 HAL callbacks
         */
        HAL_DMA_IRQHandler(pI2C->hi2c.hdmatx);
    }
}



/** See description in the interface declaration */
Retcode_T MCU_I2C_Initialize(I2C_T i2c, MCU_I2C_Callback_T callback)
{
    Retcode_T rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    struct MCU_I2C_S* pI2C = (struct MCU_I2C_S*) i2c;
    if (pI2C && callback)
    {
    	/* Now we have to check the configuration
    	 * of the interface in order to assign the correct transmit and
    	 * receive functions depending on the configured transfer mode.
    	 */
    	if (BCDS_HAL_TRANSFER_MODE_BLOCKING == pI2C->TransferMode)
    	{
    		/* this mode is currently not supported */
    		rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NOT_SUPPORTED);
    	}
    	else if (BCDS_HAL_TRANSFER_MODE_INTERRUPT == pI2C->TransferMode)
    	{
    		pI2C->IRQCallback = MCU_BSP_I2C_EV_Callback;
    		pI2C->ERRCallback = MCU_BSP_I2C_ER_Callback;
    		pI2C->DMATxCallback = NULL;
    		pI2C->DMARxCallback = NULL;
    		pI2C->AppLayerCallback = callback;

    		pI2C->TxFunPtr = HAL_I2C_Master_Transmit_IT;
    		pI2C->RxFunPtr = HAL_I2C_Master_Receive_IT;

    		rc = RETCODE_OK;
    	}
    	else if (BCDS_HAL_TRANSFER_MODE_DMA == pI2C->TransferMode)
    	{
    		pI2C->IRQCallback = MCU_BSP_I2C_EV_Callback;
    		pI2C->ERRCallback = MCU_BSP_I2C_ER_Callback;
    		pI2C->DMARxCallback = MCU_BSP_I2C_DMA_RX_Callback;
    		pI2C->DMATxCallback = MCU_BSP_I2C_DMA_TX_Callback;
    		pI2C->AppLayerCallback = callback;

    		pI2C->RxFunPtr = HAL_I2C_Master_Receive_DMA;
    		pI2C->TxFunPtr = HAL_I2C_Master_Transmit_DMA;

    		rc = RETCODE_OK;
    	}
    	else
    	{
    		/* all other modes are currently not supported */
    		rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NOT_SUPPORTED);
    	}
    }/* if (pI2C && callback) */
    return rc;
}

/** See description in the interface declaration */
Retcode_T MCU_I2C_Deinitialize(I2C_T i2c)
{
    /* Here we are checking if the passed i2c handle is valid and if it
     * is valid we mark our local interface context for that handle as being
     * free again.
     */
    Retcode_T rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    struct MCU_I2C_S* pI2C = (struct MCU_I2C_S*) i2c;
    if (pI2C)
    {
        /* Seems to be valid and initialized, now remove it from the
         * I2C interface array
         */
        pI2C->AppLayerCallback = NULL;
        pI2C->IRQCallback = NULL;
        pI2C->ERRCallback = NULL;
        pI2C->DMARxCallback = NULL;
        pI2C->DMATxCallback = NULL;
        pI2C->Index = 0;
        pI2C->RxFunPtr = NULL;
        pI2C->TxFunPtr = NULL;
        rc = RETCODE_OK;
    }
    return rc;
}

/** See description in the interface declaration */
/** todo implement cancel for sending */
Retcode_T MCU_I2C_Send(I2C_T i2c, uint16_t slaveAddr,
        uint8_t * data, uint32_t len)
{
    Retcode_T rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    struct MCU_I2C_S* pI2C = (struct MCU_I2C_S*) i2c;
    if (pI2C)
    {
        if (data && len)
        {
            /* Need to send something but first check whether len is in uint16_t
             * range because uint32_t sends are not supported by the STM driver
             */
            if (UINT16_MAX < len)
            {
                rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NOT_SUPPORTED);
            }
            else
            {
                if (I2C_ADDRESSINGMODE_7BIT == pI2C->hi2c.Init.AddressingMode)
                {
                    slaveAddr = slaveAddr << 1; /**< 7bit Adressing Mode */
                }

                if (HAL_OK == HAL_I2C_IsDeviceReady(&pI2C->hi2c, slaveAddr, TRIALS, TIMEOUT_MS))
                {

                    /* SEND */
                    HAL_StatusTypeDef halStatus;

                    halStatus = pI2C->TxFunPtr(&pI2C->hi2c,
                            slaveAddr, data, len);

                    if (HAL_OK == halStatus)
                    {
                        rc = RETCODE_OK;
                    }
                    else
                    {
                        rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_FAILURE);
                    }
                }
                else
                {
                    rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_FAILURE);
                }
            }
        }
        else
        if ((0 == len) && ((uint32_t) pI2C->hi2c.State & (uint32_t) HAL_I2C_STATE_BUSY))
        {
            /* Need to cancel running transfer, but STM32 HAL does not provide
             * functions to abort running transfer. This may change in future
             * versions of STMCUBE32.
             */
            rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NOT_SUPPORTED);
        }
    }
    return rc;
}

/** See description in the interface declaration */
/** todo implement cancel for receiving */
Retcode_T MCU_I2C_Receive(I2C_T i2c, uint16_t slaveAddr,
        uint8_t * buffer, uint32_t len)
{
    Retcode_T rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    struct MCU_I2C_S* pI2C = (struct MCU_I2C_S*) i2c;
    if (pI2C)
    {
        if (buffer && len)
        {
            /* Need to receive something but first check whether len is in
             * uint16_t range because uint32_t receives are not supported by
             * the STM driver
             */
            if (UINT16_MAX < len)
            {
                rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NOT_SUPPORTED);
            }
            else
            {
                if (I2C_ADDRESSINGMODE_7BIT == pI2C->hi2c.Init.AddressingMode)
                {
                    slaveAddr = slaveAddr << 1; /**< 7bit Adressing Mode */
                }

                if (HAL_OK == HAL_I2C_IsDeviceReady(&pI2C->hi2c, slaveAddr, TRIALS, TIMEOUT_MS))
                {
                    /* RECEIVE */
                    HAL_StatusTypeDef halStatus;

                    halStatus = pI2C->RxFunPtr(&pI2C->hi2c,
                            slaveAddr, buffer, len);

                    if (HAL_OK == halStatus)
                    {
                        rc = RETCODE_OK;
                    }
                    else
                    {
                        rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_FAILURE);
                    }
                }
                else
                {
                    rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_FAILURE);
                }
            }
        }
        else
        if ((0 == len) && ((uint32_t) pI2C->hi2c.State & (uint32_t) HAL_I2C_STATE_BUSY))
        {
            /* Cancel an eventually running transfer, however the  STM32 HAL
             * does not provide functions to abort a running transfer. This
             * may change in future versions of STM32Cube.
             */
            rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NOT_SUPPORTED);
        }
    }
    return rc;
}

/** See description in the interface declaration */
Retcode_T MCU_I2C_ReadRegister(I2C_T i2c, uint16_t slaveAddr, uint8_t registerAddr,
        uint8_t * rxBuffer, uint32_t rxLen)
{
    Retcode_T rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    struct MCU_I2C_S* pI2C = (struct MCU_I2C_S*) i2c;
    if (pI2C)
    {
        if (rxBuffer && rxLen)
        {
            /* Need to receive something but first check whether len is in
             * uint16_t range because uint32_t receives are not supported by
             * the STM driver
             */
            if (UINT16_MAX < rxLen)
            {
                rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NOT_SUPPORTED);
            }
            else
            {
                if (I2C_ADDRESSINGMODE_7BIT == pI2C->hi2c.Init.AddressingMode)
                {
                    slaveAddr = slaveAddr << 1; /**< 7bit Adressing Mode */
                }

                if (HAL_OK == HAL_I2C_IsDeviceReady(&pI2C->hi2c, slaveAddr, TRIALS, TIMEOUT_MS))
                {
                    /* RECEIVE */
                    HAL_StatusTypeDef halStatus;

                    rxBuffer[0] = registerAddr;

                    rc = MCU_I2C_Send(i2c, slaveAddr, rxBuffer, 1);

                    if (RETCODE_OK == rc)
                    {
                        halStatus = pI2C->RxFunPtr(&pI2C->hi2c,
                                slaveAddr, rxBuffer, rxLen);

                        if (HAL_OK == halStatus)
                        {
                            rc = RETCODE_OK;
                        }
                        else
                        {
                            rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_FAILURE);
                        }
                    }
                    else
                    {
                        rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_FAILURE);
                    }
                }
                else
                {
                    rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_FAILURE);
                }
            }
        }
        else if ((0 == rxLen) && ((uint32_t) pI2C->hi2c.State & (uint32_t) HAL_I2C_STATE_BUSY))
        {
            /* Cancel an eventually running transfer, however the  STM32 HAL
             * does not provide functions to abort a running transfer. This
             * may change in future versions of STM32Cube.
             */
            rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NOT_SUPPORTED);
        }
        else
        {
            rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_FAILURE);
        }
    }
    return rc;

}

/** See description in the interface declaration */
Retcode_T MCU_I2C_WriteRegister(I2C_T i2c, uint16_t slaveAddr, uint8_t registerAddr,
        uint8_t * txData, uint32_t txLen)
{
    Retcode_T rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    struct MCU_I2C_S* pI2C = (struct MCU_I2C_S*) i2c;
    if (pI2C)
    {

        if (txData && txLen)
        {
            if (UINT16_MAX < txLen)
            {
                rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NOT_SUPPORTED);
            }
            else
            {
                if (I2C_ADDRESSINGMODE_7BIT == pI2C->hi2c.Init.AddressingMode)
                {
                    slaveAddr = slaveAddr << 1; /**< 7bit Adressing Mode */
                }

                if (HAL_OK == HAL_I2C_IsDeviceReady(&pI2C->hi2c, slaveAddr, TRIALS, TIMEOUT_MS))
                {
                    /* SEND */
                    HAL_StatusTypeDef halStatus;

                    uint8_t Buf[txLen + 1];
                    uint8_t i;

                    Buf[0] = registerAddr;
                    for (i = 0; i < txLen; i++)
                    {
                        Buf[i + 1] = *(txData + i);
                    }

                    halStatus = pI2C->TxFunPtr(&pI2C->hi2c,
                            slaveAddr, Buf, txLen + 1);

                    if (HAL_OK == halStatus)
                    {
                        rc = RETCODE_OK;
                    }
                    else
                    {
                        rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_FAILURE);
                    }
                }
                {
                    rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_FAILURE);
                }
            }
        }
        else if ((0 == txLen) && ((uint32_t) pI2C->hi2c.State & (uint32_t) HAL_I2C_STATE_BUSY))
        {
            /* Cancel an eventually running transfer, however the  STM32 HAL
             * does not provide functions to abort a running transfer. This
             * may change in future versions of STM32Cube.
             */
            rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NOT_SUPPORTED);
        }
        else
        {
            rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_FAILURE);
        }
    }
    return rc;
}

/**
 * @brief Callback function for I2C error events
 * @details This function overrides the weak definition provided by the STM32
 * HAL and is called upon every error interrupt request occurring on a
 * particular I2C interface.
 * @param hi2c is the STM32 HAL handle of the interface that reports the error
 */
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
	struct MCU_I2C_S* pI2C = (struct MCU_I2C_S*) hi2c;
    if (pI2C)
    {
        /* Has a valid handle, now process the event */
        /* First read the error value from the I2C device. This value is
         * bit coded.
         */
        uint32_t errorValue = HAL_I2C_GetError(hi2c);
        /* Then create the return value for the callback to the application if
         * an callback pointer is assigned.
         */
        if ((HAL_I2C_ERROR_NONE != errorValue) && pI2C->AppLayerCallback)
        {
            struct MCU_I2C_Event_S event = { 0, 0, 0, 0 };
            /* Set the error bit in the event and signal it to application */
            event.TransferError = 1;
            pI2C->AppLayerCallback((I2C_T) pI2C, event);
        }
    }
}

/**
 * This section contains callback functions that override the STM32 HAL
 * default callback implementations.
 */

/**
 * @brief Callback function for I2C master TX complete events.
 * @details This function overrides the weak definition provided by the STM32
 * HAL and is called when a Tx transfer is completed.
 * @param hi2c is the STM32 HAL handle of the interface that reports the event
 */
void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    wTransferState = TRANSFER_COMPLETE;
    struct MCU_I2C_S* pI2C = (struct MCU_I2C_S*) hi2c;
    if (pI2C && pI2C->AppLayerCallback)
    {
        /* Has a valid handle, now process the event */
        struct MCU_I2C_Event_S event = { 0, 0, 0, 0 };
        /* Signal that transfer is complete to application */
        event.TxComplete = 1;
        pI2C->AppLayerCallback((I2C_T) pI2C, event);
    }
}

/**
 * @brief Callback function for I2C master RX complete events.
 * @details This function overrides the weak definition provided by the STM32
 * HAL and is called when a Rx transfer is completed.
 * @param hi2c is the STM32 HAL handle of the interface that reports the event
 */
void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    wTransferState = TRANSFER_COMPLETE;
    struct MCU_I2C_S* pI2C = (struct MCU_I2C_S*) hi2c;
    if (pI2C && pI2C->AppLayerCallback)
    {
        /* Has a valid handle, now process the event */
        struct MCU_I2C_Event_S event = { 0, 0, 0, 0 };
        /* Signal that transfer is complete to application */
        event.RxComplete = 1;
        pI2C->AppLayerCallback((I2C_T) pI2C, event);
    }
}

#endif //-- BCDS_FEATURE_I2C && BCDS_I2C_COUNT

