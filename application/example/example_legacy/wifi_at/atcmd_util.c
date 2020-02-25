/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "aos/kernel.h"
#include "ulog/ulog.h"

#include "atcmd_config.h"
#include "atcmd_util.h"

#define TAG "linkkit_at"

static aos_sem_t g_sem_response;
static aos_queue_t g_msg_queue;
static int g_ret_val = -1;
static int g_atcm_state = WAIT_NONE;
static uart_dev_t uart_dev;
static int at_dev_fd = -1;

int atcmd_read(char *outbuf, int readsize)
{
    return at_read(at_dev_fd, outbuf, readsize);
}

int atcmd_send_wait_reply(const char *cmd, int cmdlen, bool delimiter,
                          const char *data, int datalen,
                          char *replybuf, int bufsize,
                          const at_reply_config_t *atcmdconfig)
{
    return at_send_wait_reply(at_dev_fd, cmd, cmdlen, delimiter, data,
                             datalen, replybuf, bufsize, atcmdconfig);
}

int atcmd_send_no_reply(const char *data, int datalen, bool delimiter)
{
    return at_send_no_reply(at_dev_fd, data, datalen, delimiter);
}

int atcmd_send_wait_ret_val(const char *data, int datalen, bool delimiter, int state, int timeout_ms) 
{
    int ret = -1;

    if (atcmd_send_no_reply(data, datalen, delimiter) < 0) {
        return ret;
    }

    atcmd_set_state(state);

    if (0 == atcmd_wait_notify(timeout_ms)) {
        ret = atcmd_get_ret_val();
    }

    atcmd_set_state(WAIT_NONE);

    return ret;
}

int atcmd_insert_msg(void *msg_ptr)
{
    return aos_queue_send(&g_msg_queue, msg_ptr, sizeof(void*));
}

int atcmd_tryfetch_msg(void *msg_ptr)
{
    int len;
    return aos_queue_recv(&g_msg_queue, 0, msg_ptr, &len);
}

int atcmd_get_ret_val(void)
{
    return g_ret_val;
}

void atcmd_set_ret_val(int val)
{
    g_ret_val = val;
}

int atcmd_get_state(void)
{
    return g_atcm_state;
}

void atcmd_set_state(int state)
{
    g_atcm_state = state;
}

void atcmd_notify_sender(void)
{
    aos_sem_signal(&g_sem_response);
}

int atcmd_wait_notify(int timeout_ms)
{
    return aos_sem_wait(&g_sem_response, timeout_ms);
}

int atcmd_integer_check(char data)
{
    if (data > '9' || data < '0') {
        return -1;
    }
    return 0;
}

int atcmd_info_from_stream(char *buf, unsigned int buflen, at_data_check_cb_t valuecheck)
{
    unsigned int i = 0;

    if (NULL == buf || 0 == buflen) {
        return -1;
    }

    do {
        at_read(at_dev_fd, &buf[i], 1);
        if (buf[i] == ',') {
            buf[i] = 0;
            break;
        } else if (buf[i] == '\r') {
            LOGD(TAG, "delimiter found\n");
            buf[i] = 0;
            return 1;
        }

        if (NULL != valuecheck) {
            if (valuecheck(buf[i])) {
                LOGE(TAG, "Invalid string!!!, reader is %s last char %d\r\n",
                     buf, buf[i]);
                return -1;
            }
        }
        i++;
    } while (1);

    return 0;
}

/* add slash before quote for payload */
int atcmd_add_slash_quote(char *dest, int size, char *payload, int payload_len)
{
    int i = 0, j = 0;

    dest[i++] = '\"'; 
    while (i < size && j < payload_len) {
        if (payload[j] == '\"') {
            dest[i++] = '\\';
            dest[i++] = '\"';
            j++;
        } else {
            dest[i++] = payload[j++];
        }
    }

    if (i < size) {
        dest[i++] = '\"';
    }

    return i;
}

/*
* +IDMSSR:0
* OK
*/
static void service_set_reply_rsp(void)
{
    char integer_buf[15] = { 0 };
    int packid;
    int ret;

    /* check whether first field is packet id */
    ret = atcmd_info_from_stream(integer_buf, sizeof(integer_buf), atcmd_integer_check);
    if (ret == 1) { /* packet id */
        packid = atoi(integer_buf);
        if (packid < 0) {
           LOGE(TAG, "%d Parese int failed: %s", __LINE__, integer_buf);
           return;
        }

        LOGI(TAG, "Service set reply sent!");
    } else {
        LOGE(TAG, "%d Parese int failed: %s", __LINE__, integer_buf);
        return;
    }
}

/* +IDMSS:0,"ReceiveEvents","1009345953",2,"{}" */
static void service_set_request(void)
{
    char integer_buf[15] = { 0 };
    req_msg_t  *serv_req;
    char *payload = NULL;
    char single;
    int devid, payload_len;
    int ret;

    serv_req = aos_malloc(sizeof(req_msg_t));
    if (!serv_req) {
        LOGE(TAG, "serv req malloc failed!");
        return;
    }

    serv_req->type = SERV_SET_MSG;

    /* first field is device id */
    ret = atcmd_info_from_stream(integer_buf, sizeof(integer_buf), atcmd_integer_check);
    if (ret == 0) {
        devid = atoi(integer_buf);
        if (devid < 0) {
            LOGE(TAG, "%d Parese int failed: %s", __LINE__, integer_buf);
            goto err;
        }
    } else {
        LOGE(TAG, "%d Parese int failed: %s", __LINE__, integer_buf);
        goto err;
    }

    memset(serv_req->serv_id, 0, sizeof(serv_req->serv_id));
    atcmd_read(&single, 1);

    /* second field is service id */
    ret = atcmd_info_from_stream(serv_req->serv_id, sizeof(serv_req->serv_id), NULL);
    if (ret == 0) {
        if (serv_req->serv_id[strlen(serv_req->serv_id) - 1] != '\"') {
            LOGE(TAG, "invalid service id %s", serv_req->serv_id);
            goto err;
        }

        serv_req->serv_id[strlen(serv_req->serv_id) - 1] = '\0'; 
    } else {
        LOGE(TAG, "%d Parese failed: %s", __LINE__, serv_req->serv_id);
        goto err;
    }

    memset(serv_req->msg_id, 0, sizeof(serv_req->msg_id));
    atcmd_read(&single, 1);

    /* third field is msg id */
    ret = atcmd_info_from_stream(serv_req->msg_id, sizeof(serv_req->msg_id), NULL);
    if (ret == 0) {
        if (serv_req->msg_id[strlen(serv_req->msg_id) - 1] != '\"') {
            LOGE(TAG, "invalid msg id %s", serv_req->msg_id);
            goto err;
        }

        serv_req->msg_id[strlen(serv_req->msg_id) - 1] = '\0'; 
    } else {
        LOGE(TAG, "%d Parese failed: %s", __LINE__, serv_req->msg_id);
         goto err;
    }

    memset(integer_buf, 0, sizeof(integer_buf));

    /* fourth field is len */
    ret = atcmd_info_from_stream(integer_buf, sizeof(integer_buf), atcmd_integer_check);
    if (ret == 0) {
        payload_len = atoi(integer_buf);
        if (payload_len < 0) {
           LOGE(TAG, "%d Parese int failed: %s", __LINE__, integer_buf);
            goto err;
        }
    } else {
        LOGE(TAG, "%d Parese int failed: %s", __LINE__, integer_buf);
         goto err;
    }

    serv_req->payload_len = payload_len;

    /* process payload */
    payload = (char *) aos_malloc(payload_len + 1);
    if (payload == NULL) {
        LOGE(TAG, "payload buf malloc failed!");
    }

    atcmd_read(&single, 1);
    atcmd_read(payload, payload_len);
    payload[payload_len] = '\0';

    serv_req->payload = payload;

    LOGI(TAG, "Service set serv id %s msg id %s Payload: %s", serv_req->serv_id, serv_req->msg_id, payload);

    atcmd_insert_msg(&serv_req);

    return;
err:
    aos_free(serv_req);
    aos_free(payload);
}

static void prop_post_rsp_process(void)
{
    char integer_buf[15] = { 0 };
    char *payload = NULL;
    char single;
    int devid, packid, code, payload_len;
    int ret;

    /* check whether first field is packet id */
    ret = atcmd_info_from_stream(integer_buf, sizeof(integer_buf), atcmd_integer_check);
    if (ret == 1) { /* packet id */
        packid = atoi(integer_buf);
        if (packid < 0) {
           LOGE(TAG, "Parese int failed: %s", integer_buf);
           return;
        }

        if (WAIT_PACKET_ID == atcmd_get_state()) {
            atcmd_set_ret_val(packid);
            atcmd_notify_sender();
        }

        return;
    } else if (ret == 0) { /* devid id */
        devid = atoi(integer_buf);
        if (devid < 0) {
            LOGE(TAG, "Parese int failed: %s", integer_buf);
            return;
        }
    } else {
        LOGE(TAG, "%d Parese int failed: %s", __LINE__, integer_buf);
        return;
    }

    memset(integer_buf, 0, sizeof(integer_buf));

    /* second field is packet id */
    ret = atcmd_info_from_stream(integer_buf, sizeof(integer_buf), atcmd_integer_check);
    if (ret == 0) { /* packet id */
        packid = atoi(integer_buf);
        if (packid < 0) {
            LOGE(TAG, "%d Parese int failed: %s", __LINE__, integer_buf);
           return;
        }
    } else {
        LOGE(TAG, "%d Parese int failed: %s", __LINE__, integer_buf);
        return;
    }

    memset(integer_buf, 0, sizeof(integer_buf));

    /* third field is code */
    ret = atcmd_info_from_stream(integer_buf, sizeof(integer_buf), atcmd_integer_check);
    if (ret == 0) { /* code */
        code = atoi(integer_buf);
        if (code < 0) {
            LOGE(TAG, "%d Parese int failed: %s", __LINE__, integer_buf);
            return;
        }
    } else {
        LOGE(TAG, "%d Parese int failed: %s", __LINE__, integer_buf);
        return;
    }

    memset(integer_buf, 0, sizeof(integer_buf));

    /* fourth field is len */
    ret = atcmd_info_from_stream(integer_buf, sizeof(integer_buf), atcmd_integer_check);
    if (ret == 0) { /* code */
        payload_len = atoi(integer_buf);
        if (payload_len < 0) {
           LOGE(TAG, "Parese int failed: %s", integer_buf);
           return;
        }
    } else {
        LOGE(TAG, "%d Parese int failed: %s", __LINE__, integer_buf);
        return;
    }

    /* process payload */
    payload = (char *) aos_malloc(payload_len + 1);
    if (payload == NULL) {
        LOGE(TAG, "payload buf malloc failed!");
    }

    atcmd_read(&single, 1);
    atcmd_read(payload, payload_len);
    payload[payload_len] = '\0';

    LOGI(TAG, "Property post Pkid: %d Code: %d Payload: %s", packid, code, payload);
    aos_free(payload);
}

static void conn_stat_process(void)
{
    int state = 0;
    char state_str[5] = { 0 };

    if (atcmd_info_from_stream(state_str, sizeof(state_str),
                               atcmd_integer_check) < 0){
        LOGE(TAG, "%s %d failed", __func__, __LINE__);
        return;
    }

    state = atoi(state_str);
    if (state < 0) {
        LOGE(TAG, "%s %d failed", __func__, __LINE__);
        return;
    }

    if (WAIT_NET_STATUS == atcmd_get_state()) {
        atcmd_set_ret_val(state);
        atcmd_notify_sender();
    }
}

/*
 * +IDMSTA
 * +IDMPP
 * +IDMEP
 */
void dm_data_process(void)
{
    char  prefix[AT_MAX_PREFIX_LEN] = { 0 };
    char  single;
    int i = 0;

    do {
        atcmd_read(&single, 1);
        prefix[i++] = single;
        if (i >= AT_MAX_PREFIX_LEN) {
            LOGE(TAG, "Prefix too long!");
            return;
        }
    } while(single != ':');

    prefix[i - 1] = '\0';

    if (memcmp(prefix, "PP", strlen("PP")) == 0) {
        prop_post_rsp_process();
    } else if (memcmp(prefix, "SS", strlen(prefix)) == 0) {
        service_set_request();
    } else if (memcmp(prefix, "SSR", strlen(prefix)) == 0) {
        service_set_reply_rsp();
    } else if (memcmp(prefix, "STA", strlen(prefix)) == 0) {
        conn_stat_process();
    } else {
        LOGE(TAG, "Unknown prefix %s", prefix);
    }
}

void cme_err_process(void)
{
    LOGE(TAG, "CME error recved!");
}

static void at_response_callback(void *arg, char *rspinfo, int rsplen)
{
    char prefix[4] = {0};
    int size = 4;
    char *input = (char *)arg;

    if (NULL == input)
        return;

    if (0 == memcmp(input, AT_DM_PREFIX, strlen(AT_DM_PREFIX))) {
        dm_data_process();
    } else if (0 == memcmp(input, AT_CME_ERR_PREFIX, strlen(AT_CME_ERR_PREFIX))) {
        cme_err_process();
    } else {
        LOGE(TAG, "Unknown arg %s", input);
    }

    return;
}

int at_device_init(void)
{
    at_config_t at_config = { 0 };
    void *ptr_start = NULL;
    int ptr_size = sizeof(void *);
    int ptr_num = 5;

    at_init();

    /* uart_dev should be maintained in whole life cycle */
    uart_dev.port                = AT_UART_PORT;
    uart_dev.config.baud_rate    = AT_UART_BAUDRATE;
    uart_dev.config.data_width   = AT_UART_DATA_WIDTH;
    uart_dev.config.parity       = AT_UART_PARITY;
    uart_dev.config.stop_bits    = AT_UART_STOP_BITS;
    uart_dev.config.flow_control = AT_UART_FLOW_CONTROL;
    uart_dev.config.mode         = AT_UART_MODE;

    /* configure and add one uart dev */
    at_config.type                             = AT_DEV_UART;
    at_config.port                             = AT_UART_PORT;
    at_config.dev_cfg                          = &uart_dev;
    at_config.recv_task_priority               = AT_WORKER_PRIORITY;
    at_config.recv_task_stacksize              = AT_WORKER_STACK_SIZE;

    if ((at_dev_fd = at_add_dev(&at_config)) < 0) {
        LOGE(TAG, "AT parser device add failed!\n");
        return -1;
    }

    if (aos_sem_new(&g_sem_response, 0) != 0) {
        LOGE(TAG, "sem create failed!\n");
        return -1;
    }

    ptr_start = aos_malloc(ptr_num * ptr_size);
    if (ptr_start == NULL) {
        return -1;
    }

    if (aos_queue_new(&g_msg_queue, ptr_start, ptr_num * ptr_size, ptr_size) != 0) {
        return -1;
    }

    /* +IDM prefix */
    at_register_callback(at_dev_fd, AT_DM_PREFIX, NULL, NULL,
                         0, at_response_callback, AT_DM_PREFIX);

    /* +CME ERROR prefix */
    at_register_callback(at_dev_fd, AT_CME_ERR_PREFIX, NULL, NULL,
                         0, at_response_callback, AT_CME_ERR_PREFIX);

    return 0;
}

 /* config AT device. i.e. auth info */
int at_device_config(const char *productkey,
                     const char *devicename,
                     const char *devicensecret,
                     const char *productsecret)
{
    char cmd_str[200] = { 0 };
    char rsp_str[20] = { 0 };
    int len = 0;

    len = snprintf(cmd_str, sizeof(cmd_str),
                   AT_AUTH_FORMAT, AT_AUTH_PREFIX,
                   productkey, devicename,
                   devicensecret, productsecret);

    atcmd_send_wait_reply(cmd_str, len, true, NULL, 0,
                          rsp_str, sizeof(rsp_str), NULL);
    if (strstr(rsp_str, CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "%s %d failed", __func__, __LINE__);
        return -1;
    }

    return 0;
}

static int query_cloud_conn_status(void)
{
    char cmd_str[20] = { 0 };
    int state = 0, len;

    len = snprintf(cmd_str, sizeof(cmd_str), "%s", AT_QUERY_CONN_PREFIX);

    state = atcmd_send_wait_ret_val(cmd_str, len, true, WAIT_NET_STATUS, SEND_WAIT_REPLY_TIMEOUT_MS);

    return state;
}

static int connect_wifi_by_cmd(const char * ssid, const char * pwd)
{
    char cmd_str[50] = { 0 };
    char rsp_str[100] = { 0 };
    int len = 0;
    at_reply_config_t atcmdconfig = {
        AT_WIFI_RSP_IP_PREFIX,
        CMD_SUCCESS_RSP,
        CMD_FAIL_RSP
    };

    if (!ssid || !pwd) {
        return -1;
    }

    len = snprintf(cmd_str, sizeof(cmd_str),
                   AT_WIFI_CONN_FORMAT,
                   AT_WIFI_CONN_PREFIX,
                   ssid, pwd);

    atcmd_send_wait_reply(cmd_str, len, true,
                          NULL, 0, rsp_str, sizeof(rsp_str), NULL);
    /* TODO: station down response */
    if (strstr(rsp_str, CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "%s %d failed", __func__, __LINE__);
        return -1;
    }

    memset(cmd_str, 0, sizeof(cmd_str));
    len = snprintf(cmd_str, sizeof(cmd_str), "%s",
                   AT_WIFI_OBTAIN_IP);

    /* poll ip status */
    while (1) {
        aos_msleep(3000);

        LOGI(TAG, "check connect status");
        memset(rsp_str, 0, sizeof(rsp_str));
        atcmd_send_wait_reply(cmd_str, len, true,
                           NULL, 0, rsp_str, sizeof(rsp_str), &atcmdconfig);
        if (strstr(rsp_str, CMD_SUCCESS_RSP)) {
            LOGI(TAG, "ip rsp %s", rsp_str);
            break;
        }
    }
}

static int connect_wifi_by_awss(void)
{
    char cmd_str[20] = { 0 };
    int len = 0;
    int type = 0; /* default type */

    len = snprintf(cmd_str, sizeof(cmd_str), "%s=%d",
                   AT_AWSS_START_PREFIX, type);

    atcmd_send_no_reply(cmd_str, len, true);

    aos_msleep(5000);

    return 0;
}

/* connect wifi */
int at_device_connect(const char *ssid, const char *pwd)
{
    int count = 0;

    if (query_cloud_conn_status() == 2) {
        LOGI(TAG, "Already connected!");
        return 0;
    }

    /* connnect wifi and module will auto connct cloud */
    if (ssid && pwd) {
        connect_wifi_by_cmd(ssid, pwd);
    } else {
        connect_wifi_by_awss();
    }

    /* poll cloud conn status */
    while (1) {
        aos_msleep(2000);

        if (query_cloud_conn_status() == 2) {
            LOGI(TAG, "Cloud connected!");
            return 0;
        }

        if (count > 60) {
            LOGE(TAG, "poll_cloud_status timeout!");
            break;
        }

        count++;
    }

    return -1;
}

int at_device_reset(void)
{
    char cmd_str[20] = { 0 };
    int len = 0;

    /* AT+IWSSTOP */
    len = snprintf(cmd_str, sizeof(cmd_str),
                   "%s", AT_AWSS_STOP_PREFIX);

    atcmd_send_no_reply(cmd_str, len, true);

    memset(cmd_str, 0, sizeof(cmd_str));

    aos_msleep(1000);

    /* AT+IDMCLS */
    len = snprintf(cmd_str, sizeof(cmd_str),
                   "%s", AT_DISCON_PREFIX);

    atcmd_send_no_reply(cmd_str, len, true);

    return 0;
}

