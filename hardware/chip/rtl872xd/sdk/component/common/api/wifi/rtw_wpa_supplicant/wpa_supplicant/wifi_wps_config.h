/**
  ******************************************************************************
  * @file    wifi_wps_config.h
  * @author
  * @version
  * @brief   This file provides user interfaces for WPS functionality.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */

/** @addtogroup wpsp2p WPS/P2P
 *  @ingroup    wlan
 *  @brief      WPS/P2P functions
 *  @{
 */

/**
 * @brief  Start WPS enrollee process.
 * @warning  Before invoking this function, the Wi-Fi should be enabled by using @ref wifi_on().
 * @param[in]  wps_config: WPS configure method. Options are: WPS_CONFIG_DISPLAY, WPS_CONFIG_KEYPAD, and WPS_CONFIG_PUSHBUTTON.
 * @param[in]  pin: PIN number. Can be set to NULL if using WPS_CONFIG_PUSHBUTTON.
 * @param[in]  channel: Channel. Currently un-used, can be set to 0.
 * @param[in]  ssid: Target network SSID. Can be set to NULL if no target network specified.
 * @return  0 on success.
 * @note  Please make sure CONFIG_ENABLE_WPS is enabled in platform_opts.h.\n
 *			You can reference @ref cmd_wps() to know how to choose input parameters.
 */
int wps_start(u16 wps_config, char *pin, u8 channel, char *ssid);

/**
 * @brief  Command to start WPS enrollee process for commonly use. Can refer to ATWW in atcmd_wifi.c.
 * @warning  Before invoking this function, the Wi-Fi should be enabled by using @ref wifi_on().
 * @param[in]  argc: Command line argument. Argument count.
 * @param[in]  argv: Command line argument. Argument vector.
 * @return  0 on success.
 * @note  Command style for example:
 *			- cmd_wps pbc
 *			- cmd_wps pin
 *			- cmd_wps pin 12345678
 * @note  Please make sure CONFIG_ENABLE_WPS is enabled in platform_opts.h.
 */
void cmd_wps(int argc, char **argv);

/**
 * @brief  Start a WPS registrar thread.
 * @warning Before invoking this function, the Wi-Fi should be in SoftAP mode.
 * @param[in]  config_methods: WPS configure method. Options are: WPS_CONFIG_DISPLAY, WPS_CONFIG_KEYPAD, and WPS_CONFIG_PUSHBUTTON.
 * @param[in]  pin: PIN number. Can be set to NULL.
 * @return  None
 * @note  Please make sure CONFIG_ENABLE_WPS and CONFIG_ENABLE_WPS_AP are enabled in platform_opts.h.\n
 *			You can reference @ref cmd_ap_wps() to know how to choose input parameters.
 */
void wifi_start_ap_wps_thread(u16 config_methods, char *pin);

/**
 * @brief  Command to start WPS registrar process for commonly use. Can refer to ATWw in atcmd_wifi.c.
 * @warning Before invoking this function, the Wi-Fi should be in SoftAP mode.
 * @param[in]  argc: Command line argument. Argument count.
 * @param[in]  argv: Command line argument. Argument vector.
 * @return  0 on success
 * @note  Command style for example:
 *			- cmd_ap_wps pbc
 *			- cmd_ap_wps pin
 *			- cmd_ap_wps pin 12345678
 * @note  Please make sure CONFIG_ENABLE_WPS and CONFIG_ENABLE_WPS_AP are enabled in platform_opts.h.
 */
void cmd_ap_wps(int argc, char **argv);

/* extern function declaration */
extern int wpas_wps_enrollee_init_probe_ie(u16 config_methods);
extern int wpas_wps_enrollee_init_assoc_ie(void);
extern void wpas_wps_deinit(void);
extern unsigned int wps_generate_pin(void);
extern unsigned int wps_pin_valid(unsigned int pin);

/*\@}*/
