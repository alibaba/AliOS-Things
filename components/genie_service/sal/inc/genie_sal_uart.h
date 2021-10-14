#ifndef __GENIE_SAL_UART_H__
#define __GENIE_SAL_UART_H__
#include "ulog/ulog.h"

#ifndef GENIE_MESH_DEFAULT_LOG_LEVEL
#define GENIE_MESH_DEFAULT_LOG_LEVEL LOG_DEBUG
#endif

int genie_sal_uart_init(void);
int32_t genie_sal_uart_send_one_byte(uint8_t byte);
int32_t genie_sal_uart_send_str(const char *fmt, ...);

#endif
