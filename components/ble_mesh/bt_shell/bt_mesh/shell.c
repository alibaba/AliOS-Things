/** @file
 *  @brief Bluetooth Mesh shell
 *
 */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <ctype.h>
#include <ble_os.h>
//#include <shell/shell.h>
//#include <misc/printk.h>

#include <settings/settings.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/uuid.h>
#include <bluetooth/hci.h>
#include <bluetooth/addr.h>

#include <common/log.h>

#include <api/mesh.h>

#include <aos/cli.h>
#if defined(CONFIG_BT_MESH_CFG_CLI)
#include <api/mesh/cfg_cli.h>
#endif
#if defined(CONFIG_BT_MESH_HEALTH_CLI)
#include <api/mesh/health_cli.h>
#endif

/* Private includes for raw Network & Transport layer access */
#include "mesh.h"
#include "net.h"
#include "ble_transport.h"
#include "access.h"
#include "lpn.h"

#include "bt_errno.h"
#include "settings.h"

#ifdef CONFIG_BT_MESH_PROVISIONER
#include "provisioner_prov.h"
#include "provisioner_main.h"
#include "provisioner_proxy.h"
#endif

#define CID_NVAL 0xffff
#define OP_VENDOR_BUTTON BT_MESH_MODEL_OP_3(0x00, BT_COMP_ID_LF)

/* Default net, app & dev key values, unless otherwise specified */
static const u8_t default_key[16] = {
    0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
};

static struct {
    u16_t local;
    u16_t dst;
    u16_t net_idx;
    u16_t app_idx;
} net = {
    .local = BT_MESH_ADDR_UNASSIGNED,
    .dst = BT_MESH_ADDR_UNASSIGNED,
};

static struct bt_mesh_cfg_srv cfg_srv = {
    .relay = BT_MESH_RELAY_DISABLED,
    .beacon = BT_MESH_BEACON_DISABLED,
#if defined(CONFIG_BT_MESH_FRIEND)
    .frnd = BT_MESH_FRIEND_DISABLED,
#else
    .frnd = BT_MESH_FRIEND_NOT_SUPPORTED,
#endif
#if defined(CONFIG_BT_MESH_GATT_PROXY)
    .gatt_proxy = BT_MESH_GATT_PROXY_DISABLED,
#else
    .gatt_proxy = BT_MESH_GATT_PROXY_NOT_SUPPORTED,
#endif

    .default_ttl = 7,

    /* 3 transmissions with 20ms interval */
    .net_transmit = BT_MESH_TRANSMIT(2, 20),
    .relay_retransmit = BT_MESH_TRANSMIT(2, 20),
};

#define CUR_FAULTS_MAX 4

static u8_t cur_faults[CUR_FAULTS_MAX];
static u8_t reg_faults[CUR_FAULTS_MAX * 2];
extern struct k_sem prov_input_sem;
extern u8_t prov_input[8];
extern u8_t prov_input_size;

static void get_faults(u8_t *faults, u8_t faults_size, u8_t *dst, u8_t *count)
{
    u8_t i, limit = *count;

    for (i = 0, *count = 0; i < faults_size && *count < limit; i++) {
        if (faults[i]) {
            *dst++ = faults[i];
            (*count)++;
        }
    }
}

static int fault_get_cur(struct bt_mesh_model *model, u8_t *test_id, u16_t *company_id, u8_t *faults, u8_t *fault_count)
{
    printf("Sending current faults\n");

    *test_id = 0x00;
    *company_id = BT_COMP_ID_LF;

    get_faults(cur_faults, sizeof(cur_faults), faults, fault_count);

    return 0;
}

static int fault_get_reg(struct bt_mesh_model *model, u16_t cid, u8_t *test_id, u8_t *faults, u8_t *fault_count)
{
    if (cid != BT_COMP_ID_LF) {
        printf("Faults requested for unknown Company ID 0x%04x\n", cid);
        return -EINVAL;
    }

    printf("Sending registered faults\n");

    *test_id = 0x00;

    get_faults(reg_faults, sizeof(reg_faults), faults, fault_count);

    return 0;
}

static int fault_clear(struct bt_mesh_model *model, uint16_t cid)
{
    if (cid != BT_COMP_ID_LF) {
        return -EINVAL;
    }

    memset(reg_faults, 0, sizeof(reg_faults));

    return 0;
}

static int fault_test(struct bt_mesh_model *model, uint8_t test_id, uint16_t cid)
{
    if (cid != BT_COMP_ID_LF) {
        return -EINVAL;
    }

    if (test_id != 0x00) {
        return -EINVAL;
    }

    return 0;
}

static const struct bt_mesh_health_srv_cb health_srv_cb = {
    .fault_get_cur = fault_get_cur,
    .fault_get_reg = fault_get_reg,
    .fault_clear = fault_clear,
    .fault_test = fault_test,
};

static struct bt_mesh_health_srv health_srv = {
    .cb = &health_srv_cb,
};

BT_MESH_HEALTH_PUB_DEFINE(health_pub, CUR_FAULTS_MAX);

/*onoff cli*/
static uint8_t g_cli_onoff = 0;
static uint8_t tid;

static int on_off_cli_update(struct bt_mesh_model *mod);

BT_MESH_MODEL_PUB_DEFINE(gen_onoff_cli_pub, on_off_cli_update, 2);

static int on_off_cli_update(struct bt_mesh_model *mod)
{
    uint8_t msg[2];
    g_cli_onoff = !g_cli_onoff;
    msg[0] = g_cli_onoff;
    msg[1] = tid;

    struct net_buf_simple *msg_buf = gen_onoff_cli_pub.msg;
    bt_mesh_model_msg_init(msg_buf, BT_MESH_MODEL_OP_2(0x82, 0x03));
    net_buf_simple_add_mem(msg_buf, msg, 2);
    printf("PUB LED %s, TID %d\n", g_cli_onoff ? "ON" : "OFF", tid);
    tid++;
    return 0;
}

static void gen_onoff_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct net_buf_simple *buf)
{
    g_cli_onoff = buf->data[0];
    printf("src addr:%04x ,LED %s\n", ctx->addr, buf->data[0] ? "ON" : "OFF");
}

static const struct bt_mesh_model_op gen_onoff_cli_op[] = {
    { BT_MESH_MODEL_OP_2(0x82, 0x04), 0, gen_onoff_status },
    BT_MESH_MODEL_OP_END,
};

/*onoff srv*/
static uint8_t g_srv_onoff = 0;

static int on_off_srv_update(struct bt_mesh_model *mod);

BT_MESH_MODEL_PUB_DEFINE(gen_onoff_srv_pub, on_off_srv_update, 2);

static int on_off_srv_update(struct bt_mesh_model *mod)
{
    uint8_t msg[2];
    msg[0] = g_srv_onoff;
    msg[1] = tid++;

    struct net_buf_simple *msg_buf = gen_onoff_srv_pub.msg;

    bt_mesh_model_msg_init(msg_buf, BT_MESH_MODEL_OP_2(0x82, 0x04));
    net_buf_simple_add_mem(msg_buf, msg, 2);
    return 0;
}

static void gen_onoff_get(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct net_buf_simple *buf)
{
    /* Needed size: opcode (2 bytes) + msg + MIC */
    NET_BUF_SIMPLE_DEFINE(msg, 2 + 1 + 4);
    bt_mesh_model_msg_init(&msg, BT_MESH_MODEL_OP_2(0x82, 0x04));
    net_buf_simple_add_u8(&msg, g_srv_onoff);

    bt_mesh_model_send(model, ctx, &msg, NULL, NULL);
}

static void gen_onoff_set(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct net_buf_simple *buf)
{
    /* Needed size: opcode (2 bytes) + msg + MIC */
    NET_BUF_SIMPLE_DEFINE(msg, 2 + 1 + 4);
    g_srv_onoff = buf->data[0];

    if (tid && tid == buf->data[1]) {
        return;
    }

    tid = buf->data[1];

    printf("LED %s, TID %d\r\n", g_srv_onoff ? "ON" : "OFF", tid);

    bt_mesh_model_msg_init(&msg, BT_MESH_MODEL_OP_2(0x82, 0x04));
    net_buf_simple_add_u8(&msg, g_srv_onoff);

    bt_mesh_model_send(model, ctx, &msg, NULL, NULL);
}

static void gen_onoff_set_unack(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct net_buf_simple *buf)
{
    // led_onoff(buf->data[0]);
    g_srv_onoff = buf->data[0];

    if (tid && tid == buf->data[1]) {
        return;
    }

    tid = buf->data[1];

    printf("LED %s, TID %d\r\n", g_srv_onoff ? "ON" : "OFF", tid);
}

static const struct bt_mesh_model_op gen_onoff_srv_op[] = {
    { BT_MESH_MODEL_OP_2(0x82, 0x01), 0, gen_onoff_get },
    { BT_MESH_MODEL_OP_2(0x82, 0x02), 2, gen_onoff_set },
    { BT_MESH_MODEL_OP_2(0x82, 0x03), 2, gen_onoff_set_unack },
    BT_MESH_MODEL_OP_END,
};

static struct bt_mesh_cfg_cli cfg_cli = { 0 };

void show_faults(u8_t test_id, u16_t cid, u8_t *faults, size_t fault_count)
{
    size_t i;

    if (!fault_count) {
        printf("Health Test ID 0x%02x Company ID 0x%04x: no faults\n", test_id, cid);
        return;
    }

    printf("Health Test ID 0x%02x Company ID 0x%04x Fault Count %zu:\n", test_id, cid, fault_count);

    for (i = 0; i < fault_count; i++) {
        printf("\t0x%02x\n", faults[i]);
    }
}

#if defined(CONFIG_BT_MESH_HEALTH_CLI)
static void health_current_status(struct bt_mesh_health_cli *cli, u16_t addr, u8_t test_id, u16_t cid, u8_t *faults,
                                  size_t fault_count)
{
    printf("Health Current Status from 0x%04x\n", addr);
    show_faults(test_id, cid, faults, fault_count);
}

static struct bt_mesh_health_cli health_cli = {
    .current_status = health_current_status,
};
#endif

static u8_t dev_uuid[16] = { 0xdd, 0xdd };

static void vnd_func(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct net_buf_simple *buf)
{
    printf("enter vnd_func\r\n");
}

static struct bt_mesh_model_op vnd_ops[] = {
    { OP_VENDOR_BUTTON, 0, vnd_func },
    { 0, 0, NULL },
};

static struct bt_mesh_model vnd_models[] = {
    BT_MESH_MODEL_VND(BT_COMP_ID_LF, 0x0, vnd_ops, NULL, NULL),
};

static struct bt_mesh_model root_models[] = {
    BT_MESH_MODEL_CFG_SRV(&cfg_srv),
#if defined(CONFIG_BT_MESH_CFG_CLI)
    BT_MESH_MODEL_CFG_CLI(&cfg_cli),
#endif
    BT_MESH_MODEL_HEALTH_SRV(&health_srv, &health_pub),
#if defined(CONFIG_BT_MESH_HEALTH_CLI)
    BT_MESH_MODEL_HEALTH_CLI(&health_cli),
#endif
    BT_MESH_MODEL(BT_MESH_MODEL_ID_GEN_ONOFF_CLI, gen_onoff_cli_op, &gen_onoff_cli_pub, NULL),
    BT_MESH_MODEL(BT_MESH_MODEL_ID_GEN_ONOFF_SRV, gen_onoff_srv_op, &gen_onoff_srv_pub, NULL),
};

static struct bt_mesh_elem elements[] = {
    BT_MESH_ELEM(0, root_models, vnd_models, 0),
};

static const struct bt_mesh_comp comp = {
    .cid = BT_COMP_ID_LF,
    .elem = elements,
    .elem_count = ARRAY_SIZE(elements),
};

static u8_t hex2val(char c)
{
    if (c >= '0' && c <= '9') {
        return c - '0';
    } else if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    } else if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    } else {
        return 0;
    }
}

static void prov_complete(u16_t net_idx, u16_t addr)
{
    printf("Local node provisioned, net_idx 0x%04x address 0x%04x\n", net_idx, addr);
    net.net_idx = net_idx, net.local = addr;
    net.dst = addr;
}

/*[Genie begin] add by wenbing.cwb at 2021-01-21*/
void genie_prov_complete_notify(u16_t net_idx, u16_t addr)
{
    printf("genie node provisioned notify, net_idx 0x%04x address 0x%04x\n", net_idx, addr);
    net.net_idx = net_idx, net.local = addr;
    net.dst = addr;
}
/*[Genie end] add by wenbing.cwb at 2021-01-21*/

static void prov_reset(void)
{
    printf("The local node has been reset and needs reprovisioning\n");
}

static int output_number(bt_mesh_output_action_t action, bt_u32_t number)
{
    printf("OOB Number: %u\n", number);
    return 0;
}

static int output_string(const char *str)
{
    printf("OOB String: %s\n", str);
    return 0;
}

static bt_mesh_input_action_t input_act;
static u8_t input_size;

static int cmd_input_num(int argc, char *argv[])
{
    int err;

    if (argc < 2) {
        return -EINVAL;
    }

    if (input_act != BT_MESH_ENTER_NUMBER) {
        printf("A number hasn't been requested!\n");
        return 0;
    }

    if (strlen(argv[1]) < input_size) {
        printf("Too short input (%u digits required)\n", input_size);
        return 0;
    }

    err = bt_mesh_input_number(strtoul(argv[1], NULL, 10));

    if (err) {
        printf("Numeric input failed (err %d)\n", err);
        return 0;
    }

    input_act = BT_MESH_NO_INPUT;
    return 0;
}

static int cmd_input_str(int argc, char *argv[])
{
    int err;

    if (argc < 2) {
        return -EINVAL;
    }

    if (input_act != BT_MESH_ENTER_STRING) {
        printf("A string hasn't been requested!\n");
        return 0;
    }

    if (strlen(argv[1]) < input_size) {
        printf("Too short input (%u characters required)\n", input_size);
        return 0;
    }

    err = bt_mesh_input_string(argv[1]);

    if (err) {
        printf("String input failed (err %d)\n", err);
        return 0;
    }

    input_act = BT_MESH_NO_INPUT;
    return 0;
}

static int input(bt_mesh_input_action_t act, u8_t size)
{
    switch (act) {
    case BT_MESH_ENTER_NUMBER:
        printf("Enter a number (max %u digits) with: input-num <num>\n", size);
        break;

    case BT_MESH_ENTER_STRING:
        printf("Enter a string (max %u chars) with: input-str <str>\n", size);
        break;

    default:
        printf("Unknown input action %u (size %u) requested!\n", act, size);
        return -EINVAL;
    }

    input_act = act;
    input_size = size;
    return 0;
}

static const char *bearer2str(int bearer)
{
    switch (bearer) {
    case BT_MESH_PROV_ADV:
        return "PB-ADV";

    case BT_MESH_PROV_GATT:
        return "PB-GATT";

    default:
        return "unknown";
    }
}

static u8_t static_val[16];

static void link_open(bt_mesh_prov_bearer_t bearer)
{
    printf("Provisioning link opened on %s\n", bearer2str(bearer));
}

static void link_close(bt_mesh_prov_bearer_t bearer)
{
    printf("Provisioning link closed on %s\n", bearer2str(bearer));
}

static struct bt_mesh_prov prov = {
    .uuid = dev_uuid,
    .link_open = link_open,
    .link_close = link_close,
    .complete = prov_complete,
    .reset = prov_reset,
    .static_val = NULL,
    .static_val_len = 0,
    .output_size = 6,
    .output_actions = (BT_MESH_DISPLAY_NUMBER | BT_MESH_DISPLAY_STRING),
    .output_number = output_number,
    .output_string = output_string,
    .input_size = 6,
    .input_actions = (BT_MESH_ENTER_NUMBER | BT_MESH_ENTER_STRING),
    .input = input,
};

#ifdef CONFIG_BT_MESH_PROVISIONER
static void provisioner_link_open(bt_mesh_prov_bearer_t bearer, uint8_t addr_val[6], uint8_t addr_type,
                                  uint8_t uuid[16], uint8_t prov_count)
{
    printf("Provisioner link opened on %s\n", bearer2str(bearer));
}

static void provisioner_link_close(bt_mesh_prov_bearer_t bearer, u8_t reason, uint8_t addr_val[6], uint8_t addr_type,
                                   uint8_t uuid[16], uint8_t prov_count)
{
    printf("Provisioner link closed on %s, reason %d\n", bearer2str(bearer), reason);
}

static void provisioner_complete(int node_idx, const u8_t device_uuid[16], u16_t unicast_addr, u8_t element_num,
                                 u16_t netkey_idx, bool gatt_flag)
{
    printf("provisioner_complete\r\n");
    printf("node_idx: %d\r\n", node_idx);
    printf("device_uuid: %02x, %02x, %02x, %02x, %02x, %02x\r\n", device_uuid[0], device_uuid[1], device_uuid[2],
           device_uuid[3], device_uuid[4], device_uuid[5]);
    printf("unicast_addr: %u\r\n", unicast_addr);
    printf("element_num: %u\r\n", element_num);
    printf("netkey_idx: %u\r\n", netkey_idx);
    printf("gatt_flag: %u\r\n", gatt_flag);
}

static int provisioner_input_num(bt_mesh_output_action_t act, u8_t size)
{
    bool input_num_flag = 0;

    if (BT_MESH_DISPLAY_NUMBER == act) {
        input_num_flag = true;
    } else if (BT_MESH_DISPLAY_STRING == act) {
        input_num_flag = false;
    }

    k_sem_take(&prov_input_sem, K_FOREVER);

    printf("xxxxxxxxxxxx: %s", prov_input);
    printf("get input ");
    printf("xxxxxxxxxxxx: %u", prov_input_size);

    bt_mesh_prov_input_data(prov_input, prov_input_size, input_num_flag);

    return 0;
}

static int provisioner_output_num(bt_mesh_input_action_t act, u8_t size)
{
    bt_u32_t div[8] = { 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000 };
    bt_u32_t num = 0;
    u8_t temp[8];
    u8_t i;
    bool output_num_flag = 0;

    if (BT_MESH_ENTER_NUMBER == act) {
        output_num_flag = true;

        bt_rand(&num, size);

        num %= div[size - 1];

        printf("===================");
        printf("input number %06u in the device", num);
        printf("===================");

        memset(temp, 0, sizeof(temp));

        for (i = size; i > 0; i--) {
            temp[i - 1] = num & 0xFF;
            num >>= 8;

            if (num == 0) {
                break;
            }
        }
    } else if (BT_MESH_ENTER_STRING == act) {
        output_num_flag = false;

        bt_rand(temp, size);

        /* Normalize to '0' .. '9' & 'A' .. 'Z' */
        for (i = 0; i < size; i++) {
            temp[i] %= 36;

            if (temp[i] < 10) {
                temp[i] += '0';
            } else {
                temp[i] += 'A' - 10;
            }
        }

        temp[size] = '\0';

        printf("===================");
        printf("input string %s in the device", temp);
        printf("===================");
    }

    bt_mesh_prov_output_data(temp, size, output_num_flag);

    return 0;
}

static struct bt_mesh_provisioner provisioner = {
    .prov_uuid = 0,
    .prov_unicast_addr = 0x0001,
    .prov_start_address = 0x0002,
    .prov_attention = 0,
    .prov_algorithm = 0,
    .prov_pub_key_oob = 0,
    .prov_pub_key_oob_cb = 0,
    .prov_static_oob_val = 0,
    .prov_static_oob_len = 0,
    .prov_input_num = provisioner_input_num,
    .prov_output_num = provisioner_output_num,
    .flags = 0,
    .iv_index = 0,
    .prov_link_open = provisioner_link_open,
    .prov_link_close = provisioner_link_close,
    .prov_complete = provisioner_complete,
};
#endif

static int cmd_static_oob(int argc, char *argv[])
{
    if (argc < 2) {
        prov.static_val = NULL;
        prov.static_val_len = 0;
    } else {
        prov.static_val_len = hex2bin(argv[1], strlen(argv[1]), static_val, 16);

        if (prov.static_val_len) {
            prov.static_val = static_val;
        } else {
            prov.static_val = NULL;
        }
    }

    if (prov.static_val) {
        printf("Static OOB value set (length %u)\n", prov.static_val_len);
    } else {
        printf("Static OOB value cleared\n");
    }

    return 0;
}

#define AUTH_METHOD_NO_OOB 0x00
#define AUTH_METHOD_STATIC 0x01
#define AUTH_METHOD_OUTPUT 0x02
#define AUTH_METHOD_INPUT 0x03

static int cmd_oob_mode(int argc, char *argv[])
{
    if (argc < 2) {
        return -EINVAL;
    }
    uint8_t mode = strtoul(argv[1], NULL, 16);
    uint8_t action = 0;
    if (mode == AUTH_METHOD_NO_OOB) {
        prov.static_val = NULL;
        prov.static_val_len = 0;
        prov.output_size = 0;
        prov.output_actions = BT_MESH_NO_OUTPUT;
        prov.output_number = NULL;
        prov.output_string = NULL;
        prov.input_size = 0;
        prov.input_actions = BT_MESH_NO_INPUT;
        prov.input = NULL;
    } else if (mode == AUTH_METHOD_STATIC) {
        prov.output_size = 0;
        prov.output_actions = BT_MESH_NO_OUTPUT;
        prov.output_number = NULL;
        prov.output_string = NULL;
        prov.input_size = 0;
        prov.input_actions = BT_MESH_NO_INPUT;
        prov.input = NULL;
    } else if (mode == AUTH_METHOD_OUTPUT) {
        if (argc > 2) {
            action = strtoul(argv[2], NULL, 16);
        } else {
            action = BT_MESH_DISPLAY_NUMBER;
        }
        prov.static_val = NULL;
        prov.static_val_len = 0;
        prov.output_size = 6;
        prov.output_actions = action;
        prov.output_number = output_number;
        prov.output_string = output_string;
        prov.input_size = 0;
        prov.input_actions = BT_MESH_NO_INPUT;
        prov.input = NULL;
    } else if (mode == AUTH_METHOD_INPUT) {
        if (argc > 2) {
            action = strtoul(argv[2], NULL, 16);
        } else {
            action = BT_MESH_ENTER_NUMBER;
        }
        prov.static_val = NULL;
        prov.static_val_len = 0;
        prov.output_size = 0;
        prov.output_actions = BT_MESH_NO_OUTPUT;
        prov.output_number = NULL;
        prov.output_string = NULL;
        prov.input_size = 6;
        prov.input_actions = action, prov.input = input;
    } else {
        printf("err mode\r\n");
        return -EINVAL;
    }

    return 0;
}

static int cmd_uuid(int argc, char *argv[])
{
    u8_t uuid[16];
    size_t len;

    if (argc < 2) {
        return -EINVAL;
    }

    len = hex2bin(argv[1], strlen(argv[1]), uuid, sizeof(uuid));

    if (len < 1 && len != 16) {
        return -EINVAL;
    }

    memcpy(dev_uuid, uuid, len);

    printf("Device UUID set\n");

    return 0;
}

static int cmd_reset(int argc, char *argv[])
{
    bt_mesh_reset();
    printf("Local node reset complete\n");
    return 0;
}

static u8_t str2u8(const char *str)
{
    if (isdigit((unsigned char)str[0])) {
        return strtoul(str, NULL, 0);
    }

    return (!strcmp(str, "on") || !strcmp(str, "enable"));
}

static bool str2bool(const char *str)
{
    return str2u8(str);
}

#if defined(CONFIG_BT_MESH_LOW_POWER)
static int cmd_lpn(int argc, char *argv[])
{
    static bool enabled;
    int err;

    if (argc < 2) {
        printf("%s\n", enabled ? "enabled" : "disabled");
        return 0;
    }

    if (str2bool(argv[1])) {
        if (enabled) {
            printf("LPN already enabled\n");
            return 0;
        }

        err = bt_mesh_lpn_set(true);

        if (err) {
            printf("Enabling LPN failed (err %d)\n", err);
        } else {
            enabled = true;
        }
    } else {
        if (!enabled) {
            printf("LPN already disabled\n");
            return 0;
        }

        err = bt_mesh_lpn_set(false);

        if (err) {
            printf("Enabling LPN failed (err %d)\n", err);
        } else {
            enabled = false;
        }
    }

    return 0;
}

static int cmd_poll(int argc, char *argv[])
{
    int err;

    err = bt_mesh_lpn_poll();

    if (err) {
        printf("Friend Poll failed (err %d)\n", err);
    }

    return 0;
}

static int cmd_group_add(int argc, char *argv[])
{
    uint16_t group = 0;

    if (argc < 2) {
        return -EINVAL;
    }

    group = strtoul(argv[1], NULL, 16);
    bt_mesh_lpn_group_add(group);

    return 0;
}
static int cmd_group_del(int argc, char *argv[])
{
    if (argc < 2) {
        return -EINVAL;
    }

    uint16_t group = 0;
    group = strtoul(argv[1], NULL, 16);

    bt_mesh_lpn_group_del(&group, 1);

    return 0;
}

static int cmd_friend_clear(int argc, char *argv[])
{
    int flagtrue, flagfalse;

    flagtrue = strcmp(argv[1], "true");
    flagfalse = strcmp(argv[1], "false");

    if ((flagtrue != 0) && (flagfalse != 0)) {
        printf("Friend friend_clear input failed \n");
    }

    if (flagtrue == 0) {
        bt_mesh_lpn_disable(true);
    } else {
        bt_mesh_lpn_disable(false);
    }

    return 0;
}

static void lpn_cb(u16_t friend_addr, bool established)
{
    if (established == true) {
        printf("friend addr:%x,friendship keep\n", friend_addr);
    } else {
        printf("friend addr:%x,friendship finish\n", friend_addr);
    }
}

#endif /* MESH_LOW_POWER */

static int cmd_tx_power(int argc, char *argv[])
{
    if (argc < 2) {
        return -EINVAL;
    }
#if defined(BOARD_6121) || defined(BOARD_PHY6220_EVB)
    uint8_t tx = strtoul(argv[1], NULL, 0);
    extern void rf_phy_set_txPower(uint8_t txPower);
    rf_phy_set_txPower(tx);
#else
    return -1;
#endif
    return 0;
}

static int cmd_init(int argc, char *argv[])
{
    int err = 0;

#ifdef CONFIG_BT_MESH_PROVISIONER
    err = bt_mesh_init(&prov, &comp, &provisioner);
#else
    err = bt_mesh_init(&prov, &comp, NULL);
#endif

    if (err && err != -EALREADY) {
        printf("Mesh initialization failed (err %d)\n", err);
        return err;
    }

    printf("Mesh initialized\n");

    if (err == -EALREADY) {
        return 0;
    }

    if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
        settings_load();
    }

    if (bt_mesh_is_provisioned()) {
        printf("Mesh network restored from flash\n");
    } else {
        printf("Use \"pb-adv on\" or \"pb-gatt on\" to enable"
               " advertising\n");
    }

#if IS_ENABLED(CONFIG_BT_MESH_LOW_POWER)
    bt_mesh_lpn_set_cb(lpn_cb);
#endif

    return 0;
}

#if defined(CONFIG_BT_MESH_GATT_PROXY)
static int cmd_ident(int argc, char *argv[])
{
    int err;

    if (argc < 2) {
        return -EINVAL;
    }

    if (str2bool(argv[1])) {
        err = bt_mesh_proxy_identity_enable();
    } else {
        err = bt_mesh_proxy_identity_disable();
    }

    if (err) {
        printf("Failed advertise using Node Identity (err %d)\n", err);
    }

    return 0;
}
#endif /* MESH_GATT_PROXY */

static int cmd_get_comp(int argc, char *argv[])
{
    NET_BUF_SIMPLE_DEFINE(comp, 128);
    u8_t status, page = 0x00;
    int err;

    if (argc > 1) {
        page = strtol(argv[1], NULL, 0);
    }

    err = bt_mesh_cfg_comp_data_get(net.net_idx, net.dst, page, &status, &comp);

    if (err) {
        printf("Getting composition failed (err %d)\n", err);
        return 0;
    }

    if (status != 0x00) {
        printf("Got non-success status 0x%02x\n", status);
        return 0;
    }

    printf("Got Composition Data for 0x%04x:\n", net.dst);
    printf("\tCID      0x%04x\n", net_buf_simple_pull_le16(&comp));
    printf("\tPID      0x%04x\n", net_buf_simple_pull_le16(&comp));
    printf("\tVID      0x%04x\n", net_buf_simple_pull_le16(&comp));
    printf("\tCRPL     0x%04x\n", net_buf_simple_pull_le16(&comp));
    printf("\tFeatures 0x%04x\n", net_buf_simple_pull_le16(&comp));

    while (comp.len > 4) {
        u8_t sig, vnd;
        u16_t loc;
        int i;

        loc = net_buf_simple_pull_le16(&comp);
        sig = net_buf_simple_pull_u8(&comp);
        vnd = net_buf_simple_pull_u8(&comp);

        printf("\n\tElement @ 0x%04x:\n", loc);

        if (comp.len < ((sig * 2) + (vnd * 4))) {
            printf("\t\t...truncated data!\n");
            break;
        }

        if (sig) {
            printf("\t\tSIG Models:\n");
        } else {
            printf("\t\tNo SIG Models\n");
        }

        for (i = 0; i < sig; i++) {
            u16_t mod_id = net_buf_simple_pull_le16(&comp);

            printf("\t\t\t0x%04x\n", mod_id);
        }

        if (vnd) {
            printf("\t\tVendor Models:\n");
        } else {
            printf("\t\tNo Vendor Models\n");
        }

        for (i = 0; i < vnd; i++) {
            u16_t cid = net_buf_simple_pull_le16(&comp);
            u16_t mod_id = net_buf_simple_pull_le16(&comp);

            printf("\t\t\tCompany 0x%04x: 0x%04x\n", cid, mod_id);
        }
    }

    return 0;
}

static int cmd_dst(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Destination address: 0x%04x%s\n", net.dst, net.dst == net.local ? " (local)" : "");
        return 0;
    }

    if (!strcmp(argv[1], "local")) {
        net.dst = net.local;
    } else {
        net.dst = strtoul(argv[1], NULL, 0);
    }

    printf("Destination address set to 0x%04x%s\n", net.dst, net.dst == net.local ? " (local)" : "");
    return 0;
}

static int cmd_netidx(int argc, char *argv[])
{
    if (argc < 2) {
        printf("NetIdx: 0x%04x\n", net.net_idx);
        return 0;
    }

    net.net_idx = strtoul(argv[1], NULL, 0);
    printf("NetIdx set to 0x%04x\n", net.net_idx);
    return 0;
}

static int cmd_appidx(int argc, char *argv[])
{
    if (argc < 2) {
        printf("AppIdx: 0x%04x\n", net.app_idx);
        return 0;
    }

    net.app_idx = strtoul(argv[1], NULL, 0);
    printf("AppIdx set to 0x%04x\n", net.app_idx);
    return 0;
}

extern u8_t bt_mesh_net_transmit_get(void);
static int cmd_net_send(int argc, char *argv[])
{
    NET_BUF_SIMPLE_DEFINE(msg, 32);
    struct bt_mesh_msg_ctx ctx = {
        .send_ttl = BT_MESH_TTL_DEFAULT,
        .net_idx = net.net_idx,
        .addr = net.dst,
        .app_idx = net.app_idx,

    };
    struct bt_mesh_net_tx tx = {
        .ctx = &ctx,
        .src = net.local,
        .xmit = bt_mesh_net_transmit_get(),
        .sub = bt_mesh_subnet_get(net.net_idx),
    };
    size_t len;
    int err;

    if (argc < 2) {
        return -EINVAL;
    }

    if (!tx.sub) {
        printf("No matching subnet for NetKey Index 0x%04x\n", net.net_idx);
        return 0;
    }

    len = hex2bin(argv[1], strlen(argv[1]), msg.data, net_buf_simple_tailroom(&msg) - 4);
    net_buf_simple_add(&msg, len);

    err = bt_mesh_trans_send(&tx, &msg, NULL, NULL);

    if (err) {
        printf("Failed to send (err %d)\n", err);
    }

    return 0;
}

#if defined(CONFIG_BT_MESH_IV_UPDATE_TEST)
static int cmd_iv_update(int argc, char *argv[])
{
    if (bt_mesh_iv_update()) {
        printf("Transitioned to IV Update In Progress state\n");
    } else {
        printf("Transitioned to IV Update Normal state\n");
    }

    printf("IV Index is 0x%08x\n", bt_mesh.iv_index);

    return 0;
}

static int cmd_iv_update_test(int argc, char *argv[])
{
    bool enable;

    if (argc < 2) {
        return -EINVAL;
    }

    enable = str2bool(argv[1]);

    if (enable) {
        printf("Enabling IV Update test mode\n");
    } else {
        printf("Disabling IV Update test mode\n");
    }

    bt_mesh_iv_update_test(enable);

    return 0;
}
#endif

static int cmd_rpl_clear(int argc, char *argv[])
{
    bt_mesh_rpl_clear();
    return 0;
}

static int cmd_beacon(int argc, char *argv[])
{
    u8_t status;
    int err;

    if (argc < 2) {
        err = bt_mesh_cfg_beacon_get(net.net_idx, net.dst, &status);
    } else {
        u8_t val = str2u8(argv[1]);

        err = bt_mesh_cfg_beacon_set(net.net_idx, net.dst, val, &status);
    }

    if (err) {
        printf("Unable to send Beacon Get/Set message (err %d)\n", err);
        return 0;
    }

    printf("Beacon state is 0x%02x\n", status);

    return 0;
}

static int cmd_ttl(int argc, char *argv[])
{
    u8_t ttl;
    int err;

    if (argc < 2) {
        err = bt_mesh_cfg_ttl_get(net.net_idx, net.dst, &ttl);
    } else {
        u8_t val = strtoul(argv[1], NULL, 0);

        err = bt_mesh_cfg_ttl_set(net.net_idx, net.dst, val, &ttl);
    }

    if (err) {
        printf("Unable to send Default TTL Get/Set (err %d)\n", err);
        return 0;
    }

    printf("Default TTL is 0x%02x\n", ttl);

    return 0;
}

static int cmd_friend(int argc, char *argv[])
{
    u8_t frnd;
    int err;

    if (argc < 2) {
        err = bt_mesh_cfg_friend_get(net.net_idx, net.dst, &frnd);
    } else {
        u8_t val = str2u8(argv[1]);

        err = bt_mesh_cfg_friend_set(net.net_idx, net.dst, val, &frnd);
    }

    if (err) {
        printf("Unable to send Friend Get/Set (err %d)\n", err);
        return 0;
    }

    printf("Friend is set to 0x%02x\n", frnd);

    return 0;
}

static int cmd_gatt_proxy(int argc, char *argv[])
{
    u8_t proxy;
    int err;

    if (argc < 2) {
        err = bt_mesh_cfg_gatt_proxy_get(net.net_idx, net.dst, &proxy);
    } else {
        u8_t val = str2u8(argv[1]);

        err = bt_mesh_cfg_gatt_proxy_set(net.net_idx, net.dst, val, &proxy);
    }

    if (err) {
        printf("Unable to send GATT Proxy Get/Set (err %d)\n", err);
        return 0;
    }

    printf("GATT Proxy is set to 0x%02x\n", proxy);

    return 0;
}

static int cmd_relay(int argc, char *argv[])
{
    u8_t relay, transmit;
    int err;

    if (argc < 2) {
        err = bt_mesh_cfg_relay_get(net.net_idx, net.dst, &relay, &transmit);
    } else {
        u8_t val = str2u8(argv[1]);
        u8_t count, interval, new_transmit;

        if (val) {
            if (argc > 2) {
                count = strtoul(argv[2], NULL, 0);
            } else {
                count = 2;
            }

            if (argc > 3) {
                interval = strtoul(argv[3], NULL, 0);
            } else {
                interval = 20;
            }

            new_transmit = BT_MESH_TRANSMIT(count, interval);
        } else {
            new_transmit = 0;
        }

        err = bt_mesh_cfg_relay_set(net.net_idx, net.dst, val, new_transmit, &relay, &transmit);
    }

    if (err) {
        printf("Unable to send Relay Get/Set (err %d)\n", err);
        return 0;
    }

    printf("Relay is 0x%02x, Transmit 0x%02x (count %u interval %ums)\n", relay, transmit,
           BT_MESH_TRANSMIT_COUNT(transmit), BT_MESH_TRANSMIT_INT(transmit));

    return 0;
}

static int cmd_net_key_add(int argc, char *argv[])
{
    u8_t key_val[16];
    u16_t key_net_idx;
    u8_t status;
    int err;

    if (argc < 2) {
        return -EINVAL;
    }

    key_net_idx = strtoul(argv[1], NULL, 0);

    if (argc > 2) {
        size_t len;

        len = hex2bin(argv[2], strlen(argv[1]), key_val, sizeof(key_val));
        memset(key_val, 0, sizeof(key_val) - len);
    } else {
        memcpy(key_val, default_key, sizeof(key_val));
    }

    err = bt_mesh_cfg_net_key_add(net.net_idx, net.dst, key_net_idx, key_val, &status);

    if (err) {
        printf("Unable to send NetKey Add (err %d)\n", err);
        return 0;
    }

    if (status) {
        printf("NetKeyAdd failed with status 0x%02x\n", status);
    } else {
        printf("NetKey added with NetKey Index 0x%03x\n", key_net_idx);
    }

    return 0;
}

#ifdef CONFIG_BT_MESH_PROVISIONER
static int cmd_net_key_update(int argc, char *argv[])
{
    u16_t key_net_idx, send_net_idx, send_dst;
    u8_t status;
    int err;
    struct bt_mesh_subnet *sub;

    if (argc < 4) {
        return -EINVAL;
    }

    send_net_idx = strtoul(argv[1], NULL, 0);
    send_dst = strtoul(argv[2], NULL, 0);
    key_net_idx = strtoul(argv[3], NULL, 0);

    sub = bt_mesh_subnet_get(key_net_idx);

    if (!sub) {
        return -1;
    }

    err = bt_mesh_net_keys_create(&sub->keys[1], default_key);

    if (err) {
        printf("create new key faild\n");
        return -1;
    }

    err = bt_mesh_cfg_net_key_update(send_net_idx, send_dst, key_net_idx, default_key, &status);

    if (err) {
        printf("Unable to send NetKey update (err %d)\n", err);
        return 0;
    }

    if (status) {
        printf("NetKeyupdate  failed with status 0x%02x\n", status);
    } else {
        printf("NetKeyupdate with NetKey Index 0x%03x\n", key_net_idx);
    }

    return 0;
}

static int update_appkey = 0;
static int cmd_app_key_update(int argc, char *argv[])
{
    u16_t key_net_idx, send_net_idx, send_dst, key_app_idx;
    u8_t status;
    int err;
    struct bt_mesh_subnet *sub;

    if (argc < 4) {
        return -EINVAL;
    }

    send_net_idx = strtoul(argv[1], NULL, 0);
    send_dst = strtoul(argv[2], NULL, 0);
    key_net_idx = strtoul(argv[3], NULL, 0);
    key_app_idx = strtoul(argv[4], NULL, 0);

    sub = bt_mesh_subnet_get(key_net_idx);

    if (!sub) {
        return -1;
    }

    err = bt_mesh_cfg_app_key_update(send_net_idx, send_dst, key_net_idx, key_app_idx, default_key, &status);

    if (err) {
        printf("Unable to send AppKey update (err %d)\n", err);
        return 0;
    }

    if (status) {
        printf("AppKeyupdate  failed with status 0x%02x\n", status);
    } else {
        printf("NetKeyupdate with NetKey Index 0x%03x AppKey Index 0x%03x \n", key_net_idx, key_app_idx);
    }

    update_appkey = 1;
    return 0;
}

static int cmd_krp_set(int argc, char *argv[])
{
    u16_t key_net_idx, send_net_idx, send_dst;
    u8_t status, phase;
    int err;

    if (argc < 5) {
        return -EINVAL;
    }

    send_net_idx = strtoul(argv[1], NULL, 0);
    send_dst = strtoul(argv[2], NULL, 0);
    key_net_idx = strtoul(argv[3], NULL, 0);
    phase = strtoul(argv[4], NULL, 0);

    if (phase < BT_MESH_KR_PHASE_2 || phase > BT_MESH_KR_PHASE_3) {
        printf("the phase invaild\n");
        return -EINVAL;
    }

    err = bt_mesh_cfg_krp_set(send_net_idx, send_dst, key_net_idx, &phase, &status);

    if (err) {
        printf("Unable to send krp set (err %d)\n", err);
        return 0;
    }

    if (status) {
        printf("krp set failed with status 0x%02x\n", status);
    } else {
        printf("krp set with  0x%03x\n", phase);
    }

    return 0;
}

static int cmd_krp_get(int argc, char *argv[])
{
    u16_t key_net_idx, send_net_idx, send_dst;
    u8_t status, phase;
    int err;
    struct bt_mesh_subnet *sub = NULL;

    if (argc < 4) {
        return -EINVAL;
    }

    send_net_idx = strtoul(argv[1], NULL, 0);
    send_dst = strtoul(argv[2], NULL, 0);
    key_net_idx = strtoul(argv[3], NULL, 0);

    err = bt_mesh_cfg_krp_get(send_net_idx, send_dst, key_net_idx, &phase, &status);

    if (err) {
        printf("Unable to send krp set (err %d)\n", err);
        return 0;
    }

    if (status) {
        printf("krp set failed with status 0x%02x\n", status);
    } else {
        printf("krp get 0x%02x\n", phase);
    }

    if (0x01 == phase) {
        sub = bt_mesh_subnet_get(key_net_idx);

        if (NULL == sub) {
            printf("get subnet failed\n");
            return -1;
        }

        sub->kr_phase = BT_MESH_KR_PHASE_2;
        sub->kr_flag = 1;
    }

    return 0;
}

static int cmd_local_revoke_keys(int argc, char *argv[])
{
    struct bt_mesh_subnet *sub;
    int err;
    uint16_t key_net_idx, appkey_idx;

    if (!update_appkey && argc < 2) {
        return -EINVAL;
    }

    if (update_appkey && argc < 3) {
        return -EINVAL;
    }

    key_net_idx = strtoul(argv[1], NULL, 0);

    if (1 == update_appkey) {
        appkey_idx = strtoul(argv[2], NULL, 0);
        err = bt_mesh_provisioner_local_app_key_delete(key_net_idx, appkey_idx);

        if (err) {
            printf("delete the AppKey faild\n");
            return -1;
        }

        err = bt_mesh_provisioner_local_app_key_add(default_key, key_net_idx, &appkey_idx);

        if (err) {
            printf("readd the AppKey faild\n ");
            return -1;
        }
    }

    sub = bt_mesh_subnet_get(key_net_idx);

    if (!sub) {
        return -1;
    }

    bt_mesh_net_revoke_keys(sub);
    update_appkey = 0;
    return 0;
}
#endif
static int cmd_app_key_add(int argc, char *argv[])
{
    u8_t key_val[16];
    u16_t key_net_idx, key_app_idx;
    u8_t status;
    int err;

    if (argc < 3) {
        return -EINVAL;
    }

    key_net_idx = strtoul(argv[1], NULL, 0);
    key_app_idx = strtoul(argv[2], NULL, 0);

    if (argc > 3) {
        size_t len;

        len = hex2bin(argv[3], strlen(argv[3]), key_val, sizeof(key_val));
        memset(key_val, 0, sizeof(key_val) - len);
    } else {
#ifdef CONFIG_BT_MESH_PROVISIONER
        const u8_t *src_key_val;

        src_key_val = bt_mesh_provisioner_local_app_key_get(key_net_idx, key_app_idx);

        if (NULL == src_key_val) {
            printf("no appkey found\n");
            return -EINVAL;
        } else {
            memcpy(key_val, src_key_val, sizeof(key_val));
        }

#else
        memcpy(key_val, default_key, sizeof(key_val));
#endif
    }

    err = bt_mesh_cfg_app_key_add(net.net_idx, net.dst, key_net_idx, key_app_idx, key_val, &status);

    if (err) {
        printf("Unable to send App Key Add (err %d)\n", err);
        return 0;
    }

    if (status) {
        printf("AppKeyAdd failed with status 0x%02x\n", status);
    } else {
        printf("AppKey added, NetKeyIndex 0x%04x AppKeyIndex 0x%04x\n", key_net_idx, key_app_idx);
    }

    return 0;
}

static int cmd_mod_app_bind(int argc, char *argv[])
{
    u16_t elem_addr, mod_app_idx, mod_id, cid;
    u8_t status;
    int err;

    if (argc < 4) {
        return -EINVAL;
    }

    elem_addr = strtoul(argv[1], NULL, 0);
    mod_app_idx = strtoul(argv[2], NULL, 0);
    mod_id = strtoul(argv[3], NULL, 0);

    if (argc > 4) {
        cid = strtoul(argv[4], NULL, 0);
        err = bt_mesh_cfg_mod_app_bind_vnd(net.net_idx, net.dst, elem_addr, mod_app_idx, mod_id, cid, &status);
    } else {
        err = bt_mesh_cfg_mod_app_bind(net.net_idx, net.dst, elem_addr, mod_app_idx, mod_id, &status);
    }

    if (err) {
        printf("Unable to send Model App Bind (err %d)\n", err);
        return 0;
    }

    if (status) {
        printf("Model App Bind failed with status 0x%02x\n", status);
    } else {
        printf("AppKey successfully bound\n");
    }

    return 0;
}

static int cmd_mod_sub_get(int argc, char *argv[])
{
    u16_t sub_addr[CONFIG_BT_MESH_MODEL_GROUP_COUNT], mod_id, cid, addr;
    u8_t status;
    int err, index;

    if (argc < 3) {
        return -EINVAL;
    }
    memset(sub_addr, 0, CONFIG_BT_MESH_MODEL_GROUP_COUNT * 2);

    addr = strtoul(argv[1], NULL, 0);
    mod_id = strtoul(argv[2], NULL, 0);
    if (argc < 4) {
        cid = CID_NVAL;
    } else {
        cid = strtoul(argv[3], NULL, 0);
    }

    if (cid == CID_NVAL) {
        err = bt_mesh_cfg_mod_sub_get(net.net_idx, addr, mod_id, sub_addr, &status);
    } else {
        err = bt_mesh_cfg_mod_sub_get_vnd(net.net_idx, addr, mod_id, cid, sub_addr, &status);
    }

    if (err) {
        printf("Model sublist Get failed (err %d)\n", err);
        return 0;
    }

    if (status) {
        printf("Model sublist Get failed (status 0x%02x)\n", status);
        return 0;
    }

    printf("Model sublist for Element 0x%04x, Model 0x%04x ,CID 0x%04x\r\n", addr, mod_id, cid);
    for (index = 0; index < CONFIG_BT_MESH_MODEL_GROUP_COUNT; index++) {
        if (sub_addr[index] != 0x0000) {
            printf("0x%04x;", sub_addr[index]);
            index++;
        }
    }

    memset(sub_addr, 0, CONFIG_BT_MESH_MODEL_GROUP_COUNT * 2);

    return 0;
}

static int cmd_mod_sub_add(int argc, char *argv[])
{
    u16_t elem_addr, sub_addr, mod_id, cid;
    u8_t status;
    int err;

    if (argc < 4) {
        return -EINVAL;
    }

    elem_addr = strtoul(argv[1], NULL, 0);
    sub_addr = strtoul(argv[2], NULL, 0);
    mod_id = strtoul(argv[3], NULL, 0);

    if (argc > 4) {
        cid = strtoul(argv[4], NULL, 0);
        err = bt_mesh_cfg_mod_sub_add_vnd(net.net_idx, net.dst, elem_addr, sub_addr, mod_id, cid, &status);
    } else {
        err = bt_mesh_cfg_mod_sub_add(net.net_idx, net.dst, elem_addr, sub_addr, mod_id, &status);
    }

    if (err) {
        printf("Unable to send Model Subscription Add (err %d)\n", err);
        return 0;
    }

    if (status) {
        printf("Model Subscription Add failed with status 0x%02x\n", status);
    } else {
        printf("Model subscription was successful\n");
    }

    return 0;
}

static int cmd_mod_sub_del(int argc, char *argv[])
{
    u16_t elem_addr, sub_addr, mod_id, cid;
    u8_t status;
    int err;

    if (argc < 4) {
        return -EINVAL;
    }

    elem_addr = strtoul(argv[1], NULL, 0);
    sub_addr = strtoul(argv[2], NULL, 0);
    mod_id = strtoul(argv[3], NULL, 0);

    if (argc > 4) {
        cid = strtoul(argv[4], NULL, 0);
        err = bt_mesh_cfg_mod_sub_del_vnd(net.net_idx, net.dst, elem_addr, sub_addr, mod_id, cid, &status);
    } else {
        err = bt_mesh_cfg_mod_sub_del(net.net_idx, net.dst, elem_addr, sub_addr, mod_id, &status);
    }

    if (err) {
        printf("Unable to send Model Subscription Delete (err %d)\n", err);
        return 0;
    }

    if (status) {
        printf("Model Subscription Delete failed with status 0x%02x\n", status);
    } else {
        printf("Model subscription deltion was successful\n");
    }

    return 0;
}

static int cmd_mod_sub_add_va(int argc, char *argv[])
{
    u16_t elem_addr, sub_addr, mod_id, cid;
    u8_t label[16];
    u8_t status;
    size_t len;
    int err;

    if (argc < 4) {
        return -EINVAL;
    }

    elem_addr = strtoul(argv[1], NULL, 0);

    len = hex2bin(argv[2], strlen(argv[2]), label, sizeof(label));
    memset(label + len, 0, sizeof(label) - len);

    mod_id = strtoul(argv[3], NULL, 0);

    if (argc > 4) {
        cid = strtoul(argv[4], NULL, 0);
        err = bt_mesh_cfg_mod_sub_va_add_vnd(net.net_idx, net.dst, elem_addr, label, mod_id, cid, &sub_addr, &status);
    } else {
        err = bt_mesh_cfg_mod_sub_va_add(net.net_idx, net.dst, elem_addr, label, mod_id, &sub_addr, &status);
    }

    if (err) {
        printf("Unable to send Mod Sub VA Add (err %d)\n", err);
        return 0;
    }

    if (status) {
        printf("Mod Sub VA Add failed with status 0x%02x\n", status);
    } else {
        printf("0x%04x subscribed to Label UUID %s (va 0x%04x)\n", elem_addr, argv[2], sub_addr);
    }

    return 0;
}

static int cmd_mod_sub_del_va(int argc, char *argv[])
{
    u16_t elem_addr, sub_addr, mod_id, cid;
    u8_t label[16];
    u8_t status;
    size_t len;
    int err;

    if (argc < 4) {
        return -EINVAL;
    }

    elem_addr = strtoul(argv[1], NULL, 0);

    len = hex2bin(argv[2], strlen(argv[2]), label, sizeof(label));
    memset(label + len, 0, sizeof(label) - len);

    mod_id = strtoul(argv[3], NULL, 0);

    if (argc > 4) {
        cid = strtoul(argv[4], NULL, 0);
        err = bt_mesh_cfg_mod_sub_va_del_vnd(net.net_idx, net.dst, elem_addr, label, mod_id, cid, &sub_addr, &status);
    } else {
        err = bt_mesh_cfg_mod_sub_va_del(net.net_idx, net.dst, elem_addr, label, mod_id, &sub_addr, &status);
    }

    if (err) {
        printf("Unable to send Model Subscription Delete (err %d)\n", err);
        return 0;
    }

    if (status) {
        printf("Model Subscription Delete failed with status 0x%02x\n", status);
    } else {
        printf("0x%04x unsubscribed from Label UUID %s (va 0x%04x)\n", elem_addr, argv[2], sub_addr);
    }

    return 0;
}

#ifdef CONFIG_BT_MESH_PROVISIONER

static int cmd_local_model_sub_set(int argc, char *argv[])
{
    int i;

    if (argc < 3) {
        printf("usage: local-model-sub-set <model_index> <sub_addr>");
        return -EINVAL;
    }

    uint8_t model_idx = strtoul(argv[1], NULL, 0);

    if (model_idx >= sizeof(root_models) / sizeof(root_models[0])) {
        printf("err model_index\n");
        return -EINVAL;
    }

    uint16_t sub_addr = strtoul(argv[2], NULL, 0);

    if (!BT_MESH_ADDR_IS_GROUP(sub_addr)) {
        printf("err sub_addr range\n");
        return -EINVAL;
    }

    if (bt_mesh_model_find_group(&root_models[model_idx], sub_addr)) {
        printf("the sub addr already exist\n");
        return 0;
    }

    for (i = 0; i < ARRAY_SIZE(root_models[model_idx].groups); i++) {
        if (root_models[model_idx].groups[i] == BT_MESH_ADDR_UNASSIGNED) {
            root_models[model_idx].groups[i] = sub_addr;
            break;
        }
    }

    if (i == ARRAY_SIZE(root_models[model_idx].groups)) {
        printf("sub addr group already full\n");
        return -EINVAL;
    }

    if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
        bt_mesh_store_mod_sub(&root_models[model_idx]);
    }

    printf("add sub addr success\n");
    return 0;
}

static int cmd_local_model_sub_del(int argc, char *argv[])
{
    u16_t *addr;

    if (argc < 3) {
        printf("usage: local-model-sub-del <model_index> <sub_addr>");
        return -EINVAL;
    }

    u8_t model_idx = strtoul(argv[1], NULL, 0);

    if (model_idx >= sizeof(root_models) / sizeof(root_models[0])) {
        printf("err model_index\n");
        return -EINVAL;
    }

    u16_t sub_addr = strtoul(argv[2], NULL, 0);

    if (!BT_MESH_ADDR_IS_GROUP(sub_addr)) {
        printf("err sub_addr range\n");
        return -EINVAL;
    }

    addr = bt_mesh_model_find_group(&root_models[model_idx], sub_addr);

    if (NULL == addr) {
        printf("the sub addr not exist\n");
        return EINVAL;
    } else {
        *addr = BT_MESH_ADDR_UNASSIGNED;
    }

    if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
        bt_mesh_store_mod_sub(&root_models[model_idx]);
    }

    printf("del sub addr success\n");
    return 0;
}

static int cmd_local_model_pub_set(int argc, char *argv[])
{
    uint16_t interval;
    uint8_t model_idx, count;

    if (argc < 9) {
        printf(
            "usage: local-model-pub-set <modex_index> <PubAddr> <AppKeyIndex> <cred> <ttl> <period> <count> <interval>\n");
        return -EINVAL;
    }

    model_idx = strtoul(argv[1], NULL, 0);

    if (model_idx >= sizeof(root_models) / sizeof(root_models[0])) {
        printf("err model_index\n");
        return -EINVAL;
    }

    if (!root_models[model_idx].pub) {
        printf("model pub is null\n");
        return -EINVAL;
    }

    root_models[model_idx].pub->addr = strtoul(argv[2], NULL, 0);
    root_models[model_idx].pub->key = strtoul(argv[3], NULL, 0);
    root_models[model_idx].pub->cred = strtoul(argv[4], NULL, 0);
    ;
    root_models[model_idx].pub->ttl = strtoul(argv[5], NULL, 0);
    root_models[model_idx].pub->period = strtoul(argv[6], NULL, 0);

    count = strtoul(argv[7], NULL, 0);

    if (count > 7) {
        printf("Invalid retransmit count\n");
        return -EINVAL;
    }

    interval = strtoul(argv[8], NULL, 0);

    if (interval > (31 * 50) || (interval % 50)) {
        printf("Invalid retransmit interval %u\n", interval);
        return -EINVAL;
    }

    root_models[model_idx].pub->retransmit = BT_MESH_PUB_TRANSMIT(count, interval);

    if (root_models[model_idx].pub->update) {
        uint32_t period_ms;

        period_ms = bt_mesh_model_pub_period_get(&root_models[model_idx]);
        printf("period %u ms\n", period_ms);

        if (period_ms) {
            k_delayed_work_submit(&(root_models[model_idx].pub->timer), period_ms);
        } else {
            k_delayed_work_cancel(&(root_models[model_idx].pub->timer));
        }
    }

    if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
        bt_mesh_store_mod_pub(&root_models[model_idx]);
    }

    printf("set pub addr success\n");
    return 0;
}

static int local_model_send(struct bt_mesh_model *model, uint16_t addr, uint8_t ttl, const struct bt_mesh_send_cb *cb,
                            void *cb_data)
{
#ifdef CONFIG_BT_MESH_PROVISIONER

    if (!bt_mesh_is_provisioner_en()) {
#endif

        if (!bt_mesh_is_provisioned()) {
            printf("Local node is not yet provisioned\n");
            return -EAGAIN;
        }

#ifdef CONFIG_BT_MESH_PROVISIONER
    }

#endif

    NET_BUF_SIMPLE_DEFINE(sdu, BT_MESH_TX_SDU_MAX);
    struct bt_mesh_model_pub *pub = model->pub;
    struct bt_mesh_app_key *key;
    struct bt_mesh_msg_ctx ctx = {};
    struct bt_mesh_net_tx tx = {
        .ctx = &ctx,
        .src = bt_mesh_model_elem(model)->addr,
        .xmit = bt_mesh_net_transmit_get(),
    };
    int err;

    if (!pub) {
        return -ENOTSUP;
    }

#ifdef CONFIG_BT_MESH_PROVISIONER
    key = provisioner_app_key_find(pub->key);
#else
    key = bt_mesh_app_key_find(pub->key);
#endif

    if (!key) {
        return -EADDRNOTAVAIL;
    }

    if (pub->msg->len + 4 > BT_MESH_TX_SDU_MAX) {
        printf("Message does not fit maximum SDU size\n");
        return -EMSGSIZE;
    }

    net_buf_simple_add_mem(&sdu, pub->msg->data, pub->msg->len);

    ctx.addr = addr;
    ctx.send_ttl = ttl;
    ctx.net_idx = key->net_idx;
    ctx.app_idx = key->app_idx;

    tx.friend_cred = pub->cred;
    tx.sub = bt_mesh_subnet_get(ctx.net_idx);

    if (!tx.sub) {
        printf("No available subnet found\n");
        return -EINVAL;
    }

    if (net_buf_simple_tailroom(&sdu) < 4) {
        printf("Not enough tailroom for TransMIC\n");
        return -EINVAL;
    }

    if (sdu.len > BT_MESH_TX_SDU_MAX - 4) {
        printf("Too big message");
        return -EMSGSIZE;
    }

    err = bt_mesh_trans_send(&tx, &sdu, cb, cb_data);

    if (err) {
        return err;
    }

    return 0;
}

static int cmd_onoff_model_cli_send(int argc, char *argv[])
{
    int ret;
    uint8_t msg[2], ack, modex_index, ttl;
    uint16_t addr;

    if (argc < 5) {
        printf("usage: onoff-model-cli-send <model_index> <ack:1 unack: 0> <on:1 off:0> <addr> [ttl]");
        return -EINVAL;
    }

    modex_index = strtoul(argv[1], NULL, 0);

    if (modex_index > sizeof(root_models) / sizeof(root_models[0])) {
        printf("invalid modex_index\n");
        return -EINVAL;
    }

    if (root_models[modex_index].id != BT_MESH_MODEL_ID_GEN_ONOFF_CLI) {
        printf("the model is not a gen on_off model cli\n");
        return -EINVAL;
    }

    ack = strtoul(argv[2], NULL, 0);
    msg[0] = strtoul(argv[3], NULL, 0);
    addr = strtoul(argv[4], NULL, 0);
    msg[1] = tid;

    if (argc >= 6) {
        ttl = strtoul(argv[5], NULL, 0);
    } else {
        ttl = BT_MESH_TTL_DEFAULT;
    }

    struct net_buf_simple *msg_buf = root_models[4].pub->msg;

    if (1 == ack) {
        bt_mesh_model_msg_init(msg_buf, BT_MESH_MODEL_OP_2(0x82, 0x02));
    } else {
        bt_mesh_model_msg_init(msg_buf, BT_MESH_MODEL_OP_2(0x82, 0x03));
    }

    net_buf_simple_add_mem(msg_buf, msg, 2);

    ret = local_model_send(&root_models[modex_index], addr, ttl, NULL, NULL);

    if (ret) {
        printf("gen off pub fail %d", ret);
    }

    printf("SEND LED %s, TID %d\n", msg[0] ? "ON" : "OFF", tid);
    tid++;
    return 0;
}
#endif

#ifdef CONFIG_BT_MESH_PROVISIONER
static int cmd_prov_node_reset(int argc, char *argv[])
{
    int err;
    uint16_t dst_addr, net_idx;
    struct bt_mesh_node_t *node_temp = NULL;
    struct bt_mesh_device_delete del_dev;

    if (argc < 3) {
        printf("usage: prov-node-reset <net_idx> <dst_addr>");
        return -EINVAL;
    }

    net_idx = strtoul(argv[1], NULL, 0);
    dst_addr = strtoul(argv[2], NULL, 0);

    err = bt_mesh_cfg_node_reset(net_idx, dst_addr);

    if (err) {
        printf("reset node failed\n");
        return err;
    }

    node_temp = bt_mesh_provisioner_get_node_info(dst_addr);

    if (NULL == node_temp) {
        printf("did not found the node\n");
        return -EINVAL;
    }

    memcpy(del_dev.addr, node_temp->addr_val, sizeof(node_temp->addr_val) / sizeof(node_temp->addr_val[0]));
    del_dev.addr_type = node_temp->addr_type;
    memcpy(del_dev.uuid, node_temp->dev_uuid, sizeof(node_temp->dev_uuid) / sizeof(node_temp->dev_uuid[0]));

    err = bt_mesh_provisioner_delete_device(&del_dev);

    if (err) {
        printf("delete local node info failed\n");
        return err;
    }

    printf("delete node success\n");

    printf("reset node success\n");
    return 0;
}

static int cmd_prov_node_del(int argc, char *argv[])
{
    struct bt_mesh_device_delete del_dev;
    struct bt_mesh_node_t *node_temp = NULL;
    int err;
    uint16_t dst_addr;

    if (argc < 2) {
        printf("usage: prov-node-del <dst_addr>");
        return -EINVAL;
    }

    dst_addr = strtoul(argv[1], NULL, 0);
    node_temp = bt_mesh_provisioner_get_node_info(dst_addr);

    if (NULL == node_temp) {
        printf("did not found the node\n");
        return -EINVAL;
    }

    memcpy(del_dev.addr, node_temp->addr_val, sizeof(node_temp->addr_val) / sizeof(node_temp->addr_val[0]));
    del_dev.addr_type = node_temp->addr_type;
    memcpy(del_dev.uuid, node_temp->dev_uuid, sizeof(node_temp->dev_uuid) / sizeof(node_temp->dev_uuid[0]));

    err = bt_mesh_provisioner_delete_device(&del_dev);

    if (err) {
        printf("delete local node info failed\n");
        return err;
    }

    printf("delete node success\n");
    return 0;
}

#endif
static int mod_pub_get(u16_t addr, u16_t mod_id, u16_t cid)
{
    struct bt_mesh_cfg_mod_pub pub;
    u8_t status;
    int err;

    if (cid == CID_NVAL) {
        err = bt_mesh_cfg_mod_pub_get(net.net_idx, net.dst, addr, mod_id, &pub, &status);
    } else {
        err = bt_mesh_cfg_mod_pub_get_vnd(net.net_idx, net.dst, addr, mod_id, cid, &pub, &status);
    }

    if (err) {
        printf("Model Publication Get failed (err %d)\n", err);
        return 0;
    }

    if (status) {
        printf("Model Publication Get failed (status 0x%02x)\n", status);
        return 0;
    }

    printf("Model Publication for Element 0x%04x, Model 0x%04x:\n"
           "\tPublish Address:                0x%04x\n"
           "\tAppKeyIndex:                    0x%04x\n"
           "\tCredential Flag:                %u\n"
           "\tPublishTTL:                     %u\n"
           "\tPublishPeriod:                  0x%02x\n"
           "\tPublishRetransmitCount:         %u\n"
           "\tPublishRetransmitInterval:      %ums\n",
           addr, mod_id, pub.addr, pub.app_idx, pub.cred_flag, pub.ttl, pub.period,
           BT_MESH_PUB_TRANSMIT_COUNT(pub.transmit), BT_MESH_PUB_TRANSMIT_INT(pub.transmit));

    return 0;
}

static int mod_pub_set(u16_t addr, u16_t mod_id, u16_t cid, char *argv[])
{
    struct bt_mesh_cfg_mod_pub pub;
    u8_t status, count;
    u16_t interval;
    int err;

    pub.addr = strtoul(argv[0], NULL, 0);
    pub.app_idx = strtoul(argv[1], NULL, 0);
    pub.cred_flag = str2bool(argv[2]);
    pub.ttl = strtoul(argv[3], NULL, 0);
    pub.period = strtoul(argv[4], NULL, 0);

    count = strtoul(argv[5], NULL, 0);

    if (count > 7) {
        printf("Invalid retransmit count\n");
        return -EINVAL;
    }

    interval = strtoul(argv[6], NULL, 0);

    if (interval > (31 * 50) || (interval % 50)) {
        printf("Invalid retransmit interval %u\n", interval);
        return -EINVAL;
    }

    pub.transmit = BT_MESH_PUB_TRANSMIT(count, interval);

    if (cid == CID_NVAL) {
        err = bt_mesh_cfg_mod_pub_set(net.net_idx, net.dst, addr, mod_id, &pub, &status);
    } else {
        err = bt_mesh_cfg_mod_pub_set_vnd(net.net_idx, net.dst, addr, mod_id, cid, &pub, &status);
    }

    if (err) {
        printf("Model Publication Set failed (err %d)\n", err);
        return 0;
    }

    if (status) {
        printf("Model Publication Set failed (status 0x%02x)\n", status);
    } else {
        printf("Model Publication successfully set\n");
    }

    return 0;
}

static int cmd_mod_pub(int argc, char *argv[])
{
    u16_t addr, mod_id, cid;

    if (argc < 3) {
        return -EINVAL;
    }

    addr = strtoul(argv[1], NULL, 0);
    mod_id = strtoul(argv[2], NULL, 0);

    argc -= 3;
    argv += 3;

    if (argc == 1 || argc == 8) {
        cid = strtoul(argv[0], NULL, 0);
        argc--;
        argv++;
    } else {
        cid = CID_NVAL;
    }

    if (argc > 0) {
        if (argc < 7) {
            return -EINVAL;
        }

        return mod_pub_set(addr, mod_id, cid, argv);
    } else {
        return mod_pub_get(addr, mod_id, cid);
    }
}

static void hb_sub_print(struct bt_mesh_cfg_hb_sub *sub)
{
    printf("Heartbeat Subscription:\n"
           "\tSource:      0x%04x\n"
           "\tDestination: 0x%04x\n"
           "\tPeriodLog:   0x%02x\n"
           "\tCountLog:    0x%02x\n"
           "\tMinHops:     %u\n"
           "\tMaxHops:     %u\n",
           sub->src, sub->dst, sub->period, sub->count, sub->min, sub->max);
}

static int hb_sub_get(int argc, char *argv[])
{
    struct bt_mesh_cfg_hb_sub sub;
    u8_t status;
    int err;

    err = bt_mesh_cfg_hb_sub_get(net.net_idx, net.dst, &sub, &status);

    if (err) {
        printf("Heartbeat Subscription Get failed (err %d)\n", err);
        return 0;
    }

    if (status) {
        printf("Heartbeat Subscription Get failed (status 0x%02x)\n", status);
    } else {
        hb_sub_print(&sub);
    }

    return 0;
}

static int hb_sub_set(int argc, char *argv[])
{
    struct bt_mesh_cfg_hb_sub sub;
    u8_t status;
    int err;

    sub.src = strtoul(argv[1], NULL, 0);
    sub.dst = strtoul(argv[2], NULL, 0);
    sub.period = strtoul(argv[3], NULL, 0);

    err = bt_mesh_cfg_hb_sub_set(net.net_idx, net.dst, &sub, &status);

    if (err) {
        printf("Heartbeat Subscription Set failed (err %d)\n", err);
        return 0;
    }

    if (status) {
        printf("Heartbeat Subscription Set failed (status 0x%02x)\n", status);
    } else {
        hb_sub_print(&sub);
    }

    return 0;
}

static int cmd_hb_sub(int argc, char *argv[])
{
    if (argc > 1) {
        if (argc < 4) {
            return -EINVAL;
        }

        return hb_sub_set(argc, argv);
    } else {
        return hb_sub_get(argc, argv);
    }
}

static int hb_pub_get(int argc, char *argv[])
{
    struct bt_mesh_cfg_hb_pub pub;
    u8_t status;
    int err;

    err = bt_mesh_cfg_hb_pub_get(net.net_idx, net.dst, &pub, &status);

    if (err) {
        printf("Heartbeat Publication Get failed (err %d)\n", err);
        return 0;
    }

    if (status) {
        printf("Heartbeat Publication Get failed (status 0x%02x)\n", status);
        return 0;
    }

    printf("Heartbeat publication:\n");
    printf("\tdst 0x%04x count 0x%02x period 0x%02x\n", pub.dst, pub.count, pub.period);
    printf("\tttl 0x%02x feat 0x%04x net_idx 0x%04x\n", pub.ttl, pub.feat, pub.net_idx);

    return 0;
}

static int hb_pub_set(int argc, char *argv[])
{
    struct bt_mesh_cfg_hb_pub pub;
    u8_t status;
    int err;

    pub.dst = strtoul(argv[1], NULL, 0);
    pub.count = strtoul(argv[2], NULL, 0);
    pub.period = strtoul(argv[3], NULL, 0);
    pub.ttl = strtoul(argv[4], NULL, 0);
    pub.feat = strtoul(argv[5], NULL, 0);
    pub.net_idx = strtoul(argv[6], NULL, 0);

    err = bt_mesh_cfg_hb_pub_set(net.net_idx, net.dst, &pub, &status);

    if (err) {
        printf("Heartbeat Publication Set failed (err %d)\n", err);
        return 0;
    }

    if (status) {
        printf("Heartbeat Publication Set failed (status 0x%02x)\n", status);
    } else {
        printf("Heartbeat publication successfully set\n");
    }

    return 0;
}

static int cmd_hb_pub(int argc, char *argv[])
{
    if (argc > 1) {
        if (argc < 7) {
            return -EINVAL;
        }

        return hb_pub_set(argc, argv);
    } else {
        return hb_pub_get(argc, argv);
    }
}

/*[Genie begin] add by wenbing.cwb at 2021-01-21*/
#ifdef CONFIG_BT_MESH_CTRL_RELAY
static int ctrl_relay_get(int argc, char *argv[])
{
    struct ctrl_relay_param cr;
    u8_t status;
    int err;

    err = bt_mesh_cfg_ctrl_relay_get(net.net_idx, net.dst, &cr, &status);
    if (err) {
        printf("Ctrl Relay Get failed (err %d)\n", err);
        return 0;
    }

    if (status) {
        printf("Ctrl Relay Get failed (status 0x%02x)\n", status);
        return 0;
    }

    printf("Ctrl Relay Cfg:\n");
    printf("enable:%d N:%d rssi:%d\n", cr.enable, cr.trd_n, cr.rssi);
    printf("status_period:%d check_period:%d request_period:%d\n", cr.sta_period, cr.chk_period, cr.req_period);

    return 0;
}

static int ctrl_relay_set(int argc, char *argv[])
{
    struct ctrl_relay_param cr;
    u8_t status;
    int err;

    cr.enable = strtoul(argv[1], NULL, 0);
    cr.trd_n = strtoul(argv[2], NULL, 0);
    cr.rssi = strtoul(argv[3], NULL, 0);
    cr.sta_period = strtoul(argv[4], NULL, 0);
    cr.chk_period = strtoul(argv[5], NULL, 0);
    cr.req_period = strtoul(argv[6], NULL, 0);

    err = bt_mesh_cfg_ctrl_relay_set(net.net_idx, net.dst, &cr, &status);
    if (err) {
        printf("Ctrl Relay Set failed (err %d)\n", err);
        return 0;
    }

    if (status) {
        printf("Ctrl Relay Set failed (status 0x%02x)\n", status);
    } else {
        printf("Ctrl Relay successfully set\n");
    }

    return 0;
}

static int cmd_ctrl_relay(int argc, char *argv[])
{
    if (argc > 1) {
        if (argc < 7) {
            return -EINVAL;
        }

        return ctrl_relay_set(argc, argv);
    } else {
        return ctrl_relay_get(argc, argv);
    }
}
#endif
/*[Genie end] add by wenbing.cwb at 2021-01-21*/

#if defined(CONFIG_BT_MESH_PROV)
static int cmd_pb(int bearer, int argc, char *argv[])
{
    int err;

    if (argc < 2) {
        return -EINVAL;
    }

    if (str2bool(argv[1])) {
        err = bt_mesh_prov_enable(bearer);

        if (err) {
            printf("Failed to enable %s (err %d)\n", bearer2str(bearer), err);
        } else {
            printf("%s enabled\n", bearer2str(bearer));
        }
    } else {
        err = bt_mesh_prov_disable(bearer);

        if (err) {
            printf("Failed to disable %s (err %d)\n", bearer2str(bearer), err);
        } else {
            printf("%s disabled\n", bearer2str(bearer));
        }
    }

    return 0;
}
#endif

#if defined(CONFIG_BT_MESH_PB_ADV)
static int cmd_pb_adv(int argc, char *argv[])
{
    return cmd_pb(BT_MESH_PROV_ADV, argc, argv);
}
#endif /* CONFIG_BT_MESH_PB_ADV */

#if defined(CONFIG_BT_MESH_PB_GATT)
static int cmd_pb_gatt(int argc, char *argv[])
{
    return cmd_pb(BT_MESH_PROV_GATT, argc, argv);
}
#endif /* CONFIG_BT_MESH_PB_GATT */

static int cmd_provision(int argc, char *argv[])
{
    u16_t net_idx, addr;
    bt_u32_t iv_index;
    int err;

    if (!bt_mesh_is_init()) {
        printf("mesh is not init, please send <blemesh init> command first\n");
        return -EINVAL;
    }

    if (argc < 3) {
        return -EINVAL;
    }

    net_idx = strtoul(argv[1], NULL, 0);
    addr = strtoul(argv[2], NULL, 0);

    if (argc > 3) {
        iv_index = strtoul(argv[3], NULL, 0);
    } else {
        iv_index = 0;
    }

    err = bt_mesh_provision(default_key, net_idx, 0, iv_index, addr, default_key);

    if (err) {
        printf("Provisioning failed (err %d)\n", err);
    }

    return 0;
}

int cmd_timeout(int argc, char *argv[])
{
    bt_s32_t timeout;

    if (argc < 2) {
        timeout = bt_mesh_cfg_cli_timeout_get();

        if (timeout == K_FOREVER) {
            printf("Message timeout: forever\n");
        } else {
            printf("Message timeout: %u seconds\n", timeout / 1000);
        }

        return 0;
    }

    timeout = strtol(argv[1], NULL, 0);

    if (timeout < 0 || timeout > (INT32_MAX / 1000)) {
        timeout = K_FOREVER;
    } else {
        timeout = timeout * 1000;
    }

    bt_mesh_cfg_cli_timeout_set(timeout);

    if (timeout == K_FOREVER) {
        printf("Message timeout: forever\n");
    } else {
        printf("Message timeout: %u seconds\n", timeout / 1000);
    }

    return 0;
}

#if defined(CONFIG_BT_MESH_HEALTH_CLI)
static int cmd_fault_get(int argc, char *argv[])
{
    u8_t faults[32];
    size_t fault_count;
    u8_t test_id;
    u16_t cid;
    int err;

    if (argc < 2) {
        return -EINVAL;
    }

    cid = strtoul(argv[1], NULL, 0);
    fault_count = sizeof(faults);

    err = bt_mesh_health_fault_get(net.net_idx, net.dst, net.app_idx, cid, &test_id, faults, &fault_count);

    if (err) {
        printf("Failed to send Health Fault Get (err %d)\n", err);
    } else {
        show_faults(test_id, cid, faults, fault_count);
    }

    return 0;
}

static int cmd_fault_clear(int argc, char *argv[])
{
    u8_t faults[32];
    size_t fault_count;
    u8_t test_id;
    u16_t cid;
    int err;

    if (argc < 2) {
        return -EINVAL;
    }

    cid = strtoul(argv[1], NULL, 0);
    fault_count = sizeof(faults);

    err = bt_mesh_health_fault_clear(net.net_idx, net.dst, net.app_idx, cid, &test_id, faults, &fault_count);

    if (err) {
        printf("Failed to send Health Fault Clear (err %d)\n", err);
    } else {
        show_faults(test_id, cid, faults, fault_count);
    }

    return 0;
}

static int cmd_fault_clear_unack(int argc, char *argv[])
{
    u16_t cid;
    int err;

    if (argc < 2) {
        return -EINVAL;
    }

    cid = strtoul(argv[1], NULL, 0);

    err = bt_mesh_health_fault_clear(net.net_idx, net.dst, net.app_idx, cid, NULL, NULL, NULL);

    if (err) {
        printf("Health Fault Clear Unacknowledged failed (err %d)\n", err);
    }

    return 0;
}

static int cmd_fault_test(int argc, char *argv[])
{
    u8_t faults[32];
    size_t fault_count;
    u8_t test_id;
    u16_t cid;
    int err;

    if (argc < 3) {
        return -EINVAL;
    }

    cid = strtoul(argv[1], NULL, 0);
    test_id = strtoul(argv[2], NULL, 0);
    fault_count = sizeof(faults);

    err = bt_mesh_health_fault_test(net.net_idx, net.dst, net.app_idx, cid, test_id, faults, &fault_count);

    if (err) {
        printf("Failed to send Health Fault Test (err %d)\n", err);
    } else {
        show_faults(test_id, cid, faults, fault_count);
    }

    return 0;
}

static int cmd_fault_test_unack(int argc, char *argv[])
{
    u16_t cid;
    u8_t test_id;
    int err;

    if (argc < 3) {
        return -EINVAL;
    }

    cid = strtoul(argv[1], NULL, 0);
    test_id = strtoul(argv[2], NULL, 0);

    err = bt_mesh_health_fault_test(net.net_idx, net.dst, net.app_idx, cid, test_id, NULL, NULL);

    if (err) {
        printf("Health Fault Test Unacknowledged failed (err %d)\n", err);
    }

    return 0;
}

static int cmd_period_get(int argc, char *argv[])
{
    u8_t divisor;
    int err;

    err = bt_mesh_health_period_get(net.net_idx, net.dst, net.app_idx, &divisor);

    if (err) {
        printf("Failed to send Health Period Get (err %d)\n", err);
    } else {
        printf("Health FastPeriodDivisor: %u\n", divisor);
    }

    return 0;
}

static int cmd_period_set(int argc, char *argv[])
{
    u8_t divisor, updated_divisor;
    int err;

    if (argc < 2) {
        return -EINVAL;
    }

    divisor = strtoul(argv[1], NULL, 0);

    err = bt_mesh_health_period_set(net.net_idx, net.dst, net.app_idx, divisor, &updated_divisor);

    if (err) {
        printf("Failed to send Health Period Set (err %d)\n", err);
    } else {
        printf("Health FastPeriodDivisor: %u\n", updated_divisor);
    }

    return 0;
}

static int cmd_period_set_unack(int argc, char *argv[])
{
    u8_t divisor;
    int err;

    if (argc < 2) {
        return -EINVAL;
    }

    divisor = strtoul(argv[1], NULL, 0);

    err = bt_mesh_health_period_set(net.net_idx, net.dst, net.app_idx, divisor, NULL);

    if (err) {
        printf("Failed to send Health Period Set (err %d)\n", err);
    }

    return 0;
}

static int cmd_attention_get(int argc, char *argv[])
{
    u8_t attention;
    int err;

    err = bt_mesh_health_attention_get(net.net_idx, net.dst, net.app_idx, &attention);

    if (err) {
        printf("Failed to send Health Attention Get (err %d)\n", err);
    } else {
        printf("Health Attention Timer: %u\n", attention);
    }

    return 0;
}

static int cmd_attention_set(int argc, char *argv[])
{
    u8_t attention, updated_attention;
    int err;

    if (argc < 2) {
        return -EINVAL;
    }

    attention = strtoul(argv[1], NULL, 0);

    err = bt_mesh_health_attention_set(net.net_idx, net.dst, net.app_idx, attention, &updated_attention);

    if (err) {
        printf("Failed to send Health Attention Set (err %d)\n", err);
    } else {
        printf("Health Attention Timer: %u\n", updated_attention);
    }

    return 0;
}

static int cmd_attention_set_unack(int argc, char *argv[])
{
    u8_t attention;
    int err;

    if (argc < 2) {
        return -EINVAL;
    }

    attention = strtoul(argv[1], NULL, 0);

    err = bt_mesh_health_attention_set(net.net_idx, net.dst, net.app_idx, attention, NULL);

    if (err) {
        printf("Failed to send Health Attention Set (err %d)\n", err);
    }

    return 0;
}
#endif

static int cmd_add_fault(int argc, char *argv[])
{
    u8_t fault_id;
    u8_t i;

    if (argc < 2) {
        return -EINVAL;
    }

    fault_id = strtoul(argv[1], NULL, 0);

    if (!fault_id) {
        printf("The Fault ID must be non-zero!\n");
        return -EINVAL;
    }

    for (i = 0; i < sizeof(cur_faults); i++) {
        if (!cur_faults[i]) {
            cur_faults[i] = fault_id;
            break;
        }
    }

    if (i == sizeof(cur_faults)) {
        printf("Fault array is full. Use \"del-fault\" to clear it\n");
        return 0;
    }

    for (i = 0; i < sizeof(reg_faults); i++) {
        if (!reg_faults[i]) {
            reg_faults[i] = fault_id;
            break;
        }
    }

    if (i == sizeof(reg_faults)) {
        printf("No space to store more registered faults\n");
    }

    //bt_mesh_fault_update(&elements[0]);

    return 0;
}

static int cmd_del_fault(int argc, char *argv[])
{
    u8_t fault_id;
    u8_t i;

    if (argc < 2) {
        memset(cur_faults, 0, sizeof(cur_faults));
        printf("All current faults cleared\n");
        //bt_mesh_fault_update(&elements[0]);
        return 0;
    }

    fault_id = strtoul(argv[1], NULL, 0);

    if (!fault_id) {
        printf("The Fault ID must be non-zero!\n");
        return -EINVAL;
    }

    for (i = 0; i < sizeof(cur_faults); i++) {
        if (cur_faults[i] == fault_id) {
            cur_faults[i] = 0;
            printf("Fault cleared\n");
        }
    }

    //bt_mesh_fault_update(&elements[0]);

    return 0;
}

typedef int (*shell_cmd_function_t)(int argc, char *argv[]);

struct shell_cmd {
    const char *cmd_name;
    shell_cmd_function_t cb;
    const char *help;
    const char *desc;
};

static int str2bt_addr(const char *str, bt_addr_le_t *addr)
{
    int i, j;
    u8_t tmp;

    if (strlen(str) != 17) {
        return -EINVAL;
    }

    for (i = 5, j = 1; *str != '\0'; str++, j++) {
        if (!(j % 3) && (*str != ':')) {
            return -EINVAL;
        } else if (*str == ':') {
            i--;
            continue;
        }

        addr->a.val[i] = addr->a.val[i] << 4;

        if (char2hex(*str, &tmp) < 0) {
            return -EINVAL;
        }

        addr->a.val[i] |= tmp;
    }

    return 0;
}

int str2bt_addr_le(const char *str, const char *type, bt_addr_le_t *addr)
{
    int err;

    err = str2bt_addr(str, addr);

    if (err < 0) {
        return err;
    }

    if (!strcmp(type, "public") || !strcmp(type, "(public)")) {
        addr->type = BT_ADDR_LE_PUBLIC;
    } else if (!strcmp(type, "random") || !strcmp(type, "(random)")) {
        addr->type = BT_ADDR_LE_RANDOM;
    } else {
        return -EINVAL;
    }

    return 0;
}

static char *addr_le_str(const bt_addr_le_t *addr)
{
    static char bufs[2][27];
    static u8_t cur;
    char *str;

    str = bufs[cur++];
    cur %= ARRAY_SIZE(bufs);
    bt_addr_le_to_str(addr, str, sizeof(bufs[cur]));

    return str;
}

#if defined(CONFIG_BT_MESH_PROVISIONER)
static uint8_t show_unprovison_dev = 0;
static void unprovison_dev_found(const u8_t addr[6], const u8_t addr_type, const u8_t adv_type, const u8_t dev_uuid[16],
                                 u16_t oob_info, bt_mesh_prov_bearer_t bearer)
{
    if (show_unprovison_dev) {
        struct bt_uuid_128 uuid = { { BT_UUID_TYPE_128 }, { 0 } };
        bt_addr_le_t btaddr;
        memcpy(uuid.val, dev_uuid, 16);
        btaddr.type = addr_type;
        memcpy(btaddr.a.val, addr, 6);
        printf("unprovisioned device: %s adv type:%d UUID: %s oob_info: 0x%x bearer: 0x%x\n", bt_addr_le_str(&btaddr),
               adv_type, bt_uuid_str(&uuid.uuid), oob_info, bearer);
    }
}

static int cmd_provisioner(int argc, char *argv[])
{
    int err;
    int bearer = 0;
    int onoff = 0;

    if (argc < 4) {
        return -EINVAL;
    }

    bearer = strtoul(argv[1], NULL, 0);
    onoff = strtoul(argv[2], NULL, 0);
    show_unprovison_dev = strtoul(argv[3], NULL, 0);

    if (onoff) {
        err = bt_mesh_provisioner_local_provision();
        if (err && err != -EALREADY) {
            return err;
        }

        err = bt_mesh_provisioner_enable(bearer);

        if (err && err != -EALREADY) {
            return err;
        }

        if (err == -EALREADY) {
            printf("provisioner %s success\n", onoff ? "enable" : "disable");
            return 0;
        }

        bt_mesh_prov_adv_pkt_cb_register(unprovison_dev_found);
    } else {
        err = bt_mesh_provisioner_disable(bearer);

        if (err == -EALREADY) {
            printf("provisioner %s success\n", onoff ? "enable" : "disable");
            return 0;
        }

        if (err) {
            return err;
        }
    }

    printf("provisioner %s success\n", onoff ? "enable" : "disable");
    return 0;
}

static int cmd_add_unprov_dev(int argc, char *argv[])
{
    bt_addr_le_t addr;
    int err;
    uint8_t flag;
    struct bt_mesh_unprov_dev_add unprov_dev = { 0 };

    if (argc < 7) {
        return -EINVAL;
    }

    err = str2bt_addr_le(argv[1], argv[2], &addr);

    if (err) {
        printf("Invalid address\n");
        return err;
    }

    unprov_dev.addr_type = addr.type;
    memcpy(unprov_dev.addr, addr.a.val, 6);

    size_t len;

    len = hex2bin(argv[3], strlen(argv[3]), unprov_dev.uuid, sizeof(unprov_dev.uuid));

    if (len < 1) {
        return -EINVAL;
    }

    unprov_dev.oob_info = strtoul(argv[4], NULL, 16);
    unprov_dev.bearer = strtoul(argv[5], NULL, 16);
    flag = strtoul(argv[6], NULL, 0);

    err = bt_mesh_provisioner_add_unprov_dev(&unprov_dev, flag);

    if (err) {
        printf("add unprov device fail err %d\n", err);
        return err;
    }

    printf("add unprov device success!\n");
    return 0;
}

static int cmd_provisioner_input_str(int argc, char *argv[])
{
    prov_input_size = strlen(argv[1]);

    memcpy(prov_input, argv[1], prov_input_size);

    k_sem_give(&prov_input_sem);
    return 0;
}

static int cmd_provisioner_input_num(int argc, char *argv[])
{
    bt_u32_t num = 0;
    u8_t i;

    num = strtoul(argv[1], NULL, 0);

    memset(prov_input, 0, sizeof(prov_input));

    for (i = prov_input_size; i > 0; i--) {
        prov_input[i - 1] = num & 0xFF;
        num >>= 8;
    }

    k_sem_give(&prov_input_sem);
    return 0;
}

extern void send_white_list(u8_t index);
static int cmd_white_list(int argc, char *argv[])
{
    u8_t index = strtoul(argv[1], NULL, 0);
    send_white_list(index);
    return 0;
}

static int cmd_conn(int argc, char *argv[])
{
    //my_test_func();
    return 0;
}

static int prov_netkey_add(int argc, char *argv[])
{
    int len;
    u8_t netkey[16];
    u16_t netkey_idx = 0;
    int err;

    if (argc < 2) {
        err = bt_mesh_provisioner_local_net_key_add(NULL, &netkey_idx);

        if (err) {
            printf("add netkey err %d\n", err);
            return err;
        }

        printf("add netkey success, netkey id %d, netkey %s\n", netkey_idx,
               bt_hex(bt_mesh_provisioner_local_net_key_get(netkey_idx), 16));
        return 0;
    }

    if (argc < 3) {
        return -EINVAL;
    }

    netkey_idx = strtoul(argv[1], NULL, 0);
    len = hex2bin(argv[2], strlen(argv[2]), netkey, sizeof(netkey));

    if (len != sizeof(netkey)) {
        return -EINVAL;
    }

    err = bt_mesh_provisioner_local_net_key_add(netkey, &netkey_idx);

    if (err) {
        printf("add netkey err %d\n", err);
        return err;
    }

    printf("add netkey success, netkey id %d %s\n", netkey_idx,
           bt_hex(bt_mesh_provisioner_local_net_key_get(netkey_idx), 16));
    return 0;
}

static int prov_netkey_get(int argc, char *argv[])
{
    const u8_t *netkey = NULL;
    u16_t netkey_idx = 0;

    if (argc < 2) {
        return -EINVAL;
    }

    netkey_idx = strtoul(argv[1], NULL, 0);

    netkey = bt_mesh_provisioner_local_net_key_get(netkey_idx);

    if (!netkey) {
        printf("get netkey err\n");
        return -EIO;
    }

    printf("get netkey success, netkey(%d) %s\n", netkey_idx, bt_hex(netkey, 16));
    return 0;
}

static int prov_netkey_rm(int argc, char *argv[])
{
    u16_t netkey_idx = 0;
    int err;

    if (argc < 2) {
        return -EINVAL;
    }

    netkey_idx = strtoul(argv[1], NULL, 0);

    err = bt_mesh_provisioner_local_net_key_delete(netkey_idx);

    if (err) {
        printf("remove netkey err %d\n", err);
        return -EIO;
    }

    printf("remove netkey success\n");
    return 0;
}

static int prov_appkey_add(int argc, char *argv[])
{
    int len;
    u8_t appkey[16];
    u16_t netkey_idx = 0;
    u16_t appkey_idx = 0;
    int err;

    if (argc < 2) {
        return -EINVAL;
    }

    if (argc < 3) {
        netkey_idx = strtoul(argv[1], NULL, 0);
        err = bt_mesh_provisioner_local_app_key_add(NULL, netkey_idx, &appkey_idx);

        if (err) {
            printf("add appkey err %d\n", err);
            return err;
        }

        printf("add appkey success, netkey id %d, appkey id %d, appkey %s\n", netkey_idx, appkey_idx,
               bt_hex(bt_mesh_provisioner_local_app_key_get(netkey_idx, appkey_idx), 16));
        return 0;
    }

    if (argc < 4) {
        return -EINVAL;
    }

    netkey_idx = strtoul(argv[1], NULL, 0);
    appkey_idx = strtoul(argv[2], NULL, 0);

    len = hex2bin(argv[3], strlen(argv[3]), appkey, sizeof(appkey));

    if (len != sizeof(appkey)) {
        return -EINVAL;
    }

    err = bt_mesh_provisioner_local_app_key_add(appkey, netkey_idx, &appkey_idx);

    if (err) {
        printf("add appkey err %d\n", err);
        return err;
    }

    printf("add appkey success, netkey id %d, appkey id %d, appkey %s\n", netkey_idx, appkey_idx,
           bt_hex(bt_mesh_provisioner_local_app_key_get(netkey_idx, appkey_idx), 16));
    return 0;
}

static int prov_appkey_get(int argc, char *argv[])
{
    const u8_t *appkey = NULL;
    u16_t appkey_id = 0;
    u16_t netkey_idx = 0;

    if (argc < 3) {
        return -EINVAL;
    }

    netkey_idx = strtoul(argv[1], NULL, 0);
    appkey_id = strtoul(argv[1], NULL, 0);

    appkey = bt_mesh_provisioner_local_app_key_get(netkey_idx, appkey_id);

    if (!appkey) {
        printf("get appkey err\n");
        return -EIO;
    }

    printf("get appkey success, netkey id %d, appkey id %d, appkey %s\n", netkey_idx, appkey_id, bt_hex(appkey, 16));
    return 0;
}

static int prov_appkey_rm(int argc, char *argv[])
{
    u16_t appkey_id = 0;
    u16_t netkey_idx = 0;
    int err;

    if (argc < 3) {
        return -EINVAL;
    }

    netkey_idx = strtoul(argv[1], NULL, 0);
    appkey_id = strtoul(argv[2], NULL, 0);

    err = bt_mesh_provisioner_local_app_key_delete(netkey_idx, appkey_id);

    if (err) {
        printf("remove appkey err %d\n", err);
        return -EIO;
    }

    printf("remove appkey success\n");
    return 0;
}

static int prov_local_info(int argc, char *argv[])
{
    bt_mesh_provisioner_print_local_element_info();
    bt_mesh_provisioner_print_node_info();
    return 0;
}

static int prov_appkey_bind(int argc, char *argv[])
{
    u16_t elem_addr;
    u16_t mod_id;
    u16_t cid;
    u16_t app_idx;
    int err;

    if (argc < 5) {
        return -EINVAL;
    }

    elem_addr = strtoul(argv[1], NULL, 16);
    mod_id = strtoul(argv[2], NULL, 16);
    cid = strtoul(argv[3], NULL, 16);
    app_idx = strtoul(argv[4], NULL, 16);

    err = bt_mesh_provisioner_bind_local_model_app_idx(elem_addr, mod_id, cid, app_idx);

    if (err) {
        return err;
    }

    if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
        bt_mesh_store_mod_bind(&root_models[4]);
    }

    printf("appkey model bind success!\n");
    return 0;
}

static int prov_netkey_bind(int argc, char *argv[])
{
    u16_t netkey_idx;
    u16_t appkey_idx;
    int err;

    if (argc < 3) {
        return -EINVAL;
    }

    netkey_idx = strtoul(argv[1], NULL, 16);
    appkey_idx = strtoul(argv[2], NULL, 16);

    err = bt_mesh_provisioner_bind_local_app_net_idx(netkey_idx, appkey_idx);

    if (err) {
        return err;
    }

    printf("appkey netkey bind success!!");
    return 0;
}
#endif

static int cmd_id_create(int argc, char *argv[])
{
    bt_addr_le_t addr;
    int err;

    if (argc > 1) {
        err = str2bt_addr_le(argv[1], "random", &addr);

        if (err) {
            printf("Invalid address\n");
            return err;
        }
    } else {
        bt_addr_le_copy((bt_addr_le_t *)&addr, BT_ADDR_LE_ANY);
    }

    err = bt_id_create((bt_addr_le_t *)&addr, NULL);

    if (err < 0) {
        printf("Creating new ID failed (err %d)\n", err);
        return 0;
    }

    printf("New identity (%d) created: %s\n", err, addr_le_str((bt_addr_le_t *)&addr));

    return 0;
}

#define HELP_ADDR_LE "<address: XX:XX:XX:XX:XX:XX> <type: (public|random)>"

static const struct shell_cmd mesh_commands[] = {
    { "tx", cmd_tx_power, "<power>" },
    { "init", cmd_init, NULL },
    { "id-create", cmd_id_create, "[addr]" },
    { "timeout", cmd_timeout, "[timeout in seconds]" },
#if defined(CONFIG_BT_MESH_PB_ADV)
    { "pb-adv", cmd_pb_adv, "<val: off, on>" },
#endif
#if defined(CONFIG_BT_MESH_PB_GATT)
    { "pb-gatt", cmd_pb_gatt, "<val: off, on>" },
#endif
    { "reset", cmd_reset, NULL },
    { "uuid", cmd_uuid, "<UUID: 1-16 hex values>" },
    { "input-num", cmd_input_num, "<number>" },
    { "input-str", cmd_input_str, "<string>" },
    { "static-oob", cmd_static_oob, "[val: 1-16 hex values]" },
    { "oob-mode", cmd_oob_mode,
      "<mode:0 NO_OOB 1 STATIC 2 OUTPUT 3 INPUT> [action: 4 ENTER_NUM 8 ENTER_STR|OUTPUT_NUM 16 OUTPUT_STR]" },
    { "provision", cmd_provision, "<NetKeyIndex> <addr> [IVIndex]" },
#if defined(CONFIG_BT_MESH_LOW_POWER)
    { "lpn", cmd_lpn, "<value: off, on>" },
    { "poll", cmd_poll, NULL },

    { "group-add", cmd_group_add, "<value>" },
    { "group-del", cmd_group_del, "<value>" },
    { "friend-clear", cmd_friend_clear, "<value:true,false>" },

#endif
#if defined(CONFIG_BT_MESH_GATT_PROXY)
    { "ident", cmd_ident, "<val: off, on>" },
#endif
    { "dst", cmd_dst, "[destination address]" },
    { "netidx", cmd_netidx, "[NetIdx]" },
    { "appidx", cmd_appidx, "[AppIdx]" },

    /* Commands which access internal APIs, for testing only */
    { "net-send", cmd_net_send, "<hex string>" },
#if defined(CONFIG_BT_MESH_IV_UPDATE_TEST)
    { "iv-update", cmd_iv_update, NULL },
    { "iv-update-test", cmd_iv_update_test, "<value: off, on>" },
#endif
    { "rpl-clear", cmd_rpl_clear, NULL },

    /* Configuration Client Model operations */
    { "get-comp", cmd_get_comp, "[page]" },
    { "beacon", cmd_beacon, "[val: off, on]" },
    { "ttl", cmd_ttl, "[ttl: 0x00, 0x02-0x7f]" },
    { "friend", cmd_friend, "[val: off, on]" },
    { "gatt-proxy", cmd_gatt_proxy, "[val: off, on]" },
    { "relay", cmd_relay, "[val: off, on] [count: 0-7] [interval: 0-32]" },
    { "net-key-add", cmd_net_key_add, "<NetKeyIndex> [val]" },
    { "app-key-add", cmd_app_key_add, "<NetKeyIndex> <AppKeyIndex> [val]" },
    { "mod-app-bind", cmd_mod_app_bind, "<addr> <AppIndex> <Model ID> [Company ID]" },
    { "mod-pub", cmd_mod_pub,
      "<addr> <mod id> [cid] [<PubAddr> "
      "<AppKeyIndex> <cred> <ttl> <period> <count> <interval>]" },
    { "mod-sub-get", cmd_mod_sub_get, "<elem addr> <Model ID> [Company ID]" },
    { "mod-sub-add", cmd_mod_sub_add, "<elem addr> <sub addr> <Model ID> [Company ID]" },
    { "mod-sub-del", cmd_mod_sub_del, "<elem addr> <sub addr> <Model ID> [Company ID]" },
    { "mod-sub-add-va", cmd_mod_sub_add_va, "<elem addr> <Label UUID> <Model ID> [Company ID]" },
    { "mod-sub-del-va", cmd_mod_sub_del_va, "<elem addr> <Label UUID> <Model ID> [Company ID]" },
    { "hb-sub", cmd_hb_sub, "[<src> <dst> <period>]" },
    { "hb-pub", cmd_hb_pub, "[<dst> <count> <period> <ttl> <features> <NetKeyIndex>]" },
/*[Genie begin] add by wenbing.cwb at 2021-01-21*/
#ifdef CONFIG_BT_MESH_CTRL_RELAY
    { "ctrl-relay", cmd_ctrl_relay, "[<enable> <N> <rssi> <status period> <check period> <request peroid>]" },
#endif
/*[Genie end] add by wenbing.cwb at 2021-01-21*/
#ifdef CONFIG_BT_MESH_PROVISIONER
    { "net-key-update", cmd_net_key_update, "<net_idx>" },
    { "app-key-update", cmd_app_key_update, "<send_net_idx> <send_dst> <key_net_idx> <key_app_idx>" },
    { "krp-set", cmd_krp_set, "<send_net_idx> <send_dst> <key_net_idx> <phase>" },
    { "krp-get", cmd_krp_get, "<send_net_idx> <send_dst> <key_net_idx>" },
    { "local-revoke-keys", cmd_local_revoke_keys, "<key_net_idx> <app_key_idx>" },
#endif
#if defined(CONFIG_BT_MESH_HEALTH_CLI)
    /* Health Client Model Operations */
    { "fault-get", cmd_fault_get, "<Company ID>" },
    { "fault-clear", cmd_fault_clear, "<Company ID>" },
    { "fault-clear-unack", cmd_fault_clear_unack, "<Company ID>" },
    { "fault-test", cmd_fault_test, "<Company ID> <Test ID>" },
    { "fault-test-unack", cmd_fault_test_unack, "<Company ID> <Test ID>" },
    { "period-get", cmd_period_get, NULL },
    { "period-set", cmd_period_set, "<divisor>" },
    { "period-set-unack", cmd_period_set_unack, "<divisor>" },
    { "attention-get", cmd_attention_get, NULL },
    { "attention-set", cmd_attention_set, "<timer>" },
    { "attention-set-unack", cmd_attention_set_unack, "<timer>" },
#endif
    /* Health Server Model Operations */
    { "add-fault", cmd_add_fault, "<Fault ID>" },
    { "del-fault", cmd_del_fault, "[Fault ID]" },

#if defined(CONFIG_BT_MESH_PROVISIONER)
    { "provisioner", cmd_provisioner,
      "<bearer: 0x01 PB-ADV 0x02 PB-GATT 0x03 PB-ADV|PB-GATT> <onoff:0 off 1 on> <show unprovison dev: 0 off 1 on>" },
    { "add_unprov_dev", cmd_add_unprov_dev, HELP_ADDR_LE " <uuid> <oob_info> <bearer> <flag>" },
    { "pinput-str", cmd_provisioner_input_str, "<string>" },
    { "pinput-num", cmd_provisioner_input_num, "<number>" },
    { "wl", cmd_white_list, "cmd_white_list" },
    { "conn", cmd_conn, "cmd_conn" },
    { "prov-netkey-add", prov_netkey_add, "[netkey idx] [netkey]" },
    { "prov-netkey-get", prov_netkey_get, "<netkey idx>" },
    { "prov-netkey-rm", prov_netkey_rm, " <netkey idx>" },
    { "prov-appkey-add", prov_appkey_add, "<netkey idx> [appkey idx] [appkey]" },
    { "prov-appkey-get", prov_appkey_get, "<netkey idx> <appkey idx>" },
    { "prov-appkey-rm", prov_appkey_rm, "<netkey idx> <appkey idx>" },
    { "prov-local_info", prov_local_info, NULL },
    { "prov-appkey-model-bind", prov_appkey_bind, "<elem_addr> <model id> <Copmany id> <appkey idx>" },
    { "prov-appkey-netkey-bind", prov_netkey_bind, "<netkey idx> <appkey idx>" },
    { "prov-node-reset", cmd_prov_node_reset, "<netkey idx> <dst_addr>" },
    { "prov-node-del", cmd_prov_node_del, "<dst_addr>" },
    { "local-model-sub-set", cmd_local_model_sub_set, "<model_index> <sub_addr>" },
    { "local-model-sub-del", cmd_local_model_sub_del, "<model_index> <sub_addr>" },
    { "local-model-pub-set", cmd_local_model_pub_set,
      "<modex_index> <PubAddr> <AppKeyIndex> <cred> <ttl> <period> <count> <interval>" },
    { "onoff-model-cli-send", cmd_onoff_model_cli_send, "<modex_index> <ack:1 unack: 0> <on:1 off:0> <addr> [ttl]" },
#endif
    { NULL, NULL, NULL }
};

static void cmd_blemesh_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    int i = 0;
    int err;

    if (argc < 2) {
        printf("Ble mesh support commands\n");

        for (i = 0; mesh_commands[i].cmd_name != NULL; i++) {
            printf("    %s %s\n", mesh_commands[i].cmd_name, mesh_commands[i].help);
        }

        return;
    }

    for (i = 0; mesh_commands[i].cmd_name != NULL; i++) {
        if (strlen(mesh_commands[i].cmd_name) == strlen(argv[1]) &&
            !strncmp(mesh_commands[i].cmd_name, argv[1], strlen(mesh_commands[i].cmd_name))) {
            if (mesh_commands[i].cb) {
                err = mesh_commands[i].cb(argc - 1, &argv[1]);

                if (err) {
                    printf("%s execute fail, %d\n", mesh_commands[i].cmd_name, err);
                }

                break;
            }
        }
    }
}

void cli_reg_cmd_blemesh(void)
{
    static const struct cli_command cmd_info = {
        "blemesh",
        "blemesh commands",
        cmd_blemesh_func,
    };

    aos_cli_register_command(&cmd_info);
}
