/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <errno.h>
#include <stddef.h>
#include <port/mesh_hal_sec.h>

#ifndef CONFIG_MESH_STACK_ALONE
#include <tinycrypt/constants.h>
#include <tinycrypt/utils.h>
#include <tinycrypt/aes.h>
#include <tinycrypt/cmac_mode.h>
#include <tinycrypt/ccm_mode.h>

int bt_mesh_rand(void *buf, size_t len)
{
    return bt_rand(buf, len);
}

int bt_mesh_aes_encrypt(const uint8_t key[16], const uint8_t plaintext[16],
                        uint8_t enc_data[16])
{
    return bt_encrypt_be(key, plaintext, enc_data);
}

int bt_mesh_aes_cmac(const uint8_t key[16], struct bt_mesh_sg *sg,
                     size_t sg_len, uint8_t mac[16])
{
#ifndef CONFIG_MESH_STACK_ALONE
    struct tc_aes_key_sched_struct sched;
    struct tc_cmac_struct state;

    if (tc_cmac_setup(&state, key, &sched) == TC_CRYPTO_FAIL) {
        return -EIO;
    }

    for (; sg_len; sg_len--, sg++) {
        if (tc_cmac_update(&state, sg->data,
                           sg->len) == TC_CRYPTO_FAIL) {
            return -EIO;
        }
    }

    if (tc_cmac_final(mac, &state) == TC_CRYPTO_FAIL) {
        return -EIO;
    }
#endif

    return 0;
}

const uint8_t *bt_mesh_pub_key_get(void)
{
    return bt_pub_key_get();
}

int bt_mesh_dh_key_gen(const uint8_t remote_pk[64], bt_mesh_dh_key_cb_t cb)
{
    return bt_dh_key_gen(remote_pk, cb);
}

int bt_mesh_pub_key_gen(struct bt_mesh_pub_key_cb *cb)
{
    return bt_pub_key_gen(cb);
}
#else
int bt_mesh_rand(void *buf, size_t len)
{
    return 0;
}

int bt_mesh_aes_encrypt(const uint8_t key[16], const uint8_t plaintext[16],
                        uint8_t enc_data[16])
{
    return 0;
}

int bt_mesh_aes_cmac(const uint8_t key[16], struct bt_mesh_sg *sg,
                     size_t sg_len, uint8_t mac[16])
{
    return 0;
}

const uint8_t *bt_mesh_pub_key_get(void)
{
    return 0;
}

int bt_mesh_dh_key_gen(const uint8_t remote_pk[64], bt_mesh_dh_key_cb_t cb)
{
    return 0;
}

int bt_mesh_pub_key_gen(struct bt_mesh_pub_key_cb *cb)
{
    return 0;
}
#endif
