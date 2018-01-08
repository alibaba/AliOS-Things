
/** @file
 *
 */

#include "mico.h"
#include "LinkListUtils.h"
#include "mico_bt_smartbridge.h"
#include "mico_bt_smart_interface.h"
#include "bt_smartbridge_stack_interface.h"
#include "bt_smartbridge_helper.h"
#include "bt_smartbridge_att_cache_manager.h"


/******************************************************
 *                      Macros
 ******************************************************/

#define CHECK_FOR_ERROR( condition, error_code ) \
do \
{ \
    if ( ( condition ) ) \
    { \
        bt_smartbridge_log( "[Att Cache] Error: %d",  (int)error_code); \
        error_code_var = (error_code); \
        goto error; \
    } \
} \
while (0)

#define CALCULATE_ATT_CACHE_MANAGER_SIZE( cache_count ) \
sizeof( bt_smartbridge_att_cache_manager_t ) + \
sizeof( bt_smartbridge_att_cache_t ) * ( cache_count - 1 )

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

typedef struct bt_smartbridge_att_cache {
    linked_list_node_t              node;
    mico_bool_t                     is_active;
    mico_bool_t                     is_discovering;
    mico_bt_smart_device_t          remote_device;
    uint16_t                        connection_handle;
    mico_bt_smart_attribute_list_t  attribute_list;
    mico_mutex_t                    mutex;
} bt_smartbridge_att_cache_t;

typedef struct {
    uint32_t                   count;
    mico_bt_uuid_t            *att_cache_services;
    uint32_t                   att_cache_services_count;
    linked_list_t              free_list;
    linked_list_t              used_list;
    mico_mutex_t               mutex;
    bt_smartbridge_att_cache_t pool[1];
} bt_smartbridge_att_cache_manager_t;

/******************************************************
 *               Static Function Declarations
 ******************************************************/

static OSStatus smartbridge_att_cache_get_free_cache          ( bt_smartbridge_att_cache_t **free_cache );
static OSStatus smartbridge_att_cache_insert_to_used_list     ( bt_smartbridge_att_cache_t *instance );
static OSStatus smartbridge_att_cache_return_to_free_list     ( bt_smartbridge_att_cache_t *instance );
static OSStatus smartbridge_att_cache_discover_all            ( bt_smartbridge_att_cache_t *cache,
                                                                uint16_t connection_handle );
static bool     smartbridge_att_cache_find_by_device_callback ( linked_list_node_t *node_to_compare, void *user_data );
static bool     smartbridge_att_cache_get_free_callback       ( linked_list_node_t *node_to_compare, void *user_data );

/******************************************************
 *               Variable Definitions
 ******************************************************/

/* Attribute Caching Management Globals */

static bt_smartbridge_att_cache_manager_t *att_cache_manager = NULL;

/******************************************************
 *               Function Definitions
 ******************************************************/

OSStatus bt_smartbridge_att_cache_enable( uint32_t cache_count, mico_bt_uuid_t cache_services[],
                                          uint32_t service_count )
{
    uint32_t a;
    OSStatus result;
    bt_smartbridge_att_cache_manager_t *manager;
    mico_bt_uuid_t *services = NULL;

    if ( att_cache_manager != NULL ) {
        return MICO_BT_SUCCESS;
    }

    manager = (bt_smartbridge_att_cache_manager_t *)malloc_named( "att_cache",
                                                                  CALCULATE_ATT_CACHE_MANAGER_SIZE( cache_count ) );
    if ( manager == NULL ) {
        return MICO_BT_OUT_OF_HEAP_SPACE;
    }

    if ( service_count != 0 ) {
        services = (mico_bt_uuid_t *)malloc_named( "cache_services", service_count * sizeof(mico_bt_uuid_t) );
        if ( services == NULL ) {
            return MICO_BT_OUT_OF_HEAP_SPACE;
        }
    }


    memset( manager, 0, CALCULATE_ATT_CACHE_MANAGER_SIZE( cache_count ) );

    att_cache_manager = manager;
    manager->count    = cache_count;
    manager->att_cache_services_count = service_count;
    if ( service_count != 0 ) {
        manager->att_cache_services = services;
        memcpy( manager->att_cache_services, cache_services, service_count * sizeof(mico_bt_uuid_t) );
    }


    result = linked_list_init( &manager->free_list );
    if ( result != MICO_BT_SUCCESS ) {
        bt_smartbridge_log( "Error creating linked list\n" );
        goto error;
    }

    result = linked_list_init( &manager->used_list );
    if ( result != MICO_BT_SUCCESS ) {
        bt_smartbridge_log( "Error creating linked list\n" );
        goto error;
    }

    result = mico_rtos_init_mutex( &manager->mutex );
    if ( result != MICO_BT_SUCCESS ) {
        bt_smartbridge_log( "Error creating mutex\n" );
        goto error;
    }

    /* Initialise mutexes for protecting access to cached attributes */
    for ( a = 0; a < manager->count; a++ ) {
        result = mico_rtos_init_mutex( &manager->pool[a].mutex );
        if ( result != MICO_BT_SUCCESS ) {
            goto error;
        }

        /* Point node data to cached attribute instance */
        manager->pool[a].node.data = (void *)&manager->pool[a];

        /* Insert cached attribute instance into free list */
        result = linked_list_insert_node_at_rear( &manager->free_list, &manager->pool[a].node );
        if ( result != MICO_BT_SUCCESS ) {
            goto error;
        }
    }

    return MICO_BT_SUCCESS;

error:
    bt_smartbridge_att_cache_disable();
    return result;
}

OSStatus bt_smartbridge_att_cache_disable( void )
{
    uint32_t a;
    linked_list_node_t *node = NULL;
    bt_smartbridge_att_cache_manager_t *manager = att_cache_manager;

    if ( att_cache_manager == NULL ) {
        return MICO_BT_SUCCESS;
    }

    /* Set status at the beginning to prevent cached attributes being used even when deinitialisation failed */
    att_cache_manager = NULL;

    while ( linked_list_remove_node_from_front( &manager->free_list, &node ) == MICO_BT_SUCCESS ) {
        bt_smartbridge_att_cache_t *cache = (bt_smartbridge_att_cache_t *)node->data;

        mico_bt_smart_attribute_delete_list( &cache->attribute_list );
    }

    linked_list_deinit( &manager->free_list );

    while ( linked_list_remove_node_from_front( &manager->used_list, &node ) == MICO_BT_SUCCESS ) {
        bt_smartbridge_att_cache_t *cache = (bt_smartbridge_att_cache_t *)node->data;

        mico_bt_smart_attribute_delete_list( &cache->attribute_list );
    }

    linked_list_deinit( &manager->used_list );
    mico_rtos_deinit_mutex( &manager->mutex );

    /* Deinitialise mutexes for protecting access to cached attributes */
    for ( a = 0; a < manager->count; a++ ) {
        mico_rtos_deinit_mutex( &manager->pool[a].mutex );
    }

    if ( manager->att_cache_services_count != 0 ) {
        free( manager->att_cache_services );
    }

    memset( manager, 0, CALCULATE_ATT_CACHE_MANAGER_SIZE( manager->count ) );
    free( manager );

    return MICO_BT_SUCCESS;
}

OSStatus bt_smartbridge_att_cache_get_list( bt_smartbridge_att_cache_t *cache, mico_bt_smart_attribute_list_t **list )
{
    if ( cache == NULL || list == NULL ) {
        return MICO_BT_BADARG;
    }

    if ( att_cache_manager == NULL ) {
        return MICO_BT_ATT_CACHE_UNINITIALISED;
    }

    *list = &cache->attribute_list;
    return MICO_BT_SUCCESS;
}

mico_bool_t   bt_smartbridge_att_cache_is_enabled( void )
{
    return ( att_cache_manager == NULL ) ? MICO_FALSE : MICO_TRUE;
}

mico_bool_t   bt_smartbridge_att_cache_is_discovering( const bt_smartbridge_att_cache_t *cache )
{
    if ( cache == NULL || att_cache_manager == NULL ) {
        return MICO_FALSE;
    }

    return cache->is_discovering;
}

mico_bool_t   bt_smartbridge_att_cache_get_active_state( const bt_smartbridge_att_cache_t *cache )
{
    if ( cache == NULL || att_cache_manager == NULL ) {
        return MICO_FALSE;
    }

    return cache->is_active;
}

OSStatus bt_smartbridge_att_cache_set_active_state( bt_smartbridge_att_cache_t *cache, mico_bool_t is_active )
{
    if ( cache == NULL ) {
        return MICO_BT_BADARG;
    }

    if ( att_cache_manager == NULL ) {
        return MICO_BT_ATT_CACHE_UNINITIALISED;
    }

    cache->is_active = is_active;
    return MICO_BT_SUCCESS;
}

OSStatus bt_smartbridge_att_cache_lock( bt_smartbridge_att_cache_t *cache )
{
    if ( cache == NULL ) {
        return MICO_BT_BADARG;
    }

    if ( att_cache_manager == NULL ) {
        return MICO_BT_ATT_CACHE_UNINITIALISED;
    }

    return mico_rtos_lock_mutex( &cache->mutex );
}

OSStatus bt_smartbridge_att_cache_unlock( bt_smartbridge_att_cache_t *cache )
{
    if ( cache == NULL ) {
        return MICO_BT_BADARG;
    }

    if ( att_cache_manager == NULL ) {
        return MICO_BT_ATT_CACHE_UNINITIALISED;
    }

    return mico_rtos_unlock_mutex( &cache->mutex );
}

OSStatus bt_smartbridge_att_cache_find( const mico_bt_smart_device_t *remote_device,
                                        bt_smartbridge_att_cache_t **cache )
{
    OSStatus      result;
    linked_list_node_t *node_found;

    if ( remote_device == NULL || cache == NULL ) {
        return MICO_BT_BADARG;
    }

    if ( att_cache_manager == NULL ) {
        return MICO_BT_ATT_CACHE_UNINITIALISED;
    }

    /* Lock protection */
    result = mico_rtos_lock_mutex( &att_cache_manager->mutex );
    if ( result != MICO_BT_SUCCESS ) {
        return result;
    }

    result = linked_list_find_node( &att_cache_manager->used_list, smartbridge_att_cache_find_by_device_callback,
                                    (void *)remote_device, &node_found );
    if ( result == MICO_BT_SUCCESS ) {
        *cache = (bt_smartbridge_att_cache_t *)node_found->data;
    }

    /* Unlock protection */
    mico_rtos_unlock_mutex( &att_cache_manager->mutex );
    return result;
}

OSStatus bt_smartbridge_att_cache_generate( const mico_bt_smart_device_t *remote_device, uint16_t connection_handle,
                                            bt_smartbridge_att_cache_t **cache )
{
    bt_smartbridge_att_cache_t *new_cache = NULL;
    OSStatus              result;

    if ( cache == NULL ) {
        return MICO_BT_BADARG;
    }

    if ( att_cache_manager == NULL ) {
        return MICO_BT_ATT_CACHE_UNINITIALISED;
    }

    /* Cached attributes not found. Get a free instance and discover services */
    result = smartbridge_att_cache_get_free_cache( &new_cache );
    if ( result != MICO_BT_SUCCESS ) {
        return result;
    }

    /* Copy remote device to cache */
    mico_rtos_lock_mutex( &new_cache->mutex );
    memcpy( &new_cache->remote_device, remote_device, sizeof( new_cache->remote_device ) );
    new_cache->connection_handle = connection_handle;
    new_cache->is_discovering    = MICO_TRUE;
    mico_rtos_unlock_mutex( &new_cache->mutex );

    /* Rediscover services */
    result = smartbridge_att_cache_discover_all( new_cache, new_cache->connection_handle );
    mico_rtos_lock_mutex( &new_cache->mutex );
    new_cache->is_discovering = MICO_FALSE;
    mico_rtos_unlock_mutex( &new_cache->mutex );

    if ( result == MICO_BT_SUCCESS ) {
        result = smartbridge_att_cache_insert_to_used_list( new_cache );

        if ( result == MICO_BT_SUCCESS ) {
            *cache = new_cache;
        }
    } else {
        smartbridge_att_cache_return_to_free_list( new_cache );
    }

    return result;
}


OSStatus bt_smartbridge_att_cache_release( bt_smartbridge_att_cache_t *cache )
{
    OSStatus            result;

    if ( att_cache_manager == NULL ) {
        return MICO_BT_ATT_CACHE_UNINITIALISED;
    }

    /* Lock protection */
    result = mico_rtos_lock_mutex( &att_cache_manager->mutex );
    if ( result != MICO_BT_SUCCESS ) {
        return result;
    }

    result = linked_list_remove_node( &att_cache_manager->used_list, &cache->node );
    if ( result == MICO_BT_SUCCESS ) {
        /* Delete list and set data to NULL */
        mico_bt_smart_attribute_delete_list( &cache->attribute_list );
    }

    smartbridge_att_cache_return_to_free_list( cache );

    /* Unlock protection */
    mico_rtos_unlock_mutex( &att_cache_manager->mutex );

    return result;
}


static OSStatus smartbridge_att_cache_get_free_cache( bt_smartbridge_att_cache_t **free_cache )
{
    OSStatus            result;
    linked_list_node_t *node;

    if ( att_cache_manager == NULL ) {
        return MICO_BT_ATT_CACHE_UNINITIALISED;
    }

    /* Lock protection */
    result = mico_rtos_lock_mutex( &att_cache_manager->mutex );
    if ( result != kNoErr ) {
        return result;
    }

    /* Remove from front of free list */
    result = linked_list_remove_node_from_front( &att_cache_manager->free_list, &node );

    /* Free list is empty. Remove the oldest one from used list */
    if ( result != MICO_BT_SUCCESS ) {
        result = linked_list_find_node( &att_cache_manager->used_list, smartbridge_att_cache_get_free_callback, NULL, &node );
        if ( result == MICO_BT_SUCCESS ) {
            result = linked_list_remove_node( &att_cache_manager->used_list, node );
            if ( result == MICO_BT_SUCCESS ) {
                mico_bt_smart_attribute_list_t *list = (mico_bt_smart_attribute_list_t *)node->data;

                /* Delete list and set data to NULL */
                mico_bt_smart_attribute_delete_list( list );
            }
        }
    }

    if ( result == MICO_BT_SUCCESS ) {
        *free_cache = (bt_smartbridge_att_cache_t *)node->data;
    }

    /* Unlock protection */
    mico_rtos_unlock_mutex( &att_cache_manager->mutex );

    return result;
}

static OSStatus smartbridge_att_cache_insert_to_used_list( bt_smartbridge_att_cache_t *cache )
{
    OSStatus  result;

    if ( att_cache_manager == NULL ) {
        return MICO_BT_ATT_CACHE_UNINITIALISED;
    }

    /* Lock protection */
    result = mico_rtos_lock_mutex( &att_cache_manager->mutex );
    if ( result != MICO_BT_SUCCESS ) {
        return result;
    }

    result = linked_list_insert_node_at_rear( &att_cache_manager->used_list, &cache->node );

    /* Unlock protection */
    mico_rtos_unlock_mutex( &att_cache_manager->mutex );

    return result;
}

static OSStatus smartbridge_att_cache_return_to_free_list( bt_smartbridge_att_cache_t *cache )
{
    OSStatus result;

    if ( att_cache_manager == NULL ) {
        return MICO_BT_ATT_CACHE_UNINITIALISED;
    }

    /* Lock protection */
    result = mico_rtos_lock_mutex( &att_cache_manager->mutex );
    if ( result != MICO_BT_SUCCESS ) {
        return result;
    }

    result = linked_list_insert_node_at_rear( &att_cache_manager->free_list, &cache->node );

    /* Unlock protection */
    mico_rtos_unlock_mutex( &att_cache_manager->mutex );

    return result;
}

static OSStatus smartbridge_att_cache_discover_all( bt_smartbridge_att_cache_t *cache, uint16_t connection_handle )
{
    /* This function performs the following:
     * 1. Primary Services discovery
     * 2. Relationship (Included Services) Discovery for every Primary Service
     * 3. Characteristic Discovery for every Primary Service
     * 4. Characteristic Value Read for every Charactertistic
     * 5. Characteristic Descriptor Discovery for every Characteristic
     */

    mico_bt_smart_attribute_t     **primary_service_array    = NULL;
    mico_bt_smart_attribute_t     **characteristic_array     = NULL;
    mico_bt_smart_attribute_t      *characteristic_value     = NULL;
    mico_bt_smart_attribute_t      *descriptor_with_no_value = NULL;
    mico_bt_smart_attribute_t      *descriptor_with_value    = NULL;
    mico_bt_smart_attribute_t      *iterator                 = NULL;
    OSStatus                         result                   = MICO_BT_SUCCESS;
    OSStatus                         error_code_var           = MICO_BT_ERROR;
    uint32_t                         i                        = 0;
    uint32_t                         j                        = 0;
    uint32_t                         primary_service_count    = 0;
    uint32_t                         characteristic_count     = 0;
    mico_bt_smart_attribute_list_t  primary_service_list;
    mico_bt_smart_attribute_list_t  branch_primary_service_list;
    mico_bt_smart_attribute_list_t  included_service_list;
    mico_bt_smart_attribute_list_t  characteristic_list;
    mico_bt_smart_attribute_list_t  descriptor_list;

    mico_bool_t characteristic_list_merged = MICO_FALSE;
    mico_bool_t included_service_list_merged = MICO_FALSE;

    if ( att_cache_manager == NULL ) {
        return MICO_BT_ATT_CACHE_UNINITIALISED;
    }

    /* Initialise local variables */
    memset( &primary_service_list,          0, sizeof( primary_service_list  ) );
    memset( &branch_primary_service_list,   0, sizeof( primary_service_list  ) );
    memset( &included_service_list,         0, sizeof( included_service_list ) );
    memset( &characteristic_list,           0, sizeof( characteristic_list   ) );
    memset( &descriptor_list,               0, sizeof( descriptor_list       ) );

    mico_rtos_lock_mutex( &cache->mutex );

    result = mico_bt_smart_attribute_create_list( &primary_service_list );

    mico_rtos_unlock_mutex( &cache->mutex );

    CHECK_FOR_ERROR( result != MICO_BT_SUCCESS, result );

    /**************************************************************************
     * Primary Services Discovery
     **************************************************************************/
    if ( att_cache_manager->att_cache_services_count == 0 ) {
        result = smartbridge_bt_interface_discover_all_primary_services( connection_handle, &primary_service_list );
        CHECK_FOR_ERROR( result != MICO_BT_SUCCESS, result );
    } else {
        for ( i = 0; i < att_cache_manager->att_cache_services_count; i++ ) {
            result = smartbridge_bt_interface_discover_primary_services_by_uuid( connection_handle,
                                                                                 &att_cache_manager->att_cache_services[i], &branch_primary_service_list );
            CHECK_FOR_ERROR( result != MICO_BT_SUCCESS, result );
            result = mico_bt_smart_attribute_merge_lists( &primary_service_list, &branch_primary_service_list );
            CHECK_FOR_ERROR( result != MICO_BT_SUCCESS, result );
        }
    }

    mico_bt_smart_attribute_get_list_count( &primary_service_list, &primary_service_count );

    primary_service_array = (mico_bt_smart_attribute_t **)malloc_named( "svc_array",
                                                                        primary_service_count * sizeof(mico_bt_smart_attribute_t));

    CHECK_FOR_ERROR( primary_service_array == NULL, MICO_BT_OUT_OF_HEAP_SPACE );

    /* Keep the original pointers to the primary service list before the list gets merged */
    mico_bt_smart_attribute_get_list_head( &primary_service_list, &iterator );

    for ( i = 0; i < primary_service_count; i++ ) {
        primary_service_array[i] = iterator;
        iterator                 = iterator->next;
    }

    bt_smartbridge_log( "[Cache] All services discovered. count: %u", (unsigned int) primary_service_count );

    /* Check if characteristic is readable. If not readable, create a control-point attribute */
    for ( i = 0; i < primary_service_count; i++ ) {
        /* Initialise variable for this iteration */
        memset( &characteristic_list,   0, sizeof( characteristic_list   ) );
        memset( &included_service_list, 0, sizeof( included_service_list ) );
        characteristic_array = NULL;

        /**********************************************************************
         * Relationship Discovery
         **********************************************************************/
        /*result = bt_smart_gatt_find_included_services( connection_handle, primary_service_array[i]->value.service.start_handle, primary_service_array[i]->value.service.start_handle, &included_service_list );

        CHECK_FOR_ERROR( result == MICO_BT_GATT_TIMEOUT, result );

        mico_rtos_lock_mutex( &cache->mutex );

        result = mico_bt_smart_attribute_merge_lists( &primary_service_list, &included_service_list );

        if( result == MICO_BT_SUCCES )
        {
            included_service_list_merged = MICO_TRUE;
        }

        mico_rtos_unlock_mutex( &cache->mutex );

        CHECK_FOR_ERROR( result != MICO_BT_SUCCESS, result );
        */

        /**********************************************************************
         * Characteristic Discovery
         **********************************************************************/
        result = smartbridge_bt_interface_discover_all_characteristics_in_a_service( connection_handle,
                                                                                     primary_service_array[i]->value.service.start_handle, primary_service_array[i]->value.service.end_handle,
                                                                                     &characteristic_list );

        CHECK_FOR_ERROR( result == MICO_BT_GATT_TIMEOUT || result == MICO_BT_TIMEOUT, result );

        mico_bt_smart_attribute_get_list_count( &characteristic_list, &characteristic_count );


        characteristic_array = (mico_bt_smart_attribute_t **)malloc_named( "char_array",
                                                                           characteristic_count * sizeof(characteristic_list));

        CHECK_FOR_ERROR( characteristic_array == NULL, MICO_BT_OUT_OF_HEAP_SPACE );

        /* Keep the original pointers to the characteristic list before the list gets merged. */
        mico_bt_smart_attribute_get_list_head( &characteristic_list, &iterator );

        for ( j = 0; j < characteristic_count; j++ ) {
            characteristic_array[j] = iterator;
            iterator                = iterator->next;
        }

        /* Traverse through all characteristics to perform Characteristic Value Read and Descriptors Discovery */
        for ( j = 0; j < characteristic_count; j++ ) {
            /* Initialise local variables for this iteration */
            memset( &descriptor_list, 0, sizeof( descriptor_list ) );
            characteristic_value = NULL;

            /******************************************************************
             * Characteristic Value Read
             ******************************************************************/
            if ( ( characteristic_array[j]->value.characteristic.properties & 0x02 ) != 0 ) {
                bt_smartbridge_log( "[Cache] Characteristic Read-Property IS set" );
                /* If characteristic is readable. If not readable, create a control-point attribute */
                result = smartbridge_bt_interface_read_characteristic_value( connection_handle,
                                                                             characteristic_array[j]->value.characteristic.value_handle, &characteristic_array[j]->value.characteristic.uuid,
                                                                             &characteristic_value );

                CHECK_FOR_ERROR( result == MICO_BT_GATT_TIMEOUT || result == MICO_BT_TIMEOUT, result );
            } else {
                bt_smartbridge_log( "[Cache] Characteristic Read-Properties is NOT set" );
                /* Failed to read. Let's enter a control-point attribute (dummy) here so when notification come, UUID is known. */
                result = mico_bt_smart_attribute_create( &characteristic_value, MICO_ATTRIBUTE_TYPE_NO_VALUE, 0 );

                if ( result == MICO_BT_SUCCESS ) {
                    characteristic_value->handle = characteristic_array[j]->value.characteristic.value_handle;
                    characteristic_value->type   = characteristic_array[j]->value.characteristic.uuid;
                }

                CHECK_FOR_ERROR( result != MICO_BT_SUCCESS, result );
            }

            if ( characteristic_value != NULL ) {
                /* Add Characteristic Value to main list */
                mico_rtos_lock_mutex( &cache->mutex );

                /* Add characteristic_value to main list */
                result = mico_bt_smart_attribute_add_to_list( &primary_service_list, characteristic_value );

                /* Merge success, it will be deleted by primary_service_list when error */
                if ( result == MICO_BT_SUCCESS ) {
                    characteristic_value = NULL;
                }

                mico_rtos_unlock_mutex( &cache->mutex );

                CHECK_FOR_ERROR( result != MICO_BT_SUCCESS, result );
            }
            /******************************************************************
             * Characteristic Descriptor Discovery
             ******************************************************************/

            /* Check whether the Descriptor Handle is valid or not. Add by zhj */
            if (j < characteristic_count - 1) {
                if ((characteristic_array[j]->value.characteristic.descriptor_start_handle <=
                     characteristic_array[j]->value.characteristic.value_handle)
                    || (characteristic_array[j]->value.characteristic.descriptor_end_handle >= characteristic_array[j + 1]->handle)) {
                    continue;
                }
            } else {
                if ((characteristic_array[j]->value.characteristic.descriptor_start_handle <=
                     characteristic_array[j]->value.characteristic.value_handle)
                    || (characteristic_array[j]->value.characteristic.descriptor_end_handle >
                        primary_service_array[i]->value.service.end_handle)) {
                    continue;
                }
            }


            if ( characteristic_array[j]->value.characteristic.descriptor_start_handle <=
                 characteristic_array[j]->value.characteristic.descriptor_end_handle ) {
                result = smartbridge_bt_interface_discover_all_characteristic_descriptors( connection_handle,
                                                                                           characteristic_array[j]->value.characteristic.descriptor_start_handle,
                                                                                           characteristic_array[j]->value.characteristic.descriptor_end_handle, &descriptor_list );

                CHECK_FOR_ERROR( result == MICO_BT_GATT_TIMEOUT || result == MICO_BT_TIMEOUT, result );

                mico_bt_smart_attribute_get_list_head( &descriptor_list, &descriptor_with_no_value );

                /* Traverse through all descriptors */
                while ( descriptor_with_no_value != NULL ) {
                    /* Initialise variable for this iteration */
                    descriptor_with_value = NULL;

                    result = smartbridge_bt_interface_read_characteristic_descriptor( connection_handle, descriptor_with_no_value->handle,
                                                                                      &descriptor_with_no_value->type, &descriptor_with_value );

                    CHECK_FOR_ERROR( result == MICO_BT_GATT_TIMEOUT || result == MICO_BT_TIMEOUT, result );

                    /* Add Descriptor with Value to main list */
                    result = mico_bt_smart_attribute_add_to_list( &primary_service_list, descriptor_with_value );

                    /* Merge success, it will be deleted by primary_service_list when error */
                    if ( result == MICO_BT_SUCCESS ) {
                        descriptor_with_value = NULL;
                    }

                    CHECK_FOR_ERROR( result != MICO_BT_SUCCESS, result );

                    descriptor_with_no_value = descriptor_with_no_value->next;
                }

                /* Delete the empty descriptor list */
                result = mico_bt_smart_attribute_delete_list( &descriptor_list );

                CHECK_FOR_ERROR( result != MICO_BT_SUCCESS, result );
            }
        }

        /* Merge Characteristics to main list */
        mico_rtos_lock_mutex( &cache->mutex );

        result = mico_bt_smart_attribute_merge_lists( &primary_service_list, &characteristic_list );

        if ( result == MICO_BT_SUCCESS ) {
            characteristic_list_merged = MICO_TRUE;
        }

        mico_rtos_unlock_mutex( &cache->mutex );

        CHECK_FOR_ERROR( result != MICO_BT_SUCCESS, result );

        /* Free primary service array */
        free( characteristic_array );
    }

    /* Free primary service array */
    free( primary_service_array );

    /* Successful. Now copy the primary service list to the cached attributes list */
    memcpy( &cache->attribute_list, &primary_service_list, sizeof( cache->attribute_list ) );

    //mico_bt_smart_attribute_print_list( &(cache->attribute_list) );

    return MICO_BT_SUCCESS;

error:


    /* Delete all local attributes */

    if ( descriptor_with_value != NULL ) {
        mico_bt_smart_attribute_delete( descriptor_with_value );
    }

    if ( characteristic_value != NULL ) {
        mico_bt_smart_attribute_delete( characteristic_value );
    }

    if ( characteristic_array != NULL ) {
        free( characteristic_array );
    }

    if ( primary_service_array != NULL ) {
        free( primary_service_array );
    }

    if ( characteristic_list_merged == MICO_FALSE ) {
        mico_bt_smart_attribute_delete_list( &characteristic_list );
    }

    if ( included_service_list_merged == MICO_FALSE ) {
        mico_bt_smart_attribute_delete_list( &included_service_list );
    }

    mico_bt_smart_attribute_delete_list( &descriptor_list );
    mico_bt_smart_attribute_delete_list( &primary_service_list );

    /* Return the error code to the caller */
    return error_code_var;
}

static bool   smartbridge_att_cache_find_by_device_callback( linked_list_node_t *node_to_compare, void *user_data )
{
    bt_smartbridge_att_cache_t *cached_attributes = (bt_smartbridge_att_cache_t *)node_to_compare->data;
    mico_bt_smart_device_t    *remote_device     = (mico_bt_smart_device_t *)user_data;

    return ( ( memcmp( cached_attributes->remote_device.address, remote_device->address,
                       sizeof( remote_device->address ) ) == 0 ) &&
             ( cached_attributes->remote_device.address_type == remote_device->address_type) ) ? true : false;
}

static bool   smartbridge_att_cache_get_free_callback( linked_list_node_t *node_to_compare, void *user_data )
{
    bt_smartbridge_att_cache_t *cache = (bt_smartbridge_att_cache_t *)node_to_compare->data;

    return ( cache->is_active == MICO_FALSE ) ? true : false;
}
