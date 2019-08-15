/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef BE_ADDON_H
#define BE_ADDON_H

#include "be.h"

void module_builtin_load(void);
void module_process_load(void);
void module_wifi_register(void);
/*
 * JS MQTT Class
 *
 * API:
 * MQTT.start(config, callback)
 * MQTT.subscribe("reply", function(message){});
 * MQTT.unsubscribe("reply");
 * MQTT.publish(topic, payload);
 *
 * Event:
 */
void module_mqtt_register(void);
void module_adc_register(void);
void module_dac_register(void);
void module_gpio_register(void);
void module_i2c_register(void);
void module_ir_register(void);
void module_lcd_register(void);
void module_pwm_register(void);
void module_rtc_register(void);
void module_uart_register(void);
void module_wdg_register(void);
void module_http_register(void);
void module_net_register(void);
void module_udp_register(void);
void module_fs_register(void);
void module_miio_register(void);

#endif /* BE_ADDON_H */
