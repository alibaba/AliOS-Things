#ifndef __UART_PORT_H
#define __UART_PORT_H

#ifdef __cplusplus
extern "C" {
#endif
	 
//#include <aos/kernel.h>

/* bufferQueue for uart */
#define MAX_BUF_UART_BYTES  2000
#define COMn    2

typedef enum {
    UART0 = 0,
    UART1,
    UART2,
    UART3,
    UART4,
    UART5
} uart_port;


typedef enum {
  HAL_OK       = 0x00U,
  HAL_ERROR    = 0x01U,
  HAL_BUSY     = 0x02U,
  HAL_TIMEOUT  = 0x03U
} HAL_Status;

int default_UART_Init(void);

#ifdef __cplusplus
}
#endif

#endif 

