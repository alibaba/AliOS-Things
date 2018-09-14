/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef BREEZE_API_EXPORT_H
#define BREEZE_API_EXPORT_H

#if defined(__cplusplus) /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>

#define BD_ADDR_LEN      (6)      /**< Length of Bluetooth Device Address. */
#define STR_MODEL_LEN    (20 + 1) /**< Reserved. */
#define STR_SEC_LEN      (40 + 1) /**< Length of device secret. */
#define STR_PROD_SEC_LEN (32 + 1) /**< Length of product secret. */
#define STR_VER_LEN      (8 + 1)  /**< Length of version. */
#define STR_PROD_KEY_LEN (20 + 1) /**< Length of product key. */
#define STR_DEV_KEY_LEN  (32 + 1) /**< Length of product key. */

/***** BLE STATUS ******/
typedef enum
{
    CONNECTED,     // connect with phone success
    DISCONNECTED,  // lost connection with phone
    AUTHENTICATED, // success authentication
    TX_DONE,       // send data complete
    NONE
} breeze_event_t;

typedef enum
{
    ALI_AUTH_BY_PRODUCT_SECRET, /**< Authentication by product secret. */
    ALI_AUTH_BY_DEVICE_SECRET,  /**< Authentication by device secret. */
} ali_auth_type_t;

typedef struct
{
    char    ssid[32 + 1];
    char    pw[64 + 1];
    uint8_t bssid[6];
} breeze_apinfo_t;

typedef struct {
    uint32_t product_id;
    char *product_key;
    char *product_secret;
    char *device_name;
    char *device_secret;
} breeze_dev_info_t;

/**
 * @brief Callback when device status changed.
 *
 * @param[in] status @n Device Status.
 * @return None.
 * @see None.
 * @note This API should be implemented by user, and will be called by SDK
 *       when device statuc changed, e.g. bluetooth connection status change.
 */
typedef void (*dev_status_changed_cb)(breeze_event_t event);

/**
 * @brief Callback when there is device status to set.
 *
 * @param[in] buffer @n The data to be set.
 * @param[in] model @n Length of the data.
 * @return None.
 * @see None.
 * @note This API should be implemented by user and will be called by SDK.
 */
typedef void (*set_dev_status_cb)(uint8_t *buffer, uint32_t length);

/**
 * @brief Callback when there is device status to get.
 *
 * @param[out] buffer @n The data of device status.
 * @param[out] model @n Length of the data.
 * @return None.
 * @see None.
 * @note This API should be implemented by user and will be called by SDK.
 */
typedef void (*get_dev_status_cb)(uint8_t *buffer, uint32_t length);

typedef void (*apinfo_ready_cb)(breeze_apinfo_t *ap);

/**
 * This structure includes the information which is 
 * required to initialize the SDK.
 */
struct device_config
{
    uint8_t         bd_addr[BD_ADDR_LEN];
    char            model[STR_MODEL_LEN];
    uint32_t        product_id;
    char            product_key[STR_PROD_KEY_LEN];
    uint8_t         product_key_len;
    char            device_key[STR_DEV_KEY_LEN];
    uint8_t         device_key_len;
    char            secret[STR_SEC_LEN];
    uint8_t         secret_len;
    char            product_secret[STR_PROD_SEC_LEN];
    uint8_t         product_secret_len;
    char            version[STR_VER_LEN];
    bool            enable_ota;
    bool            enable_auth;
    ali_auth_type_t auth_type;
    dev_status_changed_cb status_changed_cb;
    set_dev_status_cb     set_cb;
    get_dev_status_cb     get_cb;
    apinfo_ready_cb       apinfo_cb;
};

/**
 * @brief Start breeze SDK services.
 *
 * @param[in] dev_info @n Device information
 * @return result 0:success  -1 failed.
 * @see None.
 * @note This API is called by user to initialize and start breeze services.
 */
int breeze_start(struct device_config *dev_conf);

/**
 * @brief Stop breeze services.
 * @return result 0:success  -1 failed.
 * @see None.
 * @note This API is called by user to stop the breeze services.
 */
int breeze_end(void);

/**
 * @brief Post device status.
 *
 * @param[in] buffer @n Data to post.
 * @param[in] model @n Length of the data.
 * @return None.
 * @see None.
 * @note This API can be used to update date to server, in non-blocked way.
 *       This API uses ble indicate way to send the data.
 */
void breeze_post(uint8_t *buffer, uint32_t length);


/**
 * @brief Post device status, in a fast way.
 *
 * @param[in] buffer @n Data to post.
 * @param[in] model @n Length of the data.
 * @return result 0:success  -1 failed.
 * @see None.
 * @note This API is similiar with breeze_post. The difference is that
 *       ble notify way is used to post the data.
 */
void breeze_post_fast(uint8_t *buffer, uint32_t length);


/**
 * @brief Start event dispatcher. Note that:
 *        1. This API blocks (never return).
 *        2. This API should be called after breeze_start called.
 */
void breeze_event_dispatcher();


/**
 * @brief Append user specific data to the tail of the breeze adv data.
 *
 * @param[in] Data to append.
 * @param[in] Data length.
 * @return None.
 * @see None.
 * @note User can call this API if additional adv data is needed.
 *       Breeze SDK has its own adv data and format, find more details
 *       in Breeze spec.
 */
void breeze_append_adv_data(uint8_t *data, uint32_t len);


/**
 * @brief Restart BLE advertisement. This API will stop and then start the adv.
 *
 * @param None.
 * @return None.
 * @see None.
 * @note User can call this API if he/she wants to update the adv
 *       content from time to time.
 */
void breeze_restart_advertising();

/**
 * @brief Start breeze awss process.
 *
 * @param[in] cb    The callback to be called by breeze SDK when AP info ready.
 * @param[in] info  The device information required by breeze SDK.
 * @return None.
 * @see None.
 * @note When this API is called, do not call breeze_start anymore.
 */
void breeze_awss_start(apinfo_ready_cb cb, breeze_dev_info_t *info);

#if defined(__cplusplus) /* If this is a C++ compiler, use C linkage */
}
#endif

#endif // BREEZE_API_EXPORT_H
