/*
 * The Clear BSD License
 * Copyright (c) 2017, NXP Semiconductor, Inc.
 * All rights reserved.
 *
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 * that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
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
 *
 */

/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!GlobalInfo
product: Pins v3.0
processor: LPC54018
package_id: LPC54018JET180
mcu_data: ksdk2_0
processor_version: 0.0.0
pin_labels:
- {pin_num: A14, pin_signal: PIO4_7/CTIMER4_CAP3/USB0_PORTPWRN/USB0_FRAME/SCT0_GPI0, label: PWRON, identifier: PWRON}
- {pin_num: P11, pin_signal: PIO1_22/FC8_RTS_SCL_SSEL1/SD_CMD/CTIMER2_MAT3/SCT0_GPI5/FC4_SSEL3/EMC_CKE(1), label: IRQ, identifier: IRQ}
- {pin_num: A3, pin_signal: PIO3_10/SCT0_OUT3/CTIMER3_MAT0/EMC_DYCSN(1)/TRACEDATA(0), label: PWRON, identifier: PWRON}
- {pin_num: C10, pin_signal: PIO3_2/LCD_VD(16)/FC9_RXD_SDA_MOSI/CTIMER1_MAT2, label: IRQ, identifier: IRQ}
- {pin_num: E3, pin_signal: PIO3_14/SCT0_OUT4/FC9_RTS_SCL_SSEL1/CTIMER3_MAT1/TRACEDATA(2), label: KFFET, identifier: KFFET}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */

#include "fsl_common.h"
#include "fsl_iocon.h"
#include "pin_mux.h"

/*FUNCTION**********************************************************************
 * 
 * Function Name : BOARD_InitBootPins
 * Description   : Calls initialization functions.
 * 
 *END**************************************************************************/
void BOARD_InitBootPins(void) {
    BOARD_InitPins();
}

#define IOCON_PIO_DIGITAL_EN        0x0100u   /*!< Enables digital function */
#define IOCON_PIO_FUNC1               0x01u   /*!< Selects pin function 1 */
#define IOCON_PIO_FUNC2               0x02u   /*!< Selects pin function 2 */
#define IOCON_PIO_INPFILT_OFF       0x0200u   /*!< Input filter disabled */
#define IOCON_PIO_INV_DI              0x00u   /*!< Input function is not inverted */
#define IOCON_PIO_MODE_INACT          0x00u   /*!< No addition pin function */
#define IOCON_PIO_MODE_PULLUP         0x20u   /*!< Selects pull-up function */
#define IOCON_PIO_OPENDRAIN_DI        0x00u   /*!< Open drain is disabled */
#define IOCON_PIO_SLEW_STANDARD       0x00u   /*!< Standard mode, output slew rate control is enabled */
#define PIN0_IDX                         0u   /*!< Pin number for pin 0 in a port 0 */
#define PIN1_IDX                         1u   /*!< Pin number for pin 1 in a port 0 */
#define PIN2_IDX                         2u   /*!< Pin number for pin 2 in a port 0 */
#define PIN3_IDX                         3u   /*!< Pin number for pin 3 in a port 0 */
#define PIN29_IDX                       29u   /*!< Pin number for pin 29 in a port 0 */
#define PIN30_IDX                       30u   /*!< Pin number for pin 30 in a port 0 */
#define PORT0_IDX                        0u   /*!< Port index */

/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
BOARD_InitPins:
- options: {callFromInitBoot: 'true', coreID: core0, enableClock: 'true'}
- pin_list:
  - {pin_num: A2, peripheral: FLEXCOMM0, signal: TXD_SCL_MISO, pin_signal: PIO0_30/FC0_TXD_SCL_MISO/CTIMER0_MAT0/SCT0_OUT9/TRACEDATA(1), mode: inactive, invert: disabled,
    glitch_filter: disabled, slew_rate: standard, open_drain: disabled}
  - {pin_num: B13, peripheral: FLEXCOMM0, signal: RXD_SDA_MOSI, pin_signal: PIO0_29/FC0_RXD_SDA_MOSI/CTIMER2_MAT3/SCT0_OUT8/TRACEDATA(2), mode: inactive, invert: disabled,
    glitch_filter: disabled, slew_rate: standard, open_drain: disabled}
  - {pin_num: D6, peripheral: FLEXCOMM3, signal: SCK, pin_signal: PIO0_0/CAN1_RD/FC3_SCK/CTIMER0_MAT0/SCT0_GPI0/PDM0_CLK, mode: pullUp, invert: disabled, glitch_filter: disabled,
    slew_rate: standard, open_drain: disabled}
  - {pin_num: A1, peripheral: FLEXCOMM3, signal: CTS_SDA_SSEL0, pin_signal: PIO0_1/CAN1_TD/FC3_CTS_SDA_SSEL0/CTIMER0_CAP0/SCT0_GPI1/PDM0_DATA, mode: pullUp, invert: disabled,
    glitch_filter: disabled, slew_rate: standard, open_drain: disabled}
  - {pin_num: E9, peripheral: FLEXCOMM3, signal: TXD_SCL_MISO, pin_signal: PIO0_2/FC3_TXD_SCL_MISO/CTIMER0_CAP1/SCT0_OUT0/SCT0_GPI2/EMC_D(0), mode: pullUp, invert: disabled,
    glitch_filter: disabled, slew_rate: standard, open_drain: disabled}
  - {pin_num: A10, peripheral: FLEXCOMM3, signal: RXD_SDA_MOSI, pin_signal: PIO0_3/FC3_RXD_SDA_MOSI/CTIMER0_MAT1/SCT0_OUT1/SCT0_GPI3/EMC_D(1), mode: pullUp, invert: disabled,
    glitch_filter: disabled, slew_rate: standard, open_drain: disabled}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */

/*FUNCTION**********************************************************************
 *
 * Function Name : BOARD_InitPins
 *
 *END**************************************************************************/
void BOARD_InitPins(void) { /* Function assigned for the Core #0 (ARM Cortex-M4) */
  CLOCK_EnableClock(kCLOCK_Iocon);                           /* Enables the clock for the IOCON block. 0 = Disable; 1 = Enable.: 0x01u */

  const uint32_t port0_pin0_config = (
    IOCON_PIO_FUNC2 |                                        /* Pin is configured as FC3_SCK */
    IOCON_PIO_MODE_PULLUP |                                  /* Selects pull-up function */
    IOCON_PIO_INV_DI |                                       /* Input function is not inverted */
    IOCON_PIO_DIGITAL_EN |                                   /* Enables digital function */
    IOCON_PIO_INPFILT_OFF |                                  /* Input filter disabled */
    IOCON_PIO_SLEW_STANDARD |                                /* Standard mode, output slew rate control is enabled */
    IOCON_PIO_OPENDRAIN_DI                                   /* Open drain is disabled */
  );
  IOCON_PinMuxSet(IOCON, PORT0_IDX, PIN0_IDX, port0_pin0_config); /* PORT0 PIN0 (coords: D6) is configured as FC3_SCK */
  const uint32_t port0_pin1_config = (
    IOCON_PIO_FUNC2 |                                        /* Pin is configured as FC3_CTS_SDA_SSEL0 */
    IOCON_PIO_MODE_PULLUP |                                  /* Selects pull-up function */
    IOCON_PIO_INV_DI |                                       /* Input function is not inverted */
    IOCON_PIO_DIGITAL_EN |                                   /* Enables digital function */
    IOCON_PIO_INPFILT_OFF |                                  /* Input filter disabled */
    IOCON_PIO_SLEW_STANDARD |                                /* Standard mode, output slew rate control is enabled */
    IOCON_PIO_OPENDRAIN_DI                                   /* Open drain is disabled */
  );
  IOCON_PinMuxSet(IOCON, PORT0_IDX, PIN1_IDX, port0_pin1_config); /* PORT0 PIN1 (coords: A1) is configured as FC3_CTS_SDA_SSEL0 */
  const uint32_t port0_pin2_config = (
    IOCON_PIO_FUNC1 |                                        /* Pin is configured as FC3_TXD_SCL_MISO */
    IOCON_PIO_MODE_PULLUP |                                  /* Selects pull-up function */
    IOCON_PIO_INV_DI |                                       /* Input function is not inverted */
    IOCON_PIO_DIGITAL_EN |                                   /* Enables digital function */
    IOCON_PIO_INPFILT_OFF |                                  /* Input filter disabled */
    IOCON_PIO_SLEW_STANDARD |                                /* Standard mode, output slew rate control is enabled */
    IOCON_PIO_OPENDRAIN_DI                                   /* Open drain is disabled */
  );
  IOCON_PinMuxSet(IOCON, PORT0_IDX, PIN2_IDX, port0_pin2_config); /* PORT0 PIN2 (coords: E9) is configured as FC3_TXD_SCL_MISO */
  const uint32_t port0_pin29_config = (
    IOCON_PIO_FUNC1 |                                        /* Pin is configured as FC0_RXD_SDA_MOSI */
    IOCON_PIO_MODE_INACT |                                   /* No addition pin function */
    IOCON_PIO_INV_DI |                                       /* Input function is not inverted */
    IOCON_PIO_DIGITAL_EN |                                   /* Enables digital function */
    IOCON_PIO_INPFILT_OFF |                                  /* Input filter disabled */
    IOCON_PIO_SLEW_STANDARD |                                /* Standard mode, output slew rate control is enabled */
    IOCON_PIO_OPENDRAIN_DI                                   /* Open drain is disabled */
  );
  IOCON_PinMuxSet(IOCON, PORT0_IDX, PIN29_IDX, port0_pin29_config); /* PORT0 PIN29 (coords: B13) is configured as FC0_RXD_SDA_MOSI */
  const uint32_t port0_pin3_config = (
    IOCON_PIO_FUNC1 |                                        /* Pin is configured as FC3_RXD_SDA_MOSI */
    IOCON_PIO_MODE_PULLUP |                                  /* Selects pull-up function */
    IOCON_PIO_INV_DI |                                       /* Input function is not inverted */
    IOCON_PIO_DIGITAL_EN |                                   /* Enables digital function */
    IOCON_PIO_INPFILT_OFF |                                  /* Input filter disabled */
    IOCON_PIO_SLEW_STANDARD |                                /* Standard mode, output slew rate control is enabled */
    IOCON_PIO_OPENDRAIN_DI                                   /* Open drain is disabled */
  );
  IOCON_PinMuxSet(IOCON, PORT0_IDX, PIN3_IDX, port0_pin3_config); /* PORT0 PIN3 (coords: A10) is configured as FC3_RXD_SDA_MOSI */
  const uint32_t port0_pin30_config = (
    IOCON_PIO_FUNC1 |                                        /* Pin is configured as FC0_TXD_SCL_MISO */
    IOCON_PIO_MODE_INACT |                                   /* No addition pin function */
    IOCON_PIO_INV_DI |                                       /* Input function is not inverted */
    IOCON_PIO_DIGITAL_EN |                                   /* Enables digital function */
    IOCON_PIO_INPFILT_OFF |                                  /* Input filter disabled */
    IOCON_PIO_SLEW_STANDARD |                                /* Standard mode, output slew rate control is enabled */
    IOCON_PIO_OPENDRAIN_DI                                   /* Open drain is disabled */
  );
  IOCON_PinMuxSet(IOCON, PORT0_IDX, PIN30_IDX, port0_pin30_config); /* PORT0 PIN30 (coords: A2) is configured as FC0_TXD_SCL_MISO */
}


#define PIO122_DIGIMODE_DIGITAL       0x01u   /*!< Select Analog/Digital mode.: Digital mode. */
#define PIO122_FUNC_ALT0              0x00u   /*!< Selects pin function.: Alternative connection 0. */
#define PIO122_INVERT_DISABLED        0x00u   /*!< Input polarity.: Disabled. Input function is not inverted. */
#define PIO122_MODE_PULL_UP           0x02u   /*!< Selects function mode (on-chip pull-up/pull-down resistor control).: Pull-up. Pull-up resistor enabled. */
#define PIO320_DIGIMODE_DIGITAL       0x01u   /*!< Select Analog/Digital mode.: Digital mode. */
#define PIO320_FUNC_ALT1              0x01u   /*!< Selects pin function.: Alternative connection 1. */
#define PIO321_DIGIMODE_DIGITAL       0x01u   /*!< Select Analog/Digital mode.: Digital mode. */
#define PIO321_FUNC_ALT1              0x01u   /*!< Selects pin function.: Alternative connection 1. */
#define PIO322_DIGIMODE_DIGITAL       0x01u   /*!< Select Analog/Digital mode.: Digital mode. */
#define PIO322_FUNC_ALT1              0x01u   /*!< Selects pin function.: Alternative connection 1. */
#define PIO330_DIGIMODE_DIGITAL       0x01u   /*!< Select Analog/Digital mode.: Digital mode. */
#define PIO330_FUNC_ALT1              0x01u   /*!< Selects pin function.: Alternative connection 1. */
#define PIO47_DIGIMODE_DIGITAL        0x01u   /*!< Select Analog/Digital mode.: Digital mode. */
#define PIO47_FUNC_ALT0               0x00u   /*!< Selects pin function.: Alternative connection 0. */
#define PIO47_INVERT_DISABLED         0x00u   /*!< Input polarity.: Disabled. Input function is not inverted. */
#define PIO47_MODE_PULL_DOWN          0x01u   /*!< Selects function mode (on-chip pull-up/pull-down resistor control).: Pull-down. Pull-down resistor enabled. */

/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
BOARD_InitGT202Shield:
- options: {coreID: core0, enableClock: 'true'}
- pin_list:
  - {pin_num: N2, peripheral: FLEXCOMM9, signal: SCK, pin_signal: PIO3_20/FC9_SCK/SD_CARD_INT_N/CLKOUT/SCT0_OUT7}
  - {pin_num: N5, peripheral: FLEXCOMM9, signal: TXD_SCL_MISO, pin_signal: PIO3_22/FC9_TXD_SCL_MISO/ADC0_10}
  - {pin_num: P5, peripheral: FLEXCOMM9, signal: RXD_SDA_MOSI, pin_signal: PIO3_21/FC9_RXD_SDA_MOSI/SD_BACKEND_PWR/CTIMER4_MAT3/UTICK_CAP2/ADC0_9}
  - {pin_num: K13, peripheral: FLEXCOMM9, signal: CTS_SDA_SSEL0, pin_signal: PIO3_30/FC9_CTS_SDA_SSEL0/SCT0_OUT4/FC4_SSEL2/EMC_A(19)}
  - {pin_num: A14, peripheral: GPIO, signal: 'PIO4, 7', pin_signal: PIO4_7/CTIMER4_CAP3/USB0_PORTPWRN/USB0_FRAME/SCT0_GPI0, direction: OUTPUT, mode: pullDown, invert: disabled}
  - {pin_num: P11, peripheral: GPIO, signal: 'PIO1, 22', pin_signal: PIO1_22/FC8_RTS_SCL_SSEL1/SD_CMD/CTIMER2_MAT3/SCT0_GPI5/FC4_SSEL3/EMC_CKE(1), direction: INPUT,
    mode: pullUp, invert: disabled}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */

/*FUNCTION**********************************************************************
 *
 * Function Name : BOARD_InitGT202Shield
 *
 *END**************************************************************************/
void BOARD_InitGT202Shield(void) { /* Function assigned for the Core #0 (ARM Cortex-M4) */
  CLOCK_EnableClock(kCLOCK_Iocon);                           /* Enables the clock for the IOCON block. 0 = Disable; 1 = Enable.: 0x01u */

  IOCON->PIO[1][22] = ((IOCON->PIO[1][22] &
    (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_MODE_MASK | IOCON_PIO_INVERT_MASK | IOCON_PIO_DIGIMODE_MASK))) /* Mask bits to zero which are setting */
      | IOCON_PIO_FUNC(PIO122_FUNC_ALT0)                     /* Selects pin function.: PORT122 (pin P11) is configured as PIO1_22 */
      | IOCON_PIO_MODE(PIO122_MODE_PULL_UP)                  /* Selects function mode (on-chip pull-up/pull-down resistor control).: Pull-up. Pull-up resistor enabled. */
      | IOCON_PIO_INVERT(PIO122_INVERT_DISABLED)             /* Input polarity.: Disabled. Input function is not inverted. */
      | IOCON_PIO_DIGIMODE(PIO122_DIGIMODE_DIGITAL)          /* Select Analog/Digital mode.: Digital mode. */
    );
  IOCON->PIO[3][20] = ((IOCON->PIO[3][20] &
    (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))      /* Mask bits to zero which are setting */
      | IOCON_PIO_FUNC(PIO320_FUNC_ALT1)                     /* Selects pin function.: PORT320 (pin N2) is configured as FC9_SCK */
      | IOCON_PIO_DIGIMODE(PIO320_DIGIMODE_DIGITAL)          /* Select Analog/Digital mode.: Digital mode. */
    );
  IOCON->PIO[3][21] = ((IOCON->PIO[3][21] &
    (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))      /* Mask bits to zero which are setting */
      | IOCON_PIO_FUNC(PIO321_FUNC_ALT1)                     /* Selects pin function.: PORT321 (pin P5) is configured as FC9_RXD_SDA_MOSI */
      | IOCON_PIO_DIGIMODE(PIO321_DIGIMODE_DIGITAL)          /* Select Analog/Digital mode.: Digital mode. */
    );
  IOCON->PIO[3][22] = ((IOCON->PIO[3][22] &
    (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))      /* Mask bits to zero which are setting */
      | IOCON_PIO_FUNC(PIO322_FUNC_ALT1)                     /* Selects pin function.: PORT322 (pin N5) is configured as FC9_TXD_SCL_MISO */
      | IOCON_PIO_DIGIMODE(PIO322_DIGIMODE_DIGITAL)          /* Select Analog/Digital mode.: Digital mode. */
    );
  IOCON->PIO[3][30] = ((IOCON->PIO[3][30] &
    (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))      /* Mask bits to zero which are setting */
      | IOCON_PIO_FUNC(PIO330_FUNC_ALT1)                     /* Selects pin function.: PORT330 (pin K13) is configured as FC9_CTS_SDA_SSEL0 */
      | IOCON_PIO_DIGIMODE(PIO330_DIGIMODE_DIGITAL)          /* Select Analog/Digital mode.: Digital mode. */
    );
  IOCON->PIO[4][7] = ((IOCON->PIO[4][7] &
    (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_MODE_MASK | IOCON_PIO_INVERT_MASK | IOCON_PIO_DIGIMODE_MASK))) /* Mask bits to zero which are setting */
      | IOCON_PIO_FUNC(PIO47_FUNC_ALT0)                      /* Selects pin function.: PORT47 (pin A14) is configured as PIO4_7 */
      | IOCON_PIO_MODE(PIO47_MODE_PULL_DOWN)                 /* Selects function mode (on-chip pull-up/pull-down resistor control).: Pull-down. Pull-down resistor enabled. */
      | IOCON_PIO_INVERT(PIO47_INVERT_DISABLED)              /* Input polarity.: Disabled. Input function is not inverted. */
      | IOCON_PIO_DIGIMODE(PIO47_DIGIMODE_DIGITAL)           /* Select Analog/Digital mode.: Digital mode. */
    );
}

/*******************************************************************************
 * EOF
 ******************************************************************************/
