/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <stdio.h>
#include <stdbool.h>
#include <breeze_export.h>

#define SOFTWARE_VERSION "0.2.0"
#define SOFTWARE_VERSION_LEN 5

#define PRODUCT_ID     213226
#define DEVICE_SECRET  "cNwnA4W7amnkgG6s8zGXSJD3nI1c7kO1"
#define DEVICE_NAME    "112233445566"
#define PRODUCT_KEY    "b1XVhqfan1X"
#define PRODUCT_SECRET "iX6XqAjaCTXBv4h3"

static bool ble_connected = false;

/* @brief Event handler for Ali-SDK. */
static void dev_status_changed_handler(breeze_event_t event)
{
    switch (event) {
        case CONNECTED:
            ble_connected = true;
            printf("dev_status_changed(): Connected.\n");
            break;

        case DISCONNECTED:
            ble_connected = false;
            printf("dev_status_changed(): Disconnected.\n");
            break;

        case AUTHENTICATED:
            printf("dev_status_changed(): Authenticated.\n");
            break;

        case TX_DONE:
            printf("dev_status_changed(): Tx-done.\n");
            break;

        default:
            break;
    }
}

/* @brief Data handler for control command 0x00. */
static void set_dev_status_handler(uint8_t *buffer, uint32_t length)
{
    printf("%s command (len: %d) received.\r\n", __func__, length);
}

/* @brief Data handler for query command 0x02. */
static void get_dev_status_handler(uint8_t *buffer, uint32_t length)
{
    /* Flip one of the bits and then echo. */
    buffer[length - 1] ^= 2;
    breeze_post(buffer, length);
}

#ifdef CONTINUE_BEL_ADV
static void adv_work(void *arg)
{
#ifdef APPEND_USER_ADV
    static uint8_t user_adv[] = {0x55, 0xaa};

    user_adv[0]++;
    user_adv[1]++;
    breeze_append_adv_data(user_adv, sizeof(user_adv) / sizeof(user_adv[0]));
#endif

    if (!ble_connected) breeze_restart_advertising();

    aos_post_delayed_action(2000, adv_work, NULL);
}
#endif

static void apinfo_handler(breeze_apinfo_t *ap)
{
    printf("Hello %s\r\n", __func__);
}

static void alink_work(void *arg)
{
    bool                 ret;
    uint32_t             err_code;
    struct device_config init_alink;
    uint8_t              bd_addr[BD_ADDR_LEN] = { 0 };

    (void)arg;

    memset(&init_alink, 0, sizeof(struct device_config));
    init_alink.product_id        = PRODUCT_ID;
    init_alink.status_changed_cb = dev_status_changed_handler;
    init_alink.set_cb            = set_dev_status_handler;
    init_alink.get_cb            = get_dev_status_handler;
    init_alink.apinfo_cb         = apinfo_handler;
#ifdef CONFIG_AIS_OTA
    init_alink.enable_ota = true;
#endif
    init_alink.enable_auth = true;
    init_alink.auth_type   = ALI_AUTH_BY_PRODUCT_SECRET;

    init_alink.secret_len = strlen(DEVICE_SECRET);
    memcpy(init_alink.secret, DEVICE_SECRET, init_alink.secret_len);

    init_alink.product_secret_len = strlen(PRODUCT_SECRET);
    memcpy(init_alink.product_secret, PRODUCT_SECRET,
           init_alink.product_secret_len);

    init_alink.product_key_len = strlen(PRODUCT_KEY);
    memcpy(init_alink.product_key, PRODUCT_KEY, init_alink.product_key_len);

    init_alink.device_key_len = strlen(DEVICE_NAME);
    memcpy(init_alink.device_key, DEVICE_NAME, init_alink.device_key_len);

    memcpy(init_alink.version, SOFTWARE_VERSION, SOFTWARE_VERSION_LEN);

    ret = breeze_start(&init_alink);
    if (ret != 0) {
        printf("breeze_start failed.\r\n");
    } else {
        printf("breeze_start succeed.\r\n");
    }
}

int application_start(void)
{
    alink_work(NULL);

#ifdef CONTINUE_BEL_ADV
    aos_post_delayed_action(5000, adv_work, NULL);
#endif

    breeze_event_dispatcher();
    return 0;
}
