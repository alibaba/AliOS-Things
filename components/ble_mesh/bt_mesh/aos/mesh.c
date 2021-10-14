    /*
 * Copyright (C) 2017 C-SKY Microsystems Co., All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <string.h>
#include <ble_os.h>
#include <yoc/eventid.h>
#include <yoc/uservice.h>

#include <settings/settings.h>
#include <bluetooth/bluetooth.h>

#include <api/mesh.h>

#include <aos/log.h>
#include <aos/mesh.h>
#include  "errno.h"
#define TAG "BLE_MESH"
#define BLE_LOGD(fmt, ...) LOGD(TAG, "[%s]"fmt,__FUNCTION__,  ##__VA_ARGS__)
#define BLE_LOGE(fmt, ...) LOGE(TAG, "[%s]"fmt,__FUNCTION__,  ##__VA_ARGS__)

typedef struct _ble_mesh_dev_t {
    uint8_t is_init;
    struct bt_mesh_prov *prov;
    struct bt_mesh_comp *comp;
    mesh_param_init_t init;
} ble_mesh_dev_t;

ble_mesh_dev_t ble_mesh_dev = {0};

static void ble_stack_event_callback(mesh_event_en event, void *event_data, uint32_t event_len)
{
    if (ble_mesh_dev.init.event_cb) {
        ble_mesh_dev.init.event_cb(event, event_data);
    }
}

static int mesh_output_number_cb(bt_mesh_output_action_t act, bt_u32_t num)
{
    evt_data_oob_output_t event_data;

    event_data.action = act;
    event_data.num = num;

    ble_stack_event_callback(EVENT_MESH_OOB_OUTPUT, &event_data, sizeof(event_data));
    return 0;
}

static int mesh_output_string_cb(const char *str)
{
    evt_data_oob_output_t event_data;

    event_data.action = MESH_DISPLAY_STRING;
    event_data.str = str;

    ble_stack_event_callback(EVENT_MESH_OOB_OUTPUT, &event_data, sizeof(event_data));

    return 0;
}

static int mesh_input_cb(bt_mesh_input_action_t act, u8_t size)
{
    evt_data_oob_input_t event_data;

    event_data.action = act;
    event_data.size = size;

    ble_stack_event_callback(EVENT_MESH_OOB_INPUT, &event_data, sizeof(event_data));

    return 0;
}

static void mesh_link_open_cb(bt_mesh_prov_bearer_t bearer)
{
    evt_data_prov_bearer_t event_data;
    event_data.bearer = bearer;

    ble_stack_event_callback(EVENT_MESH_PROV_START, &event_data, sizeof(event_data));
}

static void mesh_link_close_cb(bt_mesh_prov_bearer_t bearer)
{
    evt_data_prov_bearer_t event_data;
    event_data.bearer = bearer;

    ble_stack_event_callback(EVENT_MESH_PROV_END, &event_data, sizeof(event_data));
}

static void mesh_complete_cb(u16_t net_idx, u16_t addr)
{
    evt_data_prov_complete_t event_data;
    event_data.net_idx = net_idx;
    event_data.addr = addr;

    ble_stack_event_callback(EVENT_MESH_PROV_COMPLETE, &event_data, sizeof(event_data));
}

static void mesh_reset_cb(void)
{
    ble_stack_event_callback(EVENT_MESH_NODE_RESET, NULL, 0);
}

static void mesh_op_cb(struct bt_mesh_model *model,
                       struct bt_mesh_msg_ctx *ctx,
                       struct net_buf_simple *buf, uint32_t opcode)
{
    evt_data_mesh_msg_t event_data;
    mesh_model_t *mesh_model;
    uint8_t mesh_model_count;
    int i = 0;
    event_data.net_idx = ctx->net_idx;
    event_data.app_idx = ctx->app_idx;
    event_data.remote_addr = ctx->addr;
    event_data.recv_dst = ctx->recv_dst;
    event_data.recv_ttl = ctx->recv_ttl;
    event_data.msg_len = buf->len;
    event_data.msg = buf->data;

    if (model->elem_idx < ble_mesh_dev.init.comp->elem_count) {
        if (opcode < 0x10000) {
            mesh_model_count = ble_mesh_dev.init.comp->elem[model->elem_idx].model_count;
            mesh_model = ble_mesh_dev.init.comp->elem[model->elem_idx].models;
        } else {
            mesh_model_count = ble_mesh_dev.init.comp->elem[model->elem_idx].vnd_model_count;
            mesh_model = ble_mesh_dev.init.comp->elem[model->elem_idx].vnd_models;
        }

        if (model->mod_idx < mesh_model_count) {
            i = 0;

            while (i < mesh_model[model->mod_idx].op_num) {
                if (mesh_model[model->mod_idx].op_list[i].op == opcode) {
                    if (mesh_model[model->mod_idx].op_list[i].op_cb) {
                        mesh_model[model->mod_idx].op_list[i].op_cb(&event_data);
                    }

                    break;
                }

                i++;
            }
        }
    }

}

static void mesh_op_cb_dummy(struct bt_mesh_model *model,
                             struct bt_mesh_msg_ctx *ctx,
                             struct net_buf_simple *buf)
{
}

#if defined(CONFIG_BT_MESH_LOW_POWER)
static void mesh_lpn_cb(u16_t friend_addr, bool established)
{
    evt_data_friendship_chanage_t event_data;
    event_data.established = established;
    event_data.friend_addr = friend_addr;

    ble_stack_event_callback(EVENT_MESH_FRIENDSHIP_CHANGE, &event_data, sizeof(event_data));
}
#endif

int ble_stack_mesh_get_local_addr(dev_addr_t *addr)
{
    struct bt_le_oob oob;

    if (addr == NULL) {
        return -BLE_STACK_ERR_NULL;
    }

    bt_le_oob_get_local(0, &oob);
    memcpy(addr, &oob.addr, sizeof(dev_addr_t));

    return 0;
}

int ble_stack_mesh_init(mesh_param_init_t *param)
{
    int ret;
    static struct bt_mesh_prov prov = {0};
    static struct bt_mesh_comp comp = {0};
    static uint8_t uuid[16] = {0};

    if (ble_mesh_dev.is_init) {
        return 0;
    }

    ble_mesh_dev.init = *param;

    if (ble_mesh_dev.init.dev_addr) {
        bt_addr_le_t addr = *(bt_addr_le_t *)ble_mesh_dev.init.dev_addr;

        if (addr.type == BT_ADDR_LE_RANDOM) {
            ret = bt_set_id_addr(&addr);

            if (ret) {
                ret = -BLE_STACK_ERR_PARAM;
                return ret;
            }
        } else if (addr.type == BT_ADDR_LE_PUBLIC) {
#if defined(CONFIG_BT_HCI_VS_EXT)
            extern int bt_set_bdaddr(const bt_addr_le_t *addr);
            ret = bt_set_bdaddr(&addr);

            if (ret) {
                ret = -BLE_STACK_ERR_PARAM;
                return ret;
            }

#endif
        }
    }

    //ret = bt_enable(NULL);

    //if (ret) {
    //    LOGE(TAG, "bt init fail %d", ret);
    //    return -BLE_STACK_ERR_INIT;
    //}

    if (ble_mesh_dev.init.dev_name) {
        ret = bt_set_name(ble_mesh_dev.init.dev_name);

        if (ret) {
            ret = -BLE_STACK_ERR_PARAM;
            return ret;
        }
    }

    memcpy(uuid, param->prov->dev_uuid, sizeof(uuid));
    prov.uuid = uuid;
    prov.uri = param->prov->uri;
    prov.oob_info = param->prov->oob_info;
    prov.static_val = param->prov->cap.static_val;
    prov.static_val_len = param->prov->cap.static_val_len;
    prov.output_size = param->prov->cap.output_oob_size;
    prov.output_actions = param->prov->cap.output_oob_actions;
    prov.input_size = param->prov->cap.input_oob_size;
    prov.input_actions = param->prov->cap.input_oob_actions;

    prov.output_number = mesh_output_number_cb;
    prov.output_string = mesh_output_string_cb;
    prov.input = mesh_input_cb;
    prov.link_open = mesh_link_open_cb;
    prov.link_close = mesh_link_close_cb;
    prov.complete = mesh_complete_cb;
    prov.reset = mesh_reset_cb;

    comp.cid = param->comp->cid;
    comp.pid = param->comp->pid;
    comp.vid = param->comp->vid;
    comp.elem_count = param->comp->elem_count;

    struct bt_mesh_elem *elems = (struct bt_mesh_elem *) malloc(sizeof(struct bt_mesh_elem) * param->comp->elem_count);

    if (elems == NULL) {
        return -BLE_STACK_ERR_NULL;
    }

    comp.elem = elems;
    int i, j;

    for (i = 0; i < param->comp->elem_count; i++, elems++) {
        elems->model_count = param->comp->elem[i].model_count;
        elems->vnd_model_count = param->comp->elem[i].vnd_model_count;

        if (elems->model_count) {
            struct bt_mesh_model *models = (struct bt_mesh_model *) malloc(sizeof(struct bt_mesh_model) * elems->model_count);

            if (models == NULL) {
                return -BLE_STACK_ERR_NULL;
            }

            memset(models, 0, sizeof(struct bt_mesh_model) * elems->model_count);
            elems->models = models;

            for (j = 0; j < elems->model_count; j++, models++) {
                memset(models->keys, 0xFF, sizeof(models->keys));

                if (param->comp->elem[i].models[j].id == MESH_MODEL_ID_CFG_SRV) {
                    mesh_cfg_srv_t *cfg_srv =  param->comp->elem[i].models[j].user_data;
                    static struct bt_mesh_cfg_srv bt_cfg_srv = {0};
                    bt_cfg_srv.net_transmit = cfg_srv->net_transmit;
                    bt_cfg_srv.relay = cfg_srv->relay;
                    bt_cfg_srv.relay_retransmit = cfg_srv->relay_retransmit;
                    bt_cfg_srv.beacon = cfg_srv->beacon;
                    bt_cfg_srv.gatt_proxy = cfg_srv->gatt_proxy;
                    bt_cfg_srv.frnd = cfg_srv->frnd;
                    bt_cfg_srv.default_ttl = cfg_srv->default_ttl;
                    models->id = BT_MESH_MODEL_ID_CFG_SRV;
                    models->op = bt_mesh_cfg_srv_op;
                    models->pub = NULL;
                    models->user_data = &bt_cfg_srv;
                } else if (param->comp->elem[i].models[j].id == MESH_MODEL_ID_CFG_CLI) {
                    static struct bt_mesh_cfg_cli bt_cfg_cli = {};
                    models->id = BT_MESH_MODEL_ID_CFG_CLI;
                    models->op = bt_mesh_cfg_cli_op;
                    models->pub = NULL;
                    models->user_data = &bt_cfg_cli;
                } else if (param->comp->elem[i].models[j].id == MESH_MODEL_ID_HEALTH_SRV) {
                    struct bt_health_srv_data_t {
                        struct bt_mesh_health_srv srv;
                        struct bt_mesh_model_pub bt_pub;
                        struct net_buf_simple msg;
                    };

                    struct bt_health_srv_data_t *srv_data = (struct bt_health_srv_data_t *)malloc(sizeof(struct bt_health_srv_data_t));

                    if (srv_data == NULL) {
                        return -BLE_STACK_ERR_NULL;
                    }

                    memset(srv_data, 0, sizeof(struct bt_health_srv_data_t));

                    uint8_t *msg_data = (uint8_t *)malloc(param->comp->elem[i].models[j].pub_mesh_len + 1 + 3);

                    if (msg_data == NULL) {
                        free(srv_data);
                        srv_data = NULL;
                        return -BLE_STACK_ERR_NULL;
                    }

                    srv_data->msg.data = msg_data;
                    srv_data->msg.len  = 0;
                    srv_data->msg.size = param->comp->elem[i].models[j].pub_mesh_len + 1 + 3;
                    srv_data->msg.__buf = msg_data;
                    srv_data->bt_pub.msg = &srv_data->msg;
                    srv_data->bt_pub.update = NULL;

                    models->id = BT_MESH_MODEL_ID_HEALTH_SRV;
                    models->op = bt_mesh_health_srv_op;
                    models->pub = &srv_data->bt_pub;
                    models->user_data = &srv_data->srv;
                } else if (param->comp->elem[i].models[j].id == MESH_MODEL_ID_HEALTH_CLI) {
                    struct bt_mesh_health_cli *bt_health_cli = (struct bt_mesh_health_cli *)malloc(sizeof(struct bt_mesh_health_cli));

                    if (bt_health_cli == NULL) {
                        return -BLE_STACK_ERR_NULL;
                    }

                    bt_health_cli->current_status = NULL;
                    models->id = BT_MESH_MODEL_ID_HEALTH_CLI;
                    models->op = bt_mesh_health_cli_op;
                    models->pub = NULL;
                    models->user_data = bt_health_cli;
                } else {
                    struct bt_mesh_model_op *op = malloc(sizeof(struct bt_mesh_model_op) * (param->comp->elem[i].models[j].op_num + 1));

                    if (op == NULL) {
                        return -BLE_STACK_ERR_NULL;
                    }

                    memset(op, 0, sizeof(struct bt_mesh_model_op) * param->comp->elem[i].models[j].op_num);
                    int k;

                    for (k = 0; k < param->comp->elem[i].models[j].op_num; k++) {
                        op[k].opcode = param->comp->elem[i].models[j].op_list[k].op;
                        op[k].min_len = 0;
                        op[k].func = mesh_op_cb_dummy;
                        op[k].func2 = mesh_op_cb;
                    }

                    struct bt_model_pub_data_t {
                        struct bt_mesh_model_pub bt_pub;
                        struct net_buf_simple msg;
                    };

                    struct bt_model_pub_data_t *pub_data = (struct bt_model_pub_data_t *)malloc(sizeof(struct bt_model_pub_data_t));

                    if (pub_data == NULL) {
                        return -BLE_STACK_ERR_NULL;
                    }

                    memset(pub_data, 0, sizeof(struct bt_model_pub_data_t));

                    uint8_t *msg_data = (uint8_t *)malloc(param->comp->elem[i].models[j].pub_mesh_len + 1 + 3);

                    if (msg_data == NULL) {
                        return -BLE_STACK_ERR_NULL;
                    }

                    pub_data->msg.data = msg_data;
                    pub_data->msg.len  = 0;
                    pub_data->msg.size = param->comp->elem[i].models[j].pub_mesh_len + 1 + 3;
                    pub_data->msg.__buf = msg_data;
                    pub_data->bt_pub.msg = &pub_data->msg;
                    pub_data->bt_pub.update = NULL;

                    models->id = param->comp->elem[i].models[j].id;
                    models->op = op;
                    models->pub = &pub_data->bt_pub;
                    models->user_data = &param->comp->elem[i].models[j];
                }
            }
        }

        if (elems->vnd_model_count) {
            struct bt_mesh_model *models = (struct bt_mesh_model *) malloc(sizeof(struct bt_mesh_model) * elems->vnd_model_count);

            if (models == NULL) {
                return -BLE_STACK_ERR_NULL;
            }

            memset(models, 0, sizeof(struct bt_mesh_model) * elems->vnd_model_count);
            elems->vnd_models = models;

            for (j = 0; j < elems->vnd_model_count; j++, models++) {
                memset(models->keys, 0xFF, sizeof(models->keys));

                struct bt_mesh_model_op *op = malloc(sizeof(struct bt_mesh_model_op) * (param->comp->elem[i].vnd_models[j].op_num + 1));

                if (op == NULL) {
                    return -BLE_STACK_ERR_NULL;
                }

                memset(op, 0, sizeof(struct bt_mesh_model_op) * param->comp->elem[i].vnd_models[j].op_num);
                int k;

                for (k = 0; k < param->comp->elem[i].vnd_models[j].op_num; k++) {
                    op[k].opcode = param->comp->elem[i].vnd_models[j].op_list[k].op;
                    op[k].min_len = 0;
                    op[k].func = mesh_op_cb_dummy;
                    op[k].func2 = mesh_op_cb;
                }

                struct bt_model_pub_data_t {
                    struct bt_mesh_model_pub bt_pub;
                    struct net_buf_simple msg;
                };

                struct bt_model_pub_data_t *pub_data = (struct bt_model_pub_data_t *)malloc(sizeof(struct bt_model_pub_data_t));

                if (pub_data == NULL) {
                    return -BLE_STACK_ERR_NULL;
                }

                memset(pub_data, 0, sizeof(struct bt_model_pub_data_t));

                uint8_t *msg_data = (uint8_t *)malloc(param->comp->elem[i].vnd_models[j].pub_mesh_len + 1 + 3);

                if (msg_data == NULL) {
                    free(pub_data);
                    pub_data = NULL;
                    free(op);
                    op = NULL;
                    return -BLE_STACK_ERR_NULL;
                }

                pub_data->msg.data = msg_data;
                pub_data->msg.len  = 0;
                pub_data->msg.size = param->comp->elem[i].vnd_models[j].pub_mesh_len + 1 + 3;
                pub_data->msg.__buf = msg_data;
                pub_data->bt_pub.msg = &pub_data->msg;
                pub_data->bt_pub.update = NULL;

                models->id = param->comp->elem[i].vnd_models[j].id;
                models->op = op;
                models->pub = &pub_data->bt_pub;
                models->user_data = &param->comp->elem[i].vnd_models[j];
            }
        }
    }

    ble_mesh_dev.comp = &comp;
    ble_mesh_dev.prov = &prov;
    ret = bt_mesh_init(&prov, &comp, NULL);

    if (ret) {
        LOGE(TAG, "mesh init fail %d", ret);
        return -BLE_STACK_ERR_INIT;
    }

    ble_mesh_dev.is_init = 1;

#if defined(CONFIG_BT_MESH_LOW_POWER)
    bt_mesh_lpn_set_cb(mesh_lpn_cb);
#endif

    ble_stack_event_callback(EVENT_MESH_STACK_INIT, NULL, 0);
    return ret;
}

int ble_stack_mesh_prov_start(mesh_prov_en bearers)
{
    int ret;
    ret = bt_mesh_prov_enable((bt_mesh_prov_bearer_t)bearers);

    if (ret == -EALREADY) {
        ret = BLE_STACK_OK;
    }

    return ret;
}

int ble_stack_mesh_prov_stop(mesh_prov_en bearers)
{
    int ret;
    ret = bt_mesh_prov_disable((bt_mesh_prov_bearer_t)bearers);

    if (ret == -EALREADY) {
        ret = BLE_STACK_OK;
    }

    return ret;
}

int ble_stack_mesh_send(uint16_t net_id,
                        uint16_t app_id,
                        uint16_t remote_addr,
                        uint8_t ttl,
                        uint8_t elem_id,
                        uint8_t mod_id,
                        uint32_t op,
                        uint8_t *msg,
                        uint16_t msg_len)
{
    struct bt_mesh_model *model = NULL;
    struct bt_mesh_msg_ctx ctx = {
        .net_idx = net_id,
        .app_idx = app_id,
        .addr = remote_addr,
        .send_ttl = ttl,
    };

    if (!ble_mesh_dev.is_init) {
        return 0;
    }

    if (elem_id <= ble_mesh_dev.comp->elem_count) {
        if (op < 0x10000) {
            if (mod_id < ble_mesh_dev.comp->elem[elem_id].model_count) {
                model = &ble_mesh_dev.comp->elem[elem_id].models[mod_id];
            }
        } else {
            if (mod_id < ble_mesh_dev.comp->elem[elem_id].vnd_model_count) {
                model = &ble_mesh_dev.comp->elem[elem_id].vnd_models[mod_id];
            }
        }
    }

    if (model == NULL) {
        return -BLE_STACK_ERR_PARAM;
    }

    NET_BUF_SIMPLE_DEFINE(msg_buf, 2 + msg_len + 4);

    bt_mesh_model_msg_init(&msg_buf, op);

    net_buf_simple_add_mem(&msg_buf, msg, msg_len);

    return bt_mesh_model_send(model, &ctx, &msg_buf, NULL, NULL);
}

int ble_stack_mesh_pub(uint8_t elem_id,
                       uint8_t mod_id,
                       uint32_t op,
                       uint8_t *msg,
                       uint16_t msg_len)
{
    struct bt_mesh_model *model = NULL;

    if (!ble_mesh_dev.is_init) {
        return 0;
    }

    if (elem_id <= ble_mesh_dev.comp->elem_count) {
        if (op < 0x10000) {
            if (mod_id < ble_mesh_dev.comp->elem[elem_id].model_count) {
                model = &ble_mesh_dev.comp->elem[elem_id].models[mod_id];
            }
        } else {
            if (mod_id < ble_mesh_dev.comp->elem[elem_id].vnd_model_count) {
                model = &ble_mesh_dev.comp->elem[elem_id].vnd_models[mod_id];
            }
        }
    }

    if (model == NULL || model->pub == NULL || model->pub->msg == NULL) {
        return -BLE_STACK_ERR_PARAM;
    }

    struct net_buf_simple *msg_buf = model->pub->msg;

    bt_mesh_model_msg_init(msg_buf, op);

    net_buf_simple_add_mem(msg_buf, msg, msg_len);

    return bt_mesh_model_publish(model);
}


int ble_stack_mesh_input_num(uint32_t num)
{
    return bt_mesh_input_number(num);
}

int ble_stack_mesh_input_string(char *str)
{
    return bt_mesh_input_string(str);
}

int ble_stack_mesh_cfg_beacon_get(uint16_t net_idx, uint16_t addr, uint8_t *status)
{
    return bt_mesh_cfg_beacon_get(net_idx, addr, status);
}

int ble_stack_mesh_cfg_beacon_set(uint16_t net_idx, uint16_t addr, uint8_t val, uint8_t *status)
{
    return bt_mesh_cfg_beacon_set(net_idx, addr, val, status);
}

int ble_stack_mesh_reset(void)
{
    bt_mesh_reset();
    return 0;
}

int ble_stack_mesh_lpn_set(int enable)
{
    return bt_mesh_lpn_set(enable);
}

int ble_stack_mesh_lpn_poll(void)
{
    return bt_mesh_lpn_poll();
}

int ble_stack_mesh_iv_update_test(int enable)
{
    bt_mesh_iv_update_test(enable);
    return 0;
}

int ble_stack_mesh_iv_update(void)
{
    return bt_mesh_iv_update();
}

