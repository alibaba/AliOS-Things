/**
 * Copyright (C) 2016 The YunOS Project. All rights reserved.
 */

#include "tee_osa.h"
#include "tomcrypt.h"

static int _g_prng_idx = 0;
static prng_state _g_prng_state;

static uint8_t RSA_1024_N[128] = {
    0xa5, 0x6e, 0x4a, 0x0e, 0x70, 0x10, 0x17, 0x58,
    0x9a, 0x51, 0x87, 0xdc, 0x7e, 0xa8, 0x41, 0xd1,
    0x56, 0xf2, 0xec, 0x0e, 0x36, 0xad, 0x52, 0xa4,
    0x4d, 0xfe, 0xb1, 0xe6, 0x1f, 0x7a, 0xd9, 0x91,
    0xd8, 0xc5, 0x10, 0x56, 0xff, 0xed, 0xb1, 0x62,
    0xb4, 0xc0, 0xf2, 0x83, 0xa1, 0x2a, 0x88, 0xa3,
    0x94, 0xdf, 0xf5, 0x26, 0xab, 0x72, 0x91, 0xcb,
    0xb3, 0x07, 0xce, 0xab, 0xfc, 0xe0, 0xb1, 0xdf,
    0xd5, 0xcd, 0x95, 0x08, 0x09, 0x6d, 0x5b, 0x2b,
    0x8b, 0x6d, 0xf5, 0xd6, 0x71, 0xef, 0x63, 0x77,
    0xc0, 0x92, 0x1c, 0xb2, 0x3c, 0x27, 0x0a, 0x70,
    0xe2, 0x59, 0x8e, 0x6f, 0xf8, 0x9d, 0x19, 0xf1,
    0x05, 0xac, 0xc2, 0xd3, 0xf0, 0xcb, 0x35, 0xf2,
    0x92, 0x80, 0xe1, 0x38, 0x6b, 0x6f, 0x64, 0xc4,
    0xef, 0x22, 0xe1, 0xe1, 0xf2, 0x0d, 0x0c, 0xe8,
    0xcf, 0xfb, 0x22, 0x49, 0xbd, 0x9a, 0x21, 0x37
};
static uint8_t RSA_1024_E[3] = {0x01, 0x00, 0x01};
static uint8_t RSA_1024_D[128] = {
    0x33, 0xa5, 0x04, 0x2a, 0x90, 0xb2, 0x7d, 0x4f,
    0x54, 0x51, 0xca, 0x9b, 0xbb, 0xd0, 0xb4, 0x47,
    0x71, 0xa1, 0x01, 0xaf, 0x88, 0x43, 0x40, 0xae,
    0xf9, 0x88, 0x5f, 0x2a, 0x4b, 0xbe, 0x92, 0xe8,
    0x94, 0xa7, 0x24, 0xac, 0x3c, 0x56, 0x8c, 0x8f,
    0x97, 0x85, 0x3a, 0xd0, 0x7c, 0x02, 0x66, 0xc8,
    0xc6, 0xa3, 0xca, 0x09, 0x29, 0xf1, 0xe8, 0xf1,
    0x12, 0x31, 0x88, 0x44, 0x29, 0xfc, 0x4d, 0x9a,
    0xe5, 0x5f, 0xee, 0x89, 0x6a, 0x10, 0xce, 0x70,
    0x7c, 0x3e, 0xd7, 0xe7, 0x34, 0xe4, 0x47, 0x27,
    0xa3, 0x95, 0x74, 0x50, 0x1a, 0x53, 0x26, 0x83,
    0x10, 0x9c, 0x2a, 0xba, 0xca, 0xba, 0x28, 0x3c,
    0x31, 0xb4, 0xbd, 0x2f, 0x53, 0xc3, 0xee, 0x37,
    0xe3, 0x52, 0xce, 0xe3, 0x4f, 0x9e, 0x50, 0x3b,
    0xd8, 0x0c, 0x06, 0x22, 0xad, 0x79, 0xc6, 0xdc,
    0xee, 0x88, 0x35, 0x47, 0xc6, 0xa3, 0xb3, 0x25
};

static void _print_data(const char *name, uint8_t *data, size_t size)
{
     size_t i;

     if (data == NULL || size == 0) {
          printf("print_data: no data\n");
          return;
     }

    printf("%s size: %d\n", name, size);
    for (i = 0; i < size - size % 8; i += 8) {
        printf("%s data: %02x%02x %02x%02x %02x%02x %02x%02x\n",
                   name,
                   data[i+0], data[i+1], data[i+2], data[i+3],
                   data[i+4], data[i+5], data[i+6], data[i+7]);
    }
    while(i < size) {
        printf("%s data: %02x\n", name, data[i]);
        i++;
    }

    return;
}

static void _print_rsa_key(rsa_key *key)
{
    uint32_t len;
    uint8_t tmp[256];

    printf("RSA %s key:\n", (key->type == PK_PUBLIC)? "public" : "private");
    if (key->type == PK_PUBLIC) {
        len = mp_unsigned_bin_size(key->N);
        mp_to_unsigned_bin(key->N, tmp);
        _print_data("RSA N", tmp, len);

        len = mp_unsigned_bin_size(key->e);
        mp_to_unsigned_bin(key->e, tmp);
        _print_data("RSA e", tmp, len);
    } else {
        len = mp_unsigned_bin_size(key->N);
        mp_to_unsigned_bin(key->N, tmp);
        _print_data("RSA N", tmp, len);

        len = mp_unsigned_bin_size(key->e);
        mp_to_unsigned_bin(key->e, tmp);
        _print_data("RSA e", tmp, len);

        len = mp_unsigned_bin_size(key->d);
        mp_to_unsigned_bin(key->d, tmp);
        _print_data("RSA d", tmp, len);
    }

    return;
}

static int _rsa_test_gen_key(rsa_key *key)
{
    int ret;

    ret = rsa_make_key(&_g_prng_state, _g_prng_idx, 1024/8, 65537, key);
    if (ret != CRYPT_OK) {
        printf("rsa make key fail(%d)\n", ret);
        return -1;
    }

    return 0;
}

static int _rsa_test_init_key(rsa_key *key)
{
    int ret;
    int type;

    type = key->type;
    memset(key, 0, sizeof(rsa_key));
    key->type = type;

    ret = mp_init_multi(&key->N, &key->e, &key->d, NULL);
    if (ret < 0) {
        printf("init_key: mp init multi fail(%d)\n", ret);
        return -1;
    }

    if (key->type == PK_PUBLIC) {
        mp_read_unsigned_bin(key->N, RSA_1024_N, 1024/8);
        mp_read_unsigned_bin(key->e, RSA_1024_E, 3);
    } else {
        mp_read_unsigned_bin(key->N, RSA_1024_N, 1024/8);
        mp_read_unsigned_bin(key->e, RSA_1024_E, 3);
        mp_read_unsigned_bin(key->d, RSA_1024_D, 1024/8);
    }

    return 0;
}

static int _rsa_test_encrypt_decrypt_nopad(void)
{
    int ret = 0;
    rsa_key key;
    uint8_t src_data[128];
    uint8_t plaintext[128];
    uint8_t ciphertext[128];
    ulong_t src_size = 128;
    ulong_t dst_size = 128;

    ret = _rsa_test_gen_key(&key);
    if (ret < 0) {
        printf("rsa gen key fail\n");
        return -1;
    }

    /* public encrypt */
    memset(src_data, 0xa, src_size);
    ret = rsa_exptmod(src_data, src_size,
                  ciphertext, &dst_size, PK_PUBLIC, &key);
    if (ret != CRYPT_OK) {
        printf("public encrypt fail(%d)\n", ret);
        goto _out;
    }

    /* private decrypt */
    dst_size = 128;
    ret = rsa_exptmod(ciphertext, src_size,
                  plaintext, &dst_size, PK_PRIVATE, &key);
    if (ret != CRYPT_OK) {
        printf("private decrypt fail(%d)\n", ret);
        goto _out;
    }

    if (memcmp(src_data, plaintext, src_size)) {
        printf("RSA encrypt and decrypt with no-padding fail!\n");
        _print_data("plaintext", plaintext, dst_size);
        _print_data("ciphertext", ciphertext, dst_size);
    } else {
        printf("RSA encrypt and decrypt with no-padding success!\n");
    }

_out:
    rsa_free(&key);

    return ret;
}

static int _rsa_test_encrypt_decrypt_v1_5(void)
{
    int ret, stat;
    rsa_key key;
    uint8_t src_data[128];
    uint8_t plaintext[128];
    uint8_t ciphertext[128];
    ulong_t src_size = 117;
    ulong_t dst_size = 128;

    ret = _rsa_test_gen_key(&key);
    if (ret < 0) {
        printf("rsa gen key fail\n");
        return -1;
    }

    /* public encrypt */
    memset(src_data, 0xa, src_size);
    ret = rsa_encrypt_key_ex(src_data, src_size, ciphertext, &dst_size,
              NULL, 0, &_g_prng_state, _g_prng_idx, 0, LTC_PKCS_1_V1_5, &key);
    if (ret != CRYPT_OK) {
        printf("public encrypt with pkcs1_v1_5 fail(%d)\n", ret);
        goto _out;
    }

    /* private decrypt */
    dst_size = 128;
    ret = rsa_decrypt_key_ex(ciphertext, 128, plaintext, &dst_size,
                             NULL, 0, 0, LTC_PKCS_1_V1_5, &stat, &key);
    if (ret != CRYPT_OK || stat != 1) {
        printf("private decrypt with pkcs1_v1_5 fail(%d)\n", ret);
        goto _out;
    }

    if (memcmp(src_data, plaintext, src_size)) {
        printf("RSA encrypt and decrypt with pkcs1_v1_5 fail!\n");
        _print_data("plaintext", plaintext, src_size);
        _print_data("ciphertext", ciphertext, dst_size);
    } else {
        printf("RSA encrypt and decrypt with pkcs1_v_5 success!\n");
    }

_out:
    rsa_free(&key);

    return ret;
}

static int _rsa_test_encrypt_decrypt_oaep(void)
{
    int ret, stat;
    rsa_key key;
    int hash_idx;
    uint8_t src_data[128];
    uint8_t plaintext[128];
    uint8_t ciphertext[128];
    ulong_t src_size = 86;
    ulong_t dst_size = 128;
    uint8_t lparam[] = {0x01, 0x02, 0x03, 0x04};

    key.type = PK_PRIVATE;
    ret = _rsa_test_init_key(&key);
    if (ret < 0) {
        printf("rsa init key fail\n");
        return -1;
    }

    hash_idx = find_hash("sha1");
    if (hash_idx < 0) {
        printf("not find sha1\n");
        return -1;
    }

    /* public encrypt without lparam */
    memset(src_data, 0xa, src_size);
    ret = rsa_encrypt_key_ex(src_data, src_size, ciphertext, &dst_size,
              NULL, 0, &_g_prng_state, _g_prng_idx, hash_idx, LTC_PKCS_1_OAEP, &key);
    if (ret != CRYPT_OK) {
        printf("public encrypt with oaep(without lparam) fail(%d)\n", ret);
        goto _out;
    }

    /* private decrypt without lparam*/
    ret = rsa_decrypt_key_ex(ciphertext, 128, plaintext, &dst_size,
                             NULL, 0, hash_idx, LTC_PKCS_1_OAEP, &stat, &key);
    if (ret != CRYPT_OK || stat != 1) {
        printf("private decrypt with oaep(without lparam) fail(%d)\n", ret);
        goto _out;
    }

    if (memcmp(src_data, plaintext, src_size)) {
        printf("RSA encrypt and decrypt with pkcs1_oaep(without lparam) fail!\n");
        _print_data("plaintext", plaintext, src_size);
        _print_data("ciphertext", ciphertext, dst_size);
    } else {
        printf("RSA encrypt and decrypt with pkcs1_oaep(without lparam) success!\n");
    }

    /* public encrypt with lparam */
    dst_size = 128;
    ret = rsa_encrypt_key_ex(src_data, src_size, ciphertext, &dst_size,
              lparam, sizeof(lparam), &_g_prng_state, _g_prng_idx, hash_idx, LTC_PKCS_1_OAEP, &key);
    if (ret != CRYPT_OK) {
        printf("public encrypt with oaep(with lparam) fail(%d)\n", ret);
        goto _out;
    }

    /* private decrypt with lparam*/
    ret = rsa_decrypt_key_ex(ciphertext, 128, plaintext, &dst_size,
                             lparam, sizeof(lparam), hash_idx, LTC_PKCS_1_OAEP, &stat, &key);
    if (ret != CRYPT_OK || stat != 1) {
        printf("private decrypt with oaep(with lparam) fail(%d)\n", ret);
        goto _out;
    }

    if (memcmp(src_data, plaintext, src_size)) {
        printf("RSA encrypt and decrypt with pkcs1_oaep(with lparam) fail!\n");
        _print_data("plaintext", plaintext, src_size);
        _print_data("ciphertext", ciphertext, dst_size);
    } else {
        printf("RSA encrypt and decrypt with pkcs1_oaep(with lparam) success!\n");
    }

_out:
    rsa_free(&key);

    return ret;
}

static int _rsa_test_sign_verify(void)
{
    int ret;
    int stat;
    rsa_key key;
    int hash_idx;
    ulong_t src_size;
    ulong_t sig_size = 128;
    uint8_t src_data[64];
    uint8_t signature[128];

    ret = _rsa_test_gen_key(&key);
    if (ret < 0) {
        printf("rsa gen key fail\n");
        return -1;
    }

    hash_idx = find_hash("sha1");
    if (hash_idx < 0) {
        printf("not find hash\n");
        return -1;
    }
    src_size = 20;

    /* private sign with v1_5 padding */
    memset(src_data, 0xa, 64);
    ret = rsa_sign_hash_ex(src_data, src_size, signature, &sig_size,
                           LTC_PKCS_1_V1_5, &_g_prng_state, _g_prng_idx,
                           hash_idx, 0, &key);
    if (ret != CRYPT_OK) {
        printf("rsa private sign(v1_5) fail(%d)\n", ret);
        goto _out;
    }

    /* public verify with v1_5 padding */
    key.type = PK_PUBLIC;
    ret = rsa_verify_hash_ex(signature, sig_size, src_data, src_size,
                             LTC_PKCS_1_V1_5, hash_idx, 0, &stat, &key);
    if (ret != CRYPT_OK || stat != 1) {
        printf("RSA public verify(v1_5) fail(ret; %d stat: %d)\n", ret , stat);
        _print_data("v1_5_sig", signature, sig_size);
        ret = -1;
        goto _out;
    } else {
        printf("rsa public verify(v1_5) success!\n");
    }

    /* private sign with pss padding */
    key.type = PK_PRIVATE;
    ret = rsa_sign_hash_ex(src_data, src_size, signature, &sig_size,
                           LTC_PKCS_1_PSS, &_g_prng_state, _g_prng_idx,
                           hash_idx, 8, &key);
    if (ret != CRYPT_OK) {
        printf("RSA private sign(pss) fail(%d)\n", ret);
        goto _out;
    }

    /* public verify with pss padding */
    key.type = PK_PUBLIC;
    ret = rsa_verify_hash_ex(signature, sig_size, src_data, src_size,
                             LTC_PKCS_1_PSS, hash_idx, 8, &stat, &key);
    if (ret != CRYPT_OK || stat != 1) {
        printf("RSA public verify(pss) fail(ret; %d stat: %d)\n", ret , stat);
        _print_data("pss_sig", signature, sig_size);
        ret = -1;
        goto _out;
    } else {
        printf("RSA public verify(pss) success!\n");
    }

_out:
    rsa_free(&key);

    return ret;
}

static void _mbed_crypto_rsa_test(uint32_t level)
{
    int ret;
    rsa_key key;

#if 0
    ret = _rsa_test_gen_key(&key);
    if (ret < 0) {
        printf("rsa gen key fail\n");
        return;
    }
    rsa_free(&key);
#endif

    key.type = PK_PRIVATE;
    ret = _rsa_test_init_key(&key);
    if (ret < 0) {
        printf("rsa gen key fail\n");
        return;
    }
    rsa_free(&key);

    ret = _rsa_test_encrypt_decrypt_nopad();
    if (ret < 0) {
        printf("rsa encrypt decrypt with nopad test fail\n");
        return;
    }

    ret = _rsa_test_encrypt_decrypt_v1_5();
    if (ret < 0) {
        printf("rsa encrypt decrypt with pkcs1_v_5 test fail\n");
        return;
    }

    ret = _rsa_test_encrypt_decrypt_oaep();
    if (ret < 0) {
        printf("rsa encrypt decrypt with pkcs1_oaep test fail\n");
        return;
    }

    ret = _rsa_test_sign_verify();
    if (ret < 0) {
        printf("rsa sign verify test fail\n");
        return;
   }

    return;
}
