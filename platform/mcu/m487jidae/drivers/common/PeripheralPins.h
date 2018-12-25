/* Microcontroller Library
 * Copyright (c) 2015-2016 Nuvoton
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/ 

#ifndef SOC_PERIPHERALPINS_H
#define SOC_PERIPHERALPINS_H

#include "pinmap.h"
#include "PeripheralNames.h"

#ifdef __cplusplus
extern "C" {
#endif


//*** ADC ***
extern const PinMap PinMap_ADC[];

//*** DAC ***
extern const PinMap PinMap_DAC[];

//*** I2C_SDA ***
extern const PinMap PinMap_I2C_SDA[];

//*** I2C_SCL ***
extern const PinMap PinMap_I2C_SCL[];

//*** PWM ***
extern const PinMap PinMap_PWM[];

//*** UART_TX ***
extern const PinMap PinMap_UART_TX[];

//*** UART_RX ***
extern const PinMap PinMap_UART_RX[];

//*** UART_RTS ***
extern const PinMap PinMap_UART_RTS[];

//*** UART_CTS ***
extern const PinMap PinMap_UART_CTS[];

//*** SPI_MOSI ***
extern const PinMap PinMap_SPI_MOSI[];

//*** SPI_MISO ***
extern const PinMap PinMap_SPI_MISO[];

//*** SPI_SCLK ***
extern const PinMap PinMap_SPI_SCLK[];

//*** SPI_SSEL ***
extern const PinMap PinMap_SPI_SSEL[];

//*** QSPI_D0 ***
extern const PinMap PinMap_QSPI_D0[];

//*** QSPI_D1 ***
extern const PinMap PinMap_QSPI_D1[];

//*** QSPI_D2 ***
extern const PinMap PinMap_QSPI_D2[];

//*** QSPI_D3 ***
extern const PinMap PinMap_QSPI_D3[];

//*** QSPI_SCLK ***
extern const PinMap PinMap_QSPI_SCLK[];

//*** QSPI_SSEL ***
extern const PinMap PinMap_QSPI_SSEL[];

//*** SDH_DAT0 ***
extern const PinMap PinMap_SDH_DAT0[];

//*** SDH_DAT1 ***
extern const PinMap PinMap_SDH_DAT1[];

//*** SDH_DAT2 ***
extern const PinMap PinMap_SDH_DAT2[];

//*** SDH_DAT3 ***
extern const PinMap PinMap_SDH_DAT3[];

//*** SDH_CMD ***
extern const PinMap PinMap_SDH_CMD[];

//*** SDH_CLK ***
extern const PinMap PinMap_SDH_CLK[];

//*** SDH_CD ***
extern const PinMap PinMap_SDH_CD[];

//*** CAN_TX ***
extern const PinMap PinMap_CAN_TX[];

//*** CAN_RX ***
extern const PinMap PinMap_CAN_RX[];

#ifdef __cplusplus
}
#endif

#endif //SOC_PERIPHERALPINS_H

