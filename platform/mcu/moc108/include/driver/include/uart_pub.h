#ifndef _UART_PUB_H
#define _UART_PUB_H

#include <stdio.h>

#define os_null_printf(fmt, ...)

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

#define UART_DEV_NAME                ("uart")


#define UART_CMD_MAGIC               (0xC124000)

enum
{
    CMD_SEND_BACKGROUND            = UART_CMD_MAGIC + 0,
    CMD_UART_RESET                 = UART_CMD_MAGIC + 1,
    CMD_RX_COUNT,
    CMD_RX_PEEK,
    CMD_UART_INIT,
};

/* CMD_RX_PEEK*/
#define URX_PEEK_SIG              (0x0ee)
typedef struct _peek_rx_
{
    UINT32 sig;

    UINT32 len;
    void *ptr;
} UART_PEEK_RX_T, *UART_PEEK_RX_PTR;

/*******************************************************************************
* Function Declarations
*******************************************************************************/
extern void uart_init(void);
extern void uart_exit(void);
extern void uart_isr(void);
extern void fatal_print(const char *fmt, ...);
extern INT32 uart_printf(const char *fmt, ...);
extern void bk_printf(const char *fmt, ...);
extern void uart_send_byte(UINT8 data);

#endif // _UART_PUB_H
