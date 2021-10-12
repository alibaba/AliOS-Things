/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

//#include <aos/aos.h>

#include <misc/printk.h>
#include <misc/byteorder.h>
#include <tinycrypt/sha256.h>
#include <tinycrypt/constants.h>
#include <port/mesh_hal_sec.h>

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_MESH_DEBUG_GENIE_CRYPTO)
#include "common/log.h"

#include "genie_service.h"

static uint8_t g_auth[32];
static uint8_t g_ble_key[32];

uint8_t *genie_crypto_get_auth(const uint8_t random[16])
{
    int ret;
    genie_triple_t *p_genie_triple = genie_triple_get();
    char mac_str[(GENIE_TRIPLE_MAC_SIZE << 1) + 1] = "";
    char key_str[(GENIE_TRIPLE_KEY_SIZE << 1) + 1] = "";
    char static_str[88] = ""; // pid + ',' + mac + ',' + secret = 8+1+12+1+32+1+32'\0'
    char rad_str[33] = "";
    struct tc_sha256_state_struct sha256_ctx;

    hextostring(p_genie_triple->mac, mac_str, GENIE_TRIPLE_MAC_SIZE);
    hextostring(p_genie_triple->key, key_str, GENIE_TRIPLE_KEY_SIZE);
    hextostring(random, rad_str, 16);

#ifdef GENIE_OLD_AUTH
    sprintf(static_str, "%08x,%s,%s", p_genie_triple->pid, mac_str, key_str);
#else
    sprintf(static_str, "%08x,%s,%s,%s", p_genie_triple->pid, mac_str, key_str, rad_str);
#endif
    BT_DBG("static oob: %s", static_str);

    /* calculate the sha256 of oob info and
     * fetch the top 16 bytes as static value
     */
    ret = tc_sha256_init(&sha256_ctx);
    if (ret != TC_CRYPTO_SUCCESS)
    {
        BT_ERR("sha256 init fail\n");
    }

    ret = tc_sha256_update(&sha256_ctx, (const uint8_t *)static_str, strlen(static_str));
    if (ret != TC_CRYPTO_SUCCESS)
    {
        BT_ERR("sha256 udpate fail\n");
    }

    ret = tc_sha256_final(g_auth, &sha256_ctx);
    if (ret != TC_CRYPTO_SUCCESS)
    {
        BT_ERR("sha256 final fail\n");
    }
    else
    {
        BT_DBG("auth: %s", bt_hex((char *)g_auth, 16));
    }
    return g_auth;
}

int genie_crypto_encrypt(const uint8_t data_in[16], uint8_t data_out[16])
{
    uint8_t i = 0;
    uint8_t local_data[16];
    uint8_t aes_iv[16] = {0x31, 0x32, 0x33, 0x61, 0x71, 0x77, 0x65, 0x64,
                          0x23, 0x2a, 0x24, 0x21, 0x28, 0x34, 0x6a, 0x75};

    memcpy(local_data, data_in, 16);
    while (i < 16)
    {
        local_data[i] ^= aes_iv[i];
        i++;
    }

    return bt_mesh_aes_encrypt(g_ble_key, local_data, data_out);
}

int genie_crypto_decrypt(const uint8_t data_in[16], uint8_t data_out[16])
{
    int ret = bt_mesh_aes_decrypt(g_ble_key, data_in, data_out);
    uint8_t aes_iv[16] = {0x31, 0x32, 0x33, 0x61, 0x71, 0x77, 0x65, 0x64,
                          0x23, 0x2a, 0x24, 0x21, 0x28, 0x34, 0x6a, 0x75};
    uint8_t i = 0;

    while (i < 16)
    {
        data_out[i] ^= aes_iv[i];
        i++;
    }

    return ret;
}

void genie_ais_get_cipher(const uint8_t random[16], uint8_t *cipher)
{
    int ret;
    genie_triple_t *p_genie_triple = genie_triple_get();
    char mac_str[(GENIE_TRIPLE_MAC_SIZE << 1) + 1] = "";
    char key_str[(GENIE_TRIPLE_KEY_SIZE << 1) + 1] = "";
    char static_str[72] = ""; // random + ',' + pid + ',' + mac + ',' + secret = 16+1+8+1+12+1+32'\0'
    struct tc_sha256_state_struct sha256_ctx;

    hextostring(p_genie_triple->mac, mac_str, GENIE_TRIPLE_MAC_SIZE);
    hextostring(p_genie_triple->key, key_str, GENIE_TRIPLE_KEY_SIZE);

    memcpy(static_str, random, 16);
    sprintf(static_str + 16, ",%08x,%s,%s", p_genie_triple->pid, mac_str, key_str);

    BT_DBG("string: %s", static_str);

    /* calculate the sha256 of oob info and
     * fetch the top 16 bytes as static value
     */
    ret = tc_sha256_init(&sha256_ctx);
    if (ret != TC_CRYPTO_SUCCESS)
    {
        BT_ERR("sha256 init fail\n");
    }

    ret = tc_sha256_update(&sha256_ctx, (const uint8_t *)static_str, strlen(static_str));
    if (ret != TC_CRYPTO_SUCCESS)
    {
        BT_ERR("sha256 udpate fail\n");
    }

    ret = tc_sha256_final(g_ble_key, &sha256_ctx);
    if (ret != TC_CRYPTO_SUCCESS)
    {
        BT_ERR("sha256 final fail\n");
    }
    else
    {
        BT_DBG("auth: %s", bt_hex((char *)g_ble_key, 16));
    }
    genie_crypto_encrypt(random, cipher);

    BT_DBG("cipher: %s", bt_hex((char *)cipher, 16));
}

void genie_ais_reset(void)
{
    memset(g_ble_key, 0, 32);
}

void genie_crypto_adv_create(uint8_t ad_structure[14], uint8_t is_silent)
{
    genie_triple_t *p_genie_triple = genie_triple_get();

    ad_structure[3] |= 0x08; //FMSK auth enable
    if (is_silent)
    {
        ad_structure[3] |= 0x20;
    }
    else
    {
        ad_structure[3] &= ~0x20;
    }

    memcpy(ad_structure + GENIE_TRIPLE_PID_SIZE, &p_genie_triple->pid, GENIE_TRIPLE_PID_SIZE);
    // mac addr
    for (int i = 0; i < GENIE_TRIPLE_MAC_SIZE; i++)
    {
        ad_structure[8 + i] = p_genie_triple->mac[GENIE_TRIPLE_MAC_SIZE - 1 - i];
    }
}
