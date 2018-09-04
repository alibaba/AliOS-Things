/*
 * Copyright (c) 2011-2012, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "obds.h"
#include "board_id/board_id.h"

#define FAIL     1

extern unsigned int sdio_init(int bus_width, int base_address);
extern unsigned int sdio_read_cio(int base_address);

static void wifi_power_on(void)
{
#if !defined(BOARD_MX6SL)
    /* Power Enable */
    writel(ALT1, IOMUXC_SW_MUX_CTL_PAD_EIM_DA10);
    writel(0x80, IOMUXC_SW_PAD_CTL_PAD_EIM_DA10);
    gpio_set_direction(GPIO_PORT3, 10, GPIO_GDIR_OUTPUT);
    gpio_set_level(GPIO_PORT3, 10, GPIO_HIGH_LEVEL);
    hal_delay_us(1000 * 1000);

    /* Enable DCDC1V8 Switch */
    writel(ALT1, IOMUXC_SW_MUX_CTL_PAD_EIM_DA1);
    writel(0x80, IOMUXC_SW_PAD_CTL_PAD_EIM_DA1);
    gpio_set_direction(GPIO_PORT3, 1, GPIO_GDIR_OUTPUT);
    gpio_set_level(GPIO_PORT3, 1, GPIO_HIGH_LEVEL);
    hal_delay_us(1000 * 1000);

    /* Reset Module */
    writel(ALT1, IOMUXC_SW_MUX_CTL_PAD_EIM_DA5);
    writel(0x80, IOMUXC_SW_PAD_CTL_PAD_EIM_DA5);
    gpio_set_direction(GPIO_PORT3, 5, GPIO_GDIR_OUTPUT);
    gpio_set_level(GPIO_PORT3, 5, GPIO_HIGH_LEVEL);
    hal_delay_us(100 * 1000);
    gpio_set_level(GPIO_PORT3, 5, GPIO_LOW_LEVEL);
    hal_delay_us(500 * 1000);
    gpio_set_level(GPIO_PORT3, 5, GPIO_HIGH_LEVEL);
    hal_delay_us(100 * 1000);
#endif
}

int sdio_test(unsigned int bus_width, unsigned int base_address)
{
    unsigned int status;

    wifi_power_on();

    status = sdio_init(bus_width, base_address);
    if (status == FAIL) {
        printf("Wifi Module not detected.\n");
        return TEST_FAILED;
    }

    status = sdio_read_cio(base_address);
    if (status == FAIL) {
        printf("Read VSR Failed.\n");
        return TEST_FAILED;
    }

    return TEST_PASSED;
}

int sdio_test_enable = 0;
uint32_t sdio_bus_width, sdio_base_address;
static int sdio_card_test(void)
{
    if (!sdio_test_enable) {
        return TEST_NOT_PRESENT;
    }

    PROMPT_RUN_TEST("SDIO", NULL);

    return sdio_test(sdio_bus_width, sdio_base_address);
}

//RUN_TEST_INTERACTIVE("MMC/SD", mmcsd_test)
//    RUN_TEST("WIFI", wifi_pcie_test)
