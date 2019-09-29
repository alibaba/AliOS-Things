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
#ifdef CONFIG_AOS_DDA
#include <stdlib.h>
#include <dda.h>
#endif

#ifdef AOS_NETMGR
#include "netmgr.h"
#endif

#include <umesh.h>
#include "umesh_hal.h"


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

#ifdef MESH_HAL_TEST_ENABLED
static uint16_t g_interval = 1000;  /* ms */
static mac_address_t g_dest = {.addr = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}, .len = 0};
//static mac_address_t g_dest = {.addr = {0xfc, 0xee, 0xe6, 0x05, 0x72, 0xbd}, .len = 8};

static void handle_received_frame(void *context, frame_t *frame,
                                  frame_info_t *frame_info,
                                  int error)
{
    mac_address_t *addr = &frame_info->peer;

    printf("Rx data from: %02x:%02x:%02x:%02x:%02x:%02x\r\n",
           addr->addr[0], addr->addr[1], addr->addr[2], addr->addr[3], addr->addr[4], addr->addr[5]);
}

static void handle_sent(void *context, frame_t *frame, int error);
static void send_data_task(void *arg)
{
    frame_t frame;
    frame.len = 0;

    if (g_dest.len == 8) {
        hal_umesh_send_ucast_request(NULL, &frame, &g_dest, handle_sent, NULL);
    } else {
        hal_umesh_send_bcast_request(NULL, &frame, handle_sent, NULL);
    }
}

static void handle_sent(void *context, frame_t *frame, int error)
{
    if (g_dest.len == 8) {
        printf("Tx to: %02x:%02x:%02x:%02x:%02x:%02x %s\r\n",
               g_dest.addr[0], g_dest.addr[1], g_dest.addr[2], \
               g_dest.addr[3], g_dest.addr[4], g_dest.addr[5], \
               error == 0? "success": "fail");
    } else {
        printf("Tx to: 0xff:0xff:0xff:0xff:0xff:0xff %s\r\n", error == 0? "success": "fail");
    }
    aos_post_delayed_action(g_interval, send_data_task, NULL);
}
#endif

static void app_delayed_action(void *arg)
{
#ifndef MESH_HAL_TEST_ENABLED
#ifdef AOS_NETMGR
    netmgr_init();
    netmgr_start(false);
#endif
#else
    hal_umesh_init();
    hal_umesh_enable(NULL);
    hal_umesh_register_receiver(NULL, handle_received_frame, NULL);
    hal_umesh_set_channel(NULL, 6);
    aos_post_delayed_action(g_interval, send_data_task, NULL);
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

    if (strcmp(mode, "--mesh-node") == 0) {
#ifdef CONFIG_AOS_DDA
        dda_enable(atoi(argv[argc-1]));
        dda_service_init();
        dda_service_start();
#endif
    }
    else if (strcmp(mode, "--mesh-master") == 0) {
#ifdef CONFIG_AOS_DDM
        ddm_run(argc, argv);
#endif
    }
    else {
        aos_task_new("meshappmain", app_main_entry, NULL, 8192);
#ifdef MESHAPP_LIGHT_ENABLED
        light_init();
#endif
    }

    return 0;
}

