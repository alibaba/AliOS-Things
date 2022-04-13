/*
 * Copyright (c) 2019 MediaTek Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
//#include "FreeRTOS.h"
//#include "queue.h"
#include <stdint.h>
#include "mt_gic_v3.h"
#include "mt_uart.h"
#include "mt_printf.h"
#include "mt_reg_base.h"
#include "mt_irq_define.h"
#include "RingBufferUtils.h"
#include "freertos_to_aos.h"
#include <typedef.h>
#include <k_api.h>
#include "aos/hal/uart.h"

mtk_uart_setting uart_setting[DRV_SUPPORT_UART_PORTS];

const UINT32 UART_BaseAddr[DRV_SUPPORT_UART_PORTS] ={
	UART0_BASE_ADDR,
	UART1_BASE_ADDR,
	UART2_BASE_ADDR,
};

const UINT32 UART_IRQ_NUM[DRV_SUPPORT_UART_PORTS] ={
	UART0_IRQ,
	UART1_IRQ,
	UART2_IRQ,
};


#define DIV_ROUND_CLOSEST(x, div)	(((x) + ((div) / 2)) / (div))
#define DIV_ROUND_UP(n, div)		(((n) + (div) - 1) / (div))
void __uart_baudrate_config(unsigned int addr, unsigned int baudrate, unsigned int uart_clk)
{
    int highspeed, quot;
    int sample, sample_count, sample_point;
    int dll, dlh;
    unsigned int val;

    /* set baud rate */
    if (baudrate < 115200) {
        highspeed = 0;
        quot = DIV_ROUND_CLOSEST(uart_clk, 16 * baudrate);
    }  else {
        highspeed = 3;
        quot = DIV_ROUND_UP(uart_clk, 256 * baudrate);
    }

    sample = DIV_ROUND_CLOSEST(uart_clk, quot * baudrate);
    sample_count = sample - 1;
    sample_point = (sample - 2) >> 1;
    dll = quot & 0xff;
    dlh = quot >> 8;

    DRV_WriteReg32(UART_HIGHSPEED(addr), highspeed);

    DRV_WriteReg32(UART_LCR(addr), UART_LCR_DLAB); // set LCR to DLAB to set DLL,DLH
    DRV_WriteReg32(UART_DLL(addr), dll);
    DRV_WriteReg32(UART_DLH(addr), dlh);
    DRV_WriteReg32(UART_LCR(addr), UART_WLS_8); //word length 8
    DRV_WriteReg32(UART_SAMPLE_COUNT(addr), sample_count);
    DRV_WriteReg32(UART_SAMPLE_POINT(addr), sample_point);

    val = UART_FCR_FIFO_INIT; /* Set FIFO */
    val |= UART_FCR_TRIGGER_LEVEL; /* Set trigger level */
    DRV_WriteReg32(UART_FCR(addr), val);
}

/* temp api, will be removed when uart driver ready */
static void uart_tx_rx_gpio_set(int port_line)
{
	switch(port_line)
	{
	case UART_PORT0:
		break;
	case UART_PORT1:
		DRV_WriteReg32(0x10005210, DRV_Reg32(0x10005210) | (1<<24));
		break;
	case UART_PORT2:
		break;
	default:
		printf("[UART PIN Set] get invalid UART port\n");
	}
}

static void uart_platform_init(int port_line)
{
	uart_tx_rx_gpio_set(port_line);
}

sio_fd_t uart_init_early(void)
{
	__uart_baudrate_config(UART_LOG_PORT, CONFIG_BAUDRATE, SRC_CLOCK);
	__uart_baudrate_config(DSP_UART_LOG_PORT, CONFIG_BAUDRATE, SRC_CLOCK);

	//uart_platform_init(UART_PORT1);
	//uart_init(UART1_BASE, CONFIG_BAUDRATE, SRC_CLOCK);
	return UART_LOG_PORT;
}

UINT32 uart_hw_init(UINT8 port, mtk_uart_setting *u_setting)
{
	UINT32 uart_base;
	UINT8 *rx_buf = (UINT8 *)aos_malloc(UART_FIFO_SIZE);

	ring_buffer_init(&(u_setting->rx_buffer), rx_buf, UART_FIFO_SIZE);
	u_setting->uart_reg_base = UART_BaseAddr[port];
	u_setting->uart_irq_id = UART_IRQ_NUM[port];
	u_setting->port_id = port;
	u_setting->rx_size              = 0;
  	u_setting->tx_size              = 0;
  	u_setting->last_transmit_result = kNoErr;
  	u_setting->last_receive_result  = kNoErr;

#ifdef CONFIG_AOS_SEM
  	aos_sem_new( &u_setting->tx_complete, 1 );
  	aos_sem_new( &u_setting->rx_complete, 0 );
  	aos_sem_new( &u_setting->sem_wakeup,  0 );
  	aos_mutex_new( &u_setting->tx_mutex );
#else
  	u_setting->tx_complete = false;
  	u_setting->rx_complete = false;
#endif
	uart_base = u_setting->uart_reg_base;
	DRV_WriteReg32(UART_MCR(uart_base), UART_MCR_Normal);
	DRV_WriteReg32(UART_FCR(uart_base), UART_FCR_NORMAL);
	DRV_WriteReg32(UART_IER(uart_base), IER_HW_NORMALINTS);

	u_setting->initialized = TRUE;
	u_setting->opened = TRUE;
	u_setting->dropped = 0;
	//unmask_irq(u_setting->uart_irq_id);
	return 0;
}



void uart_set_dcb_config(mtk_uart_setting *u_setting, UART_dcb_struct *UART_Config)
{
	UINT32   byte;
	UINT32   IER;
	UINT32   uart_base = u_setting->uart_reg_base;

	IER = DRV_Reg32(UART_IER(uart_base));
	DRV_WriteReg32(UART_IER(uart_base),UART_IER_ALLOFF);

	//__uart_set_baud_rate(u_setting, UART_Config->baud);

	/* Setup N81 */
	byte = DRV_Reg32(UART_LCR(uart_base)); /* DLAB start */
	byte &= ~UART_WLS_MASK;
	switch(UART_Config->dataBits)
	{
	case len_5:
		byte |= UART_WLS_5;
		break;

	case len_6:
		byte |= UART_WLS_6;
		break;

	case len_7:
		byte |= UART_WLS_7;
		break;

	case len_8:
		byte |= UART_WLS_8;
		break;

	default:
		break;
	}
	byte &= ~UART_STOP_MASK;
	switch(UART_Config->stopBits)
	{
	case sb_1:
		byte |= UART_1_STOP;
		break;

	case sb_2:
		byte |= UART_2_STOP;
		break;

	case sb_1_5:
		byte |= UART_1_5_STOP;
		break;

	default:
		break;
	}

	byte &= ~UART_PARITY_MASK;
	switch(UART_Config->parity)
	{
	case pa_none:
		byte |= UART_NONE_PARITY;
		break;

	case pa_odd:
		byte |= UART_ODD_PARITY;
		break;

	case pa_even:
		byte |= UART_EVEN_PARITY;
		break;

	case pa_space:
		byte |= UART_SPACE_PARITY;
		break;

	default:
		break;
	}
	DRV_WriteReg32(UART_LCR(uart_base),byte);            /* DLAB End */

	/* flowControl */
	byte = DRV_Reg32(UART_LCR(uart_base));
	DRV_WriteReg32(UART_LCR(uart_base),0xbf);            /* Enchance setting */
	DRV_WriteReg32(UART_ESCAPE_EN(uart_base),0x0);
	switch(UART_Config->flowControl)
	{
	case fc_none:
		DRV_WriteReg32(UART_EFR(uart_base),UART_EFR_ALLOFF);
		break;

	case fc_hw:
		DRV_WriteReg32(UART_EFR(uart_base),UART_EFR_AutoRTSCTS | UART_EFR_Enchance);
		IER |= (UART_IER_RTSI | UART_IER_CTSI);
		break;

	case fc_sw:
		DRV_WriteReg32(UART_EFR(uart_base),UART_EFR_SWFlowCtrlX1);
		DRV_WriteReg32(UART_ESCAPE_EN(uart_base),0x0);
		break;
	case fc_sw_mtk:
		DRV_WriteReg32(UART_EFR(uart_base),UART_EFR_SWFlowCtrlX1);
		DRV_WriteReg32(UART_ESCAPE_DAT(uart_base),UART_Config->escapeChar);
		DRV_WriteReg32(UART_ESCAPE_EN(uart_base),0x1);
		break;

	default:
		break;
	}

	DRV_WriteReg32(UART_XON1(uart_base),UART_Config->xonChar);
	DRV_WriteReg32(UART_XOFF1(uart_base),UART_Config->xoffChar);
	DRV_WriteReg32(UART_XON2(uart_base),UART_Config->xon1Char);
	DRV_WriteReg32(UART_XOFF2(uart_base),UART_Config->xoff1Char);
	DRV_WriteReg32(UART_LCR(uart_base),byte);         /* DLAB End */

	memcpy( &(u_setting->dcb), UART_Config, sizeof(UART_dcb_struct) );
	DRV_WriteReg32(UART_IER(uart_base),IER); // Restore IER

}



static void console_uart_interrupt_handler(int irq, void *dev);
void enable_console_uart_interrupt(void)
{
	DRV_WriteReg32(UART_IER(UART_LOG_PORT), UART_IER_ALLOFF);
	request_irq(UART0_IRQ, console_uart_interrupt_handler, IRQ_TYPE_LEVEL_LOW, "uart_console", NULL);
	DRV_WriteReg32(UART_IER(UART_LOG_PORT), UART_IER_ERBFI);
}
volatile unsigned int * const UART0DR = (unsigned int *) 0x10009000u;
volatile unsigned int * const UART0FR = (unsigned int *) 0x10009018u;
#define UART_RXFE   0x10    // receive fifo is empty
int uart_input_read()
{
	unsigned int ch;
	unsigned int LSR;
#if 0
	while(1){
		LSR = DRV_Reg32(UART_LSR(UART_LOG_PORT));
		if (LSR & UART_LSR_DR) {
			ch = (unsigned int)DRV_Reg32(UART_RBR(UART_LOG_PORT));
			break;
		}
	}
#else
    while(1){
		LSR = *UART0FR;
		if ((LSR & UART_RXFE) == 0){
			ch = (unsigned int)*UART0DR;
			break;
		}
	}
#endif
	return (int)ch;
}


int mtk_console_putc(char c)
{
	unsigned int LSR;

	while (1) {
		LSR = DRV_Reg32(UART_LSR(UART_LOG_PORT));
		if (LSR & UART_LSR_THRE) {
			DRV_WriteReg32(UART_THR(UART_LOG_PORT), (unsigned int)c);
			break;
		}
	}
	return 0;
}

/* for printk*/
int alios_debug_print(const char *buf, int size)
{
    uart_dev_t debug_uart;
    debug_uart.port = 0;

    return hal_uart_send(&debug_uart, buf, size, 0);
}

int mtk_uart_putc(UINT32 uart_base, char c)
{
	unsigned int LSR;
#if 0
	while (1) {
		LSR = DRV_Reg32(UART_LSR(uart_base));
		if (LSR & UART_LSR_THRE) {
			DRV_WriteReg32(UART_THR(uart_base), (unsigned int)c);
			break;
		}
	}
#else
    *UART0DR = (unsigned int)c; /* Transmit char */
#endif
	return 0;
}


int mtk_uart_getc(void)
{
	unsigned int ch;
	unsigned int LSR;
	LSR = DRV_Reg32(UART_LSR(UART_LOG_PORT));
	if (LSR & UART_LSR_DR) {
		ch = (unsigned int)DRV_Reg32(UART_RBR(UART_LOG_PORT));
	} else {
		ch = DEBUG_SERIAL_READ_NODATA;
	}

	return (int)ch;
}

static QueueHandle_t xRxedChars;
void xSerialPortInitMinimal( unsigned portBASE_TYPE uxQueueLength )
{
    /* Create the queues used to hold Rx characters */
    xRxedChars = xQueueCreate( uxQueueLength, (unsigned portBASE_TYPE)sizeof(signed char) );
}

signed portBASE_TYPE xSerialGetChar( signed char *pcRxedChar, TickType_t xBlockTime )
{
    /* Get the next character from the buffer. Return false if no characters
       are available, or arrive before xBlockTime expires. */
    if( xQueueReceive( xRxedChars, pcRxedChar, xBlockTime ))
    {
        return pdTRUE;
    }
    else
    {
        return pdFALSE;
    }
}
static void console_uart_interrupt_handler(int irq, void *dev)
{
	char cChar;
	unsigned int iir;
	mtk_uart_setting* driver = &uart_setting[0];

    iir = DRV_Reg32(UART_IIR(UART_LOG_PORT));
	if (iir & UART_IIR_INT_INVALID)
		return;

    if (iir & UART_IIR_RDA || iir & UART_IIR_CTI) {
        while (DRV_Reg32(UART_LSR(UART_LOG_PORT)) & (UART_LSR_DR)) {
            cChar = (char)DRV_Reg32(UART_RBR(UART_LOG_PORT));
			//mtk_uart_putc(cChar);
			/*push cChar form HW to rx ring buffer*/
			ring_buffer_write( &(driver->rx_buffer), &cChar,1 );
			/* Notify thread if sufficient data are available  & rx_size asssignment in read thread */
    		if ( ( driver->rx_size > 0 ) &&
        		( ring_buffer_used_space( &(driver->rx_buffer)) >= driver->rx_size ) ){
  #ifdef CONFIG_AOS_SEM
      			aos_sem_signal( &driver->rx_complete );
  #else
      			driver->rx_complete = true;
  #endif
      			driver->rx_size = 0;
    		}
        }
    }
}

