#pragma once

/** @file
 *  Smartbridge's Helper Function Headers
 */

//#include "mico_utilities.h"
#include "mico_bt_smartbridge.h"
#include "mico_bt_smart_interface.h"

#include "mico_bt_peripheral.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
 *                      Macros
 ******************************************************/

//#define bt_smartbridge_log(M, ...) custom_log("Smartbridge", M, ##__VA_ARGS__)
#define bt_smartbridge_log(M, ...)

//#define bt_peripheral_log(M, ...) custom_log("Peripheral", M, ##__VA_ARGS__)
#define bt_peripheral_log(M, ...)

//#define bt_manager_log(M, ...) custom_log("Manager", M, ##__VA_ARGS__)
#define bt_manager_log(M, ...)

/******************************************************
 *                    Constants
 ******************************************************/

#define SOCKET_STATE_DISCONNECTED              ( 0 )
#define SOCKET_STATE_LINK_CONNECTED            ( 1 )
#define SOCKET_STATE_LINK_ENCRYPTED            ( 2 )
#define SOCKET_STATE_LINK_CONNECTING           ( 3 )

#define SOCKET_ACTION_HOST_CONNECT             ( 1 << 0 )
#define SOCKET_ACTION_HOST_DISCONNECT          ( 1 << 2 )
#define SOCKET_ACTION_INITIATE_PAIRING         ( 1 << 3 )
#define SOCKET_ACTION_ENCRYPT_USING_BOND_INFO  ( 1 << 4 )

/******************************************************
 *                   Enumerations
 ******************************************************/

/* GATT Feature Sub-Procedures (SPEC v4.0 Part G Section 4) */
typedef enum {
    GATT_SUBPROCEDURE_NONE,                         /* Default Value                                       */
    GATT_EXCHANGE_MTU,                              /* GATT Feature: Server Configuration                  */
    GATT_DISCOVER_ALL_PRIMARY_SERVICES,             /* GATT Feature: Primary Service Discovery             */
    GATT_DISCOVER_PRIMARY_SERVICE_BY_SERVICE_UUID,  /* GATT Feature: Primary Service Discovery             */
    GATT_FIND_INCLUDED_SERVICES,                    /* GATT Feature: Relationship Discovery                */
    GATT_DISCOVER_ALL_CHARACTERISTICS_OF_A_SERVICE, /* GATT Feature: Characteristic Discovery              */
    GATT_DISCOVER_CHARACTERISTIC_BY_UUID,           /* GATT Feature: Characteristic Discovery              */
    GATT_DISCOVER_ALL_CHARACTERISTICS_DESCRIPTORS,  /* GATT Feature: Characteristic Descriptor Discovery   */
    GATT_READ_CHARACTERISTIC_VALUE,                 /* GATT Feature: Characteristic Value Read             */
    GATT_READ_USING_CHARACTERISTIC_UUID,            /* GATT Feature: Characteristic Value Read             */
    GATT_READ_LONG_CHARACTERISTIC_VALUES,           /* GATT Feature: Characteristic Value Read             */
    GATT_READ_MULTIPLE_CHARACTERISTIC_VALUES,       /* GATT Feature: Characteristic Value Read             */
    GATT_WRITE_WITHOUT_RESPONSE,                    /* GATT Feature: Characteristic Value Write            */
    GATT_SIGNED_WRITE_WITHOUT_RESPONSE,             /* GATT Feature: Characteristic Value Write            */
    GATT_WRITE_CHARACTERISTIC_VALUE,                /* GATT Feature: Characteristic Value Write            */
    GATT_WRITE_LONG_CHARACTERISTIC_VALUE,           /* GATT Feature: Characteristic Value Write            */
    GATT_CHARACTERISTIC_VALUE_RELIABLE_WRITES,      /* GATT Feature: Characteristic Value Write            */
    GATT_NOTIFICATIONS,                             /* GATT Feature: Characteristic Value Notification     */
    GATT_INDICATIONS,                               /* GATT Feature: Characteristic Value Indication       */
    GATT_READ_CHARACTERISTIC_DESCRIPTORS,           /* GATT Feature: Characteristic Descriptor Value Read  */
    GATT_READ_LONG_CHARACTERISTIC_DESCRIPTORS,      /* GATT Feature: Characteristic Descriptor Value Read  */
    GATT_WRITE_CHARACTERISTIC_DESCRIPTORS,          /* GATT Feature: Characteristic Descriptor Value Write */
    GATT_WRITE_LONG_CHARACTERISTIC_DESCRIPTORS,     /* GATT Feature: Characteristic Descriptor Value Write */
} bt_smart_gatt_subprocedure_t;

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

typedef struct {
    bt_smart_gatt_subprocedure_t subprocedure;
    mico_mutex_t                 mutex;
    mico_semaphore_t             done_semaphore;
    OSStatus                     result;
    mico_bt_uuid_t               uuid;
    mico_bt_smart_attribute_t  *attr_head;
    mico_bt_smart_attribute_t  *attr_tail;
    uint32_t                     attr_count;
    uint16_t                     server_mtu;
    uint16_t                     start_handle;
    uint16_t                     end_handle;
    uint16_t                     length;
    uint16_t                     offset;
    //bt_smart_att_pdu_t*          pdu;
    uint16_t                     connection_handle;
} gatt_subprocedure_t;

typedef struct {
    mico_timer_t        timer;      // MiCO timer object
    mico_bool_t         is_started; // is it started?
    mico_bool_t         one_shot;   // one-shot?
    timer_handler_t     handler;    // timer expired handler
    void               *context;    // user context
} smartbridge_helper_timer_t;
/******************************************************
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

OSStatus  smartbridge_helper_delete_scan_result_list          ( void );
OSStatus  smartbridge_helper_add_scan_result_to_list          ( mico_bt_smart_scan_result_t *result );
OSStatus  smartbridge_helper_find_device_in_scan_result_list  ( mico_bt_device_address_t *address,
                                                                mico_bt_smart_address_type_t type,  mico_bt_smart_scan_result_t **result );
OSStatus  smartbridge_helper_get_scan_results                 ( mico_bt_smart_scan_result_t **result_list,
                                                                uint32_t *count );

mico_bool_t    smartbridge_helper_socket_check_actions_enabled      ( mico_bt_smartbridge_socket_t *socket,
                                                                      uint8_t action_bits );
mico_bool_t    smartbridge_helper_socket_check_actions_disabled     ( mico_bt_smartbridge_socket_t *socket,
                                                                      uint8_t action_bits );
void           smartbridge_helper_socket_set_actions               ( mico_bt_smartbridge_socket_t *socket,
                                                                     uint8_t action_bits );
void           smartbridge_helper_socket_clear_actions             ( mico_bt_smartbridge_socket_t *socket,
                                                                     uint8_t action_bits );



mico_bool_t    peripheral_helper_socket_check_actions_enabled      ( mico_bt_peripheral_socket_t *socket,
                                                                     uint8_t action_bits );
mico_bool_t    peripheral_helper_socket_check_actions_disabled     ( mico_bt_peripheral_socket_t *socket,
                                                                     uint8_t action_bits );
void           peripheral_helper_socket_set_actions                ( mico_bt_peripheral_socket_t *socket,
                                                                     uint8_t action_bits );
void           peripheral_helper_socket_clear_actions              ( mico_bt_peripheral_socket_t *socket,
                                                                     uint8_t action_bits );

OSStatus  subprocedure_notify_complete               ( gatt_subprocedure_t *subprocedure );
OSStatus  subprocedure_unlock                        ( gatt_subprocedure_t *subprocedure );
OSStatus  subprocedure_lock                          ( gatt_subprocedure_t *subprocedure );
OSStatus  subprocedure_reset                         ( gatt_subprocedure_t *subprocedure );
OSStatus  subprocedure_wait_for_completion           ( gatt_subprocedure_t *subprocedure );
OSStatus  subprocedure_wait_clear_semaphore          ( gatt_subprocedure_t *subprocedure );

/* This function is used to stop a active timer.
 * return MICO_TRUE if successful, otherwise, return MICO_FALSE
 */
mico_bool_t smartbridge_helper_timer_stop(smartbridge_helper_timer_t *timer);

/* This function is used to start or restart a timer.
 * The 'timer' will be restarted if it is active.
 */
mico_bool_t smartbridge_helper_timer_start(smartbridge_helper_timer_t *timer, mico_bool_t one_shot, uint32_t ms,
                                           timer_handler_t handler, void *arg);

#ifdef __cplusplus
} /* extern "C" */
#endif
