/* Auto-generated config file winc1500_config.h */
#ifndef WILC1000_CONFIG_H
#define WILC1000_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

#include <stdio.h>
// <h> Basic configuration

// <q> Enable Wi-Fi Management
// <i> Enable Wi-Fi Management
// <id> wifi_mgmt_enable
#ifndef CONF_MGMT
#define CONF_MGMT 0
#endif
// #ifndef CONF_WILC_USE_SPI
// #define CONF_WILC_USE_SPI 1
// #endif
#ifndef CONF_WILC_USE_SDIO
#define CONF_WILC_USE_SDIO 1
#endif
#define CONF_WILC_DEBUG (1)
#define CONF_WILC_PRINTF printf
#define CONF_WILC_USE_1000_REV_B
#define CONF_DRIVER_LOCK				1
#if CONF_DRIVER_LOCK
#  include <os/include/os_hook.h>
#  define DRIVER_SIGNAL_ISR				os_hook_isr
#else
#  define DRIVER_SIGNAL_ISR()
#endif
// </h>
#define SAME54
#define CONF_WILC_USE_SDIO_EXT_IRQ 1
// <<< end of configuration section >>>

#endif // WINC1500_CONFIG_H
