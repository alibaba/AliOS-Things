/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 */

#include "tee_id2.h"
#include "tee_rsa.h"
#include "tee_dbg.h"
#include "tee_sym.h"

#define ID2_TEST_DEBUG 1

#define ID_LEN                 (24)
#define SIGN_LEN               (128)

#define SYM_TEST_DATA_LEN   48

uint32_t short_test_count = 5;
uint32_t short_test_len[] = {13, 17, 35, 16, 32};

static uint8_t aes_test_data[SYM_TEST_DATA_LEN] = {
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
    0x38, 0x39, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
    0x38, 0x39, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
    0x38, 0x39, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66
};

void id2_test_dump_data(char *name, uint8_t *data, uint32_t len)
{
    uint32_t i;
    printk("name is %s, len is %d\n", name, len);

    for (i = 0; i < (len - len % 8); i += 8) {
        printk("%s:  %02x %02x %02x %02x %02x %02x %02x %02x\n",
                          name, data[i+0], data[i+1], data[i+2], data[i+3],
                          data[i+4], data[i+5], data[i+6], data[i+7]);
    }

    printk("%s:: ", name);
    while(i < len) {
        printk("%02x, ", data[i++]);
    }
    printk("\n");

    return;
}

int tee_id2_test(void)
{
    int ret;
    uint8_t ID = KEY_ID_0;
    uint8_t type = ID2_MD5;
    uint8_t padding = PKCS1_PADDING;
    uint8_t id_value[ID_LEN];
    uint8_t data[128];
    uint32_t len = 64;
    uint8_t e_data[128];
    uint32_t e_len = SIGN_LEN;
    uint8_t d_data[128];
    uint32_t d_len = SIGN_LEN;
    uint8_t s_data[SIGN_LEN];
    uint32_t s_len = SIGN_LEN;

    ret = tee_get_ID2(id_value, NULL);
    if (ret < 0) {
        tee_dbg_print(ERR, "fail to get ID2!\n");
        return ret;
    }

    memset(data, 0xa, 128);
    ret = tee_RSA_sign(ID, data, len, s_data, &s_len, type);
    if (ret < 0) {
        tee_dbg_print(ERR, "fail to sign data!\n");
        return ret;
    }

    ret = tee_RSA_verify(ID, data, len, s_data, s_len, type);
    if (ret < 0) {
        tee_dbg_print(ERR, "fail to verify data!\n");
        return ret;
    }

    ret = tee_RSA_public_encrypt(ID,
                  data, len, e_data, &e_len, padding);
    if (ret < 0) {
        tee_dbg_print(ERR, "fail to encrypt data!\n");
        return ret;
    }
    tee_dbg_print(INF, "rsa encrypted data len: %d\n", e_len);

    ret = tee_RSA_private_decrypt(ID,
                  e_data, e_len, d_data, &d_len, padding);
    if (ret < 0) {
        tee_dbg_print(ERR, "fail to decrypt data!\n");
        return ret;
    }
    tee_dbg_print(INF, "rsa decrypted data len: %d\n", d_len);

    if (len != d_len) {
        tee_dbg_print(ERR, "rsa encrypt and decrypt data fail!!\n");
        return -1;
    }
    if (memcmp(data, d_data, len)) {
        tee_dbg_print(ERR, "rsa encrypt and decrypt data fail!!\n");
        return -1;
    } else {
        tee_dbg_print(INF, "rsa encrypt and decrypt data success!\n");
        return 0;
    }

    return 0;
}

int tee_id2_test2(void)
{
    int ret;
    uint8_t tst_data[] = { 0x59, 0x30, 0x30, 0x46, 0x33, 0x30, 0x30,
                           0x30, 0x31, 0x36, 0x32, 0x34, 0x37, 0x42,
                           0x31, 0x37, 0x43, 0x7E, 0x37, 0x31, 0x42, 0x34};
    uint8_t s_data[SIGN_LEN];
    uint32_t s_len = SIGN_LEN;
    int32_t i;

    ret = tee_RSA_sign(KEY_ID_0, tst_data, 22, s_data, &s_len, ID2_MD5);
    if (ret < 0) {
        tee_dbg_print(ERR, "fail to sign data!\n");
        return ret;
    }

    for (i = 0; i < s_len; i++) {
        tee_dbg_print(ERR, "0x%x ", s_data[i]);
        if ((i & 0x7) == 0x7) {
            tee_dbg_print(ERR, "\n");
        }
    }
    return 0;
}

int test_get_id()
{
    int ret;
    uint8_t id_value[ID_LEN + 1];
    uint32_t id2_len = ID_LEN;

    ret = tee_get_ID2(id_value, &id2_len);
    if (ret < 0) {
        tee_dbg_print(ERR, "fail to get ID!\n");
        return ret;
    }
    id_value[ID_LEN] = '\0';
    tee_dbg_print(ERR, "id2_len is %d, ID2 ID: %s\n", id2_len, id_value);

    return ret;
}

int test_cipher(uint8_t cipher_type, uint8_t block_mode, uint8_t padding_type)
{
    uint32_t test_len = 0;
    int ret = 0;
    uint8_t ID = KEY_ID_33;
    cipher_param_t cipher_param;
    uint8_t in[128];
    uint32_t in_len = 0;
    uint8_t *enc = NULL;
    uint32_t enc_len = 0;
    uint8_t *dec = NULL;
    uint32_t dec_len = 0;
    uint8_t *iv = NULL;
    uint8_t iv_len = 0;
    uint8_t test_iv[16] = {
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66,
    };

    if (padding_type == ID2_PKCS5) {
        test_len = SYM_TEST_DATA_LEN - 3;
    } else {
        test_len = SYM_TEST_DATA_LEN;
    }

    if (block_mode == ID2_ECB) {
        iv = NULL;
    } else if (block_mode == ID2_CBC || block_mode == ID2_CTR) {
        iv = test_iv;
        iv_len = 16;
    }

    in_len = test_len;

    cipher_param.cipher_type = cipher_type;
    cipher_param.block_mode = block_mode;
    cipher_param.padding_type = padding_type;
    cipher_param.is_enc = ID2_ENCRYPT;

    memcpy(in, aes_test_data, test_len);

    ret = tee_sym_cipher(ID, &cipher_param, iv, iv_len,
                     in, in_len, enc, &enc_len);
    if (!ret) {
        tee_dbg_print(ERR, "fail to get cipher enc len\n");
        ret = -1;
        return ret;
    }

    enc = malloc(enc_len);
    if (!enc) {
        tee_dbg_print(ERR, "malloc enc buffer failed\n");
        return -1;
    }

    ret = tee_sym_cipher(ID, &cipher_param, iv, iv_len,
                       in, in_len, enc, &enc_len);
    if (ret) {
        tee_dbg_print(ERR, "fail to cipher enc\n");
        ret = -1;
        goto clean1;
    }

#if ID2_TEST_DEBUG
    id2_test_dump_data("enc is:", enc, enc_len);
#endif

    //test decrypt
    cipher_param.is_enc = ID2_DECRYPT;
    ret = tee_sym_cipher(ID, &cipher_param, iv, iv_len,
                        enc, enc_len, dec, &dec_len);
    if (!ret) {
        tee_dbg_print(ERR, "fail to get cipher dec len %d\n", ret);
        ret = -1;
        goto clean1;
    }

    dec = malloc(dec_len);
    if (!dec) {
        tee_dbg_print(ERR, "malloc enc buffer failed\n");
        ret = -1;
        goto clean1;
    }

    ret = tee_sym_cipher(ID, &cipher_param, iv, iv_len,
                          enc, enc_len, dec, &dec_len);
    if (ret) {
        tee_dbg_print(ERR, "fail to cipher dec\n");
        ret = -1;
        goto clean2;
    }

#if ID2_TEST_DEBUG
    id2_test_dump_data("dec is:", dec, dec_len);
#endif

    if (dec_len != in_len) {
        tee_dbg_print(ERR, "decrypt len is not correct\n");
        ret = -1;
        goto clean2;
    }

    if (memcmp(in, dec, in_len)) {
        tee_dbg_print(ERR, "decrypt result is not correct\n");
        ret = -1;
    } else {
        tee_dbg_print(ERR, "sym cipher test success\n");
    }

clean2:
    if (dec) {
        free(dec);
        dec = NULL;
    }

clean1:
    if (enc) {
        free(enc);
        enc = NULL;
    }

    return ret;
}


int test_sym_cipher()
{
    int ret = 0;

    ret = test_get_id();
    if (ret) {
        tee_dbg_print(ERR, "get id failed\n");
        return -1;
    }

    ret = test_cipher(ID2_AES, ID2_ECB, ID2_PKCS5);
    if (ret) {
        tee_dbg_print(ERR, "test aes ecb pkcs5 failed\n");
        return ret;
    }

    ret = test_cipher(ID2_AES, ID2_CBC, ID2_PKCS5);
    if (ret) {
        tee_dbg_print(ERR, "test aes ecb pkcs5 failed\n");
        return ret;
    }

    ret = test_cipher(ID2_AES, ID2_ECB, No_PADDING);
    if (ret) {
        tee_dbg_print(ERR, "test aes ecb no padding failed\n");
        return ret;
    }

    ret = test_cipher(ID2_AES, ID2_CBC, No_PADDING);
    if (ret) {
        tee_dbg_print(ERR, "test aes ecb no padding failed\n");
        return ret;
    }

#if 0
    ret = test_cipher(ID2_AES, ID2_CTR, No_PADDING);
    if (ret) {
        tee_dbg_print(ERR, "test aes ecb no padding failed\n");
        return ret;
    }
#endif

    return ret;
}

int test_cipher_short_buffer(uint8_t cipher_type,
               uint8_t block_mode, uint32_t index)
{
    uint32_t test_len = short_test_len[index];
    int ret = 0;
    uint8_t ID = KEY_ID_33;
    cipher_param_t cipher_param;
    uint8_t in[128];
    uint32_t in_len = test_len;
    uint8_t *enc = NULL;
    uint32_t enc_len = 0;
    uint8_t *dec = NULL;
    uint32_t dec_len = 0;
    uint8_t tmp_buf[SYM_TEST_DATA_LEN];
    uint8_t *iv = NULL;
    uint32_t iv_len = 0;
    uint8_t test_iv[16] = {
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66,
    };


    cipher_param.cipher_type = cipher_type;
    cipher_param.block_mode = block_mode;
    cipher_param.padding_type = ID2_PKCS5;
    cipher_param.is_enc = ID2_ENCRYPT;

    if (block_mode == ID2_ECB) {
        iv = NULL;
    } else if (block_mode == ID2_CBC || block_mode == ID2_CTR) {
        iv = test_iv;
        iv_len = 16;
    }

    memcpy(in, aes_test_data, test_len);

    enc_len = in_len;
    ret = tee_sym_cipher(ID, &cipher_param, iv, iv_len,
                        in, in_len, tmp_buf, &enc_len);
    if (!ret) {
        tee_dbg_print(ERR, "fail to get cipher enc len\n");
        ret = -1;
        return ret;
    }
    tee_dbg_print(ERR, "ID2_SHORT_BUFFER: get enc len is %d\n", enc_len);
    enc = malloc(enc_len);
    if (!enc) {
        tee_dbg_print(ERR, "malloc enc buffer failed\n");
        return -1;
    }

    ret = tee_sym_cipher(ID, &cipher_param, iv, iv_len,
                           in, in_len, enc, &enc_len);
    if (ret) {
        tee_dbg_print(ERR, "fail to cipher enc\n");
        ret = -1;
        goto clean1;
    }

#if ID2_TEST_DEBUG
    id2_test_dump_data("enc is:", enc, enc_len);
#endif

    //test decrypt
    cipher_param.is_enc = ID2_DECRYPT;
    dec_len = in_len - 1;
    ret = tee_sym_cipher(ID, &cipher_param, iv, iv_len,
                 enc, enc_len, tmp_buf, &dec_len);
    if (!ret) {
        tee_dbg_print(ERR, "fail to get cipher dec len %d\n", ret);
        ret = -1;
        goto clean1;
    }
    tee_dbg_print(ERR, "ID2_SHORT_BUFFER: dec len is %d\n", dec_len);

    dec = malloc(dec_len);
    if (!dec) {
        tee_dbg_print(ERR, "malloc enc buffer failed\n");
        ret = -1;
        goto clean1;
    }

    ret = tee_sym_cipher(ID, &cipher_param, iv, iv_len,
                           enc, enc_len, dec, &dec_len);
    if (ret) {
        tee_dbg_print(ERR, "fail to cipher dec\n");
        ret = -1;
        goto clean2;
    }

#if ID2_TEST_DEBUG
    id2_test_dump_data("dec is:", dec, dec_len);
#endif

    if (dec_len != in_len) {
        tee_dbg_print(ERR, "decrypt len is not correct\n");
        ret = -1;
        goto clean2;
    }

    if (memcmp(in, dec, in_len)) {
        tee_dbg_print(ERR, "decrypt result is not correct\n");
        ret = -1;
    } else {
        tee_dbg_print(ERR, "sym cipher test success\n");
    }

clean2:
    if (dec) {
        free(dec);
        dec = NULL;
    }

clean1:
    if (enc) {
        free(enc);
        enc = NULL;
    }

    return ret;
}

int test_short_sym_cipher()
{
    int ret = 0;
    int i = 0;

    ret = test_get_id();
    if (ret) {
        tee_dbg_print(ERR, "get id failed\n");
        return -1;
    }

    for (i = 0; i < short_test_count; i++) {
        ret = test_cipher_short_buffer(ID2_AES, ID2_ECB, i);
        if (ret) {
            tee_dbg_print(ERR, "test aes ecb pkcs5 failed\n");
            return ret;
        }

        ret = test_cipher_short_buffer(ID2_AES, ID2_CBC, i);
        if (ret) {
            tee_dbg_print(ERR, "test aes ecb pkcs5 failed\n");
            return ret;
        }
    }

    return ret;
}

int tee_id2_cipher_test()
{
    int ret = 0;

    ret = test_sym_cipher();
    if (ret) {
        tee_dbg_print(ERR, "test sym cipher success\n");
        return -1;
    }
    tee_dbg_print(ERR, "\n<<<<<<< test sym cipher success>>>>>>>>\n");

    ret = test_short_sym_cipher();
    if (ret) {
        tee_dbg_print(ERR, "test short sym cipher success\n");
        return -1;
    }
    tee_dbg_print(ERR, "\n<<<<<<< test short sym cipher success>>>>>>>>\n");

    return 0;
}

