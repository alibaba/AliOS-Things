#include <stdbool.h>
#include <math.h>
#include "realview.h"
#include "uart_pl011.h"
#include "irq.h"

typedef struct {
    aos_sem_t rx_irq_bottom_sem;
    aos_mutex_t rx_cb_mutex;
    uart_rx_cb rx_cb;
    uart_dev_t *rx_cb_arg;
    aos_task_t *rx_irq_bottom_task;
} uart_context_t;

static const uint32_t refclock = 24000000u; /* 24 MHz */
static uart_registers* uart[4] = {
    (uart_registers*)REALVIEW_UART0_BASE,
    (uart_registers*)REALVIEW_UART1_BASE,
    (uart_registers*)REALVIEW_UART2_BASE,
    (uart_registers*)REALVIEW_UART3_BASE
};
static uart_buff *uart_recv_buf[4];
static uart_context_t uart_ctx[4];

static void rx_irq_bottom(void *arg)
{
    uart_context_t *uart_ctx = arg;

    while (1) {
        aos_sem_wait(&uart_ctx->rx_irq_bottom_sem, AOS_WAIT_FOREVER);
        aos_mutex_lock(&uart_ctx->rx_cb_mutex, AOS_WAIT_FOREVER);
        if (uart_ctx->rx_cb)
            uart_ctx->rx_cb(uart_ctx->rx_cb_arg);
        aos_mutex_unlock(&uart_ctx->rx_cb_mutex);
    }
}

uart_error uart_init(uint8_t port) {
    if(port < 4) {
        aos_status_t r;

        uart_recv_buf[port] = aos_zalloc(sizeof(uart_buff));
        aos_sem_new(&uart_recv_buf[port]->uart_rx_sem, 0);
        aos_sem_new(&uart_ctx[port].rx_irq_bottom_sem, 0);
        r = aos_mutex_new(&uart_ctx[port].rx_cb_mutex);
        if (r) {
            aos_sem_free(&uart_ctx[port].rx_irq_bottom_sem);
            aos_sem_free(&uart_recv_buf[port]->uart_rx_sem);
            aos_free(uart_recv_buf[port]);
            uart_recv_buf[port] = NULL;
            return -1;
        }
        uart_ctx[port].rx_cb = NULL;
        uart_ctx[port].rx_cb_arg = NULL;
        r = aos_task_new_ext(&uart_ctx[port].rx_irq_bottom_task,
                             "uart_rx_irq_bottom", rx_irq_bottom,
                             &uart_ctx[port], 4096, 20);
        if (r) {
            aos_mutex_free(&uart_ctx[port].rx_cb_mutex);
            aos_sem_free(&uart_ctx[port].rx_irq_bottom_sem);
            aos_sem_free(&uart_recv_buf[port]->uart_rx_sem);
            aos_free(uart_recv_buf[port]);
            uart_recv_buf[port] = NULL;
            return -1;
        }
    }
    else
        return UART_INVALID_PORT;

    return UART_OK;
}

uart_error uart_deinit(uint8_t port) {
    if(port < 4) {
        aos_task_delete(&uart_ctx[port].rx_irq_bottom_task);
        uart_ctx[port].rx_irq_bottom_task = NULL;
        aos_sem_free(&uart_ctx[port].rx_irq_bottom_sem);
        aos_mutex_free(&uart_ctx[port].rx_cb_mutex);
        uart_ctx[port].rx_cb = NULL;
        uart_ctx[port].rx_cb_arg = NULL;
        if(uart_recv_buf[port]) {
            aos_sem_free(&uart_recv_buf[port]->uart_rx_sem);
            aos_free(uart_recv_buf[port]);
            uart_recv_buf[port] = NULL;
        }
    }
    else
        return UART_INVALID_PORT;

    return UART_OK;
}

uart_error uart_configure(uint8_t port, uart_config* config) {
    uart_registers* uart0;
    if(port < 4)
        uart0 = uart[port];
    else
        return UART_INVALID_PORT;

    /* Validate config */
    if (config->data_bits < 5u || config->data_bits > 8u) {
        return UART_INVALID_ARGUMENT_WORDSIZE;
    }
    if (config->stop_bits == 0u || config->stop_bits > 2u) {
        return UART_INVALID_ARGUMENT_STOP_BITS;
    }
    if (config->baudrate < 110u || config->baudrate > 460800u) {
        return UART_INVALID_ARGUMENT_BAUDRATE;
    }
    /* Disable the UART */
    uart0->CR &= ~CR_UARTEN;
    /* Finish any current transmission, and flush the FIFO */
    while (uart0->FR & FR_BUSY);
    uart0->LCRH &= ~LCRH_FEN;

    /* Set baudrate */
    double intpart, fractpart;
    double baudrate_divisor = (double)refclock / (16u * config->baudrate);
    fractpart = modf(baudrate_divisor, &intpart);

    uart0->IBRD = (uint16_t)intpart;
    uart0->FBRD = (uint8_t)((fractpart * 64u) + 0.5);

    uint32_t lcrh = 0u;

    /* Set data word size */
    switch (config->data_bits)
    {
    case 5:
        lcrh |= LCRH_WLEN_5BITS;
        break;
    case 6:
        lcrh |= LCRH_WLEN_6BITS;
        break;
    case 7:
        lcrh |= LCRH_WLEN_7BITS;
        break;
    case 8:
        lcrh |= LCRH_WLEN_8BITS;
        break;
    }

    /* Set parity. If enabled, use even parity */
    if (config->parity) {
        lcrh |= LCRH_PEN;
        lcrh |= LCRH_EPS;
        lcrh |= LCRH_SPS;
    } else {
        lcrh &= ~LCRH_PEN;
        lcrh &= ~LCRH_EPS;
        lcrh &= ~LCRH_SPS;
    }

    /* Set stop bits */
    if (config->stop_bits == 1u) {
        lcrh &= ~LCRH_STP2;
    } else if (config->stop_bits == 2u) {
        lcrh |= LCRH_STP2;
    }

    /* Enable FIFOs */
    lcrh |= LCRH_FEN;

    uart0->LCRH = lcrh;

    uart0->IMSC |= IMSC_RXIM;

    /* Register the interrupt, to be updated */
    if(0 == port) {
        (void)irq_register_isr(UART0_INTERRUPT, uart0_isr);
        gic_enable_interrupt(UART0_INTERRUPT);
    } else if(1 == port) {
        (void)irq_register_isr(UART1_INTERRUPT, uart1_isr);
        gic_enable_interrupt(UART1_INTERRUPT);
    } else if(2 == port) {
        (void)irq_register_isr(UART2_INTERRUPT, uart2_isr);
        gic_enable_interrupt(UART2_INTERRUPT);
    } else if(3 == port) {
        (void)irq_register_isr(UART3_INTERRUPT, uart3_isr);
        gic_enable_interrupt(UART3_INTERRUPT);
    }

    /* Enable the UART */
    uart0->CR |= CR_UARTEN;

    return UART_OK;
}

void uart_putchar(uint8_t port, char c) {
    uart_registers* uart0;

#if ENABLE_USER_MODE_UART
    static int hint = 0;
    char *hints = "skip kernel space uart driver\n";
    char *ch = (char *)hints;
    int i = 0;

    if(port < 4)
        uart0 = uart[port];
    else
        return;

    if (hint) {
        return 0;
    }
    hint = 1;

    for(i = 0; i < strlen(hints); i++) {
        while (uart0->FR & FR_TXFF);
        uart0->DR = *ch;
        ch++;
    }
#else
    if(port < 4)
        uart0 = uart[port];
    else
        return;
    while (uart0->FR & FR_TXFF);
    uart0->DR = c;
#endif
}

void uart_write(uint8_t port, const char* data) {
    while (*data) {
        uart_putchar(port, *data++);
    }
}

void uart_write_uint(uint8_t port, uint32_t num) {
    char buf[8];
    int8_t i = 0;
    do {
        uint8_t remainder = num % 10;
        buf[i++] = '0' + remainder;
        num /= 10;
    } while (num != 0);
    for (i--; i >= 0; i--) {
        uart_putchar(port, buf[i]);
    }
}
int uart_rx_wait(uint8_t port, uint32_t timeout)
{
    return aos_sem_wait(&uart_recv_buf[port]->uart_rx_sem, timeout);
}

uart_error uart_getchar(uint8_t port, char* c) {
    uart_registers* uart0;
    if(port < 4)
        uart0 = uart[port];
    else
        return UART_INVALID_PORT;
    if (uart0->FR & FR_RXFE) {
        return UART_NO_DATA;
    }

    *c = uart0->DR & DR_DATA_MASK;
    if (uart0->RSRECR & RSRECR_ERR_MASK) {
        /* The character had an error */
        uart0->RSRECR &= RSRECR_ERR_MASK;
        return UART_RECEIVE_ERROR;
    }
    return UART_OK;
}

uart_error uart_getchar_from_interrupt(uint8_t port, char* c) {
    uart_registers* uart0;
    if(port < 4)
        uart0 = uart[port];
    else
        return UART_INVALID_PORT;
    uart_buff *uart0_ptr = uart_recv_buf[port];
    if(uart0_ptr->read_idx == uart0_ptr->write_idx)
        return UART_NO_DATA;
    else {
        *c = uart0_ptr->data[uart0_ptr->read_idx++];
        if(uart0_ptr->read_idx >= RECV_BUFF_SIZE)
            uart0_ptr->read_idx = 0;
    }
    return UART_OK;
}

void uart_isr(uint8_t port) {
    uart_registers* uart0 = uart[port];
    uart_buff *uart0_ptr = uart_recv_buf[port];
    uint32_t status = uart0->MIS;
    if (status & RX_INTERRUPT) {
        /* Read the received character and print it back*/
        char c = uart0->DR & DR_DATA_MASK;
        uart0_ptr->data[uart0_ptr->write_idx++] = c;
        if(uart0_ptr->write_idx >= RECV_BUFF_SIZE)
            uart0_ptr->write_idx = 0;
        aos_sem_signal(&uart0_ptr->uart_rx_sem);
        aos_sem_signal(&uart_ctx[port].rx_irq_bottom_sem);
    } else if (status & BE_INTERRUPT) {
        uart_write(port, "Break error detected!\n");
        /* Clear the error flag */
        uart0->RSRECR = ECR_BE;
        /* Clear the interrupt */
        uart0->ICR = BE_INTERRUPT;
    }
}

void uart0_isr(void) {
    uart_isr(0);
}

void uart1_isr(void) {
    uart_isr(1);
}

void uart2_isr(void) {
    uart_isr(2);
}

void uart3_isr(void) {
    uart_isr(3);
}

int uart_rx_callback_register(uart_dev_t *uart, uart_rx_cb cb)
{
    int id;

    if (!uart || uart->port >= 4)
        return -1;

    id = uart->port;
    aos_mutex_lock(&uart_ctx[id].rx_cb_mutex, AOS_WAIT_FOREVER);
    uart_ctx[id].rx_cb = cb;
    uart_ctx[id].rx_cb_arg = cb ? uart : NULL;
    aos_mutex_unlock(&uart_ctx[id].rx_cb_mutex);

    return 0;
}
