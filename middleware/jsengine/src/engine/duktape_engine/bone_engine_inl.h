/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef BONE_ENGINE_INL_H
#define BONE_ENGINE_INL_H

#include <stdint.h>
#include "be_jse_export.h"
#include "duktape/duktape.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * 获取JSE上下文
 */
duk_context *bone_engine_get_context();

/*
 * Create a global array refs in the heap stash
 */
void bone_engine_ref_setup(duk_context *ctx);

/*
 * 存储top of stack对象到refs中并返回对象在refs中的存放位置 (注意:top
 * of stack对象会出栈)
 *
 * 返回值: 0 - 失败; >0 - 成功
 */
int bone_engine_ref(duk_context *ctx);

/*
 * 获取refs中ref位置的对象
 */
void bone_engine_push_ref(duk_context *ctx, int ref);

/*
 * 清空存储在refs中ref位置的对象并腾出ref位置
 */
void bone_engine_unref(duk_context *ctx, int ref);

/*
 * 初始node.js方式的模块管理
 */
void be_module_node_init(duk_context *ctx);

/*
 * 设置入口文件路径
 * entry － 入口文件路径，需要全路径，如：/spiffs/index.js
 */
void be_module_node_set_entry(duk_context *ctx, const char *entry);

/*
 * 引擎通过bone_websocket_send_frame发出的消息需要带上
 * `BoneEngine > `前缀
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
