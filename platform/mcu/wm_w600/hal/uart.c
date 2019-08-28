/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "aos/hal/uart.h"

#include "wm_uart.h"
#include "wm_regs.h"

typedef volatile unsigned int TLS_REG; 

static inline void wm_reg_write32(unsigned int reg, unsigned int val)
{
    *(TLS_REG *)reg = val;
}

static inline unsigned int wm_reg_read32(unsigned int reg)
{
    unsigned int val = *(TLS_REG *)reg;
    return val;
}

uart_dev_t uart_0 = {
   .port = 0,    /* uart port */
   .config = {115200, DATA_WIDTH_8BIT, NO_PARITY, STOP_BITS_1, FLOW_CONTROL_DISABLED},  /* uart config */
   .priv = NULL    /* priv data */
};

uart_dev_t uart_1 = {
   .port = 0,    /* uart port */
   .config = {115200, TLS_UART_CHSIZE_8BIT, TLS_UART_PMODE_DISABLED, TLS_UART_ONE_STOPBITS, FLOW_CONTROL_DISABLED},  /* uart config */
   .priv = NULL    /* priv data */
};

int32_t hal_uart_init(uart_dev_t *uart)
{
#if 1	
    struct tls_uart_options uart_opts;
    
    uart_opts.baudrate = uart->config.baud_rate;
    uart_opts.charlength = uart->config.data_width;
    uart_opts.flow_ctrl = (enum TLS_UART_FLOW_CTRL_MODE) uart->config.flow_control;
    uart_opts.paritytype = (enum TLS_UART_PMODE) uart->config.parity;
    uart_opts.stopbits = (enum TLS_UART_STOPBITS) uart->config.stop_bits;

    if (WM_SUCCESS != tls_uart_port_init(uart->port, NULL, 0))
        return -1;		
#endif	
    return 0;
}

int32_t hal_uart_finalize(uart_dev_t *uart)
{
    return 0;
}

int sendchar(int ch)
{
	wm_reg_write32(HR_UART0_INT_MASK, 0x3);
    if(ch == '\n')  
	{
		while (wm_reg_read32(HR_UART0_FIFO_STATUS) & 0x3F);
		wm_reg_write32(HR_UART0_TX_WIN, '\r');
    }
    while(wm_reg_read32(HR_UART0_FIFO_STATUS) & 0x3F);
    wm_reg_write32(HR_UART0_TX_WIN, (char)ch);
    //wm_reg_write32(HR_UART0_INT_MASK, 0x0);
    return ch;
}

int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
	char* pdata = (char *)data;
#if 1	
	int i = 0;
	//endchar(uart->port+'0');
    for(i = 0; i < size; i++)
    {
        sendchar(pdata[i]);
    }
	//sendchar('=');
#endif	
    //tls_uart_write(uart->port, data, size);
//	sendchar('#');
    return 0;
}

int32_t hal_uart_recv(uart_dev_t *uart, void *data, uint32_t expect_size,  uint32_t timeout)
{  
	if (expect_size == 0)
		return 1;
    char *buf = data;
   
    int ret = tls_uart_read(uart->port,buf, expect_size);
     
	return ret?0:1;
}

int32_t hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size,
                         uint32_t *recv_size, uint32_t timeout)
{   
    int ret ;
    char *buf = data;
    uint32_t size = 0;
  
    if (expect_size == 0) {
        return 1;
    }

    while (expect_size > 0) {
        int one_time_size = 0;
        size += (one_time_size = tls_uart_read(uart->port, buf, expect_size));
        expect_size -= one_time_size;
        buf += one_time_size;
        if (!timeout)
        {
            break;
        }
        if (expect_size > 0)
        {
            aos_msleep(1000);
            timeout--;
        }
    }

    if (recv_size != NULL) {
        *recv_size = size;
    }
    return 0;
}


