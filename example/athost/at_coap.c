/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <aos/aos.h>
#include <atparser.h>
#include <aos/network.h>
#include <hal/wifi.h>

#include "athost.h"

enum
{
    ATCMD_ICOAP_PARA = 0,
    ATCMD_ICOAP_OPEN,
    ATCMD_ICOAP_AUTH,
    ATCMD_ICOAP_SENDREQ,
    ATCMD_ICOAP_SENDBIN,
    ATCMD_ICOAP_SENDREQN,
    ATCMD_ICOAP_SENDNBIN,
    ATCMD_ICOAP_RCVRSP,
    ATCMD_ICOAP_RCVBIN,
};

static const atcmd_hdl_t at_coap_cmds_table[] = {
    { .name     = "AT+ICOAPARA",
      .help     = "AT+ICOAPARA=<ParaTag>,<ParaValue>",
      .function = atcmd_uart_echo },
};

static atcmd_hdl_ptr_t get_atcmd_uart_handler()
{
    return NULL;
}

static int coap_init()
{
    // do somthing
    return 0;
}

static int coap_deinit()
{
    // do somthing
    return;
}

atcmd_op_t coap_op = {
    .name              = "coap",
    .prefix            = "IC",
    .init              = coap_init,
    .deinit            = coap_deinit,
    .get_atcmd_handler = at_coap_cmds_table,
};

int register_atcmd_coap_opt()
{
    return atcmd_resigter_handlers(&coap_op);
}
