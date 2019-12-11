/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

/**
 * @file atparser_app.c
 *
 * This file includes sample code of how to use atparser
 * to ease the communication with AT modules.
 *
 * In this sample, it will be demonstrated how to communicate
 * with M5310A (a NBIoT AT module from China Modbile IoT) through
 * atparser provided APIs.
 */

#include <stdio.h>
#include <aos/kernel.h>
#include <ulog/ulog.h>
#include <network/nal/atparser/atparser.h>
/**
 * @note **DO** include below 2 lines to use the platform 
 *       and module dependent AT config
 */
#include <atcmd_config_platform.h>
#include <atcmd_config_module.h>

#define TAG "atparser_app"
#define M5310A_DEFAULT_CMD_LEN 64
#define M5310A_DEFAULT_RSP_LEN 64

static uart_dev_t uart_dev;
static int at_dev_fd;
static int socket_fd = -1;
static bool socket_connected = false;

/* The callback function to handle socket data */
static void m5310a_socket_data_indicator_handler
(void *arg, char *buf, int buflen)
{
    char data = 0, cmd[M5310A_DEFAULT_CMD_LEN] = {0}, *rsp;
    int fd = 0, length = 0;

    while (data != ',') {
        at_read(at_dev_fd, &data, 1);
        if(data >= '0' && data <= '9') {
            fd = fd * 10 + (data - '0');
        }
    }

    while (data != '\n') {
        at_read(at_dev_fd, &data, 1);
        if(data >= '0' && data <= '9') {
            length = length * 10 + (data - '0');
        }
    }

    LOGD(TAG, "Socket data arrived, fd: %d, length: %d.", fd, length);

    /* hexstring needs to be converted to hex, so (2*n+1) space needed */
    rsp = (char *)aos_malloc(length << 2 + 1);
    if (!rsp) {
        LOGE(TAG, "%s %d failed to allocate mm\r\n", __func__, __LINE__);
        return;
    }

    snprintf(cmd, sizeof(cmd) - 1, "AT+NSORF=%d,%d", fd, length);
    at_send_wait_reply(at_dev_fd, cmd, strlen(cmd), true, NULL, 0,
                       rsp, M5310A_DEFAULT_RSP_LEN, NULL);
    if (strstr(rsp, "OK") == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return;
    }

    LOGD(TAG, "Socket data recevied: %s", rsp);
    aos_free(rsp);
}

/* The callback function to handle socket connection state */
static void m5310a_socket_connect_indicator_handler
(void *arg, char *buf, int buflen)
{
    char data[5] = {0};

    at_read(at_dev_fd, data, 2);
    if (data[0] != '\r' || data[1] != '\n') {
        LOGE(TAG, "Abnormal socket connect event (postfix: %s)!", data);
        return;
    }

    socket_connected = true;
    LOGD(TAG, "Socket connected.");
}

/* The callback function to handle socket close */
static void m5310a_socket_close_indicator_handler
(void *arg, char *buf, int buflen)
{
    char data[5] = {0};

    at_read(at_dev_fd, data, 3);
    if ((data[0] > '9') || (data[1] < '0') ||
        (data[1] != '\r' || (data[2] != "\n"))) {
        LOGE(TAG, "Abnormal socket close event (conent: %s)", data);
        return;
    }

    if ((data[0] - '0') == socket_fd) {
        socket_connected = false;
        LOGD(TAG, "Socket closed.");
    }
}

int application_start(int argc, char *argv[])
{
    at_config_t at_config = {0};
    char cmd[M5310A_DEFAULT_CMD_LEN] = {0};
    char rsp[M5310A_DEFAULT_RSP_LEN] = {0};
    char *str;

    /* Initialize atparser core via at_init(). */
    at_init();

    /**
     * Add AT device via at_add_dev().
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

    /* UART config */
    uart_dev.port                = AT_UART_PORT;
    uart_dev.config.baud_rate    = AT_UART_BAUDRATE;
    uart_dev.config.data_width   = AT_UART_DATA_WIDTH;
    uart_dev.config.parity       = AT_UART_PARITY;
    uart_dev.config.stop_bits    = AT_UART_STOP_BITS;
    uart_dev.config.flow_control = AT_UART_FLOW_CONTROL;
    uart_dev.config.mode         = AT_UART_MODE;

    /* AT config, which has the UART config included */
    at_config.type                             = AT_DEV_UART;
    at_config.port                             = AT_UART_PORT;
    at_config.dev_cfg                          = &uart_dev;
    at_config.send_wait_prompt                 = 0;

    /* Add AT device, and save the returned fd which will be used later */
    if ((at_dev_fd = at_add_dev(&at_config)) < 0) {
        LOGE(TAG, "AT parser device add failed!\n");
        return -1;
    }

    /**
     * Register OOB event handler via at_register_callback.
     *
     * When there is any event to handle, such as socket data arrival,
     * WiFi connection/disconnection, domain server available, etc.,
     * please use the at_register_callback to register a callback handler.
     * The handler will be called when the according AT event happen.
     *
     * Note: if you are interested on an AT event which has fixed prefix
     *       and postfix, you should specify the postfix, recvbuf and bufsize
     *       arguments in the at_register_callback API; otherwise,
     *       these fields should be filled to NULL or 0. The result which
     *       matches the prefix and postfix pattern will be stored in
     *       recvbuf and the result length will be filled.
     */
    at_register_callback(at_dev_fd, "+NSONMI:", NULL, NULL, 0,
                         m5310a_socket_data_indicator_handler, NULL);
    at_register_callback(at_dev_fd, "\r\nCONNECT OK", NULL, NULL, 0,
                         m5310a_socket_connect_indicator_handler, NULL);
    at_register_callback(at_dev_fd, "+NSOCLI:", NULL, NULL, 0,
                         m5310a_socket_close_indicator_handler, NULL);

    /* Test AT */
    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd) - 1, "%s", "AT");
    at_send_wait_reply(at_dev_fd, cmd, strlen(cmd), true, NULL, 0,
                       rsp, M5310A_DEFAULT_RSP_LEN, NULL);
    if (strstr(rsp, "OK") == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }

    /* Reset the module */
    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd) - 1, "%s", "AT+NRB");
    at_send_wait_reply(at_dev_fd, cmd, strlen(cmd), true, NULL, 0,
                       rsp, M5310A_DEFAULT_RSP_LEN, NULL);
    if ((strstr(rsp, "M5310-A") == NULL) || (strstr(rsp, "OK") == NULL)) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }

    /* Open functionarity */
    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd) - 1, "%s", "AT+CFUN=1");
    at_send_wait_reply(at_dev_fd, cmd, strlen(cmd), true, NULL, 0,
                       rsp, M5310A_DEFAULT_RSP_LEN, NULL);
    if (strstr(rsp, "OK") == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }

    /* Setup network */
    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd) - 1, "%s", "AT+CGATT=1");
    at_send_wait_reply(at_dev_fd, cmd, strlen(cmd), true, NULL, 0,
                       rsp, M5310A_DEFAULT_RSP_LEN, NULL);
    if (strstr(rsp, "OK") == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }

    /* Check the signal quality */
    int rssi, ber;
    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd) - 1, "%s", "AT+CSQ");
    at_send_wait_reply(at_dev_fd, cmd, strlen(cmd), true, NULL, 0,
                       rsp, M5310A_DEFAULT_RSP_LEN, NULL);
    if (strstr(rsp, "OK") == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }
    if (str = strstr(rsp, "+CSQ:") == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    } else {
        sscanf(str, "+CSQ:%d,%d", &rssi, &ber);
        if (rssi == 99) {
            LOGE(TAG, "%s %d failed, invalid rssi: %d\r\n", __func__, __LINE__, rsp);
            return -1;
        }
    }

    /* Create a TCP socket */
    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd) - 1, "AT+NSOCR=%s,%d", "STREAM", 6);
    at_send_wait_reply(at_dev_fd, cmd, strlen(cmd), true, NULL, 0,
                       rsp, M5310A_DEFAULT_RSP_LEN, NULL);
    if (strstr(rsp, "OK") == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }
    sscanf(rsp, "%d\r\nOK", &socket_fd);

    /* Connect the TCP server 134.102.218.18:80 */
    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd) - 1, "AT+NSOCO=%d,%s,%d", socket_fd, "134.102.218.18", 80);
    at_send_wait_reply(at_dev_fd, cmd, strlen(cmd), true, NULL, 0,
                       rsp, M5310A_DEFAULT_RSP_LEN, NULL);
    if (strstr(rsp, "OK") == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }

    /* Wait for socket connection success before socket data exchange */
    while (!socket_connected) aos_msleep(100);

    /* Send test payload to server */
    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd) - 1, "AT+NSOSD=%d,%d,%04x", socket_fd, 4, 0x55aa);
    at_send_wait_reply(at_dev_fd, cmd, strlen(cmd), true, NULL, 0,
                       rsp, M5310A_DEFAULT_RSP_LEN, NULL);
    if (strstr(rsp, "OK") == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }

    /* Close the socket */
    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd) - 1, "AT+NSOCL=%d", socket_fd);
    at_send_wait_reply(at_dev_fd, cmd, strlen(cmd), true, NULL, 0,
                       rsp, M5310A_DEFAULT_RSP_LEN, NULL);
    if (strstr(rsp, "OK") == NULL) {
        LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }
    socket_connected = false;

    /* End of the sample */
    return 0;
}
