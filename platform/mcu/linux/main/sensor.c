/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <errno.h>

#include "aos/kernel.h"

static const char *TAG      = "yoc_sensor";

static const sensor_node_t g_sensor_list[] = {
    {SENSOR_TYPE_TEMPERATURE,"tempearture"}
};

static int hce_sensor_init(hal_sensor_module_t *m, void *something)
{
    LOGI(TAG, "hce_sensor_init ");
    return 0;
}

static int hce_sensor_get_sensor_list(hal_sensor_module_t *m, sensor_node_t const **list)
{
    *list = g_sensor_list;
    return sizeof g_sensor_list / sizeof(sensor_node_t);
}

static int hce_sensor_enable(hal_sensor_module_t *m, sensor_type type)
{
    LOGI(TAG, "hce_sensor_enable handle= %d!", type);
    return 0;
}

static int hce_sensor_disable(hal_sensor_module_t *m, sensor_type type)
{
    LOGI(TAG, "hce_sensor_disable handle= %d!", type);
    return 0;
}

static int hce_sensor_read(hal_sensor_module_t *m, sensor_type type, char *buf, int buf_size)
{
    LOGI(TAG, "hce_sensor_read handle= %d!", type);
    return 0;
}

hal_sensor_module_t linuxhost_sensor_module = {
    .base.name          = "yoc_sensor_module_linuxhost",
    .init               = hce_sensor_init,
    .get_sensor_list    = hce_sensor_get_sensor_list,
    .enable             = hce_sensor_enable,
    .disable            = hce_sensor_disable,
    .read               = hce_sensor_read,
};
