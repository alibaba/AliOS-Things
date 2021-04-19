/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "cli_uagent.h"
#include "aos/cli.h"
#include "cli_api.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static uint8_t g_cloud_cmd_enable = 0;
unsigned char g_cmd_from_cloud = 0;

int32_t cli_handle_input(char *inbuf);

static int on_cli_handler(void *p, const unsigned short len, void *cmd)
{
    int rc = -1;
    if (cmd == NULL) {
        return rc;
    }

    if (NULL != strstr(cmd, "cli cloud ")) {
        g_cmd_from_cloud = 1;
        if (0 == strncmp("cli cloud start", cmd, len)) {
            aos_cli_printf("ready for your command!\n");
            g_cloud_cmd_enable = 1;
            g_cmd_from_cloud = 0;
            aos_cli_printf("CLI: switch on cloud control\n");
            rc = 0;
        } else if (0 == strncmp("cli cloud stop", cmd, len)) {
            aos_cli_printf("byebye!\n");
            g_cloud_cmd_enable = 0;
            g_cmd_from_cloud = 0;
            aos_cli_printf("CLI: switch off cloud control\n");
            rc = 0;
        } else if (NULL != strstr(cmd, "cli cloud timeout=")) {
            if (0 != uagent_request_service(
                         UAGENT_MOD_CLI, UAGENT_MOD_UAGENT,
                         UAGENT_FUNC_UA_DUMP_DELAY_SEND_PARAM, len, cmd)) {
                aos_cli_printf("CLI Setting Delay Parameter %s Fail\n",
                               (char *)cmd);
            } else {
                aos_cli_printf("CLI Setting Delay Parameter %s OK\n",
                               (char *)cmd);
                rc = 0;
            }
        }
        g_cmd_from_cloud = 0;
        return rc;
    }

    if (1 == g_cloud_cmd_enable) {
        g_cmd_from_cloud = 1;
        aos_cli_printf("\n# %s", (char *)cmd);
        rc = cli_handle_input(cmd);
        if (rc == CLI_ERR_BADCMD) {
            if (cmd != NULL) {
                aos_cli_printf("command '%s' not found\r\n", cmd);
            }
        } else if (rc == CLI_ERR_SYNTAX) {
            aos_cli_printf("syntax error\r\n");
        }
    }
    return rc;
}

static uagent_func_node_t cli_uagent_funclist[] = {
    {CLI_RESPONE, "cli result", NULL, NULL, &cli_uagent_funclist[1]},
    {CLI_CMD_INPUT, "cli input", on_cli_handler, NULL, NULL},
};

static mod_func_t cli_uagent_func = {{UAGENT_MOD_CLI, 2, "CLI", MOD_VER},
                                     cli_uagent_funclist};

void cli_uagent_init(void)
{
    const char cmd[] = "cli cloud timeout=20";
    uagent_func_node_t *p = cli_uagent_func.header;
    while (NULL != p) {
        if (0 != uagent_register(UAGENT_MOD_CLI, "CLI", MOD_VER, p->func,
                                 p->func_name, p->service, p->argu)) {
            printf("register into uagent fail");
        }
        p = p->next;
    }
    uagent_request_service(UAGENT_MOD_CLI, UAGENT_MOD_UAGENT,
                           UAGENT_FUNC_UA_DUMP_DELAY_SEND_PARAM, strlen(cmd),
                           cmd);
}
