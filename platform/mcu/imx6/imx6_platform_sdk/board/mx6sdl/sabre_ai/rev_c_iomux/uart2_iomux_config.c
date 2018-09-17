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

// File: uart2_iomux_config.c

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

// Function to configure IOMUXC for uart2 module.
void uart2_iomux_config(void)
{
    // Config uart2.UART2_CTS_B to pad SD4_DATA6(B20)
    // HW_IOMUXC_SW_MUX_CTL_PAD_SD4_DATA6_WR(0x00000002);
    // HW_IOMUXC_SW_PAD_CTL_PAD_SD4_DATA6_WR(0x0001B0B0);
    // HW_IOMUXC_UART2_UART_RTS_B_SELECT_INPUT_WR(0x00000004);
    // Mux Register:
    // IOMUXC_SW_MUX_CTL_PAD_SD4_DATA6(0x020E0358)
    //   SION [4] - Software Input On Field Reset: DISABLED
    //              Force the selected mux mode Input path no matter of MUX_MODE functionality.
    //     DISABLED (0) - Input Path is determined by functionality of the selected mux mode (regular).
    //     ENABLED (1) - Force input path of pad.
    //   MUX_MODE [2:0] - MUX Mode Select Field Reset: ALT5
    //                    Select iomux modes to be used for pad.
    //     ALT1 (1) - Select instance: usdhc4 signal: SD4_DATA6
    //     ALT2 (2) - Select instance: uart2 signal: UART2_CTS_B
    //     ALT5 (5) - Select instance: gpio2 signal: GPIO2_IO14
    HW_IOMUXC_SW_MUX_CTL_PAD_SD4_DATA6_WR(
            BF_IOMUXC_SW_MUX_CTL_PAD_SD4_DATA6_SION_V(DISABLED) | 
            BF_IOMUXC_SW_MUX_CTL_PAD_SD4_DATA6_MUX_MODE_V(ALT2));
    // Pad Control Register:
    // IOMUXC_SW_PAD_CTL_PAD_SD4_DATA6(0x020E0740)
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
    HW_IOMUXC_SW_PAD_CTL_PAD_SD4_DATA6_WR(
            BF_IOMUXC_SW_PAD_CTL_PAD_SD4_DATA6_HYS_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD4_DATA6_PUS_V(100K_OHM_PU) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD4_DATA6_PUE_V(PULL) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD4_DATA6_PKE_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD4_DATA6_ODE_V(DISABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD4_DATA6_SPEED_V(100MHZ) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD4_DATA6_DSE_V(40_OHM) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD4_DATA6_SRE_V(SLOW));
    // Pad SD4_DATA6 is involved in Daisy Chain.
    // Input Select Register:
    // IOMUXC_UART2_UART_RTS_B_SELECT_INPUT(0x020E0900)
    //   DAISY [2:0] - MUX Mode Select Field Reset: EIM_DATA28_ALT4
    //                 Selecting Pads Involved in Daisy Chain.
    //     EIM_DATA28_ALT4 (0) - Select signal uart2 UART2_CTS_B as input from pad EIM_DATA28(ALT4).
    //     EIM_DATA29_ALT4 (1) - Select signal uart2 UART2_RTS_B as input from pad EIM_DATA29(ALT4).
    //     SD3_CLK_ALT1 (2) - Select signal uart2 UART2_RTS_B as input from pad SD3_CLK(ALT1).
    //     SD3_CMD_ALT1 (3) - Select signal uart2 UART2_CTS_B as input from pad SD3_CMD(ALT1).
    //     SD4_DATA5_ALT2 (4) - Select signal uart2 UART2_RTS_B as input from pad SD4_DATA5(ALT2).
    //     SD4_DATA6_ALT2 (5) - Select signal uart2 UART2_CTS_B as input from pad SD4_DATA6(ALT2).
    HW_IOMUXC_UART2_UART_RTS_B_SELECT_INPUT_WR(
            BF_IOMUXC_UART2_UART_RTS_B_SELECT_INPUT_DAISY_V(SD4_DATA5_ALT2));

    // Config uart2.UART2_RTS_B to pad SD4_DATA5(C19)
    // HW_IOMUXC_SW_MUX_CTL_PAD_SD4_DATA5_WR(0x00000002);
    // HW_IOMUXC_SW_PAD_CTL_PAD_SD4_DATA5_WR(0x0001B0B0);
    // HW_IOMUXC_UART2_UART_RTS_B_SELECT_INPUT_WR(0x00000004);
    // Mux Register:
    // IOMUXC_SW_MUX_CTL_PAD_SD4_DATA5(0x020E0354)
    //   SION [4] - Software Input On Field Reset: DISABLED
    //              Force the selected mux mode Input path no matter of MUX_MODE functionality.
    //     DISABLED (0) - Input Path is determined by functionality of the selected mux mode (regular).
    //     ENABLED (1) - Force input path of pad.
    //   MUX_MODE [2:0] - MUX Mode Select Field Reset: ALT5
    //                    Select iomux modes to be used for pad.
    //     ALT1 (1) - Select instance: usdhc4 signal: SD4_DATA5
    //     ALT2 (2) - Select instance: uart2 signal: UART2_RTS_B
    //     ALT5 (5) - Select instance: gpio2 signal: GPIO2_IO13
    HW_IOMUXC_SW_MUX_CTL_PAD_SD4_DATA5_WR(
            BF_IOMUXC_SW_MUX_CTL_PAD_SD4_DATA5_SION_V(DISABLED) | 
            BF_IOMUXC_SW_MUX_CTL_PAD_SD4_DATA5_MUX_MODE_V(ALT2));
    // Pad Control Register:
    // IOMUXC_SW_PAD_CTL_PAD_SD4_DATA5(0x020E073C)
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
    HW_IOMUXC_SW_PAD_CTL_PAD_SD4_DATA5_WR(
            BF_IOMUXC_SW_PAD_CTL_PAD_SD4_DATA5_HYS_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD4_DATA5_PUS_V(100K_OHM_PU) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD4_DATA5_PUE_V(PULL) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD4_DATA5_PKE_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD4_DATA5_ODE_V(DISABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD4_DATA5_SPEED_V(100MHZ) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD4_DATA5_DSE_V(40_OHM) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD4_DATA5_SRE_V(SLOW));
    // Pad SD4_DATA5 is involved in Daisy Chain.
    // Input Select Register:
    // IOMUXC_UART2_UART_RTS_B_SELECT_INPUT(0x020E0900)
    //   DAISY [2:0] - MUX Mode Select Field Reset: EIM_DATA28_ALT4
    //                 Selecting Pads Involved in Daisy Chain.
    //     EIM_DATA28_ALT4 (0) - Select signal uart2 UART2_CTS_B as input from pad EIM_DATA28(ALT4).
    //     EIM_DATA29_ALT4 (1) - Select signal uart2 UART2_RTS_B as input from pad EIM_DATA29(ALT4).
    //     SD3_CLK_ALT1 (2) - Select signal uart2 UART2_RTS_B as input from pad SD3_CLK(ALT1).
    //     SD3_CMD_ALT1 (3) - Select signal uart2 UART2_CTS_B as input from pad SD3_CMD(ALT1).
    //     SD4_DATA5_ALT2 (4) - Select signal uart2 UART2_RTS_B as input from pad SD4_DATA5(ALT2).
    //     SD4_DATA6_ALT2 (5) - Select signal uart2 UART2_CTS_B as input from pad SD4_DATA6(ALT2).
    HW_IOMUXC_UART2_UART_RTS_B_SELECT_INPUT_WR(
            BF_IOMUXC_UART2_UART_RTS_B_SELECT_INPUT_DAISY_V(SD4_DATA5_ALT2));

    // Config uart2.UART2_RX_DATA to pad GPIO08(R5)
    // HW_IOMUXC_SW_MUX_CTL_PAD_GPIO08_WR(0x00000004);
    // HW_IOMUXC_SW_PAD_CTL_PAD_GPIO08_WR(0x0001B0B0);
    // HW_IOMUXC_UART2_UART_RX_DATA_SELECT_INPUT_WR(0x00000003);
    // Mux Register:
    // IOMUXC_SW_MUX_CTL_PAD_GPIO08(0x020E023C)
    //   SION [4] - Software Input On Field Reset: DISABLED
    //              Force the selected mux mode Input path no matter of MUX_MODE functionality.
    //     DISABLED (0) - Input Path is determined by functionality of the selected mux mode (regular).
    //     ENABLED (1) - Force input path of pad.
    //   MUX_MODE [3:0] - MUX Mode Select Field Reset: ALT5
    //                    Select iomux modes to be used for pad.
    //     ALT0 (0) - Select instance: esai signal: ESAI_TX5_RX0
    //     ALT1 (1) - Select instance: xtalosc signal: XTALOSC_REF_CLK_32K
    //     ALT2 (2) - Select instance: epit2 signal: EPIT2_OUT
    //     ALT3 (3) - Select instance: flexcan1 signal: FLEXCAN1_RX
    //     ALT4 (4) - Select instance: uart2 signal: UART2_RX_DATA
    //     ALT5 (5) - Select instance: gpio1 signal: GPIO1_IO08
    //     ALT6 (6) - Select instance: spdif signal: SPDIF_SR_CLK
    //     ALT7 (7) - Select instance: usb signal: USB_OTG_PWR_CTL_WAKE
    //     ALT8 (8) - Select instance: i2c4 signal: I2C4_SDA
    HW_IOMUXC_SW_MUX_CTL_PAD_GPIO08_WR(
            BF_IOMUXC_SW_MUX_CTL_PAD_GPIO08_SION_V(DISABLED) | 
            BF_IOMUXC_SW_MUX_CTL_PAD_GPIO08_MUX_MODE_V(ALT4));
    // Pad Control Register:
    // IOMUXC_SW_PAD_CTL_PAD_GPIO08(0x020E060C)
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
    HW_IOMUXC_SW_PAD_CTL_PAD_GPIO08_WR(
            BF_IOMUXC_SW_PAD_CTL_PAD_GPIO08_HYS_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_GPIO08_PUS_V(100K_OHM_PU) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_GPIO08_PUE_V(PULL) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_GPIO08_PKE_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_GPIO08_ODE_V(DISABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_GPIO08_SPEED_V(100MHZ) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_GPIO08_DSE_V(40_OHM) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_GPIO08_SRE_V(SLOW));
    // Pad GPIO08 is involved in Daisy Chain.
    // Input Select Register:
    // IOMUXC_UART2_UART_RX_DATA_SELECT_INPUT(0x020E0904)
    //   DAISY [2:0] - MUX Mode Select Field Reset: EIM_DATA26_ALT4
    //                 Selecting Pads Involved in Daisy Chain.
    //     EIM_DATA26_ALT4 (0) - Select signal uart2 UART2_TX_DATA as input from pad EIM_DATA26(ALT4).
    //     EIM_DATA27_ALT4 (1) - Select signal uart2 UART2_RX_DATA as input from pad EIM_DATA27(ALT4).
    //     GPIO07_ALT4 (2) - Select signal uart2 UART2_TX_DATA as input from pad GPIO07(ALT4).
    //     GPIO08_ALT4 (3) - Select signal uart2 UART2_RX_DATA as input from pad GPIO08(ALT4).
    //     SD3_DATA4_ALT1 (4) - Select signal uart2 UART2_RX_DATA as input from pad SD3_DATA4(ALT1).
    //     SD3_DATA5_ALT1 (5) - Select signal uart2 UART2_TX_DATA as input from pad SD3_DATA5(ALT1).
    //     SD4_DATA4_ALT2 (6) - Select signal uart2 UART2_RX_DATA as input from pad SD4_DATA4(ALT2).
    //     SD4_DATA7_ALT2 (7) - Select signal uart2 UART2_TX_DATA as input from pad SD4_DATA7(ALT2).
    HW_IOMUXC_UART2_UART_RX_DATA_SELECT_INPUT_WR(
            BF_IOMUXC_UART2_UART_RX_DATA_SELECT_INPUT_DAISY_V(GPIO08_ALT4));

    // Config uart2.UART2_TX_DATA to pad GPIO07(R3)
    // HW_IOMUXC_SW_MUX_CTL_PAD_GPIO07_WR(0x00000004);
    // HW_IOMUXC_SW_PAD_CTL_PAD_GPIO07_WR(0x0001B0B0);
    // HW_IOMUXC_UART2_UART_RX_DATA_SELECT_INPUT_WR(0x00000003);
    // Mux Register:
    // IOMUXC_SW_MUX_CTL_PAD_GPIO07(0x020E0238)
    //   SION [4] - Software Input On Field Reset: DISABLED
    //              Force the selected mux mode Input path no matter of MUX_MODE functionality.
    //     DISABLED (0) - Input Path is determined by functionality of the selected mux mode (regular).
    //     ENABLED (1) - Force input path of pad.
    //   MUX_MODE [3:0] - MUX Mode Select Field Reset: ALT5
    //                    Select iomux modes to be used for pad.
    //     ALT0 (0) - Select instance: esai signal: ESAI_TX4_RX1
    //     ALT2 (2) - Select instance: epit1 signal: EPIT1_OUT
    //     ALT3 (3) - Select instance: flexcan1 signal: FLEXCAN1_TX
    //     ALT4 (4) - Select instance: uart2 signal: UART2_TX_DATA
    //     ALT5 (5) - Select instance: gpio1 signal: GPIO1_IO07
    //     ALT6 (6) - Select instance: spdif signal: SPDIF_LOCK
    //     ALT7 (7) - Select instance: usb signal: USB_OTG_HOST_MODE
    //     ALT8 (8) - Select instance: i2c4 signal: I2C4_SCL
    HW_IOMUXC_SW_MUX_CTL_PAD_GPIO07_WR(
            BF_IOMUXC_SW_MUX_CTL_PAD_GPIO07_SION_V(DISABLED) | 
            BF_IOMUXC_SW_MUX_CTL_PAD_GPIO07_MUX_MODE_V(ALT4));
    // Pad Control Register:
    // IOMUXC_SW_PAD_CTL_PAD_GPIO07(0x020E0608)
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
    HW_IOMUXC_SW_PAD_CTL_PAD_GPIO07_WR(
            BF_IOMUXC_SW_PAD_CTL_PAD_GPIO07_HYS_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_GPIO07_PUS_V(100K_OHM_PU) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_GPIO07_PUE_V(PULL) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_GPIO07_PKE_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_GPIO07_ODE_V(DISABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_GPIO07_SPEED_V(100MHZ) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_GPIO07_DSE_V(40_OHM) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_GPIO07_SRE_V(SLOW));
    // Pad GPIO07 is involved in Daisy Chain.
    // Input Select Register:
    // IOMUXC_UART2_UART_RX_DATA_SELECT_INPUT(0x020E0904)
    //   DAISY [2:0] - MUX Mode Select Field Reset: EIM_DATA26_ALT4
    //                 Selecting Pads Involved in Daisy Chain.
    //     EIM_DATA26_ALT4 (0) - Select signal uart2 UART2_TX_DATA as input from pad EIM_DATA26(ALT4).
    //     EIM_DATA27_ALT4 (1) - Select signal uart2 UART2_RX_DATA as input from pad EIM_DATA27(ALT4).
    //     GPIO07_ALT4 (2) - Select signal uart2 UART2_TX_DATA as input from pad GPIO07(ALT4).
    //     GPIO08_ALT4 (3) - Select signal uart2 UART2_RX_DATA as input from pad GPIO08(ALT4).
    //     SD3_DATA4_ALT1 (4) - Select signal uart2 UART2_RX_DATA as input from pad SD3_DATA4(ALT1).
    //     SD3_DATA5_ALT1 (5) - Select signal uart2 UART2_TX_DATA as input from pad SD3_DATA5(ALT1).
    //     SD4_DATA4_ALT2 (6) - Select signal uart2 UART2_RX_DATA as input from pad SD4_DATA4(ALT2).
    //     SD4_DATA7_ALT2 (7) - Select signal uart2 UART2_TX_DATA as input from pad SD4_DATA7(ALT2).
    HW_IOMUXC_UART2_UART_RX_DATA_SELECT_INPUT_WR(
            BF_IOMUXC_UART2_UART_RX_DATA_SELECT_INPUT_DAISY_V(GPIO08_ALT4));
}
