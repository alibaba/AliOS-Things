/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdarg.h>

#include "amp_system.h"
#include "amp_defines.h"
#include "amp_network.h"
#include "dev_model_api.h"
#include "be_inl.h"

#define MOD_STR "CELLULAR"

typedef struct {
    int status;
    int js_cb_ref;
} network_status_notify_param_t;

static int g_js_cb_ref = 0;
extern int update_flag;
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
static duk_ret_t native_cellular_getInfo(duk_context *ctx)
{
    int ret     = -1;
    sim_info_t sim_info;
    locator_info_t locator_info;
    signal_info_t signal_info;

    ret = HAL_Get_SimInfo(&sim_info);
    if (ret != 0) {
        amp_debug(MOD_STR, "get sim card info failed");
        goto out;
    }

    ret = HAL_Get_LocatorInfo(&locator_info);
    if (ret != 0) {
        amp_debug(MOD_STR, "get locator card info failed");
        goto out;
    }

    ret = HAL_Get_SignalInfo(&signal_info);
    if (ret != 0) {
        amp_debug(MOD_STR, "get locator card info failed");
        goto out;
    }

    duk_push_object(ctx);

    duk_push_string(ctx, sim_info.imsi);
    duk_put_prop_string(ctx, -2, "imsi");

    duk_push_string(ctx, sim_info.imei);
    duk_put_prop_string(ctx, -2, "imei");

    duk_push_string(ctx, sim_info.iccid);
    duk_put_prop_string(ctx, -2, "iccid");

    duk_push_int(ctx, locator_info.cid);
    duk_put_prop_string(ctx, -2, "cid");

    duk_push_int(ctx, locator_info.lac);
    duk_put_prop_string(ctx, -2, "lac");

    duk_push_string(ctx, locator_info.mcc);
    duk_put_prop_string(ctx, -2, "mcc");

    duk_push_string(ctx, locator_info.mnc);
    duk_put_prop_string(ctx, -2, "mnc");

    duk_push_int(ctx, signal_info.rssi);
    duk_put_prop_string(ctx, -2, "rssi");

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
void cellInfo_receive_callback(locator_info_t *locator_info, int cell_num)
{
    int ret     = -1;
    int i;

    duk_context *ctx = be_get_context();
    be_push_ref(ctx, g_js_cb_ref);
    int arr_idx = duk_push_array(ctx);
    for (i = 0; i < cell_num; i++) {
        duk_push_object(ctx);

        duk_push_int(ctx, locator_info[i].cid);
        duk_put_prop_string(ctx, -2, "cid");

        duk_push_int(ctx, locator_info[i].lac);
        duk_put_prop_string(ctx, -2, "lac");

        duk_push_string(ctx, locator_info[i].mcc);
        duk_put_prop_string(ctx, -2, "mcc");

        duk_push_string(ctx, locator_info[i].mnc);
        duk_put_prop_string(ctx, -2, "mnc");

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

    ret = HAL_Get_neighborLocatorInfo(cellInfo_receive_callback);
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

    ret = HAL_Get_network_status();
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
    amp_free(p);
    duk_pop(ctx);
    duk_gc(ctx, 0);
}

static void network_status_callback(int status, void *args)
{
    int js_cb_ref = (int)args;
    network_status_notify_param_t *p = amp_calloc(1, sizeof(network_status_notify_param_t));
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

    ret = HAL_network_status_registercb(network_status_callback, js_cb_ref);
    if (ret != 0) {
        duk_context *ctx = be_get_context();
        be_unref(ctx, js_cb_ref);
        return -1;
    }

out:
    duk_push_int(ctx, ret);
    return 1;
}

void module_cellular_register(void)
{
    duk_context *ctx = be_get_context();

    duk_push_object(ctx);

    duk_push_c_function(ctx, native_cellular_getInfo, 0);
    duk_put_prop_string(ctx, -2, "getInfo");

    duk_push_c_function(ctx, native_cellular_getStatus, 0);
    duk_put_prop_string(ctx, -2, "getStatus");

    duk_push_c_function(ctx, native_cellular_onconnect, 1);
    duk_put_prop_string(ctx, -2, "onConnect");

    duk_push_c_function(ctx, native_cellular_neighborCellInfo, 0);
    duk_put_prop_string(ctx, -2, "getNeighborCellInfo");

    duk_put_prop_string(ctx, -2, "CELLULAR");
}
