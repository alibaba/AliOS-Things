/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#include "aos/cli.h"
#include "stddef.h"

extern void handle_networktestcmd(char *pwbuf, int blen, int argc, char **argv);

static struct cli_command networktestcmds[] = {
    {
        .name = "network",
        .help = "netowork { tcp_c remote_ip remote_port data [times] } | { domain domain_info [ remote_port ]}",
        .function = handle_networktestcmd
    }
};

int application_start(int argc, char *argv[])
{
    printf("%s-%d called\n", __FUNCTION__, __LINE__);
    wifi_service_init();
    wifi_service_wifi_connect("ssid", "passwd", NULL, 10 * 1000);
    aos_cli_register_commands((const struct cli_command *)&networktestcmds[0], sizeof(networktestcmds) / sizeof(networktestcmds[0]));
}

void handle_networktestcmd(char *pwbuf, int blen, int argc, char **argv)
{
    char *ptype = NULL;
    int  ret = 0;

    aos_cli_init();


    printf("tcp demo entry here!\r\n");

    if (argc < 2 || NULL == argv){
        printf("invalid input netword test command argc %d argv %p \r\n", argc, argv);
        return;
    }

    ptype = argv[1];

    if (strcmp(ptype, "tcp_c") == 0) {
        ret = networktestcmd_tcp_client(argc, argv);
        if (ret){
            printf("fail to execute tcp client test command \r\n");
            return;
        }
    } else {
        printf("invalid netword test command input \r\n");
    }
    printf("network command test successed \r\n");
    
}
