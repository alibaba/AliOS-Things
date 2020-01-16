/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

#include <string.h>
#include <aos/cli.h>
#include <lwip/netdb.h>

static void lsfd_command(char *buffer, int32_t buf_len, int32_t argc, char **argv);

struct cli_command lsfd_cmd[] = {
    { "lsfd", "lsfd app",  lsfd_command},
};
static void lsfd_help_command(void)
{
    LWIP_DEBUGF( LSFD_DEBUG, ("Usage: lsfd        Show socket fd info\n"));
    LWIP_DEBUGF( LSFD_DEBUG, ("Eample:\n"));
    LWIP_DEBUGF( LSFD_DEBUG, ("lsfd\n"));
}

static void lsfd_exec_command(void)
{
    extern void print_sock_alloc_info(void);
    print_sock_alloc_info();
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
    if (cli_register_commands(lsfd_cmd, 1)) {
        return 0;
    }

    return -1;
}
