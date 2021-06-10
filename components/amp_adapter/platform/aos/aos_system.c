/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "k_api.h"
#include "aos/kernel.h"
#include "aos/kv.h"
#include "aos/vfs.h"
#include "ulog/ulog.h"
#include "netmgr.h"

#include "netmgr_wifi.h"

//#include "infra_config.h"
//#include "infra_compat.h"
//#include "infra_defs.h"
//#include "wrappers_defs.h"
#include "aos_system.h"

#ifdef BOARD_HAAS200
#include "vfsdev/wifi_dev.h"
#endif

#define _SYSINFO_DEVICE_NAME "AliOS Things"
#define SYSINFO_VERSION "0.0.1"

void aos_printf(const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    fflush(stdout);
}

int aos_snprintf(char *str, const int len, const char *fmt, ...)
{
    va_list args;
    int     rc;

    va_start(args, fmt);
    rc = vsnprintf(str, len, fmt, args);
    va_end(args);

    return rc;
}

int aos_vsnprintf(char *str, const int len, const char *format, va_list ap)
{
    return vsnprintf(str, len, format, ap);
}

const char *aos_get_system_version(void)
{
    return SYSINFO_VERSION;
}

const char *aos_get_platform_type(void)
{
    return _SYSINFO_DEVICE_NAME;
}

int aos_get_ip(char *ip)
{
    netmgr_hdl_t hdl;
    netmgr_ifconfig_info_t info = {0};

    hdl = netmgr_get_dev("/dev/wifi0");
    if(hdl < 0) {
        return -1;
    }

    if(netmgr_get_ifconfig(hdl, &info) < 0) {
        return -1;
    }

    if (0 == strcmp(info.ip_addr, "0.0.0.0")) {
        return -1;
    }
    return 0;
}

int aos_get_mac_addr(unsigned char mac[8])
{
    // TODO: ?
    return 0;
}


int aos_network_status_registercb(void (*cb)(int status, void *), void *arg)
{
    return aos_wifi_set_msg_cb(cb);
}

int aos_get_network_status(void)
{
    int ret = 0;
    ret = aos_wifi_get_state();

    return (ret == CONN_STATE_NETWORK_CONNECTED);
}

/**
 * @brief get wifi mac address
 * @param[out] mac: mac address，format: "01:23:45:67:89:ab"
 * @param[in] len: size of mac
 * @returns 0: succeed; -1: failed
 */
#ifndef MAC2STR
#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"
#endif
int hal_wifi_get_mac_address(char *mac, size_t len)
{
    int ret = 0;
#if (defined(BOARD_HAAS100) || defined(BOARD_HAASEDUK1))

    extern uint8_t* factory_section_get_wifi_address(void);
    uint8_t *_mac = factory_section_get_wifi_address();
    if(_mac == NULL)
        ret = -1;
    else
        snprintf(mac, len, MACSTR, MAC2STR(_mac));
#elif defined(BOARD_HAAS200)
    int fd;
    uint8_t _mac[16];
    fd = open("/dev/wifi0", O_RDWR);
    if (fd < 0) {
        return -1;
    }

    ioctl(fd, WIFI_DEV_CMD_GET_MAC, (unsigned long)_mac);
    snprintf(mac, len, MACSTR, MAC2STR(_mac));
    close(fd);
#endif
    return ret;
}

char g_chip_id[32];
const char *aos_get_device_name(void)
{
    hal_wifi_get_mac_address(g_chip_id, sizeof(g_chip_id));
    return g_chip_id;
}

extern k_mm_head *g_kmm_head;
int amp_heap_memory_info(amp_heap_info_t *heap_info)
{
    heap_info->heap_total = g_kmm_head->free_size + g_kmm_head->used_size;
    heap_info->heap_used = g_kmm_head->used_size;
    heap_info->heap_free = g_kmm_head->free_size;
    return 0;
}

int aos_system_init(void)
{
    return 0;
}

const char *aos_get_module_hardware_version(void)
{
    return "Module_Hardware_version";
}

const char *aos_get_module_software_version(void)
{
    return "Module_Software_version";
}
