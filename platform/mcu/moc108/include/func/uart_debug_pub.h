#ifndef _UART_DEBUG_PUB_H_
#define _UART_DEBUG_PUB_H_

#include "uart_pub.h"

/*******************************************************************************
* Function Declarations
*******************************************************************************/
extern UINT32 udebug_has_ctrlc(void);
extern UINT32 udebug_wait_ctrlc_exit(void);
extern UINT32 uart_debug_init(void);

#endif // _UART_DEBUG_PUB_H_

