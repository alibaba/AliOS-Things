/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <stdio.h>
#include <ali_common.h>
#include <bluetooth/gatt.h>
#include <bluetooth/conn.h>
#include <api_export.h>
#include <dis.h>
#include <ali_core.h>

#define MODEL_ID 0x3126 /* Model ID, obtained from Ali-Cloud. */
#define SOFTWARE_VERSION "0.1.0" /* Version number defined by user. Must be in format "%d.%d.%d". */
#define SOFTWARE_VERSION_LEN 5

uint32_t      m_ali_context[ALI_CONTEXT_SIZE];
uint8_t const m_secret[40] = "sFqTYrjneyyEUlhbZpdOwsDPmShLwMNH8ZHdqLWL";

#define MOD "aisapp"

#define DEVICE_MANUFACURE_NAME "AliOSThings"
#define DEVICE_MODEL_NUM "AIS"
static uint8_t sys_id[] = {0x12, 0x34};

static uint8_t ali_data[] = {0xA8, 0x01, 0x04, 0x0C, 0x7B, 0x9C, 0x00, \
                             0x00, 0xB0, 0xB4, 0x48, 0xD0, 0x78, 0x82};
static struct bt_data ad[3] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA_BYTES(BT_DATA_UUID16_ALL, 0x0a, 0x18),
    BT_DATA(0xFF, ali_data, sizeof(ali_data)),
};

static const struct bt_data sd[] = {
    BT_DATA(BT_DATA_NAME_COMPLETE, CONFIG_BT_DEVICE_NAME, \
            sizeof(CONFIG_BT_DEVICE_NAME) - 1),
};

static void advertising_start(void)
{
    int err;
    uint32_t err_code;
    uint8_t manuf_spec_data_raw[16] = {0};
    uint16_t length = sizeof(manuf_spec_data_raw);
#if 0
    err_code = ali_get_manuf_spec_adv_data(m_ali_context,
                                           manuf_spec_data_raw,
                                           &length);
    if (err_code) {
        LOGE(MOD, "ali_get_manuf_spec_adv_data failed (err: %d)", err_code);
        return;
    }

    ad[2].type = 0xFF;
    ad[2].data_len = length;
    ad[2].data = manuf_spec_data_raw;
#endif
    err = bt_le_adv_start(BT_LE_ADV_CONN, ad, ARRAY_SIZE(ad),
                          sd, ARRAY_SIZE(sd));
    if (err) {
        LOGE(MOD, "Advertising failed to start (err %d)\n", err);
        return;
    }
}

/* @brief Event handler for Ali-SDK. */
static void dev_status_changed_handler(alink_event_t event)
{
    switch (event)
    {
        case CONNECTED:
            LOGI(MOD, "dev_status_changed(): Connected.");
            break;

        case DISCONNECTED:
            LOGI(MOD, "dev_status_changed(): Disconnected.");
            break;

        case AUTHENTICATED:
            LOGI(MOD, "dev_status_changed(): Authenticated.");
            break;

        case TX_DONE:
            LOGI(MOD, "dev_status_changed(): Tx-done.");
            break;

        default:
            break;
    }
}

/* @brief Data handler for query command 0x00. */
static void set_dev_status_handler(uint8_t * buffer, uint32_t length)
{
    /* Flip one of the bits and then echo. */
    buffer[length-1] ^= 1;
    alink_post_fast(buffer, length);
}

/* @brief Data handler for control command 0x02. */
static void get_dev_status_handler(uint8_t * buffer, uint32_t length)
{
    /* Flip one of the bits and then echo. */
    buffer[length-1] ^= 2;
    alink_post(buffer, length);
}

static void get_bd_addr(uint8_t *addr)
{
     memset(addr, 0, BD_ADDR_LEN);
}

static void ali_lib_init(void)
{
    bool ret;
    uint32_t err_code;
    struct device_config init_alink;
    uint8_t bd_addr[BD_ADDR_LEN];

    get_bd_addr(bd_addr);

    memset(&init_alink, 0, sizeof(struct device_config));
    init_alink.shortmodel        = MODEL_ID;
    init_alink.status_changed_cb = dev_status_changed_handler;
    init_alink.set_cb            = set_dev_status_handler;
    init_alink.get_cb            = get_dev_status_handler;
    init_alink.enable_ota        = false;
    init_alink.enable_auth       = true;
    memcpy(init_alink.bd_addr, bd_addr, BD_ADDR_LEN);
    memcpy(init_alink.secret, m_secret, sizeof(m_secret));
    memcpy(init_alink.version, SOFTWARE_VERSION, SOFTWARE_VERSION_LEN);

    ret = alink_start(&init_alink);
    if (ret != 0) LOGE(MOD, "alink_start failed.");
}

static void bt_ready(int err)
{
    ali_lib_init();
    printf("Bluetooth init succeed.\n");
    dis_init(DEVICE_MODEL_NUM, DEVICE_MANUFACURE_NAME);
    printf("dis_init succeed.\n");
    advertising_start();
    printf("advertising_start succeed.\n");
}

extern int hci_driver_init();
static void app_delayed_action(void *arg)
{
    int err;

    (void *)arg;

    hci_driver_init();
    err = bt_enable(bt_ready);
    if (err) {
        printf("Bluetooth init failed (err %d)\n", err);
        return;
    }
    printf("Bluetooth init succeed.\n");
}

int application_start( void )
{
    aos_post_delayed_action(1000, app_delayed_action, NULL);
    aos_loop_run();
    return 0;
}
