/**
 * @file      board_api.h
 * @copyright Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef BOARD_API_H
#define BOARD_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include "k_api.h"

/**
  * @brief Board Initialization Function. This function is defined in app,
  * it initializes the peripherals required by the application.
  *
  * @param None
  * @retval None
  */
void board_init(void);

/**
  * @general Board basic initialization. At the time of this function call, the kernel has not been initialized, so the system interface cannot be used.
  *
  * @param None
  * @retval None
  */
void board_basic_init(void);

/**
  * @general Initialize the system ticks
  *
  * @param None
  * @retval None
  */
void board_tick_init(void);

/**
  * @general Initializes the standard serial port, which is normally used as
  * the standard input/output
  *
  * @param None
  * @retval None
  */
void board_stduart_init(void);

/**
  * @brief Initialize the watchdog, which will also complete the setting for feeding the dog.
  * After calling this function to start the watchdog, the user does not need to perform the
  * dog-feeding operation, so the initialization here means that all the initialization related
  * to the watchdog has been completed.
  *
  * @param None
  * @retval None
  */
void board_wdg_init(void);

/**
  * @brief Initialize GPIO pins, such as initializing GPIO controller, pin mapping.
  *
  * @param None
  * @retval None
  */
void board_gpio_init(void);

/**
  * @brief flash control Initialization
  * @param None
  * @retval None
  */
void board_flash_init(void);

/**
  * @brief Initializes network related peripherals, such as wifi and Ethernet
  *
  * @param None
  * @retval None
  */
void board_network_init(void);

/**
  * @brief ota Initialization Function. Perform OTA related processing during the
  * initialization phase, such as detecting whether a rollback is required.
  *
  * @param None
  * @retval None
  */
void board_ota_init(void);

/**
  * @brief Initialize the DMA controller.
  *
  * @param None
  * @retval None
  */
void board_dma_init(void);

#ifdef __cplusplus
}
#endif

#endif

