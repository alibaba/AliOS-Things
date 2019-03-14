/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <aos/kernel.h>

#include <misc/printk.h>
#include <misc/byteorder.h>

#include <api/mesh.h>

#ifndef CONFIG_MESH_STACK_ALONE
#include <tinycrypt/sha256.h>
#include <tinycrypt/constants.h>
#endif

#include <aos/hal/gpio.h>

#include "tmall_model.h"
#include "tmall_config.h"

#define PID_STRING_LEN 4
#define DEVICE_NAME_LEN 6
#define STATIC_VALUE_LEN 100

#ifdef CONFIG_PRINT_HEAP
static void print_heap()
{
    extern k_mm_head *g_kmm_head;
    int               free = g_kmm_head->free_size;
    LOG("============free heap size =%d==========", free);
}
#endif

static struct bt_mesh_prov prov;
static struct bt_mesh_comp comp;
static struct bt_mesh_provisioner provisioner;

static struct bt_mesh_model_pub gen_level_pub;
static struct bt_mesh_model_pub lightness_srv_pub;
static struct bt_mesh_model_pub lightness_setup_srv_pub;

static gpio_dev_t gpio_led1;
#ifndef CONFIG_MESH_STACK_ALONE
static struct tc_sha256_state_struct sha256_ctx;
#endif

static char static_value[STATIC_VALUE_LEN] = { 0x00 }; // pid + ',' + mac + ',' + secret

struct onoff_state {
    u8_t current;
    u8_t previous;
};
static struct onoff_state g_onoff_state;

static struct bt_mesh_model_pub health_pub = {
    .msg = BT_MESH_HEALTH_FAULT_MSG(0),
};

static struct bt_mesh_model_pub gen_onoff_pub = {
    .msg = NET_BUF_SIMPLE(2 + 2),
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
#ifdef CONFIG_BT_MESH_FRIEND
    .frnd = BT_MESH_FRIEND_NOT_SUPPORTED,
#else
    .frnd = BT_MESH_FRIEND_NOT_SUPPORTED,
#endif
#ifdef CONFIG_BT_MESH_GATT_PROXY
    .gatt_proxy = BT_MESH_GATT_PROXY_ENABLED,
#else
    .gatt_proxy = BT_MESH_GATT_PROXY_NOT_SUPPORTED,
#endif
    .default_ttl = 7,

    /* 3 transmissions with 20ms interval */
    .net_transmit = BT_MESH_TRANSMIT(2, 20),
    .relay_retransmit = BT_MESH_TRANSMIT(2, 20),
};

static void gen_onoff_get(struct bt_mesh_model *model,
                          struct bt_mesh_msg_ctx *ctx,
                          struct net_buf_simple *buf)
{
    struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 1 + 4);

    printk("onoff get: addr 0x%04x onoff 0x%02x\n",
           model->elem->addr, g_onoff_state.current);
    bt_mesh_model_msg_init(&msg, BT_MESH_MODEL_OP_2(0x82, 0x04));
    net_buf_simple_add_u8(&msg, g_onoff_state.current);

    if (bt_mesh_model_send(model, ctx, &msg, NULL, NULL)) {
        printk("Unable to send On Off Status response\n");
    }
}

static void gen_onoff_set_unack(struct bt_mesh_model *model,
                                struct bt_mesh_msg_ctx *ctx,
                                struct net_buf_simple *buf)
{
    struct net_buf_simple *msg = model->pub->msg;
    int err;

    g_onoff_state.current = net_buf_simple_pull_u8(buf);
    printk("addr 0x%02x state 0x%02x\n",
           model->elem->addr, g_onoff_state.current);

    // turn on/off LED on the nrf52832-pca10040 board
    if (g_onoff_state.current) {
       hal_gpio_output_low(&gpio_led1);
    } else {
       hal_gpio_output_high(&gpio_led1);
    }

    /*
     * If a server has a publish address, it is required to
     * publish status on a state change
     *
     * See Mesh Profile Specification 3.7.6.1.2
     *
     * Only publish if there is an assigned address
     */
    if (g_onoff_state.previous != g_onoff_state.current &&
        model->pub->addr != BT_MESH_ADDR_UNASSIGNED) {
        printk("publish last 0x%02x cur 0x%02x\n",
                g_onoff_state.previous, g_onoff_state.current);
        g_onoff_state.previous = g_onoff_state.current;
        bt_mesh_model_msg_init(msg, BT_MESH_MODEL_OP_2(0x82, 0x04));
        net_buf_simple_add_u8(msg, g_onoff_state.current);
        err = bt_mesh_model_publish(model);
        if (err) {
            printk("bt_mesh_model_publish err %d\n", err);
        }
    }
}

static void gen_onoff_set(struct bt_mesh_model *model,
                          struct bt_mesh_msg_ctx *ctx,
                          struct net_buf_simple *buf)
{
    printk("gen_onoff_set\n");

    gen_onoff_set_unack(model, ctx, buf);
    gen_onoff_get(model, ctx, buf);
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

void bt_mesh_profile_calculate_digest(const uint8_t *digest, const uint8_t *pid,
                                      const uint8_t *mac_addr, const char *secret)
{
    int ret;
    char pid_string[16] = "";
    char mac_addr_string[16] = "";

    // convert the hex byte stream to hex string
    hextostring(pid, pid_string, PID_STRING_LEN);
    printk("pid string: %s\n", pid_string);
    hextostring(mac_addr, mac_addr_string, DEVICE_NAME_LEN);
    printk("mac_addr_string: %s\n", mac_addr_string);

    strcat(static_value, pid_string);
    strcat(static_value, ",");
    strcat(static_value, mac_addr_string);
    strcat(static_value, ",");

    // generally we should ensure the secret lenght won't exceed the STATIC_VALUE_LEN
    if (strlen(secret) > (STATIC_VALUE_LEN - PID_STRING_LEN - DEVICE_NAME_LEN - 2)) {
        printk("secret length over size\n");
        return;
    } else {
        strcat(static_value, secret);
    }

    printk("static oob: %s\n", static_value);

#ifndef CONFIG_MESH_STACK_ALONE
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
#endif /* CONFIG_MESH_STACK_ALONE */

    return;
}

void bt_mesh_profile_construct_uuid(char *dev_uuid, const uint8_t *pid,
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

static void bt_mesh_ready(int err)
{
    int ret;

#ifdef CONFIG_PRINT_HEAP
    printk("After bt init\r\n");
    print_heap();
#endif

    if (err) {
        printk("Bluetooth init failed (err %d)\n", err);
        return;
    }

    ret = bt_mesh_init(&prov, &comp, &provisioner);
    if (ret) {
        printk("Initializing mesh failed (err %d)\n", ret);
        return;
    }

#ifdef CONFIG_PRINT_HEAP
    printk("After bt_mesh init\r\n");
    print_heap();
#endif

    bt_mesh_prov_enable(BT_MESH_PROV_GATT | BT_MESH_PROV_ADV);

    printk("Bluetooth Mesh initialized\n");

#ifdef CONFIG_PRINT_HEAP
    printk("After bt_mesh_prov_enable init\r\n");
    print_heap();
#endif
}

int bt_mesh_profile_start(void)
{
    int ret = 0;

#ifndef CONFIG_MESH_STACK_ALONE
    hci_driver_init();

#ifdef CONFIG_PRINT_HEAP
    printk("After hci driver init\r\n");
    print_heap();
#endif

    ret = bt_enable(bt_mesh_ready);
    if (ret) {
        printk("Bluetooth init failed (err %d)\n", ret);
    }
#else // CONFIG_MESH_STACK_ALONE
    bt_mesh_ready(0);
#endif

    return ret;
}

int bt_mesh_profile_prov_init(const uint8_t *dev_uuid,
                              const uint8_t *digest, size_t digest_len,
                              bt_mesh_profile_prov_complete_t prov_complete,
                              bt_mesh_profile_prov_reset_t prov_reset)
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

int bt_mesh_profile_composition_data_init(void)
{
    /* Node composition data used to configure a node while provisioning */
    comp.cid = CONFIG_CID_TAOBAO;
    comp.pid = 0;
    comp.vid = 1; // firmware version fir ota
    comp.elem = elements;
    comp.elem_count = ARRAY_SIZE(elements);

    return 0;
}

int bt_mesh_profile_model_init(void)
{
    /* add all supported elements and models from here
     * configure all light bulb related models for demo
     */

    // configure LED1 pin
    gpio_led1.port = CONFIG_BT_MESH_LED_PIN;

    return 0;
}

#if 0
void bt_mesh_profile_model_add()
{
}
void bt_mesh_profile_model_remove()
{
}
#endif
