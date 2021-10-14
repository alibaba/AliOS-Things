/* log.c - logging helpers */

/*
 * Copyright (c) 2017 Nordic Semiconductor ASA
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Helper for printk parameters to convert from binary to hex.
 * We declare multiple buffers so the helper can be used multiple times
 * in a single printk call.
 */

#include <stddef.h>
#include <ble_types/types.h>
#include <ble_os.h>
#include <misc/util.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/uuid.h>
#include <bluetooth/hci.h>

const char *bt_hex_real(const void *buf, size_t len)
{
	static const char hex[] = "0123456789abcdef";
	static char str[129];
	const u8_t *b = buf;
	size_t i;

	len = MIN(len, (sizeof(str) - 1) / 2);

	for (i = 0; i < len; i++) {
		str[i * 2]     = hex[b[i] >> 4];
		str[i * 2 + 1] = hex[b[i] & 0xf];
	}

	str[i * 2] = '\0';

	return str;
}

const char *bt_addr_str_real(const bt_addr_t *addr)
{
	static char str[BT_ADDR_STR_LEN];

	bt_addr_to_str(addr, str, sizeof(str));

	return str;
}

const char *bt_addr_le_str_real(const bt_addr_le_t *addr)
{
	static char str[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(addr, str, sizeof(str));

	return str;
}

const char *bt_uuid_str_real(const struct bt_uuid *uuid)
{
	static char str[BT_UUID_STR_LEN];

	bt_uuid_to_str(uuid, str, sizeof(str));

	return str;
}

void hextostring(const uint8_t *source, char *dest, int len)
{
    int i;
    char tmp[3];

    for (i = 0; i < len; i++) {
        sprintf(tmp, "%02x", (unsigned char)source[i]);
        memcpy(&dest[i * 2], tmp, 2);
    }
}

u8_t stringtohex(char *str, u8_t *out, u8_t count)
{
    u8_t i = 0, j = 0;
    u8_t n = 0;

    memset(out, 0, count);
    if (strlen(str) != count << 1) {
        return 0;
    }

    while (i < count) {
        while (j < 2) {
            n = i * 2 + j;
            if (str[n] >= 'a' && str[n] <= 'f') {
                out[i] = out[i] << 4 | (str[n] - 'a' + 10);
            } else if (str[n] >= 'A' && str[n] <= 'F') {
                out[i] = out[i] << 4 | (str[n] - 'A' + 10);
            } else if (str[n] >= '0' && str[n] <= '9') {
                out[i] = out[i] << 4 | (str[n] - '0');
            } else {
                return 0;
            }
            j++;
        }
        j = 0;
        i++;
    }

    return count;
}

