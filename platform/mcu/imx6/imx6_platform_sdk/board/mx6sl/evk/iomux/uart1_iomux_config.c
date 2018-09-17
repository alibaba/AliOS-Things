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
    // Config uart1.UART1_RX_DATA to pad UART1_RXD(B19)
    // HW_IOMUXC_SW_MUX_CTL_PAD_UART1_RXD_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_UART1_RXD_WR(0x000110B0);
    // HW_IOMUXC_UART1_UART_RX_DATA_SELECT_INPUT_WR(0x00000000);
    // Mux Register:
    // IOMUXC_SW_MUX_CTL_PAD_UART1_RXD(0x020E0298)
    //   SION [4] - Software Input On Field Reset: DISABLED
    //              Force the selected mux mode Input path no matter of MUX_MODE functionality.
    //     DISABLED (0) - Input Path is determined by functionality of the selected mux mode (regular).
    //     ENABLED (1) - Force input path of pad.
    //   MUX_MODE [2:0] - MUX Mode Select Field Reset: ALT5
    //                    Select iomux modes to be used for pad.
    //     ALT0 (0) - Select instance: uart1 signal: UART1_RX_DATA
    //     ALT1 (1) - Select instance: pwm1 signal: PWM1_OUT
    //     ALT2 (2) - Select instance: uart4 signal: UART4_RX_DATA
    //     ALT3 (3) - Select instance: fec signal: FEC_COL
    //     ALT4 (4) - Select instance: uart5 signal: UART5_RX_DATA
    //     ALT5 (5) - Select instance: gpio3 signal: GPIO3_IO16
    HW_IOMUXC_SW_MUX_CTL_PAD_UART1_RXD_WR(
            BF_IOMUXC_SW_MUX_CTL_PAD_UART1_RXD_SION_V(DISABLED) | 
            BF_IOMUXC_SW_MUX_CTL_PAD_UART1_RXD_MUX_MODE_V(ALT0));
    // Pad Control Register:
    // IOMUXC_SW_PAD_CTL_PAD_UART1_RXD(0x020E05A0)
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
    HW_IOMUXC_SW_PAD_CTL_PAD_UART1_RXD_WR(
            BF_IOMUXC_SW_PAD_CTL_PAD_UART1_RXD_LVE_V(DISABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_UART1_RXD_HYS_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_UART1_RXD_PUS_V(100K_OHM_PD) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_UART1_RXD_PUE_V(KEEP) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_UART1_RXD_PKE_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_UART1_RXD_ODE_V(DISABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_UART1_RXD_SPEED_V(100MHZ) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_UART1_RXD_DSE_V(40_OHM) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_UART1_RXD_SRE_V(SLOW));
    // Pad UART1_RXD is involved in Daisy Chain.
    // Input Select Register:
    // IOMUXC_UART1_UART_RX_DATA_SELECT_INPUT(0x020E07FC)
    //   DAISY [0] - MUX Mode Select Field Reset: UART1_RXD_ALT0
    //               Selecting Pads Involved in Daisy Chain.
    //     UART1_RXD_ALT0 (0) - Select signal uart1 UART1_RX_DATA as input from pad UART1_RXD(ALT0).
    //     UART1_TXD_ALT0 (1) - Select signal uart1 UART1_TX_DATA as input from pad UART1_TXD(ALT0).
    HW_IOMUXC_UART1_UART_RX_DATA_SELECT_INPUT_WR(
            BF_IOMUXC_UART1_UART_RX_DATA_SELECT_INPUT_DAISY_V(UART1_RXD_ALT0));

    // Config uart1.UART1_TX_DATA to pad UART1_TXD(D19)
    // HW_IOMUXC_SW_MUX_CTL_PAD_UART1_TXD_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_UART1_TXD_WR(0x000110B0);
    // HW_IOMUXC_UART1_UART_RX_DATA_SELECT_INPUT_WR(0x00000000);
    // Mux Register:
    // IOMUXC_SW_MUX_CTL_PAD_UART1_TXD(0x020E029C)
    //   SION [4] - Software Input On Field Reset: DISABLED
    //              Force the selected mux mode Input path no matter of MUX_MODE functionality.
    //     DISABLED (0) - Input Path is determined by functionality of the selected mux mode (regular).
    //     ENABLED (1) - Force input path of pad.
    //   MUX_MODE [2:0] - MUX Mode Select Field Reset: ALT5
    //                    Select iomux modes to be used for pad.
    //     ALT0 (0) - Select instance: uart1 signal: UART1_TX_DATA
    //     ALT1 (1) - Select instance: pwm2 signal: PWM2_OUT
    //     ALT2 (2) - Select instance: uart4 signal: UART4_TX_DATA
    //     ALT3 (3) - Select instance: fec signal: FEC_RX_CLK
    //     ALT4 (4) - Select instance: uart5 signal: UART5_TX_DATA
    //     ALT5 (5) - Select instance: gpio3 signal: GPIO3_IO17
    //     ALT7 (7) - Select instance: uart5 signal: UART5_DCD_B
    HW_IOMUXC_SW_MUX_CTL_PAD_UART1_TXD_WR(
            BF_IOMUXC_SW_MUX_CTL_PAD_UART1_TXD_SION_V(DISABLED) | 
            BF_IOMUXC_SW_MUX_CTL_PAD_UART1_TXD_MUX_MODE_V(ALT0));
    // Pad Control Register:
    // IOMUXC_SW_PAD_CTL_PAD_UART1_TXD(0x020E05A4)
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
    HW_IOMUXC_SW_PAD_CTL_PAD_UART1_TXD_WR(
            BF_IOMUXC_SW_PAD_CTL_PAD_UART1_TXD_LVE_V(DISABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_UART1_TXD_HYS_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_UART1_TXD_PUS_V(100K_OHM_PD) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_UART1_TXD_PUE_V(KEEP) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_UART1_TXD_PKE_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_UART1_TXD_ODE_V(DISABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_UART1_TXD_SPEED_V(100MHZ) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_UART1_TXD_DSE_V(40_OHM) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_UART1_TXD_SRE_V(SLOW));
    // Pad UART1_TXD is involved in Daisy Chain.
    // Input Select Register:
    // IOMUXC_UART1_UART_RX_DATA_SELECT_INPUT(0x020E07FC)
    //   DAISY [0] - MUX Mode Select Field Reset: UART1_RXD_ALT0
    //               Selecting Pads Involved in Daisy Chain.
    //     UART1_RXD_ALT0 (0) - Select signal uart1 UART1_RX_DATA as input from pad UART1_RXD(ALT0).
    //     UART1_TXD_ALT0 (1) - Select signal uart1 UART1_TX_DATA as input from pad UART1_TXD(ALT0).
    HW_IOMUXC_UART1_UART_RX_DATA_SELECT_INPUT_WR(
            BF_IOMUXC_UART1_UART_RX_DATA_SELECT_INPUT_DAISY_V(UART1_RXD_ALT0));
}
