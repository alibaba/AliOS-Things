/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include "uart.h"
#include "2ndboot.h"

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

unsigned char uart_recv_byte(unsigned char *c)
{
    unsigned char tc = 0;

    if(uart0_recv_byte(&tc)) {
        *c = tc;
        return 1;
    }

    return 0;
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

static void uart0_send_string(char *buff)
{
    while (*buff)
        uart0_send_byte(*buff++);
}

void uart_init(void)
{
    uart0_init();
    uart0_send_string("uart0 init suc.\n");
}

void uart_send_byte(unsigned char c)
{
    uart0_send_byte(c);
}

void uart_send(unsigned char *buff, int len)
{
    uart0_send(buff, len);
}

void uart_send_string(char *buff)
{
    uart0_send_string(buff);
}

