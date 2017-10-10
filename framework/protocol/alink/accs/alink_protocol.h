/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef ALINK_PROTOCOL_H
#define ALINK_PROTOCOL_H

#include "../system/device.h"
#include "os.h"

#define PLATFORM_MAX_SSID_LEN           (32 + 1)
enum {
    PHASE_INIT,
    PHASE_REGISTER,
    PHASE_LOGIN,
    PHASE_LOGIN_SUCCESS,
    PHASE_UNREGISTER,
    PHASE_WAIT,
    PHASE_READY,
    PHASE_ABORT,
};

/* more error code detail see service/accs/error_code.txt */
enum ALINK_CODE {
    //GENERAL
    ALINK_CODE_ERROR_MALFORMED_PACK = -2,
    ALINK_CODE_ERROR = -1,
    ALINK_CODE_ERROR_TIMEOUT = -2,
    //SUCCESS
    ALINK_CODE_SUCCESS = 1000,
    ALINK_CODE_SUCCESS_REBOUND = 3086,
    //RETRY LATER
    ALINK_CODE_ERROR_SYSTEM = 2001,
    ALINK_CODE_ERROR_SERVICE_TIMEOUT = 2018,
    ALINK_CODE_ERROR_SERVICE_UNAVAILABLE = 2019,
    ALINK_CODE_ERROR_TOKEN_SERVICE = 3046,
    //RE-LOGIN
    ALINK_CODE_ERROR_DEV_NOT_LOGIN = 3001,
    //RE-HANDSHAKE
    ALINK_CODE_ERROR_TOKEN = 3047,
    ALINK_CODE_ERROR_TOKEN_STATE = 3083,
    ALINK_CODE_ERROR_UUID = 3048,
    ALINK_CODE_ERROR_UUID_NO_INFO = 3049,
    ALINK_CODE_ERROR_INVALID_UUID = 3037,
    ALINK_CODE_ERROR_UUID_TOKEN_MISMATCH = 3060,
    //FATAL, system hang
    ALINK_CODE_ERROR_MISSING_PARAM = 3009,
    ALINK_CODE_ERROR_INVLIAD_METHOD = 3010,
    ALINK_CODE_ERROR_INVLIAD_CID = 3011,
    ALINK_CODE_ERROR_CID_MISMATCH = 3012,
    ALINK_CODE_ERROR_INVALID_PARAM_NODE = 3015,
    ALINK_CODE_ERROR_NULL_MODEL = 3020,
    ALINK_CODE_ERROR_NULL_MAC = 3021,
    ALINK_CODE_ERROR_NULL_REQDATA = 3024,
    ALINK_CODE_ERROR_NULL_METHOD = 3025,
    ALINK_CODE_ERROR_NULL_PARAM = 3026,
    ALINK_CODE_ERROR_PARAM_ARGS = 3027,
    ALINK_CODE_ERROR_NOT_MATCH_UUIDGEN = 3040,
    ALINK_CODE_ERROR_SN_SHORT = 3041,
    ALINK_CODE_ERROR_MAC = 3042,
    ALINK_CODE_ERROR_SN_LONG = 3043,
    ALINK_CODE_FAIL_UUID_GEN = 3044,
    ALINK_CODE_ERROR_SN = 3045,
    ALINK_CODE_ERROR_NOT_DEVICE11 = 3061,
    ALINK_CODE_ERROR_MODEL = 3081,
    ALINK_CODE_ERROR_DEVICE_IN_BLACKLIST = 3501,
    //SUBDEV
    ALINK_CODE_ERROR_SUBDEV_INVALID_UUID = 3038,
    ALINK_CODE_ERROR_SUBDEV_NO_RELATIONSHIP = 3065,
    ALINK_CODE_ERROR_SUBDEV_NOT_LOGIN = 3066,
    //IGNORE, see more error code in error_code.txt, most of them are ignored
    ALINK_CODE_ERROR_SIGN_EXCEPTION = 2004,
    ALINK_CODE_ERROR_DEVICE_NON_EXIST = 3019,
    ALINK_CODE_ERROR_UTC = 3071,
    ALINK_CODE_ERROR_SIGN = 3074,
    //APP
    ALINK_CODE_ERROR_APP_NOT_LOGIN = 3002,
    ALINK_CODE_FAIL_ADMIN_EXIST = 3204,
    ALINK_CODE_ERROR_AUTHTOKEN = 3601,
    ALINK_CODE_ERROR_AUTHTOKEN_EXPIRED = 3603,
    ALINK_CODE_ERROR_AUTHTOKEN_CANCELED = 3607,
    ALINK_CODE_ERROR_AUTHTOKEN_INUSE = 3608,
    ALINK_CODE_ERROR_AUTHTOKEN_STATE = 3609,
};

typedef struct AlinkResponse {
    int code;
    int msgLen;
    int dataLen;
    char *p_cMsg;
    char *p_cData;
} AlinkResponse;

typedef struct AlinkRequest {
    int idLen;
    int timeLen;
    int accountLen;
    int methodLen;
    int paramsLen;
    int paramsType;
    char *p_cId;
    char *p_cTime;
    char *p_cAccount;
    char *p_cMethod;
    char *p_cParams;
} AlinkRequest;

typedef struct alink_data {
    char *method;
    char *data;
} alink_data_t;

typedef struct AlinkRequestState {
    uint32_t state;
    uint32_t id;
    char account[32];
} AlinkRequestState;

extern AlinkRequestState last_state;

int32_t alink_sync_time(char *time, int len);
int32_t alink_login(void);
int32_t alink_logout(void);
int32_t alink_register(const device_t *dev);
int32_t alink_post(const char *method, char *buff);
int32_t __alink_post(const char *method, char *buff);
int32_t alink_get(const char *method, char *buff, char *result, int len);
int32_t alink_parse_data(void *data, int len, alink_data_t *pack);

int alink_handshake(void);
int alink_error_code_handler(uint32_t error_code);

const char *alink_get_sdk_version(char *buff, unsigned int len);

int32_t alink_unregister(void);

int32_t alink_response_get_data(char *in, char *data, int len);
int32_t alink_response_get_x(char *in, char *out, int outLen, char *key);
int32_t alink_parse_request(char *p_cJsonStr, int iStrLen,
                            AlinkRequest *p_stReq);
int32_t alink_parse_response(char *p_cJsonStr, int iStrLen,
                             AlinkResponse *p_stRsp);
void alink_attrfilter(char *cJsonStr);
#endif
