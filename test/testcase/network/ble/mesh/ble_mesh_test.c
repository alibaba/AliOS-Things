/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>

#include "yunit/yunit.h"
#include "yunit/yts.h"

#include <aos/kernel.h>
#include <aos/cli.h>

#include <bluetooth/bluetooth.h>
#include <blemesh.h>


/*
 *API test:
 *1.for ble mesh, API list:
    -*bt_mesh_init 
    -*bt_mesh_reset
    -*bt_mesh_prov_enable
    -*bt_mesh_prov_disable
 -*2.And for below APIs we can test in the provisioning procedure:
    -*bt_mesh_model_msg_init
    -*bt_mesh_model_send
    -*bt_mesh_model_publish
    -*bt_mesh_model_shell_cmd_list
 *for ble func test
 *use provision procdure for connections, note that provisioner is pre-set in another pca10040 platform.
 *
 * */

/*
 *mesh test infos
 * */

#define CID_NVAL   0xffff
#define CID_LOCAL  0x01a8
#define CUR_FAULTS_MAX 4

static struct bt_mesh_cfg_srv cfg_srv = {
    .relay = BT_MESH_RELAY_ENABLED,
    .beacon = BT_MESH_BEACON_ENABLED,
#if defined(CONFIG_BT_MESH_FRIEND)
    .frnd = BT_MESH_FRIEND_ENABLED,
#else
    .frnd = BT_MESH_FRIEND_NOT_SUPPORTED,
#endif
#if defined(CONFIG_BT_MESH_GATT_PROXY)
    .gatt_proxy = BT_MESH_GATT_PROXY_ENABLED,
#else
    .gatt_proxy = BT_MESH_GATT_PROXY_NOT_SUPPORTED,
#endif

    .default_ttl = 7,

    /* 3 transmissions with 20ms interval */
    .net_transmit = BT_MESH_TRANSMIT(2, 20),
    .relay_retransmit = BT_MESH_TRANSMIT(2, 20),
};

static struct bt_mesh_cfg_cli cfg_cli = {
};

static const struct bt_mesh_health_srv_cb health_srv_cb = {
    .fault_get_cur = NULL,
    .fault_get_reg = NULL,
    .fault_clear = NULL,
    .fault_test = NULL,
};

static struct bt_mesh_health_srv health_srv = {
    .cb = &health_srv_cb,
};

static struct bt_mesh_model_pub health_pub = {
    .msg = BT_MESH_HEALTH_FAULT_MSG(CUR_FAULTS_MAX),
};

static void health_current_status(struct bt_mesh_health_cli *cli, u16_t addr,
                  u8_t test_id, u16_t cid, u8_t *faults,
                  size_t fault_count)
{

};

static struct bt_mesh_health_cli health_cli = {
    .current_status = health_current_status,
};

static u8_t dev_uuid[16] = { 0xdd, 0xdd };

static struct bt_mesh_model root_models[] = {
    BT_MESH_MODEL_CFG_SRV(&cfg_srv),
    BT_MESH_MODEL_CFG_CLI(&cfg_cli),
    BT_MESH_MODEL_HEALTH_SRV(&health_srv, &health_pub),
    BT_MESH_MODEL_HEALTH_CLI(&health_cli),
};

static struct bt_mesh_elem elements[] = {
    BT_MESH_ELEM(0, root_models, BT_MESH_MODEL_NONE),
};

static const struct bt_mesh_comp comp = {
    .cid = CID_LOCAL,
    .elem = elements,
    .elem_count = ARRAY_SIZE(elements),
};

static void link_open(bt_mesh_prov_bearer_t bearer)
{
    //printk("Provisioning link opened on %s\n", bearer2str(bearer));
}

static void link_close(bt_mesh_prov_bearer_t bearer)
{
    //printk("Provisioning link closed on %s\n", bearer2str(bearer));
}

static void prov_complete(u16_t net_idx, u16_t addr)
{

}

static void prov_reset(void)
{
    //printk("The local node has been reset and needs reprovisioning\n");
}

static int output_number(bt_mesh_output_action_t action, uint32_t number)
{
    //printk("OOB Number: %u\n", number);
    return 0;
}

static int output_string(const char *str)
{
    //printk("OOB String: %s\n", str);
    return 0;
}

static int input(bt_mesh_input_action_t act, u8_t size)
{
    return 0;
}


static struct bt_mesh_prov prov = {
    .uuid = dev_uuid,
    .link_open = link_open,
    .link_close = link_close,
    .complete = prov_complete,
    .reset = prov_reset,
    //.static_val = NULL,
    //.static_val_len = 0,
    .output_size = 6,
    .output_actions = (BT_MESH_DISPLAY_NUMBER | BT_MESH_DISPLAY_STRING),
    .output_number = output_number,
    .output_string = output_string,
    .input_size = 6,
    .input_actions = (BT_MESH_ENTER_NUMBER | BT_MESH_ENTER_STRING),
    .input = input,
};

int b_mesh_init = false;
static void bt_ready(err)
{
    int ret;
    if(err){
        LOGI("ble", "stack initialized failed");
        return;
    }
    if(!bt_mesh_init(&prov, &comp)){
        b_mesh_init = true;
    }
}

static void test_ble_mesh_init(void)
{
    int err = 0;
    err = bt_enable(bt_ready);
    if(err){
        LOGE("ble", "stack initialized failed");
    }
    LOGI("ble","Stack initialized");
    aos_msleep(2*1000);
    YUNIT_ASSERT_FALSE(b_mesh_init == false);
}

static void test_ble_mesh_prov(void)
{
    
    YUNIT_ASSERT_FALSE(bt_mesh_prov_enable(BT_MESH_PROV_ADV));
    YUNIT_ASSERT_FALSE(bt_mesh_prov_disable(BT_MESH_PROV_ADV));
    YUNIT_ASSERT_FALSE(bt_mesh_prov_enable(BT_MESH_PROV_GATT));
    YUNIT_ASSERT_FALSE(bt_mesh_prov_disable(BT_MESH_PROV_GATT));
    YUNIT_ASSERT_FALSE(bt_mesh_prov_reset());
}



extern int hci_driver_init(void);
static int init(void)
{
    int err = hci_driver_init();
    if(err < 0){
        LOGE("ble", "Driver init failed");
        return -1;
    }
    return 0;
}

static int cleanup(void)
{
    LOGE("ble", "test ble mesh finished");
    return 0;
}

static void setup(void)
{

}

static void teardown(void)
{

}

static yunit_test_case_t aos_ble_mesh_testcases[] = {
    { "ble mesh init", test_ble_mesh_init },
    { "ble mesh provision:", test_ble_mesh_prov },
    YUNIT_TEST_CASE_NULL
};

static yunit_test_suite_t suites[] = {
    { "blemesh", init, cleanup, setup, teardown, aos_ble_mesh_testcases },
    YUNIT_TEST_SUITE_NULL
};

void test_ble(void)
{
    yunit_add_test_suites(suites);
}
AOS_TESTCASE(test_ble);
