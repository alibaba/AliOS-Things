/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "aos/kernel.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <breeze.h>

#define BZ_PRINT   printf

#if defined(OTA_CONFIG_BLE)
#include "ota/ota_agent.h"
static ota_service_t ctx = {0};
#endif

/*device info defination*/
#ifndef CONFIG_COMP_BZ_MODEL_SEC
#define PRODUCT_ID     850958
#define DEVICE_SECRET  "jDp6NxUImRfZ231nt5Nt1AjEVYPJF4e3"
#define DEVICE_NAME    "new_bz_test123"
#define PRODUCT_KEY    "a1oZpFjRUf3"
#define PRODUCT_SECRET "eZ2JTVukkTz0juw0"
#else
#define PRODUCT_ID     577245
#define DEVICE_SECRET  "FFbidcX3hIuMMFeg70WzCdNEjZL6NZLE"
#define DEVICE_NAME    "12345678901234567890"
#define PRODUCT_KEY    "a1qWr3Wk3Oo"
#define PRODUCT_SECRET "oFTYrP7idjkYEBL9"
#endif

static bool ble_connected = false;

/* @brief Event handler for Ali-SDK. */
static void dev_status_changed_handler(breeze_event_t event)
{
    switch (event) {
        case CONNECTED:
            ble_connected = true;
            BZ_PRINT("dev_status_changed(): Connected.\n");
            break;

        case DISCONNECTED:
            ble_connected = false;
            BZ_PRINT("dev_status_changed(): Disconnected.\n");
            break;

        case AUTHENTICATED:
            BZ_PRINT("dev_status_changed(): Authenticated.\n");
            break;

        case TX_DONE:
            break;

        default:
            break;
    }
}

/* @brief Data handler for control command 0x00. */
static void set_dev_status_handler(uint8_t *buffer, uint32_t length)
{
    BZ_PRINT("%s command (len: %u) received.\r\n", __func__, length);
}

/* @brief Data handler for query command 0x02. */
static void get_dev_status_handler(uint8_t *buffer, uint32_t length)
{
    /* echo the receiving data */
    uint8_t cmd = 0x03;
    breeze_post_ext(cmd, buffer, length);
    BZ_PRINT("%s command (len: %u) received.\r\n", __func__, length);
}

#ifdef CONFIG_COMP_BZ_SECURE_ADV
/* @brief Continue advertising func, restart ble adv with data reload. */
static void continue_adv_work(void *arg)
{
    static uint8_t user_adv[] = {0x55, 0xaa};

    user_adv[0]++;
    user_adv[1]++;
    breeze_append_adv_data(user_adv, sizeof(user_adv) / sizeof(user_adv[0]));

    if (!ble_connected) breeze_restart_advertising();
    aos_post_delayed_action(2000, continue_adv_work, NULL);
}
#endif
/* @brief Callback when there is AWSS info to get. */
static void apinfo_handler(breeze_apinfo_t *ap)
{
    BZ_PRINT("AWSS event post %s\r\n", __func__);
}

/* @brief Default callbacks for ota event, users should overwrite when OTA is enabled. */
static void ota_handler(breeze_otainfo_t *ota)
{
    if(ota != NULL){
        if(ota->type == OTA_CMD){
            BZ_PRINT("OTA cmd(%02x) post\n", ota->cmd_evt.m_cmd.cmd);
        } else if(ota->type == OTA_EVT){
            BZ_PRINT("OTA evt(%02x) post\n", ota->cmd_evt.m_evt.evt);
        } else{
            BZ_PRINT("unknown ota info\r\n");
        }

    }
}

/* @brief application entry for breeze SDK. */
static void breeze_work(void *arg)
{
    bool                 ret;
    uint32_t             err_code;
    struct device_config init_bzlink;
    uint8_t              bd_addr[BD_ADDR_LEN] = { 0 };
    (void)arg;
    
    /*intialize struct device_config with predfined device info and callbacks*/
    memset(&init_bzlink, 0, sizeof(struct device_config));
    init_bzlink.product_id        = PRODUCT_ID;
    init_bzlink.status_changed_cb = dev_status_changed_handler;
    init_bzlink.set_cb            = set_dev_status_handler;
    init_bzlink.get_cb            = get_dev_status_handler;
    init_bzlink.apinfo_cb         = apinfo_handler;


    init_bzlink.product_secret_len = strlen(PRODUCT_SECRET);
    memcpy(init_bzlink.product_secret, PRODUCT_SECRET,
           init_bzlink.product_secret_len);

    init_bzlink.product_key_len = strlen(PRODUCT_KEY);
    memcpy(init_bzlink.product_key, PRODUCT_KEY, init_bzlink.product_key_len);
    
    init_bzlink.device_key_len = strlen(DEVICE_NAME);
    memcpy(init_bzlink.device_key, DEVICE_NAME, init_bzlink.device_key_len);

#ifndef CONFIG_COMP_BZ_MODEL_SEC
    /*For security per device, initialize device_config.secret and device_config.secret_len*/
    init_bzlink.secret_len = strlen(DEVICE_SECRET);
    memcpy(init_bzlink.secret, DEVICE_SECRET, init_bzlink.secret_len);
#else
    /*Otherwise for security per product,initialize device_config.secret_len to 0*/
    init_bzlink.secret_len = 0;
#endif

#ifdef CONFIG_COMP_BZ_OTA
    ota_service_init(&ctx);
    init_bzlink.ota_cb = ctx.on_message;
#else
    init_bzlink.ota_cb = ota_handler;
#endif
    /*Start breeze service*/
    ret = breeze_start(&init_bzlink);
    if (ret != 0) {
        BZ_PRINT("breeze_start failed.\r\n");
    } else {
        BZ_PRINT("breeze_start succeed.\r\n");
    }
}

int application_start(int argc, char **argv)
{
    breeze_work(NULL);

#ifdef CONFIG_COMP_BZ_SECURE_ADV
    continue_adv_work(NULL);
#endif
    return 0;
}
