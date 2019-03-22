/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include "hal_conf.h"
#include "aos/hal/uart.h"
#include "aos/errno.h"

#define LOG_TAG		"uart"
#define	SOC_USART_COUNT			4
#define UART_RING_BUFFER_SIZE	256

typedef struct{

	uint16_t		in;   /*write buffer count*/
	uint16_t		out;  /*read buffer count*/
	uint16_t		length;    /*buffer max index*/
	uint8_t		*p_buf;   /*buffer addr*/
}BUF_FIFO;

static const uint32_t s_uartBaseAddrs[SOC_USART_COUNT] = { UART0_BASE, UART1_BASE, UART2_BASE, UART3_BASE };
static uint8_t uart_ringbuffer[SOC_USART_COUNT][UART_RING_BUFFER_SIZE];
static kmutex_t uart_recv_lock[SOC_USART_COUNT];
static kmutex_t uart_send_lock[SOC_USART_COUNT];
static BUF_FIFO	uart_buf_fifo;
static BUF_FIFO	* pbuf_fifo = &uart_buf_fifo;

static void fifo_init(BUF_FIFO  *pfifo, uint8_t *pbuffer, uint16_t size)
{
	pfifo->in = 0;
	pfifo->out = 0;
	pfifo->p_buf = pbuffer;
	pfifo->length = size;		
}



/*fifo write in*/
static void fifo_in(BUF_FIFO *pfifo,uint8_t *pbuffer,uint16_t size)
{
	uint16_t  i;

	for(i = 0; i < size; i ++)
	{
		*(pfifo->p_buf + pfifo->in) = *(pbuffer +i);
		pfifo->in ++;
		pfifo->in %= pfifo->length;
	}
}

/*fifo read out*/
static void fifo_out(BUF_FIFO *pfifo, uint16_t size)
{
	uint16_t  i;

	for(i = 0; i < size; i ++)
	{
		pfifo->out ++;
		pfifo->out %= pfifo->length;
	}	
}

static uint16_t fifo_query(BUF_FIFO *pfifo)
{
	uint16_t temp_head;
	uint16_t temp_tail;
	uint16_t datalength;

	temp_head = pfifo->in;
	temp_tail = pfifo->out;

	if(temp_head > temp_tail)
	{
		datalength = temp_head - temp_tail;
	}
	else
	{
		datalength = temp_head + pfifo->length - temp_tail;
	}

	return datalength;
}
/**
 * Initialises a UART interface
 *
 *
 * @param[in]  uart  the interface which should be initialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_uart_init(uart_dev_t *uart)
{
	UART_InitStructure UART_initStruct;

	PORT_Init(PORTA, PIN2, FUNMUX0_UART0_RXD, 1);	//GPIOA.2配置为UART0输入引脚
	PORT_Init(PORTA, PIN3, FUNMUX1_UART0_TXD, 0);	//GPIOA.3配置为UART0输出引脚
	
	UART_initStruct.Baudrate = uart->config.baud_rate;
	UART_initStruct.RXThreshold = 0;
	UART_initStruct.RXThresholdIEn = 0;
	UART_initStruct.TXThresholdIEn = 0;
	UART_initStruct.TimeoutIEn = 0;

	
	if (uart->port != 0) {

	}
 	else
	{
		fifo_init(pbuf_fifo, uart_ringbuffer[uart->port], UART_RING_BUFFER_SIZE);//暂时只支持串口0的操作
	}
	
	switch(uart->config.parity)
	{
		case NO_PARITY:
			UART_initStruct.Parity = UART_PARITY_NONE;	
			break;

		case ODD_PARITY:
			UART_initStruct.Parity = UART_PARITY_ODD;	
			break;

		case EVEN_PARITY:
			UART_initStruct.Parity = UART_PARITY_EVEN;	
			break;

		default:
			return -EIO;
	}

	switch(uart->config.data_width)
	{
		case DATA_WIDTH_8BIT:
			UART_initStruct.DataBits = UART_DATA_8BIT;
			break;
		
		case DATA_WIDTH_9BIT:
			UART_initStruct.DataBits = UART_DATA_9BIT;
			break;

		default:
			return -EIO;
	}

	switch(uart->config.stop_bits)
	{
		case STOP_BITS_1:
			UART_initStruct.StopBits = UART_STOP_1BIT;
			break;
			
		case STOP_BITS_2:
			UART_initStruct.StopBits = UART_STOP_2BIT;
			break;
			
		default:
			return -EIO;
	}

	UART_Init((UART_TypeDef *) s_uartBaseAddrs[uart->port], &UART_initStruct);


	if (krhino_mutex_create(&uart_send_lock[uart->port], "uartSendLock"))
	{
		return -EIO;
	}

	if (krhino_mutex_create(&uart_recv_lock[uart->port], "uartRecvLock"))
	{
		return -EIO;
	}

	if(0==uart->port)
	{
		UART_Open(UART0);
	}
	else if(1==uart->port)
	{
		UART_Open(UART1);
	}
	else if(1==uart->port)
	{
		UART_Open(UART2);
	}
	else if(1==uart->port)
	{
		UART_Open(UART3);
	}
	else
	{

	}
	
	return 0;
}

/**
 * Transmit data on a UART interface
 *
 * @param[in]  uart  the UART interface
 * @param[in]  data  pointer to the start of data
 * @param[in]  size  number of bytes to transmit
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
	kstat_t stat = RHINO_SUCCESS;

	if (timeout == 0)
	{
		timeout = 1000;
	}

	stat = krhino_mutex_lock(&uart_send_lock[uart->port], AOS_WAIT_FOREVER);
	
	if (stat != RHINO_SUCCESS)
	{
		return stat;
	}

	for (int i = 0; i < size; i++)
	{
		UART_WriteByte((UART_TypeDef *) s_uartBaseAddrs[uart->port], ((uint8_t *)data)[i]);
		while(UART_IsTXBusy(UART0));
	}

	krhino_mutex_unlock(&uart_send_lock[uart->port]);
	
	if (stat != RHINO_SUCCESS)
	{
		return stat;
	}

	return stat;
}

/**
 * Receive data on a UART interface
 *
 * @param[in]   uart         the UART interface
 * @param[out]  data         pointer to the buffer which will store incoming data
 * @param[in]   expect_size  number of bytes to receive
 * @param[in]   timeout      timeout in milisecond
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size,
                         uint32_t *recv_size, uint32_t timeout)
{

	kstat_t stat = RHINO_SUCCESS;

	stat = krhino_mutex_lock(&uart_recv_lock[uart->port], AOS_WAIT_FOREVER);

	if (stat != RHINO_SUCCESS)
	{
		return stat;
	}

	while(1)
	{
		uint16_t query_size = fifo_query(pbuf_fifo);

		if(query_size>=expect_size)
		{
			break;
		}
	}

  	
	for(uint16_t i = 0;i < expect_size;i++)
	{
		*((uint8_t *)data + i) = pbuf_fifo->p_buf[(pbuf_fifo->out + i) % pbuf_fifo->length];
	}

	fifo_out(pbuf_fifo, expect_size);
	
	krhino_mutex_unlock(&uart_recv_lock[uart->port]);

	if (stat != RHINO_SUCCESS)
	{
		return stat;
	}

	return stat;
}

/**
 * Deinitialises a UART interface
 *
 * @param[in]  uart  the interface which should be deinitialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_uart_finalize(uart_dev_t *uart)
{
	UART_Close((UART_TypeDef *) s_uartBaseAddrs[uart->port]);
	krhino_mutex_del(&uart_send_lock[uart->port]);
	krhino_mutex_del(&uart_recv_lock[uart->port]);

	return 0;
}

void UART0_Handler(void)
{
	uint32_t chr;
	
	krhino_intrpt_enter();
	
	if(UART_INTRXThresholdStat(UART0) || UART_INTTimeoutStat(UART0))
	{
		while(UART_IsRXFIFOEmpty(UART0) == 0)
		{
			if(UART_ReadByte(UART0, &chr) == 0)
			{
				fifo_in(pbuf_fifo, (uint8_t *)&chr, 1);
			}
		}
	}

	krhino_intrpt_exit();
}
