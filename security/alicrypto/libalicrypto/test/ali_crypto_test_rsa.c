/**
 * Copyright (C) 2016 The YunOS Project. All rights reserved.
 */

#include "ali_crypto.h"
#include "ali_crypto_test.h"

#define RSA_KEY_LEN (128)

static uint8_t RSA_1024_N[128] = {
    0xa5, 0x6e, 0x4a, 0x0e, 0x70, 0x10, 0x17, 0x58, 0x9a, 0x51, 0x87, 0xdc,
    0x7e, 0xa8, 0x41, 0xd1, 0x56, 0xf2, 0xec, 0x0e, 0x36, 0xad, 0x52, 0xa4,
    0x4d, 0xfe, 0xb1, 0xe6, 0x1f, 0x7a, 0xd9, 0x91, 0xd8, 0xc5, 0x10, 0x56,
    0xff, 0xed, 0xb1, 0x62, 0xb4, 0xc0, 0xf2, 0x83, 0xa1, 0x2a, 0x88, 0xa3,
    0x94, 0xdf, 0xf5, 0x26, 0xab, 0x72, 0x91, 0xcb, 0xb3, 0x07, 0xce, 0xab,
    0xfc, 0xe0, 0xb1, 0xdf, 0xd5, 0xcd, 0x95, 0x08, 0x09, 0x6d, 0x5b, 0x2b,
    0x8b, 0x6d, 0xf5, 0xd6, 0x71, 0xef, 0x63, 0x77, 0xc0, 0x92, 0x1c, 0xb2,
    0x3c, 0x27, 0x0a, 0x70, 0xe2, 0x59, 0x8e, 0x6f, 0xf8, 0x9d, 0x19, 0xf1,
    0x05, 0xac, 0xc2, 0xd3, 0xf0, 0xcb, 0x35, 0xf2, 0x92, 0x80, 0xe1, 0x38,
    0x6b, 0x6f, 0x64, 0xc4, 0xef, 0x22, 0xe1, 0xe1, 0xf2, 0x0d, 0x0c, 0xe8,
    0xcf, 0xfb, 0x22, 0x49, 0xbd, 0x9a, 0x21, 0x37
};
static uint8_t RSA_1024_E[3]   = { 0x01, 0x00, 0x01 };
static uint8_t RSA_1024_D[128] = {
    0x33, 0xa5, 0x04, 0x2a, 0x90, 0xb2, 0x7d, 0x4f, 0x54, 0x51, 0xca, 0x9b,
    0xbb, 0xd0, 0xb4, 0x47, 0x71, 0xa1, 0x01, 0xaf, 0x88, 0x43, 0x40, 0xae,
    0xf9, 0x88, 0x5f, 0x2a, 0x4b, 0xbe, 0x92, 0xe8, 0x94, 0xa7, 0x24, 0xac,
    0x3c, 0x56, 0x8c, 0x8f, 0x97, 0x85, 0x3a, 0xd0, 0x7c, 0x02, 0x66, 0xc8,
    0xc6, 0xa3, 0xca, 0x09, 0x29, 0xf1, 0xe8, 0xf1, 0x12, 0x31, 0x88, 0x44,
    0x29, 0xfc, 0x4d, 0x9a, 0xe5, 0x5f, 0xee, 0x89, 0x6a, 0x10, 0xce, 0x70,
    0x7c, 0x3e, 0xd7, 0xe7, 0x34, 0xe4, 0x47, 0x27, 0xa3, 0x95, 0x74, 0x50,
    0x1a, 0x53, 0x26, 0x83, 0x10, 0x9c, 0x2a, 0xba, 0xca, 0xba, 0x28, 0x3c,
    0x31, 0xb4, 0xbd, 0x2f, 0x53, 0xc3, 0xee, 0x37, 0xe3, 0x52, 0xce, 0xe3,
    0x4f, 0x9e, 0x50, 0x3b, 0xd8, 0x0c, 0x06, 0x22, 0xad, 0x79, 0xc6, 0xdc,
    0xee, 0x88, 0x35, 0x47, 0xc6, 0xa3, 0xb3, 0x25
};

static int _rsa_test_gen_key(void)
{
    int           ret, result;
    uint8_t       rsa_n[RSA_KEY_LEN];
    uint8_t       rsa_e[RSA_KEY_LEN];
    uint8_t       rsa_d[RSA_KEY_LEN];
    uint8_t       rsa_p[RSA_KEY_LEN];
    uint8_t       rsa_q[RSA_KEY_LEN];
    uint8_t       rsa_dp[RSA_KEY_LEN];
    uint8_t       rsa_dq[RSA_KEY_LEN];
    uint8_t       rsa_qp[RSA_KEY_LEN];
    uint32_t      n_size   = RSA_KEY_LEN;
    uint32_t      e_size   = RSA_KEY_LEN;
    uint32_t      d_size   = RSA_KEY_LEN;
    uint32_t      p_size   = RSA_KEY_LEN;
    uint32_t      q_size   = RSA_KEY_LEN;
    uint32_t      dp_size  = RSA_KEY_LEN;
    uint32_t      dq_size  = RSA_KEY_LEN;
    uint32_t      qp_size  = RSA_KEY_LEN;
    uint8_t *     pub_key  = NULL;
    uint8_t *     key_pair = NULL;
    size_t        pub_key_len, key_pair_len;
    uint8_t       src_data[RSA_KEY_LEN];
    uint8_t       ciphertext[RSA_KEY_LEN];
    uint8_t       plaintext[RSA_KEY_LEN];
    size_t        src_size, dst_size;
    rsa_padding_t rsa_padding;

    (void)result;

    CRYPT_INF("rsa gen key test!\n");
    /* for gcov coverage */
    ret = ali_rsa_get_pubkey_size(RSA_KEY_LEN << 3, NULL);
    if (ret == ALI_CRYPTO_SUCCESS) {
        return -1;
    }

    /* for gcov coverage */
    ret = ali_rsa_get_pubkey_size(255, &pub_key_len);
    if (ret == ALI_CRYPTO_SUCCESS) {
        return -1;
    }

    /* for gcov coverage */
    ret = ali_rsa_get_keypair_size(RSA_KEY_LEN << 3, NULL);
    if (ret == ALI_CRYPTO_SUCCESS) {
        return -1;
    }

    /* for gcov coverage */
    ret = ali_rsa_get_keypair_size(255, &key_pair_len);
    if (ret == ALI_CRYPTO_SUCCESS) {
        return -1;
    }

    /* for gcov coverage */
    ret = ali_rsa_init_keypair(RSA_KEY_LEN << 3, rsa_n, n_size, rsa_e, e_size,
                               rsa_d, d_size, NULL, 0, NULL, 0, NULL, 0, NULL,
                               0, NULL, 0, NULL);
    if (ret == ALI_CRYPTO_SUCCESS) {
        return -1;
    }

    /* for gcov coverage */
    ret =
      ali_rsa_init_pubkey(RSA_KEY_LEN << 3, rsa_n, n_size, rsa_e, e_size, NULL);
    if (ret == ALI_CRYPTO_SUCCESS) {
        return -1;
    }

    /* for gcov coverage */
    ret =
      ali_rsa_init_pubkey(RSA_KEY_LEN << 3, rsa_n, n_size, rsa_e, e_size, NULL);
    if (ret == ALI_CRYPTO_SUCCESS) {
        return -1;
    }

    ret = ali_rsa_get_pubkey_size(RSA_KEY_LEN << 3, &pub_key_len);
    if (ret != ALI_CRYPTO_SUCCESS) {
        PRINT_RET(-1, "init_key: get pubkey size fail(%08x)\n", ret)
    }
    ret = ali_rsa_get_keypair_size(RSA_KEY_LEN << 3, &key_pair_len);
    if (ret != ALI_CRYPTO_SUCCESS) {
        PRINT_RET(-1, "init_key: get keypair size fail(%08x)\n", ret)
    }

    pub_key = CRYPT_MALLOC(pub_key_len);
    if (pub_key == NULL) {
        GO_RET(-1, "init_key: malloc(%d) fail\n", (int)pub_key_len);
    }
    key_pair = CRYPT_MALLOC(key_pair_len);
    if (pub_key == NULL) {
        GO_RET(-1, "init_key: malloc(%d) fail\n", (int)pub_key_len);
    }

    /* for gcov coverage */
    ret = ali_rsa_gen_keypair(RSA_KEY_LEN << 3, NULL, 0, NULL);
    if (ret == ALI_CRYPTO_SUCCESS) {
        GO_RET(ALI_CRYPTO_ERROR, "ali_rsa_gen_keypair: not expect\n");
    }

    ret =
      ali_rsa_gen_keypair(RSA_KEY_LEN << 3, NULL, 0, (rsa_keypair_t *)key_pair);
    if (ret != ALI_CRYPTO_SUCCESS) {
        GO_RET(-1, "init_key: gen keypair fail(%08x)\n", ret);
    }

    /* for gcov coverage */
    ret = ali_rsa_get_key_attr(RSA_MODULUS, NULL, rsa_n, (size_t *)&n_size);
    if (ret == ALI_CRYPTO_SUCCESS) {
        GO_RET(ALI_CRYPTO_ERROR, "ali_rsa_get_key_attr: not expect\n");
    }
    /* for gcov coverage */
    ret = ali_rsa_get_key_attr((rsa_key_attr_t)-1, (rsa_keypair_t *)key_pair,
                               rsa_n, (size_t *)&n_size);
    if (ret == ALI_CRYPTO_SUCCESS) {
        GO_RET(ALI_CRYPTO_ERROR, "ali_rsa_get_key_attr: not expect\n");
    }

    /* get key attrs */
    ret = ali_rsa_get_key_attr(RSA_MODULUS, (rsa_keypair_t *)key_pair, rsa_n,
                               (size_t *)&n_size);
    if (ret != ALI_CRYPTO_SUCCESS) {
        goto _OUT;
    }
    ret = ali_rsa_get_key_attr(RSA_PUBLIC_EXPONENT, (rsa_keypair_t *)key_pair,
                               rsa_e, (size_t *)&e_size);
    if (ret != ALI_CRYPTO_SUCCESS) {
        goto _OUT;
    }
    ret = ali_rsa_get_key_attr(RSA_PRIVATE_EXPONENT, (rsa_keypair_t *)key_pair,
                               rsa_d, (size_t *)&d_size);
    if (ret != ALI_CRYPTO_SUCCESS) {
        goto _OUT;
    }
    ret = ali_rsa_get_key_attr(RSA_PRIME1, (rsa_keypair_t *)key_pair, rsa_p,
                               (size_t *)&p_size);
    if (ret != ALI_CRYPTO_SUCCESS) {
        goto _OUT;
    }
    ret = ali_rsa_get_key_attr(RSA_PRIME2, (rsa_keypair_t *)key_pair, rsa_q,
                               (size_t *)&q_size);
    if (ret != ALI_CRYPTO_SUCCESS) {
        goto _OUT;
    }
    ret = ali_rsa_get_key_attr(RSA_EXPONENT1, (rsa_keypair_t *)key_pair, rsa_dp,
                               (size_t *)&dp_size);
    if (ret != ALI_CRYPTO_SUCCESS) {
        goto _OUT;
    }
    ret = ali_rsa_get_key_attr(RSA_EXPONENT2, (rsa_keypair_t *)key_pair, rsa_dq,
                               (size_t *)&dq_size);
    if (ret != ALI_CRYPTO_SUCCESS) {
        goto _OUT;
    }
    ret = ali_rsa_get_key_attr(RSA_COEFFICIENT, (rsa_keypair_t *)key_pair,
                               rsa_qp, (size_t *)&qp_size);
    if (ret != ALI_CRYPTO_SUCCESS) {
        goto _OUT;
    }

    /* for gcov coverage */
    ret = ali_rsa_init_keypair(RSA_KEY_LEN << 3, rsa_n, 129, rsa_e, e_size,
                               rsa_d, d_size, NULL, 0, NULL, 0, NULL, 0, NULL,
                               0, NULL, 0, (rsa_keypair_t *)key_pair);
    if (ret == ALI_CRYPTO_SUCCESS) {
        GO_RET(ALI_CRYPTO_ERROR, "ali_rsa_init_keypair: not expect\n");
    }

    /* for gcov coverage */
    ret = ali_rsa_init_keypair(RSA_KEY_LEN << 3, rsa_n, n_size, rsa_e, e_size,
                               rsa_d, d_size, rsa_p, p_size, rsa_q, q_size,
                               rsa_dp, dp_size, rsa_dq, dq_size, rsa_qp,
                               qp_size, (rsa_keypair_t *)key_pair);
    if (ret != ALI_CRYPTO_SUCCESS) {
        goto _OUT;
    }

    CRYPT_MEMSET(key_pair, 0, key_pair_len);
    ret = ali_rsa_init_keypair(RSA_KEY_LEN << 3, rsa_n, n_size, rsa_e, e_size,
                               rsa_d, d_size, NULL, 0, NULL, 0, NULL, 0, NULL,
                               0, NULL, 0, (rsa_keypair_t *)key_pair);
    if (ret != ALI_CRYPTO_SUCCESS) {
        GO_RET(ret, "init_key: init keypair fail(%08x)\n", ret);
    }

    /* for gcov coverage */
    ret = ali_rsa_init_pubkey(RSA_KEY_LEN << 3, rsa_n, 129, rsa_e, e_size,
                              (rsa_pubkey_t *)pub_key);
    if (ret == ALI_CRYPTO_SUCCESS) {
        GO_RET(ALI_CRYPTO_ERROR, "ali_rsa_init_pubkey: not expect\n");
    }

    CRYPT_MEMSET(pub_key, 0, pub_key_len);
    ret = ali_rsa_init_pubkey(RSA_KEY_LEN << 3, rsa_n, n_size, rsa_e, e_size,
                              (rsa_pubkey_t *)pub_key);
    if (ret != ALI_CRYPTO_SUCCESS) {
        GO_RET(ALI_CRYPTO_ERROR, "init_key: init pub_key fail(%08x)\n", ret);
    }

    rsa_padding.type = RSAES_PKCS1_V1_5;
    /* for gcov coverage */
    src_size = RSA_KEY_LEN;
    dst_size = RSA_KEY_LEN;
    ret = ali_rsa_public_encrypt((const rsa_pubkey_t *)NULL, src_data, src_size,
                                 ciphertext, &dst_size, rsa_padding);
    if (ret == ALI_CRYPTO_SUCCESS) {
        GO_RET(ALI_CRYPTO_ERROR, "ali_rsa_public_encrypt: not expect\n");
    }

    /* for gcov coverage */
    ret = ali_rsa_private_decrypt(NULL, ciphertext, RSA_KEY_LEN, plaintext,
                                  &dst_size, rsa_padding);
    if (ret == ALI_CRYPTO_SUCCESS) {
        GO_RET(ALI_CRYPTO_ERROR, "ali_rsa_private_decrypt: not expect\n");
    }

    CRYPT_MEMSET(&rsa_padding, 0, sizeof(rsa_padding_t));
    rsa_padding.type = RSAES_PKCS1_V1_5;
    src_size         = RSA_KEY_LEN - 11;
    CRYPT_MEMSET(src_data, 0xa, src_size);

    dst_size = RSA_KEY_LEN;
    ret      = ali_rsa_public_encrypt((const rsa_pubkey_t *)pub_key, src_data,
                                 src_size, ciphertext, &dst_size, rsa_padding);
    if (ret != ALI_CRYPTO_SUCCESS) {
        GO_RET(ALI_CRYPTO_ERROR, "ali_rsa_public_encrypt: rsa_v1_5 fail %d\n",
               ret);
    }

    ret =
      ali_rsa_private_decrypt((const rsa_keypair_t *)key_pair, ciphertext,
                              RSA_KEY_LEN, plaintext, &dst_size, rsa_padding);
    if (ret != ALI_CRYPTO_SUCCESS || dst_size != src_size) {
        GO_RET(ALI_CRYPTO_ERROR, "ali_rsa_private_decrypt: rsa_v1_5 fail %d\n",
               ret);
    }
    if (CRYPT_MEMCMP(src_data, plaintext, src_size)) {
        ali_crypto_print_data("pliantext", plaintext, src_size);
        ali_crypto_print_data("ciphertext", ciphertext, dst_size);
        PRINT_RET(-1, "RSA encrypt/decrypt with PKCS1_V1_5 test fail!\n");
    } else {
        CRYPT_INF("RSA encrypt/decrypt with PKCS1_V1_5 test success!\n");
    }

    CRYPT_FREE(pub_key);
    CRYPT_FREE(key_pair);
    return 0;
_OUT:
    if (pub_key) {
        CRYPT_FREE(pub_key);
    }
    if (key_pair) {
        CRYPT_FREE(key_pair);
    }

    return -1;
}

static int _ali_crypto_init_key(rsa_keypair_t **keypair, rsa_pubkey_t **pubkey)
{
    ali_crypto_result ret, result;
    uint8_t           rsa_n[RSA_KEY_LEN];
    uint8_t           rsa_e[RSA_KEY_LEN];
    uint8_t           rsa_d[RSA_KEY_LEN];
    uint32_t          n_size   = RSA_KEY_LEN;
    uint32_t          e_size   = RSA_KEY_LEN;
    uint32_t          d_size   = RSA_KEY_LEN;
    uint8_t *         pub_key  = NULL;
    uint8_t *         key_pair = NULL;
    size_t            pub_key_len, key_pair_len;

    (void)e_size;
    (void)rsa_d;
    (void)rsa_e;
    (void)rsa_n;
    (void)result;
    if (keypair == NULL || pubkey == NULL) {
        PRINT_RET(-1, "init_key: invalid input args!\n");
    }

    ret = ali_rsa_get_pubkey_size(RSA_KEY_LEN << 3, &pub_key_len);
    if (ret != ALI_CRYPTO_SUCCESS) {
        PRINT_RET(-1, "init_key: get pubkey size fail(%08x)\n", ret);
    }
    ret = ali_rsa_get_keypair_size(RSA_KEY_LEN << 3, &key_pair_len);
    if (ret != ALI_CRYPTO_SUCCESS) {
        PRINT_RET(-1, "init_key: get keypair size fail(%08x)\n", ret);
    }

    pub_key = CRYPT_MALLOC(pub_key_len);
    if (pub_key == NULL) {
        GO_RET(-1, "init_key: malloc(%d) fail\n", (int)pub_key_len);
    }
    key_pair = CRYPT_MALLOC(key_pair_len);
    if (pub_key == NULL) {
        GO_RET(-1, "init_key: malloc(%d) fail\n", (int)pub_key_len);
    }

#if 0
    ret = ali_rsa_gen_keypair(RSA_KEY_LEN << 3, NULL, 0, (rsa_keypair_t *)key_pair);
    if (ret != ALI_CRYPTO_SUCCESS) {
        CRYPT_ERR("init_key: gen keypair fail(%08x)\n", ret);
        goto _OUT;
    }

    /* get key attrs */
    ret = ali_rsa_get_key_attr(RSA_MODULUS,
                               (rsa_keypair_t *)key_pair, rsa_n, (size_t *)&n_size);
    if (ret != ALI_CRYPTO_SUCCESS) {
        goto _OUT;
    }
    ret = ali_rsa_get_key_attr(RSA_PUBLIC_EXPONENT,
                               (rsa_keypair_t *)key_pair, rsa_e, (size_t *)&e_size);
    if (ret != ALI_CRYPTO_SUCCESS) {
        goto _OUT;
    }
    ret = ali_rsa_get_key_attr(RSA_PRIVATE_EXPONENT,
                               (rsa_keypair_t *)key_pair, rsa_d, (size_t *)&d_size);
    if (ret != ALI_CRYPTO_SUCCESS) {
        goto _OUT;
    }

    CRYPT_MEMSET(key_pair, 0, key_pair_len);
    ret = ali_rsa_init_keypair(RSA_KEY_LEN << 3,
                               rsa_n, n_size, rsa_e, e_size, rsa_d, d_size,
                               NULL , 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, (rsa_keypair_t *)key_pair);
    if (ret != ALI_CRYPTO_SUCCESS) {
        CRYPT_ERR("init_key: init keypair fail(%08x)\n", ret);
        goto _OUT;
    }

    CRYPT_MEMSET(pub_key, 0, pub_key_len);
    ret = ali_rsa_init_pubkey(RSA_KEY_LEN << 3,
                              rsa_n, n_size, rsa_e, e_size, (rsa_pubkey_t *)pub_key);
    if (ret != ALI_CRYPTO_SUCCESS) {
        CRYPT_ERR("init_key: init pub_key fail(%08x)\n", ret);
        goto _OUT;
    }
#else
    CRYPT_MEMSET(key_pair, 0, key_pair_len);
    ret = ali_rsa_init_keypair(RSA_KEY_LEN << 3, RSA_1024_N, n_size, RSA_1024_E,
                               3, RSA_1024_D, d_size, NULL, 0, NULL, 0, NULL, 0,
                               NULL, 0, NULL, 0, (rsa_keypair_t *)key_pair);
    if (ret != ALI_CRYPTO_SUCCESS) {
        GO_RET(-1, "init_key: init keypair fail(%08x)\n", ret);
    }

    CRYPT_MEMSET(pub_key, 0, pub_key_len);
    ret = ali_rsa_init_pubkey(RSA_KEY_LEN << 3, RSA_1024_N, n_size, RSA_1024_E,
                              3, (rsa_pubkey_t *)pub_key);
    if (ret != ALI_CRYPTO_SUCCESS) {
        GO_RET(-1, "init_key: init pub_key fail(%08x)\n", ret);
    }
#endif

    *pubkey  = (rsa_pubkey_t *)pub_key;
    *keypair = (rsa_keypair_t *)key_pair;

    return 0;

_OUT:
    if (pub_key) {
        CRYPT_FREE(pub_key);
    }
    if (key_pair) {
        CRYPT_FREE(key_pair);
    }

    return -1;
}

#if 0
static int _ali_crypto_encrypt_decrypt_nopad(
    rsa_pubkey_t *pubkey, rsa_keypair_t *keypair)
{
    ali_crypto_result ret;
    uint8_t src_data[RSA_KEY_LEN];
    uint8_t plaintext[RSA_KEY_LEN];
    uint8_t ciphertext[RSA_KEY_LEN];
    size_t src_size, dst_size;
    rsa_padding_t rsa_padding;

    if (pubkey == NULL || keypair == NULL) {
        CRYPT_ERR("rsa_nopad: invalid input args!\n");
        return -1;
    }

    rsa_padding.type = RSA_NOPAD;
    src_size = RSA_KEY_LEN;
    CRYPT_MEMSET(src_data, 0xa, src_size);

    dst_size = RSA_KEY_LEN;
    ret = ali_rsa_public_encrypt(pubkey, src_data, src_size,
                                 ciphertext, &dst_size, rsa_padding);
    if (ret != ALI_CRYPTO_SUCCESS) {
        CRYPT_ERR("rsa_nopad: public encrypt fail(%08x)\n", ret);
        return -1;
    }

    ret = ali_rsa_private_decrypt(keypair, ciphertext, RSA_KEY_LEN,
                                  plaintext, &dst_size, rsa_padding);
    if (ret != ALI_CRYPTO_SUCCESS || dst_size != src_size) {
        CRYPT_ERR("rsa_nopad: private decrypt fail(%08x)\n", ret);
        return -1;
    }

    if (CRYPT_MEMCMP(src_data, plaintext, src_size)) {
        CRYPT_ERR("RSA encrypt/decrypt with no-padding test fail!\n");
        ali_crypto_print_data("pliantext", plaintext, src_size);
        ali_crypto_print_data("ciphertext", ciphertext, dst_size);
    } else {
        CRYPT_INF("RSA encrypt/decrypt with no-padding test success!\n");
    }

    return 0;
}
#endif

static int _ali_crypto_encrypt_decrypt_v1_5(rsa_pubkey_t * pubkey,
                                            rsa_keypair_t *keypair)
{
    ali_crypto_result ret;
    uint8_t           src_data[RSA_KEY_LEN];
    uint8_t           plaintext[RSA_KEY_LEN];
    uint8_t           ciphertext[RSA_KEY_LEN];
    size_t            src_size, dst_size;
    rsa_padding_t     rsa_padding;

    if (pubkey == NULL || keypair == NULL) {
        PRINT_RET(-1, "rsa_v1_5: invalid input args!\n");
    }

    CRYPT_MEMSET(&rsa_padding, 0, sizeof(rsa_padding_t));
    rsa_padding.type = RSAES_PKCS1_V1_5;
    src_size         = RSA_KEY_LEN - 11;
    CRYPT_MEMSET(src_data, 0xa, src_size);

    dst_size = RSA_KEY_LEN;
    ret      = ali_rsa_public_encrypt(pubkey, src_data, src_size, ciphertext,
                                 &dst_size, rsa_padding);
    if (ret != ALI_CRYPTO_SUCCESS) {
        PRINT_RET(-1, "rsa_v1_5: public encrypt fail(%08x)\n", ret);
    }

    /* for gcov coverage */
    ret = ali_rsa_public_encrypt(pubkey, src_data, RSA_KEY_LEN, ciphertext,
                                 &dst_size, rsa_padding);
    if (ret == ALI_CRYPTO_SUCCESS) {
        return -1;
    }

    ret = ali_rsa_private_decrypt(keypair, ciphertext, RSA_KEY_LEN, plaintext,
                                  &dst_size, rsa_padding);
    if (ret != ALI_CRYPTO_SUCCESS || dst_size != src_size) {
        PRINT_RET(-1, "rsa_v1_5: public decrypt fail(%08x)\n", ret);
    }

    if (CRYPT_MEMCMP(src_data, plaintext, src_size)) {
        ali_crypto_print_data("pliantext", plaintext, src_size);
        ali_crypto_print_data("ciphertext", ciphertext, dst_size);
        PRINT_RET(-1, "RSA encrypt/decrypt with PKCS1_V1_5 test fail!\n");
    } else {
        CRYPT_INF("RSA encrypt/decrypt with PKCS1_V1_5 test success!\n");
    }

    return 0;
}

static int _ali_crypto_encrypt_decrypt_oaep(rsa_pubkey_t * pubkey,
                                            rsa_keypair_t *keypair)
{
    ali_crypto_result ret;
    hash_type_t       hash_type;
    rsa_padding_t     rsa_padding;
    uint8_t           src_data[RSA_KEY_LEN];
    uint8_t           plaintext[RSA_KEY_LEN];
    uint8_t           ciphertext[RSA_KEY_LEN];
    size_t            src_size, dst_size;
    // uint8_t lparam[] = {0xe1, 0xe2, 0xe2, 0xe4, 0xe5};

    if (pubkey == NULL || keypair == NULL) {
        CRYPT_ERR("rsa_v1_5: invalid input args!\n");
        PRINT_RET(-1, "rsa_v1_5: invalid input args!\n");
    }

    rsa_padding.type = RSAES_PKCS1_OAEP_MGF1;

    for (hash_type = SHA1; hash_type <= MD5; hash_type++) {
        if (hash_type == SHA512 || hash_type == SHA384) {
            CRYPT_INF("rsa oeap not support hash 384 512\n");
            continue;
        }
        /*
                if (2*HASH_SIZE(hash_type) >= RSA_KEY_LEN - 2) {
                    continue;
                }
        */
        src_size = RSA_KEY_LEN - 2 * HASH_SIZE(hash_type) - 2;
        CRYPT_MEMSET(src_data, 0xa, src_size);

        /* without lparam */
        rsa_padding.pad.rsaes_oaep.type = hash_type;
        dst_size                        = RSA_KEY_LEN;
        ret = ali_rsa_public_encrypt(pubkey, src_data, src_size, ciphertext,
                                     &dst_size, rsa_padding);
        if (ret != ALI_CRYPTO_SUCCESS) {
            PRINT_RET(
              -1, "rsa_oaep: public encrypt(without lparam) fail(%08x)\n", ret);
        }
        ret = ali_rsa_private_decrypt(keypair, ciphertext, RSA_KEY_LEN,
                                      plaintext, &dst_size, rsa_padding);
        if (ret != ALI_CRYPTO_SUCCESS || dst_size != src_size) {
            PRINT_RET(-1,
                      "rsa_oaep: private decrypt(without lparam) fail(%08x)\n",
                      ret);
        }
        if (CRYPT_MEMCMP(src_data, plaintext, src_size)) {
            ali_crypto_print_data("pliantext", plaintext, src_size);
            ali_crypto_print_data("ciphertext", ciphertext, dst_size);
            PRINT_RET(-1, "RSA encrypt/decrypt with PKCS1_OAEP(without lparam) "
                          "test fail!\n");
        } else {
            CRYPT_INF("RSA encrypt/decrypt with PKCS1_OAEP(without lparam) "
                      "test success!\n");
        }

        /* with lparam */
        rsa_padding.pad.rsaes_oaep.type = hash_type;
        dst_size                        = RSA_KEY_LEN;
        ret = ali_rsa_public_encrypt(pubkey, src_data, src_size, ciphertext,
                                     &dst_size, rsa_padding);
        if (ret != ALI_CRYPTO_SUCCESS) {
            PRINT_RET(-1, "rsa_oaep: public encrypt(with lparam) fail(%08x)\n",
                      ret);
        }
        ret = ali_rsa_private_decrypt(keypair, ciphertext, RSA_KEY_LEN,
                                      plaintext, &dst_size, rsa_padding);
        if (ret != ALI_CRYPTO_SUCCESS || dst_size != src_size) {
            PRINT_RET(-1, "rsa_oaep: private decrypt(with lparam) fail(%08x)\n",
                      ret);
        }
        if (CRYPT_MEMCMP(src_data, plaintext, src_size)) {
            ali_crypto_print_data("pliantext", plaintext, src_size);
            ali_crypto_print_data("ciphertext", ciphertext, dst_size);
            PRINT_RET(
              -1,
              "RSA encrypt/decrypt with PKCS1_OAEP(with lparam) test fail!\n");
        } else {
            CRYPT_INF("RSA encrypt/decrypt with PKCS1_OAEP(with lparam) test "
                      "success!\n");
        }
    }

    return 0;
}

static int _ali_crypto_sign_verify_v1_5(rsa_pubkey_t * pubkey,
                                        rsa_keypair_t *keypair)
{
    bool              result1, result2;
    ali_crypto_result ret;
    hash_type_t       hash_type;
    uint8_t           src_data[RSA_KEY_LEN];
    uint8_t           signature[RSA_KEY_LEN];
    size_t            src_size, dst_size;
    rsa_padding_t     rsa_padding;

    if (pubkey == NULL || keypair == NULL) {
        PRINT_RET(-1, "rsa_v1_5: invalid input args!\n");
    }

    rsa_padding.type = RSASSA_PKCS1_V1_5;
    for (hash_type = SHA1; hash_type <= MD5; hash_type++) {
        rsa_padding.pad.rsassa_v1_5.type = hash_type;

        if (hash_type == SHA512 || hash_type == SHA384) {
            CRYPT_INF("mbedtls rsa V1.5 not support hash 384 512\n");
            continue;
        }
#if 0
        if (HASH_SIZE(hash_type) + 11 > RSA_KEY_LEN) {
            continue;
        }
#endif

        src_size = HASH_SIZE(hash_type);
        CRYPT_MEMSET(src_data, 0xa, src_size);

        dst_size = RSA_KEY_LEN;
        ret = ali_rsa_sign(keypair, src_data, src_size, signature, &dst_size,
                           rsa_padding);
        if (ret != ALI_CRYPTO_SUCCESS) {
            PRINT_RET(-1, "rsa_v1_5: sign fail(%08x)\n", ret);
        }

        ret = ali_rsa_verify(pubkey, src_data, src_size, signature, dst_size,
                             rsa_padding, &result1);
        if (ret != ALI_CRYPTO_SUCCESS) {
            PRINT_RET(-1, "rsa_v1_5: verify fail(%08x)\n", ret);
        }

        src_data[0] = src_data[0] ^ 0x1;
        ret = ali_rsa_verify(pubkey, src_data, src_size, signature, dst_size,
                             rsa_padding, &result2);
        if (ret == ALI_CRYPTO_SUCCESS) {
            PRINT_RET(-1, "rsa_v1_5: verify fail(%08x)\n", ret);
        }

        if (result1 == true && result2 == false) {
            CRYPT_INF("RSA sign/verify with PKCS1_V1_5 success!\n");
        } else {
            PRINT_RET(-1, "RSA sign/verify with PKCS1_V1_5 fail!\n");
        }
    }

    return 0;
}

static int _ali_crypto_sign_verify_pss(rsa_pubkey_t * pubkey,
                                       rsa_keypair_t *keypair)
{
    bool              result1, result2;
    ali_crypto_result ret;
    hash_type_t       hash_type;
    uint8_t           src_data[RSA_KEY_LEN];
    uint8_t           signature[RSA_KEY_LEN];
    size_t            src_size, dst_size;
    rsa_padding_t     rsa_padding;

    if (pubkey == NULL || keypair == NULL) {
        PRINT_RET(-1, "rsa_pss: invalid input args!\n");
    }

    rsa_padding.type = RSASSA_PKCS1_PSS_MGF1;
    for (hash_type = SHA1; hash_type <= MD5; hash_type++) {
        rsa_padding.pad.rsassa_pss.type     = hash_type;
        rsa_padding.pad.rsassa_pss.salt_len = 28;

        if (hash_type == SHA512 || hash_type == SHA384) {
            CRYPT_INF("mbedtls rsa pss not support hash 512\n");
            continue;
        }
#if 0
        if (HASH_SIZE(hash_type) + 28 + 2 > RSA_KEY_LEN) {
            continue;
        }
#endif

        src_size = HASH_SIZE(hash_type);

        CRYPT_MEMSET(src_data, 0xa, src_size);

        dst_size = RSA_KEY_LEN;
        ret = ali_rsa_sign(keypair, src_data, src_size, signature, &dst_size,
                           rsa_padding);
        if (ret != ALI_CRYPTO_SUCCESS) {
            PRINT_RET(-1, "rsa_pss: sign fail(%08x)\n", ret);
        }

        ret = ali_rsa_verify(pubkey, src_data, src_size, signature, dst_size,
                             rsa_padding, &result1);
        if (ret != ALI_CRYPTO_SUCCESS) {
            PRINT_RET(-1, "rsa_pss: verify fail(%08x)\n", ret);
        }

        src_data[0] = src_data[0] ^ 0x1;
        ret = ali_rsa_verify(pubkey, src_data, src_size, signature, dst_size,
                             rsa_padding, &result2);
        if (ret == ALI_CRYPTO_SUCCESS) {
            PRINT_RET(-1, "rsa_pss: verify fail(%08x)\n", ret);
        }

        if (result1 == true && result2 == false) {
            CRYPT_INF("RSA sign/verify with PKCS1_PSS_MGF1 success!\n");
        } else {
            PRINT_RET(-1, "RSA sign/verify with PKCS1_PSS_MGF1 fail!\n");
        }
    }

    return 0;
}

int ali_crypto_rsa_test(void)
{
    int            ret;
    rsa_pubkey_t * pubkey  = NULL;
    rsa_keypair_t *keypair = NULL;

    ret = _rsa_test_gen_key();
    if (ret < 0) {
        goto _out;
    }

    ret = _ali_crypto_init_key(&keypair, &pubkey);
    if (ret < 0) {
        goto _out;
    }

    /* TODO */
#if 0
    ret = _ali_crypto_encrypt_decrypt_nopad(pubkey, keypair);
    if (ret < 0) {
        goto _out;
    }
#endif

    ret = _ali_crypto_encrypt_decrypt_v1_5(pubkey, keypair);
    if (ret < 0) {
        goto _out;
    }

    ret = _ali_crypto_encrypt_decrypt_oaep(pubkey, keypair);
    if (ret < 0) {
        goto _out;
    }

    ret = _ali_crypto_sign_verify_v1_5(pubkey, keypair);
    if (ret < 0) {
        goto _out;
    }

    ret = _ali_crypto_sign_verify_pss(pubkey, keypair);
    if (ret < 0) {
        goto _out;
    }

_out:
    if (pubkey) {
        CRYPT_FREE(pubkey);
    }
    if (keypair) {
        CRYPT_FREE(keypair);
    }
    if (0 == ret) {
        CRYPT_INF("================ALI crypto test SUCCESS!\n");
    } else {
        CRYPT_INF("================ALI crypto test FAIL!\n");
    }

    return ret;
}
