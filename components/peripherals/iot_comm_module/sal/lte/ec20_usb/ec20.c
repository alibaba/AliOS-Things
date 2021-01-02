/*
 * Copyright (C) 2020-2022 Alibaba Group Holding Limited
 */


#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include "aos/kernel.h"

#include "ulog/ulog.h"
#include <atcmd_config_module.h>

#include <at/at.h>
#include <hal_sal.h>

#define TAG "ec20_sal_module"

typedef enum {
    WAIT_NONE = 0,
    WAIT_SIMPLE_RSP,    /* OK or Error */
    WAIT_PIN_RSP,
    WAIT_REG_RSP,
    WAIT_SIGQ_RSP,
    WAIT_ATTCH_RSP,
    WAIT_OPEN_RSP,
    WAIT_CONN_RSP,
    WAIT_SEND_PMT,
    WAIT_DISC_RSP,
    WAIT_IP_RSP,
    WAIT_DOMAIN_RSP,
} ec20_sal_send_state_t;

typedef enum {
    RSP_ERROR  = -1,
    RSP_OK     = 0,
    RSP_PROMPT = 1,
} ec20_sal_rsp_val_t;

typedef struct link_s {
    int fd;
} link_t;

#define AT_WAIT_MAX_TIMEOUT            60*1000
#define AT_WAIT_DEF_TIMEOUT            3*1000
#define AT_WAIT_MIN_TIMEOUT            800

#define AT_CMD_RETRY_MAX               20
#define AT_CMD_PREFIX_MAX_LEN          32
#define EC20_MAX_LINK_NUM              12
#define AT_CMD_MAX_LEN                 256

#define AT_EC20_AT_RSP_OK              "OK"
#define AT_EC20_AT_RSP_ERROR           "ERROR"
#define AT_EC20_PUB_PROMPT             "\r\n>"

#define AT_EC20_NET_CMD_PREFIX         "+C"

#define AT_EC20_QI_PREFIX              "+Q"

#define AT_CMD_TCP_CLOSE               "AT+QICLOSE"
#define AT_CMD_TCP_CONNECT             "AT+QIOPEN"
#define AT_CMD_TCP_SEND                "AT+QISEND"

#define AT_CMD_IP_QUERY                "AT+QIACT?\r"
#define AT_CMD_DOMAIN_2_IP             "AT+QIDNSGIP"

#define AT_CMD_PDP_DEACT               "+QIDEACT"
#define AT_CMD_PDP_CONFIG              "+QICSGP"
#define AT_CMD_PDP_ACT                 "+QIACT"

#define AT_CMD_SIM_PIN_CHECK            "AT+CPIN?\r"
#define AT_CMD_SIGNAL_QUALITY_CHECK     "AT+CSQ\r"
#define AT_CMD_NETWORK_REG_CHECK        "AT+CGREG?\r"
#define AT_CMD_ATTACH_CHECK             "AT+CGATT?\r"
#define AT_CMD_ECHO_OFF                 "ATE0\r"
#define AT_CMD_AT                       "AT\r"

static usb_dev_t ec20_at_usb_dev;
static int ec20_at_dev_fd = -1;
static int ec20_init_flag = 0;
static aos_sem_t st_ec20_atcmdsend_sem4 = {0};
static aos_mutex_t g_link_mutex;

static volatile ec20_sal_send_state_t   en_ec20_send_state = WAIT_NONE;
static volatile ec20_sal_rsp_val_t      en_ec20_rsp = RSP_ERROR;

static char ec20_at_cmd_buf[AT_CMD_MAX_LEN];
static char ec20_domain_2_ip[16];
static int  ec20_contextid = 1;
static link_t g_link[EC20_MAX_LINK_NUM];

static netconn_data_input_cb_t g_netconn_data_input_cb;

typedef int32_t (*at_data_check_cb_t)(char data);

static int prepare_network_process(void);

static int32_t ec20_atcmd_num_check(char data)
{
    if ((data > '9' || data < '0') && (data != '.')
        && (data != '"') && (data != '-')) {
        return -1;
    }

    return 0;
}

static int32_t ec20_atcmd_info_get(char *buf, uint32_t buflen,
                               at_data_check_cb_t valuecheck)
{
    uint32_t i = 0;

    if (NULL == buf || 0 == buflen) {
        return -1;
    }

    do {
        if (at_read(ec20_at_dev_fd,&buf[i], 1) < 1) {
            LOGE(TAG, "atcmd_read failed");
            return -1;
        }

        if (buf[i] == ',') {
            buf[i] = 0;
            break;
        } else if (buf[i] == '\r') {
            LOGD(TAG, "delimiter found\n");
            buf[i] = 0;
            return 1;
        }

        if (i >= buflen) {
            LOGE(TAG, "Too long length of data.reader is %s \r\n", buf);
            return -1;
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


static int32_t ec20_atcmd_read_out_string(int8_t *buffer, int32_t size, const int8_t *postfix)
{
    int8_t single;
    int32_t len = 0;

    /* read out prefix */
    do {
        if (at_read(ec20_at_dev_fd, &single, 1) < 0) {
            LOGE(TAG, "at read failed!");
            return -1;
        }

        buffer[len++] = single;
        if (len >= size) {
            LOGE(TAG, "Not found!");
            return -1;
        }

        if (len >= strlen(postfix) &&
            memcmp(postfix, buffer + len - strlen(postfix),
                   strlen(postfix)) == 0) {
            break;
        }
    } while(1);

    return len;
}


static int32_t ec20_atcmd_send_wait(uint32_t timeout_ms)
{
    return aos_sem_wait(&st_ec20_atcmdsend_sem4, timeout_ms);
}

static void ec20_atcmd_send_post(void)
{
    aos_sem_signal(&st_ec20_atcmdsend_sem4);
}

static void ec20_atcmd_sal_send_state_set(ec20_sal_send_state_t state)
{
    en_ec20_send_state = state;
}

static ec20_sal_send_state_t ec20_atcmd_get_send_state(void)
{
    return en_ec20_send_state;
}

static bool ec20_atcmd_send_state_ready(void)
{
    return (en_ec20_send_state == WAIT_NONE);
}

ec20_sal_rsp_val_t ec20_atcmd_get_ret_val(void)
{
    return en_ec20_rsp;
}

void ec20_atcmd_set_ret_val(ec20_sal_rsp_val_t ret)
{
    en_ec20_rsp = ret;
}

static int32_t ec20_atcmd_send_wait_ret_val(const char *data, int datalen,
                                  ec20_sal_send_state_t state, int timeout_ms) 
{
    int ret = 0;

    if (!ec20_atcmd_send_state_ready()) {
        LOGE(TAG, "Not in ready state\r\n");
        return -1;
    }

    ec20_atcmd_sal_send_state_set(state);

    ret = at_send_no_reply(ec20_at_dev_fd, data, datalen, false);
    if (ret < 0) {
        ec20_atcmd_sal_send_state_set(WAIT_NONE);
        LOGE(TAG, "state %d at send failed", state);
        return -1;
    }

    if (timeout_ms > 0) { 
        if (0 == ec20_atcmd_send_wait(timeout_ms)) {
            ret = ec20_atcmd_get_ret_val();
        } else {
            ret = -1;
        }
    }
    
    ec20_atcmd_sal_send_state_set(WAIT_NONE);

    return ret;
}

static int32_t ec20_atcmd_send_nothing_wait_ret_val(ec20_sal_send_state_t state, int timeout_ms) 
{
    int ret = 0;

    if (!ec20_atcmd_send_state_ready()) {
        LOGE(TAG, "Not in ready state\r\n");
        return -1;
    }

    ec20_atcmd_sal_send_state_set(state);


    if (timeout_ms > 0) { 
        if (0 == ec20_atcmd_send_wait(timeout_ms)) {
            ret = ec20_atcmd_get_ret_val();
        } else {
            ret = -1;
        }
    }

    ec20_atcmd_sal_send_state_set(WAIT_NONE);

    return ret;
}


static void ec20_atcmd_rsp_process(ec20_sal_rsp_val_t rsp_val)
{
    if (ec20_atcmd_get_send_state() != WAIT_SIMPLE_RSP &&
        ec20_atcmd_get_send_state() != WAIT_SEND_PMT) {
        LOGE(TAG, "%s state %d val %d", __func__, ec20_atcmd_get_send_state(), rsp_val);
        return;
    }

    ec20_atcmd_set_ret_val(rsp_val);

    ec20_atcmd_send_post();
}

static void net_pin_check_process(void)
{
    char  prefix[AT_CMD_PREFIX_MAX_LEN] = { 0 };
    int len = 0;

    /* read out prefix */
    len = ec20_atcmd_read_out_string(prefix, AT_CMD_PREFIX_MAX_LEN, "OK\r\n");
    if (len < 0) {
        return;
    }
    
    if (ec20_atcmd_get_send_state() != WAIT_PIN_RSP) {
        return;
    }

    if (strstr(prefix, "READY") != NULL) {
        ec20_atcmd_set_ret_val(RSP_OK);
    } else {
        ec20_atcmd_set_ret_val(RSP_ERROR);
    }

    ec20_atcmd_send_post();
}

static void net_sig_quality_process(void)
{
    char  prefix[AT_CMD_PREFIX_MAX_LEN] = { 0 };
    int len = 0, ret = -1;
    int rssi, ber;
    char single;

    /* eat space */
    at_read(ec20_at_dev_fd, &single, 1);

    /* rssi */
    ret = ec20_atcmd_info_get(prefix, sizeof(prefix), ec20_atcmd_num_check);
    if (ret != 0 || (rssi = atoi(prefix)) < 0) {
        LOGE(TAG, "%s %d invalid rssi %s", __func__, __LINE__, prefix);
        return;
    }

    /* ber */
    memset(prefix, 0, sizeof(prefix));
    ret = ec20_atcmd_info_get(prefix, sizeof(prefix), ec20_atcmd_num_check);
    if (ret != 1 || (ber = atoi(prefix)) < 0) {
        LOGE(TAG, "%s %d invalid ber %s", __func__, __LINE__, prefix);
        return;
    }

    ec20_atcmd_read_out_string(prefix, sizeof(prefix), "OK\r\n");

    if (ec20_atcmd_get_send_state() != WAIT_SIGQ_RSP) {
        return;
    }

    //LOGI(TAG, "Sig quality %d %d", rssi, ber);

    if (rssi > 0 && rssi < 99) {
        ec20_atcmd_set_ret_val(RSP_OK);
    } else {
        ec20_atcmd_set_ret_val(RSP_ERROR);
    }

    ec20_atcmd_send_post();
}

/*AT+CGREG?
+CGREG: 0,1
*/
static void net_register_process()
{
    char  prefix[AT_CMD_PREFIX_MAX_LEN] = { 0 };
    int len = 0, ret = -1;
    int n = 0, stat = 0;
    char single;

    /* eat space */
    at_read(ec20_at_dev_fd, &single, 1);

    /* n */
    ret = ec20_atcmd_info_get(prefix, sizeof(prefix), ec20_atcmd_num_check);
    if (ret != 0 || (n = atoi(prefix)) < 0) {
        LOGE(TAG, "%s %d invalid n %s", __func__, __LINE__, prefix);
        return;
    }

    /* stat */
    memset(prefix, 0, sizeof(prefix));
    ret = ec20_atcmd_info_get(prefix, sizeof(prefix), ec20_atcmd_num_check);
    if (ret != 1 || (stat = atoi(prefix)) < 0) {
        LOGE(TAG, "%s %d invalid stat %s", __func__, __LINE__, prefix);
        return;
    }

    ec20_atcmd_read_out_string(prefix, sizeof(prefix), "OK\r\n");

    if (ec20_atcmd_get_send_state() != WAIT_REG_RSP) {
        return;
    }

    if (1 == stat) {
        ec20_atcmd_set_ret_val(RSP_OK);
    } else {
        ec20_atcmd_set_ret_val(RSP_ERROR);
    }

    ec20_atcmd_send_post();
}

static void net_attach_process()
{
    char  prefix[AT_CMD_PREFIX_MAX_LEN] = { 0 };
    int len = 0, ret = -1;
    int attach = 0;
    char single;

    /* eat space */
    at_read(ec20_at_dev_fd, &single, 1);

    /* attach */
    memset(prefix, 0, sizeof(prefix));
    ret = ec20_atcmd_info_get(prefix, sizeof(prefix), ec20_atcmd_num_check);
    if (ret != 1 || (attach = atoi(prefix)) < 0) {
        LOGE(TAG, "%s %d invalid attach %s", __func__, __LINE__, prefix);
        return;
    }
    
    ec20_atcmd_read_out_string(prefix, sizeof(prefix), "OK\r\n");

    if (ec20_atcmd_get_send_state() != WAIT_ATTCH_RSP) {
        return;
    }

    LOGI(TAG, "Attach state %d", attach);

    if (1 == attach) {
        ec20_atcmd_set_ret_val(RSP_OK);
    } else {
        ec20_atcmd_set_ret_val(RSP_ERROR);
    }

    ec20_atcmd_send_post();
}

static void cme_error_process()
{
    char  prefix[AT_CMD_PREFIX_MAX_LEN] = { 0 };
    int len = 0, ret = -1;
    int error = 0;
    char single;

    /* eat space */
    at_read(ec20_at_dev_fd, &single, 1);

    /* error */
    memset(prefix, 0, sizeof(prefix));
    ret = ec20_atcmd_info_get(prefix, sizeof(prefix), ec20_atcmd_num_check);
    if (ret != 1 || (error = atoi(prefix)) < 0) {
        LOGE(TAG, "%s %d invalid error %s", __func__, __LINE__, prefix);
        return;
    }
    
    if (ec20_atcmd_get_send_state() == WAIT_NONE) {
        return;
    }

    ec20_atcmd_set_ret_val(RSP_ERROR);

    ec20_atcmd_send_post();
}

static void ec20_netcmd_rsp_process(void)
{
    char  prefix[AT_CMD_PREFIX_MAX_LEN] = { 0 };
    int len = 0;

    /* read out prefix */
    len = ec20_atcmd_read_out_string(prefix, AT_CMD_PREFIX_MAX_LEN, ":");
    if (len < 0) {
        return;
    }

    prefix[len - 1] = '\0';

    if (memcmp(prefix, "PIN", strlen(prefix)) == 0) {
        net_pin_check_process(); 
    } else if (memcmp(prefix, "SQ", strlen(prefix)) == 0) {
        net_sig_quality_process();
    } else if (memcmp(prefix, "GREG", strlen(prefix)) == 0) {
        net_register_process();
    } else if (memcmp(prefix, "GATT", strlen(prefix)) == 0) {
        net_attach_process();
    } else if (memcmp(prefix, "ME ERROR", strlen(prefix)) == 0) {
        cme_error_process();
    } else {
        LOGE(TAG, "Unknown preifx %s!", prefix);
    }
}

/*+QIOPEN: <connectID>,<err>*/
static void at_ec20_qiopen_rsp_process(void)
{
    char  prefix[AT_CMD_PREFIX_MAX_LEN] = { 0 };
    int len = 0, ret = -1;
    int connectid = 0, err = 0;
    char single;

    /* eat space */
    at_read(ec20_at_dev_fd, &single, 1);

    /* connectid */
    ret = ec20_atcmd_info_get(prefix, sizeof(prefix), ec20_atcmd_num_check);
    if (ret != 0 || (connectid = atoi(prefix)) < 0) {
        LOGE(TAG, "%s %d invalid n %s", __func__, __LINE__, prefix);
        return;
    }

    /* err */
    memset(prefix, 0, sizeof(prefix));
    ret = ec20_atcmd_info_get(prefix, sizeof(prefix), ec20_atcmd_num_check);
    if (ret != 1 || (err = atoi(prefix)) < 0) {
        LOGE(TAG, "%s %d invalid stat %s", __func__, __LINE__, prefix);
        return;
    }

    if (ec20_atcmd_get_send_state() != WAIT_CONN_RSP) {
        return;
    }

    LOGI(TAG, "link %d open result %d", connectid, err);

    if (0 == err) {
        ec20_atcmd_set_ret_val(RSP_OK);
    } else {
        ec20_atcmd_set_ret_val(RSP_ERROR);
    }

    ec20_atcmd_send_post();
}

/*+QIACT: <contextid>,<context state>,<context_type>,[<ip_add>]
  1,1,1,"10.7.157.1"
*/
static void at_ec20_ip_query_rsp_process(void)
{
    char  prefix[AT_CMD_PREFIX_MAX_LEN] = { 0 };
    int len = 0, ret = -1;
    int ctxid = 0, ctxstate = 0, ctxtype = 0;
    char single;

    /* eat space */
    at_read(ec20_at_dev_fd, &single, 1);

    /* ctxid check*/
    ret = ec20_atcmd_info_get(prefix, sizeof(prefix), ec20_atcmd_num_check);
    if (ret != 0 || (ctxid = atoi(prefix)) < 0) {
        LOGE(TAG, "%s %d invalid n %s", __func__, __LINE__, prefix);
        return;
    }

    /* ctxstate check*/
    ret = ec20_atcmd_info_get(prefix, sizeof(prefix), ec20_atcmd_num_check);
    if (ret != 0 || (ctxstate = atoi(prefix)) < 0) {
        LOGE(TAG, "%s %d invalid n %s", __func__, __LINE__, prefix);
        return;
    }
    
    /* ctxtype check*/
    ret = ec20_atcmd_info_get(prefix, sizeof(prefix), ec20_atcmd_num_check);
    if (ret != 0 || (ctxtype = atoi(prefix)) < 0) {
        LOGE(TAG, "%s %d invalid n %s", __func__, __LINE__, prefix);
        return;
    }

    if (ec20_atcmd_get_send_state() != WAIT_IP_RSP) {
        LOGE(TAG, "error here %d ", ec20_atcmd_get_send_state());
        return;
    }
    
    if (ctxid < 1 || ctxid > 16) {
        LOGE(TAG, "ctxid check fail %d", ctxid);
        ec20_atcmd_set_ret_val(RSP_ERROR);
        return;
    }
    
    if (ctxstate != 1) {
        LOGE(TAG, "ctxstate check fail %d", ctxstate);
        ec20_atcmd_set_ret_val(RSP_ERROR);
        return;
    }
    
    if (ctxtype != 1) {
        LOGE(TAG, "ctxtype check fail %d", ctxtype);
        ec20_atcmd_set_ret_val(RSP_ERROR);
        return;
    }
    
    /*get ip address*/
    memset(prefix, 0, sizeof(prefix));
    ret = ec20_atcmd_info_get(prefix, sizeof(prefix), ec20_atcmd_num_check);
    if (ret < 0) {
        LOGE(TAG, "%s %d invalid error %s", __func__, __LINE__, prefix);
        ec20_atcmd_set_ret_val(RSP_ERROR);
        return;
    }

    ret = ec20_atcmd_read_out_string(prefix, sizeof(prefix), "OK\r\n");
    if (ret < 0) {
        LOGE(TAG, "error here %s ", prefix);
        ec20_atcmd_set_ret_val(RSP_ERROR);
        return;
    }
    
    ec20_atcmd_set_ret_val(RSP_OK);

    ec20_atcmd_send_post();
}

/*
+QIURC: "dnsgip",0,1,600

+QIURC: "dnsgip","122.51.89.94"

*/
static void at_ec20_domain_rsp_process()
{
    char  prefix[32] = { 0 };
    int ret = -1;
    int err = 0, count = 0, dns_ttl = 0;

    /* get err value*/
    ret = ec20_atcmd_info_get(prefix, sizeof(prefix), ec20_atcmd_num_check);
    /*if the next char is \r, means is ip addr*/
    LOGE(TAG,"err value %d ret %d, ip len %d", err, ret, strlen(prefix));
    if (ret == 1) {
        memcpy(ec20_domain_2_ip, prefix + 1, strlen(prefix) - 2);
        if (ec20_atcmd_get_send_state() != WAIT_DOMAIN_RSP) {
            LOGE(TAG,"Error here");
            return;
        }
        ec20_atcmd_set_ret_val(RSP_OK);
        ec20_atcmd_send_post();
    } else if (ret == 0) {
        /*means it is err*/
        err = atoi(prefix);
        /* get ipcount value*/
        memset(prefix, 0, sizeof(prefix));
        ret = ec20_atcmd_info_get(prefix, sizeof(prefix), ec20_atcmd_num_check);
        if (ret < 0 || (count = atoi(prefix)) < 0) {
            LOGE(TAG, "%s %d invalid n %s", __func__, __LINE__, prefix);
        }

        /*get dns ttl value*/
        memset(prefix, 0, sizeof(prefix));
        ret = ec20_atcmd_info_get(prefix, sizeof(prefix), ec20_atcmd_num_check);
        if (ret < 0 || (dns_ttl = atoi(prefix)) < 0) {
            LOGE(TAG, "%s %d invalid n %s", __func__, __LINE__, prefix);
        }
        
        if ((err != 0) || (count == 0)) {
            LOGE(TAG,"Error here");
            if (ec20_atcmd_get_send_state() != WAIT_DOMAIN_RSP) {
                LOGE(TAG,"Error here");
                return;
            }
            ec20_atcmd_set_ret_val(RSP_ERROR);
            ec20_atcmd_send_post();
            return ;
        } 
        /*else nothing to do ,wait ip*/
    } else {
        LOGE(TAG, "%s %d invalid n %s", __func__, __LINE__, prefix);
        return;
    }
}

static void at_ec20_close_report_process()
{
    char prefix[AT_CMD_PREFIX_MAX_LEN] = { 0 };
    int  ret = 0;
    int  linkid = 0;

    ret = ec20_atcmd_info_get(prefix, sizeof(prefix), ec20_atcmd_num_check);
    if (ret != 1) {
        LOGE(TAG, "%s %d invalid n %s", __func__, __LINE__, prefix);
        return;
    }

    linkid = atoi(prefix);
}

static void at_ec20_recv_data_process()
{
    char prefix[AT_CMD_PREFIX_MAX_LEN] = { 0 };
    char remote_ip[16];
    char  *data = NULL;
    int  ret = 0;
    int  linkid = 0, recvlen = 0, port = 0;

    /*get linkid*/
    ret = ec20_atcmd_info_get(prefix, sizeof(prefix), ec20_atcmd_num_check);
    if (ret != 0) {
        LOGE(TAG, "%s %d invalid n %s", __func__, __LINE__, prefix);
    }
    linkid = atoi(prefix);
    memset(prefix, 0, sizeof(prefix));
    /*get data len*/
    ret = ec20_atcmd_info_get(prefix, sizeof(prefix), ec20_atcmd_num_check);
    if (ret < 0) {
        LOGE(TAG, "%s %d invalid n %s", __func__, __LINE__, prefix);
    }
    recvlen = atoi(prefix);
    
    if (ret == 0) {
        /*means data formate is 
        +QIURC:"recv",<connectID>,<currentrecvlength>,<remoteIP>,<remote_port><CR><LF><data>
        */
        /*get ip */
        memset(prefix, 0, sizeof(prefix));
        ret = ec20_atcmd_info_get(prefix, sizeof(prefix), ec20_atcmd_num_check);
        if (ret < 0) {
            LOGE(TAG, "%s %d invalid n %s", __func__, __LINE__, prefix);
        }
        
        if (strlen(prefix) > sizeof(remote_ip) + 1) {
            LOGE(TAG, "Invalid remote ip %s", remote_ip);
        } else {
            memcpy(remote_ip, prefix + 1, strlen(prefix) - 2);
        }

        memset(prefix, 0, sizeof(prefix));
        /*get port*/
        ret = ec20_atcmd_info_get(prefix, sizeof(prefix), ec20_atcmd_num_check);
        if (ret != 1) {
            LOGE(TAG, "%s %d invalid n %s", __func__, __LINE__, remote_ip);
        }
        port = atoi(prefix);
    }

    /* eat \n */
    at_read(ec20_at_dev_fd, prefix, 1);
    if (linkid < 0 || linkid > 11 || recvlen <= 0) {
        LOGE(TAG, "Invalid input linkid data %d %d", linkid, recvlen);
        return;
    }

    data = aos_malloc(recvlen);
    if (NULL == data) {
        LOGE(TAG, "Fail to malloc %d ", recvlen);
        return;
    }
    memset(data, 0, recvlen);
    
    /* get data */
    if (at_read(ec20_at_dev_fd, data, recvlen) < 0) {
        LOGE(TAG, "Fail to get data %d ", recvlen);
        return;
    }
    
    if (g_netconn_data_input_cb && (g_link[linkid].fd >= 0)) {
        if (g_netconn_data_input_cb(g_link[linkid].fd, data, recvlen, remote_ip, port)) {
            LOGE(TAG, " %s socket %d get data len %d fail to post to sal, drop it\n",
                 __func__, g_link[linkid].fd, recvlen);
        }
    }
    
    aos_free(data);
    return;
}

static void at_ec20_pdpdeactive_process(void)
{
    char prefix[AT_CMD_PREFIX_MAX_LEN] = { 0 };
    int  ret = 0;
    int  ctxid = 0;

    ret = ec20_atcmd_info_get(prefix, sizeof(prefix), ec20_atcmd_num_check);
    if (ret != 1) {
        LOGE(TAG, "%s %d invalid n %s", __func__, __LINE__, prefix);
        return;
    }

    ctxid = atoi(prefix);
    if (ctxid < 1 || ctxid > 16) {
        return ;
    }
    LOGW(TAG, "ec20 pdp deactivated, need to reactivate it.");
    return;
}

static void at_ec20_urc_cmd_proc(void)
{
    char prefix[AT_CMD_PREFIX_MAX_LEN] = { 0 };
    char single;
    int  len;
    
    /* eat space */
    at_read(ec20_at_dev_fd, &single, 1);
    
    /* read out prefix */
    len = ec20_atcmd_read_out_string(prefix, AT_CMD_PREFIX_MAX_LEN, ",");
    if (len < 0) {
        LOGE(TAG, "Fail to get prefix end char ,");
        return;
    }
    
    prefix[len - 1] = '\0';
    
    /*get ip by domain*/
    if (memcmp(prefix, "\"dnsgip\"", strlen(prefix)) == 0) {
        at_ec20_domain_rsp_process();
    } else if (memcmp(prefix, "\"closed\"", strlen(prefix)) == 0) {
        at_ec20_close_report_process();
    } else if (memcmp(prefix, "\"recv\"", strlen(prefix)) == 0) {
        at_ec20_recv_data_process();
    } else if (memcmp(prefix, "\"pdpdeact\"", strlen(prefix)) == 0) {
        at_ec20_pdpdeactive_process();
    } else {
        LOGE(TAG, "Unknown preifx %s!", prefix);
    }
}

static void at_ec20_qdmurc_process(void)
{
    char prefix[AT_CMD_PREFIX_MAX_LEN] = { 0 };
    char single;
    int  len;
    int  ret = 0;
    
    /* eat space */
    at_read(ec20_at_dev_fd, &single, 1);
    
    /* read out prefix */
    len = ec20_atcmd_read_out_string(prefix, AT_CMD_PREFIX_MAX_LEN, ",");
    if (len < 0) {
        LOGE(TAG, "Fail to get prefix end char");
        return;
    }

    LOGI(TAG, "ec20 get qdmurc %s", prefix);

    memset(prefix, 0, sizeof(prefix));
    ret = ec20_atcmd_info_get(prefix, sizeof(prefix), ec20_atcmd_num_check);
    if (ret != 1) {
        LOGE(TAG, "%s %d invalid n %s", __func__, __LINE__, prefix);
        return;
    }

    LOGI(TAG, "ec20 get qdmurc value %s", prefix);
    return;
}


static void at_ec20_qi_cmd_process(void)
{
    char  prefix[AT_CMD_PREFIX_MAX_LEN] = { 0 };
    int len = 0;

    /* read out prefix */
    len = ec20_atcmd_read_out_string(prefix, AT_CMD_PREFIX_MAX_LEN, ":");
    if (len < 0) {
        LOGE(TAG, "Fail to get prefix end char :");
        return;
    }

    prefix[len - 1] = '\0';

    /*+QIOPEN:*/
    if (memcmp(prefix, "IOPEN", strlen(prefix)) == 0) {
        at_ec20_qiopen_rsp_process(); 
    } else if (memcmp(prefix, "IACT", strlen(prefix)) == 0) {
        /*+QIACT:*/
        at_ec20_ip_query_rsp_process();
    } else if (memcmp(prefix, "IURC", strlen(prefix)) == 0) {
        /*+QIURC:*/
        at_ec20_urc_cmd_proc();
    } else if (memcmp(prefix, "DMURC", strlen(prefix)) == 0) {
        /*+QDM:*/
        at_ec20_qdmurc_process();
    }else {
        LOGE(TAG, "Unknown preifx %s!", prefix);
    }
}

static void at_ec20_atcmd_rsp_callback(void *arg, char *rspinfo, int rsplen)
{
    char *input = (char *)arg;

    if (NULL == input) {
        LOGE(TAG, "invalid input of rsp callback");
        return;
    }

    if (0 == memcmp(input, AT_EC20_QI_PREFIX,
                    strlen(AT_EC20_QI_PREFIX))) {
        at_ec20_qi_cmd_process();
    } else if (0 == memcmp(input, AT_EC20_NET_CMD_PREFIX,
                    strlen(AT_EC20_NET_CMD_PREFIX))) {
        ec20_netcmd_rsp_process();
    } else if (0 == memcmp(input, AT_EC20_AT_RSP_OK,
                    strlen(AT_EC20_AT_RSP_OK))) {
        ec20_atcmd_rsp_process(RSP_OK);
    } else if (0 == memcmp(input, AT_EC20_AT_RSP_ERROR,
                    strlen(AT_EC20_AT_RSP_ERROR))) {
        ec20_atcmd_rsp_process(RSP_ERROR);
    } else if (0 == memcmp(input, AT_EC20_PUB_PROMPT,
                    strlen(AT_EC20_PUB_PROMPT))) {
        ec20_atcmd_rsp_process(RSP_PROMPT);
    } else {
        LOGE(TAG, "invalid input of rsp callback");
        return;
    }

    return;
}


static int32_t at_ec20_module_ready(void)
{      
    return ec20_atcmd_send_wait_ret_val(AT_CMD_AT, strlen(AT_CMD_AT),
                                   WAIT_SIMPLE_RSP, AT_WAIT_DEF_TIMEOUT); 
}

static int32_t at_ec20_echo_off(void)
{      
    return ec20_atcmd_send_wait_ret_val(AT_CMD_ECHO_OFF, strlen(AT_CMD_ECHO_OFF),
                                   WAIT_SIMPLE_RSP, AT_WAIT_DEF_TIMEOUT); 
}

static int at_ec20_lte_status_check(void)
{
    int retry = 0;
    int ret;

    do {
        retry++;
        /*sim card status check*/
        ret = ec20_atcmd_send_wait_ret_val(AT_CMD_SIM_PIN_CHECK, strlen(AT_CMD_SIM_PIN_CHECK),
                                      WAIT_PIN_RSP, AT_WAIT_DEF_TIMEOUT);
    } while (ret != 0 && retry < AT_CMD_RETRY_MAX);

    /* Signal quaility check */
    ret = ec20_atcmd_send_wait_ret_val(AT_CMD_SIGNAL_QUALITY_CHECK, strlen(AT_CMD_SIGNAL_QUALITY_CHECK),
                                  WAIT_SIGQ_RSP, AT_WAIT_DEF_TIMEOUT);
    if (ret != 0) {
        aos_msleep(AT_WAIT_DEF_TIMEOUT);
        LOGE(TAG, "Signal quaility check failed\r\n");
        return -1;
    }

    /* network registration check*/
    ret = ec20_atcmd_send_wait_ret_val(AT_CMD_NETWORK_REG_CHECK, strlen(AT_CMD_NETWORK_REG_CHECK),
                                  WAIT_REG_RSP, AT_WAIT_DEF_TIMEOUT);
    if (ret != 0) {
        aos_msleep(AT_WAIT_DEF_TIMEOUT);
        LOGE(TAG, "Network registration check failed\r\n");
        return -1;
    }

    /* PS attach check*/
    ret = ec20_atcmd_send_wait_ret_val(AT_CMD_ATTACH_CHECK, strlen(AT_CMD_ATTACH_CHECK),
                                  WAIT_ATTCH_RSP, AT_WAIT_DEF_TIMEOUT);
    if (ret != 0) {
        aos_msleep(AT_WAIT_DEF_TIMEOUT);
        LOGE(TAG, "Attchement check failed\r\n");
        return -1;
    }

    return 0;
}


static int32_t ec20_prepare_network(void)
{
    int ret = 0;
    int retry = 0;

    /* wait module ready */
    do {
       retry++;
       ret = at_ec20_module_ready();
       if (ret != 0) {
            LOGD(TAG, "%s %d retry ...\r\n", __func__, __LINE__);
        }
    } while(ret != 0 && retry < AT_CMD_RETRY_MAX * 3);

    if (ret) {
        LOGE(TAG, "module dectect fail \r\n");
        return -1;
    }

    retry = 0;

    /* turn off echo */
    do {
        retry++;
        ret = at_ec20_echo_off();
        if (ret != 0) {
            LOGD(TAG, "%s %d retry ...\r\n", __func__, __LINE__);
        }
    } while(ret != 0 && retry < AT_CMD_RETRY_MAX);

    if (ret) {
        LOGE(TAG, "module turn off uart echo back fail \r\n");
        return -1;
    }

    retry = 0;

    /* lte status */
    do {
        retry++;
        ret = at_ec20_lte_status_check();
        if (ret != 0) {
            LOGD(TAG, "%s %d retry ...\r\n", __func__, __LINE__);
        }
    } while(ret != 0 && retry < AT_CMD_RETRY_MAX);

    if (ret) {
        LOGE(TAG, "module lte status check fail \r\n");
        return -1;
    }

    return ret;
}

static int ec20_pdp_config(void)
{
    int ret = 0;
    int context_type = 1;  /*IPV4*/
    int auth_type = 0;     /*none*/
    
#define APN_TYPE    "\"CMNET\""
#define APN_USERNAME "\"\""
#define APN_PASSWORKD "\"\""

    memset(ec20_at_cmd_buf, 0, sizeof(ec20_at_cmd_buf));
    snprintf(ec20_at_cmd_buf,
                sizeof(ec20_at_cmd_buf),
                "AT%s=%d\r", AT_CMD_PDP_DEACT, ec20_contextid);
    if (0 != ec20_atcmd_send_wait_ret_val(ec20_at_cmd_buf, strlen(ec20_at_cmd_buf),
                                         WAIT_SIMPLE_RSP, AT_WAIT_DEF_TIMEOUT)) {
        LOGE(TAG, "%s cmd fail", ec20_at_cmd_buf);
        return -1;
    }

    memset(ec20_at_cmd_buf, 0, sizeof(ec20_at_cmd_buf));
    snprintf(ec20_at_cmd_buf,
                sizeof(ec20_at_cmd_buf),
                "AT%s=%d,%d,%s,%s,%s,%d\r", AT_CMD_PDP_CONFIG, 
                ec20_contextid, context_type, APN_TYPE, APN_USERNAME,
                APN_PASSWORKD, auth_type);
    if (0 != ec20_atcmd_send_wait_ret_val(ec20_at_cmd_buf, strlen(ec20_at_cmd_buf),
                                         WAIT_SIMPLE_RSP, AT_WAIT_DEF_TIMEOUT)) {
        LOGE(TAG, "%s cmd fail", ec20_at_cmd_buf);
        return -1;
    }
    
    memset(ec20_at_cmd_buf, 0, sizeof(ec20_at_cmd_buf));
    snprintf(ec20_at_cmd_buf,
                sizeof(ec20_at_cmd_buf),
                "AT%s=%d\r", AT_CMD_PDP_ACT, ec20_contextid);
    if (0 != ec20_atcmd_send_wait_ret_val(ec20_at_cmd_buf, strlen(ec20_at_cmd_buf),
                                         WAIT_SIMPLE_RSP, AT_WAIT_DEF_TIMEOUT)) {
        LOGE(TAG, "%s cmd fail", ec20_at_cmd_buf);
        return -1;
    }

    return 0;
}

static int32_t ec20_query_module_ip_addr()
{
    int32_t ret = 0;

    ret = ec20_atcmd_send_wait_ret_val(AT_CMD_IP_QUERY, strlen(AT_CMD_IP_QUERY), WAIT_IP_RSP, AT_WAIT_DEF_TIMEOUT);
    if (ret != 0) {
        LOGE(TAG, "%s failed \r\n", AT_CMD_IP_QUERY);
    }
    
    return ret;
}

static int fd_to_linkid(int fd)
{
    int link_id;

    if (aos_mutex_lock(&g_link_mutex, AOS_WAIT_FOREVER) != 0) {
        LOGE(TAG, "Failed to lock mutex (%s).", __func__);
        return -1;
    }
    for (link_id = 0; link_id < EC20_MAX_LINK_NUM; link_id++) {
        if (g_link[link_id].fd == fd) {
            break;
        }
    }

    aos_mutex_unlock(&g_link_mutex);

    return link_id;
}


static int HAL_SAL_DomainToIp(char *domain, char ip[16])
{
    char *pccmd = NULL;
    int   cmd_length = 0;
    int   ret = 0;
    
    if (NULL == domain) {
        LOGE(TAG, "Invalid Input");
        return -1;
    }

    if (!ec20_init_flag) {
        if (prepare_network_process() != 0) {
            LOGE(TAG, "%s ec20 lte module haven't init yet \r\n", __func__);
            return -1;
        }
    }

    memset(ec20_domain_2_ip, 0, 16);
    
    cmd_length = strlen(domain) + AT_CMD_PREFIX_MAX_LEN;
    pccmd = aos_malloc(cmd_length);
    if (NULL == pccmd) {
        LOGE(TAG, "Malloc %d fail ", cmd_length);
        return -1;
    }
    memset(pccmd, 0, cmd_length);

    snprintf(pccmd, cmd_length, "%s=%d,\"%s\"\r", AT_CMD_DOMAIN_2_IP, ec20_contextid, domain);

    ret = ec20_atcmd_send_wait_ret_val(pccmd, strlen(pccmd), WAIT_SIMPLE_RSP, AT_WAIT_DEF_TIMEOUT);
    if (ret) {
        LOGE(TAG, "cmd %s fail ", pccmd);
        aos_free(pccmd);

        if (ec20_init_flag) {
            if (prepare_network_process() != 0) {
                LOGE(TAG, "%s ec20 lte turn module uninted \r\n", __func__);
                ec20_init_flag = 0;
            }
        }

        return -1;
    }

    /*max reponse time is 60s */
    ret = ec20_atcmd_send_nothing_wait_ret_val(WAIT_DOMAIN_RSP, AT_WAIT_DEF_TIMEOUT * 20);
    if (ret) {
        LOGE(TAG, "wait cmd %s response fail ", pccmd);
        aos_free(pccmd);
        return -1;
    }

    strncpy(ip, ec20_domain_2_ip, strlen(ec20_domain_2_ip));
    LOGI(TAG, "get domain %s ip %s", domain, ec20_domain_2_ip);
    return 0;
}

static int prepare_network_process(void)
{
    int ret = 0;

    /* prepare lte network */
    ret = ec20_prepare_network();
    if (ret != 0) {
        LOGE(TAG, "prepare network failed");
        return -1;
    }

    /*pdp config get ip */
    ret = ec20_pdp_config();
    if (ret != 0) {
        LOGE(TAG, "pdp config failed");
        return -1;
    }

    /* get ip */
    int retry = 0;
    do {
        retry++;
        ret = ec20_query_module_ip_addr();
        if (ret != 0) {
            LOGD(TAG, "%s %d retry ...\r\n", __func__, __LINE__);
        }
    } while(ret != 0 && retry < AT_CMD_RETRY_MAX);

    if (ret == 0) {
        LOGI(TAG, "prepare network success");
        ec20_init_flag = 1;
    }

    return 0;
}

static int HAL_SAL_Init(void)
{
    int ret = 0;
    uint32_t link_num = 0;

    if (ec20_init_flag) {
        LOGE(TAG, "ec20 module have already inited");
        return -1;
    }
    
    memset(&st_ec20_atcmdsend_sem4, 0, sizeof(aos_sem_t));
    ret = aos_sem_new(&st_ec20_atcmdsend_sem4, 0);
    if (ret) {
        LOGE(TAG, "ec20 at cmd send sem4 creat fail");
        return -1;
    }

    memset(&g_link_mutex, 0, sizeof(aos_mutex_t));
    if (0 != aos_mutex_new(&g_link_mutex)) {
        LOGE(TAG, "Creating link mutex failed (%s %d).", __func__, __LINE__);
        aos_sem_free(&st_ec20_atcmdsend_sem4);
        return -1;
    }

    for (link_num = 0; link_num < EC20_MAX_LINK_NUM; link_num++) {
        g_link[link_num].fd = -1;
    }

    return prepare_network_process();
}

static int HAL_SAL_Deinit()
{
    if (!ec20_init_flag) {
        return 0;
    }

    aos_mutex_free(&g_link_mutex);
    aos_sem_free(&st_ec20_atcmdsend_sem4);
    ec20_init_flag = 0;
    
    return 0;
}

static int HAL_SAL_Start(sal_conn_t *conn)
{
    int  linkid = 0;
    int  ret = 0;
    char *pccmd = NULL;

    if (!ec20_init_flag) {
        if (prepare_network_process() != 0) {
            LOGE(TAG, "%s ec20 lte module haven't init yet \r\n", __func__);
            return -1;
        }
    }

    if (!conn || !conn->addr) {
        LOGE(TAG, "%s %d - invalid input \r\n", __func__, __LINE__);
        return -1;
    }

    aos_mutex_lock(&g_link_mutex, AOS_WAIT_FOREVER);
    for (linkid = 0; linkid < EC20_MAX_LINK_NUM; linkid++) {
        if (g_link[linkid].fd >= 0) {
            continue;
        }
        g_link[linkid].fd = conn->fd;
        break;
    }
    aos_mutex_unlock(&g_link_mutex);

    if (linkid >= EC20_MAX_LINK_NUM) {
        LOGE(TAG, "No link available for now, %s failed. \r\n", __func__);
        return -1;
    }

    switch (conn->type) {
        case TCP_SERVER:
            pccmd = "TCP LISTENER";
            LOGE(TAG, "tcp server not supported for now on ec20");
            goto err;
            break;
        case TCP_CLIENT:
            pccmd = "TCP";
            memset(ec20_at_cmd_buf, 0, AT_CMD_MAX_LEN);
            snprintf(ec20_at_cmd_buf, AT_CMD_MAX_LEN - 1, "%s=%d,%d,\"%s\",\"%s\",%d,%d,%d\r", AT_CMD_TCP_CONNECT,
                    ec20_contextid, linkid, pccmd, conn->addr, conn->r_port, 0, 1);

            /*wait for tcp connect reponse*/
            ret = ec20_atcmd_send_wait_ret_val(ec20_at_cmd_buf, strlen(ec20_at_cmd_buf), WAIT_SIMPLE_RSP, AT_WAIT_DEF_TIMEOUT);
            if (ret != 0) {
                LOGE(TAG, "tcp connect %s response failed \r\n", ec20_at_cmd_buf);
                goto err;
            }

            ret = ec20_atcmd_send_nothing_wait_ret_val(WAIT_CONN_RSP, AT_WAIT_DEF_TIMEOUT * 50);
            if (ret != 0) {
                memset(ec20_at_cmd_buf, 0, AT_CMD_MAX_LEN);
                /*close the connect*/
                snprintf(ec20_at_cmd_buf, AT_CMD_MAX_LEN - 1, "%s=%d\r", AT_CMD_TCP_CLOSE, linkid);
                ret = ec20_atcmd_send_wait_ret_val(ec20_at_cmd_buf, strlen(ec20_at_cmd_buf), WAIT_SIMPLE_RSP, AT_WAIT_DEF_TIMEOUT);
                if (ret != 0) {
                    LOGE(TAG, "%s failed \r\n", ec20_at_cmd_buf);
                }
   
                if (ec20_init_flag) {
                    if (prepare_network_process() != 0) {
                        LOGE(TAG, "%s ec20 lte turn module uninted \r\n", __func__);
                        ec20_init_flag = 0;
                    }
                }
                goto err;
            }
            break;
        case UDP_UNICAST:
            pccmd = "UDP";
            LOGE(TAG, "udp not supported for now on ec20");
            goto err;
            break;
        case SSL_CLIENT:
        case UDP_BROADCAST:
        default:
            LOGE(TAG, "n720 lte module connect type %d not support \r\n", conn->type);
            goto err;
    }

    return 0;
err:
    
    aos_mutex_lock(&g_link_mutex, AOS_WAIT_FOREVER);
    g_link[linkid].fd = -1;
    aos_mutex_unlock(&g_link_mutex);

    return -1;
}

static int HAL_SAL_Send(int fd,
                 uint8_t *data,
                 uint32_t len,
                 char remote_ip[16],
                 int32_t remote_port,
                 int32_t timeout)
{
    int  linkid;
    /*max send data max length*/
    int  link_send_capacity = 1460;
    int  ret = 0;
    char cmd[32] = {0};

    if (!ec20_init_flag) {
        LOGE(TAG, "%s ec20 lte module haven't init yet \r\n", __func__);
        return -1;
    }
    
    linkid = fd_to_linkid(fd);
    if (linkid >= EC20_MAX_LINK_NUM) {
        LOGE(TAG, "No connection found for fd (%d) in %s \r\n", fd, __func__);
        return -1;
    }

    /*get the datalength can be sent at this time*/
    if (link_send_capacity < len) {
        LOGE(TAG, "Data can be send on link %d oversize len %d capacity %d", 
            linkid, len, link_send_capacity);
        return -1;
    }
    
    /*try start tcp length*/
    snprintf(cmd, 32 - 1, "%s=%d,%d\r", AT_CMD_TCP_SEND, linkid, len);
    ret = ec20_atcmd_send_wait_ret_val(cmd, strlen(cmd), WAIT_SEND_PMT, AT_WAIT_DEF_TIMEOUT);
    if (ret != RSP_PROMPT) {
        LOGE(TAG, "%s failed \r\n", cmd);
        return -1;
    }

    /*send tcp data*/
    ret = ec20_atcmd_send_wait_ret_val((char *)data, len, WAIT_SIMPLE_RSP, AT_WAIT_DEF_TIMEOUT);
    if (ret != 0) {
        LOGE(TAG, "tcp %d send data failed \r\n", linkid);
        return -1;
    }

    return 0;
}

static int HAL_SAL_Close(int fd, int32_t remote_port)
{
    int  linkid = 0;
    int  ret = 0;
    char cmd[32] = {0};

    if (!ec20_init_flag) {
        LOGE(TAG, "%s n720 lte module haven't init yet \r\n", __func__);
        return -1;
    }

    linkid = fd_to_linkid(fd);
    if (linkid >= EC20_MAX_LINK_NUM) {
        LOGE(TAG, "No connection found for fd (%d) in %s \r\n", fd, __func__);
        return -1;
    }

    snprintf(cmd, 32 - 1, "%s=%d\r", AT_CMD_TCP_CLOSE, linkid);
    ret = ec20_atcmd_send_wait_ret_val(cmd, strlen(cmd), WAIT_SIMPLE_RSP, AT_WAIT_DEF_TIMEOUT);
    if (ret != 0) {
        LOGE(TAG, "%s failed \r\n", cmd);
    }

    aos_mutex_lock(&g_link_mutex, AOS_WAIT_FOREVER);
    g_link[linkid].fd = -1;
    aos_mutex_unlock(&g_link_mutex);

    return ret;
}

static int HAL_SAL_RegisterNetconnDataInputCb(netconn_data_input_cb_t cb)
{
    if (cb) {
        g_netconn_data_input_cb = cb;
    }
    return 0;
}

/* To implement the driver configuration */
static int ec20_sal_add_dev_usb(void* data)
{
    at_config_t at_config = { 0 };

    extern void register_at_usb_ops(void);
    register_at_usb_ops();

    if (at_init() != 0) {
        LOGE(TAG, "at init failed");
        return -1;
    }

    if(data != NULL)
    {
        sal_device_config_t *config = (sal_device_config_t *)data;
        ec20_at_usb_dev.class_name = config->usb_dev.class_name;
        ec20_at_usb_dev.interface_id = config->usb_dev.interface_id;
    }
    else
    {
        LOGE(TAG, "Error: Uart dev is not configured! Please"
             " provide the config in sal_add_dev() call.\n");
        return -1;
    }

    /* configure and add one usb dev */
    at_config.type                             = AT_DEV_USB;
    at_config.port                             = 0;
    at_config.dev_cfg                          = &ec20_at_usb_dev;
    at_config.send_wait_prompt                 = AT_SEND_DATA_WAIT_PROMPT;
    at_config.recv_task_stacksize              = 8192;
    at_config.recv_task_priority               = 32;

    if ((ec20_at_dev_fd = at_add_dev(&at_config)) < 0) {
        LOGE(TAG, "AT parser device add failed!\n");
        return -1;
    }

    at_register_callback(ec20_at_dev_fd, AT_EC20_QI_PREFIX,
                         NULL, NULL, 0,
                         at_ec20_atcmd_rsp_callback,
                         AT_EC20_QI_PREFIX);

    at_register_callback(ec20_at_dev_fd, AT_EC20_NET_CMD_PREFIX,
                         NULL, NULL, 0,
                         at_ec20_atcmd_rsp_callback,
                         AT_EC20_NET_CMD_PREFIX);

    at_register_callback(ec20_at_dev_fd, AT_EC20_AT_RSP_OK,
                         NULL, NULL, 0,
                         at_ec20_atcmd_rsp_callback,
                         AT_EC20_AT_RSP_OK);

    at_register_callback(ec20_at_dev_fd, AT_EC20_AT_RSP_ERROR,
                         NULL, NULL, 0,
                         at_ec20_atcmd_rsp_callback,
                         AT_EC20_AT_RSP_ERROR);

    at_register_callback(ec20_at_dev_fd, AT_EC20_PUB_PROMPT,
                         NULL, NULL, 0,
                         at_ec20_atcmd_rsp_callback,
                         AT_EC20_PUB_PROMPT);

   return 0;
}

int testcase_ec20_sal_add_dev(void)
{
    int ret = ec20_sal_add_dev_usb(NULL);
    if (ret == 0) {
        printf("%s %d test success\n", __func__, __LINE__);
    } else {
        printf("%s %d test failed\n", __func__, __LINE__);
    }

    return ret;
}

int testcase_HAL_SAL_Init(void)
{
    int ret = HAL_SAL_Init();

    if (ret == 0) {
        printf("%s %d test success\n", __func__, __LINE__);
    } else {
        printf("%s %d test failed\n", __func__, __LINE__);
    }

    return ret;
}

int testcase_HAL_SAL_Start(char *addr)
{
    sal_conn_t conn;
    int ret;
  
    conn.fd = 0;
    conn.addr = addr;
    conn.type = TCP_CLIENT;
    conn.r_port = 80;

    ret =  HAL_SAL_Start(&conn);
    if (ret == 0) {
        printf("%s %d test success\n", __func__, __LINE__);
    } else {
        printf("%s %d test failed\n", __func__, __LINE__);
    }

    return ret;
}

int testcase_HAL_SAL_DomainToIp(char *domain, char *res)
{
    int ret;
    char result[16] = {0};

    ret = HAL_SAL_DomainToIp(domain, result);
    if (ret == 0) {
        printf("%s %d test success ip %s\n", __func__, __LINE__, result);
        if (res) {
            strncpy(res, result, 16);
        }
    } else {
        printf("%s %d test failed\n", __func__, __LINE__);
    }

    return ret;
}

static char * test_tcp_payload ="GET / HTTP/1.1 \r\n\
Host: coap.me\r\n\
Connection: keep-alive\r\n\
Upgrade-Insecure-Requests: 1\r\n\
User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/73.0.3683.103 Safari/537.36\r\n\
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3\r\n\
Accept-Encoding: gzip, deflate\r\n\
Accept-Language: zh-CN,zh;q=0.9\r\n\
\r\n";

int testcase_HAL_SAL_Send(char remote_ip, int port)
{
    int ret = 0;

    ret = HAL_SAL_Send(0, test_tcp_payload,
                       strlen(test_tcp_payload),
                       remote_ip, port, 5000);
    if (ret == 0) {
        printf("%s %d test success ip %s\n", __func__, __LINE__);
    } else {
        printf("%s %d test failed\n", __func__, __LINE__);
    }

    return ret;
}

int testcase_recvcb(int fd, void *data, size_t len,
                     char *remote_ip, uint16_t remote_port)
{
    printf("==> %s fd %d len %d content:\n%s\n", __func__, fd, len, (char *)data);
    return 0;
}

void usb_ec20_testcases(void)
{
    int port = 80;
    char *domain = "coap.me";
    char result[16] = {0};

    extern int usb_wwan_init(void);
    usb_wwan_init();

    HAL_SAL_RegisterNetconnDataInputCb(testcase_recvcb);

    testcase_ec20_sal_add_dev();

    testcase_HAL_SAL_Init();

    testcase_HAL_SAL_DomainToIp(domain, result);

    testcase_HAL_SAL_Start(result);

    testcase_HAL_SAL_Send(result, port);
}

static sal_op_t sal_op = {
    .next = NULL,
    .version = "1.0.0",
    .name = "ec20_usb",
    .add_dev = ec20_sal_add_dev_usb,
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
