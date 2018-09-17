
#include <stdarg.h>
#include <stdio.h>
#include <errno.h>
#include <aos/aos.h>
#include <k_api.h>
#include "hal/soc/uart.h"

#include "port_mcu.h"
#include "util_buf.h"

#define EFM32G_USART_COUNT               2

#define EFM32G_USART0_RX_BUF_SIZE        50
#define EFM32G_USART1_RX_BUF_SIZE        150

#define EFM32_UART1_LOC                  0
#define EFM32_LEUART0_LOC                0

BUF_Define(EFM32G_USART0_RX_BUF,EFM32G_USART0_RX_BUF_SIZE)
BUF_Define(EFM32G_USART1_RX_BUF,EFM32G_USART1_RX_BUF_SIZE)

static  USART_TypeDef * const s_uartBaseAddrs[EFM32G_USART_COUNT] = {USART0, USART1};

uint8_t s_uartLoc[EFM32G_USART_COUNT]={EFM32_LEUART0_LOC,EFM32_UART1_LOC};
static kmutex_t s_uartSendMutex[EFM32G_USART_COUNT];
static kmutex_t s_uartRecvMutex[EFM32G_USART_COUNT];
static BUF_STRUCT *s_uartRecvBuf[EFM32G_USART_COUNT]={&EFM32G_USART0_RX_BUF,&EFM32G_USART1_RX_BUF};

static void _dev_uart_putchar(uint8_t port, char ch)
{
  volatile uint32_t i;
  USART_TypeDef *usart=(USART_TypeDef *)s_uartBaseAddrs[port];
  
  USART_Tx(usart,ch);
}

uart_dev_t uart_0 = {
  .port = 0,                                                                        /* uart port */
  .config = {115200, DATA_WIDTH_8BIT, NO_PARITY, STOP_BITS_1, FLOW_CONTROL_DISABLED}, /* uart config */
  .priv = NULL                                                                      /* priv data */
};

uart_dev_t uart_1 = {
  .port = 1,                                                                        /* uart port */
  .config = {115200, DATA_WIDTH_8BIT, NO_PARITY, STOP_BITS_1, FLOW_CONTROL_DISABLED}, /* uart config */
  .priv = NULL                                                                      /* priv data */
};

int32_t hal_uart_send(uart_dev_t *p_uart_dev, const void *data, uint32_t size, uint32_t timeout)
{
  kstat_t stat = RHINO_SUCCESS;
  
  if (timeout == 0)
  {
    timeout = 1000;
  }

  stat = krhino_mutex_lock(&s_uartSendMutex[p_uart_dev->port], AOS_WAIT_FOREVER);
  if (stat != RHINO_SUCCESS)
  {
    return stat;
  }

  for (int i = 0; i < size; i++) 
  {
    _dev_uart_putchar(p_uart_dev->port, ((uint8_t *)data)[i]);
  }

  krhino_mutex_unlock(&s_uartSendMutex[p_uart_dev->port]);
  if (stat != RHINO_SUCCESS)
  {
    return stat;
  }

  return stat;
}

int32_t hal_uart_recv(uart_dev_t *p_uart_dev, void *data, uint32_t expect_size, uint32_t timeout)
{
  kstat_t stat = RHINO_SUCCESS;
  
  
  stat = krhino_mutex_lock(&s_uartRecvMutex[p_uart_dev->port], AOS_WAIT_FOREVER);
  if (stat != RHINO_SUCCESS)
  {
    return stat;
  }
  
  while(1)
  {
    uint16_t query_size=BUF_Query(s_uartRecvBuf[p_uart_dev->port]);
    
    if(query_size>=expect_size)
    {      
      break;
    }     
  }
  
  BUF_Get(s_uartRecvBuf[p_uart_dev->port],data,expect_size);
  
  krhino_mutex_unlock(&s_uartRecvMutex[p_uart_dev->port]);
  if (stat != RHINO_SUCCESS)
  {
    return stat;
  }
  
  return stat; 
}
#if 0
void USART0_RX_IRQHandler(void)
{ 
  uint8_t data;
  data=USART0->RXDATA;

  BUF_PutUnit(&EFM32G_USART0_RX_BUF,data);     
}
#endif
void USART1_RX_IRQHandler(void)
{ 
  uint8_t data;
  data=USART1->RXDATA;
  
#ifdef CONFIG_LINKWAN_AT
  extern void linkwan_serial_input(uint8_t cmd);
  linkwan_serial_input(data);
#endif
  BUF_PutUnit(&EFM32G_USART1_RX_BUF,data);     
}


int32_t uart_dev_cfg_to_hw_cfg(uart_dev_t *uart,USART_InitAsync_TypeDef *p_uartInit)
{
  p_uartInit->enable=true;
  p_uartInit->baudrate = uart->config.baud_rate;
  p_uartInit->refFreq=  SystemCoreClockGet();
  p_uartInit->oversampling= USART_CTRL_OVS_X4;
    
  switch (uart->config.parity)
  {
  case NO_PARITY:
    p_uartInit->parity=usartNoParity;      
    break;
  case ODD_PARITY:
    p_uartInit->parity= usartOddParity;
    break;
  case EVEN_PARITY:
    p_uartInit->parity = usartEvenParity;
    break;
  default:
    return EIO;
  }
  
  switch (uart->config.data_width)
  {
  case DATA_WIDTH_7BIT:
    p_uartInit->databits = usartDatabits7;
    break;
  case DATA_WIDTH_8BIT:
    p_uartInit->databits = usartDatabits8;
    break;
  default:
    return EIO;
  }
  
  switch (uart->config.stop_bits)
  {
  case STOP_BITS_1:
    p_uartInit->stopbits = usartStopbits1;
    break;
  case STOP_BITS_2:
    p_uartInit->stopbits = usartStopbits2;
    break;
  default:
    return EIO;
  }
  
  return 0;
}

int32_t hal_uart_init(uart_dev_t *p_uart_dev)
{
  int status;
  USART_TypeDef *p_usart=(USART_TypeDef *)s_uartBaseAddrs[p_uart_dev->port];
  
  USART_InitAsync_TypeDef   uartInit={0};  
  status=uart_dev_cfg_to_hw_cfg(p_uart_dev,&uartInit);
  if(0!=status)
  {
    return status;
  }
  
  USART_InitAsync(p_usart, &uartInit);
  p_usart->ROUTE =((s_uartLoc[p_uart_dev->port])<<8)| USART_ROUTE_TXPEN | USART_ROUTE_RXPEN ;
  USART_Enable(p_usart,usartEnable);

  if (0 != status)
  {
    return EIO;
  }
  
  if (krhino_mutex_create(&s_uartSendMutex[p_uart_dev->port], "uartSendMutex") != RHINO_SUCCESS)
  {
    return EIO;
  }
  
  if (krhino_mutex_create(&s_uartRecvMutex[p_uart_dev->port], "uartRecvMutex") != RHINO_SUCCESS)
  {
    return EIO;
  }
  
  if(0==p_uart_dev->port)
  {
    NVIC_ClearPendingIRQ(USART0_RX_IRQn);
    NVIC_EnableIRQ(USART0_RX_IRQn);
  }
  else if(1==p_uart_dev->port)
  {
    NVIC_ClearPendingIRQ(USART1_RX_IRQn);
    NVIC_EnableIRQ(USART1_RX_IRQn);   
  }

  USART_IntEnable(p_usart, USART_IEN_RXDATAV);
  return 0;
}

int32_t hal_uart_finalize(uart_dev_t *p_uart_dev)
{
  // LPUART_Deinit((LPUART_Type *)s_uartBaseAddrs[p_uart_dev->port]);
  
  krhino_mutex_del(&s_uartSendMutex[p_uart_dev->port]);
  krhino_mutex_del(&s_uartRecvMutex[p_uart_dev->port]);
  return 0;
  
}
