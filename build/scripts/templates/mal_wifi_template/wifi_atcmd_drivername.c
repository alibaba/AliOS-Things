/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "network/hal/base.h"
#include <hal/wifi.h>

/* handle wifi status event */
void at_wevent_handler(void *arg, char *buf, int buflen)
{
}

/* init wifi module */
static int wifi_init(hal_wifi_module_t *m)
{
    return 0;
};

/* get mac address from wifi module */
static void wifi_get_mac_addr(hal_wifi_module_t *m, uint8_t *mac)
{
};

/* connect wifi AP */
static int wifi_start(hal_wifi_module_t *m, hal_wifi_init_type_t *init_para)
{
    return 0;
}

/* connect wifi AP with more parameters */
static int wifi_start_adv(hal_wifi_module_t *m, hal_wifi_init_type_adv_t *init_para_adv)
{
    return 0;
}

/* get ip address from wifi module */
static int get_ip_stat(hal_wifi_module_t *m, hal_wifi_ip_stat_t *out_net_para, hal_wifi_type_t wifi_type)
{
    return 0;
}

hal_wifi_module_t aos_wifi_module_@drivername@ = {
    .base.name           = "aos_wifi_module_@drivername@",
    .init                =  wifi_init,
    .get_mac_addr        =  wifi_get_mac_addr,
    .start               =  wifi_start,
    .start_adv           =  wifi_start_adv,
    .get_ip_stat         =  get_ip_stat,
};
