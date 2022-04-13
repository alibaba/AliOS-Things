/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "infra_config.h"

#ifdef INFRA_NET
#include <stdio.h>
#include <string.h>
#include "infra_state.h"
#include "infra_defs.h"
#include "infra_net.h"
#include "wrappers_defs.h"
#include "wrappers.h"

#ifdef INFRA_LOG
    #include "infra_log.h"
    #define net_err(...)      log_err("infra_net", __VA_ARGS__)
#else
    #define net_err(...)
#endif

/*** SSL connection ***/
#ifdef INFRA_MEM_STATS
    #include "infra_mem_stats.h"
    #define NET_MALLOC(size) LITE_malloc(size, MEM_MAGIC, "infra_net")
    #define NET_FREE(ptr)    LITE_free(ptr)
    extern int ssl_hooks_set(ssl_hooks_t *hooks);
#else
    #define NET_MALLOC(size) HAL_Malloc(size)
    #define NET_FREE(ptr)    HAL_Free(ptr)
#endif

static void *ssl_malloc(uint32_t size)
{
    return NET_MALLOC(size);
}
static void ssl_free(void *ptr)
{
    NET_FREE(ptr);
}

static int read_ssl(utils_network_pt pNetwork, char *buffer, uint32_t len, uint32_t timeout_ms)
{
    if (NULL == pNetwork) {
        return STATE_SYS_DEPEND_NWK_INVALID_HANDLE;
    }

    return HAL_SSL_Read((uintptr_t)pNetwork->handle, buffer, len, timeout_ms);
}

static int write_ssl(utils_network_pt pNetwork, const char *buffer, uint32_t len, uint32_t timeout_ms)
{
    if (NULL == pNetwork) {
        return STATE_SYS_DEPEND_NWK_INVALID_HANDLE;
    }

    return HAL_SSL_Write((uintptr_t)pNetwork->handle, buffer, len, timeout_ms);
}

static int disconnect_ssl(utils_network_pt pNetwork)
{
    if (NULL == pNetwork) {
        return STATE_SYS_DEPEND_NWK_INVALID_HANDLE;
    }

    HAL_SSL_Destroy((uintptr_t)pNetwork->handle);
    pNetwork->handle = 0;

    return STATE_SUCCESS;
}

static int connect_ssl(utils_network_pt pNetwork)
{
    ssl_hooks_t ssl_hooks;

    if (NULL == pNetwork) {
        return STATE_SYS_DEPEND_NWK_INVALID_HANDLE;
    }

#ifdef INFRA_MEM_STATS
    memset(&ssl_hooks, 0, sizeof(ssl_hooks_t));
    ssl_hooks.malloc = ssl_malloc;
    ssl_hooks.free = ssl_free;

    ssl_hooks_set(&ssl_hooks);
#else
    (void)ssl_hooks;
#endif

    if (0 != (pNetwork->handle = (intptr_t)HAL_SSL_Establish(
            pNetwork->pHostAddress,
            pNetwork->port,
            pNetwork->ca_crt,
            pNetwork->ca_crt_len + 1))) {
        return STATE_SUCCESS;
    }
    else {
        /* TODO SHOLUD not remove this handle space */
        /* The space will be freed by calling disconnect_ssl() */
        /* utils_memory_free((void *)pNetwork->handle); */
        return STATE_SYS_DEPEND_NWK_INVALID_HANDLE;
    }
}

/*** TCP connection ***/
static int read_tcp(utils_network_pt pNetwork, char *buffer, uint32_t len, uint32_t timeout_ms)
{
    return HAL_TCP_Read(pNetwork->handle, buffer, len, timeout_ms);
}


static int write_tcp(utils_network_pt pNetwork, const char *buffer, uint32_t len, uint32_t timeout_ms)
{
    return HAL_TCP_Write(pNetwork->handle, buffer, len, timeout_ms);
}

static int disconnect_tcp(utils_network_pt pNetwork)
{
    if (pNetwork->handle == (uintptr_t)(-1)) {
        return STATE_SYS_DEPEND_NWK_INVALID_HANDLE;
    }

    HAL_TCP_Destroy(pNetwork->handle);
    pNetwork->handle = (uintptr_t)(-1);
    return STATE_SUCCESS;
}

static int connect_tcp(utils_network_pt pNetwork)
{
    if (NULL == pNetwork) {
        return STATE_SYS_DEPEND_NWK_INVALID_HANDLE;
    }

    pNetwork->handle = HAL_TCP_Establish(pNetwork->pHostAddress, pNetwork->port);
    if (pNetwork->handle == (uintptr_t)(-1)) {
        return STATE_SYS_DEPEND_NWK_INVALID_HANDLE;
    }

    return STATE_SUCCESS;
}

/****** network interface ******/
int utils_net_read(utils_network_pt pNetwork, char *buffer, uint32_t len, uint32_t timeout_ms)
{
    int ret = 0;
#if defined(SUPPORT_TLS)
    ret = read_ssl(pNetwork, buffer, len, timeout_ms);
#else
    ret = read_tcp(pNetwork, buffer, len, timeout_ms);
#endif

    return ret;
}

int utils_net_write(utils_network_pt pNetwork, const char *buffer, uint32_t len, uint32_t timeout_ms)
{
    int ret = 0;
#if defined(SUPPORT_TLS)
    ret = write_ssl(pNetwork, buffer, len, timeout_ms);
#else
    ret = write_tcp(pNetwork, buffer, len, timeout_ms);
#endif

    return ret;
}

int iotx_net_disconnect(utils_network_pt pNetwork)
{
    int ret = 0;
#if defined(SUPPORT_TLS)
    ret = disconnect_ssl(pNetwork);
#else
    ret = disconnect_tcp(pNetwork);
#endif

    return  ret;
}

int iotx_net_connect(utils_network_pt pNetwork)
{
    int ret = 0;
#if defined(SUPPORT_TLS)
    ret = connect_ssl(pNetwork);
#else
    ret = connect_tcp(pNetwork);
#endif

    return ret;
}

/****** internal network interface ******/
int internal_utils_net_read(utils_network_pt pNetwork, char *buffer, uint32_t len, uint32_t timeout_ms)
{
    int ret = 0;

    ret = read_ssl(pNetwork, buffer, len, timeout_ms);

    return ret;
}

int internal_utils_net_write(utils_network_pt pNetwork, const char *buffer, uint32_t len, uint32_t timeout_ms)
{
    int ret = 0;

    ret = write_ssl(pNetwork, buffer, len, timeout_ms);

    return ret;
}

int internal_iotx_net_disconnect(utils_network_pt pNetwork)
{
    int ret = 0;

    ret = disconnect_ssl(pNetwork);

    return  ret;
}

int internal_iotx_net_connect(utils_network_pt pNetwork)
{
    int ret = 0;

    ret = connect_ssl(pNetwork);

    return ret;
}

int iotx_net_init_internal(utils_network_pt pNetwork, const char *host, uint16_t port, const char *ca_crt)
{
    if (!pNetwork || !host) {
        return STATE_USER_INPUT_INVALID;
    }
    pNetwork->pHostAddress = host;
    pNetwork->port = port;
    pNetwork->ca_crt = ca_crt;

    if (NULL == ca_crt) {
        pNetwork->ca_crt_len = 0;
    } else {
        pNetwork->ca_crt_len = strlen(ca_crt);
    }

    pNetwork->handle = 0;
    pNetwork->read = internal_utils_net_read;
    pNetwork->write = internal_utils_net_write;
    pNetwork->disconnect = internal_iotx_net_disconnect;
    pNetwork->connect = internal_iotx_net_connect;

    return STATE_SUCCESS;
}

int iotx_net_init(utils_network_pt pNetwork, const char *host, uint16_t port, const char *ca_crt)
{
    if (!pNetwork || !host) {
        return STATE_USER_INPUT_INVALID;
    }
    pNetwork->pHostAddress = host;
    pNetwork->port = port;
    pNetwork->ca_crt = ca_crt;

    if (NULL == ca_crt) {
        pNetwork->ca_crt_len = 0;
    } else {
        pNetwork->ca_crt_len = strlen(ca_crt);
    }

    pNetwork->handle = 0;
    pNetwork->read = utils_net_read;
    pNetwork->write = utils_net_write;
    pNetwork->disconnect = iotx_net_disconnect;
    pNetwork->connect = iotx_net_connect;

    return STATE_SUCCESS;
}
#endif


