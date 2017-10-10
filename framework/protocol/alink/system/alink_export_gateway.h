/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _ALINK_EXPORT_GATEWAY_H_
#define _ALINK_EXPORT_GATEWAY_H_

#if defined(__cplusplus) /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

/** @defgroup gateway_device gateway API
 *  @{
 */

/** @defgroup gateway_debug debug
 *  @{
 */

/**
 * log level def.
 */
enum ALINK_LOG_LEVEL {
    ALINK_LL_NONE, /**< disable log */
    ALINK_LL_FATAL, /**< fatal error log will output */
    ALINK_LL_ERROR, /**< error + fatal log will output */
    ALINK_LL_WARN, /**< warn + error + fatal log will output(default level) */
    ALINK_LL_INFO, /**< info + warn + error + fatal log will output */
    ALINK_LL_DEBUG, /**< debug + info + warn + error + fatal log will output */
    ALINK_LL_TRACE, /**< trace + debug + info + warn + error + fatal will output */
};

/**
 * @brief log level control
 *
 * @param[in] loglevel
 * @return None.
 * @see enum ALINK_LOG_LEVEL.
 * @note None.
 */
void alink_set_loglevel(enum ALINK_LOG_LEVEL loglevel);

/**
 * @brief enable sandbox mode, for debug
 *
 * @param None.
 * @retval 0 on success, otherwise -1 will return
 * @see None.
 * @note None.
 */
int alink_enable_sandbox_mode(void);

/**
 * @brief enable daily mode, for debug
 *
 * @param None.
 * @retval 0 on success, otherwise -1 will return
 * @see None.
 * @note None.
 */
int alink_enable_daily_mode(const char *server_ip, int port);

/** @} */ //end of debug



/** @defgroup gateway_entry main
 *  @{
 */

/**
 * @brief entry function. start alink gateway service.
 *
 * @param None.
 * @retval 0 on success, otherwise -1 will return
 * @see None.
 * @note None.
 */
int alink_start(void);

#define ALINK_WAIT_FOREVER      (0)
/**
 * @brief waiting alink connect to aliyun server
 *
 * @param[in] timeout_ms: time in milliseconds,
 *              use ALINK_WAIT_FOREVER to wait until aliyun server is connected
 * @retval 0 when connect to server successfully, otherwise -1 will return
 * @see None.
 * @note None.
 */
int alink_wait_connect(int timeout_ms);

/**
 * @brief destroy alink service and free resources
 *
 * @param None.
 * @retval 0 on success, otherwise -1 will return
 * @see None.
 * @note this func will block at most 15 seconds to
 *      stop all alink related process(thread)
 */
int alink_end(void);

/** @} */ //end of entry

/** @defgroup gateway_feature register attr & service
 *  @{
 */

/**
 * @brief device attribute get callback function type definition
 * @param[in] buf: buffer to get attribute value
 * @param[in] buf_sz: buffer to get attribute value
 * @retval 0 on success, otherwise -1 will return
 */
typedef int (*ALINK_ATTR_GET_CB)(char *buf, unsigned int buf_sz);

/***
 * @brief device attribute set callback function type definition
 * @param[in] json_in: attribute value to set, end by '\0'
 * @retval 0 on success, otherwise -1 will return
 */
typedef int (*ALINK_ATTR_SET_CB)(char *json_in);

/***
 * @brief register device's attribute
 * @param[in] name: the name of the attribute
 * @param[in] get_cb: callback function to get the value of the attribute
 * @param[in] set_cb: callback function to set the value of the attribute
 * @retval 0 on success, otherwise -1 will return
 */
int alink_register_attribute(const char *name,
                             ALINK_ATTR_GET_CB get_cb,
                             ALINK_ATTR_SET_CB set_cb);
/***
 * @brief device service execute callback function type definition
 * @param[in] json_in: input parameters, end by '\0'
 * @param[in] json_out_buf: buffer to get output parameters
 * @param[in] buf_sz: buffer size
 * @retval 0 on success, otherwise -1 will return
 */
typedef int (*ALINK_SERVICE_EXECUTE_CB)(char *json_in,
                                        char *json_out_buf, unsigned int buf_sz);

/***
 * @brief register device's service
 * @param[in] name: the name of the service
 * @param[in] exec_cb: callback function to execute the service
 * @retval 0 on success, otherwise -1 will return
 * @Note service list defined by alink.
 */
int alink_register_service(const char *name, ALINK_SERVICE_EXECUTE_CB exec_cb);

/***
 * @brief unregister device's service
 * @param[in] name: the name of the service
 * @retval 0 on success, otherwise -1 will return
 */
int alink_unregister_service(const char *name);

/**
 * @brief reset user account binding.
 *
 * @retval 0 on success, -1 when params invalid
 * @see None.
 * @note None.
 */
int alink_factory_reset(void);

/** @} */ //end of gateway feature



/** @defgroup gateway_status report status
 *  @{
 */

/**
 * @brief report gateway's attr list to server, at least 1 attr will be include
 *
 * @param[in] attr_name[]: NULL-terminated array which hold the attr name list
 * @retval 0 when successfully got response from cloud,
 *          otherwise this func will block until timeout and -1 will return
 * @see None.
 * @note when connection with server is unstable, this func will block
 *      until got response from server or timeout.
 */
int alink_report_attrs(const char *attr_name[]);

/** @} */ //end of status

/** @defgroup gateway_callback    callback
 *  @{
 */

/**
 * alink callback event
 */
enum ALINK_GATEWAY_CALLBACK {
    /**
     * void callback_cloud_connected(void)
     * @n@n called each time when gateway successfully connect(or reconnect)
     * to aliyun server
     */
    ALINK_CLOUD_CONNECTED = 0,

    /**
     * void callback_cloud_disconnected(void)
     * @n@n called each time when gateway lose connection with aliyun server
     */
    ALINK_CLOUD_DISCONNECTED,
};

/**
 * @brief register misc callback
 *
 * @param[in] cb_type: callback type.
 * @param[in] cb_func: callback func pointer, func declaration see related comments.
 *
 * @retval  0 on success, otherwise -1 will return
 * @see enum ALINK_GATEWAY_CALLBACK, ALINK_ZIGBEE_CALLBACK.
 * @note None.
 */
int alink_register_callback(unsigned char cb_type, void *cb_func);
/** @} */ //end of callback

/** @defgroup gateway_awss awss
 *  @{
 */

/**
 * @brief start awss service, block method,
 *      block until awss succeed, or timeout(default 1min).
 *
 * @retval  0 on success, otherwise -1 will return
 * @see None.
 * @note None.
 */
int awss_start(void);

/**
 * @brief exit awss
 *      calling this func force awss_start exit.
 *
 * @param None.
 * @retval  0 on success, otherwise -1 will return
 * @see None.
 * @note None.
 */
int awss_end(void);

/** @} */ //end of awss

/** @} */ //end of gateway api

#if defined(__cplusplus) /* If this is a C++ compiler, use C linkage */
}
#endif

#endif
