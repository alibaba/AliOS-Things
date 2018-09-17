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

// File: uart3_iomux_config.c

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

// Function to configure IOMUXC for uart3 module.
void uart3_iomux_config(void)
{
    // Config uart3.UART3_CTS_B to pad EIM_DATA30(J20)
    // HW_IOMUXC_SW_MUX_CTL_PAD_EIM_DATA30_WR(0x00000004);
    // HW_IOMUXC_SW_PAD_CTL_PAD_EIM_DATA30_WR(0x0001B0B0);
    // HW_IOMUXC_UART3_UART_RTS_B_SELECT_INPUT_WR(0x00000003);
    // Mux Register:
    // IOMUXC_SW_MUX_CTL_PAD_EIM_DATA30(0x020E017C)
    //   SION [4] - Software Input On Field Reset: DISABLED
    //              Force the selected mux mode Input path no matter of MUX_MODE functionality.
    //     DISABLED (0) - Input Path is determined by functionality of the selected mux mode (regular).
    //     ENABLED (1) - Force input path of pad.
    //   MUX_MODE [3:0] - MUX Mode Select Field Reset: ALT5
    //                    Select iomux modes to be used for pad.
    //     ALT0 (0) - Select instance: eim signal: EIM_DATA30
    //     ALT1 (1) - Select instance: ipu1 signal: IPU1_DISP1_DATA21
    //     ALT2 (2) - Select instance: ipu1 signal: IPU1_DI0_PIN11
    //     ALT3 (3) - Select instance: ipu1 signal: IPU1_CSI0_DATA03
    //     ALT4 (4) - Select instance: uart3 signal: UART3_CTS_B
    //     ALT5 (5) - Select instance: gpio3 signal: GPIO3_IO30
    //     ALT6 (6) - Select instance: usb signal: USB_H1_OC
    //     ALT8 (8) - Select instance: epdc signal: EPDC_SDOEZ
    HW_IOMUXC_SW_MUX_CTL_PAD_EIM_DATA30_WR(
            BF_IOMUXC_SW_MUX_CTL_PAD_EIM_DATA30_SION_V(DISABLED) | 
            BF_IOMUXC_SW_MUX_CTL_PAD_EIM_DATA30_MUX_MODE_V(ALT4));
    // Pad Control Register:
    // IOMUXC_SW_PAD_CTL_PAD_EIM_DATA30(0x020E054C)
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
    HW_IOMUXC_SW_PAD_CTL_PAD_EIM_DATA30_WR(
            BF_IOMUXC_SW_PAD_CTL_PAD_EIM_DATA30_HYS_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_EIM_DATA30_PUS_V(100K_OHM_PU) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_EIM_DATA30_PUE_V(PULL) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_EIM_DATA30_PKE_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_EIM_DATA30_ODE_V(DISABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_EIM_DATA30_SPEED_V(100MHZ) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_EIM_DATA30_DSE_V(40_OHM) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_EIM_DATA30_SRE_V(SLOW));
    // Pad EIM_DATA30 is involved in Daisy Chain.
    // Input Select Register:
    // IOMUXC_UART3_UART_RTS_B_SELECT_INPUT(0x020E0908)
    //   DAISY [2:0] - MUX Mode Select Field Reset: EIM_DATA23_ALT2
    //                 Selecting Pads Involved in Daisy Chain.
    //     EIM_DATA23_ALT2 (0) - Select signal uart3 UART3_CTS_B as input from pad EIM_DATA23(ALT2).
    //     EIM_DATA30_ALT4 (1) - Select signal uart3 UART3_CTS_B as input from pad EIM_DATA30(ALT4).
    //     EIM_DATA31_ALT4 (2) - Select signal uart3 UART3_RTS_B as input from pad EIM_DATA31(ALT4).
    //     EIM_EB3_ALT2 (3) - Select signal uart3 UART3_RTS_B as input from pad EIM_EB3(ALT2).
    //     SD3_DATA3_ALT1 (4) - Select signal uart3 UART3_CTS_B as input from pad SD3_DATA3(ALT1).
    //     SD3_RESET_ALT1 (5) - Select signal uart3 UART3_RTS_B as input from pad SD3_RESET(ALT1).
    HW_IOMUXC_UART3_UART_RTS_B_SELECT_INPUT_WR(
            BF_IOMUXC_UART3_UART_RTS_B_SELECT_INPUT_DAISY_V(EIM_EB3_ALT2));

    // Config uart3.UART3_RTS_B to pad EIM_EB3(F23)
    // HW_IOMUXC_SW_MUX_CTL_PAD_EIM_EB3_WR(0x00000002);
    // HW_IOMUXC_SW_PAD_CTL_PAD_EIM_EB3_WR(0x0001B0B0);
    // HW_IOMUXC_UART3_UART_RTS_B_SELECT_INPUT_WR(0x00000003);
    // Mux Register:
    // IOMUXC_SW_MUX_CTL_PAD_EIM_EB3(0x020E01D0)
    //   SION [4] - Software Input On Field Reset: DISABLED
    //              Force the selected mux mode Input path no matter of MUX_MODE functionality.
    //     DISABLED (0) - Input Path is determined by functionality of the selected mux mode (regular).
    //     ENABLED (1) - Force input path of pad.
    //   MUX_MODE [3:0] - MUX Mode Select Field Reset: ALT5
    //                    Select iomux modes to be used for pad.
    //     ALT0 (0) - Select instance: eim signal: EIM_EB3
    //     ALT1 (1) - Select instance: ecspi4 signal: ECSPI4_RDY
    //     ALT2 (2) - Select instance: uart3 signal: UART3_RTS_B
    //     ALT3 (3) - Select instance: uart1 signal: UART1_RI_B
    //     ALT4 (4) - Select instance: ipu1 signal: IPU1_CSI1_HSYNC
    //     ALT5 (5) - Select instance: gpio2 signal: GPIO2_IO31
    //     ALT6 (6) - Select instance: ipu1 signal: IPU1_DI1_PIN03
    //     ALT7 (7) - Select instance: src signal: SRC_BOOT_CFG31
    //     ALT8 (8) - Select instance: epdc signal: EPDC_SDCE0
    //     ALT9 (9) - Select instance: eim signal: EIM_ACLK_FREERUN
    HW_IOMUXC_SW_MUX_CTL_PAD_EIM_EB3_WR(
            BF_IOMUXC_SW_MUX_CTL_PAD_EIM_EB3_SION_V(DISABLED) | 
            BF_IOMUXC_SW_MUX_CTL_PAD_EIM_EB3_MUX_MODE_V(ALT2));
    // Pad Control Register:
    // IOMUXC_SW_PAD_CTL_PAD_EIM_EB3(0x020E05A0)
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
    HW_IOMUXC_SW_PAD_CTL_PAD_EIM_EB3_WR(
            BF_IOMUXC_SW_PAD_CTL_PAD_EIM_EB3_HYS_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_EIM_EB3_PUS_V(100K_OHM_PU) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_EIM_EB3_PUE_V(PULL) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_EIM_EB3_PKE_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_EIM_EB3_ODE_V(DISABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_EIM_EB3_SPEED_V(100MHZ) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_EIM_EB3_DSE_V(40_OHM) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_EIM_EB3_SRE_V(SLOW));
    // Pad EIM_EB3 is involved in Daisy Chain.
    // Input Select Register:
    // IOMUXC_UART3_UART_RTS_B_SELECT_INPUT(0x020E0908)
    //   DAISY [2:0] - MUX Mode Select Field Reset: EIM_DATA23_ALT2
    //                 Selecting Pads Involved in Daisy Chain.
    //     EIM_DATA23_ALT2 (0) - Select signal uart3 UART3_CTS_B as input from pad EIM_DATA23(ALT2).
    //     EIM_DATA30_ALT4 (1) - Select signal uart3 UART3_CTS_B as input from pad EIM_DATA30(ALT4).
    //     EIM_DATA31_ALT4 (2) - Select signal uart3 UART3_RTS_B as input from pad EIM_DATA31(ALT4).
    //     EIM_EB3_ALT2 (3) - Select signal uart3 UART3_RTS_B as input from pad EIM_EB3(ALT2).
    //     SD3_DATA3_ALT1 (4) - Select signal uart3 UART3_CTS_B as input from pad SD3_DATA3(ALT1).
    //     SD3_RESET_ALT1 (5) - Select signal uart3 UART3_RTS_B as input from pad SD3_RESET(ALT1).
    HW_IOMUXC_UART3_UART_RTS_B_SELECT_INPUT_WR(
            BF_IOMUXC_UART3_UART_RTS_B_SELECT_INPUT_DAISY_V(EIM_EB3_ALT2));

    // Config uart3.UART3_RX_DATA to pad SD4_CLK(E16)
    // HW_IOMUXC_SW_MUX_CTL_PAD_SD4_CLK_WR(0x00000002);
    // HW_IOMUXC_SW_PAD_CTL_PAD_SD4_CLK_WR(0x0001B0B0);
    // HW_IOMUXC_UART3_UART_RX_DATA_SELECT_INPUT_WR(0x00000002);
    // Mux Register:
    // IOMUXC_SW_MUX_CTL_PAD_SD4_CLK(0x020E0338)
    //   SION [4] - Software Input On Field Reset: DISABLED
    //              Force the selected mux mode Input path no matter of MUX_MODE functionality.
    //     DISABLED (0) - Input Path is determined by functionality of the selected mux mode (regular).
    //     ENABLED (1) - Force input path of pad.
    //   MUX_MODE [2:0] - MUX Mode Select Field Reset: ALT5
    //                    Select iomux modes to be used for pad.
    //     ALT0 (0) - Select instance: usdhc4 signal: SD4_CLK
    //     ALT1 (1) - Select instance: gpmi signal: NAND_WE_B
    //     ALT2 (2) - Select instance: uart3 signal: UART3_RX_DATA
    //     ALT5 (5) - Select instance: gpio7 signal: GPIO7_IO10
    HW_IOMUXC_SW_MUX_CTL_PAD_SD4_CLK_WR(
            BF_IOMUXC_SW_MUX_CTL_PAD_SD4_CLK_SION_V(DISABLED) | 
            BF_IOMUXC_SW_MUX_CTL_PAD_SD4_CLK_MUX_MODE_V(ALT2));
    // Pad Control Register:
    // IOMUXC_SW_PAD_CTL_PAD_SD4_CLK(0x020E0720)
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
    HW_IOMUXC_SW_PAD_CTL_PAD_SD4_CLK_WR(
            BF_IOMUXC_SW_PAD_CTL_PAD_SD4_CLK_HYS_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD4_CLK_PUS_V(100K_OHM_PU) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD4_CLK_PUE_V(PULL) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD4_CLK_PKE_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD4_CLK_ODE_V(DISABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD4_CLK_SPEED_V(100MHZ) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD4_CLK_DSE_V(40_OHM) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD4_CLK_SRE_V(SLOW));
    // Pad SD4_CLK is involved in Daisy Chain.
    // Input Select Register:
    // IOMUXC_UART3_UART_RX_DATA_SELECT_INPUT(0x020E090C)
    //   DAISY [1:0] - MUX Mode Select Field Reset: EIM_DATA24_ALT2
    //                 Selecting Pads Involved in Daisy Chain.
    //     EIM_DATA24_ALT2 (0) - Select signal uart3 UART3_TX_DATA as input from pad EIM_DATA24(ALT2).
    //     EIM_DATA25_ALT2 (1) - Select signal uart3 UART3_RX_DATA as input from pad EIM_DATA25(ALT2).
    //     SD4_CLK_ALT2 (2) - Select signal uart3 UART3_RX_DATA as input from pad SD4_CLK(ALT2).
    //     SD4_CMD_ALT2 (3) - Select signal uart3 UART3_TX_DATA as input from pad SD4_CMD(ALT2).
    HW_IOMUXC_UART3_UART_RX_DATA_SELECT_INPUT_WR(
            BF_IOMUXC_UART3_UART_RX_DATA_SELECT_INPUT_DAISY_V(SD4_CLK_ALT2));

    // Config uart3.UART3_TX_DATA to pad SD4_CMD(B17)
    // HW_IOMUXC_SW_MUX_CTL_PAD_SD4_CMD_WR(0x00000002);
    // HW_IOMUXC_SW_PAD_CTL_PAD_SD4_CMD_WR(0x0001B0B0);
    // HW_IOMUXC_UART3_UART_RX_DATA_SELECT_INPUT_WR(0x00000002);
    // Mux Register:
    // IOMUXC_SW_MUX_CTL_PAD_SD4_CMD(0x020E033C)
    //   SION [4] - Software Input On Field Reset: DISABLED
    //              Force the selected mux mode Input path no matter of MUX_MODE functionality.
    //     DISABLED (0) - Input Path is determined by functionality of the selected mux mode (regular).
    //     ENABLED (1) - Force input path of pad.
    //   MUX_MODE [2:0] - MUX Mode Select Field Reset: ALT5
    //                    Select iomux modes to be used for pad.
    //     ALT0 (0) - Select instance: usdhc4 signal: SD4_CMD
    //     ALT1 (1) - Select instance: gpmi signal: NAND_RE_B
    //     ALT2 (2) - Select instance: uart3 signal: UART3_TX_DATA
    //     ALT5 (5) - Select instance: gpio7 signal: GPIO7_IO09
    HW_IOMUXC_SW_MUX_CTL_PAD_SD4_CMD_WR(
            BF_IOMUXC_SW_MUX_CTL_PAD_SD4_CMD_SION_V(DISABLED) | 
            BF_IOMUXC_SW_MUX_CTL_PAD_SD4_CMD_MUX_MODE_V(ALT2));
    // Pad Control Register:
    // IOMUXC_SW_PAD_CTL_PAD_SD4_CMD(0x020E0724)
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
    HW_IOMUXC_SW_PAD_CTL_PAD_SD4_CMD_WR(
            BF_IOMUXC_SW_PAD_CTL_PAD_SD4_CMD_HYS_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD4_CMD_PUS_V(100K_OHM_PU) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD4_CMD_PUE_V(PULL) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD4_CMD_PKE_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD4_CMD_ODE_V(DISABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD4_CMD_SPEED_V(100MHZ) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD4_CMD_DSE_V(40_OHM) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_SD4_CMD_SRE_V(SLOW));
    // Pad SD4_CMD is involved in Daisy Chain.
    // Input Select Register:
    // IOMUXC_UART3_UART_RX_DATA_SELECT_INPUT(0x020E090C)
    //   DAISY [1:0] - MUX Mode Select Field Reset: EIM_DATA24_ALT2
    //                 Selecting Pads Involved in Daisy Chain.
    //     EIM_DATA24_ALT2 (0) - Select signal uart3 UART3_TX_DATA as input from pad EIM_DATA24(ALT2).
    //     EIM_DATA25_ALT2 (1) - Select signal uart3 UART3_RX_DATA as input from pad EIM_DATA25(ALT2).
    //     SD4_CLK_ALT2 (2) - Select signal uart3 UART3_RX_DATA as input from pad SD4_CLK(ALT2).
    //     SD4_CMD_ALT2 (3) - Select signal uart3 UART3_TX_DATA as input from pad SD4_CMD(ALT2).
    HW_IOMUXC_UART3_UART_RX_DATA_SELECT_INPUT_WR(
            BF_IOMUXC_UART3_UART_RX_DATA_SELECT_INPUT_DAISY_V(SD4_CLK_ALT2));
}
