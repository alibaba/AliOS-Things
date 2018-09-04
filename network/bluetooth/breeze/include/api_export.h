/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
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
        char    ssid[332 + 1];
        char    pw[64 + 1];
        uint8_t bssid[6];
    } breeze_apinfo_t;

    /**
     * @brief Callback while Device status change
     *
     * @param[in] status @n Device Status.
     * @return None.
     * @see None.
     * @note None.
     */

    //客户需要实现此函数，SDK回调此函数。状态通知：当DEVICE发生状态变化，如蓝牙连接事件，如果客户有LED等状态的更新就在此函数中实现
    typedef void (*dev_status_changed_cb)(breeze_event_t event);

    /**
     * @brief Callback while Setting Device status
     *
     * @param[in] buffer @n The Data of Setting Device status.
     * @param[in] model @n Length of Data.
     * @return None.
     * @see None.
     * @note None.
     */
    //客户需要实现此函数。当APP有控制指令发给客户端时，(SDK)会调用此函数
    typedef void (*set_dev_status_cb)(uint8_t *buffer, uint32_t length);

    /**
     * @brief Callback while Getting Device status
     *
     * @param[out] buffer @n The Data of  Device status.
     * @param[out] model @n Length of Data.
     * @return None.
     * @see None.
     * @note None.
     */
    //客户需要实现此函数。当APP需要获取客户端状态时，(SDK)会调用此函数
    typedef void (*get_dev_status_cb)(uint8_t *buffer, uint32_t length);

    typedef void (*apinfo_ready_cb)(breeze_apinfo_t *ap);

    struct device_config
    {
        uint8_t         bd_addr[BD_ADDR_LEN]; /**< Bluetooth Device Address. */
        char            model[STR_MODEL_LEN]; /**< reserved. */
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
     * @brief Start Alink Service
     *
     * @param[in] dev_info @n Device information
     * @return result 0:success  -1 failed.
     * @see None.
     * @note None.
     */
    // 1.启动SDK的服务，由客户代码调用
    int breeze_start(struct device_config *dev_conf);

    /**
     * @brief End Alink Service
     * @return result 0:success  -1 failed.
     * @see None.
     * @note None.
     */
    // 2.停止SDK的服务，由客户代码调用
    int breeze_end(void);

    /**
     * @brief Post Device status
     *
     * @param[in] buffer @n Data needing to post.
     * @param[in] model @n Length of Data.
     * @return None.
     * @see None.
     * @note None.
     */
    // 3.一旦客户有数据更新，需要通过此函数发送给服务器，Callback
    // 函数中通知调用结果，蓝牙采用indicate 的方式post数据，非阻塞
    void breeze_post(uint8_t *buffer, uint32_t length);


    /**
     * @brief Post Device status
     *
     * @param[in] buffer @n Data needing to post.
     * @param[in] model @n Length of Data.
     * @return result 0:success  -1 failed.
     * @see None.
     * @note None.
     */
    // 4.一旦客户有数据更新，需要通过此函数发送给服务器,callback
    // 函数中通知调用结果，蓝牙采用notify 的方式post数据，非阻塞
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
#if defined(__cplusplus) /* If this is a C++ compiler, use C linkage */
}
#endif

#endif // BREEZE_API_EXPORT_H
