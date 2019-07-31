/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <arpa/inet.h>
#include <assert.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include "../miio-device.h"

void on_event(void *priv, const char *event)
{
    printf("receive event: %s\n", event);
}

int main(void)
{
    /* power switch */
    /* miio_device_t *device = miio_device_create("192.168.8.192",
     * "a3605b0d49c7da8c2e88b8d71233c3bd"); */

    /* light */
    miio_device_t *device =
        miio_device_create("192.168.8.181", "3da77b4d20fd2f5c911b90983a5dcf02");
    miio_device_control(device, "set_power", "[\"off\"]", NULL);

    /* gateway */
    miio_device_t *gateway =
        miio_device_create("192.168.8.219", "61acba51e12e5e57c14eb9ec50b06e45");

    char *device_list = miio_device_control(
        gateway, "get_device_prop", "[\"lumi.0\",\"device_list\"]", NULL);

    /* get temperature */
    miio_device_control(
        gateway, "get_device_prop_exp",
        "[[\"lumi.158d0002100f9e\",\"temperature\",\"humidity\"]]", NULL);

    miio_device_set_event_cb(gateway, on_event, NULL);

    while (1) {
        sleep(2);
    }
}
