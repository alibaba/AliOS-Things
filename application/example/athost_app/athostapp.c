/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

/**
 * @file atparser_app.c
 *
 * This file includes sample code of how to implement an AT
 * device which comply with ICA AT spec, by leveraging the
 * athost component provided inside AliOS Things.
 *
 * Blow two major things have to be done in order to implement
 * an ICA AT device with the help of AliOS Things athost:
 *   (0) add AT device, which is usually UART device. The other side
 *       of AT will communicate with us via the specified AT device.
 *   (1) implement HAL_Athost_xxx APIs, which is to be used by athost.
 *   (2) call athost_instance_init(), which will initialize and start
 *       athost core system. After this, the other side of AT can
 *       send ICA AT commands to us, and we provide result to the
 *       other side.
 */

#include <stdint.h>
#include <string.h>

#include "aos/kernel.h"
#include "ulog/ulog.h"

#ifdef AOS_ATCMD
/**
 * @note **DO** include below 2 lines to use the platform 
 *       and module dependent AT config
 */
#include <atcmd_config_platform.h>
#include <atcmd_config_module.h>
#include <atparser.h>
#endif

#include "athost_export.h"
#include "athost_import.h"

#define TAG "athostapp"

#ifdef AOS_ATCMD
static uart_dev_t uart_dev;
int at_dev_fd = -1;

/**
 * This function adds AT device via at_dev_add().
 *
 * For now the only supported AT device is actually UART device.
 * Here we need to fill the UART configuration parameters, including
 * UART port number (board dependent), baudrate (module dependent),
 * stop bits (module dependent), parity (module dependent), etc.
 * There UART configurations are defined in board or module header,
 * so please ensure you have include the atcmd_config_platform.h and
 * atcmd_config_module.h files which have the UART config definition.
 *
 * Note: uart_dev should be maintained in whole life cycle
 */
static int at_device_init(void)
{
    at_config_t at_config = { 0 };

    at_init();

    /* uart_dev should be maintained in whole life cycle */
    uart_dev.port                = AT_UART_PORT;
    uart_dev.config.baud_rate    = AT_UART_BAUDRATE;
    uart_dev.config.data_width   = AT_UART_DATA_WIDTH;
    uart_dev.config.parity       = AT_UART_PARITY;
    uart_dev.config.stop_bits    = AT_UART_STOP_BITS;
    uart_dev.config.flow_control = AT_UART_FLOW_CONTROL;
    uart_dev.config.mode         = AT_UART_MODE;

    /* configure and add one uart dev */
    at_config.type                             = AT_DEV_UART;
    at_config.port                             = AT_UART_PORT;
    at_config.dev_cfg                          = &uart_dev;
    at_config.recv_task_priority               = AT_WORKER_PRIORITY;
    at_config.recv_task_stacksize              = AT_WORKER_STACK_SIZE;

    if ((at_dev_fd = at_add_dev(&at_config)) < 0) {
        LOGE(TAG, "AT parser device add failed!\n");
        return -1;
    }

    return 0;
}
#endif

/* athost read HAL implementation */
int HAL_Athost_Read(char *outbuf, uint32_t len)
{
    int ret = 0;

#ifdef AOS_ATCMD
    ret = at_read(at_dev_fd, outbuf, len);
#endif
    return ret;
}

/* athost write HAL implementation */
int HAL_Athost_Write(const char *header, const uint8_t *data, uint32_t len,
                     const char *tailer)
{
    int ret = 0;

#ifdef AOS_ATCMD
    if (!header) {
        LOGE(TAG, "Invalid null header\n");
        return -1;
    }

    if ((ret = at_send_no_reply(at_dev_fd, header, strlen(header), false)) != 0) {
        LOGE(TAG, "uart send packet header failed");
        return -1;
    }

    if (data && len) {
        if ((ret = at_send_no_reply(at_dev_fd, (char *)data, len, false)) != 0) {
            LOGE(TAG, "uart send packet failed");
            return -1;
        }
    }

    if (tailer) {
        if ((ret = at_send_no_reply(at_dev_fd, tailer, strlen(tailer), false)) != 0) {
            LOGE(TAG, "uart send packet tailer failed");
            return -1;
        }
    }
#endif

    return ret;
}

/* athost callback register HAL implementation */
int HAL_Athost_HandleRegisterCb(const char              *prefix,
                                athost_atcmd_handle_cb_t fn)
{
    int ret = 0;

#ifdef AOS_ATCMD
    at_register_callback(at_dev_fd, prefix, NULL, NULL, 0, fn, NULL);
#endif

    return ret;
}

static void app_delayed_action(void *arg)
{
    LOG("AT host server: alive %s:%d %s\r\n", __func__, __LINE__,
        aos_task_name());
    aos_post_delayed_action(50000, app_delayed_action, NULL);
}

int application_start(int argc, char *argv[])
{
#ifdef AOS_ATCMD
    /**
     * - Step1:
     *
     * Initialize AT device, the athost and the other side of AT will
     * communicate with each other via the AT device. In this sample, atparser
     * is used as the AT device. Anyway, users are free to change to use other
     * AT device (but ensure to implemente the corresponding read/write HAL
     * APIs above, on which athost depends to function well).
     */
    at_device_init();
#endif

    /**
     * - Step2:
     *
     * Initialize athost core system. Hereafter, the athost will be
     * able to response to the AT commands from the AT channel sent by the
     * other side.
     */
    athost_instance_init();

    LOG("NEW AT host server start!\n");

    /* Here we add code to post an alive message, not required */
    aos_post_delayed_action(1000, app_delayed_action, NULL);

    /* Enter yloop, never return! */
    aos_loop_run();

    return 0;
}
