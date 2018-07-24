/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdlib.h>
#include "chip_config.h"
#include "irot_hal.h"
#include "log/chiplog.h"

#if (CHIP_TYPE_CONFIG == CHIP_TYPE_SECURE_MCU)

irot_result_t irot_hal_init(void)
{
    return IROT_SUCCESS;
}

irot_result_t irot_hal_cleanup(void)
{
    return IROT_SUCCESS;
}

irot_result_t irot_hal_get_id2(uint8_t *id2, uint32_t *len)
{
    chip_log("irot_hal_get_id2 not implemented !!!\n");
    return IROT_ERROR_NOT_IMPLEMENTED;
}

irot_result_t irot_hal_sym_crypto(key_object *key_obj, uint8_t key_id,
                                  const uint8_t *iv, uint32_t iv_len,
                                  const uint8_t *in, uint32_t in_len,
                                  uint8_t *out, uint32_t *out_len,
                                  sym_crypto_param_t *crypto_param)
{
    chip_log("irot_hal_sym_crypto not implemented !!!\n");
    return IROT_ERROR_NOT_IMPLEMENTED;
}

#if (CHIP_CRYPTO_TYPE_CONFIG == CHIP_CRYPTO_TYPE_RSA)
irot_result_t irot_hal_asym_priv_sign(key_object *key_obj, uint8_t key_id,
                                      const uint8_t *in, uint32_t in_len,
                                      uint8_t *out, uint32_t *out_len,
                                      asym_sign_verify_t type)
{
    chip_log("irot_hal_asym_priv_sign not implemented !!!\n");
    return IROT_ERROR_NOT_IMPLEMENTED;
}

irot_result_t irot_hal_asym_priv_decrypt(key_object *key_obj, uint8_t key_id,
                                         const uint8_t *in, uint32_t in_len,
                                         uint8_t *out, uint32_t *out_len,
                                         irot_asym_padding_t padding)
{
    chip_log("irot_hal_asym_priv_decrypt not implemented !!!\n");
    return IROT_ERROR_NOT_IMPLEMENTED;
}
#endif

#endif
