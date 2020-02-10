/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

/**
 *                      caution
 * linuxhost hw.c won't use any lwip functionalities,
 * disable WITH_LWIP to avoid close() -> lwip_close()
 */
#undef WITH_LWIP

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "aos/kernel.h"

#include "aos/hal/timer.h"
#include "aos/hal/flash.h"
#include "aos/hal/uart.h"
#include "aos/hal/wdg.h"

#include "network/hal/base.h"
#include <hal/wifi.h>
#include <arg_options.h>

#define TAG "hw"

static bool per_pid_flash;
static int open_flash(int pno, bool w)
{
    char fn[64];
    int flash_fd;

    if (per_pid_flash)
        snprintf(fn, sizeof(fn), "./aos_partition_%d_%d.bin", getpid(), pno);
    else
        snprintf(fn, sizeof(fn), "./aos_partition_%d.bin", pno);

    if(w)
        flash_fd = open(fn, O_RDWR);
    else
        flash_fd = open(fn, O_RDONLY);

    if (flash_fd >= 0) {
        goto out;
    }

    umask(0111);
    flash_fd = creat(fn, S_IRWXU | S_IRWXG);
    if (flash_fd < 0)
        goto out;

    close(flash_fd);
    flash_fd = open(fn, O_RDWR);
out:
    return flash_fd;
}

int32_t hal_flash_write(hal_partition_t pno, uint32_t* poff, const void* buf ,uint32_t buf_size)
{
    int ret;
    char *tmp, *new_val;

    int flash_fd = open_flash(pno, true);
    if (flash_fd < 0)
        return -1;

    char *origin = (char *)malloc(buf_size);
    if (!origin) {
        ret = -1;
        goto exit;
    }
    memset(origin, -1, buf_size);

    ret = pread(flash_fd, origin, buf_size, *poff);
    if (ret < 0) {
        perror("error reading flash:");
        goto exit;
    }

    tmp = origin;
    new_val = (char *)buf;
    for (int i = 0; i < buf_size; i++) {
        (*tmp) &= (*new_val);
        new_val++;
        tmp++;
    }

    ret = pwrite(flash_fd, origin, buf_size, *poff);
    if (ret < 0)
        perror("error writing flash:");
    else
        *poff += ret;

exit:
    close(flash_fd);
    free(origin);
    return ret < 0 ? ret : 0;
}

int32_t hal_flash_read(hal_partition_t pno, uint32_t* poff, void* buf, uint32_t buf_size)
{
    int flash_fd;

    if (poff == NULL)
        return -1;

    flash_fd = open_flash(pno, false);
    if (flash_fd < 0)
        return -1;

    int ret = pread(flash_fd, buf, buf_size, *poff);
    if (ret < 0)
        perror("error reading flash:");
    else
        *poff += ret;
    close(flash_fd);

    return ret < 0 ? ret : 0;
}

int32_t hal_flash_erase(hal_partition_t in_partition, uint32_t off_set,
                        uint32_t size)
{
    int ret;
    int flash_fd = open_flash(in_partition, true);
    if (flash_fd < 0)
        return -1;

    char *buf = (char *)malloc(size);
    if (!buf) {
        ret = -1;
        goto exit;
    }
    memset(buf, -1, size);

    ret = pwrite(flash_fd, buf, size, off_set);
    if (ret < 0)
        perror("error erase flash:");

exit:
    close(flash_fd);
    free(buf);
    return ret < 0 ? ret : 0;
}

int32_t hal_flash_dis_secure(hal_partition_t partition, uint32_t off_set, uint32_t size)
{
    return 0;
}

int32_t hal_flash_info_get(hal_partition_t pno, hal_logic_partition_t *partition)
{
    return 0;
}


void hal_reboot(void)
{

}

int32_t hal_wdg_init(wdg_dev_t *wdg)
{
    return 0;
}

void hal_wdg_reload(wdg_dev_t *wdg)
{
}

int32_t hal_wdg_finalize(wdg_dev_t *wdg)
{
    return 0;
}

#ifdef OSAL_RHINO
#include <k_api.h>
#define us2tick(us) \
    ((us * RHINO_CONFIG_TICKS_PER_SECOND + 999999) / 1000000)

static void _timer_cb(void *timer, void *arg)
{
    timer_dev_t *tmr = arg;
    tmr->config.cb(tmr->config.arg);
}

int32_t hal_timer_init(timer_dev_t *tim)
{
    if (tim->config.reload_mode == TIMER_RELOAD_AUTO) {
        krhino_timer_dyn_create((ktimer_t **)&tim->priv, "hwtmr", _timer_cb,
                                us2tick(tim->config.period), us2tick(tim->config.period), tim, 0);
    }
    else {
        krhino_timer_dyn_create((ktimer_t **)&tim->priv, "hwtmr", _timer_cb,
                                us2tick(tim->config.period), 0, tim, 0);
    }
    return 0;
}

int hal_timer_start(timer_dev_t *tmr)
{
    return krhino_timer_start(tmr->priv);
}

void hal_timer_stop(timer_dev_t *tmr)
{
    krhino_timer_stop(tmr->priv);
    krhino_timer_dyn_del(tmr->priv);
    tmr->priv = NULL;
}

int csp_printf(const char *fmt, ...)
{
    CPSR_ALLOC();

    va_list args;
    int ret;

    RHINO_CRITICAL_ENTER();

    va_start(args, fmt);
    ret = vprintf(fmt, args);
    va_end(args);

    fflush(stdout);

    RHINO_CRITICAL_EXIT();

    return ret;
}
#else
int csp_printf(const char *fmt, ...)
{
    va_list args;
    int ret;

    va_start(args, fmt);
    ret = vprintf(fmt, args);
    va_end(args);

    fflush(stdout);

    return ret;
}
#endif

#if defined(DEV_SAL_MK3060)
extern hal_wifi_module_t aos_wifi_module_mk3060;
#elif defined(DEV_SAL_ATHOST)
extern hal_wifi_module_t aos_wifi_module_athost;
#else
extern hal_wifi_module_t sim_aos_wifi_linux;
#endif
uart_dev_t uart_0;

void linux_wifi_register(void);
void hw_start_hal(options_t *poptions)
{
    uart_0.port                = 0;
    uart_0.config.baud_rate    = 921600;
    uart_0.config.data_width   = DATA_WIDTH_8BIT;
    uart_0.config.parity       = NO_PARITY;
    uart_0.config.stop_bits    = STOP_BITS_1;
    uart_0.config.flow_control = FLOW_CONTROL_DISABLED;

    per_pid_flash = poptions->flash.per_pid;

#ifdef AOS_COMP_CLI
    if (poptions->cli.enable)
        hal_uart_init(&uart_0);
#endif

#if defined(DEV_SAL_MK3060)
    hal_wifi_register_module(&aos_wifi_module_mk3060);
#elif defined(DEV_SAL_ATHOST)
    hal_wifi_register_module(&aos_wifi_module_athost);
#else
    hal_wifi_register_module(&sim_aos_wifi_linux);
#endif
}
