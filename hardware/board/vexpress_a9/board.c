/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <board.h>
#include <aos/init.h>

extern void chip_tick_init(void);
extern void chip_stduart_init(void);

void board_tick_init(void)
{
    chip_tick_init();
}

void board_stduart_init(void)
{
    chip_stduart_init();
}

void board_dma_init(void)
{
}

void board_gpio_init(void)
{
}

void board_kinit_init(kinit_t *kinit)
{
    kinit->argc = 0;
    kinit->argv = NULL;
    kinit->cli_enable = 1;
}
