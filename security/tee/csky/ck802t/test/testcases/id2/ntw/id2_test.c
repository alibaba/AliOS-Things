/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 */

#include "tee_id2.h"
#include "tee_rsa.h"
#include "tee_dbg.h"

#define ID_LEN                 (17)
#define SIGN_LEN               (128)

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
