/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */


#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include "aos/kernel.h"
#include "ulog/ulog.h"
#include "aos/yloop.h"

#include <atparser.h>
#include <sal_import.h>

#define TAG "sim800_gprs_module"

#define SIM800_AT_CMD_SUCCESS_RSP "OK"
#define SIM800_AT_CMD_FAIL_RSP "ERROR"

#define AT_CMD_TEST "AT"
#define AT_CMD_TEST_RESULT "OK\r\n"

#define AT_CMD_ECHO_OFF   "ATE0"
#define AT_CMD_BAUDRATE_SET "AT+IPR"
#define AT_CMD_FLOW_CONTROL "AT+IFC"

#define AT_CMD_SAVE_CONFIG  "AT&W"

#define AT_CMD_SIM_PIN_CHECK  "AT+CPIN?"
#define AT_CMD_SIGNAL_QUALITY_CHECK "AT+CSQ"
#define AT_CMD_NETWORK_REG_CHECK "AT+CREG?"
#define AT_CMD_GPRS_ATTACH_CHECK "AT+CGATT?"

#define AT_CMD_GPRS_PDP_DEACTIVE "AT+CIPSHUT"
#define AT_CMD_MULTI_IP_CONNECTION "AT+CIPMUX"
#define AT_CMD_SEND_DATA_PROMPT_SET "AT+CIPSPRT"
#define AT_CMD_RECV_DATA_FORMAT_SET "AT+CIPSRIP"

#define AT_CMD_DOMAIN_TO_IP "AT+CDNSGIP"
#define AT_CMD_DOMAIN_RSP   "\r\n+CDNSGIP: "

#define AT_CMD_START_TASK   "AT+CSTT"
#define AT_CMD_BRING_UP_GPRS_CONNECT "AT+CIICR"
#define AT_CMD_GOT_LOCAL_IP "AT+CIFSR"

#define AT_CMD_START_CLIENT_CONN "AT+CIPSTART"
#define AT_CMD_START_TCP_SERVER  "AT+CIPSERVER"

#define AT_CMD_CLIENT_CONNECT_OK "CONNECT OK\r\n"
#define AT_CMD_CLIENT_CONNECT_FAIL "CONNECT FAIL\r\n"

#define AT_CMD_STOP_CONN "AT+CIPCLOSE"

#define AT_CMD_SEND_DATA "AT+CIPSEND"

#define AT_CMD_DATA_RECV "\r\n+RECEIVE,"

#define SIM800_DEFAULT_CMD_LEN    64
#define SIM800_DEFAULT_RSP_LEN    64

#define SIM800_MAX_LINK_NUM       6

#define SIM800_DOMAIN_MAX_LEN     256
#define SIM800_DOMAIN_RSP_MAX_LEN 512
#define SIM800_DOMAIN_CMD_LEN (sizeof(AT_CMD_DOMAIN_TO_IP) + SIM800_DOMAIN_MAX_LEN + 1)

#define SIM800_CONN_CMD_LEN   (SIM800_DOMAIN_MAX_LEN + SIM800_DEFAULT_CMD_LEN)

#define SIM800_RETRY_MAX          50
/* Change to include data slink for each link id respectively. <TODO> */
typedef struct link_s {
    int fd;
    aos_sem_t sem_start;
    aos_sem_t sem_close;
} link_t;

static uint8_t inited = 0;
static link_t g_link[SIM800_MAX_LINK_NUM];
static aos_mutex_t g_link_mutex;
static aos_mutex_t g_domain_mutex;
static aos_sem_t   g_domain_sem;
static char  g_pcdomain_rsp[SIM800_DOMAIN_RSP_MAX_LEN];
static char  g_pccmd[SIM800_CONN_CMD_LEN];
static netconn_data_input_cb_t g_netconn_data_input_cb;

static int fd_to_linkid(int fd)
{
    int link_id;

    if (aos_mutex_lock(&g_link_mutex, AOS_WAIT_FOREVER) != 0) {
        LOGE(TAG, "Failed to lock mutex (%s).", __func__);
        return -1;
    }
    for (link_id = 0; link_id < SIM800_MAX_LINK_NUM; link_id++) {
        if (g_link[link_id].fd == fd) {
            break;
        }
    }

    aos_mutex_unlock(&g_link_mutex);

    return link_id;
}

static void sim800_gprs_domain_rsp_callback(void *arg, char *rspinfo, int rsplen)
{
    if (NULL == rspinfo || rsplen == 0) {
        LOGE(TAG, "invalid input at %s \r\n", __func__);
        return;
    }

    LOGI(TAG, "get domain rsp %s \r\n", rspinfo);
    memcpy(g_pcdomain_rsp, rspinfo, rsplen);
    aos_sem_signal(&g_domain_sem);
    return;
}

static void sim800_gprs_module_socket_data_handle(void *arg, char *rspinfo, int rsplen)
{
    unsigned char uclinkid = 0;
    unsigned char unusesymbol = 0;
    unsigned char datalen[6] = {0};
    unsigned char ipaddr[16] = {0};
    unsigned char port[6] = {0};
    int           i = 0;
    int           j = 0;
    int           len = 0;
    int           remoteport = 0;
    int           linkid = 0;
    char          *recvdata = NULL;

    at_read(&uclinkid, 1);
    linkid = uclinkid - '0';
    if (linkid < 0 || linkid >=  SIM800_MAX_LINK_NUM) {
        LOGE(TAG, "Invalid link id 0x%02x !!!\r\n", linkid);
        return;
    }

    /*eat , char*/
    at_read(&unusesymbol, 1);

    /* get data len */
    i = 0;
    do {
        at_read(&datalen[i], 1);
        if (datalen[i] == ',') {
            break;
        }
        if (i >= sizeof(datalen)) {
            LOGE(TAG, "Too long length of data.datalen is %s \r\n", datalen);
            return;
        }
        if (datalen[i] > '9' || datalen[i] < '0') {
            LOGE(TAG, "Invalid len string!!!, datalen is %s \r\n", datalen);
            return;
        }
        i++;
    } while (1);

    /* len: string to number */
    for (j = 0; j < i; j++) {
        len = len * 10 + datalen[j] - '0';
    }

    /*get ip addr and port*/
    i = 0;
    do {
        at_read(&ipaddr[i], 1);
        if (ipaddr[i] == ':') {
            break;
        }
        if (i >= sizeof(ipaddr)) {
            LOGE(TAG, "Too long length of ipaddr.ipaddr is %s \r\n", ipaddr);
            return;
        }

        if (!((ipaddr[i] <= '9' && ipaddr[i] >= '0') || ipaddr[i] == '.')) {
            LOGE(TAG, "Invalid ipaddr string!!!, ipaddr is %s \r\n", ipaddr);
            return;
        }
        i++;
    } while (1);

    ipaddr[i] = 0;

    i = 0;
    do {
        at_read(&port[i], 1);
        if (port[i] == '\r') {
            break;
        }
        if (i >= sizeof(port)) {
            LOGE(TAG, "Too long length of remote port.port is %s \r\n", port);
            return;
        }

        if (port[i] > '9' || port[i] < '0') {
            LOGE(TAG, "Invalid ipaddr string!!!, port is %s \r\n", port);
            return;
        }
        i++;
    } while (1);

    port[i] = 0;

    /*eat \n char*/
    at_read(&unusesymbol, 1);

    for (j = 0; j < i; j++) {
        remoteport = remoteport * 10 + port[j] - '0';
    }

    /* Prepare socket data */
    recvdata = (char *)aos_malloc(len + 1);
    if (!recvdata) {
        LOGE(TAG, "Error: %s %d out of memory.", __func__, __LINE__);
        return;
    }

    memset(recvdata, 0, len + 1);

    at_read(recvdata, len);

    if (g_netconn_data_input_cb && (g_link[linkid].fd >= 0)) {
        if (g_netconn_data_input_cb(g_link[linkid].fd, recvdata, len, ipaddr, remoteport)) {
            LOGE(TAG, " %s socket %d get data len %d fail to post to sal, drop it\n",
                 __func__, g_link[linkid].fd, len);
        }
    }
    LOGD(TAG, "%s socket data on link %d with length %d posted to sal\n",
         __func__, linkid, len);
    aos_free(recvdata);
    return;
}

static int sim800_send_with_retry(const char *cmd, int cmdlen, bool delimiter, const char *data,
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
        at_send_wait_reply(cmd, cmdlen, delimiter, data, datalen, rspbuf, buflen, NULL);
        if (strstr(rspbuf, expectrsp) == NULL) {
            if (retry > SIM800_RETRY_MAX) {
                return -1;
            }

            LOGE(TAG, "%s %d failed rsp %s retry count %d\r\n", __func__, __LINE__, rspbuf, retry);
            aos_msleep(50);
        } else {
            break;
        }
    }

    return 0;
}

int sim800_uart_selfadaption(const char *command, const char *rsp, uint32_t rsplen)
{
    char buffer[SIM800_DEFAULT_RSP_LEN] = {0};

    if (NULL == command || NULL == rsp || 0 == rsplen) {
        LOGE(TAG, "invalid input %s %d\r\n", __FILE__, __LINE__);
        return -1;
    }

    if (sim800_send_with_retry(command, strlen(command), true, NULL, 0,
                               buffer, SIM800_DEFAULT_RSP_LEN, rsp) < 0) {
        return -1;
    }

    return 0;
}

static int sim800_uart_init(void)
{
    int ret = 0;
    char cmd[SIM800_DEFAULT_CMD_LEN] = {0};
    char rsp[SIM800_DEFAULT_RSP_LEN] = {0};

    /* uart baudrate self adaption*/
    ret = sim800_uart_selfadaption(AT_CMD_TEST, AT_CMD_TEST_RESULT, strlen(AT_CMD_TEST_RESULT));
    if (ret) {
        LOGE(TAG, "sim800_uart_selfadaption fail \r\n");
        return ret;
    }

    /*turn off echo*/
    at_send_wait_reply(AT_CMD_ECHO_OFF, strlen(AT_CMD_ECHO_OFF), true,
                       NULL, 0, rsp, SIM800_DEFAULT_RSP_LEN, NULL);
    if (strstr(rsp, SIM800_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }

    /*set baudrate 115200*/
    snprintf(cmd, SIM800_DEFAULT_CMD_LEN - 1, "%s=%d", AT_CMD_BAUDRATE_SET, AT_UART_BAUDRATE);
    at_send_wait_reply(cmd, strlen(cmd), true, NULL, 0,
                       rsp, SIM800_DEFAULT_RSP_LEN, NULL);
    if (strstr(rsp, SIM800_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }


    memset(cmd, 0, SIM800_DEFAULT_CMD_LEN);
    memset(rsp, 0, SIM800_DEFAULT_RSP_LEN);
    /*turn off flow control*/
    snprintf(cmd, SIM800_DEFAULT_CMD_LEN - 1, "%s=%d,%d", AT_CMD_FLOW_CONTROL, 0, 0);
    at_send_wait_reply(cmd, strlen(cmd), true, NULL, 0,
                       rsp, SIM800_DEFAULT_RSP_LEN, NULL);
    if (strstr(rsp, SIM800_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }

    memset(rsp, 0, SIM800_DEFAULT_RSP_LEN);
    /*save configuration */
    at_send_wait_reply(AT_CMD_SAVE_CONFIG, strlen(AT_CMD_SAVE_CONFIG), true,
                       NULL, 0, rsp, SIM800_DEFAULT_RSP_LEN, NULL);
    if (strstr(rsp, SIM800_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }

    return 0;
}

static int sim800_gprs_status_check(void)
{
    char rsp[SIM800_DEFAULT_RSP_LEN] = {0};

    /*sim card status check*/
    if (sim800_send_with_retry(AT_CMD_SIM_PIN_CHECK, strlen(AT_CMD_SIM_PIN_CHECK), true,
        NULL, 0, rsp, SIM800_DEFAULT_RSP_LEN, SIM800_AT_CMD_SUCCESS_RSP) < 0) {
        LOGE(TAG, "sim card status check failed\n");
        return -1;
    }

    memset(rsp, 0, SIM800_DEFAULT_RSP_LEN);
    /*Signal quaility check*/
    at_send_wait_reply(AT_CMD_SIGNAL_QUALITY_CHECK, strlen(AT_CMD_SIGNAL_QUALITY_CHECK), true,
                       NULL, 0, rsp, SIM800_DEFAULT_RSP_LEN, NULL);
    if (strstr(rsp, SIM800_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }
    LOGI(TAG, "signal quality is %s \r\n", rsp);

    memset(rsp, 0, SIM800_DEFAULT_RSP_LEN);
    /*network registration check*/
    at_send_wait_reply(AT_CMD_NETWORK_REG_CHECK, strlen(AT_CMD_NETWORK_REG_CHECK), true,
                       NULL, 0, rsp, SIM800_DEFAULT_RSP_LEN, NULL);
    if (strstr(rsp, SIM800_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }
    LOGI(TAG, "network registration is %s \r\n", rsp);


    memset(rsp, 0, SIM800_DEFAULT_RSP_LEN);
    /*GPRS attach check*/
    at_send_wait_reply(AT_CMD_GPRS_ATTACH_CHECK, strlen(AT_CMD_GPRS_ATTACH_CHECK),true,
                       NULL, 0, rsp, SIM800_DEFAULT_RSP_LEN, NULL);
    if (strstr(rsp, SIM800_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }
    LOGI(TAG, "gprs attach check %s \r\n", rsp);

    return 0;
}

static int sim800_gprs_ip_init(void)
{
    char cmd[SIM800_DEFAULT_CMD_LEN] = {0};
    char rsp[SIM800_DEFAULT_RSP_LEN] = {0};

    /*Deactivate GPRS PDP Context*/
    if (sim800_send_with_retry(AT_CMD_GPRS_PDP_DEACTIVE, strlen(AT_CMD_GPRS_PDP_DEACTIVE), true,
        NULL, 0, rsp, SIM800_DEFAULT_RSP_LEN, SIM800_AT_CMD_SUCCESS_RSP) < 0) {
        LOGE(TAG, "Deactivate GPRS PDP Context failed\n");
        return -1;
    }

    /*set multi ip connection mode*/
    memset(rsp, 0, SIM800_DEFAULT_RSP_LEN);
    snprintf(cmd, SIM800_DEFAULT_CMD_LEN - 1, "%s=%d", AT_CMD_MULTI_IP_CONNECTION, 1);
    at_send_wait_reply(cmd, strlen(cmd), true, NULL, 0, rsp, SIM800_DEFAULT_RSP_LEN, NULL);
    if (strstr(rsp, SIM800_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }

    /*not prompt echo > when sending data*/
    memset(rsp, 0, SIM800_DEFAULT_RSP_LEN);
    memset(cmd, 0, SIM800_DEFAULT_CMD_LEN);
    snprintf(cmd, SIM800_DEFAULT_CMD_LEN - 1, "%s=%d", AT_CMD_SEND_DATA_PROMPT_SET, 0);
    at_send_wait_reply(cmd, strlen(cmd), true, NULL, 0, rsp, SIM800_DEFAULT_RSP_LEN, NULL);
    if (strstr(rsp, SIM800_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }

    /*Show Remote ip and port when receive data*/
    memset(rsp, 0, SIM800_DEFAULT_RSP_LEN);
    memset(cmd, 0, SIM800_DEFAULT_CMD_LEN);
    snprintf(cmd, SIM800_DEFAULT_CMD_LEN - 1, "%s=%d", AT_CMD_RECV_DATA_FORMAT_SET, 1);
    at_send_wait_reply(cmd, strlen(cmd), true, NULL, 0, rsp, SIM800_DEFAULT_RSP_LEN, NULL);
    if (strstr(rsp, SIM800_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }

    return 0;
}

static void sim800_get_ip_delayed_action(void *arg)
{
    LOGD(TAG, "post got ip event \r\n");
    aos_post_event(EV_WIFI, CODE_WIFI_ON_GOT_IP, 0xdeaddead);
}


static int sim800_gprs_got_ip(void)
{
    char rsp[SIM800_DEFAULT_RSP_LEN] = {0};
    atcmd_config_t atcmd_config = {NULL, AT_RECV_PREFIX, NULL};

    /*start gprs stask*/
    if (sim800_send_with_retry(AT_CMD_START_TASK, strlen(AT_CMD_START_TASK), true,
        NULL, 0, rsp, SIM800_DEFAULT_RSP_LEN, SIM800_AT_CMD_SUCCESS_RSP) < 0) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }

    /*bring up wireless connectiong with gprs*/
    memset(rsp, 0, SIM800_DEFAULT_RSP_LEN);
    at_send_wait_reply(AT_CMD_BRING_UP_GPRS_CONNECT, strlen(AT_CMD_BRING_UP_GPRS_CONNECT), true,
                       NULL, 0, rsp, SIM800_DEFAULT_RSP_LEN, NULL);
    if (strstr(rsp, SIM800_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
    }

    /*try to got ip*/
    memset(rsp, 0, SIM800_DEFAULT_RSP_LEN);
    at_send_wait_reply(AT_CMD_GOT_LOCAL_IP, strlen(AT_CMD_GOT_LOCAL_IP), true,
                       NULL, 0, rsp, SIM800_DEFAULT_RSP_LEN, &atcmd_config);
    if (strstr(rsp, SIM800_AT_CMD_FAIL_RSP) != NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
    }

    /*delay 5 seconds to post got ip event*/
    aos_post_delayed_action(5000, sim800_get_ip_delayed_action, NULL);
    return 0;
}

static int sim800_gprs_get_ip_only()
{
    char rsp[SIM800_DEFAULT_RSP_LEN] = {0};
    atcmd_config_t atcmd_config = {NULL, AT_RECV_PREFIX, NULL};
    at_send_wait_reply(AT_CMD_GOT_LOCAL_IP, strlen(AT_CMD_GOT_LOCAL_IP), true,
                       NULL, 0, rsp, SIM800_DEFAULT_RSP_LEN, &atcmd_config);
    if (strstr(rsp, SIM800_AT_CMD_FAIL_RSP) != NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }
    return 0;
}

#define AT_CMD_GETENGMODE   "AT+CENG?"
#define AT_CMD_SETENGMODE   "AT+CENG=3,1"
#define AT_CMD_CLOSEENGMODE "AT+CENG=0,0"

#define AT_CMD_GETENGMODE_LEN  512
void sim800_get_gprs_network_info(char * bts, int btslen, char * neighterbts, int nbtslen)
{
    char rsp[AT_CMD_GETENGMODE_LEN] = {0};

    memset(bts,         0, btslen);
    memset(neighterbts, 0, nbtslen);

    if (!inited)
    {
        LOGE(TAG, "%s sim800 gprs module haven't init yet \r\n", __func__);
    }

    /* set engineering mode to engineering mode with neighbering cell info */
    memset(rsp, 0, sizeof(rsp));
    at_send_wait_reply(AT_CMD_SETENGMODE, strlen(AT_CMD_SETENGMODE), true,
                       NULL, 0, rsp, AT_CMD_GETENGMODE_LEN, NULL);

    if (strstr(rsp, SIM800_AT_CMD_FAIL_RSP) != NULL)
    {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }

    /* get cell info */

    memset(rsp, 0, sizeof(rsp));
    at_send_wait_reply(AT_CMD_GETENGMODE, strlen(AT_CMD_GETENGMODE), true,
                       NULL, 0, rsp, AT_CMD_GETENGMODE_LEN, NULL);

    if (strstr(rsp, SIM800_AT_CMD_FAIL_RSP) != NULL)
    {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }

    LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);

    char cellinfo_prefix_buf[64] = {"+CENG: 0,"};

    char * p_start = strstr(rsp, cellinfo_prefix_buf);

    if (p_start != NULL)
    {
        int cell = -1;
        int mcc  = 0;
        int mnc;
        int lac;
        int cellid;
        int bsic;
        int rxl;

        /* <cell>,<mcc>,<mnc>,<lac>,<cellid>,<bsic>,<rxl> */
        sscanf(p_start,
               "+CENG: %d,\"%d,%d,%x,%x,%d,%d\"",
               &cell, &mcc, &mnc, &lac, &cellid, &bsic, &rxl);

        if (cell != 0)
        {
            LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
            return -1;
        }

        int signal;
        int first_neighber = 1;

        while (mcc != 0)
        {
            signal = rxl - 113; /* signal strength = 2 * rxl - 113 dbm */

            /* -113 < signal < 0 */
            signal = signal > 0 ? 0 : (signal < (-113) ? -113 : signal);

            if (cell == 0)
            {
                /* mcc,mnc,lac,cellid,signal */
                snprintf(bts, btslen, "%d,%d,%d,%d,%d", mcc, mnc, lac, cellid, signal);
            }
            else
            {
                if (first_neighber)
                {
                    /* mcc,mnc,lac,cellid,signal */
                    snprintf(neighterbts, nbtslen, "%d,%d,%d,%d,%d", mcc, mnc, lac, cellid, signal);
                    first_neighber = 0;
                }
                else
                {
                    /* mcc,mnc,lac,cellid,signal */
                    snprintf(neighterbts + strlen(neighterbts), nbtslen - strlen(neighterbts), "|%d,%d,%d,%d,%d", mcc, mnc, lac, cellid, signal);
                }
            }

            memset(cellinfo_prefix_buf, 0, sizeof(cellinfo_prefix_buf));
            sprintf(cellinfo_prefix_buf, "+CENG: %d,", cell + 1);

            p_start = strstr(p_start, cellinfo_prefix_buf);

            if (p_start == NULL)
            {
                break;
            }

            mcc = 0;
            sscanf(p_start,
                   "+CENG: %d,\"%d,%d,%x,%x,%d,%d\"",
                   &cell, &mcc, &mnc, &lac, &cellid, &bsic, &rxl);
        }

    }

    /* close engineering mode */
    memset(rsp, 0, sizeof(rsp));
    at_send_wait_reply(AT_CMD_CLOSEENGMODE, strlen(AT_CMD_CLOSEENGMODE), true,
                       NULL, 0, rsp, AT_CMD_GETENGMODE_LEN, NULL);

    if (strstr(rsp, SIM800_AT_CMD_FAIL_RSP) != NULL)
    {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }

    /* restore original engineering mode */
}

#define AT_CMD_GPS_DEFAULT_RSP_LEN         (256)
#define AT_CMD_GPS_POWER_OFF         "AT+CGNSPWR=0"
#define AT_CMD_GPS_POWER_ON          "AT+CGNSPWR=1"
#define AT_CMD_GPS_POWER_CHECK       "AT+CGNSPWR?"

#define AT_CMD_GPS_INTERVAL_CLOSE    "AT+CGNSURC=0"

#define AT_CMD_GPS_LASTPARSE_SET     "AT+CGNSSEQ=\"RMC\""
#define AT_CMD_GPS_POSITION_GET      "AT+CGNSINF"
#define GPS_TYPE_NAME_LEN            (16)
#define GET_GPS_INFO_MIN_NUM         (6)

void sim800_get_gps(float * latitude, float * longitude, float * altitude)
{
    int ret = 0;
    char rsp[AT_CMD_GPS_DEFAULT_RSP_LEN] = {0};

    *latitude = 90; /* north pole */
    *altitude = 0;

    if (!inited) {
        return;
    }

    memset(rsp, 0, sizeof(rsp));
    ret = at_send_wait_reply(AT_CMD_GPS_POWER_ON, strlen(AT_CMD_GPS_POWER_ON),
                             true, NULL, 0, rsp, AT_CMD_GPS_DEFAULT_RSP_LEN, NULL);

    if ((0 != ret) || (strstr(rsp, SIM800_AT_CMD_SUCCESS_RSP) == NULL))
    {
        LOGE(TAG, "%s %d failed rsp %s errno %d\r\n", __func__, __LINE__, rsp,ret);
        return;
    }

    memset(rsp, 0, sizeof(rsp));
    ret = at_send_wait_reply(AT_CMD_GPS_LASTPARSE_SET, strlen(AT_CMD_GPS_LASTPARSE_SET),
                             true, NULL, 0, rsp, AT_CMD_GPS_DEFAULT_RSP_LEN, NULL);

    if ((0 != ret) || (strstr(rsp, SIM800_AT_CMD_SUCCESS_RSP) == NULL))
    {
        LOGE(TAG, "%s %d failed rsp %s errno %d\r\n", __func__, __LINE__, rsp,ret);
        return;
    }

    memset(rsp, 0, sizeof(rsp));
    ret = at_send_wait_reply(AT_CMD_GPS_INTERVAL_CLOSE, strlen(AT_CMD_GPS_INTERVAL_CLOSE),
                             true, NULL, 0, rsp, AT_CMD_GPS_DEFAULT_RSP_LEN, NULL);

    if ((0 != ret) || (strstr(rsp, SIM800_AT_CMD_SUCCESS_RSP) == NULL))
    {
        LOGE(TAG, "%s %d failed rsp %s errno %d\r\n", __func__, __LINE__, rsp,ret);
        return;
    }

    memset(rsp, 0, sizeof(rsp));
    ret = at_send_wait_reply(AT_CMD_GPS_POSITION_GET, strlen(AT_CMD_GPS_POSITION_GET),
                             true, NULL, 0, rsp, AT_CMD_GPS_DEFAULT_RSP_LEN, NULL);

    if ((0 != ret) || (strstr(rsp, SIM800_AT_CMD_SUCCESS_RSP) == NULL))
    {
        LOGE(TAG, "%s %d failed rsp %s errno %d\r\n", __func__, __LINE__, rsp,ret);
        return;
    }

    LOGI(TAG, "%s %d rsp %s errno %d\r\n", __func__, __LINE__, rsp,ret);


    /* +CGNSINF: <GNSS run status>,<Fix status>,
                 <UTC date & Time>,<Latitude>,<Longitude>, <MSL Altitude>,
                 <Speed Over Ground>, <Course Over Ground>,
                 <Fix Mode>,<Reserved1>,<HDOP>,
                 <PDOP>, <VDOP>,<Reserved2>,
                 <GNSS Satellites in View>, <GNSS Satellites Used>,
                 <GLONASS Satellites Used>,<Reserved3>,<C/N0 max>,<HPA>,<VPA> */

    char tmp_buf[128];
    char tmp_lat[32];
    char tmp_log[32];
    char tmp_alt[32];

    memset(tmp_lat, 0, sizeof(tmp_lat));
    memset(tmp_log, 0, sizeof(tmp_log));
    memset(tmp_alt, 0, sizeof(tmp_alt));

    ret = sscanf(rsp, "%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,]",
                 tmp_buf, tmp_buf, tmp_buf, tmp_lat, tmp_log, tmp_alt, tmp_buf);

    if (ret >= GET_GPS_INFO_MIN_NUM)
    {
        *latitude  = (float)atof(tmp_lat);
        *longitude = (float)atof(tmp_log);
        *altitude  = (float)atof(tmp_alt);
        *altitude = *altitude > 0 ? *altitude : 0;
    }
}
int HAL_SAL_Init(void)
{
    int ret = 0;
    uint32_t linknum = 0;

    if (inited) {
        LOGI(TAG, "sim800 gprs module have already inited \r\n");
        return 0;
    }

    memset(g_pcdomain_rsp , 0, SIM800_DOMAIN_RSP_MAX_LEN);

    if (0 != aos_mutex_new(&g_link_mutex)) {
        LOGE(TAG, "Creating link mutex failed (%s %d).", __func__, __LINE__);
        goto err;
    }

    if (0 != aos_mutex_new(&g_domain_mutex)) {
        LOGE(TAG, "Creating link mutex failed (%s %d).", __func__, __LINE__);
        goto err;
    }

    if (0 != aos_sem_new(&g_domain_sem, 0)) {
        LOGE(TAG, "Creating domain mutex failed (%s %d).", __func__, __LINE__);
        goto err;
    }

    memset(g_link, 0, sizeof(g_link));

    for (linknum = 0; linknum < SIM800_MAX_LINK_NUM; linknum++) {
        g_link[linknum].fd = -1;
    }
    ret = sim800_uart_init();
    if (ret) {
        LOGE(TAG, "%s %d failed \r\n", __func__, __LINE__);
        goto err;
    }

    ret = sim800_gprs_status_check();
    if (ret) {
        LOGE(TAG, "%s %d failed \r\n", __func__, __LINE__);
        goto err;
    }

    ret = sim800_gprs_ip_init();
    if (ret) {
        LOGE(TAG, "%s %d failed \r\n", __func__, __LINE__);
        goto err;
    }

    /* reg oob for domain and packet input*/
    at_register_callback(AT_CMD_DOMAIN_RSP, AT_RECV_PREFIX, SIM800_DOMAIN_RSP_MAX_LEN,
                         sim800_gprs_domain_rsp_callback, NULL);
    at_register_callback(AT_CMD_DATA_RECV, NULL, 0, sim800_gprs_module_socket_data_handle, NULL);
    ret = sim800_gprs_got_ip();
    if (ret) {
        LOGE(TAG, "%s %d failed \r\n", __func__, __LINE__);
        goto err;
    }

    inited = 1;

    return 0;
err:
    if (aos_mutex_is_valid(&g_link_mutex)) {
        aos_mutex_free(&g_link_mutex);
    }

    if (aos_mutex_is_valid(&g_domain_mutex)) {
        aos_mutex_free(&g_domain_mutex);
    }

    if (aos_sem_is_valid(&g_domain_sem)) {
        aos_sem_free(&g_domain_sem);
    }

    return -1;
}

int HAL_SAL_Deinit()
{
    if (!inited) {
        return 0;
    }

    aos_mutex_free(&g_link_mutex);
    inited = 0;
    return 0;
}

int HAL_SAL_DomainToIp(char *domain, char ip[16])
{
    char *pccmd = NULL;
    char *head = NULL;
    char *end = NULL;
    int count = 0;
    char rsp[SIM800_DEFAULT_RSP_LEN] = {0};

    if (!inited) {
        LOGE(TAG, "%s sim800 gprs module haven't init yet \r\n", __func__);
        return -1;
    }

    if (NULL == domain || NULL == ip) {
        LOGE(TAG, "invalid input at %s \r\n", __func__);
        return -1;
    }

    if (strlen(domain) > SIM800_DOMAIN_MAX_LEN) {
        LOGE(TAG, "domain length oversize at %s \r\n", __func__);
        return -1;
    }

    pccmd = g_pccmd;
    if (NULL == pccmd) {
        LOGE(TAG, "fail to malloc memory %d at %s \r\n", SIM800_DOMAIN_CMD_LEN, __func__);
        return -1;
    }

    memset(pccmd, 0, SIM800_DOMAIN_CMD_LEN);
    snprintf(pccmd, SIM800_DEFAULT_CMD_LEN - 1, "%s=%s", AT_CMD_DOMAIN_TO_IP, domain);

    aos_mutex_lock(&g_domain_mutex, AOS_WAIT_FOREVER);
restart:
    count++;
    if (count > SIM800_RETRY_MAX) {
        LOGE(TAG,"domain to ip retry failed!\r\n");
        HAL_MutexUnlock(g_domain_mutex);
        return -1;
    }

    at_send_wait_reply(pccmd, strlen(pccmd), true, NULL, 0, rsp, SIM800_DEFAULT_RSP_LEN, NULL);
    if (strstr(rsp, SIM800_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        goto err;
    }

    /*TODO wait for reponse for ever for now*/
    aos_sem_wait(&g_domain_sem, AOS_WAIT_FOREVER);
    /*
     * formate is :
       +CDNSGIP: 1,"www.baidu.com","183.232.231.173","183.232.231.172"
       or :
       +CDNSGIP: 0,8
    */
    if ((head = strstr(g_pcdomain_rsp, domain)) == NULL) {
        LOGE(TAG, "invalid domain rsp %s at %d\r\n", g_pcdomain_rsp, __LINE__);
        goto err;
    }

    head += (strlen(domain) + 3);
    if ((end = strstr(head, "\"")) == NULL) {
        LOGE(TAG, "invalid domain rsp head is %s at %d\r\n", head, __LINE__);
        goto err;
    }

    if ((end - head) > 15 || (end - head) < 7) {
        LOGE(TAG, "invalid domain rsp head is %s at %d\r\n", head, __LINE__);
        goto err;
    }

    /* We find a good IP, save it. */
    memcpy(ip, head, end - head);
    ip[end - head] = '\0';
    memset(g_pcdomain_rsp, 0, SIM800_DOMAIN_RSP_MAX_LEN);
    aos_mutex_unlock(&g_domain_mutex);

    LOGD(TAG, "domain %s get ip %s \r\n", domain , ip);
    return 0;
err:
    if (sim800_gprs_get_ip_only() != 0) {
        sim800_gprs_ip_init();
        sim800_gprs_got_ip();
        goto restart;
    }

    memset(g_pcdomain_rsp, 0, SIM800_DOMAIN_RSP_MAX_LEN);
    aos_mutex_unlock(&g_domain_mutex);

    return -1;
}

int HAL_SAL_Start(sal_conn_t *conn)
{
    int  linkid = 0;
    char *pccmd = NULL;
    char rsp[SIM800_DEFAULT_RSP_LEN] = {0};

    if (!inited) {
        LOGE(TAG, "%s sim800 gprs module haven't init yet \r\n", __func__);
        return -1;
    }

    if (!conn || !conn->addr) {
        LOGE(TAG, "%s %d - invalid input \r\n", __func__, __LINE__);
        return -1;
    }

    aos_mutex_lock(&g_link_mutex, AOS_WAIT_FOREVER);
    for (linkid = 0; linkid < SIM800_MAX_LINK_NUM; linkid++) {
        if (g_link[linkid].fd >= 0) {
            continue;
        }
        g_link[linkid].fd = conn->fd;
        break;
    }
    aos_mutex_unlock(&g_link_mutex);

    if (linkid >= SIM800_MAX_LINK_NUM) {
        LOGE(TAG, "No link available for now, %s failed. \r\n", __func__);
        return -1;
    }

    pccmd = g_pccmd;
    if (NULL == pccmd) {
        LOGE(TAG, "fail to malloc %d at %s \r\n", SIM800_CONN_CMD_LEN, __func__);
        goto err;
    }
    memset(pccmd, 0, SIM800_CONN_CMD_LEN);

    switch (conn->type) {
        case TCP_SERVER:
            snprintf(pccmd, SIM800_CONN_CMD_LEN - 1, "%s=%d,%d", AT_CMD_START_TCP_SERVER, 1, conn->l_port);
            at_send_wait_reply(pccmd, strlen(pccmd), true, NULL, 0, rsp, SIM800_DEFAULT_RSP_LEN, NULL);
            if (strstr(rsp, SIM800_AT_CMD_SUCCESS_RSP) == NULL) {
                LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
                goto err;
            }
            break;
        case TCP_CLIENT:
            snprintf(pccmd, SIM800_CONN_CMD_LEN - 1, "%s=%d,\"TCP\",\"%s\",%d", AT_CMD_START_CLIENT_CONN, linkid, conn->addr,
                     conn->r_port);
            atcmd_config_t atcmd_config_client = { NULL, AT_CMD_CLIENT_CONNECT_OK, AT_CMD_CLIENT_CONNECT_FAIL};
            at_send_wait_reply(pccmd, strlen(pccmd), true, NULL, 0, rsp, SIM800_DEFAULT_RSP_LEN, &atcmd_config_client);
            if (strstr(rsp, AT_CMD_CLIENT_CONNECT_FAIL) != NULL) {
                LOGE(TAG, "pccmd %s fail, rsp %s \r\n", pccmd, rsp);
                goto err;
            }
            break;
        case UDP_UNICAST:
            snprintf(pccmd, SIM800_CONN_CMD_LEN - 1, "%s=%d,\"UDP\",\"%s\",%d", AT_CMD_START_CLIENT_CONN, linkid, conn->addr,
                     conn->r_port);
            atcmd_config_t atcmd_config_unicast = { NULL, AT_CMD_CLIENT_CONNECT_OK, AT_CMD_CLIENT_CONNECT_FAIL};
            at_send_wait_reply(pccmd, strlen(pccmd), true, NULL, 0, rsp, SIM800_DEFAULT_RSP_LEN, &atcmd_config_unicast);
            if (strstr(rsp, AT_CMD_CLIENT_CONNECT_FAIL) != NULL) {
                LOGE(TAG, "pccmd %s fail, rsp %s \r\n", pccmd, rsp);
                goto err;
            }
            break;
        case SSL_CLIENT:
        case UDP_BROADCAST:
        default:
            LOGE(TAG, "sim800 gprs module connect type %d not support \r\n", conn->type);
            goto err;
    }

    return 0;
err:
    aos_mutex_lock(&g_link_mutex, AOS_WAIT_FOREVER);
    g_link[linkid].fd = -1;
    aos_mutex_unlock(&g_link_mutex);
    return -1;
}

int HAL_SAL_Close(int fd, int32_t remote_port)
{
    int  linkid = 0;
    int  ret = 0;
    char cmd[SIM800_DEFAULT_CMD_LEN] = {0};
    char rsp[SIM800_DEFAULT_RSP_LEN] = {0};

    if (!inited) {
        LOGE(TAG, "%s sim800 gprs module haven't init yet \r\n", __func__);
        return -1;
    }

    linkid = fd_to_linkid(fd);
    if (linkid < 0 || linkid >= SIM800_MAX_LINK_NUM) {
        LOGE(TAG, "No connection found for fd (%d) in %s \r\n", fd, __func__);
        return -1;
    }

    snprintf(cmd, SIM800_DEFAULT_CMD_LEN - 1, "%s=%d", AT_CMD_STOP_CONN, linkid);
    at_send_wait_reply(cmd, strlen(cmd), true, NULL, 0, rsp, SIM800_DEFAULT_RSP_LEN, NULL);
    if (strstr(rsp, SIM800_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "cmd %s rsp is %s \r\n", cmd, rsp);
        ret = -1;
    }

    aos_mutex_lock(&g_link_mutex, AOS_WAIT_FOREVER);
    g_link[linkid].fd = -1;
    aos_mutex_unlock(&g_link_mutex);

    return ret;
}

int HAL_SAL_Send(int fd,
                 uint8_t *data,
                 uint32_t len,
                 char remote_ip[16],
                 int32_t remote_port,
                 int32_t timeout)
{
    int  linkid;
    char cmd[SIM800_DEFAULT_CMD_LEN] = {0};
    char rsp[SIM800_DEFAULT_RSP_LEN] = {0};

    if (!inited) {
        LOGE(TAG, "%s sim800 gprs module haven't init yet \r\n", __func__);
        return -1;
    }

    linkid = fd_to_linkid(fd);
    if (linkid < 0 || linkid >= SIM800_MAX_LINK_NUM) {
        LOGE(TAG, "No connection found for fd (%d) in %s \r\n", fd, __func__);
        return -1;
    }

    snprintf(cmd, SIM800_DEFAULT_CMD_LEN - 1, "%s=%d,%d", AT_CMD_SEND_DATA, linkid, len);

    /*TODO data send fail rsp is SEND FAIL*/
    if (sim800_send_with_retry((const char *)cmd, strlen(cmd), true, (const char *)data, len,
        rsp, sizeof(rsp), SIM800_AT_CMD_SUCCESS_RSP) < 0) {
        LOGE(TAG, "cmd %s rsp %s at %s %d failed \r\n", cmd, rsp, __func__, __LINE__);
        return -1;
    }

    return 0;
}

int HAL_SAL_RegisterNetconnDataInputCb(netconn_data_input_cb_t cb)
{
    if (cb) {
        g_netconn_data_input_cb = cb;
    }
    return 0;
}

int sal_device_init(void)
{
    at_init();
    return 0;
}
