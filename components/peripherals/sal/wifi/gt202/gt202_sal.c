#include <stdio.h>
#include <stddef.h>
#include <aos/kernel.h>

#include "qcom_api.h"
#include <hal_sal.h>

#define CONN_LIST_MAX               (10)
#define QCOM_SELECT_TIMEOUT         (10)

// Convert IP address in uint32_t to comma separated bytes
#define UINT32_IPADDR_TO_CSV_BYTES(a) (((a) >> 24) & 0xFF), (((a) >> 16) & 0xFF), (((a) >> 8) & 0xFF), ((a)&0xFF)
// Convert comma separated bytes to a uint32_t IP address
#define CSV_BYTES_TO_UINT32_IPADDR(a0, a1, a2, a3)  (((uint32_t)(a0)&0xFF) << 24) | (((uint32_t)(a1)&0xFF) << 16) | (((uint32_t)(a2)&0xFF) << 8) | ((uint32_t)(a3)&0xFF)

typedef struct {
   int fd;
   int qcom_socket;
}SocketFDMap;

static SocketFDMap sock_list[CONN_LIST_MAX];

static netconn_data_input_cb_t g_netconn_data_input_cb;

int HAL_SAL_Init(void)
{
    return 0;
}

static int remove_sock_form_list_by_fd(int fd)
{
    int i;
    for(i=0; i<CONN_LIST_MAX; i++)
    {
        if(sock_list[i].fd == fd)
        {
            sock_list[i].fd = -1;
            sock_list[i].qcom_socket = -1;
        }
    }
    return 0;
}

static int remove_sock_form_list_by_qsock(int socket)
{
    int i;
    for(i=0; i<CONN_LIST_MAX; i++)
    {
        if(sock_list[i].qcom_socket == socket)
        {
            sock_list[i].fd = -1;
            sock_list[i].qcom_socket = -1;
        }
    }
    return 0;
}
static int add_sock_to_list(int fd, int sock)
{
    int i;
    for(i=0; i<CONN_LIST_MAX; i++)
    {
        if(sock_list[i].qcom_socket < 0)
        {
            sock_list[i].fd = fd;
            sock_list[i].qcom_socket = sock;
            return 0;
        }
    }

    return 1;
}

static int fd_to_socket(int fd)
{
    int i;
    for(i=0; i<CONN_LIST_MAX; i++)
    {
        if(sock_list[i].fd == fd)
        {
            return sock_list[i].qcom_socket;
        }
    }

    return -1;
}

static int socket_to_fd(int socket)
{
    int i;
    for(i=0; i<CONN_LIST_MAX; i++)
    {
        if(sock_list[i].qcom_socket == socket)
        {
            return sock_list[i].fd;
        }
    }

    return -1;
}

static int HAL_SAL_DomainToIp(char *domain, char ip[16])
{
    uint32_t addr = 0;
    A_STATUS status;
    PRINTF("DNS Request %s\r\n", domain);
    if (strcmp(domain, "public.iot-as-mqtt.cn-shanghai.aliyuncs.com") == 0) {
        /* Workaround DNS */
        PRINTF("Work around DNS\r\n");
        strcpy(ip, "139.196.135.135");
        return 0;
    }
    if (strcmp(domain, "iotx-ota.oss-cn-shanghai.aliyuncs.com") == 0) {
        PRINTF("Work around DNS\r\n");
        strcpy(ip, "106.14.228.182");
        return 0;
    }

    // NOTE: This function returns the address in reverse byte order
    status = qcom_dnsc_get_host_by_name((char *)domain, &addr);

    PRINTF("DNS Resolve %d, status = %d\r\n", addr, status);
    if (status == 0)
    {
        int len = snprintf(ip, 16, "%d.%d.%d.%d", UINT32_IPADDR_TO_CSV_BYTES(addr));
        ip[len] = 0;
        PRINTF("Looked up %s as %d.%d.%d.%d\r\n", domain, UINT32_IPADDR_TO_CSV_BYTES(addr));
    }

    return status;
}

static int HAL_SAL_Start(sal_conn_t *c)
{
    int sock, ret;
    SOCKADDR_T l_addr, r_addr;
    memset(&l_addr, 0, sizeof(l_addr));
    memset(&r_addr, 0, sizeof(r_addr));

    A_STATUS status;

    if(!c || !c->addr)
    {
        PRINTF("sal_qcom_start paramter error\r\n");
        return 1;
    }

    PRINTF("sal_qcom_start %s\r\n", c->addr);

    // NOTE: This function returns the address in reverse byte order
    status = qcom_dnsc_get_host_by_name(c->addr, (uint32_t*)&r_addr.sin_addr.s_addr);
    if(status != A_OK)
    {
        /*domain resolve failed, try to use IP directly */

        int a0, a1, a2, a3;
        ret = sscanf(c->addr, "%d.%d.%d.%d", &a0, &a1, &a2, &a3);
        if(ret == 4)
        {
            r_addr.sin_addr.s_addr = CSV_BYTES_TO_UINT32_IPADDR(a0, a1, a2, a3);
        }
        else
        {
            return 1;
        }
    }

    switch(c->type)
    {
        case TCP_CLIENT:
        case TCP_SERVER:
            sock = qcom_socket(ATH_AF_INET, SOCK_STREAM_TYPE, 0);
            PRINTF("QCOM TCP SOCKET = %d\r\n", sock);
            break;
        case UDP_UNICAST:
        case UDP_BROADCAST:
            sock = qcom_socket(ATH_AF_INET, SOCK_DGRAM_TYPE, 0);
            PRINTF("QCOM UDP SOCKET = %d\r\n", sock);
            break;
        default:
            return 1;
            break;
    }

    if(sock < 0)
    {
        return 1;
    }

    if(c->type == TCP_CLIENT || c->type == SSL_CLIENT)
    {
        r_addr.sin_port = c->r_port;
        r_addr.sin_family = ATH_AF_INET;

        status = (A_STATUS)qcom_connect(sock, (struct sockaddr *)&r_addr, sizeof(r_addr));
        if(status != A_OK)
        {
            return 1;
        }
    }

    if(c->type == TCP_SERVER)
    {
        l_addr.sin_port = c->l_port;
        l_addr.sin_family = ATH_AF_INET;
        l_addr.sin_addr.s_addr = CSV_BYTES_TO_UINT32_IPADDR(0, 0, 0, 0);

        status = (A_STATUS)qcom_bind(sock, (struct sockaddr *)(&l_addr), sizeof(l_addr));
        if(status != A_OK)
        {
            qcom_socket_close(sock);
            return 1;
        }

        status = qcom_listen(sock, 1);
        if(status != A_OK)
        {
            qcom_socket_close(sock);
            return 1;
        }
    }

    if(c->type == UDP_UNICAST || c->type == UDP_BROADCAST)
    {
        l_addr.sin_port = c->l_port;
        l_addr.sin_family = ATH_AF_INET;
        l_addr.sin_addr.s_addr = CSV_BYTES_TO_UINT32_IPADDR(0, 0, 0, 0);

        status = (A_STATUS)qcom_bind(sock, (struct sockaddr *)(&l_addr), sizeof(l_addr));
        if(status != A_OK)
        {
            qcom_socket_close(sock);
            return 1;
        }
    }

    PRINTF("sal_qcom_start c->fd = %d, socket = %d\r\n", c->fd, sock);

    add_sock_to_list(c->fd, sock);

    return 0;
}

static int HAL_SAL_Send(int fd, uint8_t *data, uint32_t len, char remote_ip[16], int32_t remote_port, int32_t timeout)
{
    volatile int sent;
    int qcom_socket = 0;

    /* qcom driver enable ZERO_COPY, then must use custom_alloc */

    uint8_t *send_buf = custom_alloc(len);

    PRINTF("sal_qcom_send fd = %d, size = %d\r\n",fd, len);

    qcom_socket = fd_to_socket(fd);

    if(send_buf == NULL)
    {
        PRINTF("sal_qcom_send allocation failed\r\n");
        return 1;
    }

    memcpy(send_buf, data, len);

    /* if remote_ip, regard it as TCP send */
    if(remote_ip == NULL)
    {
        sent = qcom_send(qcom_socket, (char*)send_buf, len, 0);
        PRINTF("TCP Send return %d\r\n", sent);
    }
    else    /* UDP send */
    {
        int a0, a1, a2, a3;
        sscanf(remote_ip, "%d.%d.%d.%d", &a0, &a1, &a2, &a3);

        SOCKADDR_T r_addr;
        r_addr.sin_family = ATH_AF_INET;
        r_addr.sin_port = remote_port;
        r_addr.sin_addr.s_addr = CSV_BYTES_TO_UINT32_IPADDR(a0, a1, a2, a3);
        PRINTF("UDP Send, %d-%d-%d-%d:%d\r\n", a0,a1,a2,a3,remote_port);
        sent = qcom_sendto(qcom_socket, (char*)send_buf, len, 0, (struct sockaddr *)&r_addr, sizeof(SOCKADDR_T));
        PRINTF("UDP Send return %d\r\n", sent);
    }

    custom_free(send_buf);
    return 0;
}

static int HAL_SAL_Close(int fd, int32_t remote_port)
{
    int sock;

    sock = fd_to_socket(fd);
    if (sock == -1) {
        /* SOCKET close by remote */
        return 0;
    }
    PRINTF("sal_qcom_close fd = %d, socket =%d\r\n", fd, sock);

    qcom_socket_close(sock);
    remove_sock_form_list_by_fd(fd);

    return 0;
}

static int HAL_SAL_RegisterNetconnDataInputCb(netconn_data_input_cb_t cb)
{
    PRINTF("sal_qcom_register_netconn_data_input_cb\r\n");

    if (cb)
        g_netconn_data_input_cb = cb;

    return 0;
}

static int HAL_SAL_Deinit(void)
{
    return 0;
}


void sal_recv_task(void *param)
{
    int i, rev_len, timeout, sock, fd;
    char *recv_buf;

    SOCKADDR_T r_addr;
    socklen_t r_addr_len;
    PRINTF("sal_recv_task starting... \r\n");
    while(1)
    {
        for(i=0; i<CONN_LIST_MAX; i++)
        {
            if(sock_list[i].qcom_socket > 0)
            {
                sock = sock_list[i].qcom_socket;
                QCA_CONTEXT_STRUCT *enetCtx = wlan_get_context();
                switch(t_select(enetCtx, sock, QCOM_SELECT_TIMEOUT))
                {
                    case A_SOCK_INVALID:    /* socket err */
                    PRINTF("CLOSE BY REMOTE\r\n");
                    remove_sock_form_list_by_qsock(sock);
                    continue;
                    case A_OK:
                    recv_buf = NULL;
                    rev_len = qcom_recvfrom(sock, &recv_buf, 1400, 0, (struct sockaddr *)&r_addr, &r_addr_len);
                    if(rev_len >= 0)
                    {
                        fd = socket_to_fd(sock);
                        PRINTF("qcom_recvfrom get data %d, fd = %d, sock = %d\r\n", rev_len, fd, sock);
                        g_netconn_data_input_cb(fd, recv_buf, rev_len, NULL, 0);
                        zero_copy_free(recv_buf);
                        continue;
                    }
                    break;
                    case A_ERROR:   /* timeout */
                        continue;
                    default:
                        break;
                }
            }
        }
        A_MDELAY(1);
    }

}

static ktask_t *g_sal_task = NULL;
#define WIFI_SAL_STACK_SIZE 256
int gt202_sal_add_dev(void* data)
{
    kstat_t status;
    int i = 0;

    PRINTF("gt202_sal_init\r\n");

    while (i < CONN_LIST_MAX) {
        sock_list[i].fd = -1;
        sock_list[i].qcom_socket = -1;
        i++;
    }

    status = krhino_task_dyn_create(&g_sal_task, "WIFI SAL", 0, (AOS_DEFAULT_APP_PRI - 9), 0,
                                    WIFI_SAL_STACK_SIZE, (task_entry_t)sal_recv_task, 1);
    if(status != RHINO_SUCCESS)
    {
        PRINTF("WIFI SAL Task creation failed %d\r\n", status);
        return 1;
    }

    return 0;
}

static sal_op_t sal_op = {
    .next = NULL,
    .version = "1.0.0",
    .name = "gt202",
    .add_dev = gt202_sal_add_dev,
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
