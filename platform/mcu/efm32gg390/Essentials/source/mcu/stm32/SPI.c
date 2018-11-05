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
 *
 * @brief
 * Handles MCU STM32 SPI driver encapsulation
 */

#include "BCDS_Essentials.h"
#undef BCDS_MODULE_ID
#define BCDS_MODULE_ID BCDS_ESSENTIALS_MODULE_ID_SPI

#include "BCDS_MCU_SPI.h"
#include "BCDS_MCU_SPI_Handle.h"

#if BCDS_FEATURE_SPI && BCDS_SPI_COUNT

/* The STM32Cube Library provides a HAL driver for SPI I/O operations.
 * However depending on the configured transfer mode (Polling, Interrupt or DMA)
 * the developer has to call different functions (see stm32l4xx_hal_spi.h).
 * Only the function signatures for IRQ and DMA transfer are the same. The
 * polling functions are introducing an additional parameter for a timeout.
 *
 * The plan is to provide function pointers on these functions (at least for
 * IRQ and DMA mode) and to set them according to the configuration of the SPI
 * interface upon initialization.
 */

void MCU_BSP_SPI_IRQ_Callback(SPI_T spi);
void MCU_BSP_SPI_DMA_TX_Callback(SPI_T spi);
void MCU_BSP_SPI_DMA_RX_Callback(SPI_T spi);



/** See description in the interface declaration */
Retcode_T MCU_SPI_Initialize(SPI_T spi, MCU_SPI_Callback_T callback)
{
    Retcode_T rc = RETCODE(RETCODE_SEVERITY_WARNING, RETCODE_INVALID_PARAM);

    struct MCU_SPI_S* pSPI = (struct MCU_SPI_S*) spi;

    if (pSPI && callback)
    {

        if (BCDS_HAL_TRANSFER_MODE_BLOCKING == pSPI->TransferMode)
        {
            /* this mode is currently not supported */
            rc = RETCODE(RETCODE_SEVERITY_FATAL, RETCODE_NOT_SUPPORTED);
        }
        else if (BCDS_HAL_TRANSFER_MODE_INTERRUPT == pSPI->TransferMode)
        {
        	pSPI->IRQCallback = MCU_BSP_SPI_IRQ_Callback;
        	pSPI->DmaTxCallback = NULL;
        	pSPI->DmaRxCallback = NULL;
        	pSPI->AppCallback = callback;

        	pSPI->TxFunPtr = HAL_SPI_Transmit_IT;
        	pSPI->RxFunPtr = HAL_SPI_Receive_IT;
        	pSPI->TransferFunPtr = HAL_SPI_TransmitReceive_IT;

            rc = RETCODE_OK;
        }
        else if (BCDS_HAL_TRANSFER_MODE_DMA == pSPI->TransferMode)
        {
        	pSPI->IRQCallback = MCU_BSP_SPI_IRQ_Callback;
        	pSPI->DmaTxCallback = MCU_BSP_SPI_DMA_TX_Callback;
        	pSPI->DmaRxCallback = MCU_BSP_SPI_DMA_RX_Callback;
        	pSPI->AppCallback = callback;

        	pSPI->TxFunPtr = HAL_SPI_Transmit_DMA;
        	pSPI->RxFunPtr = HAL_SPI_Receive_DMA;
        	pSPI->TransferFunPtr = HAL_SPI_TransmitReceive_DMA;

            rc = RETCODE_OK;
        }
        else
        {
            /* all other modes are currently not supported */
            rc = RETCODE(RETCODE_SEVERITY_WARNING, RETCODE_NOT_SUPPORTED);
        }
    }

    return rc;
}


/** See description in the interface declaration */
Retcode_T MCU_SPI_Deinitialize(SPI_T spi)
{
    /* Here we are checking if the passed SPI handle is valid and if it
     * is valid we mark our local interface context for that handle as being
     * free again.
     */
    Retcode_T rc = RETCODE(RETCODE_SEVERITY_WARNING, RETCODE_INVALID_PARAM);
    struct MCU_SPI_S* pSPI = (struct MCU_SPI_S*) spi;
    if (pSPI)
    {
        /* Seems to be valid and initialized, now remove it from the
         * SPI interface array
         */
    	pSPI->IRQCallback = NULL;
    	pSPI->DmaTxCallback = NULL;
    	pSPI->DmaRxCallback = NULL;
    	pSPI->AppCallback = NULL;
    	pSPI->TxFunPtr = NULL;
    	pSPI->RxFunPtr = NULL;
    	pSPI->TransferFunPtr = NULL;

        rc = RETCODE_OK;
    }
    return rc;
}

/** See description in the interface declaration */
Retcode_T MCU_SPI_Send(SPI_T spi, uint8_t * data, uint32_t len)
{
    Retcode_T rc = RETCODE(RETCODE_SEVERITY_WARNING, RETCODE_INVALID_PARAM);
    struct MCU_SPI_S* pSPI = (struct MCU_SPI_S*) spi;
    if (pSPI)
    {
        if (data && len)
        {
            /* Need to send something but first check whether len is in uint16_t
             * range because uint32_t sends are not supported by the STM driver
             */
            if (UINT16_MAX < len)
            {
                rc = RETCODE(RETCODE_SEVERITY_WARNING, RETCODE_NOT_SUPPORTED);
            }
            else
            {
                HAL_StatusTypeDef halStatus;
                halStatus = pSPI->TxFunPtr(&pSPI->hspi, data, len);
                if (HAL_OK == halStatus)
                {
                    rc = RETCODE_OK;
                }
                else
                {
                    rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_FAILURE);
                }
            }
        }
        else
        {
            rc = RETCODE(RETCODE_SEVERITY_WARNING, RETCODE_NOT_SUPPORTED);
        }
    }
    return rc;
}

/** See description in the interface declaration */
Retcode_T MCU_SPI_Receive(SPI_T spi, uint8_t * buffer, uint32_t len)
{
    Retcode_T rc = RETCODE(RETCODE_SEVERITY_WARNING, RETCODE_INVALID_PARAM);
    struct MCU_SPI_S* pSPI = (struct MCU_SPI_S*) spi;
    if (pSPI)
    {
        if (buffer && len)
        {
            /* Need to send something but first check whether len is in uint16_t
             * range because uint32_t sends are not supported by the STM driver
             */
            if (UINT16_MAX < len)
            {
                rc = RETCODE(RETCODE_SEVERITY_WARNING, RETCODE_NOT_SUPPORTED);
            }
            else
            {
                /*
                 * Before starting the transfer, it is strongly recommended to flush the rx fifo.
                 * Otherwise the DMA rx might read the data that was still stored in the rx fifo
                 * from previous SPI bus communication (like previous send calls). - bnl8fe
                 */
                volatile uint32_t dummy = 0;
                BCDS_UNUSED(dummy);
                while ((pSPI->hspi.Instance->SR & SPI_FLAG_RXNE) == SPI_FLAG_RXNE)
                {
                    dummy = pSPI->hspi.Instance->DR;
                }

                HAL_StatusTypeDef halStatus;
                halStatus = pSPI->RxFunPtr(&pSPI->hspi, buffer, len);
                if (HAL_OK == halStatus)
                {
                    rc = RETCODE_OK;
                }
                else
                {
                    rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_FAILURE);
                }
            }
        }
        else
        {
            rc = RETCODE(RETCODE_SEVERITY_WARNING, RETCODE_NOT_SUPPORTED);
        }
    }
    return rc;
}

/** See description in the interface declaration */
Retcode_T MCU_SPI_Transfer(SPI_T spi, uint8_t* data_out, uint8_t* data_in, uint32_t numTransfer)
{
    Retcode_T rc = RETCODE(RETCODE_SEVERITY_WARNING, RETCODE_INVALID_PARAM);
    struct MCU_SPI_S* pSPI = (struct MCU_SPI_S*) spi;
    if (pSPI)
    {
        if (data_out && data_in && numTransfer)
        {
            /* Need to send something but first check whether len is in uint16_t
             * range because uint32_t sends are not supported by the STM driver
             */
            if (UINT16_MAX < numTransfer)
            {
                rc = RETCODE(RETCODE_SEVERITY_WARNING, RETCODE_NOT_SUPPORTED);
            }
            else
            {
                /*
                 * Before starting the transfer, it is strongly recommended to flush the rx fifo.
                 * Otherwise the DMA rx might read the data that was still stored in the rx fifo
                 * from previous SPI bus communication (like previous send calls). - bnl8fe
                 */
                volatile uint32_t dummy = 0;
                BCDS_UNUSED(dummy);
                while ((pSPI->hspi.Instance->SR & SPI_FLAG_RXNE) == SPI_FLAG_RXNE)
                {
                    dummy = pSPI->hspi.Instance->DR;
                }

                HAL_StatusTypeDef halStatus;
                halStatus = pSPI->TransferFunPtr(&pSPI->hspi, data_out, data_in, numTransfer);
                if (HAL_OK == halStatus)
                {
                    rc = RETCODE_OK;
                }
                else
                {
                    rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_FAILURE);
                }
            }
        }
        else
        {
            rc = RETCODE(RETCODE_SEVERITY_WARNING, RETCODE_NOT_SUPPORTED);
        }
    }
    return rc;
}

uint32_t MCU_SPI_GetDataCount(SPI_T spi)
{
    uint32_t dataCount = 0;
    struct MCU_SPI_S* pSPI = (struct MCU_SPI_S*) spi;
    if (pSPI)
    {
        HAL_SPI_StateTypeDef halState;
        halState = HAL_SPI_GetState(&pSPI->hspi);

        switch (halState)
        {
        case HAL_SPI_STATE_BUSY_TX:
            dataCount = pSPI->hspi.TxXferCount;
            break;
        case HAL_SPI_STATE_BUSY_RX:
            dataCount = pSPI->hspi.RxXferCount;
            break;
        case HAL_SPI_STATE_BUSY_TX_RX:
            dataCount = (pSPI->hspi.TxXferCount) + (pSPI->hspi.RxXferCount);
            break;
        default:
            dataCount = 0;
            break;
        }
    }
    return dataCount;
}

/** SPI IRQ callback handler which is called back
 * by the BSP SPI IRQ handler
 * @param spi is the BSP handle for the interface
 */
void MCU_BSP_SPI_IRQ_Callback(SPI_T spi)
{
	struct MCU_SPI_S* pSPI = (struct MCU_SPI_S*) spi;
    if (pSPI)
    {
        HAL_SPI_IRQHandler(&pSPI->hspi);
    }
}

/**
 * @brief      IRQ handler for DMA RX called by BSP
 * @param      dma: Handle to MCU uart (MCU_SPI_handle_S)
 * @retval     None
 */
void MCU_BSP_SPI_DMA_RX_Callback(SPI_T spi)
{
	struct MCU_SPI_S* pSPI = (struct MCU_SPI_S*) spi;
    if (pSPI)
    {
        HAL_DMA_IRQHandler(pSPI->hspi.hdmarx);
    }
}

/**
 * @brief      IRQ handler for DMA TX called by BSP
 * @param      dma: Handle to MCU uart (MCU_SPI_handle_S)
 * @retval     None
 */
void MCU_BSP_SPI_DMA_TX_Callback(SPI_T spi)
{
	struct MCU_SPI_S* pSPI = (struct MCU_SPI_S*) spi;
    if (pSPI)
    {
        HAL_DMA_IRQHandler(pSPI->hspi.hdmatx);
    }
}

/**
 * This section contains callback functions that override the STM32 HAL
 * default callback implementations.
 */

/**
 * @brief Callback function for SPI error events
 * @details This function overrides the weak definition provided by the STM32
 * HAL and is called upon every error interrupt request occurring on a
 * particular SPI interface.
 * @param hspi is the STM32 HAL handle of the interface that reports the error
 */
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
    struct MCU_SPI_S* pSPI = (struct MCU_SPI_S*) hspi;

    if (pSPI)
    {
        /* Has a valid handle, now process the event */
        struct MCU_SPI_Event_S Events = { 0, 0, 0, 0, 0, 0 };

        uint32_t HalError = HAL_SPI_GetError(hspi);
        switch (HalError)
        {
        case HAL_SPI_ERROR_MODF : /*!< MODF error                        */
            Events.DataLoss = 1;
            break;
        case HAL_SPI_ERROR_CRC : /*!< CRC error                         */
            Events.RxError = 1;
            Events.DataLoss = 1;
            break;
        case HAL_SPI_ERROR_OVR : /*!< OVR error                         */
            Events.RxError = 1;
            Events.DataLoss = 1;
            break;
        case HAL_SPI_ERROR_FRE : /*!< FRE error                         */
            Events.RxError = 1;
            Events.DataLoss = 1;
            break;
        case HAL_SPI_ERROR_DMA : /*!< DMA transfer error                */
            Events.RxError = 1;
            Events.TxError = 1;
            Events.DataLoss = 1;
            break;
        case HAL_SPI_ERROR_FLAG : /*!< Error on BSY/TXE/FTLVL/FRLVL Flag */
            Events.RxError = 1;
            Events.TxError = 1;
            Events.DataLoss = 1;
            break;
        default:
            break;
        }

        pSPI->AppCallback((SPI_T) pSPI, Events);
    }
}

/**
 * @brief Callback function for SPI TX complete events.
 * @details This function overrides the weak definition provided by the STM32
 * HAL and is called when a Tx transfer is completed.
 * @param hspi is the STM32 HAL handle of the interface that reports the event
 */
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	struct MCU_SPI_S* pSPI = (struct MCU_SPI_S*) hspi;
    if (pSPI)
    {
        /* Has a valid handle, now process the event */
        struct MCU_SPI_Event_S Events = { 0, 0, 0, 0, 0, 0 };
        Events.TxComplete = 1;

        pSPI->AppCallback((SPI_T) pSPI, Events);
    }
}

/**
 * @brief Callback function for SPI RX complete events.
 * @details This function overrides the weak definition provided by the STM32
 * HAL and is called when a Rx transfer is completed.
 * @param hspi is the STM32 HAL handle of the interface that reports the event
 */
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
	struct MCU_SPI_S* pSPI = (struct MCU_SPI_S*) hspi;
    if (pSPI)
    {
        /* Has a valid handle, now process the event */
        struct MCU_SPI_Event_S Events = { 0, 0, 0, 0, 0, 0 };
        Events.RxComplete = 1;

        pSPI->AppCallback((SPI_T) pSPI, Events);
    }
}

/**
 * @brief Callback function for SPI RX/TX complete events.
 * @details This function overrides the weak definition provided by the STM32
 * HAL and is called when a Rx/Tx transfer is completed.
 * @param hspi is the STM32 HAL handle of the interface that reports the event
 */
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
	struct MCU_SPI_S* pSPI = (struct MCU_SPI_S*) hspi;
    if (pSPI)
    {
        /* Has a valid handle, now process the event */
        struct MCU_SPI_Event_S Events = { 0, 0, 0, 0, 0, 0 };
        Events.RxComplete = 1;
        Events.TxComplete = 1;

        pSPI->AppCallback((SPI_T) pSPI, Events);
    }
}

#endif //-- BCDS_FEATURE_SPI
