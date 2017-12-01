/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef AOS_ALINK_H
#define AOS_ALINK_H

#ifdef __cplusplus
extern "C"
{
#endif

enum ALINK_LOG_LEVEL {
    ALINK_LL_NONE,  /* disable log */
    ALINK_LL_FATAL, /* fatal error log will output */
    ALINK_LL_ERROR, /* error + fatal log will output */
    ALINK_LL_WARN,  /* warn + error + fatal log will output(default level) */
    ALINK_LL_INFO,  /* info + warn + error + fatal log will output */
    ALINK_LL_DEBUG, /* debug + info + warn + error + fatal log will output */
    ALINK_LL_TRACE, /* trace + debug + info + warn + error + fatal will output */
};

/**
 * Log level control
 *
 * @param[in]  loglevel  ALINK_LOG_LEVEL
 */
void alink_set_loglevel(enum ALINK_LOG_LEVEL loglevel);

/**
 * Enable sandbox mode, for debug
 *
 * @return  0 on success, otherwise -1 will return
 */
int alink_enable_sandbox_mode(void);

/**
 * Enable daily mode, for debug
 *
 * @param[in]  server_ip  IP Addr
 * @param[in]  port       server port
 *
 * @return  0 on success, otherwise -1 will return
 */
int alink_enable_daily_mode(const char *server_ip, int port);

/**
 * Entry function. start alink gateway service.
 *
 * @return  0 on success, otherwise -1 will return
 */
int alink_start(void);

#define ALINK_WAIT_FOREVER (0)

/**
 * Waiting alink connect to aliyun server
 *
 * @param[in]  timeout_ms  time in milliseconds, use ALINK_WAIT_FOREVER to wait until server is connected
 *
 * @return  0 when connect to server successfully, otherwise -1 will return
 */
int alink_wait_connect(int timeout_ms);

/**
 * Destroy alink service and free resources
 *
 * @note  this func will block at most 15 seconds to stop all alink related process(thread)
 *
 * @return  0 on success, otherwise -1 will return
 */
int alink_end(void);

/**
 * Reset user account binding.
 *
 * @return  0 on success, -1 when params invalid
 */
int alink_factory_reset(void);

/**
 * Report alink message, it is a fundamental func.
 *
 * @note when connection with server is unstable, this func will block until got response from server or timeout.
 *
 * @param[in]  method       alink protocol method, i.e. "postDeviceRawData", "retrieveDeviceData".
 * @param[in]  json_buffer  json format buffer, like {"OnOff":"1", "Light":"80"}.
 *
 * @return  0 when successfully got response from cloud,
 *          otherwise this func will block until timeout and -1 will return
 */
int alink_report(const char *method, const char *json_buffer);
int alink_report_async(const char *method, const char *json_buffer,
                       void *(*cb)(void *), void *arg);

int alink_report_rawdata(const char *rawdata, int len);

enum ALINK_WIFI_CALLBACK {
    /*
     * void callback_cloud_connected(void)
     * @n@n called each time when gateway successfully connect(or reconnect)
     * to aliyun server
     */
    ALINK_CLOUD_CONNECTED = 0,

    /*
     * void callback_cloud_disconnected(void)
     * @n@n called each time when gateway lose connection with aliyun server
     */
    ALINK_CLOUD_DISCONNECTED,
    /*
     * int callback_read_device_status(const char *params)
     * @n@nuccessfully
     */
    ALINK_GET_DEVICE_STATUS,

    /*
     * void callback_write_device_status(const char *params)
     */
    ALINK_SET_DEVICE_STATUS,
    /*
     * int callback_read_device_rawdata(const char *params)
     * @n@nuccessfully
     */
    ALINK_GET_DEVICE_RAWDATA,

    /*
     * void callback_write_device_rawdata(const char *params)
     */
    ALINK_SET_DEVICE_RAWDATA,
};

int awss_register_callback(unsigned char cb_type, void *cb_func);

/**
 * Register misc callback
 *
 * @param[in]  cb_type  callback type.
 * @param[in]  cb_func  callback func pointer, func declaration see related comments.
 *
 * @return  0 on success, otherwise -1 will return
 */
int alink_register_callback(unsigned char cb_type, void *cb_func);

/**
 * Start awss service, block method, block until awss succeed, or timeout(see Note).
 *
 * @note  platform_awss_get_timeout_interval_ms() return monitor timeout interval,
 *        AP connection timeout is 30s.
 *
 * @return  0 on success, otherwise non-zero value will return
 *                =  0: connect AP & DHCP success
 *                = -1: get ssid & passwd fail
 *                = -2: connect AP / DHCP fail
 */
int awss_start(void);

/**
 * Calling this func force awss_start exit.
 *
 * @return  0 on success, otherwise -1 will return
 */
int awss_stop(void);

#ifdef __cplusplus
}
#endif

#endif /* AOS_ALINK_H */

