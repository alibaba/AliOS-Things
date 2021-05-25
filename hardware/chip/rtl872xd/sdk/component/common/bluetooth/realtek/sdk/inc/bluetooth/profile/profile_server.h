/**
*****************************************************************************************
*     Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     profile_server.h
  * @brief    Head file for server structure.
  * @details  Common data struct definition.
  * @author
  * @date     2017-02-18
  * @version  v1.0
  * *************************************************************************************
  */

/* Define to prevent recursive inclusion */
#ifndef PROFILE_SERVER_H
#define PROFILE_SERVER_H

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <bt_flags.h>
#if F_BT_LE_GATT_SERVER_SUPPORT
#include <bt_types.h>
#include <gatt.h>
#include <gap_le.h>


/** @defgroup GATT_SERVER_API GATT Server API
  * @brief GATT Server API
  * @{
  */
/*============================================================================*
 *                         Macros
 *============================================================================*/
/** @defgroup GATT_SERVER_Exported_Macros GATT Server Exported Macros
  * @brief
  * @{
  */

/** @defgroup General_Service_ID General Service ID
  * @brief Service ID for general profile events.
  * @{
  */
#define SERVICE_PROFILE_GENERAL_ID  0xff
/** @} */

/** End of GATT_SERVER_Exported_Macros
  * @}
  */


/*============================================================================*
 *                         Types
 *============================================================================*/
/** @defgroup GATT_SERVER_Exported_Types GATT Server Exported Types
  * @brief
  * @{
  */

typedef uint8_t T_SERVER_ID;    //!< Service ID

/** @brief  GATT write data type*/
typedef enum
{
    WRITE_REQUEST,                      /**< Write request. */
    WRITE_WITHOUT_RESPONSE,             /**< Write without response. */
    WRITE_SIGNED_WITHOUT_RESPONSE,      /**< Signed write without response. */
    WRITE_LONG,                         /**< Write long request. */
} T_WRITE_TYPE;

/** @brief  GATT PDU type*/
typedef enum
{
    GATT_PDU_TYPE_ANY           = 0x00, /**<  Any PDU type. */
    GATT_PDU_TYPE_NOTIFICATION  = 0x01, /**<  Notification PDU type. */
    GATT_PDU_TYPE_INDICATION    = 0x02  /**<  Indication PDU type. */
} T_GATT_PDU_TYPE;

/** @brief Event type to inform app*/
typedef enum
{
    SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION = 1,    /**< CCCD update event */
    SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE = 2,              /**< client read event */
    SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE = 3,             /**< client write event */
} T_SERVICE_CALLBACK_TYPE;

/** @defgroup GATT_SERVER_CB_DATA App Callback data
  * @brief data for profile to inform application.
  * @{
  */
/** @brief Event ID */
typedef enum
{
    PROFILE_EVT_SRV_REG_COMPLETE,              /**< Services register complete event
    when application calls server_add_service before calling gap_start_bt_stack. */
    PROFILE_EVT_SEND_DATA_COMPLETE,            /**< Notification or indication data send complete event. */
    PROFILE_EVT_SRV_REG_AFTER_INIT_COMPLETE,   /**< Services register complete event
    when application calls server_add_service after receiving @ref GAP_INIT_STATE_STACK_READY. */
} T_SERVER_CB_TYPE;

/** @brief  The callback data of PROFILE_EVT_SRV_REG_COMPLETE */
typedef enum
{
    GATT_SERVER_SUCCESS,
    GATT_SERVER_FAIL
} T_SERVER_RESULT;

/** @brief  The callback data of PROFILE_EVT_SRV_REG_AFTER_INIT_COMPLETE */
typedef struct
{
    T_SERVER_RESULT result;
    T_SERVER_ID     service_id;
    uint16_t        cause;
} T_SERVER_REG_AFTER_INIT_RESULT;

/** @brief The callback data of PROFILE_EVT_SEND_DATA_COMPLETE */
typedef struct
{
    uint16_t        credits;
    uint8_t         conn_id;
    T_SERVER_ID     service_id;
    uint16_t        attrib_idx;
    uint16_t        cause;
} T_SEND_DATA_RESULT;

/** @brief Service callback data */
typedef union
{
    T_SERVER_RESULT     service_reg_result;
    T_SEND_DATA_RESULT  send_data_result;
    T_SERVER_REG_AFTER_INIT_RESULT    server_reg_after_init_result;
} T_SERVER_CB_DATA;

typedef struct
{
    T_SERVER_CB_TYPE   eventId;    /**<  @brief EventId defined upper */
    T_SERVER_CB_DATA   event_data; /**<  @brief Event data */
} T_SERVER_APP_CB_DATA;
/** @} End of GATT_SERVER_CB_DATA */



/** @defgroup P_FUN_WRITE_IND_POST_PROC TGATTDWriteIndPostProc
  * @brief Call back function to execute some post procedure after handle write request from client.
  * @{
  */
typedef void (* P_FUN_WRITE_IND_POST_PROC)(uint8_t conn_id, T_SERVER_ID service_id,
                                           uint16_t attrib_index, uint16_t length,
                                           uint8_t *p_value);
/** @} End of P_FUN_WRITE_IND_POST_PROC */

/** @defgroup Specific_srv_cb Specific Service Callback Function Point Definition
  * @{ Function ponter used in each specific service module, to send events to specific service module.
  */
typedef T_APP_RESULT(*P_FUN_GATT_READ_ATTR_CB)(uint8_t conn_id, T_SERVER_ID service_id,
                                               uint16_t attrib_index,
                                               uint16_t offset, uint16_t *p_length, uint8_t **pp_value);
typedef T_APP_RESULT(*P_FUN_GATT_WRITE_ATTR_CB)(uint8_t conn_id, T_SERVER_ID service_id,
                                                uint16_t attrib_index, T_WRITE_TYPE write_type,
                                                uint16_t length, uint8_t *p_value, P_FUN_WRITE_IND_POST_PROC *p_write_post_proc);
typedef void (*P_FUN_GATT_CCCD_UPDATE_CB)(uint8_t conn_id, T_SERVER_ID service_id,
                                          uint16_t attrib_index, uint16_t ccc_bits);
/** End of Specific_srv_cb
  * @}
  */

/** @defgroup P_FUN_SERVER_GENERAL_CB General Server Callback Function Point Definition
  * @brief function ponter Type used to generate Call back, to send events to application.
  * @{
  */
typedef T_APP_RESULT(*P_FUN_SERVER_GENERAL_CB)(T_SERVER_ID service_id, void *p_para);
/** @} End of pfnAPPHandleInfoCB_t1 */


/** @brief GATT service callbacks */
typedef struct
{
    P_FUN_GATT_READ_ATTR_CB read_attr_cb;     /**< Read callback function pointer.
                                                   Return value: @ref T_APP_RESULT. */
    P_FUN_GATT_WRITE_ATTR_CB write_attr_cb;   /**< Write callback function pointer.
                                                   Return value: @ref T_APP_RESULT. */
    P_FUN_GATT_CCCD_UPDATE_CB cccd_update_cb; /**< Update cccd callback function pointer. */
} T_FUN_GATT_SERVICE_CBS;

/** End of GATT_SERVER_Exported_Types
  * @}
  */

/*============================================================================*
 *                         Functions
 *============================================================================*/
/** @defgroup GATT_SERVER_Exported_Functions GATT Server Exported Functions
  * @brief
  * @{
  */

/**
 * @brief Initialize parameters of GATT Server.
 *
 * @param[in] service_num Set the number of services that needs to register.
 * @retval None
 *
 * <b>Example usage</b>
 * \code{.c}
    void app_le_profile_init(void)
    {
        server_init(1);
        simp_srv_id = simp_ble_service_add_service(app_profile_callback);
        server_register_app_cb(app_profile_callback);
    }
 * \endcode
 */
void server_init(uint8_t service_num);

/**
 * @brief Register builtin services including GAP and GATT service.
 *
 * If application does not need to register GAP and GATT service.
 * Application shall call server_builtin_service_reg(false) before server_init().
 *
 * @param[in] reg Whether to register builtin services. Default value is true.
 * @retval None
 *
 * <b>Example usage</b>
 * \code{.c}
    void app_le_profile_init(void)
    {
        server_builtin_service_reg(false);
        server_init(1);
        simp_srv_id = simp_ble_service_add_service(app_profile_callback);
        server_register_app_cb(app_profile_callback);
    }
 * \endcode
 */
void server_builtin_service_reg(bool reg);

/**
 * @brief Register specific service without start handle
 *
 * Add specific service infomation to gatt_svc_table struct, will be registered to GATT later.
 *
 * @param[in,out] p_out_service_id     Service ID of specific service.
 * @param[in] p_database            Database pointer of specific service.
 * @param[in] length                Length of Database of specific service.
 * @param[in] srv_cbs               Service callback functions of specific service.
 * @retval true Add service success
 * @retval false Add service failed
 *
 * <b>Example usage</b>
 * \code{.c}
    T_SERVER_ID bas_add_service(void *p_func)
    {
        T_SERVER_ID service_id;
        if (false == server_add_service(&service_id,
                                       (uint8_t *)bas_attr_tbl,
                                       bas_attr_tbl_size,
                                       bas_cbs))
        {
            APP_PRINT_ERROR1("bas_add_service: service_id %d", service_id);
            service_id = 0xff;
        }
        pfn_bas_cb = (P_FUN_SERVER_GENERAL_CB)p_func;
        return service_id;
    }
 * \endcode
 */
bool server_add_service(T_SERVER_ID *p_out_service_id, uint8_t *p_database, uint16_t length,
                        const T_FUN_GATT_SERVICE_CBS srv_cbs);

/**
 * @brief Register specific service with start handle
 *
 * Add specific service infomation to gatt_svc_table struct, will be registered to GATT later.
 *
 * @param[in,out] p_out_service_id     Service ID of specific service.
 * @param[in] p_database            Database pointer of specific service.
 * @param[in] length                Length of Database of specific service.
 * @param[in] srv_cbs               Service callback functions of specific service.
 * @param[in] start_handle          Start handle of this service.
 * @retval true Add service success
 * @retval false Add service failed
 *
 * <b>Example usage</b>
 * \code{.c}
    T_SERVER_ID bas_add_service(void *p_func)
    {
        T_SERVER_ID service_id;
        if (false == server_add_service_by_start_handle(&service_id,
                                       (uint8_t *)bas_attr_tbl,
                                       bas_attr_tbl_size,
                                       bas_cbs, 0x00f0))
        {
            APP_PRINT_ERROR1("bas_add_service: service_id %d", service_id);
            service_id = 0xff;
        }
        pfn_bas_cb = (P_FUN_SERVER_GENERAL_CB)p_func;
        return service_id;
    }
 * \endcode
 */
bool server_add_service_by_start_handle(uint8_t *p_out_service_id, uint8_t *p_database,
                                        uint16_t length,
                                        const T_FUN_GATT_SERVICE_CBS srv_cbs, uint16_t start_handle);
/**
 * @brief Register callback function to send events to application.
 *
 * @param[in] p_fun_cb          Callback function.
 * @retval None
 *
 * <b>Example usage</b>
 * \code{.c}
    void app_le_profile_init(void)
    {
        server_init(1);
        simp_srv_id = simp_ble_service_add_service(app_profile_callback);
        server_register_app_cb(app_profile_callback);
    }
 * \endcode
 */
void server_register_app_cb(P_FUN_SERVER_GENERAL_CB p_fun_cb);

/**
  * @brief  Confirm from application when receive read Request from client.
  * @param[in]  conn_id       Connection id indicate which link is.
  * @param[in]  service_id    Service ID.
  * @param[in]  attrib_index  Attribute index of attribute to read confirm from application.
  * @param[in]  p_data        Point to the readed value.
  * @param[in]  length        The length of the data.
  * @param[in]  cause         Cause for read confirm. @ref T_APP_RESULT
  * @retval true: confirm from app OK.
  * @retval false: confirm from app failed.
  */
bool server_attr_read_confirm(uint8_t conn_id, T_SERVER_ID service_id, uint16_t attrib_index,
                              uint8_t *p_data, uint16_t length, T_APP_RESULT cause);
/**
  * @brief  Confirm from application when receive Execute Write Request from client.
  * @param[in]  conn_id     Connection id indicate which link is.
  * @param[in]  cause       Cause for execute write confirm. @ref T_APP_RESULT
  * @param[in]  handle      Gatt attribute handle.
  * @retval true: confirm from app OK.
  * @retval false: confirm from app failed.
  */
bool server_exec_write_confirm(uint8_t conn_id, uint16_t cause, uint16_t handle);

/**
  * @brief  Confirm from application when receive Write Request from client.
  * @param[in]  conn_id      Connection id indicate which link is.
  * @param[in]  service_id   Service ID.
  * @param[in]  attrib_index Attribute index of attribute to write confirm from application.
  * @param[in]  cause        Write request app handle result, APP_RESULT_SUCCESS or other. @ref T_APP_RESULT
  * @retval true: confirm from app OK.
  * @retval false: confirm from app failed.
  */
bool server_attr_write_confirm(uint8_t conn_id, T_SERVER_ID service_id,
                               uint16_t attrib_index, T_APP_RESULT cause);

/**
 * @brief Send characteristic value to peer device.
 *
 * @param[in] conn_id         Connection id indicate which link is.
 * @param[in] service_id      Service ID.
 * @param[in] attrib_index    Attribute index of characteristic.
 * @param[in] p_data          Point to data to be sent.
 * @param[in] data_len        Length of value to be sent, range: 0~(mtu_size - 3).
                              uint16_t mtu_size is acquired by le_get_conn_param(GAP_PARAM_CONN_MTU_SIZE, &mtu_size, conn_id).
 * @param[in] type            GATT pdu type.
 * @return Data sent result
 * @retval true Success.
 * @retval false Failed.
 *
 * <b>Example usage</b>
 * \code{.c}
    bool bas_battery_level_value_notify(uint8_t conn_id, uint8_t service_id, uint8_t battery_level)
    {
        return server_send_data(conn_id, service_id, GATT_SVC_BAS_BATTERY_LEVEL_INDEX, &battery_level,
                                sizeof(battery_level), GATT_PDU_TYPE_ANY);
    }
 * \endcode
 */
bool server_send_data(uint8_t conn_id, T_SERVER_ID service_id, uint16_t attrib_index,
                      uint8_t *p_data, uint16_t data_len, T_GATT_PDU_TYPE type);

/** @} End of GATT_SERVER_Exported_Functions */

/** @} End of GATT_SERVER_API */
#endif

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif /* PROFILE_SERVER_H */
