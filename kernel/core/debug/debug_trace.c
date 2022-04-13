/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "k_api.h"
#include "aos/kernel.h"
#include "debug_api.h"

#ifdef AOS_COMP_CLI
#include "aos/cli.h"

#if (RHINO_CONFIG_USER_SPACE > 0)
#include "syscall_no.h"

typedef struct {
    aos_timer_t timer_handle;
    uint32_t    trace_syscall[MAX_SYSCALL_NO + 1];
    uint64_t    trace_syscall_total;
} TRACE_SYSCALL_STAT;

TRACE_SYSCALL_STAT g_debug_trace_syscall;

void debug_trace_syscall_hook(uint32_t syscall_no)
{
    g_debug_trace_syscall.trace_syscall[syscall_no] ++;
    g_debug_trace_syscall.trace_syscall_total ++;
}

static void trace_syscall_stat_show(void *timer, void *para)
{
    int i;

    aos_cli_printf("----- syscall stat show ----- \r\n");
    aos_cli_printf("syscall_no  sysacll_cnt\r\n");

    for (i = 0; i < MAX_SYSCALL_NO; i++) {
        if (g_debug_trace_syscall.trace_syscall[i] > 0) {
            aos_cli_printf("  %03d        %08d\r\n", i, g_debug_trace_syscall.trace_syscall[i]);
        }
    }

    aos_cli_printf("syscall total cnt : %lld\r\n", g_debug_trace_syscall.trace_syscall_total);
}

static void trace_syscall_stat_cmd(char *buf, int len, int argc, char **argv)
{
    uint32_t period;

    if (argc == 1) {
        aos_cli_printf("trace help :\r\n"
               "trace -s[-d] : show syscall stats\r\n");
        return;
    }

    if (0 == strcmp(argv[1], "-s")) {
        switch (argc) {
            case 2:
                trace_syscall_stat_show(NULL, NULL);
                break;
            case 3:
                if (0 == strcmp(argv[2], "stop")) {
                    aos_timer_stop(&g_debug_trace_syscall.timer_handle);
                    aos_timer_free(&g_debug_trace_syscall.timer_handle);
                }
                break;
            case 4:
                if (0 == strcmp(argv[2], "-d")) {
                    period = atoi(argv[3]);
                    if (0 != aos_timer_new(&g_debug_trace_syscall.timer_handle, trace_syscall_stat_show, NULL, period, 1)) {
                        aos_cli_printf("syscall stats timer create fail\r\n");
                    }
                }
                break;
            default:
                break;
        }
    }
}

#endif /* (RHINO_CONFIG_USER_SPACE > 0) */

static const struct cli_command trace_cmd[] = {
#if (RHINO_CONFIG_USER_SPACE > 0)
    { "trace", "Syscall Statistic Show", trace_syscall_stat_cmd },
#endif
};

static void debug_trace_cmds_register(void)
{
    int32_t ret;

    ret = aos_cli_register_commands(trace_cmd, sizeof(trace_cmd) / sizeof(struct cli_command));
    if (ret) {
        printf("%s %d failed, ret = %d\r\n", __func__, __LINE__, ret);
    }
}
#endif

void debug_trace_init(void)
{
#ifdef AOS_COMP_CLI
    debug_trace_cmds_register();
#endif
}