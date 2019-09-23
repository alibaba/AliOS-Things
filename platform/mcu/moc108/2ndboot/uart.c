/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include "bootloader.h"
#include "uart.h"

static void GPIO_UART_function_enable(unsigned char ucChannel)
{
    if (ucChannel == 0)
    {
        GPIO_UART0_RX_CONFIG = GPIO_CFG_OUTPUT_ENABLE_MASK
                             | GPIO_CFG_FUNCTION_ENABLE_SET;
        GPIO_UART0_TX_CONFIG = GPIO_CFG_OUTPUT_ENABLE_MASK
                             | GPIO_CFG_FUNCTION_ENABLE_SET;
        REG_GPIO_FUNTION_MODE &= ~(GPIO_X_FUNTION_MODE_MASK(GPIO_UART0_RX_PIN)
                                 | GPIO_X_FUNTION_MODE_MASK(GPIO_UART0_TX_PIN));
    }
    else if (ucChannel == 1)
    {
        GPIO_UART1_TX_CONFIG = GPIO_CFG_OUTPUT_ENABLE_MASK
                             | GPIO_CFG_FUNCTION_ENABLE_SET;
        GPIO_UART1_RX_CONFIG = GPIO_CFG_OUTPUT_ENABLE_MASK
                             | GPIO_CFG_FUNCTION_ENABLE_SET;
        REG_GPIO_FUNTION_MODE &= ~(GPIO_X_FUNTION_MODE_MASK(GPIO_UART1_RX_PIN)
                                 | GPIO_X_FUNTION_MODE_MASK(GPIO_UART1_TX_PIN));
    }
}

unsigned char uart0_recv_byte(unsigned char *c)
{
    if(!UART0_RX_READ_READY)
        return 0;

    *c = UART0_READ_BYTE();
    return 1;
}

static void uart0_init(void)
{
    REG_UART0_CONFIG = 0x00;

    // UART clock enable
    ICU_PERI_CLK_PWD_CLEAR(ICU_PERI_CLK_PWD_UART0_MASK);

    GPIO_UART_function_enable(0);

    REG_UART0_FLOW_CONTROL = 0x00;
    REG_UART0_WAKEUP_CONFIG = 0x00;

    REG_UART0_CONFIG = UART0_CONFIG_TX_ENABLE_SET
                     | UART0_CONFIG_RX_ENABLE_SET
                     | UART0_CONFIG_IRDA_MODE_CLEAR
                     | UART0_CONFIG_LENGTH_8_BIT
                     | UART0_CONFIG_PAR_ENABLE_CLEAR
                     | UART0_CONFIG_PAR_MODE_EVEN
                     | UART0_CONFIG_STOP_LEN_1_BIT
                     | (UART0_CLK_DIVID_SET << UART0_CONFIG_CLK_DIVID_POSI);
}

static void uart0_disable(void)
{
    REG_UART0_CONFIG = (REG_UART0_CONFIG & (~(0x01 << UART0_CONFIG_TX_ENABLE_POSI)))
                       | (0x01 << UART0_CONFIG_RX_ENABLE_POSI);
}

static void uart0_send_byte(unsigned char data)
{
    while (!UART0_TX_WRITE_READY);
    UART0_WRITE_BYTE(data);
}

static void uart0_send(unsigned char *buff, int len)
{
    while (len--)
        uart0_send_byte(*buff++);
}


int32_t hal_uart_init(uart_dev_t *uart)
{
    uart0_init();
    return 0;
}

int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
    int i = 0;
    unsigned char* buff = NULL;

    if (data == NULL) {
        return -1;
    }

    if (size == 0) {
        return -1;
    }

    buff = (unsigned char*)data;

    uart0_send(buff, size);

    return 0;
}

int32_t hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size, uint32_t *recv_size, uint32_t timeout)
{
    unsigned char *buff = NULL;
    uint32_t       size = 0;
    unsigned char  c;

    if ((data == NULL) || (recv_size == NULL)) {
        return -1;
    }

    if (expect_size == 0) {
        return -1;
    }

    buff = (unsigned char *)data;

    while (1) {
        if (0 == uart0_recv_byte(&c)) {
            break;
        }

        *buff = c;
        buff++;
        size++;
        if (size == expect_size) {
            break;
        }
    }

    *recv_size = size;
    return 0;
}

