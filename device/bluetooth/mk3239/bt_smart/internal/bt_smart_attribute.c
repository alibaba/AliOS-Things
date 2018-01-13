/** @file
 *
 */

#include "mico_rtos.h"
#include "mico_bt_constants.h"
#include "string.h"
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

static const uint16_t fixed_attribute_size_list[] = {
    [MICO_ATTRIBUTE_TYPE_NO_VALUE                                      ] =  ATTR_NO_VALUE_SIZE,
    [MICO_ATTRIBUTE_TYPE_LONG_VALUE                                    ] =  ATTR_LONG_VALUE_SIZE,
    [MICO_ATTRIBUTE_TYPE_PRIMARY_SERVICE                               ] =  ATTR_SERVICE_SIZE,
    [MICO_ATTRIBUTE_TYPE_INCLUDE                                       ] =  ATTR_INCLUDE_SIZE,
    [MICO_ATTRIBUTE_TYPE_CHARACTERISTIC                                ] =  ATTR_CHARACTERISTIC_SIZE,
    [MICO_ATTRIBUTE_TYPE_CHARACTERISTIC_VALUE                          ] =  0, // Variable length
    [MICO_ATTRIBUTE_TYPE_CHARACTERISTIC_DESCRIPTOR_EXTENDED_PROPERTIES ] =  ATTR_EXTENDED_PROPERTIES_SIZE,
    [MICO_ATTRIBUTE_TYPE_CHARACTERISTIC_DESCRIPTOR_USER_DESCRIPTION    ] =  0, // Variable length
    [MICO_ATTRIBUTE_TYPE_CHARACTERISTIC_DESCRIPTOR_CLIENT_CONFIGURATION] =  ATTR_CLIENT_CONFIG_SIZE,
    [MICO_ATTRIBUTE_TYPE_CHARACTERISTIC_DESCRIPTOR_SERVER_CONFIGURATION] =  ATTR_SERVER_CONFIG_SIZE,
    [MICO_ATTRIBUTE_TYPE_CHARACTERISTIC_DESCRIPTOR_PRESENTATION_FORMAT ] =  ATTR_PRESENTATION_FORMAT_SIZE,
    [MICO_ATTRIBUTE_TYPE_CHARACTERISTIC_DESCRIPTOR_AGGREGATE_FORMAT    ] =  0, // Variable length
};

static uint32_t attributes_created = 0;
static uint32_t attributes_deleted = 0;

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

static OSStatus print_uuid( const mico_bt_uuid_t *uuid );
static OSStatus print_type( const mico_bt_uuid_t *uuid );

/******************************************************
 *               Variable Definitions
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/

OSStatus mico_bt_smart_attribute_create( mico_bt_smart_attribute_t **attribute, mico_bt_smart_attribute_type_t type,
                                         uint16_t variable_length )
{
    uint16_t size;

    if ( attribute == NULL ) {
        return MICO_BT_BADARG;
    }

    switch ( type ) {
        case MICO_ATTRIBUTE_TYPE_CHARACTERISTIC_VALUE: {
            size = ATTR_CHARACTERISTIC_VALUE_SIZE( variable_length );
            break;
        }
        case MICO_ATTRIBUTE_TYPE_CHARACTERISTIC_DESCRIPTOR_USER_DESCRIPTION: {
            size = ATTR_USER_DESCRIPTION_SIZE( variable_length );
            break;
        }
        case MICO_ATTRIBUTE_TYPE_CHARACTERISTIC_DESCRIPTOR_AGGREGATE_FORMAT: {
            size = ATTR_AGGREGATE_FORMAT_SIZE( variable_length );
            break;
        }
        default: {
            size = fixed_attribute_size_list[type];
            break;
        }
    }

    *attribute = (mico_bt_smart_attribute_t *)malloc_named("attribute", size );

    if ( *attribute == NULL ) {
        return MICO_BT_OUT_OF_HEAP_SPACE;
    }

    memset( *attribute, 0, size );
    (*attribute)->value_struct_size = size - ATTR_COMMON_FIELDS_SIZE;
    attributes_created++;

    return MICO_BT_SUCCESS;
}

OSStatus mico_bt_smart_attribute_delete( mico_bt_smart_attribute_t *attribute )
{
    if ( attribute == NULL ) {
        return MICO_BT_BADARG;
    }

    /* Set to NULL to make sure this doesn't point to any used attribute */
    attribute->next = NULL;

    /* For malloc debugging */
    malloc_transfer_to_curr_thread( attribute );
    free( attribute );

    attributes_deleted++;

    return MICO_BT_SUCCESS;
}

OSStatus mico_bt_smart_attribute_create_list( mico_bt_smart_attribute_list_t *list )
{
    if ( list == NULL ) {
        return MICO_BT_BADARG;
    }

    memset( list, 0, sizeof( *list ) );
    return MICO_BT_SUCCESS;
}

OSStatus mico_bt_smart_attribute_add_to_list( mico_bt_smart_attribute_list_t *list,
                                              mico_bt_smart_attribute_t *attribute )
{
    if ( list == NULL || attribute == NULL ) {
        return MICO_BT_BADARG;
    }

    if ( list->count == 0 ) {
        /* List is empty. Point list to the attribute */
        attribute->next = NULL;
        list->list = attribute;
    } else {
        mico_bt_smart_attribute_t *curr = list->list;
        mico_bt_smart_attribute_t *prev = NULL;

        /* Traverse the list and compare handle */
        while ( curr != NULL ) {
            if ( curr->handle > attribute->handle ) {
                if ( prev == NULL ) {
                    /* Insert attribute at first position */
                    attribute->next = curr;
                    list->list = attribute;
                    break;
                } else {
                    /* Insert attribute in between prev and curr */
                    prev->next = attribute;
                    attribute->next = curr;
                    break;
                }
            } else if ( curr->next == NULL ) {
                /* Insert attribute at the end of the list */
                curr->next = attribute;
                attribute->next = NULL;
                break;
            }

            /* Update previous and current attribute pointers */
            prev = curr;
            curr = curr->next;
        }
    }

    /* Increment count */
    list->count++;
    return MICO_BT_SUCCESS;
}

OSStatus mico_bt_smart_attribute_remove_from_list( mico_bt_smart_attribute_list_t *list, uint16_t handle )
{
    if ( list == NULL ) {
        return MICO_BT_BADARG;
    }

    if ( list->count == 0 ) {
        return MICO_BT_LIST_EMPTY;
    } else {
        mico_bt_smart_attribute_t *curr = list->list;
        mico_bt_smart_attribute_t *prev = NULL;

        /* Traverse the list and compare handle */
        while ( curr != NULL ) {
            if ( curr->handle == handle ) {
                if ( prev == NULL ) {
                    /* Remove attribute at first position */
                    list->list = curr->next;
                } else {
                    /* Remove curr */
                    prev->next = curr->next;
                }

                mico_bt_smart_attribute_delete( curr );

                /* Decrement count */
                list->count--;
                return MICO_BT_SUCCESS;
            }

            /* Update previous and current attribute pointers */
            prev = curr;
            curr = curr->next;
        }
    }

    return MICO_BT_ITEM_NOT_IN_LIST;
}

OSStatus mico_bt_smart_attribute_delete_list( mico_bt_smart_attribute_list_t *list )
{
    mico_bt_smart_attribute_t *curr;

    if ( list == NULL ) {
        return MICO_BT_BADARG;
    }

    curr = list->list;

    /* Traverse through the list and delete all attributes */
    while ( curr != NULL ) {
        /* Store pointer to next because curr is about to be deleted */
        mico_bt_smart_attribute_t *next = curr->next;

        mico_bt_smart_attribute_delete( curr );

        /* Update curr */
        curr = next;
    }

    memset( list, 0, sizeof( *list ) );
    return MICO_BT_SUCCESS;
}

OSStatus mico_bt_smart_attribute_search_list_by_handle( const mico_bt_smart_attribute_list_t *list, uint16_t handle,
                                                        mico_bt_smart_attribute_t **attribute )
{
    if ( list == NULL || attribute == NULL ) {
        return MICO_BT_BADARG;
    }

    if ( list->count == 0 ) {
        return MICO_BT_LIST_EMPTY;
    } else {
        mico_bt_smart_attribute_t *curr = list->list;

        /* Traverse the list and compare handle */
        while ( curr != NULL ) {
            if ( curr->handle == handle ) {
                *attribute = curr;
                return MICO_BT_SUCCESS;
            }

            /* Update current attribute pointers */
            curr = curr->next;
        }
    }

    return MICO_BT_ITEM_NOT_IN_LIST;
}

OSStatus mico_bt_smart_attribute_search_list_by_uuid( const mico_bt_smart_attribute_list_t *list,
                                                      const mico_bt_uuid_t *uuid, uint16_t starting_handle, uint16_t ending_handle, mico_bt_smart_attribute_t **attribute )
{
    if ( list == NULL || uuid == NULL || attribute == NULL ) {
        return MICO_BT_BADARG;
    }

    if ( list->count == 0 ) {
        return MICO_BT_LIST_EMPTY;
    } else {
        mico_bt_smart_attribute_t *curr = list->list;

        /* Traverse the list until it's larger or equal the starting handle provided */
        while ( curr != NULL && curr->handle <= ending_handle ) {
            if ( curr->handle >= starting_handle ) {
                break;
            }

            curr = curr->next;
        }

        /* Return if reaches the end of the list */
        if ( curr == NULL || curr->handle > ending_handle ) {
            return MICO_BT_ITEM_NOT_IN_LIST;
        }

        /* Traverse the list and compare handle */
        while ( curr != NULL && curr->handle <= ending_handle ) {
            /* Check if UUID is found */
            if ( memcmp( (void *)&curr->type.uu, &uuid->uu, uuid->len ) == 0 ) {
                *attribute = curr;
                return MICO_BT_SUCCESS;
            }

            /* Update current attribute pointers */
            curr = curr->next;
        }
    }

    return MICO_BT_ITEM_NOT_IN_LIST;
}

OSStatus mico_bt_smart_attribute_merge_lists( mico_bt_smart_attribute_list_t *trunk_list,
                                              mico_bt_smart_attribute_list_t *branch_list )
{
    mico_bt_smart_attribute_t *curr;

    if ( trunk_list == NULL || branch_list == NULL ) {
        return MICO_BT_BADARG;
    }

    curr = branch_list->list;

    /* Traverse through the branch list */
    while ( curr != NULL ) {
        mico_bt_smart_attribute_t *prev = curr;

        /* Increment curr and take prev of the branch list */
        curr = curr->next;
        prev->next = NULL;

        /* Add prev to the trunk list */
        mico_bt_smart_attribute_add_to_list( trunk_list, prev );
    }

    memset( branch_list, 0, sizeof( *branch_list ) );

    return MICO_BT_SUCCESS;
}

OSStatus mico_bt_smart_attribute_print( const mico_bt_smart_attribute_t *attribute )
{
    mico_bt_smart_attribute_t *curr_attr = (mico_bt_smart_attribute_t *)attribute;

    if ( attribute == NULL ) {
        return MICO_BT_BADARG;
    }

    WPRINT_LIB_INFO( ( "----------------------------------------------------\n" ) );
    print_type( &curr_attr->type );
    WPRINT_LIB_INFO( ( "\n" ) );
    WPRINT_LIB_INFO( ( "Handle                         : %d\n", (int)curr_attr->handle ) );
    WPRINT_LIB_INFO( ( "Type                           : " ) );
    print_uuid( &curr_attr->type );
    WPRINT_LIB_INFO( ( "Permission                     : %d\n", (int)curr_attr->permission ) );
    WPRINT_LIB_INFO( ( "Value Length                   : %d\n", (int)curr_attr->value_length ) );

    if ( curr_attr->type.len == UUID_16BIT ) {
        switch ( curr_attr->type.uu.uuid16 ) {
            case 0x2800: {
                WPRINT_LIB_INFO( ( "Start Handle                   : %d\n", (int)curr_attr->value.service.start_handle ) );
                WPRINT_LIB_INFO( ( "End Handle                     : %d\n", (int)curr_attr->value.service.end_handle   ) );
                WPRINT_LIB_INFO( ( "Service UUID                   : ") );
                print_uuid( &curr_attr->value.service.uuid );
                break;
            }
            case 0x2802: {
                WPRINT_LIB_INFO( ( "Start Handle                   : %d\n", (int)curr_attr->value.include.included_service_handle ) );
                WPRINT_LIB_INFO( ( "End Handle                     : %d\n", (int)curr_attr->value.include.end_group_handle ) );
                WPRINT_LIB_INFO( ( "Service UUID                   : ") );
                print_uuid( &curr_attr->value.include.uuid );
                break;
            }
            case 0x2803: {
                WPRINT_LIB_INFO( ( "Properties                     : %d\n", (int)curr_attr->value.characteristic.properties ) );
                WPRINT_LIB_INFO( ( "Value Handle                   : %d\n", (int)curr_attr->value.characteristic.value_handle ) );
                WPRINT_LIB_INFO( ( "Value UUID                     : ") );
                print_uuid( &curr_attr->value.characteristic.uuid );
                break;
            }
            case 0x2900: {
                WPRINT_LIB_INFO( ( "Extended Properties            : %d\n", (int)curr_attr->value.extended_properties.properties ) );
                break;
            }
            case 0x2901: {
                WPRINT_LIB_INFO( ( "Extended Properties            : %s\n", curr_attr->value.user_description.string ) );
                break;
            }
            case 0x2902: {
                WPRINT_LIB_INFO( ( "Client Configuration           : %d\n", (int)curr_attr->value.client_config.config_bits ) );
                break;
            }
            case 0x2903: {
                WPRINT_LIB_INFO( ( "Server Configuration           : %d\n", (int)curr_attr->value.server_config.config_bits ) );
                break;
            }
            case 0x2904: {
                WPRINT_LIB_INFO( ( "Format                         : %d\n", (int)curr_attr->value.presentation_format.format ) );
                WPRINT_LIB_INFO( ( "Exponent                       : %d\n", (int)curr_attr->value.presentation_format.exponent ) );
                WPRINT_LIB_INFO( ( "Unit                           : %d\n", (int)curr_attr->value.presentation_format.unit ) );
                WPRINT_LIB_INFO( ( "Namespace                      : %d\n", (int)curr_attr->value.presentation_format.name_space ) );
                WPRINT_LIB_INFO( ( "Description                    : %d\n", (int)curr_attr->value.presentation_format.description ) );
                break;
            }
            case 0x2905: {
                uint32_t i;

                WPRINT_LIB_INFO( ( "List of Handles                : \n" ) );
                for ( i = 0; i < curr_attr->value_length / 2; i ++ ) {
                    WPRINT_LIB_INFO( ( "%02d ", (int)curr_attr->value.aggregate_format.handle_list[i] ) );
                }
                WPRINT_LIB_INFO( ( "\n" ) );
                break;
            }
            case 0x2A00: {
                WPRINT_LIB_INFO( ( "Device Name                    : %s\n", curr_attr->value.device_name.device_name ) );
                break;
            }
            case 0x2A01: {
                WPRINT_LIB_INFO( ( "Appearance                     : %d\n", (int)curr_attr->value.appearance.appearance ) );
                break;
            }
            case 0x2A02: {
                WPRINT_LIB_INFO( ( "Peripheral Privacy Flag        : %d\n",
                                   (int)curr_attr->value.periph_privacy_flag.periph_privacy_flag ) );
                break;
            }
            case 0x2A03: {
                WPRINT_LIB_INFO( ( "Reconnection Address           : %02x:%02x:%02x:%02x:%02x:%02x\n",
                                   (int)curr_attr->value.reconn_address.reconn_address[0],
                                   (int)curr_attr->value.reconn_address.reconn_address[1],
                                   (int)curr_attr->value.reconn_address.reconn_address[2],
                                   (int)curr_attr->value.reconn_address.reconn_address[3],
                                   (int)curr_attr->value.reconn_address.reconn_address[4],
                                   (int)curr_attr->value.reconn_address.reconn_address[5] ) );
                break;
            }
            case 0x2A04: {
                WPRINT_LIB_INFO( ( "Max Connection Interval        : %d\n",
                                   (int)curr_attr->value.periph_preferred_conn_params.max_conn_interval ) );
                WPRINT_LIB_INFO( ( "Min Connection Interval        : %d\n",
                                   (int)curr_attr->value.periph_preferred_conn_params.min_conn_interval ) );
                WPRINT_LIB_INFO( ( "Slave Latency                  : %d\n",
                                   (int)curr_attr->value.periph_preferred_conn_params.slave_latency ) );
                WPRINT_LIB_INFO( ( "Supervision Timeout Multiplier : %d\n",
                                   (int)curr_attr->value.periph_preferred_conn_params.conn_supervision_timeout_multiplier ) );
                break;
            }
            default: {
                uint32_t i;

                WPRINT_LIB_INFO( ( "Value                          : \n" ) );
                for ( i = 0; i < curr_attr->value_length; i ++ ) {
                    WPRINT_LIB_INFO( ( "%02x ", (int)curr_attr->value.value[i] ) );
                }
                WPRINT_LIB_INFO( ( "\n" ) );
                break;
            }
        }
    }

    WPRINT_LIB_INFO( ( "----------------------------------------------------\n" ) );
    return MICO_BT_SUCCESS;
}

OSStatus mico_bt_smart_attribute_print_list( const mico_bt_smart_attribute_list_t *list )
{
    mico_bt_smart_attribute_t *curr_attr = (mico_bt_smart_attribute_t *)list->list;

    while ( curr_attr != NULL ) {
        mico_bt_smart_attribute_print( curr_attr );
        curr_attr = curr_attr->next;
    }

    return MICO_BT_SUCCESS;
}

static OSStatus print_uuid( const mico_bt_uuid_t *uuid )
{
    if ( uuid->len == UUID_16BIT ) {
        WPRINT_LIB_INFO( ( "%04x\n", (int)uuid->uu.uuid16 ) );
    } else {
        WPRINT_LIB_INFO( ( "%04x %04x %04x %04x %04x %04x %04x %04x\n",
                           (int)uuid->uu.uuid128[0],
                           (int)uuid->uu.uuid128[1],
                           (int)uuid->uu.uuid128[2],
                           (int)uuid->uu.uuid128[3],
                           (int)uuid->uu.uuid128[4],
                           (int)uuid->uu.uuid128[5],
                           (int)uuid->uu.uuid128[6],
                           (int)uuid->uu.uuid128[7] ) );
    }

    return MICO_BT_SUCCESS;
}

static OSStatus print_type( const mico_bt_uuid_t *uuid  )
{
    if ( uuid->len != UUID_16BIT ) {
        WPRINT_LIB_INFO( ( "Unknown Type" ) );
        return MICO_BT_SUCCESS;
    }

    switch ( uuid->uu.uuid16 ) {
        case 0x2800: {
            WPRINT_LIB_INFO( ( "Primary Service" ) );
            break;
        }
        case 0x2801: {
            WPRINT_LIB_INFO( ( "Secondary Service" ) );
            break;
        }
        case 0x2802: {
            WPRINT_LIB_INFO( ( "Include" ) );
            break;
        }
        case 0x2803: {
            WPRINT_LIB_INFO( ( "Characteristic" ) );
            break;
        }
        case 0x2900: {
            WPRINT_LIB_INFO( ( "Characteristic Descriptor - Extended Characteristic Properties" ) );
            break;
        }
        case 0x2901: {
            WPRINT_LIB_INFO( ( "Characteristic Descriptor - User Description" ) );
            break;
        }
        case 0x2902: {
            WPRINT_LIB_INFO( ( "Characteristic Descriptor - Client Characteristic Configuration" ) );
            break;
        }
        case 0x2903: {
            WPRINT_LIB_INFO( ( "Characteristic Descriptor - Server Characteristic Configuration" ) );
            break;
        }
        case 0x2904: {
            WPRINT_LIB_INFO( ( "Characteristic Descriptor - Characteristic Presentation Format" ) );
            break;
        }
        case 0x2905: {
            WPRINT_LIB_INFO( ( "Characteristic Descriptor - Characteristic Aggregate Format" ) );
            break;
        }
        case 0x2A00: {
            WPRINT_LIB_INFO( ( "Characteristic Type - Device Name" ) );
            break;
        }
        case 0x2A01: {
            WPRINT_LIB_INFO( ( "Characteristic Type - Appearance" ) );
            break;
        }
        case 0x2A02: {
            WPRINT_LIB_INFO( ( "Characteristic Type - Peripheral Privacy Flags" ) );
            break;
        }
        case 0x2A03: {
            WPRINT_LIB_INFO( ( "Characteristic Type - Reconnection Address" ) );
            break;
        }
        case 0x2A04: {
            WPRINT_LIB_INFO( ( "Characteristic Type - Peripheral Preferred Connection Parameters" ) );
            break;
        }
        case 0x2A05: {
            WPRINT_LIB_INFO( ( "Service Changed" ) );
            break;
        }
        default: {
            WPRINT_LIB_INFO( ( "Unknown Type" ) );
            break;
        }
    }

    return MICO_BT_SUCCESS;
}

OSStatus mico_bt_smart_attribute_get_list_head( const mico_bt_smart_attribute_list_t *list,
                                                mico_bt_smart_attribute_t **head )
{
    if ( list == NULL ) {
        return MICO_BT_BADARG;
    }

    *head = list->list;
    return MICO_BT_SUCCESS;
}

OSStatus mico_bt_smart_attribute_get_list_count( const mico_bt_smart_attribute_list_t *list, uint32_t *count )
{
    if ( list == NULL ) {
        return MICO_BT_BADARG;
    }

    *count = list->count;
    return MICO_BT_SUCCESS;
}
