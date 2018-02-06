/*
 * Copyright (C) 2017 The YunOS Project. All rights reserved.
 */
#include "tee_client_api.h"
#include "tee_sym.h"
#include "tee_id2_cmd.h"

#define AES_BLOCK_LEN 16
#define DES_BLOCK_LEN 8

static const TEEC_UUID id2_uuid = ID2_SRV_UUID;

static int _get_pkcs_padding(unsigned char *input, size_t input_len,
                uint32_t *data_len)
{
    uint32_t i, pad_idx;
    uint8_t padding_len, bad = 0;

    if(NULL == input || NULL == data_len) {
        return -1;
    }

    padding_len = input[input_len - 1];
    *data_len = input_len - padding_len;

    /* Avoid logical || since it results in a branch */
    bad |= padding_len > input_len;
    bad |= padding_len == 0;

    /* The number of bytes checked must be independent of padding_len,
     ** so pick input_len, which is usually 8 or 16 (one block) */
    pad_idx = input_len - padding_len;
    for(i = 0; i < input_len; i++) {
        bad |= (input[i] ^ padding_len) * (i >= pad_idx);
    }

    if (bad != 0) {
        ID2_DBG_E("bad pkcs padding\n");
        return -1;
    }

    return 0;
}

int tee_sym_cipher(uint8_t ID, cipher_param_t *cipher_param,
                   const uint8_t *iv, uint32_t iv_len,
                   const uint8_t *in, uint32_t in_len,
                   uint8_t *out, uint32_t *out_len)
{
    TEEC_UUID tee_uuid;
    TEEC_Result result;
    TEEC_Session ss;
    TEEC_Operation op;
    TEEC_SharedMemory shm;
    cipher_param_t sym_param;
    uint32_t src_len = 0; //for padding
    uint32_t dest_len = 0;
    uint32_t block_len = 0;
    uint32_t padding_data = 0;
    int ret = 0;
    uint32_t dec_len = 0;

    if (in == NULL || in_len <= 0 || out_len == NULL ||
        (out == NULL && *out_len != 0)) {
        ID2_DBG_E("tee_sym_cipher: invalid input args!\n");
        return -1;
    }

    if (ID == KEY_ID_18 || ID == KEY_ID_33) {
        tee_uuid = id2_uuid;
    } else {
        ID2_DBG_E("tee_sym_cipher: not support this ID(%d)\n", ID);
        return -1;
    }

    if (ID2_AES == cipher_param->cipher_type) {
        block_len = AES_BLOCK_LEN;
    } else if (ID2_3DES != cipher_param->cipher_type ||
               ID2_DES != cipher_param->cipher_type) {
        block_len = DES_BLOCK_LEN;
    } else {
        ID2_DBG_E("tee_sym_cipher: not support cipher type %d\n",
                cipher_param->cipher_type);
        return -1;
    }

    if (ID2_ECB == cipher_param->block_mode) {
        iv_len = 0;
    } else if (ID2_CBC == cipher_param->block_mode ||
               ID2_CTR == cipher_param->block_mode) {
        if (iv_len < block_len) {
            ID2_DBG_E("tee_sym_cipher: bad iv\n");
            return -1;
        } else {
            iv_len = block_len;
        }
    } else {
        ID2_DBG_E("tee_sym_cipher: not support block mode %d\n",
                cipher_param->block_mode);
        return -1;
    }

    if (ID2_PKCS5 == cipher_param->padding_type ||
        ID2_PKCS7 == cipher_param->padding_type) {
        if (cipher_param->is_enc) {
            padding_data = block_len - in_len % block_len;
            src_len = in_len + padding_data;
            dest_len = src_len;
            if (*out_len < dest_len) {
                ID2_DBG_E("tee_sym_cipher: aes enc short buffer %d : %d\n", *out_len, dest_len);
                *out_len = dest_len;
                return -1;
            }
        } else {
            src_len = in_len;
            dest_len = src_len;
        }
    } else if (No_PADDING == cipher_param->padding_type) {
        src_len = in_len;
        dest_len = src_len;
        if (*out_len < dest_len) {
            ID2_DBG_E("tee_sym_cipher: aes enc short buffer %d : %d\n", *out_len, dest_len);
            *out_len = dest_len;
            return -1;
        }
    } else {
        ID2_DBG_E("tee_sym_cipher: not support block mode %d\n", cipher_param->block_mode);
        return -1;
    }

    result = TEEC_OpenSession(&ss, &tee_uuid, NULL);
    if (result != TEEC_SUCCESS) {
        ID2_DBG_E("tee_sym_cipher: open ss err(%08x)\n", result);
        ret = -1;
        goto cleanup2;
    }

    memcpy(&sym_param, cipher_param, sizeof(cipher_param_t));

    //iv | src | dest
    shm.size  = iv_len + src_len + dest_len;
    shm.flags = TEEC_MEM_INPUT | TEEC_MEM_OUTPUT;
    result = TEEC_AllocateSharedMemory(&shm);
    if (TEEC_SUCCESS != result) {
        ID2_DBG_E("tee_sym_cipher: alloc shm err(%08x)\n", result);
        ret = -1;
        goto cleanup3;
    }

    if (iv_len) {
        memcpy(shm.buffer, (uint8_t *)iv, iv_len);
    }
    memcpy(shm.buffer + iv_len, (uint8_t *)in, in_len);
    if (padding_data) {
        memset(shm.buffer + iv_len + in_len, padding_data, padding_data);
    }

    op.paramTypes = TEEC_PARAM_TYPES(
                        TEEC_MEMREF_TEMP_INPUT,
                        TEEC_MEMREF_PARTIAL_INPUT,
                        TEEC_MEMREF_PARTIAL_OUTPUT,
                        TEEC_NONE);
    op.params[0].tmpref.buffer = (uint8_t *)(&sym_param);
    op.params[0].tmpref.size = sizeof(cipher_param_t);
    op.params[1].memref.parent = &shm;
    op.params[1].memref.offset = 0;
    op.params[1].memref.size = iv_len + src_len;
    op.params[2].memref.parent = &shm;
    op.params[2].memref.offset = iv_len + src_len;
    op.params[2].memref.size = dest_len;

    result = TEEC_InvokeCommand(&ss, TEE_ID2_CIPHER, &op);
    if (result != TEEC_SUCCESS) {
        ID2_DBG_E("tee_sym_cipher: inv cmd err(%08x)\n", result);
        ret = -1;
        goto cleanup4;
    }

    //for ecb cbc decrypt
    if (!cipher_param->is_enc &&
        (ID2_PKCS5 == cipher_param->padding_type ||
         ID2_PKCS7 == cipher_param->padding_type)) {
        if (_get_pkcs_padding((uint8_t *)shm.buffer + iv_len + src_len, dest_len, &dec_len)) {
            ID2_DBG_E("tee_sym_cipher: dec wrong pkcs padding\n");
            ret = -1;
            goto cleanup4;
        } else {
            if (*out_len < dec_len) {
                ID2_DBG_E("tee_sym_cipher: short buffer %d : %d\n", *out_len, dec_len);
                *out_len = dec_len;
                ret = -1;
                goto cleanup4;
            }
            *out_len = dec_len;
        }
    } else {
        *out_len = op.params[2].memref.size;
    }

    memcpy(out, (uint8_t *)shm.buffer + src_len + iv_len, *out_len);

cleanup4:
    TEEC_ReleaseSharedMemory(&shm);
cleanup3:
    TEEC_CloseSession(&ss);
cleanup2:

    return ret;
}

