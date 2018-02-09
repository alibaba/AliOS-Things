/**
 * File: clk_si.c
 * Brief: bsp APIs of Sanechips clk
 *
 * Copyright (C) 2017 Sanechips Technology Co., Ltd.
 * Author:
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

/*******************************************************************************
 *                           Include header files                              *
 ******************************************************************************/
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <addrspace.h>
#include <soc.h>
#include <csi_core.h>
#include <drv_clk.h>
#include <errno.h>
#include <io.h>
#include "zx29_clk.h"

/*******************************************************************************
 *                             Macro definitions                               *
 ******************************************************************************/
#define INVLID_ADDR     (0xffffffff)
#define INVLID_VALUE    (0xffff)

/*******************************************************************************
 *                             Type definitions                                *
 ******************************************************************************/

/*******************************************************************************
 *                         Local variable definitions                          *
 ******************************************************************************/
static const struct clk_si_info ck_timer0_pclk = {
    .name       = "ck_timer0_pclk",
    .clken      = {AP_CRPM_BASEADDR + 0x50, 0, 1},
    .auto_gate  = {AP_CRPM_BASEADDR + 0x50, 1, 1},
    .clk_sel    = {INVLID_ADDR, INVLID_VALUE, INVLID_VALUE},
    .clk_div    = {INVLID_ADDR, INVLID_VALUE, INVLID_VALUE},
    .reset      = {AP_CRPM_BASEADDR + 0x54, 0, 1},
};

static const struct clk_si_info ck_timer1_pclk = {
    .name       = "ck_timer1_pclk",
    .clken      = {AP_CRPM_BASEADDR + 0x50, 4, 1},
    .auto_gate  = {AP_CRPM_BASEADDR + 0x50, 5, 1},
    .clk_sel    = {INVLID_ADDR, INVLID_VALUE, INVLID_VALUE},
    .clk_div    = {INVLID_ADDR, INVLID_VALUE, INVLID_VALUE},
    .reset      = {AP_CRPM_BASEADDR + 0x54, 2, 1},
};

static const struct clk_si_info ck_timer0_wclk = {
    .name       = "ck_timer0_wclk",
    .clken      = {AP_CRPM_BASEADDR + 0x50, 2, 1},
    .auto_gate  = {INVLID_ADDR, INVLID_VALUE, INVLID_VALUE},
    .clk_sel    = {AP_CRPM_BASEADDR + 0x48, 0, 1},
    .clk_div    = {AP_CRPM_BASEADDR + 0x4c, 0, 4},
    .reset      = {AP_CRPM_BASEADDR + 0X54, 1, 1},
};

static const struct clk_si_info ck_timer1_wclk = {
    .name       = "ck_timer1_wclk",
    .clken      = {AP_CRPM_BASEADDR + 0x50, 6, 1},
    .auto_gate  = {INVLID_ADDR, INVLID_VALUE, INVLID_VALUE},
    .clk_sel    = {AP_CRPM_BASEADDR + 0x48, 3, 1},
    .clk_div    = {AP_CRPM_BASEADDR + 0x4c, 4, 4},
    .reset      = {AP_CRPM_BASEADDR + 0X54, 3, 1},
};

static const struct clk_si_info ap_wdt_pclk = {
    .name       = "ap_wdt_pclk",
    .clken      = {AP_CRPM_BASEADDR + 0x70, 0, 1},
    .auto_gate  = {AP_CRPM_BASEADDR + 0x70, 1, 1},
    .clk_sel    = {INVLID_ADDR, INVLID_VALUE, INVLID_VALUE},
    .clk_div    = {INVLID_ADDR, INVLID_VALUE, INVLID_VALUE},
    .reset      = {AP_CRPM_BASEADDR + 0x74, 0, 1},
};

static const struct clk_si_info ap_wdt_wclk = {
    .name       = "ap_wdt_wclk",
    .clken      = {AP_CRPM_BASEADDR + 0x70, 2, 1},
    .auto_gate  = {INVLID_ADDR, INVLID_VALUE, INVLID_VALUE},
    .clk_sel    = {AP_CRPM_BASEADDR + 0x68, 0, 1},
    .clk_div    = {AP_CRPM_BASEADDR + 0x6c, 0, 4},
    .reset      = {AP_CRPM_BASEADDR + 0x74, 1, 1},
};

static const struct clk_si_info uart0_pclk = {
    .name       = "uart0_pclk",
    .clken      = {LSP_CRPM_BASEADDR + 0x4, 4, 1},
    .auto_gate  = {LSP_CRPM_BASEADDR + 0x4, 8, 1},
    .clk_sel    = {INVLID_ADDR, INVLID_VALUE, INVLID_VALUE},
    .clk_div    = {INVLID_ADDR, INVLID_VALUE, INVLID_VALUE},
    .reset      = {LSP_CRPM_BASEADDR + 0x4, 0, 1},
};

static const struct clk_si_info uart1_pclk = {
    .name       = "uart1_pclk",
    .clken      = {LSP_CRPM_BASEADDR + 0x10, 4, 1},
    .auto_gate  = {LSP_CRPM_BASEADDR + 0x10, 8, 1},
    .clk_sel    = {INVLID_ADDR, INVLID_VALUE, INVLID_VALUE},
    .clk_div    = {INVLID_ADDR, INVLID_VALUE, INVLID_VALUE},
    .reset      = {LSP_CRPM_BASEADDR + 0x10, 0, 1},
};

static const struct clk_si_info uart2_pclk = {
    .name       = "uart2_pclk",
    .clken      = {LSP_CRPM_BASEADDR + 0x4c, 4, 1},
    .auto_gate  = {LSP_CRPM_BASEADDR + 0x4c, 8, 1},
    .clk_sel    = {INVLID_ADDR, INVLID_VALUE, INVLID_VALUE},
    .clk_div    = {INVLID_ADDR, INVLID_VALUE, INVLID_VALUE},
    .reset      = {LSP_CRPM_BASEADDR + 0x4c, 0, 1},
};

static const struct clk_si_info lp_uart_pclk = {
    .name       = "lp_uart_pclk",
    .clken      = {A0_CRPM_BASEADDR + 0x10, 4, 1},
    .auto_gate  = {A0_CRPM_BASEADDR + 0x10, 5, 1},
    .clk_sel    = {INVLID_ADDR, INVLID_VALUE, INVLID_VALUE},
    .clk_div    = {INVLID_ADDR, INVLID_VALUE, INVLID_VALUE},
    .reset      = {A0_CRPM_BASEADDR + 0x14, 2, 1},
};

static const struct clk_si_info uart0_wclk_l1 = {
    .name       = "uart0_wclk_l1",
    .clken      = {LSP_CRPM_BASEADDR + 0x4, 5, 1},
    .auto_gate  = {LSP_CRPM_BASEADDR + 0x4, 9, 1},
    .clk_sel    = {LSP_CRPM_BASEADDR + 0x4, 12, 2},
    .clk_div    = {INVLID_ADDR, INVLID_VALUE, INVLID_VALUE},
    .reset      = {LSP_CRPM_BASEADDR + 0x4, 1, 1},
};

static const struct clk_si_info uart0_wclk_l2 = {
    .name       = "uart0_wclk_l2",
    .clken      = {LSP_CRPM_BASEADDR + 0x4, 6, 1},
    .auto_gate  = {INVLID_ADDR, INVLID_VALUE, INVLID_VALUE},
    .clk_sel    = {INVLID_ADDR, INVLID_VALUE, INVLID_VALUE},
    .clk_div    = {INVLID_ADDR, INVLID_VALUE, INVLID_VALUE},
    .reset      = {INVLID_ADDR, INVLID_VALUE, INVLID_VALUE},
};

static const struct clk_si_info uart1_wclk = {
    .name       = "uart1_wclk",
    .clken      = {LSP_CRPM_BASEADDR + 0x10, 5, 1},
    .auto_gate  = {LSP_CRPM_BASEADDR + 0x10, 9, 1},
    .clk_sel    = {LSP_CRPM_BASEADDR + 0x10, 12, 1},
    .clk_div    = {INVLID_ADDR, INVLID_VALUE, INVLID_VALUE},
    .reset      = {LSP_CRPM_BASEADDR + 0x10, 1, 1},
};

static const struct clk_si_info uart2_wclk = {
    .name       = "uart2_wclk",
    .clken      = {LSP_CRPM_BASEADDR + 0x4c, 5, 1},
    .auto_gate  = {LSP_CRPM_BASEADDR + 0x4c, 9, 1},
    .clk_sel    = {LSP_CRPM_BASEADDR + 0x4c, 12, 1},
    .clk_div    = {INVLID_ADDR, INVLID_VALUE, INVLID_VALUE},
    .reset      = {LSP_CRPM_BASEADDR + 0x4c, 1, 1},
};

static const struct clk_si_info lp_uart_wclk = {
    .name       = "lp_uart_wclk",
    .clken      = {A0_CRPM_BASEADDR + 0x10, 6, 1},
    .auto_gate  = {A0_CRPM_BASEADDR + 0x10, 7, 1},
    .clk_sel    = {A0_CRPM_BASEADDR + 0x8, 4, 2},
    .clk_div    = {INVLID_ADDR, INVLID_VALUE, INVLID_VALUE},
    .reset      = {A0_CRPM_BASEADDR + 0x14, 3, 1},
};

static const struct clk_si_info i2c0_pclk = {
    .name       = "i2c0_pclk",
    .clken      = {LSP_CRPM_BASEADDR + 0x1c, 4, 1},
    .auto_gate  = {LSP_CRPM_BASEADDR + 0x1c, 8, 1},
    .clk_sel    = {INVLID_ADDR, INVLID_VALUE, INVLID_VALUE},
    .clk_div    = {INVLID_ADDR, INVLID_VALUE, INVLID_VALUE},
    .reset      = {LSP_CRPM_BASEADDR + 0x1c, 0, 1},
};

static const struct clk_si_info i2c1_pclk = {
    .name       = "i2c0_pclk",
    .clken      = {LSP_CRPM_BASEADDR + 0x38, 4, 1},
    .auto_gate  = {LSP_CRPM_BASEADDR + 0x38, 8, 1},
    .clk_sel    = {INVLID_ADDR, INVLID_VALUE, INVLID_VALUE},
    .clk_div    = {INVLID_ADDR, INVLID_VALUE, INVLID_VALUE},
    .reset      = {LSP_CRPM_BASEADDR + 0x38, 0, 1},
};

static const struct clk_si_info i2c0_wclk = {
    .name       = "i2c0_wclk",
    .clken      = {LSP_CRPM_BASEADDR + 0x1c, 5, 1},
    .auto_gate  = {LSP_CRPM_BASEADDR + 0x1c, 9, 1},
    .clk_sel    = {LSP_CRPM_BASEADDR + 0x1c, 12, 1},
    .clk_div    = {INVLID_ADDR, INVLID_VALUE, INVLID_VALUE},
    .reset      = {LSP_CRPM_BASEADDR + 0x1c, 1, 1},
};

static const struct clk_si_info i2c1_wclk = {
    .name       = "i2c0_wclk",
    .clken      = {LSP_CRPM_BASEADDR + 0x38, 5, 1},
    .auto_gate  = {LSP_CRPM_BASEADDR + 0x38, 9, 1},
    .clk_sel    = {LSP_CRPM_BASEADDR + 0x38, 12, 1},
    .clk_div    = {INVLID_ADDR, INVLID_VALUE, INVLID_VALUE},
    .reset      = {LSP_CRPM_BASEADDR + 0x38, 1, 1},
};


static const struct clk_si_info ssp0_pclk = {
    .name       = "ssp0_pclk",
    .clken      = {LSP_CRPM_BASEADDR + 0x14, 4, 1},
    .auto_gate  = {LSP_CRPM_BASEADDR + 0x14, 8, 1},
    .clk_sel    = {INVLID_ADDR, INVLID_VALUE, INVLID_VALUE},
    .clk_div    = {INVLID_ADDR, INVLID_VALUE, INVLID_VALUE},
    .reset      = {LSP_CRPM_BASEADDR + 0x14, 0, 1},
};

static const struct clk_si_info ssp1_pclk = {
    .name       = "ssp1_pclk",
    .clken      = {LSP_CRPM_BASEADDR + 0x18, 4, 1},
    .auto_gate  = {LSP_CRPM_BASEADDR + 0x18, 8, 1},
    .clk_sel    = {INVLID_ADDR, INVLID_VALUE, INVLID_VALUE},
    .clk_div    = {INVLID_ADDR, INVLID_VALUE, INVLID_VALUE},
    .reset      = {LSP_CRPM_BASEADDR + 0x18, 0, 1},
};

static const struct clk_si_info ssp2_pclk = {
    .name       = "ssp2_pclk",
    .clken      = {LSP_CRPM_BASEADDR + 0x48, 4, 1},
    .auto_gate  = {LSP_CRPM_BASEADDR + 0x48, 8, 1},
    .clk_sel    = {INVLID_ADDR, INVLID_VALUE, INVLID_VALUE},
    .clk_div    = {INVLID_ADDR, INVLID_VALUE, INVLID_VALUE},
    .reset      = {LSP_CRPM_BASEADDR + 0x48, 0, 1},
};

static const struct clk_si_info ssp0_wclk = {
    .name       = "ssp0_wclk",
    .clken      = {LSP_CRPM_BASEADDR + 0x14, 5, 1},
    .auto_gate  = {LSP_CRPM_BASEADDR + 0x14, 9, 1},
    .clk_sel    = {LSP_CRPM_BASEADDR + 0x14, 12, 2},
    .clk_div    = {LSP_CRPM_BASEADDR + 0x14, 16, 4},
    .reset      = {LSP_CRPM_BASEADDR + 0x14, 1, 1},
};

static const struct clk_si_info ssp1_wclk = {
    .name       = "ssp1_wclk",
    .clken      = {LSP_CRPM_BASEADDR + 0x18, 5, 1},
    .auto_gate  = {LSP_CRPM_BASEADDR + 0x18, 9, 1},
    .clk_sel    = {LSP_CRPM_BASEADDR + 0x18, 12, 2},
    .clk_div    = {LSP_CRPM_BASEADDR + 0x18, 16, 4},
    .reset      = {LSP_CRPM_BASEADDR + 0x18, 1, 1},
};

static const struct clk_si_info ssp2_wclk = {
    .name       = "ssp2_wclk",
    .clken      = {LSP_CRPM_BASEADDR + 0x48, 5, 1},
    .auto_gate  = {LSP_CRPM_BASEADDR + 0x48, 9, 1},
    .clk_sel    = {LSP_CRPM_BASEADDR + 0x48, 12, 2},
    .clk_div    = {LSP_CRPM_BASEADDR + 0x48, 16, 4},
    .reset      = {LSP_CRPM_BASEADDR + 0x48, 1, 1},
};

static const struct clk_si_info gpio_pd_pclk = {
    .name       = "gpio_pd_pclk",
    .clken      = {LSP_CRPM_BASEADDR + 0x30, 4, 1},
    .auto_gate  = {LSP_CRPM_BASEADDR + 0x30, 8, 1},
    .clk_sel    = {INVLID_ADDR, INVLID_VALUE, INVLID_VALUE},
    .clk_div    = {INVLID_ADDR, INVLID_VALUE, INVLID_VALUE},
    .reset      = {LSP_CRPM_BASEADDR + 0x30, 0, 1},
};

static const struct clk_si_info adc_pclk = {
    .name       = "adc_pclk",
    .clken      = {LSP_CRPM_BASEADDR + 0x34, 0, 1},
    .auto_gate  = {LSP_CRPM_BASEADDR + 0x34, 11, 1},
    .clk_sel    = {INVLID_ADDR, INVLID_VALUE, INVLID_VALUE},
    .clk_div    = {INVLID_ADDR, INVLID_VALUE, INVLID_VALUE},
    .reset      = {LSP_CRPM_BASEADDR + 0x34, 8, 1},
};

static const struct clk_si_info adc_wclk = {
    .name       = "adc_wclk",
    .clken      = {LSP_CRPM_BASEADDR + 0x34, 1, 1},
    .auto_gate  = {LSP_CRPM_BASEADDR + 0x34, 10, 1},
    .clk_sel    = {LSP_CRPM_BASEADDR + 0x34, 4, 2},
    .clk_div    = {LSP_CRPM_BASEADDR + 0x34, 12, 6},
    .reset      = {LSP_CRPM_BASEADDR + 0x34, 9, 1},
};

static const struct clk_si_info dac_pclk = {
    .name       = "dac_pclk",
    .clken      = {LSP_CRPM_BASEADDR + 0x2c, 0, 1},
    .auto_gate  = {INVLID_ADDR, INVLID_VALUE, INVLID_VALUE},
    .clk_sel    = {INVLID_ADDR, INVLID_VALUE, INVLID_VALUE},
    .clk_div    = {INVLID_ADDR, INVLID_VALUE, INVLID_VALUE},
    .reset      = {LSP_CRPM_BASEADDR + 0x2c, 8, 1},
};

static const struct clk_si_info dac_wclk = {
    .name       = "dac_wclk",
    .clken      = {LSP_CRPM_BASEADDR + 0x2c, 1, 1},
    .auto_gate  = {INVLID_ADDR, INVLID_VALUE, INVLID_VALUE},
    .clk_sel    = {LSP_CRPM_BASEADDR + 0x2c, 4, 1},
    .clk_div    = {LSP_CRPM_BASEADDR + 0x2c, 12, 6},
    .reset      = {LSP_CRPM_BASEADDR + 0x2c, 9, 1},
};

static const struct clk_si_info ad_timer0_pclk = {
    .name       = "ad_timer0_pclk",
    .clken      = {LSP_CRPM_BASEADDR + 0x3c, 0, 1},
    .auto_gate  = {LSP_CRPM_BASEADDR + 0x3c, 11, 1},
    .clk_sel    = {INVLID_ADDR, INVLID_VALUE, INVLID_VALUE},
    .clk_div    = {INVLID_ADDR, INVLID_VALUE, INVLID_VALUE},
    .reset      = {LSP_CRPM_BASEADDR + 0x3c, 8, 1},
};

static const struct clk_si_info ad_timer1_pclk = {
    .name       = "ad_timer1_pclk",
    .clken      = {LSP_CRPM_BASEADDR + 0x40, 0, 1},
    .auto_gate  = {LSP_CRPM_BASEADDR + 0x40, 11, 1},
    .clk_sel    = {INVLID_ADDR, INVLID_VALUE, INVLID_VALUE},
    .clk_div    = {INVLID_ADDR, INVLID_VALUE, INVLID_VALUE},
    .reset      = {LSP_CRPM_BASEADDR + 0x40, 8, 1},
};

static const struct clk_si_info ad_timer0_wclk = {
    .name       = "ad_timer0_wclk",
    .clken      = {LSP_CRPM_BASEADDR + 0x3c, 1, 1},
    .auto_gate  = {INVLID_ADDR, INVLID_VALUE, INVLID_VALUE},
    .clk_sel    = {LSP_CRPM_BASEADDR + 0x3c, 4, 2},
    .clk_div    = {LSP_CRPM_BASEADDR + 0x3c, 12, 5},
    .reset      = {LSP_CRPM_BASEADDR + 0x3c, 9, 1},
};

static const struct clk_si_info ad_timer1_wclk = {
    .name       = "ad_timer1_wclk",
    .clken      = {LSP_CRPM_BASEADDR + 0x40, 1, 1},
    .auto_gate  = {INVLID_ADDR, INVLID_VALUE, INVLID_VALUE},
    .clk_sel    = {LSP_CRPM_BASEADDR + 0x40, 4, 2},
    .clk_div    = {LSP_CRPM_BASEADDR + 0x40, 12, 5},
    .reset      = {LSP_CRPM_BASEADDR + 0x40, 9, 1},
};

static const struct clk_si_info ap_dma_pclk = {
    .name       = "ap_dma_pclk",
    .clken      = {AP_CRPM_BASEADDR + 0x40, 0, 1},
    .auto_gate  = {AP_CRPM_BASEADDR + 0x40, 1, 1},
    .clk_sel    = {INVLID_ADDR, INVLID_VALUE, INVLID_VALUE},
    .clk_div    = {INVLID_ADDR, INVLID_VALUE, INVLID_VALUE},
    .reset      = {AP_CRPM_BASEADDR + 0x44, 0, 1},
};

static const struct clk_si_info lp_timer_pclk = {
    .name       = "lp_timer_pclk",
    .clken      = {A0_CRPM_BASEADDR + 0x10, 0, 1},
    .auto_gate  = {A0_CRPM_BASEADDR + 0x10, 1, 1},
    .clk_sel    = {INVLID_ADDR, INVLID_VALUE, INVLID_VALUE},
    .clk_div    = {INVLID_ADDR, INVLID_VALUE, INVLID_VALUE},
    .reset      = {A0_CRPM_BASEADDR + 0x14, 0, 1},
};

static const struct clk_si_info lp_timer_wclk = {
    .name       = "lp_timer_wclk",
    .clken      = {A0_CRPM_BASEADDR + 0x10, 2, 1},
    .auto_gate  = {INVLID_ADDR, INVLID_VALUE, INVLID_VALUE},
    .clk_sel    = {A0_CRPM_BASEADDR + 0x8, 0, 2},
    .clk_div    = {A0_CRPM_BASEADDR + 0xc, 0, 5},
    .reset      = {A0_CRPM_BASEADDR + 0x14, 1, 1},
};

static const struct clk_si_info *const clk_info_table[] = {
    &ck_timer0_pclk,
    &ck_timer1_pclk,
    &ck_timer0_wclk,
    &ck_timer1_wclk,
    &ap_wdt_pclk,
    &ap_wdt_wclk,
    &uart0_pclk,
    &uart1_pclk,
    &uart2_pclk,
    &lp_uart_pclk,
    &uart0_wclk_l1,
    &uart0_wclk_l2,
    &uart1_wclk,
    &uart2_wclk,
    &lp_uart_wclk,
    &i2c0_pclk,
    &i2c1_pclk,
    &i2c0_wclk,
    &i2c1_wclk,
    &ssp0_pclk,
    &ssp1_pclk,
    &ssp2_pclk,
    &ssp0_wclk,
    &ssp1_wclk,
    &ssp2_wclk,
    &gpio_pd_pclk,
    &adc_pclk,
    &adc_wclk,
    &dac_pclk,
    &dac_wclk,
    &ad_timer0_pclk,
    &ad_timer1_pclk,
    &ad_timer0_wclk,
    &ad_timer1_wclk,
    &ap_dma_pclk,
    &lp_timer_pclk,
    &lp_timer_wclk,
};

static uint32_t clk_max_num = sizeof(clk_info_table) / sizeof(clk_info_table[0]);

/*******************************************************************************
 *                                   Local function declarations                                      *
 ******************************************************************************/

/*******************************************************************************
 *                      Local function implementations                         *
 ******************************************************************************/

/*******************************************************************************
 *                      Global function implementations                        *
 ******************************************************************************/
/**
 * This function is used to set software clock gate
 * @param[in]   name    the clock name to set
 * @param[in]   ctrl        0 enbale;     1 disable
 * @return          0 success; -1 failure
 */
int csi_clk_sw_gate_ctrl(clk_name name, uint32_t ctrl)
{
    const struct clk_si_info *pclk = clk_info_table[name];

    if (pclk->clken.regbase == INVLID_ADDR) {
        return -1;
    }

    __write_bit(pclk->clken.offset, (volatile uint32_t *)pclk->clken.regbase, ctrl);

    return 0;

}

/**
 * This function is used to set clock autogate
 * @param[in]   name    the clock name to set
 * @param[in]   ctrl        0 enbale;     1 disable
 * @return          0 success; -1 failure
 */
int csi_clk_hw_gate_ctrl(clk_name name, uint32_t ctrl)
{
    const struct clk_si_info *pclk = clk_info_table[name];

    if (pclk->auto_gate.regbase == INVLID_ADDR) {
        return -1;
    }

    __write_bit(pclk->auto_gate.offset, (volatile uint32_t *)pclk->auto_gate.regbase, ctrl);

    return 0;
}

/**
 * This function is used to config clock reset ctrl
 * @param[in]   name    the clock name to set
 * @param[in]   ctrl        0 enbale;     1 disable
 * @return          0 success; -1 failure
 */
int csi_clk_reset(clk_name name, uint32_t ctrl)
{
    const struct clk_si_info *pclk = clk_info_table[name];

    if (pclk->reset.regbase == INVLID_ADDR) {
        return -1;
    }

    __write_bit(pclk->reset.offset, (volatile uint32_t *)pclk->reset.regbase, ctrl);
    return 0;

}

/**
 * This function is used to set clock freq
 * @param[in]   name    the clock name to set
 * @param[in]   freq        the corresponding freq to set
 * @return          0 success; -1 failure
 */
int csi_clk_set_freq(clk_name name, clk_freq_sel freq)
{
    const struct clk_si_info *pclk = clk_info_table[name];
    uint32_t clk_div;
    uint32_t clk_sel;
    uint32_t flag;

    uint32_t clk_nr = (freq >> 20) & 0xf;

    if ((name >= clk_max_num) || ((name - (freq >> 24)) >= clk_nr)) {
        return -1;
    }

    flag = csi_irq_save();

    if (pclk->clk_div.regbase != INVLID_ADDR) {
        /*get division value*/
        clk_div = freq & 0xff;

        __write_bits(pclk->clk_div.size, pclk->clk_div.offset, (volatile uint32_t *)pclk->clk_div.regbase, clk_div);
    }

    if (pclk->clk_sel.regbase != INVLID_ADDR) {
        /*get selection value*/
        clk_sel = (freq >> 8) & 0xff;

        __write_bits(pclk->clk_sel.size, pclk->clk_sel.offset, (volatile uint32_t *)pclk->clk_sel.regbase, clk_sel);
    }

    csi_irq_restore(flag);

    return 0;
}


