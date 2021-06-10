/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef __AMP_REPL_H
#define __AMP_REPL_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef JSE_HW_ADDON_GPIO
void module_gpio_register(void);
#endif

#ifdef JSE_HW_ADDON_IR
void module_ir_register(void);
#endif

#ifdef JSE_HW_ADDON_PWM
void module_pwm_register(void);
#endif

#ifdef JSE_HW_ADDON_ONEWIRE
void module_onewire_register(void);
#endif

#ifdef JSE_HW_ADDON_RTC
void module_rtc_register(void);
#endif

#ifdef JSE_CORE_ADDON_CRYPTO
void module_crypto_register(void);
#endif

#ifdef JSE_HW_ADDON_WDG
void module_wdg_register(void);
#endif

#ifdef JSE_HW_ADDON_UART
void module_uart_register(void);
#endif

#ifdef JSE_HW_ADDON_SPI
void module_spi_register(void);
#endif

#ifdef JSE_HW_ADDON_ADC
void module_adc_register(void);
#endif

#ifdef JSE_HW_ADDON_TIMER
void module_timer_register(void);
#endif

#ifdef JSE_HW_ADDON_DAC
void module_dac_register(void);
#endif

#ifdef JSE_CORE_ADDON_SYSTIMER
void module_systimer_register(void);
#endif

#ifdef JSE_NET_ADDON_HTTP
void module_http_register(void);
#endif

#ifdef JSE_NET_ADDON_NETMGR
void module_netmgr_register(void);
#endif

#ifdef JSE_CORE_ADDON_SYSTEM
void module_system_register(void);
#endif

#ifdef JSE_ADVANCED_ADDON_OTA
void module_app_ota_register(void);
#endif

#ifdef JSE_ADVANCED_ADDON_UI
void page_entry_register(void);
void app_entry_register(void);
void module_ui_register(void);
#endif

#ifdef JSE_HW_ADDON_LCD
void module_lcd_register(void);
#endif

#ifdef JSE_HW_ADDON_I2C
void module_i2c_register(void);
#endif

#ifdef JSE_ADVANCED_ADDON_AIOT_DEVICE
void module_aiot_device_register(void);
#endif

#ifdef JSE_ADVANCED_ADDON_AIOT_GATEWAY
void module_aiot_gateway_register(void);
#endif


#ifdef JSE_CORE_ADDON_LOG
void module_log_register(void);
#endif

#ifdef JSE_WIRELESS_ADDON_BT_HOST
void module_bt_host_register(void);
#endif

#ifdef JSE_CORE_ADDON_FS
void module_fs_register(void);
#endif

#ifdef JSE_CORE_ADDON_KV
void module_kv_register(void);
#endif

#ifdef JSE_ADVANCED_ADDON_AUDIOPLAYER
void module_audio_register(void);
#endif

#ifdef JSE_CORE_ADDON_CHECKSUM
void  module_checksum_register(void);
#endif

#ifdef JSE_ADVANCED_ADDON_BLECFGNET
void module_blecfgnet_register(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __AMP_REPL_H */