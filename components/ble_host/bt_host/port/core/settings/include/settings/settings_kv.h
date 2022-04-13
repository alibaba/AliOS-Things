/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 * Copyright (c) 2015 Runtime Inc
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __SETTINGS_KV_H_
#define __SETTINGS_KV_H_

#include "settings/settings.h"

#ifdef __cplusplus
extern "C" {
#endif

struct settings_kv {
	struct settings_store cf_store;
	const char *name;	/* filename */
};

/* register file to be source of settings */
int settings_kv_src(struct settings_kv *cf);

/* settings saves go to a file */
int settings_kv_dst(struct settings_kv *cf);

#ifdef __cplusplus
}
#endif

#endif /* __SETTINGS_KV_H_ */
