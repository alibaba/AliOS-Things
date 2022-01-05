/*
 *  * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdlib.h>
#include "aos/debug.h"
#include <ck_usart.h>

/* use for printk */
int alios_debug_print(const char *buf, int size)
{
    uint32_t timecount = 0;
    uint32_t trans_num = 0;
    uint8_t *ch        = (uint8_t *)buf;

    ck_usart_reg_t *addr = (ck_usart_reg_t *)0x10015000;

    while (trans_num < (int32_t)size) {
        while ((!(addr->LSR & DW_LSR_TRANS_EMPTY)))
            ;
        trans_num++;
        addr->THR = *ch++;
    }

    return trans_num;
}

