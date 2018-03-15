/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 */

#ifndef _TEE_TRM_H_
#define _TEE_TRM_H_

#include "hal_memmap.h"
#include "device.h"

#define TEE_DRV_TRM_NAME        "trm"

#define TEE_TRM_GET_ID2_ID      (0x0)
#define TEE_TRM_GET_ID2_PRVK    (0x1)

#define MANIFEST_IDX_MAGIC      (0x78446E49)  // "InDx"

/* position info type */
#define POS_INFO_TYPE_FUSE      (0x1)
#define POS_INFO_TYPE_NANDFLASH (0x2)
#define POS_INFO_TYPE_NORFLASH  (0x3)
#define POS_INFO_TYPE_SE        (0x4)

//FIXME, this need be synced with BOOTROM
#define KEY_TYPE_MNF_TB_ADDR    (1)
#define KEY_TYPE_JTAG_KEY       (2)
#define KEY_TYPE_PUBK_TB        (3)
#define KEY_TYPE_ID2_ID         (4)
#define KEY_TYPE_ID2_PRVK       (5)
#define KEY_TYPE_SOC_CONFIG     (6)
#define KEY_TYPE_LPM_RECOVERY   (7)

/* ID2 Key info */
#if CONFIG_ID2_RSA_KEY
#define ID2_KEY_BITS            (1024)
#elif CONFIG_ID2_AES_KEY
#define ID2_AES_KEY_MAX_BITS         (256)
#define ID2_AES_KEY_MAX_BYTES        (32)

#define TEE_TRM_AES   0x01
#define TEE_TRM_DES   0x02
#define TEE_TRM_3DES  0x03

#endif /* CONFIG_ID2_RSA_KEY */

typedef struct _mft_key_flag_t {
    uint16_t type:1; // key or data
    uint16_t key_sche; //reserve if type is data;
} mft_key_flag_t;

typedef struct _mft_pos_info_t {
    uint32_t info:28;
    uint32_t type:4;
} mft_pos_info_t;

typedef struct _mft_key_meta_t {
    uint16_t type;
    uint16_t size;
    uint16_t flag;
    uint16_t rsvd;
    mft_pos_info_t pos_info;
} mft_key_meta_t;

typedef struct _mft_idx_info_t {
    uint32_t magic;
    uint16_t version;
    uint16_t key_num;
    mft_key_meta_t key_metas[0];
} mft_idx_info_t;

typedef struct _tee_trm_arg_t {
    void *data;
    uint32_t size;
} tee_trm_arg_t;
#endif
