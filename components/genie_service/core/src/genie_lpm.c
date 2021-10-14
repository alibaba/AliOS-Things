/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_MESH_DEBUG_GLP)
#include <stdio.h>
#include <string.h>
#include <ctype.h>
//#include <aos/aos.h>
#include <aos/kernel.h>
#include <hal/hal.h>
#include <aos/hal/gpio.h>
#include <genie_sal_lpm.h>
#include <genie_lpm.h>

#if defined(BOARD_TG7100B)
#include "pm.h"
#endif

#include "common/log.h"
#include "genie_service.h"

static genie_lpm_ctx_t genie_lpm_ctx;

#ifndef GENIE_WAKE_UP_IO_NUM_MAX
#define GENIE_WAKE_UP_IO_NUM_MAX 5
#endif

genie_lpm_wakeup_io_config_t g_wakeup_io[GENIE_WAKE_UP_IO_NUM_MAX];

static uint8_t g_wakeup_io_num = 0;

#ifndef CONFIG_GENIE_MESH_ENABLE_SLEEP_WHEN_NOT_PROV
#define CONFIG_GENIE_MESH_ENABLE_SLEEP_WHEN_NOT_PROV 0
#endif

static void _genie_lpm_timer_irq_handler(void *p_timer, void *args)
{
    int ret = 0;

    BT_DBG("[%u]cur status: %d\n", k_uptime_get_32(), genie_lpm_ctx.status);

    if (genie_lpm_ctx.has_disabled == 1)
    {
        BT_DBG("lpm already disable, stop timer\n");
        aos_timer_stop(&genie_lpm_ctx.wakeup_timer);
        return;
    }

    if (STATUS_WAKEUP == genie_lpm_ctx.status)
    {
        BT_DBG("[%u]sleep and suspend mesh stack\n", k_uptime_get_32());
        ret = genie_mesh_suspend(false);
        if (0 == ret || -EALREADY == ret)
        {
            genie_lpm_ctx.status = STATUS_SLEEP;
            if (!genie_lpm_ctx.p_config.genie_lpm_cb)
            {
                genie_lpm_ctx.p_config.genie_lpm_cb(WAKEUP_BY_TIMER, genie_lpm_ctx.status, NULL);
            }
            aos_timer_stop(&genie_lpm_ctx.wakeup_timer);
            aos_timer_change(&genie_lpm_ctx.wakeup_timer, genie_lpm_ctx.p_config.sleep_ms);
            aos_timer_start(&genie_lpm_ctx.wakeup_timer);
        }
        else
        {
            aos_timer_stop(&genie_lpm_ctx.wakeup_timer);
            aos_timer_change(&genie_lpm_ctx.wakeup_timer, genie_lpm_ctx.p_config.wakeup_ms);
            aos_timer_start(&genie_lpm_ctx.wakeup_timer);
        }
    }
    else if (STATUS_SLEEP == genie_lpm_ctx.status)
    {
        BT_DBG("[%u]wake up and resume mesh stack\n", k_uptime_get_32());
        ret = genie_mesh_resume();
        if (0 == ret || -EALREADY == ret)
        {
            genie_lpm_ctx.status = STATUS_WAKEUP;
            genie_lpm_ctx.p_config.genie_lpm_cb(WAKEUP_BY_TIMER, genie_lpm_ctx.status, NULL);
            aos_timer_stop(&genie_lpm_ctx.wakeup_timer);
            aos_timer_change(&genie_lpm_ctx.wakeup_timer, genie_lpm_ctx.p_config.wakeup_ms);
            aos_timer_start(&genie_lpm_ctx.wakeup_timer);
        }
        else
        {
            aos_timer_stop(&genie_lpm_ctx.wakeup_timer);
            aos_timer_change(&genie_lpm_ctx.wakeup_timer, genie_lpm_ctx.p_config.sleep_ms);
            aos_timer_start(&genie_lpm_ctx.wakeup_timer);
        }
    }
}
/*
static void _genie_lpm_io_wakeup_timer_irq_handler(void *p_timer, void *args)
{
    int ret = 0;

    BT_DBG("[%u]wakeup by i/o\n", k_uptime_get_32());
    aos_timer_stop(&genie_lpm_ctx.io_wakeup_timer);

    BT_DBG("[%u]cur status: %d\n", k_uptime_get_32(), genie_lpm_ctx.status);
    if (STATUS_SLEEP == genie_lpm_ctx.status)
    {
        BT_DBG("[%u]wake up and resume mesh stack\n", k_uptime_get_32());
        ret = genie_mesh_resume();
        if (0 == ret)
        {
            genie_lpm_ctx.status = STATUS_WAKEUP;
            genie_lpm_ctx.p_config.genie_lpm_cb(WAKEUP_BY_IO, genie_lpm_ctx.status);
        }
    }
    else
    {
        genie_lpm_ctx.p_config.genie_lpm_cb(WAKEUP_IS_WAKEUP, genie_lpm_ctx.status);
    }
}*/

__attribute__((section(".__sram.code"))) void genie_lpm_io_wakeup_handler(void *arg)
{
    if (genie_lpm_ctx.p_config.lpm_wakeup_io != 0)
    {
        uint8_t trigger_found = 0;
        _genie_lpm_io_status_t g_io_status[GENIE_WAKE_UP_IO_NUM_MAX];
        long long interrupt_time = aos_now_ms();

        if ((interrupt_time - genie_lpm_ctx.last_interrupt_time < INTERRUPT_DELAY_TIME))
        {
            return; //for debounce
        }

        genie_lpm_ctx.last_interrupt_time = interrupt_time;

        for (int i = 0; i < g_wakeup_io_num; i++)
        {
            bool status = genie_sal_sleep_wakeup_io_get_status(g_wakeup_io[i].port);
            if (((g_wakeup_io[i].io_pol == FALLING || g_wakeup_io[i].io_pol == ACT_LOW) && status == false) || ((g_wakeup_io[i].io_pol == RISING || g_wakeup_io[i].io_pol == ACT_HIGH) && status == true))
            {
                g_io_status[i].port = g_wakeup_io[i].port;
                g_io_status[i].trigger_flag = 1;
                g_io_status[i].status = status;
                trigger_found = 1;
            }
        }

        if (trigger_found)
        {
            BT_DBG("[%u]wakeup by i/o\n", k_uptime_get_32());
            BT_DBG("[%u]cur status: %d\n", k_uptime_get_32(), genie_lpm_ctx.status);
            if (STATUS_SLEEP == genie_lpm_ctx.status)
            {
                int ret = 0;
                BT_DBG("[%u]wake up and resume mesh stack\n", k_uptime_get_32());
                ret = genie_mesh_resume();
                if (0 == ret)
                {
                    genie_lpm_ctx.status = STATUS_WAKEUP;
                    _genie_lpm_io_status_list_t list;
                    list.size = g_wakeup_io_num;
                    list.io_status = g_io_status;
                    genie_lpm_ctx.p_config.genie_lpm_cb(WAKEUP_BY_IO, genie_lpm_ctx.status, &list);
                }
            }
            else
            {
                genie_lpm_ctx.p_config.genie_lpm_cb(WAKEUP_IS_WAKEUP, genie_lpm_ctx.status, NULL);
            }
        }
    }
}

static void _genie_lpm_io_wakeup_init(genie_lpm_wakeup_io_t config)
{
    if (config.io_list_size > GENIE_WAKE_UP_IO_NUM_MAX)
    {
        GENIE_LOG_ERR("Wakeup i/o num should no more than %d", GENIE_WAKE_UP_IO_NUM_MAX);
    }
    g_wakeup_io_num = GENIE_WAKE_UP_IO_NUM_MAX < config.io_list_size ? GENIE_WAKE_UP_IO_NUM_MAX : config.io_list_size;
    memcpy(g_wakeup_io, config.io_config, sizeof(genie_lpm_wakeup_io_config_t) * g_wakeup_io_num);

    for (int i = 0; i < g_wakeup_io_num; i++)
    {
        genie_sal_sleep_wakup_io_set(g_wakeup_io[i].port, g_wakeup_io[i].io_pol);
    }

    genie_sal_io_wakeup_cb_register(genie_lpm_io_wakeup_handler);
}

/*
static void _genie_lpm_io_wakeup_timer_init(void)
{
    aos_timer_new(&genie_lpm_ctx.io_wakeup_timer, _genie_lpm_io_wakeup_timer_irq_handler, NULL, 10, 0);
    aos_timer_stop(&genie_lpm_ctx.io_wakeup_timer);
}*/

static void _genie_lpm_wakeup_timer_init(void)
{
    aos_timer_new(&genie_lpm_ctx.wakeup_timer, _genie_lpm_timer_irq_handler, NULL, genie_lpm_ctx.p_config.delay_sleep_time, 0);
    aos_timer_stop(&genie_lpm_ctx.wakeup_timer);
}

int genie_lpm_disable(void)
{
    GENIE_LOG_INFO("lpm disable");

    if (genie_lpm_ctx.is_mesh_init == 1)
    {
        genie_mesh_resume();
    }

    genie_lpm_ctx.has_disabled = 1;
    genie_lpm_ctx.status = STATUS_WAKEUP;
    aos_timer_stop(&genie_lpm_ctx.wakeup_timer);
    genie_sal_sleep_disable();

    return 0;
}

int genie_lpm_enable(bool force)
{
    GENIE_LOG_INFO("lpm enable");

    if (genie_lpm_ctx.is_mesh_init == 1)
    {
        genie_mesh_suspend(force);
    }

    genie_lpm_ctx.has_disabled = 0;
    genie_sal_sleep_enable();
    genie_lpm_ctx.status = STATUS_SLEEP;

    if (force)
    {
        aos_timer_stop(&genie_lpm_ctx.wakeup_timer);
    }
    else
    {
        aos_timer_stop(&genie_lpm_ctx.wakeup_timer);
        aos_timer_change(&genie_lpm_ctx.wakeup_timer, genie_lpm_ctx.p_config.sleep_ms);
        aos_timer_start(&genie_lpm_ctx.wakeup_timer);
    }

    return 0;
}

int genie_lpm_deep_sleep(void)
{
    *(volatile uint32_t *)PWR_BOOT_REASON = PWR_STANDBY_BOOT_FLAG;
    return genie_sal_sleep_enter_standby();
}

void genie_lpm_init(genie_lpm_conf_t *lpm_conf)
{
    uint32_t *p_boot_reason = (uint32_t *)PWR_BOOT_REASON;

    memcpy(&genie_lpm_ctx.p_config, lpm_conf, sizeof(genie_lpm_conf_t));

    if (genie_lpm_ctx.p_config.genie_lpm_cb == NULL)
    {
        GENIE_LOG_ERR("lpm param err");
        return;
    }

    if (genie_lpm_ctx.p_config.delay_sleep_time == 0)
    {
        genie_lpm_ctx.p_config.delay_sleep_time = DEFAULT_BOOTUP_DELAY_SLEEP_TIME;
    }

    genie_lpm_ctx.status = STATUS_WAKEUP;

    GENIE_LOG_INFO("io_wakeup:%d, sleep:%dms, wakeup:%dms, after:%dms", genie_lpm_ctx.p_config.lpm_wakeup_io, genie_lpm_ctx.p_config.sleep_ms, genie_lpm_ctx.p_config.wakeup_ms, genie_lpm_ctx.p_config.delay_sleep_time);
    if (genie_lpm_ctx.p_config.lpm_wakeup_io != 0 && genie_lpm_ctx.p_config.lpm_wakeup_io_config.io_list_size != 0)
    {
        _genie_lpm_io_wakeup_init(lpm_conf->lpm_wakeup_io_config);
        //_genie_lpm_io_wakeup_timer_init(); //Do things in the timer when wakeup by IO
    }

    if ((genie_lpm_ctx.p_config.sleep_ms != 0) && (genie_lpm_ctx.p_config.wakeup_ms != 0))
    {
        _genie_lpm_wakeup_timer_init();
    }

#if !(CONFIG_GENIE_MESH_ENABLE_SLEEP_WHEN_NOT_PROV)
    genie_sal_sleep_disable();
#endif

    if (*p_boot_reason == PWR_STANDBY_BOOT_FLAG)
    {
        GENIE_LOG_INFO("standy boot flag");
        *(volatile uint32_t *)PWR_BOOT_REASON = 0;
    }
}

int genie_lpm_start(void)
{
    genie_lpm_ctx.is_mesh_init = 1;
    BT_DBG("genie lpm mesh ready");
    if (genie_lpm_ctx.p_config.is_auto_enable == 1)
    {
        genie_lpm_ctx.has_disabled = 0;
        genie_sal_sleep_enable();
        genie_lpm_ctx.status = STATUS_SLEEP;
        aos_timer_stop(&genie_lpm_ctx.wakeup_timer);
        aos_timer_start(&genie_lpm_ctx.wakeup_timer);
    }
    else
    {
        if (genie_lpm_ctx.p_config.delay_sleep_time > 0)
        {
            genie_lpm_ctx.has_disabled = 0;
            aos_timer_start(&genie_lpm_ctx.wakeup_timer);
        }
    }

    return 0;
}
