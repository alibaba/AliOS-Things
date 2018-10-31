/*----------------------------------------------------------------------------*/
/*
 * Copyright (C) Bosch Connected Devices and Solutions GmbH.
 * All Rights Reserved. Confidential.
 *
 * Distribution only to people who need to know this information in
 * order to do their job.(Need-to-know principle).
 * Distribution to persons outside the company, only if these persons
 * signed a non-disclosure agreement.
 * Electronic transmission, e.g. via electronic mail, must be made in
 * encrypted form.
 */
/*----------------------------------------------------------------------------*/

/**
*  @ingroup CONFIGURATION
*
*  @defgroup BCDS_HALCONFIG BCDS_HALConfig
*  @{
*
*  @brief Contains a default configuration to enable or disable HAL components.
*
*  @details This header file is usually included by the BCDS_HAL.h from the
*  HAL (Hardware Abstraction Layer) module. It is used to allow a per project
*  configuration of the features provided by the HAL component.
*  Features are enabled or disabled by defining a particular feature to either
*  1 or 0 e.g.
*  @code{.c}
*  #define BCDS_FEATURE_I2C    1   // Enable HAL feature I2C
*  #define BCDS_FEATURE_SPI    0   // Disable HAL feature SPI
*  @endcode
*  If no defines are made then all HAL features will be enabled.
*
*  @file
*
*/

#ifndef BCDS_HAL_CONFIG_H_
#define BCDS_HAL_CONFIG_H_

/* Define nothing, so all features are enabled. */
#define BCDS_FEATURE_BSP_SENSOR_NODE        1
#define BCDS_FEATURE_BSP_SD_CARD            1
#define BCDS_FEATURE_I2C                    1
#if (BCDS_EMLIB_INCLUDE_USB == 1)
#define BCDS_FEATURE_BSP_USB                1         /**< USB feature is enabled only if emlib usb is included  */
#else
#define BCDS_FEATURE_BSP_USB                0
#endif
#define BCDS_FEATURE_SPI                    1
#define BCDS_FEATURE_BSP_LED                1
#define BCDS_FEATURE_BSP_BUTTON             1
#define BCDS_FEATURE_HAL_TICK_HANDLER       1
#define BCDS_FEATURE_FLASH                  1
#define BCDS_FEATURE_BSP_SD_CARD            1
#define BCDS_FEATURE_BSP_SD_CARD_AUTO_DETECT 1
#define BCDS_FEATURE_BSP_SD_CARD_AUTO_DETECT_FORCE 0
#define BCDS_FEATURE_UART                   1
#define BCDS_FEATURE_LEUART                 0
#define BCDS_FEATURE_GPIO                   1
#define BCDS_FEATURE_BSP_EXTENSION_GPIO     1
#define BCDS_FEATURE_BSP_MAX31865           1
#define BCDS_FEATURE_BSP_BT_EM9301          1
#define BCDS_FEATURE_BSP_WIFI_CC3100MOD     1
#define BCDS_FEATURE_DMA                    1
#define BCDS_FEATURE_EFM32_BURTC            0
#define BCDS_FEATURE_TIMER                  1
#define BCDS_FEATURE_EFM32_RTC              1
#define BCDS_FEATURE_BSP_EXTENSIONPORT      1
#ifndef BCDS_FEATURE_BSP_LORA_RN2XX3
#define BCDS_FEATURE_BSP_LORA_RN2XX3        (0)         /**< By Default Disabled BCDS_FEATURE_BSP_LORA_RN2XX3 */
#endif
#define BCDS_FEATURE_BSP_CHARGER_BQ2407X    1
#define BCDS_FEATURE_ADC                    1
#define BCDS_FEATURE_BSP_LEM_SENSOR			1
#define BCDS_FEATURE_BSP_IRSENSOR_NODE      1
#define BCDS_FEATURE_BSP_MIC_AKU340			1


/** @todo this change is done only to make Refboards to compile. need to revert back after fixing the build settings of refboards*/
#define BCDS_FEATURE_BSP_UMTS_LISAU2_UART 0

#define BCDS_I2C_COUNT 1
#define BCDS_SPI_COUNT 2
#define BCDS_UART_COUNT 2

#if BCDS_FEATURE_BSP_SENSOR_NODE
#define BSP_XDK_SENSOR_BMA280_ENABLE_INTERRUPT      1
#define BSP_XDK_SENSOR_MAX44009_ENABLE_INTERRUPT    1
#define BSP_XDK_SENSOR_BMA280_FORCE_INTERRUPT       0

#ifndef BSP_XDK_SENSOR_BMG160_ENABLE_INTERRUPT
#define BSP_XDK_SENSOR_BMG160_ENABLE_INTERRUPT      0
#endif

#ifndef BSP_XDK_SENSOR_BMM150_ENABLE_INTERRUPT
#define BSP_XDK_SENSOR_BMM150_ENABLE_INTERRUPT      0
#endif

#define BSP_XDK_SENSOR_BMM150_FORCE_INTERRUPT       0

#ifndef BSP_XDK_SENSOR_BMI160_ENABLE_INTERRUPT
#define BSP_XDK_SENSOR_BMI160_ENABLE_INTERRUPT      0
#endif

#define BSP_XDK_SENSOR_BMI160_FORCE_INTERRUPT       0
#endif

#if BCDS_FEATURE_BSP_BUTTON
#define BSP_XDK_KEY1_INTERRUPT                                                  1
#define BSP_XDK_KEY1_INTERRUPT_FORCE                                            1
#define BSP_XDK_KEY2_INTERRUPT                                                  1
#define BSP_XDK_KEY2_INTERRUPT_FORCE                                            0
#endif /* BCDS_FEATURE_BSP_BUTTON */

#endif /*BCDS_HAL_CONFIG_H_*/

/**@}*/
