/*
 * @file uknx.h
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */


#ifndef __UKNX_H__
#define __UKNX_H__

/** @addtogroup uknx_stack uknx
 *  Optional policy of uknx.
 *
 *  @{
 */

#include "uknx_def.h"
#include "uknx_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef int UKNX_RET;
#define UKNX_RET_OK (UKNX_RET)0
#define UKNX_RET_ERR (UKNX_RET)1
#define UKNX_RET_RETRY (UKNX_RET)2

typedef enum {
    UKNX_EVT_INIT,
    UKNX_EVT_CONNECTING,
    UKNX_EVT_CONNECTED,
    UKNX_EVT_DISCONNECTING,
    UKNX_EVT_DISCONNECTED,
    UKNX_EVT_DATA_IND,
    UKNX_EVT_DATA_ACK,
    UKNX_EVT_BUSY_IN,
    UKNX_EVT_BUSY_OUT,
    UKNX_EVT_KNX_T_DISCONNECTED,
} UKNX_EVENT;

#define DATA_EVENT_GROUP 0x1
#define DATA_EVENT_PROP 0x2
#define DATA_EVENT_SYSTEM 0x3
#define DATA_EVENT_PROP_DESC 0x4

typedef struct {
    UKNX_U8 src_h;
    UKNX_U8 src_l;
    UKNX_U8 dest_h;
    UKNX_U8 dest_l;

    UKNX_U8 type;
    UKNX_U8 len;
    UKNX_U8 *data;
} UKNX_DATA_EVENT;

/**
 * evt: event type;
 * info: event stucture define, based on event type, such as UKNX_DATA_EVENT* etc.
 */
typedef UKNX_RET( *UKNX_NOTIFY )( UKNX_EVENT evt, void *info );

/**
 * API for uknx stack initialization.
 *
 *
 * @return  UKNX_RET_OK on success, otherwize on failure.
 */
UKNX_RET uknx_init();

/**
 * API for uknx stack tick function, called by stack user when needed or regularly.
 *
 *
 * @return  UKNX_RET_OK on success, UKNX_RET_RETRY for retry, otherwize on failure.
 */
UKNX_RET uknx_proc();

/**
 * API for set stack event notify callback.
 *
 * @param[in]  cb      event notify callback
 *
 * @return  UKNX_RET_OK on success, otherwize on failure.
 */
UKNX_RET uknx_notify_callback( UKNX_NOTIFY cb );

/**
 * API for get event printable name.
 *
 * @param[in]  evt      event type
 *
 * @return  printable event name string, or NULL.
 */
const char *uknx_event_name( UKNX_EVENT evt );

/**
 * API for get data event type printable name.
 *
 * @param[in]  type      data event type
 *
 * @return  printable data event type name string, or NULL.
 */
const char *uknx_event_data_type_name( UKNX_U8 type );

#if UKNX_LE
#define UKNX_SHORT(s) ((((s)&0xff)<<8)|((s)>>8))
#else
#define UKNX_SHORT(s) s
#endif

void uknx_dump_buffer( const unsigned char *buf, unsigned n );

/**
 * API for compute knx frame checksum.
 *
 * @param[in]  frame      package frame buffer pointer
 * @param[in]  length     input frame buffer length
 *
 * @return  8 bit checksum.
 */
UKNX_U8 uknx_checksum( UKNX_U8 const *frame, UKNX_U8 length );

/* KNX Application layer standard API. */

/**
 * API for read group value.
 *
 * @param[in]   pb               knx frame buffer, caller privided.
 * @param[in]   dest_h, dest_l   high byte and low byte of dest knx device address
 * @param[in]   prio             frame transport priority
 *
 * @return  UKNX_RET_OK on success, otherwize on failure.
 */
UKNX_RET A_GroupValue_Read_Req( UKNX_LL_FRAME *pb, UKNX_U8 dest_h, UKNX_U8 dest_l,  UKNX_U8 prio );

/**
 * API for write group value.
 *
 * @param[in]   pb               knx frame buffer, caller privided.
 * @param[in]   dest_h, dest_l   high byte and low byte of dest knx device address
 * @param[in]   prio             frame transport priority
 * @param[in]   data, len        value data pointer and length
 *
 * @return  UKNX_RET_OK on success, otherwize on failure.
 */
UKNX_RET A_GroupValue_Write_Req( UKNX_LL_FRAME *pb, UKNX_U8 dest_h, UKNX_U8 dest_l, UKNX_U8 prio,
                                 UKNX_BUF data, UKNX_U8 len );

/**
 * API for read property value.
 *
 * @param[in]   pb               knx frame buffer, caller privided.
 * @param[in]   dest_h, dest_l   high byte and low byte of dest knx device address
 * @param[in]   prop_id            property ID
 * @param[in]   elem            value element index of property
 * @param[in]   start_index            value start index of property element
 *
 * @return  UKNX_RET_OK on success, otherwize on failure.
 */
UKNX_RET A_PropertyValue_Read_Req( UKNX_LL_FRAME *pb, UKNX_U8 dest_h, UKNX_U8 dest_l, UKNX_U8 obj_index,
                                   UKNX_U8 prop_id, UKNX_U8 nr_of_elem, UKNX_U16 start_index );

/**
 * API for write property value.
 *
 * @param[in]   pb               knx frame buffer, caller privided.
 * @param[in]   dest_h, dest_l   high byte and low byte of dest knx device address
 * @param[in]   prop_id            property ID
 * @param[in]   elem            value element index of property
 * @param[in]   start_index            value start index of property element
 * @param[in]   data, len        value data pointer and length
 *
 * @return  UKNX_RET_OK on success, otherwize on failure.
 */
UKNX_RET A_PropertyValue_Write_Req( UKNX_LL_FRAME *pb, UKNX_U8 dest_h, UKNX_U8 dest_l, UKNX_U8 obj_index,
                                    UKNX_U8 prop_id, UKNX_U8 nr_of_elem, UKNX_U16 start_index, UKNX_BUF data, UKNX_U8 len );

/**
 * API for read property desciprtion.
 *
 * @param[in]   pb               knx frame buffer, caller privided.
 * @param[in]   dest_h, dest_l   high byte and low byte of dest knx device address
 * @param[in]   obj_index        object index
 * @param[in]   prop_id          property ID
 * @param[in]   elem             value element index of property
 * @param[in]   prop_index       property index
 *
 * @return  UKNX_RET_OK on success, otherwize on failure.
 */
UKNX_RET A_PropertyDescription_Read_Req( UKNX_LL_FRAME *pb, UKNX_U8 dest_h, UKNX_U8 dest_l, UKNX_U8 obj_index,
        UKNX_U8 prop_id, UKNX_U8 prop_index );

/**
 * API for read individual address.
 *
 * @param[in]   pb               knx frame buffer, caller privided.
 *
 * @return  UKNX_RET_OK on success, otherwize on failure.
 */
UKNX_RET A_IndividualAddress_Read_Req( UKNX_LL_FRAME *pb );

/**
 * API for write individual address.
 *
 * @param[in]   pb               knx frame buffer, caller privided.
 * @param[in]   dest_h, dest_l   high byte and low byte of knx device address to be set
 *
 * @return  UKNX_RET_OK on success, otherwize on failure.
 */
UKNX_RET A_IndividualAddress_Write_Req( UKNX_LL_FRAME *pb, UKNX_U8 addr_h, UKNX_U8 addr_l );

UKNX_RET A_SystemNetworkParameter_Read( UKNX_LL_FRAME *pb, UKNX_U8 obj_h, UKNX_U8 obj_l, UKNX_U8 pid_h, UKNX_U8 pid_l, UKNX_U8 ti );

UKNX_RET A_Memory_Read_Req( UKNX_LL_FRAME *pb, UKNX_U8 dest_h, UKNX_U8 dest_l, UKNX_U16 memref,
                            UKNX_U8 len );
UKNX_RET A_Memory_Write_Req( UKNX_LL_FRAME *pb, UKNX_U8 dest_h, UKNX_U8 dest_l, UKNX_U16 memref,
                             UKNX_BUF buf, UKNX_U8 len );
UKNX_RET A_UserMemory_Read_Req( UKNX_LL_FRAME *pb, UKNX_U8 dest_h, UKNX_U8 dest_l, UKNX_U32 memref,
                                UKNX_U8 len );
UKNX_RET A_UserMemory_Write_Req( UKNX_LL_FRAME *pb, UKNX_U8 dest_h, UKNX_U8 dest_l, UKNX_U32 memref,
                                 UKNX_BUF buf, UKNX_U8 len );

UKNX_RET A_DeviceDescriptor_Read_Req( UKNX_LL_FRAME *pb, UKNX_U8 dest_h, UKNX_U8 dest_l );
UKNX_RET A_Authorize_Req( UKNX_LL_FRAME *pb, UKNX_U8 dest_h, UKNX_U8 dest_l, UKNX_U8 key[4] );

/**
 * API for knx connect of transport layer.
 *
 * @param[in]   pb               knx frame buffer, caller privided.
 * @param[in]   dest_h, dest_l   high byte and low byte of dest knx device address
 *
 * @return  UKNX_RET_OK on success, otherwize on failure.
 */
UKNX_RET T_Connect_Req( UKNX_LL_FRAME *pb, UKNX_U8 addr_h, UKNX_U8 addr_l );

#ifdef __cplusplus
}
#endif


/** @} */
#endif
