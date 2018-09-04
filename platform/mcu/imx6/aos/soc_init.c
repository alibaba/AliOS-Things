#include "k_api.h"
#include "hal/soc/uart.h"

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


/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  uart_putchar(g_debug_uart_port, &ch);	
  return ch;
}



int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout) {

    int i;
    char *cdata = (char *)data;
    for(i = 0;i < size;i++ )
    {
        uart_putchar(g_debug_uart_port,cdata++);
    }
    return 0;
}


int32_t hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size, uint32_t *recv_size, uint32_t timeout) {

    return 0;
}