/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <aos/aos.h>
#include <aos/log.h>

#include <aos/mesh.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/mesh.h>
#include <tinycrypt/sha256.h>
#include <tinycrypt/constants.h>
#include "drv_gpio.h"


#include "ali_vendormodel_profile.h"
#include "ali_vendormodel_profile_config.h"

#define printk printf

#define PID_STRING_LEN 4
#define DEVICE_NAME_LEN 6
#define STATIC_VALUE_LEN 100

static struct bt_mesh_prov prov;
static struct bt_mesh_comp comp;

//static struct bt_mesh_model_pub gen_level_pub;
struct bt_mesh_model_pub lightness_srv_pub;
bt_mesh_opcode_cb_t proc_opcode;

//static gpio_pin_handle_t gpio_led1;
static struct tc_sha256_state_struct sha256_ctx;

static char static_value[STATIC_VALUE_LEN] = { 0x00 }; // pid + ',' + mac + ',' + secret

struct onoff_state {
    u8_t current;
    u8_t previous;
};
static struct onoff_state g_onoff_state;

static struct bt_mesh_model_pub health_pub = {
    .msg = BT_MESH_HEALTH_FAULT_MSG(0),
};

struct bt_mesh_model_pub gen_onoff_pub = {
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

    //printk("onoff get: addr 0x%04x onoff 0x%02x\n",
    //       model->elem->addr, g_onoff_state.current);
    bt_mesh_model_msg_init(msg, BT_MESH_MODEL_OP_2(0x82, 0x04));
    net_buf_simple_add_u8(msg, g_onoff_state.current);

    if (bt_mesh_model_send(model, ctx, msg, NULL, NULL)) {
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
    //printk("addr 0x%02x state 0x%02x\n",
    //       model->elem->addr, g_onoff_state.current);

    // turn on/off LED on the nrf52832-pca10040 board
    if (g_onoff_state.current) {
       //hal_gpio_output_low(&gpio_led1);
    } else {
       //hal_gpio_output_high(&gpio_led1);
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
    if (proc_opcode != NULL)
        proc_opcode(1,NULL,0);
}

const struct bt_mesh_model_op gen_onoff_op[] = {
    { BT_MESH_MODEL_OP_2(0x82, 0x01), 0, gen_onoff_get },
    { BT_MESH_MODEL_OP_2(0x82, 0x02), 2, gen_onoff_set },
    { BT_MESH_MODEL_OP_2(0x82, 0x03), 2, gen_onoff_set_unack },
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

const struct bt_mesh_model_op lightness_srv_op[] = {
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

static void vendor_model_get(struct bt_mesh_model *model,
                          struct bt_mesh_msg_ctx *ctx,
                          struct net_buf_simple *buf)
{
    struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 1 + 4);

    static u8_t tid = 0;

    //printk("vendor get: src_addr dst_addr 0x%04x\n",ctx->addr,model->elem->addr);
    bt_mesh_model_msg_init(msg, BT_MESH_MODEL_OP_3(VENDOR_OPCODE_STATUS_ACK,CONFIG_MESH_VENDOR_COMPANY_ID));
    if (proc_opcode != NULL)
        proc_opcode(0,buf->data,buf->len);

    if(tid >= 250) tid = 1;
    net_buf_simple_add_u8(msg, tid++);
    net_buf_simple_add_u8(msg, 0x00);
    net_buf_simple_add_u8(msg, 0x00);
    net_buf_simple_add_u8(msg, 129);

    if (bt_mesh_model_send(model, ctx, msg, NULL, NULL)) {
        printk("Unable to send On Off Status response\n");
    }
}

static void vendor_model_set_ack(struct bt_mesh_model *model,
                                 struct bt_mesh_msg_ctx *ctx,
                                 struct net_buf_simple *buf)
{
    struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 1 + 4);

    //printk("vendor get: src_addr dst_addr 0x%04x\n",ctx->addr,model->elem->addr);
    bt_mesh_model_msg_init(msg, BT_MESH_MODEL_OP_3(VENDOR_OPCODE_STATUS_ACK,CONFIG_MESH_VENDOR_COMPANY_ID));
    if (proc_opcode != NULL)
        proc_opcode(0,buf->data,buf->len);
    net_buf_simple_add_u8(msg, 0x12);
    net_buf_simple_add_u8(msg, 0x00);
    net_buf_simple_add_u8(msg, 0x00);
    net_buf_simple_add_u8(msg, 129);

    if (bt_mesh_model_send(model, ctx, msg, NULL, NULL)) {
        printk("Unable to send On Off Status response\n");
    }
}

static void vendor_model_set_unack(struct bt_mesh_model *model,
                                   struct bt_mesh_msg_ctx *ctx,
                                   struct net_buf_simple *buf)
{
    //struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 1 + 4);

    //printk("vendor get: src_addr dst_addr 0x%04x\n",ctx->addr,model->elem->addr);
    if (proc_opcode != NULL)
        proc_opcode(0,buf->data,buf->len);
}

static void vendor_model_indicate(struct bt_mesh_model *model,
                                  struct bt_mesh_msg_ctx *ctx,
                                  struct net_buf_simple *buf)
{
    struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 1 + 4);

    //printk("vendor indicate: addr 0x%04x\n", model->elem->addr);
    bt_mesh_model_msg_init(msg, BT_MESH_MODEL_OP_3(CONFIG_MESH_VENDOR_COMPANY_ID, VENDOR_OPCODE_CONFIME));
    if (proc_opcode != NULL)
        proc_opcode(0,msg->data,msg->len);
}

static const struct bt_mesh_model_op vendor_model_op[] = {
    { BT_MESH_MODEL_OP_3(VENDOR_OPCODE_GET_STATUS, CONFIG_MESH_VENDOR_COMPANY_ID), 0, vendor_model_get },
    { BT_MESH_MODEL_OP_3(VENDOR_OPCODE_SET_ACK, CONFIG_MESH_VENDOR_COMPANY_ID), 2, vendor_model_set_ack },
    { BT_MESH_MODEL_OP_3(VENDOR_OPCODE_SET_UNACK, CONFIG_MESH_VENDOR_COMPANY_ID), 2, vendor_model_set_unack },
    //{ BT_MESH_MODEL_OP_3(VENDOR_OPCODE_STATUS_ACK, CONFIG_MESH_VENDOR_COMPANY_ID), 2, vendor_model_status_ack },
    { BT_MESH_MODEL_OP_3(VENDOR_OPCODE_INDICATE, CONFIG_MESH_VENDOR_COMPANY_ID), 2, vendor_model_indicate },
    //{ BT_MESH_MODEL_OP_3(VENDOR_OPCODE_CONFIME, CONFIG_MESH_VENDOR_COMPANY_ID), 2, vendor_model_set_ack },
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

    /* calculate the sha256 of oob info and
     * fetch the top 16 bytes as static value
     */
    ret = tc_sha256_init(&sha256_ctx);
    if (ret != TC_CRYPTO_SUCCESS) {
        printk("sha256 init fail\n");
    }

    ret = tc_sha256_update(&sha256_ctx, (const uint8_t *)static_value, strlen(static_value));
    if (ret != TC_CRYPTO_SUCCESS) {
        printk("sha256 udpate fail\n");
    }

    ret = tc_sha256_final((uint8_t *)digest, &sha256_ctx);
    if (ret != TC_CRYPTO_SUCCESS) {
        printk("sha256 final fail\n");
    }
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

    dev_uuid[13] = 0x2;
    return;
}

static void bt_mesh_ready(int err)
{
    int ret;

    if (err) {
        printk("Bluetooth init failed (err %d)\n", err);
        return;
    }

    ret = bt_mesh_init(&prov, &comp, NULL);
    if (ret) {
        printk("Initializing mesh failed (err %d)\n", ret);
        return;
    }

    ret = bt_mesh_prov_enable(BT_MESH_PROV_GATT | BT_MESH_PROV_ADV);
    if(ret){
        printk("%d ret mesh enable\r\n",ret);
        return;
    }
    printk("Bluetooth Mesh initialized\n");
}

int bt_mesh_profile_start(void)
{
    int ret = 0;

    //hci_driver_init();

    ret = bt_enable(bt_mesh_ready);
    if (ret) {
        printk("Bluetooth init failed (err %d)\n", ret);
    }

    return ret;
}
#if 1
int bt_mesh_profile_prov_init(const uint8_t *dev_uuid,
                              const uint8_t *digest, size_t digest_len,
                              bt_mesh_profile_prov_complete_t prov_complete,
                              bt_mesh_profile_prov_reset_t prov_reset,
                              bt_mesh_profile_prov_link_open_t prov_link_open)
#else
int bt_mesh_profile_prov_init(const uint8_t *dev_uuid,
                              const uint8_t *digest, size_t digest_len,
                              bt_mesh_evt_t bt_mesh_evt_cb)
#endif
{
    prov.uuid = dev_uuid;
    prov.static_val = digest;
    prov.static_val_len = digest_len;
    prov.complete = prov_complete;
    prov.reset = prov_reset;
    prov.link_open = prov_link_open;
    //prov.mesh_evt = bt_mesh_evt_cb;

    return 0;
}

static struct bt_mesh_model root_models[CONFIG_MESH_MODEL_NUM] = {
    BT_MESH_MODEL_CFG_SRV(&cfg_srv),
    BT_MESH_MODEL_HEALTH_SRV(&health_srv, &health_pub),
    //BT_MESH_MODEL(BT_MESH_MODEL_ID_GEN_ONOFF_SRV, gen_onoff_op,&gen_onoff_pub, NULL),
    //BT_MESH_MODEL(BT_MESH_MODEL_ID_LIGHT_LIGHTNESS_SRV, lightness_srv_op,&lightness_srv_pub, NULL),
};

static struct bt_mesh_model ali_vendor_models[CONFIG_MESH_VENDOR_MODEL_NUM] = {
    BT_MESH_MODEL_VND(CONFIG_MESH_VENDOR_COMPANY_ID,CONFIG_MESH_VENDOR_MODEL_SRV, vendor_model_op,
    NULL, NULL),
};

static struct bt_mesh_elem elements[CONFIG_MESH_ELEM_NUM] = {
    BT_MESH_ELEM(0, root_models, ali_vendor_models, 0xc006),
    //BT_MESH_ELEM(0, root_models, BT_MESH_MODEL_NONE),
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

int bt_mesh_profile_model_init(bt_mesh_opcode_cb_t bt_mesh_opcode_cb)
{
    /* add all supported elements and models from here
     * configure all light bulb related models for demo
     */

    // configure LED1 pin
    //gpio_led1.port = CONFIG_BT_MESH_LED_PIN;
    proc_opcode = bt_mesh_opcode_cb;
    return 0;
}

