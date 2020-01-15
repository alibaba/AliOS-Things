/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

#include <string.h>
#include <cli/cli_api.h>
#include <lwip/etharp.h>

static void arp_command(char *buffer, int32_t buf_len, int32_t argc, char **argv);

struct cli_command_st arp_cmd[] = {
    { "arp", "arp app", arp_command},
};

static void arp_help_command(void)
{
    LWIP_DEBUGF( ARP_DEBUG, ("Usage: arp -a\n"));
    LWIP_DEBUGF( ARP_DEBUG, ("Eample:\n"));
    LWIP_DEBUGF( ARP_DEBUG, ("arp -a\n"));
}
 
static void arp_query_command(void)
{
    if (etharp_info_print() < 0) {
        LWIP_DEBUGF( ARP_DEBUG, ("arp list empty!\n"));
    }
}

static void arp_command(char *buffer, int32_t buf_len, int32_t argc, char **argv)
{
    int32_t index = 0;

    if (argc < 2) {
        LWIP_DEBUGF( ARP_DEBUG, ("%s, invalid command\n", __func__));
        arp_help_command();
        return;
    }

    if (strcmp(argv[1], "-a") == 0) {
        arp_query_command();
    } else {
        arp_help_command();
    }
}

int32_t arp_cli_register(void)
{
    if (cli_register_commands(arp_cmd, 1)) {
        return 0;
    }

    return -1;
}
