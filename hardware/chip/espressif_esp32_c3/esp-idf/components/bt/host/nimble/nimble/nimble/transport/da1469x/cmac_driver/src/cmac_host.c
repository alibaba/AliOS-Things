/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "os/os.h"
#include "mcu/cmsis_nvic.h"
#include "mcu/da1469x_lpclk.h"
#include "mcu/da1469x_hal.h"
#include "mcu/da1469x_pdc.h"
#include "mcu/mcu.h"
#include "cmac_driver/cmac_host.h"

#define CMAC_SYM_CONFIG     ((void *)(0x00818f20 + MEMCTRL->CMI_CODE_BASE_REG))
#define CMAC_SYM_CONFIG_DYN ((void *)(0x00821af8 + MEMCTRL->CMI_CODE_BASE_REG))
#define CMAC_SYM_MBOX_RX    ((void *)(0x008216b0 + MEMCTRL->CMI_CODE_BASE_REG))
#define CMAC_SYM_MBOX_TX    ((void *)(0x008218b0 + MEMCTRL->CMI_CODE_BASE_REG))

#define CMAC_MBOX_SIZE              504
#define CMAC_MBOX_F_RESET           0x0008
#define CMAC_MBOX_F_WRITEPENDING    0x0010

struct cmac_config {
    uint8_t  bdaddr[6];             /* Device address */

    uint8_t  rf_calibration_delay;

    uint8_t  lp_clock_freq;         /* Sleep clock frequency (0 = 32768Hz, 1 = 32000Hz) */
    uint16_t lp_clock_sca;          /* Sleep clock accuracy [ppm] */

    uint16_t rx_buf_len;            /* RX buffer size */
    uint16_t tx_buf_len;            /* TX buffer size */
    bool     initial_length_req;

    /* Channel assessment algorithm settings */
    uint16_t chan_assess_itvl;
    uint8_t  chan_assess_itvl_mult;
    int8_t   chan_assess_min_rssi;
    uint16_t chan_assess_pkt_num;
    uint16_t chan_assess_bad_pkt_num;

    /* Calibration settings */
    uint8_t  system_tcs_length;
    uint8_t  synth_tcs_length;
    uint8_t  rfcu_tcs_length;

    uint8_t  default_tx_power;      /* Default TX power for connection/advertising */
    bool     filter_dup_ov_discard; /* Discard unknown devices when filter buffer is full */
    bool     use_hp_1m;
    bool     use_hp_2m;
};

struct cmac_config_dynamic {
    bool     enable_sleep;          /* Enable sleep */

    /* More options here, don't care now */
};

struct cmac_mbox {
    volatile uint16_t magic;
    volatile uint16_t flags;
    volatile uint16_t wr_off;
    volatile uint16_t rd_off;
    uint8_t data[CMAC_MBOX_SIZE];
};

/* CMAC data */
extern char _binary_cmac_rom_bin_start[];
extern char _binary_cmac_rom_bin_end;
extern char _binary_cmac_ram_bin_start[];
extern char _binary_cmac_ram_bin_end;

struct cmac_image_info {
    uint32_t _dummy1;
    uint32_t size_rom;
    uint32_t _dummy2;
    uint32_t magic;
    uint32_t _dummy3[5];
};

/* Mailboxes for SYS<->CMAC communication */
static struct cmac_mbox *cmac_mbox_rx;
static struct cmac_mbox *cmac_mbox_tx;

/* PDC entry for waking up CMAC */
static int8_t g_cmac_host_pdc_sys2cmac;
/* PDC entry for waking up M33 */
static int8_t g_cmac_host_pdc_cmac2sys;
/* Callback for data ready from CMAC */
static cmac_mbox_read_cb g_cmac_mbox_read_cb;

static inline void
cmac_host_signal2cmac(void)
{
    da1469x_pdc_set(g_cmac_host_pdc_sys2cmac);
}

static void
cmac2sys_isr(void)
{
    uint16_t wr_off;
    uint16_t rd_off;
    uint16_t chunk;
    uint16_t len;

    os_trace_isr_enter();

    /* Clear CMAC2SYS interrupt */
    *(volatile uint32_t *)0x40002000 = 2;

    if (*(volatile uint32_t *)0x40002000 & 0x1c00) {
        /* XXX CMAC is in error state, need to recover */
        assert(0);
    }

    if (cmac_mbox_rx->flags & CMAC_MBOX_F_RESET) {
        cmac_mbox_rx->flags &= ~CMAC_MBOX_F_RESET;
        goto done;
    }

    if (!g_cmac_mbox_read_cb) {
        cmac_mbox_rx->rd_off = cmac_mbox_rx->wr_off;
        goto done;
    }

    do {
        rd_off = cmac_mbox_rx->rd_off;
        wr_off = cmac_mbox_rx->wr_off;

        if (rd_off <= wr_off) {
            chunk = wr_off - rd_off;
        } else {
            chunk = CMAC_MBOX_SIZE - rd_off;
        }

        while (chunk) {
            len = g_cmac_mbox_read_cb(&cmac_mbox_rx->data[rd_off], chunk);

            rd_off += len;
            chunk -= len;
        };

        cmac_mbox_rx->rd_off = rd_off % CMAC_MBOX_SIZE;
    } while (cmac_mbox_rx->rd_off != cmac_mbox_rx->wr_off);

done:

    if (cmac_mbox_rx->flags & CMAC_MBOX_F_WRITEPENDING) {
        cmac_host_signal2cmac();
    }

    os_trace_isr_exit();
}

static void
cmac_host_lpclk_cb(uint32_t freq)
{
    struct cmac_config_dynamic *cmac_config_dyn;

    cmac_config_dyn = CMAC_SYM_CONFIG_DYN;
    cmac_config_dyn->enable_sleep = freq == 32768;
}

void
cmac_mbox_write(const uint8_t *buf, size_t len)
{
    uint16_t wr_off;
    uint16_t rd_off;
    uint16_t chunk;
    uint32_t primask;

    __HAL_DISABLE_INTERRUPTS(primask);

    while (len) {
        rd_off = cmac_mbox_tx->rd_off;
        wr_off = cmac_mbox_tx->wr_off;

        if (rd_off > wr_off) {
            chunk = min(len, rd_off - wr_off);
        } else {
            chunk = min(len, CMAC_MBOX_SIZE - wr_off);
        }

        if (chunk == 0) {
            continue;
        }

        memcpy(&cmac_mbox_tx->data[wr_off], buf, chunk);

        wr_off += chunk;
        cmac_mbox_tx->wr_off = wr_off % CMAC_MBOX_SIZE;

        cmac_host_signal2cmac();

        len -= chunk;
        buf += chunk;
    }

    __HAL_ENABLE_INTERRUPTS(primask);
}

void
cmac_mbox_set_read_cb(cmac_mbox_read_cb cb)
{
    g_cmac_mbox_read_cb = cb;
}

void
cmac_host_init(void)
{
    struct cmac_image_info ii;
    uint32_t cmac_rom_size;
    uint32_t cmac_ram_size;
    struct cmac_config *cmac_config;
    struct cmac_config_dynamic *cmac_config_dyn;

    /* Add PDC entry to wake up CMAC from M33 */
    g_cmac_host_pdc_sys2cmac = da1469x_pdc_add(MCU_PDC_TRIGGER_MAC_TIMER,
                                             MCU_PDC_MASTER_CMAC,
                                             MCU_PDC_EN_XTAL);
    da1469x_pdc_set(g_cmac_host_pdc_sys2cmac);
    da1469x_pdc_ack(g_cmac_host_pdc_sys2cmac);

    /* Add PDC entry to wake up M33 from CMAC, if does not exist yet */
    g_cmac_host_pdc_cmac2sys = da1469x_pdc_find(MCU_PDC_TRIGGER_COMBO,
                                              MCU_PDC_MASTER_M33, 0);
    if (g_cmac_host_pdc_cmac2sys < 0) {
        g_cmac_host_pdc_cmac2sys = da1469x_pdc_add(MCU_PDC_TRIGGER_COMBO,
                                                 MCU_PDC_MASTER_M33,
                                                 MCU_PDC_EN_XTAL);
        da1469x_pdc_set(g_cmac_host_pdc_cmac2sys);
        da1469x_pdc_ack(g_cmac_host_pdc_cmac2sys);
    }

    /* Enable Radio LDO */
    CRG_TOP->POWER_CTRL_REG |= CRG_TOP_POWER_CTRL_REG_LDO_RADIO_ENABLE_Msk;

    /* Enable CMAC, but keep it in reset */
    CRG_TOP->CLK_RADIO_REG = (1 << CRG_TOP_CLK_RADIO_REG_RFCU_ENABLE_Pos) |
                             (1 << CRG_TOP_CLK_RADIO_REG_CMAC_SYNCH_RESET_Pos) |
                             (0 << CRG_TOP_CLK_RADIO_REG_CMAC_CLK_SEL_Pos) |
                             (1 << CRG_TOP_CLK_RADIO_REG_CMAC_CLK_ENABLE_Pos) |
                             (0 << CRG_TOP_CLK_RADIO_REG_CMAC_DIV_Pos);

    /* Calculate size of ROM and RAM area (for now they should be the same) */
    cmac_rom_size = &_binary_cmac_rom_bin_end - &_binary_cmac_rom_bin_start[0];
    cmac_ram_size = &_binary_cmac_ram_bin_end - &_binary_cmac_ram_bin_start[0];
    assert(cmac_rom_size == cmac_ram_size);

    /* Load image header and check if image can be loaded */
    memcpy(&ii, &_binary_cmac_rom_bin_start, sizeof(ii));
    assert(ii.magic == 0x43414d43); /* "CMAC" */

    /* Copy CMAC image to RAM */
    memset(&_binary_cmac_ram_bin_start, 0, cmac_ram_size);
    memcpy(&_binary_cmac_ram_bin_start, &_binary_cmac_rom_bin_start[sizeof(ii)],
           ii.size_rom);

    /*
     * Setup memory controller for CMAC
     * Code and data are set to the same address initially since CMAC will
     * update data address on init. Also shared address is updated on init.
     */
    MEMCTRL->CMI_CODE_BASE_REG = (uint32_t)&_binary_cmac_ram_bin_start;
    MEMCTRL->CMI_DATA_BASE_REG = MEMCTRL->CMI_CODE_BASE_REG;
    MEMCTRL->CMI_SHARED_BASE_REG = 0;
    MEMCTRL->CMI_END_REG = (uint32_t)&_binary_cmac_ram_bin_end;

    /* Symbols below are in shared memory, can update them now */
    cmac_config = CMAC_SYM_CONFIG;
    cmac_config_dyn = CMAC_SYM_CONFIG_DYN;
    cmac_mbox_rx = CMAC_SYM_MBOX_RX;
    cmac_mbox_tx = CMAC_SYM_MBOX_TX;

    /* Update CMAC configuration */
    cmac_config->lp_clock_freq = 0;
    cmac_config->lp_clock_sca = 50;
    cmac_config->rx_buf_len = 251 + 11;
    cmac_config->tx_buf_len = 251 + 11;
    cmac_config->initial_length_req = 0;
    cmac_config->system_tcs_length = 0;
    cmac_config->synth_tcs_length = 0;
    cmac_config->rfcu_tcs_length = 0;
    cmac_config->default_tx_power = 4;
    cmac_config_dyn->enable_sleep = false;

    /* Release CMAC from reset */
    CRG_TOP->CLK_RADIO_REG &= ~CRG_TOP_CLK_RADIO_REG_CMAC_SYNCH_RESET_Msk;

    /* Wait for CMAC to update registers */
    while (MEMCTRL->CMI_DATA_BASE_REG == MEMCTRL->CMI_CODE_BASE_REG);
    while (MEMCTRL->CMI_SHARED_BASE_REG != (MEMCTRL->CMI_END_REG & 0xfffffc00));

    /* Initialize mailboxes and sync with CMAC */
    cmac_mbox_tx->flags = CMAC_MBOX_F_RESET;
    cmac_mbox_tx->wr_off = 0;
    cmac_mbox_tx->rd_off = 0;
    cmac_mbox_tx->magic = 0xa55a;
    while (cmac_mbox_rx->magic != 0xa55a);

    NVIC_SetVector(CMAC2SYS_IRQn, (uint32_t)cmac2sys_isr);
    NVIC_SetPriority(CMAC2SYS_IRQn, 0);
    NVIC_EnableIRQ(CMAC2SYS_IRQn);

    cmac_host_signal2cmac();

    da1469x_lpclk_register_cmac_cb(cmac_host_lpclk_cb);
}
