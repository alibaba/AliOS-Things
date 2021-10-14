/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <bt_errno.h>
#include <stddef.h>
#include <bluetooth/bluetooth.h>
#include <port/mesh_hal_sec.h>

#ifndef CONFIG_MESH_STACK_ALONE
#include <crypto.h>
#include <tinycrypt/constants.h>
#include <tinycrypt/utils.h>
#include <tinycrypt/aes.h>
#include <tinycrypt/cmac_mode.h>
#include <tinycrypt/ccm_mode.h>

int bt_mesh_rand(void *buf, size_t len)
{
    return bt_rand(buf, len);
}

int bt_mesh_aes_encrypt(const uint8_t key[16], const uint8_t plaintext[16], uint8_t enc_data[16])
{
    return bt_encrypt_be(key, plaintext, enc_data);
}

int bt_mesh_aes_decrypt(const uint8_t key[16], const uint8_t enc_data[16], uint8_t dec_data[16])
{
    return bt_decrypt_be(key, enc_data, dec_data);
}

#endif
