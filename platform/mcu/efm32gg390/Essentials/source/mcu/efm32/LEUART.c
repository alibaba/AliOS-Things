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
 * Handles MCU EFM32 LEUART driver encapsulation
 */

#include "BCDS_Essentials.h"

#undef BCDS_MODULE_ID
#define BCDS_MODULE_ID BCDS_ESSENTIALS_MODULE_ID_LEUART

#include "BCDS_HAL.h"
#include "BCDS_HALConfig.h"

#if BCDS_FEATURE_LEUART && BCDS_LEUART_COUNT

#include "BCDS_MCU_LEUART.h"
#include "BCDS_MCU_LEUART_Handle.h"

#include "BCDS_MCU_DMA.h"
#include "BCDS_MCU_DMA_Handle.h"


void MCU_LEUART_IRQ_RX_Callback(LEUART_T leuart);
void MCU_LEUART_IRQ_TX_Callback(LEUART_T leuart);
void MCU_LEUART_DMA_RX_Callback(LEUART_T leuart);
void MCU_LEUART_DMA_TX_Callback(LEUART_T leuart);

static Retcode_T LEUART_Abort_TX_IntMode(struct MCU_LEUART_Handle_S* leuart_ptr);
static Retcode_T LEUART_Abort_RX_IntMode(struct MCU_LEUART_Handle_S* leuart_ptr);
static Retcode_T LEUART_Send_Async(struct MCU_LEUART_Handle_S* leuart_ptr, uint8_t * data, uint32_t len);
static Retcode_T LEUART_Receive_Async(struct MCU_LEUART_Handle_S* leuart_ptr, uint8_t *rx_buffer, uint32_t rx_xfer_size);
static Retcode_T LEUART_GetRxCountIntMode(struct MCU_LEUART_Handle_S* leuart_ptr, uint32_t * count);

/*************************** CALLBACKS *****************************/

void MCU_LEUART_IRQ_RX_Callback(LEUART_T leuart)
{
	struct MCU_LEUART_Handle_S* leuart_ptr = (struct MCU_LEUART_Handle_S*) leuart;
	union MCU_LEUART_Event_U event;
	uint32_t leuartIntFlags = leuart_ptr->leuart_ptr->IF;
	LEUART_TypeDef* leuartInstance = leuart_ptr->leuart_ptr;
	event.registerValue = 0;
	if (leuartIntFlags & _LEUART_IF_RXDATAV_MASK)
	{
		/* we have valid data in the buffer, so we read it */
		leuart_ptr->_DriverCtx.pRxBuffPtr[(leuart_ptr->_DriverCtx.RxXferSize) - (leuart_ptr->_DriverCtx.RxXferCount)] = leuartInstance->RXDATA;
		leuart_ptr->_DriverCtx.RxXferCount--;

		if (!(leuart_ptr->_DriverCtx.RxXferCount))
		{
			event.bitfield.RxComplete = 1;
			leuart_ptr->_DriverCtx.RxXferCount = leuart_ptr->_DriverCtx.RxXferSize;
			leuart_ptr->_DriverCtx.RxState = LEUART_STATE_READY;
		}
	}
	if (leuartIntFlags & (LEUART_IF_PERR | LEUART_IF_FERR | LEUART_IF_RXOF))
	{
		/* we do not propagate the exact error,
		 * so we clear all possible interrupts here
		 */
		leuartInstance->IFC = LEUART_IFC_PERR | LEUART_IFC_FERR | LEUART_IFC_RXOF;
		event.bitfield.RxError = 1;
	}
	/* in case of a set event call the AppCallback */
	if (event.registerValue)
	{
		leuart_ptr->_DriverCtx.AppCallback(leuart, event.bitfield);
	}
}

void MCU_LEUART_IRQ_TX_Callback(LEUART_T leuart)
{
	struct MCU_LEUART_Handle_S* leuart_ptr = (struct MCU_LEUART_Handle_S*) leuart;
	union MCU_LEUART_Event_U event;
	uint8_t *buf = leuart_ptr->_DriverCtx.pTxBuffPtr;
	Retcode_T retcode = RETCODE_OK;

	if (leuart_ptr->leuart_ptr != NULL)
	{
		if( leuart_ptr->leuart_ptr->IF & LEUART_IF_TXBL)
		{
			event.registerValue = 0;
			/* There is still data to send */
			if ((leuart_ptr->_DriverCtx.TxXferCount <= (leuart_ptr->_DriverCtx.TxXferSize - 1)))
			{
				LEUART_Tx(leuart_ptr->leuart_ptr, buf[leuart_ptr->_DriverCtx.TxXferCount]);
				leuart_ptr->_DriverCtx.TxXferCount++;
			}
			if (leuart_ptr->_DriverCtx.TxXferCount >= leuart_ptr->_DriverCtx.TxXferSize)
			{
				if ( leuart_ptr->leuart_ptr->IF & LEUART_IF_TXC )
				{
					/* Last byte has been successfully transmitted. Stop the procedure */
					retcode = LEUART_Abort_TX_IntMode(leuart_ptr);
					if (RETCODE_OK != retcode)
					{
						Retcode_RaiseErrorFromIsr(retcode);
					}
					leuart_ptr->_DriverCtx.TxState = LEUART_STATE_READY;
					event.bitfield.TxComplete = 1;
					leuart_ptr->_DriverCtx.AppCallback(leuart, event.bitfield);
				}
				else
				{
					/* Last byte has been put in TX, set up TXC interrupt */
					LEUART_IntDisable(leuart_ptr->leuart_ptr, LEUART_IEN_TXBL);
					LEUART_IntEnable(leuart_ptr->leuart_ptr, LEUART_IEN_TXC);
				}
			}
		}
	}
}

void MCU_LEUART_DMA_RX_Callback(LEUART_T leuart)
{
    struct MCU_LEUART_Handle_S* leuart_ptr = (struct MCU_LEUART_Handle_S*) leuart;
    struct MCU_DMA_Channel_S * dmaRxChannel = (struct MCU_DMA_Channel_S *)  leuart_ptr->Link2;
    union MCU_LEUART_Event_U event;
    /* Last byte has been received */
    leuart_ptr->_DriverCtx.RxXferCount = 0;
    event.bitfield.RxComplete = 1;
    leuart_ptr->_DriverCtx.AppCallback(leuart, event.bitfield);

    DMA_ActivateBasic(dmaRxChannel->ChannelId,true, false, NULL, NULL, (leuart_ptr->_DriverCtx.RxXferSize-1));
    leuart_ptr->_DriverCtx.RxXferCount = leuart_ptr->_DriverCtx.RxXferSize;

}

void MCU_LEUART_DMA_TX_Callback(LEUART_T leuart)
{
    struct MCU_LEUART_Handle_S* leuart_ptr = (struct MCU_LEUART_Handle_S*) leuart;
    /* Last byte has been put in TX, set up TXC interrupt */
    leuart_ptr->_DriverCtx.TxXferCount = leuart_ptr->_DriverCtx.TxXferSize;
    LEUART_IntEnable(leuart_ptr->leuart_ptr, LEUART_IEN_TXC);
}

/*************************** CALLBACKS END *************************/

/*************************** INTERFACE FUNCTIONS *******************/

/** @brief See public interface function description in BCDS_MCU_LEUART.h */
Retcode_T MCU_LEUART_Initialize(LEUART_T leuart, MCU_LEUART_Callback_T callback)
{
	struct MCU_LEUART_Handle_S* leuart_ptr = (struct MCU_LEUART_Handle_S*) leuart;
	struct MCU_DMA_Channel_S * leuartRxChannel = NULL;
	struct MCU_DMA_Channel_S * leuartTxChannel = NULL;
	DMA_CfgDescr_TypeDef leuartCfgDescrTx;
	DMA_CfgDescr_TypeDef leuartCfgDescrRx;
	bool dmaTx = false;
	bool dmaRx = false;
    Retcode_T ret = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);

    if ((NULL != leuart_ptr) && (NULL != callback))
    {
        /* second: Add callback function pointers so that BSP is able to callback this
         * module when IRQs are pending. */
        leuart_ptr->IrqRxCallback = MCU_LEUART_IRQ_RX_Callback; /* invoked by BSP if leuart in interrupt mode, callback of MCU */
        leuart_ptr->IrqTxCallback = MCU_LEUART_IRQ_TX_Callback; /* invoked by BSP if leuart in interrupt mode, callback of MCU */
        leuart_ptr->DmaRxCallback = MCU_LEUART_DMA_RX_Callback; /* invoked by BSP if uart in DMA mode, callback of MCU */
        leuart_ptr->DmaTxCallback = MCU_LEUART_DMA_TX_Callback; /* invoked by BSP if uart in DMA mode, callback of MCU */

        leuart_ptr->_DriverCtx.receive = LEUART_Receive_Async;
        leuart_ptr->_DriverCtx.send = LEUART_Send_Async;
        leuart_ptr->_DriverCtx.AppCallback = callback; /* invoked by MCU, callback of application */
        leuart_ptr->_DriverCtx.receiveCancel = LEUART_Abort_RX_IntMode;
        leuart_ptr->_DriverCtx.sendCancel = LEUART_Abort_TX_IntMode;
        leuart_ptr->_DriverCtx.countRx = LEUART_GetRxCountIntMode;
        leuart_ptr->_DriverCtx.RxState = LEUART_STATE_READY;/* set receive state machine on ready state */
        leuart_ptr->_DriverCtx.TxState = LEUART_STATE_READY;/* set transmit state machine on ready state */

        switch (leuart_ptr->TransferMode)
        {
            case BCDS_HAL_TRANSFER_MODE_INTERRUPT:
            	dmaTx = false;
            	dmaRx = false;
            	ret = RETCODE_OK;
            	break;
            case BCDS_HAL_TRANSFER_MODE_DMA:
        	    dmaTx=true;
        	    dmaRx=true;
        	    ret = RETCODE_OK;
        	    break;
            case BCDS_HAL_TRANSFER_MODE_INTERRUPT_RX_DMA_TX:
        	    dmaTx=true;
        	    dmaRx=false;
        	    break;
            case BCDS_HAL_TRANSFER_MODE_INTERRUPT_TX_DMA_RX:
        	    dmaTx=false;
        	    dmaRx=true;
        	    ret = RETCODE_OK;
        	    break;
            default:
        	    dmaTx = false;
        	    dmaRx = false;
        	    leuart_ptr->_DriverCtx.RxState = LEUART_STATE_INIT;/* set receive state machine on init state */
   	            leuart_ptr->_DriverCtx.TxState = LEUART_STATE_INIT;/* set transmit state machine on init state */

        	    ret = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NOT_SUPPORTED);
            	break;
        }
        if (dmaTx)
        {
        	leuartTxChannel = (struct MCU_DMA_Channel_S *) leuart_ptr->Link1;
        	leuartTxChannel->Config.cb = &leuartTxChannel->CallBack;
        	DMA_CfgChannel( leuartTxChannel->ChannelId, &leuartTxChannel->Config );
        	leuartCfgDescrTx.dstInc = dmaDataIncNone;
        	leuartCfgDescrTx.srcInc = dmaDataInc1;
        	leuartCfgDescrTx.arbRate = dmaArbitrate1;
        	leuartCfgDescrTx.size = dmaDataSize1;
        	leuartCfgDescrTx.hprot = 0;
        	DMA_CfgDescr(leuartTxChannel->ChannelId, true, &leuartCfgDescrTx);
        }
        if (dmaRx)
        {
        	leuartRxChannel = (struct MCU_DMA_Channel_S *) leuart_ptr->Link2;
        	leuartRxChannel->Config.cb = &leuartRxChannel->CallBack;
        	DMA_CfgChannel( leuartRxChannel->ChannelId, &leuartRxChannel->Config );
        	leuartCfgDescrRx.dstInc = dmaDataInc1;
        	leuartCfgDescrRx.srcInc = dmaDataIncNone;
        	leuartCfgDescrRx.arbRate = dmaArbitrate1;
        	leuartCfgDescrRx.size = dmaDataSize1;
        	leuartCfgDescrRx.hprot = 0;
        	DMA_CfgDescr(leuartRxChannel->ChannelId, true, &leuartCfgDescrRx);
        }
    }
    return ret;
}

/** @brief See public interface function description in BCDS_MCU_LEUART.h */
Retcode_T MCU_LEUART_Deinitialize(LEUART_T leuart)
{
    Retcode_T ret = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
	
	struct MCU_LEUART_Handle_S* leuart_ptr = (struct MCU_LEUART_Handle_S*) leuart;
    if (NULL != leuart_ptr)
    {
        ret = leuart_ptr->_DriverCtx.sendCancel(leuart_ptr);
        if (RETCODE_OK != ret)
        {
            Retcode_RaiseError(ret);
        }
        ret = leuart_ptr->_DriverCtx.receiveCancel(leuart_ptr);
        if (RETCODE_OK != ret)
        {
            Retcode_RaiseError(ret);
        }
        leuart_ptr->_DriverCtx.send = NULL;
        leuart_ptr->_DriverCtx.sendCancel = NULL;
        leuart_ptr->_DriverCtx.receive = NULL;
        leuart_ptr->_DriverCtx.receiveCancel = NULL;
        leuart_ptr->_DriverCtx.TxState = LEUART_STATE_INIT;
        leuart_ptr->_DriverCtx.RxState = LEUART_STATE_INIT;
        leuart_ptr->DmaRxCallback = NULL;
        leuart_ptr->DmaTxCallback = NULL;
        leuart_ptr->IrqRxCallback = NULL;
        leuart_ptr->IrqTxCallback = NULL;
        leuart_ptr->RtsCallback = NULL;
        leuart_ptr->CtsCallback = NULL;
        ret = RETCODE_OK;
    }
	
    return ret;
}

/** @brief See public interface function description in BCDS_MCU_LEUART.h */
Retcode_T MCU_LEUART_Send(LEUART_T leuart, uint8_t* data, uint32_t len)
{
    Retcode_T retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
	if ((NULL == leuart) || (NULL == data))
	{
		return retcode;
	}

	struct MCU_LEUART_Handle_S* leuart_ptr = (struct MCU_LEUART_Handle_S*) leuart;
	if (NULL != leuart_ptr->_DriverCtx.send)
	{
		if (0 != len)
		{
			retcode = leuart_ptr->_DriverCtx.send(leuart_ptr, data, len);
		}
		else
		{
			/* application wants to cancel ongoing sending which is signaled by len == 0*/
			retcode = leuart_ptr->_DriverCtx.sendCancel(leuart_ptr);
		}
	}
	return retcode;
}

/** @brief See public interface function description in BCDS_MCU_LEUART.h */
Retcode_T MCU_LEUART_Receive(LEUART_T leuart, uint8_t* data, uint32_t len)
{
    Retcode_T retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
	if ((NULL == data) || (NULL == leuart))
	{
		return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
	}

	struct MCU_LEUART_Handle_S* leuart_ptr = (struct MCU_LEUART_Handle_S*) leuart;
	if (NULL != leuart_ptr->_DriverCtx.receive)
	{
		if (0 != len)
		{
			retcode = leuart_ptr->_DriverCtx.receive(leuart_ptr, data, len);
		}
		else
		{
			/*  application wants to cancel ongoing reception which is signaled by len == 0
			 *  NOTE: In case DMA: reception is canceled. In case interrupt mode: only sending of RxComplete will be prevented.
			 *  */
			retcode = leuart_ptr->_DriverCtx.receiveCancel(leuart_ptr);
		}
	}
	return retcode;
}

/** @brief See public interface function description in BCDS_MCU_LEUART.h */
Retcode_T MCU_LEUART_GetRxCount(LEUART_T leuart, uint32_t * count)
{
    Retcode_T retcode = (RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM));
    if ((NULL != count) && (NULL != leuart))
    {
        struct MCU_LEUART_Handle_S* leuart_ptr = (struct MCU_LEUART_Handle_S*) leuart;
        if ( leuart_ptr->_DriverCtx.RxState != LEUART_STATE_INIT)
        {
            retcode = leuart_ptr->_DriverCtx.countRx(leuart_ptr, count);
        }
    }
    return retcode;
}

static Retcode_T LEUART_GetRxCountIntMode(struct MCU_LEUART_Handle_S* leuart_ptr, uint32_t * count)
{
    *count = leuart_ptr->_DriverCtx.RxXferSize - leuart_ptr->_DriverCtx.RxXferCount;
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
static Retcode_T LEUART_Send_Async(struct MCU_LEUART_Handle_S* leuart_ptr, uint8_t * data, uint32_t len)
{
    Retcode_T rc;
    struct MCU_DMA_Channel_S * dmaTxChannel = (struct MCU_DMA_Channel_S *)  leuart_ptr->Link1;

    if (leuart_ptr->_DriverCtx.TxState == LEUART_STATE_READY)
    {
        leuart_ptr->_DriverCtx.TxState = LEUART_STATE_RUNNING;
        leuart_ptr->_DriverCtx.pTxBuffPtr = data;
        leuart_ptr->_DriverCtx.TxXferSize = len;
        leuart_ptr->_DriverCtx.TxXferCount = 0;
        switch (leuart_ptr->TransferMode)
		{
			case BCDS_HAL_TRANSFER_MODE_INTERRUPT:
			case BCDS_HAL_TRANSFER_MODE_INTERRUPT_RX_DMA_TX:

				leuart_ptr->_DriverCtx.RxState = LEUART_STATE_READY;
				// Activate TX and clear TX buffer
				leuart_ptr->leuart_ptr->CMD = LEUART_CMD_CLEARTX;
				LEUART_Enable(leuart_ptr->leuart_ptr, leuartEnableTx);
				// Enable interrupt
				leuart_ptr->leuart_ptr->IEN |= LEUART_IEN_TXBL;
				rc = RETCODE_OK;
				break;
			case BCDS_HAL_TRANSFER_MODE_DMA:
			case BCDS_HAL_TRANSFER_MODE_INTERRUPT_TX_DMA_RX:
				leuart_ptr->_DriverCtx.RxState = LEUART_STATE_RUNNING;
				/* disable TXBL interrrupt */
				leuart_ptr->leuart_ptr->IEN &= ~(LEUART_IEN_TXBL);
				// Activate TX and clear TX buffer
				leuart_ptr->leuart_ptr->CMD = LEUART_CMD_TXEN | LEUART_CMD_CLEARTX;
				DMA_ActivateBasic(dmaTxChannel->ChannelId, true, false, (void*)&(leuart_ptr->leuart_ptr->TXDATA), data, (unsigned int) (len - 1));
				rc = RETCODE_OK;
				break;
			default:
				rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NOT_SUPPORTED);
				break;
		}
    }
    else if (leuart_ptr->_DriverCtx.TxState ==  LEUART_STATE_INIT)
    {
    	rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_UNINITIALIZED);
    }
    else
    {
        rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_HAL_LEUART_DRIVER_RX_NOT_READY);
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
static Retcode_T LEUART_Receive_Async(struct MCU_LEUART_Handle_S* leuart_ptr, uint8_t *rx_buffer, uint32_t rx_xfer_size)
{
	Retcode_T retcode = RETCODE_OK;
    // Check that a buffer has indeed been set up
	LEUART_TypeDef* leuartInstance = leuart_ptr->leuart_ptr;
	struct MCU_DMA_Channel_S * dmaRxChannel = (struct MCU_DMA_Channel_S *)  leuart_ptr->Link2;
	// Set up buffer
    if (leuart_ptr->_DriverCtx.RxState == LEUART_STATE_READY)
    {
        leuart_ptr->_DriverCtx.pRxBuffPtr = rx_buffer;
        leuart_ptr->_DriverCtx.RxXferSize = rx_xfer_size;
        leuart_ptr->_DriverCtx.RxXferCount = rx_xfer_size;
    	switch (leuart_ptr->TransferMode)
    	{
    		case BCDS_HAL_TRANSFER_MODE_INTERRUPT:
    		case BCDS_HAL_TRANSFER_MODE_INTERRUPT_RX_DMA_TX:
    			leuart_ptr->_DriverCtx.RxState = LEUART_STATE_RUNNING;
				// Activate RX and clear RX buffer
    			leuart_ptr->leuart_ptr->CMD = LEUART_CMD_CLEARRX;
    			LEUART_Enable(leuart_ptr->leuart_ptr, leuartEnableRx);
				//clear Flags that might be set
				leuartInstance->IFC = LEUART_IFC_PERR | LEUART_IFC_FERR | LEUART_IFC_RXOF;
				// Enable interrupt
				leuartInstance->IEN = LEUART_IEN_RXDATAV;

				retcode = RETCODE_OK;
				break;
    		case BCDS_HAL_TRANSFER_MODE_DMA:
    		case BCDS_HAL_TRANSFER_MODE_INTERRUPT_TX_DMA_RX:
    			leuart_ptr->_DriverCtx.RxState = LEUART_STATE_RUNNING;
    			/* disable RXDATA interrrupt */
    			leuartInstance->IEN &= ~(LEUART_IEN_RXDATAV);
				// Activate RX and clear RX buffer
				leuartInstance->CMD = LEUART_CMD_RXEN | LEUART_CMD_CLEARRX;

				DMA_ActivateBasic(dmaRxChannel->ChannelId, true, false, (void *)rx_buffer, (void*)&(leuartInstance->RXDATA), (unsigned int) (rx_xfer_size - 1));
				retcode = RETCODE_OK;
				break;
	        default:
	            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NOT_SUPPORTED);
	            break;
    	}
    }
    else
    {
        retcode =  RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_HAL_LEUART_DRIVER_RX_NOT_READY);
    }
    return retcode;
}

/**
 * @brief   Abort the ongoing TX transaction. It disables the enabled interrupt for TX and
 *          flush TX hardware buffer if TX FIFO is used
 *
 * @param[in]       element_ptr
 */
static Retcode_T LEUART_Abort_TX_IntMode(struct MCU_LEUART_Handle_S* leuart_ptr)
{
    leuart_ptr->_DriverCtx.TxState = LEUART_STATE_READY;
    LEUART_Enable(leuart_ptr->leuart_ptr, leuartDisable);
    /* disable interrupts */
    LEUART_IntDisable(leuart_ptr->leuart_ptr, LEUART_IEN_TXC);
    LEUART_IntClear(leuart_ptr->leuart_ptr, LEUART_IFC_TXC);
    return RETCODE_OK;
}

/**
 * @brief   Abort the ongoing RX transaction. It disables the enabled interrupt for RX and
 *          flush RX hardware buffer if RX FIFO is used
 *
 * @param[in]       element_ptr
 */
static Retcode_T LEUART_Abort_RX_IntMode(struct MCU_LEUART_Handle_S* leuart_ptr)
{
    leuart_ptr->_DriverCtx.RxState = LEUART_STATE_READY;
    /* Stop receiver */
    leuart_ptr->leuart_ptr->CMD |= LEUART_CMD_RXDIS;
    /* disable all interrupts */
    LEUART_IntDisable(leuart_ptr->leuart_ptr, LEUART_IEN_RXDATAV | LEUART_IEN_PERR | LEUART_IEN_FERR | LEUART_IEN_RXOF);
    /*clear all set interrupts*/
    LEUART_IntClear(leuart_ptr->leuart_ptr, LEUART_IFC_PERR | LEUART_IFC_FERR | LEUART_IFC_RXOF);
    return RETCODE_OK;
}

/*************************** ASYNC TRANSFER FUNCTIONS END **********/

#endif //-- BCDS_FEATURE_LEUART && BCDS_LEUART_COUNT
