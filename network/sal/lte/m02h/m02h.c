/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */


#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <aos/aos.h>
#include <atparser.h>
#include <sal_arch.h>
#include <sal_ipaddr.h>
#include <sal.h>
#include <k_api.h>

typedef enum {
	MODE_BUFFER_ACCESS = 0,
	MODE_DIRECT_PUSH,
	MODE_TRANSPARENT_ACCESS,
	MODE_END
} ACCESS_MODE;

#define TAG "m02h_lte_module"

#define M02H_AT_CMD_SUCCESS_RSP "OK"
#define M02H_AT_CMD_FAIL_RSP "ERROR"

#define AT_CMD_TEST "AT\r"
#define AT_CMD_TEST_RESULT "\r\nOK\r\n"

#define AT_CMD_ECHO_OFF   "ATE0"
#define AT_CMD_BAUDRATE_SET "AT+IPR"
#define AT_CMD_FLOW_CONTROL "AT+IFC"

#define AT_CMD_SIM_PIN_CHECK  "AT+CPIN?"
#define AT_CMD_SIGNAL_QUALITY_CHECK "AT+CSQ"
#define AT_CMD_NETWORK_REG_CHECK "AT+CREG?"
#define AT_CMD_GPRS_ATTACH_CHECK "AT+CGATT?"

#define AT_CMD_NETWORK_REG_CHECK_RSP "+CREG:"
#define AT_CMD_GPRS_ATTACH_CHECK_RSP "+CGATT:"

#define AT_CMD_QUERY_PDP_CONTEXT "AT+CGDCONT?"
#define AT_CMD_PDP_CONTEXT_RSP "+CGDCONT:"
#define AT_CMD_CONFIGURE_CONTEXT "AT+QICSGP"
#define AT_CMD_ACTIVATE_CONTEXT "AT+QIACT"
#define AT_CMD_ACTIVATE_CONTEXT_RSP "+QIACT:"

#define AT_CMD_START_SOCKET_SERVICE "AT+QIOPEN"
#define AT_CMD_CLOSE_SOCKET_SERVICE "AT+QICLOSE"
#define AT_CMD_SEND_DATA "AT+QISEND"
#define AT_CMD_OOB_PREFIX "\r\n+QIURC:"

#define AT_CMD_DOMAIN_TO_IP "AT+DNS"
#define AT_CMD_DOMAIN_TO_IP_RSP "+DNS:"

#define M02H_NETREG_INTERVAL            1000
#define M02H_NETREG_LOOP                30
#define M02H_CONN_CMD_LEN               256
#define M02H_DNS_CMD_RSP_LEN            128
#define M02H_DEFAULT_CMD_LEN            64
#define M02H_DEFAULT_RSP_LEN            64
#define M02H_LARGE_RSP_LEN              256
#define M02H_APN_LEN                    64
#define M02H_CONTEXT_ID                 1


/* default is 12 connect id, but in fact it is only one connect supporting now */
#define M02H_MAX_LINK_NUM               1

#define M02H_DEFAULT_CONTEXT_ID         1
#define M02H_U32_STRING_LEN             10

/* Change to include data slink for each link id respectively. <TODO> */
typedef struct link_s {
    int fd;
    aos_sem_t sem_start;
    aos_sem_t sem_close;
} link_t;

static uint8_t inited = 0;
static link_t g_link[M02H_MAX_LINK_NUM];
static aos_mutex_t g_link_mutex;
static aos_mutex_t g_domain_mutex;

static netconn_data_input_cb_t g_netconn_data_input_cb;


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
		at.read(&buf[i], 1);
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
        at.read(&str_value[i], 1);
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

static int socket_data_handle(void)
{
	char ch = 0;
	int linkid = 0;
	int i = 0;
	uint32_t len = 0;
	char *rcv_data = NULL;
	
	/*eat , char*/
	at.read(&ch, 1);
	
	linkid = (int)at_read_uint(',');
	if (linkid < 0 || linkid >=  M02H_MAX_LINK_NUM) {
		LOGE(TAG, "[Error: %s %d] Invalid link id 0x%02x.", __func__, __LINE__, linkid);
        return -1;
    }
	len = at_read_uint('\r');
	
	/*eat \n char*/
	at.read(&ch, 1);
	
	rcv_data = (char *)aos_malloc(len + 1);
    if (!rcv_data) {
        LOGE(TAG, "[Error: %s %d] out of memory.", __func__, __LINE__);
        return -1;
    }

    memset(rcv_data, 0, len + 1);

    at.read(rcv_data, len);

    if (g_netconn_data_input_cb && (g_link[linkid].fd >= 0)) {
        if (g_netconn_data_input_cb(g_link[linkid].fd, rcv_data, len, NULL, 0)) {
            LOGE(TAG, "%s socket %d get data len %d fail to post to sal, drop it\n",
					__func__, g_link[linkid].fd, len);
        }
    }
    LOGD(TAG, "%s socket data on link %d with length %d posted to sal\n",
			__func__, linkid, len);
    aos_free(rcv_data);
	
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
		at.read(&ch, 1);
	} while (ch == ' ');
	
	if (ch == '\"') {
		len = at_read_string(str_type, sizeof(str_type), '\"');
		if (!strcmp(str_type, "recv")) {
			socket_data_handle();
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
		len = at_read_string(&str_hint[1], sizeof(str_hint) - 1, '\r');

		/*eat \n char*/
		at.read(&ch, 1);
		
		LOGW(TAG, "QIURC event: %s", str_hint);
	}
}

static void at_start_test(void *psttimer, void *command)
{
    int ret = 0;
    static int flag = 0;

    if (flag == 0) {
        printf("send at command %s \r\n", command);
        flag = 1;
    }

    ret = at.write(command, strlen(command));
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

    aos_timer_new(&test_timer, at_start_test, command, 2000, 1);
    aos_timer_start(&test_timer);

    while (true) {
        ret = at.read(buffer, rsplen * 3);

        if (ret > 0 && (strstr(buffer, rsp) != NULL)) {
            break;
        }
    }
    aos_timer_stop(&test_timer);
    aos_timer_free(&test_timer);
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
    at.send_raw(AT_CMD_ECHO_OFF, rsp, M02H_DEFAULT_RSP_LEN);
    if (strstr(rsp, M02H_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }

    /*set baudrate 115200*/
    snprintf(cmd, M02H_DEFAULT_CMD_LEN - 1, "%s=%d", AT_CMD_BAUDRATE_SET, AT_UART_BAUDRATE);
    at.send_raw(cmd, rsp, M02H_DEFAULT_RSP_LEN);
    if (strstr(rsp, M02H_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }

    memset(cmd, 0, M02H_DEFAULT_CMD_LEN);
    memset(rsp, 0, M02H_DEFAULT_RSP_LEN);
    /*turn off flow control*/
    snprintf(cmd, M02H_DEFAULT_CMD_LEN - 1, "%s=%d,%d", AT_CMD_FLOW_CONTROL, 0, 0);
    at.send_raw(cmd, rsp, M02H_DEFAULT_RSP_LEN);
    if (strstr(rsp, M02H_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }

    return 0;
}

static int m02h_lte_status_check(void)
{
    char rsp[M02H_DEFAULT_RSP_LEN] = {0};
	char *prsp = NULL;
	unsigned int creg_stat = 0;
	unsigned int cgatt_state = 0;

    /*sim card status check*/
    at.send_raw(AT_CMD_SIM_PIN_CHECK, rsp, M02H_DEFAULT_RSP_LEN);
    if (strstr(rsp, M02H_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }

    memset(rsp, 0, M02H_DEFAULT_RSP_LEN);
    /*Signal quaility check*/
    at.send_raw(AT_CMD_SIGNAL_QUALITY_CHECK, rsp, M02H_DEFAULT_RSP_LEN);
    if (strstr(rsp, M02H_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }
    LOGI(TAG, "signal quality is %s \r\n", rsp);

    memset(rsp, 0, M02H_DEFAULT_RSP_LEN);
    /*network registration check*/
    at.send_raw(AT_CMD_NETWORK_REG_CHECK, rsp, M02H_DEFAULT_RSP_LEN);
    if (strstr(rsp, M02H_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }
	prsp = strstr(rsp, AT_CMD_NETWORK_REG_CHECK_RSP);
	if (prsp == NULL) {
		LOGW(TAG, "%s %d NETWORK_REG is not ready. rsp %s\r\n", __func__, __LINE__, rsp);
        return -2;
	}
	prsp += strlen(AT_CMD_NETWORK_REG_CHECK_RSP);
	if (m02h_rsp_parse_uint(prsp, 2, &creg_stat) != 0) {
		return -1;
	}
	if (creg_stat != 1) {
		LOGW(TAG, "%s %d creg_stat %u is not OK.\r\n", __func__, __LINE__, creg_stat);
        return -2;
	}
	LOGI(TAG, "network registration is %s \r\n", rsp);

    memset(rsp, 0, M02H_DEFAULT_RSP_LEN);
    /*GPRS attach check*/
    at.send_raw(AT_CMD_GPRS_ATTACH_CHECK, rsp, M02H_DEFAULT_RSP_LEN);
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

static int m02h_lte_got_ip(void)
{
	char cmd[M02H_DEFAULT_CMD_LEN] = {0};
    char rsp[M02H_LARGE_RSP_LEN] = {0};
	char apn[M02H_APN_LEN] = {0};
	int apn_len = 0;
	char ipaddr[16] = {0};
	char *prsp = NULL;
	char *papn = NULL;

	memset(rsp, 0, M02H_LARGE_RSP_LEN);
    at.send_raw(AT_CMD_QUERY_PDP_CONTEXT, rsp, M02H_LARGE_RSP_LEN);
    if (strstr(rsp, M02H_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }
	prsp = strstr(rsp, AT_CMD_PDP_CONTEXT_RSP);
	if (prsp == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }
	prsp += strlen(AT_CMD_PDP_CONTEXT_RSP);
	if (m02h_rsp_parse_string(prsp, 3, apn, M02H_APN_LEN) != 0) {
		return -1;
	}

	snprintf(cmd, M02H_DEFAULT_CMD_LEN - 1, "%s=%d,1,\"%s\",\"\",\"\",1", 
			AT_CMD_CONFIGURE_CONTEXT, M02H_CONTEXT_ID, apn);
	memset(rsp, 0, M02H_LARGE_RSP_LEN);
    at.send_raw(cmd, rsp, M02H_LARGE_RSP_LEN);
    if (strstr(rsp, M02H_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }
	
	snprintf(cmd, M02H_DEFAULT_CMD_LEN - 1, "%s=%d", AT_CMD_ACTIVATE_CONTEXT, M02H_CONTEXT_ID);
	memset(rsp, 0, M02H_LARGE_RSP_LEN);
    at.send_raw(cmd, rsp, M02H_LARGE_RSP_LEN);
    if (strstr(rsp, M02H_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }

	snprintf(cmd, M02H_DEFAULT_CMD_LEN - 1, "%s?", AT_CMD_ACTIVATE_CONTEXT);
	memset(rsp, 0, M02H_LARGE_RSP_LEN);
    at.send_raw(cmd, rsp, M02H_LARGE_RSP_LEN);
    if (strstr(rsp, M02H_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }
	prsp = strstr(rsp, AT_CMD_ACTIVATE_CONTEXT_RSP);
	if (prsp == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }
	prsp += strlen(AT_CMD_ACTIVATE_CONTEXT_RSP);
	if (m02h_rsp_parse_string(prsp, 4, ipaddr, 16) != 0) {
		return -1;
	}

    printf("m02h got ip %s \r\n", ipaddr);
    aos_post_event(EV_WIFI, CODE_WIFI_ON_GOT_IP, 0xdeaddead);
	
    return 0;
}

static int m02h_lte_module_init(void)
{
    int ret = 0;
    uint32_t linknum = 0;
	int i = 0;

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

	ret = m02h_lte_status_check();
	while (ret == -2 && i++ < M02H_NETREG_LOOP) {
		aos_msleep(M02H_NETREG_INTERVAL);
		ret = m02h_lte_status_check();
	} 
    if (ret) {
        LOGE(TAG, "%s %d failed \r\n", __func__, __LINE__);
        goto err;
    }
	
	ret = m02h_lte_got_ip();
    if (ret) {
        LOGE(TAG, "%s %d failed \r\n", __func__, __LINE__);
        goto err;
    }

    at.oob(AT_CMD_OOB_PREFIX, NULL, 0, m02h_lte_module_socket_event_handle, NULL);

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

static int m02h_lte_module_deinit()
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

static int m02h_lte_module_domain_to_ip(char *domain, char ip[16])
{
	char cmd[M02H_DNS_CMD_RSP_LEN] = {0};
    char rsp[M02H_DNS_CMD_RSP_LEN] = {0};
	char *prsp = NULL;
	int ret = 0;
	
	snprintf(cmd, M02H_DNS_CMD_RSP_LEN - 1, "%s=\"%s\"", AT_CMD_DOMAIN_TO_IP, domain);
	memset(rsp, 0, M02H_DNS_CMD_RSP_LEN);
	
	aos_mutex_lock(&g_domain_mutex, AOS_WAIT_FOREVER);
    at.send_raw(cmd, rsp, M02H_DNS_CMD_RSP_LEN);
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
	
	printf("domain %s get ip %s \r\n", domain, ip);
end:
	aos_mutex_unlock(&g_domain_mutex);
    return ret;
}

static int m02h_lte_module_conn_start(sal_conn_t *conn)
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

    switch (conn->type) {
        case TCP_SERVER:
            snprintf(pccmd, M02H_CONN_CMD_LEN - 1, "%s=%d,%d,\"%s\",\"%s\",%d,%d,%d", AT_CMD_START_SOCKET_SERVICE, 
					M02H_DEFAULT_CONTEXT_ID, linkid, "TCP LISTENER", "127.0.0.1", 0, conn->l_port, MODE_DIRECT_PUSH);
            at.send_raw(pccmd, rsp, M02H_DEFAULT_RSP_LEN);
            if (strstr(rsp, M02H_AT_CMD_SUCCESS_RSP) == NULL) {
                LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
                goto err;
            }
            break;
        case TCP_CLIENT:
            snprintf(pccmd, M02H_CONN_CMD_LEN - 1, "%s=%d,%d,\"%s\",\"%s\",%d,%d,%d", AT_CMD_START_SOCKET_SERVICE, 
					M02H_DEFAULT_CONTEXT_ID, linkid, "TCP", conn->addr, conn->r_port, 0, MODE_DIRECT_PUSH);
            at.send_raw(pccmd, rsp, M02H_DEFAULT_RSP_LEN);
            if (strstr(rsp, M02H_AT_CMD_SUCCESS_RSP) == NULL) {
                LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
                goto err;
            }
            break;
        case UDP_UNICAST:
            snprintf(pccmd, M02H_CONN_CMD_LEN - 1, "%s=%d,%d,\"%s\",\"%s\",%d,%d,%d", AT_CMD_START_SOCKET_SERVICE, 
					M02H_DEFAULT_CONTEXT_ID, linkid, "UDP", conn->addr, conn->r_port, 0, MODE_DIRECT_PUSH);
            at.send_raw(pccmd, rsp, M02H_DEFAULT_RSP_LEN);
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
    aos_free(pccmd);
    aos_mutex_lock(&g_link_mutex, AOS_WAIT_FOREVER);
    g_link[linkid].fd = -1;
    aos_mutex_unlock(&g_link_mutex);
    return -1;
}

static int m02h_lte_module_conn_close(int fd, int32_t remote_port)
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

    snprintf(cmd, M02H_DEFAULT_CMD_LEN - 1, "%s=%d", AT_CMD_CLOSE_SOCKET_SERVICE, linkid);
    at.send_raw(cmd, rsp, M02H_DEFAULT_RSP_LEN);
    if (strstr(rsp, M02H_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "cmd %s rsp is %s \r\n", cmd, rsp);
        ret = -1;
    }

    aos_mutex_lock(&g_link_mutex, AOS_WAIT_FOREVER);
    g_link[linkid].fd = -1;
    aos_mutex_unlock(&g_link_mutex);

    return ret;
}

static int m02h_lte_module_send(int fd, uint8_t *data, uint32_t len,
                                   char remote_ip[16], int32_t remote_port)
{
    int  linkid;
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

    snprintf(cmd, M02H_DEFAULT_CMD_LEN - 1, "%s=%d,%d", AT_CMD_SEND_DATA, linkid, len);

    /*TODO data send fail rsp is SEND FAIL*/
    at.send_data_2stage((const char *)cmd, (const char *)data, len, rsp, sizeof(rsp));
    if (strstr(rsp, M02H_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "cmd %s rsp %s at %s %d failed \r\n", cmd, rsp, __func__, __LINE__);
        return -1;
    }

    return 0;
}

static int m02h_lte_packet_input_cb_register(netconn_data_input_cb_t cb)
{
    if (cb) {
        g_netconn_data_input_cb = cb;
    }
    return 0;
}


sal_op_t m02h_sal_opt = {
    .version = "1.0.0",
    .init = m02h_lte_module_init,
    .start = m02h_lte_module_conn_start,
    .send = m02h_lte_module_send,
	.domain_to_ip = m02h_lte_module_domain_to_ip,
    .close = m02h_lte_module_conn_close,
    .deinit = m02h_lte_module_deinit,
    .register_netconn_data_input_cb = m02h_lte_packet_input_cb_register,
};


int m02h_sal_init(void)
{
    return sal_module_register(&m02h_sal_opt);
}
