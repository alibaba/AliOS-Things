/**
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "tee_trm.h"
#include "tee_trm_priv.h"

#if CONFIG_ID2_RSA_KEY
extern void _convert_buf_to_bndata(const uint8_t *src, int32_t src_bytes,
                                   uint32_t *dst, int32_t dst_words);

extern uint32_t _g_id2_rsa_n[ID2_KEY_BITS >> 5];
extern uint32_t _g_id2_rsa_e[ID2_KEY_BITS >> 5];
extern uint32_t _g_id2_rsa_d[ID2_KEY_BITS >> 5];
uint8_t         _g_id2_rsa_fixed_raw_e[3] = { 0x01, 0x00, 0x01 };
#endif /* CONFIG_ID2_RSA_KEY */

#if CONFIG_ID2_AES_KEY
extern uint32_t _g_id2_aes_key[ID2_AES_KEY_MAX_BYTES];
extern uint32_t _g_id2_aes_key_bits;
#endif /* CONFIG_ID2_AES_KEY */

extern uint32_t g_mft_idx_base;

static int32_t _get_key_meta_info(mft_idx_info_t *mft_idx, uint32_t key_type,
                                  mft_key_meta_t *key_meta)
{
    uint32_t i;

    TEE_ASSERT(mft_idx && key_meta);

    for (i = 0; i < mft_idx->key_num; i++) {
        if (mft_idx->key_metas[i].type == key_type) {
            memcpy(key_meta, &mft_idx->key_metas[i], sizeof(mft_key_meta_t));
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
        bit_idx  = pos_info->info & 0xffff;
        byte_idx = bit_idx >> 3;

        *key_addr = bank_num * PLATFORM_OTP_BANK_SIZE + byte_idx +
                    PLATFORM_OTP_SECURE_SECTOR_START;
    } else {
        TRM_DRV_DBG_E("_get_key_addr: not support this type(%d)\n",
                      pos_info->type);
        return -1;
    }

    return 0;
}

int32_t trm_get_key_data(uint32_t key_type, void *data, uint32_t *size)
{
    int             ret;
    uint32_t        key_addr;
    mft_idx_info_t *mft_idx;
    mft_key_meta_t  key_meta;

    if (data == 0) {
        TRM_DRV_DBG_E("trm_get_key_data: bad arg\n");
        return -1;
    }

    mft_idx = (mft_idx_info_t *)g_mft_idx_base;
    if (mft_idx->magic != MANIFEST_IDX_MAGIC) {
        TRM_DRV_DBG_E("trm_get_key_data: bad mft idx magic(0x%08x)\n",
                      mft_idx->magic);
        return -1;
    }

    ret = _get_key_meta_info(mft_idx, key_type, &key_meta);
    if (ret < 0) {
        TRM_DRV_DBG_E("trm_get_key_data: fail to get key meta\n");
        return -1;
    }

    if (*size < key_meta.size) {
        TRM_DRV_DBG_E("trm_get_key_data: short buffer %d vs %d\n", *size,
                      key_meta.size);
        *size = key_meta.size;
        return -1;
    }
    *size = key_meta.size;

    ret = _get_key_addr(&key_meta.pos_info, &key_addr);
    if (ret < 0) {
        TRM_DRV_DBG_E("trm_get_key_data: get key addr fail\n");
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

#if CONFIG_ID2_RSA_KEY
static int32_t tee_trm_read_rsa_key(void)
{
    int32_t  ret;
    uint8_t *tmp_n_d;
    uint32_t tmp_n_d_sz;

    tmp_n_d_sz = ID2_KEY_BITS >> 2;
    tmp_n_d    = (uint8_t *)malloc(tmp_n_d_sz);
    if (tmp_n_d == NULL) {
        TRM_DRV_DBG_E("_tee_trm_read_rsa_key: malloc %d out of memory!\n",
                      ID2_KEY_BITS >> 3);
        return -1;
    }

    ret = trm_get_key_data(KEY_TYPE_ID2_PRVK, tmp_n_d, &tmp_n_d_sz);
    if (ret != 0) {
        TRM_DRV_DBG_E("_tee_trm_read_rsa_key: fail to get id2 priv key\n");
        goto __out;
    }

    _convert_buf_to_bndata((const uint8_t *)tmp_n_d, (tmp_n_d_sz >> 1),
                           (uint32_t *)_g_id2_rsa_n, (ID2_KEY_BITS >> 5));
    _convert_buf_to_bndata((const uint8_t *)(tmp_n_d + (tmp_n_d_sz >> 1)),
                           (tmp_n_d_sz >> 1), (uint32_t *)_g_id2_rsa_d,
                           (ID2_KEY_BITS >> 5));
    _convert_buf_to_bndata((const uint8_t *)_g_id2_rsa_fixed_raw_e,
                           sizeof(_g_id2_rsa_fixed_raw_e),
                           (uint32_t *)_g_id2_rsa_e, (ID2_KEY_BITS >> 5));

    memset(tmp_n_d, 0, tmp_n_d_sz);

    ret = 0;

__out:
    free(tmp_n_d);
    return ret;
}
#endif /* CONFIG_ID2_RSA_KEY */

#if CONFIG_ID2_AES_KEY
int32_t tee_trm_read_sym_key(uint32_t type)
{
    int      ret          = 0;
    uint8_t *sym_key      = NULL;
    uint32_t sym_key_byte = ID2_AES_KEY_MAX_BYTES;

    sym_key = (uint8_t *)malloc(ID2_AES_KEY_MAX_BYTES);
    if (!sym_key) {
        TRM_DRV_DBG_E("_tee_trm_read_sym_key: malloc out of memory!\n");
        return -1;
    }

    ret = trm_get_key_data(KEY_TYPE_ID2_PRVK, sym_key, &sym_key_byte);
    if (ret != 0) {
        TRM_DRV_DBG_E("_tee_trm_read_rsa_key: fail to get id2 priv key\n");
        goto __out;
    }

    switch (type) {
        case TEE_TRM_AES: {
            memcpy(_g_id2_aes_key, sym_key, sym_key_byte);
            _g_id2_aes_key_bits = sym_key_byte << 3;
            break;
        }
        default: {
            TRM_DRV_DBG_E("_tee_trm_read_rsa_key: not support type\n", type);
            goto __out;
        }
    }

__out:
    if (sym_key) {
        free(sym_key);
        sym_key = NULL;
    }

    return ret;
}

#endif /* CONFIG_ID2_AES_KEY */
