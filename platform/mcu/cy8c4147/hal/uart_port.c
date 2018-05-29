#include "hal/soc/soc.h"
#include "board.h"
#include "uart_port.h"
#include <project.h>

#include <stdio.h>
#include <k_api.h>
#include <aos\kernel.h>

uart_dev_t uart_0;
#ifndef CERTIFICATION
uart_os_t uart_os[UARTn];
#endif

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

#ifndef NO_PRINTF
#if defined (__CC_ARM) && defined(__MICROLIB)
 
    /***************************************************************************
    * Function Name: fputc
    ***************************************************************************/
    __attribute__((weak)) int fputc(int ch, FILE *f)
    {
        (void)f;
        if (ch == '\n') {
            hal_uart_send(&console_uart, (void *)"\r", 1, 30000);
        }
        hal_uart_send(&console_uart, &ch, 1, 30000);
        return (ch);
    }
    
    /***************************************************************************
    * Function Name: fgetc
    ***************************************************************************/
    __attribute__((weak)) int fgetc(FILE *f) 
    {
        (void)f;
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
#elif defined(__ICCARM__)
    #include <yfuns.h>

    /***************************************************************************
    * Function Name: __write
    ***************************************************************************/
    __weak size_t __write(int handle, const unsigned char * buffer, size_t size)
    {
        size_t nChars = 0;
        /* This template only writes to "standard out", for all other file
        * handles it returns failure. */
        if (handle != _LLIO_STDOUT)
        {
            return (_LLIO_ERROR);
        }
        if (buffer != NULL)
        {
            for (/* Empty */; nChars < size; ++nChars)
            {
                if(*buffer == '\n')
                {
                    hal_uart_send(&console_uart, (void *)"\r", 1, 30000);
                }
                hal_uart_send(&console_uart, buffer, 1, 30000);
                ++buffer;
            }
        }
        return (nChars);
    }

    __weak size_t __read(int handle, unsigned char * buffer, size_t size)
    {
        uint32_t recv_size;
        int32_t ret = 0;
        
        /* This template only reads from "standard in", for all other file
        handles it returns failure. */
        if ((handle != _LLIO_STDIN) || (buffer == NULL))
        {
            return (_LLIO_ERROR);
        }
        else
        {
            ret = hal_uart_recv_II(&console_uart, buffer, 1, &recv_size, HAL_WAIT_FOREVER);
            return (1);
        }
    }    
#else /* (__GNUC__)  GCC */
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
                    hal_uart_send(&console_uart, (void *)"\r", 1, 30000);
                }
                hal_uart_send(&console_uart, ptr, 1, 30000);
                ++ptr;
            }
        }
        return (nChars);
    }
    
    /* Add an explicit reference to the floating point scanf library to allow
    the usage of floating point conversion specifier. */
    __asm (".global _scanf_float");
    __attribute__((weak)) int _read (int fd, char *ptr, int len)
    {
        int nChars = 0;
        uint32_t recv_size;
        int32_t ret = 0;
        
        (void)fd;
        if (ptr != NULL)
        {
            for(/* Empty */;nChars < len;++ptr)
            {
                ret = hal_uart_recv_II(&console_uart, ptr, 1, &recv_size, HAL_WAIT_FOREVER);
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

#endif


void UART_API(_customISR)(void)
{
#if 0
    /* TX FIFO empty */
    if(0u != (UART_API(_GetTxInterruptSourceMasked()) & UART_FLAG(_INTR_TX_EMPTY)))
    {
        UART_API(_ClearTxInterruptSource)(UART_FLAG(_INTR_TX_EMPTY));
    }
    
    /* TX FIFO overflow */
    if(0u != (UART_API(_GetTxInterruptSourceMasked()) & UART_FLAG(_INTR_TX_OVERFLOW)))
    {
        UART_API(_ClearTxInterruptSource)(UART_FLAG(_INTR_TX_OVERFLOW));
    }    
    
    /* TX FIFO underflow */
    if(0u != (UART_API(_GetTxInterruptSourceMasked()) & UART_FLAG(_INTR_TX_UNDERFLOW)))
    {
        UART_API(_ClearTxInterruptSource)(UART_FLAG(_INTR_TX_UNDERFLOW));
    }
#endif

    /* UART done */
    if(0u != (UART_API(_GetTxInterruptSourceMasked)() & UART_FLAG(_INTR_TX_UART_DONE)))
    {
        UART_API(_ClearTxInterruptSource)(UART_FLAG(_INTR_TX_UART_DONE));
        #ifndef CERTIFICATION
        //need to change to something more generic
        aos_sem_signal(&uart_os[STDIO_UART].uart_tx_sem);
        #endif
    }

#if 0
    /* RX FIFO full */
    if(0u != (UART_API(_GetRxInterruptSourceMasked()) & UART_FLAG(_INTR_RX_FULL)))
    {
        UART_API(_ClearRxInterruptSource)(UART_FLAG(_INTR_RX_FULL));
    }    
    /* RX FIFO overflow */
    if(0u != (UART_API(_GetRxInterruptSourceMasked()) & UART_FLAG(_INTR_RX_OVERFLOW)))
    {
        UART_API(_ClearRxInterruptSource)(UART_FLAG(_INTR_RX_OVERFLOW));
    }    
    /* RX FIFO underflow */
    if(0u != (UART_API(_GetRxInterruptSourceMasked()) & UART_FLAG(_INTR_RX_UNDERFLOW)))
    {
        UART_API(_ClearRxInterruptSource)(UART_FLAG(_INTR_RX_UNDERFLOW));
    }    
#endif
}

int32_t hal_uart_init(uart_dev_t *uart)
{
    (void)uart;
    
    #ifndef CERTIFICATION
    aos_sem_new(&uart_os[uart->port].uart_tx_sem, 0);
    aos_sem_new(&uart_os[uart->port].uart_rx_sem, 0);
    aos_mutex_new(&uart_os[uart->port].uart_tx_mutex);
    aos_mutex_new(&uart_os[uart->port].uart_rx_mutex);
    #endif
    UART_API(_SetCustomInterruptHandler)(&UART_API(_customISR));
    UART_API(_Start)();

    return 0;
}

int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
    uint8 *pdata = (uint8 *)data;
    uint32 i = 0;
    
    (void) timeout;
    
    if((uart == NULL) || (data == NULL))
    {
        return -1;
    }
    

    #ifndef CERTIFICATION
    aos_mutex_lock(&uart_os[uart->port].uart_tx_mutex, AOS_WAIT_FOREVER);
    #endif
    UART_API(_SpiUartPutArray)(pdata, size);
    #ifndef CERTIFICATION
    aos_sem_wait(&uart_os[uart->port].uart_tx_sem, AOS_WAIT_FOREVER);
    aos_mutex_unlock(&uart_os[uart->port].uart_tx_mutex);
    #endif

    return 0;
}

int32_t hal_uart_recv(uart_dev_t *uart, void *data, uint32_t expect_size, uint32_t timeout)
{
    uint32 *pdata = (uint32 *)data;
    uint32 i = 0;
    
    (void)timeout;
    (void)uart;
    
    #ifndef CERTIFICATION
    aos_mutex_lock(&uart_os[uart->port].uart_rx_mutex, AOS_WAIT_FOREVER);
    #endif
    for(i = 0; i < expect_size; i++)
    {
        *pdata = UART_API(_UartGetChar());
    }
    #ifndef CERTIFICATION
    aos_mutex_unlock(&uart_os[uart->port].uart_rx_mutex);
    #endif

    return 0;
}

int32_t hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size,
                      uint32_t *recv_size, uint32_t timeout)
{
    uint32 *pdata = (uint32 *)data;
    uint32 i = 0;
    uint32 rx_count = 0;
    
    (void)timeout;
    (void)uart;
    
    #ifndef CERTIFICATION
    aos_mutex_lock(&uart_os[uart->port].uart_rx_mutex, AOS_WAIT_FOREVER);
    #endif
    for(i = 0; i < expect_size; i++)
    {
        *pdata = UART_API(_UartGetChar());
        if(*pdata != 0x0)
        {
            rx_count++;
        }   
    }
    if(recv_size != NULL)
    {
        *recv_size = rx_count;
    }
    #ifndef CERTIFICATION
    aos_mutex_unlock(&uart_os[uart->port].uart_rx_mutex);
    #endif
    
    return 0;
}

int32_t hal_uart_finalize(uart_dev_t *uart)
{
    (void)uart;
    
    UART_API(_Stop());
    
    return 0;
}
