/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef LWIP_RPC_SOCKET_H
#define LWIP_RPC_SOCKET_H

#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include "sys/socket.h"
#include "netdb.h"
#include "lwip/opt.h"
#include <rpc.h>

#define AOS_SUCCESS (0)
#define AOS_FAILED (-1)

#define USOCKET_RPC_NAME_FORMAT "rpc-st-%d" //!< rpc name format for socket service
#define USOCKET_PARAM_MAX_LEN 64            //!< max length of socket service parameter without append data for big data transfer

/**
 * socket Event Identification for RPC request
 *
 * */
typedef enum u_socket_evtid
{
    EVT_ST_SOCKET = 0,  //!< 0 socket event
    EVT_ST_ACCEPT,      //!< 1 accept event
    EVT_ST_BIND,        //!< 2 bind event
    EVT_ST_CONNECT,     //!< 3 connet event
    EVT_ST_LISTEN,      //!< 4 listen event
    EVT_ST_RECV,        //!< 5 recv event
    EVT_ST_RECVFROM,    //!< 6 recvfrom event
    EVT_ST_SEND,        //!< 7 send event
    EVT_ST_SENDTO,      //!< 8 sendto event
    EVT_ST_GETSOCKOPT,  //!< 9 getsockopt event
    EVT_ST_SETSOCKOPT,  //!< 10 setsockopt event
    EVT_ST_CLOSE,       //!< 11 close event
    EVT_ST_FCNTL,       //!< 12 fcntl event
    EVT_ST_SELECT,      //!< 13 select event
    EVT_ST_GETADDRINFO, //!< 14 getaddrinfo event
    EVT_ST_SHUTDOWN,    //!< 15 shutdown event
    EVT_ST_SELECT_ALLOC,//!< 16 select alloc event
    EVT_ST_SELECT_FREE, //!< 17 select free event
    EVT_ST_MAX          //!< max number of socket event array
} u_socket_evtid_t;

/**
 * socket parameter for RPC request
 *
 * */
typedef struct
{
    int domain;
    int type;
    int protocol;
} socket_in_t;

/**
 * socket parameter for RPC response
 *
 * */
typedef struct
{
    int st;
} socket_out_t;

/**
 * accept parameter for RPC request
 *
 * */
typedef struct
{
    int st;
} accept_in_t;

/**
 * accept parameter for RPC response
 *
 * */
typedef struct
{
    int newsock;
    socklen_t addrlen;
} accept_out_t;

/**
 * bind parameter for RPC request
 *
 * */
typedef struct
{
    int st;
    socklen_t namelen;
} bind_in_t;

/**
 * bind parameter for RPC response
 *
 * */
typedef struct
{
    int status;
} bind_out_t;

/**
 * connect parameter for RPC request
 *
 * */
typedef struct
{
    int st;
    socklen_t namelen;
} connect_in_t;

/**
 * connect parameter for RPC response
 *
 * */
typedef struct
{
    int status;
} connect_out_t;

/**
 * listen parameter for RPC request
 *
 * */
typedef struct
{
    int st;
    int backlog;
} listen_in_t;

/**
 * listen parameter for RPC response
 *
 * */
typedef struct
{
    int status;
} listen_out_t;

/**
 * recv parameter for RPC request
 *
 * */
typedef struct
{
    int st;
    size_t len;
    int flags;
} recv_in_t;

/**
 * recv parameter for RPC response
 *
 * */
typedef struct
{
    int off;
} recv_out_t;

/**
 * recvfrom parameter for RPC request
 *
 * */
typedef struct
{
    int st;
    size_t len;
    int flags;
} recvfrom_in_t;

/**
 * recvfrom parameter for RPC response
 *
 * */
typedef struct
{
    int off;
    socklen_t fromlen;
    struct sockaddr_in from;
} recvfrom_out_t;

/**
 * send parameter for RPC request
 *
 * */
typedef struct
{
    int st;
    size_t size;
    int flags;
} send_in_t;

/**
 * send parameter for RPC response
 *
 * */
typedef struct
{
    int written;
} send_out_t;

/**
 * sendto parameter for RPC request
 *
 * */
typedef struct
{
    int st;
    size_t size;
    int flags;
    socklen_t tolen;
    struct sockaddr_in to;
} sendto_in_t;

/**
 * sendto parameter for RPC response
 *
 * */
typedef struct
{
    int written;
} sendto_out_t;

/**
 * getsockopt parameter for RPC request
 *
 * */
typedef struct
{
    int st;
    int level;
    int optname;
} getsockopt_in_t;

/**
 * getsockopt parameter for RPC response
 *
 * */
typedef struct
{
    int status;
    socklen_t optlen;
} getsockopt_out_t;

/**
 * setsockopt parameter for RPC request
 *
 * */
typedef struct
{
    int st;
    int level;
    int optname;
    socklen_t optlen;
} setsockopt_in_t;

/**
 * setsockopt parameter for RPC response
 *
 * */
typedef struct
{
    int status;
} setsockopt_out_t;

/**
 * close parameter for RPC request
 *
 * */
typedef struct
{
    int st;
} close_in_t;

/**
 * close parameter for RPC response
 *
 * */
typedef struct
{
    int status;
} close_out_t;

/**
 * fcntl parameter for RPC request
 *
 * */
typedef struct
{
    int st;
    int cmd;
    int val;
} fcntl_in_t;

/**
 * fcntl parameter for RPC response
 *
 * */
typedef struct
{
    int status;
} fcntl_out_t;

/**
 * select parameter for RPC request
 *
 * */
typedef struct
{
    int id;
    int maxfdp1;
} select_in_t;

/**
 * select parameter for RPC response
 *
 * */
typedef struct
{
    int nready;
} select_out_t;

/**
 * select parameter for RPC request
 *
 * */
typedef struct
{
    int dummy;
} select_alloc_in_t;

/**
 * select parameter for RPC response
 *
 * */
typedef struct
{
    int id;
    int rpc_handle;
} select_alloc_out_t;

/**
 * select close parameter for RPC response
 *
 * */
typedef struct
{
    int status;
} select_free_out_t;

/**
 * select close parameter for RPC request
 *
 * */
typedef struct
{
    int id;
    int rpc_handle;
} select_free_in_t;

/**
 * getaddrinfo parameter for RPC request
 *
 * */
typedef struct
{
    int nodename_len;
    int servname_len;
} getaddrinfo_in_t;

/**
 * getaddrinfo parameter for RPC response
 *
 * */
typedef struct
{
    int status;
    int ai_family; /* Address family of socket. */
    socklen_t ai_addrlen;
    struct sockaddr_storage sa;
} getaddrinfo_out_t;

/**
 * shutdown parameter for RPC response
 *
 * */
typedef struct
{
    int status;
} shutdown_out_t;

/**
 * shutdown parameter for RPC request
 *
 * */
typedef struct
{
    int st;
    int how;
} shutdown_in_t;

/**
 * the input parameter of socket interface for RPC request
 *
 * */
typedef union
{
    socket_in_t m_socket;
    accept_in_t m_accept;
    bind_in_t m_bind;
    listen_in_t m_listen;
    connect_in_t m_connect;
    recv_in_t m_recv;
    recvfrom_in_t m_recvfrom;
    send_in_t m_send;
    sendto_in_t m_sendto;
    getsockopt_in_t m_getsockopt;
    setsockopt_in_t m_setsockopt;
    close_in_t m_close;
    fcntl_in_t m_fcntl;
    select_alloc_in_t m_select_alloc;
    select_in_t m_select;
    select_free_in_t m_select_free;
    getaddrinfo_in_t m_getaddrinfo;
    shutdown_in_t m_shutdown;
} u_socket_in_t;

/**
 * the output parameter of socket interface
 *
 * */
typedef union u_socket_output
{
    socket_out_t m_socket;
    accept_out_t m_accept;
    bind_out_t m_bind;
    listen_out_t m_listen;
    connect_out_t m_connect;
    recv_out_t m_recv;
    recvfrom_out_t m_recvfrom;
    send_out_t m_send;
    sendto_out_t m_sendto;
    getsockopt_out_t m_getsockopt;
    setsockopt_out_t m_setsockopt;
    close_out_t m_close;
    fcntl_out_t m_fcntl;
    select_alloc_out_t m_select_alloc;
    select_out_t m_select;
    select_free_out_t m_select_free;
    getaddrinfo_out_t m_getaddrinfo;
    shutdown_out_t m_shutdown;
} u_socket_out_t;

/**
 * the parameter of socket interface for RPC request
 *
 * */
typedef struct u_socket_request
{
    u_socket_evtid_t m_evt;
    size_t m_len;
    u_socket_in_t m_data;
    const char append[0]; //!< for big data transfer
} u_socket_request_t;

/**
 * the parameter of socket interface for RPC response
 *
 * */
typedef struct u_socket_response
{
    u_socket_evtid_t m_evt;
    size_t m_len;
    u_socket_out_t m_data;
    char append[0]; //!< for big data transfer
} u_socket_reponse_t;

/**
 * the function point of socket evnet proccess handler
 *
 * */
typedef void (*u_socket_func)(u_socket_request_t *p_in, u_socket_reponse_t **p_out);

/**
 * the function point array of socket evnet proccess handler
 *
 * */
extern u_socket_func m_socket_opt[EVT_ST_MAX];

#endif /* LWIP_RPC_SOCKET_H */
