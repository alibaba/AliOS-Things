/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <string.h>

#include "iot_import.h"
#include "utils_net.h"
#include "lite-log.h"

static int tcp_fd=-1;
int get_tcp_fd()
{
    return tcp_fd;
}
/*** TCP connection ***/
int  read_tcp(utils_network_pt pNetwork, char *buffer, uint32_t len, uint32_t timeout_ms)
{
    return HAL_TCP_Read(pNetwork->handle, buffer, len, timeout_ms);
}


static int write_tcp(utils_network_pt pNetwork, const char *buffer, uint32_t len, uint32_t timeout_ms)
{
    return HAL_TCP_Write(pNetwork->handle, buffer, len, timeout_ms);
}

static int disconnect_tcp(utils_network_pt pNetwork)
{
    if (-1 == pNetwork->handle) {
        return -1;
    }

    HAL_TCP_Destroy(pNetwork->handle);
    pNetwork->handle = -1;
    return 0;
}


static int connect_tcp(utils_network_pt pNetwork)
{
    if (NULL == pNetwork) {
        log_err("network is null");
        return 1;
    }

    pNetwork->handle = HAL_TCP_Establish(pNetwork->pHostAddress, pNetwork->port);
    tcp_fd=pNetwork->handle;
    if (-1 == pNetwork->handle) {
        return -1;
    }

    return 0;
}


/*** SSL connection ***/
#ifndef IOTX_WITHOUT_TLS
static int read_ssl(utils_network_pt pNetwork, char *buffer, uint32_t len, uint32_t timeout_ms)
{
    if (NULL == pNetwork) {
        log_err("network is null");
        return -1;
    }

    return HAL_SSL_Read((uintptr_t)pNetwork->handle, buffer, len, timeout_ms);
}

static int write_ssl(utils_network_pt pNetwork, const char *buffer, uint32_t len, uint32_t timeout_ms)
{
    if (NULL == pNetwork) {
        log_err("network is null");
        return -1;
    }

    return HAL_SSL_Write((uintptr_t)pNetwork->handle, buffer, len, timeout_ms);
}

static int disconnect_ssl(utils_network_pt pNetwork)
{
    if (NULL == pNetwork) {
        log_err("network is null");
        return -1;
    }

    HAL_SSL_Destroy((uintptr_t)pNetwork->handle);
    pNetwork->handle = 0;

    return 0;
}

static int connect_ssl(utils_network_pt pNetwork)
{
    if (NULL == pNetwork) {
        log_err("network is null");
        return 1;
    }

    if (0 != (pNetwork->handle = (intptr_t)HAL_SSL_Establish(
                                     pNetwork->pHostAddress,
                                     pNetwork->port,
                                     pNetwork->ca_crt,
                                     pNetwork->ca_crt_len + 1))) {
        return 0;
    } else {
        //TODO SHOLUD not remove this handle space
        // The space will be freed by calling disconnect_ssl()
        //utils_memory_free((void *)pNetwork->handle);
        return -1;
    }
}
#endif  /* #ifndef IOTX_WITHOUT_TLS */


/****** network interface ******/

int utils_net_read(utils_network_pt pNetwork, char *buffer, uint32_t len, uint32_t timeout_ms)
{

#ifdef IOTX_WITHOUT_TLS
    return read_tcp(pNetwork, buffer, len, timeout_ms);
#else
    return read_ssl(pNetwork, buffer, len, timeout_ms);
#endif
    

}


int utils_net_write(utils_network_pt pNetwork, const char *buffer, uint32_t len, uint32_t timeout_ms)
{
#ifdef IOTX_WITHOUT_TLS
    return write_tcp(pNetwork, buffer, len, timeout_ms);
#else     

    return write_ssl(pNetwork, buffer, len, timeout_ms);
#endif
    
}


int iotx_net_disconnect(utils_network_pt pNetwork)
{
    if (NULL == pNetwork->ca_crt) { //TCP connection
        return disconnect_tcp(pNetwork);
#ifndef IOTX_WITHOUT_TLS
    } else { //SSL connection
        return disconnect_ssl(pNetwork);
#endif
    }
}
#ifndef IOTX_WITHOUT_TLS
extern int get_ssl_fd();
#endif

static int iotx_fd=-1;
int get_iotx_fd(){
#ifdef IOTX_WITHOUT_TLS

    return get_tcp_fd();
#else    
    return get_ssl_fd();
#endif    
}

#include <aos/log.h>
int iotx_net_connect(utils_network_pt pNetwork)
{
    if (NULL == pNetwork->ca_crt) { //TCP connection
        return connect_tcp(pNetwork);
#ifndef IOTX_WITHOUT_TLS
    } else { //SSL connection
        return connect_ssl(pNetwork);
#endif
    }
}


int iotx_net_init(utils_network_pt pNetwork, const char *host, uint16_t port, const char *ca_crt)
{
    if (!pNetwork || !host) {
        log_err("parameter error! pNetwork=%p, host = %p", pNetwork, host);
        return -1;
    }
    
    pNetwork->pHostAddress = host;
    pNetwork->port = port;
    pNetwork->ca_crt = ca_crt;

    if (NULL == ca_crt) {
        pNetwork->ca_crt_len = 0;
    } else {
        pNetwork->ca_crt_len = strlen(ca_crt);
    }

    pNetwork->handle = -1;
    pNetwork->read = utils_net_read;
    pNetwork->write = utils_net_write;
    pNetwork->disconnect = iotx_net_disconnect;
    pNetwork->connect = iotx_net_connect;

    return 0;
}
