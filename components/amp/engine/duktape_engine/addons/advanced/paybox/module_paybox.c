/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdarg.h>

#include "amp_config.h"
#include "aos_system.h"
#include "amp_defines.h"
#include "amp_task.h"
#include "be_inl.h"

#include "alipay_box_sdk.h"
#include "alipay_box_sdk_api.h"
#include "alipay_xp_sdk_api.h"
#include "alipay_xp_sdk_ex_api.h"
#include "ant_kal.h"

#ifdef CUSTOM_PROFILE
#include "custom_profile.h"
#endif

#define MOD_STR "PAYBOX"

#define PAYBOX_APP_VERSION    "1.2.1"

typedef struct {
    const ant_char* mqtt_product_key;
    const ant_char* supplier_id;
    const ant_char* item_id;
    const ant_char* box_model;
    const ant_char* box_version;
#ifdef USE_SERVER_XP
    const ant_char *xp_device_secret;
    const ant_char* xp_product_key;
    const ant_char* xp_product_secret;
#endif
} alipay_box_sdk_profile_t;

typedef struct paybox_module {
    uint8_t flag;
    //uint8_t data[128];
    int js_cb_ref;
} paybox_module_t;

static paybox_module_t g_paybox_module;
static void (*g_network_status_cb)(int);

static ant_char *__paybox_dn_dynamic_get(void)
{
#define XP_DN_MAX  200
    static ant_char s_dn[XP_DN_MAX+1] = {0};
    if(0 == strlen(s_dn)){
        ant_s32 ret = ant_system_get_sn(s_dn,XP_DN_MAX); 
        if(0 != ret){
           ant_memset(s_dn, 0x0, XP_DN_MAX);
           amp_error(MOD_STR, "%s: get sn null,ret:%d", __func__,ret);
           return NULL;
        }
    }
    amp_debug(MOD_STR, "%s: %s", __func__, s_dn);
    return s_dn;
}

static char *__paybox_string_duplicate(char *src)
{
    char   *dst;
    size_t  len = 0;

    if (src == NULL) {
        return NULL;
    }

    len = strlen(src);

    dst = aos_malloc(len+1);
    if (dst == NULL) {
        return NULL;
    }

    memcpy(dst, src, len);
    dst[len] = '\0';
    return dst;
}

static int native_paybox_ota_attach(void)
{
    static xp_ota_api_t pos_api = {0};

    pos_api.start = ant_fota_start;
    pos_api.w = ant_fota_write;
    pos_api.upgrade  = ant_fota_upgrade;
    pos_api.get_max_size = ant_fota_get_max;
    pos_api.erase_block = ant_fota_block_erase;
    pos_api.end = ant_fota_end;
    pos_api.get_security = ant_fota_get_security;

    return alipay_xp_sdk_fota_api_attach(pos_api);
}

static void native_paybox_event_notify(char *name)
{
    duk_context *ctx;

    if (g_paybox_module.flag == 0) {
        return;
    }

    ctx = be_get_context();
    be_push_ref(ctx, g_paybox_module.js_cb_ref);

    duk_push_string(ctx, name);
    duk_push_object(ctx);

    if (duk_pcall(ctx, 2) != DUK_EXEC_SUCCESS) {
        amp_console("%s", duk_safe_to_stacktrace(ctx, -1));
    }

    duk_pop(ctx);

    duk_gc(ctx, 0);
}


static void native_paybox_event_init_notify(char *name, alipay_box_sdk_init_event_t *data)
{
    duk_context *ctx;

    if (name == NULL) {
        return;
    }

    if (data == NULL) {
        return;
    }

    if (g_paybox_module.flag == 0) {
        return;
    }

    ctx = be_get_context();
    be_push_ref(ctx, g_paybox_module.js_cb_ref);

    duk_push_string(ctx, name);

    duk_push_object(ctx);
    duk_push_int(ctx, data->result);
    duk_put_prop_string(ctx, -2, "result");

    if (duk_pcall(ctx, 2) != DUK_EXEC_SUCCESS) {
        amp_console("%s", duk_safe_to_stacktrace(ctx, -1));
    }

    duk_pop(ctx);

    duk_gc(ctx, 0);
}

static void native_paybox_event_login_notify(char *name, alipay_box_login_event_t *data)
{
    duk_context *ctx;

    if (name == NULL) {
        return;
    }

    if (data == NULL) {
        return;
    }

    if (g_paybox_module.flag == 0) {
        return;
    }

    ctx = be_get_context();
    be_push_ref(ctx, g_paybox_module.js_cb_ref);

    duk_push_string(ctx, name);

    duk_push_object(ctx);
    duk_push_int(ctx, data->bind_status);
    duk_put_prop_string(ctx, -2, "status");

    if (duk_pcall(ctx, 2) != DUK_EXEC_SUCCESS) {
        amp_console("%s", duk_safe_to_stacktrace(ctx, -1));
    }

    duk_pop(ctx);

    duk_gc(ctx, 0);
}


static void native_paybox_event_trade_notify(char *name, alipay_box_trade_event_t *data)
{
    duk_context *ctx;

    if (name == NULL) {
        return;
    }

    if (data == NULL) {
        return;
    }

    if (g_paybox_module.flag == 0) {
        return;
    }

    ctx = be_get_context();
    be_push_ref(ctx, g_paybox_module.js_cb_ref);

    duk_push_string(ctx, name);
    duk_push_object(ctx);

    duk_push_string(ctx, data->trade_id);
    duk_put_prop_string(ctx, -2, "tradeid");
    duk_push_string(ctx, data->money);
    duk_put_prop_string(ctx, -2, "amount");

    duk_push_int(ctx, data->money_switch);
    duk_put_prop_string(ctx, -2, "moneySwitch");

#ifdef ALIPAY_BOX_ISV
    duk_push_string(ctx, data->prefix);
    duk_put_prop_string(ctx, -2, "prefix");

    duk_push_string(ctx, data->suffix);
    duk_put_prop_string(ctx, -2, "suffix");
#else
    duk_push_string(ctx, data->file_id);
    duk_put_prop_string(ctx, -2, "fileId");
#endif

    if (duk_pcall(ctx, 2) != DUK_EXEC_SUCCESS) {
        amp_console("%s", duk_safe_to_stacktrace(ctx, -1));
    }

    duk_pop(ctx);

    duk_gc(ctx, 0);
}

static void native_paybox_sdk_event_handler(alipay_box_sdk_event_t *event, void *user_data)
{
    amp_debug(MOD_STR, "%s: receive event: %d", __func__, event->event_id);
    switch(event->event_id)
    {
    case ALIPAY_BOX_EVENT_SDK_INIT:
        amp_debug(MOD_STR, "%s: alipay_box_sdk init result: %d", __func__, event->data->init.result);
        native_paybox_event_init_notify("init", &(event->data->init));
        break;

    case ALIPAY_BOX_EVENT_SERVER_CONNECTED:
        amp_debug(MOD_STR, "Paybox server connected");
        native_paybox_event_notify("connect");
        break;

    case ALIPAY_BOX_EVENT_SERVER_DISCONNECTED:
        amp_debug(MOD_STR, "Paybox server disconnected");
        native_paybox_event_notify("disconnect");
        break;

    case ALIPAY_BOX_EVENT_LOGIN:
        amp_debug(MOD_STR, "Paybox login with '%s'", (event->data->login.bind_status == 1) ? "BIND" : "UNBIND");
        native_paybox_event_login_notify("login", &event->data->login);
        break;

    case ALIPAY_BOX_EVENT_BIND:
        amp_debug(MOD_STR, "Paybox bind");
        native_paybox_event_notify("bind");
        break;

    case ALIPAY_BOX_EVENT_UNBIND:
        amp_debug(MOD_STR, "Paybox unbind");
        native_paybox_event_notify("unbind");
        break;

    case ALIPAY_BOX_EVENT_TRADE: {
            alipay_box_trade_event_t *trade = &event->data->trade;
            amp_debug(MOD_STR, "Paybox trade event: %s", trade->money);
            native_paybox_event_trade_notify("trade", &event->data->trade);
        }
        break;

    default:
        break;
    }
}

void native_paybox_network_status_register(void (*cb)(int))
{
    g_network_status_cb = cb;
    amp_info(MOD_STR, "register network status callback success");
}

static void native_paybox_event_handler(alipay_box_event_t *event, void *usr_data)
{
    amp_debug(MOD_STR, "%s: receive event %d", __func__, event->event_id - ANT_MSG_ID_IDX_BASE);
    switch (event->event_id)
    {
    case ANT_EVENT_KEY:
        amp_debug(MOD_STR, "Paybox key");
        break;

    case ANT_EVENT_TIMESYNC:
        amp_debug(MOD_STR, "Paybox time sync");
        //alipay_box_sdk_init(native_paybox_sdk_event_handler, NULL, ALIPAY_BOX_MODE_NORMAL);
        break;

    case ANT_EVENT_NETWORK:
        amp_debug(MOD_STR, "Paybox network");
        if (g_network_status_cb)
            g_network_status_cb(1);
        break;

    case ANT_EVENT_IDLE_EVENT:
        amp_debug(MOD_STR, "Paybox idle");
        break;
    case ANT_EVENT_BATTERY_CHARGE_STATUS:
        amp_debug(MOD_STR, "Paybox battery charge status");
        break;

    case ANT_EVENT_KEY_FUNC_LONG_PRESSED:
        amp_debug(MOD_STR, "Paybox key long press");
        break;

    default:
        break;
    }
}

static int native_paybox_shutdown_handler(void)
{
    amp_debug(MOD_STR, "Paybox shut down");
    if (g_paybox_module.flag == 1) {
        native_paybox_event_notify("shutdown");
    }
    return 1;
}

static duk_ret_t native_paybox_open(duk_context *ctx)
{
    alipay_box_sdk_profile_t profile;
    alipay_xp_sdk_device_config_t devcfg;
    char *str;
    ant_char *p_dn = NULL;
    static char init_ok = 0;
    char dn_static = 1;
    int ret = -1;

    if (init_ok)
    {
        amp_warn(MOD_STR, "paybox init already, ignore");
        ret = 0;
        goto out;
    }

    p_dn = __paybox_dn_dynamic_get();
    if(!p_dn)
    {
        dn_static = 0;
    }

    if (!duk_is_object(ctx, 0))
    {
        amp_warn(MOD_STR, "parameter must be object and function");
        ret = -1;
        goto out;
    }

    duk_get_prop_string(ctx, 0, "mqttPrductKey");
    duk_get_prop_string(ctx, 0, "supplierId");
    duk_get_prop_string(ctx, 0, "itemId");
    if (!duk_is_string(ctx, -3) || !duk_is_string(ctx, -2) ||
        !duk_is_string(ctx, -1))
    {
        amp_warn(MOD_STR,
            "Parameter 1 must be an object like {mqttPrductKey: string, supplierId: string, itemId: string, "
            "boxModel: string, boxVersion: string, xpPrductKey: string, xpPrductSecret: string ");
        ret = -1;
        goto out;
    }

    str = (char*)duk_get_string(ctx, -3);
    profile.mqtt_product_key  = __paybox_string_duplicate(str);
    str = (char*)duk_get_string(ctx, -2);
    profile.supplier_id  = __paybox_string_duplicate(str);
    str = (char*)duk_get_string(ctx, -1);
    profile.item_id  = __paybox_string_duplicate(str);

    duk_pop_3(ctx);

    duk_get_prop_string(ctx, 0, "boxModel");
    duk_get_prop_string(ctx, 0, "boxVersion");
    if (!duk_is_string(ctx, -2) || !duk_is_string(ctx, -1))
    {
        amp_warn(MOD_STR,
            "Parameter 1 must be an object like {mqttPrductKey: string, supplierId: string, itemId: string, "
            "boxModel: string, boxVersion: string, xpPrductKey: string, xpPrductSecret: string ");
        ret = -1;
        goto out;
    }

    str = (char*)duk_get_string(ctx, -2);
    profile.box_model  = __paybox_string_duplicate(str);
    str = (char*)duk_get_string(ctx, -1);
    profile.box_version  = __paybox_string_duplicate(str);

    duk_pop_2(ctx);

    duk_get_prop_string(ctx, 0, "xpPrductKey");
    duk_get_prop_string(ctx, 0, "xpPrductSecret");
    duk_get_prop_string(ctx, 0, "xpDeviceSecret");
    if (!duk_is_string(ctx, -3) || !duk_is_string(ctx, -2) || !duk_is_string(ctx, -1))
    {
        amp_warn(MOD_STR,
            "Parameter 1 must be an object like {mqttPrductKey: string, supplierId: string, itemId: string, "
            "boxModel: string, boxVersion: string, xpPrductKey: string, xpPrductSecret: string ");
        ret = -1;
        goto out;
    }

    str = (char*)duk_get_string(ctx, -3);
    profile.xp_product_key  = __paybox_string_duplicate(str);
    str = (char*)duk_get_string(ctx, -2);
    profile.xp_product_secret  = __paybox_string_duplicate(str);
    str = (char *)duk_get_string(ctx, -1);
    profile.xp_device_secret = __paybox_string_duplicate(str);

    duk_pop_3(ctx);

#if 0 //may be required in the future
    if (ret = alipay_box_sdk_set_mqtt_product_key(profile.mqtt_product_key))
    {
        amp_warn(MOD_STR, "alipay_box_sdk_set_mqtt_product_key failed!");
        ret = -1;
        goto out;
    }
    if (strcmp(profile.supplier_id, "null") &&
        (ret = alipay_box_sdk_set_supplier_id(profile.supplier_id)) != 0)
    {
        amp_warn(MOD_STR, "alipay_box_sdk_set_supplier_id failed!");
        ret = -1;
        goto out;
    }
    if (strcmp(profile.item_id, "null") &&
        (ret = alipay_box_sdk_set_item_id(profile.item_id)) != 0)
    {
        amp_warn(MOD_STR, "alipay_box_sdk_set_item_id failed!");
        ret = -1;
        goto out;
    }
    if (ret = alipay_box_sdk_set_box_model(profile.box_model))
    {
        amp_warn(MOD_STR, "alipay_box_sdk_set_box_model failed!");
        ret = -1;
        goto out;
    }
    if (ret = alipay_box_sdk_set_box_version(profile.box_version))
    {
        amp_warn(MOD_STR, "alipay_box_sdk_set_box_version failed!");
        ret = -1;
        goto out;
    }
    if (ret = alipay_box_sdk_set_xp_product_key(profile.xp_product_key))
    {
        amp_warn(MOD_STR, "alipay_box_sdk_set_xp_product_key failed!");
        ret = -1;
        goto out;
    }
    if (ret = alipay_box_sdk_set_xp_product_secret(profile.xp_product_secret))
    {
        amp_warn(MOD_STR, "alipay_box_sdk_set_xp_product_secret failed!");
        ret = -1;
        goto out;
    }
#endif

    memset(&devcfg, 0, sizeof(devcfg));
    devcfg.product_key = profile.xp_product_key;
    devcfg.product_secret = profile.xp_product_secret;
    devcfg.device_secret = profile.xp_device_secret;
    devcfg.version = PAYBOX_APP_VERSION;
    devcfg.device_name = p_dn;
    devcfg.dn_api = __paybox_dn_dynamic_get;

    if (!dn_static && !devcfg.dn_api) {
        amp_error(MOD_STR, "deviname is not found and devicename dynamic is null");
        ret = -1;
        goto out;
    }

    ret = alipay_xp_sdk_device_init(devcfg);
    if (ret) {
        amp_error(MOD_STR, "xp sdk device init fail %d", ret);
        ret = -1;
        goto out;
    }

    ret = native_paybox_ota_attach();
    if (ret) {
        amp_error(MOD_STR, "paybox ota attach fail %d", ret);
        ret = -1;
        goto out;
    }

    ret = alipay_sdk_init(native_paybox_event_handler, native_paybox_sdk_event_handler,
        native_paybox_shutdown_handler, NULL, NULL, NULL, ALIPAY_BOX_MODE_NORMAL);
    if (ret) {
        amp_error(MOD_STR, "alipaybox sdk init fail %d", ret);
        ret = -1;
        goto out;
    }
    amp_debug(MOD_STR, "%s: alipay sdk init success", __func__);
    init_ok = 1;

    duk_push_int(ctx, 0);
    return 1;

out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_paybox_on_data(duk_context *ctx)
{
    int ret = -1;

    if (!duk_is_function(ctx, 1)) {
        amp_warn(MOD_STR, "parameter must be handle, function");
        goto out;
    }

    duk_dup(ctx, 1);
    g_paybox_module.js_cb_ref = be_ref(ctx);
    g_paybox_module.flag = 1;

    ret = 1;
out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_paybox_close(duk_context *ctx)
{
    int ret = -1;

    if (!duk_is_number(ctx, 0)) {
        amp_warn(MOD_STR, "parameter must be number");
        goto out;
    }

    memset(&g_paybox_module, 0, sizeof(g_paybox_module));

    duk_push_int(ctx, 0);
    return 1;

out:
    duk_push_int(ctx, ret);
    return 1;
}

static void native_paybox_clean(void)
{

}

void module_paybox_register(void)
{
    duk_context *ctx = be_get_context();

    amp_module_free_register(native_paybox_clean);

    memset(&g_paybox_module, 0, sizeof(g_paybox_module));

    duk_push_object(ctx);

    AMP_ADD_FUNCTION("open",  native_paybox_open, 2);
    AMP_ADD_FUNCTION("on",    native_paybox_on_data, 2);
    AMP_ADD_FUNCTION("close", native_paybox_close, 1);

    duk_put_prop_string(ctx, -2, "PAYBOX");
}

