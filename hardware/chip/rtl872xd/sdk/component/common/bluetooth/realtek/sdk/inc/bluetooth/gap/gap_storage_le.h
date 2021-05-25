/**
*********************************************************************************************************
*               Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      gap_storage_le.h
* @brief     key storage function.
* @details
* @author    jane
* @date      2016-02-18
* @version   v1.0
* *********************************************************************************************************
*/

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef     FLASH_KEY_MGR_LE_H
#define     FLASH_KEY_MGR_LE_H

#ifdef __cplusplus
extern "C"
{
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <bt_flags.h>
#include <gap.h>
#include <gap_le_types.h>

/** @defgroup GAP_LE_STORAGE GAP LE Storage
  * @brief GAP LE Storage
  * @{
  */

/*============================================================================*
 *                         Macros
 *============================================================================*/
/** @defgroup GAP_LE_STORAGE_Exported_Macros GAP LE Storage Exported Macros
  * @{
  */

/** @defgroup GAP_LE_STORAGE_BITS LE Key Storage Bits
* @{
 */
#define LE_KEY_STORE_REMOTE_BD_BIT   0x01
#define LE_KEY_STORE_LOCAL_LTK_BIT   0x02
#define LE_KEY_STORE_REMOTE_LTK_BIT  0x04
#define LE_KEY_STORE_REMOTE_IRK_BIT  0x08
#define LE_KEY_STORE_LOCAL_CSRK_BIT  0x10
#define LE_KEY_STORE_REMOTE_CSRK_BIT 0x20
#define LE_KEY_STORE_CCCD_DATA_BIT   0x40
#define LE_KEY_STORE_LOCAL_IRK_BIT   0x80
/**
  * @}
  */

/** End of GAP_LE_STORAGE_Exported_Macros
  * @}
  */

/*============================================================================*
 *                         Types
 *============================================================================*/
/** @defgroup GAP_LE_STORAGE_Exported_Types GAP LE Storage Exported Types
  * @{
  */

/** @brief Local Device Name */
typedef struct
{
    uint8_t  local_name[40];
} T_LOCAL_NAME;

/** @brief Local Device Appearance */
typedef struct
{
    uint16_t local_appearance;
    uint8_t  padding[2];
} T_LOCAL_APPEARANCE;

/** @brief Local IRK */
typedef struct
{
    uint8_t local_irk[16];
} T_LOCAL_IRK;

/** @brief Remote Bluetooth device address info */
typedef struct
{
    uint8_t  addr[6];
    uint8_t  remote_bd_type;
    uint8_t  bond_flags;
} T_LE_REMOTE_BD;

#if F_BT_LE_PRIVACY_SUPPORT
/** @brief LE privacy information */
typedef struct
{
    bool is_discov;
    bool central_addr_resolv;
    bool resolv_addr_only;
} T_LE_PRIVACY_INFO;
#endif

/** @brief LE CCCD info */
typedef struct
{
    uint8_t data_length;
    uint8_t padding[3];
    uint8_t data[1];
} T_LE_CCCD;

/** @brief LE Key Type */
typedef enum
{
    LE_KEY_UNAUTHEN              = 0x04, /**< SSP generated link key without MITM protection. */
    LE_KEY_AUTHEN                = 0x05, /**< SSP generated link key with MITM protection. */
    LE_KEY_UNAUTHEN_P256         = 0x07, /**< Security Connections generated link key without MITM protection. */
    LE_KEY_AUTHEN_P256           = 0x08, /**< Security Connections link key with MITM protection. */
} T_LE_KEY_TYPE;

/** @brief LE key entry */
typedef struct
{
    bool is_used;
    uint8_t idx;
    uint16_t flags;
    uint8_t local_bd_type;
    uint8_t app_data;
    uint8_t reserved[2];
    T_LE_REMOTE_BD remote_bd;
    T_LE_REMOTE_BD resolved_remote_bd;
} T_LE_KEY_ENTRY;

extern uint8_t bond_storage_num;

/** End of GAP_LE_STORAGE_Exported_Types
  * @}
  */

/*============================================================================*
 *                         Functions
 *============================================================================*/
/**
 * @defgroup GAP_LE_STORAGE_EXPORT_Functions GAP LE Storage Exported Functions
 *
 * @{
 */

/**
 * @brief  Save local device name to flash.
 * @param[in] p_data pointer to local device name
 * @return Operation result.
 * @retval 0 Operation success.
 * @retval Others Operation failure.
 *
 * <b>Example usage</b>
 * \code{.c}
    T_APP_RESULT gap_service_callback(T_SERVER_ID service_id, void *p_para)
    {
        T_APP_RESULT  result = APP_RESULT_SUCCESS;
        T_GAPS_CALLBACK_DATA *p_gap_data = (T_GAPS_CALLBACK_DATA *)p_para;
        APP_PRINT_INFO2("gap_service_callback conn_id = %d msg_type = %d\n", p_gap_data->conn_id,
                       p_gap_data->msg_type);
        if (p_gap_data->msg_type == SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE)
        {
            switch (p_gap_data->msg_data.opcode)
            {
            case GAPS_WRITE_DEVICE_NAME:
               {
                    T_LOCAL_NAME device_name;
                    memcpy(device_name.local_name, p_gap_data->msg_data.p_value, p_gap_data->msg_data.len);
                    device_name.local_name[p_gap_data->msg_data.len] = 0;
                    flash_save_local_name(&device_name);
               }
               break;

            case GAPS_WRITE_APPEARANCE:
               {
                    uint16_t appearance_val;
                    T_LOCAL_APPEARANCE appearance;

                    LE_ARRAY_TO_UINT16(appearance_val, p_gap_data->msg_data.p_value);
                    appearance.local_appearance = appearance_val;
                    flash_save_local_appearance(&appearance);
               }
               break;

            default:
               break;
            }
        }
        else if (p_gap_data->msg_type == SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION)
        {
            if (p_gap_data->msg_data.opcode == GATT_SERVICE_CHANGE_CCCD_ENABLE)
            {
                APP_PRINT_INFO0("GATT_SERVICE_CHANGE_CCCD_ENABLE");
            }
        }
        return result;
    }
 * \endcode
 */
uint32_t flash_save_local_name(T_LOCAL_NAME *p_data);

/**
 * @brief  Load local device name from flash.
 * @param[in] p_data pointer to local device name
 * @return Operation result.
 * @retval 0 Operation success.
 * @retval Others Operation failure.
 *
 * <b>Example usage</b>
 * \code{.c}
    void test()
    {
        uint8_t device_name[GAP_DEVICE_NAME_LEN] = "BB3_GapTest";
        uint16_t appearance = GAP_GATT_APPEARANCE_UNKNOWN;
        uint8_t appearance_prop = GAPS_PROPERTY_WRITE_ENABLE;
        uint8_t device_name_prop = GAPS_PROPERTY_WRITE_ENABLE;
        T_LOCAL_APPEARANCE appearance_local;
        T_LOCAL_NAME local_device_name;
        if (flash_load_local_appearance(&appearance_local) == 0)
        {
            if (appearance_local.local_appearance != 0xffff)
            {
                appearance = appearance_local.local_appearance;
            }
        }

        if (flash_load_local_name(&local_device_name) == 0)
        {
            if (local_device_name.local_name[0] != 0xff)
            {
                memcpy(device_name, local_device_name.local_name, GAP_DEVICE_NAME_LEN);
            }
        }
    }
 * \endcode
 */
uint32_t flash_load_local_name(T_LOCAL_NAME *p_data);

/**
 * @brief  Save local device appearance to flash.
 * @param[in] p_data Pointer to local device name
 * @return Operation result
 * @retval 0 Operation success.
 * @retval Others Operation failure.
 *
 * <b>Example usage</b>
 * \code{.c}
   T_APP_RESULT gap_service_callback(T_SERVER_ID service_id, void *p_para)
   {
       T_APP_RESULT  result = APP_RESULT_SUCCESS;
       T_GAPS_CALLBACK_DATA *p_gap_data = (T_GAPS_CALLBACK_DATA *)p_para;
       APP_PRINT_INFO2("gap_service_callback conn_id = %d msg_type = %d\n", p_gap_data->conn_id,
                       p_gap_data->msg_type);
       if (p_gap_data->msg_type == SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE)
       {
           switch (p_gap_data->msg_data.opcode)
           {
           case GAPS_WRITE_DEVICE_NAME:
               {
                   T_LOCAL_NAME device_name;
                   memcpy(device_name.local_name, p_gap_data->msg_data.p_value, p_gap_data->msg_data.len);
                   device_name.local_name[p_gap_data->msg_data.len] = 0;
                   flash_save_local_name(&device_name);
               }
               break;

           case GAPS_WRITE_APPEARANCE:
               {
                   uint16_t appearance_val;
                   T_LOCAL_APPEARANCE appearance;

                   LE_ARRAY_TO_UINT16(appearance_val, p_gap_data->msg_data.p_value);
                   appearance.local_appearance = appearance_val;
                   flash_save_local_appearance(&appearance);
               }
               break;

           default:
               break;
           }
       }
       else if (p_gap_data->msg_type == SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION)
       {
           if (p_gap_data->msg_data.opcode == GATT_SERVICE_CHANGE_CCCD_ENABLE)
           {
               APP_PRINT_INFO0("GATT_SERVICE_CHANGE_CCCD_ENABLE");
           }
       }
       return result;
   }
 * \endcode
 */
uint32_t flash_save_local_appearance(T_LOCAL_APPEARANCE *p_data);

/**
 * @brief  Load local device appearance from flash.
 * @param[in] p_data Pointer to local device name
 * @return Operation result
 * @retval 0 Operation success.
 * @retval Others Operation failure.
 *
 * <b>Example usage</b>
 * \code{.c}
    void test()
    {
        uint8_t device_name[GAP_DEVICE_NAME_LEN] = "BB3_GapTest";
        uint16_t appearance = GAP_GATT_APPEARANCE_UNKNOWN;
        uint8_t appearance_prop = GAPS_PROPERTY_WRITE_ENABLE;
        uint8_t device_name_prop = GAPS_PROPERTY_WRITE_ENABLE;
        T_LOCAL_APPEARANCE appearance_local;
        T_LOCAL_NAME local_device_name;
        if (flash_load_local_appearance(&appearance_local) == 0)
        {
            if (appearance_local.local_appearance != 0xffff)
            {
                appearance = appearance_local.local_appearance;
            }
        }

        if (flash_load_local_name(&local_device_name) == 0)
        {
            if (local_device_name.local_name[0] != 0xff)
            {
                memcpy(device_name, local_device_name.local_name, GAP_DEVICE_NAME_LEN);
            }
        }
    }
 * \endcode
 */
uint32_t flash_load_local_appearance(T_LOCAL_APPEARANCE *p_data);

/**
 * @brief  Save local IRK to flash.
 * @param[in] p_data Pointer to local device name
 * @return Operation result
 * @retval 0 Operation success.
 * @retval Others Operation failure.
 *
 * <b>Example usage</b>
 * \code{.c}
   void test()
   {
       T_LOCAL_IRK irk = {0x01,0x02,0x03,};
       flash_save_local_irk(&irk);
   }
 * \endcode
 */
#if F_BT_LE_LOCAL_IRK_SETTING_SUPPORT
uint32_t flash_save_local_irk(T_LOCAL_IRK *p_data);
#endif

/**
 * @brief  Load local IRK from flash.
 * @param[in] p_data Pointer to local device name
 * @return Operation result
 * @retval 0 Operation success.
 * @retval Others Operation failure.
 *
 * <b>Example usage</b>
 * \code{.c}
    void test()
    {
        T_LOCAL_IRK irk;
        flash_load_local_irk(&irk);
    }
 * \endcode
 */
#if F_BT_LE_LOCAL_IRK_SETTING_SUPPORT
uint32_t flash_load_local_irk(T_LOCAL_IRK *p_data);
#endif

/**
 * @brief      Find key entry by bluetooth device address and address type.
 *
 * @param[in] bd_addr Remote bluetooth device address.
 * @param[in] bd_type Remote bluetooth device address type.
 * @return p_entry Pointer to the found key entry.
 * @retval null No entry found.
 * @retval others Pointer to the found key entry.
 */
T_LE_KEY_ENTRY *le_find_key_entry(uint8_t *bd_addr, T_GAP_REMOTE_ADDR_TYPE bd_type);

/**
 * @brief      Find key entry by index.
 *
 * @param[in] idx Key entry index.
 * @return p_entry Pointer to the found key entry.
 * @retval null No entry found.
 * @retval Others Pointer to the found key entry.
 */
T_LE_KEY_ENTRY *le_find_key_entry_by_idx(uint8_t idx);

/**
 * @brief      Get the boonded device count.
 *
 *             Note: You can call this function after @ref le_gap_init is invoked.
 *
 * @return num Bonded device count.
 */
uint8_t le_get_bond_dev_num(void);

/**
 * @brief      Get the low priority bond device key entry.
 *
 *             Note: You can call this function after @ref le_gap_init is invoked.
 *
 * @return p_entry Pointer to the found key entry.
 * @retval null No entry found.
 * @retval Others Pointer to the found key entry.
 */
T_LE_KEY_ENTRY *le_get_low_priority_bond(void);

/**
 * @brief      Get the high priority bond device key entry.
 *
 *             Note: You can call this function after @ref le_gap_init is invoked.
 *
 * @return p_entry Pointer to the found key entry.
 * @retval null No entry found.
 * @retval others Pointer to the found key entry.
 */
T_LE_KEY_ENTRY *le_get_high_priority_bond(void);

/**
 * @brief   Make the specified bonded device with the high priority.
 *
 * @param[in] bd_addr Remote bluetooth device address.
 * @param[in] bd_type Remote bluetooth device address type.
 * @return Operation result.
 * @retval true     Operation success.
 * @retval false    Operation failure.
 */
bool le_set_high_priority_bond(uint8_t *bd_addr, T_GAP_REMOTE_ADDR_TYPE bd_type);

/**
 * @brief   Resolve the specified bonded device.
 *
 * @param[in]       unresolved_addr       Unresolved remote bluetooth device address.
 * @param[in,out]   resolved_addr         Resolved remote bluetooth device address.
 * @param[in,out]   resolved_addr_type    Resolved remote bluetooth device address type.
 * @return Operation result.
 * @retval true     Operation success.
 * @retval false    Operation failure.
 *
 * <b>Example usage</b>
 * \code{.c}
    void app_handle_authen_state_evt(uint8_t conn_id, uint8_t new_state, uint16_t status)
    {
        APP_PRINT_INFO1("app_handle_authen_state_evt:conn_id %d", conn_id);
        switch (new_state)
        {
        ...
        case GAP_AUTHEN_STATE_COMPLETE:
           {
               APP_PRINT_INFO1("GAP_MSG_LE_AUTHEN_STATE_CHANGE:(GAP_AUTHEN_STATE_COMPLETE) status 0x%x",
                               status);
               if (status == 0)
               {
                   APP_PRINT_INFO0("GAP_MSG_LE_AUTHEN_STATE_CHANGE pair success");
                   {
                       uint8_t addr[6];
                       T_GAP_REMOTE_ADDR_TYPE bd_type;
                       uint8_t resolved_addr[6];
                       T_GAP_IDENT_ADDR_TYPE resolved_bd_type;
                       le_get_conn_addr(conn_id, addr, &bd_type);
                       if (bd_type == GAP_REMOTE_ADDR_LE_RANDOM)
                       {
                           if (le_resolve_random_address(addr, resolved_addr, &resolved_bd_type))
                           {
                               APP_PRINT_INFO2("GAP_AUTHEN_STATE_COMPLETE: resolved_addr %s, resolved_addr_type %d",
                                               TRACE_BDADDR(resolved_addr), resolved_bd_type);
                           }
                           else
                           {
                               APP_PRINT_INFO0("GAP_AUTHEN_STATE_COMPLETE: resolved addr failed");
                           }
                       }
                   }
               }
               else
               {
                   APP_PRINT_INFO0("GAP_MSG_LE_AUTHEN_STATE_CHANGE pair failed");
               }
           }
           break;
           ...
        }
    }
 * \endcode
 */
bool le_resolve_random_address(uint8_t *unresolved_addr, uint8_t *resolved_addr,
                               T_GAP_IDENT_ADDR_TYPE *resolved_addr_type);

/**
 * @brief   Get the cccd information of the bonded device.
 *
 * @param[in]       p_entry       Pointer the key entry of the bonded device.
 * @param[out]      p_data        Pointer to ccccd data to read.
 * @return Operation result.
 * @retval true     Operation success.
 * @retval false    Operation failure.
 *
 * <b>Example usage</b>
 * \code{.c}
    void test(void)
    {
        T_GAP_REMOTE_ADDR_TYPE remote_addr_type;
        uint8_t bd_addr[6];
        uint8_t conn_id = 0;
        uint8_t ccc_bits_count = 16;
        T_LE_CCCD *p_cccd_data = os_mem_alloc(RAM_TYPE_DATA_ON, 4 + ccc_bits_count * 4);
        T_LE_KEY_ENTRY *p_entry = NULL;
        if(le_get_conn_addr(conn_id, bd_addr, &remote_addr_type))
        {
            p_entry = le_find_key_entry(bd_addr, remote_addr_type);
            if (p_entry)
            {
                if (le_get_cccd_data(p_entry, p_cccd_data))
                {
                    APP_PRINT_INFO2("Get cccd data: len %d, data %s", p_cccd_data->data_length,
                                    TRACE_BINARY(p_cccd_data->data_length, p_cccd_data->data));
                }
            }
        }
        os_mem_free(p_cccd_data);
    }
 * \endcode
 */
bool le_get_cccd_data(T_LE_KEY_ENTRY *p_entry, T_LE_CCCD *p_data);

/**
 * @brief   Generate a new bonded device information by APP.
 *
 * @param[in] bd_addr        Remote bluetooth device address.
 * @param[in] bd_type        Remote bluetooth device address type.
 * @param[in] local_bd_type  Local bluetooth device address type.
 * @param[in] ltk_length     Local LTK length.
 * @param[in] local_ltk      Pointer to local LTK to write.
 * @param[in] p_cccd         Pointer to cccd data to write.
 * @return Operation result.
 * @retval true     Operation success.
 * @retval false    Operation failure.
 *
 * <b>Example usage</b>
 * \code{.c}
    void test(void)
    {
        T_GAP_REMOTE_ADDR_TYPE remote_addr_type = GAP_REMOTE_ADDR_LE_PUBLIC;
        uint8_t bd_addr[6] = {0x11,0x22,0x33,0x44,0x55,0x66};
        uint8_t ltk_length = 16;
        uint8_t local_ltk[16] = {0x0, 0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f};
        T_LE_KEY_TYPE key_type = LE_KEY_UNAUTHEN;
        uint8_t ccc_bits_count = 16;
        T_LE_CCCD *p_cccd_data = os_mem_alloc(RAM_TYPE_DATA_ON, 4 + ccc_bits_count * 4);

        p_cccd_data->data_length = 4;
        p_cccd_data->data[0] = 0x04;
        p_cccd_data->data[1] = 0x00;
        p_cccd_data->data[2] = 0x02;
        p_cccd_data->data[3] = 0x00;

        if(le_gen_bond_dev(bd_addr, remote_addr_type, GAP_LOCAL_ADDR_LE_PUBLIC,
                     ltk_length, local_ltk, key_type, p_cccd_data))
        {
            APP_PRINT_INFO0("Generate bond device success");
        }

        os_mem_free(p_cccd_data);
    }
 * \endcode
 */
bool le_gen_bond_dev(uint8_t *bd_addr, T_GAP_REMOTE_ADDR_TYPE bd_type,
                     T_GAP_LOCAL_ADDR_TYPE local_bd_type,
                     uint8_t ltk_length, uint8_t *local_ltk, T_LE_KEY_TYPE key_type, T_LE_CCCD *p_cccd);

/**
 * @brief   Save privacy information.
 *
 * @param[in] p_entry        Pointer to the key entry of bonded device.
 * @param[in] p_privacy_info Pointer to privacy information to write.
 * @return Operation result.
 * @retval true     Operation success.
 * @retval false    Operation failure.
 */
#if F_BT_LE_PRIVACY_SUPPORT
bool le_set_privacy_info(T_LE_KEY_ENTRY *p_entry, T_LE_PRIVACY_INFO *p_privacy_info);
#endif
/**
 * @brief   Get privacy information.
 *
 * @param[in]  p_entry        Pointer to the key entry of bonded device.
 * @param[out] p_privacy_info Pointer to privacy information to read.
 * @return Operation result.
 * @retval true     Operation success.
 * @retval false    Operation failure.
 */
#if F_BT_LE_PRIVACY_SUPPORT
bool le_get_privacy_info(T_LE_KEY_ENTRY *p_entry, T_LE_PRIVACY_INFO *p_privacy_info);
#endif
/**
 * @brief   Check the bonded device whether it is a privacy device.
 *
 * @param[in] p_entry         Pointer to the key entry of bonded device.
 * @return Operation result.
 * @retval true     This bonded device is a privacy device.
 * @retval false    This bonded device is not a privacy device.
 */
#if F_BT_LE_PRIVACY_SUPPORT
bool le_check_privacy_bond(T_LE_KEY_ENTRY *p_entry);
#endif

/**
 * @brief   Get length of device bond information.
 *
 * @return Length of device bond information.
 */
uint16_t le_get_dev_bond_info_len(void);

/**
 * @brief   Get device bond information.
 *
 * @param[in] p_entry        Pointer to the key entry of bonded device.
 * @param[in] p_data        Pointer to bond information to read.
 * @return Operation result.
 * @retval true     Operation success.
 * @retval false    Operation failure.
 */
bool le_get_dev_bond_info(T_LE_KEY_ENTRY *p_entry, uint8_t *p_data);

/**
 * @brief   Set device bond information.
 *
 * @param[in] length         Length of device bond information.
 * @param[in] p_data        Pointer to bond information to write.
 * @param[in,out] exist       Indicate whether bond information of the device is existed.
 * @return p_entry Pointer to the key entry.
 * @retval null No entry, operation failure.
 * @retval others Pointer to the key entry. If parameter exist is false, operation success. If parameter exist is true, bond information of the device is existed.
 */
T_LE_KEY_ENTRY *le_set_dev_bond_info(uint16_t length, uint8_t *p_data, bool *exist);

/** @} */ /* End of group GAP_LE_STORAGE_EXPORT_Functions */
/** @} */ /* End of group GAP_LE_STORAGE */


#ifdef __cplusplus
}
#endif

#endif /* FLASH_KEY_MGR_LE_H */
