/*
 * Copyright (c) 2012, Freescale Semiconductor, Inc.
 * All rights reserved.
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

// File: i2c2_iomux_config.c

/* ------------------------------------------------------------------------------
 * <auto-generated>
 *     This code was generated by a tool.
 *     Runtime Version:3.4.0.0
 *
 *     Changes to this file may cause incorrect behavior and will be lost if
 *     the code is regenerated.
 * </auto-generated>
 * ------------------------------------------------------------------------------
*/

#include "iomux_config.h"
#include "registers/regsiomuxc.h"

// Function to configure IOMUXC for i2c2 module.
void i2c2_iomux_config(void)
{
    // Config i2c2.I2C2_SCL to pad I2C2_SCL(E18)
    // HW_IOMUXC_SW_MUX_CTL_PAD_I2C2_SCL_WR(0x00000010);
    // HW_IOMUXC_SW_PAD_CTL_PAD_I2C2_SCL_WR(0x000110B0);
    // HW_IOMUXC_I2C2_SCL_IN_SELECT_INPUT_WR(0x00000001);
    // Mux Register:
    // IOMUXC_SW_MUX_CTL_PAD_I2C2_SCL(0x020E0164)
    //   SION [4] - Software Input On Field Reset: DISABLED
    //              Force the selected mux mode Input path no matter of MUX_MODE functionality.
    //     DISABLED (0) - Input Path is determined by functionality of the selected mux mode (regular).
    //     ENABLED (1) - Force input path of pad.
    //   MUX_MODE [2:0] - MUX Mode Select Field Reset: ALT5
    //                    Select iomux modes to be used for pad.
    //     ALT0 (0) - Select instance: i2c2 signal: I2C2_SCL
    //     ALT1 (1) - Select instance: audmux signal: AUD4_RXFS
    //     ALT2 (2) - Select instance: spdif signal: SPDIF_IN
    //     ALT3 (3) - Select instance: fec signal: FEC_TX_DATA1
    //     ALT4 (4) - Select instance: usdhc3 signal: SD3_WP
    //     ALT5 (5) - Select instance: gpio3 signal: GPIO3_IO14
    //     ALT6 (6) - Select instance: ecspi1 signal: ECSPI1_RDY
    HW_IOMUXC_SW_MUX_CTL_PAD_I2C2_SCL_WR(
            BF_IOMUXC_SW_MUX_CTL_PAD_I2C2_SCL_SION_V(ENABLED) | 
            BF_IOMUXC_SW_MUX_CTL_PAD_I2C2_SCL_MUX_MODE_V(ALT0));
    // Pad Control Register:
    // IOMUXC_SW_PAD_CTL_PAD_I2C2_SCL(0x020E0454)
    //   LVE [22] - Low Voltage Enable Field Reset: DISABLED
    //     DISABLED (0) - High Voltage
    //     ENABLED (1) - Low Voltage
    //   HYS [16] - Hysteresis Enable Field Reset: ENABLED
    //     DISABLED (0) - CMOS input
    //     ENABLED (1) - Schmitt trigger input
    //   PUS [15:14] - Pull Up / Down Config. Field Reset: 100K_OHM_PD
    //     100K_OHM_PD (0) - 100K Ohm Pull Down
    //     47K_OHM_PU (1) - 47K Ohm Pull Up
    //     100K_OHM_PU (2) - 100K Ohm Pull Up
    //     22K_OHM_PU (3) - 22K Ohm Pull Up
    //   PUE [13] - Pull / Keep Select Field Reset: KEEP
    //     KEEP (0) - Keeper Enabled
    //     PULL (1) - Pull Enabled
    //   PKE [12] - Pull / Keep Enable Field Reset: ENABLED
    //     DISABLED (0) - Pull/Keeper Disabled
    //     ENABLED (1) - Pull/Keeper Enabled
    //   ODE [11] - Open Drain Enable Field Reset: DISABLED
    //              Enables open drain of the pin.
    //     DISABLED (0) - Output is CMOS.
    //     ENABLED (1) - Output is Open Drain.
    //   SPEED [7:6] - Speed Field Reset: 100MHZ
    //     RESERVED0 (0) - Reserved
    //     50MHZ (1) - Low (50 MHz)
    //     100MHZ (2) - Medium (100 MHz)
    //     200MHZ (3) - Maximum (200 MHz)
    //   DSE [5:3] - Drive Strength Field Reset: 40_OHM
    //     HIZ (0) - HI-Z
    //     240_OHM (1) - 240 Ohm
    //     120_OHM (2) - 120 Ohm
    //     80_OHM (3) - 80 Ohm
    //     60_OHM (4) - 60 Ohm
    //     48_OHM (5) - 48 Ohm
    //     40_OHM (6) - 40 Ohm
    //     34_OHM (7) - 34 Ohm
    //   SRE [0] - Slew Rate Field Reset: SLOW
    //             Slew rate control.
    //     SLOW (0) - Slow Slew Rate
    //     FAST (1) - Fast Slew Rate
    HW_IOMUXC_SW_PAD_CTL_PAD_I2C2_SCL_WR(
            BF_IOMUXC_SW_PAD_CTL_PAD_I2C2_SCL_LVE_V(DISABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_I2C2_SCL_HYS_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_I2C2_SCL_PUS_V(100K_OHM_PD) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_I2C2_SCL_PUE_V(KEEP) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_I2C2_SCL_PKE_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_I2C2_SCL_ODE_V(DISABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_I2C2_SCL_SPEED_V(100MHZ) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_I2C2_SCL_DSE_V(40_OHM) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_I2C2_SCL_SRE_V(SLOW));
    // Pad I2C2_SCL is involved in Daisy Chain.
    // Input Select Register:
    // IOMUXC_I2C2_SCL_IN_SELECT_INPUT(0x020E0724)
    //   DAISY [1:0] - MUX Mode Select Field Reset: EPDC_SDCLK_ALT2
    //                 Selecting Pads Involved in Daisy Chain.
    //     EPDC_SDCLK_ALT2 (0) - Select signal i2c2 I2C2_SCL as input from pad EPDC_SDCLK(ALT2).
    //     I2C2_SCL_ALT0 (1) - Select signal i2c2 I2C2_SCL as input from pad I2C2_SCL(ALT0).
    //     KEY_COL0_ALT1 (2) - Select signal i2c2 I2C2_SCL as input from pad KEY_COL0(ALT1).
    //     LCD_DATA16_ALT4 (3) - Select signal i2c2 I2C2_SCL as input from pad LCD_DATA16(ALT4).
    HW_IOMUXC_I2C2_SCL_IN_SELECT_INPUT_WR(
            BF_IOMUXC_I2C2_SCL_IN_SELECT_INPUT_DAISY_V(I2C2_SCL_ALT0));

    // Config i2c2.I2C2_SDA to pad I2C2_SDA(D18)
    // HW_IOMUXC_SW_MUX_CTL_PAD_I2C2_SDA_WR(0x00000010);
    // HW_IOMUXC_SW_PAD_CTL_PAD_I2C2_SDA_WR(0x000110B0);
    // HW_IOMUXC_I2C2_SDA_IN_SELECT_INPUT_WR(0x00000001);
    // Mux Register:
    // IOMUXC_SW_MUX_CTL_PAD_I2C2_SDA(0x020E0168)
    //   SION [4] - Software Input On Field Reset: DISABLED
    //              Force the selected mux mode Input path no matter of MUX_MODE functionality.
    //     DISABLED (0) - Input Path is determined by functionality of the selected mux mode (regular).
    //     ENABLED (1) - Force input path of pad.
    //   MUX_MODE [2:0] - MUX Mode Select Field Reset: ALT5
    //                    Select iomux modes to be used for pad.
    //     ALT0 (0) - Select instance: i2c2 signal: I2C2_SDA
    //     ALT1 (1) - Select instance: audmux signal: AUD4_RXC
    //     ALT2 (2) - Select instance: spdif signal: SPDIF_OUT
    //     ALT3 (3) - Select instance: fec signal: FEC_REF_OUT
    //     ALT4 (4) - Select instance: usdhc3 signal: SD3_CD_B
    //     ALT5 (5) - Select instance: gpio3 signal: GPIO3_IO15
    HW_IOMUXC_SW_MUX_CTL_PAD_I2C2_SDA_WR(
            BF_IOMUXC_SW_MUX_CTL_PAD_I2C2_SDA_SION_V(ENABLED) | 
            BF_IOMUXC_SW_MUX_CTL_PAD_I2C2_SDA_MUX_MODE_V(ALT0));
    // Pad Control Register:
    // IOMUXC_SW_PAD_CTL_PAD_I2C2_SDA(0x020E0458)
    //   LVE [22] - Low Voltage Enable Field Reset: DISABLED
    //     DISABLED (0) - High Voltage
    //     ENABLED (1) - Low Voltage
    //   HYS [16] - Hysteresis Enable Field Reset: ENABLED
    //     DISABLED (0) - CMOS input
    //     ENABLED (1) - Schmitt trigger input
    //   PUS [15:14] - Pull Up / Down Config. Field Reset: 100K_OHM_PD
    //     100K_OHM_PD (0) - 100K Ohm Pull Down
    //     47K_OHM_PU (1) - 47K Ohm Pull Up
    //     100K_OHM_PU (2) - 100K Ohm Pull Up
    //     22K_OHM_PU (3) - 22K Ohm Pull Up
    //   PUE [13] - Pull / Keep Select Field Reset: KEEP
    //     KEEP (0) - Keeper Enabled
    //     PULL (1) - Pull Enabled
    //   PKE [12] - Pull / Keep Enable Field Reset: ENABLED
    //     DISABLED (0) - Pull/Keeper Disabled
    //     ENABLED (1) - Pull/Keeper Enabled
    //   ODE [11] - Open Drain Enable Field Reset: DISABLED
    //              Enables open drain of the pin.
    //     DISABLED (0) - Output is CMOS.
    //     ENABLED (1) - Output is Open Drain.
    //   SPEED [7:6] - Speed Field Reset: 100MHZ
    //     RESERVED0 (0) - Reserved
    //     50MHZ (1) - Low (50 MHz)
    //     100MHZ (2) - Medium (100 MHz)
    //     200MHZ (3) - Maximum (200 MHz)
    //   DSE [5:3] - Drive Strength Field Reset: 40_OHM
    //     HIZ (0) - HI-Z
    //     240_OHM (1) - 240 Ohm
    //     120_OHM (2) - 120 Ohm
    //     80_OHM (3) - 80 Ohm
    //     60_OHM (4) - 60 Ohm
    //     48_OHM (5) - 48 Ohm
    //     40_OHM (6) - 40 Ohm
    //     34_OHM (7) - 34 Ohm
    //   SRE [0] - Slew Rate Field Reset: SLOW
    //             Slew rate control.
    //     SLOW (0) - Slow Slew Rate
    //     FAST (1) - Fast Slew Rate
    HW_IOMUXC_SW_PAD_CTL_PAD_I2C2_SDA_WR(
            BF_IOMUXC_SW_PAD_CTL_PAD_I2C2_SDA_LVE_V(DISABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_I2C2_SDA_HYS_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_I2C2_SDA_PUS_V(100K_OHM_PD) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_I2C2_SDA_PUE_V(KEEP) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_I2C2_SDA_PKE_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_I2C2_SDA_ODE_V(DISABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_I2C2_SDA_SPEED_V(100MHZ) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_I2C2_SDA_DSE_V(40_OHM) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_I2C2_SDA_SRE_V(SLOW));
    // Pad I2C2_SDA is involved in Daisy Chain.
    // Input Select Register:
    // IOMUXC_I2C2_SDA_IN_SELECT_INPUT(0x020E0728)
    //   DAISY [1:0] - MUX Mode Select Field Reset: EPDC_SDLE_ALT2
    //                 Selecting Pads Involved in Daisy Chain.
    //     EPDC_SDLE_ALT2 (0) - Select signal i2c2 I2C2_SDA as input from pad EPDC_SDLE(ALT2).
    //     I2C2_SDA_ALT0 (1) - Select signal i2c2 I2C2_SDA as input from pad I2C2_SDA(ALT0).
    //     KEY_ROW0_ALT1 (2) - Select signal i2c2 I2C2_SDA as input from pad KEY_ROW0(ALT1).
    //     LCD_DATA17_ALT4 (3) - Select signal i2c2 I2C2_SDA as input from pad LCD_DATA17(ALT4).
    HW_IOMUXC_I2C2_SDA_IN_SELECT_INPUT_WR(
            BF_IOMUXC_I2C2_SDA_IN_SELECT_INPUT_DAISY_V(I2C2_SDA_ALT0));
}
