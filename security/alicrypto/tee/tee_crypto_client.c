#include "../libalicrypto/mbed/inc/mbed_crypto.h"
#include "ali_crypto.h"
#include "tee_client_api.h"
#include "tee_crypto_client.h"

static const TEEC_UUID tee_crypt_uuid = CRYPTO_SRV_UUID;

void tee_aes_init(uint32_t *ctx) {
    // zerorize crypto engine ctx
    memset(ctx, 0, AES_ENGINE_CTX_SIZE >> 2);
}

void tee_aes_free(uint32_t *ctx) {
    // zerorize ctx for hw crypto
    memset(ctx, 0, AES_ENGINE_CTX_SIZE >> 2);
}

TEEC_Result tee_aes_encrypt_ecb(const uint8_t *in, uint32_t in_len,
                                const uint8_t *key, uint32_t key_len,
                                tee_aes_ctx_t *ctx,
                                uint8_t *out) {
    TEEC_UUID tee_uuid;
    TEEC_Result result;
    TEEC_Session ss;
    TEEC_Operation op;

    // open session
    tee_uuid = tee_crypt_uuid;
    result = TEEC_OpenSession(&ss, &tee_uuid, NULL);
    if (result != TEEC_SUCCESS) {
        PRINT_RET(ALI_CRYPTO_ERROR,
                "tee_aes_encrypt_ecb: open ss err(0x%08x)\n", result);
    }

    // invoke command
    op.paramTypes = TEEC_PARAM_TYPES(
            TEEC_MEMREF_TEMP_INPUT,
            TEEC_MEMREF_TEMP_INPUT,
            TEEC_MEMREF_TEMP_INPUT,
            TEEC_MEMREF_TEMP_OUTPUT);
    // in
    op.params[0].tmpref.buffer = (uint8_t *)in;
    op.params[0].tmpref.size = in_len;
    // key
    op.params[1].tmpref.buffer = (uint8_t *)key;
    op.params[1].tmpref.size = key_len;
    // iv
    op.params[2].tmpref.buffer = (uint8_t *)ctx;
    //op.params[2].tmpref.size = AES_IV_SIZE;
    // out
    op.params[3].tmpref.buffer = out;

    result = TEEC_InvokeCommand(&ss, TEE_AES_ENC_ECB, &op);
    if (result != TEEC_SUCCESS) {
        TEEC_CloseSession(&ss);
        PRINT_RET(result,
                "tee_aes_encrypt_ecb: inv cmd err(%08x)\n", result);
    }

    // close session
    TEEC_CloseSession(&ss);

    return TEEC_SUCCESS;
}

TEEC_Result tee_aes_decrypt_ecb(const uint8_t *in, uint32_t in_len,
                                const uint8_t *key, uint32_t key_len,
                                tee_aes_ctx_t *ctx,
                                uint8_t *out) {
    TEEC_UUID tee_uuid;
    TEEC_Result result;
    TEEC_Session ss;
    TEEC_Operation op;

    uint32_t ctx_sz;
    result = ali_aes_get_ctx_size(AES_ECB, &ctx_sz);

    // open session
    tee_uuid = tee_crypt_uuid;
    result = TEEC_OpenSession(&ss, &tee_uuid, NULL);
    if (result != TEEC_SUCCESS) {
        PRINT_RET(ALI_CRYPTO_ERROR,
                "tee_aes_decrypt_ecb: open ss err(0x%08x)\n", result);
    }

    // invoke command
    op.paramTypes = TEEC_PARAM_TYPES(
            TEEC_MEMREF_TEMP_INPUT,
            TEEC_MEMREF_TEMP_INPUT,
            TEEC_MEMREF_TEMP_INPUT,
            TEEC_MEMREF_TEMP_OUTPUT);
    // in
    op.params[0].tmpref.buffer = (uint8_t *)in;
    op.params[0].tmpref.size = in_len;
    // key
    op.params[1].tmpref.buffer = (uint8_t *)key;
    op.params[1].tmpref.size = key_len;
    // iv
    op.params[2].tmpref.buffer = (uint8_t *)ctx;
    //op.params[2].tmpref.size = AES_IV_SIZE;
    // out
    op.params[3].tmpref.buffer = out;

    result = TEEC_InvokeCommand(&ss, TEE_AES_DEC_ECB, &op);
    if (result != TEEC_SUCCESS) {
        TEEC_CloseSession(&ss);
        PRINT_RET(result,
                "tee_aes_encrypt_ecb: inv cmd err(%08x)\n", result);
    }

    // close session
    TEEC_CloseSession(&ss);

    return TEEC_SUCCESS;
}

TEEC_Result tee_aes_encrypt_cbc(const uint8_t *in, uint32_t in_len,
                                const uint8_t *key, uint32_t key_len,
                                tee_aes_ctx_t *ctx,
                                uint8_t *out) {
    TEEC_UUID tee_uuid;
    TEEC_Result result;
    TEEC_Session ss;
    TEEC_Operation op;

    // open session
    tee_uuid = tee_crypt_uuid;
    result = TEEC_OpenSession(&ss, &tee_uuid, NULL);
    if (result != TEEC_SUCCESS) {
        PRINT_RET(ALI_CRYPTO_ERROR,
                "tee_aes_encrypt_cbc: open ss err(0x%08x)\n", result);
    }

    // invoke command
    op.paramTypes = TEEC_PARAM_TYPES(
            TEEC_MEMREF_TEMP_INPUT,
            TEEC_MEMREF_TEMP_INPUT,
            TEEC_MEMREF_TEMP_INPUT,
            TEEC_MEMREF_TEMP_OUTPUT);
    // in
    op.params[0].tmpref.buffer = (uint8_t *)in;
    op.params[0].tmpref.size = in_len;
    // key
    op.params[1].tmpref.buffer = (uint8_t *)key;
    op.params[1].tmpref.size = key_len;
    // ctx
    op.params[2].tmpref.buffer = (uint8_t *)ctx;
    // out
    op.params[3].tmpref.buffer = out;

    result = TEEC_InvokeCommand(&ss, TEE_AES_ENC_CBC, &op);
    if (result != TEEC_SUCCESS) {
        TEEC_CloseSession(&ss);
        PRINT_RET(result,
                "tee_aes_encrypt_cbc: inv cmd err(%08x)\n", result);
    }

    // close session
    TEEC_CloseSession(&ss);

    return TEEC_SUCCESS;
}

TEEC_Result tee_aes_decrypt_cbc(const uint8_t *in, uint32_t in_len,
                                const uint8_t *key, uint32_t key_len,
                                tee_aes_ctx_t *ctx,
                                uint8_t *out) {

    TEEC_UUID tee_uuid;
    TEEC_Result result;
    TEEC_Session ss;
    TEEC_Operation op;

    // open session
    tee_uuid = tee_crypt_uuid;
    result = TEEC_OpenSession(&ss, &tee_uuid, NULL);
    if (result != TEEC_SUCCESS) {
        PRINT_RET(ALI_CRYPTO_ERROR,
                "tee_aes_decrypt_cbc: open ss err(0x%08x)\n", result);
    }

    // invoke command
    op.paramTypes = TEEC_PARAM_TYPES(
            TEEC_MEMREF_TEMP_INPUT,
            TEEC_MEMREF_TEMP_INPUT,
            TEEC_MEMREF_TEMP_INPUT,
            TEEC_MEMREF_TEMP_OUTPUT);
    // in
    op.params[0].tmpref.buffer = (uint8_t *)in;
    op.params[0].tmpref.size = in_len;
    // key
    op.params[1].tmpref.buffer = (uint8_t *)key;
    op.params[1].tmpref.size = key_len;
    // ctx
    op.params[2].tmpref.buffer = (uint8_t *)ctx;
    // out
    op.params[3].tmpref.buffer = out;

    result = TEEC_InvokeCommand(&ss, TEE_AES_DEC_CBC, &op);
    if (result != TEEC_SUCCESS) {
        TEEC_CloseSession(&ss);
        PRINT_RET(result,
                "tee_aes_decrypt_cbc: inv cmd err(%08x)\n", result);
    }

    // close session
    TEEC_CloseSession(&ss);

    return TEEC_SUCCESS;
}
