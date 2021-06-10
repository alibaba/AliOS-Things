/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>

#include "amp_config.h"
#include "aos_system.h"
#include "amp_task.h"
#include "amp_defines.h"
#include "aos_hal_uart.h"
#include "quickjs.h"
#include "repl.h"

#ifdef AOS_COMP_CLI
#include "aos/cli.h"
#endif

#define MOD_STR "REPL"

int g_repl_config = 0;
static uart_dev_t g_repl_uart;
static char g_repl_tag[64] = {0};
static uint8_t g_repl_tag_len = 0;
static char repl_message[REPL_OUTBUF_SIZE] = {0};

int aos_repl_read(char *inbuf, uint32_t expected_length, uint32_t *recv_size)
{
    int32_t ret = REPL_OK;

    ret = aos_hal_uart_recv_II(&g_repl_uart, inbuf, expected_length, recv_size, 100);
    if (ret == 0) {
        return *recv_size;
    } else {
        return 0;
    }
}

static int32_t repl_putstr(char *msg)
{
    if (msg[0] != 0)
    {
        aos_hal_uart_send(&g_repl_uart, (void *)msg, strlen(msg), 0xFFFFFFFFU);
    }

    return REPL_OK;
}

int32_t repl_printf(const char *buffer, ...)
{
    va_list ap;

    int32_t sz, len;

    char *pos = NULL;

    memset(repl_message, 0, REPL_OUTBUF_SIZE);

    sz = 0;
    if (g_repl_tag_len > 0)
    {
        len = strlen(g_repl_tag);
        strncpy(repl_message, g_repl_tag, len);
        sz = len;
    }

    pos = repl_message + sz;

    va_start(ap, buffer);
    len = vsnprintf(pos, REPL_OUTBUF_SIZE - sz, buffer, ap);
    va_end(ap);
    if (len <= 0)
    {
        return REPL_OK;
    }

    len = strlen(repl_message);

    repl_putstr(repl_message);
    if(repl_message[len-1] == '\n') {
        repl_putstr("\r");
    }

    return REPL_OK;
}

int aos_repl_write(char *str)
{
    repl_printf("%s", str);
}

static int32_t repl_init(void)
{
    g_repl_uart.port = AMP_REPL_UART_PORT;
    g_repl_uart.config.baud_rate = AMP_REPL_UART_BAUDRATE;
    g_repl_uart.config.data_width = DATA_WIDTH_8BIT;
    g_repl_uart.config.flow_control = FLOW_CONTROL_DISABLED;
    g_repl_uart.config.mode = MODE_TX_RX;
    g_repl_uart.config.parity = NO_PARITY;
    g_repl_uart.config.stop_bits = STOP_BITS_1;
    aos_hal_uart_init(&g_repl_uart);

    repl_read_task_start();

    amp_debug(MOD_STR, "REPL Enabled\r\n");
    return 0;
}

void aos_repl_init(void *arg)
{
#ifdef AOS_COMP_CLI
    repl_printf("cli suspend\n");
    aos_cli_suspend();
    repl_printf("repl init\r\n");
#endif
    repl_init();

    return ;
}

int32_t aos_repl_close()
{
#ifdef AOS_COMP_CLI
    repl_printf("cli resume\n");
    aos_cli_resume();
#endif
    return 0;
}

#ifdef AOS_COMP_CLI
void jsrepl_startup()
{
    aos_task_t repl_task;
    aos_task_new_ext(&repl_task, "amp init task", aos_repl_init, NULL, 1024 * 4, AOS_DEFAULT_APP_PRI);
}
/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(jsrepl_startup, jsrepl, "start js amp repl")
#endif
