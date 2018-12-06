/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "k_api.h"
#include "aos/hal/uart.h"
#include "imx_uart.h"

#if (RHINO_CONFIG_CPU_NUM > 1)
#include "smp_port.h"

kspinlock_t g_smp_printlock;
#endif

#define MAX_BUF_UART_BYTES   32
kbuf_queue_t g_buf_queue_uart;
char g_buf_uart[MAX_BUF_UART_BYTES];

#define OS_WAIT_FOREVER		0xffffffffU

#define HAL_WAIT_FOREVER    OS_WAIT_FOREVER


uart_dev_t   uart_0;

extern uint32_t g_debug_uart_port ;

#if defined (__CC_ARM) && defined(__MICROLIB)
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#define GETCHAR_PROTOTYPE int fgetc(FILE *f)
#elif defined(__ICCARM__)
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#define GETCHAR_PROTOTYPE int fgetc(FILE *f)
#else
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#define GETCHAR_PROTOTYPE int __io_getchar(void)
#endif /* defined (__CC_ARM) && defined(__MICROLIB) */


/*! 
 * UART interrupt handler.
 *
 * @return  none
 */
void uart_interrupt_handler(void)
{
    uint8_t read_char;

    read_char = uart_getchar(g_debug_uart_port);
    if(NONE_CHAR == read_char){
        return;
    }
    krhino_buf_queue_send(&g_buf_queue_uart, &read_char, 1);

}


int32_t hal_uart_init(uart_dev_t *uart)
{
    int err_code;
    
    uart_init(g_debug_uart_port, 115200, PARITY_NONE, STOPBITS_ONE, EIGHTBITS, FLOWCTRL_OFF);

    /* Set the IRQ mode for the Rx FIFO */
    uart_set_FIFO_mode(g_debug_uart_port, RX_FIFO, 1, IRQ_MODE);

	/* Set the ISR and enable the interrupts for UART3 */
    uart_setup_interrupt(g_debug_uart_port, uart_interrupt_handler, TRUE);    

    (void)krhino_buf_queue_create(&g_buf_queue_uart, "buf_queue_uart",g_buf_uart, MAX_BUF_UART_BYTES, 1);
    
#if (RHINO_CONFIG_CPU_NUM > 1)
    krhino_spin_lock_init(&g_smp_printlock);
#endif

    // flush UART RX FIFO 
    uint8_t c;
    do {
        c = uart_getchar(g_debug_uart_port);
    } while (c != NONE_CHAR);
    
    return 0;
}

int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout) {

    int i;
    char *cdata = (char *)data;

#if (RHINO_CONFIG_CPU_NUM > 1)
    krhino_spin_lock(&g_smp_printlock);
#endif

    for(i = 0;i < size;i++ )
    {
        uart_putchar(g_debug_uart_port,cdata++);
    }

#if (RHINO_CONFIG_CPU_NUM > 1)
    krhino_spin_unlock(&g_smp_printlock); 
#endif

    return 0;
}


int32_t hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size, uint32_t *recv_size, uint32_t timeout) {

    uint8_t *pdata = (uint8_t *)data;
    int i = 0;
    uint32_t rx_count = 0;
    int32_t ret = -1;
    int32_t rev_size;

    if (data == NULL) {
        return -1;
    }

    for (i = 0; i < expect_size; i++)
    {
        ret = krhino_buf_queue_recv(&g_buf_queue_uart, RHINO_WAIT_FOREVER, &pdata[i], &rev_size);
        if((ret == 0) && (rev_size == 1))
        {
            rx_count++;
        }else {
            break;
        }

    }
    
    if (recv_size)
    {
        *recv_size = rx_count;
    }

    if(rx_count != 0)
    {
        ret = 0;
    }
    else
    {
        ret = -1;
    }

    return ret;
}

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{

  if (ch == '\n') {
    //hal_uart_send(&console_uart, (void *)"\r", 1, 30000);
    hal_uart_send(&uart_0, (void *)"\r", 1, 30000);
  }
  hal_uart_send(&uart_0, &ch, 1, 30000);
  return ch;
}

/**
  * @brief  Retargets the C library scanf function to the USART.
  * @param  None
  * @retval None
  */

GETCHAR_PROTOTYPE
{
  /* Place your implementation of fgetc here */
  /* e.g. readwrite a character to the USART2 and Loop until the end of transmission */
  uint8_t ch = EOF;
  int32_t ret = -1;
  
  uint32_t recv_size;
  ret = hal_uart_recv_II(&uart_0, &ch, 1, &recv_size, HAL_WAIT_FOREVER);

  if (ret == 0) {
      return ch;
  } else {
      return -1;
  }
}

int32_t hal_uart_finalize(uart_dev_t *uart)
{
    return 0;
}


