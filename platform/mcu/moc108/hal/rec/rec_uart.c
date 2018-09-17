/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include "rec_uart.h"

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

static void uart1_init(void)
{
    REG_UART1_CONFIG = 0x00;        // by gwf

    // UART clock enable
    ICU_PERI_CLK_PWD_CLEAR(ICU_PERI_CLK_PWD_UART1_MASK);

    GPIO_UART_function_enable(1);

    REG_UART1_FLOW_CONTROL = 0x00;
    REG_UART1_WAKEUP_CONFIG = 0x00;

    REG_UART1_CONFIG = (0x01 << UART1_CONFIG_TX_ENABLE_POSI)
                     | (0x01 << UART1_CONFIG_RX_ENABLE_POSI)
                     | (0x00 << UART1_CONFIG_IRDA_MODE_POSI)
                     | (0x03 << UART1_CONFIG_LENGTH_POSI)
                     | (0x00 << UART1_CONFIG_PAR_ENABLE_POSI)
                     | (0x00 << UART1_CONFIG_PAR_MODE_POSI)
                     | (0x00 << UART1_CONFIG_STOP_LEN_POSI)
                     | (UART1_CLK_DIVID_SET << UART1_CONFIG_CLK_DIVID_POSI);

}

static void uart1_disable(void)
{
    REG_UART1_CONFIG = (REG_UART1_CONFIG & (~(0x01 << UART1_CONFIG_TX_ENABLE_POSI)))
                       | (0x01 << UART1_CONFIG_RX_ENABLE_POSI);
}

static void uart1_send_byte(unsigned char data)
{
    while (!UART1_TX_WRITE_READY);
    UART1_WRITE_BYTE(data);
}

static void uart1_send(unsigned char *buff, int len)
{
    while (len--)
        uart1_send_byte(*buff++);
}

static void uart1_send_string(char *buff)
{
    while (*buff)
        uart1_send_byte(*buff++);
}

void rec_uart_init()
{
    uart0_init();
    uart0_send_string("uart0 init success\r\n");
    
    uart1_init();
    uart1_send_string("uart1 init success\r\n");
}

void rec_uart_deinit()
{
    uart0_disable();
    uart1_disable();
}


void rec_uart_send(unsigned char *buff, int len)
{
    uart0_send(buff, len);
    uart1_send(buff, len);
}

void rec_uart_send_string(char *buff)
{
    uart0_send_string(buff);
    uart1_send_string(buff);
}

