/*----------------------------------------------------------------------------*/
/*
 * Copyright (C) Bosch Connected Devices and Solutions GmbH.
 * All Rights Reserved. Confidential.
 *
 * Distribution only to people who need to know this information in
 * Distribution only to people who need to know this information in
 * order to do their job.(Need-to-know principle).
 * Distribution to persons outside the company, only if these persons
 * signed a non-disclosure agreement.
 * Electronic transmission, e.g. via electronic mail, must be made in
 * encrypted form.
 */
/*----------------------------------------------------------------------------*/

/**
 * @ingroup  BCDS_HAL_BSP_IF
 *
 * @defgroup BSP_BOARDEXTSETTINGS  BSP_BoardExtSettings 
 * @{
 * 
 * @brief XDK Extension Board macros definitions
 *
 * @details This file holds static configurations which are meant to be
 * assigned to a peripheral at the XDK110 extension board at a certain point
 * in time for the Initialize, Connect, Enable, Disable, Disconnect functions.
 * It is preferred to have only macro definitions in this file so that it
 * contains compile time necessary information only.
 * 
 * @file
 */

#ifndef BoardExtSettings_H
#define BoardExtSettings_H

#include "BSP_BoardSettings.h"
#include "BCDS_HAL.h"

/* Check feature configure macro BCDS_FEATURE_BSP_LORA_RN2XX3 or BCDS_FEATURE_BSP_UMTS_LISAU2 set for Extension Board's components and display error message if incompatible setting done */
#if( ((BCDS_FEATURE_BSP_LORA_RN2XX3) && (BCDS_FEATURE_BSP_UMTS_LISAU2))|| ((BCDS_FEATURE_BSP_LORA_RN2XX3) && (BCDS_FEATURE_BSP_UMTS_QUECTELUC20))||((BCDS_FEATURE_BSP_UMTS_QUECTELUC20) && (BCDS_FEATURE_BSP_UMTS_LISAU2)))
#error "Multiple XDK extension boards with UART support is enabled"
#endif

/** Default settings to be used as template if none of extension boards connected */
#define UART1_STATE                                                             usartDisable
#define UART1_REF_FREQ                                                          0
#define UART1_BAUDRATE                                                          115200
#define UART1_OVERSAMPLING                                                      usartOVS16
#define UART1_DATABITS                                                          usartDatabits8
#define UART1_PARITY                                                            usartNoParity
#define UART1_STOPBITS                                                          usartStopbits1
#define UART1_MAJORITY_VOTING                                                   false
#define UART1_PRS_RX_ENABLE                                                     false
#define UART1_PRS_RX_CHANNEL                                                    usartPrsRxCh0

#if BCDS_FEATURE_BSP_CHARGER_BQ2405X 
////////////////////////////////////////////////////////////////////////////////EXTERNAL BATTERY CHARGER
/**
 * @ingroup BSP_BOARDEXTSETTINGS
 *
 * @defgroup EXTERNAL_CHARGER_MODULE_SETTINGS External Battery charger extension board pins
 * @{
 *
 * @brief Settings used if the External Battery charger extension module connected to XDK
 *
 * @warning Please do not change these settings, since these are specific to hardware
 */

#define EXT_CHARGE_VREG_NO_BATTERY                                              3680
#define EXT_CHARGE_NO_BATTERY_HIGH_LIMIT                                        600

#define EXT_CHARGE_ON_UPPER_LIMIT                                               4520
#define EXT_CHARGE_ON_LOWER_LIMIT                                               4300

#define EXT_CHARGE_CRITICAL_UPPER_LIMIT                                         3500
#define EXT_CHARGE_CRITICAL_LOWER_LIMIT                                         3300

#define EXT_CHARGE_LOW_UPPER_LIMIT                                              3650
#define EXT_CHARGE_LOW_LOWER_LIMIT                                              3500

#define EXT_CHARGE_NORMAL_UPPER_LIMIT                                           3925
#define EXT_CHARGE_NORMAL_LOWER_LIMIT                                           3650

#define EXT_CHARGE_HIGH_UPPER_LIMIT                                             4300
#define EXT_CHARGE_HIGH_LOWER_LIMIT                                             3925

#define VBAT_SENSE_PIN                                                          (5)
#define VBAT_SENSE_PORT                                                         (gpioPortD)
#define VBAT_SENSE_MODE                                                         (gpioModeDisabled)

#define CHARGING_N_PIN                                                          (8)
#define CHARGING_N_PORT                                                         (gpioPortC)
#define CHARGING_N_MODE                                                         (gpioModeInput)

#define PGOOD_N_PIN                                                             (8)
#define PGOOD_N_PORT                                                            (gpioPortC)
#define PGOOD_N_MODE                                                            (gpioModeInputPull)
/**@}*/
#endif /* BCDS_FEATURE_BSP_CHARGER_BQ2405X */

#if BCDS_FEATURE_BSP_IRSENSOR_NODE 
////////////////////////////////////////////////////////////////////////////////EXTERNAL GRIDEYE SENSOR
/**
 * @ingroup BSP_BOARDEXTSETTINGS
 *
 * @defgroup EXTERNAL_IR_SENSOR_SETTINGS External IR sensor extension board pins
 * @{
 *
 * @brief Settings used if the External IR Sensor Extension module connected to XDK
 *
 * @warning Please do not change these settings, since these are specific to hardware
 */

/*DATA pin settings for connecting IRSENSOR_NODE in Extension Bus */
#define PTD_PORT_EXTENSION_I2C1_SDA                                             (gpioPortB)
#define PTD_PIN_EXTENSION_I2C1_SDA                                              (11)
#define PTD_MODE_EXTENSION_I2C1_SDA                                             (gpioModeWiredAnd)
#define PTD_DOUT_EXTENSION_I2C1_SDA                                             (1)

/*CLOCK pin settings for connecting IRSENSOR_NODE in Extension Bus */
#define PTD_PORT_EXTENSION_I2C1_SCL                                             (gpioPortB)
#define PTD_PIN_EXTENSION_I2C1_SCL                                              (12)
#define PTD_MODE_EXTENSION_I2C1_SCL                                             (gpioModeWiredAnd)
#define PTD_DOUT_EXTENSION_I2C1_SCL                                             (1)
/**@}*/
#endif /* BCDS_FEATURE_BSP_IRSENSOR_NODE */
////////////////////////////////////////////////////////////////////////////////EXTERNAL TEMPERATURE SENSOR

/**
 * @ingroup BSP_BOARDEXTSETTINGS
 *
 * @defgroup EXTERNAL_TEMP_SETTINGS External Temperature Sensor extension board pins
 * @{
 *
 * @brief Settings used if the External Temperature Sensor Extension module connected to XDK
 *
 * @warning Please do not change these settings, since these are specific to hardware
 */

#define EXTENSION_BUS_TEMP_PORT_GPIO_IN_A                                       (gpioPortC)
#define EXTENSION_BUS_TEMP_PIN_GPIO_IN_A                                        (8)
#define EXTENSION_BUS_TEMP_PORT_GPIO_IN_B                                       (gpioPortC)
#define EXTENSION_BUS_TEMP_PIN_GPIO_IN_B                                        (9)

#define EXTENSION_BUS_TEMP_PORT_GPIO_OUT_A                                      (gpioPortA)
#define EXTENSION_BUS_TEMP_PIN_GPIO_OUT_A                                       (1)
#define EXTENSION_BUS_TEMP_PORT_GPIO_OUT_B                                      (gpioPortE)
#define EXTENSION_BUS_TEMP_PIN_GPIO_OUT_B                                       (2)

/**@}*/

////////////////////////////////////////////////////////////////////////////////Extension GPIO Input/Output
#if BCDS_FEATURE_BSP_EXTENSIONPORT
/**
 * @ingroup BSP_BOARDEXTSETTINGS
 *
 * @defgroup EXTENSION_PORT_MODULE_SETTINGS Extension Port pins
 * @{
 *
 * @brief   Settings used for the Extension Port interface external connection to board.
 *
 * @warning Please do not change these settings, since these are specific to hardware
 */

/** Port PA0 can be used as Gpio or Timer0 output pin for compare operations of extension port */
#define  EXTENSION_PORT_GPIO_PA0         gpioPortA
#define  EXTENSION_PIN_GPIO_PA0          0
#define  EXTENSION_MODE_GPIO_PA0         gpioModeDisabled
#define  EXTENSION_DOUT_GPIO_PA0         0

/** Port PC0 can be used as Gpio or Timer0 output pin for compare operations of extension bus */
#define  EXTENSION_PORT_GPIO_PC0        gpioPortC
#define  EXTENSION_PIN_GPIO_PC0         0
#define  EXTENSION_MODE_GPIO_PC0        gpioModeDisabled
#define  EXTENSION_DOUT_GPIO_PC0        0

/** Port PC1 can be used as Gpio or Timer0 output pin for compare operations of extension port */
#define  EXTENSION_PORT_GPIO_PC1         gpioPortC
#define  EXTENSION_PIN_GPIO_PC1          1
#define  EXTENSION_MODE_GPIO_PC1         gpioModeDisabled
#define  EXTENSION_DOUT_GPIO_PC1         0

/** Port PC2 can be used as Gpio or Timer0 output pin for pwm operations of extension port */
#define  EXTENSION_PORT_GPIO_PC2      gpioPortC
#define  EXTENSION_PIN_GPIO_PC2       2
#define  EXTENSION_MODE_GPIO_PC2      gpioModeDisabled
#define  EXTENSION_DOUT_GPIO_PC2      0

/** Port PC2 can be used as Gpio or Timer0 output pin for pwm operations of extension port */
#define  EXTENSION_PORT_GPIO_PC3        gpioPortC
#define  EXTENSION_PIN_GPIO_PC3         3
#define  EXTENSION_MODE_GPIO_PC3        gpioModeDisabled
#define  EXTENSION_DOUT_GPIO_PC3        0

/** Port PC4 can be used as Gpio or Timer0 output pin for pwm operations of extension port */
#define  EXTENSION_PORT_GPIO_PC4      gpioPortC
#define  EXTENSION_PIN_GPIO_PC4       4
#define  EXTENSION_MODE_GPIO_PC4      gpioModeDisabled
#define  EXTENSION_DOUT_GPIO_PC4      0

/** Port PC8 can be used as Gpio or Timer2 input pin for capture operations of extension port */
#define  EXTENSION_PORT_GPIO_PC8       gpioPortC
#define  EXTENSION_PIN_GPIO_PC8        8
#define  EXTENSION_MODE_GPIO_PC8       gpioModeDisabled
#define  EXTENSION_DOUT_GPIO_PC8       0

/** Port PC9 can be used as Gpio or  Timer2 input pin for capture operations of extension port */
#define  EXTENSION_PORT_GPIO_PC9       gpioPortC
#define  EXTENSION_PIN_GPIO_PC9        9
#define  EXTENSION_MODE_GPIO_PC9       gpioModeDisabled
#define  EXTENSION_DOUT_GPIO_PC9       0

/** Port PC10 can be used as Gpio or Timer2 input pin for capture operations of extension port */
#define  EXTENSION_PORT_GPIO_PC10       gpioPortC
#define  EXTENSION_PIN_GPIO_PC10        10
#define  EXTENSION_MODE_GPIO_PC10       gpioModeDisabled
#define  EXTENSION_DOUT_GPIO_PC10       0

/** Port PD5 can be used as Gpio or ADC0 input pin for extension port */
#define  EXTENSION_PORT_GPIO_PD5     gpioPortD
#define  EXTENSION_PIN_GPIO_PD5      5
#define  EXTENSION_MODE_GPIO_PD5     gpioModeDisabled
#define  EXTENSION_DOUT_GPIO_PD5     0

/** Port PD6 can be used as Gpio or ADC0 input pin for extension port */
#define  EXTENSION_PORT_GPIO_PD6     gpioPortD
#define  EXTENSION_PIN_GPIO_PD6      6
#define  EXTENSION_MODE_GPIO_PD6     gpioModeDisabled
#define  EXTENSION_DOUT_GPIO_PD6      0

/** Port PA1 can be used as GPIO input/output pin configuration for extension port */
#define  EXTENSION_PORT_GPIO_PA1              gpioPortA
#define  EXTENSION_PIN_GPIO_PA1               1
#define  EXTENSION_MODE_GPIO_PA1              gpioModeDisabled
#define  EXTENSION_DOUT_GPIO_PA1              0

/** Port PE2 can be used as GPIO input/output pin configuration for extension port */
#define  EXTENSION_PORT_GPIO_PE2               gpioPortE
#define  EXTENSION_PIN_GPIO_PE2                2
#define  EXTENSION_MODE_GPIO_PE2               gpioModeDisabled
#define  EXTENSION_DOUT_GPIO_PE2               0

/** Port PB9 can be used as Gpio or UART TX pin configuration for extension port */
#define  EXTENSION_PORT_GPIO_PB9             gpioPortB
#define  EXTENSION_PIN_GPIO_PB9              9
#define  EXTENSION_MODE_GPIO_PB9             gpioModeDisabled
#define  EXTENSION_DOUT_GPIO_PB9             1

/** Port PB10 can be used as Gpio or UART RX pin configuration for extension port */
#define  EXTENSION_PORT_GPIO_PB10            gpioPortB
#define  EXTENSION_PIN_GPIO_PB10             10
#define  EXTENSION_MODE_GPIO_PB10            gpioModeDisabled
#define  EXTENSION_DOUT_GPIO_PB10            0

/** Port PB2 can be used as GPIO input/output pin configuration for extension port  */
#define  EXTENSION_PORT_GPIO_PB2             gpioPortB
#define  EXTENSION_PIN_GPIO_PB2              2
#define  EXTENSION_MODE_GPIO_PB2             gpioModeDisabled
#define  EXTENSION_DOUT_GPIO_PB2             1

/** Port PF6 can be used as GPIO input/output pin configuration for extension port  */
#define  EXTENSION_PORT_GPIO_PF6             gpioPortF
#define  EXTENSION_PIN_GPIO_PF6              6
#define  EXTENSION_MODE_GPIO_PF6             gpioModeDisabled
#define  EXTENSION_DOUT_GPIO_PF6             0

/** Port PB3 can be used as Gpio or  SPI MOSI pin configuration for extension port */
#define  EXTENSION_PORT_GPIO_PB3               gpioPortB
#define  EXTENSION_PIN_GPIO_PB3                3
#define  EXTENSION_MODE_GPIO_PB3               gpioModeDisabled
#define  EXTENSION_DOUT_GPIO_PB3               0

/** Port PB4 can be used as Gpio or  SPI MISO pin configuration for extension port */
#define  EXTENSION_PORT_GPIO_PB4              gpioPortB
#define  EXTENSION_PIN_GPIO_PB4               4
#define  EXTENSION_MODE_GPIO_PB4              gpioModeDisabled
#define  EXTENSION_DOUT_GPIO_PB4              0

/**  Port PD8 can be used as GPIO input/output pin configuration for extension ports*/
#define  EXTENSION_PORT_GPIO_PD8                gpioPortD
#define  EXTENSION_PIN_GPIO_PD8                 8
#define  EXTENSION_MODE_GPIO_PD8                gpioModeDisabled
#define  EXTENSION_DOUT_GPIO_PD8                1

/** Port PB5 can be used as Gpio or SPI clock pin configuration for extension port */
#define  EXTENSION_PORT_GPIO_PB5               gpioPortB
#define  EXTENSION_PIN_GPIO_PB5                5
#define  EXTENSION_MODE_GPIO_PB5               gpioModeDisabled
#define  EXTENSION_DOUT_GPIO_PB5               0

/** Port PB11 can be used as Gpio or DATA pin for I2C in extension port */
#define  EXTENSION_PORT_GPIO_PB11              gpioPortB
#define  EXTENSION_PIN_GPIO_PB11               11
#define  EXTENSION_MODE_GPIO_PB11              gpioModeDisabled
#define  EXTENSION_DOUT_GPIO_PB11              1

/** Port PB12 can be used as Gpio or CLOCK pin for I2C in extension port */
#define  EXTENSION_PORT_GPIO_PB12               gpioPortB
#define  EXTENSION_PIN_GPIO_PB12                12
#define  EXTENSION_MODE_GPIO_PB12               gpioModeDisabled
#define  EXTENSION_DOUT_GPIO_PB12               1

#define EXTENSION_PORT_I2C_BUS                  I2C1            /**< MCU I2C1 peripheral base pointer*/
#define EXTENSION_PORT_I2C_CLOCK                (cmuClock_I2C1) /**< MCU I2C1 peripheral clock enumeration value*/
#define EXTENSION_PORT_I2C_IRQn                 (I2C1_IRQn)     /**< MCU I2C1 peripheral interrupt number enumeration value*/
#define EXTENSION_PORT_I2C_NVIC_PRIORITY         UINT32_C(5)     /**< MCU I2C1 peripheral interrupt controller(NVIC) priority value*/

#define  EXTENSION_I2C_SDA_MODE                 gpioModeWiredAnd
#define  EXTENSION_I2C_SDA_DOUT                 1

#define  EXTENSION_ADC_CHANNEL_PIN_MODE         gpioModeInput
#define  EXTENSION_ADC_CHANNEL_PIN_DOUT         1

#define  EXTENSION_I2C_SCK_MODE                 gpioModeWiredAnd
#define  EXTENSION_I2C_SCK_DOUT                 1

#define EXTENSIONPORT_SPI_FREQUENCY      		HSE_VALUE

#define EXTENSIONPORT_SPI_TX_DMA_CHANNEL        (4)
#define EXTENSIONPORT_SPI_RX_DMA_CHANNEL        (5)

#define EXTENSIONPORT_SPI_RX_IRQN				USART2_RX_IRQn
#define EXTENSIONPORT_SPI_TX_IRQN               USART2_TX_IRQn

#define EXTENSIONPORT_SPI_RX_INT_PRIORITY       UINT32_C(5)
#define EXTENSIONPORT_SPI_TX_INT_PRIORITY       UINT32_C(5)

#define EXTENSIONPORT_SPI_STATE                                                             usartDisable
#define EXTENSIONPORT_SPI_REF_FREQ                                                          EXTENSIONPORT_SPI_FREQUENCY
#define EXTENSIONPORT_SPI_BAUDRATE                                                          1000000
#define EXTENSIONPORT_SPI_DATABITS                                                          usartDatabits8
#define EXTENSIONPORT_SPI_MASTERMODE_ENABLE                                                 true
#define EXTENSIONPORT_SPI_MSB_ENABLE                                                        false
#define EXTENSIONPORT_SPI_CLOCKMODE                                                         usartClockMode0
#define EXTENSIONPORT_SPI_PRS_RX_ENABLE                                                     false
#define EXTENSIONPORT_SPI_PRS_RX_CHANNEL                                                    usartPrsRxCh0
#define EXTENSIONPORT_SPI_AUTOTX_ENABLE                                                     false

#define EXTENSION_PORT_UART_RX_NVIC_PRIORITY                                           UINT32_C(5)
#define EXTENSION_PORT_UART_TX_NVIC_PRIORITY                                           EXTENSION_PORT_UART_RX_NVIC_PRIORITY

/**@}*/

#endif /* BCDS_FEATURE_BSP_EXTENSIONPORT */
/**@}*/

#endif /* BoardExtSettings_H */
/**@}*/
