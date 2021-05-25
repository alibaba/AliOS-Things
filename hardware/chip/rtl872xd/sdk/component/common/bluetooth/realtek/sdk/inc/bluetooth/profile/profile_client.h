/**
*****************************************************************************************
*     Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     profile_client.h
  * @brief    Head file for profile client structure.
  * @details  Common data struct definition.
  * @author   ethan_su
  * @date     2016-02-18
  * @version  v1.0
  * *************************************************************************************
  */

/* Define to prevent recursive inclusion */
#ifndef PROFILE_CLIENT_H
#define PROFILE_CLIENT_H

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <bt_flags.h>
#if F_BT_LE_GATT_CLIENT_SUPPORT
#include <gatt.h>
#include <bt_types.h>
#include <gap_le.h>


/** @defgroup GATT_CLIENT_API GATT Client API
  * @brief GATT client API
  * @{
  */

/*============================================================================*
 *                         Macros
 *============================================================================*/
/** @defgroup GATT_Client_Exported_Macros GATT Client Exported Macros
  * @{
  */
#define CLIENT_PROFILE_GENERAL_ID   0xff        //!< General Client ID used by application, when directly calls the APIs of profile client layer. Distinguish with other specific client module.

/** End of GATT_Client_Exported_Macros
* @}
*/
/*============================================================================*
 *                         Types
 *============================================================================*/
/** @defgroup GATT_Client_Exported_Types GATT Client Exported Types
  * @{
  */

typedef uint8_t T_CLIENT_ID; //!< Client ID

/* callback related data to inform application. */


/** @brief  Discovery state during discovery procedure.*/
typedef enum
{
    DISC_STATE_IDLE,
    DISC_STATE_SRV,
    DISC_STATE_SRV_DONE,
    DISC_STATE_RELATION,
    DISC_STATE_RELATION_DONE,
    DISC_STATE_CHAR,
    DISC_STATE_CHAR_DONE,
    DISC_STATE_CHAR_UUID16_DONE,
    DISC_STATE_CHAR_UUID128_DONE,
    DISC_STATE_CHAR_DESCRIPTOR,
    DISC_STATE_CHAR_DESCRIPTOR_DONE,
    DISC_STATE_FAILED
} T_DISCOVERY_STATE;

/** @brief  Discovery result type*/
typedef enum
{
    DISC_RESULT_ALL_SRV_UUID16,
    DISC_RESULT_ALL_SRV_UUID128,
    DISC_RESULT_SRV_DATA,
    DISC_RESULT_CHAR_UUID16,
    DISC_RESULT_CHAR_UUID128,
    DISC_RESULT_CHAR_DESC_UUID16,
    DISC_RESULT_CHAR_DESC_UUID128,
    DISC_RESULT_RELATION_UUID16,
    DISC_RESULT_RELATION_UUID128,
    DISC_RESULT_BY_UUID16_CHAR,
    DISC_RESULT_BY_UUID128_CHAR,
} T_DISCOVERY_RESULT_TYPE;

/** @brief  GATT write type*/
typedef enum
{
    GATT_WRITE_TYPE_REQ = 0x01,    /**<  ATT "Write Request"  */
    GATT_WRITE_TYPE_CMD = 0x02,           /**<  ATT "Write Command"  */
    GATT_WRITE_TYPE_SIGNED_CMD = 0x04     /**<  ATT "Signed Write Command"  */
} T_GATT_WRITE_TYPE;

typedef struct
{
    uint16_t    att_handle;         /**< The handle for the service declaration */
    uint16_t    end_group_handle;   /**< The handle of the last attribute
    within the service definition */
    uint16_t    uuid16;             /**< 16 bit UUID */
} T_GATT_SERVICE_ELEM16;

typedef struct
{
    uint16_t    att_handle;         /**< The handle for the service declaration */
    uint16_t    end_group_handle;
    /**< The handle of the last attribute within the service definition */
    uint8_t     uuid128[16];        /**< 128 bit UUID */
} T_GATT_SERVICE_ELEM128;


typedef struct
{
    uint16_t    att_handle;         /**< The handle for the service declaration */
    uint16_t    end_group_handle;
    /**< The handle of the last attribute within the service definition */
} T_GATT_SERVICE_BY_UUID_ELEM;

/** @brief  Characteristic declaration for 16 bit UUID.*/
typedef struct
{
    uint16_t    decl_handle;   /**<  Attribute handle */
    uint16_t    properties;    /**< Characteristic Properties, high nibble is reserved */
    uint16_t    value_handle;  /**<  Characteristic Value Handle */
    uint16_t    uuid16;        /**<  16-bit Bluetooth UUID for Characteristic Value */
} T_GATT_CHARACT_ELEM16;

/** @brief  Characteristic declaration for 128 bit UUID.*/
typedef struct
{
    uint16_t    decl_handle;    /**<  Attribute handle */
    uint16_t    properties;     /**< Characteristic Properties, high nibble is reserved */
    uint16_t    value_handle;   /**<  Characteristic Value Handle */
    uint8_t     uuid128[16];    /**<  128-bit UUID for Characteristic Value */
} T_GATT_CHARACT_ELEM128;

/** @brief  Characteristic descriptor for 16 bit UUID.*/
typedef struct
{
    uint16_t    handle;         /**< Attribute handle */
    uint16_t    uuid16;         /**< 16 bit UUID */
} T_GATT_CHARACT_DESC_ELEM16;

/** @brief  Characteristic descriptor for 128 bit UUID.*/
typedef struct
{
    uint16_t    handle;         /**< Attribute handle */
    uint8_t     uuid128[16];    /**< 128 bit UUID */
} T_GATT_CHARACT_DESC_ELEM128;

/** @brief  Relationship discovery for 16 bit UUID.*/
typedef struct
{
    uint16_t    decl_handle;
    uint16_t    att_handle;
    uint16_t    end_group_handle;
    uint16_t    uuid16;
} T_GATT_RELATION_ELEM16;

/** @brief  Relationship discovery for 128 bit UUID.*/
typedef struct
{
    uint16_t    decl_handle;
    uint16_t    att_handle;
    uint16_t    end_group_handle;
    uint8_t     uuid128[16];
} T_GATT_RELATION_ELEM128;

/** @brief  Discovery result data*/
typedef union
{
    T_GATT_SERVICE_ELEM16        *p_srv_uuid16_disc_data;
    T_GATT_SERVICE_ELEM128       *p_srv_uuid128_disc_data;
    T_GATT_SERVICE_BY_UUID_ELEM  *p_srv_disc_data;
    T_GATT_CHARACT_ELEM16        *p_char_uuid16_disc_data;
    T_GATT_CHARACT_ELEM128       *p_char_uuid128_disc_data;
    T_GATT_CHARACT_DESC_ELEM16   *p_char_desc_uuid16_disc_data;
    T_GATT_CHARACT_DESC_ELEM128  *p_char_desc_uuid128_disc_data;
    T_GATT_RELATION_ELEM16       *p_relation_uuid16_disc_data;
    T_GATT_RELATION_ELEM128      *p_relation_uuid128_disc_data;
} T_DISCOVERY_RESULT_DATA;

/** @defgroup General_cb_data General Client Callback Data
  * @{
  */
/** @brief  The callback data of CLIENT_APP_CB_TYPE_DISC_STATE.
  *
  * Discovery procedure related data to inform application
  */
typedef struct
{
    T_DISCOVERY_STATE disc_state;
} T_DISC_STATE_CB_DATA;

/** @brief  The callback data of CLIENT_APP_CB_TYPE_DISC_RESULT.
  *
  * Discovery result data will be sent to upper through the callback
  */
typedef struct
{
    T_DISCOVERY_RESULT_TYPE result_type;
    T_DISCOVERY_RESULT_DATA result_data;
} T_DISC_RESULT_CB_DATA;


/** @brief  The callback type of T_CLIENT_APP_CB_DATA.
  *
  * Message data type, when data sent to app directly
  */
typedef enum
{
    CLIENT_APP_CB_TYPE_DISC_STATE,
    CLIENT_APP_CB_TYPE_DISC_RESULT
} T_CLIENT_CB_TYPE;

/** @brief  The callback data of T_CLIENT_APP_CB_DATA.
  *
  * Client received data from server, when no specific client registered, will be sent to app directly
  */
typedef union
{
    T_DISC_STATE_CB_DATA   disc_state_data;
    T_DISC_RESULT_CB_DATA  disc_result_data;
} T_CLIENT_CB_DATA;

/** @brief  The General Client Callback Data Struct.
  *
  * Callback data sent to application directly from client, include type and content
  */
typedef struct
{
    T_CLIENT_CB_TYPE     cb_type;
    T_CLIENT_CB_DATA     cb_content;
} T_CLIENT_APP_CB_DATA;
/** End of General_cb_data
  * @}
  */


/** @defgroup pfnSpecificClientAppCB_t1 General Client Callback Function Point Definition
  * @{ function ponter used in each specific profile, to send events to application
  */
typedef T_APP_RESULT(*P_FUN_GENERAL_APP_CB)(T_CLIENT_ID client_id, uint8_t conn_id, void *p_data);
/** End of pfnSpecificClientAppCB_t1
  * @}
  */


/** @defgroup Specific_cb Specific Client Callback Function Point Definition
  * @{ Function ponter used in each specific client module, to send events to specific client module.
  */
typedef void (*P_FUN_DISCOVER_STATE_CB)(uint8_t conn_id, T_DISCOVERY_STATE discovery_state);
typedef void (*P_FUN_DISCOVER_RESULT_CB)(uint8_t conn_id,  T_DISCOVERY_RESULT_TYPE result_type,
                                         T_DISCOVERY_RESULT_DATA result_data);
typedef void (*P_FUN_READ_RESULT_CB)(uint8_t conn_id,  uint16_t cause, uint16_t handle,
                                     uint16_t value_size, uint8_t *p_value);
typedef void (*P_FUN_WRITE_RESULT_CB)(uint8_t conn_id, T_GATT_WRITE_TYPE type, uint16_t handle,
                                      uint16_t cause, uint8_t credits);
typedef T_APP_RESULT(*P_FUN_NOTIFY_IND_RESULT_CB)(uint8_t conn_id, bool notify, uint16_t handle,
                                                  uint16_t value_size, uint8_t *p_value);
typedef void (*P_FUN_DISCONNECT_CB)(uint8_t conn_id);
/** End of Specific_cb
  * @}
  */

/** @defgroup T_FUN_CLIENT_CBS Specific Client Callback Functions Struct
  * @{
  */
typedef struct
{
    P_FUN_DISCOVER_STATE_CB    discover_state_cb;   //!< Discovery state callback function pointer
    P_FUN_DISCOVER_RESULT_CB   discover_result_cb;  //!< Discovery reault callback function pointer
    P_FUN_READ_RESULT_CB       read_result_cb;      //!< Read response callback function pointer
    P_FUN_WRITE_RESULT_CB      write_result_cb;     //!< Write result callback function pointer
    P_FUN_NOTIFY_IND_RESULT_CB notify_ind_result_cb;//!< Notify Indication callback function pointer
    P_FUN_DISCONNECT_CB        disconnect_cb;       //!< Disconnection callback function pointer
} T_FUN_CLIENT_CBS;
/** End of T_FUN_CLIENT_CBS * @} */

/** End of GATT_Client_Exported_Types
* @}
*/
/*============================================================================*
 *                         Functions
 *============================================================================*/
/** @defgroup GATT_Client_Exported_Functions GATT Client Exported Functions
  * @{
  */
/**
 * @brief Initialize parameters of GATT client.
 *
 * @param[in] client_num  Set the number of clients that needs to register.
 * @retval None
 *
 * <b>Example usage</b>
 * \code{.c}
    void app_le_profile_init(void)
    {
        client_init(1);
        simple_ble_client_id = simp_ble_add_client(app_client_callback);
        client_register_general_client_cb(app_client_callback);
    }
  * \endcode
  */
void client_init(uint8_t client_num);

/**
  * @brief  Used by application, register general client callback.
  * @param[in]  p_fun_cb  Fuction offered by application.
  * @retval None
 *
 * <b>Example usage</b>
 * \code{.c}
    void app_le_profile_init(void)
    {
        client_init(1);
        simple_ble_client_id = simp_ble_add_client(app_client_callback);
        client_register_general_client_cb(app_client_callback);
    }
 * \endcode
 */
void client_register_general_client_cb(P_FUN_GENERAL_APP_CB p_fun_cb);

/**
 * @brief  Used by specific client, register callback.
 * @param[in,out]  p_out_client_id  Client ID generated for registered specific client module.
 * @param[in]      client_cbs       Callback functions implemented in specific client module.
 * @retval true   Register successful.
 * @retval false  Register failed.
 *
 * <b>Example usage</b>
 * \code{.c}

    T_CLIENT_ID simp_ble_add_client(P_FUN_GENERAL_APP_CB app_cb, uint8_t link_num)
    {
        uint16_t size;
        if (link_num > SIMP_MAX_LINKS)
        {
            APP_PRINT_ERROR1("simp_ble_add_client: invalid link_num %d", link_num);
            return 0xff;
        }
        if (false == client_register_spec_client_cb(&simp_client, &simp_ble_client_cbs))
        {
            simp_client = CLIENT_PROFILE_GENERAL_ID;
            APP_PRINT_ERROR0("simp_ble_add_client failed");
            return simp_client;
        }
        APP_PRINT_INFO1("simp_ble_add_client: simp_client %d", simp_client);

        simp_client_cb = app_cb;
        simp_link_num = link_num;
        size = simp_link_num * sizeof(T_SIMP_LINK);
        simp_table = os_mem_zalloc(RAM_TYPE_DATA_ON, size);

        return simp_client;
    }
 * \endcode
 */
bool client_register_spec_client_cb(T_CLIENT_ID *p_out_client_id,
                                    const T_FUN_CLIENT_CBS *client_cbs);

/**
  * @brief  Send discovery all primary services request.
  * @param[in]  conn_id        Connection ID
  * @param[in]  client_id      Client ID of specific client module.
  * @retval GAP_CAUSE_SUCCESS  Discovery request success.
  * @retval other              Discovery request failed.
  */
T_GAP_CAUSE client_all_primary_srv_discovery(uint8_t conn_id, T_CLIENT_ID client_id);

/**
  * @brief  Send discovery services by 16 bit UUID request.
  * @param[in]  conn_id        Connection ID
  * @param[in]  client_id      Client ID of specific client module.
  * @param[in]  uuid16         16 bit UUID.
  * @retval GAP_CAUSE_SUCCESS  Discovery request success.
  * @retval other              Discovery request failed.
  */
T_GAP_CAUSE client_by_uuid_srv_discovery(uint8_t conn_id, T_CLIENT_ID client_id, uint16_t uuid16);

/**
  * @brief  Send discovery services by 128 bit UUID request.
  * @param[in]  conn_id        Connection ID
  * @param[in]  client_id      Client ID of specific client module.
  * @param[in]  p_uuid128      128 bit UUID.
  * @retval GAP_CAUSE_SUCCESS  Discovery request success.
  * @retval other              Discovery request failed.
  */
T_GAP_CAUSE client_by_uuid128_srv_discovery(uint8_t conn_id, T_CLIENT_ID client_id,
                                            uint8_t *p_uuid128);

/**
  * @brief  Send discovery relationship services request.
  * @param[in]  conn_id        Connection ID
  * @param[in]  client_id      Client ID of specific client module.
  * @param[in]  start_handle   Start handle of range to be searched.
  * @param[in]  end_handle     End handle of range to be searched.
  * @retval GAP_CAUSE_SUCCESS  Discovery request success.
  * @retval other              Discovery request failed.
  */
T_GAP_CAUSE client_relationship_discovery(uint8_t conn_id, T_CLIENT_ID client_id,
                                          uint16_t start_handle, uint16_t end_handle);

/**
  * @brief  Send discovery characteristics request.
  * @param[in]  conn_id        Connection ID
  * @param[in]  client_id      Client ID of specific client module.
  * @param[in]  start_handle   Start handle of range to be searched.
  * @param[in]  end_handle     End handle of range to be searched.
  * @retval GAP_CAUSE_SUCCESS  Discovery request success.
  * @retval other              Discovery request failed.
  */
T_GAP_CAUSE client_all_char_discovery(uint8_t conn_id, T_CLIENT_ID client_id, uint16_t start_handle,
                                      uint16_t end_handle);

/**
  * @brief  Send discovery characteristics request by caracteristic uuid.
  * @param[in]  conn_id        Connection ID
  * @param[in]  client_id      Client ID of specific client module.
  * @param[in]  start_handle   Start handle of range to be searched.
  * @param[in]  end_handle     End handle of range to be searched.
  * @param[in]  uuid16         16bit characteristic uuid to be searched.
  * @retval GAP_CAUSE_SUCCESS  Discovery request success.
  * @retval other              Discovery request failed.
  */
T_GAP_CAUSE client_by_uuid_char_discovery(uint8_t conn_id, T_CLIENT_ID client_id,
                                          uint16_t start_handle,
                                          uint16_t end_handle, uint16_t uuid16);
/**
  * @brief  Send discovery characteristics request by caracteristic uuid.
  * @param[in]  conn_id        Connection ID
  * @param[in]  client_id      Client ID of specific client module.
  * @param[in]  start_handle   Start handle of range to be searched.
  * @param[in]  end_handle     End handle of range to be searched.
  * @param[in]  p_uuid128      128bit characteristic uuid to be searched.
  * @retval GAP_CAUSE_SUCCESS  Discovery request success.
  * @retval other              Discovery request failed.
  */
T_GAP_CAUSE client_by_uuid128_char_discovery(uint8_t conn_id, T_CLIENT_ID client_id,
                                             uint16_t start_handle,
                                             uint16_t end_handle, uint8_t *p_uuid128);
/**
  * @brief  Send discovery characteristics descriptor request.
  * @param[in]  conn_id        Connection ID
  * @param[in]  client_id      Client ID of specific client module.
  * @param[in]  start_handle   Start handle of range to be searched.
  * @param[in]  end_handle     End handle of range to be searched.
  * @retval GAP_CAUSE_SUCCESS  Discovery request success.
  * @retval other              Discovery request failed.
  */
T_GAP_CAUSE client_all_char_descriptor_discovery(uint8_t conn_id, T_CLIENT_ID client_id,
                                                 uint16_t start_handle, uint16_t end_handle);

/**
  * @brief  Read characteristic by handle request.
  * @param[in]  conn_id        Connection ID
  * @param[in]  client_id      Client ID of specific client module.
  * @param[in]  handle         Request handle.
  * @retval GAP_CAUSE_SUCCESS  Read request success.
  * @retval other              Read request failed.
  */
T_GAP_CAUSE client_attr_read(uint8_t conn_id, T_CLIENT_ID client_id, uint16_t handle);

/**
  * @brief  Read characteristic by 16 bit UUID request.
  * @param[in]  conn_id       Connection ID
  * @param[in]  client_id     Client ID of specific client module.
  * @param[in]  start_handle  Start handle of range to be searched.
  * @param[in]  end_handle    End handle of range to be searched.
  * @param[in]  uuid16        Request 16 bit UUID.
  * @param[in]  p_uuid128     Request 128 bit UUID.
  * @retval GAP_CAUSE_SUCCESS Read request success.
  * @retval other             Read request failed.
  */
T_GAP_CAUSE client_attr_read_using_uuid(uint8_t conn_id, T_CLIENT_ID client_id,
                                        uint16_t start_handle,
                                        uint16_t end_handle, uint16_t uuid16, uint8_t *p_uuid128);
/**
 * @brief  Write characteristic request.
 * @param[in]  conn_id    Connection ID
 * @param[in]  client_id  Client ID of specific client module.
 * @param[in]  write_type Type of write.
 * @param[in]  handle     Attribute handle.
 * @param[in]  length     Length of data to be written.
                          If write_type is GATT_WRITE_TYPE_REQ, range of length is from 0 to 512.
                          If write_type is GATT_WRITE_TYPE_CMD, range of length is from 0 to (mtu_size - 3).
                          If write_type is GATT_WRITE_TYPE_SIGNED_CMD, range of length is from 0 to (mtu_size - 15).
                          uint16_t mtu_size is acquired by le_get_conn_param(GAP_PARAM_CONN_MTU_SIZE, &mtu_size, conn_id).
 * @param[in]  p_data     Point to the data to be written.
 * @retval GAP_CAUSE_SUCCESS: Write request success.
 * @retval other: Write request failed.
 *
 * <b>Example usage</b>
 * \code{.c}
    //if write_type is GATT_WRITE_TYPE_SIGNED_CMD, GAP_MSG_LE_GATT_SIGNED_STATUS_INFO will be notified to app
    T_APP_RESULT app_gap_callback(uint8_t cb_type, void *p_cb_data)
    {
        T_APP_RESULT result = APP_RESULT_SUCCESS;
        T_LE_CB_DATA cb_data;
        memcpy(&cb_data, p_cb_data, sizeof(T_LE_CB_DATA));
        APP_PRINT_TRACE1("app_gap_callback: cb_type = %d", cb_type);
        switch (cb_type)
        {
        ...
        case GAP_MSG_LE_GATT_SIGNED_STATUS_INFO:
        APP_PRINT_INFO5("GAP_MSG_LE_GATT_SIGNED_STATUS_INFO:conn_id %d, cause 0x%x, update_local %d, local_sign_count %d,remote_sign_count %d",
                       cb_data.p_le_gatt_signed_status_info->conn_id,
                       cb_data.p_le_gatt_signed_status_info->cause,
                       cb_data.p_le_gatt_signed_status_info->update_local,
                       cb_data.p_le_gatt_signed_status_info->local_sign_count,
                       cb_data.p_le_gatt_signed_status_info->remote_sign_count);
        break;
        ...
        }
    }
 * \endcode
 */
T_GAP_CAUSE client_attr_write(uint8_t conn_id, T_CLIENT_ID client_id,
                              T_GATT_WRITE_TYPE write_type,
                              uint16_t handle, uint16_t length, uint8_t *p_data);

/**
  * @brief  Confirm from application when receive indication from server.
  * @param[in]  conn_id       Connection ID indicate which link is.
  * @retval true: Confirm OK.
  * @retval false: Confirm failed.
  */
T_GAP_CAUSE client_attr_ind_confirm(uint8_t conn_id);

/** End of GATT_Client_Exported_Functions
* @}
*/

/** End of GATT_CLIENT_API
* @}
*/
#endif


#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif /* PROFILE_CLIENT_H */

