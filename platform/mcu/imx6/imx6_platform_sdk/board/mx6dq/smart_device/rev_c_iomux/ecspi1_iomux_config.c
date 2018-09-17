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

// File: ecspi1_iomux_config.c

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

// Function to configure IOMUXC for ecspi1 module.
void ecspi1_iomux_config(void)
{
    // Config ecspi1.ECSPI1_MISO to pad KEY_COL1(U7)
    // HW_IOMUXC_SW_MUX_CTL_PAD_KEY_COL1_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_KEY_COL1_WR(0x0001B0B0);
    // HW_IOMUXC_ECSPI1_MISO_SELECT_INPUT_WR(0x00000002);
    // Mux Register:
    // IOMUXC_SW_MUX_CTL_PAD_KEY_COL1(0x020E0200)
    //   SION [4] - Software Input On Field Reset: DISABLED
    //              Force the selected mux mode Input path no matter of MUX_MODE functionality.
    //     DISABLED (0) - Input Path is determined by functionality of the selected mux mode (regular).
    //     ENABLED (1) - Force input path of pad.
    //   MUX_MODE [2:0] - MUX Mode Select Field Reset: ALT5
    //                    Select iomux modes to be used for pad.
    //     ALT0 (0) - Select instance: ecspi1 signal: ECSPI1_MISO
    //     ALT1 (1) - Select instance: enet signal: ENET_MDIO
    //     ALT2 (2) - Select instance: audmux signal: AUD5_TXFS
    //     ALT3 (3) - Select instance: kpp signal: KEY_COL1
    //     ALT4 (4) - Select instance: uart5 signal: UART5_TX_DATA
    //     ALT5 (5) - Select instance: gpio4 signal: GPIO4_IO08
    //     ALT6 (6) - Select instance: usdhc1 signal: SD1_VSELECT
    HW_IOMUXC_SW_MUX_CTL_PAD_KEY_COL1_WR(
            BF_IOMUXC_SW_MUX_CTL_PAD_KEY_COL1_SION_V(DISABLED) | 
            BF_IOMUXC_SW_MUX_CTL_PAD_KEY_COL1_MUX_MODE_V(ALT0));
    // Pad Control Register:
    // IOMUXC_SW_PAD_CTL_PAD_KEY_COL1(0x020E05D0)
    //   HYS [16] - Hysteresis Enable Field Reset: ENABLED
    //     DISABLED (0) - CMOS input
    //     ENABLED (1) - Schmitt trigger input
    //   PUS [15:14] - Pull Up / Down Config. Field Reset: 100K_OHM_PU
    //     100K_OHM_PD (0) - 100K Ohm Pull Down
    //     47K_OHM_PU (1) - 47K Ohm Pull Up
    //     100K_OHM_PU (2) - 100K Ohm Pull Up
    //     22K_OHM_PU (3) - 22K Ohm Pull Up
    //   PUE [13] - Pull / Keep Select Field Reset: PULL
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
    //     TBD (0) - TBD
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
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_COL1_HYS_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_COL1_PUS_V(100K_OHM_PU) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_COL1_PUE_V(PULL) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_COL1_PKE_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_COL1_ODE_V(DISABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_COL1_SPEED_V(100MHZ) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_COL1_DSE_V(40_OHM) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_COL1_SRE_V(SLOW));
    // Pad KEY_COL1 is involved in Daisy Chain.
    // Input Select Register:
    // IOMUXC_ECSPI1_MISO_SELECT_INPUT(0x020E07F8)
    //   DAISY [1:0] - MUX Mode Select Field Reset: EIM_DATA17_ALT1
    //                 Selecting Pads Involved in Daisy Chain.
    //     EIM_DATA17_ALT1 (0) - Select signal ecspi1 ECSPI1_MISO as input from pad EIM_DATA17(ALT1).
    //     DISP0_DATA22_ALT2 (1) - Select signal ecspi1 ECSPI1_MISO as input from pad DISP0_DATA22(ALT2).
    //     KEY_COL1_ALT0 (2) - Select signal ecspi1 ECSPI1_MISO as input from pad KEY_COL1(ALT0).
    //     CSI0_DATA06_ALT2 (3) - Select signal ecspi1 ECSPI1_MISO as input from pad CSI0_DATA06(ALT2).
    HW_IOMUXC_ECSPI1_MISO_SELECT_INPUT_WR(
            BF_IOMUXC_ECSPI1_MISO_SELECT_INPUT_DAISY_V(KEY_COL1_ALT0));

    // Config ecspi1.ECSPI1_MOSI to pad KEY_ROW0(V6)
    // HW_IOMUXC_SW_MUX_CTL_PAD_KEY_ROW0_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW0_WR(0x0001B0B0);
    // HW_IOMUXC_ECSPI1_MOSI_SELECT_INPUT_WR(0x00000002);
    // Mux Register:
    // IOMUXC_SW_MUX_CTL_PAD_KEY_ROW0(0x020E01FC)
    //   SION [4] - Software Input On Field Reset: DISABLED
    //              Force the selected mux mode Input path no matter of MUX_MODE functionality.
    //     DISABLED (0) - Input Path is determined by functionality of the selected mux mode (regular).
    //     ENABLED (1) - Force input path of pad.
    //   MUX_MODE [2:0] - MUX Mode Select Field Reset: ALT5
    //                    Select iomux modes to be used for pad.
    //     ALT0 (0) - Select instance: ecspi1 signal: ECSPI1_MOSI
    //     ALT1 (1) - Select instance: enet signal: ENET_TX_DATA3
    //     ALT2 (2) - Select instance: audmux signal: AUD5_TXD
    //     ALT3 (3) - Select instance: kpp signal: KEY_ROW0
    //     ALT4 (4) - Select instance: uart4 signal: UART4_RX_DATA
    //     ALT5 (5) - Select instance: gpio4 signal: GPIO4_IO07
    //     ALT6 (6) - Select instance: dcic2 signal: DCIC2_OUT
    HW_IOMUXC_SW_MUX_CTL_PAD_KEY_ROW0_WR(
            BF_IOMUXC_SW_MUX_CTL_PAD_KEY_ROW0_SION_V(DISABLED) | 
            BF_IOMUXC_SW_MUX_CTL_PAD_KEY_ROW0_MUX_MODE_V(ALT0));
    // Pad Control Register:
    // IOMUXC_SW_PAD_CTL_PAD_KEY_ROW0(0x020E05CC)
    //   HYS [16] - Hysteresis Enable Field Reset: ENABLED
    //     DISABLED (0) - CMOS input
    //     ENABLED (1) - Schmitt trigger input
    //   PUS [15:14] - Pull Up / Down Config. Field Reset: 100K_OHM_PU
    //     100K_OHM_PD (0) - 100K Ohm Pull Down
    //     47K_OHM_PU (1) - 47K Ohm Pull Up
    //     100K_OHM_PU (2) - 100K Ohm Pull Up
    //     22K_OHM_PU (3) - 22K Ohm Pull Up
    //   PUE [13] - Pull / Keep Select Field Reset: PULL
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
    //     TBD (0) - TBD
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
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW0_HYS_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW0_PUS_V(100K_OHM_PU) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW0_PUE_V(PULL) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW0_PKE_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW0_ODE_V(DISABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW0_SPEED_V(100MHZ) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW0_DSE_V(40_OHM) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW0_SRE_V(SLOW));
    // Pad KEY_ROW0 is involved in Daisy Chain.
    // Input Select Register:
    // IOMUXC_ECSPI1_MOSI_SELECT_INPUT(0x020E07FC)
    //   DAISY [1:0] - MUX Mode Select Field Reset: EIM_DATA18_ALT1
    //                 Selecting Pads Involved in Daisy Chain.
    //     EIM_DATA18_ALT1 (0) - Select signal ecspi1 ECSPI1_MOSI as input from pad EIM_DATA18(ALT1).
    //     DISP0_DATA21_ALT2 (1) - Select signal ecspi1 ECSPI1_MOSI as input from pad DISP0_DATA21(ALT2).
    //     KEY_ROW0_ALT0 (2) - Select signal ecspi1 ECSPI1_MOSI as input from pad KEY_ROW0(ALT0).
    //     CSI0_DATA05_ALT2 (3) - Select signal ecspi1 ECSPI1_MOSI as input from pad CSI0_DATA05(ALT2).
    HW_IOMUXC_ECSPI1_MOSI_SELECT_INPUT_WR(
            BF_IOMUXC_ECSPI1_MOSI_SELECT_INPUT_DAISY_V(KEY_ROW0_ALT0));

    // Config ecspi1.ECSPI1_SCLK to pad KEY_COL0(W5)
    // HW_IOMUXC_SW_MUX_CTL_PAD_KEY_COL0_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_KEY_COL0_WR(0x0001B0B0);
    // HW_IOMUXC_ECSPI1_CSPI_CLK_IN_SELECT_INPUT_WR(0x00000002);
    // Mux Register:
    // IOMUXC_SW_MUX_CTL_PAD_KEY_COL0(0x020E01F8)
    //   SION [4] - Software Input On Field Reset: DISABLED
    //              Force the selected mux mode Input path no matter of MUX_MODE functionality.
    //     DISABLED (0) - Input Path is determined by functionality of the selected mux mode (regular).
    //     ENABLED (1) - Force input path of pad.
    //   MUX_MODE [2:0] - MUX Mode Select Field Reset: ALT5
    //                    Select iomux modes to be used for pad.
    //     ALT0 (0) - Select instance: ecspi1 signal: ECSPI1_SCLK
    //     ALT1 (1) - Select instance: enet signal: ENET_RX_DATA3
    //     ALT2 (2) - Select instance: audmux signal: AUD5_TXC
    //     ALT3 (3) - Select instance: kpp signal: KEY_COL0
    //     ALT4 (4) - Select instance: uart4 signal: UART4_TX_DATA
    //     ALT5 (5) - Select instance: gpio4 signal: GPIO4_IO06
    //     ALT6 (6) - Select instance: dcic1 signal: DCIC1_OUT
    HW_IOMUXC_SW_MUX_CTL_PAD_KEY_COL0_WR(
            BF_IOMUXC_SW_MUX_CTL_PAD_KEY_COL0_SION_V(DISABLED) | 
            BF_IOMUXC_SW_MUX_CTL_PAD_KEY_COL0_MUX_MODE_V(ALT0));
    // Pad Control Register:
    // IOMUXC_SW_PAD_CTL_PAD_KEY_COL0(0x020E05C8)
    //   HYS [16] - Hysteresis Enable Field Reset: ENABLED
    //     DISABLED (0) - CMOS input
    //     ENABLED (1) - Schmitt trigger input
    //   PUS [15:14] - Pull Up / Down Config. Field Reset: 100K_OHM_PU
    //     100K_OHM_PD (0) - 100K Ohm Pull Down
    //     47K_OHM_PU (1) - 47K Ohm Pull Up
    //     100K_OHM_PU (2) - 100K Ohm Pull Up
    //     22K_OHM_PU (3) - 22K Ohm Pull Up
    //   PUE [13] - Pull / Keep Select Field Reset: PULL
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
    //     TBD (0) - TBD
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
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_COL0_HYS_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_COL0_PUS_V(100K_OHM_PU) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_COL0_PUE_V(PULL) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_COL0_PKE_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_COL0_ODE_V(DISABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_COL0_SPEED_V(100MHZ) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_COL0_DSE_V(40_OHM) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_COL0_SRE_V(SLOW));
    // Pad KEY_COL0 is involved in Daisy Chain.
    // Input Select Register:
    // IOMUXC_ECSPI1_CSPI_CLK_IN_SELECT_INPUT(0x020E07F4)
    //   DAISY [1:0] - MUX Mode Select Field Reset: EIM_DATA16_ALT1
    //                 Selecting Pads Involved in Daisy Chain.
    //     EIM_DATA16_ALT1 (0) - Select signal ecspi1 ECSPI1_SCLK as input from pad EIM_DATA16(ALT1).
    //     DISP0_DATA20_ALT2 (1) - Select signal ecspi1 ECSPI1_SCLK as input from pad DISP0_DATA20(ALT2).
    //     KEY_COL0_ALT0 (2) - Select signal ecspi1 ECSPI1_SCLK as input from pad KEY_COL0(ALT0).
    //     CSI0_DATA04_ALT2 (3) - Select signal ecspi1 ECSPI1_SCLK as input from pad CSI0_DATA04(ALT2).
    HW_IOMUXC_ECSPI1_CSPI_CLK_IN_SELECT_INPUT_WR(
            BF_IOMUXC_ECSPI1_CSPI_CLK_IN_SELECT_INPUT_DAISY_V(KEY_COL0_ALT0));

    // Config ecspi1.ECSPI1_SS0 to pad KEY_ROW1(U6)
    // HW_IOMUXC_SW_MUX_CTL_PAD_KEY_ROW1_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW1_WR(0x0001B0B0);
    // HW_IOMUXC_ECSPI1_SS0_SELECT_INPUT_WR(0x00000002);
    // Mux Register:
    // IOMUXC_SW_MUX_CTL_PAD_KEY_ROW1(0x020E0204)
    //   SION [4] - Software Input On Field Reset: DISABLED
    //              Force the selected mux mode Input path no matter of MUX_MODE functionality.
    //     DISABLED (0) - Input Path is determined by functionality of the selected mux mode (regular).
    //     ENABLED (1) - Force input path of pad.
    //   MUX_MODE [2:0] - MUX Mode Select Field Reset: ALT5
    //                    Select iomux modes to be used for pad.
    //     ALT0 (0) - Select instance: ecspi1 signal: ECSPI1_SS0
    //     ALT1 (1) - Select instance: enet signal: ENET_COL
    //     ALT2 (2) - Select instance: audmux signal: AUD5_RXD
    //     ALT3 (3) - Select instance: kpp signal: KEY_ROW1
    //     ALT4 (4) - Select instance: uart5 signal: UART5_RX_DATA
    //     ALT5 (5) - Select instance: gpio4 signal: GPIO4_IO09
    //     ALT6 (6) - Select instance: usdhc2 signal: SD2_VSELECT
    HW_IOMUXC_SW_MUX_CTL_PAD_KEY_ROW1_WR(
            BF_IOMUXC_SW_MUX_CTL_PAD_KEY_ROW1_SION_V(DISABLED) | 
            BF_IOMUXC_SW_MUX_CTL_PAD_KEY_ROW1_MUX_MODE_V(ALT0));
    // Pad Control Register:
    // IOMUXC_SW_PAD_CTL_PAD_KEY_ROW1(0x020E05D4)
    //   HYS [16] - Hysteresis Enable Field Reset: ENABLED
    //     DISABLED (0) - CMOS input
    //     ENABLED (1) - Schmitt trigger input
    //   PUS [15:14] - Pull Up / Down Config. Field Reset: 100K_OHM_PU
    //     100K_OHM_PD (0) - 100K Ohm Pull Down
    //     47K_OHM_PU (1) - 47K Ohm Pull Up
    //     100K_OHM_PU (2) - 100K Ohm Pull Up
    //     22K_OHM_PU (3) - 22K Ohm Pull Up
    //   PUE [13] - Pull / Keep Select Field Reset: PULL
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
    //     TBD (0) - TBD
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
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW1_HYS_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW1_PUS_V(100K_OHM_PU) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW1_PUE_V(PULL) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW1_PKE_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW1_ODE_V(DISABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW1_SPEED_V(100MHZ) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW1_DSE_V(40_OHM) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW1_SRE_V(SLOW));
    // Pad KEY_ROW1 is involved in Daisy Chain.
    // Input Select Register:
    // IOMUXC_ECSPI1_SS0_SELECT_INPUT(0x020E0800)
    //   DAISY [1:0] - MUX Mode Select Field Reset: EIM_EB2_ALT1
    //                 Selecting Pads Involved in Daisy Chain.
    //     EIM_EB2_ALT1 (0) - Select signal ecspi1 ECSPI1_SS0 as input from pad EIM_EB2(ALT1).
    //     DISP0_DATA23_ALT2 (1) - Select signal ecspi1 ECSPI1_SS0 as input from pad DISP0_DATA23(ALT2).
    //     KEY_ROW1_ALT0 (2) - Select signal ecspi1 ECSPI1_SS0 as input from pad KEY_ROW1(ALT0).
    //     CSI0_DATA07_ALT2 (3) - Select signal ecspi1 ECSPI1_SS0 as input from pad CSI0_DATA07(ALT2).
    HW_IOMUXC_ECSPI1_SS0_SELECT_INPUT_WR(
            BF_IOMUXC_ECSPI1_SS0_SELECT_INPUT_DAISY_V(KEY_ROW1_ALT0));
}
