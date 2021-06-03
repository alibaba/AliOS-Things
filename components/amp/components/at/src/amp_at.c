#include "stdlib.h"
#include "stdint.h"
#include "stdbool.h"
#include "stdarg.h"
#include "at.h"
#include "at_internal.h"
#include "atcmd_config_module.h"
#include "aos_hal_uart.h"
#include "aos_network.h"

static uart_dev_t uart_dev;
static int at_dev_fd = -1;
#define AT_CMD_RSP_LEN     512
#define AT_RECV_TIMEOUT_MS 100

int amp_at_yield(int fd, char *replybuf, int bufsize, int timeout_ms)
{
    return at_yield(at_dev_fd, replybuf, bufsize, NULL, timeout_ms);
}

int amp_at_read(char *outbuf, int readsize)
{
    return at_read(at_dev_fd, outbuf, readsize);
}

int amp_at_send_no_reply(const char *data, int datalen, bool delimiter)
{
    return at_send_no_reply(at_dev_fd, data, datalen, delimiter);
}

int amp_at_send_wait_reply(const char *cmd, int cmdlen, bool delimiter,
                       const char *data, int datalen, const char *expectrsp)
{
    char *rsp = NULL;
    char *str = NULL;
    int ret = 0;

    rsp = aos_malloc(AT_CMD_RSP_LEN);

    ret =  at_send_wait_reply(at_dev_fd, cmd, cmdlen, delimiter,
                       data, datalen,
                       rsp, AT_CMD_RSP_LEN,
                       NULL);

    if (expectrsp != NULL) {
        str = strstr(rsp, expectrsp);
    }

    aos_free(rsp);

    if ((expectrsp != NULL) && (str == NULL)) {
        return -1;
    }

    return ret;
}

int amp_at_register_callback(const char *prefix, at_recv_cb cb, void *arg)
{
    return at_register_callback(at_dev_fd, prefix, NULL, NULL, 0, cb, arg);
}

void amp_at_debug(char *str)
{
    aos_hal_uart_send(&uart_dev, str, strlen(str), -1);
}

void amp_at_debug_c(char c)
{
    aos_hal_uart_send(&uart_dev, &c, 1, -1);
}

void amp_at_deinit()
{
    at_deinit();
}

void amp_at_set_echo(int flag)
{
    at_set_echo(flag);
}

int amp_at_init(int at_uart_port, uint32_t baud_rate, char *send_delimiter)
{
    at_config_t at_config = {0};

    at_init();

    /* UART config */
    uart_dev.port                = at_uart_port;
    uart_dev.config.baud_rate    = baud_rate;
    uart_dev.config.data_width   = DATA_WIDTH_8BIT;
    uart_dev.config.parity       = NO_PARITY;
    uart_dev.config.stop_bits    = STOP_BITS_1;
    uart_dev.config.flow_control = FLOW_CONTROL_DISABLED;
    uart_dev.config.mode         = MODE_TX_RX;

    /* AT config, which has the UART config included */
    at_config.type                             = AT_DEV_UART;
    at_config.port                             = uart_dev.port;
    at_config.dev_cfg                          = &uart_dev;
    at_config.send_delimiter                   = send_delimiter; /* must be global memory string */
    at_config.timeout_ms                       = AT_RECV_TIMEOUT_MS;

    /* Add AT device, and save the returned fd which will be used later */
    if ((at_dev_fd = at_add_dev(&at_config)) < 0) {
        AT_LOGE("AT parser device add failed!\n");
        return -1;
    }

    return 0;
}

static int amp_at_get_cimi(void *arg, char *buf, int buflen)
{
    char imei[33] = {0};
    char *reply = NULL;

    reply = aos_malloc(64);
    memset(imei, '3', 32);
    snprintf(reply, 64, "\nAT+CIMI\nIMEI=%s\n", imei);
    amp_at_send_no_reply(reply, strlen(reply), true);
    aos_free(reply);

    aos_printf("at cmd get imei %s\n", imei);

    return 0;
}

int amp_at_instance_init()
{
    int ret = 0;
    ret = amp_at_register_callback("AT+CIMI", amp_at_get_cimi, NULL);
    aos_printf("register cmd %s ret %d\n", "AT+CIMI", ret);

    ret = amp_at_register_callback("AT+TEST", amp_at_get_cimi, NULL);
    aos_printf("register cmd %s ret %d\n", "AT+TEST", ret);
}

int amp_at_test()
{
    int ret = 0;
    ret = amp_at_init(3, 115200, "\r\n");
    aos_printf("amp_at_init result %d\n", ret);
    amp_at_instance_init();
    aos_printf("amp_at_init end\n");
}