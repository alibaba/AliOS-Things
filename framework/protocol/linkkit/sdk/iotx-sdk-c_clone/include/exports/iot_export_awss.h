/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __IOT_EXPORT_AWSS_H__
#define __IOT_EXPORT_AWSS_H__

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

/**
 * @brief   start wifi setup service
 *
 * @retval  -1 : wifi setup fail
 * @retval  0 : sucess
 * @note: awss_config_press must been called to enable wifi setup service
 */
DLL_IOT_API int awss_start();

/**
 * @brief   stop wifi setup service
 *
 * @retval  -1 : failure
 * @retval  0 : sucess
 * @note
 *      if awss_stop is called before exit of awss_start, awss and notify will stop.
 *      it may cause failutre of awss and device bind.
 */
DLL_IOT_API int awss_stop();

/**
 * @brief   make sure user touches device belong to themselves
 *
 * @retval  -1 : failure
 * @retval  0 : sucess
 * @note: AWSS dosen't parse awss packet until user touch device using this api.
 */
DLL_IOT_API int awss_config_press();

/**
 * @brief   start wifi setup service with device ap
 *
 * @retval  -1 : failure
 * @retval  0 : sucess
 * @note
 *      1. if awss_stop or awss_dev_ap_stop is called before exit of awss_dev_ap_start
 *         awss with device ap and notify will stop, it may cause failutre of device ap
 *         and device bind.
 *      2. awss_dev_ap_start doesn't need to call awss_config_press to been enabled.
 */
DLL_IOT_API int awss_dev_ap_start();

/**
 * @brief   stop wifi setup service with device ap
 *
 * @retval  -1 : failure
 * @retval  0 : sucess
 * @note
 *      if awss_dev_ap_stop is called before exit of awss_dev_ap_start
 *      awss with device ap and notify will stop, it may cause failutre of device ap
 */
DLL_IOT_API int awss_dev_ap_stop();

/**
 * @brief   report token to cloud after wifi setup success
 *
 * @retval  -1 : failure
 * @retval  0 : sucess
 */
DLL_IOT_API int awss_report_cloud();

/**
 * @brief   report reset to cloud.
 *
 * @retval  -1 : failure
 * @retval  0 : sucess
 * @note
 *      device will save reset flag if device dosen't connect cloud, device will fails to send reset to cloud.
 *      when connection between device and cloud is ready, device will retry to report reset to cloud.
 */
DLL_IOT_API int awss_report_reset();

enum awss_event_t {
    AWSS_START = 0x1000,       // AWSS start without enbale, just supports device discover
    AWSS_ENABLE,               // AWSS enable
    AWSS_LOCK_CHAN,            // AWSS lock channel(Got AWSS sync packet)
    AWSS_CS_ERR,               // AWSS AWSS checksum is error
    AWSS_PASSWD_ERR,           // AWSS decrypt passwd error
    AWSS_GOT_SSID_PASSWD,      // AWSS parse ssid and passwd successfully
    AWSS_CONNECT_ADHA,         // AWSS try to connnect adha (device discover, router solution)
    AWSS_CONNECT_ADHA_FAIL,    // AWSS fails to connect adha
    AWSS_CONNECT_AHA,          // AWSS try to connect aha (AP solution)
    AWSS_CONNECT_AHA_FAIL,     // AWSS fails to connect aha
    AWSS_SETUP_NOTIFY,         // AWSS sends out device setup information (AP and router solution)
    AWSS_CONNECT_ROUTER,       // AWSS try to connect destination router
    AWSS_CONNECT_ROUTER_FAIL,  // AWSS fails to connect destination router.
    AWSS_GOT_IP,               // AWSS connects destination successfully and got ip address
    AWSS_SUC_NOTIFY,           // AWSS sends out success notify (AWSS sucess)
    AWSS_BIND_NOTIFY,          // AWSS sends out bind notify information to support bind between user and device
    AWSS_ENABLE_TIMEOUT,       // AWSS enable timeout(user needs to call awss_config_press again to enable awss)
    AWSS_RESET = 0x3000,       // Linkkit reset success (just got reset response from cloud without any other operation)
};

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif

#endif
