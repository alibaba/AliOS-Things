/**
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "uart.h"


static void _hal_uart_putchar(uint8_t ch);

void hal_uart_init(void)
{
    _hal_uart_init(1, 115200, 0, 8, 1);
}

void hal_uart_putchar(uint8_t ch)
{
    _hal_uart_putchar(ch);
}

void ck_uart_putchar(int8_t ch)
{
    _hal_uart_putchar(ch);
}

static void uart_putc(int ch)
{
    _hal_uart_putchar(ch);
}

int putchar(int ch)
{
    if (ch == '\n') {
        uart_putc('\r');
    }

    uart_putc(ch);

    return 0;
}

static hal_uart_info_t dbg_console;

static void _uart_udelay(int usec)
{
    int          i;
    volatile int j;

    for (i = 0x00; i < usec * 100; i++) {
        j = i;
    }
}

static int32_t _uart_set_reuse(uint32_t id)
{

    if (id == UART0) {

    } else if (id == UART1) {

    } else if (id == UART2) {
        // gpio18 for uart2 tx bit6-8, gpio19 for uart2 rx bit9-11
        // tmp = (*(volatile uint32_t*)(PAD_CRTL_BASE+0xc))&(~(0x3f<<6));//clear
        // bit6-11
        //*(volatile uint32_t*)(PAD_CRTL_BASE+0xc) = tmp|(4<<9)|(4<<6);
    } else {
        return -1;
    }

    return 0;
}

static int32_t _uart_set_clk(uint32_t id)
{

    if (id == UART0) {

    } else if (id == UART1) {

    } else if (id == UART2) {
        // uart2 wclk and pclk enable
        //*(volatile uint32_t*)(LSP_CRM_BASE + 0x4c) |= (3<<4);
    } else {
        return -1;
    }

    return 0;
}

static int32_t _uart_reg_init(uint32_t id)
{
    volatile hal_uart_reg_t *pReg = NULL;

    pReg = dbg_console.addr;

    /*¹Ø±ÕUART*/
    pReg->cr = 0x0;

    /*Çå´íÎó×´Ì¬*/
    pReg->rsr = 0x00;

    /*ÆÁ±ÎËùÓÐÖÐ¶Ï*/
    pReg->imsc = 0x0;

    /*Çå³ýËùÓÐÖÐ¶Ï*/
    pReg->icr = 0xffff;

    /*DMA·¢ËÍ½ûÄÜ*/
    pReg->dmacr = 0x0;

    return 0;
}

static int _uart_set_baudrate(uart_baudrate_t baudrate, uint32_t apbfreq)
{
    uint32_t                 ibrd, fbrd;
    volatile hal_uart_reg_t *pReg = NULL;

    pReg = dbg_console.addr;

    /* baudrate=(seriak clock freq)/(16*divisor). */
    // divisor  = ((apbfreq / baudrate) >> 4);
    ibrd = apbfreq / (baudrate << 4);
    fbrd = ((apbfreq % (baudrate << 4)) * 8 + baudrate) / (2 * baudrate);

    pReg->fbrd = fbrd;
    pReg->ibrd = ibrd;

    return 0;
}

static int _uart_set_parity(uart_parity_t parity)
{
    int                      ret  = 0;
    uint32_t                 tmp  = 0;
    volatile hal_uart_reg_t *pReg = NULL;

    pReg = dbg_console.addr;

    switch (parity) {
        case NONE:
            /* clear the PEN bit(LCR[3]) to disable parity */
            pReg->lcr_h &= (~UART_LCRH_PEN);
            break;

        case ODD:
            /* set PEN and clear EPS(LCR[4]) to set the ODD parity */
            tmp = pReg->lcr_h;
            tmp |= UART_LCRH_PEN;
            tmp &= (~UART_LCRH_EPS);
            pReg->lcr_h = tmp;
            break;

        case EVEN:
            /* set PEN and EPS(LCR[4]) to set the EVEN parity */
            tmp = pReg->lcr_h;
            tmp |= UART_LCRH_PEN;
            tmp |= UART_LCRH_EPS;
            pReg->lcr_h = tmp;
            break;

        default:
            return -1;
            break;
    }

    return ret;
}

static int _uart_set_wordsize(uart_wordsize_t wordsize)
{
    int                      ret  = 0;
    uint32_t                 tmp  = 0;
    volatile hal_uart_reg_t *pReg = NULL;

    pReg = dbg_console.addr;

    /* DLS(LCR[1:0]) is writeable when the UART is not busy(USR[0]=0).*/

    /* the word size decides by the DLS bits(LCR[1:0]), and the
     * corresponding relationship between them is:
     *    DLS   word size
     *    00 -- 5 bits
     *    01 -- 6 bits
     *    10 -- 7 bits
     *    11 -- 8 bits
     */
    tmp = pReg->lcr_h;

    switch (wordsize) {
        case WORD_SIZE_5:
            tmp &= LCR_WORD_SIZE_5;
            break;

        case WORD_SIZE_6:
            tmp &= 0xfd;
            tmp |= LCR_WORD_SIZE_6;
            break;

        case WORD_SIZE_7:
            tmp &= 0xfe;
            tmp |= LCR_WORD_SIZE_7;
            break;

        case WORD_SIZE_8:
            tmp |= LCR_WORD_SIZE_8;
            break;

        case WORD_SIZE_9:
            tmp |= LCR_WORD_SIZE_9;
            break;

        default:
            ret = -1;
            break;
    }

    tmp |= UART_LCRH_FEN;

    pReg->lcr_h = tmp;

    return ret;
}

static int _uart_set_stopbit(uart_stopbit_t stopbit)
{
    int                      ret  = 0;
    volatile hal_uart_reg_t *pReg = NULL;

    pReg = dbg_console.addr;

    /* PEN bit(LCR[3]) is writeable when the UART is not busy(USR[0]=0).*/
    switch (stopbit) {
        case LCR_STOP_BIT_1:
            /* Clear the STOP bit to set 1 stop bit*/
            pReg->lcr_h &= (~LCR_STOP_BIT2);
            break;

        case LCR_STOP_BIT_2:
            /*
             * If the STOP bit is set "1",we'd gotten 1.5 stop
             * bits when DLS(LCR[1:0]) is zero, else 2 stop bits.
             */
            pReg->lcr_h |= LCR_STOP_BIT2;
            break;

        default:
            ret = -1;
            break;
    }

    return ret;
}

static void _uart_set_rxmode(bool bquery)
{
    volatile hal_uart_reg_t *pReg = NULL;

    pReg = dbg_console.addr;

    /* PEN bit(LCR[3]) is writeable when the UART is not busy(USR[0]=0).*/
    if (bquery) {
        /* When query mode, disable the Received Data Available
         * Interrupt. To do this, we clear the ERBFI bit(IER[0]).
         */
        pReg->imsc &= ~(UART_RXIM | UART_RTIM | UART_FEIM | UART_PEIM |
                        UART_BEIM | UART_OEIM);

    } else {
        /* When interrupt mode, inable the Received Data Available
         * Interrupt. To do this, we set the ERBFI bit(IER[0]).
         */
        pReg->imsc |= (UART_RXIM | UART_RTIM | UART_FEIM | UART_PEIM |
                       UART_BEIM | UART_OEIM);
    }
}

static void _uart_set_txmode(bool bquery)
{
    volatile hal_uart_reg_t *pReg = NULL;

    pReg = dbg_console.addr;

    /* PEN bit(LCR[3]) is writeable when the UART is not busy(USR[0]=0).*/
    if (bquery) {
        /* When query mode, disable the Transmit Holding Register Empty
         * Interrupt. To do this, we clear the ETBEI bit(IER[1]).
         */
        pReg->imsc &= ~UART_TXIM;

    } else {
        /* When interrupt mode, inable the Transmit Holding Register
         * Empty Interrupt. To do this, we set the ETBEI bit(IER[1]).
         */
        pReg->imsc |= UART_TXIM;
    }
}

static void _uart_enable(void)
{
    volatile hal_uart_reg_t *pReg = NULL;

    pReg = dbg_console.addr;

    pReg->cr |= (UART_CR_RXE | UART_CR_TXE | UART_CR_UARTEN);
}

uint8_t hal_uart_getchar(void)
{
    uint32_t                 timeout_cnt = 0;
    volatile hal_uart_reg_t *pReg        = NULL;

    pReg = dbg_console.addr;

    while (pReg->fr & 0x10) {
        _uart_udelay(TIMEOUT_COUNT_CYCLE);
        timeout_cnt++;

        if (timeout_cnt > TIMEOUT_COUNT_VALUE) {
            /* time out */
            return -1;
        }
    }

    return pReg->dr;
}

void _hal_uart_putchar(uint8_t ch)
{
    volatile hal_uart_reg_t *pReg = NULL;

    pReg = (volatile hal_uart_reg_t *)SYS_UART1_ADDR;

    /* wait uart available */
    while (pReg->fr & 0x20)
        ;

    if (ch == '\n') {
        pReg->dr = '\r';

        /* wait again because we will send '\n' later */
        while (pReg->fr & 0x20)
            ;
    }

    pReg->dr = ch;
}

int32_t _hal_uart_init(uint32_t id, uint32_t baudrate, uint32_t parity,
                       int32_t wordsize, int32_t stopbit)
{
    hal_uart_info_t *info = &dbg_console;
    uint32_t         addr = 0;

    if (id == UART0) {
        addr = SYS_UART0_ADDR;
    } else if (id == UART1) {
        addr = SYS_UART1_ADDR;
    } else if (id == UART2) {
        addr = SYS_UART2_ADDR;
    } else {
        return -1;
    }

    /* Get Addr */
    info->addr = (volatile hal_uart_reg_t *)addr;

    _uart_set_reuse(id);

    _uart_set_clk(id);

    _uart_reg_init(id);

    _uart_set_baudrate(baudrate, SYS_UART_CLK);
    info->baudrate = baudrate;

    _uart_set_parity(parity);
    info->parity = parity;

    _uart_set_wordsize(wordsize);
    info->word_size = wordsize;

    _uart_set_stopbit(stopbit);
    info->stop_bits = stopbit;

    _uart_set_rxmode(false);
    info->rx_bquery = false;

    _uart_set_txmode(true);
    info->tx_bquery = true;

    _uart_enable();

    return 0;
}
