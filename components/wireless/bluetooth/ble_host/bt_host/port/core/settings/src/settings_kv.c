/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 * Copyright (c) 2015 Runtime Inc
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <assert.h>
#include <ble_os.h>

#include <aos/kv.h>
#include <errno.h>
#include "settings/settings.h"
#include "settings/settings_kv.h"
#include "settings_priv.h"

#define BT_DBG_ENABLED 0
#include "common/log.h"

static int settings_kv_load(struct settings_store *cs,
			const struct settings_load_arg *arg);
static int settings_kv_save(struct settings_store *cs, const char *name,
			const char *value, size_t val_len);

static struct settings_store_itf settings_kv_itf = {
    .csi_load = settings_kv_load,
    .csi_save = settings_kv_save,
};

int settings_kv_src(struct settings_kv *cf)
{
    if (!cf->name) {
        return -EINVAL;
    }

    cf->cf_store.cs_itf = &settings_kv_itf;
    settings_src_register(&cf->cf_store);

    return 0;
}

int settings_kv_dst(struct settings_kv *cf)
{
    if (!cf->name) {
        return -EINVAL;
    }

    cf->cf_store.cs_itf = &settings_kv_itf;
    settings_dst_register(&cf->cf_store);

    return 0;
}

struct settings_kv_read_arg_t {
    char *val;
    uint16_t val_size;
};

static ssize_t _settings_kv_read_cb(void *cb_arg, void *data, size_t len)
{
    struct settings_kv_read_arg_t *read_arg = cb_arg;
    size_t read_len = 0;

    BT_DBG("cb_arg %p, read_arg->val %p, read_arg->val_size %d, data %p, len %d", cb_arg, read_arg?read_arg->val:0,
        read_arg?read_arg->val_size:0, data, len);
    
    if (data && read_arg && read_arg->val && read_arg->val_size && len >= read_arg->val_size)
    {
       read_len = len > read_arg->val_size? read_arg->val_size : len;
       memcpy(data, read_arg->val, read_len);
       return read_len;
    }
    return -1;
}

static void _settings_kv_load(char *key, char *val, uint16_t val_size, void *arg)
{
    const char *kv_prefix = "bt/";
    struct settings_kv_read_arg_t read_arg = {0};

    if (0 == strncmp(kv_prefix, key, 3))
    {
        BT_DBG("%s,size %d", key, val_size);
        read_arg.val = val;
        read_arg.val_size = val_size;
        settings_call_set_handler(key, val_size, _settings_kv_read_cb, &read_arg, arg);
    }
}

/*
 * Called to load configuration items. cb must be called for every configuration
 * item found.
 */
static int settings_kv_load(struct settings_store *cs,
			const struct settings_load_arg *arg)
{

#ifdef CONFIG_BT_SETTINGS
#if 0 //remove for build error
    extern void __kv_foreach(void (*func)(char *key, char *val, uint16_t val_size, void *arg), void *arg);
    __kv_foreach(_settings_kv_load, (void *)arg);
#endif
#endif
    return 0;
}

/*
 * Called to save configuration.
 */
static int settings_kv_save(struct settings_store *cs, const char *name,
			const char *value, size_t val_len)
{
    int rc;

    if (!name) {
        return -EINVAL;
    }

    //delete key
    if (value == NULL) {
          aos_kv_del(name);
          return 0;
    }

    rc = aos_kv_set(name, (void *)value, val_len, 1);

    if (rc < 0) {
        BT_ERR("update key fail %s,%s", name, value);
        return -EIO;
    }

    BT_DBG("setting update %s, %s", name, value);

    return 0;
}

int settings_backend_init(void)
{
    static struct settings_kv config_init_settings_kv = {
	    .name = "btsetting",
    };

    settings_kv_src(&config_init_settings_kv);
    settings_kv_dst(&config_init_settings_kv);

	return 0;
}
