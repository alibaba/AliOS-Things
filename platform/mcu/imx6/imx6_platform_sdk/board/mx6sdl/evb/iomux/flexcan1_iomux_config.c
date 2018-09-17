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

// File: flexcan1_iomux_config.c

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

// Function to configure IOMUXC for flexcan1 module.
void flexcan1_iomux_config(void)
{
    // Config flexcan1.FLEXCAN1_RX to pad KEY_ROW2(W4)
    // HW_IOMUXC_SW_MUX_CTL_PAD_KEY_ROW2_WR(0x00000002);
    // HW_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW2_WR(0x0001B0B0);
    // HW_IOMUXC_FLEXCAN1_RX_SELECT_INPUT_WR(0x00000000);
    // Mux Register:
    // IOMUXC_SW_MUX_CTL_PAD_KEY_ROW2(0x020E0260)
    //   SION [4] - Software Input On Field Reset: DISABLED
    //              Force the selected mux mode Input path no matter of MUX_MODE functionality.
    //     DISABLED (0) - Input Path is determined by functionality of the selected mux mode (regular).
    //     ENABLED (1) - Force input path of pad.
    //   MUX_MODE [2:0] - MUX Mode Select Field Reset: ALT5
    //                    Select iomux modes to be used for pad.
    //     ALT0 (0) - Select instance: ecspi1 signal: ECSPI1_SS2
    //     ALT1 (1) - Select instance: enet signal: ENET_TX_DATA2
    //     ALT2 (2) - Select instance: flexcan1 signal: FLEXCAN1_RX
    //     ALT3 (3) - Select instance: kpp signal: KEY_ROW2
    //     ALT4 (4) - Select instance: usdhc2 signal: SD2_VSELECT
    //     ALT5 (5) - Select instance: gpio4 signal: GPIO4_IO11
    //     ALT6 (6) - Select instance: hdmi signal: HDMI_TX_CEC_LINE
    HW_IOMUXC_SW_MUX_CTL_PAD_KEY_ROW2_WR(
            BF_IOMUXC_SW_MUX_CTL_PAD_KEY_ROW2_SION_V(DISABLED) | 
            BF_IOMUXC_SW_MUX_CTL_PAD_KEY_ROW2_MUX_MODE_V(ALT2));
    // Pad Control Register:
    // IOMUXC_SW_PAD_CTL_PAD_KEY_ROW2(0x020E0648)
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
    HW_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW2_WR(
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
    // IOMUXC_FLEXCAN1_RX_SELECT_INPUT(0x020E07C8)
    //   DAISY [1:0] - MUX Mode Select Field Reset: GPIO08_ALT3
    //                 Selecting Pads Involved in Daisy Chain.
    //     GPIO08_ALT3 (0) - Select signal flexcan1 FLEXCAN1_RX as input from pad GPIO08(ALT3).
    //     KEY_ROW2_ALT2 (1) - Select signal flexcan1 FLEXCAN1_RX as input from pad KEY_ROW2(ALT2).
    //     SD3_CLK_ALT2 (2) - Select signal flexcan1 FLEXCAN1_RX as input from pad SD3_CLK(ALT2).
    HW_IOMUXC_FLEXCAN1_RX_SELECT_INPUT_WR(
            BF_IOMUXC_FLEXCAN1_RX_SELECT_INPUT_DAISY_V(GPIO08_ALT3));

    // Config flexcan1.FLEXCAN1_TX to pad GPIO07(R3)
    // HW_IOMUXC_SW_MUX_CTL_PAD_GPIO07_WR(0x00000003);
    // HW_IOMUXC_SW_PAD_CTL_PAD_GPIO07_WR(0x0001B0B0);
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
            BF_IOMUXC_SW_MUX_CTL_PAD_GPIO07_MUX_MODE_V(ALT3));
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
}
