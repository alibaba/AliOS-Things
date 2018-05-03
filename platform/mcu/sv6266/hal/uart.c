#include "hal/soc/soc.h"
#include "sys/dbgcon.h"

uart_dev_t uart_0;
int32_t hal_uart_init(uart_dev_t *uart)
{
	return 0;
}

int32_t hal_uart_finalize(uart_dev_t *uart)
{
    return 0;
}

int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
    uint8_t* pTmp = (uint8_t*)data;
    for (int i = 0; i < size; i++) {
        dbgcon_tx(*pTmp++);
    }
    return size;
}

int32_t hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size,
                      uint32_t *recv_size, uint32_t timeout)
{
    int i;
    uint8_t* pTmp = (uint8_t*)data;
    for (i = 0; i < expect_size; i++) {
    	while(!dbgcon_rx_ready());
    	*pTmp = dbgcon_rx();
    	pTmp++;
    }
    if (recv_size != NULL) {
        *recv_size = expect_size;
    }
    return 0;
}

int32_t hal_uart_recv(uart_dev_t *uart, void *data, uint32_t expect_size, uint32_t timeout) {
    // aMH: duplicate function to reduce stack size.
    int i;
    uint8_t* pTmp = (uint8_t*)data;
    for (i = 0; i < expect_size; i++) {
        while(!dbgcon_rx_ready());
        *pTmp = dbgcon_rx();
        pTmp++;
    }
    return 0;
}

