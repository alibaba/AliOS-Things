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

#define TAG "n26a_nbiot_module"

#define N26A_AT_CMD_SUCCESS_RSP "OK"
#define N26A_AT_CMD_FAIL_RSP "ERROR"

#define AT_CMD_TEST "AT\r\n"
#define AT_CMD_TEST_RESULT "\r\nOK\r\n"

#define AT_CMD_ECHO_OFF   "ATE0"
#define AT_CMD_BAUDRATE_SET "AT+IPR"
#define AT_CMD_FLOW_CONTROL "AT+IFC"
#define AT_CMD_SLEEP_MODE "AT+SM"

#define AT_CMD_SAVE_CONFIG  "AT&W"

#define AT_CMD_SIM_PIN_CHECK  "AT+CPIN?"
#define AT_CMD_SIGNAL_QUALITY_CHECK "AT+CSQ"
#define AT_CMD_NETWORK_REG_CHECK "AT+CREG?"
#define AT_CMD_GPRS_ATTACH_CHECK "AT+CGATT?"

#define AT_CMD_DOMAIN_TO_IP "AT+EDNS"
#define AT_CMD_DOMAIN_RSP   "\r\n+EDNS:"

#define AT_CMD_CREATE_SOCKET "AT+ESOC"
#define AT_CMD_SOCKET_RSP "+ESOC="
#define AT_CMD_BIND_PORT "AT+ESOB"
#define AT_CMD_CONNECT_REMOTE "AT+ESOCON"

#define AT_CMD_STOP_CONN "AT+ESOCL"

#define AT_CMD_SEND_DATA "AT+ESOSEND"

#define AT_CMD_GOT_IP "\r\n+IP: "
#define AT_CMD_DATA_RECV "\r\n+ESONMI="
#define AT_CMD_ERR_RECV "\r\n+ESOERR="

#define N26A_DEFAULT_CMD_LEN    64
#define N26A_DEFAULT_RSP_LEN    64

#define N26A_MAX_LINK_NUM       5

#define N26A_DOMAIN_MAX_LEN     256
#define N26A_DOMAIN_RSP_MAX_LEN 512
#define N26A_DOMAIN_CMD_LEN (sizeof(AT_CMD_DOMAIN_TO_IP) + N26A_DOMAIN_MAX_LEN + 3)
#define N26A_CONN_CMD_LEN   (N26A_DOMAIN_MAX_LEN + N26A_DEFAULT_CMD_LEN)

#define M02H_NETREG_INTERVAL            1000
#define M02H_NETREG_LOOP                30

/* Change to include data slink for each link id respectively. <TODO> */
typedef struct link_s {
    int fd;
    aos_sem_t sem_start;
    aos_sem_t sem_close;
} link_t;

static uint8_t inited = 0;
static link_t g_link[N26A_MAX_LINK_NUM];
static aos_mutex_t g_link_mutex;
static aos_mutex_t g_domain_mutex;
static aos_sem_t   g_domain_sem;
static char *g_pcdomain_rsp = NULL;
static char g_senddata[3000 + 1 + N26A_DEFAULT_CMD_LEN];


static netconn_data_input_cb_t g_netconn_data_input_cb;

static void str2hex(const char *string, uint8_t *hex, uint32_t hex_len)
{
	uint32_t i;
	char shex[3] = {0};

	for (i = 0; i < hex_len; ++i) {
		memcpy(shex, &string[i << 1], 2);
		hex[i] = (uint8_t)strtoul(shex, 0, 16);
	}
}

static void hex2str(const uint8_t *hex, char *string, uint32_t hex_len)
{
	uint32_t i;

    for (i = 0; i < hex_len; ++i) {
        sprintf(&string[i << 1], "%02x", hex[i]);
    }
	string[hex_len << 1] = 0;
}


static int fd_to_linkid(int fd)
{
    int link_id;

    if (aos_mutex_lock(&g_link_mutex, AOS_WAIT_FOREVER) != 0) {
        LOGE(TAG, "Failed to lock mutex (%s).", __func__);
        return -1;
    }
    for (link_id = 0; link_id < N26A_MAX_LINK_NUM; link_id++) {
        if (g_link[link_id].fd == fd) {
            break;
        }
    }

    aos_mutex_unlock(&g_link_mutex);

    return link_id;
}

static void n26a_nbiot_domain_rsp_callback(void *arg, char *rspinfo, int rsplen)
{
    if (NULL == rspinfo || rsplen == 0) {
        LOGE(TAG, "invalid input at %s \r\n", __func__);
        return;
    }

    if (NULL == g_pcdomain_rsp) {
        LOGE(TAG, "domain rsp is %s but buffer is NULL \r\n", rspinfo);
        return;
    }

    printf("get domain rsp %s \r\n", rspinfo);
    memcpy(g_pcdomain_rsp, rspinfo, rsplen);
    aos_sem_signal(&g_domain_sem);
}

static void n26a_nbiot_module_socket_data_handle(void *arg, char *rspinfo, int rsplen)
{
    unsigned char uclinkid = 0;
    unsigned char unusesymbol = 0;
    unsigned char datalen[6] = {0};
	char          *recvdata = NULL;
    int           i = 0;
    int           j = 0;
    uint32_t      len = 0;
	uint32_t      slen = 0;
    int           linkid = 0;
	int           ret = 0;

    at.read(&uclinkid, 1);
    linkid = uclinkid - '0';
    if (linkid < 0 || linkid >=  N26A_MAX_LINK_NUM) {
        LOGE(TAG, "Invalid link id 0x%02x !!!\r\n", linkid);
        return;
    }

    /*eat , char*/
    at.read(&unusesymbol, 1);

    /* get data len */
    i = 0;
    do {
        at.read(&datalen[i], 1);
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
	slen = len << 1;

	recvdata = (char *)aos_malloc(slen + 1);
    if (!recvdata) {
        LOGE(TAG, "Error: %s %d out of memory.", __func__, __LINE__);
        return;
    }
	
    ret = at.read(recvdata, slen);
	if (ret != slen) {
		LOGE(TAG, "at read error recv %d want %d!\n", ret, slen);
		goto exit;
    }
	str2hex(recvdata, recvdata, len);

    if (g_netconn_data_input_cb && (g_link[linkid].fd >= 0)) {
        if (g_netconn_data_input_cb(g_link[linkid].fd, recvdata, len, NULL, 0)) {
            LOGE(TAG, " %s socket %d get data len %d fail to post to sal, drop it\n",
                 __func__, g_link[linkid].fd, len);
        }
    }
    LOGI(TAG, "%s socket data on link %d with length %d posted to sal\n",
         __func__, linkid, len);

exit:
	if (recvdata) {
		aos_free(recvdata);
	}

    return;
}

static void n26a_nbiot_module_socket_err_handle(void *arg, char *rspinfo, int rsplen)
{
	unsigned char uclinkid = 0;
	unsigned char unusesymbol = 0;
	unsigned char cerrcode = 0;
	int           linkid = 0;
	int           errcode = 0;
	
	at.read(&uclinkid, 1);
    linkid = uclinkid - '0';
    if (linkid < 0 || linkid >=  N26A_MAX_LINK_NUM) {
        LOGE(TAG, "Invalid link id 0x%02x !!!\r\n", linkid);
        return;
    }

    /*eat , char*/
    at.read(&unusesymbol, 1);
	
	at.read(&cerrcode, 1);
	if (cerrcode > '9' || cerrcode < '0') {
		LOGE(TAG, "Invalid len cerrcode!!!, cerrcode is %c \r\n", cerrcode);
		return;
	}
	errcode = cerrcode - '0';
	switch (errcode) {
		case 1:
			LOGI(TAG, "Server conn (%d) reset by peer point.", linkid);
			break;
		case 2:
			LOGI(TAG, "Server conn (%d) disconnected.", linkid);
			break;
		default:
			LOGI(TAG, "No one handle this unkown event. errcode: %d", errcode);
			return;
	}
}

static void n26a_get_ip_delayed_action(void *arg)
{
    printf("post got ip event \r\n");
    aos_post_event(EV_WIFI, CODE_WIFI_ON_GOT_IP, 0xdeaddead);
}

static void n26a_nbiot_gotip_handle(void *arg, char *rspinfo, int rsplen)
{
	unsigned char ipaddr[16] = {0};
	unsigned char unusesymbol = 0;
	int i = 0;

	do {
		at.read(&ipaddr[i], 1);
		if (ipaddr[i] == '\r') {
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
	
	/*eat \n char*/
	at.read(&unusesymbol, 1);
	
	printf("n26a got ip %s \r\n", ipaddr);
	
	aos_msleep(1000);
	n26a_get_ip_delayed_action(NULL);
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

int n26a_uart_selfadaption(const char *command, const char *rsp, uint32_t rsplen)
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

    aos_timer_new(&test_timer, at_start_test, command, 100, 1);
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

static int n26a_uart_init(void)
{
    int ret = 0;
    char cmd[N26A_DEFAULT_CMD_LEN] = {0};
    char rsp[N26A_DEFAULT_RSP_LEN] = {0};

    /* uart baudrate self adaption*/
    ret = n26a_uart_selfadaption(AT_CMD_TEST, AT_CMD_TEST_RESULT, strlen(AT_CMD_TEST_RESULT));
    if (ret) {
        LOGE(TAG, "n26a_uart_selfadaption fail \r\n");
        return ret;
    }

    /*turn off echo*/
    at.send_raw(AT_CMD_ECHO_OFF, rsp, N26A_DEFAULT_RSP_LEN);
    if (strstr(rsp, N26A_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }

    /*set baudrate 115200*/
    snprintf(cmd, N26A_DEFAULT_CMD_LEN - 1, "%s=%d", AT_CMD_BAUDRATE_SET, AT_UART_BAUDRATE);
    at.send_raw(cmd, rsp, N26A_DEFAULT_RSP_LEN);
    if (strstr(rsp, N26A_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }


    memset(cmd, 0, N26A_DEFAULT_CMD_LEN);
    memset(rsp, 0, N26A_DEFAULT_RSP_LEN);
    /*turn off flow control*/
    snprintf(cmd, N26A_DEFAULT_CMD_LEN - 1, "%s=%d,%d", AT_CMD_FLOW_CONTROL, 0, 0);
    at.send_raw(cmd, rsp, N26A_DEFAULT_RSP_LEN);
    if (strstr(rsp, N26A_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }
	
	memset(cmd, 0, N26A_DEFAULT_CMD_LEN);
    memset(rsp, 0, N26A_DEFAULT_RSP_LEN);
    /*prevent system enter sleep mode*/
    snprintf(cmd, N26A_DEFAULT_CMD_LEN - 1, "%s=%s", AT_CMD_SLEEP_MODE, "LOCK_FOREVER");
    at.send_raw(cmd, rsp, N26A_DEFAULT_RSP_LEN);
    if (strstr(rsp, N26A_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }

    memset(rsp, 0, N26A_DEFAULT_RSP_LEN);
    /*save configuration */
    at.send_raw(AT_CMD_SAVE_CONFIG, rsp, N26A_DEFAULT_RSP_LEN);
    if (strstr(rsp, N26A_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }

    return 0;
}

static int n26a_nbiot_status_check(void)
{
    char rsp[N26A_DEFAULT_RSP_LEN] = {0};

    /*sim card status check*/
    at.send_raw(AT_CMD_SIM_PIN_CHECK, rsp, N26A_DEFAULT_RSP_LEN);
    if (strstr(rsp, N26A_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }

    memset(rsp, 0, N26A_DEFAULT_RSP_LEN);
    /*Signal quaility check*/
    at.send_raw(AT_CMD_SIGNAL_QUALITY_CHECK, rsp, N26A_DEFAULT_RSP_LEN);
    if (strstr(rsp, N26A_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }
    LOGI(TAG, "signal quality is %s \r\n", rsp);

    memset(rsp, 0, N26A_DEFAULT_RSP_LEN);
    /*network registration check*/
    at.send_raw(AT_CMD_NETWORK_REG_CHECK, rsp, N26A_DEFAULT_RSP_LEN);
    if (strstr(rsp, N26A_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }
    LOGI(TAG, "network registration is %s \r\n", rsp);


    memset(rsp, 0, N26A_DEFAULT_RSP_LEN);
    /*GPRS attach check*/
    at.send_raw(AT_CMD_GPRS_ATTACH_CHECK, rsp, N26A_DEFAULT_RSP_LEN);
    if (strstr(rsp, N26A_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }
    LOGI(TAG, "nbiot attach check %s \r\n", rsp);

    return 0;
}

static int n26a_nbiot_module_init(void)
{
    int ret = 0;
    uint32_t linknum = 0;
	int i = 0;

    if (inited) {
        LOGI(TAG, "n26a nbiot module have already inited \r\n");
        return 0;
    }

    g_pcdomain_rsp = aos_malloc(N26A_DOMAIN_RSP_MAX_LEN);
    if (NULL == g_pcdomain_rsp) {
        LOGE(TAG, "%s %d failed \r\n", __func__, __LINE__);
        goto err;
    }

    memset(g_pcdomain_rsp , 0, N26A_DOMAIN_RSP_MAX_LEN);

    if (0 != aos_mutex_new(&g_link_mutex)) {
        LOGE(TAG, "Creating link mutex failed (%s %d).", __func__, __LINE__);
        goto err;
    }

    if (0 != aos_mutex_new(&g_domain_mutex)) {
        LOGE(TAG, "Creating link mutex failed (%s %d).", __func__, __LINE__);
        goto err;
    }

    if (0 != aos_sem_new(&g_domain_sem, 0)) {
        LOGE(TAG, "Creating domain semaphore failed (%s %d).", __func__, __LINE__);
        goto err;
    }

    memset(g_link, 0, sizeof(g_link));

    for (linknum = 0; linknum < N26A_MAX_LINK_NUM; linknum++) {
        g_link[linknum].fd = -1;
    }
    ret = n26a_uart_init();
    if (ret) {
        LOGE(TAG, "%s %d failed \r\n", __func__, __LINE__);
        goto err;
    }

	ret = n26a_nbiot_status_check();
	while (ret != 0 && i++ < M02H_NETREG_LOOP) {
		aos_msleep(M02H_NETREG_INTERVAL);
		ret = n26a_nbiot_status_check();
	} 
    if (ret) {
        LOGE(TAG, "%s %d failed \r\n", __func__, __LINE__);
        goto err;
    }

    /* reg oob for domain and packet input*/
    at.oob(AT_CMD_DOMAIN_RSP, AT_RECV_PREFIX, N26A_DOMAIN_RSP_MAX_LEN,
           n26a_nbiot_domain_rsp_callback, NULL);
    at.oob(AT_CMD_DATA_RECV, NULL, 0, n26a_nbiot_module_socket_data_handle, NULL);
	at.oob(AT_CMD_ERR_RECV, NULL, 0, n26a_nbiot_module_socket_err_handle, NULL);
	at.oob(AT_CMD_GOT_IP, NULL, 0, n26a_nbiot_gotip_handle, NULL);

    inited = 1;

    return 0;
err:
    if (g_pcdomain_rsp != NULL) {
        aos_free(g_pcdomain_rsp);
        g_pcdomain_rsp = NULL;
    }

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

static int n26a_nbiot_module_deinit()
{
    if (!inited) {
        return 0;
    }

    if (g_pcdomain_rsp != NULL) {
        aos_free(g_pcdomain_rsp);
        g_pcdomain_rsp = NULL;
    }

    if (aos_mutex_is_valid(&g_link_mutex)) {
        aos_mutex_free(&g_link_mutex);
    }

    if (aos_mutex_is_valid(&g_domain_mutex)) {
        aos_mutex_free(&g_domain_mutex);
    }

    if (aos_sem_is_valid(&g_domain_sem)) {
        aos_sem_free(&g_domain_sem);
    }

    inited = 0;
    return 0;
}

static int n26a_nbiot_module_domain_to_ip(char *domain, char ip[16])
{
    char *pccmd = NULL;
    char *head = NULL;
    char *end = NULL;
    char rsp[N26A_DEFAULT_RSP_LEN] = {0};
	unsigned int domain_len = strlen(domain);

    if (!inited) {
        LOGE(TAG, "%s n26a nbiot module haven't init yet \r\n", __func__);
        return -1;
    }

    if (NULL == domain || NULL == ip) {
        LOGE(TAG, "invalid input at %s \r\n", __func__);
        return -1;
    }

    if (domain_len > N26A_DOMAIN_MAX_LEN) {
        LOGE(TAG, "domain length oversize at %s \r\n", __func__);
        return -1;
    }

    pccmd = aos_malloc(N26A_DOMAIN_CMD_LEN);
    if (NULL == pccmd) {
        LOGE(TAG, "fail to malloc memory %d at %s \r\n", N26A_DOMAIN_CMD_LEN, __func__);
        return -1;
    }

    memset(pccmd, 0, N26A_DOMAIN_CMD_LEN);
    snprintf(pccmd, N26A_DEFAULT_CMD_LEN - 1, "%s=\"%s\"", AT_CMD_DOMAIN_TO_IP, domain);

    aos_mutex_lock(&g_domain_mutex, AOS_WAIT_FOREVER);
    at.send_raw(pccmd, rsp, N26A_DEFAULT_RSP_LEN);
    if (strstr(rsp, N26A_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        goto err;
    }

    /*TODO wait for reponse for ever for now*/
    aos_sem_wait(&g_domain_sem, AOS_WAIT_FOREVER);
    /*
     * formate is :
       +EDNS:111.13.100.92
    */
    if ((head = strstr(g_pcdomain_rsp, AT_CMD_DOMAIN_RSP)) == NULL) {
        LOGE(TAG, "invalid domain rsp %s at %d\r\n", g_pcdomain_rsp, __LINE__);
        goto err;
    }

    head += strlen(AT_CMD_DOMAIN_RSP);
    if ((end = strstr(head, "\r\n")) == NULL) {
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
    memset(g_pcdomain_rsp, 0, N26A_DOMAIN_RSP_MAX_LEN);
    aos_mutex_unlock(&g_domain_mutex);

    printf("domain %s get ip %s \r\n", domain , ip);
    return 0;
err:

    aos_free(pccmd);
    memset(g_pcdomain_rsp, 0, N26A_DOMAIN_RSP_MAX_LEN);
    aos_mutex_unlock(&g_domain_mutex);

    return -1;
}

static int n26a_nbiot_module_conn_start(sal_conn_t *conn)
{
    int  linkid = 0;
	char *sid = NULL;
    char *pccmd = NULL;
    char rsp[N26A_DEFAULT_RSP_LEN] = {0};

    if (!inited) {
        LOGE(TAG, "%s n26a nbiot module haven't init yet \r\n", __func__);
        return -1;
    }

    if (!conn || !conn->addr) {
        LOGE(TAG, "%s %d - invalid input \r\n", __func__, __LINE__);
        return -1;
    }
#if 0
    /*if input addr is a domain, then turn it into ip addr */
    if (n26a_nbiot_module_domain_to_ip(conn->addr, ipaddr) != 0) {
        if (strlen(conn->addr) >= sizeof(ipaddr)) {
            LOGE(TAG, "%s invalid server addr %s \r\n", __func__, conn->addr);
            return -1;
        }
        strcpy(ipaddr, conn->addr);
    }
#endif

    pccmd = aos_malloc(N26A_CONN_CMD_LEN);
    if (NULL == pccmd) {
        LOGE(TAG, "fail to malloc %d at %s \r\n", N26A_CONN_CMD_LEN, __func__);
        goto err;
    }
    memset(pccmd, 0, N26A_CONN_CMD_LEN);

    switch (conn->type) {
        case TCP_SERVER:
			snprintf(pccmd, N26A_CONN_CMD_LEN - 1, "%s=%d,%d,%d", AT_CMD_CREATE_SOCKET, 1, 1, 1);
			at.send_raw(pccmd, rsp, N26A_DEFAULT_RSP_LEN);
            if (strstr(rsp, N26A_AT_CMD_SUCCESS_RSP) == NULL) {
                LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
                goto err;
            }
			sid = strstr(rsp, AT_CMD_SOCKET_RSP);
			if (sid == NULL) {
				LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
                goto err;
			}
			sid += strlen(AT_CMD_SOCKET_RSP);
			linkid = atoi(sid);
			aos_mutex_lock(&g_link_mutex, AOS_WAIT_FOREVER);
			if (g_link[linkid].fd >= 0) {
				LOGE(TAG, "Socket ID returned has been occupied, %s failed. \r\n", __func__);
				return -1;
			}
			g_link[linkid].fd = conn->fd;
			aos_mutex_unlock(&g_link_mutex);
			memset(rsp, 0, N26A_DEFAULT_RSP_LEN);
			snprintf(pccmd, N26A_CONN_CMD_LEN - 1, "%s=%d,%d", AT_CMD_BIND_PORT, linkid, conn->l_port);
			at.send_raw(pccmd, rsp, N26A_DEFAULT_RSP_LEN);
            if (strstr(rsp, N26A_AT_CMD_SUCCESS_RSP) == NULL) {
                LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
                goto err;
            }
			break;
        case TCP_CLIENT:
			snprintf(pccmd, N26A_CONN_CMD_LEN - 1, "%s=%d,%d,%d", AT_CMD_CREATE_SOCKET, 1, 1, 1);
			at.send_raw(pccmd, rsp, N26A_DEFAULT_RSP_LEN);
            if (strstr(rsp, N26A_AT_CMD_SUCCESS_RSP) == NULL) {
                LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
                goto err;
            }
			sid = strstr(rsp, AT_CMD_SOCKET_RSP);
			if (sid == NULL) {
				LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
                goto err;
			}
			sid += strlen(AT_CMD_SOCKET_RSP);
			linkid = atoi(sid);
			aos_mutex_lock(&g_link_mutex, AOS_WAIT_FOREVER);
			if (g_link[linkid].fd >= 0) {
				LOGE(TAG, "Socket ID returned has been occupied, %s failed. \r\n", __func__);
				return -1;
			}
			g_link[linkid].fd = conn->fd;
			aos_mutex_unlock(&g_link_mutex);
			memset(rsp, 0, N26A_DEFAULT_RSP_LEN);
			snprintf(pccmd, N26A_CONN_CMD_LEN - 1, "%s=%d,%d,\"%s\"", AT_CMD_CONNECT_REMOTE, linkid, conn->r_port, conn->addr);
			at.send_raw(pccmd, rsp, N26A_DEFAULT_RSP_LEN);
            if (strstr(rsp, N26A_AT_CMD_SUCCESS_RSP) == NULL) {
                LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
                goto err;
            }
			break;
        case UDP_UNICAST:
			snprintf(pccmd, N26A_CONN_CMD_LEN - 1, "%s=%d,%d,%d", AT_CMD_CREATE_SOCKET, 1, 2, 1);
			at.send_raw(pccmd, rsp, N26A_DEFAULT_RSP_LEN);
            if (strstr(rsp, N26A_AT_CMD_SUCCESS_RSP) == NULL) {
                LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
                goto err;
            }
			sid = strstr(rsp, AT_CMD_SOCKET_RSP);
			if (sid == NULL) {
				LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
                goto err;
			}
			sid += strlen(AT_CMD_SOCKET_RSP);
			linkid = atoi(sid);
			aos_mutex_lock(&g_link_mutex, AOS_WAIT_FOREVER);
			if (g_link[linkid].fd >= 0) {
				LOGE(TAG, "Socket ID returned has been occupied, %s failed. \r\n", __func__);
				return -1;
			}
			g_link[linkid].fd = conn->fd;
			aos_mutex_unlock(&g_link_mutex);
			memset(rsp, 0, N26A_DEFAULT_RSP_LEN);
			snprintf(pccmd, N26A_CONN_CMD_LEN - 1, "%s=%d,%d,\"%s\"", AT_CMD_CONNECT_REMOTE, linkid, conn->r_port, conn->addr);
			at.send_raw(pccmd, rsp, N26A_DEFAULT_RSP_LEN);
            if (strstr(rsp, N26A_AT_CMD_SUCCESS_RSP) == NULL) {
                LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
                goto err;
            }
			break;
        case SSL_CLIENT:
        case UDP_BROADCAST:
        default:
            LOGE(TAG, "n26a nbiot module connect type %d not support \r\n", conn->type);
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

static int n26a_nbiot_module_conn_close(int fd, int32_t remote_port)
{
    int  linkid = 0;
    int  ret = 0;
    char cmd[N26A_DEFAULT_CMD_LEN] = {0};
    char rsp[N26A_DEFAULT_RSP_LEN] = {0};

    if (!inited) {
        LOGE(TAG, "%s n26a nbiot module haven't init yet \r\n", __func__);
        return -1;
    }

    linkid = fd_to_linkid(fd);
    if (linkid >= N26A_MAX_LINK_NUM) {
        LOGE(TAG, "No connection found for fd (%d) in %s \r\n", fd, __func__);
        return -1;
    }

    snprintf(cmd, N26A_DEFAULT_CMD_LEN - 1, "%s=%d", AT_CMD_STOP_CONN, linkid);
    at.send_raw(cmd, rsp, N26A_DEFAULT_RSP_LEN);
    if (strstr(rsp, N26A_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "cmd %s rsp is %s \r\n", cmd, rsp);
        ret = -1;
    }

    aos_mutex_lock(&g_link_mutex, AOS_WAIT_FOREVER);
    g_link[linkid].fd = -1;
    aos_mutex_unlock(&g_link_mutex);

    return ret;
}

static int n26a_nbiot_module_send(int fd, uint8_t *data, uint32_t len,
                                   char remote_ip[16], int32_t remote_port)
{
    int  linkid;
	uint32_t slen = len << 1;
    char rsp[N26A_DEFAULT_RSP_LEN] = {0};
	char *senddata = NULL;
	int ret = 0;

    if (!inited) {
        LOGE(TAG, "%s n26a nbiot module haven't init yet \r\n", __func__);
        return -1;
    }

    linkid = fd_to_linkid(fd);
    if (linkid >= N26A_MAX_LINK_NUM) {
        LOGE(TAG, "No connection found for fd (%d) in %s \r\n", fd, __func__);
        return -1;
    }

	senddata = (char *)aos_malloc(slen + N26A_DEFAULT_CMD_LEN + 1);
    if (!senddata) {
        LOGE(TAG, "Error: %s %d out of memory.", __func__, __LINE__);
        return -1;
    }
	
    snprintf(senddata, N26A_DEFAULT_CMD_LEN, "%s=%d,%d,", AT_CMD_SEND_DATA, linkid, len);
	hex2str(data, senddata + strlen(senddata), len);

    /*TODO data send fail rsp is SEND FAIL*/
	at.send_raw(senddata, rsp, N26A_DEFAULT_RSP_LEN);
    if (strstr(rsp, N26A_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "cmd %s rsp %s at %s %d failed \r\n", senddata, rsp, __func__, __LINE__);
        ret = -1;
    }

	if (senddata) {
		aos_free(senddata);
	}

    return ret;
}

static int n26a_nbiot_packet_input_cb_register(netconn_data_input_cb_t cb)
{
    if (cb) {
        g_netconn_data_input_cb = cb;
    }
    return 0;
}


sal_op_t n26a_sal_opt = {
    .version = "1.0.0",
    .init = n26a_nbiot_module_init,
    .start = n26a_nbiot_module_conn_start,
    .send = n26a_nbiot_module_send,
    .domain_to_ip = n26a_nbiot_module_domain_to_ip,
    .close = n26a_nbiot_module_conn_close,
    .deinit = n26a_nbiot_module_deinit,
    .register_netconn_data_input_cb = n26a_nbiot_packet_input_cb_register,
};


int n26a_sal_init(void)
{
    return sal_module_register(&n26a_sal_opt);
}
