/*
 * Copyright (c) 2017 Nordic Semiconductor ASA
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <errno.h>

#include <ble_os.h>
#include <misc/byteorder.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>

#include <tinycrypt/constants.h>
#include <tinycrypt/hmac_prng.h>
#include <tinycrypt/aes.h>
#include <tinycrypt/utils.h>
#include <tinycrypt/cmac_mode.h>
#include <tinycrypt/ccm_mode.h>

#define BT_DBG_ENABLED 0
#include "common/log.h"

#include "host/hci_core.h"
#include "hci_api.h"

#include "bt_crypto.h"

#define __BT_CRYPTO_WEAK__ __attribute__((weak))

static struct tc_hmac_prng_struct prng;

static int prng_reseed(struct tc_hmac_prng_struct *h)
{
    u8_t seed[32];
    s64_t extra;
    int ret, i;

	for (i = 0; i < (sizeof(seed) / 8); i++) {
#if !defined(CONFIG_BT_USE_HCI_API)
		struct bt_hci_rp_le_rand *rp;
		struct net_buf *rsp;

		ret = bt_hci_cmd_send_sync(BT_HCI_OP_LE_RAND, NULL, &rsp);
		if (ret) {
			return ret;
		}

		rp = (void *)rsp->data;
		memcpy(&seed[i * 8], rp->rand, 8);

		net_buf_unref(rsp);
#else
        hci_api_le_rand(&seed[i * 8]);
#endif
	}

    extra = k_uptime_get();

    ret = tc_hmac_prng_reseed(h, seed, sizeof(seed), (u8_t *)&extra,
                              sizeof(extra));

    if (ret == TC_CRYPTO_FAIL) {
        BT_ERR("Failed to re-seed PRNG");
        return -EIO;
    }

    return 0;
}

__BT_CRYPTO_WEAK__ int prng_init(void)
{
    u8_t rand[8];
    int ret;

#if !defined(CONFIG_BT_USE_HCI_API)
	struct bt_hci_rp_le_rand *rp;
	struct net_buf *rsp;

	ret = bt_hci_cmd_send_sync(BT_HCI_OP_LE_RAND, NULL, &rsp);
	if (ret) {
		return ret;
	}

	rp = (void *)rsp->data;
    memcpy(rand, rp, 8);
#else
    hci_api_le_rand(rand);
#endif
    ret = tc_hmac_prng_init(&prng, rand, sizeof(rand));

    if (ret == TC_CRYPTO_FAIL) {
        BT_ERR("Failed to initialize PRNG");
        return -EIO;
    }

    /* re-seed is needed after init */
    return prng_reseed(&prng);
}

__BT_CRYPTO_WEAK__ int bt_crypto_rand(void *buf, size_t len)
{
    int ret;

    ret = tc_hmac_prng_generate(buf, len, &prng);

    if (ret == TC_HMAC_PRNG_RESEED_REQ) {
        ret = prng_reseed(&prng);

        if (ret) {
            return ret;
        }

        ret = tc_hmac_prng_generate(buf, len, &prng);
    }

    if (ret == TC_CRYPTO_SUCCESS) {
        return 0;
    }

    return -EIO;
}

__BT_CRYPTO_WEAK__ int bt_crypto_encrypt_le(const u8_t key[16], const u8_t plaintext[16],
        u8_t enc_data[16])
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

__BT_CRYPTO_WEAK__ int bt_crypto_encrypt_be(const u8_t key[16], const u8_t plaintext[16],
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

__BT_CRYPTO_WEAK__ int bt_crypto_decrypt_be(const u8_t key[16], const u8_t enc_data[16],
		  u8_t dec_data[16])
{
    struct tc_aes_key_sched_struct s;

    if (tc_aes128_set_decrypt_key(&s, key) == TC_CRYPTO_FAIL) {
        return -EINVAL;
    }

    if (tc_aes_decrypt(dec_data, enc_data, &s) == TC_CRYPTO_FAIL) {
        return -EINVAL;
    }

    BT_DBG("dec_data %s", bt_hex(dec_data, 16));

    return 0;
}


/* bt_crypto_cmac_* functions is modified from tinycrypt cmac_mode.c. the aes function is replaced
   by bt_encrypt_be which call aes ctypto from controller.
*/

/*
 *  Copyright (C) 2017 by Intel Corporation, All Rights Reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *    - Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *
 *    - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 *    - Neither the name of Intel Corporation nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  gf_wrap -- In our implementation, GF(2^128) is represented as a 16 byte
 *  array with byte 0 the most significant and byte 15 the least significant.
 *  High bit carry reduction is based on the primitive polynomial
 *
 *                     X^128 + X^7 + X^2 + X + 1,
 *
 *  which leads to the reduction formula X^128 = X^7 + X^2 + X + 1. Indeed,
 *  since 0 = (X^128 + X^7 + X^2 + 1) mod (X^128 + X^7 + X^2 + X + 1) and since
 *  addition of polynomials with coefficients in Z/Z(2) is just XOR, we can
 *  add X^128 to both sides to get
 *
 *       X^128 = (X^7 + X^2 + X + 1) mod (X^128 + X^7 + X^2 + X + 1)
 *
 *  and the coefficients of the polynomial on the right hand side form the
 *  string 1000 0111 = 0x87, which is the value of gf_wrap.
 *
 *  This gets used in the following way. Doubling in GF(2^128) is just a left
 *  shift by 1 bit, except when the most significant bit is 1. In the latter
 *  case, the relation X^128 = X^7 + X^2 + X + 1 says that the high order bit
 *  that overflows beyond 128 bits can be replaced by addition of
 *  X^7 + X^2 + X + 1 <--> 0x87 to the low order 128 bits. Since addition
 *  in GF(2^128) is represented by XOR, we therefore only have to XOR 0x87
 *  into the low order byte after a left shift when the starting high order
 *  bit is 1.
 */
static const unsigned char gf_wrap = 0x87;

/*
 *  assumes: out != NULL and points to a GF(2^n) value to receive the
 *            doubled value;
 *           in != NULL and points to a 16 byte GF(2^n) value
 *            to double;
 *           the in and out buffers do not overlap.
 *  effects: doubles the GF(2^n) value pointed to by "in" and places
 *           the result in the GF(2^n) value pointed to by "out."
 */
static void gf_double(uint8_t *out, uint8_t *in)
{

    /* start with low order byte */
    uint8_t *x = in + (16 - 1);

    /* if msb == 1, we need to add the gf_wrap value, otherwise add 0 */
    uint8_t carry = (in[0] >> 7) ? gf_wrap : 0;

    out += (16 - 1);

    for (;;) {
        *out-- = (*x << 1) ^ carry;

        if (x == in) {
            break;
        }

        carry = *x-- >> 7;
    }
}

__BT_CRYPTO_WEAK__ int bt_crypto_cmac_setup(struct bt_crypto_cmac_t *ctx, const uint8_t key[16])
{
    if (NULL == ctx || NULL == key) {
        return -1;
    }

    memset(ctx, 0, sizeof(*ctx));
    memcpy(ctx->aes_key, key, 16);

    memset(ctx->iv, 0, 16);
    bt_crypto_encrypt_be(ctx->aes_key, ctx->iv, ctx->iv);

    gf_double(ctx->K1, ctx->iv);
    gf_double(ctx->K2, ctx->K1);

    memset(ctx->iv, 0, 16);
    memset(ctx->leftover, 0, 16);
    ctx->leftover_offset = 0;

    ctx->countdown = ((uint64_t)1 << 48);

    return 0;
}

__BT_CRYPTO_WEAK__ int bt_crypto_cmac_update(struct bt_crypto_cmac_t *ctx, const uint8_t *data, uint16_t data_length)
{
    unsigned int i;

    /* input sanity check: */
    if (ctx == NULL) {
        return -1;
    }

    if (data_length == 0) {
        return 0;
    }

    if (data == NULL) {
        return -1;
    }

    if (ctx->countdown == 0) {
        return -1;
    }

    ctx->countdown--;

    if (ctx->leftover_offset > 0) {
        /* last data added to s didn't end on a 16 byte boundary */
        size_t remaining_space = 16 - ctx->leftover_offset;

        if (data_length < remaining_space) {
            /* still not enough data to encrypt this time either */
            memcpy(&ctx->leftover[ctx->leftover_offset], data, data_length);
            ctx->leftover_offset += data_length;
            return 0;
        }

        /* leftover block is now full; encrypt it first */
        memcpy(&ctx->leftover[ctx->leftover_offset],
               data,
               remaining_space);
        data_length -= remaining_space;
        data += remaining_space;
        ctx->leftover_offset = 0;

        for (i = 0; i < 16; ++i) {
            ctx->iv[i] ^= ctx->leftover[i];
        }

        bt_crypto_encrypt_be(ctx->aes_key, ctx->iv, ctx->iv);
    }

    /* CBC encrypt each (except the last) of the data blocks */
    while (data_length > 16) {
        for (i = 0; i < 16; ++i) {
            ctx->iv[i] ^= data[i];
        }

        bt_crypto_encrypt_be(ctx->aes_key, ctx->iv, ctx->iv);
        data += 16;
        data_length  -= 16;
    }

    if (data_length > 0) {
        /* save leftover data for next time */
        memcpy(ctx->leftover, data, data_length);
        ctx->leftover_offset = data_length;
    }

    return 0;
}

__BT_CRYPTO_WEAK__ int bt_crypto_cmac_finish(struct bt_crypto_cmac_t *ctx, uint8_t out[16])
{
    uint8_t *k;
    unsigned int i;

    if (ctx == NULL || out == NULL) {
        return -1;
    }

    if (ctx->leftover_offset == 16) {
        /* the last message block is a full-sized block */
        k = (uint8_t *) ctx->K1;
    } else {
        /* the final message block is not a full-sized  block */
        size_t remaining = 16 - ctx->leftover_offset;

        memset(&ctx->leftover[ctx->leftover_offset], 0, remaining);
        ctx->leftover[ctx->leftover_offset] = 0x80;
        k = (uint8_t *) ctx->K2;
    }

    for (i = 0; i < 16; ++i) {
        ctx->iv[i] ^= ctx->leftover[i] ^ k[i];
    }

    bt_crypto_encrypt_be(ctx->aes_key, ctx->iv, out);

    memset(ctx, 0, sizeof(*ctx));

    return 0;
}


__BT_CRYPTO_WEAK__ int bt_crypto_aes_cmac(const u8_t key[16], struct bt_crypto_sg *sg,
        size_t sg_len, u8_t mac[16])
{
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

    return 0;
}
