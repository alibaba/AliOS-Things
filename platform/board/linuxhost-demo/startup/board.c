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
#include <sys/types.h>
#include <unistd.h>

#include "aos/init.h"
#include "aos/kernel.h"
#include "ulog/ulog.h"
#include "aos/hal/uart.h"

#ifndef CONFIG_OSAL_POSIX
#include <k_api.h>
#endif

#define TAG "main"

extern void cpu_tmr_sync(void);
extern void board_kinit_init(kinit_t* init_args);
extern void aos_lwip_features_init(void);
extern void hw_start_wifi_hal(void);
extern int board_getcli_args(void);

uart_dev_t uart_0;

int board_cli_init(void)
{
    return 0;
}

static void exit_clean(void)
{
    char fn[64] = {0};
    snprintf(fn, sizeof(fn), "rm -f ./aos_partition_%d_*", getpid());
    system(fn);
}

#define ARCH_MAX_NOFILE 64
int setrlimit_for_vfs(void)
{
    int           ret;
    struct rlimit rlmt;

    getrlimit(RLIMIT_NOFILE, &rlmt);
    if (rlmt.rlim_cur > ARCH_MAX_NOFILE) {
        rlmt.rlim_cur = ARCH_MAX_NOFILE;
        ret = setrlimit(RLIMIT_NOFILE, &rlmt);
        if (ret != 0) {
            LOGE(TAG, "setrlimit error: %s", strerror(errno));
            return ret;
        }
    }
    LOGD(TAG, "Limit max open files to %d", (int)rlmt.rlim_cur);

    return 0;
}

#if defined(CONFIG_AOS_DDA) || defined(ENABLE_GCOV)
void signal_handler(int signo)
{
    LOGD(TAG, "Received signal %d\n", signo);

#ifdef ENABLE_GCOV
    __gcov_flush();
#endif

#ifdef CONFIG_AOS_DDA
    rl_free_line_state ();
    rl_cleanup_after_signal ();
#endif

    exit(0);
}
#endif

/**
  * @general board init entry board_basic_init
  * @retval None
  */
int board_basic_init(void)
{
    int ret = 0;

    setvbuf(stdout, NULL, _IONBF, 0);

#if defined(CONFIG_AOS_DDA) || defined(ENABLE_GCOV)
    signal(SIGINT, signal_handler);
#endif
#ifdef CONFIG_AOS_UT
    signal(SIGPIPE, SIG_IGN);
#endif

    atexit(exit_clean);

    ret = setrlimit_for_vfs();
    if (ret != 0) {
        return ret;
    }
    return 0;
}

void board_tick_init(void)
{
#ifndef CONFIG_OSAL_POSIX
    cpu_tmr_sync();
#endif
}

void board_stduart_init(void)
{
    int cli_enable;
    cli_enable = board_getcli_args();
    (void)cli_enable;

    uart_0.port                = 0;
    uart_0.config.baud_rate    = 921600;
    uart_0.config.data_width   = DATA_WIDTH_8BIT;
    uart_0.config.parity       = NO_PARITY;
    uart_0.config.stop_bits    = STOP_BITS_1;
    uart_0.config.flow_control = FLOW_CONTROL_DISABLED;

#ifdef AOS_COMP_CLI
    if (cli_enable){
        hal_uart_init(&uart_0);
    }
#endif

}

void board_wifi_init(void)
{
    aos_lwip_features_init();
    hw_start_wifi_hal();
}

void board_network_init(void)
{
    board_wifi_init();
}

void board_gpio_init(void)
{

}

/**
  * Enable DMA controller clock
  */
void board_dma_init(void)
{

}


