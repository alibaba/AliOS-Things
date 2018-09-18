#include <board.h>

#include "hal_uart.h"
#include <aos/types.h>
#include <k_api.h>
#include "hal/soc/soc.h"

#define DIV_ROUND_CLOSEST(x, divisor)(			\
{							\
	typeof(divisor) __divisor = divisor;		\
	(((x) + ((__divisor) / 2)) / (__divisor));	\
}							\
)


uart_dev_t uart_0;
kbuf_queue_t g_uart0_recv_queue;
unsigned char g_buf_uart0[MAX_BUF_UART_BYTES];  

typedef struct uart_priv{
	unsigned int base;
	unsigned int irq;
	kbuf_queue_t *recv_queue;
}uart_priv_t;

uart_priv_t uart0_priv;


static void os_hw_uart_isr(int irq, void *param)
{
    uart_priv_t *priv = (uart_priv_t *)param;
	unsigned int status = 0;
	unsigned char ch = 0;

	status = readl(priv->base + UART011_MIS);
	if (status & (UART011_RTIM | UART011_RXIM))
	{

		while(!(readl(priv->base + UART011_FR) & UART011_FR_RXFE))
		{
			ch = readb(priv->base + UART011_DR);
			if (priv->recv_queue != NULL)
			{    
				krhino_buf_queue_send(priv->recv_queue, &ch, 1);
			}    
		}
	}
	writel(0xffff, priv->base + UART011_ICR);
}



int uart0_init(void)
{
	uart_0.port = PORT_UART0;
	uart_0.config.baud_rate = 115200;
	uart_0.config.data_width = DATA_WIDTH_8BIT;
	uart_0.config.flow_control = FLOW_CONTROL_DISABLED;
	uart_0.config.parity = NO_PARITY;
	uart_0.config.stop_bits = STOP_BITS_1;
	uart_0.priv = &uart0_priv;
	uart0_priv.base = XM510_UART0_BASE;
    uart0_priv.irq  = IRQ_XM510_UART0;
	uart0_priv.recv_queue = &g_uart0_recv_queue;
	krhino_buf_queue_create(&g_uart0_recv_queue, "uart0_recv_queue", g_buf_uart0, MAX_BUF_UART_BYTES, 1);

	hal_uart_init(&uart_0);
}


int32_t hal_uart_init(uart_dev_t *uart)
{
	uint32_t quot, lcr_h, cr;
	
    uart_priv_t *priv = (uart_priv_t *)uart->priv;

	/* first, disable everything */
	writew(0, priv->base + UART011_CR);

	quot = DIV_ROUND_CLOSEST(24000000 * 4, uart->config.baud_rate);

	/* Set baud rate */
	writew(quot & 0x3f,priv->base + UART011_FBRD);
	writew(quot >> 6, priv->base + UART011_IBRD);

	/*
	 * ----------v----------v----------v----------v-----
	 * NOTE: lcrh_tx and lcrh_rx MUST BE WRITTEN AFTER
	 * UART011_FBRD & UART011_IBRD.
	 * ----------^----------^----------^----------^-----
	 */
 	switch (uart->config.data_width) {
	case DATA_WIDTH_5BIT:
		lcr_h = UART011_LCRH_WLEN_5;
		break;
	case DATA_WIDTH_6BIT:
		lcr_h = UART011_LCRH_WLEN_6;
		break;
	case DATA_WIDTH_7BIT:
		lcr_h = UART011_LCRH_WLEN_7;
		break;
	default: // CS8
		lcr_h = UART011_LCRH_WLEN_8;
		break;
	}
	if (uart->config.stop_bits == STOP_BITS_2)//2bit停止位
		lcr_h |= UART011_LCRH_STP2;
	if (uart->config.parity != NO_PARITY) {
		lcr_h |= UART011_LCRH_PEN;
		if (uart->config.parity  ==  EVEN_PARITY)//偶校验
			lcr_h |= UART011_LCRH_EPS;
	}
	//fifo使能
	lcr_h |= UART011_LCRH_FEN;
	writew(lcr_h, priv->base + UART011_LCRH);
	writew(0xff, priv->base + UART011_MIS);


	writew(UART011_OEIS | UART011_BEIS | UART011_PEIS | UART011_FEIS |
       UART011_RTIS | UART011_RXIS, priv->base + UART011_ICR);

	/* enable rx irq */
	writew(UART011_RTIM | UART011_RXIM, priv->base + UART011_IMSC);
	cr = UART011_CR_UARTEN | UART011_CR_RXE | UART011_CR_TXE;
	writew(cr, priv->base  + UART011_CR);
	writew(UART011_RTIS | UART011_RXIS,priv->base + UART011_ICR);
	os_hw_interrupt_create(priv->irq, os_hw_uart_isr, priv, "uart");
	os_hw_interrupt_enable(priv->irq);
	return 0;
}
int32_t hal_uart_finalize(uart_dev_t *uart)
{
	return 0;
}
int32_t hal_uart_send(uart_dev_t *uart, const void *data,
                      uint32_t size, uint32_t timeout)
{
	int i = 0;
	uart_priv_t *priv = (uart_priv_t *)(uart_0.priv);

	(void)uart;
	for(i = 0; i < size; i++)
	{
		while (readl(priv->base + UART011_FR) & UART011_FR_TXFF);
		writel(*(char *)(data + i), priv->base + UART011_DR);
	}
	return 0;
}
int32_t hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size,
                         uint32_t *recv_size, uint32_t timeout)
{
	uint32_t count = 0, i = 0;
	size_t size = 0;
	int32_t ret = -1;
	uart_priv_t *priv = (uart_priv_t *)(uart_0.priv);

	(void)uart;
	if (priv->recv_queue != NULL)
	{
		for (i = 0; i < expect_size; i++) 
		{
			ret = krhino_buf_queue_recv(priv->recv_queue, RHINO_WAIT_FOREVER, (uint8_t *)data++, &size);
			if((ret == 0) && (size == 1))
			{
				count++;
			}
			else
			{
				break;
			}
		}
	}
	if(recv_size != NULL)
		*recv_size = count;
	return ret;
}


