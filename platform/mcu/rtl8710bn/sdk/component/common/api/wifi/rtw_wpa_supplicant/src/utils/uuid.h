/*
 * Universally Unique IDentifier (UUID)
 * Copyright (c) 2008, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef UUID_H
#define UUID_H

#include <platform/platform_stdlib.h>
#define UUID_LEN 16

int uuid_str2bin(const char *str, u8 *bin);
static int uuid_bin2str(const u8 *bin, char *str, size_t max_len)
{
	int len;
	len = os_snprintf(str, max_len, "%02x%02x%02x%02x-%02x%02x-%02x%02x-"
			  "%02x%02x-%02x%02x%02x%02x%02x%02x",
			  bin[0], bin[1], bin[2], bin[3],
			  bin[4], bin[5], bin[6], bin[7],
			  bin[8], bin[9], bin[10], bin[11],
			  bin[12], bin[13], bin[14], bin[15]);
	if (len < 0 || (size_t) len >= max_len)
		return -1;
	return 0;
}
int is_nil_uuid(const u8 *uuid);

#endif /* UUID_H */
