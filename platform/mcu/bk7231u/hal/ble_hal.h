#ifndef _AIS_BLE_PORT_H_
#define _AIS_BLE_PORT_H_

/* UUIDs */
#define BLE_UUID_AIS_SERVICE 0xFEB3 /* The UUID of the Alibaba IOT Service. */
#define BLE_UUID_AIS_RC \
    0xFED4 /* The UUID of the "Read Characteristics" Characteristic. */
#define BLE_UUID_AIS_WC \
    0xFED5 /* The UUID of the "Write Characteristics" Characteristic. */
#define BLE_UUID_AIS_IC \
    0xFED6 /* The UUID of the "Indicate Characteristics" Characteristic. */
#define BLE_UUID_AIS_WWNRC                                      \
    0xFED7 /* The UUID of the "Write WithNoRsp Characteristics" \
              Characteristic. */
#define BLE_UUID_AIS_NC \
    0xFED8 /* The UUID of the "Notify Characteristics" Characteristic. */

#define AIS_BT_MAC_LEN 6

typedef enum
{
    AIS_ERR_SUCCESS = 0,
    AIS_ERR_STACK_FAIL,
    AIS_ERR_MEM_FAIL,
    AIS_ERR_INVALID_ADV_DATA,
    AIS_ERR_ADV_FAIL,
    AIS_ERR_STOP_ADV_FAIL,
    AIS_ERR_GATT_INDICATE_FAIL,
    AIS_ERR_GATT_NOTIFY_FAIL,
    /* Add more AIS error code hereafter */
} ais_err_t;

typedef enum
{
    AIS_ADV_NAME_SHORT,
    AIS_ADV_NAME_FULL
} ais_adv_name_type_t;

typedef struct
{
    ais_adv_name_type_t ntype;
    char *              name;
} ais_adv_name_t;

typedef enum
{
    AIS_AD_LIMITED  = ABIT(0), /* Limited Discoverable */
    AIS_AD_GENERAL  = ABIT(1), /* General Discoverable */
    AIS_AD_NO_BREDR = ABIT(2)  /* BR/EDR not supported */
} ais_adv_flag_t;

#define MAX_VENDOR_DATA_LEN 24
typedef struct
{
    uint8_t  data[MAX_VENDOR_DATA_LEN];
    uint16_t len;
} ais_adv_vendor_data_t;

typedef struct
{
    ais_adv_flag_t        flag;
    ais_adv_name_t        name;
    ais_adv_vendor_data_t vdata;
    /* Subject to add more hereafter in the future */
} ais_adv_init_t;

/**
 * API to initialize ble stack.
 * @parma[in]  ais_init  Bluetooth stack init parmaters.
 * @return     0 on success, error code if failure.
 */
ais_err_t ble_stack_init(ais_bt_init_t *ais_init);

/**
 * API to de-initialize ble stack.
 * @return     0 on success, error code if failure.
 */
ais_err_t ble_stack_deinit();

/**
 * API to send data via AIS's Notify Characteristics.
 * @parma[in]  p_data  data buffer.
 * @parma[in]  length  data length.
 * @return     0 on success, error code if failure.
 */
ais_err_t ble_send_notification(uint8_t *p_data, uint16_t length);

/**
 * API to send data via AIS's Indicate Characteristics.
 * @parma[in]  p_data  data buffer.
 * @parma[in]  length  data length.
 * @return     0 on success, erro code if failure.
 */
ais_err_t ble_send_indication(uint8_t *p_data, uint16_t length, void (*txdone)(uint8_t res));

/**
 * API to disconnect BLE connection.
 * @param[in]  reason  the reason to disconnect the connection.
 */
void ble_disconnect(uint8_t reason);

/**
 * API to start bluetooth advertising.
 * @return     0 on success, erro code if failure.
 */
ais_err_t ble_advertising_start(ais_adv_init_t *adv);

/**
 * API to stop bluetooth advertising.
 * @return     0 on success, erro code if failure.
 */
ais_err_t ble_advertising_stop();

/**
 * API to start bluetooth advertising.
 * @parma[out]  mac  the uint8_t[BD_ADDR_LEN] space the save the mac address.
 * @return     0 on success, erro code if failure.
 */
ais_err_t ble_get_mac(uint8_t *mac);

#endif
