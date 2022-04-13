/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "FreeRTOS.h"

#include <mt_reg_base.h>
#include <mt_printf.h>
#include <mt8512_clk.h>
#include <mt8512_power.h>
#include <mtk_pd_provider.h>
#include <mtk_pd.h>

/*
 * MT8512 power domain support
 */

static const struct mtk_pd_data mtk_pd_data_mt8512[] = {
    [MT8512_POWER_DOMAIN_MM] = {
        .name = "mm",
        .sta_mask = BIT(15),
        .ctl_ofs = 0x374,
        .sram_pdn_ofs = 0x374,
        .sram_pdn_bits = GENMASK(8, 8),
        .sram_pdn_ack_bits = GENMASK(12, 12),
        .prot_table = {
            BUS_PROT(INFRACFG_AO_BASE, 0, BIT(16) | BIT(17), 0x2a8, 0x2ac, 0x258),
            BUS_PROT(INFRACFG_AO_BASE, 0, BIT(8) | BIT(9) | BIT(10) | BIT(11), 0x42c, 0x430, 0x428),
        },
        .clks_id = {CLK_TOP_MM_SEL},
        .subsys_clks_id = {CLK_MM_SMI_COMM0, CLK_MM_SMI_COMM1},
    },
    [MT8512_POWER_DOMAIN_IMG] = {
        .name = "img",
        .sta_mask = BIT(16),
        .ctl_ofs = 0x38C,
        .sram_pdn_ofs = 0x38C,
        .sram_pdn_bits = GENMASK(8, 8),
        .sram_pdn_ack_bits = GENMASK(12, 12),
        .prot_table = {
        },
        .clks_id = {CLK_TOP_MM_SEL},
    },
    [MT8512_POWER_DOMAIN_IP0] = {
        .name = "ip0",
        .sta_mask = BIT(25),
        .ctl_ofs = 0x39C,
        .sram_pdn_ofs = 0x39C,
        .sram_pdn_bits = GENMASK(8, 8),
        .sram_pdn_ack_bits = GENMASK(12, 12),
        .prot_table = {
            BUS_PROT(INFRACFG_AO_BASE, 0, BIT(12), 0x42c, 0x430, 0x428),
            BUS_PROT(INFRACFG_AO_BASE, 0, BIT(5) | BIT(6) | BIT(7), 0x42c, 0x430, 0x428),
            BUS_PROT(INFRACFG_AO_BASE, 0, BIT(14) | BIT(16) | BIT(18), 0x42c, 0x430, 0x428),
        },
        .clks_id = {CLK_IP_TEST_26M},
        .subsys_clks_id = {CLK_IP_EMI_CK_GATE, CLK_IP_NNA0_PWR_GATE,
            CLK_IP_SRAM_OCC_GATE, CLK_IP_HD_FAXI_CK},
    },
    [MT8512_POWER_DOMAIN_IP1] = {
        .name = "ip1",
        .sta_mask = BIT(26),
        .ctl_ofs = 0x384,
        .sram_pdn_ofs = 0x384,
        .sram_pdn_bits = GENMASK(8, 8),
        .sram_pdn_ack_bits = GENMASK(12, 12),
        .prot_table = {
            BUS_PROT(INFRACFG_AO_BASE, 0, BIT(4), 0x2a0, 0x2a4, 0x228),
            BUS_PROT(INFRACFG_AO_BASE, 0, BIT(2), 0x2a0, 0x2a4, 0x228),
        },
        .clks_id = {CLK_IP_TEST_26M},
        .subsys_clks_id = {CLK_IP_NNA1_OCC_GATE, CLK_IP_NNA1_PWR_GATE},
    },
    [MT8512_POWER_DOMAIN_IP2] = {
        .name = "ip2",
        .sta_mask = BIT(27),
        .ctl_ofs = 0x388,
        .sram_pdn_ofs = 0x388,
        .sram_pdn_bits = GENMASK(8, 8),
        .sram_pdn_ack_bits = GENMASK(12, 12),
        .prot_table = {
            BUS_PROT(INFRACFG_AO_BASE, 0, BIT(5), 0x2a0, 0x2a4, 0x228),
            BUS_PROT(INFRACFG_AO_BASE, 0, BIT(3), 0x2a0, 0x2a4, 0x228),
        },
        .clks_id = {CLK_IP_TEST_26M},
        .subsys_clks_id = {CLK_IP_WFST_OCC_GATE, CLK_IP_WFST_PWR_GATE},
    },
    [MT8512_POWER_DOMAIN_CONN] = {
        .name = "conn",
        .sta_mask = BIT(1),
        .ctl_ofs = 0x32C,
        .sram_pdn_ofs = 0x32C,
        .sram_pdn_bits = 0,
        .sram_pdn_ack_bits = 0,
        .prot_table = {
            BUS_PROT(INFRACFG_AO_BASE, 0, BIT(13), 0x2a0, 0x2a4, 0x228),
            BUS_PROT(INFRACFG_AO_BASE, 0, BIT(18), 0x2a8, 0x2ac, 0x258),
            BUS_PROT(INFRACFG_AO_BASE, 0, BIT(14), 0x2a0, 0x2a4, 0x228),
            BUS_PROT(INFRACFG_AO_BASE, 0, BIT(21), 0x2a8, 0x2ac, 0x258),
        },
        .clks_id = {CLK_TOP_CONN_32K, CLK_TOP_CONN_26M},
    },
    [MT8512_POWER_DOMAIN_DSP] = {
        .name = "dsp",
        .sta_mask = BIT(17),
        .ctl_ofs = 0x37C,
        .sram_pdn_ofs = 0x37C,
        .sram_pdn_bits = GENMASK(11, 8),
        .sram_pdn_ack_bits = GENMASK(15, 12),
        .prot_table = {
            BUS_PROT(INFRACFG_AO_BASE, 0, BIT(7) | BIT(10) | BIT(11), 0x2a0, 0x2a4, 0x228),
            BUS_PROT(INFRACFG_AO_BASE, 0, BIT(1) | BIT(24), 0x2a0, 0x2a4, 0x228),
        },
        .clks_id = {CLK_TOP_DSP_SEL, CLK_TOP_DSP_26M},
        .subsys_clks_id = {CLK_TOP_SRAM_SEL, CLK_INFRA_DSP_AXI},
    },
    [MT8512_POWER_DOMAIN_AUDAFE] = {
        .name = "audafe",
        .sta_mask = BIT(24),
        .ctl_ofs = 0x314,
        .sram_pdn_ofs = 0x314,
        .sram_pdn_bits = GENMASK(11, 8),
        .sram_pdn_ack_bits = GENMASK(16, 13),
        .prot_table = {
            BUS_PROT(INFRACFG_AO_BASE, 0, BIT(28), 0x2a0, 0x2a4, 0x228),
            BUS_PROT(INFRACFG_AO_BASE, 0, BIT(22), 0x2a0, 0x2a4, 0x228),
        },
        .clks_id = {CLK_TOP_A1SYS_SEL},
        .subsys_clks_id = {CLK_INFRA_AUDIO},
    },
    [MT8512_POWER_DOMAIN_AUDSRC] = {
        .name = "audsrc",
        .sta_mask = BIT(23),
        .ctl_ofs = 0x328,
        .sram_pdn_ofs = 0x328,
        .sram_pdn_bits = GENMASK(8, 8),
        .sram_pdn_ack_bits = GENMASK(12, 12),
        .prot_table = {
            BUS_PROT(INFRACFG_AO_BASE, 0, BIT(21), 0x2a0, 0x2a4, 0x228),
        },
        .clks_id = {CLK_INFRA_AUD_ASRC},
    },
    [MT8512_POWER_DOMAIN_USB] = {
        .name = "usb",
        .sta_mask = BIT(20),
        .ctl_ofs = 0x3A4,
        .sram_pdn_ofs = 0x3A4,
        .sram_pdn_bits = GENMASK(8, 8),
        .sram_pdn_ack_bits = GENMASK(12, 12),
        .prot_table = {
            BUS_PROT(INFRACFG_AO_BASE, 0, BIT(0) | BIT(1), 0x42c, 0x430, 0x428),
        },
        .clks_id = {CLK_TOP_SSUSB_TOP_CK_EN},
    },
};

static void pd_mt8512_bring_up(void)
{
    /*struct mtk_pd *pd;
    int i;
    int mt8512_crit_pds[] = {
    };

    for (i = 0; i < ARRAY_SIZE(mt8512_crit_pds); i++) {
        pd = mtk_pd_get(mt8512_crit_pds[i]);
        if (!is_err_or_null_pd(pd))
            mtk_pd_enable(pd);
    }*/
}

void pd_mt8512_init(void)
{
    int ret = 0;

    ret = mtk_pds_register(mtk_pd_data_mt8512, ARRAY_SIZE(mtk_pd_data_mt8512));
    if (ret)
        printf("mt8512_pd_init: failed to register power domain\n");

    pd_mt8512_bring_up();

    return;
}
