/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#ifndef MBEDTLS_ENTROPY_POLL_H
#define MBEDTLS_ENTROPY_POLL_H

#if !defined(MBEDTLS_CONFIG_FILE)
#include "config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Default thresholds for built-in sources, in bytes
 */
#define MBEDTLS_ENTROPY_MIN_PLATFORM     32     /**< Minimum for platform source    */
#define MBEDTLS_ENTROPY_MIN_HAVEGE       32     /**< Minimum for HAVEGE             */
#define MBEDTLS_ENTROPY_MIN_HARDCLOCK     4     /**< Minimum for mbedtls_timing_hardclock()        */
#if !defined(MBEDTLS_ENTROPY_MIN_HARDWARE)
#define MBEDTLS_ENTROPY_MIN_HARDWARE     32     /**< Minimum for the hardware source */
#endif

/**
 * \brief           Entropy poll callback that provides 0 entropy.
 */
#if defined(MBEDTLS_TEST_NULL_ENTROPY)
    int mbedtls_null_entropy_poll( void *data,
                                unsigned char *output, size_t len, size_t *olen );
#endif

#if !defined(MBEDTLS_NO_PLATFORM_ENTROPY)
/**
 * \brief           Platform-specific entropy poll callback
 */
DLL_EXPORT_API int mbedtls_platform_entropy_poll( void *data,
                           unsigned char *output, size_t len, size_t *olen );
#endif

#if defined(MBEDTLS_HAVEGE_C)
/**
 * \brief           HAVEGE based entropy poll callback
 *
 * Requires an HAVEGE state as its data pointer.
 */
DLL_EXPORT_API int mbedtls_havege_poll( void *data,
                 unsigned char *output, size_t len, size_t *olen );
#endif

#if defined(MBEDTLS_TIMING_C)
/**
 * \brief           mbedtls_timing_hardclock-based entropy poll callback
 */
DLL_EXPORT_API int mbedtls_hardclock_poll( void *data,
                    unsigned char *output, size_t len, size_t *olen );
#endif

#if defined(MBEDTLS_ENTROPY_HARDWARE_ALT)
/**
 * \brief           Entropy poll callback for a hardware source
 *
 * \warning         This is not provided by mbed TLS!
 *                  See \c MBEDTLS_ENTROPY_HARDWARE_ALT in config.h.
 *
 * \note            This must accept NULL as its first argument.
 */
DLL_EXPORT_API int mbedtls_hardware_poll( void *data,
                           unsigned char *output, size_t len, size_t *olen );
#endif

#if defined(MBEDTLS_ENTROPY_NV_SEED)
/**
 * \brief           Entropy poll callback for a non-volatile seed file
 *
 * \note            This must accept NULL as its first argument.
 */
DLL_EXPORT_API int mbedtls_nv_seed_poll( void *data,
                          unsigned char *output, size_t len, size_t *olen );
#endif

#ifdef __cplusplus
}
#endif

#endif /* entropy_poll.h */
