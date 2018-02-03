/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "alink_export_internal.h"
#include "service_manager.h"
#include "alink_protocol.h"
#include "device.h"
#include "aos/aos.h"
#include "git_version.h"
#include "connectivity_manager.h"
#include "accs.h"
#include "config.h"
#include "cota.h"
#include "os.h"
#ifdef GATEWAY_SDK
/* #include "kvmgr.h" */
#include "msdp.h"
#include "devmgr.h"
/* #include "stdd.h" */
/* #include "stdd_zigbee.h" */
/* #include "ifttt_main.h" */
#endif


#define MODULE_NAME "alink"

/**
 * @brief enable sandbox mode, for debug
 *
 * @param None.
 * @retval 0 on success, otherwise -1 will return
 * @see None.
 * @note None.
 */
int alink_enable_sandbox_mode(void)
{
    return config_set_alinkserver(default_sandbox_server_with_port);
}
AOS_EXPORT(int, alink_enable_sandbox_mode, void);

/**
 * @brief enable daily mode, for debug
 *
 * @param None.
 * @retval 0 on success, otherwise -1 will return
 * @see None.
 * @note None.
 */
int alink_enable_daily_mode(const char *server_ip, int port)
{
    const char *server = NULL;
    char server_buf[STR_SHORT_LEN];

    if (!server_ip || !port) {
        server = default_daily_server_with_port;
        LOGI(MODULE_NAME, "use default daily server & port: %s", server);
    } else {
        /* server string formt: wsf.smart.aliyun-inc.com:9999 */
        memset(server_buf, 0, sizeof(server_buf));
        OS_CHECK_PARAMS(strlen(server_ip) + sizeof(port) < STR_SHORT_LEN);

        strncpy(server_buf, server_ip, STR_SHORT_LEN - 1);
        snprintf(server_buf + strlen(server_buf), STR_SHORT_LEN - 1 - strlen(server_buf), ":%d", port);
        server = server_buf;
    }

    return config_set_alinkserver(server);
}
AOS_EXPORT(int, alink_enable_daily_mode, const char *, int);

/**
 * @brief entry function. start alink gateway service.
 *
 * @param None.
 * @retval 0 on success, otherwise -1 will return
 * @see None.
 * @note None.
 */
int alink_start(void)
{
    int ret = SERVICE_RESULT_OK;

    LOGI(MODULE_NAME, "Build Time: %s", ALINK_AGENT_BUILD_DATE);
    LOGI(MODULE_NAME, "Git Version: %s", ALINK_AGENT_GIT_VERSION);

    os_init();

    sm_init();
    cm_init();
    config_init();
    device_init();
    //system_monitor_init();
    cota_init();

    //sm_get_service("fota")->prepare();//fota_prepare
    //sm_get_service("alcs")->prepare();//alcs_prepare
    //sm_get_service("alcs")->start();//alcs_start
    sm_get_service("accs")->prepare();//accs_prepare
    sm_get_service("accs")->start();//accs_start

#ifdef GATEWAY_SDK
    /* init_kv(); */

    ret = devmgr_init();
    OS_ASSERT(!ret, "devmgr init fail");

    ret = msdp_init();
    OS_ASSERT(!ret, "msdp init fail");

    /* ifttt_init(); */

    /* ret = stdd_init(); */
    /* OS_ASSERT(!ret, "stdd init fail"); */

    /* application_init(); */
#endif

#ifdef ASR_SDK
    asr_init();
#endif

    return ret;
}
AOS_EXPORT(int, alink_start, void);

#define ALINK_WAIT_FOREVER      (0)
/**
 * @brief waiting alink connect to aliyun server
 *
 * @param[in] timeout_ms: time in milliseconds,
 *              use ALINK_WAIT_FOREVER to wait until aliyun server is connected
 * @retval 0 when connect to server successfully, otherwise -1 will return
 * @see None.
 * @note FIXME: offline mode(disconnect with cloud) support?
 */
int alink_wait_connect(int timeout_ms)
{
    int ready = 0;
    unsigned int expires = os_get_time_ms() + timeout_ms;

    while (1) {
        ready = cloud_is_connected();
        if (ready) {
            break;    /* ready to go */
        }

        if (timeout_ms == ALINK_WAIT_FOREVER ||
            os_get_time_ms() < expires) {
            os_sleep(1);
        } else {
            break;    /* timeout */
        }
    };

    return ready ? 0 : -1;
}

/**
 * @brief destroy alink service and os_free resources
 *
 * @param None.
 * @retval 0 on success, otherwise -1 will return
 * @see None.
 * @note this func will block at most 15 seconds to
 *      stop all alink related process(thread)
 */
int alink_end(void)
{

#ifdef GATEWAY_SDK
    /* application_exit(); */

    /* stdd_exit(); */
    devmgr_exit();
    /* ifttt_destroy(); */
    msdp_exit();
    /* deinit_kv(); */
#endif

#ifdef ASR_SDK
    asr_destroy();
#endif

    //sm_get_service("fota")->stop();
    //sm_get_service("alcs")->stop();
    sm_get_service("accs")->stop();//accs_stop

    cota_exit();
    device_exit();
    config_exit();
    //system_monitor_exit();
    //work_queue_exit();

    /* deinit global variable */
    memset(alink_cb_func, 0, sizeof(alink_cb_func));
    os_exit();

    return 0;
}
AOS_EXPORT(int, alink_end, void);

/**
 * @brief reset user account binding.
 *
 * @retval 0 on success, -1 when params invalid
 * @see None.
 * @note None.
 */
int alink_factory_reset(void)
{
    int unregister_flag = 1, ret;

#ifdef GATEWAY_SDK
    devmgr_reset_all_device();
#endif
    if (cloud_is_connected()) {
        ret = alink_unregister();
        if (ret == ALINK_CODE_SUCCESS) {
            unregister_flag = 0;
        }
    }

    config_reset(unregister_flag);

    os_sys_reboot();
    aos_cli_printf("waiting sys reboot ...\r\n");

    //while (1);

    return 0;
}
AOS_EXPORT(int, alink_factory_reset, void);

#ifdef GATEWAY_SDK
/***
 * @brief register device's attribute
 * @param[in] name: the name of the attribute
 * @param[in] get_cb: callback function to get the value of the attribute
 * @param[in] set_cb: callback function to set the value of the attribute
 * @retval 0 on success, otherwise -1 will return
 */
int alink_register_attribute(const char *name,
                             ALINK_ATTR_GET_CB get_cb,
                             ALINK_ATTR_SET_CB set_cb)
{
    return msdp_register_attr_cb(DEV_TYPE_GATEWAY, name, get_cb, set_cb);
}

/***
 * @brief register device's service
 * @param[in] name: the name of the service
 * @param[in] exec_cb: callback function to execute the service
 * @retval 0 on success, otherwise -1 will return
 * @Note service list defined by alink.
 */
int alink_register_service(const char *name, ALINK_SERVICE_EXECUTE_CB exec_cb)
{
    return msdp_register_service_cb(DEV_TYPE_GATEWAY, name, exec_cb);
}

/***
 * @brief unregister device's service
 * @param[in] name: the name of the service
 * @retval 0 on success, otherwise -1 will return
 */
int alink_unregister_service(const char *name)
{
    msdp_unregister_service_cb(DEV_TYPE_GATEWAY, name);
    return SERVICE_RESULT_OK;
}

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
int alink_report_attrs(const char *attr_name[])
{
    return msdp_report_device_attrs(config_get_main_uuid(), attr_name);
}

/**
 * @brief register and authentication zigbee device to aliyun server
 *
 * @param[in] ieee_addr: 8 bytes zigbee device long address
 * @param[in] model_id: 2 bytes device model identification
 * @param[in] rand: 16 bytes random binary array
 * @param[in] sign: 16 bytes characters of signature, md5(rand|secret)
 * @retval 0 on success, -1 when params invalid
 * @see None.
 * @note when joined zigbee network, invoke this function to register sub device
 */
int alink_zigbee_register_device(unsigned char ieee_addr[IEEE_ADDR_BYTES],
                                 unsigned int model_id, const char rand[SUBDEV_RAND_BYTES], const char *sign)
{
    return devmgr_join_zigbee_device(ieee_addr, model_id, rand, sign);
}

/**
 * @brief unregister zigbee device to aliyun server
 *
 * @param[in] ieee_addr: 8 bytes zigbee device long address
 * @retval 0 on success, -1 when params invalid
 * @see None.
 * @note when factory reset, invoke this function to unregister sub device
 */
int alink_zigbee_unregister_device(unsigned char ieee_addr[IEEE_ADDR_BYTES])
{
    return devmgr_leave_zigbee_device(ieee_addr);
}

/**
 * @brief update zigbee device online status, online or offline
 *
 * @param[in] ieee_addr: 8 bytes zigbee device long address
 * @param[in] online_or_not: 1 online; 0 offline
 *
 * @retval 0 on success, -1 when params invalid
 * @see None.
 * @note alink sdk will keep syncing subdev status with aliyun server
 */
int alink_zigbee_update_online_status(unsigned char ieee_addr[IEEE_ADDR_BYTES],
                                      char online_or_not)
{
    return devmgr_update_zigbee_device_online_status(ieee_addr, online_or_not);
}

/**
 * @brief report zigbee device attribute list to server, at least 1 attribute should be include
 *
 * @param[in] ieee_addr: zigbee device's long address
 * @param[in] endpoint_id: zigbee device's endpoint
 * @param[in] attr_name[]: NULL-terminated array which hold the attr name list
 * @param[in] attr_value[]: NULL-terminated array which hold the attr value list,
 *            attr_name[x] & attr_value[x] is a key-value pair.
 * @retval 0 when successfully got response from cloud,
 *          otherwise this func will block until timeout and -1 will return
 * @see None.
 * @note when connection with server is unstable, this func will block
 *      until got response from server or timeout.
 */
int alink_zigbee_report_attrs(unsigned char ieee_addr[IEEE_ADDR_BYTES],
                              unsigned char endpoint_id,
                              const char *attr_name[], const char *attr_value[])
{
    /* return stdd_zbnet_report_attrs(ieee_addr, endpoint_id, attr_name, attr_value); */
    return 0;
}

/**
 * @brief report zigbee device command of device to coo
 *
 * @param[in] ieee_addr: zigbee device's long address
 * @param[in] endpoint_id: zigbee device's endpoint
 * @param[in] event_name: event name
 * @param[in] event_args: event params, json format
 * @retval 0 when successfully got response from cloud,
 *          otherwise this func will block until timeout and -1 will return
 * @see None.
 * @note when connection with server is unstable, this func will block
 *      until got response from server or timeout.
 */
int alink_zigbee_report_event(unsigned char ieee_addr[IEEE_ADDR_BYTES],
                              unsigned char endpoint_id,
                              const char *event_name, const char *event_args)
{
    /* return stdd_zbnet_report_event(ieee_addr, endpoint_id, event_name, event_args); */
    return 0;
}

#endif

/**
 * @brief report alink message, it is a fundamental func.
 *
 * @param[in] method: alink protocol method,
 *          i.e. "postDeviceRawData", "retrieveDeviceData".
 * @param[in] json_buffer: json format buffer, like {"OnOff":"1", "Light":"80"}.
 * @retval 0 when successfully got response from cloud,
 *          otherwise this func will block until timeout and -1 will return
 * @see None.
 * @note when connection with server is unstable, this func will block
 *      until got response from server or timeout.
 */
int alink_report(const char *method, const char *json_buffer)
{
    alink_data_t data = {(char *)method, (char *)json_buffer};
    service_t *service = sm_get_service("accs");
    int ret;

    if (!method || !json_buffer || !service) {
        return -1;
    }

    ret = service->put((void *)&data, sizeof(data)); //accs_put

    return ret;
}

int alink_report_async(const char *method, const char *json_buffer,
                       void *(*cb)(void *), void *arg)
{
    alink_data_t data = {(char *)method, (char *)json_buffer};
    service_t *service = sm_get_service("accs");
    int ret;

    if (!method || !json_buffer || !service) {
        return -1;
    }
    ret = service->put_async((void *)&data, sizeof(data), cb, arg); //accs_put

    return ret;
}
AOS_EXPORT(int, alink_report_async, const char *, const char *, void *(*)(void *), void *);

/**
 * @brief query alink cloud service, like getAlinkTime...
 *
 * @param[in] method: alink protocol method,
 *          i.e. "getAlinkTime", "retrieveDeviceData".
 * @param[in] json_buffer: json format buffer, like {} for "getAlinkTime",
 *              or {"dataKey":"param_probe_num"} for "retrieveDeviceData"
 * @param[in] result_buf: to hold json string return from cloud
 * @param[in/out] buf_len: [in] size of result_buf, [out] strlen of json string
 *              return from cloud
 * @retval 0 when successfully got response from cloud,
 *          otherwise this func will block until timeout and -1 will return
 * @see None.
 * @note when connection with server is unstable, this func will block
 *      until got response from server or timeout.
 */
int alink_query(const char *method, const char *json_buffer,
                char *result_buf, int *buf_len)
{
    alink_data_t data = {(char *)method, (char *)json_buffer};
    service_t *service = sm_get_service("accs");
    int ret;

    if (!method || !json_buffer || !result_buf || !buf_len || !*buf_len) {
        return -1;
    }

    ret = service->get((void *)&data, sizeof(data), result_buf,
                       *buf_len); //accs_get
    if (!ret) {
        *buf_len = strlen(result_buf);
    }

    return ret;
}

void *alink_cb_func[ALINK_CB_MAX_NUM];
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
int alink_register_callback(unsigned char cb_type, void *cb_func)
{
    OS_CHECK_PARAMS(cb_type < ALINK_CB_MAX_NUM && cb_func);

    if (alink_cb_func[cb_type] != NULL)
        LOGE(MODULE_NAME, "same type cb %d was registered!", cb_type);

    alink_cb_func[cb_type] = cb_func;

    return 0;
}
AOS_EXPORT(int, alink_register_callback, unsigned char, void *);

#ifdef ASR_SDK
int alink_asr_send_buf(const void *buf, int len, asr_msg_type_t type)
{
    return asr_send_buf(buf, len, type);
}
#endif





