/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef BONE_ENGINE_INL_H
#define BONE_ENGINE_INL_H

#include <stdint.h>
#include "duktape/duktape.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * get JSEngine context
 */
duk_context *be_get_context();

/*
 * Create a global array refs in the heap stash
 */
void be_ref_setup(duk_context *ctx);

/*
 * saving top of stack to refs, return refs position. (NOTE: top of stack
 * overflow)
 *
 */
int be_ref(duk_context *ctx);

/*
 * get ref from refs
 */
void be_push_ref(duk_context *ctx, int ref);

/*
 * clear ref in refs
 */
void be_unref(duk_context *ctx, int ref);

/*
 * module manage initial
 */
void be_module_node_init(duk_context *ctx);

/*
 * set entry file path, e.g. /spiffs/index.js
 */
void be_module_node_set_entry(duk_context *ctx, const char *entry);

/*
 * JSEngine log prefix, identification for be-cli
 */
#define BonePrefix "BoneEngine > "

#define AMP_ADD_GLOBAL_FUNCTION(FUNCTION_NAME_STRING, FUNCTION_NAME, PARAM_COUNT) \
		duk_push_c_function(ctx, FUNCTION_NAME, PARAM_COUNT); \
		duk_put_global_string(ctx, FUNCTION_NAME_STRING)

#define AMP_ADD_FUNCTION(FUNCTION_NAME_STRING, FUNCTION_NAME, PARAM_COUNT) \
		duk_push_c_function(ctx, FUNCTION_NAME, PARAM_COUNT); \
		duk_put_prop_string(ctx, -2, FUNCTION_NAME_STRING)

#define AMP_ADD_INT(INT_NAME, INT_VALUE) \
		duk_push_int(ctx, INT_VALUE); \
		duk_put_prop_string(ctx, -2, INT_NAME)

#define AMP_ADD_STRING(STRING_NAME, STRING_VALUE) \
		duk_push_string(ctx, STRING_VALUE); \
		duk_put_prop_string(ctx, -2, STRING_NAME)

#define AMP_ADD_BOOLEAN(BOOLEAN_NAME, BOOLEAN_VALUE) \
		duk_push_boolean(ctx, BOOLEAN_VALUE); \
		duk_put_prop_string(ctx, -2, BOOLEAN_NAME)

/* addons */
void module_adc_register(void);
void module_builtin_register(void);
void module_checksum_register(void);
void module_dac_register(void);
void module_gpio_register(void);
void module_i2c_register(void);
void module_ir_register(void);
void module_lcd_register(void);
void module_miio_register(void);
void module_mqtt_register(void);
void module_os_register(void);
void module_crypto_register(void);
void module_pwm_register(void);
void module_rtc_register(void);
void module_timer_register(void);
void module_uart_register(void);
void module_wdg_register(void);
void module_wifi_register(void);
void module_udp_register(void);
void module_http_register(void);
void module_fs_register(void);
void module_aiot_register(void);
void module_app_ota_register(void);
void module_audioplayer_register(void);
void module_tts_register(void);
void module_location_register(void);
void module_paybox_register(void);
void module_smartcard_register(void);

void module_kv_register(void);
void module_pm_register(void);
void module_battery_register(void);
void module_charger_register(void);
void module_wakelock_register(void);
void module_cellular_register(void);
void module_tcp_register(void);
void module_aiot_register(void);
void module_spi_register(void);
void module_systimer_register(void);
void module_system_register(void);
void module_vm_register(void);
void module_ui_register(void);
void module_keypad_register(void);
void module_bt_host_register(void);
void module_blecfgnet_register(void);

int32_t repl_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BONE_ENGINE_INL_H */
