/*
 * File      : drv_pl041.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2017, RT-Thread Development Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-05-25     RT-Thread    the first version
 */
#include <stdlib.h>
#include <stdio.h>
#include "drv_pl041.h"
#include "drv_ac97.h"
#include "irq.h"

#define DBG_TAG  "PL041"
// #define DBG_LVL         DBG_LOG
// #define DBG_LVL         DBG_INFO
#define DBG_LVL  DBG_WARNING
// #define DBG_LVL         DBG_ERROR

#define FRAME_PERIOD_US    (50)
#define PL041_CHANNEL_NUM  (4)

#define PL041_READ(_a)        (*(volatile unsigned int *)(_a))
#define PL041_WRITE(_a, _v)   (*(volatile unsigned int *)(_a) = (_v))

#define PL041_INTERRUPT_ID (43)

struct pl041_irq_def
{
    pl041_irq_fun_t fun;
    void *user_data;
};

static struct pl041_irq_def irq_tbl[PL041_CHANNEL_NUM];

static void aaci_pl041_delay(unsigned int us)
{
    volatile int i;

    for (i = us * 10; i != 0; i--);
}

static void aaci_ac97_select_codec(void)
{
    unsigned int v, maincr;

    maincr = AACI_MAINCR_SCRA(0) | AACI_MAINCR_IE | AACI_MAINCR_SL1RXEN | \
             AACI_MAINCR_SL1TXEN | AACI_MAINCR_SL2RXEN | AACI_MAINCR_SL2TXEN;

    v = PL041_READ(&PL041->slfr);
    if (v & AACI_SLFR_2RXV)
    {
        PL041_READ(&PL041->sl2rx);
    }
    if (v & AACI_SLFR_1RXV)
    {
        PL041_READ(&PL041->sl1rx);
    }

    if (maincr != PL041_READ(&PL041->maincr))
    {
        PL041_WRITE(&PL041->maincr, maincr);
        aaci_pl041_delay(1);
    }
}

void aaci_ac97_write(unsigned short reg, unsigned short val)
{
    unsigned int v, timeout;

    aaci_ac97_select_codec();

    PL041_WRITE(&PL041->sl2tx, val << 4);
    PL041_WRITE(&PL041->sl1tx, reg << 12);

    aaci_pl041_delay(FRAME_PERIOD_US);

    timeout = FRAME_PERIOD_US * 8;
    do
    {
        aaci_pl041_delay(1);
        v = PL041_READ(&PL041->slfr);
    }
    while ((v & (AACI_SLFR_1TXB | AACI_SLFR_2TXB)) && --timeout);

    if (v & (AACI_SLFR_1TXB | AACI_SLFR_2TXB))
    {
        printf("timeout waiting for write to complete \r\n");
    }
}

unsigned short aaci_ac97_read(unsigned short reg)
{
    unsigned int v, timeout, retries = 10;

    aaci_ac97_select_codec();

    PL041_WRITE(&PL041->sl1tx, (reg << 12) | (1 << 19));
    aaci_pl041_delay(FRAME_PERIOD_US);

    timeout = FRAME_PERIOD_US * 8;
    do
    {
        aaci_pl041_delay(1);
        v = PL041_READ(&PL041->slfr);
    }
    while ((v & AACI_SLFR_1TXB) && --timeout);

    if (v & AACI_SLFR_1TXB)
    {
        printf("timeout on slot 1 TX busy \r\n");
        v = ~0x0;
        return v;
    }

    aaci_pl041_delay(FRAME_PERIOD_US);
    timeout = FRAME_PERIOD_US * 8;
    do
    {
        aaci_pl041_delay(1);
        v = PL041_READ(&PL041->slfr) & (AACI_SLFR_1RXV | AACI_SLFR_2RXV);
    }
    while ((v != (AACI_SLFR_1RXV | AACI_SLFR_2RXV)) && --timeout);

    if (v != (AACI_SLFR_1RXV | AACI_SLFR_2RXV))
    {
        printf("timeout on RX valid \r\n");
        v = ~0x0;
        return v;
    }

    do
    {
        v = PL041_READ(&PL041->sl1rx) >> 12;
        if (v == reg)
        {
            v = PL041_READ(&PL041->sl2rx) >> 4;
            break;
        }
        else if (--retries)
        {
            printf("ac97 read back fail. retry \r\n");
            continue;
        }
        else
        {
            printf("wrong ac97 register read back (%x != %x) \r\n", v, reg);
            v = ~0x0;
        }
    }
    while (retries);

    return v;
}

int aaci_pl041_channel_disable(int channel)
{
    unsigned int v;
    void *p_rx, *p_tx;

    p_rx = (void *)((unsigned int)(&PL041->rxcr1) + channel * 0x14);
    p_tx = (void *)((unsigned int)(&PL041->txcr1) + channel * 0x14);
    v = PL041_READ(p_rx);
    v &= ~AACI_CR_EN;
    PL041_WRITE(p_rx, v);
    v = PL041_READ(p_tx);
    v &= ~AACI_CR_EN;
    PL041_WRITE(p_tx, v);
    printf("aaci_pl041_channel_disable %d \r\n", channel);
    return 0;
}

int aaci_pl041_channel_enable(int channel)
{
    unsigned int v;
    void *p_rx, *p_tx;

    p_rx = (void *)((unsigned int)(&PL041->rxcr1) + channel * 0x14);
    p_tx = (void *)((unsigned int)(&PL041->txcr1) + channel * 0x14);
    v = PL041_READ(p_rx);
    v |= AACI_CR_EN;
    PL041_WRITE(p_rx, v);
    v = PL041_READ(p_tx);
    v |= AACI_CR_EN;
    PL041_WRITE(p_tx, v);
    printf("aaci_pl041_channel_enable %d \r\n", channel);
    return 0;
}

int aaci_pl041_channel_read(int channel, unsigned short *buff, int count)
{
    void *p_data, *p_status;
    int i = 0;

    p_status = (void *)((unsigned int)(&PL041->sr1) + channel * 0x14);
    p_data = (void *)((unsigned int)(&(PL041->dr1[0])) + channel * 0x20);
    for (i = 0; (!(PL041_READ(p_status) & AACI_SR_RXFE)) && (i < count); i++)
    {
        buff[i] = (unsigned short)PL041_READ(p_data);
    }
    return i;
}

int aaci_pl041_channel_write(int channel, unsigned short *buff, int count)
{
    void *p_data, *p_status;
    int i = 0;

    //printf("aaci_pl041_channel_write %d \r\n", count);
    p_status = (void *)((unsigned int)(&PL041->sr1) + channel * 0x14);
    p_data = (void *)((unsigned int)(&(PL041->dr1[0])) + channel * 0x20);
    for (i = 0; (!(PL041_READ(p_status) & AACI_SR_TXFF)) && (i < count); i++)
    {
        PL041_WRITE(p_data, buff[i]);
    }
    return i;
}

int aaci_pl041_channel_cfg(int channel, pl041_cfg_t cgf)
{
    unsigned int v;
    void *p_rx, *p_tx;

    printf("aaci_pl041_channel_cfg itype:0x%x, otype:0x%x, vol: 0x%x, rate:0x%x \r\n", cgf->itype, cgf->otype, cgf->vol, cgf->rate);
    p_rx = (void *)((unsigned int)(&PL041->rxcr1) + channel * 0x14);
    p_tx = (void *)((unsigned int)(&PL041->txcr1) + channel * 0x14);
    v = AACI_CR_FEN | AACI_CR_SZ16 | cgf->itype;
    PL041_WRITE(p_rx, v);
    v = AACI_CR_FEN | AACI_CR_SZ16 | cgf->otype;
    PL041_WRITE(p_tx, v);

    ac97_set_vol(cgf->vol);
    ac97_set_rate(cgf->rate);

    return 0;
}

void aaci_pl041_irq_enable(int channel, unsigned int vector)
{
    unsigned int v;
    void *p_irq;

    printf("aaci_pl041_irq_enable channel: %d, vector: 0x%x \r\n", channel, vector);
    vector &= vector & 0x7f;
    p_irq = (void *)((unsigned int)(&PL041->iie1) + channel * 0x14);
    v = PL041_READ(p_irq);
    v |= vector;
    PL041_WRITE(p_irq, v);
}

void aaci_pl041_irq_disable(int channel, unsigned int vector)
{
    unsigned int v;
    void *p_irq;

    printf("aaci_pl041_irq_disable channel: %d, vector: 0x%x \r\n", channel, vector);
    vector &= vector & 0x7f;
    p_irq = (void *)((unsigned int)(&PL041->iie1) + channel * 0x14);
    v = PL041_READ(p_irq);
    v &= ~vector;
    PL041_WRITE(p_irq, v);
}

int aaci_pl041_irq_register(int channel, pl041_irq_fun_t fun, void *user_data)
{
    if (channel < 0 || channel >= PL041_CHANNEL_NUM)
    {
        printf("%s channel:%d err.\r\n", __FUNCTION__, channel);
        return -1;
    }
    irq_tbl[channel].fun = fun;
    irq_tbl[channel].user_data = user_data;
    printf("aaci_pl041_irq_register channel: %d \r\n", channel);
    return 0;
}

int aaci_pl041_irq_unregister(int channel)
{
    if (channel < 0 || channel >= PL041_CHANNEL_NUM)
    {
        printf("%s channel:%d err.\r\n", __FUNCTION__, channel);
        return -1;
    }
    irq_tbl[channel].fun = NULL;
    irq_tbl[channel].user_data = NULL;
    printf("aaci_pl041_irq_unregister channel: %d \r\n", channel);
    return 0;
}

static void aaci_pl041_irq_handle(int irqno, void *param)
{
    unsigned int mask, channel, m;
    struct pl041_irq_def *_irq = param;
    void *p_status;

    //printf("aaci_pl041_irq_handle irqno: %d \r\n", irqno);
    mask = PL041_READ(&PL041->allints);
    PL041_WRITE(&PL041->intclr, mask);

    for (channel = 0; (channel < PL041_CHANNEL_NUM) && (mask); channel++)
    {
        mask = mask >> 7;
        m = mask & 0x7f;
        if (m & AACI_ISR_ORINTR)
        {
            printf("RX overrun on chan %d \r\n", channel);
        }

        if (m & AACI_ISR_RXTOINTR)
        {
            printf("RX timeout on chan %d \r\n", channel);
        }

        if (mask & AACI_ISR_URINTR)
        {
            printf("TX underrun on chan %d \r\n", channel);
        }

        p_status = (void *)((unsigned int)(&PL041->sr1) + channel * 0x14);
        if (_irq[channel].fun != NULL)
        {
            _irq[channel].fun(PL041_READ(p_status), _irq[channel].user_data);
        }
    }
}

static void aaci_pl041_isr(void)
{
    aaci_pl041_irq_handle(0, &irq_tbl);
}

int aaci_pl041_init(void)
{
    unsigned int i, maincr;

    maincr  = AACI_MAINCR_SCRA(0) | AACI_MAINCR_IE | AACI_MAINCR_SL1RXEN | \
              AACI_MAINCR_SL1TXEN | AACI_MAINCR_SL2RXEN | AACI_MAINCR_SL2TXEN;

    printf("aaci_pl041_init \r\n");
    for (i = 0; i < 4; i++)
    {
        void *base = (void *)((unsigned int)(&PL041->rxcr1) + i * 0x14);

        PL041_WRITE(base + AACI_IE, 0);
        PL041_WRITE(base + AACI_TXCR, 0);
        PL041_WRITE(base + AACI_RXCR, 0);
    }

    PL041_WRITE(&PL041->intclr, 0x1fff);
    PL041_WRITE(&PL041->maincr, maincr);

    PL041_WRITE(&PL041->reset, 0);
    aaci_pl041_delay(2);
    PL041_WRITE(&PL041->reset, RESET_NRST);

    //rt_hw_interrupt_install(43, aaci_pl041_irq_handle, &irq_tbl, "aaci_pl041");
    //rt_hw_interrupt_umask(43);
    irq_register_isr(PL041_INTERRUPT_ID, aaci_pl041_isr);
    gic_enable_interrupt(PL041_INTERRUPT_ID);

    return 0;
}

#if 0
#define PL041_DUMP(_v)    rt_kprintf("%32s:addr:0x%08x data:0x%08x\n", #_v, &(_v), (_v))
int _aaci_pl041_reg_dump(int argc, char **argv)
{
    PL041_DUMP(PL041->rxcr1);
    PL041_DUMP(PL041->txcr1);
    PL041_DUMP(PL041->sr1);
    PL041_DUMP(PL041->isr1);
    PL041_DUMP(PL041->iie1);
    PL041_DUMP(PL041->rxcr2);
    PL041_DUMP(PL041->txcr2);
    PL041_DUMP(PL041->sr2);
    PL041_DUMP(PL041->isr2);
    PL041_DUMP(PL041->iie2);
    PL041_DUMP(PL041->rxcr3);
    PL041_DUMP(PL041->txcr3);
    PL041_DUMP(PL041->sr3);
    PL041_DUMP(PL041->isr3);
    PL041_DUMP(PL041->iie3);
    PL041_DUMP(PL041->rxcr4);
    PL041_DUMP(PL041->txcr4);
    PL041_DUMP(PL041->sr4);
    PL041_DUMP(PL041->isr4);
    PL041_DUMP(PL041->iie4);
    PL041_DUMP(PL041->sl1rx);
    PL041_DUMP(PL041->sl1tx);
    PL041_DUMP(PL041->sl2rx);
    PL041_DUMP(PL041->sl2tx);
    PL041_DUMP(PL041->sl12rx);
    PL041_DUMP(PL041->sl12tx);
    PL041_DUMP(PL041->slfr);
    PL041_DUMP(PL041->slistat);
    PL041_DUMP(PL041->slien);
    PL041_DUMP(PL041->intclr);
    PL041_DUMP(PL041->maincr);
    PL041_DUMP(PL041->reset);
    PL041_DUMP(PL041->sync);
    PL041_DUMP(PL041->allints);
    PL041_DUMP(PL041->mainfr);
    PL041_DUMP(PL041->dr1[0]);
    PL041_DUMP(PL041->dr2[0]);
    PL041_DUMP(PL041->dr3[0]);
    PL041_DUMP(PL041->dr4[0]);
    return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(_aaci_pl041_reg_dump, __cmd_pl041_dump, aaci pl041 dump reg.);
#endif
