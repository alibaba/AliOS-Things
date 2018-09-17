/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#include <string.h>

#include "iot_import.h"
#include "iot_export.h"
#include "utils_net.h"
#include "iotx_utils_internal.h"

/*** TCP connection ***/
int read_tcp(utils_network_pt pNetwork, char *buffer, uint32_t len, uint32_t timeout_ms)
{
    return HAL_TCP_Read(pNetwork->handle, buffer, len, timeout_ms);
}


static int write_tcp(utils_network_pt pNetwork, const char *buffer, uint32_t len, uint32_t timeout_ms)
{
    return HAL_TCP_Write(pNetwork->handle, buffer, len, timeout_ms);
}

static int disconnect_tcp(utils_network_pt pNetwork)
{
    if (0 == pNetwork->handle) {
        return -1;
    }

    HAL_TCP_Destroy(pNetwork->handle);
    pNetwork->handle = 0;
    return 0;
}

static int connect_tcp(utils_network_pt pNetwork)
{
    if (NULL == pNetwork) {
        utils_err("network is null");
        return 1;
    }

    pNetwork->handle = HAL_TCP_Establish(pNetwork->pHostAddress, pNetwork->port);
    if (0 == pNetwork->handle) {
        return -1;
    }

    return 0;
}

/*** SSL connection ***/
#ifndef IOTX_WITHOUT_TLS
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

    if (0 != (pNetwork->handle = (intptr_t)HAL_SSL_Establish(
            pNetwork->pHostAddress,
            pNetwork->port,
            pNetwork->ca_crt,
            pNetwork->ca_crt_len + 1))) {
        return 0;
    } else {
        /* TODO SHOLUD not remove this handle space */
        /* The space will be freed by calling disconnect_ssl() */
        /* utils_memory_free((void *)pNetwork->handle); */
        iotx_event_post(IOTX_CONN_CLOUD_FAIL);
        return -1;
    }
}
#endif  /* #ifndef IOTX_WITHOUT_TLS */

/*** iTLS connection ***/
#if defined(SUPPORT_ITLS)
static int read_itls(utils_network_pt pNetwork, char *buffer, uint32_t len, uint32_t timeout_ms)
{
    if (NULL == pNetwork) {
        utils_err("network is null");
        return -1;
    }

    return HAL_SSL_Read((uintptr_t)pNetwork->handle, buffer, len, timeout_ms);
}

static int write_itls(utils_network_pt pNetwork, const char *buffer, uint32_t len, uint32_t timeout_ms)
{
    if (NULL == pNetwork) {
        utils_err("network is null");
        return -1;
    }

    return HAL_SSL_Write((uintptr_t)pNetwork->handle, buffer, len, timeout_ms);
}

static int disconnect_itls(utils_network_pt pNetwork)
{
    if (NULL == pNetwork) {
        utils_err("network is null");
        return -1;
    }

    HAL_SSL_Destroy((uintptr_t)pNetwork->handle);
    pNetwork->handle = 0;

    return 0;
}

static int connect_itls(utils_network_pt pNetwork)
{
    if (NULL == pNetwork) {
        utils_err("network is null");
        return 1;
    }

    char pkps[PRODUCT_KEY_LEN + PRODUCT_SECRET_LEN] = {0};
    int len = strlen(pNetwork->product_key);
    strncpy(pkps, pNetwork->product_key, len);
    HAL_GetProductSecret(pkps + len + 1);
    len += strlen(pkps + len + 1) + 2;

    if (0 != (pNetwork->handle = (intptr_t)HAL_SSL_Establish(
            pNetwork->pHostAddress,
            pNetwork->port,
            pkps, len))) {
        return 0;
    } else {
        /* TODO SHOLUD not remove this handle space */
        /* The space will be freed by calling disconnect_ssl() */
        /* utils_memory_free((void *)pNetwork->handle); */
        return -1;
    }
}
#endif  /* #ifndef IOTX_WITHOUT_iTLS */

/****** network interface ******/
int utils_net_read(utils_network_pt pNetwork, char *buffer, uint32_t len, uint32_t timeout_ms)
{
    int     ret = 0;

    if (NULL == pNetwork->ca_crt && NULL == pNetwork->product_key) {
        ret = read_tcp(pNetwork, buffer, len, timeout_ms);
    }
#if defined(SUPPORT_ITLS)
    else if (NULL == pNetwork->ca_crt && NULL != pNetwork->product_key) {
        ret = read_itls(pNetwork, buffer, len, timeout_ms);
    }
#endif
#ifndef IOTX_WITHOUT_TLS
    else if (NULL != pNetwork->ca_crt && NULL == pNetwork->product_key) {
        ret = read_ssl(pNetwork, buffer, len, timeout_ms);
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

    if (NULL == pNetwork->ca_crt && NULL == pNetwork->product_key) {
        ret = write_tcp(pNetwork, buffer, len, timeout_ms);
    }
#if defined(SUPPORT_ITLS)
    else if (NULL == pNetwork->ca_crt && NULL != pNetwork->product_key) {
        ret = write_itls(pNetwork, buffer, len, timeout_ms);
    }
#endif
#ifndef IOTX_WITHOUT_TLS
    else if (NULL != pNetwork->ca_crt && NULL == pNetwork->product_key) {
        ret = write_ssl(pNetwork, buffer, len, timeout_ms);
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

    if (NULL == pNetwork->ca_crt && NULL == pNetwork->product_key) {
        ret = disconnect_tcp(pNetwork);
    }
#if defined(SUPPORT_ITLS)
    else if (NULL == pNetwork->ca_crt && NULL != pNetwork->product_key) {
        ret = disconnect_itls(pNetwork);
    }
#endif
#ifndef IOTX_WITHOUT_TLS
    else if (NULL != pNetwork->ca_crt && NULL == pNetwork->product_key) {
        ret = disconnect_ssl(pNetwork);
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

    if (NULL == pNetwork->ca_crt && NULL == pNetwork->product_key) {
        ret = connect_tcp(pNetwork);
    }
#if defined(SUPPORT_ITLS)
    else if (NULL == pNetwork->ca_crt && NULL != pNetwork->product_key) {
        ret = connect_itls(pNetwork);
    }
#endif
#ifndef IOTX_WITHOUT_TLS
    else if (NULL != pNetwork->ca_crt && NULL == pNetwork->product_key) {
        ret = connect_ssl(pNetwork);
    }
#endif
    else {
        ret = -1;
        utils_err("no method match!");
    }

    return ret;
}

int iotx_net_init(utils_network_pt pNetwork, const char *host, uint16_t port, const char *ca_crt, char *product_key)
{
    if (!pNetwork || !host) {
        utils_err("parameter error! pNetwork=%p, host = %p", pNetwork, host);
        return -1;
    }
    pNetwork->pHostAddress = host;
    pNetwork->port = port;
    pNetwork->ca_crt = ca_crt;
#if !defined(SUPPORT_ITLS)
    pNetwork->product_key = NULL;
#else
    pNetwork->product_key = product_key;
#endif

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
