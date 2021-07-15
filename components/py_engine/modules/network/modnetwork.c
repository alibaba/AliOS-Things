#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "py/stackctrl.h"

#include "HaasLog.h"
#include "modnetwork.h"
#include "wlan_mgnt.h"
#include "../video/videocommon.h"
#include "netdev.h"

#define AOS_WLAN_CFG_GOT_SSID_PASSWD AOS_WLAN_DEV_EVT_MAX
#define NETWORK_QR_RESULT_MAX_LEN   (256)

struct network_ctx_t {
    uint8_t type;
    uint8_t usbcam;
    uint8_t cam_fps;
    uint8_t finish;
    uint8_t got_ssid_pwd;
    ip4_addr_t ip_addr;
    char result[NETWORK_QR_RESULT_MAX_LEN];

    mp_obj_t evt_cb;
};

static struct network_ctx_t nctx;

void network_evt_callback(int eid)
{
    mp_obj_t callback = nctx.evt_cb;
    if (callback != mp_const_none) {
        mp_state_thread_t ts; // local thread state for the ISR
#if MICROPY_PY_THREAD
        void *old_state = (void *)mp_thread_get_state();
        mp_thread_set_state(&ts);
#endif
        mp_stack_set_top(&ts + 1);          // need to include ts in root-pointer scan
        mp_stack_set_limit(1024);           // tune based on ISR thread stack size
        mp_locals_set(mp_locals_get());     // use main thread's locals
        mp_globals_set(mp_globals_get());   // use main thread's globals

        mp_sched_lock(); // prevent VM from switching to another MicroPython thread
#if MICROPY_ENABLE_GC
        gc_lock();       // prevent memory allocation
#endif
        // execute MicroPython APIs
        mp_call_function_1_protected(callback, mp_obj_new_int(eid));
#if MICROPY_ENABLE_GC
        gc_unlock();
#endif
        mp_sched_unlock();
#if MICROPY_PY_THREAD
        mp_thread_set_state(old_state);
#endif
    }
}

static void wifi_event_cb(uint32_t event_id, const void* data, uint32_t len, void *parameter)
{
    switch (event_id) {
        case AOS_WLAN_EVT_READY:
            //network_evt_callback(NETWORK_EVT_GOT_IP);
            nctx.finish = 1;
            break;
        case AOS_WLAN_EVT_STA_CONNECTED_FAIL:
            nctx.got_ssid_pwd = 0;
            //network_evt_callback(NETWORK_EVT_CONN_FAIL);
            break;
        case AOS_WLAN_EVT_STA_NETWORK_CONNECTED:
            memcpy(&nctx.ip_addr, data, sizeof(nctx.ip_addr));
            nctx.finish = 1;
            break;
        case AOS_WLAN_EVT_STA_CONNECTED:
            //network_evt_callback(NETWORK_EVT_GOT_IP);
            nctx.finish = 1;
            break;
        case AOS_WLAN_CFG_GOT_SSID_PASSWD:
            //network_evt_callback(NETWORK_EVT_GOT_SSID_PASSWD);
            nctx.got_ssid_pwd = 1;
            break;
    }
}

static mp_obj_t network_init(size_t n_args, const mp_obj_t *args)
{
    nctx.type = mp_obj_get_int(args[0]);

    if (nctx.type == NETWORK_TYPE_WIFI) {
        char wlan_name[16];
        int len = sizeof(wlan_name);
        aos_wlan_init();
        aos_wlan_sta_start(wlan_name, &len);
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR(network_obj_init, 0, network_init);

static mp_obj_t network_close(void)
{
    memset(&nctx, 0, sizeof(nctx));

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_0(network_obj_close, network_close);

static mp_obj_t network_get_info(void)
{
    mp_obj_t dict;
    network_wifi_info_t wifi_info = {0};

    if (nctx.type == NETWORK_TYPE_WIFI) {
        int rssi;
        char buf[32];
        struct aos_wlan_info info;
        if (!aos_wlan_is_connected()) {
            printf("error: wifi not connected\n ");
            return MP_OBJ_NULL;
        }

        memset(&info, 0, sizeof(info));
        aos_wlan_get_info(&info);

        aos_wlan_get_rssi(&rssi);

        dict = mp_obj_new_dict(5);

        mp_obj_dict_store(MP_OBJ_FROM_PTR(dict), mp_obj_new_str("TYPE", strlen("TYPE")),
                mp_obj_new_int(NETWORK_TYPE_WIFI));

        mp_obj_dict_store(MP_OBJ_FROM_PTR(dict), mp_obj_new_str("SSID", strlen("SSID")),
                mp_obj_new_str(info.ssid.val, info.ssid.len));
        memset(buf, 0, sizeof(buf));
        snprintf(buf, sizeof(buf), "%02X:%02X:%02X:%02X:%02X:%02X",
                wifi_info.bssid[0], wifi_info.bssid[1], wifi_info.bssid[2],
                wifi_info.bssid[3], wifi_info.bssid[4], wifi_info.bssid[5]);
        mp_obj_dict_store(MP_OBJ_FROM_PTR(dict), mp_obj_new_str("BSSID", strlen("BSSID")),
                mp_obj_new_str(buf, strlen(buf)));
        mp_obj_dict_store(MP_OBJ_FROM_PTR(dict), mp_obj_new_str("RSSI", strlen("RSSI")),
                mp_obj_new_int(rssi));
    } else if (nctx.type == NETWORK_TYPE_CELLULAR) {
        dict = mp_obj_new_dict(2);
        mp_obj_dict_store(MP_OBJ_FROM_PTR(dict), mp_obj_new_str("TYPE", strlen("TYPE")),
                mp_obj_new_int(NETWORK_TYPE_CELLULAR));
    } else {
        dict = mp_obj_new_dict(2);
        mp_obj_dict_store(MP_OBJ_FROM_PTR(dict), mp_obj_new_str("TYPE", strlen("TYPE")),
                mp_obj_new_int(NETWORK_TYPE_ETHERNET));
    }

    mp_obj_dict_store(MP_OBJ_FROM_PTR(dict), mp_obj_new_str("IP", strlen("IP")),
            mp_obj_new_str(inet_ntoa(nctx.ip_addr), strlen(inet_ntoa(nctx.ip_addr))));

    return dict;
}
static MP_DEFINE_CONST_FUN_OBJ_0(network_obj_get_info, network_get_info);

/*
	0   :   NETWORK_WIFI,
	1   :   NETWORK_CELLULAR,
	2   :   NETWORK_ETHERNET,
	3   :   NETWORK_UNKNOW,
*/
static mp_obj_t network_obj_get_type_obj(size_t n_args, const mp_obj_t *args)
{
    return mp_obj_new_int(nctx.type);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR(network_obj_get_type, 0, network_obj_get_type_obj);

static mp_obj_t network_get_status(void)
{
    if (aos_wlan_is_ready()) {
        return mp_const_true;
    }
    return mp_const_false;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR(network_obj_get_status, 0, network_get_status);

static void wifi_connect_handle(void *args)
{
    wifi_ap_info_t* params = (wifi_ap_info_t *)args;

    aos_wlan_register_event_handler(AOS_WLAN_EVT_READY, wifi_event_cb, NULL);
    aos_wlan_register_event_handler(AOS_WLAN_EVT_STA_CONNECTED_FAIL, wifi_event_cb, NULL);
    aos_wlan_register_event_handler(AOS_WLAN_EVT_STA_NETWORK_CONNECTED, wifi_event_cb, NULL);
    aos_wlan_register_event_handler(AOS_WLAN_EVT_STA_CONNECTED, wifi_event_cb, NULL);

    aos_wlan_connect(params->ssid, params->passwd);

    free(params);
}

static int __network_connect(const char *ssid, const char *passwd)
{
    wifi_ap_info_t *params = (wifi_ap_info_t *)malloc(sizeof(wifi_ap_info_t));
    if (params == NULL) {
        LOG_E("%s:%d malloc failed\n", __func__, __LINE__);
        return mp_const_none;
    }
    memset(params, 0, sizeof(*params));
    strncpy(params->ssid, ssid, sizeof(params->ssid) - 1);
    strncpy(params->passwd, passwd, sizeof(params->passwd) - 1);

    aos_task_new("wifi_connect_task", wifi_connect_handle, params, 1024 << 2);
}

static mp_obj_t network_connect(mp_obj_t ssid, mp_obj_t pwd, mp_obj_t evt_callback)
{
    char* _ssid = mp_obj_str_get_str(ssid);
    char* _pwd = mp_obj_str_get_str(pwd);

    nctx.evt_cb = evt_callback;
    __network_connect((const char *)_ssid, (const char *)_pwd);

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_3(network_obj_connect, network_connect);

static mp_obj_t network_disconnect(void)
{
    aos_wlan_disconnect();

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_0(network_obj_disconnect, network_disconnect);

static void qrscan_control(int start)
{
    nctx.usbcam = start;
}

static int qrscan_process(isp_frame_t *frame)
{
    int ret = 0;
    int width, height;
    char ssid[NETWORK_SSID_MAX_LEN + 1], passwd[NETWORK_PASSWD_MAX_LEN + 1];

    if (nctx.got_ssid_pwd) return 0;

    py_video_camera_config_get(&width, &height, NULL);

	memset(nctx.result, 0, sizeof(nctx.result));
	ret = network_qrcode_scan(frame->vi_frame.data, width, height, nctx.result, sizeof(nctx.result));
	if (ret > 0 ) {
	    printf("=scan qr code==>ret %d: %s\n", ret, nctx.result);
        ret = network_qrscan_result_process(nctx.result, ssid, passwd);
        if (ret) {
            wifi_event_cb(AOS_WLAN_CFG_GOT_SSID_PASSWD, NULL, 0, NULL);
            __network_connect(ssid, passwd);
        }
	}
    return ret;
}

static void qrscan_task(void *args)
{
    isp_frame_t *frame = (isp_frame_t *)malloc(sizeof(*frame));

    if (frame == NULL) {
        LOG_E("%s alloc frame fail\r\n", __func__);
        return mp_const_none;
    }
    nctx.cam_fps = 2;

    memset(frame, 0, sizeof(*frame));
    py_video_camera_open(VIDEO_DEV0, nctx.cam_fps);

    py_usbcam_video_ext_process_config(qrscan_process, qrscan_control);
    py_usbcam_video_init(VIDEO_DEV0, VIDEO_CHN0);

    if (py_video_camera_working()) {
        nctx.usbcam = 1;
    }

    while (nctx.finish == 0) {
        if (nctx.usbcam) {
            aos_msleep(200);
            continue;
        }
        if (py_video_camera_frame_get(frame) == 0) {
            if (qrscan_process(frame) > 0) {
                nctx.got_ssid_pwd = 1;
            }
            py_video_camera_frame_release(frame);
        }
        aos_msleep(1000 / nctx.cam_fps);
    }
    py_usbcam_video_deinit();

    py_video_camera_close(VIDEO_DEV0);

    free(frame);
}

static mp_obj_t network_qrscan(mp_obj_t evt_callback)
{
    nctx.finish = 0;
    nctx.got_ssid_pwd = 0;
    nctx.evt_cb = evt_callback;

    aos_task_new("network_qrscan", qrscan_task, NULL, 1024 << 5);

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(network_obj_qrscan, network_qrscan);

static mp_obj_t network_smartconfig(mp_obj_t evt_callbac)
{
    LOG_E("%s not support\n", __func__);

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(network_obj_smartconfig, network_smartconfig);

static mp_obj_t network_softap(mp_obj_t ssid, mp_obj_t pwd, mp_obj_t evt_callbac)
{
    LOG_E("%s not support\n", __func__);

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_3(network_obj_softap, network_softap);

static mp_obj_t network_ble_netcfg(mp_obj_t evt_callbac)
{
    LOG_E("%s not support\n", __func__);

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(network_obj_ble_netcfg, network_ble_netcfg);

static const mp_rom_map_elem_t network_module_globals_table[] = {
    {MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_network)},
    // network type
    {MP_OBJ_NEW_QSTR(MP_QSTR_WIFI), MP_ROM_INT(NETWORK_TYPE_WIFI)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_ETHERNET), MP_ROM_INT(NETWORK_TYPE_ETHERNET)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_CELLULAR), MP_ROM_INT(NETWORK_TYPE_CELLULAR)},
    // network event type
    {MP_OBJ_NEW_QSTR(MP_QSTR_EVT_GOT_IP), MP_ROM_INT(NETWORK_EVT_GOT_IP)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_EVT_CONN_FAIL), MP_ROM_INT(NETWORK_EVT_CONN_FAIL)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_EVT_GOT_SSID_PASSWD), MP_ROM_INT(NETWORK_EVT_GOT_SSID_PASSWD)},
    // operation function list
    {MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&network_obj_init)},
    {MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&network_obj_close)},
    {MP_ROM_QSTR(MP_QSTR_getInfo), MP_ROM_PTR(&network_obj_get_info)},
    {MP_ROM_QSTR(MP_QSTR_getType), MP_ROM_PTR(&network_obj_get_type)},
    {MP_ROM_QSTR(MP_QSTR_getStatus), MP_ROM_PTR(&network_obj_get_status)},
    {MP_ROM_QSTR(MP_QSTR_connect), MP_ROM_PTR(&network_obj_connect)},
    {MP_ROM_QSTR(MP_QSTR_disconnect), MP_ROM_PTR(&network_obj_disconnect)},
    // network configuration
    {MP_ROM_QSTR(MP_QSTR_qrScan), MP_ROM_PTR(&network_obj_qrscan)},
    {MP_ROM_QSTR(MP_QSTR_smartConfig), MP_ROM_PTR(&network_obj_smartconfig)},
    {MP_ROM_QSTR(MP_QSTR_softAP), MP_ROM_PTR(&network_obj_softap)},
    {MP_ROM_QSTR(MP_QSTR_bleNetCfg), MP_ROM_PTR(&network_obj_ble_netcfg)},
};

static MP_DEFINE_CONST_DICT(network_module_globals, network_module_globals_table);

const mp_obj_module_t network_module = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t *)&network_module_globals,
};
