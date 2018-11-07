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

// File: kpp_iomux_config.c

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

// Function to configure IOMUXC for kpp module.
void kpp_iomux_config(void)
{
    // Config kpp.KEY_COL0 to pad KEY_COL0(G23)
    // HW_IOMUXC_SW_MUX_CTL_PAD_KEY_COL0_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_KEY_COL0_WR(0x000110B0);
    // HW_IOMUXC_KEY_COL0_SELECT_INPUT_WR(0x00000000);
    // Mux Register:
    // IOMUXC_SW_MUX_CTL_PAD_KEY_COL0(0x020E016C)
    //   SION [4] - Software Input On Field Reset: DISABLED
    //              Force the selected mux mode Input path no matter of MUX_MODE functionality.
    //     DISABLED (0) - Input Path is determined by functionality of the selected mux mode (regular).
    //     ENABLED (1) - Force input path of pad.
    //   MUX_MODE [2:0] - MUX Mode Select Field Reset: ALT5
    //                    Select iomux modes to be used for pad.
    //     ALT0 (0) - Select instance: kpp signal: KEY_COL0
    //     ALT1 (1) - Select instance: i2c2 signal: I2C2_SCL
    //     ALT2 (2) - Select instance: lcd signal: LCD_DATA00
    //     ALT3 (3) - Select instance: eim signal: EIM_AD00
    //     ALT4 (4) - Select instance: usdhc1 signal: SD1_CD_B
    //     ALT5 (5) - Select instance: gpio3 signal: GPIO3_IO24
    HW_IOMUXC_SW_MUX_CTL_PAD_KEY_COL0_WR(
            BF_IOMUXC_SW_MUX_CTL_PAD_KEY_COL0_SION_V(DISABLED) | 
            BF_IOMUXC_SW_MUX_CTL_PAD_KEY_COL0_MUX_MODE_V(ALT0));
    // Pad Control Register:
    // IOMUXC_SW_PAD_CTL_PAD_KEY_COL0(0x020E0474)
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
    HW_IOMUXC_SW_PAD_CTL_PAD_KEY_COL0_WR(
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_COL0_LVE_V(DISABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_COL0_HYS_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_COL0_PUS_V(100K_OHM_PD) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_COL0_PUE_V(KEEP) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_COL0_PKE_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_COL0_ODE_V(DISABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_COL0_SPEED_V(100MHZ) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_COL0_DSE_V(40_OHM) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_COL0_SRE_V(SLOW));
    // Pad KEY_COL0 is involved in Daisy Chain.
    // Input Select Register:
    // IOMUXC_KEY_COL0_SELECT_INPUT(0x020E0734)
    //   DAISY [1:0] - MUX Mode Select Field Reset: KEY_COL0_ALT0
    //                 Selecting Pads Involved in Daisy Chain.
    //     KEY_COL0_ALT0 (0) - Select signal kpp KEY_COL0 as input from pad KEY_COL0(ALT0).
    //     LCD_DATA08_ALT1 (1) - Select signal kpp KEY_COL0 as input from pad LCD_DATA08(ALT1).
    //     SD1_CLK_ALT2 (2) - Select signal kpp KEY_COL0 as input from pad SD1_CLK(ALT2).
    HW_IOMUXC_KEY_COL0_SELECT_INPUT_WR(
            BF_IOMUXC_KEY_COL0_SELECT_INPUT_DAISY_V(KEY_COL0_ALT0));

    // Config kpp.KEY_COL1 to pad KEY_COL1(F23)
    // HW_IOMUXC_SW_MUX_CTL_PAD_KEY_COL1_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_KEY_COL1_WR(0x000110B0);
    // HW_IOMUXC_KEY_COL1_SELECT_INPUT_WR(0x00000000);
    // Mux Register:
    // IOMUXC_SW_MUX_CTL_PAD_KEY_COL1(0x020E0170)
    //   SION [4] - Software Input On Field Reset: DISABLED
    //              Force the selected mux mode Input path no matter of MUX_MODE functionality.
    //     DISABLED (0) - Input Path is determined by functionality of the selected mux mode (regular).
    //     ENABLED (1) - Force input path of pad.
    //   MUX_MODE [2:0] - MUX Mode Select Field Reset: ALT5
    //                    Select iomux modes to be used for pad.
    //     ALT0 (0) - Select instance: kpp signal: KEY_COL1
    //     ALT1 (1) - Select instance: ecspi4 signal: ECSPI4_MOSI
    //     ALT2 (2) - Select instance: lcd signal: LCD_DATA02
    //     ALT3 (3) - Select instance: eim signal: EIM_AD02
    //     ALT4 (4) - Select instance: usdhc3 signal: SD3_DATA4
    //     ALT5 (5) - Select instance: gpio3 signal: GPIO3_IO26
    HW_IOMUXC_SW_MUX_CTL_PAD_KEY_COL1_WR(
            BF_IOMUXC_SW_MUX_CTL_PAD_KEY_COL1_SION_V(DISABLED) | 
            BF_IOMUXC_SW_MUX_CTL_PAD_KEY_COL1_MUX_MODE_V(ALT0));
    // Pad Control Register:
    // IOMUXC_SW_PAD_CTL_PAD_KEY_COL1(0x020E0478)
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
    HW_IOMUXC_SW_PAD_CTL_PAD_KEY_COL1_WR(
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_COL1_LVE_V(DISABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_COL1_HYS_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_COL1_PUS_V(100K_OHM_PD) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_COL1_PUE_V(KEEP) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_COL1_PKE_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_COL1_ODE_V(DISABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_COL1_SPEED_V(100MHZ) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_COL1_DSE_V(40_OHM) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_COL1_SRE_V(SLOW));
    // Pad KEY_COL1 is involved in Daisy Chain.
    // Input Select Register:
    // IOMUXC_KEY_COL1_SELECT_INPUT(0x020E0738)
    //   DAISY [1:0] - MUX Mode Select Field Reset: KEY_COL1_ALT0
    //                 Selecting Pads Involved in Daisy Chain.
    //     KEY_COL1_ALT0 (0) - Select signal kpp KEY_COL1 as input from pad KEY_COL1(ALT0).
    //     LCD_DATA10_ALT1 (1) - Select signal kpp KEY_COL1 as input from pad LCD_DATA10(ALT1).
    //     SD1_DATA0_ALT2 (2) - Select signal kpp KEY_COL1 as input from pad SD1_DATA0(ALT2).
    HW_IOMUXC_KEY_COL1_SELECT_INPUT_WR(
            BF_IOMUXC_KEY_COL1_SELECT_INPUT_DAISY_V(KEY_COL1_ALT0));

    // Config kpp.KEY_COL2 to pad KEY_COL2(E23)
    // HW_IOMUXC_SW_MUX_CTL_PAD_KEY_COL2_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_KEY_COL2_WR(0x000110B0);
    // HW_IOMUXC_KEY_COL2_SELECT_INPUT_WR(0x00000000);
    // Mux Register:
    // IOMUXC_SW_MUX_CTL_PAD_KEY_COL2(0x020E0174)
    //   SION [4] - Software Input On Field Reset: DISABLED
    //              Force the selected mux mode Input path no matter of MUX_MODE functionality.
    //     DISABLED (0) - Input Path is determined by functionality of the selected mux mode (regular).
    //     ENABLED (1) - Force input path of pad.
    //   MUX_MODE [2:0] - MUX Mode Select Field Reset: ALT5
    //                    Select iomux modes to be used for pad.
    //     ALT0 (0) - Select instance: kpp signal: KEY_COL2
    //     ALT1 (1) - Select instance: ecspi4 signal: ECSPI4_SS0
    //     ALT2 (2) - Select instance: lcd signal: LCD_DATA04
    //     ALT3 (3) - Select instance: eim signal: EIM_AD04
    //     ALT4 (4) - Select instance: usdhc3 signal: SD3_DATA6
    //     ALT5 (5) - Select instance: gpio3 signal: GPIO3_IO28
    HW_IOMUXC_SW_MUX_CTL_PAD_KEY_COL2_WR(
            BF_IOMUXC_SW_MUX_CTL_PAD_KEY_COL2_SION_V(DISABLED) | 
            BF_IOMUXC_SW_MUX_CTL_PAD_KEY_COL2_MUX_MODE_V(ALT0));
    // Pad Control Register:
    // IOMUXC_SW_PAD_CTL_PAD_KEY_COL2(0x020E047C)
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
    HW_IOMUXC_SW_PAD_CTL_PAD_KEY_COL2_WR(
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_COL2_LVE_V(DISABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_COL2_HYS_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_COL2_PUS_V(100K_OHM_PD) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_COL2_PUE_V(KEEP) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_COL2_PKE_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_COL2_ODE_V(DISABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_COL2_SPEED_V(100MHZ) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_COL2_DSE_V(40_OHM) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_COL2_SRE_V(SLOW));
    // Pad KEY_COL2 is involved in Daisy Chain.
    // Input Select Register:
    // IOMUXC_KEY_COL2_SELECT_INPUT(0x020E073C)
    //   DAISY [1:0] - MUX Mode Select Field Reset: KEY_COL2_ALT0
    //                 Selecting Pads Involved in Daisy Chain.
    //     KEY_COL2_ALT0 (0) - Select signal kpp KEY_COL2 as input from pad KEY_COL2(ALT0).
    //     LCD_DATA12_ALT1 (1) - Select signal kpp KEY_COL2 as input from pad LCD_DATA12(ALT1).
    //     SD1_DATA2_ALT2 (2) - Select signal kpp KEY_COL2 as input from pad SD1_DATA2(ALT2).
    HW_IOMUXC_KEY_COL2_SELECT_INPUT_WR(
            BF_IOMUXC_KEY_COL2_SELECT_INPUT_DAISY_V(KEY_COL2_ALT0));

    // Config kpp.KEY_COL3 to pad KEY_COL3(E22)
    // HW_IOMUXC_SW_MUX_CTL_PAD_KEY_COL3_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_KEY_COL3_WR(0x000110B0);
    // HW_IOMUXC_KEY_COL3_SELECT_INPUT_WR(0x00000000);
    // Mux Register:
    // IOMUXC_SW_MUX_CTL_PAD_KEY_COL3(0x020E0178)
    //   SION [4] - Software Input On Field Reset: DISABLED
    //              Force the selected mux mode Input path no matter of MUX_MODE functionality.
    //     DISABLED (0) - Input Path is determined by functionality of the selected mux mode (regular).
    //     ENABLED (1) - Force input path of pad.
    //   MUX_MODE [2:0] - MUX Mode Select Field Reset: ALT5
    //                    Select iomux modes to be used for pad.
    //     ALT0 (0) - Select instance: kpp signal: KEY_COL3
    //     ALT1 (1) - Select instance: audmux signal: AUD6_RXFS
    //     ALT2 (2) - Select instance: lcd signal: LCD_DATA06
    //     ALT3 (3) - Select instance: eim signal: EIM_AD06
    //     ALT4 (4) - Select instance: usdhc4 signal: SD4_DATA6
    //     ALT5 (5) - Select instance: gpio3 signal: GPIO3_IO30
    //     ALT6 (6) - Select instance: usdhc1 signal: SD1_RESET
    HW_IOMUXC_SW_MUX_CTL_PAD_KEY_COL3_WR(
            BF_IOMUXC_SW_MUX_CTL_PAD_KEY_COL3_SION_V(DISABLED) | 
            BF_IOMUXC_SW_MUX_CTL_PAD_KEY_COL3_MUX_MODE_V(ALT0));
    // Pad Control Register:
    // IOMUXC_SW_PAD_CTL_PAD_KEY_COL3(0x020E0480)
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
    HW_IOMUXC_SW_PAD_CTL_PAD_KEY_COL3_WR(
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_COL3_LVE_V(DISABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_COL3_HYS_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_COL3_PUS_V(100K_OHM_PD) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_COL3_PUE_V(KEEP) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_COL3_PKE_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_COL3_ODE_V(DISABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_COL3_SPEED_V(100MHZ) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_COL3_DSE_V(40_OHM) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_COL3_SRE_V(SLOW));
    // Pad KEY_COL3 is involved in Daisy Chain.
    // Input Select Register:
    // IOMUXC_KEY_COL3_SELECT_INPUT(0x020E0740)
    //   DAISY [1:0] - MUX Mode Select Field Reset: KEY_COL3_ALT0
    //                 Selecting Pads Involved in Daisy Chain.
    //     KEY_COL3_ALT0 (0) - Select signal kpp KEY_COL3 as input from pad KEY_COL3(ALT0).
    //     LCD_DATA14_ALT1 (1) - Select signal kpp KEY_COL3 as input from pad LCD_DATA14(ALT1).
    //     SD1_DATA4_ALT2 (2) - Select signal kpp KEY_COL3 as input from pad SD1_DATA4(ALT2).
    HW_IOMUXC_KEY_COL3_SELECT_INPUT_WR(
            BF_IOMUXC_KEY_COL3_SELECT_INPUT_DAISY_V(KEY_COL3_ALT0));

    // Config kpp.KEY_ROW0 to pad KEY_ROW0(G24)
    // HW_IOMUXC_SW_MUX_CTL_PAD_KEY_ROW0_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW0_WR(0x000170B0);
    // HW_IOMUXC_KEY_ROW0_SELECT_INPUT_WR(0x00000000);
    // Mux Register:
    // IOMUXC_SW_MUX_CTL_PAD_KEY_ROW0(0x020E018C)
    //   SION [4] - Software Input On Field Reset: DISABLED
    //              Force the selected mux mode Input path no matter of MUX_MODE functionality.
    //     DISABLED (0) - Input Path is determined by functionality of the selected mux mode (regular).
    //     ENABLED (1) - Force input path of pad.
    //   MUX_MODE [2:0] - MUX Mode Select Field Reset: ALT5
    //                    Select iomux modes to be used for pad.
    //     ALT0 (0) - Select instance: kpp signal: KEY_ROW0
    //     ALT1 (1) - Select instance: i2c2 signal: I2C2_SDA
    //     ALT2 (2) - Select instance: lcd signal: LCD_DATA01
    //     ALT3 (3) - Select instance: eim signal: EIM_AD01
    //     ALT4 (4) - Select instance: usdhc1 signal: SD1_WP
    //     ALT5 (5) - Select instance: gpio3 signal: GPIO3_IO25
    HW_IOMUXC_SW_MUX_CTL_PAD_KEY_ROW0_WR(
            BF_IOMUXC_SW_MUX_CTL_PAD_KEY_ROW0_SION_V(DISABLED) | 
            BF_IOMUXC_SW_MUX_CTL_PAD_KEY_ROW0_MUX_MODE_V(ALT0));
    // Pad Control Register:
    // IOMUXC_SW_PAD_CTL_PAD_KEY_ROW0(0x020E0494)
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
    HW_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW0_WR(
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW0_LVE_V(DISABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW0_HYS_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW0_PUS_V(47K_OHM_PU) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW0_PUE_V(PULL) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW0_PKE_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW0_ODE_V(DISABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW0_SPEED_V(100MHZ) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW0_DSE_V(40_OHM) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW0_SRE_V(SLOW));
    // Pad KEY_ROW0 is involved in Daisy Chain.
    // Input Select Register:
    // IOMUXC_KEY_ROW0_SELECT_INPUT(0x020E0754)
    //   DAISY [1:0] - MUX Mode Select Field Reset: KEY_ROW0_ALT0
    //                 Selecting Pads Involved in Daisy Chain.
    //     KEY_ROW0_ALT0 (0) - Select signal kpp KEY_ROW0 as input from pad KEY_ROW0(ALT0).
    //     LCD_DATA09_ALT1 (1) - Select signal kpp KEY_ROW0 as input from pad LCD_DATA09(ALT1).
    //     SD1_CMD_ALT2 (2) - Select signal kpp KEY_ROW0 as input from pad SD1_CMD(ALT2).
    HW_IOMUXC_KEY_ROW0_SELECT_INPUT_WR(
            BF_IOMUXC_KEY_ROW0_SELECT_INPUT_DAISY_V(KEY_ROW0_ALT0));

    // Config kpp.KEY_ROW1 to pad KEY_ROW1(F24)
    // HW_IOMUXC_SW_MUX_CTL_PAD_KEY_ROW1_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW1_WR(0x000170B0);
    // HW_IOMUXC_KEY_ROW1_SELECT_INPUT_WR(0x00000000);
    // Mux Register:
    // IOMUXC_SW_MUX_CTL_PAD_KEY_ROW1(0x020E0190)
    //   SION [4] - Software Input On Field Reset: DISABLED
    //              Force the selected mux mode Input path no matter of MUX_MODE functionality.
    //     DISABLED (0) - Input Path is determined by functionality of the selected mux mode (regular).
    //     ENABLED (1) - Force input path of pad.
    //   MUX_MODE [2:0] - MUX Mode Select Field Reset: ALT5
    //                    Select iomux modes to be used for pad.
    //     ALT0 (0) - Select instance: kpp signal: KEY_ROW1
    //     ALT1 (1) - Select instance: ecspi4 signal: ECSPI4_MISO
    //     ALT2 (2) - Select instance: lcd signal: LCD_DATA03
    //     ALT3 (3) - Select instance: eim signal: EIM_AD03
    //     ALT4 (4) - Select instance: usdhc3 signal: SD3_DATA5
    //     ALT5 (5) - Select instance: gpio3 signal: GPIO3_IO27
    HW_IOMUXC_SW_MUX_CTL_PAD_KEY_ROW1_WR(
            BF_IOMUXC_SW_MUX_CTL_PAD_KEY_ROW1_SION_V(DISABLED) | 
            BF_IOMUXC_SW_MUX_CTL_PAD_KEY_ROW1_MUX_MODE_V(ALT0));
    // Pad Control Register:
    // IOMUXC_SW_PAD_CTL_PAD_KEY_ROW1(0x020E0498)
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
    HW_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW1_WR(
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW1_LVE_V(DISABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW1_HYS_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW1_PUS_V(47K_OHM_PU) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW1_PUE_V(PULL) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW1_PKE_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW1_ODE_V(DISABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW1_SPEED_V(100MHZ) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW1_DSE_V(40_OHM) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW1_SRE_V(SLOW));
    // Pad KEY_ROW1 is involved in Daisy Chain.
    // Input Select Register:
    // IOMUXC_KEY_ROW1_SELECT_INPUT(0x020E0758)
    //   DAISY [1:0] - MUX Mode Select Field Reset: KEY_ROW1_ALT0
    //                 Selecting Pads Involved in Daisy Chain.
    //     KEY_ROW1_ALT0 (0) - Select signal kpp KEY_ROW1 as input from pad KEY_ROW1(ALT0).
    //     LCD_DATA11_ALT1 (1) - Select signal kpp KEY_ROW1 as input from pad LCD_DATA11(ALT1).
    //     SD1_DATA1_ALT2 (2) - Select signal kpp KEY_ROW1 as input from pad SD1_DATA1(ALT2).
    HW_IOMUXC_KEY_ROW1_SELECT_INPUT_WR(
            BF_IOMUXC_KEY_ROW1_SELECT_INPUT_DAISY_V(KEY_ROW1_ALT0));

    // Config kpp.KEY_ROW2 to pad KEY_ROW2(E24)
    // HW_IOMUXC_SW_MUX_CTL_PAD_KEY_ROW2_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW2_WR(0x0001B0B0);
    // HW_IOMUXC_KEY_ROW2_SELECT_INPUT_WR(0x00000000);
    // Mux Register:
    // IOMUXC_SW_MUX_CTL_PAD_KEY_ROW2(0x020E0194)
    //   SION [4] - Software Input On Field Reset: DISABLED
    //              Force the selected mux mode Input path no matter of MUX_MODE functionality.
    //     DISABLED (0) - Input Path is determined by functionality of the selected mux mode (regular).
    //     ENABLED (1) - Force input path of pad.
    //   MUX_MODE [2:0] - MUX Mode Select Field Reset: ALT5
    //                    Select iomux modes to be used for pad.
    //     ALT0 (0) - Select instance: kpp signal: KEY_ROW2
    //     ALT1 (1) - Select instance: ecspi4 signal: ECSPI4_SCLK
    //     ALT2 (2) - Select instance: lcd signal: LCD_DATA05
    //     ALT3 (3) - Select instance: eim signal: EIM_AD05
    //     ALT4 (4) - Select instance: usdhc3 signal: SD3_DATA7
    //     ALT5 (5) - Select instance: gpio3 signal: GPIO3_IO29
    HW_IOMUXC_SW_MUX_CTL_PAD_KEY_ROW2_WR(
            BF_IOMUXC_SW_MUX_CTL_PAD_KEY_ROW2_SION_V(DISABLED) | 
            BF_IOMUXC_SW_MUX_CTL_PAD_KEY_ROW2_MUX_MODE_V(ALT0));
    // Pad Control Register:
    // IOMUXC_SW_PAD_CTL_PAD_KEY_ROW2(0x020E049C)
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
    HW_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW2_WR(
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW2_LVE_V(DISABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW2_HYS_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW2_PUS_V(100K_OHM_PU) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW2_PUE_V(PULL) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW2_PKE_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW2_ODE_V(DISABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW2_SPEED_V(100MHZ) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW2_DSE_V(40_OHM) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW2_SRE_V(SLOW));
    // Pad KEY_ROW2 is involved in Daisy Chain.
    // Input Select Register:
    // IOMUXC_KEY_ROW2_SELECT_INPUT(0x020E075C)
    //   DAISY [1:0] - MUX Mode Select Field Reset: KEY_ROW2_ALT0
    //                 Selecting Pads Involved in Daisy Chain.
    //     KEY_ROW2_ALT0 (0) - Select signal kpp KEY_ROW2 as input from pad KEY_ROW2(ALT0).
    //     LCD_DATA13_ALT1 (1) - Select signal kpp KEY_ROW2 as input from pad LCD_DATA13(ALT1).
    //     SD1_DATA3_ALT2 (2) - Select signal kpp KEY_ROW2 as input from pad SD1_DATA3(ALT2).
    HW_IOMUXC_KEY_ROW2_SELECT_INPUT_WR(
            BF_IOMUXC_KEY_ROW2_SELECT_INPUT_DAISY_V(KEY_ROW2_ALT0));

    // Config kpp.KEY_ROW3 to pad KEY_ROW3(E21)
    // HW_IOMUXC_SW_MUX_CTL_PAD_KEY_ROW3_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW3_WR(0x0001B0B0);
    // HW_IOMUXC_KEY_ROW3_SELECT_INPUT_WR(0x00000000);
    // Mux Register:
    // IOMUXC_SW_MUX_CTL_PAD_KEY_ROW3(0x020E0198)
    //   SION [4] - Software Input On Field Reset: DISABLED
    //              Force the selected mux mode Input path no matter of MUX_MODE functionality.
    //     DISABLED (0) - Input Path is determined by functionality of the selected mux mode (regular).
    //     ENABLED (1) - Force input path of pad.
    //   MUX_MODE [2:0] - MUX Mode Select Field Reset: ALT5
    //                    Select iomux modes to be used for pad.
    //     ALT0 (0) - Select instance: kpp signal: KEY_ROW3
    //     ALT1 (1) - Select instance: audmux signal: AUD6_RXC
    //     ALT2 (2) - Select instance: lcd signal: LCD_DATA07
    //     ALT3 (3) - Select instance: eim signal: EIM_AD07
    //     ALT4 (4) - Select instance: usdhc4 signal: SD4_DATA7
    //     ALT5 (5) - Select instance: gpio3 signal: GPIO3_IO31
    //     ALT6 (6) - Select instance: usdhc1 signal: SD1_VSELECT
    HW_IOMUXC_SW_MUX_CTL_PAD_KEY_ROW3_WR(
            BF_IOMUXC_SW_MUX_CTL_PAD_KEY_ROW3_SION_V(DISABLED) | 
            BF_IOMUXC_SW_MUX_CTL_PAD_KEY_ROW3_MUX_MODE_V(ALT0));
    // Pad Control Register:
    // IOMUXC_SW_PAD_CTL_PAD_KEY_ROW3(0x020E04A0)
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
    HW_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW3_WR(
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW3_LVE_V(DISABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW3_HYS_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW3_PUS_V(100K_OHM_PU) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW3_PUE_V(PULL) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW3_PKE_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW3_ODE_V(DISABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW3_SPEED_V(100MHZ) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW3_DSE_V(40_OHM) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW3_SRE_V(SLOW));
    // Pad KEY_ROW3 is involved in Daisy Chain.
    // Input Select Register:
    // IOMUXC_KEY_ROW3_SELECT_INPUT(0x020E0760)
    //   DAISY [1:0] - MUX Mode Select Field Reset: KEY_ROW3_ALT0
    //                 Selecting Pads Involved in Daisy Chain.
    //     KEY_ROW3_ALT0 (0) - Select signal kpp KEY_ROW3 as input from pad KEY_ROW3(ALT0).
    //     LCD_DATA15_ALT1 (1) - Select signal kpp KEY_ROW3 as input from pad LCD_DATA15(ALT1).
    //     SD1_DATA5_ALT2 (2) - Select signal kpp KEY_ROW3 as input from pad SD1_DATA5(ALT2).
    HW_IOMUXC_KEY_ROW3_SELECT_INPUT_WR(
            BF_IOMUXC_KEY_ROW3_SELECT_INPUT_DAISY_V(KEY_ROW3_ALT0));
}
