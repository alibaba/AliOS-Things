/**
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "hal_uart.h"


void hal_uart_putchar(uint8_t ch)
{
    volatile CK_UART_REG *info = (CK_UART_REG *)PLATFORM_UART_ADDRBASE0;

    /* wait uart available */
    while ((!(info->uart_LSR & CK_LSR_TRANS_EMPTY)))
        ;

    if (ch == '\n') {
        info->uart_THR = '\r';
        /* wait again because we will send '\n' later */
        while ((!(info->uart_LSR & CK_LSR_TRANS_EMPTY)))
            ;
    }
    info->uart_THR = ch;
}

CK_UINT32 CK_GPIOA_SetReuse(CK_UINT32 pins, BOOL bSoftware)
{
    PCKStruct_GPIO reg;
    reg = (PCKStruct_GPIO)PLATFORM_UART_GPIO_REUSE_BASE;

    reg->SWPORT_DDR                                               = 0;
    reg->PORT_CTL                                                 = pins;
    *(volatile uint32_t *)(PLATFORM_UART_GPIO_REUSE_BASE + 0x100) = 0xa000000a;
    return 0;
}

int32_t hal_uart_init(uint32_t addr, uint32_t baudrate, uint32_t parity,
                      int32_t wordsize, int32_t stopbit)
{
    volatile CK_UART_REG *info = (CK_UART_REG *)addr;
    uint32_t              divisor;

    CK_GPIOA_SetReuse(0xe03, CK_GPIO_BEHARDWARE);

    info->uart_FCR = 0x1 | (0x2 << 6);

    while (info->uart_LSR & 0x1) {
        divisor = info->uart_RBR;
    }

    do {
        int32_t timecount = 0;
        while ((info->uart_USR & USR_UART_BUSY) && (timecount < 1000000)) {
            timecount++;
        }
        if (timecount >= 1000000) {
            return -1;
        }
    } while (0);

    /*baudrate=(seriak clock freq)/(16*divisor).*/
    divisor = ((APB_DEFAULT_FREQ / baudrate) >> 4);
    while (info->uart_LSR & 0x1) {
        divisor = info->uart_RBR;
    }

    do {
        int32_t timecount = 0;
        while ((info->uart_USR & USR_UART_BUSY) && (timecount < 1000000)) {
            timecount++;
        }
        if (timecount >= 1000000) {
            return -1;
        }
    } while (0);

    info->uart_LCR |= LCR_SET_DLAB;
    info->uart_DLL = divisor & 0xff;
    info->uart_DLH = (divisor >> 8) & 0xff;
    info->uart_LCR &= (~LCR_SET_DLAB);

    info->uart_LCR &= (~LCR_PARITY_ENABLE);

    info->uart_LCR &= LCR_STOP_BIT1;

    info->uart_LCR |= LCR_WORD_SIZE_8;

    // info->uart_IER &= (IER_RDA_INT_ENABLE | IER_THRE_INT_ENABLE);

    return 0;
}
