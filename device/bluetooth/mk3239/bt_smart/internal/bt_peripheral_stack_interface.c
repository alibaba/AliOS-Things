/** @file
 *
 */
#include "mico.h"
#include "mico_bt_smartbridge.h"

#include "mico_bt.h"
#include "mico_bt_gatt.h"
#include "mico_bt_ble.h"
#include "mico_bt_cfg.h"

#include "bt_smartbridge_socket_manager.h"
#include "bt_smartbridge_att_cache_manager.h"
#include "bt_smartbridge_helper.h"
#include "bt_smartbridge_stack_interface.h"

#include "StringUtils.h"

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

#define INTERNAL_SECURITY_LEVEL ( 1 ) /* Encryption enabled, no pairing requested because device is already paired */

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Static Function Declarations
 ******************************************************/

/******************************************************
 *               Variable Definitions
 ******************************************************/

gatt_subprocedure_t              peripheral_subprocedure;

extern mico_bt_cfg_settings_t         mico_bt_cfg_settings;
extern mico_bt_dev_ble_io_caps_req_t  default_io_caps_ble;

extern mico_bt_gatt_status_t   bt_peripheral_gatt_callback( mico_bt_gatt_evt_t event,
                                                            mico_bt_gatt_event_data_t *p_event_data );

mico_bt_smart_advertising_complete_callback_t      app_advertising_complete_callback;

/******************************************************
 *               Function Definitions
 ******************************************************/

OSStatus peripheral_bt_interface_initialize( void )
{
    OSStatus result;

    bt_peripheral_log( "Initializing Bluetooth Interface..." );

    result = mico_rtos_init_mutex( &peripheral_subprocedure.mutex );
    if ( result  != MICO_BT_SUCCESS ) {
        bt_peripheral_log( "Error creating mutex" );
        return result;
    }

    result = mico_rtos_init_semaphore( &peripheral_subprocedure.done_semaphore, 1 );
    if ( result != MICO_BT_SUCCESS ) {
        bt_peripheral_log( "Error creating semaphore" );
        return result;
    }
    subprocedure_reset( &peripheral_subprocedure );

    mico_bt_gatt_register( GATT_IF_FIXED_DB_APP, bt_peripheral_gatt_callback );

    return MICO_BT_SUCCESS;
}

OSStatus peripheral_bt_interface_deinitialize( void )
{
    bt_peripheral_log( "Deinitializing Bluetooth Interface..." );

    subprocedure_reset( &peripheral_subprocedure );
    mico_rtos_deinit_mutex( &peripheral_subprocedure.mutex );
    mico_rtos_deinit_semaphore( &peripheral_subprocedure.done_semaphore );

    return MICO_BT_SUCCESS;
}

OSStatus peripheral_bt_interface_cancel_last_connect( mico_bt_device_address_t address )
{
    return mico_bt_gatt_cancel_connect( address, MICO_TRUE );
}

OSStatus peripheral_bt_interface_disconnect( uint16_t connection_handle )
{
    return mico_bt_gatt_disconnect( connection_handle );
}

OSStatus peripheral_bt_interface_set_security_settings( const mico_bt_smart_security_settings_t *settings )
{
    /* update the security settings as per passed by the application */
    default_io_caps_ble.local_io_cap      = settings->io_capabilities;
    default_io_caps_ble.auth_req          = settings->authentication_requirements;
    default_io_caps_ble.oob_data          = settings->oob_authentication;
    default_io_caps_ble.max_key_size      = settings->max_encryption_key_size;
    default_io_caps_ble.init_keys         = settings->master_key_distribution;
    default_io_caps_ble.resp_keys         = settings->slave_key_distribution;

    return MICO_BT_SUCCESS;
}

OSStatus peripheral_bt_interface_start_advertisements( mico_bt_smart_advertising_settings_t *settings,
                                                       mico_bt_smart_advertising_complete_callback_t complete_callback )
{
    mico_bt_smart_advertising_type_t advertising_type = settings->type;
    mico_bool_t high_duty = settings->use_high_duty;
    mico_bt_ble_advert_mode_t mode;

    app_advertising_complete_callback = complete_callback;

    switch ( advertising_type ) {
        case BT_SMART_UNDIRECTED_ADVERTISING: {

            mico_bt_cfg_settings.ble_advert_cfg.high_duty_min_interval  = settings->high_duty_interval;
            mico_bt_cfg_settings.ble_advert_cfg.high_duty_max_interval  = settings->high_duty_interval;
            mico_bt_cfg_settings.ble_advert_cfg.high_duty_duration      = settings->high_duty_duration;

            mico_bt_cfg_settings.ble_advert_cfg.low_duty_min_interval   = settings->low_duty_interval;
            mico_bt_cfg_settings.ble_advert_cfg.low_duty_max_interval   = settings->low_duty_interval;
            mico_bt_cfg_settings.ble_advert_cfg.low_duty_duration       = settings->low_duty_duration;

            mode = high_duty ? BTM_BLE_ADVERT_UNDIRECTED_HIGH : BTM_BLE_ADVERT_UNDIRECTED_LOW;

            return mico_bt_start_advertisements( mode, 0, NULL );

        }
        case BT_SMART_NON_CONNECTABLE_UNDIRECTED_ADVERTISING:
        case BT_SMART_DISCOVERABLE_ADVERTISING: {
            mico_bt_cfg_settings.ble_advert_cfg.high_duty_nonconn_min_interval  = settings->high_duty_interval;
            mico_bt_cfg_settings.ble_advert_cfg.high_duty_nonconn_max_interval  = settings->high_duty_interval;
            mico_bt_cfg_settings.ble_advert_cfg.high_duty_nonconn_duration      = settings->high_duty_duration;

            mico_bt_cfg_settings.ble_advert_cfg.low_duty_nonconn_min_interval   = settings->low_duty_interval;
            mico_bt_cfg_settings.ble_advert_cfg.low_duty_nonconn_max_interval   = settings->low_duty_interval;
            mico_bt_cfg_settings.ble_advert_cfg.low_duty_nonconn_duration       = settings->low_duty_duration;

            if ( advertising_type == BT_SMART_NON_CONNECTABLE_UNDIRECTED_ADVERTISING ) {
                mode = high_duty ? BTM_BLE_ADVERT_NONCONN_HIGH : BTM_BLE_ADVERT_NONCONN_LOW;
            } else {
                mode = high_duty ? BTM_BLE_ADVERT_DISCOVERABLE_HIGH : BTM_BLE_ADVERT_DISCOVERABLE_LOW;
            }

            return mico_bt_start_advertisements( mode, 0, NULL );

        }
        case BT_SMART_DIRECTED_ADVERTISING: {
            mico_bt_cfg_settings.ble_advert_cfg.low_duty_directed_min_interval   = settings->low_duty_interval;
            mico_bt_cfg_settings.ble_advert_cfg.low_duty_directed_max_interval   = settings->low_duty_interval;
            mico_bt_cfg_settings.ble_advert_cfg.low_duty_directed_duration       = settings->low_duty_duration;

            mico_bt_cfg_settings.ble_advert_cfg.high_duty_directed_min_interval  = settings->high_duty_interval;
            mico_bt_cfg_settings.ble_advert_cfg.high_duty_directed_max_interval  = settings->high_duty_interval;

            mode = high_duty ? BTM_BLE_ADVERT_DIRECTED_HIGH : BTM_BLE_ADVERT_DIRECTED_LOW;

            return mico_bt_start_advertisements( BTM_BLE_ADVERT_DIRECTED_HIGH, settings->directed_advertisement_addr_type,
                                                 settings->directed_advertisement_addr );

        }
        default:
            return kUnknownErr;
    }
}

OSStatus peripheral_bt_interface_stop_advertisements( void )
{
    app_advertising_complete_callback = NULL;
    return mico_bt_start_advertisements( BTM_BLE_ADVERT_OFF, 0, NULL );
}

void peripheral_bt_interface_advertisements_state_change_callback( mico_bt_ble_advert_mode_t state )
{
    if ( state == BTM_BLE_ADVERT_OFF && app_advertising_complete_callback != NULL ) {
        mico_rtos_send_asynchronous_event( MICO_BT_EVT_WORKER_THREAD, app_advertising_complete_callback, NULL );
    }
}

OSStatus peripheral_bt_interface_indicate_attribute_value ( uint16_t connection_handle,
                                                            const mico_bt_ext_attribute_value_t *attribute )
{
    uint16_t val_len = 0;
    uint16_t offset = 0;

    subprocedure_lock( &peripheral_subprocedure );

    subprocedure_reset( &peripheral_subprocedure );

    val_len = attribute->value_length;

    while (  mico_bt_gatt_send_indication( connection_handle, attribute->handle, &val_len,
                                           attribute->p_value + offset ) == MICO_BT_GATT_SUCCESS ) {
        subprocedure_wait_for_completion( &peripheral_subprocedure );

        if ( peripheral_subprocedure.result != MICO_BT_SUCCESS ) {
            break;
        }

        offset += val_len;
        val_len = attribute->value_length - offset;

        if ( offset >= attribute->value_length ) {
            break;
        }
    }

    subprocedure_unlock( &peripheral_subprocedure );

    return peripheral_subprocedure.result;
}

OSStatus peripheral_bt_interface_notify_attribute_value( uint16_t connection_handle,
                                                         const mico_bt_ext_attribute_value_t *attribute )
{
    uint16_t val_len = 0;
    uint16_t offset = 0;

    subprocedure_lock( &peripheral_subprocedure );

    subprocedure_reset( &peripheral_subprocedure );

    val_len = attribute->value_length;

    while (  mico_bt_gatt_send_notification( connection_handle, attribute->handle, &val_len,
                                             attribute->p_value + offset ) == MICO_BT_GATT_SUCCESS ) {
        offset += val_len;
        val_len = attribute->value_length - offset;

        if ( offset >= attribute->value_length ) {
            break;
        }
    }

    subprocedure_unlock( &peripheral_subprocedure );

    return kNoErr;
}

#if 0
OSStatus smartbridge_bt_interface_start_advertise( const mico_bt_smart_advertise_settings_t *settings,
                                                   mico_bt_smart_advertise_mode_changed_callback_t advertise_mode_changed_callback )
{
    mico_bool_t duplicate_filter_enabled = MICO_FALSE;

    /* fill with the settings provided by the smartserver-application */
    mico_bt_cfg_settings.ble_advert_cfg.



    mico_bt_cfg_settings.ble_scan_cfg.scan_mode               = settings->type;
    mico_bt_cfg_settings.ble_scan_cfg.high_duty_scan_window   = settings->window;
    mico_bt_cfg_settings.ble_scan_cfg.high_duty_scan_duration = settings->duration_second;
    mico_bt_cfg_settings.ble_scan_cfg.high_duty_scan_interval = settings->interval;
    duplicate_filter_enabled                                  = settings->filter_duplicates;

    app_scan_complete_callback = complete_callback;
    app_scan_report_callback   = advertising_report_callback;

    return mico_bt_start_advertisements( BTM_BLE_ADVERT_UNDIRECTED_LOW, 0, NULL );
    return mico_bt_ble_scan( BTM_BLE_SCAN_TYPE_HIGH_DUTY, duplicate_filter_enabled, smartbridge_scan_result_callback );
}


OSStatus smartbridge_bt_interface_stop_scan( )
{
    app_scan_complete_callback = NULL;
    app_scan_report_callback   = NULL;

    return mico_bt_ble_scan( BTM_BLE_SCAN_TYPE_NONE, MICO_TRUE, smartbridge_scan_result_callback );
}

OSStatus smartbridge_bt_interface_start_scan( const mico_bt_smart_scan_settings_t *settings,
                                              mico_bt_smart_scan_complete_callback_t complete_callback,
                                              mico_bt_smart_advertising_report_callback_t advertising_report_callback )
{
    mico_bool_t duplicate_filter_enabled = MICO_FALSE;

    /* First delete the previous scan result list */
    smartbridge_helper_delete_scan_result_list();

    /* fill with the settings provided by the smartbridge-application */
    mico_bt_cfg_settings.ble_scan_cfg.scan_mode               = settings->type;
    mico_bt_cfg_settings.ble_scan_cfg.high_duty_scan_window   = settings->window;
    mico_bt_cfg_settings.ble_scan_cfg.high_duty_scan_duration = settings->duration_second;
    mico_bt_cfg_settings.ble_scan_cfg.high_duty_scan_interval = settings->interval;
    duplicate_filter_enabled                                  = settings->filter_duplicates;

    app_scan_complete_callback = complete_callback;
    app_scan_report_callback   = advertising_report_callback;

    return mico_bt_ble_scan( BTM_BLE_SCAN_TYPE_HIGH_DUTY, duplicate_filter_enabled, smartbridge_scan_result_callback );
}
#endif

OSStatus peripheral_bt_interface_update_advertisements_white_list( mico_bool_t add,
                                                                   mico_bt_device_address_t device_address )
{
    if ( device_address == 0 ) {
        return kParamErr;
    }
    if ( add ) {
        if ( TRUE != mico_bt_ble_update_advertising_white_list( MICO_TRUE, device_address ) ) {
            return kGeneralErr;
        }
    } else {
        if ( TRUE != mico_bt_ble_update_advertising_white_list( MICO_FALSE, device_address ) ) {
            return kGeneralErr;
        }
    }
    return kNoErr;
}

OSStatus peripheral_bt_interface_get_advertisements_white_list_size( uint8_t *size )
{
    if ( TRUE != mico_bt_ble_get_advertisement_white_list_size(size) ) {
        return kGeneralErr;
    }
    return kNoErr;
}

OSStatus peripheral_bt_interface_set_advertisements_filter_policy(mico_bt_peripheral_adv_filter_policy_t type)
{
    OSStatus status = kNoErr;
    mico_bt_ble_advert_filter_policy_t policy;

    switch (type) {
        case PERIPHERAL_ADVERT_FILTER_ALL_CONNECTION_REQ_ALL_SCAN_REQ:
            policy = BTM_BLE_ADVERT_FILTER_ALL_CONNECTION_REQ_ALL_SCAN_REQ;
            break;
        case PERIPHERAL_ADVERT_FILTER_ALL_CONNECTION_REQ_WHITELIST_SCAN_REQ:
            policy = BTM_BLE_ADVERT_FILTER_ALL_CONNECTION_REQ_WHITELIST_SCAN_REQ;
            break;
        case PERIPHERAL_ADVERT_FILTER_WHITELIST_CONNECTION_REQ_ALL_SCAN_REQ:
            policy = BTM_BLE_ADVERT_FILTER_WHITELIST_CONNECTION_REQ_ALL_SCAN_REQ;
            break;
        case PERIPHERAL_ADVERT_FILTER_WHITELIST_CONNECTION_REQ_WHITELIST_SCAN_REQ:
            policy = BTM_BLE_ADVERT_FILTER_WHITELIST_CONNECTION_REQ_WHITELIST_SCAN_REQ;
            break;
        default:
            return kParamErr;
    }

    if (!mico_bt_ble_update_advertisement_filter_policy(policy)) {
        status = kUnknownErr;
    }
    return status;
}


