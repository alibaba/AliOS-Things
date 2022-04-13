/*
 * Copyright (C) 2021-2023 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include "ulog/ulog.h"
#include "aos/kernel.h"
#include "ucamera_service.h"
#include "ucamera_device.h"

#define TAG "UCAMERA_SERVICE"

static ucamera_device_t *g_ucamera_dev = NULL;

/* Available camera context */
static ucamera_context_t *camera_ctx[] = {
    &wifi_camera,
    NULL
};

/*
 * Initialize the ucamera device
 */
ucamera_device_t *ucamera_device_init(const char *dev_name)
{
    ucamera_device_t *device;
    int32_t index;
    int32_t i;

    /*Reset camera service*/
    if (g_ucamera_dev != NULL)
        ucamera_device_uninit();

    /* Select the proper device driver */
    index = 0;
    device = NULL;
    if (dev_name == NULL)
        return NULL;

    if (dev_name != NULL) {
        for (i = 0; camera_ctx[i]; ++i) {
            if (strncasecmp(camera_ctx[i]->name, dev_name, strlen(dev_name)) == 0) {
                if (camera_ctx[i]->available()) {
                    device = camera_ctx[i]->create(index);
                    break;
                }
            }
        }
    } else {
        for (i = 0; camera_ctx[i]; ++i) {
            if (camera_ctx[i]->available()) {
                device = camera_ctx[i]->create(index);
                if (device != NULL)
                    break;

            }
        }
    }
    if (device == NULL) {
        if (dev_name) {
            LOGE(TAG, "%s not available", dev_name);
            return NULL;
        }
        LOGE(TAG, "No available device device");
        return NULL;
    }
    g_ucamera_dev = device;
    g_ucamera_dev->name = camera_ctx[i]->name;

    /* Initialize the camera device */
    if (g_ucamera_dev->camera_init(g_ucamera_dev) < 0) {
        ucamera_device_uninit();
        return NULL;
    }

    return g_ucamera_dev;
}

/*
 * Uninitialize the ucamera device
 */
void ucamera_device_uninit(void)
{
    int32_t i, j;

    if (!g_ucamera_dev)
        return;

    g_ucamera_dev->camera_uninit(g_ucamera_dev);
    g_ucamera_dev->camera_free(g_ucamera_dev);
    g_ucamera_dev = NULL;

    return;
}

const char *ucamera_get_device_name(void)
{
    if (!g_ucamera_dev)
        return NULL;

    return g_ucamera_dev->name;
}

ucamera_device_t *ucamera_get_device(void)
{
    return g_ucamera_dev;
}
