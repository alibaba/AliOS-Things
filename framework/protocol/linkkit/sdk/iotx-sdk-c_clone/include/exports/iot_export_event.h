/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef IOTX_EXPORT_EVENT_H
#define IOTX_EXPORT_EVENT_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief event list used for iotx_regist_event_monitor_cb
 */
enum iotx_event_t {
    IOTX_AWSS_START = 0x1000,       // AWSS start without enbale, just supports device discover
    IOTX_AWSS_ENABLE,               // AWSS enable
    IOTX_AWSS_LOCK_CHAN,            // AWSS lock channel(Got AWSS sync packet)
    IOTX_AWSS_CS_ERR,               // AWSS AWSS checksum is error
    IOTX_AWSS_PASSWD_ERR,           // AWSS decrypt passwd error
    IOTX_AWSS_GOT_SSID_PASSWD,      // AWSS parse ssid and passwd successfully
    IOTX_AWSS_CONNECT_ADHA,         // AWSS try to connnect adha (device discover, router solution)
    IOTX_AWSS_CONNECT_ADHA_FAIL,    // AWSS fails to connect adha
    IOTX_AWSS_CONNECT_AHA,          // AWSS try to connect aha (AP solution)
    IOTX_AWSS_CONNECT_AHA_FAIL,     // AWSS fails to connect aha
    IOTX_AWSS_SETUP_NOTIFY,         // AWSS sends out device setup information (AP and router solution)
    IOTX_AWSS_CONNECT_ROUTER,       // AWSS try to connect destination router
    IOTX_AWSS_CONNECT_ROUTER_FAIL,  // AWSS fails to connect destination router.
    IOTX_AWSS_GOT_IP,               // AWSS connects destination successfully and got ip address
    IOTX_AWSS_SUC_NOTIFY,           // AWSS sends out success notify (AWSS sucess)
    IOTX_AWSS_BIND_NOTIFY,          // AWSS sends out bind notify information to support bind between user and device
    IOTX_AWSS_ENABLE_TIMEOUT,       // AWSS enable timeout(user needs to call awss_config_press again to enable awss)
    IOTX_CONN_CLOUD = 0x2000,       // Device try to connect cloud
    IOTX_CONN_CLOUD_FAIL,           // Device fails to connect cloud, refer to net_sockets.h for error code
    IOTX_CONN_CLOUD_SUC,            // Device connects cloud successfully
    IOTX_RESET = 0x3000,            // Linkkit reset success (just got reset response from cloud without any other operation)
};

/**
 * @brief register callback to monitor all event from system.
 *
 * @param callback, when some event occurs, the system will trigger callback to user.
 *                  refer to enum iotx_event_t for event list supported.
 *
 * @return 0 when success, -1 when fail.
 * @note: user should make sure that callback is not block and runs to complete fast.
 */
extern int iotx_event_regist_cb(void (*monitor_cb)(int event));

/**
 * @brief post event to trigger callback resitered by iotx_event_regist_cb
 *
 * @param event, event id, refer to iotx_event_t
 *
 * @return 0 when success, -1 when fail.
 */
extern int iotx_event_post(int event);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* IOTX_EXPORT_H */
