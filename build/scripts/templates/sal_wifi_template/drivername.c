/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "network/hal/wifi.h"

#include <sal_import.h>

#include <atcmd_config_module.h>
#include <atcmd_config_platform.h>

/* To implement SAL Init */
static int HAL_SAL_Init(void)
{
    /* add your code here */
    return 0;
}

/* To implement SAL Deinit */
static int HAL_SAL_Deinit(void)
{
    /* add your code here */
    return 0;
}

/* To implement the SAL Start */
static int HAL_SAL_Start(sal_conn_t *conn)
{
    /* add your code here */
    return 0;
}

/* To implement the SAL Close */
static int HAL_SAL_Close(int fd, int32_t remote_port)
{
    /* add your code here */
    return 0;
}

/* To implement the Domain to ip address translation */
static int HAL_SAL_DomainToIp(char *domain, char ip[16])
{
    /* add your code here */
    return 0;
}

/* To implement the data transmission */
static int HAL_SAL_Send(int fd,
                 uint8_t *data,
                 uint32_t len,
                 char remote_ip[16],
                 int32_t remote_port,
                 int32_t timeout)
{
    /* add your code here */
    return 0;
}

/* To implement the data reception callback */
static int HAL_SAL_RegisterNetconnDataInputCb(netconn_data_input_cb_t cb)
{
    /* add your code here */
    return 0;
}

/* To implement the driver configuration */
static int @drivername@_sal_add_dev(void* data)
{
    /* add your code here */
    return 0;
}

sal_op_t sal_op = {
    .next = NULL,
    .version = "1.0.0",
    .name = "@drivername@",
    .add_dev = @drivername@_sal_add_dev,
    .init = HAL_SAL_Init,
    .start = HAL_SAL_Start,
    .send_data = HAL_SAL_Send,
    .domain_to_ip = HAL_SAL_DomainToIp,
    .finish = HAL_SAL_Close,
    .deinit = HAL_SAL_Deinit,
    .register_netconn_data_input_cb = HAL_SAL_RegisterNetconnDataInputCb,
};

int @drivername@_sal_init(void)
{
    return sal_module_register(&sal_op);
}
