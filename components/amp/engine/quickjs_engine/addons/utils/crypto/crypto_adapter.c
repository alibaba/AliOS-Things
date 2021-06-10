/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "stdio.h"
#include "stdint.h"
#include "stdbool.h"
#include "string.h"
#include "crypto_adapter.h"
#include "tinycrypt/aes.h"
#include "tinycrypt/constants.h"
#include "tinycrypt/cbc_mode.h"

const uint8_t aes_iv[16] = 
{ 
    0x31, 0x32, 0x33, 0x61, 0x71, 0x77, 0x65, 0x64,
    0x23, 0x2a, 0x24, 0x21, 0x28, 0x34, 0x6a, 0x75 
};

int crypto_adapter_encrypt_cbc(uint8_t *in, uint16_t inlen, uint8_t *out, uint16_t *outlen, uint8_t *key)
{
    uint8_t *encrypt_buff = NULL;
    struct tc_aes_key_sched_struct key_sched;
    uint8_t fill = 16 - (inlen) % 16;
    uint8_t *fill_buff;
    int ret;
    uint16_t enc_len;

    fill_buff = (uint8_t *)aos_malloc(inlen + fill);
    if (fill_buff == NULL) {
        return -1; 
    }
    memcpy(fill_buff, in, inlen);
    for (int i = 0; i < fill; i++) {
        fill_buff[inlen + i] = fill;
    }

    enc_len = inlen+fill;
    encrypt_buff = (uint8_t *)aos_malloc(enc_len + TC_AES_BLOCK_SIZE);
    if (encrypt_buff == NULL) {
        aos_free(fill_buff);
        return -1;
    }
    if (key != NULL) {
        (void)tc_aes128_set_encrypt_key(&key_sched, key);
    }

    ret = utils_tc_cbc_mode_encrypt(encrypt_buff, enc_len + TC_AES_BLOCK_SIZE, fill_buff, enc_len, aes_iv, &key_sched);
    aos_free(fill_buff);
    if (ret != TC_CRYPTO_SUCCESS) {
        aos_free(encrypt_buff);
        return -1;        
    }

    if (out != NULL) {
        memcpy(out, encrypt_buff + TC_AES_BLOCK_SIZE, enc_len);
    }
    aos_free(encrypt_buff);
    if (outlen) {
        *outlen = enc_len;
    }

    return 0;
}

uint8_t *crypto_adapter_decrypt_cbc(const uint8_t *in, uint16_t inlen, uint8_t *out, uint16_t *out_len, uint8_t *key)
{
    uint8_t fill;
    uint8_t *encrypt_data;
    struct tc_aes_key_sched_struct key_sched;
    int ret;

    if (key != NULL) {
        (void)tc_aes128_set_encrypt_key(&key_sched, key);
    }

    encrypt_data = (uint8_t *)aos_malloc(inlen + TC_AES_BLOCK_SIZE);
    if (encrypt_data == NULL) {
        return -1;
    }

    // TinyCrypt CBC decryption assumes that the iv and the ciphertext are contiguous
    memcpy(encrypt_data, aes_iv, TC_AES_BLOCK_SIZE);
    memcpy(encrypt_data + TC_AES_BLOCK_SIZE, in, inlen);

    ret = utils_tc_cbc_mode_decrypt(out, inlen,
                                encrypt_data+TC_AES_BLOCK_SIZE, inlen, encrypt_data, &key_sched);
    aos_free(encrypt_data);
    if (ret != TC_CRYPTO_SUCCESS) {
        return -1;
    }
    fill = out[inlen - 1];
    *out_len = inlen - fill;

    return 0;
}

int crypto_adapter_encrypt(Encrypt_mode mode, uint8_t *in, uint16_t inlen, uint8_t *out, uint16_t *outlen, uint8_t *key)
{
    switch (mode) {
    case Encrypt_mode_AES_CBC:
        return crypto_adapter_encrypt_cbc(in, inlen, out, outlen, key);
        break;
    default:
        break;
    }
    return -1;
}

int crypto_adapter_decrypt(Encrypt_mode mode, uint8_t *in, uint16_t inlen, uint8_t *out, uint16_t *outlen, uint8_t *key)
{
    switch (mode) {
    case Encrypt_mode_AES_CBC:
        return crypto_adapter_decrypt_cbc(in, inlen, out, outlen, key);
        break;
    default:
        break;
    }
    return -1;
}

