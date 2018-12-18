/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#include <string.h>

#include "iot_import.h"
#include "iot_export.h"
#include "iotx_utils.h"
#include "utils_net.h"
#include "iotx_utils_internal.h"


/*** SSL connection ***/
#ifdef  SUPPORT_TLS
static void *ssl_malloc(uint32_t size)
{
    return LITE_malloc(size, MEM_MAGIC, "tls");
}
static void ssl_free(void *ptr)
{
    LITE_free(ptr);
}
#endif

#if  defined(SUPPORT_TLS) || defined(SUPPORT_ITLS)
static int read_ssl(utils_network_pt pNetwork, char *buffer, uint32_t len, uint32_t timeout_ms)
{
    if (NULL == pNetwork) {
        utils_err("network is null");
        return -1;
    }

    return HAL_SSL_Read((uintptr_t)pNetwork->handle, buffer, len, timeout_ms);
}

static int write_ssl(utils_network_pt pNetwork, const char *buffer, uint32_t len, uint32_t timeout_ms)
{
    if (NULL == pNetwork) {
        utils_err("network is null");
        return -1;
    }

    return HAL_SSL_Write((uintptr_t)pNetwork->handle, buffer, len, timeout_ms);
}

static int disconnect_ssl(utils_network_pt pNetwork)
{
    if (NULL == pNetwork) {
        utils_err("network is null");
        return -1;
    }

    HAL_SSL_Destroy((uintptr_t)pNetwork->handle);
    pNetwork->handle = 0;

    return 0;
}

static int connect_ssl(utils_network_pt pNetwork)
{

    if (NULL == pNetwork) {
        utils_err("network is null");
        return 1;
    }

#if defined(SUPPORT_ITLS)
    char pkps[PRODUCT_KEY_LEN + PRODUCT_SECRET_LEN + 3] = {0};

    HAL_GetProductKey(pkps);
    int len = strlen(pkps);
    HAL_GetProductSecret(pkps + len + 1);
    len += strlen(pkps + len + 1) + 2;

    if (0 != (pNetwork->handle = (intptr_t)HAL_SSL_Establish(
            pNetwork->pHostAddress,
            pNetwork->port,
            pkps, len))) {
        return 0;
    }
#else
    ssl_hooks_t ssl_hooks;
    memset(&ssl_hooks, 0, sizeof(ssl_hooks_t));
    ssl_hooks.malloc = ssl_malloc;
    ssl_hooks.free = ssl_free;

    HAL_SSLHooks_set(&ssl_hooks);

    if (0 != (pNetwork->handle = (intptr_t)HAL_SSL_Establish(
            pNetwork->pHostAddress,
            pNetwork->port,
            pNetwork->ca_crt,
            pNetwork->ca_crt_len + 1))) {
        return 0;
    }
#endif
    else {
        /* TODO SHOLUD not remove this handle space */
        /* The space will be freed by calling disconnect_ssl() */
        /* utils_memory_free((void *)pNetwork->handle); */
#ifdef UTILS_EVENT
        iotx_event_post(IOTX_CONN_CLOUD_FAIL);
#endif
        return -1;
    }
}
#else
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
        return -1;
    }

    HAL_TCP_Destroy(pNetwork->handle);
    pNetwork->handle = -1;
    return 0;
}

static int connect_tcp(utils_network_pt pNetwork)
{
    if (NULL == pNetwork) {
        utils_err("network is null");
        return 1;
    }

    pNetwork->handle = HAL_TCP_Establish(pNetwork->pHostAddress, pNetwork->port);
    if (pNetwork->handle == (uintptr_t)(-1)) {
        return -1;
    }

    return 0;
}
#endif  /* #ifdef SUPPORT_TLS */

/****** network interface ******/
int utils_net_read(utils_network_pt pNetwork, char *buffer, uint32_t len, uint32_t timeout_ms)
{
    int     ret = 0;
#ifdef SUPPORT_TLS
    if (NULL != pNetwork->ca_crt) {
        ret = read_ssl(pNetwork, buffer, len, timeout_ms);
    }
#else
    if (NULL == pNetwork->ca_crt) {
#ifdef SUPPORT_ITLS
        ret = read_ssl(pNetwork, buffer, len, timeout_ms);
#else
        ret = read_tcp(pNetwork, buffer, len, timeout_ms);
#endif
    }
#endif
    else {
        ret = -1;
        utils_err("no method match!");
    }

    return ret;
}

int utils_net_write(utils_network_pt pNetwork, const char *buffer, uint32_t len, uint32_t timeout_ms)
{
    int     ret = 0;
#ifdef SUPPORT_TLS
    if (NULL != pNetwork->ca_crt) {
        ret = write_ssl(pNetwork, buffer, len, timeout_ms);
    }
#else
    if (NULL == pNetwork->ca_crt) {
#ifdef SUPPORT_ITLS
        ret = write_ssl(pNetwork, buffer, len, timeout_ms);
#else
        ret = write_tcp(pNetwork, buffer, len, timeout_ms);
#endif
    }
#endif

    else {
        ret = -1;
        utils_err("no method match!");
    }

    return ret;
}

int iotx_net_disconnect(utils_network_pt pNetwork)
{
    int     ret = 0;
#ifdef SUPPORT_TLS
    if (NULL != pNetwork->ca_crt) {
        ret = disconnect_ssl(pNetwork);
    }
#else
    if (NULL == pNetwork->ca_crt) {
#ifdef SUPPORT_ITLS
        ret = disconnect_ssl(pNetwork);
#else
        ret = disconnect_tcp(pNetwork);
#endif
    }
#endif
    else {
        ret = -1;
        utils_err("no method match!");
    }

    return  ret;
}

int iotx_net_connect(utils_network_pt pNetwork)
{
    int     ret = 0;
#ifdef SUPPORT_TLS
    if (NULL != pNetwork->ca_crt) {
        ret = connect_ssl(pNetwork);
    }
#else
    if (NULL == pNetwork->ca_crt) {
#ifdef SUPPORT_ITLS
        ret = connect_ssl(pNetwork);
#else
        ret = connect_tcp(pNetwork);
#endif
    }
#endif
    else {
        ret = -1;
        utils_err("no method match!");
    }

    return ret;
}

int iotx_net_init(utils_network_pt pNetwork, const char *host, uint16_t port, const char *ca_crt)
{
    if (!pNetwork || !host) {
        utils_err("parameter error! pNetwork=%p, host = %p", pNetwork, host);
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

    pNetwork->handle = 0;
    pNetwork->read = utils_net_read;
    pNetwork->write = utils_net_write;
    pNetwork->disconnect = iotx_net_disconnect;
    pNetwork->connect = iotx_net_connect;

    return 0;
}
