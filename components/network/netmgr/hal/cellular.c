/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>

#include "network/hal/cellular.h"

static AOS_DLIST_HEAD(g_cellular_module);

hal_cellular_module_t *hal_cellular_get_default_module(void)
{
    hal_cellular_module_t *m = 0;

    if (dlist_empty(&g_cellular_module)) {
        return 0;
    }

    m = dlist_first_entry(&g_cellular_module, hal_cellular_module_t, base.list);

    return m;
}

void hal_cellular_register_module(hal_cellular_module_t *module)
{
    dlist_add_tail(&module->base.list, &g_cellular_module);

}

void hal_cellular_install_event(hal_cellular_module_t *m, const hal_cellular_event_cb_t *cb)
{
    if (NULL == m)
        return;
    m->ev_cb = cb;
}

int hal_cellular_init(void)
{
    int          err = -1;
    dlist_t *t;

    /* do low level init */
    dlist_for_each(t, &g_cellular_module) {
        hal_cellular_module_t *m = (hal_cellular_module_t *)t;
        if(m && m->init) {
            err = m->init(m);
            if(err != 0) {
                return err;
            }
        }
    }

    return err;
}

int hal_cellular_start(hal_cellular_module_t *m, hal_cellular_init_type_t *init_para)
{
    int err = -1;

    if(m == NULL) {
        m = hal_cellular_get_default_module();
    }

    if(m && m->start) {
        err = m->start(m);
    }

    return err;
}

int hal_cellular_get_ip_stat(hal_cellular_module_t *m, hal_cellular_ip_stat_t *out_net_para)
{
    int err = -1;

    if(m == NULL) {
        m = hal_cellular_get_default_module();
    }

    if(m && m->get_ip_stat) {
        err = m->get_ip_stat(m, out_net_para);
    }

    return err;
}
