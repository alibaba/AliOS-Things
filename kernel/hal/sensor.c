/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stddef.h>
#include <hal/hal.h>

static AOS_DLIST_HEAD(g_sensor_module);

void hal_sensor_register_module(hal_sensor_module_t *module)
{
    dlist_add_tail(&module->base.list, &g_sensor_module);
}

int  hal_sensor_get_sensor_list(hal_sensor_module_t *m, sensor_node_t const  **list)
{
    if (m == NULL) {
        m = hal_sensor_get_default_module();
    }

    if (m != NULL && m->get_sensor_list != NULL) {
        return m->get_sensor_list(m, list);
    }

    return -1;
}

int  hal_sensor_enable(hal_sensor_module_t *m, sensor_type type)
{
    if (m == NULL) {
        m = hal_sensor_get_default_module();
    }

    if (m != NULL && m->enable != NULL) {
        return m->enable(m, type);
    }

    return -1;
}

int  hal_sensor_disable(hal_sensor_module_t *m, sensor_type type)
{
    if (m == NULL) {
        m = hal_sensor_get_default_module();
    }

    if (m != NULL && m->disable != NULL) {
        return m->disable(m, type);
    }

    return -1;
}

int  hal_sensor_read(hal_sensor_module_t *m, sensor_type type, char *buf, int buf_size)
{
    if (m == NULL) {
        m = hal_sensor_get_default_module();
    }

    if (m != NULL && m->read != NULL) {
        return m->read(m, type, buf, buf_size);
    }

    return -1;
}

hal_sensor_module_t *hal_sensor_get_default_module(void)
{
    hal_sensor_module_t *m = NULL;

    if (dlist_empty(&g_sensor_module)) {
        return NULL;
    }

    m = dlist_first_entry(&g_sensor_module, hal_sensor_module_t, base.list);

    return m;
}

hal_sensor_module_t *hal_sensor_get_next_module(hal_sensor_module_t *m)
{
    if (!m) {
        return NULL;
    }
    return dlist_entry(m->base.list.next, hal_sensor_module_t, base.list);
}

int hal_sensor_init(void)
{
    int          err = 0;
    dlist_t *t;

    /* do low level init */
    dlist_for_each(t, &g_sensor_module) {
        hal_sensor_module_t *m = (hal_sensor_module_t *)t;
        m->init(m, NULL);
    }

    return err;
}

