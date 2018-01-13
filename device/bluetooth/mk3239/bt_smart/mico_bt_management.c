
/** @file
 *
 */

#include "mico.h"
#include "mico_bt.h"
#include "mico_bt_smartbridge_constants.h"
#include "mico_bt_smartbridge.h"
#include "mico_bt_types.h"
#include "mico_bt_stack.h"
#ifdef BT_MFGTEST_MODE
#include "bt_mfgtest.h"
#endif
#include "bt_smartbridge_helper.h"
#include "bt_smartbridge_stack_interface.h"
#include "bt_smartbridge_socket_manager.h"

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

/* Stack size and Queue size for BT Worker Thread */
#define MICO_BT_WORKER_THREAD_STACK_SIZE        (3 * 1024)
#define MICO_BT_WORKER_THREAD_QUEUE_SIZE        15
/* Stack size and Queue size for BT Event Worker thread */
#define MICO_BT_EVT_WORKER_THREAD_STACK_SIZE    (3 * 1024)
#define MICO_BT_EVT_WORKER_THREAD_QUEUE_SIZE    15

#define BT_DEVICE_NAME_MAX_LENGTH               21

/* Encryption enabled, no pairing requested because device is already paired */
#define INTERNAL_SECURITY_LEVEL                 ( 1 )

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

static mico_bt_dev_status_t    smartbridge_bt_stack_management_callback    ( mico_bt_management_evt_t event,
                                                                             mico_bt_management_evt_data_t *p_event_data );

/******************************************************
 *               Variable Definitions
 ******************************************************/
mico_worker_thread_t                    mico_bt_worker_thread;
mico_worker_thread_t                    mico_bt_evt_worker_thread;

mico_bt_local_identity_keys_t           local_identity_keys;
mico_bt_device_sec_keys_t               device_link_keys;
mico_bool_t                             device_link_key_updated = MICO_FALSE;
extern mico_bt_cfg_settings_t           mico_bt_cfg_settings;
extern mico_bt_dev_ble_io_caps_req_t    default_io_caps_ble;
extern mico_bt_smartbridge_socket_t    *connecting_socket;
extern mico_bt_peripheral_socket_t     *connecting_peripheral_socket;
extern const mico_bt_cfg_buf_pool_t     mico_bt_cfg_buf_pools[];
static char                             bt_device_name[BT_DEVICE_NAME_MAX_LENGTH + 1] = { 0 };
mico_bool_t                             bt_initialised           = MICO_FALSE;
static mico_semaphore_t                 wait_bt_initialised_sem  = NULL;
static mico_bt_device_address_t         bt_address               = { 0 };

extern void peripheral_bt_interface_advertisements_state_change_callback( mico_bt_ble_advert_mode_t state );
extern void smartbridge_auto_connection_encryption_check(const mico_bt_dev_pairing_cplt_t *p_event_data);

/******************************************************
 *               Function Definitions
 ******************************************************/

OSStatus mico_bt_init( mico_bt_mode_t mode, const char *device_name, uint8_t client_links, uint8_t server_links )
{
    OSStatus result = MICO_BT_SUCCESS;

    if ( bt_initialised == MICO_TRUE ) {
        return MICO_BT_SUCCESS;
    }

    bt_manager_log( "Initializing Bluetooth" );

    if ( mode == MICO_BT_MPAF_MODE ) {
        bt_manager_log( "Error. MPAF-mode is not supported" );
        return MICO_BT_UNSUPPORTED;
    }

    /* Create the BT Woker thread */
    result = mico_rtos_create_worker_thread(MICO_BT_WORKER_THREAD,
                                            MICO_APPLICATION_PRIORITY,
                                            MICO_BT_WORKER_THREAD_STACK_SIZE,
                                            MICO_BT_WORKER_THREAD_QUEUE_SIZE);
    if (result != kNoErr) {
        bt_smartbridge_log("Error creating BT WORKER THREAD");
        goto exit;
    }

    /* Create the BT Event Woker thread */
    result = mico_rtos_create_worker_thread(MICO_BT_EVT_WORKER_THREAD,
                                            MICO_APPLICATION_PRIORITY,
                                            MICO_BT_EVT_WORKER_THREAD_STACK_SIZE,
                                            MICO_BT_EVT_WORKER_THREAD_QUEUE_SIZE);
    if (result != kNoErr) {
        bt_smartbridge_log("Error create BT EVT WORKER THREAD");
        goto err1;
    }

    /* Initialise Bluetooth Stack */
    wait_bt_initialised_sem = NULL;
    mico_rtos_init_semaphore( &wait_bt_initialised_sem, 1 );

    mico_bt_cfg_settings.device_name = (uint8_t *)device_name;
    mico_bt_cfg_settings.max_simultaneous_links = client_links + server_links;
    mico_bt_cfg_settings.gatt_cfg.client_max_links = client_links;
    mico_bt_cfg_settings.gatt_cfg.server_max_links = server_links;

    result = mico_bt_stack_init( smartbridge_bt_stack_management_callback, &mico_bt_cfg_settings, mico_bt_cfg_buf_pools );
    if ( result != MICO_BT_SUCCESS ) {
        bt_smartbridge_log( "Error initialising Bluetooth stack" );
        goto err2;
    }
    mico_rtos_get_semaphore( &wait_bt_initialised_sem, MICO_NEVER_TIMEOUT );
    mico_rtos_deinit_semaphore( &wait_bt_initialised_sem );

    mico_bt_dev_read_local_addr( bt_address );
    memset( bt_device_name, 0, sizeof( bt_device_name ) );
    memcpy( bt_device_name, device_name, strnlen( device_name, BT_DEVICE_NAME_MAX_LENGTH ) );
    mico_rtos_delay_milliseconds(10);

    result = MICO_BT_SUCCESS;
    goto exit;

err2:
    mico_rtos_deinit_semaphore( &wait_bt_initialised_sem );
    mico_rtos_delete_worker_thread(MICO_BT_EVT_WORKER_THREAD);

err1:
    mico_rtos_delete_worker_thread(MICO_BT_WORKER_THREAD);

exit:
    return result;
}

OSStatus mico_bt_deinit( void )
{
    OSStatus result;

    if ( bt_initialised == MICO_FALSE ) {
        return MICO_BT_SUCCESS;
    }

    result = mico_bt_stack_deinit( );
    if ( result != MICO_BT_SUCCESS ) {
        bt_manager_log( "Error de-initialising Bluetooth stack" );
        return result;
    }

    mico_rtos_delete_worker_thread(MICO_BT_WORKER_THREAD);
    mico_rtos_delete_worker_thread(MICO_BT_EVT_WORKER_THREAD);

    memset( bt_device_name, 0, sizeof( bt_device_name ) );
    bt_initialised = MICO_FALSE;
    return MICO_BT_SUCCESS;
}

OSStatus mico_bt_init_address( const mico_bt_device_address_t *address, const mico_bt_device_address_t *mask )
{
    UNUSED_PARAMETER(address);
    UNUSED_PARAMETER(mask);
    return MICO_BT_UNSUPPORTED;
}

OSStatus mico_bt_start_mfgtest_mode( const mico_uart_config_t *config )
{
#ifdef BT_MFGTEST_MODE
    OSStatus result = bt_bus_init();
    if ( result != MICO_BT_SUCCESS ) {
        bt_manager_log( "Error initialising Bluetooth transport bus" );
        return result;
    }

    return bt_mfgtest_start( config );
#else
    return MICO_BT_UNSUPPORTED;
#endif
}

OSStatus mico_bt_device_get_address( mico_bt_device_address_t *address )
{
    if ( bt_initialised == MICO_FALSE ) {
        return MICO_BT_ERROR;
    }

    memcpy( address, &bt_address, sizeof( *address ) );
    return MICO_BT_SUCCESS;
}

const char *mico_bt_device_get_name( void )
{
    /* TODO: bt_device_name is not provided to Stack/Controller. Once it is provided to the Stack
     * correctly, we should rather read the name of the device from the Stack */
    return (const char *)bt_device_name;
}

mico_bool_t mico_bt_device_is_on( void )
{
    /* TODO: Need to be implemented again as per the new Stack */
    return MICO_FALSE;
}

mico_bool_t mico_bt_device_is_connectable( void )
{
    /* TODO :  Need to be implemented for the new-stack */
    return MICO_FALSE;
}

mico_bool_t mico_bt_device_is_discoverable( void )
{
    /* TODO :  Need to be implemented for the new-stack */
    return MICO_FALSE;
}


OSStatus mico_bt_start_pairing( mico_bt_device_address_t address, mico_bt_smart_address_type_t type,
                                const mico_bt_smart_security_settings_t *settings )
{
    OSStatus    err = kNoErr;
    uint8_t     integer_passkey[4]        = { 0 };

    /* update the security settings as per passed by the application */
    default_io_caps_ble.local_io_cap      = settings->io_capabilities;
    default_io_caps_ble.auth_req          = settings->authentication_requirements;
    default_io_caps_ble.oob_data          = settings->oob_authentication;
    default_io_caps_ble.max_key_size      = settings->max_encryption_key_size;
    default_io_caps_ble.init_keys         = settings->master_key_distribution;
    default_io_caps_ble.resp_keys         = settings->slave_key_distribution;

    err = mico_bt_dev_sec_bond( address, type, BT_TRANSPORT_LE, 4, integer_passkey );
    bt_manager_log( "mico_bt_start_pairing, ret = %d", err );
    return err;
}

OSStatus mico_bt_stop_pairing( mico_bt_device_address_t address )
{
    OSStatus    err = kNoErr;

    err = mico_bt_dev_sec_bond_cancel( address );
    bt_manager_log( "mico_bt_stop_pairing, ret = %d", err );
    return err;
}

OSStatus mico_bt_start_encryption( mico_bt_device_address_t *address )
{
    OSStatus    err = kNoErr;
    uint32_t security_level = INTERNAL_SECURITY_LEVEL;

    err = mico_bt_dev_set_encryption( *address, BT_TRANSPORT_LE, &security_level );
    bt_manager_log( "mico_bt_start_encryption, ret = %d", err );
    return err;
}

OSStatus mico_bt_clear_whitelist( void )
{
    if ( TRUE != mico_bt_ble_clear_white_list( ) ) {
        return kGeneralErr;
    }
    return kNoErr;
}

OSStatus mico_bt_get_whitelist_capability( uint8_t *size )
{
    if ( size == (uint8_t *)0 ) {
        return kGeneralErr;
    }
    if ( TRUE != mico_bt_ble_get_white_list_capability( size ) ) {
        return kGeneralErr;
    }
    return kNoErr;
}

static mico_bt_dev_status_t smartbridge_bt_stack_management_callback( mico_bt_management_evt_t event,
                                                                      mico_bt_management_evt_data_t *p_event_data )
{
    mico_bt_dev_status_t            status = MICO_BT_SUCCESS;
    BD_ADDR                         address;

    switch (event) {
        case BTM_ENABLED_EVT: {
            /* Initialize GATT REST API Server once Bluetooth controller and host stack is enabled */
            if ( ( status = p_event_data->enabled.status ) == MICO_BT_SUCCESS ) {
                mico_bt_dev_read_local_addr(address);
                bt_manager_log( "Local Bluetooth Address: [%02X:%02X:%02X:%02X:%02X:%02X]", address[0], address[1], address[2],
                                address[3], address[4], address[5] );
                /* Register for GATT event notifications */
                bt_initialised = MICO_TRUE;
                if ( wait_bt_initialised_sem ) {
                    mico_rtos_set_semaphore( &wait_bt_initialised_sem );
                }
            }
            break;
        }

        case BTM_SECURITY_REQUEST_EVT: {
            bt_manager_log( "Security grant request" );
            mico_bt_ble_security_grant( p_event_data->security_request.bd_addr, MICO_BT_SUCCESS );
            break;
        }

        case BTM_LOCAL_IDENTITY_KEYS_UPDATE_EVT: {
            memcpy(&local_identity_keys, &p_event_data->local_identity_keys_update, sizeof( mico_bt_local_identity_keys_t ) );
            bt_manager_log( "Local Identity Keys Update type:%u", local_identity_keys.local_key_data[0] );
            break;
        }

        case BTM_LOCAL_IDENTITY_KEYS_REQUEST_EVT: {
            memcpy( &p_event_data->local_identity_keys_request , &local_identity_keys, sizeof(mico_bt_local_identity_keys_t) );
            bt_manager_log( "Local Identity Keys Request Event" );
            break;
        }

        case BTM_PAIRED_DEVICE_LINK_KEYS_UPDATE_EVT: {
            bt_manager_log( "Paired Device Link Keys Update Event" );
            memcpy(&device_link_keys, &p_event_data->paired_device_link_keys_request.key_data, sizeof(mico_bt_device_sec_keys_t));
            device_link_key_updated  = MICO_TRUE;
            break;
        }

        case BTM_PAIRED_DEVICE_LINK_KEYS_REQUEST_EVT: {
            bt_manager_log( "Paired Device Link Keys Request Event" );
            if ( device_link_key_updated == MICO_TRUE ) {
                memcpy( &p_event_data->paired_device_link_keys_request.key_data, &device_link_keys, sizeof(mico_bt_device_sec_keys_t));
                return MICO_BT_SUCCESS;
            }
            status = MICO_BT_ERROR;
            break;
        }

        case BTM_PAIRING_IO_CAPABILITIES_BLE_REQUEST_EVT: {
            bt_manager_log( "Pairing IO capabilities Request Event" );

            /* Peer requested for I/O capabilities. Copy local I/O caps to stack */
            bt_smartbridge_log( "Getting local I/O capabilities" );
            p_event_data->pairing_io_capabilities_ble_request.local_io_cap = default_io_caps_ble.local_io_cap;
            p_event_data->pairing_io_capabilities_ble_request.oob_data     = default_io_caps_ble.oob_data;
            p_event_data->pairing_io_capabilities_ble_request.auth_req     = default_io_caps_ble.auth_req;
            p_event_data->pairing_io_capabilities_ble_request.max_key_size = default_io_caps_ble.max_key_size;
            p_event_data->pairing_io_capabilities_ble_request.init_keys    = default_io_caps_ble.init_keys;
            p_event_data->pairing_io_capabilities_ble_request.resp_keys    = default_io_caps_ble.resp_keys;
            break;
        }

        case BTM_PAIRING_COMPLETE_EVT: {
            bt_manager_log( "Pairing complete status=%i, reason=0x%x.",
                            p_event_data->pairing_complete.pairing_complete_info.ble.status,
                            p_event_data->pairing_complete.pairing_complete_info.ble.reason );
            if ( p_event_data->pairing_complete.pairing_complete_info.ble.status == MICO_BT_SUCCESS ) {
            }
            /* Notify app thread that pairing is complete, regardless of  */
            if ( connecting_socket &&
                 memcmp( p_event_data->pairing_complete.bd_addr, connecting_socket->remote_device.address, BD_ADDR_LEN ) == 0 ) {
                if ( smartbridge_helper_socket_check_actions_enabled( connecting_socket,
                                                                      SOCKET_ACTION_INITIATE_PAIRING ) == MICO_TRUE ) {
                    if ( connecting_socket->semaphore ) {
                        mico_rtos_set_semaphore( &connecting_socket->semaphore );
                    }
                }
            }

            if ( connecting_peripheral_socket &&
                 memcmp( p_event_data->pairing_complete.bd_addr, connecting_peripheral_socket->remote_device.address,
                         BD_ADDR_LEN ) == 0 ) {
                if ( peripheral_helper_socket_check_actions_enabled( connecting_peripheral_socket,
                                                                     SOCKET_ACTION_INITIATE_PAIRING ) == MICO_TRUE ) {
                    if ( connecting_peripheral_socket->semaphore ) {
                        mico_rtos_set_semaphore( &connecting_peripheral_socket->semaphore );
                    }
                }
            }

            break;
        }

        case BTM_ENCRYPTION_STATUS_EVT: {
            bt_manager_log( "encryption status = %i", p_event_data->encryption_status.result );

            if ( p_event_data->encryption_status.result == MICO_BT_SUCCESS ) {
                /* Update state of the socket to encrypted */
                if ( connecting_socket ) {
                    connecting_socket->state = SOCKET_STATE_LINK_ENCRYPTED;
                }
                if ( connecting_peripheral_socket ) {
                    connecting_peripheral_socket->state = SOCKET_STATE_LINK_ENCRYPTED;
                }
            }

            /* Notify app thread that pairing is complete, regardless of  */
            if ( connecting_socket &&
                 memcmp( p_event_data->pairing_complete.bd_addr, connecting_socket->remote_device.address, BD_ADDR_LEN ) == 0 ) {
                if ( smartbridge_helper_socket_check_actions_enabled( connecting_socket,
                                                                      SOCKET_ACTION_INITIATE_PAIRING ) == MICO_TRUE ||
                     smartbridge_helper_socket_check_actions_enabled( connecting_socket,
                                                                      SOCKET_ACTION_ENCRYPT_USING_BOND_INFO ) ==  MICO_TRUE ) {
                    if ( connecting_socket->semaphore ) {
                        mico_rtos_set_semaphore( &connecting_socket->semaphore );
                    }
                }
            }

            if ( connecting_peripheral_socket &&
                 memcmp( p_event_data->pairing_complete.bd_addr, connecting_peripheral_socket->remote_device.address,
                         BD_ADDR_LEN ) == 0 ) {
                if ( peripheral_helper_socket_check_actions_enabled( connecting_peripheral_socket,
                                                                     SOCKET_ACTION_INITIATE_PAIRING ) == MICO_TRUE  ||
                     peripheral_helper_socket_check_actions_enabled( connecting_peripheral_socket,
                                                                     SOCKET_ACTION_ENCRYPT_USING_BOND_INFO ) ==  MICO_TRUE ) {
                    if ( connecting_peripheral_socket->semaphore ) {
                        mico_rtos_set_semaphore( &connecting_peripheral_socket->semaphore );
                    }
                }
                break;
            }

            /* Auto connection encryption completed. */
            smartbridge_auto_connection_encryption_check(&p_event_data->pairing_complete);
            break;
        }

        case BTM_BLE_SCAN_STATE_CHANGED_EVT: {
            bt_manager_log( "Scan state changed: %d", p_event_data->ble_scan_state_changed );
            break;
        }

        case BTM_BLE_ADVERT_STATE_CHANGED_EVT: {
            bt_manager_log( "Advertisement state changed: %d", p_event_data->ble_advert_state_changed );
            peripheral_bt_interface_advertisements_state_change_callback( p_event_data->ble_advert_state_changed );
            break;
        }

        case BTM_PASSKEY_NOTIFICATION_EVT: {
            bt_manager_log("Passkey Notification event( PassKey generated is: %u )",
                           (unsigned int)p_event_data->user_passkey_notification.passkey );

            /* Smartbridge socket */
            if (connecting_socket
                && memcmp(p_event_data->user_passkey_notification.bd_addr,
                          connecting_socket->remote_device.address,
                          BD_ADDR_LEN) == 0) {
                if (connecting_socket->bonding_callback
                    && smartbridge_helper_socket_check_actions_enabled(connecting_socket, SOCKET_ACTION_INITIATE_PAIRING)) {
                    connecting_socket->bond_req.type = MICO_BT_SMART_BOND_PASS_KEY_NOTIFY;
                    connecting_socket->bond_req.u.notify.passkey = p_event_data->user_passkey_notification.passkey;
                    memcpy(connecting_socket->bond_req.u.notify.addr, p_event_data->user_passkey_notification.bd_addr, 6);

                    mico_rtos_send_asynchronous_event(MICO_BT_EVT_WORKER_THREAD,
                                                      (event_handler_t)connecting_socket->bonding_callback,
                                                      (void *)&connecting_socket->bond_req);
                }
            }

            /* Smart Peripheral Socket */
            if (connecting_peripheral_socket
                && memcmp(p_event_data->user_passkey_notification.bd_addr,
                          connecting_peripheral_socket->remote_device.address,
                          BD_ADDR_LEN) == 0) {
                if (connecting_peripheral_socket->bonding_callback
                    && peripheral_helper_socket_check_actions_enabled(connecting_peripheral_socket, SOCKET_ACTION_INITIATE_PAIRING)) {
                    connecting_peripheral_socket->bond_req.type = MICO_BT_SMART_BOND_PASS_KEY_NOTIFY;
                    connecting_peripheral_socket->bond_req.u.notify.passkey = p_event_data->user_passkey_notification.passkey;
                    memcpy(connecting_peripheral_socket->bond_req.u.notify.addr, p_event_data->user_passkey_notification.bd_addr, 6);

                    mico_rtos_send_asynchronous_event(MICO_BT_EVT_WORKER_THREAD,
                                                      (event_handler_t)connecting_peripheral_socket->bonding_callback,
                                                      (void *)&connecting_peripheral_socket->bond_req);
                }
            }
            break;
        }

        case BTM_PASSKEY_REQUEST_EVT: {
            bt_manager_log("Passkey Request event: dev[%02x:%02x:%02x:%02x:%02x:%02x]" ,
                           p_event_data->user_passkey_request.bd_addr[0],
                           p_event_data->user_passkey_request.bd_addr[1],
                           p_event_data->user_passkey_request.bd_addr[2],
                           p_event_data->user_passkey_request.bd_addr[3],
                           p_event_data->user_passkey_request.bd_addr[4],
                           p_event_data->user_passkey_request.bd_addr[5]);

            if (connecting_socket
                && memcmp(p_event_data->user_passkey_request.bd_addr,
                          connecting_socket->remote_device.address,
                          BD_ADDR_LEN) == 0) {
                if (connecting_socket->bonding_callback
                    && smartbridge_helper_socket_check_actions_enabled(connecting_socket, SOCKET_ACTION_INITIATE_PAIRING)) {
                    connecting_socket->bond_req.type = MICO_BT_SMART_BOND_PASS_KEY_REQ;
                    memcpy(connecting_socket->bond_req.u.passkey.addr, p_event_data->user_passkey_request.bd_addr, 6);
                    mico_rtos_send_asynchronous_event(MICO_BT_EVT_WORKER_THREAD,
                                                      (event_handler_t)connecting_socket->bonding_callback,
                                                      (void *)&connecting_socket->bond_req);
                } else {
                    mico_bt_dev_pass_key_req_reply(MICO_BT_ERROR, p_event_data->user_passkey_request.bd_addr, 0);
                }
            }

            if (connecting_peripheral_socket
                && memcmp(p_event_data->user_passkey_request.bd_addr,
                          connecting_peripheral_socket->remote_device.address,
                          BD_ADDR_LEN) == 0) {
                if (connecting_peripheral_socket->bonding_callback
                    && peripheral_helper_socket_check_actions_enabled(connecting_peripheral_socket, SOCKET_ACTION_INITIATE_PAIRING)) {
                    connecting_peripheral_socket->bond_req.type = MICO_BT_SMART_BOND_PASS_KEY_REQ;
                    memcpy(connecting_peripheral_socket->bond_req.u.passkey.addr, p_event_data->user_passkey_request.bd_addr, 6);
                    mico_rtos_send_asynchronous_event(MICO_BT_EVT_WORKER_THREAD,
                                                      (event_handler_t)connecting_peripheral_socket->bonding_callback,
                                                      (void *)&connecting_peripheral_socket->bond_req);
                } else {
                    mico_bt_dev_pass_key_req_reply(MICO_BT_ERROR, p_event_data->user_passkey_request.bd_addr, 0);
                }
            }
            break;
        }

        case BTM_USER_CONFIRMATION_REQUEST_EVT: {
            bt_manager_log("User Confirm Request Event: dev[%02x:%02x:%02x:%02x:%02x:%02x], numeric_value[%lu], %s",
                           p_event_data->user_confirmation_request.bd_addr[0],
                           p_event_data->user_confirmation_request.bd_addr[1],
                           p_event_data->user_confirmation_request.bd_addr[2],
                           p_event_data->user_confirmation_request.bd_addr[3],
                           p_event_data->user_confirmation_request.bd_addr[4],
                           p_event_data->user_confirmation_request.bd_addr[5],
                           p_event_data->user_confirmation_request.numeric_value,
                           p_event_data->user_confirmation_request.just_works ? "Just works" : "Not Just works");

            if (connecting_socket
                && memcmp(p_event_data->user_confirmation_request.bd_addr,
                          connecting_socket->remote_device.address,
                          BD_ADDR_LEN) == 0) {
                if (connecting_socket->bonding_callback
                    && smartbridge_helper_socket_check_actions_enabled(connecting_socket, SOCKET_ACTION_INITIATE_PAIRING)) {
                    connecting_socket->bond_req.type = MICO_BT_SMART_BOND_USR_CONFIRM_REQ;
                    connecting_socket->bond_req.u.confirm.passkey = p_event_data->user_confirmation_request.numeric_value;
                    memcpy(connecting_socket->bond_req.u.confirm.addr, p_event_data->user_confirmation_request.bd_addr, 6);
                    mico_rtos_send_asynchronous_event(MICO_BT_EVT_WORKER_THREAD,
                                                      (event_handler_t)connecting_socket->bonding_callback,
                                                      (void *)&connecting_socket->bond_req);
                } else {
                    mico_bt_dev_confirm_req_reply(MICO_BT_ERROR, p_event_data->user_confirmation_request.bd_addr);
                }
            }

            if (connecting_peripheral_socket
                && memcmp(p_event_data->user_confirmation_request.bd_addr,
                          connecting_peripheral_socket->remote_device.address,
                          BD_ADDR_LEN) == 0) {
                if (connecting_peripheral_socket->bonding_callback
                    && peripheral_helper_socket_check_actions_enabled(connecting_peripheral_socket, SOCKET_ACTION_INITIATE_PAIRING)) {
                    connecting_peripheral_socket->bond_req.type = MICO_BT_SMART_BOND_USR_CONFIRM_REQ;
                    connecting_peripheral_socket->bond_req.u.confirm.passkey = p_event_data->user_confirmation_request.numeric_value;
                    memcpy(connecting_peripheral_socket->bond_req.u.confirm.addr, p_event_data->user_confirmation_request.bd_addr, 6);
                    mico_rtos_send_asynchronous_event(MICO_BT_EVT_WORKER_THREAD,
                                                      (event_handler_t)connecting_peripheral_socket->bonding_callback,
                                                      (void *)&connecting_peripheral_socket->bond_req);
                } else {
                    mico_bt_dev_confirm_req_reply(MICO_BT_ERROR, p_event_data->user_confirmation_request.bd_addr);
                }
            }
            break;
        }

#if (BTM_OOB_INCLUDED == TRUE)
        case BTM_SMP_REMOTE_OOB_DATA_REQUEST_EVT: {
            bt_manager_log( "OOB DATA Request event: dev[%02x:%02x:%02x:%02x:%02x:%02x]",
                            p_event_data->smp_remote_oob_data_request.bd_addr[0],
                            p_event_data->smp_remote_oob_data_request.bd_addr[1],
                            p_event_data->smp_remote_oob_data_request.bd_addr[2],
                            p_event_data->smp_remote_oob_data_request.bd_addr[3],
                            p_event_data->smp_remote_oob_data_request.bd_addr[4],
                            p_event_data->smp_remote_oob_data_request.bd_addr[5]);

            if (connecting_socket
                && memcmp(p_event_data->smp_remote_oob_data_request.bd_addr,
                          connecting_socket->remote_device.address,
                          BD_ADDR_LEN) == 0) {
                if (connecting_socket->bonding_callback
                    && smartbridge_helper_socket_check_actions_enabled(connecting_socket, SOCKET_ACTION_INITIATE_PAIRING)) {
                    connecting_socket->bond_req.type = MICO_BT_SMART_BOND_OOB_DATA_REQ;
                    memcpy(connecting_socket->bond_req.u.oob_data.addr, p_event_data->smp_remote_oob_data_request.bd_addr, 6);
                    mico_rtos_send_asynchronous_event(MICO_BT_EVT_WORKER_THREAD,
                                                      (event_handler_t)connecting_socket->bonding_callback,
                                                      (void *)&connecting_socket->bond_req);
                } else {
                    mico_bt_smp_oob_data_reply(p_event_data->smp_remote_oob_data_request.bd_addr, MICO_BT_ERROR, 0, NULL);
                }
            }

            if (connecting_peripheral_socket
                && memcmp(p_event_data->smp_remote_oob_data_request.bd_addr,
                          connecting_peripheral_socket->remote_device.address,
                          BD_ADDR_LEN) == 0) {
                if (connecting_peripheral_socket->bonding_callback
                    && peripheral_helper_socket_check_actions_enabled(connecting_peripheral_socket, SOCKET_ACTION_INITIATE_PAIRING)) {
                    connecting_peripheral_socket->bond_req.type = MICO_BT_SMART_BOND_OOB_DATA_REQ;
                    memcpy(connecting_peripheral_socket->bond_req.u.oob_data.addr, p_event_data->smp_remote_oob_data_request.bd_addr, 6);
                    mico_rtos_send_asynchronous_event(MICO_BT_EVT_WORKER_THREAD,
                                                      (event_handler_t)connecting_peripheral_socket->bonding_callback,
                                                      (void *)&connecting_peripheral_socket->bond_req);
                } else {
                    mico_bt_smp_oob_data_reply(p_event_data->smp_remote_oob_data_request.bd_addr, MICO_BT_ERROR, 0, NULL);
                }
            }
            break;
        }

        case BTM_SMP_SC_LOCAL_OOB_DATA_NOTIFICATION_EVT:
        case BTM_SMP_SC_REMOTE_OOB_DATA_REQUEST_EVT:
#endif
        default:
            bt_manager_log( "Unhandled Bluetooth Stack Callback event :%d", event );
            break;
    }

    return status;
}

/** mico_bt_smart_bond_reply
 *
 *  This function is called to reply BLE Bond Event posted by #mico_bt_smart_bonding_callback_t.
 *  You should fill a strcutre #mico_bt_smart_bond_reply_t to complete this bonding procedure.
 *
 *  @param[in] response     the response to current bonding event.
 *
 *  @return OSStatus
 */
OSStatus mico_bt_smart_bond_reply(const mico_bt_smart_bond_reply_t *response)
{
    OSStatus err = kNoErr;

    if (!response) {
        return kParamErr;
    }

    switch (response->type) {
        case MICO_BT_SMART_BOND_PASS_KEY_REQ:
            mico_bt_dev_pass_key_req_reply(response->res,
                                           (uint8_t *)response->u.passkey.addr,
                                           response->u.passkey.passkey);
            break;
        case MICO_BT_SMART_BOND_USR_CONFIRM_REQ:
            mico_bt_dev_confirm_req_reply(response->res, (uint8_t *)response->u.confirm.addr);
            break;
#if (BTM_OOB_INCLUDED == TRUE)
        case MICO_BT_SMART_BOND_OOB_DATA_REQ:
            mico_bt_smp_oob_data_reply((uint8_t *)response->u.oob_data.addr,
                                       response->res,
                                       response->u.oob_data.len,
                                       response->u.oob_data.data);
            break;
#endif
        default:
            err = kParamErr;
            break;
    }

    return err;
}
