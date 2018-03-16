/*
 * Copyright (C) 
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <aos/aos.h>
#include <sal_arch.h>
#include <sal_ipaddr.h>
#include <sal.h>

#include <ti/drivers/net/wifi/simplelink.h>

#define TAG "sal_wifi"

#define MAX_DOMAIN_LEN 256
#define DATA_LEN_MAX 10
#define LINK_ID_MAX 5
#define RECV_BUF_LEN 1500

/* Change to include data slink for each link id respectively. <TODO> */
typedef struct link_s {
    int fd;
    int sal_fd;
    int sock_type;
    SlSockAddrIn_t      s_addr;
    int32_t         addr_size;
} link_t;

static link_t g_link[LINK_ID_MAX];
static aos_mutex_t g_link_mutex;
static netconn_data_input_cb_t g_netconn_data_input_cb;
SlFdSet_t rset;

static void app_sock_pkt_handle(void *arg)
{
    struct SlTimeval_t tv;
    int cnt;
    int link;
    char *recvdata = NULL;
    int mfd = -1;
        
        while(1){
                SL_SOCKET_FD_ZERO(&rset);
                          
                for (link = 0; link < LINK_ID_MAX; link++) {
                    if (g_link[link].fd > mfd) mfd = g_link[link].fd;
                    if (g_link[link].fd >= 0){
                      SL_SOCKET_FD_SET(g_link[link].fd, &rset);
                    }
                }          
                
                if(mfd < 0) {
                  aos_msleep(10);
                  continue;
                }
                
		tv.tv_sec = 0;
		tv.tv_usec = 20*1000;
		cnt = sl_Select(mfd+1, &rset, NULL, NULL, &tv);
		if (cnt < 0)
		{
			LOGE(TAG, "select failed<%d>\r\n", cnt);
			break;
		}
		else if(cnt == 0)
		{
			continue;
		}
                
                for (link = 0; link < LINK_ID_MAX; link++) {
                    int ret, recv_len = RECV_BUF_LEN;
                
                    if (SL_SOCKET_FD_ISSET(g_link[link].fd, &rset)) {
                        ret = 0;
                        /* Prepare socket data */
                        recvdata = (char *)aos_malloc(RECV_BUF_LEN + 1);
                        if (!recvdata) {
                            LOGE(TAG, "Error: %s %d out of memory.", __func__, __LINE__);
                            assert(0);
                        }
                        
                        if(g_link[link].sock_type == SL_SOCK_STREAM) {
                            ret = sl_Recv(g_link[link].fd, recvdata, recv_len, 0);
                        }
                        if(g_link[link].sock_type == SL_SOCK_DGRAM) {
                            ret = sl_RecvFrom(g_link[link].fd, recvdata, recv_len, 0, (SlSockAddr_t *)&(g_link[link].s_addr), (SlSocklen_t*)&(g_link[link].addr_size));
                        }
                        
                        if(ret <= 0) {
                            LOGE(TAG, "recv failed<%d> and the connection might be dropped\r\n", ret);
                            g_link[link].fd = -1;
                            sl_Close(g_link[link].fd);
                        }
                        else if (g_netconn_data_input_cb && (g_link[link].fd >= 0)){
                            /* TODO get recv data src ip and port*/
                            if (g_netconn_data_input_cb(link, recvdata, ret, NULL, 0)){
                                LOGE(TAG, " %s socket %d get data len %d fail to post to sal, drop it\n",
                                     __func__, g_link[link].fd, ret);
                            }
                        }
                        
                        aos_free(recvdata);
                    }
                }
    }
}

static uint8_t inited = 0;

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
    for (link = 0; link < LINK_ID_MAX; link++) {
        g_link[link].fd = -1;
        g_link[link].sock_type = 0;
        g_link[link].addr_size = 0;
    }
    
    SL_SOCKET_FD_ZERO(&rset);
    aos_task_new("SLSOCK", app_sock_pkt_handle, 0, 2048);

    return 0;
}

static int sal_wifi_deinit(void)
{
    if (!inited) return 0;

    aos_mutex_free(&g_link_mutex);

    return 0;
}

static int add_fd_to_link_tail(int fd, int sal_fd, int type)
{
    int link_id;
    
    if (aos_mutex_lock(&g_link_mutex, AOS_WAIT_FOREVER) != 0) {
        LOGE(TAG, "Failed to lock mutex (%s).", __func__);
        return -1;
    }

    for (link_id = 0; link_id < LINK_ID_MAX; link_id++) {
        if (g_link[link_id].fd >= 0) continue;
        else {
            g_link[link_id].fd = fd; 
            g_link[link_id].sal_fd = sal_fd;
            g_link[link_id].sock_type = type;
            break;
        }
    }

    aos_mutex_unlock(&g_link_mutex);

    // The caller should deal with this failure
    if (link_id >= LINK_ID_MAX) {
        LOGI(TAG, "No link available for now, %s failed.", __func__);
        return -1;
    }
    
    return link_id;
}

static uint32_t ip_string_to_in(char *addr)
{
    char *s1, *s2, *s3, *s4;
    uint32_t ip = 0;
    
    s1 = strtok(addr,".,"); /* first pass */
    s2 = strtok(NULL,".,");
    s3 = strtok(NULL,".,");
    s4 = strtok(NULL,".,");
        
    ip = SL_IPV4_VAL((const char *)atoi(s1), (const char *)atoi(s2),
			(const char *)atoi(s3), (const char *)atoi(s4));    
    
    return ip;
}

int sal_wifi_start(sal_conn_t *c)
{
    int link_id, sock_fd;
    int fd_type = 0;
    uint32_t ip_addr;
    int ret;
    int enableOption = 1;

    if (!c || !c->addr) {
        LOGE(TAG, "%s %d - invalid argument", __func__, __LINE__);
        return -1;
    }

    LOGD(TAG, "Creating %d connection and port %d...", c->type, c->r_port);

    switch (c->type) {
    case TCP_SERVER:
    case TCP_CLIENT:
    case SSL_CLIENT:
        fd_type = SL_SOCK_STREAM;           
        break;
    case UDP_BROADCAST:
    case UDP_UNICAST:
        fd_type = SL_SOCK_DGRAM;   
        break;
    default:
        LOGE(TAG, "Invalid connection type.");
        goto err;
    }
    
    sock_fd = sl_Socket(SL_AF_INET,fd_type,0);
    
    link_id = add_fd_to_link_tail(sock_fd, c->fd, fd_type);
    if(link_id < 0)
        goto err;
    
    ip_addr = ip_string_to_in(c->addr);
    
    g_link[link_id].s_addr.sin_family = SL_AF_INET;
    g_link[link_id].s_addr.sin_addr.s_addr = sl_Htonl(ip_addr);
    g_link[link_id].addr_size = sizeof(SlSockAddrIn_t);
    
    if(c->type == TCP_SERVER) {
        g_link[link_id].s_addr.sin_port = sl_Htons(c->l_port);
        if((ret = sl_Bind(sock_fd, (const SlSockAddr_t *)&(g_link[link_id].s_addr), g_link[link_id].addr_size)) < 0) {
            sl_Close(sock_fd);
            return -1;
        }
        if((ret = sl_Listen(sock_fd, 1)) < 0) {
            sl_Close(sock_fd);
            return -1;
        }
        
    }else {
        g_link[link_id].s_addr.sin_port = sl_Htons(c->r_port);
        if ((ret = sl_Connect(sock_fd, ( SlSockAddr_t *)&(g_link[link_id].s_addr), g_link[link_id].addr_size)) != 0) {
                LOGE(TAG, "Connect failed: %d", ret);
                sl_Close(sock_fd);
                return -1;
        }       
        
        sl_SetSockOpt(sock_fd,SL_SOL_SOCKET,SL_SO_NONBLOCKING, (_u8 *)&enableOption,sizeof(enableOption));
    }
    
    return 0;
err:
    if (aos_mutex_lock(&g_link_mutex, AOS_WAIT_FOREVER) != 0) {
        LOGE(TAG, "Failed to lock mutex (%s).", __func__);
        return -1;
    }
    g_link[link_id].fd = -1;
    aos_mutex_unlock(&g_link_mutex);
    return -1;
}

static int fd_to_linkid(int fd)
{
    int link_id;

    for (link_id = 0; link_id < LINK_ID_MAX; link_id++) {
        if (g_link[link_id].sal_fd == fd) break;
    }

    return link_id;
}

static int sal_wifi_send(int fd,
                         uint8_t *data,
                         uint32_t len,
                         char remote_ip[16],
                         int32_t remote_port)
{
    int link_id;
    int ret;

    if (!data) return -1;

    LOGD(TAG, "\r\n%s on fd %d and sock id is: %d\r\n", __func__, fd, g_link[link_id].fd);

    link_id = fd_to_linkid(fd);
    if (link_id >= LINK_ID_MAX) {
        LOGE(TAG, "No connection found for fd (%d) in %s", fd, __func__);
        return -1;
    }
   
    if (aos_mutex_lock(&g_link_mutex, AOS_WAIT_FOREVER) != 0) {
        LOGE(TAG, "Failed to lock mutex (%s).", __func__);
        return -1;
    }
    
    aos_mutex_unlock(&g_link_mutex);
    
    if(g_link[link_id].sock_type == SL_SOCK_STREAM) {
        ret = sl_Send(g_link[link_id].fd, data, len, 0);
    }
    if(g_link[link_id].sock_type == SL_SOCK_DGRAM){
        ret = sl_SendTo(g_link[link_id].fd, data, len, 0, (SlSockAddr_t *)&(g_link[link_id].s_addr), g_link[link_id].addr_size);
    }    
    
    if(ret != len)
      return -1;
    
    return 0;
}

/* Return the first IP if multiple found. */
static int sal_wifi_domain_to_ip(char *domain,
                                 char ip[16])
{   
    unsigned long destination_ip;
    short ret_status;

    ret_status = sl_NetAppDnsGetHostByName((signed char *)domain, strlen(domain), &destination_ip,SL_AF_INET);
    
    if(ret_status < 0)
        goto err;
    
    sprintf(ip, "%d.%d.%d.%d", SL_IPV4_BYTE(destination_ip,3),
                                SL_IPV4_BYTE(destination_ip,2),
                                SL_IPV4_BYTE(destination_ip,1),
                                SL_IPV4_BYTE(destination_ip,0));
    
    LOGD(TAG, "IP address of %s is %s", domain, ip);

    return 0;

err:
    LOGD(TAG, "Failed to get IP due to unexpected result string.");
    return -1;
}

/* <TODO> close should clean the socket data buffer realted to the link id. */
static int sal_wifi_close(int fd,
                          int32_t remote_port)
{
    int link_id;
    int real_sock;
    
    link_id = fd_to_linkid(fd);

    if (link_id >= LINK_ID_MAX) {
        LOGE(TAG, "No connection found for fd (%d) in %s", fd, __func__);
        return -1;
    }
    
    if (aos_mutex_lock(&g_link_mutex, AOS_WAIT_FOREVER) != 0) {
        LOGE(TAG, "Failed to lock mutex (%s).", __func__);
        return -1;
    }
    
    real_sock = g_link[link_id].fd;
    
    g_link[link_id].fd = -1;
    g_link[link_id].sock_type = 0;
    
    aos_mutex_unlock(&g_link_mutex);
    
    sl_Close(real_sock);

    return 0;
}

static int cc3220_wifi_packet_input_cb_register(netconn_data_input_cb_t cb)
{
    if (cb)
        g_netconn_data_input_cb = cb;
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
    .register_netconn_data_input_cb = cc3220_wifi_packet_input_cb_register,        
};

int cc3220_sal_init(void)
{
    return sal_module_register(&sal_op);
}


