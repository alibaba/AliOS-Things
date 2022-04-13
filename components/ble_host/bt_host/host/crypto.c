/*
 * Copyright (c) 2017 Nordic Semiconductor ASA
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <errno.h>

#include <ble_os.h>
#include "bt_crypto.h"

int bt_rand(void *buf, size_t len)
{
	return bt_crypto_rand(buf, len);
}

int bt_encrypt_le(const u8_t key[16], const u8_t plaintext[16],
		  u8_t enc_data[16])
{
	return bt_crypto_encrypt_le(key, plaintext, enc_data);
}

int bt_encrypt_be(const u8_t key[16], const u8_t plaintext[16],
		  u8_t enc_data[16])
{
	return bt_crypto_encrypt_be(key, plaintext, enc_data);
}

int bt_decrypt_be(const u8_t key[16], const u8_t plaintext[16],
		  u8_t enc_data[16])
{
	return bt_crypto_decrypt_be(key, plaintext, enc_data);
}

