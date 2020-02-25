/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdlib.h>
#include <aos/kernel.h>
#include "aos/init.h"
#include "aos/hal/dac.h"
#include "board.h"
#include <k_api.h>

#if (AOS_HAL_DAC_ENABLED > 0)

/**
 * Convert Digital to Analog and output it to DAC channel Pin
 */
void hal_dac_app_out(void)
{
    uint32_t  val;
    dac_dev_t dac_dev;
    int       ret;
    int       cnt;

    printf("hal_dac_out start\r\n");

    dac_dev.port = PORT_DAC_DEMO;
    hal_dac_init(&dac_dev);

    val = 0;
    cnt = 500;
    while (cnt-- > 0) {
        hal_dac_start(&dac_dev, 0);
        hal_dac_set_value(&dac_dev, 0, val);

        val += 50;
        if (val >= 0x1000) {
            val = 0;
        }

        aos_msleep(50);
    }

    printf("hal_dac_out end\r\n");
}

#endif
