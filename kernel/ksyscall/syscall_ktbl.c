/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <aos/aos.h>
#ifdef WITH_LWIP
#include <lwip/netdb.h>
#include <lwip/sockets.h>
#include <lwip/priv/tcp_priv.h>
#include <lwip/udp.h>
#endif
#include <hal/hal.h>
#ifdef CONFIG_AOS_MESH
#include <umesh.h>
#include <ip/lwip_adapter.h>
#endif
#ifdef MBEDTLS_IN_KERNEL
#include <aos/mbedtls_ssl.h>
#endif
#ifdef CONFIG_ALICRYPTO
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
AOS_EXPORT(void *, sys_aos_malloc, unsigned int, size_t);

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
AOS_EXPORT(void *, sys_aos_realloc, void *, unsigned int, size_t);

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
AOS_EXPORT(void *, sys_aos_zalloc, unsigned int, size_t);

int get_errno(void)
{
    return errno;
}
AOS_EXPORT(int, get_errno, void);

void set_errno(int err)
{
    errno = err;
}
AOS_EXPORT(void, set_errno, int);

#ifdef CONFIG_ALICRYPTO
AOS_EXPORT(ali_crypto_result, ali_aes_get_ctx_size, aes_type_t, size_t *);
AOS_EXPORT(ali_crypto_result, ali_aes_init, aes_type_t, bool, const uint8_t *, const uint8_t *, size_t, const uint8_t *, void *);
AOS_EXPORT(ali_crypto_result, ali_aes_finish, const uint8_t *, size_t, uint8_t *, size_t *, sym_padding_t, void *);
#endif

#ifdef MBEDTLS_IN_KERNEL
AOS_EXPORT(void *, mbedtls_ssl_connect, void *, const char *, int);
AOS_EXPORT(int, mbedtls_ssl_send, void *, const char *, int);
AOS_EXPORT(int, mbedtls_ssl_recv, void *, char *, int);
AOS_EXPORT(int, mbedtls_ssl_close, void *);
#endif

#ifdef CONFIG_AOS_MESH
AOS_EXPORT(ur_error_t, umesh_init, node_mode_t);
AOS_EXPORT(ur_error_t, umesh_start, void);
AOS_EXPORT(ur_error_t, umesh_stop, void);
AOS_EXPORT(uint8_t, umesh_get_device_state, void);
AOS_EXPORT(uint8_t, umesh_get_mode, void);
AOS_EXPORT(ur_error_t, umesh_set_mode, uint8_t);
AOS_EXPORT(const mac_address_t *, umesh_get_mac_address, media_type_t);
AOS_EXPORT(const void *, ur_adapter_get_default_ipaddr, void);
AOS_EXPORT(const void *, ur_adapter_get_mcast_ipaddr, void);
#endif

int aos_vprintf(char *format, va_list param)
{
    return vprintf(format, param);
}
AOS_EXPORT(int, aos_vprintf, char *, va_list);

int aos_fflush(FILE *stream)
{
    return fflush(stream);
}
AOS_EXPORT(int, aos_fflush, FILE *);


/* for syscall_ktbl.h, on the last */
#include <syscall_ktbl.h>

