/*------------------------------------------------------------------------------
* qxwz_sdk.h : sdk interface
*          Copyright (C) 2015-2017 by QXSI, All rights reserved.
*/
#ifndef __QXWZ_SDK_H__
#define __QXWZ_SDK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <time.h>
#include "qxwz_types.h"
#include "qxwz_status.h"


/**
 * Data
 */
typedef struct qxwz_usr_config{
    qxwz_s8_t *appkey;
    qxwz_s8_t *appsecret;
    qxwz_s8_t *device_ID;
    qxwz_s8_t *device_Type;
} qxwz_usr_config_t;

typedef struct {
        char *appkey;
        char *deviceID;
        char *deviceType;
        time_t expire_time;
        char *NtripUserName;
} qxwz_account_info;

typedef qxwz_void_t (*qxwz_sdk_data_response)(qxwz_void_t *data, qxwz_u32_t length, qxwz_data_type_e type);
typedef qxwz_void_t (*qxwz_sdk_status_response)(qxwz_s32_t status);

typedef struct qxwz_data_response{
    qxwz_sdk_data_response  cb_ipdata;
    qxwz_sdk_data_response  cb_stdata;
}qxwz_data_response_t;

typedef struct qxwz_status_response{
    qxwz_sdk_status_response  cb_status;
}qxwz_status_response_t;

/**
 * @param[in]  config: usr config.
 *
 * @return:
 *   0 if success
 *  -1 if fail
 */
qxwz_s32_t qxwz_setting(const qxwz_usr_config_t* config, qxwz_bool_t isRealtime);

/**
 * @param[in]  data_rsp: callback for returning rtcm data.
 * @param[in]  status_rsp: callback for returnning sdk stataus.
 *
 * @return:
 *   0 if success
 *  -1 if fail
 */
qxwz_s32_t qxwz_start(qxwz_data_response_t * data_rsp, qxwz_status_response_t * status_rsp);

/**
 * @brief sdk tick, trigger sdk to check status
 * @param[in]  time: current time(s)
 *
 * @return:
 *   >0 if sdk provide the service
 *    0 if sdk stop the service
 */
qxwz_s32_t qxwz_tick(qxwz_u32_t system_time);

/**
 * @param[in]  data: data to send
 * @param[in]  size: the size of data in bytes
 * @param[in]  type: the data type sent by user
 *
 * @return:
 *   0 if success
 *  -1 if fail
 */
qxwz_s32_t qxwz_send_data(const void *data, qxwz_u32_t size, qxwz_udata_type_e type);

/**
 * @brief qxwz_release release sdk resources
 *
 * @return:
 *   NULL
 */
qxwz_void_t qxwz_release(void);


/**
 * @brief qxwz_stop stop sdk state machine run
 *
 * @return:
 *   NULL
 */
qxwz_void_t qxwz_stop(void) ;


/**
*
* @brief get user account info
*
* @return  qxwz_account_info*
*
*/
const qxwz_account_info* getqxwzAccount(void);

/**
 * @brief qxwz_sdk_version get SDK version
 *
 * @return:
 *   a pointer to version
 */
qxwz_u8_t* qxwz_sdk_version(void);


#ifdef __cplusplus
}
#endif

#endif
