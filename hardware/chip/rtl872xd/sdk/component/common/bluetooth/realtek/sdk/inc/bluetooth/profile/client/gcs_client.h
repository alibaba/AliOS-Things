/**
*****************************************************************************************
*     Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     gcs_client.h
  * @brief    Head file for using General Common Services client.
  * @details  General common services client data structs and external functions declaration.
  * @author   jane
  * @date     2018-12-13
  * @version  v1.0
  * *************************************************************************************
  */

/* Define to prevent recursive inclusion */
#ifndef _GCS_CLIENT_H_
#define _GCS_CLIENT_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

/* Add Includes here */
#include <bt_flags.h>
#if F_BT_LE_GATT_CLIENT_SUPPORT
#include <profile_client.h>

/** @defgroup GCS_Client General Common Services Client
  * @brief GCS service client
  * @details
     Application shall register gcs client when initialization through @ref gcs_add_client function.
  * \code{.c}
    T_APP_RESULT gcs_client_callback(T_CLIENT_ID client_id, uint8_t conn_id, void *p_data)
    {
        T_APP_RESULT  result = APP_RESULT_SUCCESS;
        APP_PRINT_INFO2("gcs_client_callback: client_id %d, conn_id %d",
                        client_id, conn_id);
        if (client_id == gcs_client_id)
        {
            T_GCS_CLIENT_CB_DATA *p_gcs_cb_data = (T_GCS_CLIENT_CB_DATA *)p_data;
            switch (p_gcs_cb_data->cb_type)
            {
            case GCS_CLIENT_CB_TYPE_DISC_RESULT:
                ......
        }
    }
  * \endcode
  * @{
  */

/** @defgroup GCS_Client_Exported_Macros GCS Client Exported Macros
  * @brief
  * @{
  */
/** @brief  Define maximum links number. range: 0-4 */
#define GCS_MAX_LINKS  4

/** End of GCS_Client_Exported_Macros * @} */

/** @defgroup GCS_Client_Exported_Types GCS Client Exported Types
  * @brief
  * @{
  */

/** @brief  Discovery result type.*/
typedef enum
{
    GCS_ALL_PRIMARY_SRV_DISCOV, //!<Response type for gcs_all_primary_srv_discovery
    GCS_BY_UUID128_SRV_DISCOV,  //!<Response type for gcs_by_uuid128_srv_discovery
    GCS_BY_UUID_SRV_DISCOV,     //!<Response type for gcs_by_uuid_srv_discovery
    GCS_ALL_CHAR_DISCOV,        //!<Response type for gcs_all_char_discovery
    GCS_BY_UUID_CHAR_DISCOV,    //!<Response type for gcs_by_uuid_char_discovery
    GCS_BY_UUID128_CHAR_DISCOV, //!<Response type for gcs_by_uuid128_char_discovery
    GCS_ALL_CHAR_DESC_DISCOV,   //!<Response type for gcs_all_char_descriptor_discovery
} T_GCS_DISCOV_TYPE;

/** @brief  Discovery result element.*/
typedef union
{
    T_GATT_SERVICE_ELEM16        srv_uuid16_disc_data;
    T_GATT_SERVICE_ELEM128       srv_uuid128_disc_data;
    T_GATT_SERVICE_BY_UUID_ELEM  srv_disc_data;
    T_GATT_CHARACT_ELEM16        char_uuid16_disc_data;
    T_GATT_CHARACT_ELEM128       char_uuid128_disc_data;
    T_GATT_CHARACT_DESC_ELEM16   char_desc_uuid16_disc_data;
    T_GATT_CHARACT_DESC_ELEM128  char_desc_uuid128_disc_data;
} T_GCS_DISCOV_RESULT_DATA;

/** @brief  GCS discovery table element.*/
typedef struct
{
    T_DISCOVERY_RESULT_TYPE result_type;
    T_GCS_DISCOV_RESULT_DATA result_data;
} T_GCS_DISCOV_RESULT;

/** @brief GCS client discovery result */
typedef struct
{
    bool is_success;
    T_GCS_DISCOV_TYPE discov_type;
    uint16_t result_num;
    T_GCS_DISCOV_RESULT *p_result_table;
} T_GCS_DISCOVERY_RESULT;

/** @brief GCS client read data, used to inform app read response data */
typedef struct
{
    uint16_t cause;
    uint16_t handle;
    uint16_t value_size;
    uint8_t *p_value;
} T_GCS_READ_RESULT;

/** @brief GCS client write result, used to inform app write response data */
typedef struct
{
    uint16_t cause;
    uint16_t handle;
    T_GATT_WRITE_TYPE type;
} T_GCS_WRITE_RESULT;

/** @brief GCS client notification/indication*/
typedef struct
{
    bool notify;
    uint16_t handle;
    uint16_t value_size;
    uint8_t *p_value;
} T_GCS_NOTIF_IND;

/** @brief GCS client callback type */
typedef enum
{
    GCS_CLIENT_CB_TYPE_DISC_RESULT,          //!< Discovery procedure state, done or pending.
    GCS_CLIENT_CB_TYPE_READ_RESULT,         //!< Read request's result data, responsed from server.
    GCS_CLIENT_CB_TYPE_WRITE_RESULT,
    GCS_CLIENT_CB_TYPE_NOTIF_IND,
    GCS_CLIENT_CB_TYPE_INVALID              //!< Invalid callback type, no practical usage.
} T_GCS_CLIENT_CB_TYPE;

/** @brief GCS client callback content */
typedef union
{
    T_GCS_DISCOVERY_RESULT discov_result;
    T_GCS_READ_RESULT  read_result;
    T_GCS_WRITE_RESULT write_result;
    T_GCS_NOTIF_IND    notif_ind;
} T_GCS_CLIENT_CB_CONTENT;

/** @brief GCS client callback data */
typedef struct
{
    T_GCS_CLIENT_CB_TYPE     cb_type;
    T_GCS_CLIENT_CB_CONTENT  cb_content;
} T_GCS_CLIENT_CB_DATA;

/** End of GCS_Client_Exported_Types * @} */

/** @defgroup GCS_Client_Exported_Functions GCS Client Exported Functions
  * @brief
  * @{
  */

/**
  * @brief  Send discovery all primary services request.
  * @param[in]  conn_id        Connection ID
  * @retval GAP_CAUSE_SUCCESS  Discovery request success.
  * @retval other              Discovery request failed.
  *
  * <b>Example usage</b>
  * \code{.c}
    T_USER_CMD_PARSE_RESULT cmd_gsrvdis(T_USER_CMD_PARSED_VALUE *p_parse_value)
    {
        uint8_t conn_id = p_parse_value->dw_param[0];
        T_GAP_CAUSE cause = gcs_all_primary_srv_discovery(conn_id);

        return (T_USER_CMD_PARSE_RESULT)cause;
    }
    void gcs_handle_discovery_result(uint8_t conn_id, T_GCS_DISCOVERY_RESULT discov_result)
    {
        uint16_t i;
        T_GCS_DISCOV_RESULT *p_result_table;
        uint16_t    properties;
        switch (discov_result.discov_type)
        {
        case GCS_ALL_PRIMARY_SRV_DISCOV:
        ......
        }
     }
  * \endcode
  */
T_GAP_CAUSE gcs_all_primary_srv_discovery(uint8_t conn_id);

/**
  * @brief  Send discovery services by 128 bit UUID request.
  * @param[in]  conn_id        Connection ID
  * @param[in]  p_uuid128      128 bit UUID.
  * @retval GAP_CAUSE_SUCCESS  Discovery request success.
  * @retval other              Discovery request failed.
  *
  * <b>Example usage</b>
  * \code{.c}
    T_USER_CMD_PARSE_RESULT cmd_srvuuid(T_USER_CMD_PARSED_VALUE *p_parse_value)
    {
        ......
        cause = gcs_by_uuid128_srv_discovery(conn_id, uuid128);
        return (T_USER_CMD_PARSE_RESULT)cause;
    }
    void gcs_handle_discovery_result(uint8_t conn_id, T_GCS_DISCOVERY_RESULT discov_result)
    {
        uint16_t i;
        T_GCS_DISCOV_RESULT *p_result_table;
        uint16_t    properties;
        switch (discov_result.discov_type)
        {
        case GCS_BY_UUID128_SRV_DISCOV:
        ......
        }
     }
  * \endcode
  */
T_GAP_CAUSE gcs_by_uuid128_srv_discovery(uint8_t conn_id, uint8_t *p_uuid128);

/**
  * @brief  Send discovery services by 16 bit UUID request.
  * @param[in]  conn_id        Connection ID
  * @param[in]  uuid16         16 bit UUID.
  * @retval GAP_CAUSE_SUCCESS  Discovery request success.
  * @retval other              Discovery request failed.
  *
  * <b>Example usage</b>
  * \code{.c}
    T_USER_CMD_PARSE_RESULT cmd_srvuuid(T_USER_CMD_PARSED_VALUE *p_parse_value)
    {
        ......
        cause = gcs_by_uuid_srv_discovery(conn_id, uuid16);
        return (T_USER_CMD_PARSE_RESULT)cause;
    }
    void gcs_handle_discovery_result(uint8_t conn_id, T_GCS_DISCOVERY_RESULT discov_result)
    {
        uint16_t i;
        T_GCS_DISCOV_RESULT *p_result_table;
        uint16_t    properties;
        switch (discov_result.discov_type)
        {
        case GCS_BY_UUID_SRV_DISCOV:
        ......
        }
     }
  * \endcode
  */
T_GAP_CAUSE gcs_by_uuid_srv_discovery(uint8_t conn_id, uint16_t uuid16);

/**
  * @brief  Send discovery characteristics request.
  * @param[in]  conn_id        Connection ID
  * @param[in]  start_handle   Start handle of range to be searched.
  * @param[in]  end_handle     End handle of range to be searched.
  * @retval GAP_CAUSE_SUCCESS  Discovery request success.
  * @retval other              Discovery request failed.
  *
  * <b>Example usage</b>
  * \code{.c}
    T_USER_CMD_PARSE_RESULT cmd_chardis(T_USER_CMD_PARSED_VALUE *p_parse_value)
    {
        uint8_t conn_id = p_parse_value->dw_param[0];
        uint16_t start_handle = p_parse_value->dw_param[1];
        uint16_t end_handle = p_parse_value->dw_param[2];

        T_GAP_CAUSE cause = gcs_all_char_discovery(conn_id, start_handle, end_handle);

        return (T_USER_CMD_PARSE_RESULT)cause;
    }
    void gcs_handle_discovery_result(uint8_t conn_id, T_GCS_DISCOVERY_RESULT discov_result)
    {
        uint16_t i;
        T_GCS_DISCOV_RESULT *p_result_table;
        uint16_t    properties;
        switch (discov_result.discov_type)
        {
        case GCS_ALL_CHAR_DISCOV:
        ......
        }
     }
  * \endcode
  */
T_GAP_CAUSE gcs_all_char_discovery(uint8_t conn_id, uint16_t start_handle, uint16_t end_handle);

/**
  * @brief  Send discovery characteristics request by caracteristic uuid.
  * @param[in]  conn_id        Connection ID
  * @param[in]  start_handle   Start handle of range to be searched.
  * @param[in]  end_handle     End handle of range to be searched.
  * @param[in]  uuid16         16bit characteristic uuid to be searched.
  * @retval GAP_CAUSE_SUCCESS  Discovery request success.
  * @retval other              Discovery request failed.
  *
  * <b>Example usage</b>
  * \code{.c}
    T_USER_CMD_PARSE_RESULT cmd_charuuid(T_USER_CMD_PARSED_VALUE *p_parse_value)
    {
        ......
        T_GAP_CAUSE cause = gcs_by_uuid_char_discovery(conn_id, start_handle, end_handle,
                                                       uuid16);
        return (T_USER_CMD_PARSE_RESULT)cause;
    }
    void gcs_handle_discovery_result(uint8_t conn_id, T_GCS_DISCOVERY_RESULT discov_result)
    {
        uint16_t i;
        T_GCS_DISCOV_RESULT *p_result_table;
        uint16_t    properties;
        switch (discov_result.discov_type)
        {
        case GCS_BY_UUID_CHAR_DISCOV:
        ......
        }
     }
  * \endcode
  */
T_GAP_CAUSE gcs_by_uuid_char_discovery(uint8_t conn_id, uint16_t start_handle,
                                       uint16_t end_handle, uint16_t uuid16);
/**
  * @brief  Send discovery characteristics request by caracteristic uuid.
  * @param[in]  conn_id        Connection ID
  * @param[in]  start_handle   Start handle of range to be searched.
  * @param[in]  end_handle     End handle of range to be searched.
  * @param[in]  p_uuid128      128bit characteristic uuid to be searched.
  * @retval GAP_CAUSE_SUCCESS  Discovery request success.
  * @retval other              Discovery request failed.
  *
  * <b>Example usage</b>
  * \code{.c}
    T_USER_CMD_PARSE_RESULT cmd_charuuid(T_USER_CMD_PARSED_VALUE *p_parse_value)
    {
        ......
        T_GAP_CAUSE cause = gcs_by_uuid128_char_discovery(conn_id, start_handle,
                                                          end_handle,
                                                          uuid128);
        return (T_USER_CMD_PARSE_RESULT)cause;
    }
    void gcs_handle_discovery_result(uint8_t conn_id, T_GCS_DISCOVERY_RESULT discov_result)
    {
        uint16_t i;
        T_GCS_DISCOV_RESULT *p_result_table;
        uint16_t    properties;
        switch (discov_result.discov_type)
        {
        case GCS_BY_UUID128_CHAR_DISCOV:
        ......
        }
     }
  * \endcode
  */
T_GAP_CAUSE gcs_by_uuid128_char_discovery(uint8_t conn_id, uint16_t start_handle,
                                          uint16_t end_handle, uint8_t *p_uuid128);
/**
  * @brief  Send discovery characteristics descriptor request.
  * @param[in]  conn_id        Connection ID
  * @param[in]  start_handle   Start handle of range to be searched.
  * @param[in]  end_handle     End handle of range to be searched.
  * @retval GAP_CAUSE_SUCCESS  Discovery request success.
  * @retval other              Discovery request failed.
  *
  * <b>Example usage</b>
  * \code{.c}
    T_USER_CMD_PARSE_RESULT cmd_charddis(T_USER_CMD_PARSED_VALUE *p_parse_value)
    {
        uint8_t conn_id = p_parse_value->dw_param[0];
        uint16_t start_handle = p_parse_value->dw_param[1];
        uint16_t end_handle = p_parse_value->dw_param[2];

        T_GAP_CAUSE cause = gcs_all_char_descriptor_discovery(conn_id, start_handle,
                                                              end_handle);
        return (T_USER_CMD_PARSE_RESULT)cause;
    }
    void gcs_handle_discovery_result(uint8_t conn_id, T_GCS_DISCOVERY_RESULT discov_result)
    {
        uint16_t i;
        T_GCS_DISCOV_RESULT *p_result_table;
        uint16_t    properties;
        switch (discov_result.discov_type)
        {
        case GCS_ALL_CHAR_DESC_DISCOV:
        ......
        }
     }
  * \endcode
  */
T_GAP_CAUSE gcs_all_char_descriptor_discovery(uint8_t conn_id, uint16_t start_handle,
                                              uint16_t end_handle);
/**
  * @brief  Read characteristic by handle request.
  * @param[in]  conn_id        Connection ID
  * @param[in]  handle         Request handle.
  * @retval GAP_CAUSE_SUCCESS  Read request success.
  * @retval other              Read request failed.
  *
  * <b>Example usage</b>
  * \code{.c}
    T_USER_CMD_PARSE_RESULT cmd_read(T_USER_CMD_PARSED_VALUE *p_parse_value)
    {
        uint8_t conn_id = p_parse_value->dw_param[0];
        uint16_t handle = p_parse_value->dw_param[1];
        T_GAP_CAUSE cause = GAP_CAUSE_SUCCESS;

        if (p_parse_value->param_count <= 2)
        {
            cause = gcs_attr_read(conn_id, handle);
        }

        return (T_USER_CMD_PARSE_RESULT)cause;
    }
    T_APP_RESULT gcs_client_callback(T_CLIENT_ID client_id, uint8_t conn_id, void *p_data)
    {
        T_APP_RESULT  result = APP_RESULT_SUCCESS;
        APP_PRINT_INFO2("gcs_client_callback: client_id %d, conn_id %d",
                        client_id, conn_id);
        if (client_id == gcs_client_id)
        {
            T_GCS_CLIENT_CB_DATA *p_gcs_cb_data = (T_GCS_CLIENT_CB_DATA *)p_data;
            switch (p_gcs_cb_data->cb_type)
            {
            case GCS_CLIENT_CB_TYPE_READ_RESULT:
                APP_PRINT_INFO3("READ RESULT: cause 0x%4x, handle 0x%4x,  value_len %d",
                                p_gcs_cb_data->cb_content.read_result.cause,
                                p_gcs_cb_data->cb_content.read_result.handle,
                                p_gcs_cb_data->cb_content.read_result.value_size);
                APP_PRINT_INFO1("READ VALUE: %b",
                                TRACE_BINARY(p_gcs_cb_data->cb_content.read_result.value_size,
                                             p_gcs_cb_data->cb_content.read_result.p_value));
                break;
                ......
            }
        }
        return result;
    }
  * \endcode
  */
T_GAP_CAUSE gcs_attr_read(uint8_t conn_id, uint16_t  handle);

/**
  * @brief  Read characteristic by 16 bit UUID request.
  * @param[in]  conn_id       Connection ID
  * @param[in]  start_handle  Start handle of range to be searched.
  * @param[in]  end_handle    End handle of range to be searched.
  * @param[in]  uuid16        Request 16 bit UUID.
  * @retval GAP_CAUSE_SUCCESS Read request success.
  * @retval other             Read request failed.
  *
  * <b>Example usage</b>
  * \code{.c}
    T_USER_CMD_PARSE_RESULT cmd_readu(T_USER_CMD_PARSED_VALUE *p_parse_value)
    {
        ......
        cause = gcs_attr_read_using_uuid16(conn_id, start_handle, end_handle, uuid16);
        return (T_USER_CMD_PARSE_RESULT)cause;
    }
    T_APP_RESULT gcs_client_callback(T_CLIENT_ID client_id, uint8_t conn_id, void *p_data)
    {
        T_APP_RESULT  result = APP_RESULT_SUCCESS;
        APP_PRINT_INFO2("gcs_client_callback: client_id %d, conn_id %d",
                        client_id, conn_id);
        if (client_id == gcs_client_id)
        {
            T_GCS_CLIENT_CB_DATA *p_gcs_cb_data = (T_GCS_CLIENT_CB_DATA *)p_data;
            switch (p_gcs_cb_data->cb_type)
            {
            case GCS_CLIENT_CB_TYPE_READ_RESULT:
                APP_PRINT_INFO3("READ RESULT: cause 0x%4x, handle 0x%4x,  value_len %d",
                                p_gcs_cb_data->cb_content.read_result.cause,
                                p_gcs_cb_data->cb_content.read_result.handle,
                                p_gcs_cb_data->cb_content.read_result.value_size);
                APP_PRINT_INFO1("READ VALUE: %b",
                                TRACE_BINARY(p_gcs_cb_data->cb_content.read_result.value_size,
                                             p_gcs_cb_data->cb_content.read_result.p_value));
                break;
                ......
            }
        }
        return result;
    }
  * \endcode
  */
T_GAP_CAUSE gcs_attr_read_using_uuid16(uint8_t conn_id, uint16_t start_handle, uint16_t end_handle,
                                       uint16_t uuid16);
/**
  * @brief  Read characteristic by 128 bit UUID request.
  * @param[in]  conn_id       Connection ID
  * @param[in]  start_handle  Start handle of range to be searched.
  * @param[in]  end_handle    End handle of range to be searched.
  * @param[in]  p_uuid128     Request 128 bit UUID.
  * @retval GAP_CAUSE_SUCCESS Read request success.
  * @retval other             Read request failed.
  *
  * <b>Example usage</b>
  * \code{.c}
    T_USER_CMD_PARSE_RESULT cmd_readu(T_USER_CMD_PARSED_VALUE *p_parse_value)
    {
        ......
        cause = gcs_attr_read_using_uuid128(conn_id, start_handle, end_handle, uuid128);
        return (T_USER_CMD_PARSE_RESULT)cause;
    }
    T_APP_RESULT gcs_client_callback(T_CLIENT_ID client_id, uint8_t conn_id, void *p_data)
    {
        T_APP_RESULT  result = APP_RESULT_SUCCESS;
        APP_PRINT_INFO2("gcs_client_callback: client_id %d, conn_id %d",
                        client_id, conn_id);
        if (client_id == gcs_client_id)
        {
            T_GCS_CLIENT_CB_DATA *p_gcs_cb_data = (T_GCS_CLIENT_CB_DATA *)p_data;
            switch (p_gcs_cb_data->cb_type)
            {
            case GCS_CLIENT_CB_TYPE_READ_RESULT:
                APP_PRINT_INFO3("READ RESULT: cause 0x%4x, handle 0x%4x,  value_len %d",
                                p_gcs_cb_data->cb_content.read_result.cause,
                                p_gcs_cb_data->cb_content.read_result.handle,
                                p_gcs_cb_data->cb_content.read_result.value_size);
                APP_PRINT_INFO1("READ VALUE: %b",
                                TRACE_BINARY(p_gcs_cb_data->cb_content.read_result.value_size,
                                             p_gcs_cb_data->cb_content.read_result.p_value));
                break;
                ......
            }
        }
        return result;
    }
  * \endcode
  */
T_GAP_CAUSE gcs_attr_read_using_uuid128(uint8_t conn_id, uint16_t start_handle, uint16_t end_handle,
                                        uint8_t *p_uuid128);
/**
  * @brief  Confirm from application when receive indication from server.
  * @param[in]  conn_id       Connection ID indicate which link is.
  * @retval GAP_CAUSE_SUCCESS: Success.
  * @retval other: Failed.
  *
  * <b>Example usage</b>
  * \code{.c}
    T_USER_CMD_PARSE_RESULT cmd_indconf(T_USER_CMD_PARSED_VALUE *p_parse_value)
    {
        T_GAP_CAUSE ret;
        uint8_t conn_id = p_parse_value->dw_param[0];
        ret = gcs_attr_ind_confirm(conn_id);
        return (T_USER_CMD_PARSE_RESULT)ret;
    }
  * \endcode
  */
T_GAP_CAUSE gcs_attr_ind_confirm(uint8_t conn_id);

/**
  * @brief  Write characteristic request.
  * @param[in]  conn_id    Connection ID
  * @param[in]  write_type Type of write.
  * @param[in]  handle     Attribute handle.
  * @param[in]  length     Length of data to be written.
                           If write_type is GATT_WRITE_TYPE_REQ, range of length is from 0 to 512.
                           If write_type is GATT_WRITE_TYPE_CMD, range of length is from 0 to (mtu_size - 3).
                           uint16_t mtu_size is acquired by le_get_conn_param(GAP_PARAM_CONN_MTU_SIZE, &mtu_size, conn_id).
  * @param[in]  p_data     Point to the data to be written.
  * @retval GAP_CAUSE_SUCCESS: Write request success.
  * @retval other: Write request failed.
  *
  * <b>Example usage</b>
  * \code{.c}
    T_USER_CMD_PARSE_RESULT cmd_write(T_USER_CMD_PARSED_VALUE *p_parse_value)
    {
        ......

        T_GAP_CAUSE ret = gcs_attr_write(conn_id, (T_GATT_WRITE_TYPE)write_type, handle,
                                         length, data);
        return (T_USER_CMD_PARSE_RESULT)ret;
    }
    T_APP_RESULT gcs_client_callback(T_CLIENT_ID client_id, uint8_t conn_id, void *p_data)
    {
        T_APP_RESULT  result = APP_RESULT_SUCCESS;
        APP_PRINT_INFO2("gcs_client_callback: client_id %d, conn_id %d",
                        client_id, conn_id);
        if (client_id == gcs_client_id)
        {
            T_GCS_CLIENT_CB_DATA *p_gcs_cb_data = (T_GCS_CLIENT_CB_DATA *)p_data;
            switch (p_gcs_cb_data->cb_type)
            {
            case GCS_CLIENT_CB_TYPE_WRITE_RESULT:
                APP_PRINT_INFO3("WRITE RESULT: cause 0x%4x ,handle 0x%4x, type %d",
                                p_gcs_cb_data->cb_content.write_result.cause,
                                p_gcs_cb_data->cb_content.write_result.handle,
                                p_gcs_cb_data->cb_content.write_result.type);
                break;
            ......
            }
        }

        return result;
    }
  * \endcode
  */
T_GAP_CAUSE gcs_attr_write(uint8_t conn_id, T_GATT_WRITE_TYPE write_type, uint16_t handle,
                           uint16_t length, uint8_t *p_data);

/**
  * @brief  Add general services client to application.
  * @param[in]  app_cb pointer of app callback function to handle specific client module data.
  * @param[in]  link_num initialize link num.
  * @param[in]  config the discovery table number per link.
  * @return Client ID of the specific client module.
  * @retval 0xff failed.
  * @retval other success.
  *
  * <b>Example usage</b>
  * \code{.c}
    void app_le_profile_init(void)
    {
        client_init(1);
        gcs_client_id = gcs_add_client(gcs_client_callback, APP_MAX_LINKS, APP_MAX_DISCOV_TABLE_NUM);
    }
  * \endcode
  */
T_CLIENT_ID gcs_add_client(P_FUN_GENERAL_APP_CB app_cb, uint8_t link_num,
                           uint16_t max_discov_table_size);
#endif
/** @} End of GCS_Client_Exported_Functions */

/** @} End of GCS_Client */

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif  /* _GCS_CLIENT_H_ */
