/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef JSE_CLI_H
#define JSE_CLI_H

#include "jse_port.h"

#include "ulog/ulog.h"
#include "aos/cli.h"

#define MAX_COMMANDS 64
#define INBUF_SIZE 1024 /* 必须 >= 1024,因为be push的每次为500字节*2 */
#define OUTBUF_SIZE 2048
#define HIS_SIZE 5

#define CLI_MAX_ARG_NUM 16
#define CLI_MAX_ONCECMD_NUM 6

#ifndef FUNCPTR
typedef void (*FUNCPTR)(void);
#endif

#define jse_cli_command cli_command
#define jse_cli_register_command aos_cli_register_command
#define jse_cli_printf aos_cli_printf
#define jse_cli_init aos_cli_init

void cli_cmd_register_js(void);
void cli_cmd_register_dev(void);
void cli_cmd_register_file(void);
void cli_cmd_register_app(void);
void websocket_call_cli(char *cmdname, char **argv);

#endif /* JSE_CLI_H */
