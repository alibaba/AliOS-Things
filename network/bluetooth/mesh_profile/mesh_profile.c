/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <aos/aos.h>

#include <misc/printk.h>
#include <misc/byteorder.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/mesh.h>
#include <tinycrypt/sha256.h>
#include <tinycrypt/constants.h>

#include "mesh_profile.h"
#include "mesh_profile_config.h"

static struct bt_mesh_prov prov;
static struct bt_mesh_comp comp;

static struct bt_mesh_model_pub gen_level_pub;
static struct bt_mesh_model_pub gen_onoff_pub;
static struct bt_mesh_model_pub lightness_srv_pub;
static struct bt_mesh_model_pub lightness_setup_srv_pub;

static struct tc_sha256_state_struct sha256_ctx;

static char static_value [100] = { 0x00 }; // pid + ',' + mac + ',' + secret

static struct bt_mesh_model_pub health_pub = {
    .msg = BT_MESH_HEALTH_FAULT_MSG(0),
};

static void attention_on(struct bt_mesh_model *model)
{
    printk("attention_on()\n");
}

static void attention_off(struct bt_mesh_model *model)
{
    printk("attention_off()\n");
}

static const struct bt_mesh_health_srv_cb health_srv_cb = {
    .attn_on = attention_on,
    .attn_off = attention_off,
};

static struct bt_mesh_health_srv health_srv = {
    .cb = &health_srv_cb,
};

static struct bt_mesh_cfg_srv cfg_srv = {
    .relay = BT_MESH_RELAY_ENABLED,
    .beacon = BT_MESH_BEACON_ENABLED,
    .frnd = BT_MESH_FRIEND_NOT_SUPPORTED,
    .gatt_proxy = BT_MESH_GATT_PROXY_ENABLED,
    .default_ttl = 7,

    /* 3 transmissions with 20ms interval */
    .net_transmit = BT_MESH_TRANSMIT(2, 20),
    .relay_retransmit = BT_MESH_TRANSMIT(2, 20),
};

static void gen_onoff_get(struct bt_mesh_model *model,
                          struct bt_mesh_msg_ctx *ctx,
                          struct net_buf_simple *buf)
{
}

static void gen_onoff_set(struct bt_mesh_model *model,
                          struct bt_mesh_msg_ctx *ctx,
                          struct net_buf_simple *buf)
{
}

static void gen_onoff_set_unack(struct bt_mesh_model *model,
                                struct bt_mesh_msg_ctx *ctx,
                                struct net_buf_simple *buf)
{
}

static const struct bt_mesh_model_op gen_onoff_op[] = {
    { BT_MESH_MODEL_OP_2(0x82, 0x01), 0, gen_onoff_get },
    { BT_MESH_MODEL_OP_2(0x82, 0x02), 2, gen_onoff_set },
    { BT_MESH_MODEL_OP_2(0x82, 0x03), 2, gen_onoff_set_unack },
    BT_MESH_MODEL_OP_END,
};

static void gen_level_get(struct bt_mesh_model *model,
                          struct bt_mesh_msg_ctx *ctx,
                          struct net_buf_simple *buf)
{
}

static void gen_level_set(struct bt_mesh_model *model,
                          struct bt_mesh_msg_ctx *ctx,
                          struct net_buf_simple *buf)
{
}

static void gen_level_set_unack(struct bt_mesh_model *model,
                                struct bt_mesh_msg_ctx *ctx,
                                struct net_buf_simple *buf)
{
}

static void gen_delta_set(struct bt_mesh_model *model,
                          struct bt_mesh_msg_ctx *ctx,
                          struct net_buf_simple *buf)
{
}

static void gen_delta_set_unack(struct bt_mesh_model *model,
                                struct bt_mesh_msg_ctx *ctx,
                                struct net_buf_simple *buf)
{
}

static void gen_move_set(struct bt_mesh_model *model,
                         struct bt_mesh_msg_ctx *ctx,
                         struct net_buf_simple *buf)
{
}

static void gen_move_set_unack(struct bt_mesh_model *model,
                               struct bt_mesh_msg_ctx *ctx,
                               struct net_buf_simple *buf)
{
}

static const struct bt_mesh_model_op gen_level_op[] = {
    { BT_MESH_MODEL_OP_2(0x82, 0x05), 0, gen_level_get },
    { BT_MESH_MODEL_OP_2(0x82, 0x06), 3, gen_level_set },
    { BT_MESH_MODEL_OP_2(0x82, 0x07), 3, gen_level_set_unack },
    { BT_MESH_MODEL_OP_2(0x82, 0x09), 5, gen_delta_set },
    { BT_MESH_MODEL_OP_2(0x82, 0x0a), 5, gen_delta_set_unack },
    { BT_MESH_MODEL_OP_2(0x82, 0x0b), 3, gen_move_set },
    { BT_MESH_MODEL_OP_2(0x82, 0x0c), 3, gen_move_set_unack },
    BT_MESH_MODEL_OP_END,
};

static void lightness_get(struct bt_mesh_model *model,
                          struct bt_mesh_msg_ctx *ctx,
                          struct net_buf_simple *buf)
{
}

static void lightness_set(struct bt_mesh_model *model,
                          struct bt_mesh_msg_ctx *ctx,
                          struct net_buf_simple *buf)
{
}

static void lightness_set_unack(struct bt_mesh_model *model,
                                struct bt_mesh_msg_ctx *ctx,
                                struct net_buf_simple *buf)
{
}

static void lightness_status(struct bt_mesh_model *model,
                             struct bt_mesh_msg_ctx *ctx,
                             struct net_buf_simple *buf)
{
}

static void lightness_linear_get(struct bt_mesh_model *model,
                                 struct bt_mesh_msg_ctx *ctx,
                                 struct net_buf_simple *buf)
{
}

static void lightness_linear_set(struct bt_mesh_model *model,
                                 struct bt_mesh_msg_ctx *ctx,
                                 struct net_buf_simple *buf)
{
}

static void lightness_linear_set_unack(struct bt_mesh_model *model,
                                       struct bt_mesh_msg_ctx *ctx,
                                       struct net_buf_simple *buf)
{
}

static void lightness_linear_status(struct bt_mesh_model *model,
                                    struct bt_mesh_msg_ctx *ctx,
                                    struct net_buf_simple *buf)
{
}

static void lightness_last_get(struct bt_mesh_model *model,
                               struct bt_mesh_msg_ctx *ctx,
                               struct net_buf_simple *buf)
{
}

static void lightness_last_status(struct bt_mesh_model *model,
                                  struct bt_mesh_msg_ctx *ctx,
                                  struct net_buf_simple *buf)
{
}

static void lightness_default_get(struct bt_mesh_model *model,
                                  struct bt_mesh_msg_ctx *ctx,
                                  struct net_buf_simple *buf)
{
}

static void lightness_default_status(struct bt_mesh_model *model,
                                     struct bt_mesh_msg_ctx *ctx,
                                     struct net_buf_simple *buf)
{
}

static void lightness_range_get(struct bt_mesh_model *model,
                                struct bt_mesh_msg_ctx *ctx,
                                struct net_buf_simple *buf)
{
}

static void lightness_range_status(struct bt_mesh_model *model,
                                   struct bt_mesh_msg_ctx *ctx,
                                   struct net_buf_simple *buf)
{
}

static const struct bt_mesh_model_op lightness_srv_op[] = {
    { BT_MESH_MODEL_OP_2(0x82, 0x4b), 2, lightness_get },
    { BT_MESH_MODEL_OP_2(0x82, 0x4c), 2, lightness_set },
    { BT_MESH_MODEL_OP_2(0x82, 0x4d), 2, lightness_set_unack },
    { BT_MESH_MODEL_OP_2(0x82, 0x4e), 2, lightness_status },
    { BT_MESH_MODEL_OP_2(0x82, 0x4f), 2, lightness_linear_get },
    { BT_MESH_MODEL_OP_2(0x82, 0x50), 2, lightness_linear_set },
    { BT_MESH_MODEL_OP_2(0x82, 0x51), 2, lightness_linear_set_unack },
    { BT_MESH_MODEL_OP_2(0x82, 0x52), 2, lightness_linear_status },
    { BT_MESH_MODEL_OP_2(0x82, 0x53), 2, lightness_last_get },
    { BT_MESH_MODEL_OP_2(0x82, 0x54), 2, lightness_last_status },
    { BT_MESH_MODEL_OP_2(0x82, 0x55), 2, lightness_default_get },
    { BT_MESH_MODEL_OP_2(0x82, 0x56), 2, lightness_default_status },
    { BT_MESH_MODEL_OP_2(0x82, 0x57), 2, lightness_range_get },
    { BT_MESH_MODEL_OP_2(0x82, 0x58), 2, lightness_range_status },
    BT_MESH_MODEL_OP_END,
};

static void lightness_setup_default_set(struct bt_mesh_model *model,
                                        struct bt_mesh_msg_ctx *ctx,
                                        struct net_buf_simple *buf)
{
}

static void lightness_setup_default_set_unack(struct bt_mesh_model *model,
                                              struct bt_mesh_msg_ctx *ctx,
                                              struct net_buf_simple *buf)
{
}

static void lightness_setup_range_set(struct bt_mesh_model *model,
                                      struct bt_mesh_msg_ctx *ctx,
                                      struct net_buf_simple *buf)
{
}

static void lightness_setup_range_set_unack(struct bt_mesh_model *model,
                                            struct bt_mesh_msg_ctx *ctx,
                                            struct net_buf_simple *buf)
{
}

static const struct bt_mesh_model_op lightness_setup_srv_op[] = {
    { BT_MESH_MODEL_OP_2(0x82, 0x59), 2, lightness_setup_default_set },
    { BT_MESH_MODEL_OP_2(0x82, 0x5a), 2, lightness_setup_default_set_unack },
    { BT_MESH_MODEL_OP_2(0x82, 0x5b), 2, lightness_setup_range_set },
    { BT_MESH_MODEL_OP_2(0x82, 0x5c), 2, lightness_setup_range_set_unack },
    BT_MESH_MODEL_OP_END,
};

static void hextostring(const uint8_t *source, char *dest, int len)
{
    int i;
    char tmp[3];

    for (i = 0; i < len; i++) {
        sprintf(tmp, "%02x", (unsigned char)source[i]);
        memcpy(&dest[i * 2], tmp, 2);
    }
    return;
}

void ble_mesh_calculate_digest(const uint8_t *digest, const uint8_t *pid,
                               const uint8_t *mac_addr, const char *secret)
{
    int ret;
    char pid_string[16] = "";
    char mac_addr_string[16] = "";

    // convert the byte stream to hex string
    hextostring(pid, pid_string, 4);
    printk("pid string: %s\n", pid_string);
    hextostring(mac_addr, mac_addr_string, 6);
    printk("mac_addr_string: %s\n", mac_addr_string);

    strcat(static_value, pid_string);
    strcat(static_value, ",");
    strcat(static_value, mac_addr_string);
    strcat(static_value, ",");
    strcat(static_value, secret);

    printk("static oob: %s\n", static_value);

    /* calculate the sha256 of oob info and
     * fetch the top 16 bytes as static value
     */
    ret = tc_sha256_init(&sha256_ctx);
    if (ret != TC_CRYPTO_SUCCESS) {
        printk("sha256 init fail\n");
    }

    ret = tc_sha256_update(&sha256_ctx, static_value, strlen(static_value));
    if (ret != TC_CRYPTO_SUCCESS) {
        printk("sha256 udpate fail\n");
    }

    ret = tc_sha256_final(digest, &sha256_ctx);
    if (ret != TC_CRYPTO_SUCCESS) {
        printk("sha256 final fail\n");
    }
#if 1
    else {
        printk("digest: \n");
        for (int i = 0; i < 32; i++) {
            printk("%02x:", digest[i]);
        }
        printk("\n");
    }
#endif
    return;
}

void ble_mesh_construct_uuid(char *dev_uuid, const uint8_t *pid,
                             const uint8_t *mac_addr)
{
    int i;

    // all fields in uuid should be in little-endian
    // CID: Taobao
    dev_uuid[0] = 0xa8;
    dev_uuid[1] = 0x01;

    // PID
    // Bit0~Bit3: 0001 (broadcast version)
    // Bit4：1 (one secret pre device)
    // Bit5: 1 (OTA support)
    // Bit6~Bit7: 01 (ble 4.0)
    dev_uuid[2] = 0x71;

    // Product ID
    for (i = 0; i < 4; i++) {
        dev_uuid[3 + i] = pid[3 - i];
    }

    // mac addr (device name)
    for (i = 0; i < 6; i++) {
        dev_uuid[7 + i] = mac_addr[5 - i];
    }

    return;
}

static void ble_mesh_ready(int err)
{
    int ret;

    if (err) {
        printk("Bluetooth init failed (err %d)\n", err);
        return;
    }

    ret = bt_mesh_init(&prov, &comp);
    if (ret) {
        printk("Initializing mesh failed (err %d)\n", ret);
        return;
    }

    bt_mesh_prov_enable(BT_MESH_PROV_GATT | BT_MESH_PROV_ADV);

    printk("Bluetooth Mesh initialized\n");
}

int ble_mesh_start(void)
{
    int ret = 0;

    hci_driver_init();

    ret = bt_enable(ble_mesh_ready);
    if (ret) {
        printk("Bluetooth init failed (err %d)\n", ret);
    }

    return ret;
}

int ble_mesh_prov_init(const uint8_t *dev_uuid,
                       const uint8_t *digest, size_t digest_len,
                       ble_mesh_prov_complete_t prov_complete,
                       ble_mesh_prov_reset_t prov_reset)
{
    prov.uuid = dev_uuid;
    prov.static_val = digest;
    prov.static_val_len = digest_len;
    prov.complete = prov_complete;
    prov.reset = prov_reset;

    return 0;
}

static struct bt_mesh_model root_models[CONFIG_MESH_MODEL_NUM] = {
    BT_MESH_MODEL_CFG_SRV(&cfg_srv),
    BT_MESH_MODEL_HEALTH_SRV(&health_srv, &health_pub),
    BT_MESH_MODEL(BT_MESH_MODEL_ID_GEN_ONOFF_SRV, gen_onoff_op,
    &gen_onoff_pub, NULL),
    BT_MESH_MODEL(BT_MESH_MODEL_ID_GEN_LEVEL_SRV, gen_level_op,
    &gen_level_pub, NULL),
    BT_MESH_MODEL(BT_MESH_MODEL_ID_LIGHT_LIGHTNESS_SRV, lightness_srv_op,
    &lightness_srv_pub, NULL),
    BT_MESH_MODEL(BT_MESH_MODEL_ID_LIGHT_LIGHTNESS_SETUP_SRV, lightness_setup_srv_op,
    &lightness_setup_srv_pub, NULL),
};

static struct bt_mesh_elem elements[CONFIG_MESH_ELEM_NUM] = {
    BT_MESH_ELEM(0, root_models, BT_MESH_MODEL_NONE),
};

int ble_mesh_composition_data_init(void)
{
    /* Node composition data used to configure a node while provisioning */
    comp.cid = CONFIG_CID_TAOBAO;
    comp.pid = 0;
    comp.vid = 1; // firmware version fir ota
    comp.elem = elements;
    comp.elem_count = ARRAY_SIZE(elements);

    return 0;
}

int ble_mesh_model_init(void)
{
    /* add all supported elements and models from here
     * configure all light bulb related models for demo
     */
    return 0;
}

#if 0
void ble_mesh_profile_model_add()
{
}
void ble_mesh_profile_model_remove()
{
}
#endif
