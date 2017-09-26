/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef K_ENDIAN_H
#define K_ENDIAN_H

#if (RHINO_CONFIG_LITTLE_ENDIAN != 1)

#define krhino_htons(x) x
#define krhino_htonl(x) x
#define krhino_ntohl(x) x
#define krhino_ntohs(x) x

#else

#ifndef krhino_htons
#define krhino_htons(x) ((uint16_t)(((x) & 0x00ffU) << 8)| \
                        (((x) & 0xff00U) >> 8))
#endif

#ifndef krhino_htonl
#define krhino_htonl(x) ((uint32_t)(((x) & 0x000000ffUL) << 24) | \
                        (((x) & 0x0000ff00UL) << 8) | \
                        (((x) & 0x00ff0000UL) >> 8) | \
                        (((x) & 0xff000000UL) >> 24))
#endif

#ifndef krhino_ntohs
#define krhino_ntohs(x) ((uint16_t)((x & 0x00ffU) << 8) | \
                        ((x & 0xff00U) >> 8))
#endif

#ifndef krhino_ntohl
#define krhino_ntohl(x) ((uint32_t)(((x) & 0x000000ffUL) << 24) | \
                        (((x) & 0x0000ff00UL) << 8) | \
                        (((x) & 0x00ff0000UL) >> 8) | \
                        (((x) & 0xff000000UL) >> 24))
#endif


#endif /*RHINO_CONFIG_LITTLE_ENDIAN*/



#endif
