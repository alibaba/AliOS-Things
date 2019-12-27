/**
 * Copyright (C) 2016 The YunOS Project. All rights reserved.
 */

#ifndef _PROV_H_
#define _PROV_H_

#include <stdint.h>

#define PROV_MAGIC       (0x764f7250)
#define PROV_REC_NUM     (0x00000003)

/* prov status */
#define PROV_STAT_ABORT        (0x10001000)
#define PROV_STAT_FINISHED     (0x10001001)

#define BIN_TO_UINT(buffer) ((((uint8_t *)buffer)[3] << 24) + \
                             (((uint8_t *)buffer)[2] << 16) + \
                             (((uint8_t *)buffer)[1] << 8) + \
                             ((uint8_t *)buffer)[0])

#if (CONFIG_DS_VERSION == 1)
#define PROV_VERSION     (0x00010000)
typedef struct _prov_head_t {
    uint32_t magic;
    uint32_t version;
    uint32_t rec_num;
    uint32_t prov_stat; /* prov status */
    uint32_t prov_res;  /* prov result for error*/
    uint32_t err_code;
    uint32_t rsvd;
} prov_head_t;
#elif (CONFIG_DS_VERSION == 2)
#define PROV_VERSION     (0x00020000)
typedef struct _prov_head_t {
    uint32_t magic;
    uint32_t version;
    uint32_t prov_stat; /* prov status */
    uint32_t prov_res;  /* prov result for error*/
    uint32_t err_code;
    uint32_t rsvd;
    uint32_t rec_num;
} prov_head_t;
#endif /* CONFIG_DS_VERSION */

#endif /* _PROV_H_ */

