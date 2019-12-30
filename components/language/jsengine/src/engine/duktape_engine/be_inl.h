/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef BONE_ENGINE_INL_H
#define BONE_ENGINE_INL_H

#include <stdint.h>
#include "jse_common.h"
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

/* addons */
void module_adc_register(void);
void module_builtin_register(void);
void module_dac_register(void);
void module_gpio_register(void);
void module_i2c_register(void);
void module_ir_register(void);
void module_lcd_register(void);
void module_miio_register(void);
void module_mqtt_register(void);
void module_process_register(void);
void module_pwm_register(void);
void module_rtc_register(void);
void module_timer_register(void);
void module_uart_register(void);
void module_wdg_register(void);
void module_wifi_register(void);
void module_udp_register(void);
void module_fs_register(void);

#ifdef __cplusplus
}
#endif

#endif /* BONE_ENGINE_INL_H */
