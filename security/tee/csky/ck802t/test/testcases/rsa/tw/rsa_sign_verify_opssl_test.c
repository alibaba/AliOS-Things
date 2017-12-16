/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 */

#include "rsa_test.h"

int32_t rsa_test_v4(void)
{
    tee_crypto_result res;

    uint8_t dst[KEY_BYTES];
    int32_t dst_size = KEY_BYTES;

    uint8_t dst2[KEY_BYTES];
    int32_t dst2_size = KEY_BYTES;

    bool result;

    rsa_keypair_t *keypair;
    rsa_pubkey_t *pubkey;
    rsa_padding_t padding;

    /* 1. Self encrypt and decrypt test, NOPAD */
    memset(dst, 0, sizeof(dst));
    memset(dst2, 0, sizeof(dst2));
    dst_size = KEY_BYTES;
    dst2_size = KEY_BYTES;

    res = rsa_test_format_key(KEY_BITS,
                            modulus, KEY_BYTES,
                            publicExponent, KEY_BYTES,
                            privateExponent, KEY_BYTES,
                            &keypair,
                            &pubkey);
    RSA_TEST_ASSERT(0 == res);

    padding.type = RSASSA_PKCS1_V1_5;
    padding.pad.rsassa_v1_5.type = SHA1;
    res = tee_rsa_sign(keypair,
                      Digest, DIGEST_SIZE,
                      dst, (size_t *)&dst_size,
                      padding);
    RSA_TEST_ASSERT(TEE_CRYPTO_SUCCESS == res);

#if 0
    dump_data("rsa_sign out:", dst, dst_size);
    dump_data("opssl signature out:", Digest_signature_pkcs1_padding_out, DIGEST_SIGNATURE_PKCS1_PADDING_OUT_SIZE);
#endif
    if (dst_size != DIGEST_SIGNATURE_PKCS1_PADDING_OUT_SIZE) {
        uart_print_string("ERR: length not match!\n");
        return -1;
    }
    if (0 != memcmp(dst, Digest_signature_pkcs1_padding_out, dst_size)) {
        uart_print_string("ERR: signature not match!\n");
        return -1;
    }

    padding.type = RSASSA_PKCS1_V1_5;
    padding.pad.rsassa_v1_5.type = SHA1;
    res = tee_rsa_verify(pubkey,
                      Digest, DIGEST_SIZE,
                      dst, dst_size,
                      padding, &result);
    RSA_TEST_ASSERT(TEE_CRYPTO_SUCCESS == res);

    if (true != result) {
        uart_print_string("ERR: verify fail!\n");
        return -1;
    }

    rsa_test_clean_key(keypair, pubkey);

    uart_print_string("Self Test PASS---\n");
    uart_print_string("#######################################\n");

    return 0;
}
