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

#define TAG "sim800_gprs_module"

#define SIM800_AT_CMD_SUCCESS_RSP "OK"
#define SIM800_AT_CMD_FAIL_RSP "ERROR"

#define AT_CMD_TEST "AT\r\n"
#define AT_CMD_TEST_RESULT "\r\nOK\r\n"

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
#define AT_CMD_TCPIP_MODE "AT+CIPMODE"
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
static char *g_pcdomain_rsp = NULL;

static netconn_data_input_cb_t g_netconn_data_input_cb;


static int fd_to_linkid(int fd)
{
    int link_id;
    
    if (aos_mutex_lock(&g_link_mutex, AOS_WAIT_FOREVER) != 0) {
        LOGE(TAG, "Failed to lock mutex (%s).", __func__);
        return -1;
    }
    for (link_id = 0; link_id < SIM800_MAX_LINK_NUM; link_id++) {
        if (g_link[link_id].fd == fd) 
            break;
    }

    aos_mutex_unlock(&g_link_mutex);

    return link_id;
}

static void sim800_gprs_domain_rsp_callback(void *arg, char *rspinfo, int rsplen)
{
    if (NULL == rspinfo || rsplen == 0){
        LOGE(TAG, "invalid input at %s \r\n", __func__);
        return;
    }

    if (NULL == g_pcdomain_rsp){
        LOGE(TAG, "domain rsp is %s but buffer is NULL \r\n", rspinfo);
        return;
    }

    printf("get domain rsp %s \r\n", rspinfo);
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
    
    at.read(&uclinkid, 1);
    linkid = uclinkid - '0';
    if (linkid < 0 || linkid >=  SIM800_MAX_LINK_NUM){
        LOGE(TAG, "Invalid link id 0x%02x !!!\r\n", linkid);
        return;
    }

    /*eat , char*/
    at.read(&unusesymbol, 1);

    /* get data len */
    i = 0;
    do {
        at.read(&datalen[i], 1);
        if (datalen[i] == ',') break;
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
        at.read(&ipaddr[i], 1);
        if (ipaddr[i] == ':') break;
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
        at.read(&port[i], 1);
        if (port[i] == '\r') break;
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
    at.read(&unusesymbol, 1);

    for (j = 0; j < i; j++) {
        remoteport = remoteport * 10 + port[j] - '0';
    }

    //printf("link %d get data from %s %s ,len %s \r\n", linkid, ipaddr, port, datalen);
    /* Prepare socket data */
    recvdata = (char *)aos_malloc(len + 1);
    if (!recvdata) {
        LOGE(TAG, "Error: %s %d out of memory.", __func__, __LINE__);
        return;
    }
    
    memset(recvdata, 0, len + 1);
    
    at.read(recvdata, len);
    
    if (g_netconn_data_input_cb && (g_link[linkid].fd >= 0)){
        if (g_netconn_data_input_cb(g_link[linkid].fd, recvdata, len, ipaddr, remoteport)){
            LOGE(TAG, " %s socket %d get data len %d fail to post to sal, drop it\n",
                 __func__, g_link[linkid].fd, len);
        }
    }
    LOGD(TAG, "%s socket data on link %d with length %d posted to sal\n",
         __func__, linkid, len);
    aos_free(recvdata);
    return;
}

static void at_start_test(void* psttimer,void *command)
{
    int ret = 0;
    static int flag = 0;

    if (flag == 0){
        printf("send at command %s \r\n", command);
        flag = 1;
    }
    
    ret = at.write(command, strlen(command));
    if (ret < 0){
        LOGE(TAG, "uart send command %s at %s %d failed ret is %d \r\n", command, __FILE__, __LINE__, ret);
    }
    
}

int sim800_uart_selfadaption(const char *command, const char *rsp, uint32_t rsplen)
{
    char *buffer = NULL;
    int   ret    = 0;
    aos_timer_t  test_timer;
    
    if (NULL == command || NULL == rsp || 0 == rsplen){
        LOGE(TAG, "invalid input %s %d\r\n", __FILE__, __LINE__);
        return -1;
    }

    buffer = aos_malloc(rsplen * 3 + 1);
    if (NULL == buffer){
        LOGE(TAG, "fail to malloc memory size %d at %s %d \r\n", rsplen * 3, __FILE__, __LINE__);
        return -1;
    }

    memset(buffer, 0, rsplen * 3 + 1);

    aos_timer_new(&test_timer, at_start_test, command, 10, 1);
    aos_timer_start(&test_timer);
    
    while(true){
        ret = at.read(buffer, rsplen * 3);

        if(ret > 0 && (strstr(buffer, rsp) != NULL)){
            break;
        }
    }
    aos_timer_stop(&test_timer);
    aos_timer_free(&test_timer);
    aos_free(buffer);

    return 0;
}

static int sim800_uart_init(void)
{
    int ret = 0;
    char cmd[SIM800_DEFAULT_CMD_LEN] = {0};
    char rsp[SIM800_DEFAULT_RSP_LEN] = {0};

    /* uart baudrate self adaption*/
    ret = sim800_uart_selfadaption(AT_CMD_TEST, AT_CMD_TEST_RESULT, strlen(AT_CMD_TEST_RESULT));
    if (ret){
        LOGE(TAG, "sim800_uart_selfadaption fail \r\n");
        return ret;
    }

    /*turn off echo*/
    at.send_raw(AT_CMD_ECHO_OFF, rsp, SIM800_DEFAULT_RSP_LEN);
    if (strstr(rsp, SIM800_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }
    
    /*set baudrate 115200*/
    snprintf(cmd, SIM800_DEFAULT_CMD_LEN - 1, "%s=%d", AT_CMD_BAUDRATE_SET, AT_UART_BAUDRATE);
    at.send_raw(cmd, rsp, SIM800_DEFAULT_RSP_LEN);
    if (strstr(rsp, SIM800_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }

    
    memset(cmd, 0, SIM800_DEFAULT_CMD_LEN);
    memset(rsp, 0, SIM800_DEFAULT_RSP_LEN);
    /*turn off flow control*/
    snprintf(cmd, SIM800_DEFAULT_CMD_LEN - 1, "%s=%d,%d", AT_CMD_FLOW_CONTROL, 0, 0);
    at.send_raw(cmd, rsp, SIM800_DEFAULT_RSP_LEN);
    if (strstr(rsp, SIM800_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }

    memset(rsp, 0, SIM800_DEFAULT_RSP_LEN);
    /*save configuration */
    at.send_raw(AT_CMD_SAVE_CONFIG, rsp, SIM800_DEFAULT_RSP_LEN);
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
    at.send_raw(AT_CMD_SIM_PIN_CHECK, rsp, SIM800_DEFAULT_RSP_LEN);
    if (strstr(rsp, SIM800_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }
    
    memset(rsp, 0, SIM800_DEFAULT_RSP_LEN);
    /*Signal quaility check*/
    at.send_raw(AT_CMD_SIGNAL_QUALITY_CHECK, rsp, SIM800_DEFAULT_RSP_LEN);
    if (strstr(rsp, SIM800_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }
    LOGI(TAG, "signal quality is %s \r\n", rsp);

    memset(rsp, 0, SIM800_DEFAULT_RSP_LEN);
    /*network registration check*/
    at.send_raw(AT_CMD_NETWORK_REG_CHECK, rsp, SIM800_DEFAULT_RSP_LEN);
    if (strstr(rsp, SIM800_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }
    LOGI(TAG, "network registration is %s \r\n", rsp);

    
    memset(rsp, 0, SIM800_DEFAULT_RSP_LEN);
    /*GPRS attach check*/
    at.send_raw(AT_CMD_GPRS_ATTACH_CHECK, rsp, SIM800_DEFAULT_RSP_LEN);
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
    at.send_raw(AT_CMD_GPRS_PDP_DEACTIVE, rsp, SIM800_DEFAULT_RSP_LEN);
    if (strstr(rsp, SIM800_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }

    /*set multi ip connection mode*/
    memset(rsp, 0, SIM800_DEFAULT_RSP_LEN);
    snprintf(cmd, SIM800_DEFAULT_CMD_LEN - 1, "%s=%d", AT_CMD_MULTI_IP_CONNECTION, 1);
    at.send_raw(cmd, rsp, SIM800_DEFAULT_RSP_LEN);
    if (strstr(rsp, SIM800_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }

#if 0
    /*set tcpip mode in normal mode*/
    memset(rsp, 0, SIM800_DEFAULT_RSP_LEN);
    memset(cmd, 0, SIM800_DEFAULT_CMD_LEN);
    snprintf(cmd, SIM800_DEFAULT_CMD_LEN - 1, "%s=%d", AT_CMD_TCPIP_MODE, 0);
    at.send_raw(cmd, rsp, SIM800_DEFAULT_RSP_LEN);
    if (strstr(rsp, SIM800_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "%s %d failed \r\n", __func__, __LINE__);
        return -1;
    }
#endif
    /*not prompt echo > when sending data*/
    memset(rsp, 0, SIM800_DEFAULT_RSP_LEN);
    memset(cmd, 0, SIM800_DEFAULT_CMD_LEN);
    snprintf(cmd, SIM800_DEFAULT_CMD_LEN - 1, "%s=%d", AT_CMD_SEND_DATA_PROMPT_SET, 0);
    at.send_raw(cmd, rsp, SIM800_DEFAULT_RSP_LEN);
    if (strstr(rsp, SIM800_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }

    /*Show Remote ip and port when receive data*/
    memset(rsp, 0, SIM800_DEFAULT_RSP_LEN);
    memset(cmd, 0, SIM800_DEFAULT_CMD_LEN);
    snprintf(cmd, SIM800_DEFAULT_CMD_LEN - 1, "%s=%d", AT_CMD_RECV_DATA_FORMAT_SET, 1);
    at.send_raw(cmd, rsp, SIM800_DEFAULT_RSP_LEN);
    if (strstr(rsp, SIM800_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }

    return 0;
}

static void sim800_get_ip_delayed_action(void *arg)
{
    printf("post got ip event \r\n");
    aos_post_event(EV_WIFI, CODE_WIFI_ON_GOT_IP, 0xdeaddead);
}


static int sim800_gprs_got_ip(void)
{
    char rsp[SIM800_DEFAULT_RSP_LEN] = {0};

    /*start gprs stask*/
    at.send_raw(AT_CMD_START_TASK, rsp, SIM800_DEFAULT_RSP_LEN);
    if (strstr(rsp, SIM800_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }

    /*bring up wireless connectiong with gprs*/
    memset(rsp, 0, SIM800_DEFAULT_RSP_LEN);
    at.send_raw(AT_CMD_BRING_UP_GPRS_CONNECT, rsp, SIM800_DEFAULT_RSP_LEN);
    if (strstr(rsp, SIM800_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        //return -1;
    }

    /*try to got ip*/
    memset(rsp, 0, SIM800_DEFAULT_RSP_LEN);
    at.send_raw_self_define_respone_formate(AT_CMD_GOT_LOCAL_IP, rsp, SIM800_DEFAULT_RSP_LEN,
                                        NULL, AT_RECV_PREFIX, NULL);
    if (strstr(rsp, SIM800_AT_CMD_FAIL_RSP) != NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        //return -1;
    }

    printf("sim800 got ip %s \r\n", rsp);

    /*delay 5 seconds to post got ip event*/
    aos_post_delayed_action(5000, sim800_get_ip_delayed_action, NULL);
    return 0;
}

static int sim800_gprs_module_init(void)
{
    int ret = 0;
    uint32_t linknum = 0;
    
    if (inited) {
        LOGI(TAG, "sim800 gprs module have already inited \r\n");
        return 0;
    }

    g_pcdomain_rsp = aos_malloc(SIM800_DOMAIN_RSP_MAX_LEN);
    if (NULL == g_pcdomain_rsp){
        LOGE(TAG, "%s %d failed \r\n", __func__, __LINE__);
        goto err;
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
    
    for(linknum = 0; linknum < SIM800_MAX_LINK_NUM; linknum++){
        g_link[linknum].fd = -1;
    }
    ret = sim800_uart_init();
    if (ret){
        LOGE(TAG, "%s %d failed \r\n", __func__, __LINE__);
        goto err;
    }

    ret = sim800_gprs_status_check();
    if (ret){
        LOGE(TAG, "%s %d failed \r\n", __func__, __LINE__);
        goto err;
    }

    ret = sim800_gprs_ip_init();
    if (ret){
        LOGE(TAG, "%s %d failed \r\n", __func__, __LINE__);
        goto err;
    }

    /* reg oob for domain and packet input*/
    at.oob(AT_CMD_DOMAIN_RSP, AT_RECV_PREFIX, SIM800_DOMAIN_RSP_MAX_LEN, 
            sim800_gprs_domain_rsp_callback, NULL);
    at.oob(AT_CMD_DATA_RECV, NULL, 0, sim800_gprs_module_socket_data_handle, NULL);
    ret = sim800_gprs_got_ip();
    if (ret){
        LOGE(TAG, "%s %d failed \r\n", __func__, __LINE__);
        goto err;
    }
    
    inited = 1;
    
    return 0;
err:
    if (g_pcdomain_rsp != NULL){
        aos_free(g_pcdomain_rsp);
        g_pcdomain_rsp = NULL;
    }

    if (aos_mutex_is_valid(&g_link_mutex)){
        aos_mutex_free(&g_link_mutex);
    }

    if (aos_mutex_is_valid(&g_domain_mutex)){
        aos_mutex_free(&g_domain_mutex);
    }

    if (aos_sem_is_valid(&g_domain_sem)){
        aos_sem_free(&g_domain_sem);
    }

    return -1;
}

static int sim800_gprs_module_deinit()
{
    if(!inited){
        return 0;
    }

    aos_mutex_free(&g_link_mutex);
    inited = 0;
    return 0;
}

static int sim800_gprs_module_domain_to_ip(char *domain, char ip[16])
{
    char *pccmd = NULL;
    char *head = NULL;
    char *end = NULL;
    char rsp[SIM800_DEFAULT_RSP_LEN] = {0};

    if (!inited){
        LOGE(TAG, "%s sim800 gprs module haven't init yet \r\n", __func__);
        return -1;
    }
    
    if (NULL == domain || NULL == ip){
        LOGE(TAG, "invalid input at %s \r\n", __func__);
        return -1;
    }

    if (strlen(domain) > SIM800_DOMAIN_MAX_LEN){
        LOGE(TAG, "domain length oversize at %s \r\n", __func__);
        return -1;
    }

    pccmd = aos_malloc(SIM800_DOMAIN_CMD_LEN);
    if (NULL == pccmd){
        LOGE(TAG, "fail to malloc memory %d at %s \r\n", SIM800_DOMAIN_CMD_LEN, __func__);
        return -1;
    }
    
    memset(pccmd, 0, SIM800_DOMAIN_CMD_LEN);
    snprintf(pccmd, SIM800_DEFAULT_CMD_LEN - 1, "%s=%s", AT_CMD_DOMAIN_TO_IP, domain);

    aos_mutex_lock(&g_domain_mutex, AOS_WAIT_FOREVER);
    
    at.send_raw(pccmd, rsp, SIM800_DEFAULT_RSP_LEN);
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
    if ((end = strstr(head, "\"")) == NULL){
        LOGE(TAG, "invalid domain rsp head is %s at %d\r\n", head, __LINE__);
        goto err;
    }

    if ((end - head) > 15 || (end - head) < 7){
        LOGE(TAG, "invalid domain rsp head is %s at %d\r\n", head, __LINE__);
        goto err;
    }

    /* We find a good IP, save it. */
    memcpy(ip, head, end - head);
    ip[end-head] = '\0';
    memset(g_pcdomain_rsp, 0, SIM800_DOMAIN_RSP_MAX_LEN);
    aos_mutex_unlock(&g_domain_mutex);

    printf("domain %s get ip %s \r\n", domain ,ip);
    return 0;
err:
    aos_free(pccmd);
    memset(g_pcdomain_rsp, 0, SIM800_DOMAIN_RSP_MAX_LEN);
    aos_mutex_unlock(&g_domain_mutex);

    return -1;
}

static int sim800_gprs_module_conn_start(sal_conn_t *conn)
{
    int  linkid = 0;
    char *pccmd = NULL;
    char rsp[SIM800_DEFAULT_RSP_LEN] = {0};
    
    if (!inited){
        LOGE(TAG, "%s sim800 gprs module haven't init yet \r\n", __func__);
        return -1;
    }

    if (!conn || !conn->addr){
        LOGE(TAG, "%s %d - invalid input \r\n", __func__, __LINE__);
        return -1;
    }
#if 0
    /*if input addr is a domain, then turn it into ip addr */
    if (sim800_gprs_module_domain_to_ip(conn->addr, ipaddr) != 0){
        if (strlen(conn->addr) >= sizeof(ipaddr)){
            LOGE(TAG, "%s invalid server addr %s \r\n", __func__, conn->addr);
            return -1;
        }
        strcpy(ipaddr, conn->addr);
    }
#endif
    aos_mutex_lock(&g_link_mutex, AOS_WAIT_FOREVER);
    for (linkid = 0; linkid < SIM800_MAX_LINK_NUM; linkid++){
        if (g_link[linkid].fd >= 0){
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

    pccmd = aos_malloc(SIM800_CONN_CMD_LEN);
    if (NULL == pccmd){
        LOGE(TAG, "fail to malloc %d at %s \r\n", SIM800_CONN_CMD_LEN, __func__);
        goto err;
    }
    memset(pccmd, 0, SIM800_CONN_CMD_LEN);
    
    switch(conn->type){
        case TCP_SERVER:
            snprintf(pccmd, SIM800_CONN_CMD_LEN - 1, "%s=%d,%d", AT_CMD_START_TCP_SERVER, 1, conn->l_port);
            at.send_raw(pccmd, rsp, SIM800_DEFAULT_RSP_LEN);
            if (strstr(rsp, SIM800_AT_CMD_SUCCESS_RSP) == NULL){
                LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
                goto err;
            }
        break;
        case TCP_CLIENT:
            snprintf(pccmd, SIM800_CONN_CMD_LEN - 1, "%s=%d,\"TCP\",\"%s\",%d", AT_CMD_START_CLIENT_CONN, linkid, conn->addr, conn->r_port);
            at.send_raw_self_define_respone_formate(pccmd, rsp, SIM800_DEFAULT_RSP_LEN,
                                                    NULL, AT_CMD_CLIENT_CONNECT_OK, AT_CMD_CLIENT_CONNECT_FAIL);
            if (strstr(rsp, AT_CMD_CLIENT_CONNECT_FAIL) != NULL){
                LOGE(TAG, "pccmd %s fail, rsp %s \r\n", pccmd, rsp);
                goto err;
            }
        break;
        case UDP_UNICAST:
            snprintf(pccmd, SIM800_CONN_CMD_LEN - 1, "%s=%d,\"UDP\",\"%s\",%d", AT_CMD_START_CLIENT_CONN, linkid, conn->addr, conn->r_port);
            at.send_raw_self_define_respone_formate(pccmd, rsp, SIM800_DEFAULT_RSP_LEN,
                                                    NULL, AT_CMD_CLIENT_CONNECT_OK, AT_CMD_CLIENT_CONNECT_FAIL);
            if (strstr(rsp, AT_CMD_CLIENT_CONNECT_FAIL) != NULL){
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

    aos_free(pccmd);
    return 0;
err:
    aos_free(pccmd);
    aos_mutex_lock(&g_link_mutex, AOS_WAIT_FOREVER);
    g_link[linkid].fd = -1;
    aos_mutex_unlock(&g_link_mutex);
    return -1;
}

static int sim800_gprs_module_conn_close(int fd, int32_t remote_port)
{
    int  linkid = 0;
    int  ret = 0;
    char cmd[SIM800_DEFAULT_CMD_LEN] = {0};
    char rsp[SIM800_DEFAULT_RSP_LEN] = {0};

    if (!inited){
        LOGE(TAG, "%s sim800 gprs module haven't init yet \r\n", __func__);
        return -1;
    }

    linkid = fd_to_linkid(fd);
    if (linkid >= SIM800_MAX_LINK_NUM) {
        LOGE(TAG, "No connection found for fd (%d) in %s \r\n", fd, __func__);
        return -1;
    } 

    snprintf(cmd, SIM800_DEFAULT_CMD_LEN -1, "%s=%d", AT_CMD_STOP_CONN, linkid);
    at.send_raw(cmd, rsp, SIM800_DEFAULT_RSP_LEN);
    if (strstr(rsp, SIM800_AT_CMD_SUCCESS_RSP) == NULL){
        LOGE(TAG, "cmd %s rsp is %s \r\n", cmd, rsp);
        ret = -1;
    }

    aos_mutex_lock(&g_link_mutex, AOS_WAIT_FOREVER);
    g_link[linkid].fd = -1;
    aos_mutex_unlock(&g_link_mutex);

    return ret;
}

static int sim800_gprs_module_send(int fd,uint8_t *data,uint32_t len,
                         char remote_ip[16], int32_t remote_port)
{
    int  linkid;
    char cmd[SIM800_DEFAULT_CMD_LEN] = {0};
    char rsp[SIM800_DEFAULT_RSP_LEN] = {0};

    if (!inited){
        LOGE(TAG, "%s sim800 gprs module haven't init yet \r\n", __func__);
        return -1;
    }

    linkid = fd_to_linkid(fd);
    if (linkid >= SIM800_MAX_LINK_NUM) {
        LOGE(TAG, "No connection found for fd (%d) in %s \r\n", fd, __func__);
        return -1;
    }

    snprintf(cmd, SIM800_DEFAULT_CMD_LEN - 1, "%s=%d,%d", AT_CMD_SEND_DATA, linkid, len);

    /*TODO data send fail rsp is SEND FAIL*/
    at.send_data_2stage((const char *)cmd, (const char *)data, len, rsp, sizeof(rsp));
    if (strstr(rsp, SIM800_AT_CMD_SUCCESS_RSP) == NULL) {
        LOGE(TAG, "cmd %s rsp %s at %s %d failed \r\n", cmd, rsp, __func__, __LINE__);
        return -1;
    }

    return 0;
}

static int sim800_gprs_packet_input_cb_register(netconn_data_input_cb_t cb)
{
    if (cb)
        g_netconn_data_input_cb = cb;
    return 0;
}


sal_op_t sim800_sal_opt = {
    .version = "1.0.0",
    .init = sim800_gprs_module_init,
    .start = sim800_gprs_module_conn_start,
    .send = sim800_gprs_module_send,
    .domain_to_ip = sim800_gprs_module_domain_to_ip,
    .close = sim800_gprs_module_conn_close,
    .deinit = sim800_gprs_module_deinit,
    .register_netconn_data_input_cb = sim800_gprs_packet_input_cb_register,
};


int sim800_sal_init(void)
{
    return sal_module_register(&sim800_sal_opt);
}
