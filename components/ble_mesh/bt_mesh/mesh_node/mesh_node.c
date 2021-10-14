/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>

#include <api/mesh.h>
#include <aos/debug.h>
#include <aos/kernel.h>
#include <aos/kv.h>
#include <aos/ble.h>
#include <bt_errno.h>
#include <mesh.h>
#include "mesh_node.h"
#include "mesh_model/mesh_model.h"

#ifdef CONFIG_BT_MESH_PROVISIONER
#include "provisioner_prov.h"
#include "provisioner_main.h"
#include "provisioner_proxy.h"
#endif

#define COMMON_SUB_ADDR 0xF000
#define COMMON_PUB_ADDR 0xF001
#define TIMEOUT 402

static uint8_t dev_uuid[16];
static uint8_t g_static_data[16] = { 0 };

#define TAG "BT_MESH_NODE"

#ifdef BT_MESH_BEACON_RSSI_REPORT
typedef struct {
    uint8_t mac[6];
    uint8_t rssi;
    uint8_t rssi_calibration;
} beacon_rssi_data;

typedef struct {
    uint8_t front;
    uint8_t rear;
    uint8_t length;
    beacon_rssi_data data[16];
} rssi_data_queue;
#endif
typedef void (*node_event_cb)(uint16_t event, void *p_arg);

typedef struct _mesh_node {
    const struct bt_mesh_comp *comp;
    model_event_cb user_model_cb;
    uint8_t init_flag;
#ifdef CONFIG_BT_MESH_PROVISIONER
    provisioner_cb user_provisioner_cb;
    k_timer_t add_appkey_timer;
    uint32_t add_appkey_timeout;
    uint8_t app_key_add_retry;
    uint16_t auto_add_appkey_idx;
#else
    prov_event_cb user_prov_cb;
    k_timer_t auto_config_timer;
    uint32_t auto_config_timeout;
    uint8_t app_key_add_flag:1;
    uint8_t auto_config_retry;
    uint8_t auto_config_ack_flag;
    uint8_t auto_config_status;
    uint16_t prov_addr;
    bt_mesh_input_action_t input_act;
    uint8_t input_size;
#endif

#ifdef BT_MESH_BEACON_RSSI_REPORT
    uint8_t rssi_report_enable:1;
    k_timer_t beacon_rssi_timer;
    rssi_data_queue rssi_queue;
#endif
    uint8_t node_provisioned;
    health_srv_cb health_cb;
} mesh_node;

mesh_node g_mesh_node;

#ifdef BT_MESH_BEACON_RSSI_REPORT
static int init_rssi_report();
#endif

extern void bt_mesh_store_mod_bind(struct bt_mesh_model *mod);
extern void bt_mesh_store_mod_sub(struct bt_mesh_model *mod);
extern void bt_mesh_store_mod_pub(struct bt_mesh_model *mod);
extern u16_t *bt_mesh_model_find_group(struct bt_mesh_model *mod, u16_t addr);
extern int bt_mesh_store_mod_pending_check();

#ifndef CONFIG_BT_MESH_PROVISIONER

static int report_auto_config_status(uint16_t dst_addr, uint8_t status)
{
#ifdef CONFIG_BT_MESH_MODEL_VENDOR_SRV
    static uint8_t tid = 0;
    struct bt_mesh_model *vnd_model = ble_mesh_model_find(0, BT_MESH_MODEL_VND_MODEL_SRV, CONFIG_CID_TAOBAO);

    if (!vnd_model) {
        return -1;
    }

    vnd_model_msg vnd_msg = { 0 };
    vnd_msg.netkey_idx = 0;
    vnd_msg.appkey_idx = 0;
    vnd_msg.dst_addr = dst_addr;
    vnd_msg.tid = tid++;
    vnd_msg.model = vnd_model;
    vnd_msg.opid = VENDOR_OP_ATTR_MESH_AUTOCONFIG_STATUS;
    vnd_msg.len = 1;
    vnd_msg.data = (uint8_t *)&status;
    return ble_mesh_vendor_srv_model_msg_send(&vnd_msg);
#else
    return 0;
#endif
}
static void auto_config_timeout_cb(void *timer, void *arg)
{
    int16_t ret;

    k_timer_stop(&g_mesh_node.auto_config_timer);

    if (!bt_mesh_store_mod_pending_check()) {
        g_mesh_node.auto_config_status = BT_MESH_AUTO_CONFIG_SUCCEED;
        g_mesh_node.node_provisioned = 1;

        if (g_mesh_node.auto_config_ack_flag) {
            ret = report_auto_config_status(g_mesh_node.prov_addr, g_mesh_node.auto_config_status);

            if (ret) {
                LOGE(TAG, "report auto config status faild");
            }
        }

        ret = aos_kv_setint("AUTO_CONFIG", BT_MESH_AUTO_CONFIG_SUCCEED);

        if (ret) {
            LOGE(TAG, "save auto config status faild");
        }

#ifdef BT_MESH_BEACON_RSSI_REPORT

        if (g_mesh_node.rssi_report_enable) {
            init_rssi_report();
        }

#endif
    } else {
        LOGI(TAG, "store is pending");

        if (g_mesh_node.auto_config_retry) {
            k_timer_start(&g_mesh_node.auto_config_timer, g_mesh_node.auto_config_timeout);
            g_mesh_node.auto_config_retry--;
        }
    }
}
#endif

#ifdef CONFIG_BT_MESH_PROVISIONER

static uint16_t last_addr;

static struct bt_mesh_prov node_prov = {
    .uuid = dev_uuid,
    .link_open = NULL,
    .link_close = NULL,
    .complete = NULL,
    .reset = NULL,
    .static_val = g_static_data,
    .static_val_len = 0,
    .input_size = 0,
    .input_actions = 0,
    .input = NULL,
    .output_size = 0,
    .output_actions = 0,
    .output_number = NULL,
    .output_string = NULL,
};
#else
static void link_open(bt_mesh_prov_bearer_t bearer);
static void link_close(bt_mesh_prov_bearer_t bearer);
static void prov_complete(uint16_t net_idx, uint16_t addr);
static void prov_reset(void);
static int output_number(bt_mesh_output_action_t action, bt_u32_t number);
static int output_string(const char *str);
static int node_input(bt_mesh_input_action_t act, u8_t size);

static struct bt_mesh_prov node_prov = {
    .uuid = dev_uuid,
    .link_open = link_open,
    .link_close = link_close,
    .complete = prov_complete,
    .reset = prov_reset,
    .static_val = g_static_data,
    .static_val_len = 0,
    .input_size = 0,
    .input_actions = 0,
    .input = node_input,
    .output_size = 0,
    .output_actions = 0,
    .output_number = output_number,
    .output_string = output_string,
};
#endif

typedef struct auto_config_t {
    uint16_t mod_id;
    uint16_t cid;
    uint16_t appkey_idx;
    uint16_t sub_addr;
    uint16_t pub_addr;
} auto_config_t;

static void node_config(struct bt_mesh_model *mod, struct bt_mesh_elem *elem, bool vnd, bool primary, void *user_data)
{
    auto_config_t *config = user_data;

    if (!config) {
        return;
    }

    if (!vnd && (mod->id == BT_MESH_MODEL_ID_CFG_SRV || mod->id == BT_MESH_MODEL_ID_CFG_CLI ||
                 mod->id == BT_MESH_MODEL_ID_HEALTH_SRV || mod->id == BT_MESH_MODEL_ID_HEALTH_CLI)) {
        return;
    }

    if (!vnd) {
        if (config->mod_id != 0xffff && config->mod_id != mod->id) {
            return;
        }
    } else {
        if ((config->mod_id != 0xffff && config->cid != 0xffff) &&
            (config->mod_id != mod->vnd.id || config->cid != mod->vnd.company)) {
            return;
        }
    }

    bt_mesh_mod_bind(mod, config->appkey_idx);

    bt_mesh_mod_sub_add(mod, config->sub_addr);

    bt_mesh_mod_pub_set(mod, config->pub_addr);
}

static int16_t auto_config(auto_config_t *config)
{
    if (!config) {
        return -EINVAL;
    }

    LOGD(TAG, "bind appkey:%04x sub addr:%04x pub addr:%04x", config->appkey_idx, config->sub_addr, config->pub_addr);
    extern void bt_mesh_model_foreach(void (*func)(struct bt_mesh_model *mod, struct bt_mesh_elem *elem, bool vnd,
                                                   bool primary, void *user_data),
                                      void *user_data);
    bt_mesh_model_foreach(node_config, config);

    return 0;
}

#ifdef CONFIG_BT_MESH_PROVISIONER
int ble_mesh_node_appkey_add(uint16_t netkey_idx, uint16_t appkey_idx, uint16_t unicast_addr)
{
    const uint8_t *app_key;
    int16_t ret;

    app_key = bt_mesh_provisioner_local_app_key_get(netkey_idx, appkey_idx);

    if (NULL == app_key) {
        return -1;
    }

    ret = bt_mesh_cfg_app_key_add(0, unicast_addr, netkey_idx, appkey_idx, app_key, NULL);

    if (ret && TIMEOUT != ret) {
        LOGE(TAG, "mesh appkey add set status send fail %d", ret);
        return ret;
    }

    return 0;
}

int ble_mesh_node_model_autoconfig(uint16_t netkey_idx, uint16_t appkey_idx, uint16_t addr,
                                   model_auto_config_t auto_conf)
{
#ifdef CONFIG_BT_MESH_MODEL_VENDOR_SRV
    static uint8_t tid = 0;
    struct bt_mesh_model *vendor_model;
    vnd_model_msg vnd_msg = { 0 };
    int16_t ret;
    uint16_t send_data;
    vendor_model = ble_mesh_model_find(0, BT_MESH_MODEL_VND_MODEL_CLI, BT_MESH_MODEL_VND_COMPANY_ID);

    if (!vendor_model) {
        return -1;
    }

    send_data = auto_conf.sub_addr;
    vnd_msg.netkey_idx = netkey_idx;
    vnd_msg.appkey_idx = appkey_idx;
    vnd_msg.dst_addr = addr;
    vnd_msg.tid = tid++;
    vnd_msg.model = vendor_model;
    vnd_msg.len = 2;
    vnd_msg.opid = VENDOR_OP_ATTR_MESH_AUTOCONFIG;
    vnd_msg.data = (uint8_t *)&send_data;

    ret = ble_mesh_vendor_cli_model_msg_send(&vnd_msg);

    if (ret) {
        return -1;
    }
#endif
    return 0;
}

static void add_appkey_timeout_cb(void *timer, void *arg)
{
    int16_t ret;
    k_timer_stop(&g_mesh_node.add_appkey_timer);

    if (g_mesh_node.app_key_add_retry) {
        ret = ble_mesh_node_appkey_add(0, g_mesh_node.auto_add_appkey_idx, last_addr);

        if (ret) {
            LOGE(TAG, "add appkey send faild");
        } else {
            g_mesh_node.app_key_add_retry--;
            k_timer_start(&g_mesh_node.add_appkey_timer, g_mesh_node.add_appkey_timeout);
        }

    } else {
        LOGE(TAG, "add appkey faild");
    }
}

static void provisioner_node_cb(mesh_provisioner_event_en event, void *p_arg)
{
    switch (event) {
        // provisioner message

    case BT_MESH_EVENT_PROV_COMP: {
        if (p_arg) {
            mesh_node_t *node = (mesh_node_t *)p_arg;
            uint8_t auto_add_appkey = 0;
            extern int ble_mesh_provisioner_get_add_appkey_flag(u16_t unicast_addr);
            auto_add_appkey = ble_mesh_provisioner_get_add_appkey_flag(node->prim_unicast);

            if (auto_add_appkey) {
                g_mesh_node.app_key_add_retry = DEF_ADD_APPKEY_RETRY;
                g_mesh_node.add_appkey_timeout = DEF_ADD_APPKEY_TIMEOUT;
                g_mesh_node.auto_add_appkey_idx = 0;
                last_addr = node->prim_unicast;
                k_timer_start(&g_mesh_node.add_appkey_timer, DEF_ADD_APPKEY_DELAY);
            }
        }
    } break;

    default:
        break;
    }

    if (g_mesh_node.user_provisioner_cb) {
        g_mesh_node.user_provisioner_cb(event, p_arg);
    }
}

#else

// __attribute__((unused)) static const char *bearer2str(int bearer)
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

static void link_open(bt_mesh_prov_bearer_t bearer)
{
    LOGI(TAG, "Provisioning link opened on %s\n", bearer2str(bearer));
}

static void link_close(bt_mesh_prov_bearer_t bearer)
{
    LOGI(TAG, "Provisioning link closed on %s\n", bearer2str(bearer));
}

static void prov_complete(uint16_t net_idx, uint16_t addr)
{
    LOGI(TAG, "provisioning complete netid %d, primary element address 0x%x", net_idx, addr);
    mesh_node_local_t node_info;
    node_info.prim_unicast = addr;
    g_mesh_node.auto_config_status = BT_MESH_AUTO_CONFIG_NOT_START;
    g_mesh_node.app_key_add_flag = 0;

    if (g_mesh_node.user_prov_cb) {
        g_mesh_node.user_prov_cb(BT_MESH_EVENT_NODE_PROV_COMP, &node_info);
    }
}

static void prov_reset(void)
{
    LOGD(TAG, "Node Reset");
    bt_mesh_prov_enable(BT_MESH_PROV_ADV | BT_MESH_PROV_GATT);

    if (g_mesh_node.user_prov_cb) {
        g_mesh_node.user_prov_cb(BT_MESH_EVENT_NODE_REST, NULL);
    }
}

static int output_number(bt_mesh_output_action_t action, bt_u32_t number)
{
    LOGI(TAG, "OOB Number: %u\n", number);
    return 0;
}

static int output_string(const char *str)
{
    LOGI(TAG, "OOB String: %s\n", str);
    return 0;
}

static int node_input(bt_mesh_input_action_t act, u8_t size)
{
    switch (act) {
    case BT_MESH_ENTER_NUMBER: {
        LOGI(TAG, "Enter a number (max %u digits) with: input-num <num>\n", size);

        if (g_mesh_node.user_prov_cb) {
            g_mesh_node.user_prov_cb(BT_MESH_EVENT_NODE_OOB_INPUT_NUM, &size);
        }
    } break;

    case BT_MESH_ENTER_STRING: {
        LOGI(TAG, "Enter a string (max %u chars) with: input-str <str>\n", size);

        if (g_mesh_node.user_prov_cb) {
            g_mesh_node.user_prov_cb(BT_MESH_EVENT_NODE_OOB_INPUT_STRING, &size);
        }
    } break;

    default: {
        LOGI(TAG, "Unknown input action %u (size %u) requested!\n", act, size);
        return -EINVAL;
    } break;
    }

    g_mesh_node.input_act = act;
    g_mesh_node.input_size = size;
    return 0;
}

#ifdef BT_MESH_BEACON_RSSI_REPORT
static bool is_rssi_data_queue_empty(rssi_data_queue *queue)
{
    return queue->front == queue->rear;
}

static bool is_rssi_data_queue_full(rssi_data_queue *queue)
{
    if ((queue->rear + 1) % 16 == queue->front) {
        return true;
    } else {
        return false;
    }
}

static beacon_rssi_data *get_rssi_data(rssi_data_queue *queue)
{
    if (is_rssi_data_queue_empty(queue)) {
        return NULL;
    } else {
        beacon_rssi_data *data = &queue->data[queue->front];
        queue->front = (queue->front + 1) % 16;
        return data;
    }
}

static int put_rssi_data(rssi_data_queue *queue, const uint8_t *mac, int8_t rssi, int8_t rssi_calibration)
{
    if (is_rssi_data_queue_full(queue)) {
        return -1;
    }

    memcpy(queue->data[queue->rear].mac, mac, 6);
    queue->data[queue->rear].rssi = rssi;
    queue->data[queue->rear].rssi_calibration = rssi_calibration;
    queue->rear = (queue->rear + 1) % 16;
    return 0;
}

static uint8_t data_complent_u8(int8_t data)
{
    uint8_t out;

    if (data <= 0) {
        out = data;
    } else {
        uint8_t in = -data;
        out = ~in + 1;
    }

    return out;
}

static void beacon_rssi_cb(const struct adv_addr_t *addr, s8_t rssi, u8_t adv_type, void *user_data, uint16_t len)
{
    if (!g_mesh_node.node_provisioned) {
        return;
    }

    if (!user_data || !len) {
        return;
    }

    struct net_buf_simple *buf = (struct net_buf_simple *)user_data;

    uint8_t data_len = net_buf_simple_pull_u8(buf);

    if (!data_len) {
        return;
    }

    uint8_t opid = net_buf_simple_pull_u8(buf);

    if (0x02 != opid) {
        return;
    }

    int8_t rssi_calibration = net_buf_simple_pull_u8(buf);

    put_rssi_data(&g_mesh_node.rssi_queue, addr->val, data_complent_u8(rssi), data_complent_u8(rssi_calibration));
}

void rssi_repot_timeout(void *timer, void *args)
{
    beacon_rssi_data *data = NULL;
    k_timer_start(&g_mesh_node.beacon_rssi_timer, RSSI_BEACON_REPORT_PERIOD);
    data = get_rssi_data(&g_mesh_node.rssi_queue);

    if (!data) {
        return;
    }

    struct bt_mesh_model *vendor_model;

    vendor_model = ble_mesh_model_find(0, BT_MESH_MODEL_VND_MODEL_SRV, CONFIG_CID_TAOBAO);

    if (!vendor_model || !vendor_model->pub || BT_MESH_ADDR_UNASSIGNED == vendor_model->pub->addr) {
        return;
    }

    vnd_model_msg vnd_data = { 0 };
    uint8_t rssi_data[60];
    snprintf((char *)rssi_data, sizeof(rssi_data), "rssi%02x,%02x,%02x%02x%02x%02x%02x%02x", data->rssi,
             data->rssi_calibration, data->mac[5], data->mac[4], data->mac[3], data->mac[2], data->mac[1],
             data->mac[0]);
    vnd_data.netkey_idx = 0;
    vnd_data.appkey_idx = 0;
    vnd_data.dst_addr = vendor_model->pub->addr;
    vnd_data.model = vendor_model;
    vnd_data.opid = VENDOR_OP_ATTR_TRANS_MSG;
    vnd_data.data = rssi_data;
    vnd_data.retry = 0;
    vnd_data.len = strlen((char *)rssi_data);
    ble_mesh_vendor_srv_model_msg_send(&vnd_data);
}

static int init_rssi_report()
{
    int16_t ret;
    ret = bt_mesh_vnd_adv_set_cb(beacon_rssi_cb);

    if (ret) {
        return ret;
    }

    g_mesh_node.rssi_queue.front = 0;
    g_mesh_node.rssi_queue.rear = 0;
    g_mesh_node.rssi_queue.length = 16;
    k_timer_init(&g_mesh_node.beacon_rssi_timer, rssi_repot_timeout, NULL);
    k_timer_start(&g_mesh_node.beacon_rssi_timer, RSSI_BEACON_REPORT_PERIOD);
    return 0;
}
#endif

int ble_mesh_node_OOB_input_string(const char *str)
{
    if (!g_mesh_node.init_flag) {
        return -1;
    }

    int16_t ret;

    if (g_mesh_node.input_act != BT_MESH_ENTER_STRING) {
        LOGE(TAG, "A string hasn't been requested!\n");
        return -1;
    }

    if (strlen(str) < g_mesh_node.input_size) {
        LOGE(TAG, "Too short input (%u characters required)\n", g_mesh_node.input_size);
        return -1;
    }

    ret = bt_mesh_input_string(str);

    if (ret) {
        LOGE(TAG, "String input failed (err %d)\n", ret);
        return -1;
    }

    g_mesh_node.input_act = BT_MESH_NO_INPUT;

    return 0;
}

int ble_mesh_node_OOB_input_num(uint32_t num)
{
    if (!g_mesh_node.init_flag) {
        return -1;
    }

    int16_t ret;

    if (g_mesh_node.input_act != BT_MESH_ENTER_STRING) {
        LOGE(TAG, "A num hasn't been requested!\n");
        return -1;
    }

    ret = bt_mesh_input_number(num);

    if (ret) {
        LOGE(TAG, "num input failed (err %d)\n", ret);
        return -1;
    }

    g_mesh_node.input_act = BT_MESH_NO_INPUT;

    return 0;
}

#endif

static void node_model_message_cb(mesh_model_event_en event, void *p_arg)
{
    int ret;

    switch (event) {
#ifdef CONFIG_BT_MESH_PROVISIONER

    case BT_MESH_MODEL_CFG_APPKEY_STATUS: {
        if (p_arg) {
            model_message message = *(model_message *)p_arg;
            uint8_t status = message.status_data->data[0];

            k_timer_stop(&g_mesh_node.add_appkey_timer);

            if (status) {
                if (g_mesh_node.app_key_add_retry) {
                    ret = ble_mesh_node_appkey_add(0, g_mesh_node.auto_add_appkey_idx, last_addr);

                    if (ret) {
                        LOGE(TAG, "add appkey faild");
                    } else {
                        g_mesh_node.app_key_add_retry--;
                    }

                } else {
                    LOGE(TAG, "add appkey faild");
                }
            }
        }
    } break;
#else

    case BT_MESH_MODEL_CFG_APP_KEY_ADD: {
#if defined(CONFIG_BT_MESH_MODEL_VENDOR_SRV)
        if (p_arg) {
            appkey_status *add_status = (appkey_status *)p_arg;

            if (!add_status->status) {
                if (g_mesh_node.app_key_add_flag) {
                    break;
                }

                g_mesh_node.app_key_add_flag = 1;
                auto_config_t config = {
                    .appkey_idx = add_status->appkey_idx,
                    .sub_addr = COMMON_SUB_ADDR,
                    .pub_addr = COMMON_PUB_ADDR,
                    .mod_id = BT_MESH_MODEL_VND_MODEL_SRV,
                    .cid = BT_MESH_MODEL_VND_COMPANY_ID,
                };

                auto_config(&config);
            }
        }
#endif
    } break;

    case BT_MESH_MODEL_VENDOR_MESH_AUTOCONFIG: {
        if (p_arg && BT_MESH_AUTO_CONFIG_START != g_mesh_node.auto_config_status) {
            // uint8_t tid;
            int16_t ret;
            model_message *msg = (model_message *)p_arg;

            if (!g_mesh_node.app_key_add_flag) {
                g_mesh_node.auto_config_status = BT_MESH_AUTO_CONFIG_FAILD;

                if (BT_MESH_ADDR_IS_UNICAST(msg->dst_addr)) {
                    ret = report_auto_config_status(msg->source_addr, g_mesh_node.auto_config_status);

                    if (ret) {
                        LOGE(TAG, "report auto config status faild");
                    }
                }

                return;
            }

            if (BT_MESH_ADDR_IS_UNICAST(msg->dst_addr)) {
                g_mesh_node.auto_config_ack_flag = 1;
            } else {
                g_mesh_node.auto_config_ack_flag = 0;
            }

            if (BT_MESH_AUTO_CONFIG_SUCCEED == g_mesh_node.auto_config_status) {
                if (g_mesh_node.auto_config_ack_flag) {
                    ret = report_auto_config_status(msg->source_addr, g_mesh_node.auto_config_status);

                    if (ret) {
                        LOGE(TAG, "report auto config status faild");
                    }

                    return;
                } else {
                    return;
                }
            }

            g_mesh_node.auto_config_status = BT_MESH_AUTO_CONFIG_START;

            net_buf_simple_pull_u8(msg->ven_data.user_data);
            auto_config_t config = { 0 };
            config.appkey_idx = 0;
            config.sub_addr = net_buf_simple_pull_le16(msg->ven_data.user_data);
            config.pub_addr = 0; // net_buf_simple_pull_le16(msg->ven_data.user_data);
            config.mod_id = 0xffff;
            config.cid = 0xffff;
            ret = auto_config(&config);

            if (ret) {
                g_mesh_node.auto_config_status = BT_MESH_AUTO_CONFIG_FAILD;
                ret = aos_kv_setint("AUTO_CONFIG", BT_MESH_AUTO_CONFIG_FAILD);

                if (ret) {
                    LOGE(TAG, "save auto config status faild");
                }
            }

            if (!ret) {
                g_mesh_node.prov_addr = msg->source_addr;
                g_mesh_node.auto_config_retry = DEF_AUTOCONFIG_STORE_CHECK_RETRY;
                g_mesh_node.auto_config_timeout = DEF_AUTOCONFIG_STORE_CHECK_TIMEOUT;
                k_timer_start(&g_mesh_node.auto_config_timer, g_mesh_node.auto_config_timeout);
            }
        }

    }

    break;

    case BT_MESH_MODEL_VENDOR_MESH_AUTOCONFIG_GET: {
        if (p_arg) {
            model_message *message = (model_message *)p_arg;

            if (!bt_mesh_store_mod_pending_check()) {
                ret = report_auto_config_status(message->source_addr, g_mesh_node.auto_config_status);

                if (ret) {
                    LOGE(TAG, "report auto config status faild");
                }
            }
        }
    } break;
#endif
    case BT_MESH_MODEL_CFG_RST_STATUS: {
        ret = aos_kv_setint("AUTO_CONFIG", BT_MESH_AUTO_CONFIG_NOT_START);
        if (ret) {
            LOGE(TAG, "save auto config status faild");
        }
    } break;
    default:
        break;
    }

    if (g_mesh_node.user_model_cb) {
        g_mesh_node.user_model_cb(event, p_arg);
    }
}

void node_attention_on(struct bt_mesh_model *model)
{
    if (g_mesh_node.health_cb.att_on) {
        g_mesh_node.health_cb.att_on();
    }
}

void node_attention_off(struct bt_mesh_model *model)
{
    if (g_mesh_node.health_cb.att_off) {
        g_mesh_node.health_cb.att_off();
    }
}

struct bt_mesh_health_srv_cb g_health_cb = {
    .attn_on = node_attention_on,
    .attn_off = node_attention_off,
};

int ble_mesh_node_init(node_config_t *param)
{
    if (!param) {
        return -1;
    }

    const struct bt_mesh_comp *mesh_comp = ble_mesh_model_get_comp_data();

    if (!mesh_comp || (NODE != param->role && PROVISIONER != param->role)) {
        return -1;
    }

    g_mesh_node.comp = mesh_comp;

    if (param->node_oob.static_oob_data) {
        memcpy(g_static_data, param->node_oob.static_oob_data, 16);
        node_prov.static_val_len = 16;
    }

    if (ACTION_NUM == param->node_oob.input_action && param->node_oob.input_max_size > 0) {
        node_prov.input_actions = BT_MESH_ENTER_NUMBER;
        node_prov.input_size = param->node_oob.input_max_size;
    } else if (ACTION_STR == param->node_oob.input_action && param->node_oob.input_max_size > 0) {
        node_prov.input_actions = BT_MESH_ENTER_STRING;
        node_prov.input_size = param->node_oob.input_max_size;
    } else {
        node_prov.input_actions = BT_MESH_NO_INPUT;
        node_prov.input_size = 0;
    }

    if (ACTION_NUM == param->node_oob.output_action && param->node_oob.output_max_size > 0) {
        node_prov.output_actions = BT_MESH_DISPLAY_NUMBER;
        node_prov.output_size = param->node_oob.output_max_size;
    } else if (ACTION_STR == param->node_oob.output_action && param->node_oob.output_max_size > 0) {
        node_prov.output_actions = BT_MESH_DISPLAY_STRING;
        node_prov.output_size = param->node_oob.output_max_size;
    } else {
        node_prov.output_actions = BT_MESH_NO_OUTPUT;
        node_prov.output_size = 0;
    }

    if (PROVISIONER == param->role) {
#ifdef CONFIG_BT_MESH_PROVISIONER
        int ret;
        provisioner_cb app_cb;

        if (!param->provisioner_config ||
            mesh_comp->elem_count > (param->provisioner_config->config.unicast_addr_start -
                                     param->provisioner_config->config.unicast_addr_local)) {
            return -EINVAL;
        }

        app_cb = param->provisioner_config->config.cb;
        param->provisioner_config->config.cb = provisioner_node_cb;
        ret = ble_mesh_provisioner_init(&param->provisioner_config->config);

        if (ret) {
            return ret;
        }

        uint16_t appkey_idx = 0;
        const ble_mesh_provisioner_t *prov_comp = ble_mesh_provisioner_get_provisioner_data();

        if (!prov_comp) {
            return ret;
        }

        dev_addr_t dev_addr;
        ret = ble_stack_get_local_addr(&dev_addr);

        if (ret) {
            return ret;
        }

        memcpy(dev_uuid, param->dev_uuid, 16);
        memcpy(dev_uuid, dev_addr.val, 6);

        ret = bt_mesh_init(&node_prov, mesh_comp, prov_comp->provisioner);

        if (ret) {
            return ret;
        }

        ret = ble_stack_setting_load();

        if (ret) {
            return ret;
        }

        ret = ble_mesh_provisioner_enable();

        if (ret && (ret != -EALREADY)) {
            return ret;
        }

        bt_mesh_provisioner_local_provision();

        k_timer_init(&g_mesh_node.add_appkey_timer, add_appkey_timeout_cb, NULL);

        g_mesh_node.user_provisioner_cb = app_cb;
        //add appkey 0
        ret = bt_mesh_provisioner_local_app_key_add(NULL, 0, &appkey_idx);

        if (ret < 0 && -EEXIST != ret) {
            return ret;
        } else if (0 == ret || -EEXIST == ret) {
            //bind all model to appkey0
            extern u16_t bt_mesh_primary_addr(void);
            bt_mesh_cfg_app_key_add(0, bt_mesh_primary_addr(), 0, appkey_idx,
                                    bt_mesh_provisioner_local_app_key_get(0, appkey_idx), NULL);
            auto_config_t config = {
                .appkey_idx = appkey_idx,
                .sub_addr = param->provisioner_config->local_sub,
                .pub_addr = param->provisioner_config->local_pub,
                .mod_id = 0xffff,
                .cid = 0xffff,
            };
            auto_config(&config);
        }

        g_mesh_node.node_provisioned = 1;
#else
        return -1;
#endif
    } else {
#ifndef CONFIG_BT_MESH_PROVISIONER
        int ret;

        g_mesh_node.user_prov_cb = param->user_prov_cb;

        ret = bt_mesh_init(&node_prov, mesh_comp, NULL);

        if (ret) {
            return ret;
        }

        ret = ble_stack_setting_load();

        if (ret) {
            return ret;
        }

        ret = ble_stack_set_name((char *)param->dev_name);

        if (ret) {
            return ret;
        }

        dev_addr_t dev_addr;
        ret = ble_stack_get_local_addr(&dev_addr);

        if (ret) {
            return ret;
        }

        memcpy(dev_uuid, param->dev_uuid, 16);
        memcpy(dev_uuid, dev_addr.val, 6);

        /* This will be a no-op if settings_load() loaded provisioning info */
        bt_mesh_prov_enable(BT_MESH_PROV_ADV | BT_MESH_PROV_GATT);

        k_timer_init(&g_mesh_node.auto_config_timer, auto_config_timeout_cb, NULL);
        g_mesh_node.node_provisioned = bt_mesh_is_provisioned();

        if (g_mesh_node.node_provisioned) {
            int auto_config_status = 0;
            ret = aos_kv_getint("AUTO_CONFIG", &auto_config_status);
            if (ret) {
                g_mesh_node.auto_config_status = BT_MESH_AUTO_CONFIG_NOT_START;
            } else {
                g_mesh_node.auto_config_status = auto_config_status;
            }
        } else {
            g_mesh_node.node_provisioned = BT_MESH_AUTO_CONFIG_NOT_START;
        }

        extern struct bt_mesh_app_key *bt_mesh_app_key_find(u16_t app_idx);

        if (!bt_mesh_app_key_find(0)) {
            g_mesh_node.app_key_add_flag = 0;
        } else {
            g_mesh_node.app_key_add_flag = 1;
        }

#ifdef BT_MESH_BEACON_RSSI_REPORT
        g_mesh_node.rssi_report_enable = param->rssi_report_enable;

        if (g_mesh_node.node_provisioned && g_mesh_node.rssi_report_enable) {
            init_rssi_report();
        }

#endif
#else
        return -1;
#endif
    }

    ble_mesh_model_set_cb(node_model_message_cb);

    extern int mesh_event_port_init();
    mesh_event_port_init();

    if (param->health_cb && (param->health_cb->att_on || param->health_cb->att_off)) {
        extern int health_srv_cb_register(struct bt_mesh_health_srv_cb * health_cb);

        if (health_srv_cb_register(&g_health_cb)) {
            LOGE(TAG, "register health srv cb faild");
        }

        g_mesh_node.health_cb.att_on = param->health_cb->att_on;
        g_mesh_node.health_cb.att_off = param->health_cb->att_off;
    }

    g_mesh_node.user_model_cb = param->user_model_cb;
    g_mesh_node.init_flag = 1;

    return g_mesh_node.node_provisioned;
}
