/**
 * Copyright (C) 2015-2016 The YunOS Project. All rights reserved.
 */

#include "tee_trm.h"
#include "tee_dbg.h"

#ifdef CONFIG_TRM_DRV_DBG
#define TRM_DRV_DBG_E(str, x...)                                              \
do {                                                                        \
    tee_dbg_print(ERR, "%s:%d: " str, __FUNCTION__, __LINE__, ## x);    \
} while (0)

#define TRM_DRV_DBG_I(str, x...)                                              \
do {                                                                        \
    tee_dbg_print(INF, "%s:%d: " str, __FUNCTION__, __LINE__, ## x);    \
} while (0)
#else
#define TRM_DRV_DBG_E(str, x...)      TEE_ERROR(-8)
#define TRM_DRV_DBG_I(str, x...)
#endif

extern void _convert_buf_to_bndata(const uint8_t *src, int32_t src_bytes,
                                   uint32_t *dst, int32_t dst_words);
extern int32_t _sw_exptmod_2_2m(const uint32_t *modulus, int32_t words, uint32_t *tmp_c);

static bool _g_id2_key_ready = false;
static uint32_t _g_id2_rsa_c_acc[ID2_KEY_BITS >> 5] = {0};
static uint32_t _g_id2_rsa_n[ID2_KEY_BITS >> 5] = {0};
static uint32_t _g_id2_rsa_e[ID2_KEY_BITS >> 5] = {0};
static uint32_t _g_id2_rsa_d[ID2_KEY_BITS >> 5] = {0};

static uint8_t _g_id2_rsa_fixed_raw_e[3] = {0x01, 0x00, 0x01};

#ifdef TRM_FAKE_KEY

/************************************************/
/*         FAKE ID2 KEY and ID                  */
/************************************************/
#if TRM_FAKE_KEY_SEL0
char ID[18] = "Y0482208A0D6E011A";

uint8_t RSA_N[] = {
    0xB9, 0xC2, 0x6A, 0x88, 0x90, 0xB0, 0xE1, 0x1B,
    0xD3, 0x3F, 0x93, 0x99, 0xF8, 0xCB, 0xC4, 0xF2,
    0x88, 0xB8, 0x56, 0x9A, 0x93, 0x24, 0x0B, 0x4A,
    0x75, 0xB4, 0x8D, 0x6C, 0x32, 0x40, 0x52, 0xA7,
    0x3D, 0x2B, 0x37, 0xCE, 0x57, 0x80, 0xFD, 0xA5,
    0x08, 0xC7, 0x4A, 0x77, 0x94, 0x34, 0xBA, 0x56,
    0x9F, 0xBA, 0x2E, 0xBA, 0x20, 0x1E, 0x80, 0x4A,
    0x09, 0x40, 0x07, 0xF5, 0x1D, 0xB6, 0xFC, 0x8E,
    0x5C, 0x4E, 0x02, 0x01, 0xD5, 0x97, 0x14, 0x83,
    0x2A, 0x32, 0x90, 0x4E, 0xED, 0xDC, 0x6F, 0xD6,
    0x0A, 0x1F, 0x44, 0x6A, 0xB1, 0xFE, 0x42, 0xBD,
    0xBF, 0xD7, 0x6F, 0xB5, 0x9B, 0x78, 0x5F, 0x19,
    0x07, 0x15, 0x4E, 0x65, 0xD2, 0x72, 0x09, 0x0C,
    0x14, 0xAD, 0xE8, 0xC7, 0x76, 0x25, 0xE8, 0x94,
    0xF8, 0xEE, 0x19, 0x93, 0xDF, 0xC9, 0x34, 0xEE,
    0x42, 0x1A, 0xA6, 0xA8, 0x70, 0xE2, 0xC0, 0x99
};
uint8_t RSA_D[] = {
    0x0D, 0x59, 0xA1, 0x9A, 0x6A, 0xFB, 0xAB, 0x51,
    0xFA, 0x9E, 0xA9, 0x16, 0xF9, 0xC0, 0x7B, 0x49,
    0x0A, 0xF6, 0x64, 0x92, 0xFB, 0x5C, 0x2D, 0xCB,
    0x8E, 0x26, 0x09, 0xBB, 0xEA, 0xE8, 0xA0, 0x88,
    0x10, 0x67, 0x58, 0xBD, 0x65, 0xA0, 0x89, 0x9A,
    0x20, 0xC5, 0x61, 0xDE, 0xB2, 0x32, 0x0F, 0xEA,
    0x45, 0xD4, 0x75, 0xA3, 0x3E, 0xC7, 0xB2, 0x64,
    0x9F, 0xC1, 0x90, 0x50, 0xD7, 0x6F, 0xDD, 0x3A,
    0xA6, 0xA3, 0x52, 0xED, 0xB6, 0xFC, 0x2B, 0x61,
    0xA5, 0xDB, 0xE4, 0xC7, 0xF8, 0x21, 0x30, 0xEF,
    0x7F, 0x21, 0x1E, 0x5B, 0x96, 0x45, 0x43, 0x4B,
    0x74, 0x98, 0xC6, 0x1B, 0xD9, 0x66, 0x0A, 0x64,
    0x41, 0x5F, 0xF6, 0xE0, 0x87, 0xD6, 0xFA, 0x00,
    0x4D, 0x76, 0x9B, 0x4E, 0x36, 0xC5, 0x8A, 0x10,
    0x8C, 0x61, 0xA1, 0x41, 0xB0, 0x8B, 0x25, 0xEC,
    0x19, 0x25, 0x89, 0x45, 0x8E, 0xFB, 0xCF, 0xB9
};
#else /* !TRM_FAKE_KEY_SEL0 */
char ID[18] = "Y00F300016247B17C";

uint8_t RSA_N[] = {
    0xC6, 0x99, 0x83, 0xD1, 0xC4, 0xAE, 0x92, 0xA4,
    0xBF, 0x2E, 0x36, 0x31, 0xFA, 0x5D, 0x03, 0xCD,
    0x1B, 0x3F, 0x52, 0x01, 0x06, 0xD2, 0x3E, 0xC4,
    0x3A, 0x32, 0xC9, 0x35, 0x17, 0x6D, 0x9F, 0x9E,
    0x71, 0x2F, 0x6F, 0x3B, 0xD0, 0xE8, 0xA6, 0x80,
    0x83, 0x60, 0xD5, 0x8B, 0x3D, 0xAD, 0xFF, 0xFF,
    0x62, 0x1A, 0x4C, 0xD5, 0x04, 0x43, 0x08, 0x6B,
    0x57, 0x3B, 0x63, 0xF9, 0x48, 0x5A, 0x86, 0x4B,
    0xAE, 0x4E, 0xC9, 0x6D, 0xF3, 0xAA, 0x28, 0x41,
    0xB9, 0xA6, 0x2F, 0x0E, 0xD6, 0x18, 0x59, 0x76,
    0x00, 0xBE, 0x61, 0xAE, 0x8A, 0x1A, 0xB9, 0x23,
    0x29, 0xE6, 0xBD, 0xA0, 0xBA, 0xF6, 0x0D, 0x2B,
    0xA6, 0xDE, 0x16, 0xC8, 0xE9, 0x82, 0x34, 0x71,
    0x72, 0x13, 0x63, 0x64, 0x74, 0x85, 0x70, 0xC7,
    0xE8, 0x8A, 0xDA, 0x7F, 0x42, 0x33, 0x7A, 0x98,
    0x8C, 0x72, 0x66, 0x9D, 0x85, 0x5E, 0xF3, 0x3D
};

uint8_t RSA_D[] = {
    0x2C, 0x70, 0xA3, 0xAF, 0xA7, 0xEC, 0x4C, 0x82,
    0x33, 0x09, 0x9B, 0x34, 0xF9, 0xCA, 0x20, 0x94,
    0x9F, 0xF9, 0x3E, 0x57, 0x5E, 0x78, 0x63, 0x75,
    0x33, 0x74, 0xB3, 0xD3, 0x7E, 0x87, 0x98, 0x45,
    0x33, 0x1C, 0xA3, 0x2E, 0x66, 0x63, 0xF9, 0x06,
    0x1E, 0xDF, 0xE7, 0x48, 0xFE, 0x27, 0x56, 0x76,
    0x86, 0x34, 0x4E, 0x67, 0x08, 0x4A, 0x1B, 0xB3,
    0x31, 0x54, 0xE7, 0xDD, 0x1B, 0x54, 0xAE, 0x62,
    0x12, 0x18, 0xCA, 0x43, 0x28, 0xA1, 0x7A, 0x3F,
    0xB6, 0x2C, 0xA2, 0x3D, 0xED, 0x2D, 0x7E, 0x96,
    0xE0, 0x12, 0x6F, 0xC6, 0x86, 0x20, 0x6B, 0x5A,
    0x5E, 0x76, 0x79, 0x0C, 0x38, 0x80, 0x18, 0x50,
    0xA6, 0x42, 0x8C, 0xB7, 0xC3, 0x17, 0xEF, 0xB4,
    0xB8, 0xAE, 0x74, 0xDF, 0x0F, 0x69, 0xCD, 0xB5,
    0x73, 0x3E, 0xD4, 0x75, 0x27, 0x1D, 0x94, 0x91,
    0x8A, 0x1A, 0xA1, 0x8C, 0x08, 0x4A, 0x15, 0xE1
};
#endif /* TRM_FAKE_KEY_SEL0 */

static int32_t _trm_get_key_data(uint32_t key_type, void *data, uint32_t size)
{
    if (data == 0) {
        TRM_DRV_DBG_E("_trm_get_key_data: bad arg\n");
        return -1;
    }

    if (key_type == KEY_TYPE_ID2_ID) {
        if (size < sizeof(ID) - 1) {
            TRM_DRV_DBG_E("_trm_get_key_data: ID2_ID size too short(0x%08x)\n", size);
            return -1;
        }
        memcpy(data, ID, sizeof(ID) - 1);
    } else {
        TRM_DRV_DBG_E("_trm_get_key_data: bad arg\n");
        return -1;
    }
    return 0;
}

static int32_t _tee_trm_read_rsa_key(void)
{
    /* convert _g_id2_rsa_n and _g_id2_rsa_d and _g_id2_rsa_e */
    _convert_buf_to_bndata((const uint8_t *)RSA_N, sizeof(RSA_N),
                           (uint32_t *)_g_id2_rsa_n, (ID2_KEY_BITS >> 5));
    _convert_buf_to_bndata((const uint8_t *)RSA_D, sizeof(RSA_D),
                           (uint32_t *)_g_id2_rsa_d, (ID2_KEY_BITS >> 5));
    _convert_buf_to_bndata((const uint8_t *)_g_id2_rsa_fixed_raw_e, sizeof(_g_id2_rsa_fixed_raw_e),
                           (uint32_t *)_g_id2_rsa_e, (ID2_KEY_BITS >> 5));

    return 0;
}

#else /* !TRM_FAKE_KEY */

extern uint32_t g_mft_idx_base;

static int32_t _get_key_meta_info(mft_idx_info_t *mft_idx,
                                  uint32_t key_type, mft_key_meta_t *key_meta)
{
    uint32_t i;

    TEE_ASSERT(mft_idx && key_meta);

    for (i = 0; i < mft_idx->key_num; i++) {
        if (mft_idx->key_metas[i].type == key_type) {
            memcpy(key_meta,
                 &mft_idx->key_metas[i], sizeof(mft_key_meta_t));
            return 0;
        }
    }

    TRM_DRV_DBG_I("not find key meta for type(%d)", key_type);

    return -1;
}

static int32_t _get_key_addr(mft_pos_info_t *pos_info, uint32_t *key_addr)
{
    uint32_t bank_num;
    uint32_t bit_idx;
    uint32_t byte_idx;

    if (pos_info == NULL || key_addr == NULL) {
        tee_dbg_print(ERR, "_get_key_addr: bad args\n");
         return -1;
    }

    if (pos_info->type == POS_INFO_TYPE_FUSE) {
        bank_num = pos_info->info >> 16;
        bit_idx = pos_info->info & 0xffff;
        byte_idx = bit_idx >> 3;

        *key_addr = bank_num*PLATFORM_OTP_BANK_SIZE + byte_idx + PLATFORM_OTP_SECURE_SECTOR_START;
    } else {
        TRM_DRV_DBG_E("_get_key_addr: not support this type(%d)\n", pos_info->type);
        return -1;
    }

    return 0;
}

static int32_t _trm_get_key_data(uint32_t key_type, void *data, uint32_t size)
{
    int ret;
    uint32_t key_addr;
    mft_idx_info_t *mft_idx;
    mft_key_meta_t key_meta;

    if (data == 0) {
        TRM_DRV_DBG_E("_trm_get_key_data: bad arg\n");
        return -1;
    }

    mft_idx = (mft_idx_info_t *)g_mft_idx_base;
    if (mft_idx->magic != MANIFEST_IDX_MAGIC) {
        TRM_DRV_DBG_E("_trm_get_key_data: bad mft idx magic(0x%08x)\n", mft_idx->magic);
        return -1;
    }

    ret = _get_key_meta_info(mft_idx, key_type, &key_meta);
    if (ret < 0) {
        TRM_DRV_DBG_E("_trm_get_key_data: fail to get key meta\n");
        return -1;
    }

    if (size < key_meta.size) {
        TRM_DRV_DBG_E("_trm_get_key_data: short buffer %d vs %d\n",
                          size, key_meta.size);
        return -1;
    }
    ret = _get_key_addr(&key_meta.pos_info, &key_addr);
    if (ret < 0) {
        TRM_DRV_DBG_E("_trm_get_key_data: get key addr fail\n");
        return -1;
    }

#ifdef TEE_TRM_DBG
    TRM_DRV_DBG_I("key_meta.type: %d\n", key_meta.type);
    TRM_DRV_DBG_I("key_meta.size: %d\n", key_meta.size);
    TRM_DRV_DBG_I("key_meta.flag: 0x%04x\n", key_meta.flag);
    TRM_DRV_DBG_I("key_addr: 0x%08x\n", key_addr);
#endif

    memcpy(data, (void *)key_addr, key_meta.size);
    return 0;
}

static int32_t _tee_trm_read_rsa_key(void)
{
    int32_t ret;
    uint8_t *tmp_n_d;
    uint32_t tmp_n_d_sz;

    tmp_n_d_sz = ID2_KEY_BITS >> 2;
    tmp_n_d = (uint8_t *)malloc(tmp_n_d_sz);
    if (tmp_n_d == NULL) {
        TRM_DRV_DBG_E("_tee_trm_read_rsa_key: malloc %d out of memory!\n", ID2_KEY_BITS >> 3);
        return -1;
    }

    ret = _trm_get_key_data(
              KEY_TYPE_ID2_PRVK, tmp_n_d, tmp_n_d_sz);
    if (ret != 0) {
        TRM_DRV_DBG_E("_tee_trm_read_rsa_key: fail to get id2 priv key\n");
        goto __out;
    }

    _convert_buf_to_bndata((const uint8_t *)tmp_n_d, (tmp_n_d_sz >> 1),
                           (uint32_t *)_g_id2_rsa_n, (ID2_KEY_BITS >> 5));
    _convert_buf_to_bndata((const uint8_t *)(tmp_n_d + (tmp_n_d_sz >> 1)), (tmp_n_d_sz >> 1),
                           (uint32_t *)_g_id2_rsa_d, (ID2_KEY_BITS >> 5));
    _convert_buf_to_bndata((const uint8_t *)_g_id2_rsa_fixed_raw_e, sizeof(_g_id2_rsa_fixed_raw_e),
                           (uint32_t *)_g_id2_rsa_e, (ID2_KEY_BITS >> 5));

    memset(tmp_n_d, 0, tmp_n_d_sz);

    ret = 0;
__out:
    free(tmp_n_d);
    return ret;
}

#endif /* TRM_FAKE_KEY */

int tee_trm_get_rsa_key(uint32_t **n, uint32_t **d, uint32_t **e,
                        uint32_t **c_acc, uint32_t *key_bits)
{
    if ((NULL == n) ||
        (NULL == d) ||
        (NULL == e) ||
        (NULL == c_acc) ||
        (NULL == key_bits)) {
        TRM_DRV_DBG_E("bad parameter!\n");
        return -1;
    }

    if (true == _g_id2_key_ready) {
        *n = _g_id2_rsa_n;
        *d = _g_id2_rsa_d;
        *e = _g_id2_rsa_e;
#ifdef RSA_SUPPORT
        *c_acc = _g_id2_rsa_c_acc;
#else
        *c_acc = NULL;
#endif
        *key_bits = ID2_KEY_BITS;

        return 0;
    } else {
        TRM_DRV_DBG_E("ID2 RSA key not available!\n");
        *n = NULL;
        *d = NULL;
        *e = NULL;
        *c_acc = NULL;
        *key_bits = 0;

        return -1;
    }
    return 0;
}


static int32_t _tee_trm_open(dev_t *dev)
{
    return 0;
}

static int32_t _tee_trm_close(dev_t *dev)
{
    return 0;
}

static int32_t _tee_trm_ioctl(dev_t *dev, int32_t cmd, void *arg)
{
    int ret;
    tee_trm_arg_t *trm_arg;

    trm_arg = (tee_trm_arg_t *)arg;
    TEE_ASSERT(trm_arg);

    switch(cmd) {
        case TEE_TRM_GET_ID2_ID:
        {
            ret = _trm_get_key_data(
                      KEY_TYPE_ID2_ID, trm_arg->data, trm_arg->size);
            if (ret < 0) {
                TRM_DRV_DBG_E("fail to get id2 id\n");
                return ret;
            }
            break;
        }

        default:
            TRM_DRV_DBG_E("illegal command(%d)\n", cmd);
            return -1;
    }

    return 0;
}

static dev_ops_t _tee_trm_ops = {
    .open = _tee_trm_open,
    .close = _tee_trm_close,
    .ioctl = _tee_trm_ioctl,
    .read = NULL,
    .write = NULL,
    .suspend = NULL,
    .resume = NULL,
};

static int32_t _tee_trm_probe()
{
    int ret;
    dev_t *dev;


    TRM_DRV_DBG_I("_tee_trm_probe\n");

    dev = dev_register(TEE_DRV_TRM_NAME, &_tee_trm_ops);
    if (dev == NULL) {
        TRM_DRV_DBG_E("_tee_trm_probe: dev regs fail\n");
        return -1;
    }

    ret = _tee_trm_read_rsa_key();
    if (ret != 0) {
        TRM_DRV_DBG_E("_tee_trm_probe: fail to read rsa_key N E D!\n");
        return -1;
    }

#ifdef RSA_SUPPORT
    ret = _sw_exptmod_2_2m((const uint32_t *)_g_id2_rsa_n, (ID2_KEY_BITS >> 5), _g_id2_rsa_c_acc);
    if (ret != 0) {
        TRM_DRV_DBG_E("_tee_trm_probe: fail to caculate N exptmod 2^2m!\n");
        goto __out;
    }
#endif
    _g_id2_key_ready = true;

    return 0;

__out:
    memset(_g_id2_rsa_n, 0, (ID2_KEY_BITS >> 3));
    memset(_g_id2_rsa_d, 0, (ID2_KEY_BITS >> 3));
    memset(_g_id2_rsa_e, 0, (ID2_KEY_BITS >> 3));
    memset(_g_id2_rsa_c_acc, 0, (ID2_KEY_BITS >> 3));
    return ret;
}

DEV_INIT(_tee_trm_probe, (uint8_t *)TEE_DRV_TRM_NAME);
