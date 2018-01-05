
/** @file
 *
 */
#include "mico.h"
#include "mico_bt_smartbridge.h"
#include "mico_bt_peripheral.h"

#include "mico_bt_gatt.h"
#include "mico_bt_ble.h"
#include "mico_bt_cfg.h"

#include "bt_smartbridge_socket_manager.h"
#include "bt_smartbridge_att_cache_manager.h"
#include "bt_smartbridge_helper.h"
#include "bt_smartbridge_stack_interface.h"



/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

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

static mico_bt_smart_scan_result_t                *scan_result_head        = NULL;
static mico_bt_smart_scan_result_t                *scan_result_tail        = NULL;
static uint32_t                                     scan_result_count       = 0;

/******************************************************
 *               Function Definitions
 ******************************************************/

OSStatus smartbridge_helper_get_scan_results( mico_bt_smart_scan_result_t **result_list, uint32_t *count )
{
    if ( smartbridge_bt_interface_is_scanning() == MICO_TRUE ) {
        bt_smartbridge_log("Can't Fetch Scan-Results [ Scan in-progress ? ]");
        return MICO_BT_SCAN_IN_PROGRESS;
    }

    *result_list = scan_result_head;
    *count       = scan_result_count;
    return MICO_BT_SUCCESS;
}

OSStatus smartbridge_helper_delete_scan_result_list( void )
{
    mico_bt_smart_scan_result_t *curr;

    if ( scan_result_count == 0 ) {
        return MICO_BT_LIST_EMPTY;
    }

    curr = scan_result_head;

    /* Traverse through the list and delete all attributes */
    while ( curr != NULL ) {
        /* Store pointer to next because curr is about to be deleted */
        mico_bt_smart_scan_result_t *next = curr->next;

        /* Detach result from the list and free memory */
        curr->next = NULL;
        free( curr );

        /* Update curr */
        curr = next;
    }

    scan_result_count = 0;
    scan_result_head  = NULL;
    scan_result_tail  = NULL;
    return MICO_BT_SUCCESS;
}

OSStatus smartbridge_helper_add_scan_result_to_list( mico_bt_smart_scan_result_t *result )
{
    if ( scan_result_count == 0 ) {
        scan_result_head = result;
        scan_result_tail = result;
    } else {
        scan_result_tail->next = result;
        scan_result_tail       = result;
    }

    scan_result_count++;
    //bt_smartbridge_log("New scan-result-count:%d", (int)scan_result_count);
    result->next = NULL;

    return MICO_BT_SUCCESS;
}

OSStatus smartbridge_helper_find_device_in_scan_result_list( mico_bt_device_address_t *address,
                                                             mico_bt_smart_address_type_t type,  mico_bt_smart_scan_result_t **result )
{
    mico_bt_smart_scan_result_t *iterator = scan_result_head;

    while ( iterator != NULL ) {
        if ( ( memcmp( &iterator->remote_device.address, address, sizeof( *address ) ) == 0 ) &&
             ( iterator->remote_device.address_type == type ) ) {
            *result = iterator;
            return MICO_BT_SUCCESS;
        }

        iterator = iterator->next;
    }

    return MICO_BT_ITEM_NOT_IN_LIST;
}

/******************************************************
 *            Socket Action Helper Functions
 ******************************************************/

mico_bool_t smartbridge_helper_socket_check_actions_enabled( mico_bt_smartbridge_socket_t *socket, uint8_t action_bits )
{
    return ( ( socket->actions & action_bits ) == action_bits ) ? MICO_TRUE : MICO_FALSE;
}

mico_bool_t smartbridge_helper_socket_check_actions_disabled( mico_bt_smartbridge_socket_t *socket,
                                                              uint8_t action_bits )
{
    return ( ( socket->actions | ~action_bits ) == ~action_bits ) ? MICO_TRUE : MICO_FALSE;
}

void smartbridge_helper_socket_set_actions( mico_bt_smartbridge_socket_t *socket, uint8_t action_bits )
{
    socket->actions |= action_bits;
}

void smartbridge_helper_socket_clear_actions( mico_bt_smartbridge_socket_t *socket, uint8_t action_bits )
{
    socket->actions &= ~action_bits;
}

mico_bool_t peripheral_helper_socket_check_actions_enabled( mico_bt_peripheral_socket_t *socket, uint8_t action_bits )
{
    return ( ( socket->actions & action_bits ) == action_bits ) ? MICO_TRUE : MICO_FALSE;
}

mico_bool_t peripheral_helper_socket_check_actions_disabled( mico_bt_peripheral_socket_t *socket, uint8_t action_bits )
{
    return ( ( socket->actions | ~action_bits ) == ~action_bits ) ? MICO_TRUE : MICO_FALSE;
}

void peripheral_helper_socket_set_actions( mico_bt_peripheral_socket_t *socket, uint8_t action_bits )
{
    socket->actions |= action_bits;
}

void peripheral_helper_socket_clear_actions( mico_bt_peripheral_socket_t *socket, uint8_t action_bits )
{
    socket->actions &= ~action_bits;
}

/******************************************************
 *         GATT/GAP subprocedure Functions
 ******************************************************/

#define GATT_MAX_PROCEDURE_TIMEOUT (10000)

OSStatus subprocedure_lock( gatt_subprocedure_t *subprocedure )
{
    return mico_rtos_lock_mutex( &subprocedure->mutex );
}

OSStatus subprocedure_unlock( gatt_subprocedure_t *subprocedure )
{
    return mico_rtos_unlock_mutex( &subprocedure->mutex );
}

OSStatus subprocedure_reset( gatt_subprocedure_t *subprocedure )
{
    subprocedure->subprocedure      = GATT_SUBPROCEDURE_NONE;
    subprocedure->attr_head         = NULL;
    subprocedure->attr_tail         = NULL;
    subprocedure->attr_count        = 0;
    subprocedure->result            = MICO_BT_SUCCESS;
    subprocedure->start_handle      = 0;
    subprocedure->end_handle        = 0;
    //subprocedure.pdu               = 0;
    subprocedure->length            = 0;
    subprocedure->offset            = 0;
    subprocedure->connection_handle = 0;
    memset( &subprocedure->uuid, 0, sizeof( subprocedure->uuid ) );
    subprocedure_wait_clear_semaphore( subprocedure );
    return MICO_BT_SUCCESS;
}

OSStatus subprocedure_wait_for_completion( gatt_subprocedure_t *subprocedure )
{
    if ( kNoErr != mico_rtos_get_semaphore( &subprocedure->done_semaphore, GATT_MAX_PROCEDURE_TIMEOUT ) ) {
        subprocedure->result = MICO_BT_TIMEOUT;
    }
    return subprocedure->result;
}

OSStatus subprocedure_wait_clear_semaphore( gatt_subprocedure_t *subprocedure )
{
    while ( mico_rtos_get_semaphore( &subprocedure->done_semaphore, MICO_NO_WAIT ) == kNoErr ) {
    }
    return MICO_BT_SUCCESS;
}

OSStatus subprocedure_notify_complete( gatt_subprocedure_t *subprocedure )
{
    return mico_rtos_set_semaphore( &subprocedure->done_semaphore );
}

/******************************************************
 *         Smartbridge timer Functions
 ******************************************************/

/* Smartbridge timer handler */
static void smartbridge_helper_timer_handler(void *arg)
{
    smartbridge_helper_timer_t *timer = (smartbridge_helper_timer_t *)arg;

    bt_smartbridge_log("smartbridge timer expired.");

    /* Call user timer handler */
    timer->handler(timer->context);

    /* Reload if not one-shot timer. */
    if (timer->is_started && !timer->one_shot) {
        bt_smartbridge_log("smartbridge timer reload");
        mico_rtos_reload_timer(&timer->timer);
    }
}

/* This function is used to stop a active timer.
 * return MICO_TRUE if successful, otherwise, return MICO_FALSE.
 */
mico_bool_t smartbridge_helper_timer_stop(smartbridge_helper_timer_t *timer)
{
    if (timer == (void *)0) {
        return MICO_FALSE;
    }

    if (!timer->is_started) {
        return MICO_FALSE;
    }

    if (mico_rtos_is_timer_running(&timer->timer)) {
        mico_rtos_stop_timer(&timer->timer);
    }

    mico_rtos_deinit_timer(&timer->timer);

    timer->is_started = MICO_FALSE;
    timer->one_shot = MICO_FALSE;

    return MICO_TRUE;
}

/* This function is used to start or restart a timer.
 * The 'timer' will be restarted if it is active.
 */
mico_bool_t smartbridge_helper_timer_start(smartbridge_helper_timer_t *timer, mico_bool_t one_shot, uint32_t ms,
                                           timer_handler_t handler, void *arg)
{
    if (timer == (void *)0) {
        return MICO_FALSE;
    }

    if (timer->is_started) {
        smartbridge_helper_timer_stop(timer);
    }

    if (mico_rtos_init_timer(&timer->timer, ms, smartbridge_helper_timer_handler, (void *)timer) != kNoErr) {
        return MICO_FALSE;
    }

    if (mico_rtos_start_timer(&timer->timer) != kNoErr) {
        mico_rtos_deinit_timer(&timer->timer);
        return MICO_FALSE;
    }

    timer->is_started = MICO_TRUE;
    timer->one_shot = one_shot;
    timer->handler = handler;
    timer->context = arg;

    return MICO_TRUE;
}
