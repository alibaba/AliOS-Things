/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "alink_protocol.h"
#include "stdlib.h"
#include "device.h"
#include "digest_algorithm.h"
#include "json_parser.h"
#include "accs.h"
#include "service.h"
#include "wsf.h"
#include "config.h"
#include "git_version.h"
#include "aos/aos.h"
#include "awss.h"
#include "os.h"
#include "enrollee.h"

#define ALINK_PROTOCOL_DEBUG 0
#if(ALINK_PROTOCOL_DEBUG==1)
#define alink_debug_protocol LOGD
#else
#define alink_debug_protocol
#endif

#define ALINK_JSON_PARSE_DEBUG 0
#if(ALINK_JSON_PARSE_DEBUG==1)
#define alink_debug_parse LOGD
#else
#define alink_debug_parse
#endif

#define SIGNATURE_RESULT_MAX_SIZE   (256)
#define SIGNATURE_FORMAT_MAX_SIZE   (256)

#define MODULE_NAME_ALINK_PROTOCOL "alink_protocol"
extern void *link_buff_sem;
extern uint8_t *uplink_buff;
extern uint8_t *downlink_buff;
extern const device_t *main_device;
extern const connectivity_t *remote_conn;
extern void start_accs_work(int delay);

AlinkRequestState last_state = {0, 0, {0}};

const char SystemFormat[] =
    "{\"system\":{\"alink\":\"%s\",\"jsonrpc\":\"2.0\",\"lang\":\"en\",\"sign\":\"%s\",\"key\":\"%s\",\"time\":\"%s\"},";
const char RequestFormat[] =
    "\"request\":{\"cid\":\"%s\",\"rspID\":%d,\"target\":\"%s\",\"token\":\"%s\",\"uuid\":\"%s\"},";
const char RequestFormat_test[] =
    "\"request\":{\"cid\":\"%s\",\"rspID\":%d,\"service\":\"check\",\"target\":\"%s\",\"testId\":\"%s\",\"token\":\"%s\",\"uuid\":\"%s\"},";
#if (defined(SUPPORT_ID2) || defined(CONFIG_SDS))
const char SignatureFormat[] =
    "cid:%s,rspID:%d,target:%s,token:%s,uuid:%s,time:%s";
const char SignatureFormat_test[] =
    "cid:%s,rspID:%d,service:check,target:%s,testId:%s,token:%s,uuid:%s,time:%s";
#else
const char SignatureFormat[] =
    "cid:%s,rspID:%d,target:%s,token:%s,uuid:%s,time:%s,secret:%s";
const char SignatureFormat_test[] =
    "cid:%s,rspID:%d,service:check,target:%s,testId:%s,token:%s,uuid:%s,time:%s,secret:%s";
#endif
const char SyncTimeFormat[] =
    "\"method\":\"getAlinkTime\",\"params\":{},\"id\":%d}";
const char RegisterDeviceFormat[] =
    "\"method\":\"%s\",\"params\":{\"model\":\"%s\",\"sn\":\"%s\",\"mac\":\"%s\",\"version\":\"%s;APP2.0;OTA1.0\",\"description\":\"%s\"},\"id\":%d}";
const char UnregisterDeviceFormat[] =
    "\"method\":\"unregisterDevice\",\"params\":{\"model\":\"%s\",\"sn\":\"%s\",\"mac\":\"%s\"},\"id\":%d}";
const char LoginFormat[] =
    "\"method\":\"loginDevice\",\"params\":{\"sdkversion\":\"%s\"},\"id\": %d}";
const char LoginFormatWithToken[] =
    "\"method\":\"loginDevice\",\"params\":{\"sdkversion\":\"%s\", \"traceToken\": \"%s\"},\"id\": %d}";
const char LogoutFormat[] =
    "\"method\":\"logoutDevice\",\"params\":{},\"id\": %d}";
const char PostFormat[] = "\"method\":\"%s\",\"params\":%s,\"id\":%d}";

const char registerByAuthTokenFormat[] =
    "\"method\": \"%s\",\"params\": {\"authToken\": \"%s\",\"name\": \"%s\",\"model\": \"%s\",\"type\": \"%s\",\"category\": \"%s\",\"sn\": \"%s\",\"mac\": \"%s\",\"manufacturer\": \"%s\",\"version\": \"%s\"},\"id\": %d";

static uint32_t alink_id_global = 100;

static int alink_phase;

static void get_protocol_buff(void)
{
    LOGD("accs", "------->>> lock <<<------- \n");
    os_semaphore_wait(link_buff_sem, OS_WAIT_INFINITE);
    memset(uplink_buff, 0, ALINK_BUF_SIZE);
}

static void put_protocol_buff(void)
{
    LOGD("accs", "------->>> unlock <<<--------\n");
    os_semaphore_post(link_buff_sem);
}

static inline unsigned int alink_generate_trans_id()
{
    unsigned int id;
    id = alink_id_global++;
    return id;
}

static void alink_calculate_signature(char *signbuf, int *signbuf_len,
                                      int rspid, const char *target, char *time)
{
    char buf[SIGNATURE_FORMAT_MAX_SIZE] = { 0 };

#ifdef SUPPORT_ID2
    /* no need to sign after alink login */
    if (alink_phase == PHASE_READY) {
        return;
    }
#endif

    if (main_device->config->test_mode)
        snprintf(buf, sizeof(buf), SignatureFormat_test,
                 main_device->info->cid, rspid, target,
                 main_device->config->test_id,
                 main_device->config->token,
                 main_device->config->uuid, time
#if (!defined(SUPPORT_ID2) && !defined(CONFIG_SDS))
                 , devinfo_get_secret()
#endif
                );
    else
        snprintf(buf, sizeof(buf), SignatureFormat,
                 main_device->info->cid, rspid, target,
                 main_device->config->token,
                 main_device->config->uuid, time
#if (!defined(SUPPORT_ID2) && !defined(CONFIG_SDS))
                 , devinfo_get_secret()
#endif
                );

    alink_debug_protocol(MODULE_NAME_ALINK_PROTOCOL, "fingerprint: %s", buf);

#if defined(SUPPORT_ID2)
    {
        uint8_t res[TFS_ID2_SIGN_SIZE] = { 0 };
        int ret, res_len = sizeof(res);

        ret = tfs_id2_sign(buf, strlen(buf), res, &res_len);
        if (ret) {
            log_error("tfs_id2_sign error(%d)", ret);
        }

        base64_encode(res, res_len, signbuf, signbuf_len);
    }
#elif defined(CONFIG_SDS)
    {
        uint8_t digest[16] = {0};
        int i = 0;

        digest_hmac(DIGEST_TYPE_MD5, buf, strlen(buf), devinfo_get_device_secret(), strlen(devinfo_get_device_secret()),
                    digest);
        for (i = 0; i < sizeof(digest); i++) {
            sprintf(signbuf + i * 2, "%02x", digest[i]);
        }

        if (signbuf_len) {
            *signbuf_len = sizeof(digest) * 2;
        }
        alink_debug_protocol("hmac md5 sign: %s", signbuf);
    }
#else
    {
        uint8_t md5_ret[MD5_SIZE_BYTE];
        int i;

        digest_md5(buf, strlen(buf), md5_ret);

        for (i = 0; i < MD5_SIZE_BYTE; i++) {
            sprintf(signbuf + i * 2, "%02x", md5_ret[i]);
        }

        if (signbuf_len) {
            *signbuf_len = MD5_SIZE_BYTE * 2;
        }
    }
#endif
}

static void alink_clear_push_state()
{
    last_state.state = 0;
    last_state.id = -1;
    memset(last_state.account, 0, sizeof(last_state.account));
}

static void alink_save_push_state(char *id, char *account)
{
    last_state.state = 1;
    last_state.id = atoi(id);
    if (account) {
        strncpy(last_state.account, account, sizeof(last_state.account) - 1);
    } else {
        strncpy(last_state.account, "", sizeof(last_state.account) - 1);
    }
}

static void alink_request_json_system(char *buf, int rspid, const char *target,
                                      char *time)
{
    char signature[SIGNATURE_RESULT_MAX_SIZE] = { 0 };
    int signature_len = SIGNATURE_RESULT_MAX_SIZE;
    alink_calculate_signature(signature, &signature_len, rspid, target, time);
#ifdef CONFIG_SDS
    sprintf(buf, SystemFormat, main_device->info->alink_version, signature, devinfo_get_device_key(), time);
#else
    sprintf(buf, SystemFormat, main_device->info->alink_version, signature,
            devinfo_get_key(), time);
#endif
    if (main_device->config->test_mode) {
        sprintf(buf + strlen(buf), RequestFormat_test, main_device->info->cid, rspid,
                target, main_device->config->test_id, main_device->config->token,
                main_device->config->uuid);
    } else {
        sprintf(buf + strlen(buf), RequestFormat, main_device->info->cid, rspid, target,
                main_device->config->token, main_device->config->uuid);
    }
}

//TODO: add one async version.
int32_t alink_sync_time(char *time, int len)
{
    int ret;
    connectivity_rsp_t *rsp;

    get_protocol_buff();
    alink_request_json_system(uplink_buff, -1, "", "");
    sprintf(uplink_buff + strlen(uplink_buff), SyncTimeFormat,
            alink_generate_trans_id());
    alink_debug_protocol(MODULE_NAME_ALINK_PROTOCOL, "sync time:%s", uplink_buff);

    rsp = remote_conn->send(uplink_buff, strlen(uplink_buff));
    if (!rsp) {
        ret = ALINK_CODE_ERROR_TIMEOUT;
    } else {
        ret = alink_response_get_x(rsp->data, time, len, "time");
        alink_debug_protocol(MODULE_NAME_ALINK_PROTOCOL, "result:%s", time);
    }
    put_protocol_buff();
    alink_error_code_handler(ret);
    return ret;
}
/*
* alink_get_sdk_version returns total SDK version of module
* A[alink1.0|-093805c|0000]M[linux]OS[ubuntu 12.04 LTS]T[linux.1487736092]
* A[alink1.0|-093805c|0000] -- Alink Version, Alink commit id, Feature
* OS[ubuntu 12.04 LTS] -- OS vesion
* T[linux.1487736092] -- Channel id (Modeule name + SDK build timestamp)
*/

const char *alink_get_sdk_version(char *buff, unsigned int len)
{
    char *version, *module_name;

    OS_CHECK_PARAMS(buff);
    if (!main_device) {
        return buff;
    }
    version = (char *)os_malloc(STR_SHORT_LEN);
    module_name = (char *)os_malloc(STR_SHORT_LEN);
    OS_CHECK_MALLOC(version && module_name);

    memset(version, 0, STR_SHORT_LEN);
    os_get_version(version);

    memset(module_name, 0, STR_SHORT_LEN);
    os_get_module_name(module_name);

    snprintf(buff, len, "A[%s|%s|%04x]OS[%s]T[%s.%s]",
             main_device->info->alink_version, ALINK_AGENT_GIT_VERSION, 0x0000, version,
             module_name, ALINK_AGENT_BUILD_TIME); //todo: feature to be complete
    os_free(version);
    os_free(module_name);
    return buff;
}

typedef struct {
    int (*cb)(int);
    char *uuid;
    char *token;
} _alink_reg_t;

static void *_alink_reg_cb(connectivity_rsp_t *rsp, void *cb)
{
    int ret = 0;
    _alink_reg_t *reg = cb;
    LOGD(MODULE_NAME_ALINK_PROTOCOL, "");
    if (!rsp) {
        ret = ALINK_CODE_ERROR_TIMEOUT;
    } else {
        ret = alink_response_get_x(rsp->data, reg->uuid, UUID_LEN, "uuid");
        ret = alink_response_get_x(rsp->data, reg->token, TOKEN_LEN, "token");
        alink_debug_protocol(MODULE_NAME_ALINK_PROTOCOL, "result: uuid %s, token %s",
                             reg->uuid, reg->token);
        config_update();
    }
    put_protocol_buff();
    alink_error_code_handler(ret);
    reg->cb(ALINK_CODE_SUCCESS == ret);
    os_free(reg);
    return NULL;
}

int32_t alink_register_async(const device_t *dev, int (*cb)(int))
{
    int ret;
    connectivity_rsp_t *rsp;
    char *alink_version = NULL;
    _alink_reg_t *reg = NULL;

    alink_version = (char *)os_malloc(STR_LONG_LEN);
    OS_CHECK_MALLOC(alink_version);
    reg = (_alink_reg_t *)os_malloc(sizeof(_alink_reg_t));
    OS_CHECK_MALLOC(reg);
    memset(alink_version, 0, STR_LONG_LEN);
    alink_get_sdk_version(alink_version, STR_LONG_LEN);

    get_protocol_buff();
    alink_request_json_system(uplink_buff, -1, "", "");
    snprintf(uplink_buff + strlen(uplink_buff),
             ALINK_BUF_SIZE - strlen(uplink_buff), RegisterDeviceFormat,
#ifdef ALINK_SECURITY
             "device.register",
#else
             "registerDevice",
#endif
             dev->info->model, dev->info->sn, dev->info->mac, dev->info->firmware_version,
             alink_version,
             alink_generate_trans_id());
    os_free(alink_version);
    alink_debug_protocol(MODULE_NAME_ALINK_PROTOCOL, "alink_register: %s",
                         uplink_buff);

    reg->cb = cb;
    reg->uuid = dev->config->uuid;
    reg->token = dev->config->token;
    ret = remote_conn->send_async(uplink_buff, strlen(uplink_buff), _alink_reg_cb,
                                  reg);
    return ret;
}

int32_t alink_register(const device_t *dev)
{
    int ret;
    connectivity_rsp_t *rsp;
    char *alink_version = NULL;

    alink_version = (char *)os_malloc(STR_LONG_LEN);
    OS_CHECK_MALLOC(alink_version);
    memset(alink_version, 0, STR_LONG_LEN);
    alink_get_sdk_version(alink_version, STR_LONG_LEN);

    get_protocol_buff();
    alink_request_json_system(uplink_buff, -1, "", "");
    snprintf(uplink_buff + strlen(uplink_buff),
             ALINK_BUF_SIZE - strlen(uplink_buff), RegisterDeviceFormat,
#ifdef ALINK_SECURITY
             "device.register",
#else
             "registerDevice",
#endif
             dev->info->model, dev->info->sn, dev->info->mac, dev->info->firmware_version,
             alink_version,
             alink_generate_trans_id());
    os_free(alink_version);
    alink_debug_protocol(MODULE_NAME_ALINK_PROTOCOL, "alink_register: %s",
                         uplink_buff);

    rsp = remote_conn->send(uplink_buff, strlen(uplink_buff));
    if (!rsp) {
        ret = ALINK_CODE_ERROR_TIMEOUT;
    } else {
        ret = alink_response_get_x(rsp->data, dev->config->uuid, UUID_LEN, "uuid");
        ret = alink_response_get_x(rsp->data, dev->config->token, TOKEN_LEN, "token");
        alink_debug_protocol(MODULE_NAME_ALINK_PROTOCOL, "result: uuid %s, token %s",
                             dev->config->uuid, dev->config->token);
        config_update();
    }
    put_protocol_buff();
    alink_error_code_handler(ret);
    return ret;
}

static void *_alink_login_cb(connectivity_rsp_t *rsp, void *cb)
{
    int ret = 0;
    int (*_cb)(int) = cb;
    if (!rsp) {
        ret = ALINK_CODE_ERROR_TIMEOUT;
    } else {
        alink_debug_protocol(MODULE_NAME_ALINK_PROTOCOL, "result: %s",
                             (char *)rsp->data);
        ret = alink_response_get_x(rsp->data, NULL, 0, NULL);
    }
    put_protocol_buff();
    alink_error_code_handler(ret);

    if (_cb) {
        _cb(ret);
    }

    return NULL;
}

int32_t alink_login_async(int (*cb)(int))
{
    int ret;
    connectivity_rsp_t *rsp;
    char *alink_version = NULL;

    alink_version = (char *)os_malloc(STR_LONG_LEN);
    OS_CHECK_MALLOC(alink_version);
    memset(alink_version, 0, STR_LONG_LEN);
    alink_get_sdk_version(alink_version, STR_LONG_LEN);

    get_protocol_buff();
    alink_request_json_system(uplink_buff, -1, "", "");
    char ssid[PLATFORM_MAX_SSID_LEN];
    memset(ssid, 0, PLATFORM_MAX_SSID_LEN);
    os_wifi_get_ap_info( ssid , NULL, NULL);
    if ((strcmp(ssid, DEFAULT_SSID) == 0) || (NULL == awss_get_enrollee_token()) ||
        (0 == strlen(awss_get_enrollee_token()))) {
        sprintf(uplink_buff + strlen(uplink_buff), LoginFormat, alink_version,
                alink_generate_trans_id());
    } else {
        sprintf(uplink_buff + strlen(uplink_buff), LoginFormatWithToken, alink_version,
                awss_get_enrollee_token(), alink_generate_trans_id());
    }
    //sprintf(uplink_buff+strlen(uplink_buff), LoginFormat, alink_version, alink_generate_trans_id());
    alink_debug_protocol(MODULE_NAME_ALINK_PROTOCOL, "alink_login : %s",
                         uplink_buff);
    os_free(alink_version);

    ret = remote_conn->send_async(uplink_buff, strlen(uplink_buff), _alink_login_cb,
                                  cb);
    return ret;
}


int32_t alink_login(void)
{
    int ret;
    connectivity_rsp_t *rsp;
    char *alink_version = NULL;

    alink_version = (char *)os_malloc(STR_LONG_LEN);
    OS_CHECK_MALLOC(alink_version);
    memset(alink_version, 0, STR_LONG_LEN);
    alink_get_sdk_version(alink_version, STR_LONG_LEN);

    get_protocol_buff();
    alink_request_json_system(uplink_buff, -1, "", "");
    char ssid[PLATFORM_MAX_SSID_LEN];
    memset(ssid, 0, PLATFORM_MAX_SSID_LEN);
    os_wifi_get_ap_info( ssid , NULL, NULL);
    if ((strcmp(ssid, DEFAULT_SSID) == 0) || (NULL == awss_get_enrollee_token()) ||
        (0 == strlen(awss_get_enrollee_token()))) {
        sprintf(uplink_buff + strlen(uplink_buff), LoginFormat, alink_version,
                alink_generate_trans_id());
    } else {
        sprintf(uplink_buff + strlen(uplink_buff), LoginFormatWithToken, alink_version,
                awss_get_enrollee_token(), alink_generate_trans_id());
    }
    //sprintf(uplink_buff+strlen(uplink_buff), LoginFormat, alink_version, alink_generate_trans_id());
    alink_debug_protocol(MODULE_NAME_ALINK_PROTOCOL, "alink_login : %s",
                         uplink_buff);
    os_free(alink_version);

    rsp = remote_conn->send(uplink_buff, strlen(uplink_buff));
    if (!rsp) {
        ret = ALINK_CODE_ERROR_TIMEOUT;
    } else {
        alink_debug_protocol(MODULE_NAME_ALINK_PROTOCOL, "result: %s",
                             (char *)rsp->data);
        ret = alink_response_get_x(rsp->data, NULL, 0, NULL);
    }
    put_protocol_buff();
    alink_error_code_handler(ret);
    return ret;
}

int32_t alink_logout(void)
{
    int ret;
    connectivity_rsp_t *rsp;

    get_protocol_buff();
    alink_request_json_system(uplink_buff, -1, "", "");
    sprintf(uplink_buff + strlen(uplink_buff), LogoutFormat,
            alink_generate_trans_id());

    rsp = remote_conn->send(uplink_buff, strlen(uplink_buff));
    if (!rsp) {
        ret = ALINK_CODE_ERROR_TIMEOUT;
    } else {
        alink_debug_protocol(MODULE_NAME_ALINK_PROTOCOL, "result: %s",
                             (char *)rsp->data);
        ret = alink_response_get_x(rsp->data, NULL, 0, NULL);
    }
    put_protocol_buff();
    alink_error_code_handler(ret);
    return ret;
}

typedef struct {
    void *(*cb)(void *);
    void *arg;
} __alink_post_cb_t;
static void *__alink_post_cb(connectivity_rsp_t *rsp, void *cb)
{
    int ret = 0;
    __alink_post_cb_t *_cb = cb;
    if (!rsp) {
        ret = ALINK_CODE_ERROR_TIMEOUT;
    } else {
        alink_debug_protocol(MODULE_NAME_ALINK_PROTOCOL, "alink_post:%s",
                             (char *)rsp->data);
        ret = alink_response_get_x(rsp->data, NULL, 0, NULL);
    }
    alink_error_code_handler(ret);
    LOGD(MODULE_NAME_ALINK_PROTOCOL, "get posted respon back , cb: %p. \n", _cb);
    if (_cb) {
        _cb->cb(_cb->arg);
        os_free(_cb);
    }

    return NULL;
}


int32_t __alink_post_async(const char *method, char *buff, void *(*cb)(void *),
                           void *arg)
{
    int ret;
    connectivity_rsp_t *rsp;
    __alink_post_cb_t *_cb = NULL;

    if (cb) {
        _cb = os_malloc(sizeof(__alink_post_cb_t));
        OS_CHECK_PARAMS(_cb);
        _cb->cb = cb;
        _cb->arg = arg;
    }

    get_protocol_buff();
    if (last_state.state) {
        alink_request_json_system(uplink_buff, last_state.id, last_state.account, "");
        alink_clear_push_state();
    } else {
        alink_request_json_system(uplink_buff, -1, "", "");
    }
#ifdef _ALINK_MAINTENANCE_
    alink_attrfilter(buff);
#endif
    sprintf(uplink_buff + strlen(uplink_buff), PostFormat, method, buff,
            alink_generate_trans_id());
    alink_debug_protocol(MODULE_NAME_ALINK_PROTOCOL, "alink_post:%s",
                         (char *)uplink_buff);

    ret = remote_conn->send_async(uplink_buff, strlen(uplink_buff), __alink_post_cb,
                                  _cb);
    put_protocol_buff();
    return ret;
}


int32_t __alink_post(const char *method, char *buff)
{
    int ret;
    connectivity_rsp_t *rsp;

    get_protocol_buff();
    if (last_state.state) {
        alink_request_json_system(uplink_buff, last_state.id, last_state.account, "");
        alink_clear_push_state();
    } else {
        alink_request_json_system(uplink_buff, -1, "", "");
    }
#ifdef _ALINK_MAINTENANCE_
    alink_attrfilter(buff);
#endif
    sprintf(uplink_buff + strlen(uplink_buff), PostFormat, method, buff,
            alink_generate_trans_id());
    alink_debug_protocol(MODULE_NAME_ALINK_PROTOCOL, "alink_post:%s",
                         (char *)uplink_buff);

    rsp = remote_conn->send(uplink_buff, strlen(uplink_buff));//wsf_send
    if (!rsp) {
        ret = ALINK_CODE_ERROR_TIMEOUT;
    } else {
        alink_debug_protocol(MODULE_NAME_ALINK_PROTOCOL, "alink_post:%s",
                             (char *)rsp->data);
        ret = alink_response_get_x(rsp->data, NULL, 0, NULL);
    }
    put_protocol_buff();
    alink_error_code_handler(ret);
    return ret;
}

int32_t alink_post_async(const char *method, char *buff, void *(*cb)(void *),
                         void *arg)
{
    if (!cloud_is_connected()) {
        return SERVICE_RESULT_ERR;
    }
    return (__alink_post_async(method, buff, cb,
                               arg) == ALINK_CODE_SUCCESS) ? SERVICE_RESULT_OK : SERVICE_RESULT_ERR;
}


int32_t alink_post(const char *method, char *buff)
{
    if (!cloud_is_connected()) {
        return SERVICE_RESULT_ERR;
    }
    return (__alink_post(method,
                         buff) == ALINK_CODE_SUCCESS) ? SERVICE_RESULT_OK : SERVICE_RESULT_ERR;
}

typedef struct {
    char *res;
    int len;
} _alink_prot_cb_t;
static void *_alink_protocol_parser(connectivity_rsp_t *rsp, void *cb)
{
    int ret = 0;
    _alink_prot_cb_t *_cb = cb;
    if (!rsp) {
        ret = ALINK_CODE_ERROR_TIMEOUT;
    } else {
        alink_debug_protocol(MODULE_NAME_ALINK_PROTOCOL, "alink_get:%s",
                             (char *)rsp->data);
        ret = alink_response_get_data(rsp->data, _cb->res, _cb->len);
    }

    os_free(_cb);
    alink_error_code_handler(ret);
    return NULL;
}

int32_t alink_get_async(const char *method, char *buff, char *result, int len)
{
    int ret;
    connectivity_rsp_t *rsp;
    _alink_prot_cb_t *cb = NULL;

    if (!cloud_is_connected()) {
        return SERVICE_RESULT_ERR;
    }

    cb = os_malloc(sizeof(_alink_prot_cb_t));
    if (!cb) {
        return 0;
    }
    cb->res = result;
    cb->len = len;

    get_protocol_buff();
    alink_request_json_system(uplink_buff, -1, "", "");
#ifdef _ALINK_MAINTENANCE_
    alink_attrfilter(buff);
#endif
    sprintf(uplink_buff + strlen(uplink_buff), PostFormat, method, buff,
            alink_generate_trans_id());
    alink_debug_protocol(MODULE_NAME_ALINK_PROTOCOL, "alink_get:%s",
                         (char *)uplink_buff);

    ret = remote_conn->send_async(uplink_buff, strlen(uplink_buff),
                                  _alink_protocol_parser, cb);
    put_protocol_buff();
    return ret;
}


int32_t alink_get(const char *method, char *buff, char *result, int len)
{
    int ret;
    connectivity_rsp_t *rsp;

    if (!cloud_is_connected()) {
        return SERVICE_RESULT_ERR;
    }

    get_protocol_buff();
    alink_request_json_system(uplink_buff, -1, "", "");
#ifdef _ALINK_MAINTENANCE_
    alink_attrfilter(buff);
#endif
    sprintf(uplink_buff + strlen(uplink_buff), PostFormat, method, buff,
            alink_generate_trans_id());
    alink_debug_protocol(MODULE_NAME_ALINK_PROTOCOL, "alink_get:%s",
                         (char *)uplink_buff);

    rsp = remote_conn->send(uplink_buff, strlen(uplink_buff));
    if (!rsp) {
        ret = ALINK_CODE_ERROR_TIMEOUT;
    } else {
        alink_debug_protocol(MODULE_NAME_ALINK_PROTOCOL, "alink_get:%s",
                             (char *)rsp->data);
        ret = alink_response_get_data(rsp->data, result, len);
    }
    put_protocol_buff();
    alink_error_code_handler(ret);
    return ret;
}

int32_t alink_parse_data(void *data, int len, alink_data_t *pack)
{
    AlinkRequest stReq = {0};
    int ret = alink_parse_request(data, len, (void *)&stReq);

    if (ret != JSON_RESULT_OK || !stReq.p_cMethod || !stReq.p_cParams) {
        return ALINK_CODE_ERROR_MALFORMED_PACK;
    }

    *(stReq.p_cId + stReq.idLen) = 0;
    if (stReq.p_cAccount) {
        *(stReq.p_cAccount + stReq.accountLen) = 0;
    }
    alink_save_push_state(stReq.p_cId, stReq.p_cAccount);

    *(stReq.p_cMethod + stReq.methodLen) = 0;
    *(stReq.p_cParams + stReq.paramsLen) = 0;
    pack->method = stReq.p_cMethod;
    pack->data = stReq.p_cParams;

    return ALINK_CODE_SUCCESS;
}

/*******alink json parse*********/
int32_t alink_response_get_data(char *in, char *data, int len)
{
    AlinkResponse rsp = {ALINK_CODE_ERROR_MALFORMED_PACK, 0, 0, 0, 0};

    if (!in || *in == 0 || !data || len <= 0) {
        return rsp.code;
    }

    alink_parse_response(in, strlen(in), (void *)&rsp);
    if (rsp.code != ALINK_CODE_SUCCESS || !data || len <= 0) {
        return rsp.code;
    } else {
        int copy_len = rsp.dataLen < len - 1 ? rsp.dataLen : len - 1;

        memcpy(data, rsp.p_cData, copy_len);
        data[copy_len] = '\0';

        return rsp.code;
    }
}

int32_t alink_response_get_x(char *in, char *out, int outLen, char *key)
{
    int len = 0;
    char *pVal = NULL;
    AlinkResponse rsp = {ALINK_CODE_ERROR_MALFORMED_PACK, 0, 0, 0, 0};

    if (!in || *in == 0) {
        return rsp.code;
    }

    alink_parse_response(in, strlen(in), (void *)&rsp);
    if (rsp.code != ALINK_CODE_SUCCESS || !out || !outLen || !key) {
        return rsp.code;
    }

    if (!strcmp(key, "code")) {  //if "code" is required, just fulfill msg
        pVal = rsp.p_cMsg;
        len = rsp.msgLen;
    } else if (!strcmp(key, "data")) { //if "data" is required, just fulfill data
        pVal = rsp.p_cData;
        len = rsp.dataLen;
    } else if (
        rsp.p_cData) {   //get value by key from "data", key is "uuid" or "time"
        pVal = json_get_value_by_name(rsp.p_cData, rsp.dataLen, key, &len, 0);
    }

    if (pVal) {
        out[0] = '\0';
        len = outLen < len ? outLen : len;
        strncpy(out, pVal, len);
        *(out + len) = '\0';
    }
    return rsp.code;
}

int32_t alink_parse_response_CB(char *p_cName, int iNameLen, char *p_cValue,
                                int iValueLen, int iValueType, void *p_CBData)
{
    int i;
    alink_debug_parse(MODULE_NAME_ALINK_PROTOCOL, "\nName:\n  ");
    if (p_cName)  {
        for (i = 0; i < iNameLen; i++) {
            alink_debug_parse(MODULE_NAME_ALINK_PROTOCOL, "%c", *(p_cName + i));
        }
    }
    alink_debug_parse(MODULE_NAME_ALINK_PROTOCOL, "\nValue:\n  ");
    if (p_cValue) {
        for (i = 0; i < iValueLen; i++) {
            alink_debug_parse(MODULE_NAME_ALINK_PROTOCOL, "%c", *(p_cValue + i));
        }
    }
    alink_debug_parse(MODULE_NAME_ALINK_PROTOCOL, "\n");

    if (!strncmp(p_cName, "result", iNameLen)) {
        AlinkResponse *p_stRsp = (AlinkResponse *)p_CBData;
        int resultLen = 0;
        char *p_cResult = NULL;
        p_cResult = json_get_value_by_name(p_cValue, iValueLen, "code", &resultLen, 0);
        if (p_cResult) {
            char cLastChar = *(p_cResult + resultLen);
            *(p_cResult + resultLen) = 0;
            p_stRsp->code = atoi(p_cResult);
            *(p_cResult + resultLen) = cLastChar;
        }
        p_stRsp->p_cMsg = json_get_value_by_name(p_cValue, iValueLen, "msg",
                                                 &p_stRsp->msgLen, 0);
        p_stRsp->p_cData = json_get_value_by_name(p_cValue, iValueLen, "data",
                                                  &p_stRsp->dataLen, 0);
    }
    return JSON_PARSE_OK;
}

int32_t alink_parse_response(char *p_cJsonStr, int iStrLen,
                             AlinkResponse *p_stRsp)
{
    return json_parse_name_value(p_cJsonStr, iStrLen, alink_parse_response_CB,
                                 (void *)p_stRsp);
}

int32_t alink_parse_request_CB(char *p_cName, int iNameLen, char *p_cValue,
                               int iValueLen, int iValueType, void *p_CBData)
{
    int i;
    int mokstrLen = 0, moktestidLen = 0;
    char *mokstr = NULL, *moktestid = NULL;
    int need_update = -1;
    alink_debug_parse(MODULE_NAME_ALINK_PROTOCOL, "\nName:\n  ");
    if (p_cName)  {
        for (i = 0; i < iNameLen; i++) {
            alink_debug_parse(MODULE_NAME_ALINK_PROTOCOL, "%c", *(p_cName + i));
        }
    }
    alink_debug_parse(MODULE_NAME_ALINK_PROTOCOL, "\nValue:\n  ");
    if (p_cValue) {
        for (i = 0; i < iValueLen; i++) {
            alink_debug_parse(MODULE_NAME_ALINK_PROTOCOL, "%c", *(p_cValue + i));
        }
    }
    alink_debug_parse(MODULE_NAME_ALINK_PROTOCOL, "\n");

    AlinkRequest *p_stReq = (AlinkRequest *)p_CBData;
    if (!strncmp(p_cName, "system", iNameLen)) {
        p_stReq->p_cTime = json_get_value_by_name(p_cValue, iValueLen, "time",
                                                  &p_stReq->timeLen, 0);
    } else if (!strncmp(p_cName, "request", iNameLen)) {
        p_stReq->p_cAccount = json_get_value_by_name(p_cValue, iValueLen, "account",
                                                     &p_stReq->accountLen, 0);
        if (main_device->config->test_mode) { //test mode begin
            moktestid = json_get_value_by_name(p_cValue, iValueLen, "testId", &moktestidLen,
                                               0);
            if ((moktestid != NULL) && (moktestidLen > 0) ) {
                if (moktestidLen < STR_SHORT_LEN) {
                    need_update = strncmp(main_device->config->test_id, moktestid, moktestidLen);
                    strncpy(main_device->config->test_id, moktestid, moktestidLen);
                    main_device->config->test_id[moktestidLen] = '\0';
                    if (need_update) {
                        config_update();
                    }
                }
            }
        }
    } else if (!strncmp(p_cName, "method", iNameLen)) {
        p_stReq->p_cMethod = p_cValue;
        p_stReq->methodLen = iValueLen;
    } else if (!strncmp(p_cName, "params", iNameLen)) {
        p_stReq->p_cParams = p_cValue;
        p_stReq->paramsLen = iValueLen;
        p_stReq->paramsType = iValueType;
    } else if (!strncmp(p_cName, "id", iNameLen)) {
        p_stReq->p_cId = p_cValue;
        p_stReq->idLen = iValueLen;
    }
    return JSON_PARSE_OK;
}

int32_t alink_parse_request(char *p_cJsonStr, int iStrLen,
                            AlinkRequest *p_stReq)
{
    return json_parse_name_value(p_cJsonStr, iStrLen, alink_parse_request_CB,
                                 (void *)p_stReq);
}

static void *_alink_unreg_cb(connectivity_rsp_t *rsp, void *cb)
{
    int ret = 0;
    int (*_cb)(int) = cb;
    if (!rsp) {
        ret = ALINK_CODE_ERROR_TIMEOUT;
    } else {
        alink_debug_protocol(MODULE_NAME_ALINK_PROTOCOL, "result: %s",
                             (char *)rsp->data);
        ret = alink_response_get_x(rsp->data, NULL, 0, NULL);
    }
    put_protocol_buff();
    alink_error_code_handler(ret);

    _cb(ret);
    return NULL;
}

int32_t alink_unregister_async(int (*cb)(int))
{
    int ret;
    connectivity_rsp_t *rsp;

    get_protocol_buff();
    alink_request_json_system(uplink_buff, -1, "", "");
    snprintf(uplink_buff + strlen(uplink_buff),
             ALINK_BUF_SIZE - strlen(uplink_buff),
             UnregisterDeviceFormat,
             main_device->info->model, main_device->info->sn, main_device->info->mac,
             alink_generate_trans_id());
    alink_debug_protocol(MODULE_NAME_ALINK_PROTOCOL, "alink_unregister: %s",
                         uplink_buff);

    ret = remote_conn->send_async(uplink_buff, strlen(uplink_buff), _alink_unreg_cb,
                                  cb);
    return ret;
}


int32_t alink_unregister(void)
{
    int ret;
    connectivity_rsp_t *rsp;

    get_protocol_buff();
    alink_request_json_system(uplink_buff, -1, "", "");
    snprintf(uplink_buff + strlen(uplink_buff),
             ALINK_BUF_SIZE - strlen(uplink_buff),
             UnregisterDeviceFormat,
             main_device->info->model, main_device->info->sn, main_device->info->mac,
             alink_generate_trans_id());
    alink_debug_protocol(MODULE_NAME_ALINK_PROTOCOL, "alink_unregister: %s",
                         uplink_buff);

    rsp = remote_conn->send(uplink_buff, strlen(uplink_buff));
    if (!rsp) {
        ret = ALINK_CODE_ERROR_TIMEOUT;
    } else {
        alink_debug_protocol(MODULE_NAME_ALINK_PROTOCOL, "result: %s",
                             (char *)rsp->data);
        ret = alink_response_get_x(rsp->data, NULL, 0, NULL);
    }
    put_protocol_buff();
    alink_error_code_handler(ret);

    return ret;
}

int alink_error_code_handler(uint32_t code)
{
    switch (code) {
        //SUCCESS
        case ALINK_CODE_SUCCESS:
        case ALINK_CODE_SUCCESS_REBOUND:
            break;
        //RETRY LATER
        case ALINK_CODE_ERROR_SYSTEM:
        case ALINK_CODE_ERROR_SERVICE_TIMEOUT:
        case ALINK_CODE_ERROR_SERVICE_UNAVAILABLE:
        case ALINK_CODE_ERROR_TOKEN_SERVICE:
            LOGW(MODULE_NAME_ALINK_PROTOCOL, "alink cloud busy, plz try later(%d)", code);
            alink_phase = PHASE_WAIT;
            break;
        //RE-LOGIN
        case ALINK_CODE_ERROR_DEV_NOT_LOGIN:
            LOGW(MODULE_NAME_ALINK_PROTOCOL, "device need to re-login(%d)", code);
            alink_phase = PHASE_REGISTER;
            start_accs_work(0);
            break;
        //RE-HANDSHAKE
        case ALINK_CODE_ERROR_TOKEN:
        case ALINK_CODE_ERROR_TOKEN_STATE:
        case ALINK_CODE_ERROR_UUID:
        case ALINK_CODE_ERROR_UUID_NO_INFO:
        case ALINK_CODE_ERROR_INVALID_UUID:
        case ALINK_CODE_ERROR_UUID_TOKEN_MISMATCH:
            /*
             * no need to clear config's uuid,
             * alink_register() will update_config()
             */
            main_device->config->uuid[0] = 0;
            LOGW(MODULE_NAME_ALINK_PROTOCOL, "device need to re-register(%d)", code);
            alink_phase = PHASE_REGISTER;
            start_accs_work(0);
            break;
        //FATAL, system hang
        case ALINK_CODE_ERROR_MISSING_PARAM:
        case ALINK_CODE_ERROR_INVLIAD_METHOD:
        case ALINK_CODE_ERROR_INVLIAD_CID:
        case ALINK_CODE_ERROR_CID_MISMATCH:
        case ALINK_CODE_ERROR_INVALID_PARAM_NODE:
        case ALINK_CODE_ERROR_DEVICE_NON_EXIST:
        case ALINK_CODE_ERROR_NULL_MODEL:
        case ALINK_CODE_ERROR_NULL_MAC:
        case ALINK_CODE_ERROR_NULL_REQDATA:
        case ALINK_CODE_ERROR_NULL_METHOD:
        case ALINK_CODE_ERROR_NULL_PARAM:
        case ALINK_CODE_ERROR_PARAM_ARGS:
        case ALINK_CODE_ERROR_NOT_MATCH_UUIDGEN:
        case ALINK_CODE_ERROR_SN_SHORT:
        case ALINK_CODE_ERROR_MAC:
        case ALINK_CODE_ERROR_SN_LONG:
        case ALINK_CODE_FAIL_UUID_GEN:
        case ALINK_CODE_ERROR_SN:
        case ALINK_CODE_ERROR_NOT_DEVICE11:
        case ALINK_CODE_ERROR_MODEL:
        case ALINK_CODE_ERROR_DEVICE_IN_BLACKLIST:
            LOGE(MODULE_NAME_ALINK_PROTOCOL, "fatal error: %d", code);
            alink_phase = PHASE_ABORT;
            break;
        //SUBDEV
        case ALINK_CODE_ERROR_SUBDEV_INVALID_UUID:
        case ALINK_CODE_ERROR_SUBDEV_NO_RELATIONSHIP:
        case ALINK_CODE_ERROR_SUBDEV_NOT_LOGIN:
            LOGW(MODULE_NAME_ALINK_PROTOCOL,
                 "subdev error(%d), devmgr will take care of it", code);
            break;
        //IGNORE
        case ALINK_CODE_ERROR_SIGN_EXCEPTION:
        case ALINK_CODE_ERROR_UTC:
        case ALINK_CODE_ERROR_SIGN:
        case ALINK_CODE_ERROR_TIMEOUT:
        default://most error code will be ignore
            LOGE(MODULE_NAME_ALINK_PROTOCOL, "alink err code: %d, ignore", code);
            break;
    }
    return alink_phase;
}
static int _g_handshake_cnt = 9 + 4;

static int _alink_handshake_cycle(int ret)
{
    if (--_g_handshake_cnt <= 0) {
        return 0;
    }
    LOGD(MODULE_NAME_ALINK_PROTOCOL, "cnt: %d ,phase: %d\n", _g_handshake_cnt, alink_phase);
    switch (alink_phase) {
        case PHASE_INIT:
            if (main_device->config->uuid[0] == 0) {
                alink_phase = PHASE_REGISTER;
            } else {
                alink_phase = PHASE_LOGIN;
            }
            break;
        case PHASE_REGISTER:
            if (ret) {
                alink_phase = PHASE_LOGIN;
            } else {
                return alink_register_async(main_device, _alink_handshake_cycle);
            }
            break;
        case PHASE_LOGIN:
            if (ret) {
                alink_phase = PHASE_LOGIN_SUCCESS;
                awss_clear_enrollee_token();
                break;
            } else {
                return alink_login_async(_alink_handshake_cycle);
            }

        case PHASE_LOGIN_SUCCESS:
            if (!config_get_unregister_flag()) {
                alink_phase = PHASE_READY;
            } else {
                alink_phase = PHASE_UNREGISTER;
            }
            break;
        case PHASE_UNREGISTER:
            if (ret) {
                main_device->config->uuid[0] = 0;
                config_set_unregister_flag(0);
                alink_phase = PHASE_REGISTER;
            } else {
                return alink_unregister_async(_alink_handshake_cycle);
            }
            break;
        case PHASE_READY:
            LOGI(MODULE_NAME_ALINK_PROTOCOL, "accs_handshake success");
            aos_post_event(EV_SYS, CODE_SYS_ON_ALINK_ONLINE, 0);
            return SERVICE_RESULT_OK;
        case PHASE_WAIT:
            start_accs_work(2 * 1000);
        case PHASE_ABORT:
        default:
            return SERVICE_RESULT_ERR;
    }
    return _alink_handshake_cycle(0);
}

int alink_handshake_async()
{
    LOGD(MODULE_NAME_ALINK_PROTOCOL, MODULE_NAME_ALINK_PROTOCOL);
    OS_ASSERT(main_device, "invalid main device");

    _g_handshake_cnt = 9 + 4;
    alink_phase = PHASE_INIT;
    return _alink_handshake_cycle(0);
}


int alink_handshake(void)
{
    int ret, loop = 9 + 4; /* 9 steps to login at most, +4 for redundancy */

    LOGD(MODULE_NAME_ALINK_PROTOCOL, MODULE_NAME_ALINK_PROTOCOL);
    OS_ASSERT(main_device, "invalid main device");

    alink_phase = PHASE_INIT;

    while (loop--) {
        switch (alink_phase) {
            case PHASE_INIT:
                if (main_device->config->uuid[0] == 0) {
                    alink_phase = PHASE_REGISTER;
                } else {
                    alink_phase = PHASE_LOGIN;
                }
                break;
            case PHASE_REGISTER:
                ret = alink_register(main_device);
                if (ret == ALINK_CODE_SUCCESS) {
                    alink_phase = PHASE_LOGIN;
                }
                break;
            case PHASE_LOGIN:
                ret = alink_login();
                if (ret == ALINK_CODE_SUCCESS) {
                    alink_phase = PHASE_LOGIN_SUCCESS;
                    awss_clear_enrollee_token();
                }
                break;
            case PHASE_LOGIN_SUCCESS:
                if (!config_get_unregister_flag()) {
                    alink_phase = PHASE_READY;
                } else {
                    alink_phase = PHASE_UNREGISTER;
                }
                break;
            case PHASE_UNREGISTER:
                ret = alink_unregister();
                if (ret == ALINK_CODE_SUCCESS) {
                    main_device->config->uuid[0] = 0;
                    config_set_unregister_flag(0);
                    alink_phase = PHASE_REGISTER;
                }
                break;
            case PHASE_READY:
                LOGI(MODULE_NAME_ALINK_PROTOCOL, "accs_handshake success");
                return SERVICE_RESULT_OK;
            case PHASE_WAIT:
                start_accs_work(2 * 1000);
            case PHASE_ABORT:
            default:
                return SERVICE_RESULT_ERR;
        }
    }

    return SERVICE_RESULT_ERR;
}
