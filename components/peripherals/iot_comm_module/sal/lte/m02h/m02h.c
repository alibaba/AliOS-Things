/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */


#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "aos/kernel.h"
#include "ulog/ulog.h"
#include "aos/yloop.h"

#include <atcmd_config_module.h>

#include <at/at.h>
#include <hal_sal.h>

typedef enum {
    MODE_BUFFER_ACCESS = 0,
    MODE_DIRECT_PUSH,
    MODE_TRANSPARENT_ACCESS,
    MODE_END
} ACCESS_MODE;

#define TAG "m02h_lte_module"

#define AT_UART_BAUDRATE 115200

#define M02H_AT_CMD_SUCCESS_RSP "OK"
#define M02H_AT_CMD_FAIL_RSP "ERROR"

#define AT_CMD_TEST "AT"
#define AT_CMD_TEST_RESULT "OK"

#define AT_CMD_ECHO_OFF   "ATE0"
#define AT_CMD_BAUDRATE_SET "AT+IPR"
#define AT_CMD_FLOW_CONTROL "AT+IFC"

#define AT_CMD_SIM_PIN_CHECK  "AT+CPIN?"
#define AT_CMD_SIGNAL_QUALITY_CHECK "AT+CESQ"

#define AT_CMD_LTE_NETWORK_REG_CHECK "AT+CEREG?"
#define AT_CMD_LTE_NETWORK_REG_CHECK_RSP "+CEREG:"
#define AT_CMD_NETWORK_REG_CHECK "AT+CGREG?"
#define AT_CMD_NETWORK_REG_CHECK_RSP "+CGREG:"
#define AT_CMD_GPRS_ATTACH_CHECK "AT+CGATT?"
#define AT_CMD_GPRS_ATTACH_CHECK_RSP "+CGATT:"

#define AT_CMD_PHONE_FUNCTION_LEVEL "AT+CFUN"
#define AT_CMD_DEFAULT_PDP_CONTEXT "AT*CGDFLT"
#define AT_CMD_AUTHENTICATION_PARA "AT*CGDFAUTH"

#define AT_CMD_QUERY_PDP_CONTEXT "AT+CGDCONT?"
#define AT_CMD_PDP_CONTEXT_RSP "+CGDCONT:"
#define AT_CMD_CONFIGURE_CONTEXT "AT+QICSGP"
#define AT_CMD_ACTIVATE_CONTEXT "AT+QIACT"
#define AT_CMD_ACTIVATE_CONTEXT_RSP "+QIACT:"
#define AT_CMD_DEACTIVATE_CONTEXT "AT+QIDEACT"

#define AT_CMD_START_SOCKET_SERVICE "AT+QIOPEN"
#define AT_CMD_CLOSE_SOCKET_SERVICE "AT+QICLOSE"
#define AT_CMD_SEND_DATA "AT+QISEND"
#define AT_CMD_OOB_PREFIX "\r\n+QIURC:"

#define AT_CMD_DOMAIN_TO_IP "AT+DNS"
#define AT_CMD_DOMAIN_TO_IP_RSP "+DNS:"

#define M02H_NETREG_INTERVAL            1000
#define M02H_NETREG_LOOP                120
#define M02H_CONN_CMD_LEN               256
#define M02H_DNS_CMD_RSP_LEN            128
#define M02H_DEFAULT_CMD_LEN            64
#define M02H_DEFAULT_RSP_LEN            64
#define M02H_LARGE_RSP_LEN              256
#define M02H_APN_LEN                    64
#define M02H_U32_STRING_LEN             10

/* Dual socket temp solution */
#define M02H_MAX_LINK_NUM               2

#define link2context(id)                (id + 1)
#define link2connect(id)                (id)
#define context2link(id)                (id - 1)
#define connect2link(id)                (id)

/* Change to include data slink for each link id respectively. <TODO> */
typedef struct link_s {
    int fd;
    aos_sem_t sem_start;
    aos_sem_t sem_close;
} link_t;

static int hal_start_count = 0;
static uint8_t inited = 0;
static link_t g_link[M02H_MAX_LINK_NUM];
static aos_mutex_t g_link_mutex;
static aos_mutex_t g_domain_mutex;
static char g_apn[M02H_APN_LEN] = {0};
static uint32_t g_ct_stat_bw = 0;

static netconn_data_input_cb_t g_netconn_data_input_cb;

static uart_dev_t uart_dev;
int at_dev_fd = -1;

static int fd_to_linkid(int fd)
{
    int link_id;

    if (aos_mutex_lock(&g_link_mutex, AOS_WAIT_FOREVER) != 0) {
        LOGE(TAG, "Failed to lock mutex (%s).", __func__);
        return -1;
    }
    for (link_id = 0; link_id < M02H_MAX_LINK_NUM; link_id++) {
        if (g_link[link_id].fd == fd) {
            break;
        }
    }

    aos_mutex_unlock(&g_link_mutex);

    return link_id;
}

static unsigned int at_read_string(char *buf, unsigned int size, char ch_end)
{
    unsigned int i = 0;

    if (buf == NULL || size == 0) {
        LOGE(TAG, "[Error: %s %d] Invalid parameters.", __func__, __LINE__);
        return 0;
    }

    do {
        at_read(at_dev_fd, &buf[i], 1);
        if (buf[i] == ch_end) {
            break;
        }
        if (i >= size - 1) {
            LOGE(TAG, "[Error: %s %d] Too long length.", __func__, __LINE__);
            break;
        }
        ++i;
    } while (1);
    buf[i] = 0;

    return i;
}

static unsigned int at_read_uint(char expect_end)
{
    char str_value[M02H_U32_STRING_LEN + 1] = {0};
    int i = 0;

    do {
        at_read(at_dev_fd, &str_value[i], 1);
        if (str_value[i] == expect_end) {
            break;
        }
        if (i >= M02H_U32_STRING_LEN) {
            LOGE(TAG, "[Error: %s %d] Too long length of str_value is %s.", __func__, __LINE__, str_value);
            break;
        }
        if (!isdigit(str_value[i])) {
            LOGE(TAG, "[Error: %s %d] Invalid len string, str_value is %s.", __func__, __LINE__, str_value);
            break;
        }
        ++i;
    } while (1);

    return strtoul(str_value, NULL, 10);
}

static int m02h_rsp_parse_string(const char *rsp, int field_index, char *str_buf, int str_size)
{
    const char *prsp = rsp;
    char *pfield = NULL;
    int field_len = 0;
    int i;

    if (rsp == NULL || field_index <= 0 || str_buf == NULL) {
        LOGE(TAG, "%s %d parameters is invalid\r\n", __func__, __LINE__);
        return -1;
    }
    while (*prsp == ' ') {
        ++prsp;
    }
    for (i = 1; i < field_index; ++i) {
        prsp = strchr(prsp, ',');
        if (prsp == NULL) {
            LOGE(TAG, "%s %d parse failed %s\r\n", __func__, __LINE__, rsp);
            return -1;
        }
        ++prsp;
    }
    if (*prsp != '\"') {
        LOGE(TAG, "%s %d The field %d is not string %s\r\n", __func__, __LINE__, field_index, rsp);
        return -1;
    }
    pfield = prsp + 1;
    prsp = strstr(pfield, "\",");
    if (prsp == NULL) {
        prsp = strstr(pfield, "\"\r");
    }
    field_len = prsp - pfield;
    if (prsp == NULL || field_len >= str_size) {
        LOGE(TAG, "%s %d parse failed %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }
    memcpy(str_buf, pfield, field_len);
    str_buf[field_len] = '\0';

    return 0;
}

static int m02h_rsp_parse_uint(const char *rsp, int field_index, unsigned int *pvalue)
{
    const char *prsp = rsp;
    char field[M02H_U32_STRING_LEN + 1] = {0};
    int i;

    if (rsp == NULL || field_index <= 0 || pvalue == NULL) {
        LOGE(TAG, "%s %d parameters is invalid\r\n", __func__, __LINE__);
        return -1;
    }
    while (*prsp == ' ') {
        ++prsp;
    }
    for (i = 1; i < field_index; ++i) {
        prsp = strchr(prsp, ',');
        if (prsp == NULL) {
            LOGE(TAG, "%s %d parse failed %s\r\n", __func__, __LINE__, rsp);
            return -1;
        }
        ++prsp;
    }
    for (i = 0; isdigit(prsp[i]) && i < M02H_U32_STRING_LEN; ++i) {
        field[i] = prsp[i];
    }
    if (i == 0 || (prsp[i] != ',' && prsp[i] != '\r')) {
        LOGE(TAG, "%s %d parse failed %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }

    *pvalue = strtoul(field, NULL, 10);

    return 0;
}

static int m02h_lte_wait_register(void)
{
    const char *regcmd = NULL;
    const char *regrsp = NULL;
    char rsp[M02H_DEFAULT_RSP_LEN] = {0};
    char *prsp = NULL;
    unsigned int netreg_stat = 0;
    unsigned int lte_netreg_stat = 0;
    int i = 0;

REGLOOP:
    regcmd = AT_CMD_LTE_NETWORK_REG_CHECK;
    regrsp = AT_CMD_LTE_NETWORK_REG_CHECK_RSP;
    while (1) {
        memset(rsp, 0, M02H_DEFAULT_RSP_LEN);
        at_send_wait_reply(at_dev_fd, regcmd, strlen(regcmd), true, NULL, 0,
                       rsp, M02H_DEFAULT_RSP_LEN, NULL);
        if (strstr(rsp, M02H_AT_CMD_SUCCESS_RSP) == NULL) {
            LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
            return -1;
        }
        prsp = strstr(rsp, regrsp);
        if (prsp == NULL) {
            LOGW(TAG, "%s %d NETWORK_REG is not ready. rsp %s\r\n", __func__, __LINE__, rsp);
            return -2;
        }
        prsp += strlen(regrsp);
        if (m02h_rsp_parse_uint(prsp, 2, &netreg_stat) != 0) {
            return -1;
        }
        if (netreg_stat == 1) {
            break;
        } else {
            if (regcmd == AT_CMD_LTE_NETWORK_REG_CHECK) {
                regcmd = AT_CMD_NETWORK_REG_CHECK;
                regrsp = AT_CMD_NETWORK_REG_CHECK_RSP;
                lte_netreg_stat = netreg_stat;
            } else {
                LOGI(TAG, "Network register status: lte_netreg_stat is %d, netreg_stat is %d\r\n", lte_netreg_stat, netreg_stat);
                if (++i < M02H_NETREG_LOOP) {
                    aos_msleep(M02H_NETREG_INTERVAL);
                    goto REGLOOP;
                } else {
                    LOGE(TAG, "%s network register timeout!\r\n", __func__);
                    return -1;
                }
            }
        }
    }

    return 0;
}

static int m02h_lte_active_context(int link_id)
{
    char cmd[M02H_DEFAULT_CMD_LEN] = {0};
    char rsp[M02H_DEFAULT_RSP_LEN] = {0};

    if (strlen(g_apn) == 0) {
        LOGE(TAG, "%s %d g_apn has not inited.\r\n", __func__, __LINE__);
        return -1;
    }
    if (g_ct_stat_bw >> link_id & 0x1) {
        return 0;
    } else if (g_ct_stat_bw == 0) {
        LOGI(TAG, "First active context, check network register...\r\n");
        if (m02h_lte_wait_register() != 0) {
            LOGE(TAG, "%s %d m02h_lte_wait_register failed.\r\n", __func__, __LINE__);
            return -1;
        }
        LOGI(TAG, "network has been registered\r\n");
    }

    snprintf(cmd, M02H_DEFAULT_CMD_LEN - 1, "%s=%d,1,\"%s\",\"\",\"\",1",
            AT_CMD_CONFIGURE_CONTEXT, link2context(link_id), g_apn);
    memset(rsp, 0, M02H_DEFAULT_RSP_LEN);
    at_send_wait_reply(at_dev_fd, cmd, strlen(cmd), true, NULL, 0,
                       rsp, M02H_DEFAULT_RSP_LEN, NULL);
    if (strstr(rsp, M02H_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }

    snprintf(cmd, M02H_DEFAULT_CMD_LEN - 1, "%s=%d", AT_CMD_ACTIVATE_CONTEXT, link2context(link_id));
    memset(rsp, 0, M02H_DEFAULT_RSP_LEN);
    at_send_wait_reply(at_dev_fd, cmd, strlen(cmd), true, NULL, 0,
                       rsp, M02H_DEFAULT_RSP_LEN, NULL);
    if (strstr(rsp, M02H_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }
    g_ct_stat_bw |= 1 << link_id;

    return 0;
}

static int m02h_lte_deactive_context(int link_id)
{
    return 0;
}

static void socket_data_handle(void)
{
    char ch = 0;
    int connectid = 0;
    int linkid = 0;
    int i = 0;
    uint32_t len = 0;
    char *rcv_data = NULL;
    int ret;

    /*eat , char*/
    at_read(at_dev_fd, &ch, 1);

    connectid = (int)at_read_uint(',');
    linkid = connect2link(connectid);
    if (linkid < 0 || linkid >=  M02H_MAX_LINK_NUM) {
        LOGE(TAG, "[Error: %s %d] Invalid link id 0x%02x.", __func__, __LINE__, linkid);
        return -1;
    }
    len = at_read_uint('\r');

    /*eat \n char*/
    at_read(at_dev_fd, &ch, 1);

    rcv_data = (char *)aos_malloc(len);
    if (!rcv_data) {
        LOGE(TAG, "[Error: %s %d] out of memory.", __func__, __LINE__);
        return -1;
    }

    memset(rcv_data, 0, len);
    ret = at_read(at_dev_fd, rcv_data, len);
    if (ret != len) {
        LOGE(TAG, "at read error recv %d want %d!\n", ret, len);
        goto err;
    }

    if (g_netconn_data_input_cb && (g_link[linkid].fd >= 0)) {
        if (g_netconn_data_input_cb(g_link[linkid].fd, rcv_data, len, NULL, 0)) {
            LOGE(TAG, "%s socket %d get data len %d fail to post to sal, drop it\n",
                    __func__, g_link[linkid].fd, len);
        }
    }
    LOGD(TAG, "%s socket data on link %d with length %d posted to sal\n",
            __func__, linkid, len);
err:
    aos_free(rcv_data);
}

static int socket_close_handle(void)
{
    char ch = 0;
    int connectid = 0;
    int linkid = 0;

    /*eat , char*/
    at_read(at_dev_fd, &ch, 1);
    /*eat space char*/
    at_read(at_dev_fd, &ch, 1);

    connectid = (int)at_read_uint('\r');
    linkid = connect2link(connectid);
    if (linkid < 0 || linkid >=  M02H_MAX_LINK_NUM) {
        LOGE(TAG, "[Error: %s %d] Invalid link id 0x%02x.", __func__, __LINE__, linkid);
        return -1;
    }

    if (m02h_lte_deactive_context(linkid) != 0) {
        return -1;
    }

    aos_mutex_lock(&g_link_mutex, AOS_WAIT_FOREVER);
    g_link[linkid].fd = -1;
    aos_mutex_unlock(&g_link_mutex);

    return 0;
}

static int socket_deact_handle(void)
{
    char ch = 0;
    int contextid = 0;
    int linkid = 0;

    /*eat , char*/
    at_read(at_dev_fd, &ch, 1);

    contextid = (int)at_read_uint('\r');
    linkid = context2link(contextid);
    if (linkid < 0 || linkid >=  M02H_MAX_LINK_NUM) {
        LOGE(TAG, "[Error: %s %d] Invalid link id 0x%02x.", __func__, __LINE__, linkid);
        return -1;
    }

    g_ct_stat_bw &= ~(1 << linkid);

    return 0;
}

static void m02h_lte_module_socket_event_handle(void *arg, char *rspinfo, int rsplen)
{
    char ch = 0;
    char str_type[16] = {0};
    char str_hint[M02H_DEFAULT_RSP_LEN] = {0};
    unsigned int len = 0;
    int hint_flag = 0;

    do {
        at_read(at_dev_fd, &ch, 1);
    } while (ch == ' ');

    if (ch == '\"') {
        len = at_read_string(str_type, sizeof(str_type), '\"');
        if (!strcmp(str_type, "recv")) {
            socket_data_handle();
        } else if (!strcmp(str_type, "closed")) {
            socket_close_handle();
        } else if (!strcmp(str_type, "pdpdeact")) {
            socket_deact_handle();
        } else {
            hint_flag = 1;
        }
    } else if (isdigit(ch)) {
        hint_flag = 1;
    } else {
        hint_flag = 1;
    }

    if (hint_flag) {
        str_hint[0] = ch;
        if (str_type[0] != '\0') {
            strcat(str_hint, str_type);
            strcat(str_hint, "\"");
        }
        len = at_read_string(&str_hint[strlen(str_hint)], sizeof(str_hint) - 1, '\r');

        /*eat \n char*/
        at_read(at_dev_fd, &ch, 1);

        LOGW(TAG, "QIURC event: %s", str_hint);
    }
}

#define M02H_RETRY_MAX 50
static int m02h_send_with_retry(const char *cmd, int cmdlen, bool delimiter, const char *data,
                                  int datalen, char *rspbuf, int buflen, const char *expectrsp)
{
    int retry = 0;

    if (NULL == cmd || 0 == cmdlen || NULL == rspbuf ||
        0 == buflen || NULL == expectrsp) {
        LOGE(TAG, "Invalid input %s %d\r\n", __func__, __LINE__);
        return -1;
    }

    while(true) {
        retry++;
        memset(rspbuf, 0, buflen);
        at_send_wait_reply(at_dev_fd, cmd, cmdlen, delimiter, data, datalen, rspbuf, buflen, NULL);
        if (strstr(rspbuf, expectrsp) == NULL) {
            if (retry > M02H_RETRY_MAX) {
                return -1;
            }

            LOGE(TAG, "%s %d cmd %s failed rsp %s retry count %d\r\n", __func__, __LINE__, cmd,rspbuf, retry);
            aos_msleep(1000);
        } else {
            break;
        }
    }

    return  0;
}

static void at_start_test(void *psttimer, void *command)
{
    int ret = 0;
    static int flag = 0;

    if (flag == 0) {
        flag = 1;
    }
    ret = at_send_no_reply(at_dev_fd, command, strlen(command), false);
    if (ret < 0) {
        LOGE(TAG, "uart send command %s at %s %d failed ret is %d \r\n", command, __FILE__, __LINE__, ret);
    }
}

int m02h_uart_selfadaption(const char *command, const char *rsp, uint32_t rsplen)
{
    char *buffer = NULL;
    int   ret    = 0;
    aos_timer_t  test_timer;

    if (NULL == command || NULL == rsp || 0 == rsplen) {
        LOGE(TAG, "invalid input %s %d\r\n", __FILE__, __LINE__);
        return -1;
    }

    buffer = aos_malloc(rsplen * 3 + 1);
    if (NULL == buffer) {
        LOGE(TAG, "fail to malloc memory size %d at %s %d \r\n", rsplen * 3, __FILE__, __LINE__);
        return -1;
    }

    memset(buffer, 0, rsplen * 3 + 1);

    if (m02h_send_with_retry(command, strlen(command), true, NULL, 0,
                               buffer, rsplen * 3 + 1, rsp) < 0) {
        return -1;
    }

    aos_free(buffer);

    return 0;
}

static int m02h_uart_init(void)
{
    int ret = 0;
    char cmd[M02H_DEFAULT_CMD_LEN] = {0};
    char rsp[M02H_DEFAULT_RSP_LEN] = {0};

    /* uart baudrate self adaption*/
    ret = m02h_uart_selfadaption(AT_CMD_TEST, AT_CMD_TEST_RESULT, strlen(AT_CMD_TEST_RESULT));
    if (ret) {
        LOGE(TAG, "m02h_uart_selfadaption fail \r\n");
        return ret;
    }

    /*turn off echo*/
     if (m02h_send_with_retry(AT_CMD_ECHO_OFF, strlen(AT_CMD_ECHO_OFF), true, NULL, 0,
                                rsp, M02H_DEFAULT_RSP_LEN, M02H_AT_CMD_SUCCESS_RSP) < 0) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
     }

    /*set baudrate 115200*/
    snprintf(cmd, M02H_DEFAULT_CMD_LEN - 1, "%s=%d", AT_CMD_BAUDRATE_SET, AT_UART_BAUDRATE);
    at_send_wait_reply(at_dev_fd, cmd, strlen(cmd), true, NULL, 0,
                       rsp, M02H_DEFAULT_RSP_LEN, NULL);
    if (strstr(rsp, M02H_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }

    memset(cmd, 0, M02H_DEFAULT_CMD_LEN);
    memset(rsp, 0, M02H_DEFAULT_RSP_LEN);
    /*turn off flow control*/
    snprintf(cmd, M02H_DEFAULT_CMD_LEN - 1, "%s=%d,%d", AT_CMD_FLOW_CONTROL, 0, 0);
    at_send_wait_reply(at_dev_fd, cmd, strlen(cmd), true, NULL, 0,
                       rsp, M02H_DEFAULT_RSP_LEN, NULL);
    if (strstr(rsp, M02H_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }

    return 0;
}

static int m02h_lte_reset_apn(const char *apn)
{
    char cmd[M02H_DEFAULT_CMD_LEN] = {0};
    char rsp[M02H_DEFAULT_RSP_LEN] = {0};
    char *prsp = NULL;

    snprintf(cmd, M02H_DEFAULT_CMD_LEN - 1, "%s=%d", AT_CMD_PHONE_FUNCTION_LEVEL, 4);
    at_send_wait_reply(at_dev_fd, cmd, strlen(cmd), true, NULL, 0,
                       rsp, M02H_DEFAULT_RSP_LEN, NULL);
    if (strstr(rsp, M02H_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }
    memset(rsp, 0, M02H_DEFAULT_RSP_LEN);
    if (apn) {
        snprintf(cmd, M02H_DEFAULT_CMD_LEN - 1, "%s=%d,\"%s\",\"%s\",,,,,,,,,,,,,,,,,,%d", AT_CMD_DEFAULT_PDP_CONTEXT, 1, "IP", apn, 0);
    } else {
        snprintf(cmd, M02H_DEFAULT_CMD_LEN - 1, "%s=%d,\"%s\",\"\",,,,,,,,,,,,,,,,,,%d", AT_CMD_DEFAULT_PDP_CONTEXT, 1, "IP", 0);
    }
    at_send_wait_reply(at_dev_fd, cmd, strlen(cmd), true, NULL, 0,
                       rsp, M02H_DEFAULT_RSP_LEN, NULL);
    if (strstr(rsp, M02H_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }

    memset(rsp, 0, M02H_DEFAULT_RSP_LEN);
    snprintf(cmd, M02H_DEFAULT_CMD_LEN - 1, "%s=%d,%d", AT_CMD_AUTHENTICATION_PARA, 1, 0);
    at_send_wait_reply(at_dev_fd, cmd, strlen(cmd), true, NULL, 0,
                       rsp, M02H_DEFAULT_RSP_LEN, NULL);
    if (strstr(rsp, M02H_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }
    memset(rsp, 0, M02H_DEFAULT_RSP_LEN);
    snprintf(cmd, M02H_DEFAULT_CMD_LEN - 1, "%s=%d", AT_CMD_PHONE_FUNCTION_LEVEL, 1);
    at_send_wait_reply(at_dev_fd, cmd, strlen(cmd), true, NULL, 0,
                       rsp, M02H_DEFAULT_RSP_LEN, NULL);
    if (strstr(rsp, M02H_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }

    return 0;
}

static int m02h_lte_status_check(void)
{
    const char *regcmd = NULL;
    const char *regrsp = NULL;
    char rsp[M02H_DEFAULT_RSP_LEN] = {0};
    char *prsp = NULL;
    unsigned int netreg_stat = 0;
    unsigned int lte_netreg_stat = 0;
    unsigned int cgatt_state = 0;

    /*sim card status check*/
    at_send_wait_reply(at_dev_fd, AT_CMD_SIM_PIN_CHECK, strlen(AT_CMD_SIM_PIN_CHECK), true,
                       NULL, 0, rsp, M02H_DEFAULT_RSP_LEN, NULL);
    if (strstr(rsp, M02H_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }

    memset(rsp, 0, M02H_DEFAULT_RSP_LEN);
    /*Signal quaility check*/
    at_send_wait_reply(at_dev_fd, AT_CMD_SIGNAL_QUALITY_CHECK, strlen(AT_CMD_SIGNAL_QUALITY_CHECK), true,
                       NULL, 0, rsp, M02H_DEFAULT_RSP_LEN, NULL);
    if (strstr(rsp, M02H_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }
    LOGI(TAG, "signal quality is %s \r\n", rsp);

    /*network registration check*/
    regcmd = AT_CMD_LTE_NETWORK_REG_CHECK;
    regrsp = AT_CMD_LTE_NETWORK_REG_CHECK_RSP;
    while (1) {
        memset(rsp, 0, M02H_DEFAULT_RSP_LEN);
        at_send_wait_reply(at_dev_fd, regcmd, strlen(regcmd), true, NULL, 0,
                       rsp, M02H_DEFAULT_RSP_LEN, NULL);
        if (strstr(rsp, M02H_AT_CMD_SUCCESS_RSP) == NULL) {
            LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
            return -1;
        }
        prsp = strstr(rsp, regrsp);
        if (prsp == NULL) {
            LOGW(TAG, "%s %d NETWORK_REG is not ready. rsp %s\r\n", __func__, __LINE__, rsp);
            return -2;
        }
        prsp += strlen(regrsp);
        if (m02h_rsp_parse_uint(prsp, 2, &netreg_stat) != 0) {
            return -1;
        }
        if (netreg_stat == 1) {
            break;
        } else {
            if (regcmd == AT_CMD_LTE_NETWORK_REG_CHECK) {
                regcmd = AT_CMD_NETWORK_REG_CHECK;
                regrsp = AT_CMD_NETWORK_REG_CHECK_RSP;
                lte_netreg_stat = netreg_stat;
            } else {
                if (netreg_stat != 0 && lte_netreg_stat != 0) {
                    LOGE(TAG, "%s %d network registered failed. rsp %s\r\n", __func__, __LINE__, rsp);
                    return -1;
                } else {
                    LOGW(TAG, "%s %d netreg_stat %u is not OK.\r\n", __func__, __LINE__, netreg_stat);
                    return -2;
                }
            }
        }
    }
    LOGI(TAG, "network registration is %s \r\n", rsp);

    memset(rsp, 0, M02H_DEFAULT_RSP_LEN);
    /*GPRS attach check*/
    at_send_wait_reply(at_dev_fd, AT_CMD_GPRS_ATTACH_CHECK, strlen(AT_CMD_GPRS_ATTACH_CHECK), true,
                       NULL, 0, rsp, M02H_DEFAULT_RSP_LEN, NULL);
    if (strstr(rsp, M02H_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }
    prsp = strstr(rsp, AT_CMD_GPRS_ATTACH_CHECK_RSP);
    if (prsp == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }
    prsp += strlen(AT_CMD_GPRS_ATTACH_CHECK_RSP);
    if (m02h_rsp_parse_uint(prsp, 1, &cgatt_state) != 0) {
        return -1;
    }
    if (cgatt_state != 1) {
        LOGW(TAG, "%s %d cgatt_state %u is not OK.\r\n", __func__, __LINE__, cgatt_state);
        return -2;
    }
    LOGI(TAG, "lte attach check %s \r\n", rsp);

    return 0;
}

static int m02h_lte_get_apn_ip(char *apn, int apn_len, char *ipaddr, int ip_len)
{
    char rsp[M02H_LARGE_RSP_LEN] = {0};
    char *prsp = NULL;
    char *papn = NULL;
    at_reply_config_t atcmdconfig;

    memset(rsp, 0, M02H_LARGE_RSP_LEN);

    atcmdconfig.reply_prefix = AT_CMD_PDP_CONTEXT_RSP;
    atcmdconfig.reply_success_postfix = M02H_AT_CMD_SUCCESS_RSP;
    atcmdconfig.reply_fail_postfix = M02H_AT_CMD_FAIL_RSP;

    at_send_wait_reply(at_dev_fd, AT_CMD_QUERY_PDP_CONTEXT, strlen(AT_CMD_QUERY_PDP_CONTEXT), true, NULL, 0, rsp, M02H_LARGE_RSP_LEN, &atcmdconfig);
    if (strstr(rsp, M02H_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }
    prsp = rsp;
    if (m02h_rsp_parse_string(prsp, 4, ipaddr, ip_len) != 0) {
        return -1;
    }
    if (m02h_rsp_parse_string(prsp, 3, apn, apn_len) != 0) {
        return -1;
    }

    papn = strchr(apn, '.');
    if (papn) {
        *papn = '\0';
    }
    LOGI(TAG, "Current APN is %s \r\n", apn);

    return 0;
}

static int HAL_SAL_Init(void)
{
    int ret = 0;
    uint32_t linknum = 0;
    int i = 0;
    char apn[M02H_APN_LEN] = {0};
    char *papn = NULL;
    char ipaddr[16] = {0};

    if (inited) {
        LOGI(TAG, "m02h lte module have already inited \r\n");
        return 0;
    }

    if (0 != aos_mutex_new(&g_link_mutex)) {
        LOGE(TAG, "Creating link mutex failed (%s %d).", __func__, __LINE__);
        goto err;
    }

    if (0 != aos_mutex_new(&g_domain_mutex)) {
        LOGE(TAG, "Creating link mutex failed (%s %d).", __func__, __LINE__);
        goto err;
    }

    memset(g_link, 0, sizeof(g_link));

    for (linknum = 0; linknum < M02H_MAX_LINK_NUM; linknum++) {
        g_link[linknum].fd = -1;
    }

    ret = m02h_uart_init();
    if (ret) {
        LOGE(TAG, "%s %d failed \r\n", __func__, __LINE__);
        goto err;
    }

    while (1) {
        ret = m02h_lte_reset_apn(papn);
        if (ret != 0) {
           // goto err;
           aos_msleep(10);
           continue;
        }

        i = 0;
        ret = m02h_lte_status_check();
        while (ret == -2 && ++i < M02H_NETREG_LOOP) {
            aos_msleep(M02H_NETREG_INTERVAL);
            ret = m02h_lte_status_check();
        }
        if (ret) {
            LOGE(TAG, "%s %d failed \r\n", __func__, __LINE__);
            goto err;
        }

        if (papn == NULL) {
            ret = m02h_lte_get_apn_ip(apn, M02H_APN_LEN, ipaddr, 16);
            if (ret) {
                LOGE(TAG, "%s %d failed \r\n", __func__, __LINE__);
                goto err;
            }
            papn = apn;
        } else {
            break;
        }
    }

    strcpy(g_apn, papn);
    at_register_callback(at_dev_fd, AT_CMD_OOB_PREFIX, NULL, NULL, 0, m02h_lte_module_socket_event_handle, NULL);

    aos_post_event(EV_WIFI, CODE_WIFI_ON_GOT_IP, 0xdeaddead);
    inited = 1;

    return 0;
err:

    if (aos_mutex_is_valid(&g_link_mutex)) {
        aos_mutex_free(&g_link_mutex);
    }

    if (aos_mutex_is_valid(&g_domain_mutex)) {
        aos_mutex_free(&g_domain_mutex);
    }

    return -1;
}

static int HAL_SAL_Deinit()
{
    if (!inited) {
        return 0;
    }

    if (aos_mutex_is_valid(&g_link_mutex)) {
        aos_mutex_free(&g_link_mutex);
    }

    if (aos_mutex_is_valid(&g_domain_mutex)) {
        aos_mutex_free(&g_domain_mutex);
    }

    inited = 0;

    return 0;
}

static int reinit_m02h(void)
{
    int ret = 0;
    uint32_t linknum = 0;
    int i = 0;
    char apn[M02H_APN_LEN] = {0};
    char *papn = NULL;
    char ipaddr[16] = {0};
    hal_start_count = 0;
    g_ct_stat_bw = 0;
    memset(g_link, 0, sizeof(g_link));

    for (linknum = 0; linknum < M02H_MAX_LINK_NUM; linknum++) {
        g_link[linknum].fd = -1;
    }
    ret = m02h_uart_init();
    if (ret) {
        LOGE(TAG, "%s %d failed \r\n", __func__, __LINE__);
        goto err;
    }

    while (1) {
        ret = m02h_lte_reset_apn(papn);
        if (ret != 0) {
           // goto err;
           aos_msleep(10);
           continue;
        }

        i = 0;
        ret = m02h_lte_status_check();
        while (ret == -2 && ++i < M02H_NETREG_LOOP) {
            aos_msleep(M02H_NETREG_INTERVAL);
            ret = m02h_lte_status_check();
        }
        if (ret) {
            LOGE(TAG, "%s %d failed \r\n", __func__, __LINE__);
            goto err;
        }

        if (papn == NULL) {
            ret = m02h_lte_get_apn_ip(apn, M02H_APN_LEN, ipaddr, 16);
            if (ret) {
                LOGE(TAG, "%s %d failed \r\n", __func__, __LINE__);
                goto err;
            }
            papn = apn;
        } else {
            break;
        }
    }

    strcpy(g_apn, papn);
    return 0;
err:
    return -1;
}

static int m02h_lte_context_sync(void)
{
    char cmd[M02H_DEFAULT_CMD_LEN] = {0};
    char rsp[M02H_LARGE_RSP_LEN] = {0};
    char *prsp = NULL;
    uint32_t real_ct_stat_bw = 0;
    uint32_t context_id = 0;
    int link_id = 0;

    snprintf(cmd, M02H_DEFAULT_CMD_LEN - 1, "%s?", AT_CMD_ACTIVATE_CONTEXT);
    memset(rsp, 0, M02H_LARGE_RSP_LEN);
    at_send_wait_reply(at_dev_fd, cmd, strlen(cmd), true, NULL, 0, rsp, M02H_LARGE_RSP_LEN, NULL);
    if (strstr(rsp, M02H_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }
    prsp = rsp;
    while (1) {
        prsp = strstr(prsp, AT_CMD_ACTIVATE_CONTEXT_RSP);
        if (prsp != NULL) {
            prsp += strlen(AT_CMD_ACTIVATE_CONTEXT_RSP);
            if (m02h_rsp_parse_uint(prsp, 1, &context_id) != 0) {
                LOGE(TAG, "%s %d context_id parse failed rsp %s\r\n", __func__, __LINE__, rsp);
                return -1;
            }
            link_id = context2link(context_id);
            real_ct_stat_bw |= 1 << link_id;
        } else {
            break;
        }
    }
    if (real_ct_stat_bw == 0 && g_ct_stat_bw != 0) {
        LOGI(TAG, "Module may reboot unexpactedly, reinit...\r\n");
        reinit_m02h();
    } else if (g_ct_stat_bw == 0 && real_ct_stat_bw != 0) {
        LOGI(TAG, "Context state is out sync, sync now.\r\n");
        g_ct_stat_bw = real_ct_stat_bw;
    }

    return 0;
}

static int HAL_SAL_DomainToIp(char *domain, char ip[16])
{
    char cmd[M02H_DNS_CMD_RSP_LEN] = {0};
    char rsp[M02H_DNS_CMD_RSP_LEN] = {0};
    char *prsp = NULL;
    int ret = 0;

    if (m02h_lte_context_sync() != 0) {
        return -1;
    }

    snprintf(cmd, M02H_DNS_CMD_RSP_LEN - 1, "%s=\"%s\"", AT_CMD_DOMAIN_TO_IP, domain);
    memset(rsp, 0, M02H_DNS_CMD_RSP_LEN);
    hal_start_count = 0;

    if (m02h_lte_active_context(0) != 0) {
        return -1;
    }
    aos_mutex_lock(&g_domain_mutex, AOS_WAIT_FOREVER);
    at_send_wait_reply(at_dev_fd, cmd, strlen(cmd), true, NULL, 0, rsp, M02H_DNS_CMD_RSP_LEN, NULL);
    if (strstr(rsp, M02H_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        ret = -1;
        goto end;
    }
    prsp = strstr(rsp, AT_CMD_DOMAIN_TO_IP_RSP);
    if (prsp == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        ret = -1;
        goto end;
    }
    prsp += strlen(AT_CMD_DOMAIN_TO_IP_RSP);
    if (m02h_rsp_parse_string(prsp, 1, ip, 16) != 0) {
        ret = -1;
        goto end;
    }

end:
    aos_mutex_unlock(&g_domain_mutex);
    return ret;
}

static int HAL_SAL_Start(sal_conn_t *conn)
{
    int  linkid = 0;
    char *pccmd = NULL;
    char rsp[M02H_DEFAULT_RSP_LEN] = {0};

    if (!inited) {
        LOGE(TAG, "%s m02h lte module haven't init yet \r\n", __func__);
        return -1;
    }

    if (!conn || !conn->addr) {
        LOGE(TAG, "%s %d - invalid input \r\n", __func__, __LINE__);
        return -1;
    }

    aos_mutex_lock(&g_link_mutex, AOS_WAIT_FOREVER);
    for (linkid = 0; linkid < M02H_MAX_LINK_NUM; linkid++) {
        if (g_link[linkid].fd >= 0) {
            continue;
        }
        g_link[linkid].fd = conn->fd;
        break;
    }
    aos_mutex_unlock(&g_link_mutex);

    if (linkid >= M02H_MAX_LINK_NUM) {
        LOGE(TAG, "No link available for now, %s failed. \r\n", __func__);
        return -1;
    }

    pccmd = aos_malloc(M02H_CONN_CMD_LEN);
    if (NULL == pccmd) {
        LOGE(TAG, "fail to malloc %d at %s \r\n", M02H_CONN_CMD_LEN, __func__);
        goto err;
    }
    memset(pccmd, 0, M02H_CONN_CMD_LEN);

    if (m02h_lte_active_context(linkid) != 0) {
        goto err;
    }

    switch (conn->type) {
        case TCP_SERVER:
            snprintf(pccmd, M02H_CONN_CMD_LEN - 1, "%s=%d,%d,\"%s\",\"%s\",%d,%d,%d", AT_CMD_START_SOCKET_SERVICE,
                    link2context(linkid), link2connect(linkid), "TCP LISTENER", "127.0.0.1", 0, conn->l_port, MODE_DIRECT_PUSH);
            at_send_wait_reply(at_dev_fd, pccmd, strlen(pccmd), true, NULL, 0, rsp, M02H_DEFAULT_RSP_LEN, NULL);
            if (strstr(rsp, M02H_AT_CMD_SUCCESS_RSP) == NULL) {
                LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
                goto err;
            }
            break;
        case TCP_CLIENT:
            if (conn->fd != 0)
              hal_start_count++;
            snprintf(pccmd, M02H_CONN_CMD_LEN - 1, "%s=%d,%d,\"%s\",\"%s\",%d,%d,%d", AT_CMD_START_SOCKET_SERVICE,
                    link2context(linkid), link2connect(linkid), "TCP", conn->addr, conn->r_port, 0, MODE_DIRECT_PUSH);
            at_send_wait_reply(at_dev_fd, pccmd, strlen(pccmd), true, NULL, 0, rsp, M02H_DEFAULT_RSP_LEN, NULL);
            if (strstr(rsp, M02H_AT_CMD_SUCCESS_RSP) == NULL) {
                LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
                goto err;
            }
            break;
        case UDP_UNICAST:
            snprintf(pccmd, M02H_CONN_CMD_LEN - 1, "%s=%d,%d,\"%s\",\"%s\",%d,%d,%d", AT_CMD_START_SOCKET_SERVICE,
                    link2context(linkid), link2connect(linkid), "UDP", conn->addr, conn->r_port, 0, MODE_DIRECT_PUSH);
            at_send_wait_reply(at_dev_fd, pccmd, strlen(pccmd), true, NULL, 0, rsp, M02H_DEFAULT_RSP_LEN, NULL);
            if (strstr(rsp, M02H_AT_CMD_SUCCESS_RSP) == NULL) {
                LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
                goto err;
            }
            break;
        case SSL_CLIENT:
        case UDP_BROADCAST:
        default:
            LOGE(TAG, "m02h lte module connect type %d not support \r\n", conn->type);
            goto err;
    }

    aos_free(pccmd);
    return 0;
err:
    m02h_lte_deactive_context(linkid);
    aos_free(pccmd);
    aos_mutex_lock(&g_link_mutex, AOS_WAIT_FOREVER);
    g_link[linkid].fd = -1;
    aos_mutex_unlock(&g_link_mutex);
    return -1;
}

static int HAL_SAL_Close(int fd, int32_t remote_port)
{
    int  linkid = 0;
    int  ret = 0;
    char cmd[M02H_DEFAULT_CMD_LEN] = {0};
    char rsp[M02H_DEFAULT_RSP_LEN] = {0};

    if (!inited) {
        LOGE(TAG, "%s m02h lte module haven't init yet \r\n", __func__);
        return -1;
    }

    linkid = fd_to_linkid(fd);
    if (linkid >= M02H_MAX_LINK_NUM) {
        LOGE(TAG, "No connection found for fd (%d) in %s \r\n", fd, __func__);
        return -1;
    }

    snprintf(cmd, M02H_DEFAULT_CMD_LEN - 1, "%s=%d", AT_CMD_CLOSE_SOCKET_SERVICE, link2connect(linkid));
    at_send_wait_reply(at_dev_fd, cmd, strlen(cmd), true, NULL, 0, rsp, M02H_DEFAULT_RSP_LEN, NULL);
    if (strstr(rsp, M02H_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "cmd %s rsp is %s \r\n", cmd, rsp);
        ret = -1;
    }

    if (m02h_lte_deactive_context(linkid) != 0) {
        ret = -1;
    }

    aos_mutex_lock(&g_link_mutex, AOS_WAIT_FOREVER);
    g_link[linkid].fd = -1;
    aos_mutex_unlock(&g_link_mutex);

    return ret;
}


static int HAL_SAL_Send(int fd,
                 uint8_t *data,
                 uint32_t len,
                 char remote_ip[16],
                 int32_t remote_port,
                 int32_t timeout)
{
    int  linkid;
    char cmd[M02H_DEFAULT_CMD_LEN] = {0};
    char rsp[M02H_DEFAULT_RSP_LEN] = {0};

    if (!inited) {
        LOGE(TAG, "%s m02h lte module haven't init yet \r\n", __func__);
        return -1;
    }

    if (fd == 0 && hal_start_count  > 0) {
        return 0;
    }

    linkid = fd_to_linkid(fd);
    if (linkid >= M02H_MAX_LINK_NUM) {
        LOGE(TAG, "No connection found for fd (%d) in %s \r\n", fd, __func__);
        return -1;
    }

    /* Dual socket temp solution */
    if (M02H_MAX_LINK_NUM != 1) {
        snprintf(cmd, M02H_DEFAULT_CMD_LEN - 1, "%s=%d", AT_CMD_ACTIVATE_CONTEXT, link2context(linkid));
        at_send_wait_reply(at_dev_fd, (const char *)cmd, strlen(cmd), true, NULL, 0,
                            rsp, sizeof(rsp), NULL);
    }

    snprintf(cmd, M02H_DEFAULT_CMD_LEN - 1, "%s=%d,%d", AT_CMD_SEND_DATA, link2connect(linkid), len);
    memset(rsp, 0, M02H_DEFAULT_RSP_LEN);

    /*TODO data send fail rsp is SEND FAIL*/
    at_send_wait_reply(at_dev_fd, (const char *)cmd, strlen(cmd), true, (const char *)data, len,
                        rsp, sizeof(rsp), NULL);
    if (strstr(rsp, M02H_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "cmd %s rsp %s at %s %d failed \r\n", cmd, rsp, __func__, __LINE__);
        return -1;
    }

    return 0;
}

static int HAL_SAL_RegisterNetconnDataInputCb(netconn_data_input_cb_t cb)
{
    if (cb) {
        g_netconn_data_input_cb = cb;
    }
    return 0;
}

static int m02h_sal_add_dev(void* data)
{
    at_config_t at_config = { 0 };

    at_init();

    if(data != NULL)
    {
        sal_device_config_t* config = (sal_device_config_t *)data;
        uart_dev.port  = config->uart_dev.port;
        uart_dev.config.baud_rate    = config->uart_dev.config.baud_rate;
        uart_dev.config.data_width   = config->uart_dev.config.data_width;
        uart_dev.config.parity       = config->uart_dev.config.parity;
        uart_dev.config.stop_bits    = config->uart_dev.config.stop_bits;
        uart_dev.config.flow_control = config->uart_dev.config.flow_control;
        uart_dev.config.mode         = config->uart_dev.config.mode;
    }
    else
    {
        LOGE(TAG, "Error: Uart dev is not configured! Please"
             " provide the config in sal_add_dev() call.\n");
        return -1;
    }

    /* configure and add one uart dev */
    at_config.type                             = AT_DEV_UART;
    at_config.port                             = uart_dev.port;
    at_config.dev_cfg                          = &uart_dev;
    at_config.send_wait_prompt                 = AT_SEND_DATA_WAIT_PROMPT;

    if ((at_dev_fd = at_add_dev(&at_config)) < 0) {
        LOGE(TAG, "AT parser device add failed!\n");
        return -1;
    }

    return 0;
}

static sal_op_t sal_op = {
    .next = NULL,
    .version = "1.0.0",
    .name = "m02h",
    .add_dev = m02h_sal_add_dev,
    .init = HAL_SAL_Init,
    .start = HAL_SAL_Start,
    .send_data = HAL_SAL_Send,
    .domain_to_ip = HAL_SAL_DomainToIp,
    .finish = HAL_SAL_Close,
    .deinit = HAL_SAL_Deinit,
    .register_netconn_data_input_cb = HAL_SAL_RegisterNetconnDataInputCb,
};

/* Generated by aos tools, do NOT modify! */
#ifndef __SAL_DEV_NAME
#error "__SAL_DEV_NAME C mocro not defined, please define it in the SAL device driver."
#endif

int __SAL_DEV_NAME_sal_device_init(void)
{
    return sal_module_register(&sal_op);
}
