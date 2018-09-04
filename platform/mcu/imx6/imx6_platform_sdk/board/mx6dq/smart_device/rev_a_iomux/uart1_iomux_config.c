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

// File: uart1_iomux_config.c

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

// Function to configure IOMUXC for uart1 module.
void uart1_iomux_config(void)
{
    // Config uart1.UART1_RX_DATA to pad CSI0_DATA11(M3)
    // HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA11_WR(0x00000003);
    // HW_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA11_WR(0x0001B0B0);
    // HW_IOMUXC_UART1_UART_RX_DATA_SELECT_INPUT_WR(0x00000001);
    // Mux Register:
    // IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA11(0x020E0284)
    //   SION [4] - Software Input On Field Reset: DISABLED
    //              Force the selected mux mode Input path no matter of MUX_MODE functionality.
    //     DISABLED (0) - Input Path is determined by functionality of the selected mux mode (regular).
    //     ENABLED (1) - Force input path of pad.
    //   MUX_MODE [2:0] - MUX Mode Select Field Reset: ALT5
    //                    Select iomux modes to be used for pad.
    //     ALT0 (0) - Select instance: ipu1 signal: IPU1_CSI0_DATA11
    //     ALT1 (1) - Select instance: audmux signal: AUD3_RXFS
    //     ALT2 (2) - Select instance: ecspi2 signal: ECSPI2_SS0
    //     ALT3 (3) - Select instance: uart1 signal: UART1_RX_DATA
    //     ALT5 (5) - Select instance: gpio5 signal: GPIO5_IO29
    //     ALT7 (7) - Select instance: arm signal: ARM_TRACE08
    HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA11_WR(
            BF_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA11_SION_V(DISABLED) | 
            BF_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA11_MUX_MODE_V(ALT3));
    // Pad Control Register:
    // IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA11(0x020E0654)
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
    HW_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA11_WR(
            BF_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA11_HYS_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA11_PUS_V(100K_OHM_PU) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA11_PUE_V(PULL) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA11_PKE_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA11_ODE_V(DISABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA11_SPEED_V(100MHZ) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA11_DSE_V(40_OHM) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA11_SRE_V(SLOW));
    // Pad CSI0_DATA11 is involved in Daisy Chain.
    // Input Select Register:
    // IOMUXC_UART1_UART_RX_DATA_SELECT_INPUT(0x020E0920)
    //   DAISY [1:0] - MUX Mode Select Field Reset: CSI0_DATA10_ALT3
    //                 Selecting Pads Involved in Daisy Chain.
    //     CSI0_DATA10_ALT3 (0) - Select signal uart1 UART1_TX_DATA as input from pad CSI0_DATA10(ALT3).
    //     CSI0_DATA11_ALT3 (1) - Select signal uart1 UART1_RX_DATA as input from pad CSI0_DATA11(ALT3).
    //     SD3_DATA7_ALT1 (2) - Select signal uart1 UART1_TX_DATA as input from pad SD3_DATA7(ALT1).
    //     SD3_DATA6_ALT1 (3) - Select signal uart1 UART1_RX_DATA as input from pad SD3_DATA6(ALT1).
    HW_IOMUXC_UART1_UART_RX_DATA_SELECT_INPUT_WR(
            BF_IOMUXC_UART1_UART_RX_DATA_SELECT_INPUT_DAISY_V(CSI0_DATA11_ALT3));

    // Config uart1.UART1_TX_DATA to pad CSI0_DATA10(M1)
    // HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA10_WR(0x00000003);
    // HW_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA10_WR(0x0001B0B0);
    // HW_IOMUXC_UART1_UART_RX_DATA_SELECT_INPUT_WR(0x00000001);
    // Mux Register:
    // IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA10(0x020E0280)
    //   SION [4] - Software Input On Field Reset: DISABLED
    //              Force the selected mux mode Input path no matter of MUX_MODE functionality.
    //     DISABLED (0) - Input Path is determined by functionality of the selected mux mode (regular).
    //     ENABLED (1) - Force input path of pad.
    //   MUX_MODE [2:0] - MUX Mode Select Field Reset: ALT5
    //                    Select iomux modes to be used for pad.
    //     ALT0 (0) - Select instance: ipu1 signal: IPU1_CSI0_DATA10
    //     ALT1 (1) - Select instance: audmux signal: AUD3_RXC
    //     ALT2 (2) - Select instance: ecspi2 signal: ECSPI2_MISO
    //     ALT3 (3) - Select instance: uart1 signal: UART1_TX_DATA
    //     ALT5 (5) - Select instance: gpio5 signal: GPIO5_IO28
    //     ALT7 (7) - Select instance: arm signal: ARM_TRACE07
    HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA10_WR(
            BF_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA10_SION_V(DISABLED) | 
            BF_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA10_MUX_MODE_V(ALT3));
    // Pad Control Register:
    // IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA10(0x020E0650)
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
    HW_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA10_WR(
            BF_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA10_HYS_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA10_PUS_V(100K_OHM_PU) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA10_PUE_V(PULL) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA10_PKE_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA10_ODE_V(DISABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA10_SPEED_V(100MHZ) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA10_DSE_V(40_OHM) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA10_SRE_V(SLOW));
    // Pad CSI0_DATA10 is involved in Daisy Chain.
    // Input Select Register:
    // IOMUXC_UART1_UART_RX_DATA_SELECT_INPUT(0x020E0920)
    //   DAISY [1:0] - MUX Mode Select Field Reset: CSI0_DATA10_ALT3
    //                 Selecting Pads Involved in Daisy Chain.
    //     CSI0_DATA10_ALT3 (0) - Select signal uart1 UART1_TX_DATA as input from pad CSI0_DATA10(ALT3).
    //     CSI0_DATA11_ALT3 (1) - Select signal uart1 UART1_RX_DATA as input from pad CSI0_DATA11(ALT3).
    //     SD3_DATA7_ALT1 (2) - Select signal uart1 UART1_TX_DATA as input from pad SD3_DATA7(ALT1).
    //     SD3_DATA6_ALT1 (3) - Select signal uart1 UART1_RX_DATA as input from pad SD3_DATA6(ALT1).
    HW_IOMUXC_UART1_UART_RX_DATA_SELECT_INPUT_WR(
            BF_IOMUXC_UART1_UART_RX_DATA_SELECT_INPUT_DAISY_V(CSI0_DATA11_ALT3));
}
