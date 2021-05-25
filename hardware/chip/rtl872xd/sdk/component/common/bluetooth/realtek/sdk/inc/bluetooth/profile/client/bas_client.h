/**
*****************************************************************************************
*     Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     bas_client.h
  * @brief    Head file for using battery service client.
  * @details  Battery service client data structs and external functions declaration.
  * @author   jane
  * @date     2016-02-18
  * @version  v1.0
  * *************************************************************************************
  */

/* Define to prevent recursive inclusion */
#ifndef _BAS_CLIENT_H_
#define _BAS_CLIENT_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include <bt_flags.h>
#if F_BT_LE_GATT_CLIENT_SUPPORT
/* Add Includes here */
#include <profile_client.h>
#include <stdint.h>
#include <stdbool.h>



/** @defgroup BAS_CLIENT Battery Service Client
  * @brief BAS client
  * @details
     Application shall register bas client when initialization through @ref bas_add_client function.

     Application can start discovery battery service through @ref bas_start_discovery function.

     Application can read battery level characteristic value through @ref bas_read_battery_level function.

     Application can config and read the notification flag through @ref bas_set_notify and @ref bas_read_notify function.

     Application shall handle callback function registered by bas_add_client.
  * \code{.c}
    T_APP_RESULT app_client_callback(T_CLIENT_ID client_id, uint8_t conn_id, void *p_data)
    {
        T_APP_RESULT  result = APP_RESULT_SUCCESS;
        if (client_id == bas_client_id)
        {
            T_BAS_CLIENT_CB_DATA *p_bas_cb_data = (T_BAS_CLIENT_CB_DATA *)p_data;
            switch (p_bas_cb_data->cb_type)
            {
            case BAS_CLIENT_CB_TYPE_DISC_STATE:
                switch (p_bas_cb_data->cb_content.disc_state)
                {
                case DISC_BAS_DONE:
                ......
        }
    }
  * \endcode
  * @{
  */

/*============================================================================*
 *                         Macros
 *============================================================================*/
/** @addtogroup BAS_CLIENT_Exported_Macros BAS Client Exported Macros
  * @brief
  * @{
  */

/** @brief  Define links number. range: 0-4 */
#define BAS_MAX_LINKS  4

/** End of BAS_CLIENT_Exported_Macros
* @}
*/


/*============================================================================*
 *                         Types
 *============================================================================*/
/** @defgroup BAS_CLIENT__Exported_Types BAS Client Exported Types
  * @brief
  * @{
  */

/** @brief BAS client handle type*/
typedef enum
{
    HDL_BAS_SRV_START,           //!< start handle of battery service
    HDL_BAS_SRV_END,             //!< end handle of battery service
    HDL_BAS_BATTERY_LEVEL,       //!< battery level characteristic value handle
    HDL_BAS_BATTERY_LEVEL_CCCD,  //!< battery level characteristic CCCD handle
    HDL_BAS_CACHE_LEN            //!< handle cache length
} T_BAS_HANDLE_TYPE;

/** @brief BAS client discovery state*/
typedef enum
{
    DISC_BAS_IDLE,
    DISC_BAS_START,
    DISC_BAS_DONE,
    DISC_BAS_FAILED
} T_BAS_DISC_STATE;

/** @brief BAS client notification data struct*/
typedef struct
{
    uint8_t battery_level;
} T_BAS_NOTIFY_DATA;

/** @brief BAS client write type*/
typedef enum
{
    BAS_WRITE_NOTIFY_ENABLE,
    BAS_WRITE_NOTIFY_DISABLE,
} T_BAS_WRTIE_TYPE;

/** @brief BAS client write result*/
typedef struct
{
    T_BAS_WRTIE_TYPE type;
    uint16_t cause;
} T_BAS_WRITE_RESULT;

/** @brief BAS client read data */
typedef union
{
    uint8_t battery_level;
    bool notify;
} T_BAS_READ_DATA;

/** @brief BAS client read type*/
typedef enum
{
    BAS_READ_NOTIFY,
    BAS_READ_BATTERY_LEVEL,
} T_BAS_READ_TYPE;

/** @brief BAS client read result*/
typedef struct
{
    T_BAS_READ_TYPE type;
    T_BAS_READ_DATA data;
    uint16_t cause;
} T_BAS_READ_RESULT;

/** @brief BAS client callback type*/
typedef enum
{
    BAS_CLIENT_CB_TYPE_DISC_STATE,          //!< Discovery procedure state, done or pending.
    BAS_CLIENT_CB_TYPE_READ_RESULT,         //!< Read request's result data, responsed from server.
    BAS_CLIENT_CB_TYPE_WRITE_RESULT,        //!< Write request result, success or fail.
    BAS_CLIENT_CB_TYPE_NOTIF_IND_RESULT,    //!< Notification or indication data received from server.
    BAS_CLIENT_CB_TYPE_INVALID              //!< Invalid callback type, no practical usage.
} T_BAS_CLIENT_CB_TYPE;

/** @brief BAS client callback content*/
typedef union
{
    T_BAS_DISC_STATE disc_state;
    T_BAS_READ_RESULT read_result;
    T_BAS_NOTIFY_DATA notify_data;
    T_BAS_WRITE_RESULT write_result;
} T_BAS_CLIENT_CB_CONTENT;

/** @brief BAS client callback data*/
typedef struct
{
    T_BAS_CLIENT_CB_TYPE     cb_type;
    T_BAS_CLIENT_CB_CONTENT  cb_content;
} T_BAS_CLIENT_CB_DATA;
/** End of BAS_CLIENT_Exported_Types
* @}
*/


/*============================================================================*
 *                         Functions
 *============================================================================*/
/** @defgroup BAS_CLIENT_Exported_Functions BAS Client Exported Functions
  * @brief
  * @{
  */

/**
  * @brief      Add bas client to application.
  * @param[in]  app_cb pointer of app callback function to handle specific client module data.
  * @param[in]  link_num initialize link num.
  * @return Client ID of the specific client module.
  * @retval 0xff failed.
  * @retval other success.
  *
  * <b>Example usage</b>
  * \code{.c}
    void app_le_profile_init(void)
    {
        client_init(1);
        bas_client_id = bas_add_client(app_client_callback, APP_MAX_LINKS);
    }
  * \endcode
  */
T_CLIENT_ID bas_add_client(P_FUN_GENERAL_APP_CB app_cb, uint8_t link_num);

/**
  * @brief  Used by application, to start the discovery procedure of battery service.
  * @param[in]  conn_id connection ID.
  * @retval true send request to upper stack success.
  * @retval false send request to upper stack failed.
  *
  * <b>Example usage</b>
  * \code{.c}
    static T_USER_CMD_PARSE_RESULT cmd_basdis(T_USER_CMD_PARSED_VALUE *p_parse_value)
    {
        uint8_t conn_id = p_parse_value->dw_param[0];
        bool ret = bas_start_discovery(conn_id);
        ......
    }
  * \endcode
  */
bool bas_start_discovery(uint8_t conn_id);

/**
  * @brief  Used by application, to set the notification flag.
  * @param[in]  conn_id connection ID.
  * @param[in]  notify value to enable or disable notify.
  * @retval true send request to upper stack success.
  * @retval false send request to upper stack failed.
  *
  * <b>Example usage</b>
  * \code{.c}
    static T_USER_CMD_PARSE_RESULT cmd_bascccd(T_USER_CMD_PARSED_VALUE *p_parse_value)
    {
        uint8_t conn_id = p_parse_value->dw_param[0];
        bool notify = p_parse_value->dw_param[1];
        bool ret;
        ret = bas_set_notify(conn_id, notify);
        ......
    }
  * \endcode
  */
bool bas_set_notify(uint8_t conn_id, bool notify);

/**
  * @brief  Used by application, to read the notification flag.
  * @param[in]  conn_id connection ID.
  * @retval true send request to upper stack success.
  * @retval false send request to upper stack failed.
  *
  * <b>Example usage</b>
  * \code{.c}
    static T_USER_CMD_PARSE_RESULT cmd_basread(T_USER_CMD_PARSED_VALUE *p_parse_value)
    {
        uint8_t conn_id = p_parse_value->dw_param[0];
        bool ret = false;
        ret = bas_read_notify(conn_id);
        ......
    }
  * \endcode
  */
bool bas_read_notify(uint8_t conn_id);

/**
  * @brief  Used by application, to read battery level.
  * @param[in]  conn_id connection ID.
  * @retval true send request to upper stack success.
  * @retval false send request to upper stack failed.
  *
  * <b>Example usage</b>
  * \code{.c}
    static T_USER_CMD_PARSE_RESULT cmd_basread(T_USER_CMD_PARSED_VALUE *p_parse_value)
    {
        uint8_t conn_id = p_parse_value->dw_param[0];
        bool ret = false;
        ret = bas_read_battery_level(conn_id);
        ......
    }
  * \endcode
  */
bool bas_read_battery_level(uint8_t conn_id);

/**
  * @brief  Used by application, to get handle cache.
  * @param[in]  conn_id connection ID.
  * @param[in]  p_hdl_cache pointer of the handle cache table
  * @param[in]  len the length of handle cache table
  * @retval true success.
  * @retval false failed.
  *
  * <b>Example usage</b>
  * \code{.c}
    static T_USER_CMD_PARSE_RESULT cmd_bashdl(T_USER_CMD_PARSED_VALUE *p_parse_value)
    {
        uint8_t conn_id = p_parse_value->dw_param[0];
        uint16_t hdl_cache[HDL_BAS_CACHE_LEN];
        bool ret = bas_get_hdl_cache(conn_id, hdl_cache,
                                     sizeof(uint16_t) * HDL_BAS_CACHE_LEN);

        ......
    }
  * \endcode
  */
bool bas_get_hdl_cache(uint8_t conn_id, uint16_t *p_hdl_cache, uint8_t len);

/**
  * @brief  Used by application, to set handle cache.
  * @param[in]  conn_id connection ID.
  * @param[in]  p_hdl_cache pointer of the handle cache table
  * @param[in]  len the length of handle cache table
  * @retval true success.
  * @retval false failed.
  *
  * <b>Example usage</b>
  * \code{.c}
    void app_discov_services(uint8_t conn_id, bool start)
    {
        ......
        if (app_srvs_table.srv_found_flags & APP_DISCOV_BAS_FLAG)
        {
            bas_set_hdl_cache(conn_id, app_srvs_table.bas_hdl_cache, sizeof(uint16_t) * HDL_BAS_CACHE_LEN);
        }
        ......
    }
  * \endcode
  */
bool bas_set_hdl_cache(uint8_t conn_id, uint16_t *p_hdl_cache, uint8_t len);

/** @} End of BAS_CLIENT_Exported_Functions */

/** @} End of BAS_CLIENT */


#endif

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif  /* _BAS_CLIENT_H_ */
