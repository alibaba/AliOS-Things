/*
 * Copyright (c) 2017 Nordic Semiconductor ASA
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <errno.h>

#include <zephyr.h>
#include <misc/byteorder.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>

#include <tinycrypt/constants.h>
#include <tinycrypt/hmac_prng.h>
#include <tinycrypt/aes.h>
#include <tinycrypt/utils.h>

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_DEBUG_HCI_CORE)
#include "common/log.h"

#include "hci_core.h"

static struct tc_hmac_prng_struct prng;

enum {
    PRNG_DONE = 0,
    PRNG_INIT = 0x01,
    PRNG_RESEED = 0x02,
    RAND_RESEED = 0x80,
};

static uint8_t crypto_state;
static u8_t prng_seed[32];
static u8_t prng_index = 0;

static int prng_reseed(struct tc_hmac_prng_struct *h)
{
    int ret;

    ret = bt_hci_cmd_send_sync(BT_HCI_OP_LE_RAND, NULL, NULL);
    if (ret == 0) {
        crypto_state |= PRNG_RESEED;
    }
    return ret;
}

int prng_init(void)
{
    struct bt_hci_rp_le_rand *rp;
    struct net_buf *rsp;
    int ret;

    /* Check first that HCI_LE_Rand is supported */
    if (!(bt_dev.supported_commands[27] & BIT(7))) {
        return -ENOTSUP;
    }

    ret = bt_hci_cmd_send_sync(BT_HCI_OP_LE_RAND, NULL, &rsp);
    if (ret == 0) {
        crypto_state = PRNG_INIT;
    }
    return ret;
}

int prng_process(struct net_buf *buf)
{
    struct bt_hci_rp_le_rand *rp;
    int ret;
    s64_t extra;

    if (crypto_state == PRNG_INIT) {
        if (IS_ENABLED(CONFIG_BT_HOST_CRYPTO)) {
            rp = (void *)buf->data;
            ret = tc_hmac_prng_init(&prng, rp->rand, sizeof(rp->rand));
            if (ret == TC_CRYPTO_FAIL) {
                return -EIO;
            }
        }
        prng_reseed(&prng);
    } else if ((crypto_state & PRNG_RESEED) == PRNG_RESEED) {
	if (prng_index < (sizeof(prng_seed) / 8)) {
            rp = (void *)buf->data;
            memcpy(&prng_seed[prng_index * 8], rp->rand, 8);
            prng_index++;
            ret = bt_hci_cmd_send_sync(BT_HCI_OP_LE_RAND, NULL, NULL);
            return ret;
        }

        extra = k_uptime_get();
        ret = tc_hmac_prng_reseed(&prng, prng_seed, sizeof(prng_seed),
                                  (u8_t *)&extra, sizeof(extra));
        crypto_state = PRNG_DONE;
	if (ret != TC_CRYPTO_SUCCESS) {
            ret = -EIO;
        }
    }
    return 1;
}

int bt_rand(void *buf, size_t len)
{
    int ret;

    ret = tc_hmac_prng_generate(buf, len, &prng);
    if (ret == TC_CRYPTO_SUCCESS) {
        return 0;
    }
    return -EIO;
}

int bt_encrypt_le(const u8_t key[16], const u8_t plaintext[16], u8_t enc_data[16])
{
	struct tc_aes_key_sched_struct s;
	u8_t tmp[16];

	BT_DBG("key %s plaintext %s", bt_hex(key, 16), bt_hex(plaintext, 16));

	sys_memcpy_swap(tmp, key, 16);

	if (tc_aes128_set_encrypt_key(&s, tmp) == TC_CRYPTO_FAIL) {
		return -EINVAL;
	}

	sys_memcpy_swap(tmp, plaintext, 16);

	if (tc_aes_encrypt(enc_data, tmp, &s) == TC_CRYPTO_FAIL) {
		return -EINVAL;
	}

	sys_mem_swap(enc_data, 16);

	BT_DBG("enc_data %s", bt_hex(enc_data, 16));

	return 0;
}

int bt_encrypt_be(const u8_t key[16], const u8_t plaintext[16],
		  u8_t enc_data[16])
{
	struct tc_aes_key_sched_struct s;

	BT_DBG("key %s plaintext %s", bt_hex(key, 16), bt_hex(plaintext, 16));

	if (tc_aes128_set_encrypt_key(&s, key) == TC_CRYPTO_FAIL) {
		return -EINVAL;
	}

	if (tc_aes_encrypt(enc_data, plaintext, &s) == TC_CRYPTO_FAIL) {
		return -EINVAL;
	}

	BT_DBG("enc_data %s", bt_hex(enc_data, 16));

	return 0;
}
