/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <hal/net.h>

static AOS_DLIST_HEAD(g_net_module);

hal_net_module_t *hal_net_default_module(void)
{
    hal_net_module_t *m = 0;

    if (dlist_empty(&g_net_module)) {
        return 0;
    }

    m = dlist_first_entry(&g_net_module, hal_net_module_t, base.list);
    return m;
}


void hal_net_register_module(hal_net_module_t *module)
{
    dlist_add_tail(&module->base.list, &g_net_module);
}

int hal_net_init(void)
{
    int err = 0;
    dlist_t *t;

    dlist_for_each(t, &g_net_module) {
        hal_net_module_t *m = (hal_net_module_t *)t;
        m->init(m);
    }

    return err;
}

int hal_net_get_mac_addr(hal_net_module_t *m, uint8_t *mac)
{
    if (m == NULL) {
        m = hal_net_get_default_module();
    }

    if (m && m->get_mac_addr) {
        m->get_mac_addr(m, mac);
        return 0;
    }

    return -1;
}
