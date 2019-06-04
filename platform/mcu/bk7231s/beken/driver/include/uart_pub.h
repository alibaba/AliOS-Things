#ifndef _UART_PUB_H
#define _UART_PUB_H

#include <stdio.h>
#if CFG_SUPPORT_ALIOS
#include "aos/hal/uart.h"
#endif

#if CFG_RELEASE_FIRMWARE
#define os_printf                       os_null_printf
#else
#if CFG_BACKGROUND_PRINT
#define os_printf                       os_null_printf
#else
#ifdef KEIL_SIMULATOR
#define os_printf                       os_null_printf
#else
#define os_printf                       bk_printf
#endif // KEIL_SIMULATOR
#endif // CFG_BACKGROUND_PRINT
#endif // CFG_RELEASE_FIRMWARE

#define warning_prf                    bk_printf
#define fatal_prf                      bk_printf
#define null_prf                       os_null_printf

#define UART_SUCCESS                 (0)
#define UART_FAILURE                 ((UINT32)-1)

#define UART2_DEV_NAME               ("uart2")   /*debug purpose*/
#define UART1_DEV_NAME               ("uart1")   /*comm purpose*/

#define UART_CMD_MAGIC               (0xC124000)

#define UART1_PORT				0
#define UART2_PORT				1

enum
{
    CMD_SEND_BACKGROUND            = UART_CMD_MAGIC + 0,
    CMD_UART_RESET                 = UART_CMD_MAGIC + 1,
    CMD_RX_COUNT,
    CMD_RX_PEEK,
    CMD_UART_INIT,
    CMD_UART_SET_RX_CALLBACK,
    CMD_UART_SET_TX_CALLBACK,   
    CMD_SET_STOP_END,    
    CMD_UART_SET_TX_FIFO_NEEDWR_CALLBACK,
    CMD_SET_TX_FIFO_NEEDWR_INT,
};

/* CMD_RX_PEEK*/
#define URX_PEEK_SIG              (0x0ee)

typedef struct _peek_rx_
{
    UINT32 sig;

    UINT32 len;
    void *ptr;
} UART_PEEK_RX_T, *UART_PEEK_RX_PTR;

typedef void (*uart_callback)(int uport, void *param);

typedef struct uart_callback_des
{
    uart_callback callback;
    void  *param;
}UART_CALLBACK_RX_T, *UART_CALLBACK_RX_PTR;

#if (!CFG_SUPPORT_ALIOS)
/**
 * UART data width
 */
typedef enum
{
    DATA_WIDTH_5BIT,
    DATA_WIDTH_6BIT,
    DATA_WIDTH_7BIT,
    DATA_WIDTH_8BIT
} uart_data_width_t;

/**
 * UART stop bits
 */
typedef enum
{
    BK_STOP_BITS_1,
    BK_STOP_BITS_2,
} uart_stop_bits_t;

/**
 * UART flow control
 */
typedef enum
{
    FLOW_CTRL_DISABLED,
    FLOW_CTRL_CTS,
    FLOW_CTRL_RTS,
    FLOW_CTRL_RTS_CTS
} uart_flow_control_t;

/**
 * UART parity
 */
typedef enum
{
    BK_PARITY_NO,
    BK_PARITY_ODD,
    BK_PARITY_EVEN,
} uart_parity_t;

typedef struct
{
    uint32_t				  baud_rate;
    uart_data_width_t         data_width;
    uart_parity_t 	          parity;
    uart_stop_bits_t	      stop_bits;
    uart_flow_control_t       flow_control;
    uint8_t				      flags;	 /**< if set, UART can wake up MCU from stop mode, reference: @ref UART_WAKEUP_DISABLE and @ref UART_WAKEUP_ENABLE*/
} uart_config_t;
#endif
/*******************************************************************************
* Function Declarations
*******************************************************************************/
extern void uart1_init(void);
extern void uart1_exit(void);
extern void uart1_isr(void);

extern void uart2_init(void);
extern void uart2_exit(void);
extern void uart2_isr(void);

extern INT32 os_null_printf(const char *fmt, ...);
extern void fatal_print(const char *fmt, ...);
extern INT32 uart_printf(const char *fmt, ...);
extern void bk_printf(const char *fmt, ...);
extern void uart_send_byte(UINT8 ch, UINT8 data);
extern void bk_send_string(UINT8 uport, const char *string);
extern void uart_wait_tx_over();
extern UINT8 uart_is_tx_fifo_empty(UINT8 uport);
extern UINT8 uart_is_tx_fifo_full(UINT8 uport);
extern int uart_read_byte(int uport);
extern int uart_write_byte(int uport, char c);
#endif // _UART_PUB_H

