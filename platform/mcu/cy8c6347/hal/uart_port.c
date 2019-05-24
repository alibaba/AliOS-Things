
#include "scb/cy_scb_common.h"
#include "scb/cy_scb_uart.h"
#include "board.h"
#include "UART1.h"
#include "UART5.h"
#include "uart_port.h"
#include "aos/hal/uart.h"
#include <stdio.h>
#include <k_api.h>
#include <aos/kernel.h>

uart_dev_t uart_0;

static uart_dev_t console_uart={
  .port=STDIO_UART,
};

int default_UART_Init(void)
{
    uart_0.port                = STDIO_UART;
    uart_0.config.baud_rate    = STDIO_UART_BAUDRATE;
    uart_0.config.data_width   = DATA_WIDTH_8BIT;
    uart_0.config.parity       = NO_PARITY;
    uart_0.config.stop_bits    = STOP_BITS_1;
    uart_0.config.flow_control = FLOW_CONTROL_DISABLED;

    return hal_uart_init(&uart_0);
}

#if defined (__CC_ARM) && defined(__MICROLIB)
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#define GETCHAR_PROTOTYPE int fgetc(FILE *f)
#elif defined(__ICCARM__)
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#define GETCHAR_PROTOTYPE int fgetc(FILE *f)
#else
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#define GETCHAR_PROTOTYPE int __io_getchar(void)

PUTCHAR_PROTOTYPE;
GETCHAR_PROTOTYPE;

/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
    /* Add an explicit reference to the floating point printf library to allow
    the usage of floating point conversion specifier. */
    __asm (".global _printf_float");
    /***************************************************************************
    * Function Name: _write
    ***************************************************************************/
    __attribute__((weak)) int _write (int fd, const char *ptr, int len)
    {
        int nChars = 0;
        (void)fd;
        if (ptr != NULL)
        {
            for (/* Empty */; nChars < len; ++nChars)
            {
                if(*ptr == '\n')
                {
                    __io_putchar('\r');
                }
                __io_putchar((uint32_t)*ptr);
                ++ptr;
            }
        }
        return (nChars);
    }

    __asm (".global _scanf_float");
    __attribute__((weak)) int _read (int fd, char *ptr, int len)
    {
        int nChars = 0;
        (void)fd;
        if (ptr != NULL)
        {
            for(/* Empty */;nChars < len;++ptr)
            {
                *ptr = (char)__io_getchar();
                ++nChars;
                if(*ptr == '\n')
                {
                    break;
                }
            }
        }
        return (nChars);
    }
#endif /* defined (__CC_ARM) && defined(__MICROLIB) */

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  if (ch == '\n') {
    hal_uart_send(&console_uart, (void *)"\r", 1, 30000);
  }
  hal_uart_send(&console_uart, &ch, 1, 30000);
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
    uint8_t ch = 0;
    uint32_t recv_size;
	int32_t ret = 0;

    ret = hal_uart_recv_II(&console_uart, &ch, 1, &recv_size, HAL_WAIT_FOREVER);

	if(ret == 0)
	{
		return ch;
	}
	else
	{
		return -1;
	}
}

aos_sem_t scb1_tx_sema;
aos_sem_t scb1_rx_sema;
aos_sem_t scb5_tx_sema;
aos_sem_t scb5_rx_sema;

//for UART driver lock, if not added, kernel test will fail
aos_mutex_t scb1_tx_mutex;
aos_mutex_t scb1_rx_mutex;
aos_mutex_t scb5_tx_mutex;
aos_mutex_t scb5_rx_mutex;

kbuf_queue_t g_buf_queue_uart[COMn];
char g_buf_uart[COMn][MAX_BUF_UART_BYTES];
const char *g_pc_buf_queue_name[COMn] = {"buf_queue_uart1", "buf_queue_uart5"};

void scb1_callback(uint32_t event)
{
    switch(event)
    {
        case CY_SCB_UART_TRANSMIT_DONE_EVENT:
        aos_sem_signal(&scb1_tx_sema);
        break;

        case CY_SCB_UART_RECEIVE_DONE_EVENT:
        aos_sem_signal(&scb1_rx_sema);
        break;

        case CY_SCB_UART_TRANSMIT_IN_FIFO_EVENT:
        break;

        case CY_SCB_UART_RECEIVE_ERR_EVENT:
        break;

        case CY_SCB_UART_TRANSMIT_ERR_EVENT:
        break;

        case CY_SCB_UART_RB_FULL_EVENT:
        //printf("receive buffer full");
        break;

        default:
        break;
    }

}

void scb5_callback(uint32_t event)
{
    switch(event)
    {
        case CY_SCB_UART_TRANSMIT_DONE_EVENT:
        aos_sem_signal(&scb5_tx_sema);
        break;

        case CY_SCB_UART_RECEIVE_DONE_EVENT:
        aos_sem_signal(&scb5_rx_sema);
        break;

        case CY_SCB_UART_TRANSMIT_IN_FIFO_EVENT:
        break;

        case CY_SCB_UART_RECEIVE_ERR_EVENT:
        break;

        case CY_SCB_UART_TRANSMIT_ERR_EVENT:
        break;

        case CY_SCB_UART_RB_FULL_EVENT:
        //printf("receive buffer full");
        break;

        default:
        break;
    }

}

int32_t hal_uart_recv_buf_queue_1byte(uart_dev_t *uart, uint8_t *pdata, uint32_t timeout)
{
    int32_t ret = 0;
    size_t rev_size = 0;

    switch(uart->port)
    {
        case UART1:
        Cy_SCB_UART_Receive(SCB1, pdata, 1, &UART1_context);

        if(UART1_context.buffer_queue != NULL)
        {
            ret = krhino_buf_queue_recv(UART1_context.buffer_queue, AOS_WAIT_FOREVER, pdata, &rev_size);
            if((ret == 0) && (rev_size == 1))
            {
                ret = 0;
            }
            else
            {
                ret = 2;
            }
        }
        else
        {
            ret = 2;
        }
        break;

        case UART5:
        Cy_SCB_UART_Receive(SCB5, pdata, 1, &UART5_context);

        if(UART5_context.buffer_queue != NULL)
        {
            ret = krhino_buf_queue_recv(UART5_context.buffer_queue, AOS_WAIT_FOREVER, pdata, &rev_size);
            if((ret == 0) && (rev_size == 1))
            {
                ret = 0;
            }
            else
            {
                ret = 2;
            }
        }
        else
        {
            ret = 2;
        }
        break;

        default:
        break;
    }

    return ret;
}


int32_t hal_uart_init(uart_dev_t *uart)
{
    switch(uart->port)
    {
        case UART1:
        UART1_Start();
        Cy_SCB_UART_RegisterCallback(SCB1, scb1_callback, &UART1_context);
        if(krhino_buf_queue_create(&g_buf_queue_uart[0], g_pc_buf_queue_name[0], g_buf_uart[0], MAX_BUF_UART_BYTES, 1) != 0){
            return -2;
        }
        memset(g_buf_uart[0], 0, MAX_BUF_UART_BYTES);
        UART1_context.buffer_queue = &g_buf_queue_uart[0];
        aos_sem_new(&scb1_tx_sema,0);
        aos_sem_new(&scb1_rx_sema,0);
        aos_mutex_new(&scb1_tx_mutex);
        aos_mutex_new(&scb1_rx_mutex);
        break;

        case UART5:
        UART5_Start();
        Cy_SCB_UART_RegisterCallback(SCB5, scb5_callback, &UART5_context);
        if(krhino_buf_queue_create(&g_buf_queue_uart[1], g_pc_buf_queue_name[1], g_buf_uart[1], MAX_BUF_UART_BYTES, 1) != 0){
            return -2;
        }
        memset(g_buf_uart[1], 0, MAX_BUF_UART_BYTES);
        UART5_context.buffer_queue = &g_buf_queue_uart[1];
        aos_sem_new(&scb5_tx_sema,0);
        aos_sem_new(&scb5_rx_sema,0);
        aos_mutex_new(&scb5_tx_mutex);
        aos_mutex_new(&scb5_rx_mutex);
        break;

        default:
        break;
    }

    return 0;
}

int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
    uint8_t *pdata = (uint8_t *)data;
    uint32_t i = 0;
    uint8 buf[1000];


    if(uart == NULL||data==NULL)
    {
        return -1;
    }

#if 0
    if(uart->port == UART1)
    {
#if 0
        for(i = 0; i < size; i++, pdata++)
        {
            putchar((uint32_t)*pdata);
        }
#endif
	    for(i=0; i<size; i++)
		{
            buf[i]=pdata[i];
		}

		buf[i]='\0';
        if(strstr(buf, "AT+CIPSEND") != NULL)
            printf("%s\n", buf);
    }
#endif

    switch(uart->port)
    {
        case UART1:
        aos_mutex_lock(&scb1_tx_mutex, AOS_WAIT_FOREVER);
        Cy_SCB_UART_Transmit(SCB1, (void *)data, size, &UART1_context);
        aos_sem_wait(&scb1_tx_sema, AOS_WAIT_FOREVER);
        aos_mutex_unlock(&scb1_tx_mutex);
        break;

        case UART5:
        aos_mutex_lock(&scb5_tx_mutex, AOS_WAIT_FOREVER);
        Cy_SCB_UART_Transmit(SCB5, (void *)data, size, &UART5_context);
        aos_sem_wait(&scb5_tx_sema, AOS_WAIT_FOREVER);
        aos_mutex_unlock(&scb5_tx_mutex);
        break;

        default:
        break;
    }

    return 0;
}

int32_t hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size,
                      uint32_t *recv_size, uint32_t timeout)
{
    uint8_t *pdata = (uint8_t *)(data);
    int i = 0;
    uint32_t rx_count = 0;
    int32_t ret = -1;

    if ((uart == NULL) || (data == NULL)) {
        return -1;
    }

    switch(uart->port)
    {
    case UART1:
        aos_mutex_lock(&scb1_rx_mutex, AOS_WAIT_FOREVER);
        Cy_SCB_UART_Receive(SCB1, pdata, expect_size, &UART1_context);
        aos_sem_wait(&scb1_rx_sema, AOS_WAIT_FOREVER);
        rx_count = Cy_SCB_UART_GetNumReceived(SCB1, &UART1_context);
        aos_mutex_unlock(&scb1_rx_mutex);
        break;

    case UART5:
        aos_mutex_lock(&scb5_rx_mutex, AOS_WAIT_FOREVER);
        Cy_SCB_UART_Receive(SCB5, pdata, expect_size, &UART5_context);
        aos_sem_wait(&scb5_rx_sema, AOS_WAIT_FOREVER);
        rx_count = Cy_SCB_UART_GetNumReceived(SCB5, &UART5_context);
        aos_mutex_unlock(&scb5_rx_mutex);
        break;

    default:
        break;
    }

    if(recv_size != NULL)
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

int32_t hal_uart_finalize(uart_dev_t *uart)
{
    switch(uart->port)
    {
        case UART1:
        Cy_SCB_UART_DeInit(SCB1);
        break;

        case UART5:
        Cy_SCB_UART_DeInit(SCB5);
        break;

        default:
        break;
    }

    return 0;
}
