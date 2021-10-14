/*
 * Copyright (c) 2018 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ble_os.h>
#include <bt_errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <misc/util.h>
#include <misc/byteorder.h>

#include <settings/settings.h>

#include <net/buf.h>

#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <api/mesh.h>

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_MESH_DEBUG_SETTINGS)
#include "common/log.h"

#include "host/settings.h"
#include "mesh.h"
#include "net.h"
#include "crypto.h"
#include "ble_transport.h"
#include "access.h"
#include "foundation.h"
#include "proxy.h"
// #include "settings.h"
#include "lpn.h"

#ifdef CONFIG_BT_MESH_PROVISIONER
#include "provisioner_prov.h"
#include "provisioner_main.h"
#include "provisioner_proxy.h"
#endif

#ifdef CONFIG_BT_SETTINGS

#ifndef CONFIG_BT_MESH_RPL_STORE_RATE
#define CONFIG_BT_MESH_RPL_STORE_RATE 100
#endif

/* Tracking of what storage changes are pending for App and Net Keys. We
 * track this in a separate array here instead of within the respective
 * bt_mesh_app_key and bt_mesh_subnet structs themselves, since once a key
 * gets deleted its struct becomes invalid and may be reused for other keys.
 */
struct key_update {
    u16_t key_idx:12, /* AppKey or NetKey Index */
        valid:1, /* 1 if this entry is valid, 0 if not */
        app_key:1, /* 1 if this is an AppKey, 0 if a NetKey */
        clear:1, /* 1 if key needs clearing, 0 if storing */
        p_key:1; /*  1 if is provisioner key */
};
// #ifdef CONFIG_BT_MESH_PROVISIONER
// static struct key_update key_updates[CONFIG_BT_MESH_APP_KEY_COUNT + CONFIG_BT_MESH_SUBNET_COUNT + CONFIG_BT_MESH_PROVISIONER_APP_KEY_COUNT + CONFIG_BT_MESH_PROVISIONER_SUBNET_COUNT];
// #else
static struct key_update key_updates[CONFIG_BT_MESH_APP_KEY_COUNT + CONFIG_BT_MESH_SUBNET_COUNT];
// #endif

static struct k_delayed_work pending_store;

/* Mesh network storage information */
struct net_val {
    u16_t primary_addr;
    u8_t dev_key[16];
} __packed;

/* Sequence number storage */
struct seq_val {
    u8_t val[3];
} __packed;

/* Heartbeat Publication storage */
struct hb_pub_val {
    u16_t dst;
    u8_t period;
    u8_t ttl;
    u16_t feat;
    u16_t net_idx:12, indefinite:1;
};

/* Miscelaneous configuration server model states */
struct cfg_val {
    u8_t net_transmit;
    u8_t relay;
    u8_t relay_retransmit;
    u8_t beacon;
    u8_t gatt_proxy;
    u8_t frnd;
    u8_t default_ttl;
};

/* IV Index & IV Update storage */
struct iv_val {
    bt_u32_t iv_index;
    u8_t iv_update:1, iv_duration:7;
} __packed;

/* Replay Protection List storage */
struct rpl_val {
    bt_u32_t seq:24, old_iv:1;
};

/* NetKey storage information */
struct net_key_val {
    u8_t kr_flag:1, kr_phase:7;
    u8_t val[2][16];
} __packed;

/* AppKey storage information */
struct app_key_val {
    u16_t net_idx;
    bool updated;
    u8_t val[2][16];
} __packed;

struct mod_pub_val {
    u16_t addr;
    u16_t key;
    u8_t ttl;
    u8_t retransmit;
    u8_t period;
    u8_t period_div:4, cred:1;
};

/* We need this so we don't overwrite app-hardcoded values in case FCB
 * contains a history of changes but then has a NULL at the end.
 */
static struct {
    bool valid;
    struct cfg_val cfg;
} stored_cfg;

static inline int mesh_x_set(settings_read_cb read_cb, void *cb_arg, void *out, size_t read_len)
{
    ssize_t len;

    len = read_cb(cb_arg, out, read_len);
    if (len < 0) {
        BT_ERR("Failed to read value (err %d)", len);
        return len;
    }

    // BT_HEXDUMP_DBG(out, len, "val");

    if (len != read_len) {
        BT_ERR("Unexpected value length (%zd != %u)", len, read_len);
        return -EINVAL;
    }

    return 0;
}

static int net_set(const char *name, size_t len_rd, settings_read_cb read_cb, void *cb_arg)
{
    struct net_val net;
    int err;

    if (len_rd == 0) {
        bt_mesh_comp_unprovision();
        memset(bt_mesh.dev_key, 0, sizeof(bt_mesh.dev_key));
        return 0;
    }

    err = mesh_x_set(read_cb, cb_arg, &net, sizeof(net));
    if (err) {
        BT_ERR("Failed to set 'net'");
        return err;
    }

    memcpy(bt_mesh.dev_key, net.dev_key, sizeof(bt_mesh.dev_key));
    bt_mesh_comp_provision(net.primary_addr);

    BT_DBG("Provisioned with primary address 0x%04x", net.primary_addr);
    BT_DBG("Recovered DevKey %s", bt_hex(bt_mesh.dev_key, 16));

    return 0;
}

static int iv_set(const char *name, size_t len_rd, settings_read_cb read_cb, void *cb_arg)
{
    struct iv_val iv;
    int err;

    if (len_rd == 0) {
        bt_mesh.iv_index = 0U;
        atomic_clear_bit(bt_mesh.flags, BT_MESH_IVU_IN_PROGRESS);
        return 0;
    }

    err = mesh_x_set(read_cb, cb_arg, &iv, sizeof(iv));
    if (err) {
        BT_ERR("Failed to set 'iv'");
        return err;
    }

    bt_mesh.iv_index = iv.iv_index;
    atomic_set_bit_to(bt_mesh.flags, BT_MESH_IVU_IN_PROGRESS, iv.iv_update);
    bt_mesh.ivu_duration = iv.iv_duration;

    BT_DBG("IV Index 0x%04x (IV Update Flag %u) duration %u hours", iv.iv_index, iv.iv_update, iv.iv_duration);

    return 0;
}

static int seq_set(const char *name, size_t len_rd, settings_read_cb read_cb, void *cb_arg)
{
    struct seq_val seq;
    int err;

    if (len_rd == 0) {
        BT_DBG("val (null)");

        bt_mesh.seq = 0U;
        return 0;
    }

    err = mesh_x_set(read_cb, cb_arg, &seq, sizeof(seq));
    if (err) {
        BT_ERR("Failed to set 'seq'");
        return err;
    }

    bt_mesh.seq = sys_get_le24(seq.val);

    if (CONFIG_BT_MESH_SEQ_STORE_RATE > 0) {
        /* Make sure we have a large enough sequence number. We
         * subtract 1 so that the first transmission causes a write
         * to the settings storage.
         */
        bt_mesh.seq += (CONFIG_BT_MESH_SEQ_STORE_RATE - (bt_mesh.seq % CONFIG_BT_MESH_SEQ_STORE_RATE));
        bt_mesh.seq--;
    }

    BT_DBG("Sequence Number 0x%06x", bt_mesh.seq);

    return 0;
}

static struct bt_mesh_rpl *rpl_find(u16_t src)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(bt_mesh.rpl); i++) {
        if (bt_mesh.rpl[i].src == src) {
            return &bt_mesh.rpl[i];
        }
    }

    return NULL;
}

static struct bt_mesh_rpl *rpl_alloc(u16_t src)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(bt_mesh.rpl); i++) {
        if (!bt_mesh.rpl[i].src) {
            bt_mesh.rpl[i].src = src;
            return &bt_mesh.rpl[i];
        }
    }

    return NULL;
}

static int rpl_set(const char *name, size_t len_rd, settings_read_cb read_cb, void *cb_arg)
{
    struct bt_mesh_rpl *entry;
    struct rpl_val rpl;
    int err;
    u16_t src;

    if (!name) {
        BT_ERR("Insufficient number of arguments");
        return -ENOENT;
    }

    src = strtol(name, NULL, 16);
    entry = rpl_find(src);

    if (len_rd == 0) {
        BT_DBG("val (null)");
        if (entry) {
            (void)memset(entry, 0, sizeof(*entry));
        } else {
            BT_WARN("Unable to find RPL entry for 0x%04x", src);
        }

        return 0;
    }

    if (!entry) {
        entry = rpl_alloc(src);
        if (!entry) {
            BT_ERR("Unable to allocate RPL entry for 0x%04x", src);
            return -ENOMEM;
        }
    }

    err = mesh_x_set(read_cb, cb_arg, &rpl, sizeof(rpl));
    if (err) {
        BT_ERR("Failed to set `net`");
        return err;
    }

    entry->seq = rpl.seq;
    entry->old_iv = rpl.old_iv;

    BT_DBG("RPL entry for 0x%04x: Seq 0x%06x old_iv %u", entry->src, entry->seq, entry->old_iv);

    return 0;
}

static int net_key_set(const char *name, size_t len_rd, settings_read_cb read_cb, void *cb_arg)
{
    struct bt_mesh_subnet *sub;
    struct net_key_val key;
    int i, err;
    u16_t net_idx;

    if (!name) {
        BT_ERR("Insufficient number of arguments");
        return -ENOENT;
    }

    net_idx = strtol(name, NULL, 16);
    sub = bt_mesh_subnet_get(net_idx);

    if (len_rd == 0) {
        BT_DBG("val (null)");
        if (!sub) {
            BT_ERR("No subnet with NetKeyIndex 0x%03x", net_idx);
            return -ENOENT;
        }

        BT_DBG("Deleting NetKeyIndex 0x%03x", net_idx);
        bt_mesh_subnet_del(sub, false);
        return 0;
    }

    err = mesh_x_set(read_cb, cb_arg, &key, sizeof(key));
    if (err) {
        BT_ERR("Failed to set \'net-key\'");
        return err;
    }

    if (sub) {
        BT_DBG("Updating existing NetKeyIndex 0x%03x", net_idx);

        sub->kr_flag = key.kr_flag;
        sub->kr_phase = key.kr_phase;
        memcpy(sub->keys[0].net, &key.val[0], 16);
        memcpy(sub->keys[1].net, &key.val[1], 16);

        return 0;
    }

    for (i = 0; i < ARRAY_SIZE(bt_mesh.sub); i++) {
        if (bt_mesh.sub[i].net_idx == BT_MESH_KEY_UNUSED) {
            sub = &bt_mesh.sub[i];
            break;
        }
    }

    if (!sub) {
        BT_ERR("No space to allocate a new subnet");
        return -ENOMEM;
    }

    sub->net_idx = net_idx;
    sub->kr_flag = key.kr_flag;
    sub->kr_phase = key.kr_phase;
    memcpy(sub->keys[0].net, &key.val[0], 16);
    memcpy(sub->keys[1].net, &key.val[1], 16);

    BT_DBG("NetKeyIndex 0x%03x recovered from storage", net_idx);

    return 0;
}

static int app_key_set(const char *name, size_t len_rd, settings_read_cb read_cb, void *cb_arg)
{
    struct bt_mesh_app_key *app;
    struct app_key_val key;
    u16_t app_idx;
    int err;

    if (!name) {
        BT_ERR("Insufficient number of arguments");
        return -ENOENT;
    }

    app_idx = strtol(name, NULL, 16);

    if (len_rd == 0) {
        BT_DBG("val (null)");
        BT_DBG("Deleting AppKeyIndex 0x%03x", app_idx);

        app = bt_mesh_app_key_find(app_idx);
        if (app) {
            bt_mesh_app_key_del(app, false);
        }

        return 0;
    }

    err = mesh_x_set(read_cb, cb_arg, &key, sizeof(key));
    if (err) {
        BT_ERR("Failed to set \'app-key\'");
        return err;
    }

    app = bt_mesh_app_key_find(app_idx);
    if (!app) {
        app = bt_mesh_app_key_alloc(app_idx);
    }

    if (!app) {
        BT_ERR("No space for a new app key");
        return -ENOMEM;
    }

    app->net_idx = key.net_idx;
    app->app_idx = app_idx;
    app->updated = key.updated;
    app->appkey_active = 1;
    memcpy(app->keys[0].val, key.val[0], 16);
    memcpy(app->keys[1].val, key.val[1], 16);

    bt_mesh_app_id(app->keys[0].val, &app->keys[0].id);
    bt_mesh_app_id(app->keys[1].val, &app->keys[1].id);

    BT_DBG("AppKeyIndex 0x%03x recovered from storage", app_idx);

    return 0;
}

#ifdef CONFIG_BT_MESH_PROVISIONER
// static int pnet_key_set(const char *name, size_t len_rd, settings_read_cb read_cb,
//     void *cb_arg)
// {
//     struct bt_mesh_subnet *sub;
//     struct net_key_val key;
//     int i, err;
//     u16_t net_idx;

//     net_idx = strtol(name, NULL, 16);
//     sub = provisioner_subnet_get(net_idx);

//     if (len_rd == 0) {
//         if (!sub) {
//             BT_ERR("No subnet with NetKeyIndex 0x%03x", net_idx);
//             return -ENOENT;
//         }

//         BT_DBG("Deleting NetKeyIndex 0x%03x", net_idx);
//         bt_mesh_provisioner_local_net_key_delete(net_idx);
//         return 0;
//     }

// err = mesh_x_set(read_cb, cb_arg, &key, sizeof(key));
// if (err) {
//     BT_ERR("Failed to set \'pnet key\'");
//     return err;
// }

//     if (sub) {
//         BT_DBG("Updating existing NetKeyIndex 0x%03x", net_idx);

//         sub->kr_flag = key.kr_flag;
//         sub->kr_phase = key.kr_phase;
//         memcpy(sub->keys[0].net, &key.val[0], 16);
//         memcpy(sub->keys[1].net, &key.val[1], 16);

//         return 0;
//     }

//     for (i = 0; i < ARRAY_SIZE(bt_mesh.p_sub); i++) {
//         if (bt_mesh.p_sub[i].net_idx == BT_MESH_KEY_UNUSED) {
//             sub = &bt_mesh.p_sub[i];
//             sub->net_idx = net_idx;
//             sub->kr_flag = key.kr_flag;
//             sub->kr_phase = key.kr_phase;
//             memcpy(sub->keys[0].net, &key.val[0], 16);
//             memcpy(sub->keys[1].net, &key.val[1], 16);
//             break;
//         }
//     }

//     BT_DBG("NetKeyIndex 0x%03x recovered from storage", net_idx);

//     return 0;
// }

static int papp_key_set(const char *name, size_t len_rd, settings_read_cb read_cb, void *cb_arg)
{
    struct bt_mesh_app_key *app;
    struct app_key_val key;
    u16_t app_idx;
    int err;

    app_idx = strtol(name, NULL, 16);

    if (len_rd == 0) {
        BT_DBG("Deleting AppKeyIndex 0x%03x", app_idx);

        app = provisioner_app_key_find(app_idx);

        if (app) {
            bt_mesh_provisioner_local_app_key_delete(0, app_idx);
        }

        return 0;
    }

    err = mesh_x_set(read_cb, cb_arg, &key, sizeof(key));
    if (err) {
        BT_ERR("Failed to set \'papp key\'");
        return err;
    }

    app = provisioner_app_key_find(app_idx);

    if (!app) {
        app = bt_mesh_provisioner_p_app_key_alloc();
    }

    if (!app) {
        BT_ERR("No space for a new app key");
        return -ENOMEM;
    }

    app->net_idx = key.net_idx;
    app->app_idx = app_idx;
    app->updated = key.updated;
    app->appkey_active = 1;
    memcpy(app->keys[0].val, key.val[0], 16);
    memcpy(app->keys[1].val, key.val[1], 16);

    bt_mesh_app_id(app->keys[0].val, &app->keys[0].id);
    bt_mesh_app_id(app->keys[1].val, &app->keys[1].id);

    BT_DBG("AppKeyIndex 0x%03x recovered from storage", app_idx);

    return 0;
}
#endif

static int hb_pub_set(const char *name, size_t len_rd, settings_read_cb read_cb, void *cb_arg)
{
    struct bt_mesh_hb_pub *pub = bt_mesh_hb_pub_get();
    struct hb_pub_val hb_val;
    int err;

    if (!pub) {
        return -ENOENT;
    }

    if (len_rd == 0) {
        pub->dst = BT_MESH_ADDR_UNASSIGNED;
        pub->count = 0U;
        pub->ttl = 0U;
        pub->period = 0U;
        pub->feat = 0U;

        BT_DBG("Cleared heartbeat publication");
        return 0;
    }

    err = mesh_x_set(read_cb, cb_arg, &hb_val, sizeof(hb_val));
    if (err) {
        BT_ERR("Failed to set \'hb_val\'");
        return err;
    }

    pub->dst = hb_val.dst;
    pub->period = hb_val.period;
    pub->ttl = hb_val.ttl;
    pub->feat = hb_val.feat;
    pub->net_idx = hb_val.net_idx;

    if (hb_val.indefinite) {
        pub->count = 0xffff;
    } else {
        pub->count = 0U;
    }

    BT_DBG("Restored heartbeat publication");

    return 0;
}

static int cfg_set(const char *name, size_t len_rd, settings_read_cb read_cb, void *cb_arg)
{
    struct bt_mesh_cfg_srv *cfg = bt_mesh_cfg_get();
    int err;

    if (!cfg) {
        return -ENOENT;
    }

    if (len_rd == 0) {
        stored_cfg.valid = false;
        BT_DBG("Cleared configuration state");
        return 0;
    }

    err = mesh_x_set(read_cb, cb_arg, &stored_cfg.cfg, sizeof(stored_cfg.cfg));
    if (err) {
        BT_ERR("Failed to set \'cfg\'");
        return err;
    }

    stored_cfg.valid = true;
    BT_DBG("Restored configuration state");

    return 0;
}

static int mod_set_bind(struct bt_mesh_model *mod, size_t len_rd, settings_read_cb read_cb, void *cb_arg)
{
    ssize_t len;
    int i;

    /* Start with empty array regardless of cleared or set value */
    for (i = 0; i < ARRAY_SIZE(mod->keys); i++) {
        mod->keys[i] = BT_MESH_KEY_UNUSED;
    }

    if (len_rd == 0) {
        BT_DBG("Cleared bindings for model");
        return 0;
    }

    len = read_cb(cb_arg, mod->keys, sizeof(mod->keys));
    if (len < 0) {
        BT_ERR("Failed to read value (err %zd)", len);
        return len;
    }

    BT_DBG("Decoded %zu bound keys for model", len / sizeof(mod->keys[0]));
    return 0;
}

static int mod_set_sub(struct bt_mesh_model *mod, size_t len_rd, settings_read_cb read_cb, void *cb_arg)
{
    ssize_t len;

    /* Start with empty array regardless of cleared or set value */
    (void)memset(mod->groups, 0, sizeof(mod->groups));

    if (len_rd == 0) {
        BT_DBG("Cleared subscriptions for model");
        return 0;
    }

    len = read_cb(cb_arg, mod->groups, sizeof(mod->groups));
    if (len < 0) {
        BT_ERR("Failed to read value (err %zd)", len);
        return len;
    }

    BT_DBG("Decoded %zu subscribed group addresses for model", len / sizeof(mod->groups[0]));
    return 0;
}

static int mod_set_pub(struct bt_mesh_model *mod, size_t len_rd, settings_read_cb read_cb, void *cb_arg)
{
    struct mod_pub_val pub;
    int err;

    if (!mod->pub) {
        BT_WARN("Model has no publication context!");
        return -EINVAL;
    }

    if (len_rd == 0) {
        mod->pub->addr = BT_MESH_ADDR_UNASSIGNED;
        mod->pub->key = 0U;
        mod->pub->cred = 0U;
        mod->pub->ttl = 0U;
        mod->pub->period = 0U;
        mod->pub->retransmit = 0U;
        mod->pub->count = 0U;

        BT_DBG("Cleared publication for model");
        return 0;
    }

    err = mesh_x_set(read_cb, cb_arg, &pub, sizeof(pub));
    if (err) {
        BT_ERR("Failed to set \'model-pub\'");
        return err;
    }

    mod->pub->addr = pub.addr;
    mod->pub->key = pub.key;
    mod->pub->cred = pub.cred;
    mod->pub->ttl = pub.ttl;
    mod->pub->period = pub.period;
    mod->pub->retransmit = pub.retransmit;
    mod->pub->count = 0U;

    BT_DBG("Restored model publication, dst 0x%04x app_idx 0x%03x", pub.addr, pub.key);

    return 0;
}

static int mod_set(bool vnd, const char *name, size_t len_rd, settings_read_cb read_cb, void *cb_arg)
{
    struct bt_mesh_model *mod;
    u8_t elem_idx, mod_idx;
    u16_t mod_key;
    int len;
    const char *next;

    if (!name) {
        BT_ERR("Insufficient number of arguments");
        return -ENOENT;
    }

    mod_key = strtol(name, NULL, 16);
    elem_idx = mod_key >> 8;
    mod_idx = mod_key;

    BT_DBG("Decoded mod_key 0x%04x as elem_idx %u mod_idx %u", mod_key, elem_idx, mod_idx);

    mod = bt_mesh_model_get(vnd, elem_idx, mod_idx);
    if (!mod) {
        BT_ERR("Failed to get model for elem_idx %u mod_idx %u", elem_idx, mod_idx);
        return -ENOENT;
    }

    len = settings_name_next(name, &next);

    if (!next) {
        BT_ERR("Insufficient number of arguments");
        return -ENOENT;
    }

    if (!strncmp(next, "bind", len)) {
        return mod_set_bind(mod, len_rd, read_cb, cb_arg);
    }

    if (!strncmp(next, "sub", len)) {
        return mod_set_sub(mod, len_rd, read_cb, cb_arg);
    }

    if (!strncmp(next, "pub", len)) {
        return mod_set_pub(mod, len_rd, read_cb, cb_arg);
    }

    BT_WARN("Unknown module key %s", next);
    return -ENOENT;
}

static int sig_mod_set(const char *name, size_t len_rd, settings_read_cb read_cb, void *cb_arg)
{
    return mod_set(false, name, len_rd, read_cb, cb_arg);
}

static int vnd_mod_set(const char *name, size_t len_rd, settings_read_cb read_cb, void *cb_arg)
{
    return mod_set(true, name, len_rd, read_cb, cb_arg);
}

#ifdef CONFIG_BT_MESH_PROVISIONER
static int prov_node_set(const char *name, size_t len_rd, settings_read_cb read_cb, void *cb_arg)
{
    struct bt_mesh_node_t node_info = { 0 };
    int node_id = 0;
    int err;

    if (name == NULL) {
        BT_ERR("Invail name NULL");
        return -EINVAL;
    }

    node_id = strtol(name, NULL, 16);

    BT_DBG("Restore Node %d", node_id);
    (void)node_id;

    err = mesh_x_set(read_cb, cb_arg, &node_info, sizeof(node_info));
    if (err) {
        BT_ERR("Failed to set \'cfg\'");
        return err;
    }

    bt_addr_le_t addr;
    addr.type = node_info.addr_type;
    memcpy(addr.a.val, node_info.addr_val, 6);
    provisioner_prov_restore_nodes_info(&addr, node_info.dev_uuid, node_info.oob_info, node_info.element_num,
                                        node_info.unicast_addr, node_info.net_idx, node_info.flags, node_info.iv_index,
                                        node_info.dev_key, 0);
    return 0;
}
#endif

const struct mesh_setting {
    const char *name;
    int (*func)(const char *name, size_t len_rd, settings_read_cb read_cb, void *cb_arg);
} settings[] = {
    { "Net", net_set },        { "IV", iv_set },          { "Seq", seq_set },      { "RPL", rpl_set },
    { "NetKey", net_key_set }, { "AppKey", app_key_set }, { "HBPub", hb_pub_set }, { "Cfg", cfg_set },
    { "s", sig_mod_set },      { "v", vnd_mod_set },
#ifdef CONFIG_BT_MESH_PROVISIONER
    { "Node", prov_node_set },
//    { "pNetKey", pnet_key_set },
//    { "pAppKey", papp_key_set },
#endif
};

static int mesh_set(const char *name, size_t len_rd, settings_read_cb read_cb, void *cb_arg)
{
    int i, len;
    const char *next;

    if (!name) {
        BT_ERR("Insufficient number of arguments");
        return -EINVAL;
    }

    len = settings_name_next(name, &next);

    for (i = 0; i < ARRAY_SIZE(settings); i++) {
        if (!strncmp(settings[i].name, name, len)) {
            return settings[i].func(next, len_rd, read_cb, cb_arg);
        }
    }

    BT_WARN("No matching handler for key %s", log_strdup(name));

    return -ENOENT;
}

static int subnet_init(struct bt_mesh_subnet *sub)
{
    int err;

    err = bt_mesh_net_keys_create(&sub->keys[0], sub->keys[0].net);

    if (err) {
        BT_ERR("Unable to generate keys for subnet");
        return -EIO;
    }

    if (sub->kr_phase != BT_MESH_KR_NORMAL) {
        err = bt_mesh_net_keys_create(&sub->keys[1], sub->keys[1].net);

        if (err) {
            BT_ERR("Unable to generate keys for subnet");
            memset(&sub->keys[0], 0, sizeof(sub->keys[0]));
            return -EIO;
        }
    }

    if (IS_ENABLED(CONFIG_BT_MESH_GATT_PROXY)) {
        sub->node_id = BT_MESH_NODE_IDENTITY_STOPPED;
    } else {
        sub->node_id = BT_MESH_NODE_IDENTITY_NOT_SUPPORTED;
    }

    /* Make sure we have valid beacon data to be sent */
    bt_mesh_net_beacon_update(sub);

    return 0;
}

static void commit_mod(struct bt_mesh_model *mod, struct bt_mesh_elem *elem, bool vnd, bool primary, void *user_data)
{
    if (mod->pub && mod->pub->update && mod->pub->addr != BT_MESH_ADDR_UNASSIGNED) {
        bt_s32_t ms = bt_mesh_model_pub_period_get(mod);

        if (ms) {
            BT_DBG("Starting publish timer (period %u ms)", ms);
            k_delayed_work_submit(&mod->pub->timer, ms);
        }
    }
}

static int mesh_commit(void)
{
    struct bt_mesh_hb_pub *hb_pub;
    struct bt_mesh_cfg_srv *cfg;
    int i;

#ifdef CONFIG_BT_MESH_PROVISIONER
    // BT_DBG("sub[0].net_idx 0x%03x", bt_mesh.p_sub[0].net_idx);

    // if (bt_mesh.p_sub[0].net_idx != BT_MESH_KEY_UNUSED) {
    //     for (i = 0; i < ARRAY_SIZE(bt_mesh.p_sub); i++) {
    //         struct bt_mesh_subnet *sub = &bt_mesh.p_sub[i];

    //         if (sub->net_idx == BT_MESH_KEY_UNUSED) {
    //             continue;
    //         }

    //         if (sub->kr_flag) {
    //             if (bt_mesh_net_keys_create(&sub->keys[1], sub->keys[1].net)) {
    //                 BT_ERR("%s: create net_keys fail", __func__);
    //                 sub->subnet_active = false;
    //                 continue;
    //             }

    //             sub->kr_phase = BT_MESH_KR_PHASE_2;
    //         } else {
    //             /* Currently provisioner only use keys[0] */
    //             if (bt_mesh_net_keys_create(&sub->keys[0], sub->keys[0].net)) {
    //                 BT_ERR("%s: create net_keys fail", __func__);
    //                 sub->subnet_active = false;
    //                 continue;
    //             }

    //             sub->kr_phase = BT_MESH_KR_NORMAL;
    //         }

    //         sub->node_id = BT_MESH_NODE_IDENTITY_NOT_SUPPORTED;
    //         sub->subnet_active = true;
    //     }
    // }

    if (IS_ENABLED(CONFIG_BT_SETTINGS) && !bt_mesh.seq && CONFIG_BT_MESH_SEQ_STORE_RATE > 0) {
        /* Make sure we have a large enough sequence number. We
         * subtract 1 so that the first transmission causes a write
         * to the settings storage.
         */
        bt_mesh.seq = CONFIG_BT_MESH_SEQ_STORE_RATE - 1;
    }

#endif

    BT_DBG("sub[0].net_idx 0x%03x", bt_mesh.sub[0].net_idx);

    if (bt_mesh.sub[0].net_idx == BT_MESH_KEY_UNUSED) {
        /* Nothing to do since we're not yet provisioned */
        return 0;
    }

    if (IS_ENABLED(CONFIG_BT_MESH_PB_GATT)) {
        bt_mesh_proxy_prov_disable(true);
    }

    for (i = 0; i < ARRAY_SIZE(bt_mesh.sub); i++) {
        struct bt_mesh_subnet *sub = &bt_mesh.sub[i];
        int err;

        if (sub->net_idx == BT_MESH_KEY_UNUSED) {
            continue;
        }

        err = subnet_init(sub);

        if (err) {
            BT_ERR("Failed to init subnet 0x%03x", sub->net_idx);
        } else {
            sub->subnet_active = true;
        }
    }

    if (bt_mesh.ivu_duration < BT_MESH_IVU_MIN_HOURS) {
        k_delayed_work_submit(&bt_mesh.ivu_timer, BT_MESH_IVU_TIMEOUT);
    }

    bt_mesh_model_foreach(commit_mod, NULL);

    hb_pub = bt_mesh_hb_pub_get();

    if (hb_pub && hb_pub->dst != BT_MESH_ADDR_UNASSIGNED && hb_pub->count && hb_pub->period) {
        BT_DBG("Starting heartbeat publication");
        k_work_submit(&hb_pub->timer.work);
    }

    cfg = bt_mesh_cfg_get();

    if (cfg && stored_cfg.valid) {
        cfg->net_transmit = stored_cfg.cfg.net_transmit;
        cfg->relay = stored_cfg.cfg.relay;
        cfg->relay_retransmit = stored_cfg.cfg.relay_retransmit;
        cfg->beacon = stored_cfg.cfg.beacon;
        cfg->gatt_proxy = stored_cfg.cfg.gatt_proxy;
        cfg->frnd = stored_cfg.cfg.frnd;
        cfg->default_ttl = stored_cfg.cfg.default_ttl;
    }

    atomic_set_bit(bt_mesh.flags, BT_MESH_VALID);

    // #ifndef CONFIG_BT_MESH_PROVISIONER
    bt_mesh_net_start();
    // #endif

    return 0;
}

//BT_SETTINGS_DEFINE(mesh, mesh_set, mesh_commit, NULL);

static void schedule_store(int flag)
{
    bt_s32_t timeout;

    atomic_set_bit(bt_mesh.flags, flag);

    if (atomic_test_bit(bt_mesh.flags, BT_MESH_NET_PENDING) || atomic_test_bit(bt_mesh.flags, BT_MESH_IV_PENDING) ||
        atomic_test_bit(bt_mesh.flags, BT_MESH_SEQ_PENDING) || atomic_test_bit(bt_mesh.flags, BT_MESH_KEYS_PENDING) ||
        atomic_test_bit(bt_mesh.flags, BT_MESH_MOD_PENDING) || atomic_test_bit(bt_mesh.flags, BT_MESH_CFG_PENDING) ||
        atomic_test_bit(bt_mesh.flags, BT_MESH_HB_PUB_PENDING)
#ifdef CONFIG_BT_MESH_PROVISIONER
        || atomic_test_bit(bt_mesh.flags, BT_MESH_PROV_NODE_PENDING)
#endif
    ) {
        timeout = K_NO_WAIT;
    } else if (atomic_test_bit(bt_mesh.flags, BT_MESH_RPL_PENDING) &&
               (CONFIG_BT_MESH_RPL_STORE_TIMEOUT < CONFIG_BT_MESH_STORE_TIMEOUT)) {
        timeout = K_SECONDS(CONFIG_BT_MESH_RPL_STORE_TIMEOUT);
    } else {
        timeout = K_SECONDS(CONFIG_BT_MESH_STORE_TIMEOUT);
    }

    BT_DBG("Waiting %d seconds", timeout / MSEC_PER_SEC);

    k_delayed_work_submit(&pending_store, timeout);
}

static void clear_iv(void)
{
    int err;

    err = settings_delete("bt/mesh/IV");
    if (err) {
        BT_ERR("Failed to clear IV");
    } else {
        BT_DBG("Cleared IV");
    }
}

static void clear_net(void)
{
    int err;

    err = settings_delete("bt/mesh/Net");
    if (err) {
        BT_ERR("Failed to clear Network");
    } else {
        BT_DBG("Cleared Network");
    }
}

static void store_pending_net(void)
{
    struct net_val net;
    int err;

    BT_DBG("addr 0x%04x DevKey %s", bt_mesh_primary_addr(), bt_hex(bt_mesh.dev_key, 16));

    net.primary_addr = bt_mesh_primary_addr();
    memcpy(net.dev_key, bt_mesh.dev_key, 16);

    err = settings_save_one("bt/mesh/Net", &net, sizeof(net));
    if (err) {
        BT_ERR("Failed to store Network value");
    } else {
        BT_DBG("Stored Network value");
    }
}

void bt_mesh_store_net(void)
{
    schedule_store(BT_MESH_NET_PENDING);
}

static void store_pending_iv(void)
{
    struct iv_val iv;
    int err;

    iv.iv_index = bt_mesh.iv_index;
    iv.iv_update = atomic_test_bit(bt_mesh.flags, BT_MESH_IVU_IN_PROGRESS);
    iv.iv_duration = bt_mesh.ivu_duration;

    err = settings_save_one("bt/mesh/IV", &iv, sizeof(iv));
    if (err) {
        BT_ERR("Failed to store IV value");
    } else {
        BT_DBG("Stored IV value");
    }
}

void bt_mesh_store_iv(bool only_duration)
{
    schedule_store(BT_MESH_IV_PENDING);

    if (!only_duration) {
        /* Always update Seq whenever IV changes */
        schedule_store(BT_MESH_SEQ_PENDING);
    }
}

static void store_pending_seq(void)
{
    struct seq_val seq;
    int err;

    sys_put_le24(bt_mesh.seq, seq.val);

    err = settings_save_one("bt/mesh/Seq", &seq, sizeof(seq));
    if (err) {
        BT_ERR("Failed to stor Seq value");
    } else {
        BT_DBG("Stored Seq value");
    }
}

void bt_mesh_store_seq(void)
{
    if (CONFIG_BT_MESH_SEQ_STORE_RATE && (bt_mesh.seq % CONFIG_BT_MESH_SEQ_STORE_RATE)) {
        return;
    }

    schedule_store(BT_MESH_SEQ_PENDING);
}

static void store_rpl(struct bt_mesh_rpl *entry)
{
    struct rpl_val rpl;
    char path[18];
    int err;

    BT_DBG("src 0x%04x seq 0x%06x old_iv %u", entry->src, entry->seq, entry->old_iv);

    rpl.seq = entry->seq;
    rpl.old_iv = entry->old_iv;

    snprintk(path, sizeof(path), "bt/mesh/RPL/%x", entry->src);

    err = settings_save_one(path, &rpl, sizeof(rpl));
    if (err) {
        BT_ERR("Failed to store RPL %s value", log_strdup(path));
    } else {
        BT_DBG("Stored RPL %s value", log_strdup(path));
    }
}

static void clear_rpl(void)
{
    int i, err;

    BT_DBG("");

    for (i = 0; i < ARRAY_SIZE(bt_mesh.rpl); i++) {
        struct bt_mesh_rpl *rpl = &bt_mesh.rpl[i];
        char path[18];

        if (!rpl->src) {
            continue;
        }

        snprintk(path, sizeof(path), "bt/mesh/RPL/%x", rpl->src);
        err = settings_delete(path);
        if (err) {
            BT_ERR("Failed to clear RPL");
        } else {
            BT_DBG("Cleared RPL");
        }

        (void)memset(rpl, 0, sizeof(*rpl));
    }
}

static void store_pending_rpl(void)
{
    int i;

    BT_DBG("");

    for (i = 0; i < ARRAY_SIZE(bt_mesh.rpl); i++) {
        struct bt_mesh_rpl *rpl = &bt_mesh.rpl[i];

        if (rpl->store) {
            rpl->store = false;
            store_rpl(rpl);
        }
    }
}

static void store_pending_hb_pub(void)
{
    struct bt_mesh_hb_pub *pub = bt_mesh_hb_pub_get();
    struct hb_pub_val val;
    int err;

    if (!pub) {
        return;
    }

    if (pub->dst == BT_MESH_ADDR_UNASSIGNED) {
        err = settings_delete("bt/mesh/HBPub");
    } else {
        val.indefinite = (pub->count == 0xffff);
        val.dst = pub->dst;
        val.period = pub->period;
        val.ttl = pub->ttl;
        val.feat = pub->feat;
        val.net_idx = pub->net_idx;

        err = settings_save_one("bt/mesh/HBPub", &val, sizeof(val));
    }

    if (err) {
        BT_ERR("Failed to store Heartbeat Publication");
    } else {
        BT_DBG("Stored Heartbeat Publication");
    }
}

static void store_pending_cfg(void)
{
    struct bt_mesh_cfg_srv *cfg = bt_mesh_cfg_get();
    struct cfg_val val;
    int err;

    if (!cfg) {
        return;
    }

    val.net_transmit = cfg->net_transmit;
    val.relay = cfg->relay;
    val.relay_retransmit = cfg->relay_retransmit;
    val.beacon = cfg->beacon;
    val.gatt_proxy = cfg->gatt_proxy;
    val.frnd = cfg->frnd;
    val.default_ttl = cfg->default_ttl;

    err = settings_save_one("bt/mesh/Cfg", &val, sizeof(val));
    if (err) {
        BT_ERR("Failed to store configuration value");
    } else {
        BT_DBG("Stored configuration value");
        BT_HEXDUMP_DBG(&val, sizeof(val), "raw value");
    }
}

static void clear_cfg(void)
{
    int err;

    err = settings_delete("bt/mesh/Cfg");
    if (err) {
        BT_ERR("Failed to clear configuration");
    } else {
        BT_DBG("Cleared configuration");
    }
}

static void clear_app_key(u16_t app_idx, bool p_key)
{
    int err;
    char path[21];

    BT_DBG("AppKeyIndex 0x%03x", app_idx);

#ifdef CONFIG_BT_MESH_PROVISIONER

    if (p_key) {
        snprintk(path, sizeof(path), "bt/mesh/pAppKey/%x", app_idx);
    } else
#endif
    {
        snprintk(path, sizeof(path), "bt/mesh/AppKey/%x", app_idx);
    }

    err = settings_delete(path);
    if (err) {
        BT_ERR("Failed to clear AppKeyIndex 0x%03x", app_idx);
    } else {
        BT_DBG("Cleared AppKeyIndex 0x%03x", app_idx);
    }
}

static void clear_net_key(u16_t net_idx, bool p_key)
{
    int err;
    char path[21];

    BT_DBG("NetKeyIndex 0x%03x", net_idx);

#ifdef CONFIG_BT_MESH_PROVISIONER

    if (p_key) {
        snprintk(path, sizeof(path), "bt/mesh/pNetKey/%x", net_idx);
    } else
#endif
    {
        snprintk(path, sizeof(path), "bt/mesh/NetKey/%x", net_idx);
    }

    err = settings_delete(path);
    if (err) {
        BT_ERR("Failed to clear NetKeyIndex 0x%03x", net_idx);
    } else {
        BT_DBG("Cleared NetKeyIndex 0x%03x", net_idx);
    }
}

static void store_net_key(struct bt_mesh_subnet *sub, bool p_key)
{
    int err;
    struct net_key_val key;
    char path[21];

    BT_DBG("NetKeyIndex 0x%03x NetKey %s", sub->net_idx, bt_hex(sub->keys[0].net, 16));

    memcpy(&key.val[0], sub->keys[0].net, 16);
    memcpy(&key.val[1], sub->keys[1].net, 16);
    key.kr_flag = sub->kr_flag;
    key.kr_phase = sub->kr_phase;

#ifdef CONFIG_BT_MESH_PROVISIONER
    if (p_key) {
        snprintk(path, sizeof(path), "bt/mesh/pNetKey/%x", sub->net_idx);
    } else
#endif
    {
        snprintk(path, sizeof(path), "bt/mesh/NetKey/%x", sub->net_idx);
    }

    err = settings_save_one(path, &key, sizeof(key));
    if (err) {
        BT_ERR("Failed to store NetKey value");
    } else {
        BT_DBG("Stored NetKey value");
    }
}

static void store_app_key(struct bt_mesh_app_key *app, bool p_key)
{
    int err;
    struct app_key_val key;
    char path[21];

    key.net_idx = app->net_idx;
    key.updated = app->updated;
    memcpy(key.val[0], app->keys[0].val, 16);
    memcpy(key.val[1], app->keys[1].val, 16);

#ifdef CONFIG_BT_MESH_PROVISIONER

    if (p_key) {
        snprintk(path, sizeof(path), "bt/mesh/pAppKey/%x", app->app_idx);
    } else
#endif
    {
        snprintk(path, sizeof(path), "bt/mesh/AppKey/%x", app->app_idx);
    }

    err = settings_save_one(path, &key, sizeof(key));
    if (err) {
        BT_ERR("Failed to store AppKey %s value", log_strdup(path));
    } else {
        BT_DBG("Stored AppKey %s value", log_strdup(path));
    }
}

static void store_pending_keys(void)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(key_updates); i++) {
        struct key_update *update = &key_updates[i];

        if (!update->valid) {
            continue;
        }

        if (update->clear) {
            if (update->app_key) {
                clear_app_key(update->key_idx, update->p_key);
            } else {
                clear_net_key(update->key_idx, update->p_key);
            }
        } else {
            if (update->app_key) {
                struct bt_mesh_app_key *key = NULL;
#ifdef CONFIG_BT_MESH_PROVISIONER

                if (update->p_key) {
                    key = provisioner_app_key_find(update->key_idx);
                } else
#endif
                {
                    key = bt_mesh_app_key_find(update->key_idx);
                }

                if (key) {
                    store_app_key(key, update->p_key);
                } else {
                    BT_WARN("AppKeyIndex 0x%03x not found", update->key_idx);
                }

            } else {
                struct bt_mesh_subnet *sub;
                // #ifdef CONFIG_BT_MESH_PROVISIONER

                //                if (update->p_key) {
                //                    sub = provisioner_subnet_get(update->key_idx);
                //               } else
                // #endif
                {
                    sub = bt_mesh_subnet_get(update->key_idx);
                }

                if (sub) {
                    store_net_key(sub, update->p_key);
                } else {
                    BT_WARN("NetKeyIndex 0x%03x not found", update->key_idx);
                }
            }
        }

        update->valid = 0;
    }
}

static void encode_mod_path(struct bt_mesh_model *mod, bool vnd, const char *key, char *path, size_t path_len)
{
    u16_t mod_key = (((u16_t)mod->elem_idx << 8) | mod->mod_idx);

    if (vnd) {
        snprintk(path, path_len, "bt/mesh/v/%x/%s", mod_key, key);
    } else {
        snprintk(path, path_len, "bt/mesh/s/%x/%s", mod_key, key);
    }
}

static void store_pending_mod_bind(struct bt_mesh_model *mod, bool vnd)
{
    u16_t keys[CONFIG_BT_MESH_MODEL_KEY_COUNT];
    char path[20];
    int i, count, err;

    for (i = 0, count = 0; i < ARRAY_SIZE(mod->keys); i++) {
        if (mod->keys[i] != BT_MESH_KEY_UNUSED) {
            keys[count++] = mod->keys[i];
        }
    }

    encode_mod_path(mod, vnd, "bind", path, sizeof(path));

    if (count) {
        err = settings_save_one(path, keys, count * sizeof(keys[0]));
    } else {
        err = settings_delete(path);
    }

    if (err) {
        BT_ERR("Failed to store %s value", log_strdup(path));
    } else {
        BT_DBG("Stored %s value", log_strdup(path));
    }
}

static void store_pending_mod_sub(struct bt_mesh_model *mod, bool vnd)
{
    u16_t groups[CONFIG_BT_MESH_MODEL_GROUP_COUNT];
    char path[20];
    int i, count, err;

    for (i = 0, count = 0; i < ARRAY_SIZE(mod->groups); i++) {
        if (mod->groups[i] != BT_MESH_ADDR_UNASSIGNED) {
            groups[count++] = mod->groups[i];
        }
    }

    encode_mod_path(mod, vnd, "sub", path, sizeof(path));

    if (count) {
        err = settings_save_one(path, groups, count * sizeof(groups[0]));
    } else {
        err = settings_delete(path);
    }

    if (err) {
        BT_ERR("Failed to store %s value", log_strdup(path));
    } else {
        BT_DBG("Stored %s value", log_strdup(path));
    }
}

static void store_pending_mod_pub(struct bt_mesh_model *mod, bool vnd)
{
    struct mod_pub_val pub;
    char path[20];
    int err;

    encode_mod_path(mod, vnd, "pub", path, sizeof(path));

    if (!mod->pub || mod->pub->addr == BT_MESH_ADDR_UNASSIGNED) {
        err = settings_delete(path);
    } else {
        pub.addr = mod->pub->addr;
        pub.key = mod->pub->key;
        pub.ttl = mod->pub->ttl;
        pub.retransmit = mod->pub->retransmit;
        pub.period = mod->pub->period;
        pub.period_div = mod->pub->period_div;
        pub.cred = mod->pub->cred;

        err = settings_save_one(path, &pub, sizeof(pub));
    }

    if (err) {
        BT_ERR("Failed to store %s value", log_strdup(path));
    } else {
        BT_DBG("Stored %s value", log_strdup(path));
    }
}

static void store_pending_mod(struct bt_mesh_model *mod, struct bt_mesh_elem *elem, bool vnd, bool primary,
                              void *user_data)
{
    if (!mod->flags) {
        return;
    }

    if (mod->flags & BT_MESH_MOD_BIND_PENDING) {
        mod->flags &= ~BT_MESH_MOD_BIND_PENDING;
        store_pending_mod_bind(mod, vnd);
    }

    if (mod->flags & BT_MESH_MOD_SUB_PENDING) {
        mod->flags &= ~BT_MESH_MOD_SUB_PENDING;
        store_pending_mod_sub(mod, vnd);
    }

    if (mod->flags & BT_MESH_MOD_PUB_PENDING) {
        mod->flags &= ~BT_MESH_MOD_PUB_PENDING;
        store_pending_mod_pub(mod, vnd);
    }
}

#ifdef CONFIG_BT_MESH_PROVISIONER
static void clear_prov_nodes(int node_index)
{
    char path[20] = { 0 };
    snprintk(path, sizeof(path), "bt/mesh/Node/%d", node_index);
    settings_delete(path);
}

static void store_pending_prov_nodes(void)
{
    struct bt_mesh_node_t *node = NULL;
    char path[20] = { 0 };
    int i;

    int node_count = provisioner_get_prov_node_count();

    for (i = 0; i < node_count; i++) {
        node = bt_mesh_provisioner_get_node_info_by_id(i);

        if (node && node->flag == MESH_NODE_FLAG_STORE) {
            snprintk(path, sizeof(path), "bt/mesh/Node/%d", i);
            settings_save_one(path, node, sizeof(*node));
        }
    }
}
#endif

static void store_pending(struct k_work *work)
{
    BT_DBG("");

    if (atomic_test_and_clear_bit(bt_mesh.flags, BT_MESH_RPL_PENDING)) {
        if (atomic_test_bit(bt_mesh.flags, BT_MESH_VALID)) {
            store_pending_rpl();
        } else {
            clear_rpl();
        }
    }

    if (atomic_test_and_clear_bit(bt_mesh.flags, BT_MESH_KEYS_PENDING)) {
        store_pending_keys();
    }

    if (atomic_test_and_clear_bit(bt_mesh.flags, BT_MESH_NET_PENDING)) {
        if (atomic_test_bit(bt_mesh.flags, BT_MESH_VALID)) {
            store_pending_net();
        } else {
            clear_net();
        }
    }

    if (atomic_test_and_clear_bit(bt_mesh.flags, BT_MESH_IV_PENDING)) {
        if (atomic_test_bit(bt_mesh.flags, BT_MESH_VALID)) {
            store_pending_iv();
        } else {
            clear_iv();
        }
    }

    if (atomic_test_and_clear_bit(bt_mesh.flags, BT_MESH_SEQ_PENDING)) {
        store_pending_seq();
    }

    if (atomic_test_and_clear_bit(bt_mesh.flags, BT_MESH_HB_PUB_PENDING)) {
        store_pending_hb_pub();
    }

    if (atomic_test_and_clear_bit(bt_mesh.flags, BT_MESH_CFG_PENDING)) {
        if (atomic_test_bit(bt_mesh.flags, BT_MESH_VALID)) {
            store_pending_cfg();
        } else {
            clear_cfg();
        }
    }

    if (atomic_test_and_clear_bit(bt_mesh.flags, BT_MESH_MOD_PENDING)) {
        bt_mesh_model_foreach(store_pending_mod, NULL);
    }

#ifdef CONFIG_BT_MESH_PROVISIONER

    if (atomic_test_and_clear_bit(bt_mesh.flags, BT_MESH_PROV_NODE_PENDING)) {
        store_pending_prov_nodes();
    }

#endif
}

void bt_mesh_store_rpl(struct bt_mesh_rpl *entry)
{
    entry->store = true;
    if ((entry->seq) % CONFIG_BT_MESH_RPL_STORE_RATE == 0) {
        schedule_store(BT_MESH_RPL_PENDING);
    }
}

void bt_mesh_clear_node_rpl(uint16_t addr)
{
    char path[18];
    BT_DBG("");
    snprintk(path, sizeof(path), "bt/mesh/RPL/%x", addr);
    settings_delete(path);
    return;
}

void bt_mesh_clear_all_node_rpl()
{
    int i;
    BT_DBG("");

    for (i = 0; i < ARRAY_SIZE(bt_mesh.rpl); i++) {
        struct bt_mesh_rpl *rpl = &bt_mesh.rpl[i];
        char path[18];

        if (!rpl->src) {
            continue;
        }

        snprintk(path, sizeof(path), "bt/mesh/RPL/%x", rpl->src);
        settings_delete(path);
    }
}

static struct key_update *key_update_find(bool app_key, u16_t key_idx, struct key_update **free_slot, bool p_key)
{
    struct key_update *match;
    int i;

    match = NULL;
    *free_slot = NULL;

    for (i = 0; i < ARRAY_SIZE(key_updates); i++) {
        struct key_update *update = &key_updates[i];

        if (!update->valid) {
            *free_slot = update;
            continue;
        }

        if (update->app_key != app_key) {
            continue;
        }

        if (update->p_key != p_key) {
            continue;
        }

        if (update->key_idx == key_idx) {
            match = update;
        }
    }

    return match;
}

void bt_mesh_store_subnet(struct bt_mesh_subnet *sub, bool p_key)
{
    struct key_update *update, *free_slot;

    BT_DBG("NetKeyIndex 0x%03x", sub->net_idx);

    update = key_update_find(false, sub->net_idx, &free_slot, p_key);

    if (update) {
        update->clear = 0;
        schedule_store(BT_MESH_KEYS_PENDING);
        return;
    }

    if (!free_slot) {
        store_net_key(sub, p_key);
        return;
    }

    free_slot->valid = 1;
    free_slot->key_idx = sub->net_idx;
    free_slot->app_key = 0;
    free_slot->clear = 0;
    free_slot->p_key = p_key;

    schedule_store(BT_MESH_KEYS_PENDING);
}

void bt_mesh_store_app_key(struct bt_mesh_app_key *key, bool p_key)
{
    struct key_update *update, *free_slot;

    BT_DBG("AppKeyIndex 0x%03x", key->app_idx);

    update = key_update_find(true, key->app_idx, &free_slot, p_key);

    if (update) {
        update->clear = 0;
        schedule_store(BT_MESH_KEYS_PENDING);
        return;
    }

    if (!free_slot) {
        store_app_key(key, p_key);
        return;
    }

    free_slot->valid = 1;
    free_slot->key_idx = key->app_idx;
    free_slot->app_key = 1;
    free_slot->clear = 0;
    free_slot->p_key = p_key;

    schedule_store(BT_MESH_KEYS_PENDING);
}

void bt_mesh_store_hb_pub(void)
{
    schedule_store(BT_MESH_HB_PUB_PENDING);
}

void bt_mesh_store_cfg(void)
{
    schedule_store(BT_MESH_CFG_PENDING);
}

void bt_mesh_clear_net(void)
{
    schedule_store(BT_MESH_NET_PENDING);
    schedule_store(BT_MESH_IV_PENDING);
    schedule_store(BT_MESH_CFG_PENDING);
}

void bt_mesh_clear_subnet(struct bt_mesh_subnet *sub, bool p_key)
{
    struct key_update *update, *free_slot;

    BT_DBG("NetKeyIndex 0x%03x", sub->net_idx);
    update = key_update_find(false, sub->net_idx, &free_slot, p_key);

    if (update) {
        update->clear = 1;
        schedule_store(BT_MESH_KEYS_PENDING);
        return;
    }

    if (!free_slot) {
        clear_net_key(sub->net_idx, p_key);
        return;
    }

    free_slot->valid = 1;
    free_slot->key_idx = sub->net_idx;
    free_slot->app_key = 0;
    free_slot->clear = 1;
    free_slot->p_key = p_key;

    schedule_store(BT_MESH_KEYS_PENDING);
}

void bt_mesh_clear_app_key(struct bt_mesh_app_key *key, bool p_key)
{
    struct key_update *update, *free_slot;

    BT_DBG("AppKeyIndex 0x%03x", key->app_idx);
    update = key_update_find(true, key->app_idx, &free_slot, p_key);

    if (update) {
        update->clear = 1;
        schedule_store(BT_MESH_KEYS_PENDING);
        return;
    }

    if (!free_slot) {
        clear_app_key(key->app_idx, p_key);
        return;
    }

    free_slot->valid = 1;
    free_slot->key_idx = key->app_idx;
    free_slot->app_key = 1;
    free_slot->clear = 1;
    free_slot->p_key = p_key;

    schedule_store(BT_MESH_KEYS_PENDING);
}

void bt_mesh_clear_rpl(void)
{
    schedule_store(BT_MESH_RPL_PENDING);
}

void bt_mesh_store_mod_bind(struct bt_mesh_model *mod)
{
    mod->flags |= BT_MESH_MOD_BIND_PENDING;
    schedule_store(BT_MESH_MOD_PENDING);
}

void bt_mesh_store_mod_sub(struct bt_mesh_model *mod)
{
    mod->flags |= BT_MESH_MOD_SUB_PENDING;
    schedule_store(BT_MESH_MOD_PENDING);
}

void bt_mesh_store_mod_pub(struct bt_mesh_model *mod)
{
    mod->flags |= BT_MESH_MOD_PUB_PENDING;
    schedule_store(BT_MESH_MOD_PENDING);
}

static void store_pending_mod_check(struct bt_mesh_model *mod, struct bt_mesh_elem *elem, bool vnd, bool primary,
                                    void *user_data)
{
    uint8_t *is_pending = user_data;

    if (!mod->flags) {
        return;
    }

    if (mod->flags & BT_MESH_MOD_BIND_PENDING) {
        *is_pending |= BT_MESH_MOD_BIND_PENDING;
    }

    if (mod->flags & BT_MESH_MOD_SUB_PENDING) {
        *is_pending |= BT_MESH_MOD_SUB_PENDING;
    }

    if (mod->flags & BT_MESH_MOD_PUB_PENDING) {
        *is_pending |= BT_MESH_MOD_PUB_PENDING;
    }

    return;
}

int bt_mesh_store_mod_pending_check()
{
    uint8_t is_pending = 0;
    bt_mesh_model_foreach(store_pending_mod_check, &is_pending);

    return is_pending;
}

#ifdef CONFIG_BT_MESH_PROVISIONER
void bt_mesh_clear_mesh_node(int node_index)
{
    clear_prov_nodes(node_index);
}

void bt_mesh_store_mesh_node(int node_index)
{
    bt_mesh_provisioner_get_node_info_by_id(node_index)->flag = MESH_NODE_FLAG_STORE;
    schedule_store(BT_MESH_PROV_NODE_PENDING);
}
#endif

void bt_mesh_settings_init(void)
{
    k_delayed_work_init(&pending_store, store_pending);
    SETTINGS_HANDLER_DEFINE(bt_mesh, "bt/mesh", NULL, mesh_set, mesh_commit, NULL);
}

#endif
