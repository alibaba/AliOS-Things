/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include "aos/hal/uart.h"
#include "hal_uart_xmc4000.h"
#include "xmc_uart.h"
#include "xmc_dma.h"
#include "xmc_gpio.h"

XMC_DMA_CH_CONFIG_t dma_ch_config=
{
	.block_size = MAX_BUF_UART_DMA_BYTES,
	.src_transfer_width = XMC_DMA_CH_TRANSFER_WIDTH_8,
	.dst_transfer_width = XMC_DMA_CH_TRANSFER_WIDTH_8,
	.src_address_count_mode = XMC_DMA_CH_ADDRESS_COUNT_MODE_NO_CHANGE,
	.dst_address_count_mode = XMC_DMA_CH_ADDRESS_COUNT_MODE_INCREMENT,
	.src_burst_length = XMC_DMA_CH_BURST_LENGTH_1,
	.dst_burst_length = XMC_DMA_CH_BURST_LENGTH_1,
	.transfer_flow = XMC_DMA_CH_TRANSFER_FLOW_P2M_DMA,
	.transfer_type = XMC_DMA_CH_TRANSFER_TYPE_MULTI_BLOCK_SRCADR_RELOAD_DSTADR_RELOAD,
	.src_handshaking = XMC_DMA_CH_SRC_HANDSHAKING_HARDWARE,
	.enable_interrupt = false
};

XMC_GPIO_CONFIG_t uart_tx =
{
  .output_level = XMC_GPIO_OUTPUT_LEVEL_HIGH,
  .output_strength = XMC_GPIO_OUTPUT_STRENGTH_MEDIUM
};

XMC_GPIO_CONFIG_t uart_rx =
{
  .mode = XMC_GPIO_MODE_INPUT_PULL_UP
};

static UART_MAPPING* GetUARTMappingFromChannel(XMC_USIC_CH_t *const channel);
static UART_MAPPING* GetUARTMappingFromPort(const PORT_UART_TYPE port);
void HAL_UART_IRQHandler(XMC_USIC_CH_t *const channel);
int32_t uart_dataWidth_transform(hal_uart_data_width_t data_width_hal, uint32_t *data_width_xmc4000);
int32_t uart_parity_transform(hal_uart_parity_t parity_hal, uint32_t *parity_xmc4000);
int32_t uart_stop_bits_transform(hal_uart_stop_bits_t stop_bits_hal, uint32_t *stop_bits_xmc4000);
int32_t uart_flow_control_transform(hal_uart_flow_control_t flow_control_hal);
int32_t uart_mode_transform(hal_uart_mode_t mode_hal);

typedef struct {
  aos_mutex_t uart_tx_mutex;
  aos_mutex_t uart_rx_mutex;
  aos_sem_t uart_rx_sem;
  uint16_t  uart_rx_in;
  uint16_t  uart_rx_out;
  uint16_t  uart_rx_dma_in;
  uint8_t   inited;
  char*      UartRxBuf;
  char*      UartRxDmaBuf;
}xmc4000_uart_t;

static xmc4000_uart_t xmc4000_uart[PORT_UART_SIZE];

extern uint32_t ticks;

//Get UART Instanse & attribute from Channel
static UART_MAPPING* GetUARTMappingFromChannel(XMC_USIC_CH_t *const channel)
{
    int8_t i = 0;
    UART_MAPPING* rc = NULL;
    for(i=0; i<PORT_UART_SIZE; i++)
    {
        if(UART_MAPPING_TABLE[i].uartPhyP == channel)
        {
            rc = &UART_MAPPING_TABLE[i];
            break;
        }
    }
    return rc;
}

//Get UART Instanse & attribute from Logical Port
static UART_MAPPING* GetUARTMappingFromPort(const PORT_UART_TYPE port)
{
    int8_t i = 0;
    UART_MAPPING* rc = NULL;
    for(i=0; i<PORT_UART_SIZE; i++)
    {
        if(UART_MAPPING_TABLE[i].uartFuncP == port)
        {
            rc = &UART_MAPPING_TABLE[i];
            break;
        }
    }
    return rc;
}

void USIC0_1_IRQHandler(void)
{
   krhino_intrpt_enter();
   HAL_UART_IRQHandler(XMC_UART0_CH1);
   krhino_intrpt_exit();
}

void USIC1_1_IRQHandler(void)
{
   krhino_intrpt_enter();
   HAL_UART_IRQHandler(XMC_UART1_CH1);
   krhino_intrpt_exit();
}

void USIC2_0_IRQHandler(void)
{
   krhino_intrpt_enter();
   HAL_UART_IRQHandler(XMC_UART2_CH0);
   krhino_intrpt_exit();
}

void USIC2_1_IRQHandler(void)
{
   krhino_intrpt_enter();
   HAL_UART_IRQHandler(XMC_UART2_CH1);
   krhino_intrpt_exit();
}

void HAL_UART_IRQHandler(XMC_USIC_CH_t *const channel)
{
	uint8_t data;

    UART_MAPPING* uartIns = GetUARTMappingFromChannel(channel);
    if(NULL == uartIns)
    {
        return;
    }

	while((channel->RBUFSR&0x6000)!=0)
	{
		data = XMC_UART_CH_GetReceivedData(channel);
    	if(xmc4000_uart[uartIns->uartFuncP].inited!=0)
    	{
    	    xmc4000_uart[uartIns->uartFuncP].UartRxBuf[xmc4000_uart[uartIns->uartFuncP].uart_rx_in++] = data;
    	    if(xmc4000_uart[uartIns->uartFuncP].uart_rx_in==MAX_BUF_UART_BYTES)
    	    {
    	        xmc4000_uart[uartIns->uartFuncP].uart_rx_in=0;
    	    }
    	    aos_sem_signal(&xmc4000_uart[uartIns->uartFuncP].uart_rx_sem);
    	}
    }
}

void HAL_UART_DMA_IRQHandler(void)
{
	uint8_t i;
	uint16_t rx_cnt;

    for(i=0; i<2; i++)
	{
    	if(xmc4000_uart[i].inited!=0)
    	{
			rx_cnt=XMC_DMA0->CH[i].CTLH&0x0FFF;
			if(rx_cnt==MAX_BUF_UART_DMA_BYTES)
			{
				rx_cnt=0;
			}
    		while(xmc4000_uart[i].uart_rx_dma_in!=rx_cnt)
    		{
				xmc4000_uart[i].UartRxBuf[xmc4000_uart[i].uart_rx_in++]=xmc4000_uart[i].UartRxDmaBuf[xmc4000_uart[i].uart_rx_dma_in++];
				if(xmc4000_uart[i].uart_rx_in==MAX_BUF_UART_BYTES)
				{
					xmc4000_uart[i].uart_rx_in=0;
				}
				if(xmc4000_uart[i].uart_rx_dma_in==MAX_BUF_UART_DMA_BYTES)
				{
					xmc4000_uart[i].uart_rx_dma_in=0;
				}
				if(xmc4000_uart[i].uart_rx_in==xmc4000_uart[i].uart_rx_out)
				{
					printf("uart buffer full! %d %d\r\n", rx_cnt, xmc4000_uart[i].uart_rx_in);
				}
    			aos_sem_signal(&xmc4000_uart[i].uart_rx_sem);
    		}
    	}
	}
}

int32_t hal_uart_init(uart_dev_t *uart)
{
    int32_t ret = -1;
    XMC_UART_CH_CONFIG_t uart_config;

    if (uart == NULL) {
        return -1;
    }
    UART_MAPPING* uartIns = GetUARTMappingFromPort(uart->port);
    if( NULL== uartIns ){ //no found this port in function-physical uartIns, no need initialization
        return -1;
    }
    memset(&xmc4000_uart[uart->port],0,sizeof(xmc4000_uart_t));

    uart_config.frame_length  = 0;
    uart_config.oversampling  = uartIns->attr.overSampling;
    uart_config.baudrate  = uart->config.baud_rate;
    ret = uart_dataWidth_transform(uart->config.data_width, &uart_config.data_bits);
    ret |= uart_parity_transform(uart->config.parity, &uart_config.parity_mode);
    ret |= uart_stop_bits_transform(uart->config.stop_bits, &uart_config.stop_bits);
    ret |= uart_flow_control_transform(uart->config.flow_control);
    ret |= uart_mode_transform(uart->config.mode);
    if (ret) {
        printf("invalid uart data \r\n");
    }

    if(NULL == xmc4000_uart[uart->port].UartRxBuf){
        xmc4000_uart[uart->port].UartRxBuf = aos_malloc(MAX_BUF_UART_BYTES);
        xmc4000_uart[uart->port].UartRxDmaBuf = aos_malloc(MAX_BUF_UART_DMA_BYTES);
    }

    if (NULL == xmc4000_uart[uart->port].UartRxBuf) {
        printf("Fail to malloc memory size %d at %s %d \r\d", MAX_BUF_UART_BYTES, __FILE__, __LINE__);
        return -1;
    }
    memset(xmc4000_uart[uart->port].UartRxBuf, 0, MAX_BUF_UART_BYTES);
    memset(xmc4000_uart[uart->port].UartRxDmaBuf, 0, MAX_BUF_UART_DMA_BYTES);

    switch ((uint32_t)(uartIns->uartPhyP))
		{
        case (uint32_t)XMC_UART0_CH0:		//DMA
			dma_ch_config.src_addr = (uint32_t)&XMC_UART0_CH0->RBUF;
			dma_ch_config.dst_addr = xmc4000_uart[uart->port].UartRxDmaBuf;
			dma_ch_config.src_peripheral_request = DMA0_PERIPHERAL_REQUEST_USIC0_SR0_0;
			XMC_DMA_Init(XMC_DMA0);
			XMC_DMA_CH_Init(XMC_DMA0, 0, &dma_ch_config);
			XMC_DMA_CH_Enable(XMC_DMA0, 0);
			XMC_UART_CH_Init(uartIns->uartPhyP, &uart_config);
			XMC_UART_CH_SetInputSource(uartIns->uartPhyP, XMC_UART_CH_INPUT_RXD, USIC0_C0_DX0_P1_4);
			XMC_UART_CH_Start(uartIns->uartPhyP);
			uart_tx.mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT2;
			XMC_GPIO_Init(P1_5,&uart_tx);
			XMC_GPIO_Init(P1_4,&uart_rx);
			XMC_USIC_CH_EnableEvent(uartIns->uartPhyP, XMC_USIC_CH_EVENT_STANDARD_RECEIVE);
			XMC_USIC_CH_SetInterruptNodePointer(uartIns->uartPhyP, XMC_USIC_CH_INTERRUPT_NODE_POINTER_RECEIVE, 0);
			XMC_USIC_CH_EnableEvent(uartIns->uartPhyP, XMC_USIC_CH_EVENT_ALTERNATIVE_RECEIVE);
			XMC_USIC_CH_SetInterruptNodePointer(uartIns->uartPhyP, XMC_USIC_CH_INTERRUPT_NODE_POINTER_ALTERNATE_RECEIVE, 0);
			break;

        case (uint32_t)XMC_UART0_CH1:
			XMC_UART_CH_Init(uartIns->uartPhyP, &uart_config);
			XMC_UART_CH_SetInputSource(uartIns->uartPhyP, XMC_UART_CH_INPUT_RXD, USIC0_C1_DX0_P6_3);
			XMC_UART_CH_Start(uartIns->uartPhyP);
			uart_tx.mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT2;
			XMC_GPIO_Init(P6_4,&uart_tx);
			XMC_GPIO_Init(P6_3,&uart_rx);
			XMC_USIC_CH_EnableEvent(uartIns->uartPhyP, XMC_USIC_CH_EVENT_STANDARD_RECEIVE);
			XMC_USIC_CH_SetInterruptNodePointer(uartIns->uartPhyP, XMC_USIC_CH_INTERRUPT_NODE_POINTER_RECEIVE, 1);
			XMC_USIC_CH_EnableEvent(uartIns->uartPhyP, XMC_USIC_CH_EVENT_ALTERNATIVE_RECEIVE);
			XMC_USIC_CH_SetInterruptNodePointer(uartIns->uartPhyP, XMC_USIC_CH_INTERRUPT_NODE_POINTER_ALTERNATE_RECEIVE, 1);
			NVIC_SetPriority(USIC0_1_IRQn,2);
			NVIC_EnableIRQ(USIC0_1_IRQn);
			break;

        case (uint32_t)XMC_UART1_CH0:		//DMA
			dma_ch_config.src_addr = (uint32_t)&XMC_UART1_CH0->RBUF;
			dma_ch_config.dst_addr = xmc4000_uart[uart->port].UartRxDmaBuf;
			dma_ch_config.src_peripheral_request = DMA0_PERIPHERAL_REQUEST_USIC1_SR0_1;
			XMC_DMA_Init(XMC_DMA0);
			XMC_DMA_CH_Init(XMC_DMA0, 1, &dma_ch_config);
			XMC_DMA_CH_Enable(XMC_DMA0, 1);
			XMC_UART_CH_Init(uartIns->uartPhyP, &uart_config);
			XMC_UART_CH_SetInputSource(uartIns->uartPhyP, XMC_UART_CH_INPUT_RXD, USIC1_C0_DX0_P2_15);
			XMC_UART_CH_Start(uartIns->uartPhyP);
			uart_tx.mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT2;
			XMC_GPIO_Init(P2_14,&uart_tx);
			XMC_GPIO_Init(P2_15,&uart_rx);
			XMC_USIC_CH_EnableEvent(uartIns->uartPhyP, XMC_USIC_CH_EVENT_STANDARD_RECEIVE);
			XMC_USIC_CH_SetInterruptNodePointer(uartIns->uartPhyP, XMC_USIC_CH_INTERRUPT_NODE_POINTER_RECEIVE, 0);
			XMC_USIC_CH_EnableEvent(uartIns->uartPhyP, XMC_USIC_CH_EVENT_ALTERNATIVE_RECEIVE);
			XMC_USIC_CH_SetInterruptNodePointer(uartIns->uartPhyP, XMC_USIC_CH_INTERRUPT_NODE_POINTER_ALTERNATE_RECEIVE, 0);
			break;

        case (uint32_t)XMC_UART1_CH1:
			XMC_UART_CH_Init(uartIns->uartPhyP, &uart_config);
			XMC_UART_CH_SetInputSource(uartIns->uartPhyP, XMC_UART_CH_INPUT_RXD, USIC1_C1_DX0_P0_0);
			XMC_UART_CH_Start(uartIns->uartPhyP);
			uart_tx.mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT2;
			XMC_GPIO_Init(P0_1,&uart_tx);
			XMC_GPIO_Init(P0_0,&uart_rx);
			XMC_USIC_CH_EnableEvent(uartIns->uartPhyP, XMC_USIC_CH_EVENT_STANDARD_RECEIVE);
			XMC_USIC_CH_SetInterruptNodePointer(uartIns->uartPhyP, XMC_USIC_CH_INTERRUPT_NODE_POINTER_RECEIVE, 1);
			XMC_USIC_CH_EnableEvent(uartIns->uartPhyP, XMC_USIC_CH_EVENT_ALTERNATIVE_RECEIVE);
			XMC_USIC_CH_SetInterruptNodePointer(uartIns->uartPhyP, XMC_USIC_CH_INTERRUPT_NODE_POINTER_ALTERNATE_RECEIVE, 1);
			NVIC_SetPriority(USIC1_1_IRQn,2);
			NVIC_EnableIRQ(USIC1_1_IRQn);
			break;

        case (uint32_t)XMC_UART2_CH0:
			XMC_UART_CH_Init(uartIns->uartPhyP, &uart_config);
			XMC_UART_CH_SetInputSource(uartIns->uartPhyP, XMC_UART_CH_INPUT_RXD, USIC2_C0_DX0_P5_1);
			XMC_UART_CH_Start(uartIns->uartPhyP);
			uart_tx.mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT1;
			XMC_GPIO_Init(P5_0,&uart_tx);
			XMC_GPIO_Init(P5_1,&uart_rx);
			XMC_USIC_CH_EnableEvent(uartIns->uartPhyP, XMC_USIC_CH_EVENT_STANDARD_RECEIVE);
			XMC_USIC_CH_SetInterruptNodePointer(uartIns->uartPhyP, XMC_USIC_CH_INTERRUPT_NODE_POINTER_RECEIVE, 0);
			XMC_USIC_CH_EnableEvent(uartIns->uartPhyP, XMC_USIC_CH_EVENT_ALTERNATIVE_RECEIVE);
			XMC_USIC_CH_SetInterruptNodePointer(uartIns->uartPhyP, XMC_USIC_CH_INTERRUPT_NODE_POINTER_ALTERNATE_RECEIVE, 0);
			NVIC_SetPriority(USIC2_0_IRQn,2);
			NVIC_EnableIRQ(USIC2_0_IRQn);
			break;

        case (uint32_t)XMC_UART2_CH1:
            XMC_UART_CH_Init(uartIns->uartPhyP, &uart_config);
            XMC_UART_CH_SetInputSource(uartIns->uartPhyP, XMC_UART_CH_INPUT_RXD, USIC2_C1_DX0_P3_4);
		    XMC_UART_CH_Start(uartIns->uartPhyP);
            uart_tx.mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT1;
		    XMC_GPIO_Init(P3_11,&uart_tx);
		    XMC_GPIO_Init(P3_4,&uart_rx);
            XMC_USIC_CH_EnableEvent(uartIns->uartPhyP, XMC_USIC_CH_EVENT_STANDARD_RECEIVE);
            XMC_USIC_CH_SetInterruptNodePointer(uartIns->uartPhyP, XMC_USIC_CH_INTERRUPT_NODE_POINTER_RECEIVE, 1);
            XMC_USIC_CH_EnableEvent(uartIns->uartPhyP, XMC_USIC_CH_EVENT_ALTERNATIVE_RECEIVE);
            XMC_USIC_CH_SetInterruptNodePointer(uartIns->uartPhyP, XMC_USIC_CH_INTERRUPT_NODE_POINTER_ALTERNATE_RECEIVE, 1);
            NVIC_SetPriority(USIC2_1_IRQn,2);
            NVIC_EnableIRQ(USIC2_1_IRQn);
			break;

        default:
            printf("uart %d invalid\r\n", uart->port);
            return -1;
    }

    aos_mutex_new(&xmc4000_uart[uart->port].uart_tx_mutex);
    aos_mutex_new(&xmc4000_uart[uart->port].uart_rx_mutex);
    aos_sem_new(&xmc4000_uart[uart->port].uart_rx_sem, 0);
    xmc4000_uart[uart->port].inited = 1;
    return ret;
}

int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
	int ret = 0;
	uint16_t i;
	uint8_t *p;
	char ch;
	uint32_t tickstart = 0;

    if ((uart == NULL) || (data == NULL)) {
        return -1;
    }

    UART_MAPPING* uartIns = GetUARTMappingFromPort(uart->port);
    if (uartIns == NULL) {
        return -1;
    }
    if(xmc4000_uart[uart->port].inited!=1)
    {
        int a = xmc4000_uart[uart->port].inited;
        return -1;
    }

    aos_mutex_lock(&xmc4000_uart[uart->port].uart_tx_mutex, AOS_WAIT_FOREVER);

	p=(uint8_t *)data;
	tickstart=ticks;
	for(i=0; i<size; i++)
	{
		ch = *p;
		p++;

		if((((XMC_USIC_CH_t *)(uartIns->uartPhyP))->TBCTR & USIC_CH_TBCTR_SIZE_Msk)==0)
		{
			while((((XMC_USIC_CH_t *)(uartIns->uartPhyP))->TCSR & USIC_CH_TCSR_TDV_Msk) == XMC_USIC_CH_TBUF_STATUS_BUSY)
			{
				if(timeout>0)
				{
					if(ticks-tickstart>=timeout)
					{
						ret = -1;
					}
				}
			}
			XMC_UART_CH_Transmit(uartIns->uartPhyP, ch);
		}
		else
		{
			XMC_UART_CH_Transmit(uartIns->uartPhyP, ch);
		}
	}
    aos_mutex_unlock(&xmc4000_uart[uart->port].uart_tx_mutex);

    return ret;
}

int32_t hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size,
                      uint32_t *recv_size, uint32_t timeout)
{
    uint8_t *pdata = (uint8_t *)data;
    uint32_t rx_count = 0;
    int32_t ret = -1;

    if ((uart == NULL) || (data == NULL)) {
        return -1;
    }

    aos_mutex_lock(&xmc4000_uart[uart->port].uart_rx_mutex, AOS_WAIT_FOREVER);

    while ( rx_count < expect_size )
    {
        while(xmc4000_uart[uart->port].uart_rx_out != xmc4000_uart[uart->port].uart_rx_in)
        {
            pdata[rx_count++] = xmc4000_uart[uart->port].UartRxBuf[xmc4000_uart[uart->port].uart_rx_out++];
            if(xmc4000_uart[uart->port].uart_rx_out == MAX_BUF_UART_BYTES)
            {
                xmc4000_uart[uart->port].uart_rx_out = 0;
            }

            if(rx_count==expect_size)
            {
                break;
            }
        }
        if(rx_count==expect_size)
        {
            break;
        }

        if(RHINO_SUCCESS==aos_sem_wait(&xmc4000_uart[uart->port].uart_rx_sem, timeout))
        {
            while(xmc4000_uart[uart->port].uart_rx_out != xmc4000_uart[uart->port].uart_rx_in)
            {
                pdata[rx_count++] = xmc4000_uart[uart->port].UartRxBuf[xmc4000_uart[uart->port].uart_rx_out++];
                if(xmc4000_uart[uart->port].uart_rx_out == MAX_BUF_UART_BYTES)
                {
                    xmc4000_uart[uart->port].uart_rx_out = 0;
                }

                if(rx_count==expect_size)
                {
                    break;
                }
            }
        }
        else
        {
            break;
        }
    }

    if (recv_size != NULL)
    {
        *recv_size = rx_count;
    }

    if(rx_count != 0)
    {
        ret = 0;
    }
    else
    {
        ret = -1;
    }
    aos_mutex_unlock(&xmc4000_uart[uart->port].uart_rx_mutex);

    return ret;
}

int32_t hal_uart_finalize(uart_dev_t *uart)
{
    if (uart == NULL) {
        return -1;
    }

    UART_MAPPING* uartIns = GetUARTMappingFromPort(uart->port);
    if(XMC_UART_CH_STATUS_BUSY==XMC_UART_CH_Stop(uartIns->uartPhyP))
    {
        return -1;
		}

    if(NULL != xmc4000_uart[uart->port].UartRxBuf){
        free(xmc4000_uart[uart->port].UartRxBuf);
        free(xmc4000_uart[uart->port].UartRxDmaBuf);
        xmc4000_uart[uart->port].UartRxBuf = NULL;
    }

    if (aos_mutex_is_valid(&xmc4000_uart[uart->port].uart_tx_mutex)) {
        aos_mutex_free(&xmc4000_uart[uart->port].uart_tx_mutex);
    }

    if (aos_sem_is_valid(&xmc4000_uart[uart->port].uart_rx_sem)) {
        aos_sem_free(&xmc4000_uart[uart->port].uart_rx_sem);
    }

    if (aos_mutex_is_valid(&xmc4000_uart[uart->port].uart_rx_mutex)) {
        aos_mutex_free(&xmc4000_uart[uart->port].uart_rx_mutex);
    }

    return 0;
}

int32_t uart_dataWidth_transform(hal_uart_data_width_t data_width_hal,
        uint32_t *data_width_xmc4000)
{
    uint32_t data_width = 0;
    int32_t ret = 0;

    if(data_width_hal == DATA_WIDTH_5BIT)
    {
        data_width = 5;
    }
    else if(data_width_hal == DATA_WIDTH_6BIT)
    {
        data_width = 6;
    }
    else if(data_width_hal == DATA_WIDTH_7BIT)
    {
        data_width = 7;
    }
    else if(data_width_hal == DATA_WIDTH_8BIT)
    {
        data_width = 8;
    }
    else if(data_width_hal == DATA_WIDTH_9BIT)
    {
        data_width = 9;
    }
    else
    {
        ret = -1;
    }

    if(ret == 0)
    {
        *data_width_xmc4000 = data_width;
    }

    return ret;
}

int32_t uart_parity_transform(hal_uart_parity_t parity_hal,
        uint32_t *parity_xmc4000)
{
    uint32_t parity = 0;
    int32_t ret = 0;

    if(parity_hal == NO_PARITY)
    {
        parity = XMC_USIC_CH_PARITY_MODE_NONE;
    }
    else if(parity_hal == ODD_PARITY)
    {
        parity = XMC_USIC_CH_PARITY_MODE_ODD;
    }
    else if(parity_hal == EVEN_PARITY)
    {
        parity = XMC_USIC_CH_PARITY_MODE_EVEN;
    }
    else
    {
        ret = -1;
    }

    if(ret == 0)
    {
        *parity_xmc4000 = parity;
    }

    return ret;
}

int32_t uart_stop_bits_transform(hal_uart_stop_bits_t stop_bits_hal,
        uint32_t *stop_bits_xmc4000)
{
    uint32_t stop_bits = 0;
    int32_t ret = 0;

    if(stop_bits_hal == STOP_BITS_1)
    {
        stop_bits = 1;
    }
    else if(stop_bits_hal == STOP_BITS_2)
    {
        stop_bits = 2;
    }
    else
    {
        ret = -1;
    }

    if(ret == 0)
    {
        *stop_bits_xmc4000 = stop_bits;
    }

    return ret;
}

int32_t uart_flow_control_transform(hal_uart_flow_control_t flow_control_hal)
{
	if(flow_control_hal == FLOW_CONTROL_DISABLED)
	{
	    return 0;
	}
	else
	{
		return -1;
	}
}

int32_t uart_mode_transform(hal_uart_mode_t mode_hal)
{
    if(mode_hal == MODE_TX)
    {
    	return 0;
    }
    else if(mode_hal == MODE_RX)
    {
    	return 0;
    }
    else if(mode_hal == MODE_TX_RX)
    {
    	return 0;
    }
    else
    {
        return -1;
    }
}
