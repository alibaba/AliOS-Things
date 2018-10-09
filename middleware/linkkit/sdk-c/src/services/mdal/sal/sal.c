/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "iotx_log.h"

#include <sal_arch.h>
#include <sal_ipaddr.h>
#include <sal.h>

#define TAG "sal_module"

static sal_op_t *g_sal_module = NULL;

int sal_module_register(sal_op_t *module)
{
    if (NULL == module) {
        log_err(TAG, "sal module register invalid input\n");
        return -1;
    }

    if (NULL != g_sal_module) {
        log_err(TAG, "sal module have already registered\n");
        return -1;
    }

    g_sal_module = module;

    return 0;
}

int sal_module_init(void)
{
    int err = 0;

    if (NULL == g_sal_module) {
        log_err(TAG, "sal module init fail for there is no sal module registered yet \n");
        return 0;
    }

    if (NULL == g_sal_module->init) {
        log_err(TAG, "init function in sal module is null \n");
        return -1;
    }

    /*show we deinit module at first ?*/
    err = g_sal_module->init();
    if (err) {
        log_err(TAG, "module init fail\n");
    }

    return err;
}

int sal_module_deinit(void)
{
    int err = 0;

    if (NULL == g_sal_module) {
        log_err(TAG, "sal module deinit fail for there is no sal module registered yet \n");
        return -1;
    }

    if (NULL == g_sal_module->deinit) {
        log_err(TAG, "deinit function in sal module is null \n");
        return -1;
    }

    err = g_sal_module->deinit();
    if (err) {
        log_err(TAG, "module deinit fail\n");
    }

    return err;
}


int sal_module_start(sal_conn_t *conn)
{
    int err = 0;

    if (NULL == g_sal_module) {
        log_err(TAG, "sal module start fail for there is no sal module registered yet \n");
        return -1;
    }

    if (NULL == g_sal_module->start) {
        log_err(TAG, "start function in sal module is null \n");
        return -1;
    }

    if (NULL == conn) {
        log_err(TAG, "invalid input\n");
        return -1;
    }

    err = g_sal_module->start(conn);
    if (err) {
        log_err(TAG, "module start fail err=%d\n", err);
    }

    return err;
}

int sal_module_close(int fd, int32_t remote_port)
{
    int err = 0;

    if (NULL == g_sal_module) {
        log_err(TAG, "sal module close fail for there is no sal module registered yet \n");
        return -1;
    }

    if (NULL == g_sal_module->close) {
        log_err(TAG, "close function in sal module is null \n");
        return -1;
    }

    err = g_sal_module->close(fd, remote_port);
    if (err) {
       log_err(TAG, "module close fail err=%d\n", err);
    }

    return err;
}

int sal_module_send(int fd, uint8_t *data, uint32_t len,
                    char remote_ip[16], int32_t remote_port, int32_t timeout)
{
    int err = 0;

    if (NULL == g_sal_module) {
        log_err(TAG, "sal module send fail for there is no sal module registered yet \n");
        return -1;
    }

    if (NULL == g_sal_module->send) {
        log_err(TAG, "send function in sal module is null \n");
        return -1;
    }

    if (NULL == data) {
        log_err(TAG, "invalid input\n");
        return -1;
    }

    err = g_sal_module->send(fd, data, len, remote_ip, remote_port, timeout);
    if (err) {
        log_err(TAG, "module send fail err=%d\n", err);
    }

    return err;
}

int sal_module_domain_to_ip(char *domain, char ip[16])
{
    int err = 0;

    if (NULL == g_sal_module) {
        log_err(TAG, "sal module domain_to_ip fail for there is no sal module registered yet \n");
        return -1;
    }

    if (NULL == g_sal_module->domain_to_ip) {
        log_err(TAG, "domai_to_ip function in sal module is null \n");
        return -1;
    }

    if (NULL == domain) {
        log_err(TAG, "invalid input\n");
        return -1;
    }

    err = g_sal_module->domain_to_ip(domain, ip);
    if (err) {
       log_err(TAG, "module domain_to_ip fail err=%d\n", err);
    }

    return err;
}

int sal_module_register_netconn_data_input_cb(netconn_data_input_cb_t cb)
{
    int err = 0;

    if (NULL == g_sal_module) {
        log_err(TAG, "sal module recv fail for there is no sal module registered yet \n");
        return -1;
    }

    if (NULL == g_sal_module->register_netconn_data_input_cb) {
        log_err(TAG, "recv function in sal module is null \n");
        return -1;
    }

    err = g_sal_module->register_netconn_data_input_cb(cb);
    if (err) {
        log_err(TAG, "module recv fail err=%d\n", err);
    }

    return err;
}