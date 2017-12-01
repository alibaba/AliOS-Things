/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <aos/aos.h>
#ifdef WITH_LWIP
#include <lwip/netdb.h>
#include <lwip/sockets.h>
#define WITH_LWIP 1
#endif
#include <hal/hal.h>
#ifdef CONFIG_AOS_MESH
#include <umesh.h>
#define CONFIG_AOS_MESH 1
#include <ip/lwip_adapter.h>
#endif
#ifdef MBEDTLS_IN_KERNEL
#include <aos/mbedtls_ssl.h>
#include <ali_crypto.h>
#define MBEDTLS_IN_KERNEL 1
#endif


extern void hal_wlan_register_mgnt_monitor_cb(hal_wifi_module_t *m, monitor_data_cb_t fn);
extern int  hal_wlan_send_80211_raw_frame(hal_wifi_module_t *m, uint8_t *buf, int len);

void *sys_aos_malloc(unsigned int size, size_t allocator)
{
    void *tmp = aos_malloc(size);

    if (tmp == NULL) {
        return NULL;
    }

#if (RHINO_CONFIG_MM_DEBUG > 0u && RHINO_CONFIG_GCC_RETADDR > 0u)
    if ((size & AOS_UNSIGNED_INT_MSB) == 0) {
        krhino_owner_attach(g_kmm_head, tmp, allocator);
    }
#endif

    return tmp;
}
EXPORT_SYMBOL_K(RHINO_CONFIG_MM_DEBUG > 0u && RHINO_CONFIG_GCC_RETADDR > 0u, sys_aos_malloc, \
    "void *sys_aos_malloc(unsigned int size, size_t allocator)")

void *sys_aos_realloc(void *mem, unsigned int size, size_t allocator)
{
    void *tmp = aos_realloc(mem, size);

    if (tmp == NULL) {
        return NULL;
    }

#if (RHINO_CONFIG_MM_DEBUG > 0u && RHINO_CONFIG_GCC_RETADDR > 0u)
    if ((size & AOS_UNSIGNED_INT_MSB) == 0) {
        krhino_owner_attach(g_kmm_head, tmp, allocator);
    }
#endif

    return tmp;
}
EXPORT_SYMBOL_K(RHINO_CONFIG_MM_DEBUG > 0u && RHINO_CONFIG_GCC_RETADDR > 0u, sys_aos_realloc, \
    "void *sys_aos_realloc(void *mem, unsigned int size, size_t allocator)")

void *sys_aos_zalloc(unsigned int size, size_t allocator)
{
    void *tmp = aos_zalloc(size);

    if (tmp == NULL) {
        return NULL;
    }

#if (RHINO_CONFIG_MM_DEBUG > 0u && RHINO_CONFIG_GCC_RETADDR > 0u)
    if ((size & AOS_UNSIGNED_INT_MSB) == 0) {
        krhino_owner_attach(g_kmm_head, tmp, allocator);
    }
#endif

    return tmp;
}
EXPORT_SYMBOL_K(RHINO_CONFIG_MM_DEBUG > 0u && RHINO_CONFIG_GCC_RETADDR > 0u, sys_aos_zalloc, \
    "void *sys_aos_zalloc(unsigned int size, size_t allocator)")

int get_errno(void)
{
    return errno;
}
EXPORT_SYMBOL_K(1, get_errno, "int get_errno(void)")

void set_errno(int err)
{
    errno = err;
}
EXPORT_SYMBOL_K(1, set_errno, "void set_errno(int err)")


/* for syscall_ktbl.h, on the last */
#include <syscall_ktbl.h>

