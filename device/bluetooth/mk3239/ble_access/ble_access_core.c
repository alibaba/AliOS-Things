#include <string.h>
#include <stdio.h>

#include "mico.h"
#include "mico_bt.h"
#include "mico_bt_cfg.h"
#include "mico_bt_smart_attribute.h"
#include "mico_bt_smart_interface.h"
#include "mico_bt_smartbridge.h"
#include "mico_bt_smartbridge_gatt.h"
#include "sdpdefs.h"
#include "gattdefs.h"

#include "StringUtils.h"
#include "LinkListUtils.h"

#include "ble_access_core.h"
#include "ble_access_core_i.h"

/*
 *-------------------------------------------------------------------------------------------------
 *
 *  Configurations & Constants
 *
 *-------------------------------------------------------------------------------------------------
 */

#define MAX_ATTRIBUTE_CACHE_COUNTS                  10
#define OUT_OF_BAND_AUTHENTICATION                  BT_SMART_OOB_AUTH_NONE
#define AUTHENTICATION_REQUIREMENTS                 BT_SMART_AUTH_REQ_BONDING

#define BLE_ACCESS_REMOTE_SERVS_NUM                 5
#define BLE_ACCESS_MAX_CHAR_VALUE_LENGTH            20
#define BLE_ACCESS_MAX_ATTR_VALUE_LENGTH            64

#define BLE_ACCESS_SLAVE_SLEEP_TIMEOUT              (10 * 60) // seconds

/* APP State */
#define BLE_ACCESS_APP_STATE_INIT                   0x01
#define BLE_ACCESS_APP_STATE_AUTO_CONN              0x02
#define BLE_ACCESS_APP_STATE_ADD                    0x03

#define BLE_ACCESS_AUTH_STATE_UNKNOWN               0x00
#define BLE_ACCESS_AUTH_STATE_START                 0x04
#define BLE_ACCESS_AUTH_STATE_SUCC                  0x05
#define BLE_ACCESS_AUTH_STATE_FAIL                  0x06

/*
 *-------------------------------------------------------------------------------------------------
 *
 *  Predefine Type
 *
 *-------------------------------------------------------------------------------------------------
 */

/*
 *-------------------------------------------------------------------------------------------------
 *
 *  Local Function Prototype
 *
 *-------------------------------------------------------------------------------------------------
 */

/* Local function prototype */
static OSStatus ble_access_connection_handler       (void *arg);
static OSStatus ble_access_disconnection_handler    (mico_bt_smartbridge_socket_t *socket);
static OSStatus ble_access_notification_handler     (mico_bt_smartbridge_socket_t *socket, uint16_t attribute_handle);
static OSStatus ble_access_scan_complete_handler    (void *arg);
static OSStatus ble_access_scan_result_handler      (const mico_bt_smart_advertising_report_t *result);

static OSStatus ble_access_auto_conn_parms_handler  (const mico_bt_device_address_t device_address,
                                                     const char *name,
                                                     const uint8_t *p_adv_data,
                                                     const uint8_t length,
                                                     mico_bt_smartbridge_auto_conn_cback_parms_t *parm);

static OSStatus ble_access_auto_connection_handler  (mico_bt_smartbridge_socket_t *socket);
static OSStatus ble_access_timer_event_handle       (void *arg);

static OSStatus ble_access_send_bt_event            (uint8_t evt_code, const ble_access_evt_parms_t *parms );

static OSStatus ble_access_cache_control_service    (ble_access_device_t *dev);

static OSStatus ble_access_send_command             (const ble_access_device_t *dev, uint8_t code,
                                                     uint8_t length, uint8_t *p_data);

/*
 *-------------------------------------------------------------------------------------------------
 *
 *  Local Variables
 *
 *-------------------------------------------------------------------------------------------------
 */

/* SmartBridge security settings */
const mico_bt_smart_security_settings_t ble_access_security_settings = {
    .timeout_second              = 15,
    .io_capabilities             = BT_SMART_IO_DISPLAY_ONLY,//BT_SMART_IO_NO_INPUT_NO_OUTPUT,
    .authentication_requirements = AUTHENTICATION_REQUIREMENTS,
    .oob_authentication          = OUT_OF_BAND_AUTHENTICATION,
    .max_encryption_key_size     = 16,
    .master_key_distribution     = BT_SMART_DISTRIBUTE_ENCRYPTION_AND_SIGN_KEYS,
    .slave_key_distribution      = BT_SMART_DISTRIBUTE_ALL_KEYS,
};

/* SmartBridge connection settings */
static const mico_bt_smart_connection_settings_t ble_access_connection_settings = {
    .timeout_second                = 10,
    .filter_policy                 = FILTER_POLICY_WHITE_LIST,
    .interval_min                  = 32,//MICO_BT_CFG_DEFAULT_CONN_MIN_INTERVAL,
    .interval_max                  = 64,//MICO_BT_CFG_DEFAULT_CONN_MAX_INTERVAL,
    .latency                       = MICO_BT_CFG_DEFAULT_CONN_LATENCY,
    .supervision_timeout           = MICO_BT_CFG_DEFAULT_CONN_SUPERVISION_TIMEOUT,
    .ce_length_min                 = 0,
    .ce_length_max                 = 0,
    .attribute_protocol_timeout_ms = 10000,
};

/* Used to store service UUID */
// #define BLE_ACCESS_SERV_UUID_IDX          0
// #define BLE_ACCESS_CHAR_UUID_IDX          0

#define BLE_CONTROL_SERV_UUID_IDX        0
#define BLE_CONTROL_CMD_CHAR_UUID_IDX    0
#define BLE_CONTROL_EVT_CHAR_UUID_IDX    1

// #define BLE_BATTERY_SERV_UUID_IDX        2
// #define BLE_BATTERY_LEVEL_CHAR_UUID_IDX  3

/* All services UUID */
const mico_bt_uuid_t ble_access_cache_services_uuid[] = {
    // [BLE_ACCESS_SERV_UUID_IDX] = {
    //     .len = LEN_UUID_128,
    //     .uu.uuid128 = { UUID_MXCHIP_ALERT_NOTIFICATION_SERVICE },
    // },
    [BLE_CONTROL_SERV_UUID_IDX] = {
        .len = LEN_UUID_128,
        .uu.uuid128 = { UUID_MXCHIP_CONTROL_POINT_SERVICE },
    },
    // [BLE_BATTERY_SERV_UUID_IDX] = {
    //     .len = LEN_UUID_16,
    //     .uu.uuid16 = UUID_SERVCLASS_BATTERY,
    // },
};

/* All characteristic UUID */
const mico_bt_uuid_t ble_access_cache_chars_uuid[] = {
    // [BLE_ACCESS_CHAR_UUID_IDX] = {
    //     .len = LEN_UUID_128,
    //     .uu.uuid128 = { UUID_MXCHIP_ALERT_NOTIFICATION_CHAR },
    // },
    [BLE_CONTROL_CMD_CHAR_UUID_IDX] = {
        .len = LEN_UUID_128,
        .uu.uuid128 = { UUID_MXCHIP_CONTROL_POINT_CMD_CHAR },
    },
    [BLE_CONTROL_EVT_CHAR_UUID_IDX] = {
        .len = LEN_UUID_128,
        .uu.uuid128 = { UUID_MXCHIP_CONTROL_POINT_EVT_CHAR },
    },
    // [BLE_BATTERY_LEVEL_CHAR_UUID_IDX] = {
    //     .len = LEN_UUID_16,
    //     .uu.uuid16 = GATT_UUID_BATTERY_LEVEL,
    // },
};

/*
 *-------------------------------------------------------------------------------------------------
 *
 *  Global Function Prototype
 *
 *-------------------------------------------------------------------------------------------------
 */


/*
 *-------------------------------------------------------------------------------------------------
 *
 *  Global Function Definition
 *
 *-------------------------------------------------------------------------------------------------
 */


/*
 * For all BLE Devices Resource.
 */

static ble_access_event_callback_t  ble_access_callback;
static uint8_t                      ble_access_app_state;
static mico_bt_device_address_t     ble_access_local_address;


/*
 *-------------------------------------------------------------------------------------------------
 *
 * Function Definitions
 *
 *-------------------------------------------------------------------------------------------------
 */

OSStatus ble_access_bluetooth_init(void)
{
    OSStatus                         err = kNoErr;

    ble_access_device_t              *dev = NULL;
    mico_bt_dev_bonded_device_info_t paired_device_list[MAX_CONCURRENT_CONNECTIONS];
    uint16_t                         paired_device_num = MAX_CONCURRENT_CONNECTIONS, idx = 0;

    /* Initialize MiCO Bluetooth Framework */
    err = mico_bt_init(MICO_BT_HCI_MODE, "BLE Access Center", MAX_CONCURRENT_CONNECTIONS, 0);
    require_noerr_string(err, exit, "Initializing MiCO bluetooth Framework failed");

    /* Initialize MiCO BT Smart Bridge Framework */
    err = mico_bt_smartbridge_init(MAX_CONCURRENT_CONNECTIONS);
    require_noerr_string(err, exit, "Initializing MiCO BT Smart Bridge Framework failed");

    /* Get Local device address */
    mico_bt_device_get_address(&ble_access_local_address);

    ble_access_log("Initialize BLE Access Core Successfully");

    /* Enable Attribute Cache and set maximum number of caches */
    err = mico_bt_smartbridge_enable_attribute_cache(MAX_CONCURRENT_CONNECTIONS, NULL, 0);
    require_noerr_string(err, exit, "Enable Attribute Cache failed");

    /* Create a worker thread for making a connection */
    err = ble_access_create_worker_thread();
    require_noerr_string(err, exit, "Create worker thread failed");

    /* Create connection list */
    err = ble_access_connect_list_init();
    require_noerr_action_string(err, exit, ble_access_connect_list_deinit(),
                                "Initialize Connection list failed");

    /*
     * Create all sockets and make them ready to connect.
     * A socket can connect and disconnect multiple times.
     */
    ble_access_initialize_devices();

    /* Update BLE Controller White List */
    mico_bt_dev_get_bonded_devices(paired_device_list, &paired_device_num);
    for (idx = 0; idx < paired_device_num; idx++) {

#if defined(BLE_ACCESS_DEBUG) && BLE_ACCESS_DEBUG == 1
        ble_access_log("WL: [%02x:%02x:%02x:%02x:%02x:%02x, %lu]",
                       paired_device_list[idx].bd_addr[0], paired_device_list[idx].bd_addr[1], paired_device_list[idx].bd_addr[2],
                       paired_device_list[idx].bd_addr[3], paired_device_list[idx].bd_addr[4], paired_device_list[idx].bd_addr[5],
                       ble_access_calculate_device_id(paired_device_list[idx].bd_addr));
#endif

        dev = ble_access_get_free_device();
        require_action_string(dev != NULL, exit, err = kNoResourcesErr,
                              "No resource for new connection");
        /* Calculate Device ID */
        dev->device_id = ble_access_calculate_device_id(paired_device_list[idx].bd_addr);
        dev->auth_state = BLE_ACCESS_AUTH_STATE_SUCC;

        /* Update BLE Controller White List */
        mico_bt_ble_update_background_connection_device(MICO_TRUE, paired_device_list[idx].bd_addr);
    }

exit:
    ble_access_app_state = BLE_ACCESS_APP_STATE_INIT;
    return err;
}

OSStatus ble_access_bluetooth_start(ble_access_event_callback_t callback)
{
    OSStatus err = kNoErr;

    /* Check parameters */
    require_action_string(callback != NULL,
                          exit,
                          err = kParamErr,
                          "ble_access_bluetooth_start: parameters failed");
    /* Event handler */
    ble_access_callback = callback;

exit:
    return err;
}

OSStatus ble_access_bluetooth_stop(void)
{
    OSStatus err = kNoErr;

    /* Check parameters */
    require_action_string(ble_access_callback != NULL,
                          exit,
                          err = kParamErr,
                          "ble_access_bluetooth_stop: Parameters failed");

    /* Event handler */
    ble_access_callback = NULL;

exit:
    return err;
}

OSStatus ble_access_bluetooth_request(uint8_t request, const ble_access_cmd_parms_t *parms)
{
    OSStatus                            err = kNoErr;
    uint8_t                             timeout[4];
    uint8_t                            *p_timeout = 0;

    mico_bt_smartbridge_socket_status_t status;
    mico_bt_smart_scan_settings_t       scan_settings;
    mico_bt_smart_device_t             *device = NULL;
    mico_bt_device_address_t            device_address;

    ble_access_device_t                *dev = NULL;

#if defined(BLE_ACCESS_DEBUG) && BLE_ACCESS_DEBUG == 1
    uint8_t                             size = 0;
#endif

    ble_access_log("User Request - %s[%#x] to %s device.",
                   print_request_str(request),
                   request,
                   parms == NULL ? "all" : "specificed");

    if (request == BLE_ACCESS_REQ_DEV_ADD
        || request == BLE_ACCESS_REQ_DEV_DISC
        || request == BLE_ACCESS_REQ_DEV_REMOVE) {

        /* Check Parameters */
        if (parms == NULL) {
            ble_access_log("Invalid Parameters");
            return kParamErr;
        }

        /* Generate device address */
        err = ble_access_generate_device_address(device_address, parms->device_id);
        require_noerr_string(err, exit, "Invalid device ID");
#if defined(BLE_ACCESS_DEBUG) && BLE_ACCESS_DEBUG == 1
        char *device_address_str = NULL;
        ble_access_log("Generate device address: device_id = %lu", parms->device_id);
        device_address_str = DataToHexStringWithColons(device_address, 6);
        ble_access_log("device_address: %s", device_address_str);
        free(device_address_str);
#endif
    }

    switch (request) {
        case BLE_ACCESS_REQ_DEV_SCAN:
            /* Change state */
            require_action_string(ble_access_app_state != BLE_ACCESS_APP_STATE_AUTO_CONN,
                                  exit,
                                  err = kUnknownErr,
                                  "Don't request to scan because of auto connection procedure.");

            /* Stop scanning procedure. */
            if (!parms->p.start) {
                mico_bt_smartbridge_stop_scan();
                break;
            }

            /* Clear connection list */
            while (ble_access_connect_list_get(&device, NULL) == kNoErr) {
                ble_access_connect_list_remove(device);
            }

            /* start to scanning */
            ble_access_set_scan_cfg(&scan_settings, MICO_FALSE);
            mico_bt_smartbridge_start_scan(&scan_settings,
                                           ble_access_scan_complete_handler,
                                           ble_access_scan_result_handler);
            ble_access_app_state = BLE_ACCESS_APP_STATE_ADD;
            break;
        case BLE_ACCESS_REQ_DEV_ADD:
            require_action_string(ble_access_app_state == BLE_ACCESS_APP_STATE_ADD,
                                  exit,
                                  err = kUnknownErr,
                                  "Don't request to Add new device becuase of error state");

            /* find the device */
            err = ble_access_connect_list_get_by_address(&device, NULL, device_address);
            require_noerr_action_string(err, exit, err = kNoResourcesErr, "Cannot find the device");

            /* start to connecting by asynchronous event. */
            ble_access_send_aync_event(ble_access_connection_handler, (void *)device);
            break;
        case BLE_ACCESS_REQ_DEV_DISC:
            /* Check the socket is connected. */
            dev = ble_access_find_device_by_address(device_address);
            require_action_string(dev != NULL, exit, err = kConnectionErr, "Not a connection");

            /* Send a 'DISC_REQ' command */
            p_timeout = (uint8_t *)timeout;
            UINT32_TO_STREAM(p_timeout, parms->p.timeout);
            err = ble_access_send_command(dev, BLE_ACCESS_CODE_DISC, sizeof(uint32_t), (uint8_t *)timeout);
            if (err != kNoErr) {
                ble_access_log("Send a command failed, ret = %d", err);
                goto exit;
            }
            break;
        case BLE_ACCESS_REQ_DEV_REMOVE:
            require_action_string(ble_access_app_state != BLE_ACCESS_APP_STATE_AUTO_CONN,
                                  exit,
                                  err = kInProgressErr,
                                  "Don't requrest to remove a device because of Auto connection procedure");

            /* Check the socket is connected. */
            dev = ble_access_find_device_by_address(device_address);
            require_action_string(dev != NULL, exit, err = kConnectionErr, "Not a connection");

            /* Delete it from the White List. */
            mico_bt_smartbridge_get_socket_status(&dev->socket, &status);
            require_string(status == SMARTBRIDGE_SOCKET_CONNECTED || status == SMARTBRIDGE_SOCKET_DISCONNECTED,
                           exit,
                           "This socket is busy!!");
            if (status == SMARTBRIDGE_SOCKET_CONNECTED) {
                mico_bt_smartbridge_disconnect(&dev->socket, MICO_TRUE);
            } else {
                mico_bt_ble_update_background_connection_device(MICO_FALSE, device_address);
            }
            mico_bt_dev_delete_bonded_device(device_address);
            ble_access_release_device(MICO_TRUE, dev);
            break;
        case BLE_ACCESS_REQ_DEV_START_AUTO:
            /* Change state */
            ble_access_app_state = BLE_ACCESS_APP_STATE_AUTO_CONN;
            mico_bt_smartbridge_stop_scan();

            /* Start auto connection action. */
#if defined(BLE_ACCESS_DEBUG) && BLE_ACCESS_DEBUG == 1
            err = mico_bt_smartbridge_get_background_connection_devices_size(&size);
            require_noerr_string(err, exit, "Get white list size unsuccessfully!!");
            ble_access_log("white list size: %d", size);
#endif

            /* Set auto connection */
            ble_access_set_scan_cfg(&scan_settings, MICO_TRUE);
            err = mico_bt_smartbridge_set_auto_connection_action(TRUE, &scan_settings,
                                                                 ble_access_auto_conn_parms_handler);
            require_noerr_string(err, exit, "Start auto connection type unsuccessfully!!");
            ble_access_log("Start to establish background connection...");
            break;
        case BLE_ACCESS_REQ_DEV_STOP_AUTO:
            /* Change state */
            require_action_string(ble_access_app_state == BLE_ACCESS_APP_STATE_AUTO_CONN,
                                  exit,
                                  err = kUnknownErr,
                                  "Don't request to stop auto procedure.");
            ble_access_app_state = BLE_ACCESS_APP_STATE_INIT;

            /* Stop */
#if defined(BLE_ACCESS_DEBUG) && BLE_ACCESS_DEBUG == 1
            err = mico_bt_smartbridge_get_background_connection_devices_size(&size);
            require_noerr_string(err, exit, "Get white list size unsuccessfully!!");
            ble_access_log("white list size: %d\n", size);
#endif

            err = mico_bt_smartbridge_set_auto_connection_action(FALSE, NULL, NULL);
            require_noerr_string(err, exit, "Stop auto connection type unsuccessfully!!");
            ble_access_log("Stop to establish background connection...");
            break;
        default:
            ble_access_log("Unsupported Request!");
            break;
    }

exit:
    return err;
}

static void ble_access_attribute_copy_from_smart(const mico_bt_smart_attribute_t *from, ble_access_attribute_t *to)
{
    if (from != NULL && to != NULL) {
        to->handle = from->handle;
        memcpy(&to->type, &from->type, sizeof(mico_bt_uuid_t));
        to->permission = from->permission;
        to->value_length = from->value_length;
        to->value_struct_size = from->value_struct_size;
        memcpy(to->value.value, from->value.value, sizeof(to->value));
    }
}

/* Get an attribute of Characteristic with the UUID provided from the local attribute database. */
OSStatus ble_access_get_characteritic_by_uuid(uint32_t dev_id,
                                              const ble_access_serv_t *serv,
                                              const ble_access_uuid_t *uuid,
                                              ble_access_attribute_t *attr)
{
    uint8_t                     buf[ATTR_CHARACTERISTIC_VALUE_SIZE(BLE_ACCESS_MAX_ATTR_VALUE_LENGTH)] = {0};
    mico_bt_smart_attribute_t  *attribute = (mico_bt_smart_attribute_t *)buf;

    mico_bt_device_address_t    device_address;
    ble_access_device_t        *dev = NULL;
    OSStatus                    err = kNoErr;

    require_action_string(dev_id != 0 && serv != NULL && attr != NULL,
                          exit,
                          err = kParamErr,
                          "Invalid parameters!");

    /* Generate device address */
    err = ble_access_generate_device_address(device_address, dev_id);
    require_noerr_string(err, exit, "Invalid device ID");

    /* Check the socket is connected. */
    dev = ble_access_find_device_by_address(device_address);
    require_action_string(dev != NULL, exit, err = kConnectionErr, "Not a connection");

    attribute->value_struct_size = BLE_ACCESS_MAX_ATTR_VALUE_LENGTH;
    err = mico_bt_smartbridge_get_characteritics_from_attribute_cache_by_uuid(
              &dev->socket,
              (mico_bt_uuid_t *)uuid,
              serv->start_handle,
              serv->end_handle,
              (mico_bt_smart_attribute_t *)attribute,
              ATTR_CHARACTERISTIC_VALUE_SIZE(BLE_ACCESS_MAX_ATTR_VALUE_LENGTH));
    if (err == kNoErr) {
        ble_access_attribute_copy_from_smart(attribute, attr);
    }
exit:
    return err;
}

/* Handle aynchrous 'GET All Characteristics' event */
static OSStatus ble_access_characteristic_aync_event(void *arg)
{
    uint8_t                     *p = (uint8_t *)arg;
    ble_access_device_t         *dev = NULL;
    uint32_t                     dev_id;
    ble_access_serv_t            serv;
    OSStatus                     err = kNoErr;
    mico_bt_device_address_t     device_address;

    uint8_t                      buf[ATTR_CHARACTERISTIC_VALUE_SIZE(BLE_ACCESS_MAX_ATTR_VALUE_LENGTH)] = {0};
    mico_bt_smart_attribute_t   *attribute = (mico_bt_smart_attribute_t *)buf;
    ble_access_attribute_t       attr;

    uint16_t                     start_handle, end_handle;

    mico_bt_uuid_t               characteristic_uuid = {
        .len = LEN_UUID_16,
        .uu.uuid16 = GATT_UUID_CHAR_DECLARE
    };

    ble_access_evt_parms_t       parms;

    /* un-package the parameters */
    memcpy(&dev_id, p, sizeof(uint32_t));
    memcpy(&serv, p + sizeof(uint32_t), sizeof(ble_access_serv_t));
    free(p);

    /* Generate device address */
    err = ble_access_generate_device_address(device_address, dev_id);
    require_noerr_action_string(err, exit, parms.status = BLE_ACCESS_STATUS_INVALID_PARMS, "Invalid device ID");

    dev = ble_access_find_device_by_address(device_address);
    if (!dev) {
        err = kConnectionErr;
        parms.status = BLE_ACCESS_STATUS_NO_CONNECTION;
        ble_access_log("Not a connection");
        goto exit;
    }

    start_handle = serv.start_handle;
    end_handle = serv.end_handle;

    parms.device_id = dev_id;

    /* Find service */
    attribute->value_struct_size = BLE_ACCESS_MAX_ATTR_VALUE_LENGTH;
    while ((err = mico_bt_smartbridge_get_attribute_cache_by_uuid(
                      &dev->socket,
                      &characteristic_uuid,
                      start_handle,
                      end_handle,
                      attribute,
                      ATTR_CHARACTERISTIC_VALUE_SIZE(BLE_ACCESS_MAX_ATTR_VALUE_LENGTH)))
           == MICO_BT_SUCCESS) {

        /* Notify */
        ble_access_attribute_copy_from_smart(attribute, &attr);
        parms.p.gatt.attr = &attr;
        ble_access_send_bt_event(BLE_ACCESS_EVENT_GATT_CHAR, &parms);
        start_handle = attribute->handle + 1;
    }
    parms.status = BLE_ACCESS_STATUS_SUCCESS;

exit:
    /* Finish */
    ble_access_send_bt_event(BLE_ACCESS_EVENT_GATT_CHAR_CMPL, &parms);
    return err;
}

/* Find all characteristics for 'serv' from the local attribute database */
OSStatus ble_access_get_characteristics(uint32_t dev_id, const ble_access_serv_t *serv)
{
    uint8_t                     *p = NULL;
    OSStatus                    err = kNoErr;

    require_action_string(dev_id != 0 && serv != NULL,
                          exit,
                          err = kParamErr,
                          "Invalid parameters!");

    /* Package parameters. */
    p = (uint8_t *)malloc(sizeof(ble_access_serv_t) + sizeof(uint32_t));

    if (!p) {
        ble_access_log("Malloc failed");
        return kUnknownErr;
    }
    memcpy(p, &dev_id, sizeof(uint32_t));
    memcpy(p + sizeof(uint32_t), serv, sizeof(ble_access_serv_t));
    err = ble_access_send_aync_event(ble_access_characteristic_aync_event, p);
exit:
    if (err != kNoErr && p) {
        free(p);
    }
    return err;
}

/* Find and read attribute with the Handle provided from the Attribute Cache */
OSStatus ble_access_get_attribute_by_handle(uint32_t dev_id,
                                            uint16_t handle,
                                            ble_access_attribute_t *attr)
{
    uint8_t                     buf[ATTR_CHARACTERISTIC_VALUE_SIZE(BLE_ACCESS_MAX_ATTR_VALUE_LENGTH)] = {0};
    mico_bt_smart_attribute_t  *attribute = (mico_bt_smart_attribute_t *)buf;

    mico_bt_device_address_t    device_address;
    ble_access_device_t        *dev = NULL;
    OSStatus                    err = kNoErr;

    require_action_string(dev_id != 0 && handle != 0 && attr != NULL,
                          exit,
                          err = kParamErr,
                          "Invalid parameters!");

    /* Generate device address */
    err = ble_access_generate_device_address(device_address, dev_id);
    require_noerr_string(err, exit, "Invalid device ID");

    /* Check the socket is connected. */
    dev = ble_access_find_device_by_address(device_address);
    require_action_string(dev != NULL, exit, err = kConnectionErr, "Not a connection");

    /* refresh the attribute and read it. */
    err = mico_bt_smartbridge_refresh_attribute_cache_characteristic_value(&dev->socket, handle);
    require_noerr_string(err, exit, "Refresh attribute cache failed");

    attribute->value_struct_size = BLE_ACCESS_MAX_ATTR_VALUE_LENGTH;
    err = mico_bt_smartbridge_get_attribute_cache_by_handle(
              &dev->socket,
              handle,
              (mico_bt_smart_attribute_t *)attribute,
              ATTR_CHARACTERISTIC_VALUE_SIZE(BLE_ACCESS_MAX_ATTR_VALUE_LENGTH));

    require_noerr_string(err, exit, "Get attribute cache failed");
    ble_access_attribute_copy_from_smart(attribute, attr);

exit:
    return err;
}

/* Update Characteristic Value in the Attribute To the server */
OSStatus ble_access_update_characteristic_value(uint32_t dev_id, uint16_t handle, uint8_t length, uint8_t *data)
{
    uint8_t                      buf[ATTR_CHARACTERISTIC_VALUE_SIZE(BLE_ACCESS_MAX_CHAR_VALUE_LENGTH)] = {0};
    mico_bt_smart_attribute_t   *char_value = NULL;
    mico_bt_device_address_t     device_address;
    ble_access_device_t         *dev = NULL;
    OSStatus                     err = kNoErr;

    require_action_string(dev_id != 0 && handle != 0
                          && length > 0 && length <= BLE_ACCESS_DEVICE_NAME_MAX_LEN
                          && data != NULL,
                          exit,
                          err = kParamErr,
                          "Invalid parameters!");

    /* Generate device address */
    err = ble_access_generate_device_address(device_address, dev_id);
    require_noerr_string(err, exit, "Invalid device ID");


    /* Check the socket is connected. */
    dev = ble_access_find_device_by_address(device_address);
    require_action_string(dev != NULL, exit, err = kConnectionErr, "Not a connection");

    char_value = (mico_bt_smart_attribute_t *)buf;
    char_value->value_struct_size = length;
    err = mico_bt_smartbridge_get_attribute_cache_by_handle(&dev->socket,
                                                            handle,
                                                            char_value,
                                                            ATTR_CHARACTERISTIC_VALUE_SIZE(length));
    require_noerr_string(err, exit, "Get Attribute failed");

    char_value->value_length = length;
    memcpy(&char_value->value.value[0], data, length);

    err = mico_bt_smartbridge_write_attribute_cache_characteristic_value(&dev->socket,
                                                                         (mico_bt_smart_attribute_t *)char_value);
exit:
    return err;
}

/* Enable Characteristic Client Configuration Indication or Notification */
OSStatus ble_access_enable_notification(uint32_t dev_id, const ble_access_attribute_t *attr, mico_bool_t notify)
{
    mico_bt_device_address_t    device_address;
    ble_access_device_t         *dev = NULL;
    OSStatus                    err = kNoErr;

    /* Generate device address */
    ble_access_log("Generate device address: device_id = %lu", dev_id);
    ble_access_generate_device_address(device_address, dev_id);

    /* Check the socket is connected. */
    dev = ble_access_find_device_by_address(device_address);
    require_action_string(dev != NULL, exit, err = kConnectionErr, "Not a connection");

    /* Enable Attribute Cache notification */
    err = mico_bt_smartbridge_enable_attribute_cache_notification(&dev->socket, notify);
    require_noerr_string(err, exit, "Enable Attribute notification failed");

exit:
    return err;
}

/* Scan complete handler. Scan complete event reported via this callback.
 * It runs on the MICO_BT_EVT_WORKER_THREAD context.
 */
static OSStatus ble_access_scan_complete_handler(void *arg)
{
    UNUSED_PARAMETER(arg);
    /* Scan complete, start a new scan. Do not use a infinit scan, it may store every result in RAM. */
    ble_access_log("scanning stop");

    ble_access_send_bt_event(BLE_ACCESS_EVENT_DEV_NEW_CMPL, NULL);

    return kNoErr;
}

/* Handler of Scanning result */
static OSStatus ble_access_scan_result_handler(const mico_bt_smart_advertising_report_t *scan_result)
{
    OSStatus                        err = kNoErr;
    mico_bool_t                     is_reported = MICO_FALSE;

    mico_bt_smart_device_t          *device = NULL;
    ble_access_manufactor_data_t    manufactor_data;

    ble_access_evt_parms_t          parms;

#if defined(BLE_ACCESS_DEBUG) && BLE_ACCESS_DEBUG == 1
    char *device_address_str = DataToHexStringWithColons(scan_result->remote_device.address, 6);
    ble_access_log("A device is scanned: %s[%s]",
                   scan_result->remote_device.name != NULL ? scan_result->remote_device.name : "Unknown Device",
                   device_address_str);
    free(device_address_str);
#endif

    /* Update device name and Notify user layer. */
    if (scan_result->event == BT_SMART_SCAN_RESPONSE_EVENT) {
        if (kNoErr == ble_access_connect_list_get_by_address(&device,
                                                             &is_reported,
                                                             scan_result->remote_device.address)) {
            if (!is_reported) {
                /* Update reported */
                ble_access_connect_list_set_report(device, MICO_TRUE);

                /* Update device name */
                if (strlen(scan_result->remote_device.name) > 0) {
                    parms.p.scan.name = (char *)scan_result->remote_device.name;
                } else {
                    parms.p.scan.name = "Unknown Device";
                }
                /* Notify user (device name, device ID, device RSSI) */
                parms.device_id = ble_access_calculate_device_id(scan_result->remote_device.address);
                parms.p.scan.RSSI = scan_result->signal_strength;
                ble_access_send_bt_event(BLE_ACCESS_EVENT_DEV_NEW, &parms);
            }

        } else {
            err = kUnknownErr;
        }
        goto exit;
    } else {
        if (ble_access_connect_list_find_by_address(scan_result->remote_device.address) == kNoErr) {
            goto exit;
        }
    }

    if (!ble_access_calculate_device_id(scan_result->remote_device.address)) {
        err = kUnknownErr;
        goto exit;
    }

    /* filter a device by device advertisement type and add it to the list. */
    err = ble_access_check_adv_type((uint8_t *)scan_result->eir_data,
                                    scan_result->eir_data_length,
                                    BLE_ACCESS_ADV_TYPE_INIT,
                                    &manufactor_data);
    if (err != kNoErr) {
        if (err != kParamErr) {
            ble_access_log("Advertising data failed");
        }
        goto exit;
    }

    /* Add device to the list */
    require_noerr_string(ble_access_connect_list_add(&scan_result->remote_device, MICO_FALSE),
                         exit,
                         "The device is existed!");

#if defined(BLE_ACCESS_DEBUG) && BLE_ACCESS_DEBUG == 1
    ble_access_log("ADV Data: ");
    ble_access_log("RSSI        - %d", scan_result->signal_strength);
    ble_access_log("ADV   type  - %d", manufactor_data.adv_type);
    ble_access_log("alert state - %d", manufactor_data.alert_state);
    device_address_str = DataToHexStringWithColons(manufactor_data.direct_addr, 6);
    ble_access_log("addr        - %s", device_address_str);
    ble_access_log("\n\n");
    free(device_address_str);
#endif

exit:
    return err;
}

/*
 * Connect handler. SmartBridge connect is executed in this callback.
 * It runs on the connect_worker_thread context
 */
static OSStatus ble_access_connection_handler(void *arg)
{
    OSStatus                err = kNoErr;

    ble_access_evt_parms_t  parms;
    mico_bt_smart_device_t *remote_device = (mico_bt_smart_device_t *)arg;
    ble_access_device_t    *dev = NULL;

    /* Iterate all sockets and look for the first available socket */
    dev = ble_access_find_device_by_address(remote_device->address);
    if (dev) {
        mico_bt_smartbridge_socket_status_t status;
        mico_bt_smartbridge_get_socket_status(&dev->socket, &status);
        if (status != SMARTBRIDGE_SOCKET_DISCONNECTED) {
            ble_access_log("This device is busy!!!");
            return kInProgressErr;
        }
    } else {
        ble_access_log("Not a bonded device, allocate!!");
        dev = ble_access_get_free_device();
        require_action_string(dev != 0, exit, err = kNoResourcesErr, "No resource for a new connection");
    }

    /* Log out */
#if defined(BLE_ACCESS_DEBUG) && BLE_ACCESS_DEBUG == 1
    char *bt_addr_str = NULL;
    bt_addr_str = DataToHexStringWithColons((uint8_t *)remote_device->address, 6);
    ble_access_log("Opening GATT Connection to [%s] (addr type =%s)...",
                   bt_addr_str,
                   (remote_device->address_type == BT_SMART_ADDR_TYPE_PUBLIC) ? "Public" : "Random");
    free(bt_addr_str);
#endif

    /* If there is a previously sotred device, then connect to it */
    if (ble_access_security_settings.authentication_requirements != BT_SMART_AUTH_REQ_NONE) {
        if (mico_bt_dev_find_bonded_device((uint8_t *)remote_device->address) == MICO_FALSE) {
            ble_access_log("Bond info isn't found, Initiate pairing request.");
            mico_bt_smartbridge_enable_pairing(&dev->socket, &ble_access_security_settings, NULL);
        } else {
            ble_access_log("Bond info is found. Encrypt use bond info");
            mico_bt_smartbridge_set_bond_info(&dev->socket, &ble_access_security_settings, NULL);
        }
    }

    parms.device_id = ble_access_calculate_device_id(remote_device->address);

    /* connect */
    err = mico_bt_smartbridge_connect(&dev->socket, remote_device, &ble_access_connection_settings,
                                      ble_access_disconnection_handler,
                                      ble_access_notification_handler);
    require_noerr_string(err, exit, "The Peer Device connect failed");

    /* Cache all services */
    err = ble_access_cache_control_service(dev);
    require_noerr_action_string(err,
                                exit,
                                mico_bt_smartbridge_disconnect(&dev->socket, TRUE),
                                "Cache Control Point service failed");

    /* Start a timer to wait for device authentication procedure */
    err = ble_access_start_timer(dev, ble_access_timer_event_handle, (void *)dev);
    require_noerr_action(err, exit, mico_bt_smartbridge_disconnect(&dev->socket, TRUE));
    dev->auth_state = BLE_ACCESS_AUTH_STATE_START;
    ble_access_log("Start a timer to wait device authentication.");

    /* Enable Attribute Cache notification */
    err = mico_bt_smartbridge_enable_attribute_cache_notification(&dev->socket, MICO_FALSE);
    require_noerr_action_string(err, exit,
                                mico_bt_smartbridge_disconnect(&dev->socket, TRUE),
                                "Enable Attribute notification failed");

exit:
    if (err == kNoErr) {
        dev->device_id = parms.device_id;
        ble_access_connect_list_remove(remote_device);
    } else {
        ble_access_release_device(MICO_TRUE, dev);
        parms.p.state = BLE_ACCESS_STATUS_ADD_FAILED;
        ble_access_send_bt_event(BLE_ACCESS_EVENT_DEV_ADD, &parms);
    }
    return err;
}

/* Disconnection handler. Disconnection by remote device is reported via this callback.
 * It runs on the MICO_BT_EVT_WORKER_THREAD context.
 */
static OSStatus ble_access_disconnection_handler(mico_bt_smartbridge_socket_t *socket)
{
    ble_access_evt_parms_t parms;

    /* Insert list and allow to add */
    ble_access_connect_list_add(&socket->remote_device, MICO_FALSE);
    ble_access_release_device(MICO_FALSE,
                              ble_access_find_device_by_address(socket->remote_device.address));

    /* Send event */
    parms.device_id = ble_access_calculate_device_id(socket->remote_device.address);
    parms.status = BLE_ACCESS_STATUS_SLAVE_REQ_DISC;
    ble_access_send_bt_event(BLE_ACCESS_EVENT_DEV_DISC, &parms);

    return kNoErr;
}

static mico_bool_t ble_access_compare_uuid(const mico_bt_uuid_t *uuid1, const mico_bt_uuid_t *uuid2)
{
    if ((uuid1->len == uuid2->len) && (memcmp(uuid1, uuid2, sizeof(mico_bt_uuid_t)) == 0)) {
        return MICO_TRUE;
    } else {
        return MICO_FALSE;
    }
}

static OSStatus ble_access_cache_all_services(mico_bt_smartbridge_socket_t *socket, ble_access_evt_parms_t *parms,
                                              uint8_t max_servs)
{
    uint8_t                      buf[ATTR_CHARACTERISTIC_VALUE_SIZE(BLE_ACCESS_MAX_ATTR_VALUE_LENGTH)] = {0};
    mico_bt_smart_attribute_t   *attribute = (mico_bt_smart_attribute_t *)buf;
    ble_access_serv_t           *servs = parms->p.add.serv;

    uint16_t        start_handle = 0;
    uint16_t        end_handle = 0xff;

    mico_bt_uuid_t  service_uuid    = { .len = LEN_UUID_16, .uu.uuid16 = GATT_UUID_PRI_SERVICE };
    mico_bt_uuid_t  gap_uuid        = { .len = LEN_UUID_16, .uu.uuid16 = 0x1800 };
    mico_bt_uuid_t  gatt_uuid       = { .len = LEN_UUID_16, .uu.uuid16 = 0x1801 };

    /* Find service */
    attribute->value_struct_size = BLE_ACCESS_MAX_ATTR_VALUE_LENGTH;
    while (mico_bt_smartbridge_get_attribute_cache_by_uuid(
               socket,
               &service_uuid,
               start_handle,
               end_handle,
               attribute,
               ATTR_CHARACTERISTIC_VALUE_SIZE(BLE_ACCESS_MAX_ATTR_VALUE_LENGTH))
           == MICO_BT_SUCCESS) {

        /* Not GAP, GATT or MXCHIP */
        if (!ble_access_compare_uuid(&attribute->value.service.uuid, &gap_uuid)
            && !ble_access_compare_uuid(&attribute->value.service.uuid, &gatt_uuid)
            && !ble_access_compare_uuid(&attribute->value.service.uuid,
                                        &ble_access_cache_services_uuid[BLE_CONTROL_SERV_UUID_IDX])) {

            if (parms->p.add.serv_count >= max_servs) {
                return kNoResourcesErr;
            }
            parms->p.add.serv_count++;
            memcpy(servs++, &attribute->value.service, sizeof(ble_access_serv_t));
        }

        /* Fixed a bug for a service without characteristics. */
        if (attribute->value.service.end_handle > attribute->value.service.start_handle) {
            start_handle = attribute->value.service.end_handle;
        } else {
            start_handle = attribute->handle + 1;
        }
    }
    return kNoErr;
}

/* Notification handler. GATT notification by remote device is reported via this callback.
 * It runs on the MICO_BT_EVT_WORKER_THREAD context.
 */
static OSStatus ble_access_notification_handler(mico_bt_smartbridge_socket_t *socket, uint16_t attribute_handle)
{
    /* GATT value notification event. attribute_handle is the handle
     * which value of the attribute is updated by the remote device.
     */
    OSStatus                     err = kNoErr;

    uint8_t                      buf[ATTR_CHARACTERISTIC_VALUE_SIZE(BLE_ACCESS_MAX_CHAR_VALUE_LENGTH)] = {0};
    mico_bt_smart_attribute_t   *characteristic_value = NULL;
    ble_access_device_t         *dev = NULL;
    ble_access_attribute_t       attr;

    ble_access_serv_t            servs[BLE_ACCESS_REMOTE_SERVS_NUM];
    ble_access_evt_parms_t       parms;

    /* Read cached data */
    characteristic_value = (mico_bt_smart_attribute_t *)buf;
    characteristic_value->value_struct_size = BLE_ACCESS_MAX_CHAR_VALUE_LENGTH;

    err = mico_bt_smartbridge_refresh_attribute_cache_characteristic_value(socket, attribute_handle);
    require_noerr_string(err, exit, "Refresh an Attribute failed");
    err = mico_bt_smartbridge_get_attribute_cache_by_handle(
              socket,
              attribute_handle,
              characteristic_value,
              ATTR_CHARACTERISTIC_VALUE_SIZE(BLE_ACCESS_MAX_CHAR_VALUE_LENGTH));
    require_noerr_string(err, exit, "This is not a cached value handle, ignore...");

    /* Check socket */
    dev = ble_access_find_device_by_address(socket->remote_device.address);
    require_action_string(dev != NULL, exit, mico_bt_smartbridge_disconnect(socket, TRUE), "Not a valid socket");

    /* Calculate Device ID */
    parms.device_id = ble_access_calculate_device_id(socket->remote_device.address);

    /* Authentication & DISC_REQ */
    if (dev->service.ctrl_evt_char_value_handle == attribute_handle) {
        if (dev->auth_state == BLE_ACCESS_AUTH_STATE_START) {
            /* Stop timer */
            ble_access_stop_timer(dev);

            /* Authentication Result */
            if (characteristic_value->value.value[0] == BLE_ACCESS_SLAVE_CMD_AUTH_RESULT) {
                if (characteristic_value->value.value[4] == BLE_ACCESS_ERR_NO_ERR) {
                    dev->auth_state = BLE_ACCESS_AUTH_STATE_SUCC;

                    parms.status = BLE_ACCESS_STATUS_SUCCESS;
                    parms.p.add.serv = servs;
                    parms.p.add.serv_count = 0;
                    memset(servs, 0, sizeof(servs) / sizeof(servs[0]));
                    ble_access_cache_all_services(&dev->socket, &parms, sizeof(servs) / sizeof(servs[0]));

                    ble_access_log("Authentication Device Successfully");
                } else {
                    err = kGeneralErr;
                    parms.status = BLE_ACCESS_STATUS_SLAVE_REJECT;
                    dev->auth_state = BLE_ACCESS_AUTH_STATE_FAIL;
                    ble_access_log("Authenticate Device Failed");

                    /* Disconnect */
                    mico_bt_smartbridge_disconnect(&dev->socket, MICO_TRUE);
                    /* Delete its bond information */
                    mico_bt_dev_delete_bonded_device(dev->socket.remote_device.address);
                    /* Release BLE Access Device to Pool */
                    ble_access_release_device(MICO_TRUE, dev);
                }
                ble_access_send_bt_event(BLE_ACCESS_EVENT_DEV_ADD, &parms);
            } else {
                ble_access_log("Authentication: unknown Command[0x%02x]", characteristic_value->value.value[0]);
            }
        } else if (dev->auth_state == BLE_ACCESS_AUTH_STATE_SUCC) {
            if (characteristic_value->value.value[0] == BLE_ACCESS_RES_DISC) {

                ble_access_log("Commiunity: DISC Command, status[0x%02x]", characteristic_value->value.value[4]);

                if (characteristic_value->value.value[4] == BLE_ACCESS_ERR_NO_ERR) {
                    /* Accept */
                    parms.status = BLE_ACCESS_STATUS_SUCCESS;
                    mico_bt_smartbridge_disconnect(&dev->socket, FALSE);
                    ble_access_release_device(MICO_FALSE, dev);
                } else if (characteristic_value->value.value[4] == BLE_ACCESS_ERR_REJECT) {
                    parms.status = BLE_ACCESS_STATUS_SLAVE_REJECT;
                }
                ble_access_send_bt_event(BLE_ACCESS_EVENT_DEV_DISC, &parms);
            } else {
                ble_access_log("Commiunity: unknown Command[0x%02x]", characteristic_value->value.value[0]);
            }
        } else {
            ble_access_log("Authentication or DISC_REQ sequence, substate: 0x%02x", dev->auth_state);
        }
    } else {
        if (dev->auth_state == BLE_ACCESS_AUTH_STATE_SUCC) {
            ble_access_attribute_copy_from_smart(characteristic_value, &attr);
            parms.p.gatt.attr = &attr;
            ble_access_send_bt_event(BLE_ACCESS_EVENT_GATT_NOTIFY, &parms);
        }
    }

exit:
    return err;
}

static OSStatus ble_access_auto_connection_handler(mico_bt_smartbridge_socket_t *socket)
{
    OSStatus                            err = kNoErr;
    ble_access_device_t                *dev = NULL;

    ble_access_serv_t                   servs[BLE_ACCESS_REMOTE_SERVS_NUM];
    ble_access_evt_parms_t              parms;
    mico_bt_smartbridge_socket_status_t status;

    dev = ble_access_find_device_by_address(socket->remote_device.address);
    require_action_string(dev != NULL, exit, err = kGeneralErr,
                          "Unknown a socket which isn't in the pool");

    /* Check current socket status */
    mico_bt_smartbridge_get_socket_status(socket, &status);
    if (status != SMARTBRIDGE_SOCKET_CONNECTED) {
        ble_access_log("An auto connection status: %u", status);
        ble_access_release_device(MICO_FALSE, dev);
        return kConnectionErr;
    }

    ble_access_log("An auto connection is established");

    /* Cache all services */
    err = ble_access_cache_control_service(dev);
    require_noerr_action_string(err,
                                exit,
                                mico_bt_smartbridge_disconnect(&dev->socket, TRUE),
                                "Cache Control Point service failed");

    /* Enable all service indication. */
    err = mico_bt_smartbridge_enable_attribute_cache_notification(socket, MICO_FALSE);
    require_noerr_string(err, exit, "Enable attribute indication failed");

    /* Notify user layer application */
    parms.device_id = ble_access_calculate_device_id(socket->remote_device.address);
    parms.status = BLE_ACCESS_STATUS_SUCCESS;
    parms.p.add.serv = servs;
    parms.p.add.serv_count = 0;
    memset(servs, 0, sizeof(servs) / sizeof(servs[0]));
    ble_access_cache_all_services(socket, &parms, sizeof(servs) / sizeof(servs[0]));
    ble_access_send_bt_event(BLE_ACCESS_EVENT_DEV_CONN, &parms);

exit:
    return err;
}

/* Get Auto Connection Parameters. */
OSStatus ble_access_auto_conn_parms_handler(const mico_bt_device_address_t device_address,
                                            const char *name,
                                            const uint8_t *p_adv_data,
                                            const uint8_t length,
                                            mico_bt_smartbridge_auto_conn_cback_parms_t *parm)
{
    OSStatus                       err = kNoErr;
    ble_access_manufactor_data_t   manufactor_data;
    ble_access_device_t           *dev = NULL;

    ble_access_log("Auto connection: %s[%02x:%02x:%02x:%02x:%02x:%02x]",
                   name,
                   device_address[0],
                   device_address[1],
                   device_address[2],
                   device_address[3],
                   device_address[4],
                   device_address[5]);

    /* Allow to be connected. */
    dev = ble_access_find_device_by_address(device_address);
    require_action_string(dev != NULL, exit, err = kNoResourcesErr,
                          "No resource for new connection");

    /* Check Advertisement Type */
    err = ble_access_check_adv_type(p_adv_data,
                                    length,
                                    BLE_ACCESS_ADV_TYPE_RECONN,
                                    &manufactor_data);
    require_noerr_string(err, exit, "Unknown device or initializing type");

    /* Check target device */
    if (memcmp(manufactor_data.direct_addr, ble_access_local_address, 6) != 0) {
        ble_access_log("The target device is not myself");
        return kUnknownErr;
    }

    parm->socket = &dev->socket;
    memcpy((void *)parm->socket->remote_device.address,
           (void *)device_address,
           sizeof(mico_bt_device_address_t));
    memcpy((void *)parm->socket->remote_device.name,
           (void *)name,
           MIN(sizeof(parm->socket->remote_device.name),
               strlen(name)));

    parm->auto_connection_callback = ble_access_auto_connection_handler;
    parm->auto_disconn_callback = ble_access_disconnection_handler;
    parm->notification_callback = ble_access_notification_handler;
    memcpy((void *)&parm->conn_settings,
           (void *)&ble_access_connection_settings,
           sizeof(mico_bt_smart_connection_settings_t));
    memcpy((void *)&parm->security_settings,
           (void *)&ble_access_security_settings,
           sizeof(mico_bt_smart_security_settings_t));

exit:
    return err;
}

static OSStatus ble_access_timer_event_handle(void *arg)
{
    ble_access_device_t     *dev = (ble_access_device_t *)arg;
    ble_access_evt_parms_t   parms;

    ble_access_log("Authentication Device Timeout");

    ble_access_stop_timer(dev);
    dev->auth_state = BLE_ACCESS_AUTH_STATE_FAIL;

    /* Timeout for add device */
    ble_access_connect_list_add(&dev->socket.remote_device, MICO_FALSE);

    /* Disconnect this device */
    mico_bt_smartbridge_disconnect(&dev->socket, TRUE);
    ble_access_release_device(MICO_TRUE, dev);
    /* Delete its bond information */
    mico_bt_dev_delete_bonded_device(dev->socket.remote_device.address);

    /* Notify user */
    parms.device_id = ble_access_calculate_device_id(dev->socket.remote_device.address);
    parms.status = BLE_ACCESS_STATUS_TIMEOUT;
    ble_access_send_bt_event(BLE_ACCESS_EVENT_DEV_ADD, &parms);

    return kNoErr;
}

/*
 * Post BT Event to Host Application
 */
static OSStatus ble_access_bt_event_handler(void *arg)
{
    uint8_t                 evt_code;
    ble_access_evt_parms_t *parms;

    if (ble_access_callback != NULL) {
        memcpy(&evt_code, (uint8_t *)arg, sizeof(evt_code));
        parms = (ble_access_evt_parms_t *)((uint8_t *)arg + sizeof(evt_code));
        ble_access_callback(evt_code, parms);
    }

    free(arg);
    return kNoErr;
}

/*
 * Post BT Event to Host Application.
 * You should fill the parameters.
 */
static OSStatus ble_access_send_bt_event(uint8_t evt_code, const ble_access_evt_parms_t *parms)
{
    OSStatus err = kGeneralErr;
    uint8_t *arg = NULL;

    if (ble_access_callback != NULL) {
        arg = malloc(sizeof(evt_code) + sizeof(ble_access_evt_parms_t));
        if (arg != NULL) {
            memcpy(arg, &evt_code, sizeof(evt_code));
            memcpy(arg +  sizeof(evt_code), parms, sizeof(ble_access_evt_parms_t));
            err = ble_access_send_aync_event(ble_access_bt_event_handler, arg);
            if (err != kNoErr) {
                free(arg);
            }
        } else {
            err = kNoMemoryErr;
        }
    }
    return err;
}

/* Send a command which is code to the socket pointed by idx. */
static OSStatus ble_access_send_command(const ble_access_device_t *dev, uint8_t code, uint8_t length, uint8_t *p_data)
{
    OSStatus                   err = kNoErr;
    uint8_t                    buf[ATTR_CHARACTERISTIC_VALUE_SIZE(BLE_ACCESS_MAX_CHAR_VALUE_LENGTH)] = {0};
    mico_bt_smart_attribute_t *characteristic_value = NULL;

    if (length == 0
        || p_data == NULL) {
        return kParamErr;
    }

    /* Prepare for writing */
    characteristic_value = (mico_bt_smart_attribute_t *)buf;
    characteristic_value->value_struct_size = BLE_ACCESS_MAX_CHAR_VALUE_LENGTH;

    err = mico_bt_smartbridge_get_attribute_cache_by_handle(
              (mico_bt_smartbridge_socket_t *)&dev->socket,
              dev->service.ctrl_evt_char_value_handle,
              characteristic_value,
              ATTR_CHARACTERISTIC_VALUE_SIZE(BLE_ACCESS_MAX_CHAR_VALUE_LENGTH));
    require_noerr_string(err, exit, "Get attribute failed");

    /* Fill */
    characteristic_value->value_length = 4 + length;
    characteristic_value->value.value[0] = code;
    characteristic_value->value.value[1] = 0;
    characteristic_value->value.value[2] = length + 1;
    characteristic_value->value.value[3] = length;
    memcpy(&characteristic_value->value.value[4], p_data, length);

    /* Write */
    err = mico_bt_smartbridge_write_attribute_cache_characteristic_value((mico_bt_smartbridge_socket_t *)&dev->socket,
                                                                         characteristic_value);
exit:
    return err;
}

static OSStatus ble_access_cache_control_service(ble_access_device_t *dev)
{
    OSStatus err = kNoErr;

    uint8_t buf[ATTR_CHARACTERISTIC_VALUE_SIZE(BLE_ACCESS_MAX_ATTR_VALUE_LENGTH)] = {0};
    mico_bt_smart_attribute_t *attribute = (mico_bt_smart_attribute_t *)buf;

    if (!dev) {
        return kParamErr;
    }

    /* Find service */
    err = mico_bt_smartbridge_get_service_from_attribute_cache_by_uuid(
              (mico_bt_smartbridge_socket_t *)&dev->socket,
              &ble_access_cache_services_uuid[BLE_CONTROL_SERV_UUID_IDX],
              0x00,
              0xff,
              attribute,
              ATTR_CHARACTERISTIC_VALUE_SIZE(BLE_ACCESS_MAX_ATTR_VALUE_LENGTH));
    require_noerr_string(err, exit, "Cache Control Service failed");

    /* Find Control Point Event characteristic, and save characteristic value handle */
    err = mico_bt_smartbridge_get_characteritics_from_attribute_cache_by_uuid(
              (mico_bt_smartbridge_socket_t *)&dev->socket,
              &ble_access_cache_chars_uuid[BLE_CONTROL_EVT_CHAR_UUID_IDX],
              attribute->value.service.start_handle,
              attribute->value.service.end_handle,
              (mico_bt_smart_attribute_t *)attribute,
              ATTR_CHARACTERISTIC_VALUE_SIZE(BLE_ACCESS_MAX_ATTR_VALUE_LENGTH));
    require_noerr_action_string(err,
                                exit,
                                mico_bt_smartbridge_remove_attribute_cache((mico_bt_smartbridge_socket_t *)&dev->socket),
                                "Cache Control Service Response Characteristic Failed");
    dev->service.ctrl_evt_char_value_handle = attribute->value.characteristic.value_handle;

exit:
    return err;
}
