/**
 * Copyright (C) 2015-2016 The YunOS Project. All rights reserved.
 */

#include "hal_uart.h"

static hal_uart_info_t dbg_console;

uint8_t hal_uart_getchar(void)
{
    uint8_t tmp = 0;

    hal_uart_info_t *info = &dbg_console;
    while (!(info->addr[HAL_UART_LSR] & LSR_DATA_READY));

    tmp = info->addr[HAL_UART_RBR];
    return tmp;
}

void hal_uart_putchar(uint8_t ch)
{
    hal_uart_info_t *info = &dbg_console;

    /* wait uart available */
    while ((!(info->addr[HAL_UART_LSR] & HAL_LSR_TRANS_EMPTY)));

    if (ch == '\n') {
        info->addr[HAL_UART_THR] = '\r';
        /* wait again because we will send '\n' later */
        while ((!(info->addr[HAL_UART_LSR] & HAL_LSR_TRANS_EMPTY)));
    }
    info->addr[HAL_UART_THR] = ch;
}

int32_t hal_uart_init(
        uint32_t addr,
        uint32_t baudrate,
        uint32_t parity,
        int32_t wordsize,
        int32_t stopbit)
{
    hal_uart_info_t *info = &dbg_console;
    int32_t divisor = 0;
    int32_t timecount = 0;

    if (!addr) {
        return -1;
    }

    /* Get Addr */
    info->addr = (volatile uint32_t *)addr;

    /* gpio reuse for uart */
    (*(volatile unsigned int *)(PLATFORM_UART_GPIO_REUSE_BASE)) |= 0xc00;

    /* 1. deactive uart first! */
    info->addr[HAL_UART_LCR] = 0x83;
    info->addr[HAL_UART_DLL] = 0x1;
    info->addr[HAL_UART_DLH] = 0x0;

    /* 2. wait uart status */
    timecount = 0;
    /* PEN bit(LCR[3]) is writeable when the UART is not busy(USR[0]=0).*/
    while ((info->addr[HAL_UART_USR] & USR_UART_BUSY) &&
           (timecount < UART_BUSY_TIMEOUT)) {
        timecount++;
    }

    if (timecount >= UART_BUSY_TIMEOUT) {
        return -1;
    }

    /* 3. set uart baudrate */
    /* baudrate=(seriak clock freq)/(16*divisor). */
    divisor = ((APB_DEFAULT_FREQ / baudrate) >> 4);
    if (baudrate == 115200) {
        divisor++;
    }
    info->addr[HAL_UART_LCR] |= LCR_SET_DLAB;
    /* DLL and DLH is lower 8-bits and higher 8-bits of divisor.*/
    info->addr[HAL_UART_DLL] = divisor & 0xff;
    info->addr[HAL_UART_DLH] = (divisor >> 8) & 0xff;
    /*
     * The DLAB must be cleared after the baudrate is setted
     * to access other registers.
     */
    info->addr[HAL_UART_LCR] &= (~LCR_SET_DLAB);
    info->baudrate = baudrate;

    /* 4. set uart parity */
    switch (parity) {
        case 0: /* NONE */
            /*CLear the PEN bit(LCR[3]) to disable parity.*/
            info->addr[HAL_UART_LCR] &= (~LCR_PARITY_ENABLE);
            break;
        case 8: /* ODD */
            /* Set PEN and clear EPS(LCR[4]) to set the ODD parity. */
            info->addr[HAL_UART_LCR] |= LCR_PARITY_ENABLE;
            info->addr[HAL_UART_LCR] &= LCR_PARITY_ODD;
            break;
        case 16: /* EVEN */
            /* Set PEN and EPS(LCR[4]) to set the EVEN parity.*/
            info->addr[HAL_UART_LCR] |= LCR_PARITY_ENABLE;
            info->addr[HAL_UART_LCR] |= LCR_PARITY_EVEN;
            break;
        default:
            return -1;
    }
    info->parity = parity;

    /* 5. set uart wordsize */
    /*
     * The word size decides by the DLS bits(LCR[1:0])
     */
    switch (wordsize) {
        case 5:
            info->addr[HAL_UART_LCR] &= LCR_WORD_SIZE_5;
            break;
        case 6:
            info->addr[HAL_UART_LCR] &= 0xfd;
            info->addr[HAL_UART_LCR] |= LCR_WORD_SIZE_6;
            break;
        case 7:
            info->addr[HAL_UART_LCR] &= 0xfe;
            info->addr[HAL_UART_LCR] |= LCR_WORD_SIZE_7;
            break;
        case 8:
            info->addr[HAL_UART_LCR] |= LCR_WORD_SIZE_8;
            break;
        default:
            return -1;
    }
    info->word_size = wordsize;

    /* 6. set uart stopbit */
    switch (stopbit) {
        case 1:
            /* Clear the STOP bit to set 1 stop bit*/
            info->addr[HAL_UART_LCR] &= LCR_STOP_BIT1;
            break;
        case 2:
            /*
            * If the STOP bit is set "1",we'd gotten 1.5 stop
            * bits when DLS(LCR[1:0]) is zero, else 2 stop bits.
            */
            info->addr[HAL_UART_LCR] |= LCR_STOP_BIT2;
            break;
        default:
            return -1;
    }
    info->stop_bits = stopbit;

    /* 7. Disable RX/TX Interrupt */
    info->addr[HAL_UART_IER] &= (~IER_RDA_INT_ENABLE);
    info->addr[HAL_UART_IER] &= (~IER_THRE_INT_ENABLE);

    /* 8. Stop UART interrupt if have */
    info->addr[HAL_UART_IER] &= ~IER_RDA_INT_ENABLE;

    return 0;
}
