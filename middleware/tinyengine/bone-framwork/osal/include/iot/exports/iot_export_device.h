/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

/* From device.h */
#define IOTX_PRODUCT_KEY_LEN     (11)   /* IoTx product key length  */
#define IOTX_DEVICE_NAME_LEN     (20)   /* IoTx device name length */
#define IOTX_DEVICE_ID_LEN       (64)   /* IoTx device ID length */
#define IOTX_DEVICE_SECRET_LEN   (32)   /* IoTx device secret length */
#define IOTX_URI_MAX_LEN         (135)  /* IoTx CoAP uri maximal length */

#define MODULE_VENDOR_ID    (32) // Partner ID

#define HOST_ADDRESS_LEN    (128)
#define HOST_PORT_LEN       (8)
#define CLIENT_ID_LEN       (256)
#define USER_NAME_LEN       (128)
#define PASSWORD_LEN        (128)
#define AESKEY_STR_LEN      (32)
#define AESKEY_HEX_LEN      (128/8)

typedef struct {
    char        product_key[IOTX_PRODUCT_KEY_LEN + 1];
    char        device_name[IOTX_DEVICE_NAME_LEN + 1];
    char        device_id[IOTX_DEVICE_ID_LEN + 1];
    char        device_secret[IOTX_DEVICE_SECRET_LEN + 1];
    char        module_vendor_id[MODULE_VENDOR_ID + 1];
} iotx_device_info_t, *iotx_device_info_pt;

typedef struct {
    uint16_t        port;
    char            host_name[HOST_ADDRESS_LEN + 1];
    char            client_id[CLIENT_ID_LEN + 1];
    char            username[USER_NAME_LEN + 1];
    char            password[PASSWORD_LEN + 1];
    const char     *pub_key;
#ifdef MQTT_ID2_AUTH
    char            aeskey_str[AESKEY_STR_LEN];
    uint8_t         aeskey_hex[AESKEY_HEX_LEN];
#endif
} iotx_conn_info_t, *iotx_conn_info_pt;
/* From device.h */
