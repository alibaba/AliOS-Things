/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>

#include "aos/kernel.h"
#include "ulog/ulog.h"
#include "aos/yloop.h"

#include "aos/hal/gpio.h"

#include <network/network.h>

#ifdef AOS_NETMGR
#include "netmgr.h"
#endif

#include <umesh.h>

#ifdef MESHAPP_LIGHT_ENABLED
static gpio_dev_t gpio_led;
static gpio_dev_t gpio_light;

static aos_timer_t turn_off_timer = {NULL};

static void subscribed_event_cb(input_event_t *eventinfo, void *priv_data);
static void turn_off_light(void);

static void turn_off_timer_fn(void *arg1, void *arg2)
{
    turn_off_light();
}

void light_init(void)
{
    gpio_light.port = 8;
    gpio_light.config = OUTPUT_PUSH_PULL;
    hal_gpio_init(&gpio_light);
    hal_gpio_output_low(&gpio_light);

    gpio_led.port = 12;
    gpio_led.config = OUTPUT_PUSH_PULL;
    hal_gpio_init(&gpio_led);
    hal_gpio_output_high(&gpio_led);

    aos_timer_new(&turn_off_timer, turn_off_timer_fn, NULL, 2000, 0);
    aos_register_event_filter(EV_MESH, subscribed_event_cb, NULL);
}

static void turn_on_light(void)
{
    hal_gpio_output_low(&gpio_led);
    hal_gpio_output_high(&gpio_light);
    aos_timer_stop(&turn_off_timer);
    aos_timer_start(&turn_off_timer);
}

static void turn_off_light(void)
{
    hal_gpio_output_high(&gpio_led);
    hal_gpio_output_low(&gpio_light);
}

static void subscribed_event_cb(input_event_t *eventinfo, void *priv_data)
{
    if (eventinfo->type != EV_MESH) {
        return;
    }

    if (eventinfo->code == CODE_MESH_CONNECTED) {
        turn_on_light();
    } else if (eventinfo->code == CODE_MESH_DISCONNECTED) {
        turn_off_light();
    } else if (eventinfo->code == CODE_MESH_DATA_RECV) {
        turn_on_light();
    }
}
#endif

static void app_delayed_action(void *arg)
{
#ifdef AOS_NETMGR
    netmgr_init();
    netmgr_start(false);
#endif
}

static void app_main_entry(void *arg)
{
    aos_post_delayed_action(1000, app_delayed_action, arg);
    aos_loop_run();
}

int application_start(int argc, char **argv)
{
    const char *mode = argc > 1 ? argv[1] : "";

    aos_set_log_level(AOS_LL_DEBUG);

    aos_task_new("meshappmain", app_main_entry, NULL, 8192);
#ifdef MESHAPP_LIGHT_ENABLED
    light_init();
#endif
    return 0;
}

