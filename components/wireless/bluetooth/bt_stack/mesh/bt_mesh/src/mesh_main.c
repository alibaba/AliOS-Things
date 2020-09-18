/*  Bluetooth Mesh */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <stdbool.h>
#include <errno.h>

#include <net/buf.h>
//#include <conn.h>
#include <api/mesh.h>

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_MESH_DEBUG)

#include "common/log.h"

#include "test.h"
#include "adv.h"
#include "prov.h"
#include "../inc/net.h"
#include "beacon.h"
#include "lpn.h"
#include "friend.h"
#include "transport.h"
#include "access.h"
#include "foundation.h"
#include "proxy.h"
#include "mesh.h"
#include <mesh/cfg_cli.h>
#include <mesh/cfg_srv.h>
#include <mesh/health_cli.h>
#include <mesh/health_srv.h>
#include <stdio.h>
#include <stdlib.h>

#include "bt_mesh_custom_log.h"

#define MESH_DATA_FILE "/data/meshdata.dat"
extern u16_t dev_primary_addr;

static struct bt_mesh_data_info_t mesh_data;
static struct bt_mesh_user_t *g_mesh_user = NULL;
static bool g_bt_mesh_aggressive_setting = 0;

static bool g_bt_mesh_init_flag = false;
#if 0
static void health_current_status(struct bt_mesh_health_cli *cli, u16_t addr,
                u8_t test_id, u16_t cid, u8_t *faults,
                size_t fault_count);
#endif

static bool provisioned;

static volatile bool provisioner_en;

static u8_t dev_uuid[16] = {0xdd, 0xdd};


static struct bt_mesh_cfg_cli cfg_cli = {
};

#ifdef CONFIG_BT_MESH_HEALTH_CLI
#if 0
static struct bt_mesh_health_cli health_cli = {
    .current_status = health_current_status,
};
#endif
#endif

static struct bt_mesh_model root_models[] = {
    BT_MESH_MODEL_CFG_CLI(&cfg_cli),
    //BT_MESH_MODEL_HEALTH_CLI(),   define macro error
};

#if 0
static struct bt_mesh_model vendor_models[] = {
};
#endif

static struct bt_mesh_elem elements[] = {
    {0, 0xF000, 0, ARRAY_SIZE(root_models), 0, root_models, NULL},
};

static const struct bt_mesh_comp comp = {
    0x01A8,
    0x0,
    0x0,
    ARRAY_SIZE(elements),
    elements,
};
static struct bt_mesh_prov prov;

extern void schedule_task_init(void);
extern int bt_mesh_schedule_adv_handle(int msg_id, void *pMsg);
extern int bt_mesh_schedule_prov_handle(int msg_id, void *pMsg);
extern int bt_mesh_schedule_sigmesh_handle(int msg_id, void *pMsg);
extern int scheduler_msg_send(int msg_id, void *pMsg);
extern void bt_mesh_scan_a2dp_state_setting(bool state);
int bt_mesh_enable_aggressive_setting(bool enable);
int bt_mesh_enable_aggressiveSetting_handle(bool enable);
int bt_mesh_set_a2dp_state_handle(bool state);

struct bt_mesh_model *bt_mesh_elem_first_model(void)
{
    return elements[0].models;
}

void bt_mesh_setup(u32_t seq, u16_t addr)
{
    bt_mesh.seq = seq;

    bt_mesh_comp_provision(addr);
    provisioned = true;

#ifdef CONFIG_BT_MESH_BEACON
    if (bt_mesh_beacon_get() == BT_MESH_BEACON_ENABLED) {
        bt_mesh_beacon_enable();
    } else {
        bt_mesh_beacon_disable();
    }
#endif

#ifdef CONFIG_BT_MESH_GATT_PROXY
    if (IS_ENABLED(CONFIG_BT_MESH_GATT_PROXY) &&
        bt_mesh_gatt_proxy_get() != BT_MESH_GATT_PROXY_NOT_SUPPORTED) {
        bt_mesh_proxy_gatt_enable();
#ifdef CONFIG_BT_MESH_BEACON
        bt_mesh_beacon_get();
#endif
    }
#endif

    if (IS_ENABLED(CONFIG_BT_MESH_LOW_POWER)) {
        bt_mesh_lpn_init();
    } else {
        bt_mesh_scan_enable();
    }

    if (IS_ENABLED(CONFIG_BT_MESH_FRIEND)) {
        bt_mesh_friend_init();
    }
}


int bt_mesh_provision(const u8_t net_key[16], u16_t net_idx,
              u8_t flags, u32_t iv_index, u32_t seq,
              u16_t addr, const u8_t dev_key[16])
{
    int err;
    struct bt_mesh_user_t *user = NULL;
    
    BT_INFO("Primary Element: 0x%04x\n", addr);
    BT_DBG("net_idx 0x%04x flags 0x%02x iv_index 0x%04x\n",
           net_idx, flags, iv_index);

    if (IS_ENABLED(CONFIG_BT_MESH_PB_GATT)) {
        bt_mesh_proxy_prov_disable();
    }
  
    user = mesh_main_get_mesh_user(addr, net_idx, true);
    if (user != NULL){
        BT_DBG("[%s]-->use the local information.iv:%x,seq:%d\n",__func__, user->iv_index,user->seq);     
        iv_index = user->iv_index;
        seq = user->seq;
    }else{                
        BT_DBG("[%s]-->read the local information failed,used default infomation.\n",__func__);
        mesh_data_add_user_info(addr, net_idx, iv_index, seq, (u8_t *)net_key, true);               
    }

    err = bt_mesh_net_create(net_idx, flags, net_key, iv_index);
    if (err) {
        if (IS_ENABLED(CONFIG_BT_MESH_PB_GATT)) {
            bt_mesh_proxy_prov_enable();
        }

        return err;
    }

    memcpy(bt_mesh.dev_key, dev_key, 16);

    bt_mesh_setup(seq, addr);

    if (IS_ENABLED(CONFIG_BT_MESH_PROV)) {
        bt_mesh_prov_complete(net_idx, addr);
    }

    return 0;
}

void bt_mesh_reset(void)
{
    if (!provisioned) {
        return;
    }

    bt_mesh_comp_unprovision();

    bt_mesh.iv_index = 0;
    bt_mesh.seq = 0;
    bt_mesh.iv_update = 0;
    bt_mesh.pending_update = 0;
    bt_mesh.valid = 0;
    bt_mesh.last_update = 0;
    bt_mesh.ivu_initiator = 0;

    k_delayed_work_cancel(&bt_mesh.ivu_complete);

#ifdef CONFIG_BT_MESH_CFG_SRV
    bt_mesh_cfg_reset();
#endif

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

    memset(bt_mesh.dev_key, 0, sizeof(bt_mesh.dev_key));

    memset(bt_mesh.rpl, 0, sizeof(bt_mesh.rpl));

    provisioned = false;

    bt_mesh_scan_disable();

#ifdef CONFIG_BT_MESH_BEACON
    bt_mesh_beacon_disable();
#endif
    if (IS_ENABLED(CONFIG_BT_MESH_PROV)) {
        bt_mesh_prov_reset();
    }
}

bool bt_mesh_is_provisioned(void)
{
    return provisioned;
}

int bt_mesh_prov_enable(bt_mesh_prov_bearer_t bearers)
{
    if (bt_mesh_is_provisioned()) {
        return -EALREADY;
    }

    if (IS_ENABLED(CONFIG_BT_MESH_PB_ADV) &&
        (bearers & BT_MESH_PROV_ADV)) {
        /* Make sure we're scanning for provisioning inviations */
        bt_mesh_scan_enable();
#ifdef CONFIG_BT_MESH_BEACON
        /* Enable unprovisioned beacon sending */
        bt_mesh_beacon_enable();
#endif
    }

    if (IS_ENABLED(CONFIG_BT_MESH_PB_GATT) &&
        (bearers & BT_MESH_PROV_GATT)) {
        bt_mesh_proxy_prov_enable();
        bt_mesh_adv_update();
    }

    return 0;
}

int bt_mesh_prov_disable(bt_mesh_prov_bearer_t bearers)
{
    if (bt_mesh_is_provisioned()) {
        return -EALREADY;
    }

    if (IS_ENABLED(CONFIG_BT_MESH_PB_ADV) &&
        (bearers & BT_MESH_PROV_ADV)) {
#ifdef CONFIG_BT_MESH_BEACON
        bt_mesh_beacon_disable();
#endif
        bt_mesh_scan_disable();
    }

    if (IS_ENABLED(CONFIG_BT_MESH_PB_GATT) &&
        (bearers & BT_MESH_PROV_GATT)) {
        bt_mesh_proxy_prov_disable();
        bt_mesh_adv_update();
    }

    return 0;
}

/**
 * @brief get bt mesh node count per userid.
 *
 * @param void    .        
 * @return
 * @note: the count is a const number now, wo will change it to dynamic.
*/
u32_t bt_mesh_genie_get_node_count(void)
{
//    return ARRAY_SIZE(mesh_nodes);
    return 0;
}

/**
 * @brief get bt mesh user number per ali_genie.
 *
 * @param void    .        
 * @return
 * @note: the count is a const number now, we will change it to dynamic.
*/
u32_t bt_mesh_genie_get_userid_count(void)
{
    return ARRAY_SIZE(mesh_data.mesh_user);
}

/**
 * @brief save mesh data to flash.
 *
 * @param void
 * @return
 * @note: N/A
*/
static int mesh_main_save_mesh_data()
{

    FILE *fp;
    int ret = 0;

    BT_WARN("%s", __func__);

    fp = fopen(MESH_DATA_FILE, "wb");
    if(fp == NULL) {
        BT_ERR("open mesh-data file failed\n");
        return -ENOENT;
    }
    ret = fwrite(&mesh_data, 1, sizeof(mesh_data), fp);
    if (ret != sizeof(mesh_data)) {
        BT_ERR("save mesh-data file failed\n");
        fclose(fp);
        return -EIO;
    }else {
        BT_DBG("save mesh-data file succeed\n");
        fclose(fp);
        return 0;
    }
}

/**
 * @brief load the total mesh data from flash.
 *
 * @param void
 * @return
 * @note: N/A
*/
static int mesh_main_load_mesh_data()
{
    FILE *fp;
    int ret = 0;

    BT_WARN("%s", __func__);

    fp = fopen(MESH_DATA_FILE, "rb");
    if (fp == NULL) {
        BT_ERR("open mesh-data file failed\n");
        return -ENOENT;
    } else {
        ret = fread(&mesh_data, 1, sizeof(mesh_data), fp);
        if(ret != sizeof(mesh_data)) {
           BT_ERR("read mesh-data size(0x%x) failed\n",ret);
           fclose(fp);
           return -EIO;
        }
    }   
    fclose(fp);

    return 0;
}

/**
 * @brief load the total mesh data from flash.
 *
 * @param genie_uniaddr  tianmao's unicast address,one per user_id. 
 * @param net_id 
 * @param is_need_load, true   
 * @return
 * @note: N/A
*/
struct bt_mesh_user_t *mesh_main_get_mesh_user(u16_t genie_uniaddr, u16_t net_id, bool is_need_load)
{
    int i,ret;    

    BT_DBG("%s\n", __func__);
    if (genie_uniaddr == 0) {
        BT_ERR("%s Invaid genie_uniaddr %x,plesase check it!\n", __func__, genie_uniaddr);
        return NULL;
    }

    if(is_need_load) {
        ret = mesh_main_load_mesh_data();
        if (ret != 0) {
            BT_ERR("%s load mesh data failed!\n", __func__);
        }
    }

    for (i = 0; i < bt_mesh_genie_get_userid_count(); i++) {
        if ((mesh_data.mesh_user[i].net_idx == net_id) && (mesh_data.mesh_user[i].genie_addr == genie_uniaddr)) {
            g_mesh_user = &mesh_data.mesh_user[i];
            g_mesh_user->seq += 100;
            mesh_main_save_mesh_data();
            return g_mesh_user;
        }       
    }
    return NULL;
}

/**
 * @brief save user info
 *
 * @param byte_num    
 * @param data        
 * @return
 * @note:
*/
bool mesh_main_save_user_info(void)
{
    int ret;    

    BT_DBG("%s\n", __func__);

    if (g_mesh_user == NULL) {
        BT_ERR("%s Invaid Para,plesase check it!\n", __func__);
        return false;
    }
    g_mesh_user->genie_addr = dev_primary_addr;
    g_mesh_user->seq = bt_mesh.seq;
    g_mesh_user->iv_update = bt_mesh.iv_index;
    g_mesh_user->net_idx = bt_mesh.sub[0].net_idx;

    BT_DBG("%s genie_addr:%d,iv_index:%d,seq:%d,net_idx:%d\n",
                    __func__,
                    g_mesh_user->genie_addr,
                    g_mesh_user->iv_index,
                    g_mesh_user->seq, 
                    g_mesh_user->net_idx);

    ret = mesh_main_save_mesh_data();
    if (ret != 0) {
        BT_ERR("%s mesh_main_save_user_info failed!\n", __func__);
        return false;
    }

    return true;
}

int mesh_data_add_user_info(u16_t genie_uniaddr, u16_t net_id, u32_t iv_index, u32_t seq, u8_t *netkey, bool save_flag)
{
    struct bt_mesh_user_t *user = NULL;
    int ret;    
    u16_t user_index, first_free_slot;

    first_free_slot = 0xFFFF;

    BT_DBG("%s\n", __func__);
    
    if (genie_uniaddr == 0) {
        BT_ERR("%s Invaid genie_uniaddr, plesase check it!\n", __func__);
        return -ENXIO;
    }
    for (user_index = 0; user_index < CONFIG_MAX_GENIE_USERS; user_index++) {
        user = &mesh_data.mesh_user[user_index];
        if (user->genie_addr == genie_uniaddr && user->net_idx == net_id) {
            BT_DBG("%s, modify user_id to slot %d\n", __func__, user_index);
            //memcpy(user->net_key, netkey, 16); /*not used here*/
            user->seq = seq;
            user->iv_index = iv_index;
            goto save_data;
        }
        if (user->active == false) {
            first_free_slot = user_index;
            BT_DBG("%s, add user_id to slot %d\n", __func__, user_index);
            break;   
        }        
    }

    if (first_free_slot == 0xFFFF) {        
        BT_WARN("mesh_user_list is full, reset the old item.\n");
        first_free_slot = mesh_data.older_uid++;

        if (mesh_data.older_uid >= CONFIG_MAX_GENIE_USERS) {
            BT_DBG("%s, mesh_data.older_uid is full,reset it!%d\n", __func__);
            mesh_data.older_uid = 0;
        }
    }

    mesh_data.mesh_user[first_free_slot].seq = seq;    
    mesh_data.mesh_user[first_free_slot].iv_index = iv_index;
    mesh_data.mesh_user[first_free_slot].genie_addr = genie_uniaddr;
    mesh_data.mesh_user[first_free_slot].net_idx = net_id;
    mesh_data.mesh_user[first_free_slot].active = true;

    g_mesh_user = &mesh_data.mesh_user[first_free_slot];
save_data:   
    BT_DBG("%s genie_addr%d,iv_index:%d,seq:%d,net_idx:%d\n",
                __func__,
                mesh_data.mesh_user[first_free_slot].genie_addr,
                mesh_data.mesh_user[first_free_slot].iv_index,
                mesh_data.mesh_user[first_free_slot].seq, 
                mesh_data.mesh_user[first_free_slot].net_idx);
    if (save_flag) {
        ret = mesh_main_save_mesh_data();
        if (ret != 0) {
            BT_ERR("%s genie_save_mesh_data failed!\n", __func__);
            return -ENOMEM;
        }
    }    
    return 0;
}

#ifdef CONFIG_MESH_STACK_ALONE
static void bt_mesh_prepare()
{
    k_work_q_start();
}
#endif

int bt_mesh_schedule_sigmesh_handle(int msg_id, void *pMsg)
{
    switch (msg_id) {
    case MSG_ID_SIGMESH_AGGRESSIVE:
        {
            struct sigmesh_aggressive_t *msg = (struct sigmesh_aggressive_t *)pMsg;

            if (msg) {
                bt_mesh_enable_aggressiveSetting_handle(msg->enable);
            } else {
                BT_ERR("MSG_ID_SIGMESH_AGGRESSIVE: Invalide Parameter");
            }
        }
        break;
    case MSG_ID_SIGMESH_A2DP_STATE:
        {
            struct sigmesh_a2dp_state_t *msg = (struct sigmesh_a2dp_state_t *)pMsg;

            if (msg) {
                bt_mesh_set_a2dp_state_handle(msg->state);
            } else {
                BT_ERR("MSG_ID_SIGMESH_A2DP_STATE: Invalide Parameter");
            }
        }
        break;
    }

    return 0;
}

void bt_mesh_schedule_message_handle(int msg_id, void *msg)
{
    int msg_group, msg_subid;

//    BT_DBG("%s: message id = %d, pMsg = %x", __func__, msg_id, msg);
    msg_group = MSG_GROUP(msg_id);
    msg_subid = MSG_SUBID(msg_id);
    switch (msg_group) {
    case MSG_GROUP_ADV:
        bt_mesh_schedule_adv_handle(msg_subid, msg);
        break;
    case MSG_GROUP_PROV:
        bt_mesh_schedule_prov_handle(msg_subid, msg);
        break;
    case MSG_GROUP_SIGMESH:
        bt_mesh_schedule_sigmesh_handle(msg_subid, msg);
        break;
    default:
        BT_ERR("%s: unhandled message", __func__);
        break;
    }

    if (msg)
        k_free(msg);
}

int bt_mesh_schedule_message_send(int msg_id, void *msg)
{
    int ret;

//    BT_DBG("%s: message id = %d, pMsg = %x", __func__, msg_id, msg);

    ret = scheduler_msg_send(msg_id, msg);
    if (ret)
        BT_ERR("%s: message send fail", __func__);

    return ret;
}

int bt_mesh_init(const struct bt_mesh_prov *prov,
                    const struct bt_mesh_comp *comp,
                    const struct bt_mesh_provisioner *provisioner)
{
    int err;

    if (g_bt_mesh_init_flag) {
        BT_ERR("%s Already inited", __func__);
        return -EALREADY;
    }

    g_bt_mesh_init_flag = 1;
    BT_INFO("%s-->\n", __func__);
#ifdef CONFIG_MESH_STACK_ALONE
    bt_mesh_prepare();
#endif

#ifdef CONFIG_MESH_STACK_FOR_GENIE
    schedule_task_init();
#endif

    err = bt_mesh_comp_register(comp);
    if (err) {
        return err;
    }

    if (IS_ENABLED(CONFIG_BT_MESH_PROV)) {
        err = bt_mesh_prov_init(prov);
        if (err) {
            return err;
        }
    }

    bt_mesh_net_init();
    bt_mesh_trans_init();
#ifdef CONFIG_BT_MESH_BEACON
    bt_mesh_beacon_init();
#endif

    bt_mesh_adv_init();
    if (IS_ENABLED(CONFIG_BT_MESH_GATT_PROXY)) {
        bt_mesh_proxy_init();
    }
    BT_INFO("bt_mesh_adv_init done\n");

    return 0;
}

#if 0
static void prov_complete(u16_t net_idx, u16_t addr)
{

}

static void prov_reset(void)
{

}

static void link_open(bt_mesh_prov_bearer_t bearer)
{

}

static void link_close(bt_mesh_prov_bearer_t bearer)
{

}
#endif

#ifdef CONFIG_BT_MESH_HEALTH_CLI
#if 0
void show_faults(u8_t test_id, u16_t cid, u8_t *faults, size_t fault_count)
{
    size_t i;

    if (!fault_count) {
        return;
    }

    for (i = 0; i < fault_count; i++) {

    }
}
static void health_current_status(struct bt_mesh_health_cli *cli, u16_t addr,
                u8_t test_id, u16_t cid, u8_t *faults,
                size_t fault_count)
{
    show_faults(test_id, cid, faults, fault_count);
}
#endif
#endif

int RTOSGenie_mesh_provisioner_scan(u8_t start, s32_t duration)
{
    int ret = 0;

    ret = bt_mesh_prov_scan(start, duration);

    return ret;
}

int RTOSGenie_mesh_init(void)
{
    int ret;

    /* prov needed for provisioner */
    prov.uuid = dev_uuid;
    prov.link_open = NULL;
    prov.link_close = NULL;
    prov.complete = NULL;
    prov.reset = NULL;
    //prov.static_val = NULL;
    //prov.static_val_len = 0;
    prov.output_size = 6;
    prov.output_actions = (BT_MESH_DISPLAY_NUMBER | BT_MESH_DISPLAY_STRING);
    prov.output_number = NULL;
    prov.output_string = NULL;
    prov.input_size = 6;
    prov.input_actions = (BT_MESH_ENTER_NUMBER | BT_MESH_ENTER_STRING);
    prov.input = NULL;

    comp.elem[0].models[0].groups[0] = 0xF000;
    ret = bt_mesh_init(&prov, &comp, NULL);

    return ret;
}

int bt_mesh_enable_aggressiveSetting_handle(bool enable)
{
    BT_INFO("bt_mesh_enable_aggressiveSetting, enable = %d\n", enable);
    bt_mesh_enable_aggressive_setting(enable);

#ifdef ADV_ONE_SHOT
    if(false == enable) {
        bt_mesh_scan_disable();
        bt_mesh_scan_enable_setting(0x20, 0x50);
    }
#else
    if (g_bt_mesh_aggressive_setting != enable)
        bt_mesh_scan_agressive_setting(enable);
#endif
    g_bt_mesh_aggressive_setting = enable;
    return 0;
}

int bt_mesh_enable_aggressiveSetting(bool enable)
{
    int ret;
    int msg_id;
    struct sigmesh_aggressive_t *msg;

    BT_DBG("%s", __func__);
    msg = (struct sigmesh_aggressive_t *)k_malloc(sizeof(struct sigmesh_aggressive_t));
    if (msg == NULL) {
        BT_ERR("%s: malloc failed", __func__);
        return -1;
    }

    msg->enable = enable;

    msg_id = MSG_ID(MSG_GROUP_SIGMESH, MSG_ID_SIGMESH_AGGRESSIVE);
    ret = bt_mesh_schedule_message_send(msg_id, msg);
    if (ret) {
        BT_ERR("%s: message send err %d", __func__, ret);
        k_free(msg);
    }
}

int bt_mesh_set_a2dp_state_handle(bool state)
{
    BT_INFO("bt_mesh_enable_set_a2dp_state_handle, state = %d\n", state);

    bt_mesh_scan_a2dp_state_setting(state);

    return 0;
}

int bt_mesh_set_a2dp_state(bool state)
{
    int ret;
    int msg_id;
    struct sigmesh_a2dp_state_t *msg;

    BT_DBG("%s", __func__);
    msg = (struct sigmesh_a2dp_state_t *)k_malloc(sizeof(struct sigmesh_a2dp_state_t));
    if (msg == NULL) {
        BT_ERR("%s: malloc failed", __func__);
        return -1;
    }

    msg->state = state;

    msg_id = MSG_ID(MSG_GROUP_SIGMESH, MSG_ID_SIGMESH_A2DP_STATE);
    ret = bt_mesh_schedule_message_send(msg_id, msg);
    if (ret) {
        BT_ERR("%s: message send err %d", __func__, ret);
        k_free(msg);
    }
}


bool bt_mesh_get_aggressiveSetting(void)
{
    return g_bt_mesh_aggressive_setting;
}
