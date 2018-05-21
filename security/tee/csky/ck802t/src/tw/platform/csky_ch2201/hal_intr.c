/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 */

#include "hal_memmap.h"
#include "tee_types.h"

/* This type must be align with exception.S */
typedef struct _intr_frame_t {
    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r4;
    uint32_t r5;
    uint32_t r6;
    uint32_t r7;
    uint32_t r8;
    uint32_t r9;
    uint32_t r10;
    uint32_t r11;
    uint32_t r12;
    uint32_t r13;
    uint32_t r14;
    uint32_t r15;
    uint32_t ns_epc;
    uint32_t ns_epsr;
    uint32_t se_epc;
    uint32_t se_epsr;
    uint32_t psr;
} intr_frame_t;

/* define the registers structure of the interrupt controller */
typedef struct _intr_ctlr_t
{
    uint32_t    rev0[64];
    uint32_t    iser;
    uint32_t    rev1[15];
    uint32_t    iwer;
    uint32_t    rev2[15];
    uint32_t    icer;
    uint32_t    rev3[15];
    uint32_t    iwdr;
    uint32_t    rev4[15];
    uint32_t    ispr;
    uint32_t    rev5[31];
    uint32_t    icpr;
    uint32_t    rev6[95];
    uint32_t    ipr[8];
} intr_ctlr_t;

void hal_interrupt_init(void)
{
    volatile intr_ctlr_t *icrp = ((intr_ctlr_t *)PLATFORM_INTC_BASEADDRESS);
    /* initialize PR0-PR31, big endian */
    icrp->ipr[0] = 0x00000000;
    icrp->ipr[1] = 0x00000000;
    icrp->ipr[2] = 0x00000000;
    icrp->ipr[3] = 0x00000000;
    icrp->ipr[4] = 0x00000000;
    icrp->ipr[5] = 0x00000000;
    icrp->ipr[6] = 0x00000000;
    icrp->ipr[7] = 0x00000000;

    __asm__ volatile("psrclr sie");
}

void hal_cpu_enter_critical(uint32_t *psr)
{
    __asm__ volatile ("mfcr    %0, psr\n\r"
                      "psrclr  ie, fe, sie"
                      : "=r" (*psr) );
}

void hal_cpu_exit_critical(uint32_t psr)
{
    __asm__ volatile ("mtcr   %0, psr"
                    :
                    :"r"(psr));
}

extern void hal_uart_putchar(uint8_t ch);
static void _hex_to_char(unsigned char data, char *h, char *l)
{
    unsigned char tmp = data;
    char h_tmp, l_tmp;

    tmp &= 0xff;
    l_tmp = (((tmp & 0xf) > 9) ? ('a' + ((tmp & 0xf) - 10) ) :
                                ( '0' + (tmp & 0xf) ) );
    h_tmp = ((((tmp >> 4) & 0xf) > 9) ? ('a' + (((tmp >> 4) & 0xf) - 10) ) :
                                ( '0' + ((tmp >> 4) & 0xf) ) );
    *h = h_tmp;
    *l = l_tmp;
    return;
}
static void _dump_uint32(unsigned int data)
{
    char l, h;
    unsigned char tmp;

    hal_uart_putchar('0');
    hal_uart_putchar('x');
    tmp = ((data >> 24) & 0xff);
    _hex_to_char(tmp, &h, &l);
    hal_uart_putchar(h);
    hal_uart_putchar(l);
    tmp = ((data >> 16) & 0xff);
    _hex_to_char(tmp, &h, &l);
    hal_uart_putchar(h);
    hal_uart_putchar(l);
    tmp = ((data >> 8) & 0xff);
    _hex_to_char(tmp, &h, &l);
    hal_uart_putchar(h);
    hal_uart_putchar(l);
    tmp = ((data >> 0) & 0xff);
    _hex_to_char(tmp, &h, &l);
    hal_uart_putchar(h);
    hal_uart_putchar(l);
}
static void _dump_string(char *data)
{
    unsigned int i = 0;
    while(data[i]) {
        if (data[i] == '\n')
            hal_uart_putchar('\r');
        hal_uart_putchar(data[i++]);
    }
}

void hal_sys_exception_handler(uint32_t vector, intr_frame_t *p)
{
    _dump_string("VID:");
    _dump_uint32(vector);
    _dump_string("\n");

    _dump_string(" r0:");
    _dump_uint32(p->r0);
    _dump_string(" r1:");
    _dump_uint32(p->r1);
    _dump_string(" r2:");
    _dump_uint32(p->r2);
    _dump_string(" r3:");
    _dump_uint32(p->r3);
    _dump_string("\n");

    _dump_string(" r4:");
    _dump_uint32(p->r4);
    _dump_string(" r5:");
    _dump_uint32(p->r5);
    _dump_string(" r6:");
    _dump_uint32(p->r6);
    _dump_string(" r7:");
    _dump_uint32(p->r7);
    _dump_string("\n");

    _dump_string(" r8:");
    _dump_uint32(p->r8);
    _dump_string(" r9:");
    _dump_uint32(p->r9);
    _dump_string(" r10:");
    _dump_uint32(p->r10);
    _dump_string(" r11:");
    _dump_uint32(p->r11);
    _dump_string("\n");

    _dump_string(" r12:");
    _dump_uint32(p->r12);
    _dump_string(" r13:");
    _dump_uint32(p->r13);
    _dump_string(" r14:");
    _dump_uint32(p->r14);
    _dump_string(" r15:");
    _dump_uint32(p->r15);
    _dump_string("\n");

    _dump_string(" NS_EPC:");
    _dump_uint32(p->ns_epc);
    _dump_string(" NS_EPSR:");
    _dump_uint32(p->ns_epsr);
    _dump_string("\n");

    _dump_string(" SE_EPC:");
    _dump_uint32(p->se_epc);
    _dump_string(" SE_EPSR:");
    _dump_uint32(p->se_epsr);
    _dump_string("\n");

    _dump_string(" PSR:");
    _dump_uint32(p->psr);
    _dump_string("\n");
    while(1);
}
