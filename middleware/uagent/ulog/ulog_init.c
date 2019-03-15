/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "ulog/ulog.h"
#include "ulog_api.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "k_api.h"
#include "aos/cli.h"

bool log_init = false;

#ifdef AOS_COMP_CLI

#ifdef ULOG_CONFIG_ASYNC

__attribute__((weak)) void update_net_cli(const char cmd, const char* param)
{
    /* Will be overwrite in session_udp, as this implement take effect only the UDP feature support,
       I don't like so many compile switcher in the code which result reader is not so comfortable,
       so weak attribute used here.
    */
}

__attribute__((weak)) void fs_control_cli(const char cmd, const char* param)
{
    /* Will be overwrite in session_fs */
}

__attribute__((weak)) void on_show_ulog_file()
{
    /* Will be overwrite in session_fs */
}

static void cmd_cli_ulog(char *pwbuf, int blen, int argc, char *argv[])
{
    bool exit_loop = false;
    uint8_t session = 0xFF;
    uint8_t level = 0xFF;
    uint8_t i;

    if (argc == 2) {
        const char* option = argv[1];
        switch (option[0])
        {
        case 'f':
            on_show_ulog_file();
            break;
        default:
            break;
        }
    }
    for (i = 1; i < argc && !exit_loop; i += 2)
    {
        const char* option = argv[i];
        const char* param = argv[i + 1];
        if (option != NULL && param != NULL && strlen(option) == 1) {
            switch (option[0])
            {
            case 's':/* session */
                session = strtoul(param, NULL, 10);
                break;

            case 'l':/* level */
                level = strtoul(param, NULL, 10);
                break;

            case 'a':/* syslog watcher address */
                update_net_cli(option[0], param);

            case 't':/* terminate record on fs */
                fs_control_cli(option[0], param);

                break;

            default: /* unknown option */
                exit_loop = true;
                break;
            }
        } else {/* unknown format */
            break;
        }
    }

    if ((session < SESSION_CNT) && (level <= LOG_NONE)) {
        on_filter_level_change((SESSION_TYPE)session, level);
    }
}
#endif

static void sync_log_cmd(char *buf, int len, int argc, char **argv)
{
    if (argc < 2) {
        aos_cli_printf("stop filter level : %c\r\n", get_sync_stop_level());
        return;
     }else if (argc == 2) {
        const char* option = argv[1];
        switch (option[0])
        {
        case 'F':
        case 'E':
        case 'W':
        case 'I':
        case 'D':
        case 'N': /* none stop filter, i.e. all pass */
            on_sync_filter_level_change(option[0]);
            break;

        default:
            aos_cli_printf("unknown option %c, only support[fewidn]\r\n", option[0]);
            break;
        }
    }

}

static struct cli_command ulog_cmd[] = {
    { "loglevel","set sync log level",sync_log_cmd },
#ifdef ULOG_CONFIG_ASYNC
    { "ulog", "ulog [option param]",cmd_cli_ulog },
#endif
};

#endif /* AOS_COMP_CLI */

void ulog_init(const char host_name[8])
{
    if (!log_init) {
        log_init_mutex();
#ifdef ULOG_CONFIG_ASYNC
        ulog_async_init(host_name);
#endif
#ifdef AOS_COMP_CLI
        aos_cli_register_commands(&ulog_cmd[0], sizeof(ulog_cmd) / sizeof(struct cli_command));
#endif
        log_init = true;
    }
}
