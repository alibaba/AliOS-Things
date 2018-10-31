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
 * Handles MCU STM32 UART driver encapsulation
 */
#include "BCDS_Essentials.h"
#undef BCDS_MODULE_ID
#define BCDS_MODULE_ID BCDS_ESSENTIALS_MODULE_ID_UART

#include "BCDS_MCU_UART.h"
#include "BCDS_MCU_UART_Handle.h"

#if BCDS_FEATURE_UART && BCDS_UART_COUNT

#define MCU_UART_INVALID_INDEX (0x2BAD)

static const struct MCU_UART_Event_S eventRxError =
{   1, 0, 0, 0, 0, 0, 0, 0, 0};
static const struct MCU_UART_Event_S eventRxComplete =
{   0, 1, 0, 0, 0, 0, 0, 0, 0};
static const struct MCU_UART_Event_S eventTxComplete =
{   0, 0, 0, 1, 0, 0, 0, 0, 0};

#define COUNT_OF_ELEMENTS(array) (sizeof(array) / sizeof(*(array))) /**< helper to get array size */

static void InitElement(struct MCU_UART_S* uart_ptr);
static void DeinitElement(struct MCU_UART_S* uart_ptr);

static Retcode_T MapHalRetToMcuRet(HAL_StatusTypeDef halRet);

static void MCU_UART_IRQ_Callback(UART_T uart);
static void MCU_UART_DMA_RX_Callback(UART_T uart);
static void MCU_UART_DMA_TX_Callback(UART_T uart);

/* ----- ISR -------------------------------------------------------- */

/**
 * @brief       Callback invoked by BSP in case UART IRQ
 * @note        IRQs are logically linked by an "or" function.
 *              So this call might be caused by several reasons.
 *              These reason are evaluated inside HAL_UART_IRQHandler
 * @param[in]   uart: Pointer to UART_HandleTypeDef as integer which is first element of struct MCU_UART_S
 */

static void MCU_UART_IRQ_Callback(UART_T uart)
{
    struct MCU_UART_S* uart_ptr = (struct MCU_UART_S*) uart;
    HAL_UART_IRQHandler(&uart_ptr->huart);
}

/**
 * @brief      IRQ handler for DMA RX called by BSP
 * @param      uart: Pointer to UART_HandleTypeDef as integer which is first element of struct MCU_UART_S
 */
static void MCU_UART_DMA_RX_Callback(UART_T uart)
{
    struct MCU_UART_S* uart_ptr = (struct MCU_UART_S*) uart;
    HAL_DMA_IRQHandler(uart_ptr->huart.hdmarx);
}

/**
 * @brief      IRQ handler for DMA TX called by BSP
 * @param      uart: Pointer to UART_HandleTypeDef as integer which is first element of struct MCU_UART_S
 */
static void MCU_UART_DMA_TX_Callback(UART_T uart)
{
    struct MCU_UART_S* uart_ptr = (struct MCU_UART_S*) uart;
    HAL_DMA_IRQHandler(uart_ptr->huart.hdmatx);
}

/**
 * @brief   Tx Transfer completed callback.
 * @note    MCU UART implementation of drivers (STM32Cube) weak callback
 * @param   huart: UART handle.
 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    struct MCU_UART_S* uart_ptr = (struct MCU_UART_S*) huart;
    uart_ptr->isSending = false;
    uart_ptr->AppCallback((HWHandle_T) uart_ptr, eventTxComplete);
}

/**
 * @brief   This is the RX callback function which is invoked by the stm32 uart
 * driver.
 *
 * @note    MCU UART implementation of drivers (STM32Cube) weak callback
 *
 * @param   huart: UART handle.
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    struct MCU_UART_S* uart_ptr = (struct MCU_UART_S*) huart;
    HAL_StatusTypeDef status;

    uart_ptr->AppCallback((UART_T) uart_ptr, eventRxComplete);
    if(BCDS_HAL_TRANSFER_MODE_INTERRUPT == uart_ptr->TransferMode)
    {
        status = HAL_UART_Receive_IT(&uart_ptr->huart, uart_ptr->RxUserData_ptr, uart_ptr->RxUserSize);
        Retcode_T retcode = MapHalRetToMcuRet(status);

        if (RETCODE_OK != retcode)
        {
            Retcode_RaiseErrorFromIsr(retcode);
        }
    }
    else
    {
        /* Do nothing since it is configured in circular mode */
    }
}

/**
 * @brief   Rx Half Transfer completed callback
 * @note    MCU UART implementation of drivers (STM32Cube) weak callback
 * @param   huart: UART handle
 */
void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart)
{
    /* silent ignore. No event defined. */
    BCDS_UNUSED(huart);
}

/**
 * @brief   UART error callback
 * @note    MCU UART implementation of drivers (STM32Cube) weak callback
 * @param   huart: UART handle
 */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    /* todo: proper translation of the error events from stm32 to our events */

    struct MCU_UART_S* uart_ptr = (struct MCU_UART_S*) huart;

    /* The MCU does not detect TX errors */
    if ((uint32_t)HAL_UART_ERROR_DMA & uart_ptr->huart.ErrorCode)
    {
        /* todo: Need to implement special handling for DMA error */
        Retcode_RaiseErrorFromIsr(RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_UART_DMA_ERROR));
    }
    else
    {
        uart_ptr->AppCallback((HWHandle_T) uart_ptr, eventRxError);

        /* In case of overrun error, the Interrupt is currently being disabled.
         * HAL currently doesn't support propagating this to the application.
         * Not sure if enabling the interrupt will load the interface resulting
         * in recursive errors. Understanding is that this will . */
        if((uint32_t)HAL_UART_ERROR_ORE & uart_ptr->huart.ErrorCode)
        {
            if(BCDS_HAL_TRANSFER_MODE_INTERRUPT == uart_ptr->TransferMode)
            {
                HAL_StatusTypeDef status = HAL_UART_Receive_IT(&uart_ptr->huart, uart_ptr->RxUserData_ptr, uart_ptr->RxUserSize);
                Retcode_T retcode = MapHalRetToMcuRet(status);

                if (RETCODE_OK != retcode)
                {
                    Retcode_RaiseErrorFromIsr(retcode);
                }
            }
        }
    }
}

/**
 * @brief   Tx Half Transfer completed callback
 * @note    MCU UART implementation of drivers (STM32Cube) weak callback
 * @param   huart: UART handle.
 */
void HAL_UART_TxHalfCpltCallback(UART_HandleTypeDef *huart)
{
    /* silent ignore. No event defined. */
    BCDS_UNUSED(huart);
}

/** @brief See public interface function description in BCDS_MCU_UART.h */
Retcode_T MCU_UART_Initialize(UART_T uart, MCU_UART_Callback_T callback)
{
    struct MCU_UART_S* uart_ptr = (struct MCU_UART_S*) uart;
    if (NULL == uart_ptr || NULL == callback)
    {
        return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    }

    InitElement(uart_ptr);

    /* first: set application callback */
    uart_ptr->AppCallback = callback; /* invoked by MCU, callback of application */
    /* second: Add callback function pointers so that BSP is able to callback this
     * module when IRQs are pending. */
    uart_ptr->IrqCallback = MCU_UART_IRQ_Callback; /* invoked by BSP if uart in interrupt mode, callback of MCU */
    uart_ptr->DmaRxCallback = MCU_UART_DMA_RX_Callback; /* invoked by BSP if uart in DMA mode, callback of MCU */
    uart_ptr->DmaTxCallback = MCU_UART_DMA_TX_Callback; /* invoked by BSP if uart in DMA mode, callback of MCU */

    return RETCODE_OK;
}

/** @brief See public interface function description in BCDS_MCU_UART.h */
Retcode_T MCU_UART_Deinitialize(UART_T uart)
{
    struct MCU_UART_S* uart_ptr = (struct MCU_UART_S*) uart;
    if (NULL == uart_ptr)
    {
        return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    }

    /* disable interrupts from BSP for this uart */
    uart_ptr->IrqCallback = NULL;
    uart_ptr->DmaRxCallback = NULL;
    uart_ptr->DmaTxCallback = NULL;
    DeinitElement(uart_ptr);

    return RETCODE_OK;
}

/** @brief See public interface function description in BCDS_MCU_UART.h */
Retcode_T MCU_UART_Send(UART_T uart, uint8_t * data, uint32_t len)
{
    struct MCU_UART_S* uart_ptr = (struct MCU_UART_S*) uart;

    if (NULL == uart_ptr || NULL == data || 0 == len || UINT16_MAX < len)
    {
        return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    }

    if (uart_ptr->isSending)
    {
        return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_UART_HAL_BUSY);
    }

    uart_ptr->isSending = true;

    HAL_StatusTypeDef status;
    Retcode_T retcode = RETCODE_OK;

    switch (uart_ptr->TransferMode)
    {
        case BCDS_HAL_TRANSFER_MODE_BLOCKING:
        retcode = RETCODE(RETCODE_SEVERITY_FATAL, RETCODE_NOT_SUPPORTED);
        break;

        case BCDS_HAL_TRANSFER_MODE_INTERRUPT:
        status = HAL_UART_Transmit_IT(&uart_ptr->huart, data, len);
        retcode = MapHalRetToMcuRet(status);
        break;

        case BCDS_HAL_TRANSFER_MODE_DMA:
        status = HAL_UART_Transmit_DMA(&uart_ptr->huart, data, len);
        retcode = MapHalRetToMcuRet(status);
        break;

        default:
        retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INCONSITENT_STATE);
        break;
    }

    if ( RETCODE_OK != retcode)
    {
        uart_ptr->isSending = false;
    }

    return retcode;
}

/** @brief See public interface function description in BCDS_MCU_UART.h */
Retcode_T MCU_UART_Receive(UART_T uart, uint8_t *data, uint32_t len)
{
    struct MCU_UART_S* uart_ptr = (struct MCU_UART_S*) uart;
    HAL_StatusTypeDef status;

    if(0UL == len) /* This is a abort request */
    {
        if(true == uart_ptr->isReceiving)
        {
            if(((BCDS_HAL_TRANSFER_MODE_INTERRUPT == uart_ptr->TransferMode) ||
                            (BCDS_HAL_TRANSFER_MODE_DMA == uart_ptr->TransferMode)))
            {
                /* HAL_UART_AbortReceive_IT will internally validate for DMA as well */
                if ( HAL_OK == (status = HAL_UART_AbortReceive_IT(&uart_ptr->huart)))
                {
                    uart_ptr->isReceiving = false;
                    uart_ptr->RxUserData_ptr = NULL;
                    uart_ptr->RxUserSize = 0UL;
                }
            }
            else
            {
                /* BCDS_HAL_TRANSFER_MODE_BLOCKING (and garbage) is not supported */
                return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NOT_SUPPORTED);
            }
        }
        else
        {
            /* Receive was never successfully triggered prior */
            return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INCONSITENT_STATE);
        }
    }
    else
    {
        uart_ptr->RxUserData_ptr = data;
        uart_ptr->RxUserSize = len;

        switch (uart_ptr->TransferMode)
        {
            case BCDS_HAL_TRANSFER_MODE_BLOCKING:
            return RETCODE(RETCODE_SEVERITY_FATAL, RETCODE_NOT_SUPPORTED);

            case BCDS_HAL_TRANSFER_MODE_INTERRUPT:
            status = HAL_UART_Receive_IT(&uart_ptr->huart, uart_ptr->RxUserData_ptr, uart_ptr->RxUserSize);
            break;

            case BCDS_HAL_TRANSFER_MODE_DMA:
            status = HAL_UART_Receive_DMA(&uart_ptr->huart, uart_ptr->RxUserData_ptr, uart_ptr->RxUserSize);
            break;

            default:
            return RETCODE(RETCODE_SEVERITY_FATAL, RETCODE_INCONSITENT_STATE);
        }
    }

    Retcode_T retcode = MapHalRetToMcuRet(status);

    if (RETCODE_OK == retcode)
    {
        uart_ptr->isReceiving = true;
    }

    return retcode;
}

/**
 * @brief       Removes handle from array if found
 * @param[in]   Handle_ptr
 * @retval RETCODE_OK only if successful
 * @retval RETCODE_INVALID_PARAM if not found
 */

static void DeinitElement(struct MCU_UART_S* uart_ptr)
{
    /* Note: Lock in MCU_UART_Deinitialize */
    /* don't touch BSP stuff */
    uart_ptr->AppCallback = NULL;
    uart_ptr->isSending = false;
    uart_ptr->isReceiving = false;
    uart_ptr->RxUserSize = 0;
    uart_ptr->RxUserData_ptr = NULL;
}

/**
 * @brief       Store handle in array if free position is found
 * @param[in]   Handle_ptr
 * @param[out]  Element_pptr
 * @retval RETCODE_OK only if successful
 * @retval RETCODE_OUT_OF_RESOURCES if not found and no free element available to store it
 */
static void InitElement(struct MCU_UART_S* uart_ptr)
{
    DeinitElement(uart_ptr); /* ensure that repeated call from app does not cause same handle stored more than once */
    if (NULL != uart_ptr->huart.Instance)
    {
        uart_ptr->AppCallback = NULL;
        uart_ptr->isSending = false;
        uart_ptr->isReceiving = false;
        uart_ptr->RxUserSize = 0;
        uart_ptr->RxUserData_ptr = NULL;
    }
}

/**
 * @brief Mapper for HAL function return values
 * @param Vendor driver return code
 */
static Retcode_T MapHalRetToMcuRet(HAL_StatusTypeDef halRet)
{
    Retcode_T ret;
    switch (halRet)
    {
        case HAL_OK:
        ret = RETCODE_OK;
        break;
        case HAL_BUSY:
        ret = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_UART_HAL_BUSY);
        break;
        case HAL_TIMEOUT:
        ret = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_UART_HAL_TIMEOUT);
        break;
        case HAL_ERROR:
        /* Fall Through */
        default:
        ret = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_UART_HAL_ERROR);
        break;
    }
    return ret;
}

#endif /* BCDS_FEATURE_UART && UART_COUNT */
