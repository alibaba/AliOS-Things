/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */
#if AOS_COMP_CLI
#include <string.h>
#include <aos/cli.h>
#include <lwip/netdb.h>
#include "lwip/opt.h"

static void lsfd_command(char *buffer, int32_t buf_len, int32_t argc, char **argv);

struct cli_command lsfd_cmd[] = {
    { "lsfd", "lsfd app",  lsfd_command},
};
static void lsfd_help_command(void)
{
    LWIP_DEBUGF( SOCKET_ALLOC_DEBUG, ("Usage: lsfd\n"));
    LWIP_DEBUGF( SOCKET_ALLOC_DEBUG, ("Eample:\n"));
    LWIP_DEBUGF( SOCKET_ALLOC_DEBUG, ("lsfd\n"));
}

static void lsfd_exec_command(void)
{
#if (SOCKET_ALLOC_DEBUG == LWIP_DBG_ON)
    extern void print_sock_alloc_info(void);
    print_sock_alloc_info();
#endif
}

static void lsfd_command(char *buffer, int32_t buf_len, int32_t argc, char **argv)
{
    if (argc == 1) {
        lsfd_exec_command();
        return;
    }

    lsfd_help_command();
}

int32_t lsfd_cli_register(void)
{
    if (0 == aos_cli_register_commands(lsfd_cmd, 1)) {
        return 0;
    }

    return -1;
}
#endif /* AOS_COMP_CLI */
