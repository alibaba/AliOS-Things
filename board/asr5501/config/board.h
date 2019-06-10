#ifndef __BOARD_H__
#define __BOARD_H__

#include "hal/hal.h"


extern hal_logic_partition_t hal_partitions[];
extern void flash_partition_init(void);

typedef enum {
    PORT_UART_STD,
    PORT_UART_AT,
    PORT_UART_RS485,
    PORT_UART_SCANNER,
    PORT_UART_LORA,
    PORT_UART_TEMP,
    PORT_UART_SIZE,
    PORT_UART_INVALID = 255
}PORT_UART_TYPE;

#endif //__BOARD_H__