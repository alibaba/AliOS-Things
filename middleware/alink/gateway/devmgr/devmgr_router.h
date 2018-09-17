/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __DEVMGR_ROUTER__H__
#define __DEVMGR_ROUTER__H__

#include <unistd.h>
#ifdef __cplusplus
extern "C" {
#endif

#define METHOD_DEVICE_REGISTER_SUB_WIFI      "registerSubDevice"
#define METHOD_DEVICE_UNREGISTER_SUB_WIFI    "unregisterDevice"
#define METHOD_DEVICE_ATTACH_SUB_WIFI        "attachSubDevice"
#define METHOD_DEVICE_DETACH_SUB_WIFI        "detachSubDevice"

#define PARAMS_DEVICE_REGISTER_SUB_WIFI_FMT  ("{\"name\":\"%s\","\
                                               "\"model\":\"%s\","\
                                               "\"type\":\"%s\","\
                                               "\"category\":\"%s\","\
                                               "\"sn\":\"\","\
                                               "\"mac\":\"%s\","\
                                               "\"manufacturer\":\"%s\","\
                                               "\"version\":\"\"}")

#define PARAMS_DEVICE_ATTACH_SUB_WIFI_FMT "{\"uuid\":\"%s\", \"clients\":[\"%s\"]}"
#define PARAMS_DEVICE_DETACH_SUB_WIFI_FMT "{\"uuid\":\"%s\", \"clients\":[\"%s\"]}"

int router_network_up_event_handler(dev_info_t *devinfo);
int router_network_down_event_handler(dev_info_t *devinfo);
int router_link_state_event_handler(dev_info_t *devinfo, link_state_t state);

int devmgr_router_init();
void devmgr_router_exit();

#ifdef __cplusplus
}
#endif
#endif



