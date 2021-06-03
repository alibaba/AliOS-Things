/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdarg.h>

#include "amp_config.h"
#include "aos_system.h"
#include "amp_defines.h"
#include "aos_network.h"
#include "be_inl.h"

#define MOD_STR "CELLULAR"

typedef struct {
    int status;
    int js_cb_ref;
} network_status_notify_param_t;

static int g_js_cb_ref = 0;
/*************************************************************************************
 * Function:    native_aiot_close
 * Description: js native addon for
 *            UDP.close(sock_id)
 * Called by:   js api
 * Input:       sock_id: interger
 *
 * Output:      return 0 when UDP.close call ok
 *            return error number UDP.close call fail
 **************************************************************************************/
static duk_ret_t native_cellular_get_simInfo(duk_context *ctx)
{
    int ret     = -1;
    aos_sim_info_t sim_info;

    memset(&sim_info, 0, sizeof(sim_info));

    ret = aos_get_sim_info(&sim_info);
    if (ret != 0) {
        amp_debug(MOD_STR, "get sim card info failed");
        goto out;
    }

    duk_push_object(ctx);

    AMP_ADD_STRING("imsi",  sim_info.imsi);
    AMP_ADD_STRING("imei",  sim_info.imei);
    AMP_ADD_STRING("iccid", sim_info.iccid);

    return 1;

out:
    duk_push_int(ctx, ret);
    return 1;
}

/*************************************************************************************
 * Function:    native_aiot_close
 * Description: js native addon for
 *            UDP.close(sock_id)
 * Called by:   js api
 * Input:       sock_id: interger
 *
 * Output:      return 0 when UDP.close call ok
 *            return error number UDP.close call fail
 **************************************************************************************/
static duk_ret_t native_cellular_get_locatorInfo(duk_context *ctx)
{
    int ret     = -1;
    aos_locator_info_t locator_info;

    memset(&locator_info, 0, sizeof(locator_info));

    ret = aos_get_locator_info(&locator_info);
    if (ret != 0) {
        amp_debug(MOD_STR, "get locator card info failed");
        goto out;
    }

    duk_push_object(ctx);

    AMP_ADD_INT("cellid",   locator_info.cellid);
    AMP_ADD_INT("lac",      locator_info.lac);
    AMP_ADD_STRING("mcc",   locator_info.mcc);
    AMP_ADD_STRING("mnc",   locator_info.mnc);
    AMP_ADD_INT("signal",   locator_info.signal);

    return 1;

out:
    duk_push_int(ctx, ret);
    return 1;
}

/*************************************************************************************
 * Function:    native_aiot_close
 * Description: js native addon for
 *            UDP.close(sock_id)
 * Called by:   js api
 * Input:       sock_id: interger
 *
 * Output:      return 0 when UDP.close call ok
 *            return error number UDP.close call fail
 **************************************************************************************/
void cellInfo_receive_callback(aos_locator_info_t *locator_info, int cell_num)
{
    int ret     = -1;
    int i;

    duk_context *ctx = be_get_context();
    be_push_ref(ctx, g_js_cb_ref);
    int arr_idx = duk_push_array(ctx);
    for (i = 0; i < cell_num; i++) {
        duk_push_object(ctx);

        AMP_ADD_INT("cellid",   locator_info[i].cellid);
        AMP_ADD_INT("lac",      locator_info[i].lac);
        AMP_ADD_STRING("mcc",   locator_info[i].mcc);
        AMP_ADD_STRING("mnc",   locator_info[i].mnc);

        duk_put_prop_index(ctx, arr_idx, i);
    }
    if (duk_pcall(ctx, 1) != DUK_EXEC_SUCCESS) {
        amp_console("%s", duk_safe_to_stacktrace(ctx, -1));
    }
    duk_pop(ctx);
}

static duk_ret_t native_cellular_neighborCellInfo(duk_context *ctx)
{
    int ret     = -1;
    // int i,cellnum = 0;

    // if (!duk_is_function(ctx, 0)) {
    //     amp_warn(MOD_STR, "parameter must be function");
    //     goto out;
    // }

    // duk_dup(ctx, 1);
    // g_js_cb_ref = be_ref(ctx);

    ret = aos_get_neighbor_locator_info(cellInfo_receive_callback);
    if (ret != 0) {
        amp_debug(MOD_STR, "get locator card info failed");
        goto out;
    }

    return 1;

out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_cellular_getStatus(duk_context *ctx)
{
    int ret     = -1;

    ret = aos_get_network_status();
    if (ret != 1) {
        amp_debug(MOD_STR, "network status disconnect %d", ret);
        goto out;
    }

out:
    duk_push_int(ctx, ret);
    return 1;
}

static void network_status_notify(void *pdata)
{
    int i                      = 0;
    network_status_notify_param_t *p = (network_status_notify_param_t *)pdata;
    duk_context *ctx           = be_get_context();

    be_push_ref(ctx, p->js_cb_ref);
    duk_push_int(ctx, p->status);
    if (duk_pcall(ctx, 1) != DUK_EXEC_SUCCESS) {
        amp_console("%s", duk_safe_to_stacktrace(ctx, -1));
    }
    aos_free(p);
    duk_pop(ctx);
    duk_gc(ctx, 0);
}

static void network_status_callback(int status, void *args)
{
    int js_cb_ref = (int)args;
    network_status_notify_param_t *p = aos_calloc(1, sizeof(network_status_notify_param_t));
    if (!p) {
        amp_warn(MOD_STR, "allocate memory failed");
        duk_context *ctx = be_get_context();
        be_unref(ctx, js_cb_ref);
        return;
    }

    p->status = status;
    p->js_cb_ref = js_cb_ref;

    amp_task_schedule_call(network_status_notify, p);
}

static duk_ret_t native_cellular_onconnect(duk_context *ctx)
{
    int ret     = -1;
    int js_cb_ref;

    if (!duk_is_function(ctx, 0)) {
        amp_warn(MOD_STR, "parameter must be function");
        goto out;
    }

    duk_dup(ctx, 0);
    js_cb_ref = be_ref(ctx);

    ret = aos_network_status_registercb(network_status_callback, js_cb_ref);
    if (ret != 0) {
        duk_context *ctx = be_get_context();
        be_unref(ctx, js_cb_ref);
        return -1;
    }

out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_get_netshare_mode(duk_context *ctx)
{
    int ret = -1;

    ret = aos_get_netsharemode();

out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_set_netshare_mode(duk_context *ctx)
{
    int ret = -1;
    int share_mode = 0;

    if (!duk_is_number(ctx, 0)) {
        amp_warn(MOD_STR, "parameter must be number");
        goto out;
    }

    share_mode = duk_get_number(ctx, 0);
    amp_error(MOD_STR, "native set net share mode = %d", share_mode);
    ret = aos_set_netsharemode(share_mode);
    if (ret != 0) {
        return -1;
    }
    amp_error(MOD_STR, "native set net share mode success");
out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_get_netshare_config(duk_context *ctx)
{
    int ret     = -1;
    aos_sharemode_info_t *share_mode_info;

    share_mode_info = aos_malloc(sizeof(aos_sharemode_info_t));
    if (share_mode_info == NULL) {
        amp_debug(MOD_STR, "get net share config failed");
        goto out;
    }
    memset(share_mode_info, 0, sizeof(aos_sharemode_info_t));

    ret = aos_get_netshareconfig(share_mode_info);
    if (ret != 0) {
        amp_debug(MOD_STR, "get net share config failed");
        goto out;
    }

    duk_push_object(ctx);

    AMP_ADD_INT("action",  share_mode_info->action);
    AMP_ADD_INT("auto_connect",  share_mode_info->auto_connect);
    AMP_ADD_STRING("apn", share_mode_info->apn);
    AMP_ADD_STRING("username",   share_mode_info->username);
    AMP_ADD_STRING("password",      share_mode_info->password);
    AMP_ADD_INT("ip_type",   share_mode_info->ip_type);
    AMP_ADD_INT("share_mode",   share_mode_info->share_mode);

    aos_free(share_mode_info);
    return 1;

out:
    if (share_mode_info) {
        aos_free(share_mode_info);
    }
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_set_netshare_config(duk_context *ctx)
{
    int ret     = -1;
    aos_sharemode_info_t *share_mode_info;

    /* check paramters */
    if (!duk_is_object(ctx, 0))
    {
        amp_warn(MOD_STR, "parameter must be object\n");
        goto out;
    }

    /* get device certificate */
    duk_get_prop_string(ctx, 0, "ucid");
    duk_get_prop_string(ctx, 0, "action");
    duk_get_prop_string(ctx, 0, "autoConnect");
    duk_get_prop_string(ctx, 0, "apn");
    duk_get_prop_string(ctx, 0, "username");
    duk_get_prop_string(ctx, 0, "password");
    duk_get_prop_string(ctx, 0, "authType");
    duk_get_prop_string(ctx, 0, "ipType");

    // if (!duk_is_number(ctx, -8) || !duk_is_number(ctx, -7) ||
    //     !duk_is_number(ctx, -6) || !duk_is_string(ctx, -5) ||
    //     !duk_is_string(ctx, -4) || !duk_is_string(ctx, -3) ||
    //     !duk_is_number(ctx, -2) || !duk_is_number(ctx, -1))
    // {
    //     amp_warn(MOD_STR,
    //         "Parameter 1 must be an object like {host: string, "
    //         "port: uint, client_id: string, username: string, "
    //         "password: string, keepalive_interval: uint} %d %d %d %d %d %d %d %d %d", duk_is_number(ctx, -8), duk_is_number(ctx, -7),
    //         duk_is_number(ctx, -6), duk_is_string(ctx, -5), duk_is_string(ctx, -4), duk_is_string(ctx, -3), duk_is_string(ctx, -2),
    //         duk_is_boolean(ctx, -2), duk_is_number(ctx, -1));
    //     // duk_pop_n(ctx, 8);
    //     // goto out;
    // }

    const uint16_t ipType = duk_get_number(ctx, -1);
    const uint16_t authType = duk_get_number(ctx, -2);
    const char *password = duk_get_string(ctx, -3);
    const char *username = duk_get_string(ctx, -4);
    const char *apn = duk_get_string(ctx, -5);
    const uint16_t autoConnect = duk_get_number(ctx, -6);
    const uint16_t action = duk_get_number(ctx, -7);
    const uint16_t ucid = duk_get_number(ctx, -8);

    share_mode_info = aos_malloc(sizeof(aos_sharemode_info_t));
    if (share_mode_info == NULL) {
        amp_debug(MOD_STR, "set net share config failed");
        goto out;
    }
    memset(share_mode_info, 0, sizeof(aos_sharemode_info_t));

    share_mode_info->action = action;
    share_mode_info->auto_connect = autoConnect;
    memcpy(share_mode_info->apn, apn, strlen(apn));
    memcpy(share_mode_info->username, username, strlen(username));
    memcpy(share_mode_info->password, password, strlen(password));
    share_mode_info->ip_type = ipType;

    ret = aos_set_netshareconfig(ucid, authType, share_mode_info);
    if (ret != 0) {
        amp_warn(MOD_STR, "amp set net share config failed!");
        aos_free(share_mode_info);
        return -1;
    }

out:
    if (share_mode_info) {
        aos_free(share_mode_info);
    }
    duk_push_int(ctx, ret);
    return 1;
}

void module_cellular_register(void)
{
    duk_context *ctx = be_get_context();

    duk_push_object(ctx);

    AMP_ADD_FUNCTION("getSimInfo",          native_cellular_get_simInfo, 0);
    AMP_ADD_FUNCTION("getLocatorInfo",      native_cellular_get_locatorInfo, 0);
    AMP_ADD_FUNCTION("getStatus",           native_cellular_getStatus, 0);
    AMP_ADD_FUNCTION("onConnect",           native_cellular_onconnect, 1);
    AMP_ADD_FUNCTION("getNeighborCellInfo", native_cellular_neighborCellInfo, 0);
    AMP_ADD_FUNCTION("getNetSharemode",     native_get_netshare_mode, 0);
    AMP_ADD_FUNCTION("setNetSharemode",     native_set_netshare_mode, 1);
    AMP_ADD_FUNCTION("getNetShareconfig",   native_get_netshare_config, 0);
    AMP_ADD_FUNCTION("setNetShareconfig",   native_set_netshare_config, 1);

    duk_put_prop_string(ctx, -2, "CELLULAR");
}
