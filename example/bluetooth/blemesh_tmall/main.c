/* main.c - Application main entry point */

/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <aos/aos.h>
#include <aos/kernel.h>

#include <misc/printk.h>
#include <misc/byteorder.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/mesh.h>
#include <tinycrypt/sha256.h>
#include <tinycrypt/constants.h>

#define CID_TAOBAO                      0x01A8

#define BT_MESH_MODEL_OP_SENSOR_STATUS  BT_MESH_MODEL_OP_1(0x52)
#define BT_MESH_MODEL_OP_SENSOR_GET     BT_MESH_MODEL_OP_2(0x82, 0x31)

static u16_t node_addr;
static struct tc_sha256_state_struct sha256_ctx;

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

static struct bt_mesh_model_pub temp_srv_pub = {
    .msg = NET_BUF_SIMPLE(2 + 2),
};

static struct bt_mesh_model_pub health_pub = {
    .msg = BT_MESH_HEALTH_FAULT_MSG(0),
};

/*
 * Health Server Declaration
 */

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

static struct bt_mesh_model root_models[] = {
    /* Mandatory Configuration Server model. Should be the first model
     * of root element */
    BT_MESH_MODEL_CFG_SRV(&cfg_srv),
    BT_MESH_MODEL_HEALTH_SRV(&health_srv, &health_pub),
};

static struct bt_mesh_elem elements[] = {
    BT_MESH_ELEM(0, root_models, BT_MESH_MODEL_NONE),
};

/* Node composition data used to configure a node while provisioning */
static const struct bt_mesh_comp comp = {
    .cid = CID_TAOBAO,
    .pid = 0,
    .vid = 0, // firmware version for ota
    .elem = elements,
    .elem_count = ARRAY_SIZE(elements),
    //.features and CRPL are defined by config Macro
};

static void prov_complete(u16_t net_idx, u16_t addr)
{
    printk("Provisioning completed!\n");
    printk("Net ID: %u\n", net_idx);
    printk("Unicast addr: 0x%04x\n", addr);

    node_addr = addr;
}

static void prov_reset(void)
{
    bt_mesh_prov_enable(BT_MESH_PROV_ADV | BT_MESH_PROV_GATT);
}

/* UUID for identifying the unprovisioned node */
/* sample value generated from clound for testing */
static const uint8_t pid[4] = { 0x00, 0x02, 0xa4, 0xee }; // product id:173294
static const uint8_t mac_addr[6] = { 0xaa, 0xbb, 0xcc, 0x00, 0x00, 0x01 }; // device name
static const char secret[] = "zd6sCqghXY37ySE6gmryIVkzkqruV6Vz"; // device secret
static const char pkey[] = "b1HaDlKgwoV"; // product key

static uint8_t dev_uuid[16] = { 0x00 };
static uint8_t digest[32] = { 0x00 };
static char static_value [100] = { 0x00 }; // pid + ',' + mac + ',' + secret

/* using static OOB for provisioning */
static const struct bt_mesh_prov prov = {
    .uuid = dev_uuid,
    .static_val = digest, // authval = SHA256("Product ID, MAC, Secret")
    .static_val_len = 16,
    .complete = prov_complete,
    .reset = prov_reset,
};

static void bt_ready(int err)
{
    int ret;

    if (err) {
        printk("Bluetooth init failed (err %d)\n", err);
        return;
    }

    printk("Bluetooth initialized\n");
    printk("Server\n");

    ret = bt_mesh_init(&prov, &comp);
    if (ret) {
        printk("Initializing mesh failed (err %d)\n", ret);
        return;
    }

    bt_mesh_prov_enable(BT_MESH_PROV_GATT | BT_MESH_PROV_ADV);

    printk("Mesh initialized\n");
}

static void construct_uuid(void)
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

    // RFU
    printk("dev uuid %s\n", bt_hex(dev_uuid, 16));

    return;
}

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

static void calculate_digest(void)
{
    int ret;
    char pid_string[16] = "";
    char mac_addr_string[16] = "";

    // convert the byte stream to hex string
    hextostring(pid, pid_string, 4);
    //printk("pid string: %s\n", pid_string);
    hextostring(mac_addr, mac_addr_string, 6);
    //printk("mac_addr_string: %s\n", mac_addr_string);

    strcat(static_value, pid_string);
    strcat(static_value, ",");
    strcat(static_value, mac_addr_string);
    strcat(static_value, ",");
    strcat(static_value, secret);

    printk("static oob: %s\n", static_value);

    /* calculate the sha256 of oob info
     * given an example from lark to calculate the sha256
     * oob_info = "000293E2,abcdf0f1f2f3,atFY1tGDCo4MQSVCGVDqtti3PvBI5WXb"
     * tc_sha256_init(&sha256_ctx);
     * tc_sha256_update(&sha256_ctx, oob_info, strlen(oob_info));
     * tc_sha256_final(digest, &sha256_ctx);
     * digest = DB DD 1B C2 2B B8 61 C1 C1 18 06 66 09 CC 1F 7E B0 CE 74 62 CE 7E 17 BD 23 38 1E BE 2D CE BF 88
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

extern int hci_driver_init();
void blemesh_tmall_profile(void)
{
    int ret;

    printk("Initializing tmall ble mesh profile...\n");

    // calcuate the digest of static oob info
    calculate_digest();

    // construct the uuid (will swap the pid and mac_addr)
    construct_uuid();

    hci_driver_init();

    // Initialize the Bluetooth Subsystem
    ret = bt_enable(bt_ready);
    if (ret) {
        printk("Bluetooth init failed (err %d)\n", ret);
    }
}

static void app_delayed_action(void *arg)
{
    blemesh_tmall_profile();
}

int application_start(int argc, char **argv)
{
    aos_post_delayed_action(1000, app_delayed_action, NULL);
    aos_loop_run();
    return 0;
}
