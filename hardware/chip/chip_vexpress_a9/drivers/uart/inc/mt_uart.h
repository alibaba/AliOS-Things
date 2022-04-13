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
#ifndef __MT_UART_H
#define __MT_UART_H

#include "RingBufferUtils.h"
#include "mt_reg_base.h"
#include <string.h>
#include "typedef.h"


#ifndef FALSE
    #define FALSE   0
#endif

#ifndef TRUE
    #define TRUE    (!FALSE)
#endif

#define DRV_Reg32(addr)             (*(volatile unsigned int *)(addr))
#define DRV_WriteReg32(addr, data)  ((*(volatile unsigned int *)(addr)) = (unsigned int)(data))

#define	CONFIG_BAUDRATE		(921600)
#define	UART_LOG_PORT		(UART0_BASE)
#define	DSP_UART_LOG_PORT	(DSP_UART_BASE)

#define UART_FIFO_SIZE		(1024)

#define	UART0_IRQ			(56)
#define	UART1_IRQ			(57)
#define	UART2_IRQ			(58)

#define DRV_SUPPORT_UART_PORTS 	3	/*the UART PORTs driver can support */
#define DRV_SUPPORT_VFF_CHANNEL (DRV_SUPPORT_UART_PORTS * 2)

#define UART_BASE_ADDR_NA 		0
#define UART0_BASE_ADDR			UART0_BASE
#define UART1_BASE_ADDR			UART1_BASE
#define UART2_BASE_ADDR			UART2_BASE
#define UART3_BASE_ADDR			UART_BASE_ADDR_NA
#define UART4_BASE_ADDR			UART_BASE_ADDR_NA
#define UART5_BASE_ADDR			UART_BASE_ADDR_NA


/* uart register */
#define UART_RBR(uart)                    ((uart)+0x0)  /* Read only */
#define UART_THR(uart)                    ((uart)+0x0)  /* Write only */
#define UART_IER(uart)                    ((uart)+0x4)
#define UART_IIR(uart)                    ((uart)+0x8)  /* Read only */
#define UART_FCR(uart)                    ((uart)+0x8)  /* Write only */
#define UART_LCR(uart)                    ((uart)+0xc)
#define UART_MCR(uart)                    ((uart)+0x10)
#define UART_LSR(uart)                    ((uart)+0x14)
#define UART_MSR(uart)                    ((uart)+0x18)
#define UART_SCR(uart)                    ((uart)+0x1c)
#define UART_DLL(uart)                    ((uart)+0x0)  /* Only when LCR.DLAB = 1 */
#define UART_DLH(uart)                    ((uart)+0x4)  /* Only when LCR.DLAB = 1 */
#define UART_EFR(uart)                    ((uart)+0x8)  /* Only when LCR = 0xbf */
#define UART_XON1(uart)                   ((uart)+0x10) /* Only when LCR = 0xbf */
#define UART_XON2(uart)                   ((uart)+0x14) /* Only when LCR = 0xbf */
#define UART_XOFF1(uart)                  ((uart)+0x18) /* Only when LCR = 0xbf */
#define UART_XOFF2(uart)                  ((uart)+0x1c) /* Only when LCR = 0xbf */
#define UART_AUTOBAUD_EN(uart)            ((uart)+0x20)
#define UART_HIGHSPEED(uart)              ((uart)+0x24)
#define UART_SAMPLE_COUNT(uart)           ((uart)+0x28)
#define UART_SAMPLE_POINT(uart)           ((uart)+0x2c)
#define UART_AUTOBAUD_REG(uart)           ((uart)+0x30)
#define UART_RATE_FIX_AD(uart)            ((uart)+0x34)
#define UART_AUTOBAUD_SAMPLE(uart)        ((uart)+0x38)
#define UART_GUARD(uart)                  ((uart)+0x3c)
#define UART_ESCAPE_DAT(uart)             ((uart)+0x40)
#define UART_ESCAPE_EN(uart)              ((uart)+0x44)
#define UART_SLEEP_EN(uart)               ((uart)+0x48)
#define UART_VFIFO_EN(uart)               ((uart)+0x4c) //ADSP_UART_RXDMA_EN_ADDR
#define UART_RXTRI_AD(uart)               ((uart)+0x50)
#define UART_FCR_RD(uart)                 ((uart)+0x5C)
#define UART_FEATURE_SEL(uart)            ((uart)+0x9C)

/* LSR */
#define UART_LSR_DR                 (1 << 0)
#define UART_LSR_OE                 (1 << 1)
#define UART_LSR_PE                 (1 << 2)
#define UART_LSR_FE                 (1 << 3)
#define UART_LSR_BI                 (1 << 4)
#define UART_LSR_THRE               (1 << 5)
#define UART_LSR_TEMT               (1 << 6)
#define UART_LSR_FIFOERR            (1 << 7)

/* LCR */
#define UART_LCR_BREAK              (1 << 6)
#define UART_LCR_DLAB               (1 << 7)

#define UART_WLS_5                  (0 << 0)
#define UART_WLS_6                  (1 << 0)
#define UART_WLS_7                  (2 << 0)
#define UART_WLS_8                  (3 << 0)
#define UART_WLS_MASK               (3 << 0)

#define UART_1_STOP                 (0 << 2)
#define UART_2_STOP                 (1 << 2)
#define UART_1_5_STOP               (1 << 2)    /* Only when WLS=5 */
#define UART_STOP_MASK              (1 << 2)

#define UART_NONE_PARITY            (0 << 3)
#define UART_ODD_PARITY             (0x1 << 3)
#define UART_EVEN_PARITY            (0x3 << 3)
#define UART_MARK_PARITY            (0x5 << 3)
#define UART_SPACE_PARITY           (0x7 << 3)
#define UART_PARITY_MASK            (0x7 << 3)

/* IER */
#define UART_IER_ERBFI              0x0001
#define UART_IER_ETBEI              0x0002
#define UART_IER_ELSI               0x0004
#define UART_IER_EDSSI              0x0008
#define UART_IER_XOFFI              0x0020
#define UART_IER_RTSI               0x0040
#define UART_IER_CTSI               0x0080
#define IER_HW_NORMALINTS           0x0005
#define IER_HW_ALLINTS              0x0007
#define IER_SW_NORMALINTS           0x002d
#define IER_SW_ALLINTS              0x002f
#define UART_IER_ALLOFF             0x0000
#define UART_IER_VFIFO              0x0001

//MCR
#define UART_MCR_DTR				0x0001
#define UART_MCR_RTS    	    	0x0002
#define UART_MCR_LOOPB    	   	 	0x0010
#define UART_MCR_IRE		    	0x0040  //Enable IrDA modulation/demodulation
#define UART_MCR_XOFF             	0x0080
#define UART_MCR_Normal	    		UART_MCR_RTS


/* FCR */
#define UART_FCR_FIFOE              (1 << 0)
#define UART_FCR_CLRR               (1 << 1)
#define UART_FCR_CLRT               (1 << 2)
#define UART_FCR_DMA1               (1 << 3)
#define UART_FCR_RXFIFO_1B_TRI      (0 << 6)
#define UART_FCR_RXFIFO_6B_TRI      (1 << 6)
#define UART_FCR_RXFIFO_12B_TRI     (2 << 6)
#define UART_FCR_RXFIFO_RX_TRI      (3 << 6)
#define UART_FCR_TXFIFO_1B_TRI      (0 << 4)
#define UART_FCR_TXFIFO_4B_TRI      (1 << 4)
#define UART_FCR_TXFIFO_8B_TRI      (2 << 4)
#define UART_FCR_TXFIFO_14B_TRI     (3 << 4)
#define UART_FCR_FIFO_INIT          (UART_FCR_FIFOE|UART_FCR_CLRR|UART_FCR_CLRT)
#define UART_FCR_NORMAL             (UART_FCR_FIFO_INIT|UART_FCR_TXFIFO_4B_TRI|UART_FCR_RXFIFO_12B_TRI)
#define UART_FCR_TRIGGER_LEVEL      (UART_FCR_TXFIFO_4B_TRI|UART_FCR_RXFIFO_6B_TRI)

/* IIR,RO */
#define	UART_IIR_INT_INVALID            0x0001
#define	UART_IIR_RLS                    0x0006  // Receiver Line Status
#define	UART_IIR_RDA                    0x0004  // Receive Data Available
#define	UART_IIR_CTI                    0x000C  // Character Timeout Indicator
#define	UART_IIR_THRE                   0x0002  // Transmit Holding Register Empty
#define	UART_IIR_MS                     0x0000  // Check Modem Status Register
#define	UART_IIR_SWFlowCtrl             0x0010  // Receive XOFF characters
#define	UART_IIR_HWFlowCtrl             0x0020  // CTS or RTS Rising Edge
#define	UART_IIR_FIFOS_ENABLED          0x00c0
#define	UART_IIR_NO_INTERRUPT_PENDING   0x0001
#define	UART_IIR_INT_MASK               0x003f


//EFR
#define UART_EFR_AutoCTS          	0x0080
#define UART_EFR_AutoRTS          	0x0040
#define UART_EFR_Enchance         	0x0010
#define UART_EFR_SWCtrlMask       	0x000f
#define UART_EFR_NoSWFlowCtrl     	0x0000
#define UART_EFR_ALLOFF           	0x0000
#define UART_EFR_AutoRTSCTS       	0x00c0



//Tx/Rx XON1/Xoff1 as flow control word
#define UART_EFR_SWFlowCtrlX1     	0x000a

//Tx/Rx XON2/Xoff2 as flow control word
#define UART_EFR_SWFlowCtrlX2    	0x0005

//Tx/Rx XON1&XON2/Xoff1&Xoff2 as flow control word
#define UART_EFR_SWFlowCtrlXAll   	0x000f


/*XON/XOFF Chars*/
#define UART_CHAR_XON1		0x11
#define UART_CHAR_XOFF1		0x22
#define UART_CHAR_XON2		0x18
#define UART_CHAR_XOFF2		0x28
#define UART_CHAR_ESCAPE	0x77



enum uart_port_ID {
	UART_PORT0 = 0,
	UART_PORT1,
	UART_PORT2,
	UART_PORT3,
	UART_PORT4,
	UART_PORT5,
};

enum {RX, TX, NONE};

typedef enum {
	len_5,
	len_6,
	len_7,
	len_8
}UART_bits_per_ch;

typedef enum {
	sb_1,
	sb_2,
	sb_1_5
}UART_stop_bits;

typedef enum {
	pa_none,
	pa_odd,
	pa_even,
	pa_space,
	pa_mark,
}UART_parity;

typedef enum {
	fc_none,
	fc_hw,
	fc_sw,
	fc_sw_mtk,
}UART_flow_ctrl_mode;

typedef struct
{
	UINT32			baud;
	UART_bits_per_ch	dataBits;
	UART_stop_bits		stopBits;
	UART_parity		parity;
	UART_flow_ctrl_mode	flowControl;
	UINT8			xonChar;
	UINT8			xoffChar;
	UINT8			xon1Char;
	UINT8			xoff1Char;
	UINT8			escapeChar;
}UART_dcb_struct;

typedef struct
{
      UINT8	*rx_adrs;
      UINT8	*tx_adrs;
      UINT16	rx_len;
      UINT16	tx_len;
}UART_ring_buffer_struct;

typedef struct
{
	UINT32			port_id;
	UINT32			initialized;
	UINT32			opened;
	UINT32			use_vfifo;
	UART_dcb_struct		dcb;
	ring_buffer_t		rx_buffer;  /* receive buffer */
	ring_buffer_t		tx_buffer;  /* transmit buffer */
	UINT32			uart_reg_base;
	UINT32			uart_irq_id;
	UINT32			vff_mode;
	UINT32			rx_dma_reg_base;
	UINT32			tx_dma_reg_base;
	UINT32			rx_dma_irq_id;
	UINT32			tx_dma_irq_id;
	UINT32			dropped;
	UINT32			flag;
	UINT32			init_flag;
	void*			rx_dma_static_info;
	void*			rx_dma_run_time_info;
	void*			tx_dma_static_info;
	void*			tx_dma_run_time_info;
	void*			uart_static_info;
	void*			uart_run_time_info;
	#ifdef CONFIG_AOS
    aos_sem_t		rx_complete;
    aos_sem_t		tx_complete;
    aos_mutex_t		tx_mutex;
    aos_sem_t		sem_wakeup;
#else
    volatile bool	rx_complete;
    volatile bool	tx_complete;
#endif
    volatile uint32_t          tx_size;
    volatile uint32_t          rx_size;
    volatile OSStatus          last_receive_result;
    volatile OSStatus          last_transmit_result;
}mtk_uart_setting;

struct mtk_uart_register {
	unsigned int dll;
	unsigned int dlh;
	unsigned int ier;
	unsigned int lcr;
	unsigned int mcr;
	unsigned int fcr;
	unsigned int lsr;
	unsigned int efr;
	unsigned int highspeed;
	unsigned int sample_count;
	unsigned int sample_point;
	unsigned int fracdiv_l;
	unsigned int fracdiv_m;
	unsigned int escape_en;
	unsigned int guard;
	unsigned int rx_sel;
};

struct mtk_uart {
	unsigned long base;
	struct mtk_uart_register registers;
};


/* GPIO setting table */
typedef struct gpio_info{
	int gpio_pin;
	int gpio_mode;
}gpio_T;


#define	DEBUG_SERIAL_READ_NODATA	(0)
#define	NULL				(void *)0

typedef void * sio_fd_t;
sio_fd_t uart_init_early(void);
UINT32 uart_hw_init(UINT8 port, mtk_uart_setting *u_setting);
void uart_set_dcb_config(mtk_uart_setting *u_setting, UART_dcb_struct *UART_Config);
int mtk_uart_putc(UINT32 uart_base, char c);
int mtk_uart_getc(void);
void enable_console_uart_interrupt(void);
#endif
