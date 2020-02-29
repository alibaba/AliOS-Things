/**
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "tee_trm.h"
#include "tee_trm_priv.h"

#if CONFIG_ID2_RSA_KEY
extern void    _convert_buf_to_bndata(const uint8_t *src, int32_t src_bytes,
                                      uint32_t *dst, int32_t dst_words);
extern int32_t _sw_exptmod_2_2m(const uint32_t *modulus, int32_t words,
                                uint32_t *tmp_c);

uint32_t _g_id2_rsa_c_acc[ID2_KEY_BITS >> 5] = { 0 };
uint32_t _g_id2_rsa_n[ID2_KEY_BITS >> 5]     = { 0 };
uint32_t _g_id2_rsa_e[ID2_KEY_BITS >> 5]     = { 0 };
uint32_t _g_id2_rsa_d[ID2_KEY_BITS >> 5]     = { 0 };
uint8_t  _g_id2_rsa_fixed_raw_e[3]           = { 0x01, 0x00, 0x01 };
#endif /* CONFIG_ID2_RSA_KEY */

#if CONFIG_ID2_AES_KEY
uint8_t  _g_id2_aes_key[ID2_AES_KEY_MAX_BYTES] = { 0 };
uint32_t _g_id2_aes_key_bits                   = 0;
#endif /* CONFIG_ID2_AES_KEY */

static bool _g_id2_key_ready = false;

#if CONFIG_ID2_RSA_KEY
int tee_trm_get_rsa_key(uint32_t **n, uint32_t **d, uint32_t **e,
                        uint32_t **c_acc, uint32_t *key_bits)
{
    if ((NULL == n) || (NULL == d) || (NULL == e) || (NULL == c_acc) ||
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
        *n        = NULL;
        *d        = NULL;
        *e        = NULL;
        *c_acc    = NULL;
        *key_bits = 0;

        return -1;
    }
    return 0;
}
#endif /* CONFIG_ID2_RSA_KEY */

#if CONFIG_ID2_AES_KEY
int tee_trm_get_aes_key(uint8_t **aes_key, uint32_t *key_bits)
{
    if ((NULL == aes_key) || (NULL == key_bits)) {
        TRM_DRV_DBG_E("bad parameter!\n");
        return -1;
    }

    if (true == _g_id2_key_ready) {
        *aes_key  = _g_id2_aes_key;
        *key_bits = _g_id2_aes_key_bits;

        return 0;
    } else {
        TRM_DRV_DBG_E("ID2 AES key not available!\n");
        *aes_key  = NULL;
        *key_bits = 0;

        return -1;
    }

    return 0;
}

#endif /* CONFIG_ID2_AES_KEY */

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
    int            ret;
    tee_trm_arg_t *trm_arg;

    trm_arg = (tee_trm_arg_t *)arg;
    TEE_ASSERT(trm_arg);

    switch (cmd) {
        case TEE_TRM_GET_ID2_ID: {
            ret = trm_get_key_data(KEY_TYPE_ID2_ID, trm_arg->data,
                                   &(trm_arg->size));
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
    .open    = _tee_trm_open,
    .close   = _tee_trm_close,
    .ioctl   = _tee_trm_ioctl,
    .read    = NULL,
    .write   = NULL,
    .suspend = NULL,
    .resume  = NULL,
};

static int32_t _tee_trm_probe()
{
    int    ret;
    dev_t *dev;


    TRM_DRV_DBG_I("_tee_trm_probe\n");

    dev = dev_register(TEE_DRV_TRM_NAME, &_tee_trm_ops);
    if (dev == NULL) {
        TRM_DRV_DBG_E("_tee_trm_probe: dev regs fail\n");
        return -1;
    }

#if CONFIG_ID2_RSA_KEY
    ret = tee_trm_read_rsa_key();
    if (ret != 0) {
        TRM_DRV_DBG_E("_tee_trm_probe: fail to read rsa_key N E D!\n");
        return -1;
    }

#ifdef RSA_SUPPORT
    ret = _sw_exptmod_2_2m((const uint32_t *)_g_id2_rsa_n, (ID2_KEY_BITS >> 5),
                           _g_id2_rsa_c_acc);
    if (ret != 0) {
        TRM_DRV_DBG_E("_tee_trm_probe: fail to caculate N exptmod 2^2m!\n");
        goto __out;
    }
#endif /* RSA_SUPPORT */
#endif /* CONFIG_ID2_RSA_KEY */

#if CONFIG_ID2_AES_KEY
    ret = tee_trm_read_sym_key(TEE_TRM_AES);
    if (ret) {
        TRM_DRV_DBG_E("_tee_trm_probe: fail to read aes key!\n");
        return -1;
    }
#endif /* CONFIG_ID2_AES_KEY */

    _g_id2_key_ready = true;

    return 0;

#if CONFIG_ID2_RSA_KEY
__out:
    memset(_g_id2_rsa_n, 0, (ID2_KEY_BITS >> 3));
    memset(_g_id2_rsa_d, 0, (ID2_KEY_BITS >> 3));
    memset(_g_id2_rsa_e, 0, (ID2_KEY_BITS >> 3));
    memset(_g_id2_rsa_c_acc, 0, (ID2_KEY_BITS >> 3));
#endif /* CONFIG_ID2_RSA_KEY */
    return ret;
}

DEV_INIT(_tee_trm_probe, (uint8_t *)TEE_DRV_TRM_NAME);
