/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <unistd.h>
#include "../miio-discover.h"

static void on_discover(void *priv, long device_id) {
    printf("discover device %ld\n", device_id);
}

void main() {
    miio_device_discover(60, NULL, on_discover);
    while (1) {
        sleep(1);
    }
}
