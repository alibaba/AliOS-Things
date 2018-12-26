#ifndef __UART_PORT_H
#define __UART_PORT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "board.h"
#include <aos/kernel.h>

/* USER CONFIGURABLE: Instance name of the UART component */
#if(STDIO_UART == 0)
#define CONSOLE_UART_INSTANCE      SCB0
#elif (STDIO_UART == 1)
#define CONSOLE_UART_INSTANCE      SCB1    
#elif (STDIO_UART == 2)
#define CONSOLE_UART_INSTANCE      SCB2 
#elif (STDIO_UART == 3)
#define CONSOLE_UART_INSTANCE      SCB3 
#elif (STDIO_UART == 4)
#define CONSOLE_UART_INSTANCE      SCB4     
#endif

#define UART_API(fn)          UART_API_IMPL(CONSOLE_UART_INSTANCE, fn)
#define UART_API_IMPL(a, b)    UART_API_IMPL2(a, b)
#define UART_API_IMPL2(a, b)   a ## b

#define UART_FLAG(fg)          UART_FLAG_IMPL(CONSOLE_UART_INSTANCE, fg)
#define UART_FLAG_IMPL(a, b)    UART_FLAG_IMPL2(a, b)
#define UART_FLAG_IMPL2(a, b)   a ## b

typedef enum {
    UART0 = 0,
    UART1,
    UART2,
    UART3,
    UART4,
    UARTn
} uart_port;

typedef enum {
  HAL_OK       = 0x00U,
  HAL_ERROR    = 0x01U,
  HAL_BUSY     = 0x02U,
  HAL_TIMEOUT  = 0x03U
} HAL_Status;

typedef struct {
    aos_mutex_t uart_tx_mutex;
    aos_mutex_t uart_rx_mutex;
    aos_sem_t uart_tx_sem;
    aos_sem_t uart_rx_sem;
} uart_os_t;

int default_UART_Init(void);

#ifdef __cplusplus
}
#endif

#endif 

