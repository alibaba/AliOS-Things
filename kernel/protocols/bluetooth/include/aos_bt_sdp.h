/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

/* @file
 *
 * Bluetooth SDP Application Programming Interface
 *
 */

#ifndef _AOS_BT_SDP_H_
#define _AOS_BT_SDP_H_

#include "aos_bt_dev.h"
#include "sdpdefs.h"

/****************************************************************************
 *  Constants
 ****************************************************************************/

/* SDP result - Success code and error codes */
enum aos_bt_sdp_result_t {
    AOS_BT_SDP_SUCCESS              = AOS_BT_SUCCESS,   /* SDP - Result: Success */
    AOS_BT_SDP_INVALID_VERSION                = 0x0001,   /* SDP - invalid version */
    AOS_BT_SDP_INVALID_SERV_REC_HDL           = 0x0002,   /* SDP - invalid service record */
    AOS_BT_SDP_INVALID_REQ_SYNTAX             = 0x0003,   /* SDP - invalid request syntax */
    AOS_BT_SDP_INVALID_PDU_SIZE               = 0x0004,   /* SDP - invalid PDU size */
    AOS_BT_SDP_INVALID_CONT_STATE             = 0x0005,   /* SDP - invalid controller state */
    AOS_BT_SDP_NO_RESOURCES                   = 0x0006,   /* SDP - no resources */
    AOS_BT_SDP_DI_REG_FAILED                  = 0x0007,   /* SDP - registration failed */
    AOS_BT_SDP_DI_DISC_FAILED                 = 0x0008,   /* SDP - discovery failed */
    AOS_BT_SDP_NO_DI_RECORD_FOUND             = 0x0009,   /* SDP - no record found */
    AOS_BT_SDP_ERR_ATTR_NOT_PRESENT           = 0x000A,   /* SDP - no attribute present */
    AOS_BT_SDP_ILLEGAL_PARAMETER              = 0x000B,   /* SDP - Illegal parameter */

    AOS_BT_SDP_NO_RECS_MATCH                  = 0xFFF0,   /* SDP - No records match */
    AOS_BT_SDP_CONN_FAILED                    = 0xFFF1,   /* SDP - Connection failed */
    AOS_BT_SDP_CFG_FAILED                     = 0xFFF2,   /* SDP - Configuration failed */
    AOS_BT_SDP_GENERIC_ERROR                  = 0xFFF3,   /* SDP - Generic error */
    AOS_BT_SDP_DB_FULL                        = 0xFFF4,   /* SDP - DB full */
    AOS_BT_SDP_INVALID_PDU                    = 0xFFF5,   /* SDP - Invalid PDU */
    AOS_BT_SDP_SECURITY_ERR                   = 0xFFF6,   /* SDP - Security Error */
    AOS_BT_SDP_CONN_REJECTED                  = 0xFFF7,   /* SDP - Connection rejected */
    AOS_BT_SDP_CANCEL                         = 0xFFF8    /* SDP - cancel */
};

/* Define the PSM that SDP uses */
#define SDP_PSM     0x0001

/* Masks for attr_value field of aos_bt_sdp_discovery_attribute_t */
#define SDP_DISC_ATTR_LEN_MASK          0x0FFF
#define SDP_DISC_ATTR_TYPE(len_type)    (len_type >> 12)
#define SDP_DISC_ATTR_LEN(len_type)     (len_type & SDP_DISC_ATTR_LEN_MASK)

/* Maximum number of protocol list items (list_elem in aos_bt_sdp_protocol_elem_t) */
#define SDP_MAX_LIST_ELEMS      3


/****************************************************************************
 *  Type Definitions
 ****************************************************************************/

/*
 * Function     aos_bt_sdp_discovery_complete_cback_t
 *
 *              Service discovery complete callback.
 *
 *              If discovery was successful, the discovery results database (provided when #aos_bt_sdp_service_search_request
 *              or #aos_bt_sdp_service_search_attribute_request was called) will be filled.
 *
 *              Use the aos_bt_sdp_find_* utility functions to parse the results.
 *
 * @param[in]   sdp_result : SDP result code (see #aos_bt_sdp_result_t)
 *
 * @return      Nothing
 *
 */
typedef void (aos_bt_sdp_discovery_complete_cback_t) (uint16_t sdp_result);

/* Attribute value */
typedef struct {
    union {
        uint8_t                 u8;                 /* 8-bit integer            */
        uint16_t                u16;                /* 16-bit integer           */
        uint32_t                u32;                /* 32-bit integer           */
        uint8_t                 array[4];           /* Variable length field    */
        struct t_sdp_discovery_attr  *p_sub_attr;   /* Addr of first sub-attr (list)*/
    } v;
} aos_bt_sdp_discovery_attribute_value_t;

/* SDP Attribute */
typedef struct t_sdp_discovery_attr {
    struct t_sdp_disc_attr                      *p_next_attr;   /* Addr of next linked attr     */
    uint16_t                                    attr_id;        /* Attribute ID                 */
    uint16_t                                    attr_len_type;  /* Length and type fields       */
    aos_bt_sdp_discovery_attribute_value_t    attr_value;     /* Variable length entry data   */
} aos_bt_sdp_discovery_attribute_t;

/* Discovery record from SDP search result */
typedef struct sdp_discovery_record_t {
    aos_bt_sdp_discovery_attribute_t      *p_first_attr;      /* First attribute of record    */
    struct sdp_discovery_record_t           *p_next_rec;        /* Addr of next linked record   */
    uint32_t                                time_read;          /* The time the record was read */
    aos_bt_device_address_t               remote_bd_addr;     /* Remote BD address            */
} aos_bt_sdp_discovery_record_t;

/* Discovery database (used for performing service searches and holding search results) */
typedef struct {
    uint32_t                                mem_size;                           /* Memory size of the DB        */
    uint32_t                                mem_free;                           /* Memory still available       */
    aos_bt_sdp_discovery_record_t         *p_first_rec;                       /* Addr of first record in DB   */
    uint16_t                                num_uuid_filters;                   /* Number of UUIds to filter    */
    aos_bt_uuid_t                         uid_filters[SDP_MAX_UUID_FILTERS];  /* UUIDs to filter              */
    uint16_t                                num_attr_filters;                   /* Number of attribute filters  */
    uint16_t                                attr_filters[SDP_MAX_ATTR_FILTERS]; /* Attributes to filter         */
    uint8_t                                 *p_free_mem;                        /* Pointer to free memory       */
} aos_bt_sdp_discovery_db_t;

/* This structure is used to add protocol lists and find protocol elements */
typedef struct {
    uint16_t      protocol_uuid;                        /* The protocol uuid                  */
    uint16_t      num_params;                           /* Number of parameters               */
    uint16_t      params[SDP_MAX_PROTOCOL_PARAMS];      /* Contents of protocol parameters    */
} aos_bt_sdp_protocol_elem_t;

/****************************************************************************
 *  SDP Server Database Macros
 ****************************************************************************/
#define SDP_UINT1(value)                (value)
#define SDP_UINT2(value)                (value) >> 8, (value) & 0xff
#define SDP_UINT4(value)                (value) >> 24, ((value) >> 16) & 0xff, ((value) >> 8) & 0xff, (value) & 0xff
#define SDP_UINT8(value)                    (value) >> 56, ((value) >> 48) & 0xff, ((value >> 40) >> 8) & 0xff,   \
                                            (value >> 32) & 0xff, ((value) >> 24) & 0xff, ((value) >> 16) & 0xff, \
                                            ((value) >> 8) & 0xff, (value) & 0xff
#define SDP_BOOLEAN                     SDP_UINT1

#define SDP_ATTR_VALUE_UINT1(value)     (UINT_DESC_TYPE << 3)    | SIZE_ONE_BYTE,    SDP_UINT1(value)
#define SDP_ATTR_VALUE_UINT2(value)     (UINT_DESC_TYPE << 3)    | SIZE_TWO_BYTES,   SDP_UINT2(value)
#define SDP_ATTR_VALUE_UINT4(value)     (UINT_DESC_TYPE << 3)    | SIZE_FOUR_BYTES,  SDP_UINT4(value)
#define SDP_ATTR_VALUE_UINT8(value)     (UINT_DESC_TYPE << 3)    | SIZE_EIGHT_BYTES, SDP_UINT8(value)
#define SDP_ATTR_VALUE_BOOLEAN(value)   (BOOLEAN_DESC_TYPE << 3),                    SDP_UINT1(value)

#define SDP_ATTR_VALUE_TEXT             (TEXT_STR_DESC_TYPE << 3) | SIZE_IN_NEXT_BYTE

#define SDP_ATTR_UINT1(id, value)       SDP_ATTR_ID(id), SDP_ATTR_VALUE_UINT1(value)
#define SDP_ATTR_UINT2(id, value)       SDP_ATTR_ID(id), SDP_ATTR_VALUE_UINT2(value)
#define SDP_ATTR_UINT4(id, value)       SDP_ATTR_ID(id), SDP_ATTR_VALUE_UINT4(value)
#define SDP_ATTR_UINT8(id, value)       SDP_ATTR_ID(id), SDP_ATTR_VALUE_UINT8(value)
#define SDP_ATTR_BOOLEAN(id, value)     SDP_ATTR_ID(id), SDP_ATTR_VALUE_BOOLEAN(value)

#define SDP_ATTR_ID                     SDP_ATTR_VALUE_UINT2

#define SDP_ATTR_UUID16(uuid)           ((UUID_DESC_TYPE << 3) | SIZE_TWO_BYTES), SDP_UINT2(uuid)

#define SDP_ATTR_UUID128(x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16)   \
         ((UUID_DESC_TYPE << 3) | SIZE_SIXTEEN_BYTES), \
         x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16

#define SDP_ATTR_TEXT(id, len)          SDP_ATTR_ID(id), SDP_ATTR_VALUE_TEXT, (len)

#define SDP_ATTR_SEQUENCE_1(length)     ((DATA_ELE_SEQ_DESC_TYPE << 3) | SIZE_IN_NEXT_BYTE), (length)
#define SDP_ATTR_SEQUENCE_2(length)     ((DATA_ELE_SEQ_DESC_TYPE << 3) | SIZE_IN_NEXT_WORD), SDP_UINT2(length)
#define SDP_ATTR_SEQUENCE_4(length)     ((DATA_ELE_SEQ_DESC_TYPE << 3) | SIZE_IN_NEXT_LONG), SDP_UINT4(length)

/* Service Record Handle 0x0000 */
#define SDP_ATTR_RECORD_HANDLE(handle)  SDP_ATTR_UINT4(ATTR_ID_SERVICE_RECORD_HDL, handle)

/* Service Class ID List 0x0001 */
#define SDP_ATTR_CLASS_ID(uuid)                                         \
    SDP_ATTR_ID(ATTR_ID_SERVICE_CLASS_ID_LIST), SDP_ATTR_SEQUENCE_1(3), \
        SDP_ATTR_UUID16(uuid)

#define SDP_ATTR_CLASS_ID128(x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16)  \
    SDP_ATTR_ID(ATTR_ID_SERVICE_CLASS_ID_LIST), SDP_ATTR_SEQUENCE_1(17), \
        SDP_ATTR_UUID128(x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16)

/* Service Record State 0x0002 */
#define SDP_ATTR_SERVICE_RECORD_STATE(state)                            \
        SDP_ATTR_UINT4(ATTR_ID_SERVICE_RECORD_STATE, state)

/* Service ID 0x0003 */
#define SDP_ATTR_SERVICE_ID(uuid)                                       \
    SDP_ATTR_ID(ATTR_ID_SERVICE_ID), SDP_ATTR_SEQUENCE_1(3),            \
        SDP_ATTR_UUID16(uuid)

/* Protocol Descriptor List 0x0004 for L2CAP */
#define SDP_ATTR_PROTOCOL_DESC_LIST(l2cap_chan)                         \
    SDP_ATTR_ID(ATTR_ID_PROTOCOL_DESC_LIST), SDP_ATTR_SEQUENCE_1(13),   \
        SDP_ATTR_SEQUENCE_1(6),                                         \
        SDP_ATTR_UUID16(UUID_PROTOCOL_L2CAP),                           \
            SDP_ATTR_VALUE_UINT2(1),                                    \
        SDP_ATTR_SEQUENCE_1(3),                                         \
            SDP_ATTR_UUID16(l2cap_chan)

/* Protocol Descriptor List 0x0004 for RFCOMM */
#define SDP_ATTR_RFCOMM_PROTOCOL_DESC_LIST(scn)                         \
    SDP_ATTR_ID(ATTR_ID_PROTOCOL_DESC_LIST), SDP_ATTR_SEQUENCE_1(12),   \
        SDP_ATTR_SEQUENCE_1(3),                                         \
            SDP_ATTR_UUID16(UUID_PROTOCOL_L2CAP),                       \
        SDP_ATTR_SEQUENCE_1(5),                                         \
            SDP_ATTR_UUID16(UUID_PROTOCOL_RFCOMM),                      \
            SDP_ATTR_VALUE_UINT1(scn)

/* Browse Group List 0x0005 */
#define SDP_ATTR_BROWSE_LIST                                            \
    SDP_ATTR_ID(ATTR_ID_BROWSE_GROUP_LIST), SDP_ATTR_SEQUENCE_1(3),     \
        SDP_ATTR_UUID16(UUID_SERVCLASS_PUBLIC_BROWSE_GROUP)

/* Language Base 0x0006 */
#define SDP_ATTR_LANGUAGE_BASE_ATTR_ID_LIST                                     \
    SDP_ATTR_ID(ATTR_ID_LANGUAGE_BASE_ATTR_ID_LIST), SDP_ATTR_SEQUENCE_1(9),    \
       SDP_ATTR_VALUE_UINT2(LANG_ID_CODE_ENGLISH),                              \
       SDP_ATTR_VALUE_UINT2(LANG_ID_CHAR_ENCODE_UTF8),                          \
       SDP_ATTR_VALUE_UINT2(LANGUAGE_BASE_ID)

/* Service Info Time to Live 0x0007 */
#define SDP_ATTR_SERVICE_INFO_TIME_TO_LIVE(seconds)                     \
        SDP_ATTR_UINT4(ATTR_ID_SERVICE_INFO_TIME_TO_LIVE, seconds)

/* Service Availability 0x0008 */
#define SDP_ATTR_SERVICE_AVAILABILITY(availability)                     \
        SDP_ATTR_UINT1(ATTR_ID_SERVICE_AVAILABILITY, availability)

/* BT Profile Descriptor List 0x0009 */
#define SDP_ATTR_PROFILE_DESC_LIST(uuid, version)                       \
    SDP_ATTR_ID(ATTR_ID_BT_PROFILE_DESC_LIST), SDP_ATTR_SEQUENCE_1(8),  \
        SDP_ATTR_SEQUENCE_1(6),                                         \
            SDP_ATTR_UUID16(uuid),                                      \
            SDP_ATTR_VALUE_UINT2(version)

/* Documentation URL 0x000A */
#define SDP_ATTR_DOCUMENTATION_URL(len)                                 \
    SDP_ATTR_TEXT(ATTR_ID_DOCUMENTATION_URL, len)

/* Client Executable URL 0x000B */
#define SDP_ATTR_CLIENT_EXECUTABLE_URL(len)                             \
    SDP_ATTR_TEXT(ATTR_ID_CLIENT_EXE_URL, len)

/* Icon URL 0x000C */
#define SDP_ATTR_ICON_URL(len)                                          \
    SDP_ATTR_TEXT(ATTR_ID_ICON_URL, len)

/* Service Name LANGUAGE_BASE_ID (0x0100) + 0x0000 = 0x0100 */
#define SDP_ATTR_SERVICE_NAME(len)                                      \
    SDP_ATTR_TEXT(ATTR_ID_SERVICE_NAME, len)


/* Service Description LANGUAGE_BASE_ID (0x0100) + 0x0001 = 0x0101 */
#define SDP_ATTR_SERVICE_DESCRIPTION(len)                               \
    SDP_ATTR_TEXT(ATTR_ID_SERVICE_DESCRIPTION, len)

/* Provider Name LANGUAGE_BASE_ID (0x0100) + 0x0002 = 0x0102 */
#define SDP_ATTR_PROVIDER_NAME(len)                                     \
    SDP_ATTR_TEXT(ATTR_ID_PROVIDER_NAME, len)

/* Group ID 0x0200 */
#define SDP_ATTR_GROUP_ID(uuid)                                         \
        SDP_ATTR_ID(ATTR_ID_GROUP_ID), SDP_ATTR_SEQUENCE_1(3),          \
        SDP_ATTR_UUID16(uuid)

/* Version Number List 0x0200 */
#define SDP_ATTR_VERSION_NUMBER_LIST(version)                           \
        SDP_ATTR_UINT2(ATTR_ID_SPECIFICATION_ID, version)

/* Service Database State 0x0201 */
#define SDP_ATTR_SERVICE_DATABASE_STATE(state)                          \
        SDP_ATTR_UINT4(ATTR_ID_VENDOR_ID, state)

/************************************************************************//*
 * @addtogroup  sdp_api_functions       Service Discovery (SDP)
 * @ingroup     aosbt
 *
 * Service Discovery (SDP) Functions.
 *
 * @{
 ****************************************************************************/

/*****************************************************
 *               Function Declarations
 ******************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

/* SDP Server APIs */

/*
 *
 * Function         aos_bt_sdp_db_init
 *
 * Initialize local SDP server database (database generated using
 * AOS Smart/SmartReady Designer)
 *
 * @param[in]       p_sdp_db:    First element in database array
 * @param[in]       size:        size (in bytes) of SDP database
 *
 * @return          TRUE if successful, FALSE otherwise
 *
 **/
aos_bool_t aos_bt_sdp_db_init (uint8_t *p_sdp_db, uint16_t size);

/* SDP Client APIs */

/*
 *
 * Function         aos_bt_sdp_init_discovery_db
 *
 *                  Initialize discovery database prior to performing service
 *                  discovery (using #aos_bt_sdp_service_search_request or
 *                  #aos_bt_sdp_service_search_request).
 *
 *                  Provides a list of UUIDs and/or attribute IDs to search for.
 *
 * @param[in]       p_db : Discovery database to initialize
 * @param[in]       len :  size of discovery database
 * @param[in]       num_uuid : Number of UUIDs in p_uuid_list
 * @param[in]       p_uuid_list : UUIDs to add to discovery database
 * @param[in]       num_attr : Number of attributes in p_attr_list
 * @param[in]       p_attr_list : Attributes to add to discovery database
 *
 * @return          TRUE if successful, FALSE if one or more parameters are bad
 *
 **/
aos_bool_t aos_bt_sdp_init_discovery_db (aos_bt_sdp_discovery_db_t *p_db, uint32_t len,
                                         uint16_t num_uuid,
                                         aos_bt_uuid_t *p_uuid_list,
                                         uint16_t num_attr,
                                         uint16_t *p_attr_list);

/*
 *
 * Function         aos_bt_sdp_cancel_service_search
 *
 *                  Cancel service search request
 *
 * @param[in]       p_db : Discovery database of the request being cancelled
 *
 * @return          TRUE if discovery cancelled, FALSE if a matching activity is not found.
 *
 **/
aos_bool_t aos_bt_sdp_cancel_service_search (aos_bt_sdp_discovery_db_t *p_db);

/*
 *
 * Function         aos_bt_sdp_service_search_request
 *
 *                  Initiate service search on remote device
 *
 * @param[in]       p_bd_addr   : Remote device address
 * @param[in]       p_db        : Discovery database of UUIDs and attribute IDs to search for (intialized using #aos_bt_sdp_init_discovery_db)
 * @param[in]       p_cb        : Callback for discovery results
 *
 * @return          TRUE if discovery started, FALSE if failed.
 *
 **/
aos_bool_t aos_bt_sdp_service_search_request (uint8_t *p_bd_addr,
                                              aos_bt_sdp_discovery_db_t *p_db,
                                              aos_bt_sdp_discovery_complete_cback_t *p_cb);


/*
 *
 * Function         aos_bt_sdp_service_search_attribute_request
 *
 *                  Initiate combined service search and attribute request on remote device
 *
 * @param[in]       p_bd_addr   : Remote device address
 * @param[in]       p_db        : Discovery database of UUIDs and attribute IDs to search for (intialized using #aos_bt_sdp_init_discovery_db)
 * @param[in]       p_cb        : Callback for discovery results
 *
 * @return          TRUE if discovery started, FALSE if failed.
 *
 **/
aos_bool_t aos_bt_sdp_service_search_attribute_request (uint8_t *p_bd_addr,
                                                        aos_bt_sdp_discovery_db_t *p_db,
                                                        aos_bt_sdp_discovery_complete_cback_t *p_cb);



/* API of utilities to find data in the local discovery database */

/*
 *
 * Function         aos_bt_sdp_find_attribute_in_db
 *
 *                  Parse results from service search. Look next record in discovery database
 *                  containing attribute ID.
 *
 * @param[in]       p_db        : Discovery results database
 * @param[in]       attr_id     : Attribute ID to find
 * @param[in]       p_start_rec : Starting record to search from (if NULL, start from beginning of database)
 *
 * @return          Pointer to matching record, or NULL
 *
 **/
aos_bt_sdp_discovery_record_t *aos_bt_sdp_find_attribute_in_db (aos_bt_sdp_discovery_db_t *p_db,
                                                                uint16_t attr_id,
                                                                aos_bt_sdp_discovery_record_t *p_start_rec);


/*
 *
 * Function         aos_bt_sdp_find_attribute_in_rec
 *
 *                  Parse SDP record. Look for requested attribute in the service record.
 *
 * @param[in]       p_rec       : Service record
 * @param[in]       attr_id     : Attribute ID to find
 *
 * @return          Pointer to matching attribute entry, or NULL
 *
 **/
aos_bt_sdp_discovery_attribute_t *aos_bt_sdp_find_attribute_in_rec (aos_bt_sdp_discovery_record_t *p_rec,
                                                                    uint16_t attr_id);


/*
 *
 * Function         aos_bt_sdp_find_service_in_db
 *
 *                  Parse results from service search. Look next record in discovery database
 *                  containing requested service UUID (specified using uint16_t)
 *
 * @param[in]       p_db        : Discovery results database
 * @param[in]       service_uuid: Service to find
 * @param[in]       p_start_rec : Starting record to search from (if NULL, start from beginning of database)
 *
 * @return          Pointer to matching record, or NULL
 *
 **/
aos_bt_sdp_discovery_record_t *aos_bt_sdp_find_service_in_db (aos_bt_sdp_discovery_db_t *p_db,
                                                              uint16_t service_uuid,
                                                              aos_bt_sdp_discovery_record_t *p_start_rec);


/*
 *
 * Function         aos_bt_sdp_find_service_uuid_in_db
 *
 *                  Parse results from service search. Look next record in discovery database
 *                  containing requested  service UUID (specified using aos_bt_uuid_t structure)
 *
 * @param[in]       p_db        : Discovery results database
 * @param[in]       p_uuid      : Service to find
 * @param[in]       p_start_rec : Starting record to search from (if NULL, start from beginning of database)
 *
 * @return          Pointer to matching record, or NULL
 *
 **/
aos_bt_sdp_discovery_record_t *aos_bt_sdp_find_service_uuid_in_db (aos_bt_sdp_discovery_db_t *p_db,
                                                                   aos_bt_uuid_t *p_uuid,
                                                                   aos_bt_sdp_discovery_record_t *p_start_rec);


/*
 *
 * Function         aos_bt_sdp_find_protocol_list_elem_in_rec
 *
 *                  Parse SDP record. Look for requested protocol list element in the service record.
 *
 * @param[in]       p_rec       : Service record
 * @param[in]       layer_uuid  : protocol list element to find
 * @param[out]      p_elem      : protocol list element (if found)
 *
 * @return          TRUE if found, else FALSE
 *
 **/
aos_bool_t aos_bt_sdp_find_protocol_list_elem_in_rec (aos_bt_sdp_discovery_record_t *p_rec,
                                                      uint16_t layer_uuid,
                                                      aos_bt_sdp_protocol_elem_t *p_elem);

/*
 *
 * Function         aos_bt_sdp_find_protocol_lists_elem_in_rec
 *
 *                  Parse SDP record. Look for requested protocol lists element in the service record.
 *
 * @param[in]       p_rec       : Service record
 * @param[in]       layer_uuid  : protocol lists element to find
 * @param[out]      p_elem      : protocol lists element (if found)
 *
 * @return          TRUE if found, else FALSE
 *
 **/
aos_bool_t aos_bt_sdp_find_protocol_lists_elem_in_rec (aos_bt_sdp_discovery_record_t *p_rec,
                                                       uint16_t layer_uuid,
                                                       aos_bt_sdp_protocol_elem_t *p_elem);

/*
 *
 * Function         aos_bt_sdp_find_profile_version_in_rec
 *
 *                  Parse SDP record. Look for version of requested profile.
 *
 * @param[in]       p_rec       : Service record
 * @param[in]       profile_uuid: Profile to find
 * @param[out]      p_version   : Major/minor version of profile (if found)
 *
 * @return          TRUE if found, FALSE if not
 *
 **/
aos_bool_t aos_bt_sdp_find_profile_version_in_rec (aos_bt_sdp_discovery_record_t *p_rec,
                                                   uint16_t profile_uuid,
                                                   uint16_t *p_version);

/*
 *
 * Function         aos_bt_sdp_find_service_uuid_in_rec
 *
 *                  Parse SDP record. Look for service UUID
 *
 * @param[in]       p_rec       : Service record
 * @param[out]      p_uuid      : Service UUID of the record
 *
 * @return          TRUE if found, FALSE if not
 *
 **/
aos_bool_t aos_bt_sdp_find_service_uuid_in_rec(aos_bt_sdp_discovery_record_t *p_rec, aos_bt_uuid_t *p_uuid);

#ifdef __cplusplus
}
#endif

#endif

/*@} sdp_api_functions */
