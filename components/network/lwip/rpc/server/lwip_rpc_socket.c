/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <stddef.h>
#include "k_api.h"
#include "aos/kernel.h"
#include "lwip/debug.h"
#include "lwip_rpc.h"

extern int lwip_accept(int s, struct sockaddr *addr, socklen_t *addrlen);
extern int lwip_bind(int s, const struct sockaddr *name, socklen_t namelen);
extern int lwip_shutdown(int s, int how);
extern int lwip_getpeername(int s, struct sockaddr *name, socklen_t *namelen);
extern int lwip_getsockname(int s, struct sockaddr *name, socklen_t *namelen);
extern int lwip_getsockopt(int s, int level, int optname, void *optval, socklen_t *optlen);
extern int lwip_setsockopt(int s, int level, int optname, const void *optval, socklen_t optlen);
extern int lwip_close(int s);
extern int lwip_connect(int s, const struct sockaddr *name, socklen_t namelen);
extern int lwip_listen(int s, int backlog);
extern int lwip_recv(int s, void *mem, size_t len, int flags);
extern int lwip_read(int s, void *mem, size_t len);
extern int lwip_recvfrom(int s, void *mem, size_t len, int flags,
                         struct sockaddr *from, socklen_t *fromlen);
extern int lwip_send(int s, const void *dataptr, size_t size, int flags);
extern int lwip_sendmsg(int s, const struct msghdr *message, int flags);
extern int lwip_sendto(int s, const void *dataptr, size_t size, int flags,
                       const struct sockaddr *to, socklen_t tolen);
extern int lwip_socket(int domain, int type, int protocol);
extern int lwip_write(int s, const void *dataptr, size_t size);
extern int lwip_writev(int s, const struct iovec *iov, int iovcnt);
extern int lwip_select(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset,
                       struct timeval *timeout);
extern int lwip_ioctl(int s, long cmd, void *argp);
extern int lwip_fcntl(int s, int cmd, int val);
extern int lwip_eventfd(unsigned int initval, int flags);

extern struct hostent *lwip_gethostbyname(const char *name);
extern int lwip_getaddrinfo(const char *__restrict, const char *__restrict, const struct addrinfo *__restrict, struct addrinfo **__restrict);
extern void lwip_freeaddrinfo(struct addrinfo *);
extern int lwip_try_wakeup(int s, int rcvevent, int sendevent, int errevent);
extern int lwip_gethostbyname_r(const char *name, struct hostent *ret, char *buf,
                                size_t buflen, struct hostent **result, int *h_errnop);

/** The local array of available socket's rpc handler */
static int st_rpc_handler[MEMP_NUM_NETCONN] = {-1};
extern aos_mutex_t g_lwip_rpc_mutex;

static void ulwip_socket(u_socket_request_t *p_in, u_socket_reponse_t **pp_out)
{
    socket_in_t *p_param;
    u_socket_reponse_t *p_rsp = NULL;
    char m_st_rpc_name[SOCKET_RPC_NAME_SIZE] = {0};
    int st = -1;
    int rpc_handle = 0;
    int retn = AOS_FAILED;

    if ((NULL == p_in) || (NULL == pp_out))
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]parameter is null, and st is %d\r\n", __FUNCTION__, st));
        return;
    }

    p_rsp = malloc(sizeof(*p_rsp));
    if (NULL == p_rsp)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]malloc memory failed, and st is %d\r\n", __FUNCTION__, st));
        return;
    }

    p_rsp->m_data.m_socket.st = st;
    p_rsp->m_len = sizeof(*p_rsp);

    p_param = (socket_in_t *)&(p_in->m_data);

    retn = aos_mutex_lock((&g_lwip_rpc_mutex), AOS_WAIT_FOREVER);
    if (AOS_SUCCESS != retn)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]get mutex for %s failed\r\n", __FUNCTION__, m_st_rpc_name));
    }

    st = lwip_socket(p_param->domain, p_param->type, p_param->protocol);
    if (0 > st)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]call lwip_socket failed %d\r\n", __FUNCTION__, st));
        aos_mutex_unlock((&g_lwip_rpc_mutex));
        goto response;
    }
    /* get name of ulwip rpc service */
    retn = snprintf(m_st_rpc_name, sizeof(m_st_rpc_name), USOCKET_RPC_NAME_FORMAT, st);
    if (0 > retn)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]snprintf for rpc name %s failed\r\n", __FUNCTION__, m_st_rpc_name));
        aos_mutex_unlock((&g_lwip_rpc_mutex));
        lwip_close(st);
        goto response;
    }

    /* register socket rpc service into PM */
    retn = aos_rpc_regist(m_st_rpc_name, ULWIP_RPC_BUFFER_SIZE, &rpc_handle);
    if (AOS_SUCCESS != retn)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]register rpc service %s failed, ret:%d\r\n", __FUNCTION__, m_st_rpc_name, retn));
        aos_mutex_unlock((&g_lwip_rpc_mutex));
        lwip_close(st);
        goto response;
    }

    LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]register rpc service %s succeed, handle:0x%x\r\n", __FUNCTION__, m_st_rpc_name, rpc_handle));

    /* start the socket rpc service */
    retn = aos_rpc_run(rpc_handle, ulwip_request_proccess, ulwip_request_callback);
    if (AOS_SUCCESS != retn)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]run rpc %s failed, handle:0x%x\r\n", __FUNCTION__, m_st_rpc_name, rpc_handle));

        lwip_close(st);
        /* NOTE: free resource but the risk is that rpc thread is put into the ready queue */
        aos_rpc_close(rpc_handle);
        aos_mutex_unlock((&g_lwip_rpc_mutex));
        goto response;
    }

    LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]save the handle of rpc %s to local array[%d], handle:0x%x\r\n", __FUNCTION__, m_st_rpc_name, (st - LWIP_SOCKET_OFFSET), rpc_handle));

    st_rpc_handler[(st - LWIP_SOCKET_OFFSET)] = rpc_handle;
    aos_mutex_unlock((&g_lwip_rpc_mutex));

    LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]set response info of rpc %s to output param, handle:0x%x\r\n", __FUNCTION__, m_st_rpc_name, rpc_handle));
    p_rsp->m_data.m_socket.st = st;

response:

    *pp_out = p_rsp;
    return;
}

static void ulwip_close(u_socket_request_t *p_in, u_socket_reponse_t **pp_out)
{
    close_in_t *p_param;
    u_socket_reponse_t *p_rsp = NULL;
    int retn = AOS_FAILED;
    int rpc_handle = 0;

    if ((NULL == p_in) || (NULL == pp_out))
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]parameter is null\r\n", __FUNCTION__));
        return;
    }

    p_rsp = malloc(sizeof(*p_rsp));
    if (NULL == p_rsp)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]malloc memory failed\r\n", __FUNCTION__));
        return;
    }
    p_rsp->m_data.m_close.status = retn;
    p_param = (close_in_t *)&(p_in->m_data);

    retn = aos_mutex_lock((&g_lwip_rpc_mutex), AOS_WAIT_FOREVER);
    if (AOS_SUCCESS != retn)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]get mutex failed\r\n", __FUNCTION__));
    }

    retn = lwip_close(p_param->st);
    if (AOS_SUCCESS != retn)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]call lwip_close failed %d\r\n", __FUNCTION__, p_param->st));
        aos_mutex_unlock((&g_lwip_rpc_mutex));
        goto response;
    }

    rpc_handle = st_rpc_handler[(p_param->st - LWIP_SOCKET_OFFSET)];
    if (rpc_handle == -1)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]call rpc_handle is -1, %d\r\n", __FUNCTION__, p_param->st));
        aos_mutex_unlock((&g_lwip_rpc_mutex));
        goto response;
    }

    LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]get the handle of rpc %d from local array, handle:0x%x\r\n", __FUNCTION__, p_param->st, rpc_handle));

    st_rpc_handler[(p_param->st - LWIP_SOCKET_OFFSET)] = -1;

    retn = aos_rpc_close(rpc_handle);
    if (AOS_SUCCESS != retn)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]call aos_rpc_stop failed %d\r\n", __FUNCTION__, p_param->st));
    }

    aos_mutex_unlock((&g_lwip_rpc_mutex));

response:

    p_rsp->m_len = sizeof(*p_rsp);
    p_rsp->m_data.m_close.status = retn;

    LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]set response info of rpc %d to output param, handle:0x%x\r\n", __FUNCTION__, p_param->st, rpc_handle));

    *pp_out = p_rsp;

    return;
}

static void ulwip_accept(u_socket_request_t *p_in, u_socket_reponse_t **pp_out)
{
    accept_in_t *p_param;
    u_socket_reponse_t *p_rsp = NULL;
    char m_st_rpc_name[SOCKET_RPC_NAME_SIZE] = {0};
    int newsock = -1;
    int retn = AOS_FAILED;
    int rpc_handle = 0;

    if ((NULL == p_in) || (NULL == pp_out))
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]parameter is null\r\n", __FUNCTION__));
        return;
    }

    p_rsp = malloc(sizeof(*p_rsp) + USOCKET_PARAM_MAX_LEN);
    if (NULL == p_rsp)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]malloc memory failed\r\n", __FUNCTION__));
        return;
    }
    p_rsp->m_len = sizeof(*p_rsp) + USOCKET_PARAM_MAX_LEN;
    p_rsp->m_data.m_accept.newsock = newsock;
    p_param = (accept_in_t *)&(p_in->m_data);

    retn = aos_mutex_lock((&g_lwip_rpc_mutex), AOS_WAIT_FOREVER);
    if (AOS_SUCCESS != retn)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]get mutex failed\r\n", __FUNCTION__));
    }

    newsock = lwip_accept(p_param->st, (struct sockaddr *)&(p_rsp->append[0]), (socklen_t *)&(p_rsp->m_data.m_accept.addrlen));
    if (0 > newsock)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]call lwip_accept failed %d\r\n", __FUNCTION__, p_param->st));
        aos_mutex_unlock((&g_lwip_rpc_mutex));
        retn = AOS_FAILED;
        goto response;
    }

    /* get name of the socket rpc service */
    retn = snprintf(m_st_rpc_name, sizeof(m_st_rpc_name), USOCKET_RPC_NAME_FORMAT, newsock);
    if (0 > retn)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]snprintf for rpc name %s failed\r\n", __FUNCTION__, m_st_rpc_name));
        aos_mutex_unlock((&g_lwip_rpc_mutex));
        lwip_close(newsock);
        goto response;
    }

    /* register accept rpc service into PM */
    retn = aos_rpc_regist(m_st_rpc_name, ULWIP_RPC_BUFFER_SIZE, &rpc_handle);
    if (AOS_SUCCESS != retn)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]register rpc service %s failed, ret:%d\r\n", __FUNCTION__, m_st_rpc_name, retn));
        aos_mutex_unlock((&g_lwip_rpc_mutex));
        lwip_close(newsock);
        goto response;
    }

    LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]register rpc service %s succeed, handle:0x%x\r\n", __FUNCTION__, m_st_rpc_name, rpc_handle));

    /* start the socket rpc service */
    retn = aos_rpc_run(rpc_handle, ulwip_request_proccess, ulwip_request_callback);
    if (AOS_SUCCESS != retn)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]run rpc %s failed, handle:0x%x\r\n", __FUNCTION__, m_st_rpc_name, rpc_handle));
        /* NOTE: free resource but the risk is that rpc thread is put into the ready queue */
        lwip_close(newsock);
        aos_rpc_close(rpc_handle);
        aos_mutex_unlock((&g_lwip_rpc_mutex));
        goto response;
    }

    LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]save the handle of rpc %s to local array, handle:0x%x\r\n", __FUNCTION__, m_st_rpc_name, rpc_handle));

    st_rpc_handler[(newsock - LWIP_SOCKET_OFFSET)] = rpc_handle;

    aos_mutex_unlock((&g_lwip_rpc_mutex));
    LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]set response info of rpc %s to output param, handle:0x%x\r\n", __FUNCTION__, m_st_rpc_name, rpc_handle));
    p_rsp->m_data.m_accept.newsock = newsock;

response:

    *pp_out = p_rsp;

    return;
}

static void ulwip_bind(u_socket_request_t *p_in, u_socket_reponse_t **pp_out)
{
    bind_in_t *p_param;
    u_socket_reponse_t *p_rsp = NULL;
    int retn = AOS_FAILED;

    if ((NULL == p_in) || (NULL == pp_out))
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]parameter is null\r\n", __FUNCTION__));
        return;
    }

    p_param = (bind_in_t *)&(p_in->m_data);

    p_rsp = malloc(sizeof(*p_rsp));
    if (NULL == p_rsp)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]malloc memory failed\r\n", __FUNCTION__));
        return;
    }

    retn = lwip_bind(p_param->st, (const struct sockaddr *)&(p_in->append[0]), p_param->namelen);
    if (0 != retn)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]call lwip_bind failed %d\r\n", __FUNCTION__, p_param->st));
    }

    p_rsp->m_len = sizeof(u_socket_reponse_t);
    p_rsp->m_data.m_bind.status = retn;

    LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]set response info of rpc %d to output param\r\n", __FUNCTION__, p_param->st));

    *pp_out = p_rsp;

    return;
}

static void ulwip_connect(u_socket_request_t *p_in, u_socket_reponse_t **pp_out)
{
    connect_in_t *p_param;
    u_socket_reponse_t *p_rsp = NULL;
    int retn = AOS_FAILED;

    if ((NULL == p_in) || (NULL == pp_out))
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]parameter is null\r\n", __FUNCTION__));
        return;
    }

    p_rsp = malloc(sizeof(*p_rsp));
    if (NULL == p_rsp)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]malloc memory failed\r\n", __FUNCTION__));
        return;
    }

    p_param = (connect_in_t *)&(p_in->m_data);

    retn = lwip_connect(p_param->st, (const struct sockaddr *)&(p_in->append[0]), p_param->namelen);
    if (0 != retn)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]call lwip_connect failed %d\r\n", __FUNCTION__, p_param->st));
    }

    p_rsp->m_len = sizeof(u_socket_reponse_t);
    p_rsp->m_data.m_connect.status = retn;

    LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]set response info of rpc %d to output param\r\n", __FUNCTION__, p_param->st));

    *pp_out = p_rsp;

    return;
}

static void ulwip_listen(u_socket_request_t *p_in, u_socket_reponse_t **pp_out)
{
    listen_in_t *p_param;
    u_socket_reponse_t *p_rsp = NULL;
    int retn = AOS_FAILED;

    if ((NULL == p_in) || (NULL == pp_out))
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]parameter is null\r\n", __FUNCTION__));
        return;
    }

    p_rsp = malloc(sizeof(*p_rsp));
    if (NULL == p_rsp)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]malloc memory failed\r\n", __FUNCTION__));
        return;
    }

    p_param = (listen_in_t *)&(p_in->m_data);

    retn = lwip_listen(p_param->st, p_param->backlog);
    if (0 != retn)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]call lwip_listen failed %d\r\n", __FUNCTION__, p_param->st));
    }

    p_rsp->m_len = sizeof(u_socket_reponse_t);
    p_rsp->m_data.m_listen.status = retn;

    LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]set response info of rpc %d to output param\r\n", __FUNCTION__, p_param->st));

    *pp_out = p_rsp;

    return;
}

static void ulwip_recv(u_socket_request_t *p_in, u_socket_reponse_t **pp_out)
{
    recv_in_t *p_param;
    u_socket_reponse_t *p_rsp = NULL;
    int retn = AOS_FAILED;

    if ((NULL == p_in) || (NULL == pp_out))
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]parameter is null\r\n", __FUNCTION__));
        return;
    }

    p_param = (recv_in_t *)&(p_in->m_data);

    p_rsp = malloc(sizeof(*p_rsp) + p_param->len);
    if (NULL == p_rsp)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]malloc memory failed\r\n", __FUNCTION__));
        return;
    }

    retn = lwip_recv(p_param->st, &(p_rsp->append[0]), p_param->len, p_param->flags);
    if (0 > retn)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]call lwip_recv failed %d\r\n", __FUNCTION__, p_param->st));
    }

    p_rsp->m_len = sizeof(u_socket_reponse_t) + p_param->len;
    p_rsp->m_data.m_recv.off = retn;

    LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]set response info of rpc %d to output param\r\n", __FUNCTION__, p_param->st));

    *pp_out = p_rsp;

    return;
}

static void ulwip_recvfrom(u_socket_request_t *p_in, u_socket_reponse_t **pp_out)
{
    recvfrom_in_t *p_param;
    u_socket_reponse_t *p_rsp = NULL;
    int retn = AOS_FAILED;

    if ((NULL == p_in) || (NULL == pp_out))
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]parameter is null\r\n", __FUNCTION__));
        return;
    }

    p_param = (recvfrom_in_t *)&(p_in->m_data);

    p_rsp = malloc(sizeof(*p_rsp) + p_param->len);
    if (NULL == p_rsp)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]malloc memory failed\r\n", __FUNCTION__));
        return;
    }

    retn = lwip_recvfrom(p_param->st, &(p_rsp->append[0]), p_param->len, p_param->flags, (struct sockaddr *)&(p_rsp->m_data.m_recvfrom.from), (socklen_t *)&(p_rsp->m_data.m_recvfrom.fromlen));
    if (0 > retn)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]call lwip_recv failed %d\r\n", __FUNCTION__, p_param->st));
    }

    p_rsp->m_len = sizeof(u_socket_reponse_t) + p_param->len;
    p_rsp->m_data.m_recv.off = retn;

    LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]set response info of rpc %d to output param\r\n", __FUNCTION__, p_param->st));

    *pp_out = p_rsp;

    return;
}

static void ulwip_send(u_socket_request_t *p_in, u_socket_reponse_t **pp_out)
{
    send_in_t *p_param;
    u_socket_reponse_t *p_rsp = NULL;
    int retn = AOS_FAILED;

    if ((NULL == p_in) || (NULL == pp_out))
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]parameter is null\r\n", __FUNCTION__));
        return;
    }

    p_rsp = malloc(sizeof(*p_rsp));
    if (NULL == p_rsp)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]malloc memory failed\r\n", __FUNCTION__));
        return;
    }

    p_param = (send_in_t *)&(p_in->m_data);

    retn = lwip_send(p_param->st, &(p_in->append[0]), p_param->size, p_param->flags);
    if (0 > retn)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]call lwip_send failed %d\r\n", __FUNCTION__, p_param->st));
    }

    p_rsp->m_data.m_send.written = retn;
    p_rsp->m_len = sizeof(u_socket_reponse_t);

    LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]set response info of rpc %d to output param\r\n", __FUNCTION__, p_param->st));

    *pp_out = p_rsp;

    return;
}

static void ulwip_sendto(u_socket_request_t *p_in, u_socket_reponse_t **pp_out)
{
    sendto_in_t *p_param;
    u_socket_reponse_t *p_rsp = NULL;
    int retn = AOS_FAILED;

    if ((NULL == p_in) || (NULL == pp_out))
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]parameter is null\r\n", __FUNCTION__));
        return;
    }

    p_rsp = malloc(sizeof(*p_rsp));
    if (NULL == p_rsp)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]malloc memory failed\r\n", __FUNCTION__));
        return;
    }

    p_param = (sendto_in_t *)&(p_in->m_data);

    retn = lwip_sendto(p_param->st, &(p_in->append[0]), p_param->size, p_param->flags, (const struct sockaddr *)&(p_in->m_data.m_sendto.to), p_in->m_data.m_sendto.tolen);
    if (0 > retn)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]call lwip_send failed %d\r\n", __FUNCTION__, p_param->st));
    }

    p_rsp->m_data.m_send.written = retn;
    p_rsp->m_len = sizeof(u_socket_reponse_t);

    LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]set response info of rpc %d to output param\r\n", __FUNCTION__, p_param->st));

    *pp_out = p_rsp;

    return;
}

static void ulwip_getsockopt(u_socket_request_t *p_in, u_socket_reponse_t **pp_out)
{
    getsockopt_in_t *p_param;
    u_socket_reponse_t *p_rsp = NULL;
    int retn = AOS_FAILED;

    if ((NULL == p_in) || (NULL == pp_out))
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]parameter is null\r\n", __FUNCTION__));
        return;
    }

    p_rsp = malloc(sizeof(*p_rsp) + USOCKET_PARAM_MAX_LEN);
    if (NULL == p_rsp)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]malloc memory failed\r\n", __FUNCTION__));
        return;
    }

    p_param = (getsockopt_in_t *)&(p_in->m_data);

    retn = lwip_getsockopt(p_param->st, p_param->level, p_param->optname, &(p_rsp->append[0]), &(p_rsp->m_data.m_getsockopt.optlen));
    if (0 != retn)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]call lwip_getsockopt failed %d\r\n", __FUNCTION__, p_param->st));
    }

    p_rsp->m_len = sizeof(*p_rsp) + USOCKET_PARAM_MAX_LEN;
    p_rsp->m_data.m_getsockopt.status = retn;

    LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]set response info of rpc %d to output param\r\n", __FUNCTION__, p_param->st));

    *pp_out = p_rsp;

    return;
}

static void ulwip_setsockopt(u_socket_request_t *p_in, u_socket_reponse_t **pp_out)
{
    setsockopt_in_t *p_param;
    u_socket_reponse_t *p_rsp = NULL;
    int retn = AOS_FAILED;

    if ((NULL == p_in) || (NULL == pp_out))
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]parameter is null\r\n", __FUNCTION__));
        return;
    }

    p_rsp = malloc(sizeof(*p_rsp));
    if (NULL == p_rsp)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]malloc memory failed\r\n", __FUNCTION__));
        return;
    }

    p_param = (setsockopt_in_t *)&(p_in->m_data);

    retn = lwip_setsockopt(p_param->st, p_param->level, p_param->optname, &(p_in->append[0]), p_param->optlen);
    if (0 != retn)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]call lwip_setsockopt failed %d\r\n", __FUNCTION__, p_param->st));
    }

    p_rsp->m_len = sizeof(u_socket_reponse_t);
    p_rsp->m_data.m_setsockopt.status = retn;

    LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]set response info of rpc %d to output param\r\n", __FUNCTION__, p_param->st));

    *pp_out = p_rsp;

    return;
}

static void ulwip_fcntl(u_socket_request_t *p_in, u_socket_reponse_t **pp_out)
{
    fcntl_in_t *p_param;
    u_socket_reponse_t *p_rsp = NULL;
    int retn = AOS_FAILED;

    if ((NULL == p_in) || (NULL == pp_out))
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]parameter is null\r\n", __FUNCTION__));
        return;
    }

    p_rsp = malloc(sizeof(*p_rsp));
    if (NULL == p_rsp)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]malloc memory failed\r\n", __FUNCTION__));
        return;
    }

    p_param = (fcntl_in_t *)&(p_in->m_data);

    retn = lwip_fcntl(p_param->st, p_param->cmd, p_param->val);
    if (0 != retn)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]call lwip_fcntl failed %d, retn = %d\r\n", __FUNCTION__, p_param->st, retn));
    }

    p_rsp->m_len = sizeof(u_socket_reponse_t);
    p_rsp->m_data.m_fcntl.status = retn;

    LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]set response info of rpc %d to output param\r\n", __FUNCTION__, p_param->st));

    *pp_out = p_rsp;

    return;
}

static void ulwip_shutdown(u_socket_request_t *p_in, u_socket_reponse_t **pp_out)
{
    shutdown_in_t *p_param;
    u_socket_reponse_t *p_rsp = NULL;
    int retn = AOS_FAILED;

    if ((NULL == p_in) || (NULL == pp_out))
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]parameter is null\r\n", __FUNCTION__));
        return;
    }

    p_rsp = malloc(sizeof(*p_rsp));
    if (NULL == p_rsp)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]malloc memory failed\r\n", __FUNCTION__));
        return;
    }

    p_param = (shutdown_in_t *)&(p_in->m_data);

    retn = lwip_shutdown(p_param->st, p_param->how);
    if (0 != retn)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]call ulwip_shutdown failed %d, retn = %d\r\n", __FUNCTION__, p_param->st, retn));
    }

    p_rsp->m_len = sizeof(u_socket_reponse_t);
    p_rsp->m_data.m_shutdown.status = retn;

    LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]set response info of rpc %d to output param\r\n", __FUNCTION__, p_param->st));

    *pp_out = p_rsp;

    return;
}
#include "lwip/sys.h"
#define NUM_SELECTS 1024
/** The global array of available sockets */
static int selects[NUM_SELECTS];

static int alloc_select(void)
{
    int i;
    SYS_ARCH_DECL_PROTECT(lev);

    /* allocate a new socket identifier */
    for (i = 0; i < NUM_SELECTS; ++i)
    {

        /* Protect socket array */
        SYS_ARCH_PROTECT(lev);
        if (!selects[i])
        {
            selects[i] = 1;

            /* The socket is not yet known to anyone, so no need to protect
         after having marked it as used. */
            SYS_ARCH_UNPROTECT(lev);
            return i;
        }
        SYS_ARCH_UNPROTECT(lev);
    }
    return -1;
}

static void free_select(int i)
{
    /* Protect socket array */
    SYS_ARCH_SET(selects[i], 0);
}

static void ulwip_select_alloc(u_socket_request_t *p_in, u_socket_reponse_t **pp_out)
{
    u_socket_reponse_t *p_rsp = NULL;
    char m_st_rpc_name[SOCKET_RPC_NAME_SIZE] = {0};
    int rpc_handle;
    int id;
    int retn = AOS_FAILED;

    if ((NULL == p_in) || (NULL == pp_out))
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]parameter is null\r\n", __FUNCTION__));
        return;
    }

    p_rsp = malloc(sizeof(*p_rsp));
    if (NULL == p_rsp)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]malloc memory failed\r\n", __FUNCTION__));
        return;
    }

    p_rsp->m_data.m_select_alloc.id = -1;
    p_rsp->m_len = sizeof(u_socket_reponse_t);

    id = alloc_select();
    if (0 > id)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]alloc select failed\r\n", __FUNCTION__));
        goto response;
    }

    /* get name of select rpc service */
    retn = snprintf(m_st_rpc_name, sizeof(m_st_rpc_name), USELECT_RPC_NAME_FORMAT, id);
    if (0 > retn)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]snprintf for rpc name %s failed\r\n", __FUNCTION__, m_st_rpc_name));
        free_select(id);
        goto response;
    }

    /* register socket rpc service into PM */
    retn = aos_rpc_regist(m_st_rpc_name, ULWIP_RPC_BUFFER_SIZE, &rpc_handle);
    if (AOS_SUCCESS != retn)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]register rpc service %s failed, ret:%d\r\n", __FUNCTION__, m_st_rpc_name, retn));
        free_select(id);
        goto response;
    }

    LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]register rpc service %s succeed, handle:0x%x\r\n", __FUNCTION__, m_st_rpc_name, rpc_handle));

    /* start the socket rpc service */
    retn = aos_rpc_run(rpc_handle, ulwip_request_proccess, ulwip_request_callback);
    if (AOS_SUCCESS != retn)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]run rpc %s failed, handle:0x%x\r\n", __FUNCTION__, m_st_rpc_name, rpc_handle));

        free_select(id);
        /* NOTE: free resource but the risk is that rpc thread is put into the ready queue */
        aos_rpc_close(rpc_handle);
        goto response;
    }

    LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]save the handle of rpc %s to local array[%d], handle:0x%x\r\n", __FUNCTION__, m_st_rpc_name, id, rpc_handle));

    p_rsp->m_data.m_select_alloc.id = id;
    p_rsp->m_data.m_select_alloc.rpc_handle = rpc_handle;

    LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]set response info of rpc %d to output param\r\n", __FUNCTION__, p_rsp->m_data.m_select_alloc.id));
response:

    *pp_out = p_rsp;

    return;
}

static void ulwip_select_free(u_socket_request_t *p_in, u_socket_reponse_t **pp_out)
{
    select_free_in_t *p_param;
    u_socket_reponse_t *p_rsp = NULL;
    int retn = AOS_FAILED;

    if ((NULL == p_in) || (NULL == pp_out))
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]parameter is null\r\n", __FUNCTION__));
        return;
    }

    p_rsp = malloc(sizeof(*p_rsp));
    if (NULL == p_rsp)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]malloc memory failed\r\n", __FUNCTION__));
        return;
    }

    p_param = (select_free_in_t *)&(p_in->m_data);
    retn = aos_rpc_close(p_param->rpc_handle);
    if (AOS_SUCCESS != retn)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]call aos_rpc_stop failed %d\r\n", __FUNCTION__, p_param->id));
    }

    free_select(p_param->id);

    p_rsp->m_len = sizeof(u_socket_reponse_t);
    p_rsp->m_data.m_select_free.status = retn;

    LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[0x%x][%s]set response info of rpc %d to output param\r\n", (uint32_t)aos_task_now(), __FUNCTION__, p_param->id));

    *pp_out = p_rsp;

    return;
}

void ulwip_select(u_socket_request_t *p_in, u_socket_reponse_t **pp_out)
{
    select_in_t *p_param;
    u_socket_reponse_t *p_rsp = NULL;
    fd_set *readset = NULL;
    fd_set *writeset = NULL;
    fd_set *exceptset = NULL;
    struct timeval *timeout = NULL;
    int retn = AOS_SUCCESS;

    if (NULL == pp_out)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]parameter is null\r\n", __FUNCTION__));
        return;
    }

    p_rsp = malloc(sizeof(*p_rsp));
    if (NULL == p_rsp)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]malloc memory failed\r\n", __FUNCTION__));
        return;
    }

    memset(p_rsp, 0, sizeof(*p_rsp));

    p_param = (select_in_t *)&(p_in->m_data);
    readset = (fd_set *)&(p_in->append[0]);
    writeset = (fd_set *)((char *)readset + sizeof(*readset));
    exceptset = (fd_set *)((char *)writeset + sizeof(*writeset));
    timeout = (struct timeval *)((char *)exceptset + sizeof(*exceptset));

    retn = lwip_select(p_param->maxfdp1, readset, writeset, exceptset, timeout);
    if (0 > retn)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]call lwip_setsockopt failed %d\r\n", __FUNCTION__, p_param->maxfdp1));
    }

    p_rsp->m_len = sizeof(u_socket_reponse_t);
    p_rsp->m_data.m_select.nready = retn;

    LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]set response info of rpc %d to output param\r\n", __FUNCTION__, p_param->maxfdp1));

    *pp_out = p_rsp;

    return;
}

void ulwip_getaddrinfo(u_socket_request_t *p_in, u_socket_reponse_t **pp_out)
{
    getaddrinfo_in_t *p_param;
    u_socket_reponse_t *p_rsp = NULL;
    char *nodename = NULL;
    char *servname = NULL;
    struct addrinfo *hints = NULL;
    struct addrinfo *res = NULL;
    int retn = AOS_SUCCESS;

    if (NULL == pp_out)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]parameter is null\r\n", __FUNCTION__));
        return;
    }

    p_rsp = malloc(sizeof(*p_rsp));
    if (NULL == p_rsp)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]malloc memory failed\r\n", __FUNCTION__));
        return;
    }

    memset(p_rsp, 0, sizeof(*p_rsp));
    p_rsp->m_len = sizeof(u_socket_reponse_t);
    p_rsp->m_data.m_getaddrinfo.status = -1;

    p_param = (getaddrinfo_in_t *)&(p_in->m_data);
    if (p_param->nodename_len >= 0)
    {
        nodename = malloc(p_param->nodename_len + 1);
        if (NULL == nodename)
        {
            LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]malloc memory for nodename failed\r\n", __FUNCTION__));
            goto response;
        }
        memset(nodename, 0, p_param->nodename_len + 1);
        memcpy(nodename, &(p_in->append[0]), p_param->nodename_len);
    }

    if (p_param->servname_len >= 0)
    {
        servname = malloc(p_param->servname_len + 1);
        if (NULL == servname)
        {
            LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]malloc memory for servname failed\r\n", __FUNCTION__));
            goto response;
        }
        memset(servname, 0, p_param->servname_len + 1);
        memcpy(servname, (char *)(&(p_in->append[0])) + p_param->nodename_len, p_param->servname_len);
    }

    hints = (struct addrinfo *)((char *)(&(p_in->append[0])) + p_param->nodename_len + p_param->servname_len);

    LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]nodemane[%d] (%s), severname[%d](%s)\r\n", __FUNCTION__, p_param->nodename_len, nodename, p_param->servname_len, servname));

    retn = lwip_getaddrinfo(nodename, servname, hints, &res);
    if (0 != retn)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]call lwip_getaddrinfo failed %d\r\n", __FUNCTION__, retn));
        goto response;
    }

    LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]set response info of rpc to output param\r\n", __FUNCTION__));

    p_rsp->m_data.m_getaddrinfo.status = retn;
    p_rsp->m_data.m_getaddrinfo.ai_family = res->ai_family;
    p_rsp->m_data.m_getaddrinfo.ai_addrlen = res->ai_addrlen;
    memcpy(&(p_rsp->m_data.m_getaddrinfo.sa), res->ai_addr, res->ai_addrlen);

response:
    if (NULL != res)
    {
        lwip_freeaddrinfo(res);
    }
    if (NULL != nodename)
    {
        free(nodename);
    }
    if (NULL != servname)
    {
        free(servname);
    }

    *pp_out = p_rsp;

    return;
}

u_socket_func m_socket_opt[EVT_ST_MAX] =
    {
        [EVT_ST_SOCKET] = ulwip_socket,
        [EVT_ST_ACCEPT] = ulwip_accept,
        [EVT_ST_BIND] = ulwip_bind,
        [EVT_ST_CONNECT] = ulwip_connect,
        [EVT_ST_LISTEN] = ulwip_listen,
        [EVT_ST_RECV] = ulwip_recv,
        [EVT_ST_RECVFROM] = ulwip_recvfrom,
        [EVT_ST_SEND] = ulwip_send,
        [EVT_ST_SENDTO] = ulwip_sendto,
        [EVT_ST_GETSOCKOPT] = ulwip_getsockopt,
        [EVT_ST_SETSOCKOPT] = ulwip_setsockopt,
        [EVT_ST_CLOSE] = ulwip_close,
        [EVT_ST_FCNTL] = ulwip_fcntl,
        [EVT_ST_SELECT] = ulwip_select,
        [EVT_ST_GETADDRINFO] = ulwip_getaddrinfo,
        [EVT_ST_SHUTDOWN] = ulwip_shutdown,
        [EVT_ST_SELECT_ALLOC] = ulwip_select_alloc,
        [EVT_ST_SELECT_FREE] = ulwip_select_free,
};