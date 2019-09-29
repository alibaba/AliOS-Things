/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

#include "aos/kernel.h"
#include "ulog/ulog.h"

#include "id2_test.h"

/* Hex String, getting from id2 console */
#define ID2_CIPHER_DATA     ""

int application_start(int argc, char *argv[])
{
    int ret;
    uint32_t cipher_len = 0;
    char *cipher_data = ID2_CIPHER_DATA;

    LOG("id2 app begin\n");

    ret = id2_client_unit_test();
    if (ret < 0) {
        LOG("id2 client unit test fail!!\n");
        return -1;
    }

    cipher_len = strlen(cipher_data);

    if (cipher_len < ID2_ID_LEN) {
        ret = id2_client_generate_authcode();
        if (ret < 0) {
            LOG("id2 client generate authcode fail!!\n");
            return -1;
        }
    } else {
        ret = id2_client_decrypt_data(cipher_data, cipher_len);
        if (ret < 0) {
            LOG("id2 client decrypt data fail!!\n");
            return -1;
        }
    }

    LOG("id2 app end\n");

    return 0;
}

