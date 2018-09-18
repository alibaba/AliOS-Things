/*
 * Copyright (c) 2012, Freescale Semiconductor, Inc.
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

// File: iomux_define.h

#ifndef _IOMUX_DEFINE_H_
#define _IOMUX_DEFINE_H_

// IOMUXC_SW_MUX_CTL_PAD_*
// SION
#define SION_DISABLED 0x0
#define SION_ENABLED  0x1
// MUX_MODE
#define ALT0 0x0
#define ALT1 0x1
#define ALT2 0x2
#define ALT3 0x3
#define ALT4 0x4
#define ALT5 0x5
#define ALT6 0x6
#define ALT7 0x7

// IOMUXC_SW_PAD_CTL_PAD_*
// IOMUXC_SW_PAD_CTL_GRP_*
// LVE
#define LVE_DISABLED 0x0
#define LVE_ENABLED  0x1
// HYS
#define HYS_DISABLED 0x0
#define HYS_ENABLED  0x1
// PUS
#define PUS_100KOHM_PD 0x0
#define PUS_47KOHM_PU  0x1
#define PUS_100KOHM_PU 0x2
#define PUS_22KOHM_PU  0x3
// PUE
#define PUE_KEEP 0x0
#define PUE_PULL 0x1
// PKE
#define PKE_DISABLED 0x0
#define PKE_ENABLED  0x1
// ODE
#define ODE_DISABLED 0x0
#define ODE_ENABLED  0x1
// SPEED
#define SPD_TBD    0x0
#define SPD_50MHZ  0x1
#define SPD_100MHZ 0x2
#define SPD_200MHZ 0x3
// DSE
#define DSE_DISABLED 0x0
#define DSE_240OHM   0x1
#define DSE_120OHM   0x2
#define DSE_80OHM    0x3
#define DSE_60OHM    0x4
#define DSE_48OHM    0x5
#define DSE_40OHM    0x6
#define DSE_34OHM    0x7
// SRE
#define SRE_SLOW 0x0
#define SRE_FAST 0x1
// ODT
#define ODT_OFF    0x0
#define ODT_120OHM 0x1
#define ODT_60OHM  0x2
#define ODT_40OHM  0x3
#define ODT_30OHM  0x4
#define ODT_RES5   0x5
#define ODT_20OHM  0x6
#define ODT_RES7   0x7
// DDR_INPUT
#define DDR_INPUT_CMOS 0x0
#define DDR_INPUT_DIFF 0x1
// DDR_SEL
#define DDR_SEL_LPDDR1_DDR3_DDR2_ODT 0x0
#define DDR_SEL_DDR2                 0x1
#define DDR_SEL_LPDDR2               0x2
#define DDR_SEL_RES0                 0x3
// DO_TRIM
#define DO_TRIM_RES0 0x0
#define DO_TRIM_RES1 0x1

// IOMUXC_ANALOG_USB_OTG_ID_SELECT_INPUT
// DAISY
#define SEL_EPDC_PWRCOM_ALT4 0x0
#define SEL_FEC_RXD0_ALT2    0x1
#define SEL_LCD_DAT1_ALT2    0x2
#define SEL_REF_CLK_32K_ALT3 0x3
#define SEL_SD3_DAT0_ALT4    0x4

// IOMUXC_ANALOG_USB_UH1_ID_SELECT_INPUT
// DAISY
#define SEL_EPDC_PWRINT_ALT4 0x0
#define SEL_LCD_DAT0_ALT2    0x1
#define SEL_REF_CLK_24M_ALT3 0x2
#define SEL_SD3_CMD_ALT4     0x3

// IOMUXC_AUDMUX_P4_INPUT_DA_AMX_SELECT_INPUT
// DAISY
#define SEL_ECSPI1_SS0_ALT1 0x0
#define SEL_LCD_DAT3_ALT4   0x1
#define SEL_SD2_DAT0_ALT1   0x2

// IOMUXC_AUDMUX_P4_INPUT_DB_AMX_SELECT_INPUT
// DAISY
#define SEL_ECSPI1_SCLK_ALT1 0x0
#define SEL_LCD_DAT6_ALT4    0x1
#define SEL_SD2_DAT3_ALT1    0x2

// IOMUXC_AUDMUX_P4_INPUT_RXCLK_AMX_SELECT_INPUT
// DAISY
#define SEL_I2C2_SDA_ALT1 0x0
#define SEL_LCD_DAT2_ALT4 0x1
#define SEL_SD2_CMD_ALT1  0x2

// IOMUXC_AUDMUX_P4_INPUT_RXFS_AMX_SELECT_INPUT
// DAISY
#define SEL_I2C2_SCL_ALT1 0x0
#define SEL_LCD_DAT1_ALT4 0x1
#define SEL_SD2_CLK_ALT1  0x2

// IOMUXC_AUDMUX_P4_INPUT_TXCLK_AMX_SELECT_INPUT
// DAISY
#define SEL_ECSPI1_MOSI_ALT1 0x0
#define SEL_LCD_DAT4_ALT4    0x1
#define SEL_SD2_DAT1_ALT1    0x2

// IOMUXC_AUDMUX_P4_INPUT_TXFS_AMX_SELECT_INPUT
// DAISY
#define SEL_ECSPI1_MISO_ALT1 0x0
#define SEL_LCD_DAT5_ALT4    0x1
#define SEL_SD2_DAT2_ALT1    0x2

// IOMUXC_AUDMUX_P5_INPUT_DA_AMX_SELECT_INPUT
// DAISY
#define SEL_EPDC_VCOM1_ALT1 0x0
#define SEL_SD3_DAT0_ALT1   0x1

// IOMUXC_AUDMUX_P5_INPUT_DB_AMX_SELECT_INPUT
// DAISY
#define SEL_EPDC_PWRCTRL2_ALT1 0x0
#define SEL_SD3_DAT3_ALT1      0x1

// IOMUXC_AUDMUX_P5_INPUT_RXCLK_AMX_SELECT_INPUT
// DAISY
#define SEL_EPDC_PWRCTRL0_ALT1 0x0
#define SEL_SD3_CMD_ALT1       0x1

// IOMUXC_AUDMUX_P5_INPUT_RXFS_AMX_SELECT_INPUT
// DAISY
#define SEL_EPDC_VCOM0_ALT1 0x0
#define SEL_SD3_CLK_ALT1    0x1

// IOMUXC_AUDMUX_P5_INPUT_TXCLK_AMX_SELECT_INPUT
// DAISY
#define SEL_EPDC_PWRCTRL3_ALT1 0x0
#define SEL_SD3_DAT1_ALT1      0x1

// IOMUXC_AUDMUX_P5_INPUT_TXFS_AMX_SELECT_INPUT
// DAISY
#define SEL_EPDC_PWRCTRL1_ALT1 0x0
#define SEL_SD3_DAT2_ALT1      0x1

// IOMUXC_AUDMUX_P6_INPUT_DA_AMX_SELECT_INPUT
// DAISY
#define SEL_FEC_RX_ER_ALT2 0x0
#define SEL_KEY_COL4_ALT1  0x1

// IOMUXC_AUDMUX_P6_INPUT_DB_AMX_SELECT_INPUT
// DAISY
#define SEL_FEC_TXD0_ALT2 0x0
#define SEL_KEY_ROW5_ALT1 0x1

// IOMUXC_AUDMUX_P6_INPUT_RXCLK_AMX_SELECT_INPUT
// DAISY
#define SEL_FEC_TX_CLK_ALT2 0x0
#define SEL_KEY_ROW3_ALT1   0x1

// IOMUXC_AUDMUX_P6_INPUT_RXFS_AMX_SELECT_INPUT
// DAISY
#define SEL_FEC_MDIO_ALT2 0x0
#define SEL_KEY_COL3_ALT1 0x1

// IOMUXC_AUDMUX_P6_INPUT_TXCLK_AMX_SELECT_INPUT
// DAISY
#define SEL_FEC_CRS_DV_ALT2 0x0
#define SEL_KEY_ROW4_ALT1   0x1

// IOMUXC_AUDMUX_P6_INPUT_TXFS_AMX_SELECT_INPUT
// DAISY
#define SEL_FEC_RXD1_ALT2 0x0
#define SEL_KEY_COL5_ALT1 0x1

// IOMUXC_CCM_PMIC_VFUNCIONAL_READY_SELECT_INPUT
// DAISY
#define SEL_FEC_REF_CLK_ALT4 0x0
#define SEL_LCD_RESET_ALT6   0x1
#define SEL_REF_CLK_24M_ALT4 0x2
#define SEL_SD1_DAT7_ALT3    0x3

// IOMUXC_CSI_IPP_CSI_D_0_SELECT_INPUT
// DAISY
#define SEL_EPDC_D0_ALT3   0x0
#define SEL_LCD_DAT17_ALT2 0x1
#define SEL_SD2_CLK_ALT3   0x2

// IOMUXC_CSI_IPP_CSI_D_1_SELECT_INPUT
// DAISY
#define SEL_EPDC_D1_ALT3   0x0
#define SEL_LCD_DAT16_ALT2 0x1
#define SEL_SD2_CMD_ALT3   0x2

// IOMUXC_CSI_IPP_CSI_D_2_SELECT_INPUT
// DAISY
#define SEL_EPDC_D2_ALT3   0x0
#define SEL_LCD_DAT15_ALT2 0x1
#define SEL_SD2_DAT0_ALT3  0x2

// IOMUXC_CSI_IPP_CSI_D_3_SELECT_INPUT
// DAISY
#define SEL_EPDC_D3_ALT3   0x0
#define SEL_LCD_DAT14_ALT2 0x1
#define SEL_SD2_DAT1_ALT3  0x2

// IOMUXC_CSI_IPP_CSI_D_4_SELECT_INPUT
// DAISY
#define SEL_EPDC_D4_ALT3   0x0
#define SEL_LCD_DAT13_ALT2 0x1
#define SEL_SD2_DAT2_ALT3  0x2

// IOMUXC_CSI_IPP_CSI_D_5_SELECT_INPUT
// DAISY
#define SEL_EPDC_D5_ALT3   0x0
#define SEL_LCD_DAT12_ALT2 0x1
#define SEL_SD2_DAT3_ALT3  0x2

// IOMUXC_CSI_IPP_CSI_D_6_SELECT_INPUT
// DAISY
#define SEL_EPDC_D6_ALT3   0x0
#define SEL_LCD_DAT11_ALT2 0x1
#define SEL_SD2_DAT4_ALT3  0x2

// IOMUXC_CSI_IPP_CSI_D_7_SELECT_INPUT
// DAISY
#define SEL_EPDC_D7_ALT3   0x0
#define SEL_LCD_DAT10_ALT2 0x1
#define SEL_SD2_DAT5_ALT3  0x2

// IOMUXC_CSI_IPP_CSI_D_8_SELECT_INPUT
// DAISY
#define SEL_EPDC_SDCLK_ALT3 0x0
#define SEL_LCD_DAT9_ALT2   0x1
#define SEL_SD2_DAT6_ALT3   0x2

// IOMUXC_CSI_IPP_CSI_D_9_SELECT_INPUT
// DAISY
#define SEL_EPDC_SDLE_ALT3 0x0
#define SEL_LCD_DAT8_ALT2  0x1
#define SEL_SD2_DAT7_ALT3  0x2

// IOMUXC_CSI_IPP_CSI_D_10_SELECT_INPUT
// DAISY
#define SEL_EPDC_SDOE_ALT3 0x0
#define SEL_LCD_DAT23_ALT2 0x1
#define SEL_SD3_CLK_ALT3   0x2

// IOMUXC_CSI_IPP_CSI_D_11_SELECT_INPUT
// DAISY
#define SEL_EPDC_SDSHR_ALT3 0x0
#define SEL_LCD_DAT22_ALT2  0x1
#define SEL_SD3_CMD_ALT3    0x2

// IOMUXC_CSI_IPP_CSI_D_12_SELECT_INPUT
// DAISY
#define SEL_LCD_DAT21_ALT2 0x0
#define SEL_SD3_DAT0_ALT3  0x1

// IOMUXC_CSI_IPP_CSI_D_13_SELECT_INPUT
// DAISY
#define SEL_LCD_DAT20_ALT2 0x0
#define SEL_SD3_DAT1_ALT3  0x1

// IOMUXC_CSI_IPP_CSI_D_14_SELECT_INPUT
// DAISY
#define SEL_LCD_DAT19_ALT2 0x0
#define SEL_SD3_DAT2_ALT3  0x1

// IOMUXC_CSI_IPP_CSI_D_15_SELECT_INPUT
// DAISY
#define SEL_LCD_DAT18_ALT2 0x0
#define SEL_SD3_DAT3_ALT3  0x1

// IOMUXC_CSI_IPP_CSI_HSYNC_SELECT_INPUT
// DAISY
#define SEL_ECSPI2_MOSI_ALT3 0x0
#define SEL_EPDC_GDOE_ALT3   0x1
#define SEL_LCD_DAT5_ALT2    0x2

// IOMUXC_CSI_IPP_CSI_PIXCLK_SELECT_INPUT
// DAISY
#define SEL_ECSPI2_SCLK_ALT3 0x0
#define SEL_EPDC_GDCLK_ALT3  0x1
#define SEL_LCD_DAT6_ALT2    0x2

// IOMUXC_CSI_IPP_CSI_VSYNC_SELECT_INPUT
// DAISY
#define SEL_ECSPI2_SS0_ALT3 0x0
#define SEL_EPDC_GDSP_ALT3  0x1
#define SEL_LCD_DAT4_ALT2   0x2

// IOMUXC_ECSPI1_IPP_CSPI_CLK_IN_SELECT_INPUT
// DAISY
#define SEL_ECSPI1_SCLK_ALT0 0x0
#define SEL_LCD_DAT3_ALT1    0x1

// IOMUXC_ECSPI1_IPP_IND_DATAREADY_B_SELECT_INPUT
// DAISY
#define SEL_I2C2_SCL_ALT6 0x0
#define SEL_LCD_DAT7_ALT1 0x1

// IOMUXC_ECSPI1_IPP_IND_MISO_SELECT_INPUT
// DAISY
#define SEL_ECSPI1_MISO_ALT0 0x0
#define SEL_LCD_DAT1_ALT1    0x1

// IOMUXC_ECSPI1_IPP_IND_MOSI_SELECT_INPUT
// DAISY
#define SEL_ECSPI1_MOSI_ALT0 0x0
#define SEL_LCD_DAT0_ALT1    0x1

// IOMUXC_ECSPI1_IPP_IND_SS_B_0_SELECT_INPUT
// DAISY
#define SEL_ECSPI1_SS0_ALT0 0x0
#define SEL_LCD_DAT2_ALT1   0x1

// IOMUXC_ECSPI1_IPP_IND_SS_B_1_SELECT_INPUT
// DAISY
#define SEL_I2C1_SCL_ALT6 0x0
#define SEL_LCD_DAT4_ALT1 0x1

// IOMUXC_ECSPI1_IPP_IND_SS_B_2_SELECT_INPUT
// DAISY
#define SEL_I2C1_SDA_ALT6 0x0
#define SEL_LCD_DAT5_ALT1 0x1

// IOMUXC_ECSPI1_IPP_IND_SS_B_3_SELECT_INPUT
// DAISY
#define SEL_ECSPI2_SS0_ALT1 0x0
#define SEL_LCD_DAT6_ALT1   0x1

// IOMUXC_ECSPI2_IPP_CSPI_CLK_IN_SELECT_INPUT
// DAISY
#define SEL_ECSPI2_SCLK_ALT0 0x0
#define SEL_EPDC_SDSHR_ALT1  0x1
#define SEL_LCD_DAT8_ALT4    0x2

// IOMUXC_ECSPI2_IPP_IND_MISO_SELECT_INPUT
// DAISY
#define SEL_ECSPI2_MISO_ALT0 0x0
#define SEL_EPDC_SDLE_ALT1   0x1
#define SEL_LCD_DAT10_ALT4   0x2

// IOMUXC_ECSPI2_IPP_IND_MOSI_SELECT_INPUT
// DAISY
#define SEL_ECSPI2_MOSI_ALT0 0x0
#define SEL_EPDC_SDCLK_ALT1  0x1
#define SEL_LCD_DAT9_ALT4    0x2

// IOMUXC_ECSPI2_IPP_IND_SS_B_0_SELECT_INPUT
// DAISY
#define SEL_ECSPI2_SS0_ALT0 0x0
#define SEL_EPDC_SDOE_ALT1  0x1

// IOMUXC_ECSPI2_IPP_IND_SS_B_1_SELECT_INPUT
// DAISY
#define SEL_EPDC_SDCE0_ALT1 0x0
#define SEL_LCD_DAT11_ALT4  0x1

// IOMUXC_ECSPI3_IPP_CSPI_CLK_IN_SELECT_INPUT
// DAISY
#define SEL_AUD_TXD_ALT1  0x0
#define SEL_EPDC_D11_ALT1 0x1
#define SEL_SD2_CLK_ALT2  0x2

// IOMUXC_ECSPI3_IPP_IND_DATAREADY_B_SELECT_INPUT
// DAISY
#define SEL_AUD_MCLK_ALT2 0x0
#define SEL_EPDC_D15_ALT6 0x1

// IOMUXC_ECSPI3_IPP_IND_MISO_SELECT_INPUT
// DAISY
#define SEL_AUD_TXC_ALT1  0x0
#define SEL_EPDC_D9_ALT1  0x1
#define SEL_SD2_DAT1_ALT2 0x2

// IOMUXC_ECSPI3_IPP_IND_MOSI_SELECT_INPUT
// DAISY
#define SEL_AUD_RXD_ALT1  0x0
#define SEL_EPDC_D8_ALT1  0x1
#define SEL_SD2_DAT0_ALT2 0x2

// IOMUXC_ECSPI3_IPP_IND_SS_B_0_SELECT_INPUT
// DAISY
#define SEL_AUD_RXFS_ALT6 0x0
#define SEL_EPDC_D10_ALT1 0x1
#define SEL_SD2_CMD_ALT2  0x2

// IOMUXC_ECSPI3_IPP_IND_SS_B_1_SELECT_INPUT
// DAISY
#define SEL_AUD_RXC_ALT6  0x0
#define SEL_EPDC_D12_ALT6 0x1

// IOMUXC_ECSPI3_IPP_IND_SS_B_2_SELECT_INPUT
// DAISY
#define SEL_EPDC_D13_ALT6 0x0
#define SEL_I2C1_SCL_ALT2 0x1

// IOMUXC_ECSPI3_IPP_IND_SS_B_3_SELECT_INPUT
// DAISY
#define SEL_EPDC_D14_ALT6 0x0
#define SEL_I2C1_SDA_ALT2 0x1

// IOMUXC_ECSPI4_IPP_CSPI_CLK_IN_SELECT_INPUT
// DAISY
#define SEL_EPDC_D3_ALT1    0x0
#define SEL_FEC_TX_CLK_ALT3 0x1
#define SEL_KEY_ROW2_ALT1   0x2

// IOMUXC_ECSPI4_IPP_IND_MISO_SELECT_INPUT
// DAISY
#define SEL_EPDC_D1_ALT1    0x0
#define SEL_FEC_CRS_DV_ALT3 0x1
#define SEL_KEY_ROW1_ALT1   0x2

// IOMUXC_ECSPI4_IPP_IND_MOSI_SELECT_INPUT
// DAISY
#define SEL_EPDC_D0_ALT1   0x0
#define SEL_FEC_RX_ER_ALT3 0x1
#define SEL_KEY_COL1_ALT1  0x2

// IOMUXC_ECSPI4_IPP_IND_SS_B_0_SELECT_INPUT
// DAISY
#define SEL_EPDC_D2_ALT1  0x0
#define SEL_FEC_MDIO_ALT3 0x1
#define SEL_KEY_COL2_ALT1 0x2

// IOMUXC_ECSPI4_IPP_IND_SS_B_1_SELECT_INPUT
// DAISY
#define SEL_EPDC_D4_ALT1  0x0
#define SEL_FEC_RXD1_ALT3 0x1

// IOMUXC_ECSPI4_IPP_IND_SS_B_2_SELECT_INPUT
// DAISY
#define SEL_EPDC_D5_ALT1  0x0
#define SEL_FEC_TXD0_ALT3 0x1

// IOMUXC_EPDC_IPP_EPDC_PWRIRQ_SELECT_INPUT
// DAISY
#define SEL_EPDC_D13_ALT2    0x0
#define SEL_EPDC_PWRINT_ALT0 0x1

// IOMUXC_EPDC_IPP_EPDC_PWRSTAT_SELECT_INPUT
// DAISY
#define SEL_EPDC_D14_ALT2     0x0
#define SEL_EPDC_PWRSTAT_ALT0 0x1

// IOMUXC_FEC_FEC_COL_SELECT_INPUT
// DAISY
#define SEL_FEC_RXD1_ALT6  0x0
#define SEL_SD2_DAT2_ALT2  0x1
#define SEL_UART1_RXD_ALT3 0x2

// IOMUXC_FEC_FEC_MDI_SELECT_INPUT
// DAISY
#define SEL_AUD_RXFS_ALT3 0x0
#define SEL_FEC_MDIO_ALT0 0x1
#define SEL_SD1_CLK_ALT1  0x2

// IOMUXC_FEC_FEC_RDATA_0_SELECT_INPUT
// DAISY
#define SEL_FEC_RXD0_ALT0 0x0
#define SEL_I2C1_SCL_ALT3 0x1
#define SEL_SD1_DAT5_ALT1 0x2

// IOMUXC_FEC_FEC_RDATA_1_SELECT_INPUT
// DAISY
#define SEL_AUD_TXFS_ALT3 0x0
#define SEL_FEC_RXD1_ALT0 0x1
#define SEL_SD1_DAT2_ALT1 0x2

// IOMUXC_FEC_FEC_RX_CLK_SELECT_INPUT
// DAISY
#define SEL_FEC_TXD1_ALT6  0x0
#define SEL_SD2_DAT3_ALT2  0x1
#define SEL_UART1_TXD_ALT3 0x2

// IOMUXC_FEC_FEC_RX_DV_SELECT_INPUT
// DAISY
#define SEL_AUD_TXC_ALT3    0x0
#define SEL_FEC_CRS_DV_ALT0 0x1
#define SEL_SD1_DAT1_ALT1   0x2

// IOMUXC_FEC_FEC_RX_ER_SELECT_INPUT
// DAISY
#define SEL_AUD_RXD_ALT3   0x0
#define SEL_FEC_RX_ER_ALT0 0x1
#define SEL_SD1_DAT0_ALT1  0x2

// IOMUXC_FEC_FEC_TX_CLK_SELECT_INPUT
// DAISY
#define SEL_AUD_RXC_ALT3    0x0
#define SEL_FEC_TX_CLK_ALT0 0x1
#define SEL_SD1_CMD_ALT1    0x2

// IOMUXC_GPT_IPP_IND_CAPIN1_SELECT_INPUT
// DAISY
#define SEL_FEC_MDIO_ALT4  0x0
#define SEL_LCD_DAT18_ALT4 0x1

// IOMUXC_GPT_IPP_IND_CAPIN2_SELECT_INPUT
// DAISY
#define SEL_FEC_TX_CLK_ALT4 0x0
#define SEL_LCD_DAT19_ALT4  0x1

// IOMUXC_GPT_IPP_IND_CLKIN_SELECT_INPUT
// DAISY
#define SEL_FEC_TXD0_ALT4  0x0
#define SEL_LCD_DAT23_ALT4 0x1

// IOMUXC_I2C1_IPP_SCL_IN_SELECT_INPUT
// DAISY
#define SEL_AUD_RXFS_ALT1 0x0
#define SEL_HSIC_DAT_ALT1 0x1
#define SEL_I2C1_SCL_ALT0 0x2

// IOMUXC_I2C1_IPP_SDA_IN_SELECT_INPUT
// DAISY
#define SEL_AUD_RXC_ALT1     0x0
#define SEL_HSIC_STROBE_ALT1 0x1
#define SEL_I2C1_SDA_ALT0    0x2

// IOMUXC_I2C2_IPP_SCL_IN_SELECT_INPUT
// DAISY
#define SEL_EPDC_SDCLK_ALT2 0x0
#define SEL_I2C2_SCL_ALT0   0x1
#define SEL_KEY_COL0_ALT1   0x2
#define SEL_LCD_DAT16_ALT4  0x3

// IOMUXC_I2C2_IPP_SDA_IN_SELECT_INPUT
// DAISY
#define SEL_EPDC_SDLE_ALT2 0x0
#define SEL_I2C2_SDA_ALT0  0x1
#define SEL_KEY_ROW0_ALT1  0x2
#define SEL_LCD_DAT17_ALT4 0x3

// IOMUXC_I2C3_IPP_SCL_IN_SELECT_INPUT
// DAISY
#define SEL_AUD_RXFS_ALT4    0x0
#define SEL_EPDC_SDCE2_ALT1  0x1
#define SEL_REF_CLK_24M_ALT1 0x2

// IOMUXC_I2C3_IPP_SDA_IN_SELECT_INPUT
// DAISY
#define SEL_AUD_RXC_ALT4     0x0
#define SEL_EPDC_SDCE3_ALT1  0x1
#define SEL_REF_CLK_32K_ALT1 0x2

// IOMUXC_KPP_IPP_IND_COL_0_SELECT_INPUT
// DAISY
#define SEL_KEY_COL0_ALT0 0x0
#define SEL_LCD_DAT8_ALT1 0x1
#define SEL_SD1_CLK_ALT2  0x2

// IOMUXC_KPP_IPP_IND_COL_1_SELECT_INPUT
// DAISY
#define SEL_KEY_COL1_ALT0  0x0
#define SEL_LCD_DAT10_ALT1 0x1
#define SEL_SD1_DAT0_ALT2  0x2

// IOMUXC_KPP_IPP_IND_COL_2_SELECT_INPUT
// DAISY
#define SEL_KEY_COL2_ALT0  0x0
#define SEL_LCD_DAT12_ALT1 0x1
#define SEL_SD1_DAT2_ALT2  0x2

// IOMUXC_KPP_IPP_IND_COL_3_SELECT_INPUT
// DAISY
#define SEL_KEY_COL3_ALT0  0x0
#define SEL_LCD_DAT14_ALT1 0x1
#define SEL_SD1_DAT4_ALT2  0x2

// IOMUXC_KPP_IPP_IND_COL_4_SELECT_INPUT
// DAISY
#define SEL_KEY_COL4_ALT0  0x0
#define SEL_LCD_DAT16_ALT1 0x1
#define SEL_SD1_DAT6_ALT2  0x2

// IOMUXC_KPP_IPP_IND_COL_5_SELECT_INPUT
// DAISY
#define SEL_KEY_COL5_ALT0  0x0
#define SEL_LCD_DAT18_ALT1 0x1
#define SEL_SD3_CLK_ALT2   0x2

// IOMUXC_KPP_IPP_IND_COL_6_SELECT_INPUT
// DAISY
#define SEL_KEY_COL6_ALT0  0x0
#define SEL_LCD_DAT20_ALT1 0x1
#define SEL_SD3_DAT0_ALT2  0x2

// IOMUXC_KPP_IPP_IND_COL_7_SELECT_INPUT
// DAISY
#define SEL_KEY_COL7_ALT0  0x0
#define SEL_LCD_DAT22_ALT1 0x1
#define SEL_SD3_DAT2_ALT2  0x2

// IOMUXC_KPP_IPP_IND_ROW_0_SELECT_INPUT
// DAISY
#define SEL_KEY_ROW0_ALT0 0x0
#define SEL_LCD_DAT9_ALT1 0x1
#define SEL_SD1_CMD_ALT2  0x2

// IOMUXC_KPP_IPP_IND_ROW_1_SELECT_INPUT
// DAISY
#define SEL_KEY_ROW1_ALT0  0x0
#define SEL_LCD_DAT11_ALT1 0x1
#define SEL_SD1_DAT1_ALT2  0x2

// IOMUXC_KPP_IPP_IND_ROW_2_SELECT_INPUT
// DAISY
#define SEL_KEY_ROW2_ALT0  0x0
#define SEL_LCD_DAT13_ALT1 0x1
#define SEL_SD1_DAT3_ALT2  0x2

// IOMUXC_KPP_IPP_IND_ROW_3_SELECT_INPUT
// DAISY
#define SEL_KEY_ROW3_ALT0  0x0
#define SEL_LCD_DAT15_ALT1 0x1
#define SEL_SD1_DAT5_ALT2  0x2

// IOMUXC_KPP_IPP_IND_ROW_4_SELECT_INPUT
// DAISY
#define SEL_KEY_ROW4_ALT0  0x0
#define SEL_LCD_DAT17_ALT1 0x1
#define SEL_SD1_DAT7_ALT2  0x2

// IOMUXC_KPP_IPP_IND_ROW_5_SELECT_INPUT
// DAISY
#define SEL_KEY_ROW5_ALT0  0x0
#define SEL_LCD_DAT19_ALT1 0x1
#define SEL_SD3_CMD_ALT2   0x2

// IOMUXC_KPP_IPP_IND_ROW_6_SELECT_INPUT
// DAISY
#define SEL_KEY_ROW6_ALT0  0x0
#define SEL_LCD_DAT21_ALT1 0x1
#define SEL_SD3_DAT1_ALT2  0x2

// IOMUXC_KPP_IPP_IND_ROW_7_SELECT_INPUT
// DAISY
#define SEL_KEY_ROW7_ALT0  0x0
#define SEL_LCD_DAT23_ALT1 0x1
#define SEL_SD3_DAT3_ALT2  0x2

// IOMUXC_LCDIF_LCDIF_BUSY_SELECT_INPUT
// DAISY
#define SEL_LCD_HSYNC_ALT0 0x0
#define SEL_LCD_RESET_ALT2 0x1

// IOMUXC_LCDIF_LCDIF_RXDATA_0_SELECT_INPUT
// DAISY
#define SEL_KEY_COL0_ALT2 0x0
#define SEL_LCD_DAT0_ALT0 0x1

// IOMUXC_LCDIF_LCDIF_RXDATA_1_SELECT_INPUT
// DAISY
#define SEL_KEY_ROW0_ALT2 0x0
#define SEL_LCD_DAT1_ALT0 0x1

// IOMUXC_LCDIF_LCDIF_RXDATA_2_SELECT_INPUT
// DAISY
#define SEL_KEY_COL1_ALT2 0x0
#define SEL_LCD_DAT2_ALT0 0x1

// IOMUXC_LCDIF_LCDIF_RXDATA_3_SELECT_INPUT
// DAISY
#define SEL_KEY_ROW1_ALT2 0x0
#define SEL_LCD_DAT3_ALT0 0x1

// IOMUXC_LCDIF_LCDIF_RXDATA_4_SELECT_INPUT
// DAISY
#define SEL_KEY_COL2_ALT2 0x0
#define SEL_LCD_DAT4_ALT0 0x1

// IOMUXC_LCDIF_LCDIF_RXDATA_5_SELECT_INPUT
// DAISY
#define SEL_KEY_ROW2_ALT2 0x0
#define SEL_LCD_DAT5_ALT0 0x1

// IOMUXC_LCDIF_LCDIF_RXDATA_6_SELECT_INPUT
// DAISY
#define SEL_KEY_COL3_ALT2 0x0
#define SEL_LCD_DAT6_ALT0 0x1

// IOMUXC_LCDIF_LCDIF_RXDATA_7_SELECT_INPUT
// DAISY
#define SEL_KEY_ROW3_ALT2 0x0
#define SEL_LCD_DAT7_ALT0 0x1

// IOMUXC_LCDIF_LCDIF_RXDATA_8_SELECT_INPUT
// DAISY
#define SEL_KEY_COL4_ALT2 0x0
#define SEL_LCD_DAT8_ALT0 0x1

// IOMUXC_LCDIF_LCDIF_RXDATA_9_SELECT_INPUT
// DAISY
#define SEL_KEY_ROW4_ALT2 0x0
#define SEL_LCD_DAT9_ALT0 0x1

// IOMUXC_LCDIF_LCDIF_RXDATA_10_SELECT_INPUT
// DAISY
#define SEL_KEY_COL5_ALT2  0x0
#define SEL_LCD_DAT10_ALT0 0x1

// IOMUXC_LCDIF_LCDIF_RXDATA_11_SELECT_INPUT
// DAISY
#define SEL_KEY_ROW5_ALT2  0x0
#define SEL_LCD_DAT11_ALT0 0x1

// IOMUXC_LCDIF_LCDIF_RXDATA_12_SELECT_INPUT
// DAISY
#define SEL_KEY_COL6_ALT2  0x0
#define SEL_LCD_DAT12_ALT0 0x1

// IOMUXC_LCDIF_LCDIF_RXDATA_13_SELECT_INPUT
// DAISY
#define SEL_KEY_ROW6_ALT2  0x0
#define SEL_LCD_DAT13_ALT0 0x1

// IOMUXC_LCDIF_LCDIF_RXDATA_14_SELECT_INPUT
// DAISY
#define SEL_KEY_COL7_ALT2  0x0
#define SEL_LCD_DAT14_ALT0 0x1

// IOMUXC_LCDIF_LCDIF_RXDATA_15_SELECT_INPUT
// DAISY
#define SEL_KEY_ROW7_ALT2  0x0
#define SEL_LCD_DAT15_ALT0 0x1

// IOMUXC_LCDIF_LCDIF_RXDATA_16_SELECT_INPUT
// DAISY
#define SEL_EPDC_PWRCTRL0_ALT2 0x0
#define SEL_LCD_DAT16_ALT0     0x1

// IOMUXC_LCDIF_LCDIF_RXDATA_17_SELECT_INPUT
// DAISY
#define SEL_EPDC_PWRCTRL1_ALT2 0x0
#define SEL_LCD_DAT17_ALT0     0x1

// IOMUXC_LCDIF_LCDIF_RXDATA_18_SELECT_INPUT
// DAISY
#define SEL_EPDC_PWRCTRL2_ALT2 0x0
#define SEL_LCD_DAT18_ALT0     0x1

// IOMUXC_LCDIF_LCDIF_RXDATA_19_SELECT_INPUT
// DAISY
#define SEL_EPDC_PWRCTRL3_ALT2 0x0
#define SEL_LCD_DAT19_ALT0     0x1

// IOMUXC_LCDIF_LCDIF_RXDATA_20_SELECT_INPUT
// DAISY
#define SEL_EPDC_PWRCOM_ALT2 0x0
#define SEL_LCD_DAT20_ALT0   0x1

// IOMUXC_LCDIF_LCDIF_RXDATA_21_SELECT_INPUT
// DAISY
#define SEL_EPDC_PWRINT_ALT2 0x0
#define SEL_LCD_DAT21_ALT0   0x1

// IOMUXC_LCDIF_LCDIF_RXDATA_22_SELECT_INPUT
// DAISY
#define SEL_EPDC_PWRSTAT_ALT2 0x0
#define SEL_LCD_DAT22_ALT0    0x1

// IOMUXC_LCDIF_LCDIF_RXDATA_23_SELECT_INPUT
// DAISY
#define SEL_EPDC_PWRWAKEUP_ALT2 0x0
#define SEL_LCD_DAT23_ALT0      0x1

// IOMUXC_SPDIF_SPDIF_IN1_SELECT_INPUT
// DAISY
#define SEL_FEC_TX_EN_ALT2 0x0
#define SEL_I2C2_SCL_ALT2  0x1
#define SEL_SD2_DAT5_ALT4  0x2

// IOMUXC_SPDIF_TX_CLK2_SELECT_INPUT
// DAISY
#define SEL_AUD_MCLK_ALT6    0x0
#define SEL_ECSPI2_SCLK_ALT1 0x1
#define SEL_FEC_REF_CLK_ALT6 0x2

// IOMUXC_UART1_IPP_UART_RTS_B_SELECT_INPUT
// DAISY
#define SEL_I2C1_SCL_ALT1 0x0
#define SEL_I2C1_SDA_ALT1 0x1

// IOMUXC_UART1_IPP_UART_RXD_MUX_SELECT_INPUT
// DAISY
#define SEL_UART1_RXD_ALT0 0x0
#define SEL_UART1_TXD_ALT0 0x1

// IOMUXC_UART2_IPP_UART_RTS_B_SELECT_INPUT
// DAISY
#define SEL_EPDC_D14_ALT1  0x0
#define SEL_EPDC_D15_ALT1  0x1
#define SEL_LCD_RESET_ALT4 0x2
#define SEL_LCD_VSYNC_ALT4 0x3
#define SEL_SD2_DAT6_ALT2  0x4
#define SEL_SD2_DAT7_ALT2  0x5

// IOMUXC_UART2_IPP_UART_RXD_MUX_SELECT_INPUT
// DAISY
#define SEL_EPDC_D12_ALT1   0x0
#define SEL_EPDC_D13_ALT1   0x1
#define SEL_LCD_ENABLE_ALT4 0x2
#define SEL_LCD_HSYNC_ALT4  0x3
#define SEL_SD2_DAT4_ALT2   0x4
#define SEL_SD2_DAT5_ALT2   0x5

// IOMUXC_UART3_IPP_UART_RTS_B_SELECT_INPUT
// DAISY
#define SEL_ECSPI2_MISO_ALT2 0x0
#define SEL_ECSPI2_SS0_ALT2  0x1
#define SEL_EPDC_BDR0_ALT2   0x2
#define SEL_EPDC_BDR1_ALT2   0x3

// IOMUXC_UART3_IPP_UART_RXD_MUX_SELECT_INPUT
// DAISY
#define SEL_AUD_RXC_ALT2     0x0
#define SEL_AUD_RXFS_ALT2    0x1
#define SEL_ECSPI2_MOSI_ALT2 0x2
#define SEL_ECSPI2_SCLK_ALT2 0x3
#define SEL_EPDC_VCOM0_ALT2  0x4
#define SEL_EPDC_VCOM1_ALT2  0x5

// IOMUXC_UART4_IPP_UART_RTS_B_SELECT_INPUT
// DAISY
#define SEL_AUD_TXD_ALT2  0x0
#define SEL_AUD_TXFS_ALT2 0x1
#define SEL_KEY_COL7_ALT1 0x2
#define SEL_KEY_ROW7_ALT1 0x3
#define SEL_SD1_DAT6_ALT4 0x4
#define SEL_SD1_DAT7_ALT4 0x5

// IOMUXC_UART4_IPP_UART_RXD_MUX_SELECT_INPUT
// DAISY
#define SEL_AUD_RXD_ALT2   0x0
#define SEL_AUD_TXC_ALT2   0x1
#define SEL_KEY_COL6_ALT1  0x2
#define SEL_KEY_ROW6_ALT1  0x3
#define SEL_SD1_DAT4_ALT4  0x4
#define SEL_SD1_DAT5_ALT4  0x5
#define SEL_UART1_RXD_ALT2 0x6
#define SEL_UART1_TXD_ALT2 0x7

// IOMUXC_UART5_IPP_UART_RTS_B_SELECT_INPUT
// DAISY
#define SEL_ECSPI1_MISO_ALT2 0x0
#define SEL_ECSPI1_SS0_ALT2  0x1
#define SEL_LCD_DAT12_ALT4   0x2
#define SEL_LCD_DAT13_ALT4   0x3
#define SEL_SD2_DAT0_ALT4    0x4
#define SEL_SD2_DAT1_ALT4    0x5

// IOMUXC_UART5_IPP_UART_RXD_MUX_SELECT_INPUT
// DAISY
#define SEL_ECSPI1_MOSI_ALT2 0x0
#define SEL_ECSPI1_SCLK_ALT2 0x1
#define SEL_LCD_DAT14_ALT4   0x2
#define SEL_LCD_DAT15_ALT4   0x3
#define SEL_SD2_DAT2_ALT4    0x4
#define SEL_SD2_DAT3_ALT4    0x5
#define SEL_UART1_RXD_ALT4   0x6
#define SEL_UART1_TXD_ALT4   0x7

// IOMUXC_USB_IPP_IND_OTG2_OC_SELECT_INPUT
// DAISY
#define SEL_ECSPI1_SCLK_ALT6 0x0
#define SEL_ECSPI2_SCLK_ALT6 0x1
#define SEL_KEY_ROW5_ALT6    0x2
#define SEL_SD3_DAT2_ALT6    0x3

// IOMUXC_USB_IPP_IND_OTG_OC_SELECT_INPUT
// DAISY
#define SEL_ECSPI2_MISO_ALT6 0x0
#define SEL_KEY_ROW4_ALT6    0x1
#define SEL_SD3_DAT3_ALT6    0x2

// IOMUXC_USDHC1_IPP_CARD_DET_SELECT_INPUT
// DAISY
#define SEL_ECSPI2_SS0_ALT4 0x0
#define SEL_FEC_TXD1_ALT3   0x1
#define SEL_KEY_COL0_ALT4   0x2
#define SEL_KEY_ROW7_ALT6   0x3

// IOMUXC_USDHC1_IPP_WP_ON_SELECT_INPUT
// DAISY
#define SEL_ECSPI2_MISO_ALT4 0x0
#define SEL_FEC_TX_EN_ALT3   0x1
#define SEL_KEY_COL7_ALT6    0x2
#define SEL_KEY_ROW0_ALT4    0x3

// IOMUXC_USDHC2_IPP_CARD_DET_SELECT_INPUT
// DAISY
#define SEL_ECSPI1_SS0_ALT4 0x0
#define SEL_EPDC_GDSP_ALT6  0x1
#define SEL_SD2_DAT7_ALT4   0x2

// IOMUXC_USDHC2_IPP_WP_ON_SELECT_INPUT
// DAISY
#define SEL_ECSPI1_MISO_ALT4 0x0
#define SEL_EPDC_GDRL_ALT6   0x1
#define SEL_SD2_DAT6_ALT4    0x2

// IOMUXC_USDHC3_IPP_CARD_DET_SELECT_INPUT
// DAISY
#define SEL_EPDC_PWRWAKEUP_ALT6 0x0
#define SEL_FEC_TXD1_ALT4       0x1
#define SEL_I2C2_SDA_ALT4       0x2
#define SEL_REF_CLK_32K_ALT6    0x3

// IOMUXC_USDHC3_IPP_DAT4_IN_SELECT_INPUT
// DAISY
#define SEL_KEY_COL1_ALT4 0x0
#define SEL_SD2_DAT4_ALT1 0x1

// IOMUXC_USDHC3_IPP_DAT5_IN_SELECT_INPUT
// DAISY
#define SEL_KEY_ROW1_ALT4 0x0
#define SEL_SD2_DAT5_ALT1 0x1

// IOMUXC_USDHC3_IPP_DAT6_IN_SELECT_INPUT
// DAISY
#define SEL_KEY_COL2_ALT4 0x0
#define SEL_SD2_DAT6_ALT1 0x1

// IOMUXC_USDHC3_IPP_DAT7_IN_SELECT_INPUT
// DAISY
#define SEL_KEY_ROW2_ALT4 0x0
#define SEL_SD2_DAT7_ALT1 0x1

// IOMUXC_USDHC3_IPP_WP_ON_SELECT_INPUT
// DAISY
#define SEL_EPDC_PWRSTAT_ALT6 0x0
#define SEL_FEC_TX_EN_ALT4    0x1
#define SEL_I2C2_SCL_ALT4     0x2
#define SEL_REF_CLK_24M_ALT6  0x3

// IOMUXC_USDHC4_IPP_CARD_CLK_IN_SELECT_INPUT
// DAISY
#define SEL_EPDC_BDR0_ALT1 0x0
#define SEL_FEC_MDIO_ALT1  0x1
#define SEL_KEY_COL4_ALT4  0x2

// IOMUXC_USDHC4_IPP_CARD_DET_SELECT_INPUT
// DAISY
#define SEL_EPDC_D11_ALT6      0x0
#define SEL_EPDC_PWRCTRL3_ALT6 0x1

// IOMUXC_USDHC4_IPP_CMD_IN_SELECT_INPUT
// DAISY
#define SEL_EPDC_BDR1_ALT1  0x0
#define SEL_FEC_TX_CLK_ALT1 0x1
#define SEL_KEY_ROW4_ALT4   0x2

// IOMUXC_USDHC4_IPP_DAT0_IN_SELECT_INPUT
// DAISY
#define SEL_EPDC_PWRCOM_ALT1 0x0
#define SEL_FEC_RX_ER_ALT1   0x1
#define SEL_KEY_COL5_ALT4    0x2

// IOMUXC_USDHC4_IPP_DAT1_IN_SELECT_INPUT
// DAISY
#define SEL_EPDC_PWRINT_ALT1 0x0
#define SEL_FEC_CRS_DV_ALT1  0x1
#define SEL_KEY_ROW5_ALT4    0x2

// IOMUXC_USDHC4_IPP_DAT2_IN_SELECT_INPUT
// DAISY
#define SEL_EPDC_PWRSTAT_ALT1 0x0
#define SEL_FEC_RXD1_ALT1     0x1
#define SEL_KEY_COL6_ALT4     0x2

// IOMUXC_USDHC4_IPP_DAT3_IN_SELECT_INPUT
// DAISY
#define SEL_EPDC_PWRWAKEUP_ALT1 0x0
#define SEL_FEC_TXD0_ALT1       0x1
#define SEL_KEY_ROW6_ALT4       0x2

// IOMUXC_USDHC4_IPP_DAT4_IN_SELECT_INPUT
// DAISY
#define SEL_FEC_MDC_ALT1  0x0
#define SEL_KEY_COL7_ALT4 0x1
#define SEL_LCD_CLK_ALT1  0x2

// IOMUXC_USDHC4_IPP_DAT5_IN_SELECT_INPUT
// DAISY
#define SEL_FEC_RXD0_ALT1   0x0
#define SEL_KEY_ROW7_ALT4   0x1
#define SEL_LCD_ENABLE_ALT1 0x2

// IOMUXC_USDHC4_IPP_DAT6_IN_SELECT_INPUT
// DAISY
#define SEL_FEC_TX_EN_ALT1 0x0
#define SEL_KEY_COL3_ALT4  0x1
#define SEL_LCD_HSYNC_ALT1 0x2

// IOMUXC_USDHC4_IPP_DAT7_IN_SELECT_INPUT
// DAISY
#define SEL_FEC_TXD1_ALT1  0x0
#define SEL_KEY_ROW3_ALT4  0x1
#define SEL_LCD_VSYNC_ALT1 0x2

// IOMUXC_USDHC4_IPP_WP_ON_SELECT_INPUT
// DAISY
#define SEL_EPDC_D10_ALT6      0x0
#define SEL_EPDC_PWRCTRL2_ALT6 0x1

// IOMUXC_WEIM_IPP_IND_DTACK_B_SELECT_INPUT
// DAISY
#define SEL_EPDC_PWRWAKEUP_ALT3 0x0
#define SEL_LCD_RESET_ALT1      0x1

// IOMUXC_WEIM_IPP_IND_WAIT_B_SELECT_INPUT
// DAISY
#define SEL_EPDC_PWRSTAT_ALT3 0x0
#define SEL_LCD_RESET_ALT3    0x1


#endif // _IOMUX_DEFINE_H_
