/* main.c - Application main entry point */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include <aos/aos.h>
#include <aos/kernel.h>

#include <misc/printk.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/mesh.h>

#define CID_INTEL			0x0002
#define ID_TEMP_CELSIUS			0x2A1F

#define BT_MESH_MODEL_OP_SENSOR_STATUS  BT_MESH_MODEL_OP_1(0x52)
#define BT_MESH_MODEL_OP_SENSOR_GET     BT_MESH_MODEL_OP_2(0x82, 0x31)

static u16_t node_addr;

static struct bt_mesh_cfg_cli cfg_cli = {
};

static struct bt_mesh_cfg_srv cfg_srv = {
        .relay = BT_MESH_RELAY_DISABLED,
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
        .msg  = BT_MESH_HEALTH_FAULT_MSG(0),
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

static void temp_srv_status(struct bt_mesh_model *model,
                               struct bt_mesh_msg_ctx *ctx,
                               struct net_buf_simple *buf)
{
       	struct net_buf_simple *msg = model->pub->msg;
	int ret;

	printk("Sensor Status Get request received\n");
	/* sensor status */
        bt_mesh_model_msg_init(msg, BT_MESH_MODEL_OP_SENSOR_STATUS);
	/* id: termperature in celsius */
	net_buf_simple_add_le16(msg, ID_TEMP_CELSIUS);
	/* raw temperature value in celsius */
	net_buf_simple_add_le16(msg, 27);

	ret = bt_mesh_model_publish(model);
	if (ret) {
		printk("ERR: Unable to publish sensor status: %d\n", ret);
		return;
	}
 
        printk("Sensor status sent with OpCode 0x%08x\n", BT_MESH_MODEL_OP_SENSOR_STATUS);
}

/* Sensor server model Opcode */
static const struct bt_mesh_model_op temp_srv_op[] = {
	/* Opcode, message length, message handler */
        { BT_MESH_MODEL_OP_SENSOR_GET, 2, temp_srv_status },
        BT_MESH_MODEL_OP_END,
};


static struct bt_mesh_model root_models[] = {
	/* Mandatory Configuration Server model. Should be the first model
	 * of root element */
        BT_MESH_MODEL_CFG_SRV(&cfg_srv),
        BT_MESH_MODEL_CFG_CLI(&cfg_cli),
	BT_MESH_MODEL_HEALTH_SRV(&health_srv, &health_pub),
        BT_MESH_MODEL(BT_MESH_MODEL_ID_SENSOR_SRV, temp_srv_op,
                      &temp_srv_pub, NULL),
};

static struct bt_mesh_elem elements[] = {
        BT_MESH_ELEM(0, root_models, BT_MESH_MODEL_NONE),
};

/* Node composition data used to configure a node while provisioning */
static const struct bt_mesh_comp comp = {
        .cid = CID_INTEL,
        .elem = elements,
        .elem_count = ARRAY_SIZE(elements),
};

static int output_number(bt_mesh_output_action_t action, uint32_t number)
{
        printk("OOB Number: %u\n", number);
        
	return 0;
}

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
static const uint8_t dev_uuid[16] = { 0xdd, 0xdd };

/* Only displaying the number while provisioning is supported */
static const struct bt_mesh_prov prov = {
        .uuid = dev_uuid,
        .output_size = 4,
        .output_actions = BT_MESH_DISPLAY_NUMBER,
        .output_number = output_number,
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

extern int hci_driver_init();
void blemesh_sample(void)
{
        int ret;

	printk("Initializing...\n");

        hci_driver_init();
        /* Initialize the Bluetooth Subsystem */
        ret = bt_enable(bt_ready);
        if (ret) {
                printk("Bluetooth init failed (err %d)\n", ret);
        }
}

static void app_delayed_action(void *arg)
{
    blemesh_sample();
}

int application_start(int argc, char **argv)
{
    aos_post_delayed_action(1000, app_delayed_action, NULL);
    aos_loop_run();
    return 0;
}
