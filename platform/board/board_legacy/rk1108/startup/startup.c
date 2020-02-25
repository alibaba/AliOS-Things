/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <errno.h>
#include <signal.h>
#include <sys/resource.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arg_options.h>

#include "aos/init.h"
#include "aos/kernel.h"
#include "ulog/ulog.h"

#ifndef CONFIG_OSAL_POSIX
#include <k_api.h>
#endif

extern void krhino_lwip_init(int enable_tapif);
/* check in gcc sources gcc/gcov-io.h for the prototype */
extern void __gcov_flush(void);
extern void rl_free_line_state(void);
extern void rl_cleanup_after_signal(void);
extern void hw_start_hal(options_t *poptions);
extern void netmgr_init(void);
extern void cpu_tmr_sync(void);

#ifdef TFS_EMULATE
extern int tfs_emulate_id2_index;
#endif

static options_t options = { 0 };
static kinit_t kinit = { 0 };

int csp_get_args(const char ***pargv)
{
    *pargv = (const char **)options.argv;
    return options.argc;
}

void aos_lwip_features_init(void)
{
#if defined(CONFIG_AOS_LWIP) || defined(CONFIG_VENDOR_LWIP)
    if (options.lwip.enable) {
        krhino_lwip_init(options.lwip.tapif);
    }
#endif
}

int board_kinit_init(kinit_t* init_args)
{
    if(NULL == init_args){
        return -1;
    }
    init_args->argc        = options.argc;
    init_args->argv        = options.argv;
    init_args->cli_enable  = options.cli.enable;
    return 0;
}

int board_getcli_args(void)
{
    return options.cli.enable;
}

int board_getflash_args(void)
{
#ifdef CONFIG_AOS_MESHYTS
    options.flash.per_pid = true;
#else
    options.flash.per_pid = false;
#endif

    return options.flash.per_pid;
}

void board_tick_init(void);
void board_stduart_init(void);

void flash_partition_init(void);
void board_network_init(void);
int board_basic_init(void);


static void app_entry(void *arg)
{
    board_tick_init();
    board_stduart_init();
    flash_partition_init();
    board_network_init();

    board_kinit_init(&kinit);
    aos_components_init(&kinit);
#ifndef AOS_BINS
    application_start(kinit.argc, kinit.argv);  /* jump to app/example entry */
#endif
}

static void start_app(void)
{
#ifndef CONFIG_OSAL_POSIX
#if (RHINO_CONFIG_CPU_NUM > 1)
    ktask_t     *app_task;
    krhino_task_cpu_dyn_create(&app_task, "app_task", 0, 20, 0, 2048, app_entry, 0, 1);
#else
    aos_task_new("app", app_entry, NULL, 8192);
#endif
#else
    aos_task_new("app", app_entry, NULL, 8192);
#endif
}

int main(int argc, char **argv)
{
    int ret = 0;

    ret = board_basic_init();
    if(ret){
        return -1;
    }

    options.argc        = argc;
    options.argv        = argv;
    options.lwip.enable = true;
#if defined(TAPIF_DEFAULT_OFF) || !defined(CONFIG_AOS_LWIP)
    options.lwip.tapif  = false;
#else
    options.lwip.tapif  = true;
#endif
    options.log_level   = AOS_LL_WARN;
    options.cli.enable  = true;

    parse_options(&options);

    aos_set_log_level(options.log_level);

#ifdef TFS_EMULATE
    tfs_emulate_id2_index = options.id2_index;
#endif

    aos_init();

    start_app();

    aos_start();

    return ret;
}

