
#include <stdio.h>

#include <k_api.h>
#include <hal/hal.h>
#include <hal/soc/soc.h>

#include "efm32_hal_uart.h"

static USART_TypeDef *USART_tab[1] = {
    UART1,
};
extern uart_dev_t uart_0;
static USART_TypeDef* efm_UART1;

int32_t hal_uart_init(uart_dev_t *uart_dev)
{
  /* Setup UART1 in async mode for RS232*/
  USART_InitAsync_TypeDef uartInit = USART_INITASYNC_DEFAULT;

  // gpio enable
  CMU_ClockEnable(cmuClock_GPIO, true);

  /* Enable clock for USART module */
  CMU_ClockEnable(cmuClock_UART1, true);

  /* Configure GPIO pins */
  GPIO_PinModeSet(gpioPortB, 9, gpioModePushPull, 1); // tx
  GPIO_PinModeSet(gpioPortB, 10, gpioModeInput, 0); // rx

  /* Prepare struct for initializing UART in asynchronous mode*/
  uartInit.enable       = usartDisable;   /* Don't enable UART upon intialization */
  uartInit.refFreq      = 0;              /* Provide information on reference frequency. When set to 0, the reference frequency is */
  uartInit.baudrate     = 115200;         /* Baud rate */
  uartInit.oversampling = usartOVS16;     /* Oversampling. Range is 4x, 6x, 8x or 16x */
  uartInit.databits     = usartDatabits8; /* Number of data bits. Range is 4 to 10 */
  uartInit.parity       = usartNoParity;  /* Parity mode */
  uartInit.stopbits     = usartStopbits1; /* Number of stop bits. Range is 0 to 2 */
  uartInit.mvdis        = false;          /* Disable majority voting */
  uartInit.prsRxEnable  = false;          /* Enable USART Rx via Peripheral Reflex System */
  uartInit.prsRxCh      = usartPrsRxCh0;  /* Select PRS channel if enabled */
  /* Initialize USART with uartInit struct */
  USART_InitAsync(UART1, &uartInit);


  /* Prepare UART Rx and Tx interrupts */
  USART_IntClear(UART1, _UART_IF_MASK);
  USART_IntEnable(UART1, UART_IF_RXDATAV);
  NVIC_ClearPendingIRQ(UART1_RX_IRQn);
  //NVIC_ClearPendingIRQ(UART1_TX_IRQn);
  NVIC_EnableIRQ(UART1_RX_IRQn);
  //NVIC_EnableIRQ(UART1_TX_IRQn);
  /* Enable I/O pins at UART1 location #2 */
  UART1->ROUTE = UART_ROUTE_RXPEN | UART_ROUTE_TXPEN | UART_ROUTE_LOCATION_LOC2;
  /* Enable UART */
  USART_Enable(UART1, usartEnable);

  return 0;
}


int32_t hal_uart_send(uart_dev_t *uart_dev,
                      const void *data,
                      uint32_t size,
                      uint32_t timeout)
{
    USART_TypeDef *USARTx;

    if ((uart_dev == NULL) || (data == NULL)) {
        return -1;
    }

    uint8_t *ch = (uint8_t*)data;
    int i;
    for (i = 0; i < size; i++) {
        USART_Tx(UART1, ch[i]);
    }

    return i;
}


int32_t hal_uart_recv_II(uart_dev_t *uart_dev, void *data, uint32_t expect_size,
        uint32_t *recv_size, uint32_t timeout)
{
    if ((uart_dev == NULL) || (data == NULL)) {
        return -1;
    }
    // TODO: not implemented
    return -1;
}
