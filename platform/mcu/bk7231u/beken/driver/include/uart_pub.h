#ifndef _UART_PUB_H
#define _UART_PUB_H

#include <stdio.h>
#include "hal/soc/soc.h"

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
	CMD_SET_STOP_END,
};

/* CMD_RX_PEEK*/
#define URX_PEEK_SIG              (0x0ee)

typedef struct _peek_rx_
{
    UINT32 sig;

    UINT32 len;
    void *ptr;
} UART_PEEK_RX_T, *UART_PEEK_RX_PTR;

typedef struct
{
	void (*uart_rx_cb)(uint8_t);
	void (*uart_tx_cb)(uint8_t);
}bk_uart_cb_t;

extern bk_uart_cb_t bk_uart_cb;

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
extern INT32 uart_read_byte( UINT8 uport, UINT8 *byte );
extern void uart_write_byte(UINT8 uport, UINT8 data);
#endif // _UART_PUB_H
