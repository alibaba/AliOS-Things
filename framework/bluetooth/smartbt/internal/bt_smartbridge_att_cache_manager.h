/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _BT_SMART_BRIDGE_ATT_MANAGER_H_
#define _BT_SMART_BRIDGE_ATT_MANAGER_H_

#pragma once

#include "smartbt_smart_interface.h"
#include "bt_smartbridge_att_cache_manager.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

#define MAX_CACHED_ATTRIBUTES_INSTANCES ( MAX_BT_SMART_CONNECTIONS )

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

typedef struct bt_smartbridge_att_cache bt_smartbridge_att_cache_t;

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

OSStatus bt_smartbridge_att_cache_enable( uint32_t cache_count, aos_bt_uuid_t cache_services[],
                                          uint32_t service_count );

OSStatus bt_smartbridge_att_cache_disable( void );

aos_bool_t   bt_smartbridge_att_cache_is_enabled( void );

aos_bool_t   bt_smartbridge_att_cache_is_discovering( const bt_smartbridge_att_cache_t *cache );

aos_bool_t   bt_smartbridge_att_cache_get_active_state( const bt_smartbridge_att_cache_t *cache );

OSStatus bt_smartbridge_att_cache_set_active_state( bt_smartbridge_att_cache_t *cache, aos_bool_t is_active );

OSStatus bt_smartbridge_att_cache_find( const aos_bt_smart_device_t *remote_device,
                                        bt_smartbridge_att_cache_t **cache );

OSStatus bt_smartbridge_att_cache_generate( const aos_bt_smart_device_t *remote_device, uint16_t connection_handle,
                                            bt_smartbridge_att_cache_t **cache );

OSStatus bt_smartbridge_att_cache_release( bt_smartbridge_att_cache_t *cache );

OSStatus bt_smartbridge_att_cache_get_list( bt_smartbridge_att_cache_t *cache, aos_bt_smart_attribute_list_t **list );

OSStatus bt_smartbridge_att_cache_lock( bt_smartbridge_att_cache_t *cache );

OSStatus bt_smartbridge_att_cache_unlock( bt_smartbridge_att_cache_t *cache );

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
