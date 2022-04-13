/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include "k_api.h"
#include "aos/kernel.h"
#include "netdb.h"
#include "lwip_rpc.h"
#include "lwip/debug.h"
#include "rpc.h"

int vfs_st_socket(int domain, int type, int protocol)
{
    struct aos_parcel_t reply;
    u_socket_request_t rqst;
    u_socket_reponse_t rsps;
    int rpc_handle;
    size_t len;
    int ret;

    rsps.m_data.m_socket.st = -1;
    rqst.m_evt = EVT_ST_SOCKET;
    rqst.m_len = sizeof(rqst);
    rqst.m_data.m_socket.domain = domain;
    rqst.m_data.m_socket.type = type;
    rqst.m_data.m_socket.protocol = protocol;

    LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]set request to %s\n\r", __FUNCTION__, ULWIP_RPC_MAIN_NAME));

    ret = aos_rpc_open_sync(ULWIP_RPC_MAIN_NAME, ULWIP_RPC_BUFFER_SIZE, &rpc_handle);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]open rpc:%s fail(%d)\n\r", __FUNCTION__, ULWIP_RPC_MAIN_NAME, ret));
        return AOS_FAILED;
    }

    ret = aos_rpc_call_sync(rpc_handle, &reply, 1, sizeof(rqst), &rqst);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]aos_rpc_call_sync error: %d\n\r", __FUNCTION__, ret));
        goto back;
    }

    LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]rpc call sync finished, ret = %d\n\r", __FUNCTION__, ret));

    /* parse response */
    len = sizeof(rsps);
    ret = aos_parcel_get(AP_BUF, &rsps, &len, &reply);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]aos_parcel_get error: %d\n\r", __FUNCTION__, ret));
        goto back;
    }

    LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]get response len = %d, st = %d\n\r", __FUNCTION__, rsps.m_len, rsps.m_data.m_socket.st));

back:
    ret = aos_rpc_close(rpc_handle);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]aos_rpc_close error: %d\n\r", __FUNCTION__, ret));
        return AOS_FAILED;
    }

    return rsps.m_data.m_socket.st;
}

int vfs_st_close(int st)
{
    struct aos_parcel_t reply;
    u_socket_request_t rqst;
    u_socket_reponse_t rsps;
    size_t len;
    int rpc_handle;
    int ret;

    rsps.m_data.m_close.status = -1;
    rqst.m_evt = EVT_ST_CLOSE;
    rqst.m_len = sizeof(rqst);
    rqst.m_data.m_close.st = st;

    LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]set request to %s\n\r", __FUNCTION__, ULWIP_RPC_MAIN_NAME));

    ret = aos_rpc_open_sync(ULWIP_RPC_MAIN_NAME, ULWIP_RPC_BUFFER_SIZE, &rpc_handle);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]open rpc:%s fail(%d)\n\r", __FUNCTION__, ULWIP_RPC_MAIN_NAME, ret));
        return AOS_FAILED;
    }

    ret = aos_rpc_call_sync(rpc_handle, &reply, 1, sizeof(rqst), &rqst);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]aos_rpc_call_sync error: %d\n\r", __FUNCTION__, ret));
        goto back;
    }
    /* parse response */
    len = sizeof(rsps);
    ret = aos_parcel_get(AP_BUF, &rsps, &len, &reply);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]aos_parcel_get error: %d\n\r", __FUNCTION__, ret));
        goto back;
    }

    LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]get response len = %d, status = %d\n\r", __FUNCTION__, rsps.m_len, rsps.m_data.m_close.status));

back:

    ret = aos_rpc_close(rpc_handle);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]aos_rpc_close error: %d\n\r", __FUNCTION__, ret));
        return AOS_FAILED;
    }

    return rsps.m_data.m_close.status;
}

int vfs_st_accept(int st, struct sockaddr *addr, socklen_t *addrlen)
{
    struct aos_parcel_t reply;
    u_socket_request_t rqst;
    u_socket_reponse_t *p_rsp = NULL;
    char m_st_rpc_name[SOCKET_RPC_NAME_SIZE] = {0};
    size_t rsp_len;
    int rpc_handle;
    int ret;

    if ((NULL == addr) || (NULL == addrlen))
    {
        return AOS_FAILED;
    }
    /* parse response */
    rsp_len = sizeof(*p_rsp) + USOCKET_PARAM_MAX_LEN;

    p_rsp = malloc(rsp_len);
    if (p_rsp == NULL)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]malloc memory %d for reponse data failed!\n\r", __FUNCTION__, rsp_len));
        return AOS_FAILED;
    }

    p_rsp->m_data.m_accept.newsock = -1;

    rqst.m_evt = EVT_ST_ACCEPT;
    rqst.m_len = sizeof(rqst);
    rqst.m_data.m_accept.st = st;

    /* get name of accept rpc service */
    ret = snprintf(m_st_rpc_name, sizeof(m_st_rpc_name), USOCKET_RPC_NAME_FORMAT, st);
    if (0 > ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]get rpc name %s failed, ret = %d\n\r", __FUNCTION__, m_st_rpc_name, ret));
        goto back;
    }

    LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]set request to %s\n\r", __FUNCTION__, m_st_rpc_name));

    ret = aos_rpc_open_sync(m_st_rpc_name, ULWIP_RPC_BUFFER_SIZE, &rpc_handle);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]open rpc:%s fail(%d)\n\r", __FUNCTION__, m_st_rpc_name, ret));
        goto back;
    }

    ret = aos_rpc_call_sync(rpc_handle, &reply, 1, sizeof(rqst), &rqst);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]aos_rpc_call_sync error: %d\n\r", __FUNCTION__, ret));
        goto back;
    }

    ret = aos_parcel_get(AP_BUF, p_rsp, &rsp_len, &reply);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]aos_parcel_get error: %d\n\r", __FUNCTION__, ret));
        goto back;
    }

    LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]get response len = %d, status = %d\n\r", __FUNCTION__, p_rsp->m_len, p_rsp->m_data.m_accept.newsock));

    memcpy(addr, &(p_rsp->append[0]), p_rsp->m_data.m_accept.addrlen);
    *addrlen = p_rsp->m_data.m_accept.addrlen;
back:

    ret = aos_rpc_close(rpc_handle);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]aos_rpc_close error: %d\n\r", __FUNCTION__, ret));
    }

    if (NULL != p_rsp)
    {
        ret = p_rsp->m_data.m_accept.newsock;
        free(p_rsp);
    }

    return ret;
}

int vfs_st_bind(int st, const struct sockaddr *name, socklen_t namelen)
{
    struct aos_parcel_t reply;
    u_socket_request_t rqst;
    u_socket_reponse_t rsps;
    char m_st_rpc_name[SOCKET_RPC_NAME_SIZE] = {0};
    size_t len;
    int rpc_handle;
    int ret;

    if ((NULL == name) || (0 == namelen))
    {
        return AOS_FAILED;
    }

    rsps.m_data.m_bind.status = -1;
    rqst.m_evt = EVT_ST_BIND;
    rqst.m_len = (sizeof(rqst) + namelen);
    rqst.m_data.m_bind.st = st;
    rqst.m_data.m_bind.namelen = namelen;

    /* get name of ulwip rpc service */
    ret = snprintf(m_st_rpc_name, sizeof(m_st_rpc_name), USOCKET_RPC_NAME_FORMAT, st);
    if (0 > ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s] %s, ret = %d\n\r", __FUNCTION__, m_st_rpc_name, ret));
        return AOS_FAILED;
    }

    LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]set request to %s\n\r", __FUNCTION__, m_st_rpc_name));

    ret = aos_rpc_open_sync(m_st_rpc_name, ULWIP_RPC_BUFFER_SIZE, &rpc_handle);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]open rpc:%s fail(%d)\n\r", __FUNCTION__, m_st_rpc_name, ret));
        return AOS_FAILED;
    }

    ret = aos_rpc_call_sync(rpc_handle, &reply, 2, sizeof(rqst), &rqst, namelen, name);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]aos_rpc_call_sync error: %d\n\r", __FUNCTION__, ret));
        goto back;
    }
    /* parse response */
    len = sizeof(rsps);
    ret = aos_parcel_get(AP_BUF, &rsps, &len, &reply);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]aos_parcel_get error: %d\n\r", __FUNCTION__, ret));
        goto back;
    }

    LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]get response len[0x%x] = %d, status[0x%x] = %d\n\r", __FUNCTION__, (uint32_t) & (rsps.m_len), rsps.m_len, (uint32_t) & (rsps.m_data.m_bind.status), rsps.m_data.m_bind.status));

back:

    ret = aos_rpc_close(rpc_handle);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]aos_rpc_close error: %d\n\r", __FUNCTION__, ret));
        return AOS_FAILED;
    }

    return rsps.m_data.m_bind.status;
}

int vfs_st_connect(int st, const struct sockaddr *name, socklen_t namelen)
{
    struct aos_parcel_t reply;
    u_socket_request_t rqst;
    u_socket_reponse_t rsps;
    char m_st_rpc_name[SOCKET_RPC_NAME_SIZE] = {0};
    size_t len;
    int rpc_handle;
    int ret;

    if ((NULL == name) || (0 == namelen))
    {
        return AOS_FAILED;
    }

    rsps.m_data.m_connect.status = -1;
    rqst.m_evt = EVT_ST_CONNECT;
    rqst.m_len = sizeof(rqst) + namelen;
    rqst.m_data.m_connect.st = st;
    rqst.m_data.m_connect.namelen = namelen;

    /* get name of ulwip rpc service */
    ret = snprintf(m_st_rpc_name, sizeof(m_st_rpc_name), USOCKET_RPC_NAME_FORMAT, st);
    if (0 > ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]%s, ret = %d\n\r", __FUNCTION__, m_st_rpc_name, ret));
        return AOS_FAILED;
    }

    LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]set request to %s\n\r", __FUNCTION__, m_st_rpc_name));

    ret = aos_rpc_open_sync(m_st_rpc_name, ULWIP_RPC_BUFFER_SIZE, &rpc_handle);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]open rpc:%s fail(%d)\n\r", __FUNCTION__, m_st_rpc_name, ret));
        return AOS_FAILED;
    }

    ret = aos_rpc_call_sync(rpc_handle, &reply, 2, sizeof(rqst), &rqst, namelen, name);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]aos_rpc_call_sync error: %d\n\r", __FUNCTION__, ret));
        goto back;
    }
    /* parse response */
    len = sizeof(rsps);
    ret = aos_parcel_get(AP_BUF, &rsps, &len, &reply);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]aos_parcel_get error: %d\n\r", __FUNCTION__, ret));
        goto back;
    }

    LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]get response len = %d, status = %d\n\r", __FUNCTION__, rsps.m_len, rsps.m_data.m_connect.status));

back:

    ret = aos_rpc_close(rpc_handle);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]aos_rpc_close error: %d\n\r", __FUNCTION__, ret));
        return AOS_FAILED;
    }

    return rsps.m_data.m_connect.status;
}

int vfs_st_listen(int st, int backlog)
{
    struct aos_parcel_t reply;
    u_socket_request_t rqst;
    u_socket_reponse_t rsps;
    char m_st_rpc_name[SOCKET_RPC_NAME_SIZE] = {0};
    size_t len;
    int rpc_handle;
    int ret;

    rsps.m_data.m_listen.status = -1;
    rqst.m_evt = EVT_ST_LISTEN;
    rqst.m_len = sizeof(rqst);
    rqst.m_data.m_listen.st = st;
    rqst.m_data.m_listen.backlog = backlog;

    /* get name of ulwip rpc service */
    ret = snprintf(m_st_rpc_name, sizeof(m_st_rpc_name), USOCKET_RPC_NAME_FORMAT, st);
    if (0 > ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s] %s, ret = %d\n\r", __FUNCTION__, m_st_rpc_name, ret));
        return AOS_FAILED;
    }

    LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]set request to %s\n\r", __FUNCTION__, m_st_rpc_name));

    ret = aos_rpc_open_sync(m_st_rpc_name, ULWIP_RPC_BUFFER_SIZE, &rpc_handle);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]open rpc:%s fail(%d)\n\r", __FUNCTION__, m_st_rpc_name, ret));
        return AOS_FAILED;
    }

    ret = aos_rpc_call_sync(rpc_handle, &reply, 1, sizeof(rqst), &rqst);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]aos_rpc_call_sync error: %d\n\r", __FUNCTION__, ret));
        goto back;
    }

    /* parse response */
    len = sizeof(rsps);
    ret = aos_parcel_get(AP_BUF, &rsps, &len, &reply);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]aos_parcel_get error: %d\n\r", __FUNCTION__, ret));
        goto back;
    }

    LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]get response len = %d, status = %d\n\r", __FUNCTION__, rsps.m_len, rsps.m_data.m_listen.status));

back:

    ret = aos_rpc_close(rpc_handle);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]aos_rpc_close error: %d\n\r", __FUNCTION__, ret));
        return AOS_FAILED;
    }

    return rsps.m_data.m_listen.status;
}

int vfs_st_send(int st, const void *data, size_t size, int flags)
{
    struct aos_parcel_t reply;
    u_socket_request_t rqst;
    u_socket_reponse_t rsps;
    char m_st_rpc_name[SOCKET_RPC_NAME_SIZE] = {0};
    size_t len;
    int rpc_handle;
    int ret;

    if ((NULL == data) || (0 == size))
    {
        return AOS_FAILED;
    }

    if (size > ULWIP_RPC_BUFFER_SIZE)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]send length %d is large than rpc buffer size %d!\n\r", __FUNCTION__, len, ULWIP_RPC_BUFFER_SIZE));
        return AOS_FAILED;
    }

    rsps.m_data.m_send.written = -1;
    rqst.m_evt = EVT_ST_SEND;
    rqst.m_len = sizeof(rqst) + size;
    rqst.m_data.m_send.st = st;
    rqst.m_data.m_send.size = size;
    rqst.m_data.m_send.flags = flags;

    /* get name of ulwip rpc service */
    ret = snprintf(m_st_rpc_name, sizeof(m_st_rpc_name), USOCKET_RPC_NAME_FORMAT, st);
    if (0 > ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s] %s, ret = %d\n\r", __FUNCTION__, m_st_rpc_name, ret));
        return AOS_FAILED;
    }

    LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s] %s, ret = %d\n\r", __FUNCTION__, m_st_rpc_name, ret));

    ret = aos_rpc_open_sync(m_st_rpc_name, ULWIP_RPC_BUFFER_SIZE, &rpc_handle);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]open rpc:%s fail(%d)\n\r", __FUNCTION__, m_st_rpc_name, ret));
        return AOS_FAILED;
    }

    ret = aos_rpc_call_sync(rpc_handle, &reply, 2, sizeof(rqst), &rqst, size, data);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]aos_rpc_call_sync error: %d\n\r", __FUNCTION__, ret));
        goto back;
    }

    /* parse response */
    len = sizeof(rsps);
    ret = aos_parcel_get(AP_BUF, &rsps, &len, &reply);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]aos_parcel_get error: %d\n\r", __FUNCTION__, ret));
        goto back;
    }

    LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]get response len = %d, status = %d\n\r", __FUNCTION__, rsps.m_len, rsps.m_data.m_send.written));

back:

    ret = aos_rpc_close(rpc_handle);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]aos_rpc_close error: %d\n\r", __FUNCTION__, ret));
        return AOS_FAILED;
    }

    return rsps.m_data.m_send.written;
}

int vfs_st_sendto(int st, const void *data, size_t size, int flags, const struct sockaddr *to, socklen_t tolen)
{
    struct aos_parcel_t reply;
    u_socket_request_t rqst;
    u_socket_reponse_t rsps;
    char m_st_rpc_name[SOCKET_RPC_NAME_SIZE] = {0};
    size_t len;
    int rpc_handle;
    int ret;

    if ((NULL == data) || (0 == size))
    {
        return AOS_FAILED;
    }
    if ((NULL == to) || (0 == tolen))
    {
        return vfs_st_send(st, data, size, flags);
    }

    if (size > ULWIP_RPC_BUFFER_SIZE)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]send length %d is large than rpc buffer size %d!\n\r", __FUNCTION__, len, ULWIP_RPC_BUFFER_SIZE));
        return AOS_FAILED;
    }

    rsps.m_data.m_sendto.written = -1;
    rqst.m_evt = EVT_ST_SENDTO;
    rqst.m_len = sizeof(rqst) + size;
    rqst.m_data.m_sendto.st = st;
    rqst.m_data.m_sendto.size = size;
    rqst.m_data.m_sendto.flags = flags;
    rqst.m_data.m_sendto.tolen = tolen;
    memcpy(&(rqst.m_data.m_sendto.to), to, tolen);

    /* get name of ulwip rpc service */
    ret = snprintf(m_st_rpc_name, sizeof(m_st_rpc_name), USOCKET_RPC_NAME_FORMAT, st);
    if (0 > ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s] %s, ret = %d\n\r", __FUNCTION__, m_st_rpc_name, ret));
        return AOS_FAILED;
    }

    LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s] %s, ret = %d\n\r", __FUNCTION__, m_st_rpc_name, ret));

    ret = aos_rpc_open_sync(m_st_rpc_name, ULWIP_RPC_BUFFER_SIZE, &rpc_handle);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]open rpc:%s fail(%d)\n\r", __FUNCTION__, m_st_rpc_name, ret));
        return AOS_FAILED;
    }

    ret = aos_rpc_call_sync(rpc_handle, &reply, 2, sizeof(rqst), &rqst, size, data);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]aos_rpc_call_sync error: %d\n\r", __FUNCTION__, ret));
        goto back;
    }

    /* parse response */
    len = sizeof(rsps);
    ret = aos_parcel_get(AP_BUF, &rsps, &len, &reply);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]aos_parcel_get error: %d\n\r", __FUNCTION__, ret));
        goto back;
    }

    LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]get response len = %d, status = %d\n\r", __FUNCTION__, rsps.m_len, rsps.m_data.m_send.written));

back:

    ret = aos_rpc_close(rpc_handle);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]aos_rpc_close error: %d\n\r", __FUNCTION__, ret));
        return AOS_FAILED;
    }

    return rsps.m_data.m_sendto.written;
}

int vfs_st_recv(int st, void *mem, size_t len, int flags)
{
    struct aos_parcel_t reply;
    u_socket_request_t rqst;
    u_socket_reponse_t *p_rsp = NULL;
    char m_st_rpc_name[SOCKET_RPC_NAME_SIZE] = {0};
    size_t rsp_len;
    int rpc_handle;
    int ret;

    if ((NULL == mem) || (0 == len))
    {
        return AOS_FAILED;
    }

    if (len > ULWIP_RPC_BUFFER_SIZE)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]recv length %d is large than rpc buffer size %d!\n\r", __FUNCTION__, len, ULWIP_RPC_BUFFER_SIZE));
        return AOS_FAILED;
    }

    p_rsp = malloc(sizeof(u_socket_reponse_t) + len);
    if (p_rsp == NULL)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]malloc memory %d for reponse data failed!\n\r", __FUNCTION__, len));
        return AOS_FAILED;
    }
    p_rsp->m_data.m_recv.off = -1;

    rqst.m_evt = EVT_ST_RECV;
    rqst.m_len = sizeof(rqst);
    rqst.m_data.m_recv.st = st;
    rqst.m_data.m_recv.len = len;
    rqst.m_data.m_recv.flags = flags;

    /* get name of ulwip rpc service */
    ret = snprintf(m_st_rpc_name, sizeof(m_st_rpc_name), USOCKET_RPC_NAME_FORMAT, st);
    if (0 > ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s][%d] %s, ret = %d\n\r", __FUNCTION__, __LINE__, m_st_rpc_name, ret));
        goto back;
    }

    ret = aos_rpc_open_sync(m_st_rpc_name, ULWIP_RPC_BUFFER_SIZE, &rpc_handle);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]open rpc:%s fail(%d)\n\r", __FUNCTION__, m_st_rpc_name, ret));
        goto back;
    }

    ret = aos_rpc_call_sync(rpc_handle, &reply, 1, sizeof(rqst), &rqst);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]aos_rpc_call_sync error: %d\n\r", __FUNCTION__, ret));
        goto back;
    }

    /* parse response */
    rsp_len = sizeof(u_socket_reponse_t) + len;
    ret = aos_parcel_get(AP_BUF, p_rsp, &rsp_len, &reply);
    if ((AOS_SUCCESS != ret) || (0 > p_rsp->m_data.m_recv.off))
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]aos_parcel_get error: %d\n\r", __FUNCTION__, ret));
        goto back;
    }

    LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]%s get response len = %d, off = %d\n\r", __FUNCTION__, aos_task_name(), p_rsp->m_len, p_rsp->m_data.m_recv.off));

    memcpy(mem, &(p_rsp->append[0]), p_rsp->m_data.m_recv.off);

back:

    ret = aos_rpc_close(rpc_handle);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]aos_rpc_close error: %d\n\r", __FUNCTION__, ret));
    }

    if (NULL != p_rsp)
    {
        ret = p_rsp->m_data.m_recv.off;
        free(p_rsp);
    }

    return ret;
}

int vfs_st_recvfrom(int st, void *mem, size_t len, int flags, struct sockaddr *from, socklen_t *fromlen)
{
    struct aos_parcel_t reply;
    u_socket_request_t rqst;
    u_socket_reponse_t *p_rsp = NULL;
    char m_st_rpc_name[SOCKET_RPC_NAME_SIZE] = {0};
    size_t rsp_len;
    int rpc_handle;
    int ret;

    if ((NULL == mem) || (0 == len))
    {
        return AOS_FAILED;
    }

    if ((NULL == from) || (NULL == fromlen))
    {
        return vfs_st_recv(st, mem, len, flags);
    }

    if (len > ULWIP_RPC_BUFFER_SIZE)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]recv length %d is large than rpc buffer size %d!\n\r", __FUNCTION__, len, ULWIP_RPC_BUFFER_SIZE));
        return AOS_FAILED;
    }

    p_rsp = malloc(sizeof(u_socket_reponse_t) + len);
    if (p_rsp == NULL)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]malloc memory %d for reponse data failed!\n\r", __FUNCTION__, len));
        return AOS_FAILED;
    }
    p_rsp->m_data.m_recvfrom.off = -1;

    rqst.m_evt = EVT_ST_RECVFROM;
    rqst.m_len = sizeof(rqst);
    rqst.m_data.m_recvfrom.st = st;
    rqst.m_data.m_recvfrom.len = len;
    rqst.m_data.m_recvfrom.flags = flags;

    /* get name of ulwip rpc service */
    ret = snprintf(m_st_rpc_name, sizeof(m_st_rpc_name), USOCKET_RPC_NAME_FORMAT, st);
    if (0 > ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s][%d] %s, ret = %d\n\r", __FUNCTION__, __LINE__, m_st_rpc_name, ret));
        goto back;
    }

    ret = aos_rpc_open_sync(m_st_rpc_name, ULWIP_RPC_BUFFER_SIZE, &rpc_handle);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]open rpc:%s fail(%d)\n\r", __FUNCTION__, m_st_rpc_name, ret));
        goto back;
    }

    ret = aos_rpc_call_sync(rpc_handle, &reply, 1, sizeof(rqst), &rqst);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]aos_rpc_call_sync error: %d\n\r", __FUNCTION__, ret));
        goto back;
    }

    /* parse response */
    rsp_len = sizeof(u_socket_reponse_t) + len;
    ret = aos_parcel_get(AP_BUF, p_rsp, &rsp_len, &reply);
    if ((AOS_SUCCESS != ret) || (0 > p_rsp->m_data.m_recvfrom.off))
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]aos_parcel_get error: %d\n\r", __FUNCTION__, ret));
        goto back;
    }

    LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]get response len = %d, off = %d\n\r", __FUNCTION__, p_rsp->m_len, p_rsp->m_data.m_recv.off));

    memcpy(mem, &(p_rsp->append[0]), p_rsp->m_data.m_recvfrom.off);
    memcpy(from, &(p_rsp->m_data.m_recvfrom.from), p_rsp->m_data.m_recvfrom.fromlen);
    memcpy(fromlen, &(p_rsp->m_data.m_recvfrom.fromlen), sizeof(socklen_t));

back:

    ret = aos_rpc_close(rpc_handle);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]aos_rpc_close error: %d\n\r", __FUNCTION__, ret));
    }

    if (NULL != p_rsp)
    {
        ret = p_rsp->m_data.m_recvfrom.off;
        free(p_rsp);
    }

    return ret;
}

int vfs_st_getsockopt(int st, int level, int optname, void *optval, socklen_t *optlen)
{
    struct aos_parcel_t reply;
    u_socket_request_t rqst;
    u_socket_reponse_t *p_rsp = NULL;
    char m_st_rpc_name[SOCKET_RPC_NAME_SIZE] = {0};
    size_t rsp_len;
    int rpc_handle;
    int ret;

    if ((NULL == optval) || (0 == optlen))
    {
        return AOS_FAILED;
    }

    rsp_len = sizeof(*p_rsp) + USOCKET_PARAM_MAX_LEN;

    p_rsp = malloc(rsp_len);
    if (p_rsp == NULL)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]malloc memory %d for reponse data failed!\n\r", __FUNCTION__, rsp_len));
        return AOS_FAILED;
    }

    p_rsp->m_data.m_getsockopt.status = -1;

    rqst.m_evt = EVT_ST_GETSOCKOPT;
    rqst.m_len = sizeof(rqst);
    rqst.m_data.m_connect.st = st;
    rqst.m_data.m_getsockopt.level = level;

    /* get name of ulwip rpc service */
    ret = snprintf(m_st_rpc_name, sizeof(m_st_rpc_name), USOCKET_RPC_NAME_FORMAT, st);
    if (0 > ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s][%d] %s, ret = %d\n\r", __FUNCTION__, __LINE__, m_st_rpc_name, ret));
        goto back;
    }

    ret = aos_rpc_open_sync(m_st_rpc_name, ULWIP_RPC_BUFFER_SIZE, &rpc_handle);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]open rpc:%s fail(%d)\n\r", __FUNCTION__, m_st_rpc_name, ret));
        goto back;
    }

    ret = aos_rpc_call_sync(rpc_handle, &reply, 1, sizeof(rqst), &rqst);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]aos_rpc_call_sync error: %d\n\r", __FUNCTION__, ret));
        goto back;
    }

    /* parse response */
    ret = aos_parcel_get(AP_BUF, p_rsp, &rsp_len, &reply);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]aos_parcel_get error: %d\n\r", __FUNCTION__, ret));
        goto back;
    }

    LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]get response len = %d, status = %d\n\r", __FUNCTION__, p_rsp->m_len, p_rsp->m_data.m_getsockopt.status));

    memcpy(optval, &(p_rsp->append[0]), p_rsp->m_data.m_getsockopt.optlen);
    *optlen = p_rsp->m_data.m_getsockopt.optlen;

back:

    ret = aos_rpc_close(rpc_handle);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]os_rpc_close error: %d\n\r", __FUNCTION__, ret));
    }

    if (NULL != p_rsp)
    {
        ret = p_rsp->m_data.m_getsockopt.status;
        free(p_rsp);
    }

    return ret;
}

int vfs_st_setsockopt(int st, int level, int optname, const void *optval, socklen_t optlen)
{

    struct aos_parcel_t reply;
    u_socket_request_t rqst;
    u_socket_reponse_t rsps;
    char m_st_rpc_name[SOCKET_RPC_NAME_SIZE] = {0};
    size_t len;
    int rpc_handle;
    int ret;

    rsps.m_data.m_setsockopt.status = -1;
    rqst.m_evt = EVT_ST_SETSOCKOPT;
    rqst.m_len = sizeof(rqst) + optlen;
    rqst.m_data.m_setsockopt.st = st;
    rqst.m_data.m_setsockopt.level = level;
    rqst.m_data.m_setsockopt.optname = optname;
    rqst.m_data.m_setsockopt.optlen = optlen;

    /* get name of ulwip rpc service */
    ret = snprintf(m_st_rpc_name, sizeof(m_st_rpc_name), USOCKET_RPC_NAME_FORMAT, st);
    if (0 > ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]get %s name failed, ret = %d\n\r", __FUNCTION__, m_st_rpc_name, ret));
        return AOS_FAILED;
    }

    ret = aos_rpc_open_sync(m_st_rpc_name, ULWIP_RPC_BUFFER_SIZE, &rpc_handle);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]open rpc:%s fail(%d)\n\r", __FUNCTION__, m_st_rpc_name, ret));
        return AOS_FAILED;
    }

    ret = aos_rpc_call_sync(rpc_handle, &reply, 2, sizeof(rqst), &rqst, optlen, optval);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]aos_rpc_call_sync error: %d\n\r", __FUNCTION__, ret));
        goto back;
    }
    /* parse response */
    len = sizeof(rsps);
    ret = aos_parcel_get(AP_BUF, &rsps, &len, &reply);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]aos_parcel_get error: %d\n\r", __FUNCTION__, ret));
        goto back;
    }
    LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]get response len = %d, status = %d\n\r", __FUNCTION__, rsps.m_len, rsps.m_data.m_setsockopt.status));

back:

    ret = aos_rpc_close(rpc_handle);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]aos_rpc_close error: %d\n\r", __FUNCTION__, ret));
        return AOS_FAILED;
    }

    return rsps.m_data.m_setsockopt.status;
}

int vfs_st_fcntl(int st, int cmd, int val)
{
    struct aos_parcel_t reply;
    u_socket_request_t rqst;
    u_socket_reponse_t rsps;
    char m_st_rpc_name[SOCKET_RPC_NAME_SIZE] = {0};
    size_t len;
    int rpc_handle;
    int ret;

    rsps.m_data.m_fcntl.status = -1;
    rqst.m_evt = EVT_ST_FCNTL;
    rqst.m_len = sizeof(rqst);
    rqst.m_data.m_fcntl.st = st;
    rqst.m_data.m_fcntl.cmd = cmd;
    rqst.m_data.m_fcntl.val = val;

    /* get name of ulwip rpc service */
    ret = snprintf(m_st_rpc_name, sizeof(m_st_rpc_name), USOCKET_RPC_NAME_FORMAT, st);
    if (0 > ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]get %s name failed, ret = %d\n\r", __FUNCTION__, m_st_rpc_name, ret));
        return AOS_FAILED;
    }

    ret = aos_rpc_open_sync(m_st_rpc_name, ULWIP_RPC_BUFFER_SIZE, &rpc_handle);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]open rpc:%s fail(%d)\n\r", __FUNCTION__, m_st_rpc_name, ret));
        return AOS_FAILED;
    }

    ret = aos_rpc_call_sync(rpc_handle, &reply, 1, sizeof(rqst), &rqst);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]aos_rpc_call_sync error: %d\n\r", __FUNCTION__, ret));
        goto back;
    }

    /* parse response */
    len = sizeof(rsps);
    ret = aos_parcel_get(AP_BUF, &rsps, &len, &reply);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]aos_parcel_get error: %d\n\r", __FUNCTION__, ret));
        goto back;
    }
    LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]get response len = %d, status = %d\n\r", __FUNCTION__, rsps.m_len, rsps.m_data.m_setsockopt.status));

back:

    ret = aos_rpc_close(rpc_handle);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]aos_rpc_close error: %d\n\r", __FUNCTION__, ret));
        return AOS_FAILED;
    }

    return rsps.m_data.m_fcntl.status;
}

int vfs_st_shutdown(int st, int how)
{
    struct aos_parcel_t reply;
    u_socket_request_t rqst;
    u_socket_reponse_t rsps;
    char m_st_rpc_name[SOCKET_RPC_NAME_SIZE] = {0};
    size_t len;
    int rpc_handle;
    int ret;

    rsps.m_data.m_shutdown.status = -1;
    rqst.m_evt = EVT_ST_SHUTDOWN;
    rqst.m_len = sizeof(rqst);
    rqst.m_data.m_shutdown.st = st;
    rqst.m_data.m_shutdown.how = how;

    /* get name of ulwip rpc service */
    ret = snprintf(m_st_rpc_name, sizeof(m_st_rpc_name), USOCKET_RPC_NAME_FORMAT, st);
    if (0 > ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]get %s name failed, ret = %d\n\r", __FUNCTION__, m_st_rpc_name, ret));
        return AOS_FAILED;
    }

    ret = aos_rpc_open_sync(m_st_rpc_name, ULWIP_RPC_BUFFER_SIZE, &rpc_handle);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]open rpc:%s fail(%d)\n\r", __FUNCTION__, m_st_rpc_name, ret));
        return AOS_FAILED;
    }

    ret = aos_rpc_call_sync(rpc_handle, &reply, 1, sizeof(rqst), &rqst);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]aos_rpc_call_sync error: %d\n\r", __FUNCTION__, ret));
        goto back;
    }

    /* parse response */
    len = sizeof(rsps);
    ret = aos_parcel_get(AP_BUF, &rsps, &len, &reply);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]aos_parcel_get error: %d\n\r", __FUNCTION__, ret));
        goto back;
    }
    LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]get response len = %d, status = %d\n\r", __FUNCTION__, rsps.m_len, rsps.m_data.m_setsockopt.status));

back:

    ret = aos_rpc_close(rpc_handle);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]aos_rpc_close error: %d\n\r", __FUNCTION__, ret));
        return AOS_FAILED;
    }

    return rsps.m_data.m_shutdown.status;
}

static int vfs_st_select_alloc(int *handle)
{
    struct aos_parcel_t reply;
    u_socket_request_t rqst;
    u_socket_reponse_t rsps;
    size_t len;
    int rpc_handle;
    int ret;

    if (NULL == handle)
    {
        return AOS_FAILED;
    }

    rsps.m_data.m_select_alloc.id = -1;
    rqst.m_evt = EVT_ST_SELECT_ALLOC;
    rqst.m_len = sizeof(rqst);
    //  printk("#########open[0x%x][%s][%d]\n\r", aos_task_now(), __FUNCTION__, __LINE__);
    ret = aos_rpc_open_sync(USELECT_RPC_MAIN_NAME, ULWIP_RPC_BUFFER_SIZE, &rpc_handle);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]open rpc:%s fail(%d)\n\r", __FUNCTION__, USELECT_RPC_MAIN_NAME, ret));
        return AOS_FAILED;
    }

    ret = aos_rpc_call_sync(rpc_handle, &reply, 1, sizeof(rqst), &rqst);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]aos_rpc_call_sync error: %d\n\r", __FUNCTION__, ret));
        goto back;
    }

    /* parse response */
    len = sizeof(rsps);
    ret = aos_parcel_get(AP_BUF, &rsps, &len, &reply);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]aos_parcel_get error: %d\n\r", __FUNCTION__, ret));
        goto back;
    }
    LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]get response len = %d, status = %d\n\r", __FUNCTION__, rsps.m_len, rsps.m_data.m_select_alloc.id));
    *handle = rsps.m_data.m_select_alloc.rpc_handle;
back:

    ret = aos_rpc_close(rpc_handle);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]aos_rpc_close error: %d\n\r", __FUNCTION__, ret));
        return AOS_FAILED;
    }

    return rsps.m_data.m_select_alloc.id;
}

static int vfs_st_select_free(int id, int out)
{
    struct aos_parcel_t reply;
    u_socket_request_t rqst;
    u_socket_reponse_t rsps;
    size_t len;
    int rpc_handle;
    int ret;

    rsps.m_data.m_select_free.status = -1;
    rqst.m_evt = EVT_ST_SELECT_FREE;
    rqst.m_len = sizeof(rqst);
    rqst.m_data.m_select_free.id = id;
    rqst.m_data.m_select_free.rpc_handle = out;

    ret = aos_rpc_open_sync(USELECT_RPC_MAIN_NAME, ULWIP_RPC_BUFFER_SIZE, &rpc_handle);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]open rpc:%s fail(%d)\n\r", __FUNCTION__, USELECT_RPC_MAIN_NAME, ret));
        return AOS_FAILED;
    }

    ret = aos_rpc_call_sync(rpc_handle, &reply, 1, sizeof(rqst), &rqst);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]aos_rpc_call_sync error: %d\n\r", __FUNCTION__, ret));
        goto back;
    }

    /* parse response */
    len = sizeof(rsps);
    ret = aos_parcel_get(AP_BUF, &rsps, &len, &reply);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]aos_parcel_get error: %d\n\r", __FUNCTION__, ret));
        goto back;
    }
    LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]%s get response len = %d, id = %d\n\r", __FUNCTION__, aos_task_name(), rsps.m_len, rsps.m_data.m_select_free.status));

back:

    ret = aos_rpc_close(rpc_handle);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]aos_rpc_close error: %d\n\r", __FUNCTION__, ret));
        return AOS_FAILED;
    }

    return rsps.m_data.m_select_free.status;
}

int vfs_st_select(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset, struct timeval *timeout)
{
    struct aos_parcel_t reply;
    u_socket_request_t rqst;
    u_socket_reponse_t rsps;
    size_t len;
    int rpc_handle;
    int ret;
#ifdef USELECT_MULTITASK_SUPPORT
    char m_st_rpc_name[SOCKET_RPC_NAME_SIZE] = {0};
    int out = 0;
    int fd;

    fd = vfs_st_select_alloc(&out);
    if (0 > fd)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]get select failed(%d)\n\r", __FUNCTION__, fd));
        return AOS_FAILED;
    }
#endif
    rsps.m_data.m_select.nready = 0;
    rqst.m_evt = EVT_ST_SELECT;
    rqst.m_len = sizeof(rqst) + sizeof(*readset) + sizeof(*writeset) + sizeof(*exceptset) + sizeof(*timeout);
    rqst.m_data.m_select.maxfdp1 = maxfdp1;
#ifdef USELECT_MULTITASK_SUPPORT
    /* get name of ulwip rpc service */
    ret = snprintf(m_st_rpc_name, sizeof(m_st_rpc_name), USELECT_RPC_NAME_FORMAT, fd);
    if (0 > ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]get %s name failed, ret = %d\n\r", __FUNCTION__, m_st_rpc_name, ret));
        vfs_st_select_free(fd, out);
        return AOS_FAILED;
    }

    ret = aos_rpc_open_sync(m_st_rpc_name, ULWIP_RPC_BUFFER_SIZE, &rpc_handle);
#else
    ret = aos_rpc_open_sync(USELECT_RPC_MAIN_NAME, ULWIP_RPC_BUFFER_SIZE, &rpc_handle);
#endif
    if (AOS_SUCCESS != ret)
    {
#ifdef USELECT_MULTITASK_SUPPORT
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]open rpc:%s fail(%d)\n\r", __FUNCTION__, m_st_rpc_name, ret));
        vfs_st_select_free(fd, out);
#else
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]open rpc:%s fail(%d)\n\r", __FUNCTION__, USELECT_RPC_MAIN_NAME, ret));
#endif
        return AOS_FAILED;
    }

    ret = aos_rpc_call_sync(rpc_handle, &reply, 5, sizeof(rqst), &rqst, sizeof(fd_set), readset, sizeof(fd_set), writeset, sizeof(fd_set), exceptset, sizeof(*timeout), timeout);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]aos_rpc_call_sync error: %d\n\r", __FUNCTION__, ret));
        goto back;
    }

    /* parse response */
    len = sizeof(rsps);
    ret = aos_parcel_get(AP_BUF, &rsps, &len, &reply);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]aos_parcel_get error: %d\n\r", __FUNCTION__, ret));
        goto back;
    }
    LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]get response len = %d, status = %d\n\r", __FUNCTION__, rsps.m_len, rsps.m_data.m_select.nready));

back:

    ret = aos_rpc_close(rpc_handle);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]aos_rpc_close error: %d\n\r", __FUNCTION__, ret));
#ifdef USELECT_MULTITASK_SUPPORT
        vfs_st_select_free(fd, out);
#endif
        return AOS_FAILED;
    }
#ifdef USELECT_MULTITASK_SUPPORT
    vfs_st_select_free(fd, out);
#endif
    return rsps.m_data.m_select.nready;
}

int vfs_st_getaddrinfo(const char *nodename, const char *servname, const struct addrinfo *hints, struct addrinfo **res)
{
    struct aos_parcel_t reply;
    u_socket_request_t rqst;
    u_socket_reponse_t rsps;
    size_t len;
    struct addrinfo *ai;
    struct sockaddr_storage *sa = NULL;
    size_t total_size;
    size_t namelen = 0;
    int rpc_handle;
    int ret;
#ifdef USELECT_MULTITASK_SUPPORT
    char m_st_rpc_name[SOCKET_RPC_NAME_SIZE] = {0};
    int out = 0;
    int fd;

    fd = vfs_st_select_alloc(&out);
    if (0 > fd)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]get select failed(%d)\n\r", __FUNCTION__, fd));
        return AOS_FAILED;
    }
#endif
    if (res == NULL)
    {
        return AOS_FAILED;
    }

    rsps.m_data.m_getaddrinfo.status = -1;
    rqst.m_evt = EVT_ST_GETADDRINFO;
    rqst.m_len = sizeof(rqst);
    rqst.m_data.m_getaddrinfo.nodename_len = -1;
    rqst.m_data.m_getaddrinfo.servname_len = -1;

    if (nodename != NULL)
    {
        rqst.m_data.m_getaddrinfo.nodename_len = strlen(nodename);

        if (rqst.m_data.m_getaddrinfo.nodename_len > DNS_MAX_NAME_LENGTH)
        {
            /* invalid name length */
            return AOS_FAILED;
        }
        rqst.m_len += rqst.m_data.m_getaddrinfo.nodename_len;
        namelen = rqst.m_data.m_getaddrinfo.nodename_len;
    }

    if (servname != NULL)
    {
        rqst.m_data.m_getaddrinfo.servname_len = strlen(servname);
        rqst.m_len += rqst.m_data.m_getaddrinfo.servname_len;
    }

    if (hints != NULL)
    {
        rqst.m_len += sizeof(struct addrinfo);
    }

    if ((NULL != nodename) && (NULL != servname))
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]nodemane[%d] (%s), severname[%d](%s)\r\n", __FUNCTION__, strlen(nodename), nodename, strlen(servname), servname));
    }
#ifdef USELECT_MULTITASK_SUPPORT
    /* get name of ulwip rpc service */
    ret = snprintf(m_st_rpc_name, sizeof(m_st_rpc_name), USELECT_RPC_NAME_FORMAT, fd);
    if (0 > ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]get %s name failed, ret = %d\n\r", __FUNCTION__, m_st_rpc_name, ret));
        vfs_st_select_free(fd, out);
        return AOS_FAILED;
    }

    ret = aos_rpc_open_sync(m_st_rpc_name, ULWIP_RPC_BUFFER_SIZE, &rpc_handle);
#else
    ret = aos_rpc_open_sync(ULWIP_RPC_MAIN_NAME, ULWIP_RPC_BUFFER_SIZE, &rpc_handle);
#endif
    if (AOS_SUCCESS != ret)
    {
#ifdef USELECT_MULTITASK_SUPPORT
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]open rpc:%s fail(%d)\n\r", __FUNCTION__, m_st_rpc_name, ret));
        vfs_st_select_free(fd, out);
#else
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]open rpc:%s fail(%d)\n\r", __FUNCTION__, USELECT_RPC_MAIN_NAME, ret));
#endif
        return AOS_FAILED;
    }

    ret = aos_rpc_call_sync(rpc_handle, &reply, 4, sizeof(rqst), &rqst, rqst.m_data.m_getaddrinfo.nodename_len, nodename, rqst.m_data.m_getaddrinfo.servname_len, servname, sizeof(struct addrinfo), hints);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]aos_rpc_call_sync error: %d\n\r", __FUNCTION__, ret));
        goto back;
    }

    /* parse response */
    len = sizeof(rsps);
    ret = aos_parcel_get(AP_BUF, &rsps, &len, &reply);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]aos_parcel_get error: %d\n\r", __FUNCTION__, ret));
        goto back;
    }
    if (0 != rsps.m_data.m_getaddrinfo.status)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]getaddrinfo failed\n\r", __FUNCTION__));
        goto back;
    }

    LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]get response len = %d, status = %d\n\r", __FUNCTION__, rsps.m_len, rsps.m_data.m_select.nready));

    total_size = sizeof(struct addrinfo) + sizeof(struct sockaddr_storage);
    total_size += (namelen + 1);

    ai = (struct addrinfo *)malloc(total_size);
    if (ai == NULL)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]malloc %d bytes failed error\n\r", __FUNCTION__, total_size));
        goto back;
    }

    memset(ai, 0, total_size);

    ai->ai_family = rsps.m_data.m_getaddrinfo.ai_family;

    sa = (struct sockaddr_storage *)(void *)((u8_t *)ai + sizeof(struct addrinfo));
    memcpy(sa, &(rsps.m_data.m_getaddrinfo.sa), sizeof(struct sockaddr_storage));
    /* set up addrinfo */
    if (hints != NULL)
    {
        /* copy socktype & protocol from hints if specified */
        ai->ai_socktype = hints->ai_socktype;
        ai->ai_protocol = hints->ai_protocol;
    }
    if (nodename != NULL)
    {
        /* copy nodename to canonname if specified */
        ai->ai_canonname = ((char *)ai + sizeof(struct addrinfo) + sizeof(struct sockaddr_storage));
        memcpy(ai->ai_canonname, nodename, namelen);
        ai->ai_canonname[namelen] = 0;
    }
    ai->ai_addrlen = rsps.m_data.m_getaddrinfo.ai_addrlen;
    ai->ai_addr = (struct sockaddr *)sa;

    *res = ai;

back:

    ret = aos_rpc_close(rpc_handle);
    if (AOS_SUCCESS != ret)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]aos_rpc_close error: %d\n\r", __FUNCTION__, ret));
#ifdef USELECT_MULTITASK_SUPPORT
        vfs_st_select_free(fd, out);
#endif
        return AOS_FAILED;
    }
#ifdef USELECT_MULTITASK_SUPPORT
    vfs_st_select_free(fd, out);
#endif
    return rsps.m_data.m_getaddrinfo.status;
}

void vfs_st_freeaddrinfo(struct addrinfo *ai)
{
    if (NULL != ai)
    {
        free(ai);
    }
}