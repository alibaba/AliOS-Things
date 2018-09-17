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

// File: usdhc3_iomux_config.c

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

// Function to configure IOMUXC for usdhc3 module.
void usdhc3_iomux_config(void)
{
    // Config usdhc3.SD3_CLK to pad SD3_CLK(D14)
    // HW_IOMUXC_SW_MUX_CTL_PAD_SD3_CLK_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_SD3_CLK_WR(0x0001B0B0);
    // HW_IOMUXC_USDHC3_CARD_CLK_IN_SELECT_INPUT_WR(0x00000001);
    // Mux Register:
    // IOMUXC_SW_MUX_CTL_PAD_SD3_CLK(0x020E030C)
    //   SION [4] - Software Input On Field Reset: DISABLED
    //              Force the selected mux mode Input path no matter of MUX_MODE functionality.
    //     DISABLED (0) - Input Path is determined by functionality of the selected mux mode (regular).
    //     ENABLED (1) - Force input path of pad.
    //   MUX_MODE [2:0] - MUX Mode Select Field Reset: ALT5
    //                    Select iomux modes to be used for pad.
    //     ALT0 (0) - Select instance: usdhc3 signal: SD3_CLK
    //     ALT1 (1) - Select instance: uart2 signal: UART2_RTS_B
    //     ALT2 (2) - Select instance: flexcan1 signal: FLEXCAN1_RX
    //     ALT5 (5) - Select instance: gpio7 signal: GPIO7_IO03
    HW_IOMUXC_SW_MUX_CTL_PAD_SD3_CLK_WR(
            BF_IOMUXC_SW_MUX_CTL_PAD_SD3_CLK_SION_V(DISABLED) | 
            BF_IOMUXC_SW_MUX_CTL_PAD_SD3_CLK_MUX_MODE_V(ALT0));
    // Pad Control Register:
    // IOMUXC_SW_PAD_CTL_PAD_SD3_CLK(0x020E06F4)
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
    HW_IOMUXC_SW_PAD_CTL_PAD_SD3_CLK_WR(
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_CLK_HYS_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_CLK_PUS_V(100K_OHM_PU) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_CLK_PUE_V(PULL) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_CLK_PKE_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_CLK_ODE_V(DISABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_CLK_SPEED_V(100MHZ) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_CLK_DSE_V(40_OHM) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_CLK_SRE_V(SLOW));
    // Pad SD3_CLK is involved in Daisy Chain.
    // Input Select Register:
    // IOMUXC_USDHC3_CARD_CLK_IN_SELECT_INPUT(0x020E0934)
    //   DAISY [0] - MUX Mode Select Field Reset: RESERVED0
    //               Selecting Pads Involved in Daisy Chain.
    //     RESERVED0 (0) - This field value is reserved.
    //     SD3_CLK_ALT0 (1) - Select signal usdhc3 SD3_CLK as input from pad SD3_CLK(ALT0).
    HW_IOMUXC_USDHC3_CARD_CLK_IN_SELECT_INPUT_WR(
            BF_IOMUXC_USDHC3_CARD_CLK_IN_SELECT_INPUT_DAISY_V(SD3_CLK_ALT0));

    // Config usdhc3.SD3_CMD to pad SD3_CMD(B13)
    // HW_IOMUXC_SW_MUX_CTL_PAD_SD3_CMD_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_SD3_CMD_WR(0x0001B0B0);
    // Mux Register:
    // IOMUXC_SW_MUX_CTL_PAD_SD3_CMD(0x020E0310)
    //   SION [4] - Software Input On Field Reset: DISABLED
    //              Force the selected mux mode Input path no matter of MUX_MODE functionality.
    //     DISABLED (0) - Input Path is determined by functionality of the selected mux mode (regular).
    //     ENABLED (1) - Force input path of pad.
    //   MUX_MODE [2:0] - MUX Mode Select Field Reset: ALT5
    //                    Select iomux modes to be used for pad.
    //     ALT0 (0) - Select instance: usdhc3 signal: SD3_CMD
    //     ALT1 (1) - Select instance: uart2 signal: UART2_CTS_B
    //     ALT2 (2) - Select instance: flexcan1 signal: FLEXCAN1_TX
    //     ALT5 (5) - Select instance: gpio7 signal: GPIO7_IO02
    HW_IOMUXC_SW_MUX_CTL_PAD_SD3_CMD_WR(
            BF_IOMUXC_SW_MUX_CTL_PAD_SD3_CMD_SION_V(DISABLED) | 
            BF_IOMUXC_SW_MUX_CTL_PAD_SD3_CMD_MUX_MODE_V(ALT0));
    // Pad Control Register:
    // IOMUXC_SW_PAD_CTL_PAD_SD3_CMD(0x020E06F8)
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
    HW_IOMUXC_SW_PAD_CTL_PAD_SD3_CMD_WR(
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_CMD_HYS_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_CMD_PUS_V(100K_OHM_PU) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_CMD_PUE_V(PULL) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_CMD_PKE_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_CMD_ODE_V(DISABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_CMD_SPEED_V(100MHZ) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_CMD_DSE_V(40_OHM) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_CMD_SRE_V(SLOW));

    // Config usdhc3.SD3_DATA0 to pad SD3_DATA0(E14)
    // HW_IOMUXC_SW_MUX_CTL_PAD_SD3_DATA0_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA0_WR(0x0001B0B0);
    // Mux Register:
    // IOMUXC_SW_MUX_CTL_PAD_SD3_DATA0(0x020E0314)
    //   SION [4] - Software Input On Field Reset: DISABLED
    //              Force the selected mux mode Input path no matter of MUX_MODE functionality.
    //     DISABLED (0) - Input Path is determined by functionality of the selected mux mode (regular).
    //     ENABLED (1) - Force input path of pad.
    //   MUX_MODE [2:0] - MUX Mode Select Field Reset: ALT5
    //                    Select iomux modes to be used for pad.
    //     ALT0 (0) - Select instance: usdhc3 signal: SD3_DATA0
    //     ALT1 (1) - Select instance: uart1 signal: UART1_CTS_B
    //     ALT2 (2) - Select instance: flexcan2 signal: FLEXCAN2_TX
    //     ALT5 (5) - Select instance: gpio7 signal: GPIO7_IO04
    HW_IOMUXC_SW_MUX_CTL_PAD_SD3_DATA0_WR(
            BF_IOMUXC_SW_MUX_CTL_PAD_SD3_DATA0_SION_V(DISABLED) | 
            BF_IOMUXC_SW_MUX_CTL_PAD_SD3_DATA0_MUX_MODE_V(ALT0));
    // Pad Control Register:
    // IOMUXC_SW_PAD_CTL_PAD_SD3_DATA0(0x020E06FC)
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
    HW_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA0_WR(
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA0_HYS_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA0_PUS_V(100K_OHM_PU) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA0_PUE_V(PULL) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA0_PKE_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA0_ODE_V(DISABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA0_SPEED_V(100MHZ) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA0_DSE_V(40_OHM) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA0_SRE_V(SLOW));

    // Config usdhc3.SD3_DATA1 to pad SD3_DATA1(F14)
    // HW_IOMUXC_SW_MUX_CTL_PAD_SD3_DATA1_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA1_WR(0x0001B0B0);
    // Mux Register:
    // IOMUXC_SW_MUX_CTL_PAD_SD3_DATA1(0x020E0318)
    //   SION [4] - Software Input On Field Reset: DISABLED
    //              Force the selected mux mode Input path no matter of MUX_MODE functionality.
    //     DISABLED (0) - Input Path is determined by functionality of the selected mux mode (regular).
    //     ENABLED (1) - Force input path of pad.
    //   MUX_MODE [2:0] - MUX Mode Select Field Reset: ALT5
    //                    Select iomux modes to be used for pad.
    //     ALT0 (0) - Select instance: usdhc3 signal: SD3_DATA1
    //     ALT1 (1) - Select instance: uart1 signal: UART1_RTS_B
    //     ALT2 (2) - Select instance: flexcan2 signal: FLEXCAN2_RX
    //     ALT5 (5) - Select instance: gpio7 signal: GPIO7_IO05
    HW_IOMUXC_SW_MUX_CTL_PAD_SD3_DATA1_WR(
            BF_IOMUXC_SW_MUX_CTL_PAD_SD3_DATA1_SION_V(DISABLED) | 
            BF_IOMUXC_SW_MUX_CTL_PAD_SD3_DATA1_MUX_MODE_V(ALT0));
    // Pad Control Register:
    // IOMUXC_SW_PAD_CTL_PAD_SD3_DATA1(0x020E0700)
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
    HW_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA1_WR(
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA1_HYS_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA1_PUS_V(100K_OHM_PU) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA1_PUE_V(PULL) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA1_PKE_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA1_ODE_V(DISABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA1_SPEED_V(100MHZ) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA1_DSE_V(40_OHM) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA1_SRE_V(SLOW));

    // Config usdhc3.SD3_DATA2 to pad SD3_DATA2(A15)
    // HW_IOMUXC_SW_MUX_CTL_PAD_SD3_DATA2_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA2_WR(0x0001B0B0);
    // Mux Register:
    // IOMUXC_SW_MUX_CTL_PAD_SD3_DATA2(0x020E031C)
    //   SION [4] - Software Input On Field Reset: DISABLED
    //              Force the selected mux mode Input path no matter of MUX_MODE functionality.
    //     DISABLED (0) - Input Path is determined by functionality of the selected mux mode (regular).
    //     ENABLED (1) - Force input path of pad.
    //   MUX_MODE [2:0] - MUX Mode Select Field Reset: ALT5
    //                    Select iomux modes to be used for pad.
    //     ALT0 (0) - Select instance: usdhc3 signal: SD3_DATA2
    //     ALT5 (5) - Select instance: gpio7 signal: GPIO7_IO06
    HW_IOMUXC_SW_MUX_CTL_PAD_SD3_DATA2_WR(
            BF_IOMUXC_SW_MUX_CTL_PAD_SD3_DATA2_SION_V(DISABLED) | 
            BF_IOMUXC_SW_MUX_CTL_PAD_SD3_DATA2_MUX_MODE_V(ALT0));
    // Pad Control Register:
    // IOMUXC_SW_PAD_CTL_PAD_SD3_DATA2(0x020E0704)
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
    HW_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA2_WR(
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA2_HYS_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA2_PUS_V(100K_OHM_PU) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA2_PUE_V(PULL) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA2_PKE_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA2_ODE_V(DISABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA2_SPEED_V(100MHZ) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA2_DSE_V(40_OHM) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA2_SRE_V(SLOW));

    // Config usdhc3.SD3_DATA3 to pad SD3_DATA3(B15)
    // HW_IOMUXC_SW_MUX_CTL_PAD_SD3_DATA3_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA3_WR(0x0001B0B0);
    // Mux Register:
    // IOMUXC_SW_MUX_CTL_PAD_SD3_DATA3(0x020E0320)
    //   SION [4] - Software Input On Field Reset: DISABLED
    //              Force the selected mux mode Input path no matter of MUX_MODE functionality.
    //     DISABLED (0) - Input Path is determined by functionality of the selected mux mode (regular).
    //     ENABLED (1) - Force input path of pad.
    //   MUX_MODE [2:0] - MUX Mode Select Field Reset: ALT5
    //                    Select iomux modes to be used for pad.
    //     ALT0 (0) - Select instance: usdhc3 signal: SD3_DATA3
    //     ALT1 (1) - Select instance: uart3 signal: UART3_CTS_B
    //     ALT5 (5) - Select instance: gpio7 signal: GPIO7_IO07
    HW_IOMUXC_SW_MUX_CTL_PAD_SD3_DATA3_WR(
            BF_IOMUXC_SW_MUX_CTL_PAD_SD3_DATA3_SION_V(DISABLED) | 
            BF_IOMUXC_SW_MUX_CTL_PAD_SD3_DATA3_MUX_MODE_V(ALT0));
    // Pad Control Register:
    // IOMUXC_SW_PAD_CTL_PAD_SD3_DATA3(0x020E0708)
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
    HW_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA3_WR(
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA3_HYS_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA3_PUS_V(100K_OHM_PU) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA3_PUE_V(PULL) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA3_PKE_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA3_ODE_V(DISABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA3_SPEED_V(100MHZ) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA3_DSE_V(40_OHM) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA3_SRE_V(SLOW));

    // Config usdhc3.SD3_DATA4 to pad SD3_DATA4(D13)
    // HW_IOMUXC_SW_MUX_CTL_PAD_SD3_DATA4_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA4_WR(0x0001B0B0);
    // Mux Register:
    // IOMUXC_SW_MUX_CTL_PAD_SD3_DATA4(0x020E0324)
    //   SION [4] - Software Input On Field Reset: DISABLED
    //              Force the selected mux mode Input path no matter of MUX_MODE functionality.
    //     DISABLED (0) - Input Path is determined by functionality of the selected mux mode (regular).
    //     ENABLED (1) - Force input path of pad.
    //   MUX_MODE [2:0] - MUX Mode Select Field Reset: ALT5
    //                    Select iomux modes to be used for pad.
    //     ALT0 (0) - Select instance: usdhc3 signal: SD3_DATA4
    //     ALT1 (1) - Select instance: uart2 signal: UART2_RX_DATA
    //     ALT5 (5) - Select instance: gpio7 signal: GPIO7_IO01
    HW_IOMUXC_SW_MUX_CTL_PAD_SD3_DATA4_WR(
            BF_IOMUXC_SW_MUX_CTL_PAD_SD3_DATA4_SION_V(DISABLED) | 
            BF_IOMUXC_SW_MUX_CTL_PAD_SD3_DATA4_MUX_MODE_V(ALT0));
    // Pad Control Register:
    // IOMUXC_SW_PAD_CTL_PAD_SD3_DATA4(0x020E070C)
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
    HW_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA4_WR(
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA4_HYS_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA4_PUS_V(100K_OHM_PU) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA4_PUE_V(PULL) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA4_PKE_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA4_ODE_V(DISABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA4_SPEED_V(100MHZ) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA4_DSE_V(40_OHM) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA4_SRE_V(SLOW));

    // Config usdhc3.SD3_DATA5 to pad SD3_DATA5(C13)
    // HW_IOMUXC_SW_MUX_CTL_PAD_SD3_DATA5_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA5_WR(0x0001B0B0);
    // Mux Register:
    // IOMUXC_SW_MUX_CTL_PAD_SD3_DATA5(0x020E0328)
    //   SION [4] - Software Input On Field Reset: DISABLED
    //              Force the selected mux mode Input path no matter of MUX_MODE functionality.
    //     DISABLED (0) - Input Path is determined by functionality of the selected mux mode (regular).
    //     ENABLED (1) - Force input path of pad.
    //   MUX_MODE [2:0] - MUX Mode Select Field Reset: ALT5
    //                    Select iomux modes to be used for pad.
    //     ALT0 (0) - Select instance: usdhc3 signal: SD3_DATA5
    //     ALT1 (1) - Select instance: uart2 signal: UART2_TX_DATA
    //     ALT5 (5) - Select instance: gpio7 signal: GPIO7_IO00
    HW_IOMUXC_SW_MUX_CTL_PAD_SD3_DATA5_WR(
            BF_IOMUXC_SW_MUX_CTL_PAD_SD3_DATA5_SION_V(DISABLED) | 
            BF_IOMUXC_SW_MUX_CTL_PAD_SD3_DATA5_MUX_MODE_V(ALT0));
    // Pad Control Register:
    // IOMUXC_SW_PAD_CTL_PAD_SD3_DATA5(0x020E0710)
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
    HW_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA5_WR(
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA5_HYS_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA5_PUS_V(100K_OHM_PU) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA5_PUE_V(PULL) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA5_PKE_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA5_ODE_V(DISABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA5_SPEED_V(100MHZ) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA5_DSE_V(40_OHM) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA5_SRE_V(SLOW));

    // Config usdhc3.SD3_DATA6 to pad SD3_DATA6(E13)
    // HW_IOMUXC_SW_MUX_CTL_PAD_SD3_DATA6_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA6_WR(0x0001B0B0);
    // Mux Register:
    // IOMUXC_SW_MUX_CTL_PAD_SD3_DATA6(0x020E032C)
    //   SION [4] - Software Input On Field Reset: DISABLED
    //              Force the selected mux mode Input path no matter of MUX_MODE functionality.
    //     DISABLED (0) - Input Path is determined by functionality of the selected mux mode (regular).
    //     ENABLED (1) - Force input path of pad.
    //   MUX_MODE [2:0] - MUX Mode Select Field Reset: ALT5
    //                    Select iomux modes to be used for pad.
    //     ALT0 (0) - Select instance: usdhc3 signal: SD3_DATA6
    //     ALT1 (1) - Select instance: uart1 signal: UART1_RX_DATA
    //     ALT5 (5) - Select instance: gpio6 signal: GPIO6_IO18
    HW_IOMUXC_SW_MUX_CTL_PAD_SD3_DATA6_WR(
            BF_IOMUXC_SW_MUX_CTL_PAD_SD3_DATA6_SION_V(DISABLED) | 
            BF_IOMUXC_SW_MUX_CTL_PAD_SD3_DATA6_MUX_MODE_V(ALT0));
    // Pad Control Register:
    // IOMUXC_SW_PAD_CTL_PAD_SD3_DATA6(0x020E0714)
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
    HW_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA6_WR(
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA6_HYS_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA6_PUS_V(100K_OHM_PU) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA6_PUE_V(PULL) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA6_PKE_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA6_ODE_V(DISABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA6_SPEED_V(100MHZ) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA6_DSE_V(40_OHM) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA6_SRE_V(SLOW));

    // Config usdhc3.SD3_DATA7 to pad SD3_DATA7(F13)
    // HW_IOMUXC_SW_MUX_CTL_PAD_SD3_DATA7_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA7_WR(0x0001B0B0);
    // Mux Register:
    // IOMUXC_SW_MUX_CTL_PAD_SD3_DATA7(0x020E0330)
    //   SION [4] - Software Input On Field Reset: DISABLED
    //              Force the selected mux mode Input path no matter of MUX_MODE functionality.
    //     DISABLED (0) - Input Path is determined by functionality of the selected mux mode (regular).
    //     ENABLED (1) - Force input path of pad.
    //   MUX_MODE [2:0] - MUX Mode Select Field Reset: ALT5
    //                    Select iomux modes to be used for pad.
    //     ALT0 (0) - Select instance: usdhc3 signal: SD3_DATA7
    //     ALT1 (1) - Select instance: uart1 signal: UART1_TX_DATA
    //     ALT5 (5) - Select instance: gpio6 signal: GPIO6_IO17
    HW_IOMUXC_SW_MUX_CTL_PAD_SD3_DATA7_WR(
            BF_IOMUXC_SW_MUX_CTL_PAD_SD3_DATA7_SION_V(DISABLED) | 
            BF_IOMUXC_SW_MUX_CTL_PAD_SD3_DATA7_MUX_MODE_V(ALT0));
    // Pad Control Register:
    // IOMUXC_SW_PAD_CTL_PAD_SD3_DATA7(0x020E0718)
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
    HW_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA7_WR(
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA7_HYS_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA7_PUS_V(100K_OHM_PU) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA7_PUE_V(PULL) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA7_PKE_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA7_ODE_V(DISABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA7_SPEED_V(100MHZ) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA7_DSE_V(40_OHM) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA7_SRE_V(SLOW));
}
