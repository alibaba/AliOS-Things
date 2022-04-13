/*
 * Copyright (c) 2020 MediaTek Inc.
 * Author: Chen Zhong <chen.zhong@mediatek.com>
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

#include <FreeRTOS.h>
#include <mt_printf.h>
#include <mtk_clk_provider.h>
#include <clk.h>
#include <mt_reg_base.h>

#include <mt8512_clk.h>

#define mt8512_top_base         ((void *)CKSYS_BASE)
#define mt8512_infra_base    ((void *)INFRACFG_AO_BASE)
#define mt8512_apmixed_base     ((void *)APMIXED_BASE)
#define mt8512_ip_base     ((void *)IPSYS_BASE)
#define mt8512_mm_base        ((void *)MMSYS_CONFIG_BASE)
#define mt8512_img_base        ((void *)IMGSYS_CONFIG_BASE)

static struct mtk_clk_fixed_rate pad_fixed_clks[] = {
    FIXED_CLK(CLK_PAD_32K, "clk32k", NULL, 32000),
    FIXED_CLK(CLK_PAD_26M, "clk26m", NULL, 26000000),
};

static struct mtk_clk_fixed_rate top_fixed_clks[] = {
    FIXED_CLK(CLK_TOP_CLK_NULL, "clk_null", NULL, 0),
};

static struct mtk_clk_fixed_factor top_divs[] = {
    FACTOR(CLK_TOP_SYSPLL1_D2, "syspll1_d2", "mainpll", 1, 4),
    FACTOR(CLK_TOP_SYSPLL1_D4, "syspll1_d4", "mainpll", 1, 8),
    FACTOR(CLK_TOP_SYSPLL1_D8, "syspll1_d8", "mainpll", 1, 16),
    FACTOR(CLK_TOP_SYSPLL1_D16, "syspll1_d16", "mainpll", 1, 32),
    FACTOR(CLK_TOP_SYSPLL_D3, "syspll_d3", "mainpll", 1, 3),
    FACTOR(CLK_TOP_SYSPLL2_D2, "syspll2_d2", "mainpll", 1, 6),
    FACTOR(CLK_TOP_SYSPLL2_D4, "syspll2_d4", "mainpll", 1, 12),
    FACTOR(CLK_TOP_SYSPLL2_D8, "syspll2_d8", "mainpll", 1, 24),
    FACTOR(CLK_TOP_SYSPLL_D5, "syspll_d5", "mainpll", 1, 5),
    FACTOR(CLK_TOP_SYSPLL3_D4, "syspll3_d4", "mainpll", 1, 20),
    FACTOR(CLK_TOP_SYSPLL_D7, "syspll_d7", "mainpll", 1, 7),
    FACTOR(CLK_TOP_SYSPLL4_D2, "syspll4_d2", "mainpll", 1, 14),
    FACTOR(CLK_TOP_UNIVPLL, "univpll", "univpll2", 1, 2),
    FACTOR(CLK_TOP_UNIVPLL_D2, "univpll_d2", "univpll", 1, 2),
    FACTOR(CLK_TOP_UNIVPLL1_D2, "univpll1_d2", "univpll", 1, 4),
    FACTOR(CLK_TOP_UNIVPLL1_D4, "univpll1_d4", "univpll", 1, 8),
    FACTOR(CLK_TOP_UNIVPLL1_D8, "univpll1_d8", "univpll", 1, 16),
    FACTOR(CLK_TOP_UNIVPLL_D3, "univpll_d3", "univpll", 1, 3),
    FACTOR(CLK_TOP_UNIVPLL2_D2, "univpll2_d2", "univpll", 1, 6),
    FACTOR(CLK_TOP_UNIVPLL2_D4, "univpll2_d4", "univpll", 1, 12),
    FACTOR(CLK_TOP_UNIVPLL2_D8, "univpll2_d8", "univpll", 1, 24),
    FACTOR(CLK_TOP_UNIVPLL_D5, "univpll_d5", "univpll", 1, 5),
    FACTOR(CLK_TOP_UNIVPLL3_D2, "univpll3_d2", "univpll", 1, 10),
    FACTOR(CLK_TOP_UNIVPLL3_D4, "univpll3_d4", "univpll", 1, 20),
    FACTOR(CLK_TOP_TCONPLL_D2, "tconpll_d2", "tconpll", 1, 2),
    FACTOR(CLK_TOP_TCONPLL_D4, "tconpll_d4", "tconpll", 1, 4),
    FACTOR(CLK_TOP_TCONPLL_D8, "tconpll_d8", "tconpll", 1, 8),
    FACTOR(CLK_TOP_TCONPLL_D16, "tconpll_d16", "tconpll", 1, 16),
    FACTOR(CLK_TOP_TCONPLL_D32, "tconpll_d32", "tconpll", 1, 32),
    FACTOR(CLK_TOP_TCONPLL_D64, "tconpll_d64", "tconpll", 1, 64),
    FACTOR(CLK_TOP_USB20_192M, "usb20_192m_ck", "univpll", 2, 13),
    FACTOR(CLK_TOP_USB20_192M_D2, "usb20_192m_d2", "usb20_192m_ck", 1, 2),
    FACTOR(CLK_TOP_USB20_192M_D4_T, "usb20_192m_d4_t", "usb20_192m_ck", 1, 4),
    FACTOR(CLK_TOP_APLL1, "apll1_ck", "apll1", 1, 1),
    FACTOR(CLK_TOP_APLL1_D2, "apll1_d2", "apll1", 1, 2),
    FACTOR(CLK_TOP_APLL1_D3, "apll1_d3", "apll1", 1, 3),
    FACTOR(CLK_TOP_APLL1_D4, "apll1_d4", "apll1", 1, 4),
    FACTOR(CLK_TOP_APLL1_D8, "apll1_d8", "apll1", 1, 8),
    FACTOR(CLK_TOP_APLL1_D16, "apll1_d16", "apll1", 1, 16),
    FACTOR(CLK_TOP_APLL2, "apll2_ck", "apll2", 1, 1),
    FACTOR(CLK_TOP_APLL2_D2, "apll2_d2", "apll2", 1, 2),
    FACTOR(CLK_TOP_APLL2_D3, "apll2_d3", "apll2", 1, 3),
    FACTOR(CLK_TOP_APLL2_D4, "apll2_d4", "apll2", 1, 4),
    FACTOR(CLK_TOP_APLL2_D8, "apll2_d8", "apll2", 1, 8),
    FACTOR(CLK_TOP_APLL2_D16, "apll2_d16", "apll2", 1, 16),
    FACTOR(CLK_TOP_CLK26M, "clk26m_ck", "clk26m", 1, 1),
    FACTOR(CLK_TOP_SYS_26M_D2, "sys_26m_d2", "clk26m", 1, 2),
    FACTOR(CLK_TOP_MSDCPLL, "msdcpll_ck", "msdcpll", 1, 1),
    FACTOR(CLK_TOP_MSDCPLL_D2, "msdcpll_d2", "msdcpll", 1, 2),
    FACTOR(CLK_TOP_DSPPLL, "dsppll_ck", "dsppll", 1, 1),
    FACTOR(CLK_TOP_DSPPLL_D2, "dsppll_d2", "dsppll", 1, 2),
    FACTOR(CLK_TOP_DSPPLL_D4, "dsppll_d4", "dsppll", 1, 4),
    FACTOR(CLK_TOP_DSPPLL_D8, "dsppll_d8", "dsppll", 1, 8),
    FACTOR(CLK_TOP_IPPLL, "ippll_ck", "ippll", 1, 1),
    FACTOR(CLK_TOP_IPPLL_D2, "ippll_d2", "ippll", 1, 2),
    FACTOR(CLK_TOP_NFI2X_CK_D2, "nfi2x_ck_d2", "nfi2x_sel", 1, 2),
};

static const char * const axi_parents[] = {
    "clk26m_ck",
    "syspll1_d4",
    "univpll3_d2",
    "syspll1_d8",
    "sys_26m_d2",
    "clk32k"
};

static const char * const mem_parents[] = {
    "dsppll_ck",
    "ippll_ck",
    "clk26m_ck",
    "univpll_d3"
};

static const char * const uart_parents[] = {
    "clk26m_ck",
    "univpll2_d8"
};

static const char * const spi_parents[] = {
    "clk26m_ck",
    "univpll2_d2",
    "syspll2_d2",
    "univpll1_d4",
    "syspll1_d4",
    "univpll3_d2",
    "univpll2_d4",
    "syspll4_d2"
};

static const char * const spis_parents[] = {
    "clk26m_ck",
    "univpll_d3",
    "syspll_d3",
    "univpll1_d2",
    "univpll2_d2",
    "univpll1_d4",
    "univpll2_d4",
    "syspll4_d2"
};

static const char * const msdc50_0_hc_parents[] = {
    "clk26m_ck",
    "syspll1_d2",
    "univpll1_d4",
    "syspll2_d2"
};

static const char * const msdc50_0_parents[] = {
    "clk26m_ck",
    "msdcpll_d2",
    "univpll2_d2",
    "syspll2_d2",
    "univpll1_d4",
    "syspll1_d4",
    "syspll2_d4",
    "univpll2_d8"
};

static const char * const msdc50_2_parents[] = {
    "clk26m_ck",
    "msdcpll_ck",
    "univpll_d3",
    "univpll1_d2",
    "syspll1_d2",
    "univpll2_d2",
    "syspll2_d2",
    "univpll1_d4"
};

static const char * const audio_parents[] = {
    "clk26m_ck",
    "univpll2_d8",
    "apll1_d4",
    "apll2_d4"
};

static const char * const aud_intbus_parents[] = {
    "clk26m_ck",
    "syspll1_d4",
    "univpll3_d2",
    "apll2_d8",
    "sys_26m_d2",
    "apll1_d8",
    "univpll3_d4"
};

static const char * const hapll1_parents[] = {
    "clk26m_ck",
    "apll1_ck",
    "apll1_d2",
    "apll1_d3",
    "apll1_d4",
    "apll1_d8",
    "apll1_d16",
    "sys_26m_d2"
};

static const char * const hapll2_parents[] = {
    "clk26m_ck",
    "apll2_ck",
    "apll2_d2",
    "apll2_d3",
    "apll2_d4",
    "apll2_d8",
    "apll2_d16",
    "sys_26m_d2"
};

static const char * const asm_l_parents[] = {
    "clk26m_ck",
    "univpll2_d4",
    "univpll2_d2",
    "syspll_d5"
};

static const char * const aud_spdif_parents[] = {
    "clk26m_ck",
    "univpll_d2",
    "dsppll_ck"
};

static const char * const aud_1_parents[] = {
    "clk26m_ck",
    "apll1_ck"
};

static const char * const aud_2_parents[] = {
    "clk26m_ck",
    "apll2_ck"
};

static const char * const ssusb_sys_parents[] = {
    "clk26m_ck",
    "univpll3_d4",
    "univpll2_d4",
    "univpll3_d2"
};

static const char * const spm_parents[] = {
    "clk26m_ck",
    "syspll1_d8"
};

static const char * const i2c_parents[] = {
    "clk26m_ck",
    "sys_26m_d2",
    "univpll3_d4",
    "univpll3_d2",
    "syspll1_d8",
    "syspll2_d8",
    "clk32k"
};

static const char * const pwm_parents[] = {
    "clk26m_ck",
    "univpll3_d4",
    "syspll1_d8",
    "univpll2_d4",
    "sys_26m_d2",
    "clk32k"
};

static const char * const dsp_parents[] = {
    "clk26m_ck",
    "dsppll_ck",
    "dsppll_d2",
    "dsppll_d4",
    "dsppll_d8",
    "apll2_d4",
    "sys_26m_d2",
    "clk32k"
};

static const char * const nfi2x_parents[] = {
    "clk26m_ck",
    "syspll2_d2",
    "syspll_d7",
    "syspll_d3",
    "syspll2_d4",
    "msdcpll_d2",
    "univpll1_d2",
    "univpll_d5"
};

static const char * const spinfi_parents[] = {
    "clk26m_ck",
    "univpll2_d8",
    "univpll3_d4",
    "syspll1_d8",
    "syspll4_d2",
    "syspll2_d4",
    "univpll2_d4",
    "univpll3_d2"
};

static const char * const ecc_parents[] = {
    "clk26m_ck",
    "syspll_d5",
    "syspll_d3",
    "univpll_d3"
};

static const char * const gcpu_parents[] = {
    "clk26m_ck",
    "univpll_d3",
    "syspll_d3",
    "univpll1_d2",
    "syspll1_d2",
    "univpll2_d2"
};

static const char * const gcpu_cpm_parents[] = {
    "clk26m_ck",
    "univpll2_d2",
    "syspll2_d2",
    "univpll1_d4"
};

static const char * const mbist_diag_parents[] = {
    "clk26m_ck",
    "sys_26m_d2"
};

static const char * const ip0_nna_parents[] = {
    "clk26m_ck",
    "dsppll_ck",
    "dsppll_d2",
    "dsppll_d4",
    "ippll_ck",
    "sys_26m_d2",
    "ippll_d2",
    "msdcpll_d2"
};

static const char * const ip2_wfst_parents[] = {
    "clk26m_ck",
    "univpll_d3",
    "univpll1_d2",
    "univpll2_d2",
    "ippll_ck",
    "ippll_d2",
    "sys_26m_d2",
    "msdcpll_ck"
};

static const char * const sflash_parents[] = {
    "clk26m_ck",
    "syspll1_d16",
    "syspll2_d8",
    "syspll3_d4",
    "univpll3_d4",
    "univpll1_d8",
    "usb20_192m_d2",
    "univpll2_d4"
};

static const char * const sram_parents[] = {
    "clk26m_ck",
    "dsppll_ck",
    "univpll_d3",
    "syspll1_d2",
    "apll1_ck",
    "apll2_ck",
    "syspll1_d4",
    "sys_26m_d2"
};

static const char * const mm_parents[] = {
    "clk26m_ck",
    "syspll_d3",
    "syspll1_d2",
    "syspll_d5",
    "syspll1_d4",
    "univpll_d5",
    "univpll1_d2",
    "univpll_d3"
};

static const char * const dpi0_parents[] = {
    "clk26m_ck",
    "tconpll_d2",
    "tconpll_d4",
    "tconpll_d8",
    "tconpll_d16",
    "tconpll_d32",
    "tconpll_d64"
};

static const char * const dbg_atclk_parents[] = {
    "clk26m_ck",
    "univpll1_d2",
    "univpll_d5"
};

static const char * const occ_104m_parents[] = {
    "univpll2_d4",
    "univpll2_d8"
};

static const char * const occ_68m_parents[] = {
    "syspll1_d8",
    "univpll2_d8"
};

static const char * const occ_182m_parents[] = {
    "syspll2_d2",
    "univpll1_d4",
    "univpll2_d8"
};

static const char * const apll_fi2si1_parents[] = {
    "aud_1_sel",
    "aud_2_sel"
};

#define TOP_MUX_CLR_SET_UPD(_id, _name, _parents, _mux_ofs, _mux_set_ofs,\
            _mux_clr_ofs, _shift, _width, _gate,        \
            _upd_ofs, _upd)            \
    CLR_SET_UPD_FLAGS(_id, _name, _parents, mt8512_top_base, _mux_ofs, _mux_set_ofs,\
            _mux_clr_ofs, _shift, _width, _gate,        \
            _upd_ofs, _upd, CLK_SET_RATE_PARENT)

#define TOP_MUX(_id, _name, _parents, _mux_ofs, _shift, _width)        \
    MUX_FLAGS(_id, _name, _parents, mt8512_top_base, _mux_ofs,    \
            _shift, _width, CLK_SET_RATE_PARENT)

static struct mtk_clk_mux top_muxes[] = {
    /* CLK_CFG_0 */
    TOP_MUX_CLR_SET_UPD(CLK_TOP_AXI_SEL, "axi_sel",
        axi_parents, 0x040, 0x044, 0x048, 0, 3, 7,
        0x4, 0),
    TOP_MUX_CLR_SET_UPD(CLK_TOP_MEM_SEL, "mem_sel",
        mem_parents, 0x040, 0x044, 0x048, 8, 2, 15, 0x4, 1),
    TOP_MUX_CLR_SET_UPD(CLK_TOP_UART_SEL, "uart_sel",
        uart_parents, 0x040, 0x044, 0x048, 16, 1, 23, 0x4, 2),
    TOP_MUX_CLR_SET_UPD(CLK_TOP_SPI_SEL, "spi_sel",
        spi_parents, 0x040, 0x044, 0x048, 24, 3, 31, 0x4, 3),
    /* CLK_CFG_1 */
    TOP_MUX_CLR_SET_UPD(CLK_TOP_SPIS_SEL, "spis_sel",
        spis_parents, 0x050, 0x054, 0x058, 0, 3, 7, 0x4, 4),
    TOP_MUX_CLR_SET_UPD(CLK_TOP_MSDC50_0_HC_SEL, "msdc50_0_hc_sel",
        msdc50_0_hc_parents, 0x050, 0x054, 0x058, 8, 2, 15, 0x4, 5),
    TOP_MUX_CLR_SET_UPD(CLK_TOP_MSDC2_2_HC_SEL, "msdc2_2_hc_sel",
        msdc50_0_hc_parents, 0x050, 0x054, 0x058, 16, 2, 23, 0x4, 6),
    TOP_MUX_CLR_SET_UPD(CLK_TOP_MSDC50_0_SEL, "msdc50_0_sel",
        msdc50_0_parents, 0x050, 0x054, 0x058, 24, 3, 31, 0x4, 7),
    /* CLK_CFG_2 */
    TOP_MUX_CLR_SET_UPD(CLK_TOP_MSDC50_2_SEL, "msdc50_2_sel",
        msdc50_2_parents, 0x060, 0x064, 0x068, 0, 3, 7, 0x4, 8),
    TOP_MUX_CLR_SET_UPD(CLK_TOP_MSDC30_1_SEL, "msdc30_1_sel",
        msdc50_0_parents, 0x060, 0x064, 0x068, 8, 3, 15, 0x4, 9),
    TOP_MUX_CLR_SET_UPD(CLK_TOP_AUDIO_SEL, "audio_sel",
        audio_parents, 0x060, 0x064, 0x068, 16, 2, 23, 0x4, 10),
    TOP_MUX_CLR_SET_UPD(CLK_TOP_AUD_INTBUS_SEL, "aud_intbus_sel",
        aud_intbus_parents, 0x060, 0x064, 0x068, 24, 3, 31, 0x4, 11),
    /* CLK_CFG_3 */
    TOP_MUX_CLR_SET_UPD(CLK_TOP_HAPLL1_SEL, "hapll1_sel",
        hapll1_parents, 0x070, 0x074, 0x078, 0, 3, 7, 0x4, 12),
    TOP_MUX_CLR_SET_UPD(CLK_TOP_HAPLL2_SEL, "hapll2_sel",
        hapll2_parents, 0x070, 0x074, 0x078, 8, 3, 15, 0x4, 13),
    TOP_MUX_CLR_SET_UPD(CLK_TOP_A2SYS_SEL, "a2sys_sel",
        hapll1_parents, 0x070, 0x074, 0x078, 16, 3, 23, 0x4, 14),
    TOP_MUX_CLR_SET_UPD(CLK_TOP_A1SYS_SEL, "a1sys_sel",
        hapll2_parents, 0x070, 0x074, 0x078, 24, 3, 31, 0x4, 15),
    /* CLK_CFG_4 */
    TOP_MUX_CLR_SET_UPD(CLK_TOP_ASM_L_SEL, "asm_l_sel",
        asm_l_parents, 0x080, 0x084, 0x088, 0, 2, 7, 0x4, 16),
    TOP_MUX_CLR_SET_UPD(CLK_TOP_ASM_M_SEL, "asm_m_sel",
        asm_l_parents, 0x080, 0x084, 0x088, 8, 2, 15, 0x4, 17),
    TOP_MUX_CLR_SET_UPD(CLK_TOP_ASM_H_SEL, "asm_h_sel",
        asm_l_parents, 0x080, 0x084, 0x088, 16, 2, 23, 0x4, 18),
    TOP_MUX_CLR_SET_UPD(CLK_TOP_AUD_SPDIF_SEL, "aud_spdif_sel",
        aud_spdif_parents, 0x080, 0x084, 0x088, 24, 2, 31, 0x4, 19),
    /* CLK_CFG_5 */
    TOP_MUX_CLR_SET_UPD(CLK_TOP_AUD_1_SEL, "aud_1_sel",
        aud_1_parents, 0x090, 0x094, 0x098, 0, 1, 7, 0x4, 20),
    TOP_MUX_CLR_SET_UPD(CLK_TOP_AUD_2_SEL, "aud_2_sel",
        aud_2_parents, 0x090, 0x094, 0x098, 8, 1, 15, 0x4, 21),
    TOP_MUX_CLR_SET_UPD(CLK_TOP_SSUSB_SYS_SEL, "ssusb_sys_sel",
        ssusb_sys_parents, 0x090, 0x094, 0x098, 16, 2, 23, 0x4, 22),
    TOP_MUX_CLR_SET_UPD(CLK_TOP_SSUSB_XHCI_SEL, "ssusb_xhci_sel",
        ssusb_sys_parents, 0x090, 0x094, 0x098, 24, 2, 31, 0x4, 23),
    /* CLK_CFG_6 */
    TOP_MUX_CLR_SET_UPD(CLK_TOP_SPM_SEL, "spm_sel",
        spm_parents, 0x0a0, 0x0a4, 0x0a8, 0, 1, 7,
        0x4, 24),
    TOP_MUX_CLR_SET_UPD(CLK_TOP_I2C_SEL, "i2c_sel",
        i2c_parents, 0x0a0, 0x0a4, 0x0a8, 8, 3, 15, 0x4, 25),
    TOP_MUX_CLR_SET_UPD(CLK_TOP_PWM_SEL, "pwm_sel",
        pwm_parents, 0x0a0, 0x0a4, 0x0a8, 16, 3, 23, 0x4, 26),
    TOP_MUX_CLR_SET_UPD(CLK_TOP_DSP_SEL, "dsp_sel",
        dsp_parents, 0x0a0, 0x0a4, 0x0a8, 24, 3, 31, 0x4, 27),
    /* CLK_CFG_7 */
    TOP_MUX_CLR_SET_UPD(CLK_TOP_NFI2X_SEL, "nfi2x_sel",
        nfi2x_parents, 0x0b0, 0x0b4, 0x0b8, 0, 3, 7, 0x4, 28),
    TOP_MUX_CLR_SET_UPD(CLK_TOP_SPINFI_SEL, "spinfi_sel",
        spinfi_parents, 0x0b0, 0x0b4, 0x0b8, 8, 3, 15, 0x4, 29),
    TOP_MUX_CLR_SET_UPD(CLK_TOP_ECC_SEL, "ecc_sel",
        ecc_parents, 0x0b0, 0x0b4, 0x0b8, 16, 2, 23, 0x4, 30),
    TOP_MUX_CLR_SET_UPD(CLK_TOP_GCPU_SEL, "gcpu_sel",
        gcpu_parents, 0x0b0, 0x0b4, 0x0b8, 24, 3, 31, 0x4, 31),
    /* CLK_CFG_8 */
    TOP_MUX_CLR_SET_UPD(CLK_TOP_GCPU_CPM_SEL, "gcpu_cpm_sel",
        gcpu_cpm_parents, 0x0c0, 0x0c4, 0x0c8, 0, 2, 7, 0x8, 0),
    TOP_MUX_CLR_SET_UPD(CLK_TOP_MBIST_DIAG_SEL, "mbist_diag_sel",
        mbist_diag_parents, 0x0c0, 0x0c4, 0x0c8, 8, 1, 15, 0x8, 1),
    TOP_MUX_CLR_SET_UPD(CLK_TOP_IP0_NNA_SEL, "ip0_nna_sel",
        ip0_nna_parents, 0x0c0, 0x0c4, 0x0c8, 16, 3, 23, 0x8, 2),
    TOP_MUX_CLR_SET_UPD(CLK_TOP_IP1_NNA_SEL, "ip1_nna_sel",
        ip0_nna_parents, 0x0c0, 0x0c4, 0x0c8, 24, 3, 31, 0x8, 3),
    /* CLK_CFG_9 */
    TOP_MUX_CLR_SET_UPD(CLK_TOP_IP2_WFST_SEL, "ip2_wfst_sel",
        ip2_wfst_parents, 0x0d0, 0x0d4, 0x0d8, 0, 3, 7, 0x8, 4),
    TOP_MUX_CLR_SET_UPD(CLK_TOP_SFLASH_SEL, "sflash_sel",
        sflash_parents, 0x0d0, 0x0d4, 0x0d8, 8, 3, 15, 0x8, 5),
    TOP_MUX_CLR_SET_UPD(CLK_TOP_SRAM_SEL, "sram_sel",
        sram_parents, 0x0d0, 0x0d4, 0x0d8, 16, 3, 23, 0x8, 6),
    TOP_MUX_CLR_SET_UPD(CLK_TOP_MM_SEL, "mm_sel",
        mm_parents, 0x0d0, 0x0d4, 0x0d8, 24, 3, 31, 0x8, 7),
    /* CLK_CFG_10 */
    TOP_MUX_CLR_SET_UPD(CLK_TOP_DPI0_SEL, "dpi0_sel",
        dpi0_parents, 0x0e0, 0x0e4, 0x0e8, 0, 3, 7, 0x8, 8),
    TOP_MUX_CLR_SET_UPD(CLK_TOP_DBG_ATCLK_SEL, "dbg_atclk_sel",
        dbg_atclk_parents, 0x0e0, 0x0e4, 0x0e8, 8, 2, 15, 0x8, 9),
    TOP_MUX_CLR_SET_UPD(CLK_TOP_OCC_104M_SEL, "occ_104m_sel",
        occ_104m_parents, 0x0e0, 0x0e4, 0x0e8, 16, 1, 23, 0x8, 10),
    TOP_MUX_CLR_SET_UPD(CLK_TOP_OCC_68M_SEL, "occ_68m_sel",
        occ_68m_parents, 0x0e0, 0x0e4, 0x0e8, 24, 1, 31, 0x8, 11),
    /* CLK_CFG_11 */
    TOP_MUX_CLR_SET_UPD(CLK_TOP_OCC_182M_SEL, "occ_182m_sel",
        occ_182m_parents, 0x0ec, 0x0f0, 0x0f4, 0, 2, 7, 0x8, 12),
    /* CLK_AUDDIV_4 */
    TOP_MUX(CLK_TOP_APLL_FI2SI1_SEL, "apll_fi2si1_sel", apll_fi2si1_parents,
        0x340, 11, 1),
    TOP_MUX(CLK_TOP_APLL_FTDMIN_SEL, "apll_ftdmin_sel", apll_fi2si1_parents,
        0x340, 12, 1),
    TOP_MUX(CLK_TOP_APLL_FI2SO1_SEL, "apll_fi2so1_sel", apll_fi2si1_parents,
        0x340, 13, 1),
};

#define DIV_ADJ(_id, _name, _parent, _ofs, _shift, _width)    \
    DIV(_id, _name, _parent, mt8512_top_base, _ofs, _shift, _width)

static struct mtk_clk_div top_adj_divs[] = {
    DIV_ADJ(CLK_TOP_APLL12_CK_DIV7, "apll12_ck_div7",
        "apll_fi2si1_sel", 0x344, 0, 8),
    DIV_ADJ(CLK_TOP_APLL12_CK_DIV8, "apll12_ck_div8",
        "apll_ftdmin_sel", 0x344, 8, 8),
    DIV_ADJ(CLK_TOP_APLL12_CK_DIV9, "apll12_ck_div9",
        "apll_fi2so1_sel", 0x344, 16, 8),
};

static const struct mtk_clk_gate_regs top0_cg_regs = {
    .set_ofs = 0x0,
    .clr_ofs = 0x0,
    .sta_ofs = 0x0,
};

static const struct mtk_clk_gate_regs top1_cg_regs = {
    .set_ofs = 0x104,
    .clr_ofs = 0x104,
    .sta_ofs = 0x104,
};

static const struct mtk_clk_gate_regs top2_cg_regs = {
    .set_ofs = 0x340,
    .clr_ofs = 0x340,
    .sta_ofs = 0x340,
};

#define GATE_TOP0(_id, _name, _parent, _shift)    \
    GATE(_id, _name, _parent, mt8512_top_base, _shift, top0_cg_regs, mtk_clk_gate_ops)

#define GATE_TOP1(_id, _name, _parent, _shift)    \
    GATE(_id, _name, _parent, mt8512_top_base, _shift, top1_cg_regs, mtk_clk_gate_inv_ops)

#define GATE_TOP2(_id, _name, _parent, _shift)    \
    GATE(_id, _name, _parent, mt8512_top_base, _shift, top2_cg_regs, mtk_clk_gate_ops)

static struct mtk_clk_gate top_clks[] = {
    /* TOP0 */
    GATE_TOP0(CLK_TOP_CONN_32K, "conn_32k", "clk32k", 10),
    GATE_TOP0(CLK_TOP_CONN_26M, "conn_26m", "clk26m_ck", 11),
    GATE_TOP0(CLK_TOP_DSP_32K, "dsp_32k", "clk32k", 16),
    GATE_TOP0(CLK_TOP_DSP_26M, "dsp_26m", "clk26m_ck", 17),
    /* TOP1 */
    GATE_TOP1(CLK_TOP_USB20_48M_EN, "usb20_48m_en", "usb20_192m_d4_t", 8),
    GATE_TOP1(CLK_TOP_UNIVPLL_48M_EN, "univpll_48m_en", "usb20_192m_d4_t", 9),
    GATE_TOP1(CLK_TOP_SSUSB_TOP_CK_EN, "ssusb_top_ck_en", "clk_null", 22),
    GATE_TOP1(CLK_TOP_SSUSB_PHY_CK_EN, "ssusb_phy_ck_en", "clk_null", 23),
    /* TOP2 */
    GATE_TOP2(CLK_TOP_I2SI1_MCK, "i2si1_mck", "apll12_ck_div7", 0),
    GATE_TOP2(CLK_TOP_TDMIN_MCK, "tdmin_mck", "apll12_ck_div8", 1),
    GATE_TOP2(CLK_TOP_I2SO1_MCK, "i2so1_mck", "apll12_ck_div9", 2),
};

static const struct mtk_clk_gate_regs infra0_cg_regs = {
    .set_ofs = 0x294,
    .clr_ofs = 0x294,
    .sta_ofs = 0x294,
};

static const struct mtk_clk_gate_regs infra1_cg_regs = {
    .set_ofs = 0x80,
    .clr_ofs = 0x84,
    .sta_ofs = 0x90,
};

static const struct mtk_clk_gate_regs infra2_cg_regs = {
    .set_ofs = 0x88,
    .clr_ofs = 0x8c,
    .sta_ofs = 0x94,
};

static const struct mtk_clk_gate_regs infra3_cg_regs = {
    .set_ofs = 0xa4,
    .clr_ofs = 0xa8,
    .sta_ofs = 0xac,
};

static const struct mtk_clk_gate_regs infra4_cg_regs = {
    .set_ofs = 0xc0,
    .clr_ofs = 0xc4,
    .sta_ofs = 0xc8,
};

static const struct mtk_clk_gate_regs infra5_cg_regs = {
    .set_ofs = 0xd0,
    .clr_ofs = 0xd4,
    .sta_ofs = 0xd8,
};

#define GATE_INFRA0(_id, _name, _parent, _shift)    \
    GATE(_id, _name, _parent, mt8512_infra_base, _shift, infra0_cg_regs, mtk_clk_gate_inv_ops)

#define GATE_INFRA1(_id, _name, _parent, _shift)    \
    GATE(_id, _name, _parent, mt8512_infra_base, _shift, infra1_cg_regs, mtk_clk_gate_setclr_ops)

#define GATE_INFRA2(_id, _name, _parent, _shift)    \
    GATE(_id, _name, _parent, mt8512_infra_base, _shift, infra2_cg_regs, mtk_clk_gate_setclr_ops)

#define GATE_INFRA3(_id, _name, _parent, _shift)    \
    GATE(_id, _name, _parent, mt8512_infra_base, _shift, infra3_cg_regs, mtk_clk_gate_setclr_ops)

#define GATE_INFRA4(_id, _name, _parent, _shift)    \
    GATE(_id, _name, _parent, mt8512_infra_base, _shift, infra4_cg_regs, mtk_clk_gate_setclr_ops)

#define GATE_INFRA5(_id, _name, _parent, _shift)    \
    GATE(_id, _name, _parent, mt8512_infra_base, _shift, infra5_cg_regs, mtk_clk_gate_setclr_ops)

static struct mtk_clk_gate infra_clks[] = {
    /* INFRA0 */
    GATE_INFRA0(CLK_INFRA_DSP_AXI, "infra_dsp_axi", "axi_sel", 8),
    /* INFRA1 */
    GATE_INFRA1(CLK_INFRA_ICUSB, "infra_icusb", "axi_sel", 8),
    GATE_INFRA1(CLK_INFRA_GCE, "infra_gce", "axi_sel", 9),
    GATE_INFRA1(CLK_INFRA_THERM, "infra_therm", "axi_sel", 10),
    GATE_INFRA1(CLK_INFRA_PWM_HCLK, "infra_pwm_hclk", "axi_sel", 15),
    GATE_INFRA1(CLK_INFRA_PWM1, "infra_pwm1", "pwm_sel", 16),
    GATE_INFRA1(CLK_INFRA_PWM2, "infra_pwm2", "pwm_sel", 17),
    GATE_INFRA1(CLK_INFRA_PWM3, "infra_pwm3", "pwm_sel", 18),
    GATE_INFRA1(CLK_INFRA_PWM4, "infra_pwm4", "pwm_sel", 19),
    GATE_INFRA1(CLK_INFRA_PWM5, "infra_pwm5", "pwm_sel", 20),
    GATE_INFRA1(CLK_INFRA_PWM, "infra_pwm", "pwm_sel", 21),
    GATE_INFRA1(CLK_INFRA_UART0, "infra_uart0", "uart_sel", 22),
    GATE_INFRA1(CLK_INFRA_UART1, "infra_uart1", "uart_sel", 23),
    GATE_INFRA1(CLK_INFRA_UART2, "infra_uart2", "uart_sel", 24),
    GATE_INFRA1(CLK_INFRA_DSP_UART, "infra_dsp_uart", "uart_sel", 26),
    GATE_INFRA1(CLK_INFRA_GCE_26M, "infra_gce_26m", "clk26m_ck", 27),
    GATE_INFRA1(CLK_INFRA_CQDMA_FPC, "infra_cqdma_fpc", "axi_sel", 28),
    GATE_INFRA1(CLK_INFRA_BTIF, "infra_btif", "axi_sel", 31),
    /* INFRA2 */
    GATE_INFRA2(CLK_INFRA_SPI, "infra_spi", "spi_sel", 1),
    GATE_INFRA2(CLK_INFRA_MSDC0, "infra_msdc0", "msdc50_0_hc_sel", 2),
    GATE_INFRA2(CLK_INFRA_MSDC1, "infra_msdc1", "axi_sel", 4),
    GATE_INFRA2(CLK_INFRA_DVFSRC, "infra_dvfsrc", "clk26m_ck", 7),
    GATE_INFRA2(CLK_INFRA_GCPU, "infra_gcpu", "axi_sel", 8),
    GATE_INFRA2(CLK_INFRA_TRNG, "infra_trng", "axi_sel", 9),
    GATE_INFRA2(CLK_INFRA_AUXADC, "infra_auxadc", "clk26m_ck", 10),
    GATE_INFRA2(CLK_INFRA_AUXADC_MD, "infra_auxadc_md", "clk26m_ck", 14),
    GATE_INFRA2(CLK_INFRA_AP_DMA, "infra_ap_dma", "axi_sel", 18),
    GATE_INFRA2(CLK_INFRA_DEBUGSYS, "infra_debugsys", "axi_sel", 24),
    GATE_INFRA2(CLK_INFRA_AUDIO, "infra_audio", "axi_sel", 25),
    GATE_INFRA2(CLK_INFRA_FLASHIF, "infra_flashif", "sflash_sel", 29),
    /* INFRA3 */
    GATE_INFRA3(CLK_INFRA_PWM_FB6, "infra_pwm_fb6", "pwm_sel", 0),
    GATE_INFRA3(CLK_INFRA_PWM_FB7, "infra_pwm_fb7", "pwm_sel", 1),
    GATE_INFRA3(CLK_INFRA_AUD_ASRC, "infra_aud_asrc", "axi_sel", 3),
    GATE_INFRA3(CLK_INFRA_AUD_26M, "infra_aud_26m", "clk26m_ck", 4),
    GATE_INFRA3(CLK_INFRA_SPIS, "infra_spis", "axi_sel", 6),
    GATE_INFRA3(CLK_INFRA_CQ_DMA, "infra_cq_dma", "axi_sel", 27),
    /* INFRA4 */
    GATE_INFRA4(CLK_INFRA_AP_MSDC0, "infra_ap_msdc0", "msdc50_0_sel", 7),
    GATE_INFRA4(CLK_INFRA_MD_MSDC0, "infra_md_msdc0", "msdc50_0_sel", 8),
    GATE_INFRA4(CLK_INFRA_MSDC0_SRC, "infra_msdc0_src", "msdc50_0_sel", 9),
    GATE_INFRA4(CLK_INFRA_MSDC1_SRC, "infra_msdc1_src", "msdc30_1_sel", 10),
    GATE_INFRA4(CLK_INFRA_IRRX_26M, "infra_irrx_26m", "axi_sel", 22),
    GATE_INFRA4(CLK_INFRA_IRRX_32K, "infra_irrx_32k", "clk32k", 23),
    GATE_INFRA4(CLK_INFRA_I2C0_AXI, "infra_i2c0_axi", "i2c_sel", 24),
    GATE_INFRA4(CLK_INFRA_I2C1_AXI, "infra_i2c1_axi", "i2c_sel", 25),
    GATE_INFRA4(CLK_INFRA_I2C2_AXI, "infra_i2c2_axi", "i2c_sel", 26),
    /* INFRA5 */
    GATE_INFRA5(CLK_INFRA_NFI, "infra_nfi", "nfi2x_ck_d2", 1),
    GATE_INFRA5(CLK_INFRA_NFIECC, "infra_nfiecc", "nfi2x_ck_d2", 2),
    GATE_INFRA5(CLK_INFRA_NFI_HCLK, "infra_nfi_hclk", "axi_sel", 3),
    GATE_INFRA5(CLK_INFRA_SUSB_133, "infra_susb_133", "axi_sel", 7),
    GATE_INFRA5(CLK_INFRA_USB_SYS, "infra_usb_sys", "ssusb_sys_sel", 9),
    GATE_INFRA5(CLK_INFRA_USB_XHCI, "infra_usb_xhci", "ssusb_xhci_sel", 11),
};

static const struct mtk_clk_gate_regs ip0_cg_regs = {
    .set_ofs = 0x100,
    .clr_ofs = 0x100,
    .sta_ofs = 0x100,
};

static const struct mtk_clk_gate_regs ip1_cg_regs = {
    .set_ofs = 0x104,
    .clr_ofs = 0x104,
    .sta_ofs = 0x104,
};

static const struct mtk_clk_gate_regs ip2_cg_regs = {
    .set_ofs = 0x108,
    .clr_ofs = 0x108,
    .sta_ofs = 0x108,
};

static const struct mtk_clk_gate_regs ip3_cg_regs = {
    .set_ofs = 0x110,
    .clr_ofs = 0x110,
    .sta_ofs = 0x110,
};

static const struct mtk_clk_gate_regs ip4_cg_regs = {
    .set_ofs = 0x114,
    .clr_ofs = 0x114,
    .sta_ofs = 0x114,
};

static const struct mtk_clk_gate_regs ip5_cg_regs = {
    .set_ofs = 0x118,
    .clr_ofs = 0x118,
    .sta_ofs = 0x118,
};

static const struct mtk_clk_gate_regs ip6_cg_regs = {
    .set_ofs = 0x98,
    .clr_ofs = 0x98,
    .sta_ofs = 0x98,
};

static const struct mtk_clk_gate_regs ip7_cg_regs = {
    .set_ofs = 0x9c,
    .clr_ofs = 0x9c,
    .sta_ofs = 0x9c,
};

static const struct mtk_clk_gate_regs ip8_cg_regs = {
    .set_ofs = 0xa0,
    .clr_ofs = 0xa0,
    .sta_ofs = 0xa0,
};

static const struct mtk_clk_gate_regs ip9_cg_regs = {
    .set_ofs = 0xfc,
    .clr_ofs = 0xfc,
    .sta_ofs = 0xfc,
};

#define GATE_IP0(_id, _name, _parent, _shift)    \
    GATE(_id, _name, _parent, mt8512_ip_base, _shift, ip0_cg_regs, mtk_clk_gate_inv_ops)

#define GATE_IP1(_id, _name, _parent, _shift)    \
    GATE(_id, _name, _parent, mt8512_ip_base, _shift, ip1_cg_regs, mtk_clk_gate_inv_ops)

#define GATE_IP2(_id, _name, _parent, _shift)    \
    GATE(_id, _name, _parent, mt8512_ip_base, _shift, ip2_cg_regs, mtk_clk_gate_inv_ops)

#define GATE_IP3(_id, _name, _parent, _shift)    \
    GATE(_id, _name, _parent, mt8512_ip_base, _shift, ip3_cg_regs, mtk_clk_gate_inv_ops)

#define GATE_IP4(_id, _name, _parent, _shift)    \
    GATE(_id, _name, _parent, mt8512_ip_base, _shift, ip4_cg_regs, mtk_clk_gate_inv_ops)

#define GATE_IP5(_id, _name, _parent, _shift)    \
    GATE(_id, _name, _parent, mt8512_ip_base, _shift, ip5_cg_regs, mtk_clk_gate_inv_ops)

#define GATE_IP6(_id, _name, _parent, _shift)    \
    GATE(_id, _name, _parent, mt8512_ip_base, _shift, ip6_cg_regs, mtk_clk_gate_inv_ops)

#define GATE_IP7(_id, _name, _parent, _shift)    \
    GATE(_id, _name, _parent, mt8512_ip_base, _shift, ip7_cg_regs, mtk_clk_gate_inv_ops)

#define GATE_IP8(_id, _name, _parent, _shift)    \
    GATE(_id, _name, _parent, mt8512_ip_base, _shift, ip8_cg_regs, mtk_clk_gate_inv_ops)

#define GATE_IP9(_id, _name, _parent, _shift)    \
    GATE(_id, _name, _parent, mt8512_ip_base, _shift, ip9_cg_regs, mtk_clk_gate_inv_ops)

static struct mtk_clk_gate ip_clks[] = {
    /* IP0 */
    GATE_IP0(CLK_IP_EMI_CK_GATE, "ip_emi_ck_gate", "clk_null", 0),
    /* IP1 */
    GATE_IP1(CLK_IP_SRAM_OCC_GATE, "ip_sram_occ", "sram_sel", 0),
    /* IP2 */
    GATE_IP2(CLK_IP_HD_FAXI_CK, "ip_hd_faxi_ck", "axi_sel", 0),
    /* IP3 */
    GATE_IP3(CLK_IP_NNA0_PWR_GATE, "ip_clk_nna0_pwr", "ip0_nna_sel", 0),
    /* IP4 */
    GATE_IP4(CLK_IP_NNA1_PWR_GATE, "ip_clk_nna1_pwr", "ip1_nna_sel", 0),
    /* IP5 */
    GATE_IP5(CLK_IP_WFST_PWR_GATE, "ip_clk_wfst_pwr", "ip2_wfst_sel", 0),
    /* IP6 */
    GATE_IP6(CLK_IP_NNA0_OCC_GATE, "ip_nna0_occ", "ip0_nna_sel", 0),
    /* IP7 */
    GATE_IP7(CLK_IP_NNA1_OCC_GATE, "ip_nna1_occ", "ip1_nna_sel", 0),
    /* IP8 */
    GATE_IP8(CLK_IP_WFST_OCC_GATE, "ip_wfst_occ", "ip2_wfst_sel", 0),
    /* IP9 */
    GATE_IP9(CLK_IP_TEST_26M, "ip_test_26m_ck", "clk26m_ck", 0),
};

static const struct mtk_clk_gate_regs mm_cg_regs = {
    .set_ofs = 0x104,
    .clr_ofs = 0x108,
    .sta_ofs = 0x100,
};

#define GATE_MM(_id, _name, _parent, _shift)    \
    GATE(_id, _name, _parent, mt8512_mm_base, _shift, mm_cg_regs, mtk_clk_gate_setclr_ops)

static struct mtk_clk_gate mm_clks[] = {
    GATE_MM(CLK_MM_PIPELINE0, "mm_pipeline0", "mm_sel", 0),
    GATE_MM(CLK_MM_PIPELINE1, "mm_pipeline1", "mm_sel", 1),
    GATE_MM(CLK_MM_PIPELINE2, "mm_pipeline2", "mm_sel", 2),
    GATE_MM(CLK_MM_PIPELINE3, "mm_pipeline3", "mm_sel", 3),
    GATE_MM(CLK_MM_PIPELINE4, "mm_pipeline4", "mm_sel", 4),
    GATE_MM(CLK_MM_PIPELINE5, "mm_pipeline5", "mm_sel", 5),
    GATE_MM(CLK_MM_PIPELINE7, "mm_pipeline7", "mm_sel", 7),
    GATE_MM(CLK_MM_DPI0_DPI_TMP0, "mm_dpi_tmp0", "dpi0_sel", 19),
    GATE_MM(CLK_MM_DPI0_DPI_TMP1, "mm_dpi_tmp1", "dpi0_sel", 20),
    GATE_MM(CLK_MM_DISP_FAKE, "mm_disp_fake", "mm_sel", 21),
    GATE_MM(CLK_MM_SMI_COMMON, "mm_smi_common", "mm_sel", 22),
    GATE_MM(CLK_MM_SMI_LARB0, "mm_smi_larb0", "mm_sel", 23),
    GATE_MM(CLK_MM_SMI_COMM0, "mm_smi_comm0", "mm_sel", 24),
    GATE_MM(CLK_MM_SMI_COMM1, "mm_smi_comm1", "mm_sel", 25),
};

static const struct mtk_clk_gate_regs img0_cg_regs = {
    .set_ofs = 0x104,
    .clr_ofs = 0x108,
    .sta_ofs = 0x100,
};

static const struct mtk_clk_gate_regs img1_cg_regs = {
    .set_ofs = 0x114,
    .clr_ofs = 0x118,
    .sta_ofs = 0x110,
};

#define GATE_IMG0(_id, _name, _parent, _shift)    \
    GATE(_id, _name, _parent, mt8512_img_base, _shift, img0_cg_regs, mtk_clk_gate_setclr_ops)

#define GATE_IMG1(_id, _name, _parent, _shift)    \
    GATE(_id, _name, _parent, mt8512_img_base, _shift, img1_cg_regs, mtk_clk_gate_setclr_ops)

static struct mtk_clk_gate img_clks[] = {
    /* IMG0 */
    GATE_IMG0(CLK_IMG_MDP_RDMA0, "img_mdp_rdma0", "mm_sel", 0),
    GATE_IMG0(CLK_IMG_MDP_RSZ0, "img_mdp_rsz0", "mm_sel", 2),
    GATE_IMG0(CLK_IMG_MDP_TDSHP0, "img_mdp_tdshp0", "mm_sel", 4),
    GATE_IMG0(CLK_IMG_MDP_WROT0, "img_mdp_wrot0", "mm_sel", 5),
    GATE_IMG0(CLK_IMG_DISP_OVL0_21, "img_disp_ovl0", "mm_sel", 8),
    GATE_IMG0(CLK_IMG_DISP_WDMA0, "img_disp_wdma0", "mm_sel", 11),
    GATE_IMG0(CLK_IMG_DISP_GAMMA0, "img_disp_gamma0", "mm_sel", 15),
    GATE_IMG0(CLK_IMG_DISP_DITHER0, "img_disp_di", "mm_sel", 16),
    GATE_IMG0(CLK_IMG_FAKE, "img_fake", "mm_sel", 21),
    GATE_IMG0(CLK_IMG_SMI_LARB1, "img_smi_larb1", "mm_sel", 23),
    /* IMG1 */
    GATE_IMG1(CLK_IMG_JPGDEC, "img_jpgdec", "mm_sel", 1),
    GATE_IMG1(CLK_IMG_PNGDEC, "img_pngdec", "mm_sel", 2),
    GATE_IMG1(CLK_IMG_IMGRZ, "img_imgrz", "mm_sel", 3),
};

#define MT8512_PLL_FMAX        (3800UL * MHZ)
#define MT8512_PLL_FMIN        (1500UL * MHZ)

#define CON0_MT8512_RST_BAR    BIT(23)

#define PLL_B(_id, _name, _reg, _pwr_reg, _en_mask, _flags, _pcwbits,    \
        _pd_reg, _pd_shift, _tuner_reg, _tuner_en_reg,        \
        _tuner_en_bit,    _pcw_reg, _pcw_shift, _div_table,    \
        _rst_bar_mask, _pcw_chg_reg) {                \
        .mck.id = _id,                            \
        .mck.name = _name,                        \
        .mck.pn.parent_name = "clk26m_ck",                    \
        .mck.num_parents = 1,                \
        .mck.ops = &mtk_clk_pll_ops,                    \
        .pll_base = mt8512_apmixed_base,                         \
        .base_ofs = _reg,                        \
        .pwr_ofs = _pwr_reg,                        \
        .pcw_ofs = _pcw_reg,                        \
        .pcw_shift = _pcw_shift,                    \
        .pcwbits = _pcwbits,                        \
        .pcwibits = 8,                        \
        .pcw_chg_ofs = _pcw_chg_reg,                \
        .pd_ofs = _pd_reg,                        \
        .pd_shift = _pd_shift,                        \
        .tuner_ofs = _tuner_reg,                    \
        .tuner_en_ofs = _tuner_en_reg,                    \
        .tuner_en_shift = _tuner_en_bit,                \
        .en_mask = _en_mask,                        \
        .rst_bar_mask = _rst_bar_mask,                    \
        .flags = _flags,                        \
        .fmax = MT8512_PLL_FMAX,                \
        .fmin = MT8512_PLL_FMIN,                \
        .div_table = _div_table,                    \
    }

#define PLL(_id, _name, _reg, _pwr_reg, _en_mask, _flags, _pcwbits,    \
            _pd_reg, _pd_shift, _tuner_reg,            \
            _tuner_en_reg, _tuner_en_bit, _pcw_reg,        \
            _pcw_shift, _rst_bar_mask, _pcw_chg_reg)    \
        PLL_B(_id, _name, _reg, _pwr_reg, _en_mask, _flags,    \
            _pcwbits, _pd_reg, _pd_shift,            \
            _tuner_reg, _tuner_en_reg, _tuner_en_bit,    \
            _pcw_reg, _pcw_shift, NULL, _rst_bar_mask,    \
            _pcw_chg_reg)                    \

static const struct mtk_pll_div_table armpll_div_table[] = {
    { .div = 0, .freq = MT8512_PLL_FMAX },
    { .div = 1, .freq = 1850000000 },
    { .div = 2, .freq = 750000000 },
    { .div = 3, .freq = 375000000 },
    { .div = 4, .freq = 182500000 },
    { } /* sentinel */
};

static const struct mtk_pll_div_table dsppll_div_table[] = {
    { .div = 0, .freq = MT8512_PLL_FMAX },
    { .div = 1, .freq = 1600000000 },
    { .div = 2, .freq = 750000000 },
    { .div = 3, .freq = 400000000 },
    { .div = 4, .freq = 200000000 },
    { } /* sentinel */
};

static struct mtk_clk_pll plls[] = {
    PLL_B(CLK_APMIXED_ARMPLL, "armpll", 0x030C, 0x0318, 0x00000001,
        0, 22, 0x0310, 24, 0, 0, 0, 0x0310, 0, armpll_div_table, 0, 0),
    PLL(CLK_APMIXED_MAINPLL, "mainpll", 0x0228, 0x0234, 0x00000001,
        0, 22, 0x022C, 24, 0, 0, 0, 0x022C, 0,
        CON0_MT8512_RST_BAR, 0),
    PLL(CLK_APMIXED_UNIVPLL2, "univpll2", 0x0208, 0x0214, 0x00000001,
        0, 22, 0x020C, 24, 0, 0, 0, 0x020C, 0,
        CON0_MT8512_RST_BAR, 0),
    PLL(CLK_APMIXED_MSDCPLL, "msdcpll", 0x0350, 0x035C, 0x00000001,
        0, 22, 0x0354, 24, 0, 0, 0, 0x0354, 0, 0, 0),
    PLL(CLK_APMIXED_APLL1, "apll1", 0x031C, 0x032C, 0x00000001,
        0, 32, 0x0320, 24, 0x0040, 0x000C, 0, 0x0324, 0, 0, 0x0320),
    PLL(CLK_APMIXED_APLL2, "apll2", 0x0360, 0x0370, 0x00000001,
        0, 32, 0x0364, 24, 0x004C, 0x000C, 5, 0x0368, 0, 0, 0x0364),
    PLL(CLK_APMIXED_IPPLL, "ippll", 0x0374, 0x0380, 0x00000001,
        0, 22, 0x0378, 24, 0, 0, 0, 0x0378, 0, 0, 0),
    PLL_B(CLK_APMIXED_DSPPLL, "dsppll", 0x0390, 0x039C, 0x00000001,
        0, 22, 0x0394, 24, 0, 0, 0, 0x0394, 0, dsppll_div_table, 0, 0),
    PLL(CLK_APMIXED_TCONPLL, "tconpll", 0x03A0, 0x03AC, 0x00000001,
        0, 22, 0x03A4, 24, 0, 0, 0, 0x03A4, 0, 0, 0),
};

static int clk_mt8512_pad_init()
{
    struct mtk_clk_data *clk_data;
    int ret = 0;

    clk_data = mtk_alloc_clk_data(CLK_PAD_NR_CLK);

    INIT_CLK_DATA(clk_data, pad_fixed_clks);

    ret = mtk_clk_add_provider(CLK_PVD_PAD, "pad", clk_data);
    if (ret)
        printf("could not register clock provider: pad, ret = %d!\n", ret);

    return ret;
}

static int clk_mt8512_apmixed_init(void)
{
    struct mtk_clk_data *clk_data;
    int ret = 0;

    clk_data = mtk_alloc_clk_data(CLK_APMIXED_NR_CLK);

    INIT_CLK_DATA(clk_data, plls);

    ret = mtk_clk_add_provider(CLK_PVD_APMIXED, "apmixedsys", clk_data);
    if (ret)
        printf("could not register clock provider: apmixedsys, ret = %d!\n", ret);

    return ret;
}

static int clk_mt8512_top_init(void)
{
    struct mtk_clk_data *clk_data;
    int ret = 0;

    clk_data = mtk_alloc_clk_data(CLK_TOP_NR_CLK);

    INIT_CLK_DATA(clk_data, top_fixed_clks);
    INIT_CLK_DATA(clk_data, top_divs);
    INIT_CLK_DATA(clk_data, top_muxes);
    INIT_CLK_DATA(clk_data, top_adj_divs);
    INIT_CLK_DATA(clk_data, top_clks);

    ret = mtk_clk_add_provider(CLK_PVD_TOP, "topckgen", clk_data);
    if (ret)
        printf("could not register clock provider: topckgen, ret = %d!\n", ret);

    return ret;
}

static int clk_mt8512_infra_init(void)
{
    struct mtk_clk_data *clk_data;
    int ret = 0;

    clk_data = mtk_alloc_clk_data(CLK_INFRA_NR_CLK);

    INIT_CLK_DATA(clk_data, infra_clks);

    ret = mtk_clk_add_provider(CLK_PVD_INFRA, "infrasys", clk_data);
    if (ret)
        printf("could not register clock provider: infrasys, ret = %d!\n", ret);

    return ret;
}

static int clk_mt8512_ip_init(void)
{
    struct mtk_clk_data *clk_data;
    int ret = 0;

    clk_data = mtk_alloc_clk_data(CLK_IP_NR_CLK);

    INIT_CLK_DATA(clk_data, ip_clks);

    ret = mtk_clk_add_provider(CLK_PVD_IP, "ipsys", clk_data);
    if (ret)
        printf("could not register clock provider: ipsys, ret = %d!\n", ret);

    return ret;
}

static int clk_mt8512_mm_init(void)
{
    struct mtk_clk_data *clk_data;
    int ret = 0;

    clk_data = mtk_alloc_clk_data(CLK_MM_NR_CLK);

    INIT_CLK_DATA(clk_data, mm_clks);

    ret = mtk_clk_add_provider(CLK_PVD_MM, "mmsys", clk_data);
    if (ret)
        printf("could not register clock provider: mmsys, ret = %d!\n", ret);

    return ret;
}

static int clk_mt8512_img_init(void)
{
    struct mtk_clk_data *clk_data;
    int ret = 0;

    clk_data = mtk_alloc_clk_data(CLK_IMG_NR_CLK);

    INIT_CLK_DATA(clk_data, img_clks);

    ret = mtk_clk_add_provider(CLK_PVD_IMG, "imgsys", clk_data);
    if (ret)
        printf("could not register clock provider: imgsys, ret = %d!\n", ret);

    return ret;
}

static void clk_mt8512_enable_critical(void)
{
    struct clk *clk;
    int i;
    int mt8512_crit_clks[] = {
        CLK_APMIXED_ARMPLL,
        CLK_APMIXED_MAINPLL,
        CLK_APMIXED_APLL1,
        CLK_TOP_AXI_SEL,
        CLK_INFRA_UART0,
    };

    for (i = 0; i < ARRAY_SIZE(mt8512_crit_clks); i++) {
        clk = clk_get(mt8512_crit_clks[i]);
        if (!is_err_or_null_clk(clk))
            clk_enable(clk);
    }
}

extern void mtk_clkdbg_mt8512_init(void);

void clk_mt8512_init(void)
{
    int ret;

    ret = clk_mt8512_pad_init();
    if (ret)
        printf("clk_mt8512_pad_init fail!\n");

    ret = clk_mt8512_apmixed_init();
    if (ret)
        printf("clk_mt8512_apmixed_init fail!\n");

    ret = clk_mt8512_top_init();
    if (ret)
        printf("clk_mt8512_top_init fail!\n");

    ret = clk_mt8512_infra_init();
    if (ret)
        printf("clk_mt8512_infra_init fail!\n");

    ret = clk_mt8512_ip_init();
    if (ret)
        printf("clk_mt8512_ip_init fail!\n");

    ret = clk_mt8512_mm_init();
    if (ret)
        printf("clk_mt8512_mm_init fail!\n");

    ret = clk_mt8512_img_init();
    if (ret)
        printf("clk_mt8512_img_init fail!\n");

    clk_mt8512_enable_critical();

    mtk_clkdbg_mt8512_init();

    return;
}
