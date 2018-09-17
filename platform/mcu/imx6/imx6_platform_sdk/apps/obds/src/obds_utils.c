/*
 * Copyright (c) 2009-2012, Freescale Semiconductor, Inc.
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

/*!
 * @defgroup diag_init Base OBDS files
 */

/*!
 * @file  obds_utils.c
 *
 * @ingroup diag_init
 */

#include "sdk.h"
#include "obds_utils.h"
#include "registers/regssrc.h"

/* Boot mode status registers in the SRC */
struct soc_sbmr1 {
    unsigned int boot_cfg1:8, boot_cfg2:8, boot_cfg3:8, boot_cfg4:8;
} __attribute__ ((packed));
struct soc_sbmr1 *soc_sbmr1 = (struct soc_sbmr1 *)(HW_SRC_SBMR1_ADDR);

struct soc_sbmr2 {
    unsigned int sec_config:2,
        rsv0:1, dir_bt_dis:1, bt_fuse_sel:1, rsv1:19, bmod:2, rsv2:1, test_mode:3, rsv3:2;
} __attribute__ ((packed));
struct soc_sbmr2 *soc_sbmr2 = (struct soc_sbmr2 *)(HW_SRC_SBMR2_ADDR);

/*!
 *  Display boot switch information
 */
void show_boot_switch_info(void)
{
    printf("========== BOOT configuration \n");
    printf("Security config: ");

    if (soc_sbmr2->sec_config == 0) {
        printf("FAB (Open)\n");
    } else if (soc_sbmr2->sec_config == 0x1) {
        printf("Open -allows any code to be flashed & executed (even without valid signature)\n");
    } else {
        printf("Closed (Security On)\n");
    }

    printf("Boot From: ");

    switch ((soc_sbmr1->boot_cfg1 >> 4) & 0xF) {
    case 0x0:
        printf("WEIM: ");
        if ((soc_sbmr1->boot_cfg1 & 0x08) == 0)
            printf("NOR");
        else
            printf("ONE NAND");
        break;
    case 0x1:
        printf("Reserved !!!");
        break;
    case 0x2:
        printf("SATA Hard drive: ");
        if ((soc_sbmr1->boot_cfg2 & 0x4) == 0)
            printf("GEN 2 (3.0Gbps)");
        else
            printf("GEN 1 (1.5Gbps)");
        break;
    case 0x3:
        printf("Serial ROM: ");
        if ((soc_sbmr1->boot_cfg1 & 0x2) == 0)
            printf("I2C EEPROM: ");
        else
            printf("SPI NOR: CS#%d ", (soc_sbmr1->boot_cfg4 >> 4) & 0x3);
        switch (soc_sbmr1->boot_cfg4 & 0x7) {
        case 0x0:
            printf("on eCSPI1");
            break;
        case 0x1:
            printf("on eCSPI2");
            break;
        case 0x2:
            printf("on eCSPI3");
            break;
        case 0x3:
            printf("on eCSPI4");
            break;
        case 0x4:
            printf("on eCSPI5");
            break;
        case 0x5:
            printf("on I2C-1");
            break;
        case 0x6:
            printf("on I2C-2");
            break;
        case 0x7:
            printf("on I2C-3");
            break;
        }
        break;
    case 0x4:
    case 0x5:
        printf("SD/eSD/SDXC: ");
        if ((soc_sbmr1->boot_cfg1 & 0x10) == 0)
            printf("normal boot: ");
        else
            printf("fast boot: ");
        printf("speed ");
        if ((soc_sbmr1->boot_cfg1 & 0x0C) == 0)
            printf("SDR12: ");
        else if ((soc_sbmr1->boot_cfg1 & 0x0C) == 0x04)
            printf("SDR25: ");
        else if ((soc_sbmr1->boot_cfg1 & 0x0C) == 0x08)
            printf("SDR50: ");
        else
            printf("SDR104: ");
        printf("bus width ");
        if ((soc_sbmr1->boot_cfg2 & 0xE0) == 0)
            printf("1-bit on ");
        else
            printf("4-bit on ");
        if ((soc_sbmr1->boot_cfg2 & 0x18) == 0)
            printf("USDHC1");
        else if ((soc_sbmr1->boot_cfg2 & 0x18) == 0x8)
            printf("USDHC2");
        else if ((soc_sbmr1->boot_cfg2 & 0x18) == 0x10)
            printf("USDHC3");
        else
            printf("USDHC4");
        break;
    case 0x6:
    case 0x7:
        printf("MMC/eMMC: ");
        if ((soc_sbmr1->boot_cfg1 & 0x10) == 0)
            printf("normal boot: ");
        else
            printf("fast boot: ");
        if ((soc_sbmr1->boot_cfg1 & 0x08) == 0)
            printf("normal speed: ");
        else
            printf("fast speed: ");
        printf("bus width ");
        if ((soc_sbmr1->boot_cfg1 & 0xE0) == 0)
            printf("1-bit on ");
        else if ((soc_sbmr1->boot_cfg1 & 0xE0) == 0x20)
            printf("4-bit on ");
        else if ((soc_sbmr1->boot_cfg1 & 0xE0) == 0x40)
            printf("8-bit on ");
        else if ((soc_sbmr1->boot_cfg1 & 0xE0) == 0xA0)
            printf("4-bit DDR on ");
        else if ((soc_sbmr1->boot_cfg1 & 0xE0) == 0xC0)
            printf("8-bit DDR on ");
        else
            printf("reserved on ");
        if ((soc_sbmr1->boot_cfg2 & 0x18) == 0)
            printf("USDHC1");
        else if ((soc_sbmr1->boot_cfg2 & 0x18) == 0x8)
            printf("USDHC2");
        else if ((soc_sbmr1->boot_cfg2 & 0x18) == 0x10)
            printf("USDHC3");
        else
            printf("USDHC4");
        break;
    case 0x8:
    default:
        printf("NAND: ADDR CYCLES %d: ", (soc_sbmr1->boot_cfg1 & 0x3) + 3);
        printf("%d device: ", 2 ^ ((soc_sbmr1->boot_cfg1 & 0xC) >> 2));
        if ((soc_sbmr1->boot_cfg2 & 0x06) == 0)
            printf("128 ");
        else if ((soc_sbmr1->boot_cfg2 & 0x06) == 0x2)
            printf("64 ");
        else if ((soc_sbmr1->boot_cfg2 & 0x06) == 0x4)
            printf("32 ");
        else
            printf("256 ");
        printf("page per blocks");
        break;
    }

    printf("\n==================================\n\n");
}

