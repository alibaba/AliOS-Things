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
 * Handles MCU EFM32 SPI driver encapsulation
 */

#include "BCDS_Essentials.h"
#undef BCDS_MODULE_ID
#define BCDS_MODULE_ID BCDS_ESSENTIALS_MODULE_ID_SPI

#include "BCDS_HAL.h"

#include "BCDS_Assert.h"
#include "BCDS_MCU_SPI.h"
#include "BCDS_MCU_SPI_Handle.h"
#include "BCDS_MCU_DMA_Handle.h"

#if BCDS_FEATURE_SPI && BCDS_SPI_COUNT

static Retcode_T SPI_Abort(struct MCU_SPI_Handle_S* spi_ptr);

static Retcode_T SPI_TransferIntMode(struct MCU_SPI_Handle_S* spi_ptr, uint8_t* dataOut, uint8_t* dataIn, uint32_t dataLength);
static Retcode_T SPI_SendIntMode(struct MCU_SPI_Handle_S* spi_ptr, uint8_t* dataOut, uint32_t dataLength);
static Retcode_T SPI_ReceiveIntMode(struct MCU_SPI_Handle_S* spi_ptr, uint8_t* dataIn, uint32_t dataLength);

static Retcode_T SPI_TransferDmaMode(struct MCU_SPI_Handle_S* spi_ptr, uint8_t* dataOut, uint8_t* dataIn, uint32_t dataLength);
static Retcode_T SPI_SendDmaMode(struct MCU_SPI_Handle_S* spi_ptr, uint8_t* dataOut, uint32_t dataLength);
static Retcode_T SPI_ReceiveDmaMode(struct MCU_SPI_Handle_S* spi_ptr, uint8_t* dataIn, uint32_t dataLength);

static Retcode_T SPI_TransferDefault(struct MCU_SPI_Handle_S* spi_ptr, uint8_t* dataOut, uint8_t* dataIn, uint32_t dataLength);
static Retcode_T SPI_SendDefault(struct MCU_SPI_Handle_S* spi_ptr, uint8_t* dataOut, uint32_t dataLength);
static Retcode_T SPI_ReceiveDefault(struct MCU_SPI_Handle_S* spi_ptr, uint8_t* dataIn, uint32_t dataLength);

/*************************** CALLBACKS *****************************/

/*
 * This function will be registered as the ISR for the receive interrupt handler
 */
void MCU_SPI_IRQ_RX_Callback(SPI_T spi)
{
    struct MCU_SPI_Handle_S* spi_ptr = (struct MCU_SPI_Handle_S*) spi;
    union MCU_SPI_Event_U event;
    USART_TypeDef* spiInstance = spi_ptr->Instance;
    uint32_t spiIntFlags = USART_IntGetEnabled(spiInstance);
    event.registerValue = 0;
    /* There's still space in the receive buffer */
    if ((spiIntFlags & USART_IF_RXDATAV))
    {
        spi_ptr->pRxBuffPtr[spi_ptr->XferSize - spi_ptr->RxXferCount] = spiInstance->RXDATA;
        spi_ptr->RxXferCount--;
        /* Check for final char event */
        if (!spi_ptr->RxXferCount)
        {   
            spiInstance->IEN &= ~(USART_IEN_RXDATAV | USART_IEN_PERR | USART_IEN_FERR | USART_IEN_RXOF);
            spiInstance->IFC = (USART_IFC_FERR | USART_IFC_PERR | USART_IFC_RXFULL | USART_IFC_RXOF);
            event.bitfield.RxComplete = 1;
            if (SPI_STATE_TX_RX == spi_ptr->State)
            {
                event.bitfield.TxComplete = 1;
            }
            spi_ptr->State = SPI_STATE_READY;
        }

    }
    /* Determine the source of the interrupt */
    if (spiIntFlags & (USART_IF_PERR | USART_IF_FERR | USART_IF_RXOF))
    {
        /* Parity error has occurred, and we are notifying. */
        spiInstance->IFC = (USART_IFC_PERR | USART_IF_FERR | USART_IF_RXOF);
        event.bitfield.RxError = 1;
    }
    if (event.registerValue)
    {
        spi_ptr->AppCallback(spi, event.bitfield);
    }
}

void MCU_SPI_DMA_RX_Callback(SPI_T spi)
{
    struct MCU_SPI_Handle_S* spi_ptr = (struct MCU_SPI_Handle_S*) spi;
    union MCU_SPI_Event_U event;
    event.registerValue = 0;
    event.bitfield.RxComplete = 1;
    if (SPI_STATE_TX_RX == spi_ptr->State)
    {
        event.bitfield.TxComplete = 1;
    }
    spi_ptr->AppCallback(spi, event.bitfield);
    spi_ptr->State = SPI_STATE_READY;
}

/*
 * This function will be registered as the ISR for the transmit interrupt handler
 */
void MCU_SPI_IRQ_TX_Callback(SPI_T spi)
{
    struct MCU_SPI_Handle_S* spi_ptr = (struct MCU_SPI_Handle_S*) spi;
    union MCU_SPI_Event_U event;
    USART_TypeDef* spiInstance = spi_ptr->Instance;
    /*get the interrupt flags that are pending */
    uint32_t spiIntFlags = USART_IntGetEnabled(spiInstance);
    if ((spiIntFlags & USART_IF_TXBL))
    {
        /* the tx buffer is low -so put same data if we have */
        if (spi_ptr->TxXferCount)
        {
            /* Clearing the unwanted Tx complete Interrupt flag befor data transmit*/
            USART_IntClear(spiInstance, USART_IFC_TXC);
            if (SPI_STATE_RX == spi_ptr->State)
            {
                spiInstance->TXDATA = spi_ptr->TxDefaultData;
            }
            else
            {
                spiInstance->TXDATA = spi_ptr->pTxBuffPtr[spi_ptr->XferSize - spi_ptr->TxXferCount];
            }
            spi_ptr->TxXferCount--;
            if (0 == spi_ptr->TxXferCount)
            {
                if (SPI_STATE_TX == spi_ptr->State)
                {
                    /* Last byte has been put in TX, set up TXC interrupt */
                    USART_IntDisable(spiInstance, USART_IEN_TXBL);
                    USART_IntEnable(spiInstance,  USART_IEN_TXC);
                }
            }
        }
        else
        {
            /* we shall never come here but for security disable the interrupt */
            USART_IntDisable(spiInstance, USART_IEN_TXBL);
            USART_IntDisable(spiInstance, USART_IEN_TXC);
        }
    }

    if ((spiIntFlags & USART_IF_TXC))
    {
        event.registerValue = 0;
        /* Last byte has been completely transmitted. Stop the procedure */
        USART_IntDisable(spiInstance, USART_IEN_TXC);
        USART_IntClear(spiInstance, USART_IFC_TXC);

        if (SPI_STATE_TX == spi_ptr->State )
        {
            event.bitfield.TxComplete = 1;
            spi_ptr->State = SPI_STATE_READY;
            spi_ptr->AppCallback(spi, event.bitfield);
        }
    }

}


void MCU_SPI_DMA_TX_Callback(SPI_T spi)
{
    struct MCU_SPI_Handle_S* spi_ptr = (struct MCU_SPI_Handle_S*) spi;
    USART_TypeDef* spiInstance = spi_ptr->Instance;

    /* only in case of a write only operation */
    if (SPI_STATE_TX == spi_ptr->State )
    {
        USART_IntEnable(spiInstance, USART_IEN_TXC);
    }
}

/*
 * Calling this function should result in an error because the application
 * developer should have done sometihng wrong e.g called deinitializing SPI
 * without disabling the intended peripheral
 */
static Retcode_T SPI_TransferDefault(struct MCU_SPI_Handle_S* spi_ptr, uint8_t* dataOut, uint8_t* dataIn, uint32_t dataLength)
{
    BCDS_UNUSED(spi_ptr);
    BCDS_UNUSED(dataIn);
    BCDS_UNUSED(dataOut);
    BCDS_UNUSED(dataLength);

    return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_HAL_SPI_DRIVER_RESSOURCE_NOT_AVAILABLE);
}

/*
 * Calling this function should result in an error because the application
 * developer should have done sometihng wrong e.g called deinitializing SPI
 * without disabling the intended peripheral
 */
static Retcode_T SPI_SendDefault(struct MCU_SPI_Handle_S* spi_ptr, uint8_t* dataOut, uint32_t dataLength)
{
    BCDS_UNUSED(spi_ptr);
    BCDS_UNUSED(dataOut);
    BCDS_UNUSED(dataLength);

    return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_HAL_SPI_DRIVER_RESSOURCE_NOT_AVAILABLE);
}

/*
 * Calling this function should result in an error because the application
 * developer should have done sometihng wrong e.g called deinitializing SPI
 * without disabling the intended peripheral
 */
static Retcode_T SPI_ReceiveDefault(struct MCU_SPI_Handle_S* spi_ptr, uint8_t* dataIn, uint32_t dataLength)
{
    BCDS_UNUSED(spi_ptr);
    BCDS_UNUSED(dataIn);
    BCDS_UNUSED(dataLength);

    return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_HAL_SPI_DRIVER_RESSOURCE_NOT_AVAILABLE);
}
/*************************** CALLBACKS END *************************/

/*************************** INTERFACE FUNCTIONS *******************/

/** @brief See public interface function description in BCDS_MCU_SPI.h */
Retcode_T MCU_SPI_Initialize(SPI_T spi, MCU_SPI_Callback_T callback)
{
    if ((NULL == spi) || (NULL == callback))
    {
        return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    }

    struct MCU_SPI_Handle_S* spi_ptr = (struct MCU_SPI_Handle_S*) spi;
    struct MCU_DMA_Channel_S * spiRxChannel = NULL;
    struct MCU_DMA_Channel_S * spiTxChannel = NULL;

    if (BCDS_HAL_TRANSFER_MODE_INTERRUPT == spi_ptr->TransferMode)
    {
        spi_ptr->AppCallback = callback;

        spi_ptr->send = SPI_SendIntMode;
        spi_ptr->receive = SPI_ReceiveIntMode;
        spi_ptr->cancel = SPI_Abort;
        spi_ptr->transfer = SPI_TransferIntMode;
        spi_ptr->State = SPI_STATE_READY;
        return RETCODE_OK;
    }
    else if (BCDS_HAL_TRANSFER_MODE_DMA == spi_ptr->TransferMode)
    {
        if ( (NULL != spi_ptr->Link1) && (NULL != spi_ptr->Link2))
        {
            spiTxChannel = (struct MCU_DMA_Channel_S *) spi_ptr->Link1;
            spiTxChannel->Config.cb = &spiTxChannel->CallBack;
            DMA_CfgChannel( spiTxChannel->ChannelId, &spiTxChannel->Config );

            spiRxChannel = (struct MCU_DMA_Channel_S *) spi_ptr->Link2;
            spiRxChannel->Config.cb = &spiRxChannel->CallBack;
            DMA_CfgChannel( spiRxChannel->ChannelId, &spiRxChannel->Config );

            spi_ptr->AppCallback = callback;

            spi_ptr->send = SPI_SendDmaMode;
            spi_ptr->cancel = SPI_Abort;
            spi_ptr->receive = SPI_ReceiveDmaMode;
            spi_ptr->transfer = SPI_TransferDmaMode;
            spi_ptr->State = SPI_STATE_READY;
            return RETCODE_OK;
        }
        else
        {
            return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NULL_POINTER);
        }
    }
    else
    {
        //other transfer modes are currently not supported
        return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NOT_SUPPORTED);
    }
}

/** @brief See public interface function description in BCDS_MCU_SPI.h */
Retcode_T MCU_SPI_Deinitialize(SPI_T spi)
{
    if (NULL == spi)
    {
        return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    }
    struct MCU_SPI_Handle_S* spi_ptr = (struct MCU_SPI_Handle_S*) spi;

    spi_ptr->receive = SPI_ReceiveDefault;
    spi_ptr->transfer = SPI_TransferDefault;
    spi_ptr->send = SPI_SendDefault;
    spi_ptr->cancel = NULL;
    spi_ptr->AppCallback = NULL;
    spi_ptr->State = SPI_STATE_INIT;

    return RETCODE_OK;
}

/** @brief See public interface function description in BCDS_MCU_SPI.h */
Retcode_T MCU_SPI_SetOwner(SPI_T spi, SPI_Owner_T parentHandle)
{
    struct MCU_SPI_Handle_S* spi_ptr = (struct MCU_SPI_Handle_S*) spi;
    Retcode_T retcode = RETCODE_OK;
    if ((NULL == spi_ptr) || (NULL == parentHandle))
    {
        retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NULL_POINTER);
    }
    else
    {
        spi_ptr->ParentHandle = parentHandle;
    }
    return retcode;
}

/** @brief See public interface function description in BCDS_MCU_SPI.h */
SPI_Owner_T MCU_SPI_GetOwner(SPI_T spi)
{
    struct MCU_SPI_Handle_S* spi_ptr = (struct MCU_SPI_Handle_S*) spi;
    return spi_ptr->ParentHandle;
}

/** @brief See public interface function description in BCDS_MCU_SPI.h */
Retcode_T MCU_SPI_Send(SPI_T spi, uint8_t * data, uint32_t dataLength)
{
    if (NULL == spi || NULL == data)
    {
        return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    }
    Retcode_T ret = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_UNINITIALIZED);
    struct MCU_SPI_Handle_S* spi_ptr = (struct MCU_SPI_Handle_S*) spi;

    if (0 != dataLength)
    {
        ret = spi_ptr->send(spi_ptr, data, dataLength);
    }
    else
    {
        /* application wants to cancel ongoing sending which is signaled by dataLength == 0*/
        ret = spi_ptr->cancel(spi_ptr);
    }
    return ret;
}

/** @brief See public interface function description in BCDS_MCU_SPI.h */
Retcode_T MCU_SPI_Receive(SPI_T spi, uint8_t* data, uint32_t dataLength)
{
    if (NULL == spi || NULL == data)
    {
        return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    }

    struct MCU_SPI_Handle_S* spi_ptr = (struct MCU_SPI_Handle_S*) spi;
    if (dataLength)
    {
        return spi_ptr->receive(spi_ptr, data, dataLength);
    }
    else
    {
        /*  application wants to cancel ongoing reception which is signaled by dataLength == 0
         *  NOTE: In case DMA: reception is canceled. In case interrupt mode: only sending of RxComplete will be prevented.
         *  */
        return spi_ptr->cancel(spi_ptr);
    }
}

/** @brief See public interface function description in BCDS_MCU_SPI.h */
Retcode_T MCU_SPI_Transfer(SPI_T spi, uint8_t* data_out, uint8_t* data_in, uint32_t dataLength)
{
    if (NULL == spi || NULL == data_out || NULL == data_in)
    {
        return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);

    }
    struct MCU_SPI_Handle_S* spi_ptr = (struct MCU_SPI_Handle_S*) spi;
    if (dataLength)
    {
        return spi_ptr->transfer(spi_ptr, data_out, data_in, dataLength);
    }
    else
    {
        /* application wants to cancel ongoing sending which is signaled by dataLength == 0*/
        return spi_ptr->cancel(spi_ptr);
    }
}

/** @brief See public interface function description in BCDS_MCU_SPI.h */
uint32_t MCU_SPI_GetDataCount(SPI_T spi)
{
    if (NULL == spi)
    {
        return 0;
    }
    struct MCU_SPI_Handle_S* spi_ptr = (struct MCU_SPI_Handle_S*) spi;
    return spi_ptr->XferSize - spi_ptr->TxXferCount;
}

/*************************** INTERFACE FUNCTIONS END ***************/

/*************************** HANDLE INTERFACE DRIVER ARRAY *********/

/**
 * @brief       Store handle in array if free position is found
 * @param[in]   Handle_ptr
 * @param[out]  Element_pptr
 * @retval RETCODE_OK only if successful
 * @retval RETCODE_OUT_OF_RESOURCES if not found and no free element available to store it
 */

/*************************** ASYNC TRANSFER FUNCTIONS **************/

/**
 * @brief Begin asynchronous TX transfer. The used buffer is specified in the serial object
 *
 * @param[in]       spi_ptr
 * @param[in]       data pointer
 * @param[in]       dataLength
 */

static Retcode_T SPI_SendIntMode(struct MCU_SPI_Handle_S* spi_ptr, uint8_t * data, uint32_t dataLength)
{
    if (SPI_STATE_READY == spi_ptr->State)
    {
        USART_TypeDef* spiInstance = spi_ptr->Instance;
        spi_ptr->State = SPI_STATE_TX;
        spi_ptr->pTxBuffPtr = data;
        spi_ptr->XferSize = dataLength;
        spi_ptr->TxXferCount = dataLength;
        /* Activate TX and clear TX buffer */
        spiInstance->CMD = USART_CMD_TXEN | USART_CMD_CLEARTX;
        /* Enable interrupt */
        spiInstance->IEN |= USART_IEN_TXBL;
        return RETCODE_OK;
    }
    else
    {
        return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_HAL_SPI_DRIVER_TX_NOT_READY);
    }
}

/**
 * @brief Begin asynchronous TX transfer in DMA mode.
 *
 * @param[in]       spi_ptr
 * @param[in]       data pointer
 * @param[in]       dataLength
 */

static Retcode_T SPI_SendDmaMode(struct MCU_SPI_Handle_S* spi_ptr, uint8_t * data, uint32_t dataLength)
{
    Retcode_T retcode = RETCODE_OK;
    DMA_CfgDescr_TypeDef spiCfgDescrTx;
    struct MCU_DMA_Channel_S * spiTxChannel = (struct MCU_DMA_Channel_S *) spi_ptr->Link1;

    if (SPI_STATE_READY == spi_ptr->State)
    {
        USART_TypeDef* spiInstance = spi_ptr->Instance;


        /* setup transfer for writing out to the tx buffer, reading from data incremented by 1*/
        spiCfgDescrTx.dstInc = dmaDataIncNone;
        spiCfgDescrTx.srcInc = dmaDataInc1;
        spiCfgDescrTx.arbRate = dmaArbitrate1;
        spiCfgDescrTx.size = dmaDataSize1;
        spiCfgDescrTx.hprot = 0;
        DMA_CfgDescr(spiTxChannel->ChannelId, true, &spiCfgDescrTx);

        spi_ptr->State = SPI_STATE_TX;
        spi_ptr->pTxBuffPtr = data;
        spi_ptr->XferSize = dataLength;
        spi_ptr->TxXferCount = dataLength;
        /* Activate TX and clear TX buffer */
        spiInstance->CMD = USART_CMD_TXEN | USART_CMD_CLEARTX;
        spiInstance->IEN = 0;
        spiInstance->IFC = USART_IFC_TXC;
        /* Enable interrupt */
        DMA_ActivateBasic(spiTxChannel->ChannelId, true, false, (void*)&(spi_ptr->Instance->TXDATA), data, (dataLength - 1));
        retcode =  RETCODE_OK;
    }
    else
    {
        retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_HAL_SPI_DRIVER_TX_NOT_READY);
    }
    return retcode;
}

/**
 * @brief Begin asynchronous RX transfer. The used buffer is specified in the serial object.
 *
 * @param[in]       spi_ptr
 * @param[out]      rx_buffer pointer
 * @param[in]       dataLength
 */
static Retcode_T SPI_ReceiveIntMode(struct MCU_SPI_Handle_S* spi_ptr, uint8_t *rx_buffer, uint32_t dataLength)
{

    if (SPI_STATE_READY == spi_ptr->State)
    {
        USART_TypeDef* spiInstance = spi_ptr->Instance;
        spi_ptr->State = SPI_STATE_RX;

        spi_ptr->pRxBuffPtr = rx_buffer;
        spi_ptr->XferSize = dataLength;
        spi_ptr->RxXferCount = dataLength;
        spi_ptr->pTxBuffPtr = rx_buffer;
        spi_ptr->TxXferCount = dataLength;

        /* Activate RX and clear RX buffer*/
        spiInstance->CMD = USART_CMD_RXEN | USART_CMD_CLEARRX | USART_CMD_TXEN | USART_CMD_CLEARTX;
        spiInstance->IFC = (USART_IFC_RXFULL | USART_IFC_PERR | USART_IFC_FERR | USART_IFC_RXOF);
        /* Enable interrupt */
        spiInstance->IEN |= USART_IEN_TXBL | USART_IEN_RXDATAV;

        return RETCODE_OK;
    }
    else
    {
        return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_HAL_SPI_DRIVER_RX_NOT_READY);
    }
}

/**
 * @brief Begin asynchronous RX transfer in DMA mode
 *
 * @param[in]       spi_ptr
 * @param[out]      rx_buffer pointer
 * @param[in]       dataLength
 */
static Retcode_T SPI_ReceiveDmaMode(struct MCU_SPI_Handle_S* spi_ptr, uint8_t *rx_buffer, uint32_t dataLength)
{

    USART_TypeDef* spiInstance = spi_ptr->Instance;
    DMA_CfgDescr_TypeDef spiCfgDescrTx;
    DMA_CfgDescr_TypeDef spiCfgDescrRx;
    struct MCU_DMA_Channel_S * spiTxChannel = (struct MCU_DMA_Channel_S *) spi_ptr->Link1;
    struct MCU_DMA_Channel_S * spiRxChannel = (struct MCU_DMA_Channel_S *) spi_ptr->Link2;

    if (SPI_STATE_READY == spi_ptr->State)
    {

        spi_ptr->State = SPI_STATE_RX;

        spi_ptr->pRxBuffPtr = rx_buffer;
        spi_ptr->XferSize = dataLength;
        spi_ptr->RxXferCount = dataLength;

        /* setup transfer for writing out permanently the same byte (default) */
        spiCfgDescrTx.dstInc = dmaDataIncNone;
        spiCfgDescrTx.srcInc = dmaDataIncNone;
        spiCfgDescrTx.arbRate = dmaArbitrate1;
        spiCfgDescrTx.size = dmaDataSize1;
        spiCfgDescrTx.hprot = 0;
        DMA_CfgDescr(spiTxChannel->ChannelId, true, &spiCfgDescrTx);

        /* read RxData to RxBuffer, with increment 1 */
        spiCfgDescrRx.dstInc = dmaDataInc1;
        spiCfgDescrRx.srcInc = dmaDataIncNone;
        spiCfgDescrRx.arbRate = dmaArbitrate1;
        spiCfgDescrRx.size = dmaDataSize1;
        spiCfgDescrRx.hprot = 0;
        DMA_CfgDescr(spiRxChannel->ChannelId, true, &spiCfgDescrRx);

        /* Activate RX/TX and clear RX/TX buffer*/
        spiInstance->CMD = USART_CMD_RXEN | USART_CMD_CLEARRX | USART_CMD_TXEN | USART_CMD_CLEARTX;
        spiInstance->IEN = 0;
        /* Enable DMA Channels */
        DMA_ActivateBasic(spiRxChannel->ChannelId, true, false, (void*)rx_buffer, (void*)&(spi_ptr->Instance->RXDATA), (dataLength - 1));
        /* TX finally starts the synchronous transfer */
        DMA_ActivateBasic(spiTxChannel->ChannelId, true, false, (void*)&(spi_ptr->Instance->TXDATA), (void*)&(spi_ptr->TxDefaultData), (dataLength - 1));

        return RETCODE_OK;
    }
    else
    {
        return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_HAL_SPI_DRIVER_RX_NOT_READY);
    }
}

/**
 * @brief Begin asynchronous transfer. The used buffer is specified in the serial object.
 *
 * @param[in]       spi_ptr
 * @param[in]       data_in pointer
 * @param[out]      data_out pointer
 * @param[in]       dataLength
 */
static Retcode_T SPI_TransferIntMode(struct MCU_SPI_Handle_S* spi_ptr, uint8_t * data_out, uint8_t * data_in, uint32_t dataLength)
{
    if (SPI_STATE_READY == spi_ptr->State)
    {
        USART_TypeDef* spiInstance = spi_ptr->Instance;
        spi_ptr->State = SPI_STATE_TX_RX;
        spi_ptr->pRxBuffPtr = data_in;
        spi_ptr->XferSize = dataLength;
        spi_ptr->RxXferCount = dataLength;
        spi_ptr->pTxBuffPtr = data_out;
        spi_ptr->TxXferCount = dataLength;

        /* Activate RX and clear RX buffer*/
        spiInstance->CMD = USART_CMD_RXEN | USART_CMD_CLEARRX | USART_CMD_TXEN | USART_CMD_CLEARTX;
        spiInstance->IFC = (USART_IFC_RXFULL | USART_IFC_PERR | USART_IFC_FERR | USART_IFC_RXOF);
        /* Enable interrupt */
        spiInstance->IEN |= USART_IEN_TXBL | USART_IEN_RXDATAV;

        return RETCODE_OK;
    }
    else
    {
        return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_HAL_SPI_DRIVER_TX_NOT_READY);
    }
}


/**
 * @brief Begin asynchronous transfer. The used buffer is specified in the serial object.
 *
 * @param[in]       spi_ptr
 * @param[in]       data_in pointer
 * @param[out]      data_out pointer
 * @param[in]       dataLength
 */
static Retcode_T SPI_TransferDmaMode(struct MCU_SPI_Handle_S* spi_ptr, uint8_t * data_out, uint8_t * data_in, uint32_t dataLength)
{
    DMA_CfgDescr_TypeDef spiCfgDescrTx;
    DMA_CfgDescr_TypeDef spiCfgDescrRx;
    struct MCU_DMA_Channel_S * spiTxChannel = (struct MCU_DMA_Channel_S *) spi_ptr->Link1;
    struct MCU_DMA_Channel_S * spiRxChannel = (struct MCU_DMA_Channel_S *) spi_ptr->Link2;
    USART_TypeDef* spiInstance = spi_ptr->Instance;

    if ( SPI_STATE_READY == spi_ptr->State )
    {
        spi_ptr->State = SPI_STATE_TX_RX;
        spi_ptr->pRxBuffPtr = data_in;
        spi_ptr->XferSize = dataLength;
        spi_ptr->RxXferCount = dataLength;
        spi_ptr->pTxBuffPtr = data_out;
        spi_ptr->TxXferCount = dataLength;

        /* setup transfer for writing out permanently the same byte (default) */
        spiCfgDescrTx.dstInc = dmaDataIncNone;
        spiCfgDescrTx.srcInc = dmaDataInc1;
        spiCfgDescrTx.arbRate = dmaArbitrate1;
        spiCfgDescrTx.size = dmaDataSize1;
        spiCfgDescrTx.hprot = 0;
        DMA_CfgDescr(spiTxChannel->ChannelId, true, &spiCfgDescrTx);

        /* read RxData to RxBuffer, with increment 1 */
        spiCfgDescrRx.dstInc = dmaDataInc1;
        spiCfgDescrRx.srcInc = dmaDataIncNone;
        spiCfgDescrRx.arbRate = dmaArbitrate1;
        spiCfgDescrRx.size = dmaDataSize1;
        spiCfgDescrRx.hprot = 0;
        DMA_CfgDescr(spiRxChannel->ChannelId, true, &spiCfgDescrRx);


        /* Activate RX and clear RX buffer*/
        spiInstance->CMD = USART_CMD_RXEN | USART_CMD_CLEARRX | USART_CMD_TXEN | USART_CMD_CLEARTX;
        spiInstance->IEN = 0;

        /* Enable DMA Channels */
        DMA_ActivateBasic(spiRxChannel->ChannelId, true, false, data_in, (void*)&(spi_ptr->Instance->RXDATA), (dataLength - 1));
        /* TX finally starts the synchronous transfer */
        DMA_ActivateBasic(spiTxChannel->ChannelId, true, false, (void*)&(spi_ptr->Instance->TXDATA), data_out, (dataLength - 1));


        return RETCODE_OK;
    }
    else
    {
        return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_HAL_SPI_DRIVER_TX_NOT_READY);
    }
}

/**
 * @brief   Abort the ongoing TX transaction. It disables the enabled interrupt for TX and
 *          flush TX hardware buffer if TX FIFO is used
 *
 * @param[in]       spi_ptr
 */
static Retcode_T SPI_Abort(struct MCU_SPI_Handle_S* spi_ptr)
{
    USART_TypeDef* spiInstance = spi_ptr->Instance;

    spiInstance->CMD = USART_CMD_RXDIS | USART_CMD_TXDIS;
    spiInstance->IEN &= ~(USART_IEN_RXDATAV | USART_IEN_PERR | USART_IEN_FERR | USART_IEN_RXOF | USART_IEN_TXC);
    spiInstance->IFC = USART_IFC_PERR | USART_IFC_FERR | USART_IFC_RXOF | USART_IFC_TXC;
    spi_ptr->State = SPI_STATE_READY;

    return RETCODE_OK;
}


#endif //-- BCDS_FEATURE_SPI
