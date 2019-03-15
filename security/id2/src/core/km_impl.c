/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <string.h>
#include "km.h"
#include "config.h"
#include "alg/alg.h"

#if (ID2_KM_API_EMU)

#define ID2_LEN                             24
#define MAX_KEY_LEN                         32

static uint8_t s_id2[ID2_LEN]               = {0};
static uint32_t s_id2_len                   = 0;
static uint8_t s_id2_key[MAX_KEY_LEN]       = {0};
static uint32_t s_key_len                   = 0;

uint32_t km_generate_key(const char* name, const uint32_t name_len, km_key_type key_type, void* arg)
{
    return KM_ERR_NOT_SUPPORTED;
}

uint32_t km_export_key(const char* name, const uint32_t name_len, km_format_t format, uint8_t* export_data, uint32_t* export_data_size)
{
    return KM_ERR_NOT_SUPPORTED;
}


uint32_t km_mac(const char* name, const uint32_t name_len, km_sym_param* mac_params, const uint8_t* iv, const uint32_t iv_len, uint8_t* src, size_t src_len, uint8_t* mac, uint32_t* mac_len)
{
    return KM_ERR_NOT_SUPPORTED;
}


uint32_t km_delete_key(const char* name, const uint32_t name_len)
{
    return KM_ERR_NOT_SUPPORTED;
}


uint32_t km_delete_all()
{
    return KM_ERR_NOT_SUPPORTED;
}


uint32_t km_envelope_begin(void **ctx, const char *name, const uint32_t name_len, uint8_t *iv, uint16_t iv_len, uint8_t *protected_key, uint32_t *protected_key_len, km_purpose_type is_enc)
{
    return KM_ERR_NOT_SUPPORTED;
}


uint32_t km_envelope_update(void* ctx, uint8_t* src, uint32_t src_len, uint8_t* dest, uint32_t* dest_len)
{
    return KM_ERR_NOT_SUPPORTED;
}


uint32_t km_envelope_finish(void* ctx, uint8_t* src, uint32_t src_len, uint8_t* dest, uint32_t* dest_len)
{
    return KM_ERR_NOT_SUPPORTED;
}


uint32_t km_sign(const char* name, const uint32_t name_len, void* sign_params, const uint8_t* data, const size_t data_len, uint8_t* out, size_t* out_len)
{
    return KM_ERR_NOT_SUPPORTED;
}


uint32_t km_verify(const char* name, const uint32_t name_len, void* sign_params, const uint8_t* data, const size_t data_len, const uint8_t* signature, const size_t signature_len)
{
    return KM_ERR_NOT_SUPPORTED;
}

////////////////////////////////////////////////////////////////////////////////

uint32_t km_asym_encrypt(const char* name, const uint32_t name_len, void* enc_params, const uint8_t* src, const size_t src_len, uint8_t* dest, size_t* dest_len)
{
    return KM_ERR_NOT_SUPPORTED;
}

uint32_t km_asym_decrypt(const char* name, const uint32_t name_len, void* enc_params, const uint8_t* src, const size_t src_len, uint8_t* dest, size_t* dest_len)
{
    return KM_ERR_NOT_SUPPORTED;
}

////////////////////////////////////////////////////////////////////////////////

uint32_t km_init()
{
	return KM_SUCCESS;
}

void km_cleanup()
{
}

uint32_t km_cipher(const char* name, const uint32_t name_len, km_sym_param* cipher_params, const uint8_t* iv, const uint32_t iv_len, uint8_t* src, size_t src_len, uint8_t* dest, size_t* dest_len)
{
	irot_result_t ret;
    km_key_type key_type = cipher_params->key_type;
    km_block_mode_type block_mode = cipher_params->cipher_param.block_mode;
    km_padding_type padding_type = cipher_params->cipher_param.padding_type;
    km_purpose_type purpose_type = cipher_params->cipher_param.purpose_type;
	uint8_t is_enc;
    uint32_t output_len;

    if ((block_mode != KM_ECB) || (padding_type != KM_NO_PADDING))
    {
        return KM_ERR_NOT_SUPPORTED;
    }

    is_enc = (purpose_type == KM_PURPOSE_ENCRYPT) ? 1 : 0;

    output_len = *dest_len;
#if (ID2_CRYPTO_TYPE_CONFIG == ID2_CRYPTO_TYPE_3DES)
    if (key_type == KM_DES3)
    {
        ret = des3_ecb(s_id2_key, s_key_len, src, src_len, dest, &output_len, is_enc);
    }
#endif
#if (ID2_CRYPTO_TYPE_CONFIG == ID2_CRYPTO_TYPE_AES)
    if (key_type == KM_AES)
    {
        ret = aes_ecb(s_id2_key, s_key_len, src, src_len, dest, &output_len, is_enc);
    }
#endif
    else
    {
        return KM_ERR_NOT_SUPPORTED;
    }
	if(ret != IROT_SUCCESS)
	{
		return KM_ERR_GENERIC;
	}
    *dest_len = output_len;
    return KM_SUCCESS;
}

uint32_t km_set_id2(uint8_t* id2, uint32_t len)
{
	if (len != ID2_LEN)
	{
		return KM_ERR_BAD_PARAMS;
	}
	memcpy(s_id2, id2, len);
	s_id2_len = len;
	return KM_SUCCESS;
}

uint32_t km_import_key(const char* name, const uint32_t name_len, km_format_t format, const km_key_data_t* key_data, const uint32_t key_data_len)
{
    uint8_t key_type;
    uint8_t* pkey;
    uint32_t key_len;

    key_type = key_data->type;

    if ((key_type != KM_DES3) && (key_type != KM_AES))
    {
        return KM_ERR_BAD_PARAMS;
    }
    pkey = key_data->sym_key.key;
    key_len = key_data->sym_key.key_bit >> 3;

    memcpy(s_id2_key, pkey, key_len);
    s_key_len = key_len;
    return KM_SUCCESS;
}

uint32_t km_get_id2(uint8_t* id2, uint32_t* len)
{
    if (*len < ID2_LEN)
    {
        return KM_ERR_SHORT_BUFFER;
    }
    if (s_id2_len == 0)
    {
        return KM_ERR_ITEM_NOT_FOUND;
    }
    else
    {
        memcpy(id2, s_id2, ID2_LEN);
    }
    return KM_SUCCESS;
}

uint32_t km_get_attestation(uint8_t* id, uint32_t* id_len)
{
	if (id == NULL)
	{
		*id_len = ID2_LEN;
		return KM_ERR_SHORT_BUFFER;
	}
	else
	{
		*id_len = ID2_LEN;
		memset(id, 'A', *id_len);
		return KM_SUCCESS;
	}
}

#endif
