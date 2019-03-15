/*!
 * \file      board-config.h
 *
 * \brief     Board configuration
 *
 * \copyright Revised BSD License, see section \ref LICENSE.
 *
 * \code
 *                ______                              _
 *               / _____)             _              | |
 *              ( (____  _____ ____ _| |_ _____  ____| |__
 *               \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 *               _____) ) ____| | | || |_| ____( (___| | | |
 *              (______/|_____)_|_|_| \__)_____)\____)_| |_|
 *              (C)2013-2017 Semtech
 *
 *               ___ _____ _   ___ _  _____ ___  ___  ___ ___
 *              / __|_   _/_\ / __| |/ / __/ _ \| _ \/ __| __|
 *              \__ \ | |/ _ \ (__| ' <| _| (_) |   / (__| _|
 *              |___/ |_/_/ \_\___|_|\_\_| \___/|_|_\\___|___|
 *              embedded.connectivity.solutions===============
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Gregory Cristian ( Semtech )
 *
 * \author    Daniel Jaeckle ( STACKFORCE )
 *
 * \author    Johannes Bruder ( STACKFORCE )
 */
#ifndef __BOARD_CONFIG_H__
#define __BOARD_CONFIG_H__
#include "gpio.h"

/*!
 * Defines the time required for the TCXO to wakeup [ms].
 */

/*
enum {
    RESET_PIN = 50,
    BUSY_PIN,
    DIO1_PIN,
    DIO2_PIN,
    DIO3_PIN,
    MOSI_PIN,
    MISO_PIN,
    SCLK_PIN,
    NSS_PIN,
    ANTPOW_PIN,
    DEVICE_SEL_PIN,
};
*/
/*!
 * STM32 Pin Names
 */

#define BOARD_TCXO_WAKEUP_TIME                      5
#define RADIO_NSS                                   NSS_PIN
#define RADIO_BUSY                                  BUSY_PIN
#define RADIO_DIO_1                                 DIO1_PIN

/*!
 * Board MCU pins definitions
 */
#define RADIO_RESET                                 RESET_PIN

#define RADIO_MOSI                                  MOSI_PIN
#define RADIO_MISO                                  MISO_PIN
#define RADIO_SCLK                                  SCLK_PIN

#define RADIO_ANT_SWITCH_POWER                      ANTPOW_PIN
//#define RADIO_FREQ_SEL                              PA_1
//#define RADIO_XTAL_SEL                              PB_0
#define RADIO_DEVICE_SEL                            DEVICE_SEL_PIN

//#define LED_1                                       PC_1
//#define LED_2                                       PC_0


//#define OSC_LSE_IN                                  PC_14
//#define OSC_LSE_OUT                                 PC_15

//#define OSC_HSE_IN                                  PH_0
//#define OSC_HSE_OUT                                 PH_1

//#define SWCLK                                       PA_14
//#define SWDAT                                       PA_13

//#define I2C_SCL                                     PB_8
//#define I2C_SDA                                     PB_9

//#define UART_TX                                     PA_2
//#define UART_RX                                     PA_3

// Debug pins definition.
#define RADIO_DBG_PIN_TX                            NC
#define RADIO_DBG_PIN_RX                            NC

#endif // __BOARD_CONFIG_H__
