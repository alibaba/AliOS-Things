/*  Bluetooth Mesh */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ble_os.h>
#include <stdbool.h>
#include <bt_errno.h>

#include <net/buf.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <api/mesh.h>

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_MESH_DEBUG)
#include "common/log.h"

#include "test.h"
#include "adv.h"
#include "prov.h"
#include "net.h"
#include "beacon.h"
#include "lpn.h"
#include "friend.h"
#include "ble_transport.h"
#include "access.h"
#include "foundation.h"
#include "proxy.h"
#include "settings.h"
#include "mesh.h"

#ifdef CONFIG_BT_MESH_PROVISIONER
#include "provisioner_prov.h"
#include "provisioner_main.h"
#include "provisioner_proxy.h"

static volatile bool provisioner_en;
#endif

#ifdef CONFIG_BT_MESH_EVENT_CALLBACK
#include "mesh_event_port.h"
#endif

static uint8_t mesh_init = 0;

int bt_mesh_provision(const u8_t net_key[16], u16_t net_idx, u8_t flags, bt_u32_t iv_index, u16_t addr,
                      const u8_t dev_key[16])
{
    bool pb_gatt_enabled;
    int err;

    BT_INFO("Primary Element: 0x%04x", addr);
    BT_DBG("net_idx 0x%04x flags 0x%02x iv_index 0x%04x", net_idx, flags, iv_index);

    if (atomic_test_and_set_bit(bt_mesh.flags, BT_MESH_VALID)) {
        return -EALREADY;
    }

    if (IS_ENABLED(CONFIG_BT_MESH_PB_GATT)) {
        if (bt_mesh_proxy_prov_disable(false) == 0) {
            pb_gatt_enabled = true;
        } else {
            pb_gatt_enabled = false;
        }
    } else {
        pb_gatt_enabled = false;
    }

    err = bt_mesh_net_create(net_idx, flags, net_key, iv_index);
    if (err) {
        atomic_clear_bit(bt_mesh.flags, BT_MESH_VALID);

        if (IS_ENABLED(CONFIG_BT_MESH_PB_GATT) && pb_gatt_enabled) {
            bt_mesh_proxy_prov_enable();
        }

        return err;
    }

#ifdef CONFIG_GENIE_MESH_ENABLE
    // if(bt_mesh_elem_count() == 1)
    // {
    // bt_mesh.seq = 0U;
    // }
    // else
    // {
    atomic_set_bit(bt_mesh.flags, BT_MESH_SEQ_PENDING);
    // }
    // printf("bt_mesh_provision seq:0x%x\n", bt_mesh.seq);
#else
    bt_mesh.seq = 0U;
#endif

    bt_mesh_comp_provision(addr);

    memcpy(bt_mesh.dev_key, dev_key, 16);

    if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
        BT_WARN("Storing network information persistently");
        bt_mesh_store_net();
        bt_mesh_store_subnet(&bt_mesh.sub[0], 0);
        bt_mesh_store_iv(false);
    }

    bt_mesh_net_start();

    return 0;
}

void bt_mesh_reset(void)
{
    if (!atomic_test_bit(bt_mesh.flags, BT_MESH_VALID)) {
        return;
    }

    bt_mesh.iv_index = 0U;

    memset(bt_mesh.flags, 0, sizeof(bt_mesh.flags));
#ifdef CONFIG_GENIE_MESH_ENABLE
    // if(bt_mesh_elem_count() == 1)
    // {
    //   bt_mesh.seq = 0U;
    // }
    // else
    // {
    atomic_set_bit(bt_mesh.flags, BT_MESH_SEQ_PENDING);
    // }
    // printf("bt_mesh_reset seq:0x%x\n", bt_mesh.seq);
#else
    bt_mesh.seq = 0U;
#endif

    k_delayed_work_cancel(&bt_mesh.ivu_timer);

    bt_mesh_cfg_reset();

    bt_mesh_rx_reset();
    bt_mesh_tx_reset();

    if (IS_ENABLED(CONFIG_BT_MESH_LOW_POWER)) {
        bt_mesh_lpn_disable(true);
    }

    if (IS_ENABLED(CONFIG_BT_MESH_FRIEND)) {
        bt_mesh_friend_clear_net_idx(BT_MESH_KEY_ANY);
    }

    if (IS_ENABLED(CONFIG_BT_MESH_GATT_PROXY)) {
        bt_mesh_proxy_gatt_disable();
    }

    if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
        bt_mesh_clear_net();
    }

    memset(bt_mesh.dev_key, 0, sizeof(bt_mesh.dev_key));

    bt_mesh_scan_disable();
    bt_mesh_beacon_disable();

    bt_mesh_comp_unprovision();

    if (IS_ENABLED(CONFIG_BT_MESH_PROV)) {
        bt_mesh_prov_reset();
    }

#ifdef CONFIG_BT_MESH_PROVISIONER
    provisioner_upper_reset_all_nodes();
#endif
}

bool bt_mesh_is_provisioned(void)
{
    return atomic_test_bit(bt_mesh.flags, BT_MESH_VALID);
}

int bt_mesh_prov_enable(bt_mesh_prov_bearer_t bearers)
{
    if (!mesh_init) {
        return -EINVAL;
    }

    if (bearers >= (BT_MESH_PROV_GATT << 1) || bearers == 0) {
        return -EINVAL;
    }

    if (bt_mesh_is_provisioned()) {
        return -EALREADY;
    }

    if (IS_ENABLED(CONFIG_BT_DEBUG)) {
        const struct bt_mesh_prov *prov = bt_mesh_prov_get();
        struct bt_uuid_128 uuid = { .uuid.type = BT_UUID_TYPE_128 };

        memcpy(uuid.val, prov->uuid, 16);
        BT_INFO("Device UUID: %s", bt_uuid_str(&uuid.uuid));
    }

    if (IS_ENABLED(CONFIG_BT_MESH_PB_ADV) && (bearers & BT_MESH_PROV_ADV)) {
        /* Make sure we're scanning for provisioning inviations */
        bt_mesh_scan_enable();
        /* Enable unprovisioned beacon sending */
        bt_mesh_beacon_enable();
    }

    if (IS_ENABLED(CONFIG_BT_MESH_PB_GATT) && (bearers & BT_MESH_PROV_GATT)) {
        bt_mesh_proxy_prov_enable();
        bt_mesh_adv_update();
    }

    return 0;
}

int bt_mesh_prov_disable(bt_mesh_prov_bearer_t bearers)
{
    if (!mesh_init) {
        return -EINVAL;
    }

    if (bearers >= (BT_MESH_PROV_GATT << 1) || bearers == 0) {
        return -EINVAL;
    }

    if (bt_mesh_is_provisioned()) {
        return -EALREADY;
    }

    if (IS_ENABLED(CONFIG_BT_MESH_PB_ADV) && (bearers & BT_MESH_PROV_ADV)) {
        bt_mesh_beacon_disable();
        bt_mesh_scan_disable();
    }

    if (IS_ENABLED(CONFIG_BT_MESH_PB_GATT) && (bearers & BT_MESH_PROV_GATT)) {
        bt_mesh_proxy_prov_disable(true);
    }

    return 0;
}

static void model_suspend(struct bt_mesh_model *mod, struct bt_mesh_elem *elem, bool vnd, bool primary, void *user_data)
{
    if (mod->pub && mod->pub->update) {
        mod->pub->count = 0U;
        k_delayed_work_cancel(&mod->pub->timer);
    }
}

int bt_mesh_suspend(bool force)
{
    int err;

    if (!atomic_test_bit(bt_mesh.flags, BT_MESH_VALID)) {
        return -EINVAL;
    }

    if (atomic_test_bit(bt_mesh.flags, BT_MESH_SUSPENDED)) {
        return -EALREADY;
    }

#ifdef CONFIG_GENIE_MESH_ENABLE
    extern bool bt_mesh_net_is_rx(void);
    extern bool genie_transport_tx_in_progress(void);
    if (force == false) {
        if (!bt_mesh_is_provisioned()) {
            BT_INFO("unprov stat no suspend");
            return -1;
        }

        if (bt_mesh_net_is_rx()) {
            BT_INFO("rx stat no suspend");
            return -2;
        }

        if (genie_transport_tx_in_progress()) {
            BT_INFO("tx stat no suspend");
            return -3;
        }
    }
#endif

    err = bt_mesh_scan_disable();
    if (err) {
        BT_WARN("Disabling scanning failed (err %d)", err);
        return err;
    }

    bt_mesh_hb_pub_disable();

    if (bt_mesh_beacon_get() == BT_MESH_BEACON_ENABLED) {
        bt_mesh_beacon_disable();
    }

    bt_mesh_model_foreach(model_suspend, NULL);

    atomic_set_bit(bt_mesh.flags, BT_MESH_SUSPENDED);

    return 0;
}

static void model_resume(struct bt_mesh_model *mod, struct bt_mesh_elem *elem, bool vnd, bool primary, void *user_data)
{
    if (mod->pub && mod->pub->update) {
        bt_s32_t period_ms = bt_mesh_model_pub_period_get(mod);

        if (period_ms) {
            k_delayed_work_submit(&mod->pub->timer, period_ms);
        }
    }
}

int bt_mesh_resume(void)
{
    int err;

    if (!atomic_test_bit(bt_mesh.flags, BT_MESH_VALID)) {
        return -EINVAL;
    }

    if (!atomic_test_bit(bt_mesh.flags, BT_MESH_SUSPENDED)) {
        return -EALREADY;
    }

    err = bt_mesh_scan_enable();
    if (err) {
        BT_WARN("Re-enabling scanning failed (err %d)", err);
        return err;
    }

    if (bt_mesh_beacon_get() == BT_MESH_BEACON_ENABLED) {
        bt_mesh_beacon_enable();
    }

    bt_mesh_model_foreach(model_resume, NULL);

    atomic_clear_bit(bt_mesh.flags, BT_MESH_SUSPENDED);

    return err;
}

#ifdef CONFIG_BT_MESH_PROVISIONER

bool bt_mesh_is_provisioner_en(void)
{
    return provisioner_en;
}

int bt_mesh_provisioner_enable(bt_mesh_prov_bearer_t bearers)
{
    int err;

    if (bearers >= (BT_MESH_PROV_GATT << 1) || bearers == 0) {
        return -EINVAL;
    }

    if (bt_mesh_is_provisioner_en()) {
        BT_ERR("Provisioner already enabled");
        return -EALREADY;
    }

    err = provisioner_upper_init();
    if (err) {
        BT_ERR("%s: provisioner_upper_init fail", __func__);
        return err;
    }

    if ((IS_ENABLED(CONFIG_BT_MESH_PB_ADV) && (bearers & BT_MESH_PROV_ADV)) ||
        (IS_ENABLED(CONFIG_BT_MESH_PB_GATT) && (bearers & BT_MESH_PROV_GATT))) {
        bt_mesh_scan_enable();
    }

    if (IS_ENABLED(CONFIG_BT_MESH_PB_GATT) && (bearers & BT_MESH_PROV_GATT)) {
        provisioner_pb_gatt_enable();
    }

    provisioner_en = true;

    return 0;
}

int bt_mesh_provisioner_disable(bt_mesh_prov_bearer_t bearers)
{
    if (bearers >= (BT_MESH_PROV_GATT << 1) || bearers == 0) {
        return -EINVAL;
    }

    if (!bt_mesh_is_provisioner_en()) {
        BT_ERR("Provisioner already disabled");
        return -EALREADY;
    }

    if (IS_ENABLED(CONFIG_BT_MESH_PB_GATT) && (bearers & BT_MESH_PROV_GATT)) {
        provisioner_pb_gatt_disable();
    }

    if ((IS_ENABLED(CONFIG_BT_MESH_PB_ADV) && (bearers & BT_MESH_PROV_ADV)) &&
        (IS_ENABLED(CONFIG_BT_MESH_PB_GATT) && (bearers & BT_MESH_PROV_GATT))) {
        bt_mesh_scan_disable();
    }

    provisioner_en = false;

    return 0;
}

#endif /* CONFIG_BT_MESH_PROVISIONER */

int bt_mesh_is_init()
{
    return mesh_init;
}

int bt_mesh_vnd_adv_set_cb(void (*cb)(const struct adv_addr_t *addr, s8_t rssi, u8_t adv_type, void *user_data,
                                      uint16_t len))
{
    return bt_mesh_adv_vnd_scan_register((vendor_beacon_cb)cb);
}

int bt_mesh_init(const struct bt_mesh_prov *prov, const struct bt_mesh_comp *comp,
                 const struct bt_mesh_provisioner *provisioner)
{
    int err;
    BT_WARN(" ble_mesh log is enable \r\n");

    if (mesh_init) {
        return -EALREADY;
    }

    if (prov == NULL || comp == NULL) {
        return -EINVAL;
    }

    if (prov->uuid == NULL) {
        return -EINVAL;
    }

    if (prov->oob_info >= (BT_MESH_PROV_OOB_ON_DEV << 1)) {
        return -EINVAL;
    }

    if (prov->output_actions >= (BT_MESH_DISPLAY_STRING << 1)) {
        return -EINVAL;
    }

    if (prov->input_actions >= (BT_MESH_ENTER_STRING << 1)) {
        return -EINVAL;
    }

//      bt_enable is aleady called before
//     printf("before bt_enable in %s\n", __func__);
//     err = bt_enable(NULL);
//     if (err && err != -EALREADY) {
//         return err;
//     }

    printf("before bt_mesh_test in %s\n", __func__);
    err = bt_mesh_test();
    if (err) {
        return err;
    }

    printf("before bt_mesh_comp_register in %s\n", __func__);
    err = bt_mesh_comp_register(comp);
    if (err) {
        return err;
    }

    printf("before bt_mesh_prov_init in %s\n", __func__);
    if (IS_ENABLED(CONFIG_BT_MESH_PROV)) {
        err = bt_mesh_prov_init(prov);
        if (err) {
            printf(" bt_mesh_prov_init err:%d\n", err);
            return err;
        }
    }

#ifdef CONFIG_BT_MESH_PROVISIONER
    printf("before provisioner_prov_init in %s\n", __func__);
    err = provisioner_prov_init(provisioner);
    if (err) {
        return err;
    }
#endif

    bt_mesh_net_init();
    bt_mesh_trans_init();
    bt_mesh_beacon_init();
    bt_mesh_adv_init();

    if (IS_ENABLED(CONFIG_BT_MESH_GATT_PROXY)) {
#ifdef CONFIG_BT_MESH_PROVISIONER
        provisioner_proxy_init();
#endif
        bt_mesh_proxy_init();
    }

    if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
        bt_mesh_settings_init();
        extern int settings_load();
        settings_load();
    }

    mesh_init = 1;

    return 0;
}
