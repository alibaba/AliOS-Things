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
 * Handles MCU EFM32 UART driver encapsulation
 */

#include "BCDS_Essentials.h"
#undef BCDS_MODULE_ID
#define BCDS_MODULE_ID BCDS_ESSENTIALS_MODULE_ID_UART

#if BCDS_FEATURE_UART && BCDS_UART_COUNT

#include "BCDS_MCU_UART.h"
#include "BCDS_MCU_UART_Handle.h"

#include "BCDS_MCU_DMA.h"
#include "BCDS_MCU_DMA_Handle.h"

static void MCU_UART_IRQ_RX_Callback(UART_T uart);
static void MCU_UART_IRQ_TX_Callback(UART_T uart);
static void MCU_UART_DMA_RX_Callback(UART_T uart);
static void MCU_UART_DMA_TX_Callback(UART_T uart);

static Retcode_T UART_Abort_TX_IntMode(struct MCU_UART_Handle_S* uart_ptr);
static Retcode_T UART_Abort_RX_IntMode(struct MCU_UART_Handle_S* uart_ptr);
static Retcode_T UART_Send_Async(struct MCU_UART_Handle_S* uart_ptr, uint8_t * data, uint32_t len);
static Retcode_T UART_Receive_Async(struct MCU_UART_Handle_S* uart_ptr, uint8_t *rx_buffer, uint32_t rx_xfer_size);
static Retcode_T UART_GetRxCountIntMode(struct MCU_UART_Handle_S* uart_ptr, uint32_t * count);

/*************************** CALLBACKS *****************************/

void MCU_UART_IRQ_RX_Callback(UART_T uart)
{
    struct MCU_UART_Handle_S* uart_ptr = (struct MCU_UART_Handle_S*) uart;
    union MCU_UART_Event_U event;
    uint32_t uartIntFlags = uart_ptr->Uart_ptr->IF;
    USART_TypeDef* uartInstance = uart_ptr->Uart_ptr;
    event.registerValue = 0;
    if (uartIntFlags & _UART_IF_RXDATAV_MASK)
    {
        /* we have valid data in the buffer, so we read it */

        uart_ptr->_DriverCtx.pRxBuffPtr[(uart_ptr->_DriverCtx.RxXferSize) - (uart_ptr->_DriverCtx.RxXferCount)] = uartInstance->RXDATA; //USART_RxDataGet(uartInstance);
        uart_ptr->_DriverCtx.RxXferCount--;

        if (!(uart_ptr->_DriverCtx.RxXferCount))
        {
            event.bitfield.RxComplete = 1;
            uart_ptr->_DriverCtx.RxXferCount = uart_ptr->_DriverCtx.RxXferSize;
            uart_ptr->_DriverCtx.RxState = UART_STATE_READY;
        }
    }
    if (uartIntFlags & (UART_IF_PERR | UART_IF_FERR | UART_IF_RXOF))
    {
        /* we do not propagate the exact error,
         * so we clear all possible interrupts here
         */
        uartInstance->IFC = UART_IFC_PERR | UART_IFC_FERR | UART_IFC_RXOF;
        event.bitfield.RxError = 1;
    }
    /* in case of a set event call the AppCallback */
    if (event.registerValue)
    {
        uart_ptr->_DriverCtx.AppCallback(uart, event.bitfield);
    }

}

void MCU_UART_IRQ_TX_Callback(UART_T uart)
{
    struct MCU_UART_Handle_S* uart_ptr = (struct MCU_UART_Handle_S*) uart;
    union MCU_UART_Event_U event;
    uint8_t *buf = uart_ptr->_DriverCtx.pTxBuffPtr;
    Retcode_T retcode = RETCODE_OK;

    if (uart_ptr->Uart_ptr != NULL)
    {
        event.registerValue = 0;
        /* There is still data to send */
        if ((uart_ptr->_DriverCtx.TxXferCount <= (uart_ptr->_DriverCtx.TxXferSize - 1)))
        {
            USART_Tx(uart_ptr->Uart_ptr, buf[uart_ptr->_DriverCtx.TxXferCount]);
            uart_ptr->_DriverCtx.TxXferCount++;
        }
        if (uart_ptr->_DriverCtx.TxXferCount >= uart_ptr->_DriverCtx.TxXferSize)
        {
            if (uart_ptr->Uart_ptr->IF & UART_IF_TXC)
            {
                /* Last byte has been successfully transmitted. Stop the procedure */
                retcode = UART_Abort_TX_IntMode(uart_ptr);
                if (RETCODE_OK != retcode)
                {
                    Retcode_RaiseErrorFromIsr(retcode);
                }
                uart_ptr->_DriverCtx.TxState = UART_STATE_READY;
                event.bitfield.TxComplete = 1;
                uart_ptr->_DriverCtx.AppCallback(uart, event.bitfield);
            }
            else
            {
                /* Last byte has been put in TX, set up TXC interrupt */
                USART_IntDisable(uart_ptr->Uart_ptr, _UART_IEN_RESETVALUE);
                USART_IntDisable(uart_ptr->Uart_ptr, UART_IEN_TXBL);
                USART_IntEnable(uart_ptr->Uart_ptr, UART_IEN_TXC);
            }

        }
    }
}

void MCU_UART_DMA_RX_Callback(UART_T uart)
{
    struct MCU_UART_Handle_S* uart_ptr = (struct MCU_UART_Handle_S*) uart;
    struct MCU_DMA_Channel_S * dmaRxChannel = (struct MCU_DMA_Channel_S *)  uart_ptr->Link2;
    union MCU_UART_Event_U event;
    /* Last byte has been put in TX, set up TXC interrupt */
    uart_ptr->_DriverCtx.RxXferCount = 0;
    event.bitfield.RxComplete = 1;
    uart_ptr->_DriverCtx.AppCallback(uart, event.bitfield);

    DMA_ActivateBasic(dmaRxChannel->ChannelId,true, false, NULL, NULL, (uart_ptr->_DriverCtx.RxXferSize-1));
    uart_ptr->_DriverCtx.RxXferCount = uart_ptr->_DriverCtx.RxXferSize;

}

void MCU_UART_DMA_TX_Callback(UART_T uart)
{
    BCDS_UNUSED(uart);
    struct MCU_UART_Handle_S* uart_ptr = (struct MCU_UART_Handle_S*) uart;
    /* Last byte has been put in TX, set up TXC interrupt */
    uart_ptr->_DriverCtx.TxXferCount = uart_ptr->_DriverCtx.TxXferSize;
    USART_IntEnable(uart_ptr->Uart_ptr, UART_IEN_TXC);
}


/*************************** CALLBACKS END *************************/

/*************************** INTERFACE FUNCTIONS *******************/

/** @brief See public interface function description in BCDS_MCU_UART.h */
Retcode_T MCU_UART_Initialize(UART_T uart, MCU_UART_Callback_T callback)
{
    struct MCU_UART_Handle_S* uart_ptr = (struct MCU_UART_Handle_S*) uart;
    bool dmaRx=false;
    bool dmaTx=false;

    struct MCU_DMA_Channel_S * uartRxChannel = NULL;
    struct MCU_DMA_Channel_S * uartTxChannel = NULL;
    DMA_CfgDescr_TypeDef uartCfgDescrTx;
    DMA_CfgDescr_TypeDef uartCfgDescrRx;

    Retcode_T ret = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    if ((NULL != uart_ptr) && (NULL != callback) &&
            ((BCDS_HAL_TRANSFER_MODE_BLOCKING <= uart_ptr->TransferMode)))
    {
        /* second: Add callback function pointers so that BSP is able to callback this
         * module when IRQs are pending. */
        uart_ptr->IrqRxCallback = MCU_UART_IRQ_RX_Callback; /* invoked by BSP if uart in interrupt mode, callback of MCU */
        uart_ptr->IrqTxCallback = MCU_UART_IRQ_TX_Callback; /* invoked by BSP if uart in interrupt mode, callback of MCU */
        uart_ptr->DmaRxCallback = MCU_UART_DMA_RX_Callback; /* invoked by BSP if uart in DMA mode, callback of MCU */
        uart_ptr->DmaTxCallback = MCU_UART_DMA_TX_Callback; /* invoked by BSP if uart in DMA mode, callback of MCU */

        uart_ptr->_DriverCtx.receive = UART_Receive_Async;
        uart_ptr->_DriverCtx.send = UART_Send_Async;
        uart_ptr->_DriverCtx.AppCallback = callback; /* invoked by MCU, callback of application */
        uart_ptr->_DriverCtx.receiveCancel = UART_Abort_RX_IntMode;
        uart_ptr->_DriverCtx.sendCancel = UART_Abort_TX_IntMode;
        uart_ptr->_DriverCtx.countRx = UART_GetRxCountIntMode;
        uart_ptr->_DriverCtx.RxState = UART_STATE_READY;/* set receive state machine on ready state */
        uart_ptr->_DriverCtx.TxState = UART_STATE_READY;/* set transmit state machine on ready state */

        ret = RETCODE_OK;
        switch (uart_ptr->TransferMode)
        {
            case BCDS_HAL_TRANSFER_MODE_INTERRUPT:
            	dmaTx=false;
            	dmaRx=false;
            	break;
        	case BCDS_HAL_TRANSFER_MODE_DMA:
        		dmaTx=true;
        		dmaRx=true;
        		break;
        	case BCDS_HAL_TRANSFER_MODE_INTERRUPT_RX_DMA_TX:
        		dmaTx=true;
        		dmaRx=false;
        		break;
        	case BCDS_HAL_TRANSFER_MODE_INTERRUPT_TX_DMA_RX:
        		dmaTx=false;
        		dmaRx=true;
        		break;
        	case BCDS_HAL_TRANSFER_MODE_BLOCKING:
        	default:
        		uart_ptr->_DriverCtx.RxState = UART_STATE_INIT;/* set receive state machine on init state */
   		        uart_ptr->_DriverCtx.TxState = UART_STATE_INIT;/* set transmit state machine on init state */
        		dmaTx = false;
        		dmaRx = false;
        		ret = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NOT_SUPPORTED);
        		break;
        }
        if (dmaTx)
        {
        	uartTxChannel = (struct MCU_DMA_Channel_S *) uart_ptr->Link1;
        	uartTxChannel->Config.cb = &uartTxChannel->CallBack;
        	DMA_CfgChannel( uartTxChannel->ChannelId, &uartTxChannel->Config );
        	uartCfgDescrTx.dstInc = dmaDataIncNone;
        	uartCfgDescrTx.srcInc = dmaDataInc1;
        	uartCfgDescrTx.arbRate = dmaArbitrate1;
        	uartCfgDescrTx.size = dmaDataSize1;
        	uartCfgDescrTx.hprot = 0;
        	DMA_CfgDescr(uartTxChannel->ChannelId, true, &uartCfgDescrTx);
        }
        if (dmaRx)
        {
        	uartRxChannel = (struct MCU_DMA_Channel_S *) uart_ptr->Link2;
        	uartRxChannel->Config.cb = &uartRxChannel->CallBack;
        	DMA_CfgChannel( uartRxChannel->ChannelId, &uartRxChannel->Config );
        	uartCfgDescrRx.dstInc = dmaDataInc1;
        	uartCfgDescrRx.srcInc = dmaDataIncNone;
        	uartCfgDescrRx.arbRate = dmaArbitrate1;
        	uartCfgDescrRx.size = dmaDataSize1;
        	uartCfgDescrRx.hprot = 0;
        	DMA_CfgDescr(uartRxChannel->ChannelId, true, &uartCfgDescrRx);
        }

    }
    return ret;
}

/** @brief See public interface function description in BCDS_MCU_UART.h */
Retcode_T MCU_UART_Deinitialize(UART_T uart)
{
    Retcode_T ret = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_UNINITIALIZED);

    struct MCU_UART_Handle_S* uart_ptr = (struct MCU_UART_Handle_S*) uart;
    if (NULL != uart_ptr)
    {
        ret = uart_ptr->_DriverCtx.sendCancel(uart_ptr);
        if (RETCODE_OK != ret)
        {
            Retcode_RaiseError(ret);
        }
        ret = uart_ptr->_DriverCtx.receiveCancel(uart_ptr);
        if (RETCODE_OK != ret)
        {
            Retcode_RaiseError(ret);
        }
        uart_ptr->_DriverCtx.send = NULL;
        uart_ptr->_DriverCtx.sendCancel = NULL;
        uart_ptr->_DriverCtx.receive = NULL;
        uart_ptr->_DriverCtx.receiveCancel = NULL;
        uart_ptr->_DriverCtx.TxState = UART_STATE_INIT;
        uart_ptr->_DriverCtx.RxState = UART_STATE_INIT;
        uart_ptr->DmaRxCallback = NULL;
        uart_ptr->DmaTxCallback = NULL;
        uart_ptr->IrqRxCallback = NULL;
        uart_ptr->IrqTxCallback = NULL;
        uart_ptr->RtsCallback = NULL;
        uart_ptr->CtsCallback = NULL;

        ret = RETCODE_OK;
    }

    return ret;
}

/** @brief See public interface function description in BCDS_MCU_UART.h */
Retcode_T MCU_UART_SetParentHandle(UART_T uart, SWHandle_T parentHandle)
{
    struct MCU_UART_Handle_S* uart_ptr = (struct MCU_UART_Handle_S*) uart;
    Retcode_T retcode = RETCODE_OK;
    if ((NULL == uart_ptr) || (NULL == parentHandle))
    {
        retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NULL_POINTER);
    }
    else
    {
        uart_ptr->ParentHandle = parentHandle;
    }
    return retcode;
}

/** @brief See public interface function description in BCDS_MCU_UART.h */
SWHandle_T MCU_UART_GetParentHandle(UART_T uart)
{
    struct MCU_UART_Handle_S* uart_ptr = (struct MCU_UART_Handle_S*) uart;
    return uart_ptr->ParentHandle;
}

/** @brief See public interface function description in BCDS_MCU_UART.h */
Retcode_T MCU_UART_Send(UART_T uart, uint8_t* data, uint32_t len)
{
    Retcode_T retcode=RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    if ((NULL == uart) || (NULL == data))
    {
        return retcode;
    }

    struct MCU_UART_Handle_S* uart_ptr = (struct MCU_UART_Handle_S*) uart;

    if (NULL != uart_ptr->_DriverCtx.send)
    {
    	if (0 != len)
    	{
    		retcode = uart_ptr->_DriverCtx.send(uart_ptr, data, len);
    	}
    	else
    	{
    		/* application wants to cancel ongoing sending which is signaled by len == 0*/
    		retcode = uart_ptr->_DriverCtx.sendCancel(uart_ptr);
    	}
    }
    return retcode;
}

/** @brief See public interface function description in BCDS_MCU_UART.h */
Retcode_T MCU_UART_Receive(UART_T uart, uint8_t* data, uint32_t len)
{
    Retcode_T retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    if ((NULL == data) || (NULL == uart))
    {
        return retcode;
    }

    struct MCU_UART_Handle_S* uart_ptr = (struct MCU_UART_Handle_S*) uart;
    if (NULL != uart_ptr->_DriverCtx.receive)
    {
    	if (0 != len)
    	{
    		retcode = uart_ptr->_DriverCtx.receive(uart_ptr, data, len);
    	}
    	else
    	{
    		/*  application wants to cancel ongoing reception which is signaled by len == 0
    		 *  NOTE: In case DMA: reception is canceled. In case interrupt mode: only sending of RxComplete will be prevented.
    		 *  */
    		retcode = uart_ptr->_DriverCtx.receiveCancel(uart_ptr);
    	}
    }
    return retcode;
}

/** @brief See public interface function description in BCDS_MCU_UART.h */
Retcode_T MCU_UART_GetRxCount(UART_T uart, uint32_t * count)
{
    Retcode_T retcode = (RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM));
    if ((NULL != count) && (NULL != uart))
    {
        struct MCU_UART_Handle_S* uart_ptr = (struct MCU_UART_Handle_S*) uart;
        if ( uart_ptr->_DriverCtx.RxState != UART_STATE_INIT)
        {
            retcode = uart_ptr->_DriverCtx.countRx(uart_ptr, count);
        }
    }
    return retcode;
}

static Retcode_T UART_GetRxCountIntMode(struct MCU_UART_Handle_S* uart_ptr, uint32_t * count)
{
    *count = uart_ptr->_DriverCtx.RxXferSize - uart_ptr->_DriverCtx.RxXferCount;
    return RETCODE_OK;
}

/*************************** INTERFACE FUNCTIONS END ***************/

/*************************** HANDLE INTERFACE DRIVER ARRAY *********/

/*************************** ASYNC TRANSFER FUNCTIONS **************/


/**
 * @brief Begin asynchronous TX transfer. The used buffer is specified in the serial object
 *
 * @param[in]       element_ptr
 * @param[in]       tx_buffer pointer
 * @param[in]       tx_xfer_size
 */
static Retcode_T UART_Send_Async(struct MCU_UART_Handle_S* uart_ptr, uint8_t * data, uint32_t len)
{

    Retcode_T rc;
    struct MCU_DMA_Channel_S * dmaTxChannel = (struct MCU_DMA_Channel_S *)  uart_ptr->Link1;
    if (uart_ptr->_DriverCtx.TxState == UART_STATE_READY)
    {

    	switch (uart_ptr->TransferMode)
    	{
    	 case BCDS_HAL_TRANSFER_MODE_INTERRUPT:
    		uart_ptr->_DriverCtx.TxState = UART_STATE_RUNNING;
    		uart_ptr->_DriverCtx.pTxBuffPtr = data;
    		uart_ptr->_DriverCtx.TxXferSize = len;
    		uart_ptr->_DriverCtx.TxXferCount = 0;
            // Activate TX and clear TX buffer
            uart_ptr->Uart_ptr->CMD = UART_CMD_TXEN | UART_CMD_CLEARTX;
            // Enable interrupt
            uart_ptr->Uart_ptr->IEN |= UART_IEN_TXBL;
    		rc = RETCODE_OK;
    		break;

    	case BCDS_HAL_TRANSFER_MODE_DMA:
    	case BCDS_HAL_TRANSFER_MODE_INTERRUPT_RX_DMA_TX:
    	    uart_ptr->_DriverCtx.TxState = UART_STATE_RUNNING;
    	    uart_ptr->_DriverCtx.pTxBuffPtr = data;
    	    uart_ptr->_DriverCtx.TxXferSize = len;
    	    uart_ptr->_DriverCtx.TxXferCount = 0;
    	    // Activate TX and clear TX buffer
    	    uart_ptr->Uart_ptr->CMD = UART_CMD_TXEN | UART_CMD_CLEARTX;
    	    DMA_ActivateBasic(dmaTxChannel->ChannelId, true, false, (void*)&(uart_ptr->Uart_ptr->TXDATA), data, (unsigned int) (len - 1));
    	    rc = RETCODE_OK;

            break;
        case BCDS_HAL_TRANSFER_MODE_BLOCKING: /* intentional fall through to default case: */
    	default:
    	    rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NOT_SUPPORTED);
    	    break;
    	}
    }
    else
    {
        rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_HAL_UART_DRIVER_TX_NOT_READY);
    }
    return rc;
}

/**
 * @brief Begin asynchronous RX transfer. The used buffer is specified in the serial object.
 *
 * @param[in]       element_ptr
 * @param[out]      rx_buffer pointer
 * @param[in]       rx_xfer_size
 */
static Retcode_T UART_Receive_Async(struct MCU_UART_Handle_S* uart_ptr, uint8_t *rx_buffer, uint32_t rx_xfer_size)
{
    Retcode_T retcode=RETCODE_OK;
// Check that a buffer has indeed been set up
    USART_TypeDef* UartInstance = uart_ptr->Uart_ptr;
    struct MCU_DMA_Channel_S * dmaRxChannel = (struct MCU_DMA_Channel_S *)  uart_ptr->Link2;

    if (uart_ptr->_DriverCtx.RxState == UART_STATE_READY)
    {
        switch (uart_ptr->TransferMode)
        {
        case BCDS_HAL_TRANSFER_MODE_INTERRUPT:
            uart_ptr->_DriverCtx.RxState = UART_STATE_RUNNING;
            uart_ptr->_DriverCtx.pRxBuffPtr = rx_buffer;
            uart_ptr->_DriverCtx.RxXferSize = rx_xfer_size;
            uart_ptr->_DriverCtx.RxXferCount = rx_xfer_size;

            // Activate RX and clear RX buffer
            UartInstance->CMD = UART_CMD_RXEN | UART_CMD_CLEARRX;
            //clear Flags that might be set
            UartInstance->IFC = UART_IFC_PERR | UART_IFC_FERR | UART_IFC_RXOF | UART_IFC_RXFULL;
            // Enable interrupt
            UartInstance->IEN = UART_IEN_RXDATAV;
            break;
        case BCDS_HAL_TRANSFER_MODE_DMA:
        case BCDS_HAL_TRANSFER_MODE_INTERRUPT_TX_DMA_RX:
            uart_ptr->_DriverCtx.RxState = UART_STATE_RUNNING;
            uart_ptr->_DriverCtx.pRxBuffPtr = rx_buffer;
            uart_ptr->_DriverCtx.RxXferSize = rx_xfer_size;
            uart_ptr->_DriverCtx.RxXferCount = rx_xfer_size;

            // Activate RX and clear RX buffer
            UartInstance->CMD = UART_CMD_RXEN | UART_CMD_CLEARRX;
            DMA_ActivateBasic(dmaRxChannel->ChannelId, true, false, rx_buffer, (void*)&(uart_ptr->Uart_ptr->RXDATA), (unsigned int) (rx_xfer_size - 1));
            break;
        default:
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NOT_SUPPORTED);
        }
    }
    else
    {
        retcode =  RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_HAL_UART_DRIVER_RX_NOT_READY);
    }
    return retcode;
}

/**
 * @brief   Abort the ongoing TX transaction. It disables the enabled interrupt for TX and
 *          flush TX hardware buffer if TX FIFO is used
 *
 * @param[in]       element_ptr
 */
static Retcode_T UART_Abort_TX_IntMode(struct MCU_UART_Handle_S* uart_ptr)
{
    uart_ptr->_DriverCtx.TxState = UART_STATE_READY;
    /* stop transmitter */
    uart_ptr->Uart_ptr->CMD |= USART_CMD_TXDIS;
    /* disable interrupts */
    USART_IntDisable(uart_ptr->Uart_ptr, UART_IEN_TXC);
    USART_IntClear(uart_ptr->Uart_ptr, UART_IFC_TXC);
    return RETCODE_OK;
}

/**
 * @brief   Abort the ongoing RX transaction. It disables the enabled interrupt for RX and
 *          flush RX hardware buffer if RX FIFO is used
 *
 * @param[in]       element_ptr
 */
static Retcode_T UART_Abort_RX_IntMode(struct MCU_UART_Handle_S* uart_ptr)
{
    uart_ptr->_DriverCtx.RxState = UART_STATE_READY;
    /* Stop receiver */
    uart_ptr->Uart_ptr->CMD |= UART_CMD_RXDIS;
    /* disable all interrupts */
    USART_IntDisable(uart_ptr->Uart_ptr, UART_IEN_RXDATAV | UART_IEN_PERR | UART_IEN_FERR | UART_IEN_RXOF);
    /*clear all set interrupts*/
    USART_IntClear(uart_ptr->Uart_ptr, UART_IFC_PERR | UART_IFC_FERR | UART_IFC_RXOF);
    return RETCODE_OK;

}
/*************************** ASYNC TRANSFER FUNCTIONS END **********/

#endif //-- BCDS_FEATURE_UART && BCDS_UART_COUNT
