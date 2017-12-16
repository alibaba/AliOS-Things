/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 */

#include "rsa_test.h"

int32_t rsa_test_v1(void)
{
    tee_crypto_result res;

    uint8_t dst[KEY_BYTES];
    int32_t dst_size = KEY_BYTES;

    uint8_t dst2[KEY_BYTES];
    int32_t dst2_size = KEY_BYTES;

    /* 1. Self encrypt and decrypt test, NOPAD */
    memset(dst, 0, sizeof(dst));
    memset(dst2, 0, sizeof(dst2));
    dst_size = KEY_BYTES;
    dst2_size = KEY_BYTES;

    rsa_keypair_t *keypair;
    rsa_pubkey_t *pubkey;
    rsa_padding_t padding;

    res = rsa_test_format_key(KEY_BITS,
                            modulus, KEY_BYTES,
                            publicExponent, KEY_BYTES,
                            privateExponent, KEY_BYTES,
                            &keypair,
                            &pubkey);
    RSA_TEST_ASSERT(0 == res);

    padding.type = RSA_NOPAD;
    res = tee_rsa_public_encrypt(pubkey,
                      Digest_Padded, DIGEST_PADDED_SIZE,
                      dst, (size_t *)&dst_size,
                      padding);
    RSA_TEST_ASSERT(TEE_CRYPTO_SUCCESS == res);

    if (dst_size != DIGEST_PADDED_PUB_ENC_NO_PADDING_OUT_SIZE) {
        uart_print_string("ERR: size not match!\n");
        return -1;
    }

    //dump_data("public_encrypt Digest_Padded in:", Digest_Padded, DIGEST_PADDED_SIZE);

    //dump_data("public_encrypt Digest_Padded out:", dst, dst_size);
    padding.type = RSA_NOPAD;
    res = tee_rsa_private_decrypt(keypair,
                      dst, dst_size,
                      dst2, (size_t *)&dst2_size,
                      padding);
    RSA_TEST_ASSERT(TEE_CRYPTO_SUCCESS == res);
    //dump_data("private_decrypt dst out:", dst2, dst2_size);

    if (dst2_size != DIGEST_PADDED_SIZE) {
        uart_print_string("ERR: size not match!\n");
        return -1;
    }

    if (0 != memcmp(Digest_Padded, dst2, dst2_size)) {
        uart_print_string("ERR: data not match!\n");
        return -1;
    }
    uart_print_string("Self Test1 PASS---\n");
    uart_print_string("#######################################\n");
    /* 2. Self encrypt and decrypt test, PKCS1_V1_5 PADDING */
    memset(dst, 0, sizeof(dst));
    memset(dst2, 0, sizeof(dst2));
    dst_size = KEY_BYTES;
    dst2_size = KEY_BYTES;

    padding.type = RSAES_PKCS1_V1_5;
    res = tee_rsa_public_encrypt(pubkey,
                      Digest, DIGEST_SIZE,
                      dst, (size_t *)&dst_size,
                      padding);
    RSA_TEST_ASSERT(TEE_CRYPTO_SUCCESS == res);

    if (dst_size != DIGEST_PUB_ENC_PKCS1_PADDING_OUT_SIZE) {
        uart_print_string("ERR: size not match!\n");
        return -1;
    }

    //dump_data("public_encrypt Digest out:", dst, dst_size);
    padding.type = RSAES_PKCS1_V1_5;
    res = tee_rsa_private_decrypt(keypair,
                      dst, dst_size,
                      dst2, (size_t *)&dst2_size,
                      padding);
    RSA_TEST_ASSERT(TEE_CRYPTO_SUCCESS == res);
    //dump_data("private_decrypt dst out:", dst2, dst2_size);

    if (dst2_size != DIGEST_SIZE) {
        uart_print_string("ERR: size not match!\n");
        return -1;
    }

    if (0 != memcmp(Digest, dst2, dst2_size)) {
        uart_print_string("ERR: data not match!\n");
        return -1;
    }
    rsa_test_clean_key(keypair, pubkey);

    uart_print_string("Self Test2 PASS---\n");

    return 0;
}
