#pragma once

//#include "mico_utilities.h"
#include "mico_bt_smartbridge.h"
#include "LinkListUtils.h"

#ifdef __cplusplus
extern "C" {
#endif

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
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

OSStatus bt_smartbridge_socket_manager_init( void );

OSStatus bt_smartbridge_socket_manager_deinit( void );

OSStatus bt_smartbridge_socket_manager_set_max_concurrent_connections( uint8_t count );

mico_bool_t   bt_smartbridge_socket_manager_is_full( void );

OSStatus bt_smartbridge_socket_manager_insert_socket( mico_bt_smartbridge_socket_t *socket );

OSStatus bt_smartbridge_socket_manager_remove_socket( uint16_t connection_handle,
                                                      mico_bt_smartbridge_socket_t **socket );

OSStatus bt_smartbridge_socket_manager_find_socket_by_handle( uint16_t connection_handle,
                                                              mico_bt_smartbridge_socket_t **socket );

OSStatus bt_smartbridge_socket_manager_find_socket_by_address( const mico_bt_device_address_t *address,
                                                               mico_bt_smartbridge_socket_t **socket );

#ifdef __cplusplus
} /* extern "C" */
#endif
