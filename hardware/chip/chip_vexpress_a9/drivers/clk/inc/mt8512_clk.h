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

#ifndef _CLK_MT8512_H
#define _CLK_MT8512_H

#include "mtk_clk_provider.h"

/* PAD */

#define CLK_PVD_PAD              0
#define PAD_CLK(i)               CLK_ID(CLK_PVD_PAD, i)
#define CLK_PAD_32K              PAD_CLK(0)
#define CLK_PAD_26M              PAD_CLK(1)
#define CLK_PAD_NR_CLK           2

/* APMIXEDSYS */

#define CLK_PVD_APMIXED          1
#define APMIXED_CLK(i)           CLK_ID(CLK_PVD_APMIXED, i)
#define CLK_APMIXED_ARMPLL        APMIXED_CLK(0)
#define CLK_APMIXED_MAINPLL        APMIXED_CLK(1)
#define CLK_APMIXED_UNIVPLL2        APMIXED_CLK(2)
#define CLK_APMIXED_MSDCPLL        APMIXED_CLK(3)
#define CLK_APMIXED_APLL1        APMIXED_CLK(4)
#define CLK_APMIXED_APLL2        APMIXED_CLK(5)
#define CLK_APMIXED_IPPLL        APMIXED_CLK(6)
#define CLK_APMIXED_DSPPLL        APMIXED_CLK(7)
#define CLK_APMIXED_TCONPLL        APMIXED_CLK(8)
#define CLK_APMIXED_NR_CLK        9

/* TOPCKGEN */

#define CLK_PVD_TOP          2
#define TOP_CLK(i)           CLK_ID(CLK_PVD_TOP, i)
#define CLK_TOP_CLK_NULL        TOP_CLK(0)
#define CLK_TOP_SYSPLL1_D2        TOP_CLK(1)
#define CLK_TOP_SYSPLL1_D4        TOP_CLK(2)
#define CLK_TOP_SYSPLL1_D8        TOP_CLK(3)
#define CLK_TOP_SYSPLL1_D16        TOP_CLK(4)
#define CLK_TOP_SYSPLL_D3        TOP_CLK(5)
#define CLK_TOP_SYSPLL2_D2        TOP_CLK(6)
#define CLK_TOP_SYSPLL2_D4        TOP_CLK(7)
#define CLK_TOP_SYSPLL2_D8        TOP_CLK(8)
#define CLK_TOP_SYSPLL_D5        TOP_CLK(9)
#define CLK_TOP_SYSPLL3_D4        TOP_CLK(10)
#define CLK_TOP_SYSPLL_D7        TOP_CLK(11)
#define CLK_TOP_SYSPLL4_D2        TOP_CLK(12)
#define CLK_TOP_UNIVPLL            TOP_CLK(13)
#define CLK_TOP_UNIVPLL_D2        TOP_CLK(14)
#define CLK_TOP_UNIVPLL1_D2        TOP_CLK(15)
#define CLK_TOP_UNIVPLL1_D4        TOP_CLK(16)
#define CLK_TOP_UNIVPLL1_D8        TOP_CLK(17)
#define CLK_TOP_UNIVPLL_D3        TOP_CLK(18)
#define CLK_TOP_UNIVPLL2_D2        TOP_CLK(19)
#define CLK_TOP_UNIVPLL2_D4        TOP_CLK(20)
#define CLK_TOP_UNIVPLL2_D8        TOP_CLK(21)
#define CLK_TOP_UNIVPLL_D5        TOP_CLK(22)
#define CLK_TOP_UNIVPLL3_D2        TOP_CLK(23)
#define CLK_TOP_UNIVPLL3_D4        TOP_CLK(24)
#define CLK_TOP_TCONPLL_D2        TOP_CLK(25)
#define CLK_TOP_TCONPLL_D4        TOP_CLK(26)
#define CLK_TOP_TCONPLL_D8        TOP_CLK(27)
#define CLK_TOP_TCONPLL_D16        TOP_CLK(28)
#define CLK_TOP_TCONPLL_D32        TOP_CLK(29)
#define CLK_TOP_TCONPLL_D64        TOP_CLK(30)
#define CLK_TOP_USB20_192M        TOP_CLK(31)
#define CLK_TOP_USB20_192M_D2        TOP_CLK(32)
#define CLK_TOP_USB20_192M_D4_T        TOP_CLK(33)
#define CLK_TOP_APLL1            TOP_CLK(34)
#define CLK_TOP_APLL1_D2        TOP_CLK(35)
#define CLK_TOP_APLL1_D3        TOP_CLK(36)
#define CLK_TOP_APLL1_D4        TOP_CLK(37)
#define CLK_TOP_APLL1_D8        TOP_CLK(38)
#define CLK_TOP_APLL1_D16        TOP_CLK(39)
#define CLK_TOP_APLL2            TOP_CLK(40)
#define CLK_TOP_APLL2_D2        TOP_CLK(41)
#define CLK_TOP_APLL2_D3        TOP_CLK(42)
#define CLK_TOP_APLL2_D4        TOP_CLK(43)
#define CLK_TOP_APLL2_D8        TOP_CLK(44)
#define CLK_TOP_APLL2_D16        TOP_CLK(45)
#define CLK_TOP_CLK26M            TOP_CLK(46)
#define CLK_TOP_SYS_26M_D2        TOP_CLK(47)
#define CLK_TOP_MSDCPLL            TOP_CLK(48)
#define CLK_TOP_MSDCPLL_D2        TOP_CLK(49)
#define CLK_TOP_DSPPLL            TOP_CLK(50)
#define CLK_TOP_DSPPLL_D2        TOP_CLK(51)
#define CLK_TOP_DSPPLL_D4        TOP_CLK(52)
#define CLK_TOP_DSPPLL_D8        TOP_CLK(53)
#define CLK_TOP_IPPLL            TOP_CLK(54)
#define CLK_TOP_IPPLL_D2        TOP_CLK(55)
#define CLK_TOP_NFI2X_CK_D2        TOP_CLK(56)
#define CLK_TOP_AXI_SEL            TOP_CLK(57)
#define CLK_TOP_MEM_SEL            TOP_CLK(58)
#define CLK_TOP_UART_SEL        TOP_CLK(59)
#define CLK_TOP_SPI_SEL            TOP_CLK(60)
#define CLK_TOP_SPIS_SEL        TOP_CLK(61)
#define CLK_TOP_MSDC50_0_HC_SEL        TOP_CLK(62)
#define CLK_TOP_MSDC2_2_HC_SEL        TOP_CLK(63)
#define CLK_TOP_MSDC50_0_SEL        TOP_CLK(64)
#define CLK_TOP_MSDC50_2_SEL        TOP_CLK(65)
#define CLK_TOP_MSDC30_1_SEL        TOP_CLK(66)
#define CLK_TOP_AUDIO_SEL        TOP_CLK(67)
#define CLK_TOP_AUD_INTBUS_SEL        TOP_CLK(68)
#define CLK_TOP_HAPLL1_SEL        TOP_CLK(69)
#define CLK_TOP_HAPLL2_SEL        TOP_CLK(70)
#define CLK_TOP_A2SYS_SEL        TOP_CLK(71)
#define CLK_TOP_A1SYS_SEL        TOP_CLK(72)
#define CLK_TOP_ASM_L_SEL        TOP_CLK(73)
#define CLK_TOP_ASM_M_SEL        TOP_CLK(74)
#define CLK_TOP_ASM_H_SEL        TOP_CLK(75)
#define CLK_TOP_AUD_SPDIF_SEL        TOP_CLK(76)
#define CLK_TOP_AUD_1_SEL        TOP_CLK(77)
#define CLK_TOP_AUD_2_SEL        TOP_CLK(78)
#define CLK_TOP_SSUSB_SYS_SEL        TOP_CLK(79)
#define CLK_TOP_SSUSB_XHCI_SEL        TOP_CLK(80)
#define CLK_TOP_SPM_SEL            TOP_CLK(81)
#define CLK_TOP_I2C_SEL            TOP_CLK(82)
#define CLK_TOP_PWM_SEL            TOP_CLK(83)
#define CLK_TOP_DSP_SEL            TOP_CLK(84)
#define CLK_TOP_NFI2X_SEL        TOP_CLK(85)
#define CLK_TOP_SPINFI_SEL        TOP_CLK(86)
#define CLK_TOP_ECC_SEL            TOP_CLK(87)
#define CLK_TOP_GCPU_SEL        TOP_CLK(88)
#define CLK_TOP_GCPU_CPM_SEL        TOP_CLK(89)
#define CLK_TOP_MBIST_DIAG_SEL        TOP_CLK(90)
#define CLK_TOP_IP0_NNA_SEL        TOP_CLK(91)
#define CLK_TOP_IP1_NNA_SEL        TOP_CLK(92)
#define CLK_TOP_IP2_WFST_SEL        TOP_CLK(93)
#define CLK_TOP_SFLASH_SEL        TOP_CLK(94)
#define CLK_TOP_SRAM_SEL        TOP_CLK(95)
#define CLK_TOP_MM_SEL            TOP_CLK(96)
#define CLK_TOP_DPI0_SEL        TOP_CLK(97)
#define CLK_TOP_DBG_ATCLK_SEL        TOP_CLK(98)
#define CLK_TOP_OCC_104M_SEL        TOP_CLK(99)
#define CLK_TOP_OCC_68M_SEL        TOP_CLK(100)
#define CLK_TOP_OCC_182M_SEL        TOP_CLK(101)
#define CLK_TOP_APLL_FI2SI1_SEL        TOP_CLK(102)
#define CLK_TOP_APLL_FTDMIN_SEL        TOP_CLK(103)
#define CLK_TOP_APLL_FI2SO1_SEL        TOP_CLK(104)
#define CLK_TOP_APLL12_CK_DIV7        TOP_CLK(105)
#define CLK_TOP_APLL12_CK_DIV8        TOP_CLK(106)
#define CLK_TOP_APLL12_CK_DIV9        TOP_CLK(107)
#define CLK_TOP_I2SI1_MCK        TOP_CLK(108)
#define CLK_TOP_TDMIN_MCK        TOP_CLK(109)
#define CLK_TOP_I2SO1_MCK        TOP_CLK(110)
#define CLK_TOP_USB20_48M_EN        TOP_CLK(111)
#define CLK_TOP_UNIVPLL_48M_EN        TOP_CLK(112)
#define CLK_TOP_SSUSB_TOP_CK_EN        TOP_CLK(113)
#define CLK_TOP_SSUSB_PHY_CK_EN        TOP_CLK(114)
#define CLK_TOP_CONN_32K        TOP_CLK(115)
#define CLK_TOP_CONN_26M        TOP_CLK(116)
#define CLK_TOP_DSP_32K            TOP_CLK(117)
#define CLK_TOP_DSP_26M            TOP_CLK(118)
#define CLK_TOP_NR_CLK            119

/* INFRASYS */

#define CLK_PVD_INFRA          3
#define INFRA_CLK(i)           CLK_ID(CLK_PVD_INFRA, i)
#define CLK_INFRA_ICUSB            INFRA_CLK(0)
#define CLK_INFRA_GCE            INFRA_CLK(1)
#define CLK_INFRA_THERM            INFRA_CLK(2)
#define CLK_INFRA_PWM_HCLK        INFRA_CLK(3)
#define CLK_INFRA_PWM1            INFRA_CLK(4)
#define CLK_INFRA_PWM2            INFRA_CLK(5)
#define CLK_INFRA_PWM3            INFRA_CLK(6)
#define CLK_INFRA_PWM4            INFRA_CLK(7)
#define CLK_INFRA_PWM5            INFRA_CLK(8)
#define CLK_INFRA_PWM            INFRA_CLK(9)
#define CLK_INFRA_UART0            INFRA_CLK(10)
#define CLK_INFRA_UART1            INFRA_CLK(11)
#define CLK_INFRA_UART2            INFRA_CLK(12)
#define CLK_INFRA_DSP_UART        INFRA_CLK(13)
#define CLK_INFRA_GCE_26M        INFRA_CLK(14)
#define CLK_INFRA_CQDMA_FPC        INFRA_CLK(15)
#define CLK_INFRA_BTIF            INFRA_CLK(16)
#define CLK_INFRA_SPI            INFRA_CLK(17)
#define CLK_INFRA_MSDC0            INFRA_CLK(18)
#define CLK_INFRA_MSDC1            INFRA_CLK(19)
#define CLK_INFRA_DVFSRC        INFRA_CLK(20)
#define CLK_INFRA_GCPU            INFRA_CLK(21)
#define CLK_INFRA_TRNG            INFRA_CLK(22)
#define CLK_INFRA_AUXADC        INFRA_CLK(23)
#define CLK_INFRA_AUXADC_MD        INFRA_CLK(24)
#define CLK_INFRA_AP_DMA        INFRA_CLK(25)
#define CLK_INFRA_DEBUGSYS        INFRA_CLK(26)
#define CLK_INFRA_AUDIO            INFRA_CLK(27)
#define CLK_INFRA_FLASHIF        INFRA_CLK(28)
#define CLK_INFRA_PWM_FB6        INFRA_CLK(29)
#define CLK_INFRA_PWM_FB7        INFRA_CLK(30)
#define CLK_INFRA_AUD_ASRC        INFRA_CLK(31)
#define CLK_INFRA_AUD_26M        INFRA_CLK(32)
#define CLK_INFRA_SPIS            INFRA_CLK(33)
#define CLK_INFRA_CQ_DMA        INFRA_CLK(34)
#define CLK_INFRA_AP_MSDC0        INFRA_CLK(35)
#define CLK_INFRA_MD_MSDC0        INFRA_CLK(36)
#define CLK_INFRA_MSDC0_SRC        INFRA_CLK(37)
#define CLK_INFRA_MSDC1_SRC        INFRA_CLK(38)
#define CLK_INFRA_IRRX_26M        INFRA_CLK(39)
#define CLK_INFRA_IRRX_32K        INFRA_CLK(40)
#define CLK_INFRA_I2C0_AXI        INFRA_CLK(41)
#define CLK_INFRA_I2C1_AXI        INFRA_CLK(42)
#define CLK_INFRA_I2C2_AXI        INFRA_CLK(43)
#define CLK_INFRA_NFI            INFRA_CLK(44)
#define CLK_INFRA_NFIECC        INFRA_CLK(45)
#define CLK_INFRA_NFI_HCLK        INFRA_CLK(46)
#define CLK_INFRA_SUSB_133        INFRA_CLK(47)
#define CLK_INFRA_USB_SYS        INFRA_CLK(48)
#define CLK_INFRA_USB_XHCI        INFRA_CLK(49)
#define CLK_INFRA_DSP_AXI        INFRA_CLK(50)
#define CLK_INFRA_NR_CLK        51

/* IPSYS */

#define CLK_PVD_IP          4
#define IP_CLK(i)           CLK_ID(CLK_PVD_IP, i)
#define CLK_IP_NNA0_PWR_GATE        IP_CLK(0)
#define CLK_IP_NNA1_PWR_GATE        IP_CLK(1)
#define CLK_IP_WFST_PWR_GATE        IP_CLK(2)
#define CLK_IP_HD_FAXI_CK        IP_CLK(3)
#define CLK_IP_TEST_26M            IP_CLK(4)
#define CLK_IP_EMI_CK_GATE        IP_CLK(5)
#define CLK_IP_SRAM_OCC_GATE        IP_CLK(6)
#define CLK_IP_NNA0_OCC_GATE        IP_CLK(7)
#define CLK_IP_NNA1_OCC_GATE        IP_CLK(8)
#define CLK_IP_WFST_OCC_GATE        IP_CLK(9)
#define CLK_IP_NR_CLK            10

/* MMSYS */

#define CLK_PVD_MM          5
#define MM_CLK(i)           CLK_ID(CLK_PVD_MM, i)
#define CLK_MM_PIPELINE0        MM_CLK(0)
#define CLK_MM_PIPELINE1        MM_CLK(1)
#define CLK_MM_PIPELINE2        MM_CLK(2)
#define CLK_MM_PIPELINE3        MM_CLK(3)
#define CLK_MM_PIPELINE4        MM_CLK(4)
#define CLK_MM_PIPELINE5        MM_CLK(5)
#define CLK_MM_PIPELINE7        MM_CLK(6)
#define CLK_MM_DPI0_DPI_TMP0        MM_CLK(7)
#define CLK_MM_DPI0_DPI_TMP1        MM_CLK(8)
#define CLK_MM_DISP_FAKE        MM_CLK(9)
#define CLK_MM_SMI_COMMON        MM_CLK(10)
#define CLK_MM_SMI_LARB0        MM_CLK(11)
#define CLK_MM_SMI_COMM0        MM_CLK(12)
#define CLK_MM_SMI_COMM1        MM_CLK(13)
#define CLK_MM_NR_CLK            14

/* IMGSYS */

#define CLK_PVD_IMG          6
#define IMG_CLK(i)           CLK_ID(CLK_PVD_IMG, i)
#define CLK_IMG_MDP_RDMA0        IMG_CLK(0)
#define CLK_IMG_MDP_RSZ0        IMG_CLK(1)
#define CLK_IMG_MDP_TDSHP0        IMG_CLK(2)
#define CLK_IMG_MDP_WROT0        IMG_CLK(3)
#define CLK_IMG_DISP_OVL0_21        IMG_CLK(4)
#define CLK_IMG_DISP_WDMA0        IMG_CLK(5)
#define CLK_IMG_DISP_GAMMA0        IMG_CLK(6)
#define CLK_IMG_DISP_DITHER0        IMG_CLK(7)
#define CLK_IMG_FAKE            IMG_CLK(8)
#define CLK_IMG_SMI_LARB1        IMG_CLK(9)
#define CLK_IMG_JPGDEC            IMG_CLK(10)
#define CLK_IMG_PNGDEC            IMG_CLK(11)
#define CLK_IMG_IMGRZ            IMG_CLK(12)
#define CLK_IMG_NR_CLK            13

#endif /* _CLK_MT8512_H */

