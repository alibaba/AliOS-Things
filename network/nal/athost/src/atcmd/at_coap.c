/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aos/kernel.h"

#include "linkkit/coap_api.h"
#include "wrappers.h"
#include "infra_compat.h"

#include "at_util.h"

#define TAG "atcoap"

#define IOTX_PRE_DTLS_SERVER_URI \
    "coaps://pre.iot-as-coap.cn-shanghai.aliyuncs.com:5684"
#define IOTX_PRE_NOSEC_SERVER_URI \
    "coap://pre.iot-as-coap.cn-shanghai.aliyuncs.com:5683"
#define IOTX_ONLINE_DTLS_SERVER_URL \
    "coaps://%s.iot-as-coap.cn-shanghai.aliyuncs.com:5684"
#define IOTX_ONLINE_DTLS_SERVER_URL_NEW \
    "coaps://%s.coap.cn-shanghai.link.aliyuncs.com"

#define COAP_MSG_MAX_LEN            1024
#define COAP_PATH_MAX_LEN           200
#define COAP_DEFAULT_REQ_TIMOUT_SEC 20
#define COAP_PREFIX_MAX_LEN         80
#define COAP_DEFAULT_WAIT_TIME_MS   3000

#define MAX_ATCMD_COAP_MSG_RSP_LEN \
    (COAP_MSG_MAX_LEN + COAP_PATH_MAX_LEN + COAP_PREFIX_MAX_LEN)
static char coap_msg_rsp[MAX_ATCMD_COAP_MSG_RSP_LEN] = { 0 };

typedef struct
{
    int  timeout; // second
    char hostname[MAX_ATCMD_DOMAIN_LEN];
    int  port;
} coap_config_t;

typedef struct
{
    bool valid;
    int  method; // 1:GET 2:POST 3:PUT 4:DELETE

    int msg_format; // 0: ACII 1: Hex
    int fragment_id;

    char path[COAP_PATH_MAX_LEN];
    char msg[COAP_MSG_MAX_LEN];
} coap_sendreq_t;

typedef struct
{
    bool valid;
    int  packet_token;

    char msg[COAP_MSG_MAX_LEN];
    int  msg_len;

    int msg_format; // 0: ACII 1: Hex
    int fragment_id;
} coap_rcvrsp_t;

enum
{
    COAP_AUTH_FAIL    = 0,
    COAP_AUTH_SUCCESS = 1,
};


enum {
   COAP_SEND_SUCCESS = 0,
   COAP_SEND_TIMEOUT = 1,
   COAP_SEND_ERROR   = 2,
};

typedef struct {
    char                *p_auth_token;
    int                  auth_token_len;
    char                 is_authed;
    iotx_deviceinfo_t   *p_devinfo;
    void                *p_coap_ctx;
    unsigned int         coap_token;
    unsigned int         seq;
    unsigned char        key[32];
    iotx_event_handle_t  event_handle;
} iotx_coap_t;

static coap_config_t  m_coap_config;
static coap_sendreq_t m_last_send_req;
static coap_sendreq_t m_attempt_send_req;
static coap_sendreq_t m_frag_send_req;
static coap_sendreq_t m_last_send_req_noack;
static coap_sendreq_t m_attempt_send_req_noack;
static coap_sendreq_t m_frag_send_req_noack;
static coap_rcvrsp_t  m_last_recv_rsp;

static iotx_coap_context_t *p_context = NULL;
static bool m_yield_task_start =  false;
static aos_sem_t m_yield_sem;
static aos_mutex_t m_comm_mutex;
static bool m_auth_task_start = false;

/*
 *  Common const string
 */
#define MAX_INTEGER_STR_LEN 10
#define ONE_DIGIT_STR_LEN   1
const static char *PARA_TAG_STR  = "\"ParaTag\"";
const static char *PARA_VAL_STR  = "\"ParaValue\"";
const static char *SEPARATOR_STR = ",";
const static char *QUOTE_STR     = "\"";

#define RE_AUTH_THRESHOLD 5
static bool message_acked;
static int message_fail_count = 0;
static int re_auth_count = 0;

static int post_coap_authenticate_task();
static int atcmd_icoap_sendreqn();

static int coap_set_devinfo(iotx_deviceinfo_t *p_devinfo)
{
    if (NULL == p_devinfo) {
        return IOTX_ERR_INVALID_PARAM;
    }

    memset(p_devinfo, 0x00, sizeof(iotx_deviceinfo_t));

    HAL_GetProductKey(p_devinfo->product_key);
    HAL_GetDeviceName(p_devinfo->device_name);
    HAL_GetDeviceSecret(p_devinfo->device_secret);
    HAL_GetDeviceID(p_devinfo->device_id);

    return IOTX_SUCCESS;
}

static int notify_coap_rcvrsp(unsigned int token, int status)
{
    const char *coap_rcvpub_prefix = "+ICOAPSENDREQ:";
    int         ret;
    int         offset   = 0;
    char        response[COAP_PREFIX_MAX_LEN] = {0};

    // AT_RECV_PREFIX
    if (offset + strlen(AT_RECV_PREFIX) < COAP_PREFIX_MAX_LEN) {
        offset +=
          snprintf(response + offset, COAP_PREFIX_MAX_LEN - offset, "%s",
                   AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        goto err;
    }

    // prefix
    if (offset + strlen(coap_rcvpub_prefix) < COAP_PREFIX_MAX_LEN) {
        offset +=
          snprintf(response + offset, COAP_PREFIX_MAX_LEN - offset, "%s",
                   coap_rcvpub_prefix);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        goto err;
    }

    // PACKET_TOKEN + status
    if (offset + strlen(SEPARATOR_STR) + MAX_INTEGER_STR_LEN * 2  <
        COAP_PREFIX_MAX_LEN) {
            offset += snprintf(
              response + offset, COAP_PREFIX_MAX_LEN - offset,
              "%d%s%d", token, SEPARATOR_STR, status);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        goto err;
    }

    // AT_RECV_PREFIX
    if (offset + strlen(AT_RECV_PREFIX) < COAP_PREFIX_MAX_LEN) {
        offset +=
          snprintf(response + offset, COAP_PREFIX_MAX_LEN - offset, "%s",
                   AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        goto err;
    }

    if (insert_uart_send_msg(response, NULL, strlen(response), 0) != 0) {
        LOGE(TAG, "%s %d insert uart send msg fail\r\n", __func__, __LINE__);
        goto err;
    }

    return 0;

err:
    return -1;
}

static void coap_yield_task()
{
    if (m_yield_task_start) {
        LOGW(TAG, "Yield task already run!\n");
        return;
    }

    if (aos_sem_new(&m_yield_sem, 0) != 0) {
        LOGE(TAG, "failed to allocate semaphore");
        goto exit;
    }

    m_yield_task_start = true;
    LOGD(TAG, "New coap yield task starts\n");

    while (1) {
        if (aos_sem_is_valid(&m_yield_sem)){
            aos_sem_wait(&m_yield_sem, AOS_WAIT_FOREVER);
        } else {
            LOGE(TAG, "invalid yield sem!\n");
            goto exit;
        }

        if (!m_yield_task_start) {
            goto exit;
        }

        aos_mutex_lock(&m_comm_mutex, AOS_WAIT_FOREVER);
        IOT_CoAP_Yield(p_context);
        aos_mutex_unlock(&m_comm_mutex);
    }

exit:
    m_yield_task_start = false;

    if (aos_sem_is_valid(&m_yield_sem)) {
        aos_sem_free(&m_yield_sem);
    }

    LOGD(TAG, "Coap yield task exits\n");
}

static int post_coap_yield_task()
{
    if (aos_task_new("coap_yield_task", coap_yield_task, NULL, 2048) !=
        0) {
        LOGE(TAG, "Fail to create coap yield task\r\n");
        return -1;
    }

    return 0;
}

static void coap_recvrsp(void *arg, void *p_response)
{
    int                   len       = 0;
    unsigned char        *p_payload = NULL;
    int                   status;
    iotx_coap_resp_code_t resp_code;
    CoAPMessage          *message = NULL;
    unsigned int          token = 0;

    IOT_CoAP_GetMessageCode(p_response, &resp_code);
    IOT_CoAP_GetMessagePayload(p_response, &p_payload, &len);
    LOG("[APPL]: Message response code: %d len %d\r\n", resp_code, len);

    message = (CoAPMessage*) p_response;
    for (int i = 0; i < message->header.tokenlen; i++) {
       unsigned int val = message->token[i];
       token += (val << i * 8);
    }

    if (resp_code == IOTX_COAP_RESP_CODE_CONTENT) {
        message_acked = true;
        status = COAP_SEND_SUCCESS;
    } else {
        status = COAP_SEND_ERROR;
    }

    if (len > 0) {
        memset(&m_last_recv_rsp, 0, sizeof(m_last_recv_rsp));

        m_last_recv_rsp.packet_token = token;
        m_last_recv_rsp.msg_len = len;
        memcpy(m_last_recv_rsp.msg, p_payload, len);
        m_last_recv_rsp.valid = true;
    }

    if (p_payload) {
        LOG("[APPL]: Len: %d, Payload: %s, \r\n", len, p_payload);
    }

    notify_coap_rcvrsp(token, status);
}

static int coap_sendreq(coap_sendreq_t *sendreq, bool confirmreq)
{
    char              path[IOTX_URI_MAX_LEN + 1] = { 0 };
    iotx_message_t    message;
    iotx_deviceinfo_t devinfo;
    int               ret = 0;
    iotx_coap_t       *p_coap_ctx = NULL;
    unsigned int      token = 0;
    coap_sendreq_t    *sendreq_frag = NULL;

    if (NULL == sendreq || !sendreq->valid) {
        return -1;
    }

    message_acked = false;

    if (confirmreq)
        sendreq_frag = &m_frag_send_req;
    else
        sendreq_frag = &m_frag_send_req_noack;

    if (sendreq->fragment_id != 0 ||
        sendreq_frag->fragment_id != 0) {
        int offset;
        // first time
        if (sendreq_frag->fragment_id == 0) {
            memcpy(sendreq_frag, sendreq, sizeof(coap_sendreq_t));
        } else if (sendreq_frag->fragment_id - 1 != sendreq->fragment_id) {
            LOGE(TAG, "expect frag id %d but recv %d \n",
                    sendreq_frag->fragment_id - 1,
                    sendreq->fragment_id);
            goto err;
        }

        if (sendreq_frag->fragment_id - 1 == sendreq->fragment_id) {
            // compare format
            if (memcmp(sendreq_frag->path, sendreq->path,
                strlen(sendreq_frag->path)) != 0) {
                LOGE(TAG, "unmatched frag path %s\n", sendreq->path);
                goto err;
            }

            if (sendreq_frag->method != sendreq->method ||
                sendreq_frag->msg_format != sendreq->msg_format) {
                LOGE(TAG, "unmatched frag format %d\n",
                        sendreq->msg_format);
                goto err;
            }

            offset = strlen(sendreq_frag->msg);
            if (offset + strlen(sendreq->msg) >= sizeof(sendreq_frag->msg)) {
                LOGE(TAG, "frag message exceeds max len %d\n", sizeof(sendreq_frag->msg));
                goto err;
            }

            memcpy(sendreq_frag->msg + offset, sendreq->msg, strlen(sendreq->msg));
            sendreq_frag->fragment_id = sendreq->fragment_id;
        }
    } else {
        memcpy(sendreq_frag, sendreq, sizeof(coap_sendreq_t));
    }

    // more fragment
    if (sendreq_frag->fragment_id != 0) {
        return 0;
    }

    memset(&message, 0, sizeof(message));
    message.p_payload     = sendreq_frag->msg;
    message.payload_len   = strlen(sendreq_frag->msg);
    if (confirmreq) {
        message.resp_callback = coap_recvrsp;
        message.msg_type = IOTX_MESSAGE_CON;
    }
    else {
        message.resp_callback = NULL;
        message.msg_type = IOTX_MESSAGE_NON;
    }
    message.content_type = IOTX_CONTENT_TYPE_JSON;

    LOG("send msg: Len: %d, Payload: %s, path %s \r\n", message.payload_len,
        message.p_payload, sendreq_frag->path);

    if (p_context) {
        p_coap_ctx = (iotx_coap_t*) p_context;
        token = p_coap_ctx->coap_token;
    }

    ret = IOT_CoAP_SendMessage(p_context, sendreq_frag->path, &message);
    if (ret >= 0) {
        if (confirmreq) {
            memcpy(&m_last_send_req, sendreq_frag, sizeof(coap_sendreq_t));
        } else {
            memcpy(&m_last_send_req_noack, sendreq_frag, sizeof(coap_sendreq_t));
        }

#if 0
        if (aos_sem_is_valid(&m_yield_sem)) {
              aos_sem_signal(&m_yield_sem);
        }
#endif
        IOT_CoAP_Yield(p_context);

        ret = token;

        if (confirmreq) {
            if (message_acked) {
                message_fail_count = 0;
            } else {
                message_fail_count++;
                if (message_fail_count >= RE_AUTH_THRESHOLD) {
                    post_coap_authenticate_task();
                    message_fail_count = 0;
                    re_auth_count++;
                }
            }
        }

        LOGD(TAG, "reauth:%d ack fail:%d acked:%d\n", re_auth_count, message_fail_count, message_acked);
    }

    memset(sendreq_frag, 0, sizeof(coap_sendreq_t));
    return ret;

err:
   memset(sendreq_frag, 0, sizeof(coap_sendreq_t));
   return -1;
}

static void coap_context_init()
{
    iotx_coap_config_t config;
    iotx_deviceinfo_t  deviceinfo;
    int attempt = 0;

    coap_set_devinfo(&deviceinfo);
    memset(&config, 0, sizeof(iotx_coap_config_t));
#ifdef COAP_ONLINE
#ifdef COAP_DTLS_SUPPORT
    char url[256] = { 0 };
    snprintf(url, sizeof(url), IOTX_ONLINE_DTLS_SERVER_URL,
             deviceinfo.product_key);
    config.p_url = url;
#else
    LOG("Online environment must access with DTLS\r\n");
    return -1;
#endif
#else
#ifdef COAP_DTLS_SUPPORT
    config.p_url = IOTX_PRE_DTLS_SERVER_URI;
#else
    config.p_url = IOTX_PRE_NOSEC_SERVER_URI;
#endif
#endif
    config.wait_time_ms = COAP_DEFAULT_WAIT_TIME_MS;
    config.p_devinfo = &deviceinfo;

    if (p_context) {
        IOT_CoAP_Deinit(&p_context);
    }

    do {
        if ((p_context = IOT_CoAP_Init(&config)) != NULL)
            break;
        attempt++;

        aos_msleep(200);
    } while(attempt <= 5);
}

static int notify_coap_auth_result(int result)
{
    const char *auth_result_preifx = "+ICOAPAUTH:";
    char       *auth_result;
    int         offset   = 0;
    char       *response = coap_msg_rsp;

    memset(response, 0, MAX_ATCMD_COAP_MSG_RSP_LEN);

    if (result == COAP_AUTH_FAIL) {
        auth_result = "FAIL";
    } else if (result == COAP_AUTH_SUCCESS) {
        auth_result = "OK";
    } else {
        LOGE(TAG, "invalid auth result %d\n", result);
        goto err;
    }

    // prefix
    if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_COAP_MSG_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_COAP_MSG_RSP_LEN - offset, "%s",
                   AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "at string too long %s\n", response);
        goto err;
    }

    // result
    if (offset + strlen(auth_result_preifx) + strlen(auth_result) + 2 <
        MAX_ATCMD_COAP_MSG_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_COAP_MSG_RSP_LEN - offset,
                   "%s%s\r\n", auth_result_preifx, auth_result);
    } else {
        LOGE(TAG, "at string too long %s\n", response);
        goto err;
    }

    if (insert_uart_send_msg(response, NULL, strlen(response), 0) != 0) {
        LOGE(TAG, "Error insert uart send msg fail\r\n");
        goto err;
    }

    return 0;
err:
    return -1;
}

static int coap_authenticate()
{
    int ret    = 0;
    int notify = COAP_AUTH_FAIL;
    long long current_ms;
    iotx_coap_t  *p_coap_ctx = NULL;

    m_auth_task_start = true;
    aos_mutex_lock(&m_comm_mutex, AOS_WAIT_FOREVER);
    coap_context_init();

    if (IOT_CoAP_DeviceNameAuth(p_context) != 0) {
        LOGE(TAG, "authenticate request failed!");
        ret = -1;
        goto err;
    }

    p_coap_ctx = (iotx_coap_t*) p_context;
    current_ms = aos_now_ms();
    do {
        if (p_coap_ctx->is_authed) {
            notify = COAP_AUTH_SUCCESS;
            LOGD(TAG, "COAP AUTH SUCCESS\n");
            break;
        }
        aos_msleep(200);
    } while (aos_now_ms() - current_ms < COAP_DEFAULT_WAIT_TIME_MS);

err:
    aos_mutex_unlock(&m_comm_mutex);
    notify_coap_auth_result(notify);
    m_auth_task_start = false;
    return ret;
}

const static char *TAG_TIMEOUT_STR = "\"TIMEOUT\"";
static int         atcmd_icoap_para()
{
    const char *coap_para_prefix = "+ICOAPARA:";
    char        single;
    char        body[20] = { 0 };
    int         timeout;
    int         error_no = CME_ERROR_UNKNOWN;
    int         ret;
    int         offset   = 0;
    char       *response = coap_msg_rsp;

    memset(response, 0, MAX_ATCMD_COAP_MSG_RSP_LEN);

    atcmd_read(&single, 1);
    if ('=' != single && '?' != single) {
        LOGE(TAG, "Invalid coap para prefix %c!\n", single);
        goto err;
    }

    // AT_RECV_PREFIX
    if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_COAP_MSG_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_COAP_MSG_RSP_LEN - offset, "%s",
                   AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        goto err;
    }

    if ('?' == single) {
        // COAPARA + TIMEOUT + value
        if (offset + strlen(coap_para_prefix) + strlen(TAG_TIMEOUT_STR) +
              strlen(SEPARATOR_STR) + MAX_INTEGER_STR_LEN <
            MAX_ATCMD_COAP_MSG_RSP_LEN) {
            offset +=
              snprintf(response + offset, MAX_ATCMD_COAP_MSG_RSP_LEN - offset,
                       "%s%s%s%d", coap_para_prefix, TAG_TIMEOUT_STR,
                       SEPARATOR_STR, m_coap_config.timeout);
        } else {
            LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                 response);
            error_no = CME_ERROR_TXT_STR_TOO_LONG;
            goto err;
        }

        // AT_RECV_PREFIX
        if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_COAP_MSG_RSP_LEN) {
            offset +=
              snprintf(response + offset, MAX_ATCMD_COAP_MSG_RSP_LEN - offset,
                       "%s", AT_RECV_PREFIX);
        } else {
            LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                 response);
            error_no = CME_ERROR_TXT_STR_TOO_LONG;
            goto err;
        }
    } else if ('=' == single) {
        atcmd_read(&single, 1);

        if ('?' == single) {
            // COAPARA + \"ParaTag\" + \"ParaValue\"
            if (offset + strlen(coap_para_prefix) + strlen(PARA_TAG_STR) +
                  strlen(SEPARATOR_STR) + strlen(PARA_VAL_STR) <
                MAX_ATCMD_COAP_MSG_RSP_LEN) {
                offset += snprintf(response + offset,
                                   MAX_ATCMD_COAP_MSG_RSP_LEN - offset,
                                   "%s%s%s%s", coap_para_prefix, PARA_TAG_STR,
                                   SEPARATOR_STR, PARA_VAL_STR);
            } else {
                LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                     response);
                error_no = CME_ERROR_TXT_STR_TOO_LONG;
                goto err;
            }

            if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_COAP_MSG_RSP_LEN) {
                offset += snprintf(response + offset,
                                   MAX_ATCMD_COAP_MSG_RSP_LEN - offset, "%s",
                                   AT_RECV_PREFIX);
            } else {
                LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                     response);
                error_no = CME_ERROR_TXT_STR_TOO_LONG;
                goto err;
            }
        } else {
            atcmd_read(body, strlen(TAG_TIMEOUT_STR));
            if (memcmp(body, "TIMEOUT\",", strlen(TAG_TIMEOUT_STR)) != 0) {
                LOGE(TAG, "Invalid para tag %s !\n", body);
                error_no = CME_ERROR_INCORRECT_PARA;
                goto err;
            }

            memset(body, 0, sizeof(body));
            ret = atcmd_socket_data_info_get(body, sizeof(body),
                                             &atcmd_socket_data_len_check);
            if (ret <= 0 || (timeout = atoi(body)) <= 0) {
                LOGE(TAG, "Invalid timeout %s !\n", body);
                error_no = CME_ERROR_INCORRECT_PARA;
                goto err;
            }

            m_coap_config.timeout = timeout;
        }
    } else {
        LOGE(TAG, "Invalid coap para prefix %c!\n", single);
        error_no = CME_ERROR_TXT_STR_INVALID_CHAR;
        goto err;
    }

    // AT_RECV_SUCCESS_POSTFIX
    if (offset + strlen(AT_RECV_SUCCESS_POSTFIX) < MAX_ATCMD_COAP_MSG_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_COAP_MSG_RSP_LEN - offset, "%s",
                   AT_RECV_SUCCESS_POSTFIX);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        error_no = CME_ERROR_TXT_STR_TOO_LONG;
        goto err;
    }

    if (insert_uart_send_msg(response, NULL, strlen(response), 0) != 0) {
        LOGE(TAG, "%s %d insert uart send msg fail\r\n", __func__, __LINE__);
        goto err;
    }

    return 0;

err:
    notify_atcmd_cme_error(error_no);

    return -1;
}

const static char *TAG_HOSTNAME_STR = "\"hostname\"";
const static char *TAG_PORT_STR     = "\"port\"";

static int atcmd_icoap_open()
{
    const char *coap_open_prefix = "+ICOAPOPEN:";
    char        single;
    char        hostname[MAX_ATCMD_DOMAIN_LEN] = { 0 };
    int         port;
    char        port_str[MAX_ATCMD_PORT_LEN] = { 0 };
    int         ret;
    int         offset   = 0;
    char       *response = coap_msg_rsp;

    memset(response, 0, MAX_ATCMD_COAP_MSG_RSP_LEN);

    atcmd_read(&single, 1);
    if ('=' != single && '?' != single) {
        LOGE(TAG, "Invalid coap open prefix %c!\n", single);
        goto err;
    }

    // AT_RECV_PREFIX
    if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_COAP_MSG_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_COAP_MSG_RSP_LEN - offset, "%s",
                   AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        goto err;
    }

    if ('?' == single) {
        // COAPOPEN prefix + hostname + port
        if (offset + strlen(coap_open_prefix) + strlen(QUOTE_STR) * 2 +
              MAX_ATCMD_DOMAIN_LEN + strlen(SEPARATOR_STR) +
              MAX_ATCMD_PORT_LEN <
            MAX_ATCMD_COAP_MSG_RSP_LEN) {
            offset +=
              snprintf(response + offset, MAX_ATCMD_COAP_MSG_RSP_LEN - offset,
                       "%s\"%s\"%s%d", coap_open_prefix, m_coap_config.hostname,
                       SEPARATOR_STR, m_coap_config.port);
        } else {
            LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                 response);
            goto err;
        }

        // AT_RECV_PREFIX
        if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_COAP_MSG_RSP_LEN) {
            offset +=
              snprintf(response + offset, MAX_ATCMD_COAP_MSG_RSP_LEN - offset,
                       "%s", AT_RECV_PREFIX);
        } else {
            LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                 response);
            goto err;
        }
    } else if ('=' == single) {
        atcmd_read(&single, 1);

        if ('?' == single) {
            // COAPOPEN prefix + \"hostname\" + \"port\"
            if (offset + strlen(coap_open_prefix) + strlen(TAG_HOSTNAME_STR) +
                  strlen(SEPARATOR_STR) + strlen(TAG_PORT_STR) <
                MAX_ATCMD_COAP_MSG_RSP_LEN) {
                offset += snprintf(
                  response + offset, MAX_ATCMD_COAP_MSG_RSP_LEN - offset,
                  "%s%s%s%s", coap_open_prefix, TAG_HOSTNAME_STR, SEPARATOR_STR,
                  TAG_PORT_STR);
            } else {
                LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                     response);
                goto err;
            }

            if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_COAP_MSG_RSP_LEN) {
                offset += snprintf(response + offset,
                                   MAX_ATCMD_COAP_MSG_RSP_LEN - offset, "%s",
                                   AT_RECV_PREFIX);
            } else {
                LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                     response);
                goto err;
            }
        } else {
            ret = atcmd_socket_data_info_get(hostname, sizeof(hostname), NULL);
            if (ret < 0) {
                LOGE(TAG, "Invalid hostname %s !\n", hostname);
                goto err;
            }

            // ignore "
            if (strlen(hostname) > 0 &&
                hostname[strlen(hostname) - 1] == '\"') {
                hostname[strlen(hostname) - 1] = '\0';
            } else {
                LOGE(TAG, "Invalid hostname %s !\n", hostname);
                goto err;
            }

            ret = atcmd_socket_data_info_get(port_str, sizeof(port_str),
                                             &atcmd_socket_data_len_check);
            if (ret < 0 || (port = atoi(port_str)) < 0) {
                LOGE(TAG, "Invalid port %s !\n", port_str);
                goto err;
            }

#if 1
            LOGW(TAG, "icoap open is currently not supported!");
            goto err;
#endif

            // set hostname, port
            memset(m_coap_config.hostname, 0, sizeof(m_coap_config.hostname));
            memcpy(m_coap_config.hostname, hostname, strlen(hostname));
            m_coap_config.port = port;
        }
    } else {
        LOGE(TAG, "Invalid coap mode prefix %c!\n", single);
        goto err;
    }

    // AT_RECV_SUCCESS_POSTFIX
    if (offset + strlen(AT_RECV_SUCCESS_POSTFIX) < MAX_ATCMD_COAP_MSG_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_COAP_MSG_RSP_LEN - offset, "%s",
                   AT_RECV_SUCCESS_POSTFIX);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        goto err;
    }

    if (insert_uart_send_msg(response, NULL, strlen(response), 0) != 0) {
        LOGE(TAG, "%s %d insert uart send msg fail\r\n", __func__, __LINE__);
        goto err;
    }

    return 0;

err:
    return -1;
}

static int post_coap_authenticate_task()
{
    if (aos_task_new("coap_authenticate_task", coap_authenticate, NULL, 5120) !=
        0) {
        LOGE(TAG, "Fail to create coap authenticate task\r\n");
        return -1;
    }

    return 0;
}

const static char *TAG_PRODUCT_KEY_STR   = "\"PRODUCT_KEY\"";
const static char *TAG_DEVICE_NAME_STR   = "\"DEVICE_NAME\"";
const static char *TAG_DEVICE_SECRET_STR = "\"DEVICE_SECRET\"";

static int atcmd_icoap_auth()
{
    const char *coap_auth_prefix = "+ICOAPAUTH:";
    char        single;
    char        productkey[PRODUCT_KEY_MAXLEN + 2]     = { 0 };
    char        devicename[DEVICE_NAME_MAXLEN + 2]     = { 0 };
    char        devicesecret[DEVICE_SECRET_MAXLEN + 2] = { 0 };
    int         ret;
    int         error_no = CME_ERROR_UNKNOWN;
    int         offset   = 0;
    char       *response = coap_msg_rsp;

    memset(response, 0, MAX_ATCMD_COAP_MSG_RSP_LEN);

    atcmd_read(&single, 1);
    if ('=' != single && '?' != single) {
        LOGE(TAG, "Invalid coap auth prefix %c!\n", single);
        error_no = CME_ERROR_TXT_STR_INVALID_CHAR;
        goto err;
    }

    // AT_RECV_PREFIX
    if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_COAP_MSG_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_COAP_MSG_RSP_LEN - offset, "%s",
                   AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        error_no = CME_ERROR_TXT_STR_TOO_LONG;
        goto err;
    }

    if ('?' == single) {
        // COAPAUTH prefix + productkey + productkey + devicesecret
        if (offset + strlen(coap_auth_prefix) + strlen(SEPARATOR_STR) * 2 +
              PRODUCT_KEY_MAXLEN + strlen(QUOTE_STR) * 6 +
              DEVICE_NAME_MAXLEN + DEVICE_SECRET_MAXLEN  <
            MAX_ATCMD_COAP_MSG_RSP_LEN) {
            HAL_GetProductKey(productkey);
            HAL_GetDeviceName(devicename);
            HAL_GetDeviceSecret(devicesecret);

            offset += snprintf(
              response + offset, MAX_ATCMD_COAP_MSG_RSP_LEN - offset,
              "%s\"%s\"%s\"%s\"%s\"%s\"", coap_auth_prefix,
              productkey, SEPARATOR_STR, devicename,
              SEPARATOR_STR, devicesecret);
        } else {
            LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                 response);
            error_no = CME_ERROR_TXT_STR_TOO_LONG;
            goto err;
        }

        // AT_RECV_PREFIX
        if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_COAP_MSG_RSP_LEN) {
            offset +=
              snprintf(response + offset, MAX_ATCMD_COAP_MSG_RSP_LEN - offset,
                       "%s", AT_RECV_PREFIX);
        } else {
            LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                 response);
            error_no = CME_ERROR_TXT_STR_TOO_LONG;
            goto err;
        }
    } else if ('=' == single) {
        atcmd_read(&single, 1);

        if ('?' == single) {
            // COAPAUTH prefix + \"PRODUCT_KEY\" + \"DEVICE_NAME\" +
            // \"DEVICE_SECRET\"
            if (offset + strlen(coap_auth_prefix) +
                  strlen(TAG_PRODUCT_KEY_STR) + strlen(TAG_DEVICE_NAME_STR) +
                  strlen(SEPARATOR_STR) * 2 + strlen(TAG_DEVICE_SECRET_STR) <
                MAX_ATCMD_COAP_MSG_RSP_LEN) {
                offset += snprintf(
                  response + offset, MAX_ATCMD_COAP_MSG_RSP_LEN - offset,
                  "%s%s%s%s%s%s", coap_auth_prefix, TAG_PRODUCT_KEY_STR,
                  SEPARATOR_STR, TAG_DEVICE_NAME_STR, SEPARATOR_STR,
                  TAG_DEVICE_SECRET_STR);
            } else {
                LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                     response);
                error_no = CME_ERROR_TXT_STR_TOO_LONG;
                goto err;
            }

            if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_COAP_MSG_RSP_LEN) {
                offset += snprintf(response + offset,
                                   MAX_ATCMD_COAP_MSG_RSP_LEN - offset, "%s",
                                   AT_RECV_PREFIX);
            } else {
                LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                     response);
                error_no = CME_ERROR_TXT_STR_TOO_LONG;
                goto err;
            }
        } else {
            if (m_auth_task_start) {
                LOGE(TAG, "auth in process!\n");
                error_no = CME_ERROR_OP_NOT_ALLOWED;
                goto err;
            }

            ret =
              atcmd_socket_data_info_get(productkey, sizeof(productkey), NULL);
            if (ret < 0) {
                LOGE(TAG, "Invalid productkey %s !\n", productkey);
                error_no = CME_ERROR_INCORRECT_PARA;
                goto err;
            }

            // ignore \"
            if (strlen(productkey) > 0 &&
                productkey[strlen(productkey) - 1] == '\"') {
                productkey[strlen(productkey) - 1] = '\0';
            } else {
                LOGE(TAG, "Invalid productkey %s !\n", productkey);
                error_no = CME_ERROR_INCORRECT_PARA;
                goto err;
            }

            // eat \"
            atcmd_read(&single, 1);
            ret =
              atcmd_socket_data_info_get(devicename, sizeof(devicename), NULL);
            if (ret < 0) {
                LOGE(TAG, "Invalid devicename %s !\n", devicename);
                error_no = CME_ERROR_INCORRECT_PARA;
                goto err;
            }

            // ignore \"
            if (strlen(devicename) > 0 &&
                devicename[strlen(devicename) - 1] == '\"') {
                devicename[strlen(devicename) - 1] = '\0';
            } else {
                LOGE(TAG, "Invalid devicename %s !\n", devicename);
                error_no = CME_ERROR_INCORRECT_PARA;
                goto err;
            }

            // eat \"
            atcmd_read(&single, 1);
            ret = atcmd_socket_data_info_get(devicesecret, sizeof(devicesecret),
                                             NULL);
            if (ret <= 0) {
                LOGE(TAG, "Invalid devicesecret %s !\n", devicesecret);
                error_no = CME_ERROR_INCORRECT_PARA;
                goto err;
            }

            // ignore \"
            if (strlen(devicesecret) > 0 &&
                devicesecret[strlen(devicesecret) - 1] == '\"') {
                devicesecret[strlen(devicesecret) - 1] = '\0';
            } else {
                LOGE(TAG, "Invalid devicename %s !\n", devicesecret);
                error_no = CME_ERROR_INCORRECT_PARA;
                goto err;
            }

            HAL_SetProductKey(productkey);
            HAL_SetDeviceName(devicename);
            HAL_SetDeviceSecret(devicesecret);

            // trigger auth
            post_coap_authenticate_task();
        }
    } else {
        LOGE(TAG, "Invalid coap auth prefix %c!\n", single);
        error_no = CME_ERROR_TXT_STR_INVALID_CHAR;
        goto err;
    }

    // AT_RECV_SUCCESS_POSTFIX
    if (offset + strlen(AT_RECV_SUCCESS_POSTFIX) < MAX_ATCMD_COAP_MSG_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_COAP_MSG_RSP_LEN - offset, "%s",
                   AT_RECV_SUCCESS_POSTFIX);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        error_no = CME_ERROR_TXT_STR_TOO_LONG;
        goto err;
    }

    if (insert_uart_send_msg(response, NULL, strlen(response), 0) != 0) {
        LOGE(TAG, "%s %d insert uart send msg fail\r\n", __func__, __LINE__);
        goto err;
    }

    return 0;

err:
    notify_atcmd_cme_error(error_no);

    return -1;
}

static const char *TAG_METHOD_STR  = "\"method\"";
static const char *TAG_PATH_STR    = "\"path\"";
static const char *TAG_MESSAGE_STR = "\"message\"";
static const char *TAG_FORMAT_STR  = "\"format\"";
static const char *TAG_FRAG_ID_STR = "\"fragment_id\"";

static int atcmd_icoap_sendreq()
{
    const char *coap_sendreq_prefix = "+ICOAPSENDERQ:";
    char        single;
    char        body[20] = { 0 };
    int         ret;
    int         error_no = CME_ERROR_UNKNOWN;
    int         offset   = 0;
    char        response[MAX_ATCMD_COAP_MSG_RSP_LEN] = {0};

    atcmd_read(&single, 1);
    if ('N' == single) {
        return atcmd_icoap_sendreqn();
    } else if ('=' != single && '?' != single) {
        LOGE(TAG, "Invalid coap sendreq prefix %c!\n", single);
        error_no = CME_ERROR_TXT_STR_INVALID_CHAR;
        goto err;
    }

    // AT_RECV_PREFIX
    if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_COAP_MSG_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_COAP_MSG_RSP_LEN - offset, "%s",
                   AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        error_no = CME_ERROR_TXT_STR_TOO_LONG;
        goto err;
    }

    if ('?' == single) {
        // prefix
        if (offset + strlen(coap_sendreq_prefix) < MAX_ATCMD_COAP_MSG_RSP_LEN) {
            offset +=
              snprintf(response + offset, MAX_ATCMD_COAP_MSG_RSP_LEN - offset, "%s",
                       coap_sendreq_prefix);
        } else {
            LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                 response);
            error_no = CME_ERROR_TXT_STR_TOO_LONG;
            goto err;
        }

        // METHOD + PATH + MESSAGE + [FORMAT] + [FRAGMENT_ID]
        if (offset + ONE_DIGIT_STR_LEN + strlen(m_last_send_req.path) +
              strlen(QUOTE_STR) * 4 + strlen(SEPARATOR_STR) * 4 +
              strlen(m_last_send_req.msg) + ONE_DIGIT_STR_LEN * 2 +
              MAX_INTEGER_STR_LEN <
            MAX_ATCMD_COAP_MSG_RSP_LEN) {
            if (m_last_send_req.valid)
                offset += snprintf(
                  response + offset, MAX_ATCMD_COAP_MSG_RSP_LEN - offset,
                  "%d%s\"%s\"%s\"%s\"", m_last_send_req.method,
                  SEPARATOR_STR, m_last_send_req.path, SEPARATOR_STR,
                  m_last_send_req.msg);
            else
                offset += snprintf(response + offset,
                                   MAX_ATCMD_COAP_MSG_RSP_LEN - offset, ",,,,");
        } else {
            LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                 response);
            error_no = CME_ERROR_TXT_STR_TOO_LONG;
            goto err;
        }
    } else if ('=' == single) {
        atcmd_read(&single, 1);

        if ('?' == single) {
            // prefix
            if (offset + strlen(coap_sendreq_prefix) < MAX_ATCMD_COAP_MSG_RSP_LEN) {
                offset +=
                  snprintf(response + offset, MAX_ATCMD_COAP_MSG_RSP_LEN - offset, "%s",
                           coap_sendreq_prefix);
            } else {
                LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                     response);
                error_no = CME_ERROR_TXT_STR_TOO_LONG;
                goto err;
            }

            // \"method\" + \"path\" + \"message\"
            if (offset + strlen(TAG_METHOD_STR) + strlen(TAG_PATH_STR) +
                  strlen(TAG_MESSAGE_STR) + strlen(TAG_FORMAT_STR) +
                  strlen(TAG_FRAG_ID_STR) + strlen(SEPARATOR_STR) * 4 <
                MAX_ATCMD_COAP_MSG_RSP_LEN) {
                offset += snprintf(
                  response + offset, MAX_ATCMD_COAP_MSG_RSP_LEN - offset,
                  "%s%s%s%s%s", TAG_METHOD_STR, SEPARATOR_STR,
                  TAG_PATH_STR, SEPARATOR_STR, TAG_MESSAGE_STR);
            } else {
                LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                     response);
                error_no = CME_ERROR_TXT_STR_TOO_LONG;
                goto err;
            }
        } else {
            if (m_auth_task_start) {
                LOGE(TAG, "auth in process!\n");
                error_no = CME_ERROR_OP_NOT_ALLOWED;
                goto err;
            }

            memset(&m_attempt_send_req, 0, sizeof(m_attempt_send_req));

            body[0] = single;
            // eat ,
            atcmd_read(&single, 1);
            if (memcmp(&single, SEPARATOR_STR, 1) != 0) {
                LOGE(TAG, "Invalid method %c%c!\n", body[0], single);
                error_no = CME_ERROR_INCORRECT_PARA;
                goto err;
            }

            m_attempt_send_req.method = atoi(body);
            if (m_attempt_send_req.method <= 0 ||
                m_attempt_send_req.method > 4) {
                LOGE(TAG, "Invalid method %d !\n", m_attempt_send_req.method);
                error_no = CME_ERROR_INCORRECT_PARA;
                goto err;
            }

            // eat \"
            atcmd_read(&single, 1);
            ret = atcmd_socket_data_info_get(
              m_attempt_send_req.path, sizeof(m_attempt_send_req.path), NULL);
            if (ret < 0) {
                LOGE(TAG, "Invalid path %s !\n", m_attempt_send_req.path);
                error_no = CME_ERROR_INCORRECT_PARA;
                goto err;
            }

            // ignore "
            if (strlen(m_attempt_send_req.path) > 0 &&
                m_attempt_send_req.path[strlen(m_attempt_send_req.path) - 1] ==
                  '\"') {
                m_attempt_send_req.path[strlen(m_attempt_send_req.path) - 1] =
                  '\0';
            } else {
                LOGE(TAG, "Invalid path %s !\n", m_attempt_send_req.path);
                error_no = CME_ERROR_INCORRECT_PARA;
                goto err;
            }

            // eat \"
            atcmd_read(&single, 1);
            ret = atcmd_socket_data_info_get(
              m_attempt_send_req.msg, sizeof(m_attempt_send_req.msg), NULL);
            if (ret < 0) {
                LOGE(TAG, "Invalid message %s !\n", m_attempt_send_req.msg);
                error_no = CME_ERROR_INCORRECT_PARA;
                goto err;
            }

            // ignore "
            if (strlen(m_attempt_send_req.msg) > 0 &&
                m_attempt_send_req.msg[strlen(m_attempt_send_req.msg) - 1] ==
                  '\"') {
                m_attempt_send_req.msg[strlen(m_attempt_send_req.msg) - 1] =
                  '\0';
            } else {
                LOGE(TAG, "Invalid msg %s !\n", m_attempt_send_req.msg);
                error_no = CME_ERROR_INCORRECT_PARA;
                goto err;
            }

            if (ret == 0) {
                memset(body, 0, sizeof(body));
                ret = atcmd_socket_data_info_get(body, sizeof(body),
                                                 &atcmd_socket_data_len_check);
                if (ret < 0 || (m_attempt_send_req.msg_format = atoi(body)) < 0 ||
                    m_attempt_send_req.msg_format > 1) {
                    LOGE(TAG, "Invalid format %s !\n", body);
                    error_no = CME_ERROR_INCORRECT_PARA;
                    goto err;
                }
            }

            if (ret == 0) {
                memset(body, 0, sizeof(body));
                ret = atcmd_socket_data_info_get(body, sizeof(body),
                                                 &atcmd_socket_data_len_check);
                if (ret <= 0 || (m_attempt_send_req.fragment_id = atoi(body)) < 0) {
                    LOGE(TAG, "Invalid frag_id %s !\n", body);
                    error_no = CME_ERROR_INCORRECT_PARA;
                    goto err;
                }
            }

            m_attempt_send_req.valid = true;
            ret                      = coap_sendreq(&m_attempt_send_req, true);
            if (ret < 0) {
                if (ret == IOTX_ERR_MSG_TOO_LOOG ||
                    ret == IOTX_ERR_URI_TOO_LOOG)
                    error_no = CME_ERROR_TXT_STR_TOO_LONG;
                else if (ret == IOTX_ERR_INVALID_PARAM)
                    error_no = CME_ERROR_INCORRECT_PARA;
                else {
                    LOGE(TAG, "coap sendreq unknown err! ret %d\n", ret);
                }

                goto err;
            }  else if (m_attempt_send_req.fragment_id == 0) {
                 // prefix
                if (offset + strlen(coap_sendreq_prefix) < MAX_ATCMD_COAP_MSG_RSP_LEN) {
                    offset +=
                      snprintf(response + offset, MAX_ATCMD_COAP_MSG_RSP_LEN - offset, "%s",
                               coap_sendreq_prefix);
                } else {
                    LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                         response);
                    error_no = CME_ERROR_TXT_STR_TOO_LONG;
                    goto err;
                }

                // token
                if (offset + MAX_INTEGER_STR_LEN < MAX_ATCMD_COAP_MSG_RSP_LEN) {
                    offset +=
                      snprintf(response + offset,
                               MAX_ATCMD_COAP_MSG_RSP_LEN - offset, "%d", ret);
                } else {
                    LOGE(TAG, "%s %d at string too long %s\n", __func__,
                         __LINE__, response);
                    error_no = CME_ERROR_TXT_STR_TOO_LONG;
                    goto err;
                }
            }
        }
    } else {
        LOGE(TAG, "Invalid coap sendreq prefix %c!\n", single);
        error_no = CME_ERROR_TXT_STR_INVALID_CHAR;
        goto err;
    }

    // AT_RECV_PREFIX
    if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_COAP_MSG_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_COAP_MSG_RSP_LEN - offset, "%s",
                   AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        error_no = CME_ERROR_TXT_STR_TOO_LONG;
        goto err;
    }

    // AT_RECV_SUCCESS_POSTFIX
    if (offset + strlen(AT_RECV_SUCCESS_POSTFIX) < MAX_ATCMD_COAP_MSG_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_COAP_MSG_RSP_LEN - offset, "%s",
                   AT_RECV_SUCCESS_POSTFIX);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        error_no = CME_ERROR_TXT_STR_TOO_LONG;
        goto err;
    }

    if (insert_uart_send_msg(response, NULL, strlen(response), 0) != 0) {
        LOGE(TAG, "%s %d insert uart send msg fail\r\n", __func__, __LINE__);
        goto err;
    }

    return 0;

err:
    notify_atcmd_cme_error(error_no);

    return -1;
}

static int atcmd_icoap_sendreqn()
{
    const char *coap_sendreqn_prefix = "+ICOAPSENDERQN:";
    char        single;
    char        body[20] = { 0 };
    int         ret;
    int         error_no = CME_ERROR_UNKNOWN;
    int         offset   = 0;
    char       *response = coap_msg_rsp;

    memset(response, 0, MAX_ATCMD_COAP_MSG_RSP_LEN);

    atcmd_read(&single, 1);
    if ('=' != single && '?' != single) {
        LOGE(TAG, "Invalid coap sendreqn prefix %c!\n", single);
        error_no = CME_ERROR_TXT_STR_INVALID_CHAR;
        goto err;
    }

    // AT_RECV_PREFIX
    if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_COAP_MSG_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_COAP_MSG_RSP_LEN - offset, "%s",
                   AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        error_no = CME_ERROR_TXT_STR_TOO_LONG;
        goto err;
    }

    if ('?' == single) {
        // prefix
        if (offset + strlen(coap_sendreqn_prefix) < MAX_ATCMD_COAP_MSG_RSP_LEN) {
            offset +=
              snprintf(response + offset, MAX_ATCMD_COAP_MSG_RSP_LEN - offset, "%s",
                       coap_sendreqn_prefix);
        } else {
            LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                 response);
            error_no = CME_ERROR_TXT_STR_TOO_LONG;
            goto err;
        }

        // METHOD + PATH + MESSAGE + [FORMAT] + [FRAGMENT_ID]
        if (offset + ONE_DIGIT_STR_LEN + strlen(m_last_send_req_noack.path) +
              strlen(QUOTE_STR) * 4 + strlen(SEPARATOR_STR) * 4 +
              strlen(m_last_send_req_noack.msg) + ONE_DIGIT_STR_LEN * 2 +
              MAX_INTEGER_STR_LEN <
            MAX_ATCMD_COAP_MSG_RSP_LEN) {
            if (m_last_send_req_noack.valid)
                offset += snprintf(
                  response + offset, MAX_ATCMD_COAP_MSG_RSP_LEN - offset,
                  "%d%s\"%s\"%s\"%s\"", m_last_send_req_noack.method,
                  SEPARATOR_STR, m_last_send_req_noack.path, SEPARATOR_STR,
                  m_last_send_req_noack.msg);
            else
                offset += snprintf(response + offset,
                                   MAX_ATCMD_COAP_MSG_RSP_LEN - offset, ",,,,");
        } else {
            LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                 response);
            error_no = CME_ERROR_TXT_STR_TOO_LONG;
            goto err;
        }
    } else if ('=' == single) {
        atcmd_read(&single, 1);

        if ('?' == single) {
            // prefix
            if (offset + strlen(coap_sendreqn_prefix) < MAX_ATCMD_COAP_MSG_RSP_LEN) {
                offset +=
                  snprintf(response + offset, MAX_ATCMD_COAP_MSG_RSP_LEN - offset, "%s",
                           coap_sendreqn_prefix);
            } else {
                LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                     response);
                error_no = CME_ERROR_TXT_STR_TOO_LONG;
                goto err;
            }

            // \"method\" + \"path\" + \"message\" + \"format\" +
            // \"fragment_id\"
            if (offset + strlen(TAG_METHOD_STR) + strlen(TAG_PATH_STR) +
                  strlen(TAG_MESSAGE_STR) + strlen(TAG_FORMAT_STR) +
                  strlen(TAG_FRAG_ID_STR) + strlen(SEPARATOR_STR) * 4 <
                MAX_ATCMD_COAP_MSG_RSP_LEN) {
                offset += snprintf(
                  response + offset, MAX_ATCMD_COAP_MSG_RSP_LEN - offset,
                  "%s%s%s%s%s", TAG_METHOD_STR, SEPARATOR_STR,
                  TAG_PATH_STR, SEPARATOR_STR, TAG_MESSAGE_STR);
            } else {
                LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                     response);
                error_no = CME_ERROR_TXT_STR_TOO_LONG;
                goto err;
            }
        } else {
            if (m_auth_task_start) {
                LOGE(TAG, "auth in process!\n");
                error_no = CME_ERROR_OP_NOT_ALLOWED;
                goto err;
            }

            memset(&m_attempt_send_req_noack, 0,
                   sizeof(m_attempt_send_req_noack));

            body[0] = single;
            // eat ,
            atcmd_read(&single, 1);
            if ((m_attempt_send_req_noack.method = atoi(body)) < 0) {
                LOGE(TAG, "Invalid method %d !\n",
                     m_attempt_send_req_noack.method);
                error_no = CME_ERROR_INCORRECT_PARA;
                goto err;
            }

            // eat \"
            atcmd_read(&single, 1);
            ret = atcmd_socket_data_info_get(
              m_attempt_send_req_noack.path,
              sizeof(m_attempt_send_req_noack.path), NULL);
            if (ret < 0) {
                LOGE(TAG, "Invalid path %s !\n", m_attempt_send_req_noack.path);
                error_no = CME_ERROR_INCORRECT_PARA;
                goto err;
            }

            // ignore "
            if (strlen(m_attempt_send_req_noack.path) > 0 &&
                m_attempt_send_req_noack
                    .path[strlen(m_attempt_send_req_noack.path) - 1] == '\"') {
                m_attempt_send_req_noack
                  .path[strlen(m_attempt_send_req_noack.path) - 1] = '\0';
            } else {
                LOGE(TAG, "Invalid path %s !\n", m_attempt_send_req_noack.path);
                error_no = CME_ERROR_INCORRECT_PARA;
                goto err;
            }

            // eat \"
            atcmd_read(&single, 1);
            ret = atcmd_socket_data_info_get(
              m_attempt_send_req_noack.msg,
              sizeof(m_attempt_send_req_noack.msg), NULL);
            if (ret < 0) {
                LOGE(TAG, "Invalid message %s !\n",
                     m_attempt_send_req_noack.msg);
                error_no = CME_ERROR_INCORRECT_PARA;
                goto err;
            }

            // ignore "
            if (strlen(m_attempt_send_req_noack.msg) > 0 &&
                m_attempt_send_req_noack
                    .msg[strlen(m_attempt_send_req_noack.msg) - 1] == '\"') {
                m_attempt_send_req_noack
                  .msg[strlen(m_attempt_send_req_noack.msg) - 1] = '\0';
            } else {
                LOGE(TAG, "Invalid msg %s !\n", m_attempt_send_req_noack.msg);
                error_no = CME_ERROR_INCORRECT_PARA;
                goto err;
            }

            if (ret == 0) {
                memset(body, 0, sizeof(body));
                ret = atcmd_socket_data_info_get(body, sizeof(body),
                                                 &atcmd_socket_data_len_check);
                if (ret < 0 ||
                    (m_attempt_send_req_noack.msg_format = atoi(body)) < 0 ||
                    m_attempt_send_req_noack.msg_format > 1) {
                    LOGE(TAG, "Invalid format %s !\n", body);
                    error_no = CME_ERROR_INCORRECT_PARA;
                    goto err;
                }
            }

            if (ret == 0) {
                memset(body, 0, sizeof(body));
                ret = atcmd_socket_data_info_get(body, sizeof(body),
                                                 &atcmd_socket_data_len_check);
                if (ret <= 0 ||
                    (m_attempt_send_req_noack.fragment_id = atoi(body)) < 0) {
                    LOGE(TAG, "Invalid frag_id %s !\n", body);
                    error_no = CME_ERROR_INCORRECT_PARA;
                    goto err;
                }
            }

            m_attempt_send_req_noack.valid = true;
            ret = coap_sendreq(&m_attempt_send_req_noack, false);
            if (ret < 0) {
                if (ret == IOTX_ERR_MSG_TOO_LOOG ||
                    ret == IOTX_ERR_URI_TOO_LOOG)
                    error_no = CME_ERROR_TXT_STR_TOO_LONG;
                else if (ret == IOTX_ERR_INVALID_PARAM)
                    error_no = CME_ERROR_INCORRECT_PARA;
                else {
                    LOGE(TAG, "coap sendreqn unknown err! ret %d\n", ret);
                }

                goto err;
            } else if (m_attempt_send_req_noack.fragment_id == 0) {
                // prefix
                if (offset + strlen(coap_sendreqn_prefix) < MAX_ATCMD_COAP_MSG_RSP_LEN) {
                    offset +=
                      snprintf(response + offset, MAX_ATCMD_COAP_MSG_RSP_LEN - offset, "%s",
                               coap_sendreqn_prefix);
                } else {
                    LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                         response);
                    error_no = CME_ERROR_TXT_STR_TOO_LONG;
                    goto err;
                }

                // token
                if (offset + MAX_INTEGER_STR_LEN < MAX_ATCMD_COAP_MSG_RSP_LEN) {
                    offset +=
                      snprintf(response + offset,
                               MAX_ATCMD_COAP_MSG_RSP_LEN - offset, "%d", ret);
                } else {
                    LOGE(TAG, "%s %d at string too long %s\n", __func__,
                         __LINE__, response);
                    error_no = CME_ERROR_TXT_STR_TOO_LONG;
                    goto err;
                }
            }
        }
    } else {
        LOGE(TAG, "Invalid coap sendreq prefix %c!\n", single);
        error_no = CME_ERROR_TXT_STR_INVALID_CHAR;
        goto err;
    }

    // AT_RECV_PREFIX
    if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_COAP_MSG_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_COAP_MSG_RSP_LEN - offset, "%s",
                   AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        error_no = CME_ERROR_TXT_STR_TOO_LONG;
        goto err;
    }

    // AT_RECV_SUCCESS_POSTFIX
    if (offset + strlen(AT_RECV_SUCCESS_POSTFIX) < MAX_ATCMD_COAP_MSG_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_COAP_MSG_RSP_LEN - offset, "%s",
                   AT_RECV_SUCCESS_POSTFIX);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        error_no = CME_ERROR_TXT_STR_TOO_LONG;
        goto err;
    }

    if (insert_uart_send_msg(response, NULL, strlen(response), 0) != 0) {
        LOGE(TAG, "%s %d insert uart send msg fail\r\n", __func__, __LINE__);
        goto err;
    }

    return 0;

err:
    notify_atcmd_cme_error(error_no);

    return -1;
}

static int atcmd_icoap_rcvrsp()
{
    const char *coap_rcvrsp_prefix = "+ICOAPRCVRSP:";
    char        single;
    int         ret;
    int         offset   = 0;
    char       *response = coap_msg_rsp;

    memset(response, 0, MAX_ATCMD_COAP_MSG_RSP_LEN);

    atcmd_read(&single, 1);
    if ('=' != single && '?' != single) {
        LOGE(TAG, "Invalid coap rcvrsp prefix %c!\n", single);
        goto err;
    }

    if ('=' == single) {
        atcmd_read(&single, 1);

        if ('?' != single) {
            LOGE(TAG, "Invalid coap rcvrsp prefix %c!\n", single);
            goto err;
        }
    } else if ('?' == single) {
        // AT_RECV_PREFIX
        if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_COAP_MSG_RSP_LEN) {
            offset +=
              snprintf(response + offset, MAX_ATCMD_COAP_MSG_RSP_LEN - offset,
                       "%s", AT_RECV_PREFIX);
        } else {
            LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                 response);
            goto err;
        }

        // prefix
        if (offset + strlen(coap_rcvrsp_prefix) < MAX_ATCMD_COAP_MSG_RSP_LEN) {
            offset +=
              snprintf(response + offset, MAX_ATCMD_COAP_MSG_RSP_LEN - offset,
                       "%s", coap_rcvrsp_prefix);
        } else {
            LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                 response);
            goto err;
        }

        // PACKET_TOKEN + MSG_LEN + MESSAGE + [FORMAT] + [FRAGMENT_ID]
        if (offset + strlen(m_last_recv_rsp.msg) + strlen(SEPARATOR_STR) * 4 +
              strlen(QUOTE_STR) * 2 + MAX_INTEGER_STR_LEN * 3 +
              ONE_DIGIT_STR_LEN <
            MAX_ATCMD_COAP_MSG_RSP_LEN) {
            if (m_last_recv_rsp.valid)
                offset += snprintf(
                  response + offset, MAX_ATCMD_COAP_MSG_RSP_LEN - offset,
                  "%d%s%d%s\"%s\"%s%d%s%d", m_last_recv_rsp.packet_token,
                  SEPARATOR_STR, m_last_recv_rsp.msg_len, SEPARATOR_STR,
                  m_last_recv_rsp.msg, SEPARATOR_STR,
                  m_last_recv_rsp.msg_format, SEPARATOR_STR,
                  m_last_recv_rsp.fragment_id);
            else
                offset +=
                  snprintf(response + offset,
                           MAX_ATCMD_COAP_MSG_RSP_LEN - offset, ",,,,,");
        } else {
            LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                 response);
            goto err;
        }


    } else {
        LOGE(TAG, "Invalid coap rcvrsp prefix %c!\n", single);
        goto err;
    }

    // AT_RECV_PREFIX
    if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_COAP_MSG_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_COAP_MSG_RSP_LEN - offset, "%s",
                   AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        goto err;
    }

    // AT_RECV_SUCCESS_POSTFIX
    if (offset + strlen(AT_RECV_SUCCESS_POSTFIX) < MAX_ATCMD_COAP_MSG_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_COAP_MSG_RSP_LEN - offset, "%s",
                   AT_RECV_SUCCESS_POSTFIX);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        goto err;
    }

    if (insert_uart_send_msg(response, NULL, strlen(response), 0) != 0) {
        LOGE(TAG, "%s %d insert uart send msg fail\r\n", __func__, __LINE__);
        goto err;
    }

    return 0;

err:
    return -1;
}

static int atcmd_icoap_sendbin()
{
    return -1;
}

static int atcmd_icoap_sendnbin()
{
    return -1;
}

static int atcmd_icoap_rcvbin()
{
    return -1;
}

enum
{
    ATCMD_ICOAP_PARA = 0,
    ATCMD_ICOAP_OPEN,
    ATCMD_ICOAP_AUTH,
    ATCMD_ICOAP_SENDREQ,
    ATCMD_ICOAP_SENDBIN,
    ATCMD_ICOAP_SENDREQN,
    ATCMD_ICOAP_SENDNBIN,
    ATCMD_ICOAP_RCVRSP,
    ATCMD_ICOAP_RCVBIN,
};

static const atcmd_hdl_t at_coap_cmds_table[] = {
    { .name     = "AT+ICOAPARA",
      .help     = "AT+ICOAPARA=<ParaTag>,<ParaValue>",
      .function = atcmd_icoap_para },
    { .name     = "AT+ICOAPOPEN",
      .help     = "AT+ICOAPOPEN=<hostname>,<port>",
      .function = atcmd_icoap_open },
    { .name     = "AT+ICOAPAUTH",
      .help     = "AT+ICOAPAUTH=<ParaValue1>,<ParaValue2>,...<ParaValuen>",
      .function = atcmd_icoap_auth },
    { .name = "AT+ICOAPSENDREQ",
      .help =
        "AT+ICOAPSENDREQ=<method>,<path>,<message>,[format],[fragment_id]",
      .function = atcmd_icoap_sendreq },
    { .name     = "AT+ICOAPSENDBIN",
      .help     = "AT+ICOAPSENDBIN=<method>,<path>,<msg_len>",
      .function = atcmd_icoap_sendbin },
    { .name = "AT+ICOAPSENDREQN",
      .help =
        "AT+ICOAPSENDREQN=<method>,<path>,<message>,[format],[fragment_id]",
      .function = atcmd_icoap_sendreqn },
    { .name     = "AT+ICOAPSENDNBIN",
      .help     = "AT+ICOAPSENDNBIN=<method>,<path>,<msg_len>",
      .function = atcmd_icoap_sendnbin },
    { .name     = "AT+ICOAPRCVRSP",
      .help     = "AT+ICOAPRCVRSP?",
      .function = atcmd_icoap_rcvrsp },
    { .name     = "AT+ICOAPRCVBIN",
      .help     = "AT+ICOAPRCVBIN?",
      .function = atcmd_icoap_rcvbin },
};

static atcmd_hdl_ptr_t get_atcmd_coap_handler()
{
    const char *atcmd_prefix                 = "OAP";
    char        prefix[MAX_ATCMD_PREFIX_LEN] = { 0 };
    char        single;
    int         index = -1;

    // Eat "OAP"
    atcmd_read(prefix, strlen(atcmd_prefix));
    if (memcmp(prefix, atcmd_prefix, strlen(atcmd_prefix)) != 0) {
        goto err;
    }

    atcmd_read(&single, 1);
    memset(prefix, 0, sizeof(prefix));

    switch (single) {
        case 'O':
            atcmd_read(prefix, 3);

            if (memcmp(prefix, "PEN", 3) != 0)
                goto err;

            index = ATCMD_ICOAP_OPEN;
            break;
        case 'A':
            atcmd_read(prefix, 2);

            if (memcmp(prefix, "RA", 2) == 0) {

                index = ATCMD_ICOAP_PARA;
            } else if (memcmp(prefix, "UT", 2) == 0) {
                atcmd_read(&single, 1);
                if (single != 'H')
                    goto err;

                index = ATCMD_ICOAP_AUTH;
            } else {
                goto err;
            }

            break;
        case 'S':
            atcmd_read(prefix, 3);

            if (memcmp(prefix, "END", 3) != 0)
                goto err;

            atcmd_read(prefix, 3);
            // shared by SENDREQ and SENDREQN
            if (memcmp(prefix, "REQ", 3) == 0) {
                index = ATCMD_ICOAP_SENDREQ;
            } else if (memcmp(prefix, "BIN", 3) == 0) {
                index = ATCMD_ICOAP_SENDBIN;
            } else if (memcmp(prefix, "NBI", 2) == 0) {
                atcmd_read(&single, 1);
                if (single != 'N')
                    goto err;

                index = ATCMD_ICOAP_SENDNBIN;
            } else {
                goto err;
            }

            break;
        case 'R':
            atcmd_read(prefix, 2);

            if (memcmp(prefix, "CV", 2) != 0)
                goto err;

            atcmd_read(prefix, 3);
            if (memcmp(prefix, "RSP", 3) == 0) {
                index = ATCMD_ICOAP_RCVRSP;
            } else if (memcmp(prefix, "BIN", 3) == 0) {
                index = ATCMD_ICOAP_RCVBIN;
            } else {
                goto err;
            }

            break;
        default:
            goto err;
    }

    if (index >= 0 && index < sizeof(at_coap_cmds_table)) {
        return (atcmd_hdl_ptr_t) &at_coap_cmds_table[index];
    }

err:
    LOGE(TAG, "Invalid coap prefix %s\n", prefix);
    return NULL;
}

static int coap_init()
{
    memset(&m_coap_config, 0, sizeof(m_coap_config));

    m_coap_config.timeout = COAP_DEFAULT_REQ_TIMOUT_SEC;

    memset(&m_last_send_req, 0, sizeof(m_last_send_req));
    memset(&m_frag_send_req, 0, sizeof(m_frag_send_req));
    memset(&m_last_send_req_noack, 0, sizeof(m_last_send_req_noack));
    memset(&m_frag_send_req_noack, 0, sizeof(m_frag_send_req_noack));
    memset(&m_last_recv_rsp, 0, sizeof(m_last_recv_rsp));

    if (aos_mutex_new(&m_comm_mutex) != 0) {
        LOGE(TAG, "Creating mutex failed\r\n");
        return -1;
    }

#if 0
    // not support for now
    post_coap_yield_task();
#endif

    return 0;
}

static void coap_deinit()
{
    if (aos_sem_is_valid(&m_yield_sem)){
        aos_sem_signal(&m_yield_sem);
    }

    if (aos_mutex_is_valid(&m_comm_mutex)) {
        aos_mutex_free(&m_comm_mutex);
    }

    m_yield_task_start = false;

    IOT_CoAP_Deinit(&p_context);

    return;
}

atcmd_op_t coap_op = {
    .name              = "coap",
    .prefix            = "IC",
    .init              = coap_init,
    .deinit            = coap_deinit,
    .get_atcmd_handler = get_atcmd_coap_handler,
};

int register_atcmd_coap_opt()
{
    return atcmd_register_handlers(&coap_op);
}
