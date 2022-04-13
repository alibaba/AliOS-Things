/*!
 * @file sal_def.h
 *
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef SAL_DEF_H
#define SAL_DEF_H

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup aos_sal API
 *  Socket Adapter Layer, which provides socket API service
 *  implemented on top of AT modules.
 *
 *  @{
 */

/**
 * Network-byte and host-byte convertion utility.
 *
 * @note It's recommended to use xxx() API, rather than sal_xxx().
 */
#if BYTE_ORDER == BIG_ENDIAN
#define sal_htons(x) (x)
#define sal_ntohs(x) (x)
#define sal_htonl(x) (x)
#define sal_ntohl(x) (x)
#else
#define sal_htons(x) ((((x) & 0xff) << 8) | (((x) & 0xff00) >> 8))
#define sal_ntohs(x) sal_htons(x)
#define sal_htonl(x) ((((x) & 0xff) << 24) | \
                     (((x) & 0xff00) << 8) | \
                     (((x) & 0xff0000UL) >> 8) | \
                     (((x) & 0xff000000UL) >> 24))
#define sal_ntohl(x) sal_htonl(x)
#endif

/**
 * Map the widely used socket APIs to sal_xxx() APIs.
 *
 * @note It's recommended to use xxx() API, rather than sal_xxx().
 */
#define htons(x) sal_htons(x)

#define ntohs(x) sal_ntohs(x)

#define htonl(x) sal_htonl(x)

#define ntohl(x) sal_ntohl(x)

/** @} */ /* end of group aos_sal */

#ifdef __cplusplus
}
#endif

#endif

