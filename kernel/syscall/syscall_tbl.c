/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <aos/aos.h>
#ifdef WITH_LWIP
#include <lwip/netdb.h>
#include <lwip/sockets.h>
#endif
#include <hal/hal.h>
#ifdef CONFIG_AOS_MESH
#include <umesh.h>
#endif
#ifdef MBEDTLS_IN_KERNEL
#include <aos/mbedtls_ssl.h>
#include <ali_crypto.h>
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

int get_errno(void)
{
    return errno;
}

void set_errno(int err)
{
    errno = err;
}

#define SYSCALL_MAX 190
#define SYSCALL_NUM 160

#define SYSCALL(nr, func) [nr] = func,

const void *g_syscall_tbl[] __attribute__ ((section(".syscall_tbl"))) = {
    [0 ... SYSCALL_MAX - 1] = (void *)0XABCDABCD,
#include <syscall_tbl.h>
};

