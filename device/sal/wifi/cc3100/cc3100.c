/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <aos/aos.h>
#include <hal/hal.h>
#include <sal.h>
#include "socket.h"
//#include "sal_ipaddr.h"
//#include "sal_sockets_internal.h"
#include <aos/network.h>
#define TAG "sal_wifi"

#define CMD_SUCCESS_RSP "OK"
#define CMD_FAIL_RSP "ERROR"

#define MAX_DATA_LEN   4096
#define MAX_DOMAIN_LEN 256
#define DATA_LEN_MAX 10
#define LINK_ID_MAX 5

#define STOP_CMD "AT+CIPSTOP"
#define STOP_CMD_LEN (sizeof(STOP_CMD)+1+1+5+1)

#define STOP_AUTOCONN_CMD "AT+CIPAUTOCONN"
#define STOP_AUTOCONN_CMD_LEN (sizeof(STOP_AUTOCONN_CMD)+1+1+5+1)

#define FWVER_CMD "AT+FWVER?\r"
#define FWVER_CMD_LEN (sizeof(FWVER_CMD) - 1)

#define SERVALPALWIFI_TCP_RX_BUFFER             UINT32_C(1000)  /* Maximum size of the buffer for receive */

#define PALIP_IPV4_BYTE(val,index)                  ( (val >> (index*8)) & 0xFF )

#define AT_RESET_CMD "AT"
typedef uint32_t Ip_Address_T; /**< IP address required for communicating to the server */
typedef int (*at_data_check_cb_t)(char data);

/* Change to include data slink for each link id respectively. <TODO> */
typedef struct link_s {
    int fd;
    int sl_fd;
} link_t;

static uint8_t inited = 0;

static link_t g_link[LINK_ID_MAX];
static SlSockAddrIn_t g_sockaddr[LINK_ID_MAX];

static aos_mutex_t g_link_mutex;
static netconn_data_input_cb_t g_netconn_data_input_cb;
static char localipaddr[16];
aos_task_t selecttask;
//#define SAL_TASK_STACK_SIZE 4096 // 4kByte
#define SAL_TASK_STACK_SIZE 8192

#define SAL_TASK_PRIO \
    (AOS_DEFAULT_APP_PRI - 2) // higher prio than normal app's

/**********************************************************************/
// Caller to ensure a valid ip string
static int ipstr_to_u32(char *ipstr, uint32_t *ip32)
{
    uint8_t *q = (uint8_t *)ip32, n = 0, stridx = 0, dotnum = 0;
    char *p = ipstr;

    if (!ipstr || !ip32) {
        return -1;
    }

    for (n = 0, stridx = 0, dotnum = 0;
         *p != '\0' && stridx < 15 && dotnum < 4;
         stridx++, p++) {
        if (*p == '.') {
            q[dotnum] = n; // saved in network order
            n = 0;
            dotnum++;
            continue;
        }
        if (*p < '0' || *p > '9') {
            return -1;
        }
        n = n * 10 + *p - '0';
    }

    if (dotnum >= 4 || stridx > 15) {
        return -1;
    } else {
        q[dotnum] = n;    // the last number
    }

    return 0;
}

int int2str(uint8_t n, char *str)
{
    char buf[10] = "";
    int i = 0;
    int len = 0;
    int temp = n;
    int ipstrlen = 3;
    if (str == NULL)
    {
        return -1;
    }
    if(n<10) ipstrlen=1;
    else if (10 <= n < 100) ipstrlen =2;
    else ipstrlen =3;
    for(int m=0; m<ipstrlen; m++)
    {
        buf[m] = (temp % 10) + '0'; 
        temp = temp / 10;
    }
    str[ipstrlen] = 0;            //Ä©Î²ÊÇ½áÊø·û0
    for(i = 0; i<ipstrlen; i++)
    {
        str[i] = buf[ipstrlen-i-1];
    }
    return ipstrlen;

}

static int u32_to_ipstr(uint32_t *ip32, char *ipstr)
{
    uint8_t *q = (uint8_t *)ip32, n = 0, stridx = 0, dotnum = 0;
    char *p = ipstr;
    int ipstrlen = 0;
    if (!ipstr || !ip32) {
        return -1;
    }

    for(n = 0;n<4;n++)
    {
        ipstrlen = int2str(q[n],p);
        if(ipstrlen<1 || ipstrlen>3)  return -1;
        p = p+ipstrlen;
        if(n < 3) 
        {
            *p = '.';
            p++;
        }
        else if(n == 3)
        {
            *p = '\0';
        }
        else return -1;
    }
        
    return 0;
}

int Ip_convertAddrToString(Ip_Address_T const *ipAddr_ptr, char *buffer)
{
    int len = 0;
    if ((NULL != ipAddr_ptr) && (NULL != buffer))
    {
        len = sprintf(buffer, "%d.%d.%d.%d", (int) PALIP_IPV4_BYTE(*ipAddr_ptr, 0), (int) PALIP_IPV4_BYTE(*ipAddr_ptr, 1),
                (int) PALIP_IPV4_BYTE(*ipAddr_ptr, 2), (int) PALIP_IPV4_BYTE(*ipAddr_ptr, 3));
    }

    return len;
}

static int fd_to_linkid(int fd)
{
    int link_id;
    
    if (aos_mutex_lock(&g_link_mutex, AOS_WAIT_FOREVER) != 0) {
        LOGE(TAG, "Failed to lock mutex (%s).", __func__);
        return -1;
    }
    for (link_id = 0; link_id < LINK_ID_MAX; link_id++) {
        if (g_link[link_id].fd == fd) 
            break;
    }

    aos_mutex_unlock(&g_link_mutex);

    return link_id;
}

static int sal_wifi_close(int fd,
                          int32_t remote_port)
{
    int link_id;
    int ret;
    char cmd[STOP_CMD_LEN] = {0}, out[64];

    link_id = fd_to_linkid(fd);
    if (link_id >= LINK_ID_MAX) {
        LOGE(TAG, "No connection found for fd (%d) in %s", fd, __func__);
        return -1;
    }
    ret = sl_Close(g_link[link_id].sl_fd);
    if(ret != 0 ) 
    {
        LOGE(TAG, "sl_Close fail fd (%d)  sl_fd(%d) in %s", fd,g_link[link_id].sl_fd, __func__);
    }
    else LOGD(TAG, "%s sem_wait succeed.", __func__);
    
    if (aos_mutex_lock(&g_link_mutex, AOS_WAIT_FOREVER) != 0) {
        LOGE(TAG, "Failed to lock mutex (%s).", __func__);
        return -1;
    }
    g_link[link_id].fd = -1;
    g_link[link_id].sl_fd = -1;
    memset(&g_sockaddr[link_id],0,sizeof(SlSockAddrIn_t));
    aos_mutex_unlock(&g_link_mutex);
    return 0;

}
static int sal_wifi_domain_to_ip(char *domain,
                                 char ip[16])
{
    
    if(domain == NULL) return -1;
    _u32  out_ip_addr;
    _u32 tmp;
    _u32  out_ip_addr_e=0;
    //char outstr[256] = {0};
    char* outstr = NULL;
    sl_NetAppDnsGetHostByName(domain,strlen(domain),&out_ip_addr,SL_AF_INET);
/*
    tmp = out_ip_addr&0xFF;
    out_ip_addr_e +=(tmp<<24);
    tmp = (out_ip_addr>>8)&0xFF;
    out_ip_addr_e +=(tmp<<16);
    tmp = (out_ip_addr>>16)&0xFF;
    out_ip_addr_e +=(tmp<<8);
    tmp = (out_ip_addr>>24)&0xFF;
    out_ip_addr_e +=tmp;
    */
    ip4_addr_t addr_t;
    addr_t.addr = sl_Htonl(out_ip_addr);
    
    outstr = ip4addr_ntoa((ip4_addr_t*)&addr_t);
    if(outstr == NULL)
    {
        goto err;
    }
    memcpy(ip, outstr, (strlen(outstr)+1));

    /**
     * +CIPDOMAIN:1\r\n
     * 180.97.33.108\r\n
     *
     * OK\r\n
     */
     
    printf("sal_wifi_domain_to_ip get domain %s ip %s \r\n", domain, ip);
    LOGD(TAG, "get domain %s ip %s \r\n", domain, ip);
    return 0;

err:
    LOGD(TAG, "Failed to get IP due to unexpected result string %s \r\n.");
    return -1;
}

bool Sal_IsDataReceived(int* link_id)
{
    int handler;
    SlTimeval_t sal_selectTimeout;
    int iStatus;
    bool isDataRecv;
    int max;
    SlFdSet_t readFds;
    SL_FD_ZERO(&readFds);
    isDataRecv = false;    
    max = 0;
    handler = -1;
    for (uint8_t cnt = 0; cnt < LINK_ID_MAX; cnt++)
    {
        if (g_link[cnt].fd >= 0 && g_link[cnt].sl_fd >= 0)
        {
            handler = g_link[cnt].sl_fd > handler ? g_link[cnt].fd : handler;
            SL_FD_SET(g_link[cnt].sl_fd, &readFds);
        }
    }
    if(handler >= 0)
    {
        max = handler + 1;
        sal_selectTimeout.tv_sec = UINT32_C(0); /* Represent Sl_Select timeout, s */
        sal_selectTimeout.tv_usec = UINT32_C(5000); /* Represent Sl_Select timeout, us */
        iStatus = sl_Select(max, &readFds, NULL, NULL, &sal_selectTimeout);
        if (iStatus > 0)
        {
            uint8_t cnt = 0;
            
            for (cnt = 0; cnt < LINK_ID_MAX; cnt++)
            {
                if (g_link[cnt].sl_fd >= 0)
                {
                    if (TRUE == SL_FD_ISSET(g_link[cnt].sl_fd, &readFds))
                    {
                        isDataRecv = true;
                        *link_id = cnt;
                        break;
                    }
                }
            }
                
        }
    }
    else
    {
        aos_msleep(100);
    }
    return isDataRecv;
}

static void SalWiFiSocketMonitor()
{
    
    int iStatus;
    bool isDataRecv;
    _i8 Buf[500];
    _i8 recvbuf[5000];
    int recvlen = 0;
    int recvlentmp = 0;
    int link_id;
    for (;;)
    {
        if(Sal_IsDataReceived(&link_id))
        {
            memset(recvbuf,0,sizeof(recvbuf));
            recvlen = 0;
            recvlentmp = 0;
            do{
                recvlentmp = sl_Recv(g_link[link_id].sl_fd,Buf,500,0);
                if(recvlentmp == 0) 
                {
                    sal_wifi_close(g_link[link_id].fd,0);
                    return;
                }
                memcpy(&recvbuf[recvlen],Buf,recvlentmp);
                recvlen += recvlentmp;
            }while(recvlentmp==500);
            if (recvlen > 0 && g_netconn_data_input_cb && (g_link[link_id].fd >= 0)) {
                if (g_netconn_data_input_cb(g_link[link_id].fd, recvbuf, recvlen, NULL, 0)) {
                    LOGE(TAG, " %s socket %d get data len %d fail to post to sal, drop it\n",
                        __func__, g_link[link_id].fd, recvlen);
                }
            }
        }
    }
        
        
        
}

static int sal_wifi_init(void)
{
    int link;

    if (inited) {
        LOGW(TAG, "sal component is already initialized");
        return 0;
    }

    if (0 != aos_mutex_new(&g_link_mutex)) {
        LOGE(TAG, "Creating link mutex failed (%s %d).", __func__, __LINE__);
        return -1;
    }

    memset(g_link, 0, sizeof(g_link));
    memset(g_sockaddr, 0, sizeof(g_sockaddr));
    for (link = 0; link < LINK_ID_MAX; link++) {
        g_link[link].fd = -1;
        g_link[link].sl_fd = -1;
    }
    
    int ret = aos_task_new_ext(&selecttask, "cc3100_select_task", SalWiFiSocketMonitor,
                       NULL, SAL_TASK_STACK_SIZE, SAL_TASK_PRIO);
    if (unlikely(ret)) {
        return -1;
    }

    inited = 1;

    return 0;
}

static int sal_wifi_deinit(void)
{
    if (!inited) {
        return 0;
    }

    
    int retcode;
    retcode = WlanConnect_DeInit();

    aos_mutex_free(&g_link_mutex);

    return 0;
}

int g_port=2345;
int sal_wifi_start(sal_conn_t *c)
{
    int link_id;
    char out[256] = {0};
    int type,protocol;
    int ret;
    
    if (!c || !c->addr) {
        LOGE(TAG, "%s %d - invalid argument", __func__, __LINE__);
        return -1;
    }

    if (aos_mutex_lock(&g_link_mutex, AOS_WAIT_FOREVER) != 0) {
        LOGE(TAG, "Failed to lock mutex (%s).", __func__);
        return -1;
    }

    for (link_id = 0; link_id < LINK_ID_MAX; link_id++) {
        if (g_link[link_id].fd >= 0) {
            continue;
        } else {
            g_link[link_id].fd = c->fd;
            break;
        }
    }

    aos_mutex_unlock(&g_link_mutex);


    if(c->type == TCP_SERVER ||c->type == TCP_CLIENT ||c->type == SSL_CLIENT)
    {
        type = SL_SOCK_STREAM;
        protocol = SL_IPPROTO_TCP;
    }
    else if(c->type == UDP_BROADCAST ||c->type == UDP_UNICAST)
    {
        type = SL_SOCK_DGRAM;
        protocol = SL_IPPROTO_UDP;
    }

    
    SlSockAddrIn_t Addr;
    printf("sal_wifi_start c->addr=%s,lport=%d,rport=%d\n",c->addr,c->l_port,c->r_port);
    Addr.sin_addr.s_addr = inet_addr(c->addr);
    //Addr.sin_addr.s_addr = SL_IPV4_VAL(139,196,135,162);
    //Addr.sin_addr.s_addr = inet_addr("162.135.196.139");
    Addr.sin_family = SL_AF_INET;
    Addr.sin_port = sl_Htons(c->r_port);

    ret = sl_Socket(SL_AF_INET,type,protocol);
    printf("sl_Socket ret:%d,protocol=%d,type=%d\n",ret,protocol,type);
    if(ret < 0)
    {
        LOGI(TAG, "sl_Socket2 err, %d.", ret);
        goto err;
    }
    else 
    {
        g_link[link_id].sl_fd = ret;
        memcpy(&g_sockaddr[link_id],&Addr,sizeof(SlSockAddrIn_t));
    }
    #if 0
    uint16_t newport;
    if(c->l_port != -1)
        newport = c->l_port;
    else
        newport = (g_port++);
    
    newport = sl_Htons(newport);
    
    SlSockAddrIn_t localAddr_ldt;
    localAddr_ldt.sin_family = SL_AF_INET;
    localAddr_ldt.sin_port = newport;
    localAddr_ldt.sin_addr.s_addr = SL_INADDR_ANY;
    ret= sl_Bind(g_link[link_id].sl_fd, (SlSockAddr_t *) &localAddr_ldt, sizeof(localAddr_ldt));
    printf("yftest sl_Bind ret=%d\n",ret);
    #endif
    printf("g_sockaddr :%x,port:%x,link_fd=%d\n",g_sockaddr[link_id].sin_addr.s_addr,g_sockaddr[link_id].sin_port,link_id);
    printf("sl_Connect addr:%x,oaddr=%x,str=%s,port=%x,c->r_port=%x\n",Addr.sin_addr.s_addr,inet_addr(c->addr),c->addr,Addr.sin_port,c->r_port);
    ret = sl_Connect(g_link[link_id].sl_fd,(const SlSockAddr_t *) &Addr,sizeof(Addr));
    printf("sl_Connect return ret=%d\n",ret);
    if(SL_SOC_OK != ret)
    {
        LOGI(TAG, "sl_Connect err:%d, fd:%d.",ret, g_link[link_id].sl_fd);
        goto err;
    }
    // The caller should deal with this failure
    if (link_id >= LINK_ID_MAX) {
        LOGI(TAG, "No link available for now, %s failed.", __func__);
        return -1;
    }

    LOGD(TAG, "Creating %s connection ...", start_cmd_type_str[c->type]);

    LOGD(TAG, "%s sem_wait succeed.", __func__);

    return 0;
err:
    if (aos_mutex_lock(&g_link_mutex, AOS_WAIT_FOREVER) != 0) {
        LOGE(TAG, "Failed to lock mutex (%s).", __func__);
        return -1;
    }
    g_link[link_id].fd = -1;
    g_link[link_id].sl_fd =-1;
    aos_mutex_unlock(&g_link_mutex);
    return -1;
}

static int sal_wifi_send(int fd,
                         uint8_t *data,
                         uint32_t len,
                         char remote_ip[16],
                         int32_t remote_port,
                         int32_t timeout)
{
    int link_id;

    if (!data) {
        return -1;
    }
    _i16 Status;

    LOGD(TAG, "%s on fd %d", __func__, fd);

    link_id = fd_to_linkid(fd);
    if (link_id >= LINK_ID_MAX) {
        LOGE(TAG, "No connection found for fd (%d) in %s", fd, __func__);
        return -1;
    }
    SlSockAddrIn_t Addr;
    Addr.sin_addr.s_addr = inet_addr(remote_ip);
    Addr.sin_family = SL_AF_INET;
    Addr.sin_port = remote_port;
    Status = sl_Send(g_link[link_id].sl_fd,data,len,0);
    if(Status < 0) 
    {
        printf("sl_Send error Status=%d\n",Status);
        return -1;
    }
    return 0;
}

static int cc3100_wifi_packet_input_cb_register(netconn_data_input_cb_t cb)
{
    if (cb) {
        g_netconn_data_input_cb = cb;
    }
    return 0;
}


sal_op_t sal_op = {
    .version = "1.0.0",
    .init = sal_wifi_init,
    .start = sal_wifi_start,
    .send = sal_wifi_send,
    .domain_to_ip = sal_wifi_domain_to_ip,
    .close = sal_wifi_close,
    .deinit = sal_wifi_deinit,
    .register_netconn_data_input_cb = cc3100_wifi_packet_input_cb_register,
};

int cc3100_sal_init(void)
{
    printf("cc3100_sal_init\n");
    return sal_module_register(&sal_op);
}

