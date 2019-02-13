#ifndef __UART_PORT_H
#define __UART_PORT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "board.h"
#include <aos/kernel.h>

/* USER CONFIGURABLE: Instance name of the UART component */
#if(STDIO_UART == 0)
#define CONSOLE_UART_INSTANCE      UART_1
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

#define UART_RX_QUEUE_SIZE  64
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
void uart_put_char(int c);
int32_t aos_uart_recv(void *data, uint32_t expect_size, uint32_t *recv_size, uint32_t timeout);
int32_t aos_uart_send(void *data, uint32_t size, uint32_t timeout);
struct circ_buf {
    char *buf;
    int head;
    int tail;
};

/* Return count in buffer.  */
#define CIRC_CNT(head,tail,size) (((head) - (tail)) & ((size)-1))

/* Return space available, 0..size-1.  We always leave one free char
   as a completely full buffer has head == tail, which is the same as
   empty.  */
#define CIRC_SPACE(head,tail,size) CIRC_CNT((tail),((head)+1),(size))

/* Return count up to the end of the buffer.  Carefully avoid
   accessing head and tail more than once, so they can change
   underneath us without returning inconsistent results.  */
#define CIRC_CNT_TO_END(head,tail,size) \
    ({int end = (size) - (tail); \
      int n = ((head) + end) & ((size)-1); \
      n < end ? n : end;})

/* Return space available up to the end of the buffer.  */
#define CIRC_SPACE_TO_END(head,tail,size) \
    ({int end = (size) - 1 - (head); \
      int n = (end + (tail)) & ((size)-1); \
      n <= end ? n : end+1;})
//the size should be 2^
#define UART_CONSOLE_SIZE 256
#ifdef __cplusplus
}
#endif

#endif 

