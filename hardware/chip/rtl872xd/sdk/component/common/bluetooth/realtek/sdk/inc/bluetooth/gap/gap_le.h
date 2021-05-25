/**
*****************************************************************************************
*     Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    gap_le.h
  * @brief   This file contains all the constants and functions prototypes for GAP protocol.
  * @details
  * @author  jane
  * @date    2016-02-18
  * @version v1.0
  * *************************************************************************************
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef GAP_LE_H
#define GAP_LE_H

#ifdef __cplusplus
extern "C"
{
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <bt_flags.h>
#include <gap.h>
#include <gap_callback_le.h>


/** @addtogroup GAP_LE_COMMON GAP LE Common Module
  * @brief GAP LE common module
  * @{
  */

/*============================================================================*
 *                         Types
 *============================================================================*/
/** @defgroup GAP_LE_Exported_Types GAP LE Exported Types
  * @brief
  * @{
  */
/** @brief  GAP LE Parameter Types List.*/
typedef enum
{
    GAP_PARAM_DEV_STATE                = 0x220, //!< Device's current GAP device state. Read/Write. Size is sizeof(TGapDevState).
    GAP_PARAM_APPEARANCE               = 0x221, //!< Local Device's Appearance. Read/Write. size is uint16. Appearance value please refer to GAP Appearance Values.(@ref GAP_LE_APPEARANCE_VALUES)
    GAP_PARAM_DEVICE_NAME              = 0x222, //!< Local Device's Name. Write Only. Name string length is GAP_DEVICE_NAME_LEN.
#if F_BT_LE_GAP_PERIPHERAL_SUPPORT
    GAP_PARAM_SLAVE_INIT_GATT_MTU_REQ  = 0x223, //!< Slave initiate the GATT exchange MTU procedure. Write Only.
#endif
    GAP_PARAM_RANDOM_ADDR              = 0x224, //!< Random address. Write Only.
    GAP_PARAM_LATEST_CONN_BD_ADDR      = 0x226, //!< Latest connected bluetooth devive address. Read Only.
    GAP_PARAM_LATEST_CONN_BD_ADDR_TYPE = 0x227, //!< Latest connected bluetooth devive address type. Read Only.
    GAP_PARAM_HANDLE_CREATE_CONN_IND   = 0x228, //!< App handle the create connection indication message.
#if F_BT_LE_5_0_SET_PHYS_SUPPORT
    GAP_PARAM_DEFAULT_PHYS_PREFER      = 0x229, //!< Preferred values for the transmitter PHY and receiver PHY to be used for all subsequent connections over the LE transport.
    GAP_PARAM_DEFAULT_TX_PHYS_PREFER   = 0x22a, //!< The transmitter PHYs that the Host prefers the Controller to use.
    GAP_PARAM_DEFAULT_RX_PHYS_PREFER   = 0x22b, //!< The receiver PHYs that the Host prefers the Controller to use.
#endif
#if F_BT_LE_4_2_DATA_LEN_EXT_SUPPORT
    GAP_PARAM_DEFAULT_DATA_LEN_MAX_TX_OCTETS = 0x22e,
    GAP_PARAM_DEFAULT_DATA_LEN_MAX_TX_TIME   = 0x22f,
#endif
    GAP_PARAM_LOCAL_FEATURES           = 0x230, //!< Local supported features.
    GAP_PARAM_DS_POOL_ID               = 0x231, //!< Downstream PoolID.  Read only.  size is uint16.
    GAP_PARAM_DS_DATA_OFFSET           = 0x232, //!< Downstream pool buffer data offset.  Read only.  size is uint16.
    GAP_PARAM_LE_REMAIN_CREDITS        = 0x233, //!< Remain credits avaiable for TX.  Read only.  size is uint16.
    GAP_PARAM_MAX_WL_SIZE              = 0x234, //!< Max white list size.  Read only.  size is uint16.
} T_GAP_LE_PARAM_TYPE;

/**
  * @brief Callback for gap le to notify app
  * @param[in] cb_type callback msy type @ref GAP_LE_MSG_Types.
  * @param[in] p_cb_data point to callback data @ref T_LE_CB_DATA.
  * @retval result @ref T_APP_RESULT
  */
typedef T_APP_RESULT(*P_FUN_LE_APP_CB)(uint8_t cb_type, void *p_cb_data);
/** End of GAP_LE_Exported_Types
  * @}
  */


/*============================================================================*
 *                         Functions
 *============================================================================*/
/**
 * @defgroup GAP_LE_COMMON_Exported_Functions GAP LE Common Exported Functions
 *
 * @{
 */

/**
  * @brief  Initialize parameters of GAP.
  * @param[in] link_num Initialize link number.
  * @retval true Success.
  * @retval false Failed because of invalid parameter.
 *
 * <b>Example usage</b>
 * \code{.c}
    int main(void)
    {
        board_init();
        driver_init();
        le_gap_init(1);
        app_le_gap_init();
        app_le_profile_init();
        pwr_mgr_init();
        task_init();
        os_sched_start();

        return 0;
    }
 * \endcode
 */
bool        le_gap_init(uint8_t link_num);

/**
  * @brief  Set gap message inform way.
  *
  * Default value is true.
  * 1. When use_msg is true, gap will send the gap message to io_queue registered by gap_start_bt_stack.
  *    Message type is @ref GAP_MSG_TYPE.
  * 2. When use_msg is false, gap will send the gap message using callback function registered by le_register_app_cb.
  *    Message type is @ref GAP_MSG_LE_GAP_STATE_MSG.
  *
  * @param[in] use_msg Whether to use message.
  * @retval void
 *
 * <b>Example usage</b>
 * \code{.c}
    int test(void)
    {
        le_gap_msg_info_way(false);
    }
    T_APP_RESULT app_gap_callback(uint8_t cb_type, void *p_cb_data)
    {
        T_APP_RESULT result = APP_RESULT_SUCCESS;
        T_LE_CB_DATA cb_data;
        memcpy(&cb_data, p_cb_data, sizeof(T_LE_CB_DATA));
        APP_PRINT_TRACE1("app_gap_callback: msgType = %d", cb_type);
        switch (cb_type)
        {
        ...
        case GAP_MSG_LE_GAP_STATE_MSG:
            APP_PRINT_INFO0("GAP_MSG_LE_GAP_STATE_MSG");
            app_handle_gap_msg((T_IO_MSG *)cb_data->p_gap_state_msg);
            break;
        ...
        }
    }

 * \endcode
 */
void        le_gap_msg_info_way(bool use_msg);

/**
  * @brief  Get max supported le link count.
  * @retval max_link_num Max supported le link count.
 *
 * <b>Example usage</b>
 * \code{.c}
    void test(void)
    {
        link_num = le_get_max_link_num();
        le_gap_init(link_num);
    }
 * \endcode
 */
uint8_t     le_get_max_link_num(void);

/**
  * @brief  Register app callback from gap le.
  * @param[in]  app_callback callback
  * @return void
 *
 * <b>Example usage</b>
 * \code{.c}
    void app_le_profile_init(void)
    {
        le_register_app_cb(app_gap_callback);
        client_init(1);
        simple_ble_client_id = simp_ble_add_client(app_client_callback);
        client_register_general_client_cb(app_client_callback);
    }
 * \endcode
 */
void        le_register_app_cb(P_FUN_LE_APP_CB app_callback);

/**
  * @brief  Set a GAP Common parameter.
  *
  *         NOTE: You can call this function with a GAP Common Parameter ID and it will set the
  *         GAP Parameter.  GAP Peripheral Parameters are defined in (gap.h).
  *         If the "len" field sets to the size of a "uint16_t" ,the
  *         "p_value" field must point to a data with type of "uint16_t".
  *
  * @param[in]  param Profile parameter ID: @ref T_GAP_LE_PARAM_TYPE
  * @param[in]  len Length of data to write
  * @param[in]  p_value Pointer to data to write.  This is dependent on
  *                  the parameter ID and WILL be cast to the appropriate
  *                  data type (For example: if data type param is uint16, p_value will be cast to
  *                  pointer of uint16_t).
  * @return Set result
  * @retval GAP_CAUSE_SUCCESS Set parameter success.
  * @retval other Set parameter failed.
  *
  * <b>Example usage</b>
  * \code{.c}
    void app_le_gap_init(void)
    {
        ...
        //device name and device appearance
        uint8_t  device_name[GAP_DEVICE_NAME_LEN] = "BLE_PERIP";
        uint16_t appearance = GAP_GATT_APPEARANCE_UNKNOWN;

        //Set device name and device appearance
        le_set_gap_param(GAP_PARAM_DEVICE_NAME, GAP_DEVICE_NAME_LEN, device_name);
        le_set_gap_param(GAP_PARAM_APPEARANCE, sizeof(appearance), &appearance);
        ...
    }
  * \endcode
  */
T_GAP_CAUSE le_set_gap_param(T_GAP_LE_PARAM_TYPE param, uint8_t len, void *p_value);

/**
  * @brief  Get a GAP Common parameter.
  *
  *         NOTE: You can call this function with a GAP Common Parameter ID and it will get a
  *         GAP Common Parameter.  GAP Common Parameters are defined in (gap.h).  Also, the
  *         "p_value" field must point to a "uint16".
  *
  * @param[in]  param Profile parameter ID: @ref T_GAP_LE_PARAM_TYPE
  * @param[in,out] p_value Pointer to location to get the parameter value.  This is dependent on
  *                  the parameter ID and will be cast to the appropriate
  *                  data type (For example: if data type of param is uint16_t, p_value will be cast to
 *                   pointer of uint16_t).
  *
  * @return Get result
  * @retval GAP_CAUSE_SUCCESS Get parameter success.
  * @retval other Get parameter failed.
  *
  * <b>Example usage</b>
  * \code{.c}
    void test(void)
    {
        uint8_t  addr_type = GAP_REMOTE_ADDR_LE_PUBLIC;
        uint8_t  addr[GAP_BD_ADDR_LEN] = {0};

        //get connected device address and address type
        le_get_gap_param(GAP_PARAM_LATEST_CONN_BD_ADDR_TYPE, &addr_type);
        le_get_gap_param(GAP_PARAM_LATEST_CONN_BD_ADDR, &addr);
    }
  * \endcode
  */
T_GAP_CAUSE le_get_gap_param(T_GAP_LE_PARAM_TYPE param, void *p_value);

/**
  * @brief  Modify local white list.
  *         NOTE: You can call this function to add, remove or clear whist list.
  *               This function can be called after @ref gap_start_bt_stack is invoked.
  *
  * @param[in] operation Bluetooth Device Random Address type, @ref T_GAP_RAND_ADDR_TYPE.
  * @param[in] bd_addr Bluetooth Device Address.
  * @param[in] bd_type Bluetooth Device Address type.
  * @retval GAP_CAUSE_SUCCESS Operation success.
  * @retval GAP_CAUSE_SEND_REQ_FAILED Operation failure.
  *
  * <b>Example usage</b>
  * \code{.c}
    void test()
    {
        T_GAP_WHITE_LIST_OP operation = GAP_WHITE_LIST_OP_ADD;
        uint8_t *bd_addr = {0};
        T_GAP_REMOTE_ADDR_TYPE bd_type = GAP_REMOTE_ADDR_LE_PUBLIC;
        le_modify_white_list(operation, bd_addr, bd_type);
    }
    T_APP_RESULT app_gap_callback(uint8_t cb_type, void *p_cb_data)
    {
        T_APP_RESULT result = APP_RESULT_SUCCESS;
        T_LE_CB_DATA cb_data;
        memcpy(&cb_data, p_cb_data, sizeof(T_LE_CB_DATA));
        APP_PRINT_TRACE1("app_gap_callback: cb_type = %d", cb_type);
        switch (cb_type)
        {
        ...
        case GAP_MSG_LE_MODIFY_WHITE_LIST:
           APP_PRINT_INFO2("GAP_MSG_LE_MODIFY_WHITE_LIST: operation  0x%x, cause 0x%x",
                           cb_data.p_le_modify_white_list_rsp->operation,
                           cb_data.p_le_modify_white_list_rsp->cause);
           break;
        }
        ...
    }
  * \endcode
  */
T_GAP_CAUSE le_modify_white_list(T_GAP_WHITE_LIST_OP operation, uint8_t *bd_addr,
                                 T_GAP_REMOTE_ADDR_TYPE bd_type);

/**
  * @brief    Generate local random address.
  * @param[in] rand_addr_type Bluetooth Device Random Address type, @ref T_GAP_RAND_ADDR_TYPE.
  * @param[in,out] random_bd pointer to Bluetooth Device Address.
  * @retval GAP_CAUSE_SUCCESS Operation success.
  * @retval GAP_CAUSE_SEND_REQ_FAILED Operation failure.
  *
  * <b>Example usage</b>
  * \code{.c}
    void test()
    {
        T_GAP_RAND_ADDR_TYPE rand_addr_type = GAP_RAND_ADDR_RESOLVABLE;
        uint8_t random_bd[BD_ADDR_SIZE] = {0};
        le_gen_rand_addr(rand_addr_type, random_bd);
    }
  *
  * \endcode
  */
T_GAP_CAUSE le_gen_rand_addr(T_GAP_RAND_ADDR_TYPE rand_addr_type, uint8_t *random_bd);

/**
  * @brief    Set local random address.
  *
  *           NOTE: This function can be called after @ref gap_start_bt_stack is invoked.
  *
  * @param[in] random_bd Bluetooth Device Address.
  * @retval GAP_CAUSE_SUCCESS Operation success.
  * @retval GAP_CAUSE_SEND_REQ_FAILED Operation failure.
  *
  * <b>Example usage</b>
  * \code{.c}
    void test()
    {
        uint8_t random_bd[BD_ADDR_SIZE] = {0};
        le_set_rand_addr(random_bd);
    }
    T_APP_RESULT app_gap_callback(uint8_t cb_type, void *p_cb_data)
    {
        T_APP_RESULT result = APP_RESULT_SUCCESS;
        T_LE_CB_DATA cb_data;
        memcpy(&cb_data, p_cb_data, sizeof(T_LE_CB_DATA));
        APP_PRINT_TRACE1("app_gap_callback: cb_type = %d", cb_type);
        switch (cb_type)
        {
        ...
        case GAP_MSG_LE_SET_RAND_ADDR:
            APP_PRINT_INFO1("GAP_MSG_LE_SET_RAND_ADDR: cause 0x%x",
                       cb_data.p_le_set_rand_addr_rsp->cause);
        break;
        }
        ...
    }
  * \endcode
  */
T_GAP_CAUSE le_set_rand_addr(uint8_t *random_bd);

/**
  * @brief    Configure local identity address.
  *
  *           NOTE: This function can be called before @ref gap_start_bt_stack is invoked.
  *
  * @param[in] addr Bluetooth Device Address.
  * @param[in] type Bluetooth Device Address type.
  * @retval GAP_CAUSE_SUCCESS Operation success.
  * @retval GAP_CAUSE_SEND_REQ_FAILED Operation failure.
  *
  * <b>Example usage</b>
  * \code{.c}
    void test()
    {
        T_GAP_IDENT_ADDR_TYPE type = GAP_IDENT_ADDR_PUBLIC;
        uint8_t addr[BD_ADDR_SIZE] = {0};
        le_cfg_local_identity_address(addr, type);
    }
  * \endcode
  */
T_GAP_CAUSE le_cfg_local_identity_address(uint8_t *addr, T_GAP_IDENT_ADDR_TYPE type);

/**
* @brief   Set the Host Channel Classification for master role.
*
*          NOTE: This function can be called after @ref gap_start_bt_stack is invoked.
*
* @param[in]   p_channel_map Channel bit map, the most 3 significant bits are reserved.
*
* @retval GAP_CAUSE_SUCCESS: Operation success.
* @retval GAP_CAUSE_SEND_REQ_FAILED: Operation fail.
  *
  * <b>Example usage</b>
  * \code{.c}
    void test()
    {
        uint8_t channel_map = 0;
        le_set_host_chann_classif(channel_map);
    }
    T_APP_RESULT app_gap_callback(uint8_t cb_type, void *p_cb_data)
    {
        T_APP_RESULT result = APP_RESULT_SUCCESS;
        T_LE_CB_DATA cb_data;
        memcpy(&cb_data, p_cb_data, sizeof(T_LE_CB_DATA));
        APP_PRINT_TRACE1("app_gap_callback: cb_type = %d", cb_type);
        switch (cb_type)
        {
        ...
        case GAP_MSG_LE_SET_HOST_CHANN_CLASSIF:
           APP_PRINT_INFO1("GAP_MSG_LE_SET_HOST_CHANN_CLASSIF: cause 0x%x",
                           cb_data.p_le_set_host_chann_classif_rsp->cause);
           break;
        }
        ...
    }
  * \endcode
  */
#if F_BT_LE_GAP_CENTRAL_SUPPORT
T_GAP_CAUSE le_set_host_chann_classif(uint8_t *p_channel_map);
#endif
/**
* @brief   Specify suggested values for maximum transmission number of payload
           octets and maximum packet transmission time for new connections.
*
*          NOTE: This function can be called after @ref gap_start_bt_stack is invoked.
*
* @param[in]   tx_octets maximum transmission number of payload octets, range: 0x001B-0x00FB
* @param[in]   tx_time   maximum packet transmission time, range: 0x0148-0x4290
*
* @retval GAP_CAUSE_SUCCESS: Operation success.
* @retval GAP_CAUSE_SEND_REQ_FAILED: Operation fail.
  *
  * <b>Example usage</b>
  * \code{.c}
    void test()
    {
        uint16_t tx_octets = 0x00FB;
        uint16_t tx_time = 0x0848;
        le_write_default_data_len(tx_octets, tx_time);
    }
    T_APP_RESULT app_gap_callback(uint8_t cb_type, void *p_cb_data)
    {
        T_APP_RESULT result = APP_RESULT_SUCCESS;
        T_LE_CB_DATA cb_data;
        memcpy(&cb_data, p_cb_data, sizeof(T_LE_CB_DATA));
        APP_PRINT_TRACE1("app_gap_callback: cb_type = %d", cb_type);
        switch (cb_type)
        {
        ...
        case GAP_MSG_LE_WRITE_DEFAULT_DATA_LEN:
           APP_PRINT_INFO1("GAP_MSG_LE_WRITE_DEFAULT_DATA_LEN: cause 0x%x",
                           cb_data->le_cause.cause);
           break;
        }
        ...
    }
  * \endcode
  */
#if F_BT_LE_4_2_DATA_LEN_EXT_SUPPORT
T_GAP_CAUSE le_write_default_data_len(uint16_t tx_octets, uint16_t tx_time);
#endif
/** @} */ /* End of group GAP_LE_COMMON_Exported_Functions */
/** @} */ /* End of group GAP_LE_COMMON */


/*-------------------------------------------------------------------
-------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* GAP_LE_H */




