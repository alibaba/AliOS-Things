/**
*********************************************************************************************************
*               Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     simple_ble_service.c
* @brief    simple BLE profile source file.
* @details  Demonstration of how to implement a self-definition profile.
* @author
* @date
* @version
*********************************************************************************************************
*/

#include <string.h>
#include <trace_app.h>
#include "bt_config_service.h"
#include <gap.h>
#include <diag.h>


#define BT_CONFIG_SERVICE_CHAR_V1_READ_WRITE_INDEX           0x02

T_SERVER_ID bt_config_service_id;
/**<  Value of bt config characteristic. */
static uint8_t bt_config_char_read_value[BT_CONFIG_READ_V1_MAX_LEN];
static uint16_t bt_config_char_read_len = 1;

/**<  Function pointer used to send event to application from ble config wifi profile. Initiated in bt_config_service_add_service. */
static P_FUN_SERVER_GENERAL_CB pfn_bt_config_service_cb = NULL;

/**< @brief  profile/service definition.  */
/**should changed according BT team **/
T_ATTRIB_APPL bt_config_service_tbl[] =
{
    /*----------------- uart data Service -------------------*/
    /* Attribute index 0 */
    /* <<Primary Service>>, .. */
    {
        (ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_LE),   /* wFlags     */
        {   /* bTypeValue */
            LO_WORD(GATT_UUID_PRIMARY_SERVICE),
            HI_WORD(GATT_UUID_PRIMARY_SERVICE),
            LO_WORD(0xFF01),              /* service UUID */
            HI_WORD(0xFF01)
        },
        UUID_16BIT_SIZE,                            /* bValueLen     */
        NULL,                                       /* pValueContext */
        GATT_PERM_READ                              /* wPermissions  */
    },

    /* Attribute index 1 */
    /* Data TX Characteristic */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {   /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_READ | GATT_CHAR_PROP_WRITE, /* characteristic properties */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },

    /* Attribute index 2 */
    /* Data TX Characteristic value  */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {   /* bTypeValue */
            LO_WORD(0x2A0D),
            HI_WORD(0X2A0D)
        },
        0,                                          /* variable size */
        NULL,
        GATT_PERM_READ | GATT_PERM_WRITE 			/* wPermissions    */
    },
};


/**
  * @brief  Set service related data from application.
  *
  * @param[in] param_type            parameter type to set.
  * @param[in] len                   value length to be set.
  * @param[in] p_value             value to set.
  * @return parameter set result.
  * @retval 0 false
  * @retval 1 true
  */
bool bt_config_service_set_parameter(T_BTCONFIG_PARAM_TYPE param_type, uint16_t len, void *p_value)
{
	bool ret = true;

	switch (param_type) {
		case BTCONFIG_SERVICE_PARAM_V1_READ_CHAR_VAL:
			if  (len <= BT_CONFIG_READ_V1_MAX_LEN) {
				memcpy(bt_config_char_read_value, p_value, len);
				bt_config_char_read_len = len;
			} else {
				ret = false;
			}
		break;
		
		default:
			ret = false;
		break;
	}

	if (!ret) {
		APP_PRINT_ERROR0("bt_config_service_set_parameter failed");
	}

	return ret;
}

/**
 * @brief read characteristic data from service.
 *
 * @param service_id          ServiceID of characteristic data.
 * @param attrib_index        Attribute index of getting characteristic data.
 * @param offset              Used for Blob Read.
 * @param p_length            length of getting characteristic data.
 * @param pp_value            data got from service.
 * @return Profile procedure result
*/
T_APP_RESULT  bt_config_service_attr_read_cb(uint8_t conn_id, T_SERVER_ID service_id,
                                            uint16_t attrib_index, uint16_t offset, uint16_t *p_length, uint8_t **pp_value)
{
    T_APP_RESULT  cause  = APP_RESULT_SUCCESS;

    switch (attrib_index)
    {
    default:
        APP_PRINT_ERROR1("bt_config_service_attr_read_cb, Attr not found, index %d", attrib_index);
        cause = APP_RESULT_ATTR_NOT_FOUND;
        break;
    case BT_CONFIG_SERVICE_CHAR_V1_READ_WRITE_INDEX:
        {
            TBTCONFIG_CALLBACK_DATA callback_data;
            callback_data.msg_type = SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE;
			callback_data.msg_data.read_value_index = BTCONFIG_READ_V1;
            callback_data.conn_id = conn_id;
            if (pfn_bt_config_service_cb)
            {
                pfn_bt_config_service_cb(service_id, (void *)&callback_data);
            }
            *pp_value = bt_config_char_read_value;
            *p_length = bt_config_char_read_len;
			//printf("[BT_CONFIG] Read %d\n\r", bt_config_char_read_len);
        }
        break;

    }

    return (cause);
}

/**
 * @brief write characteristic data from service.
 *
 * @param conn_id
 * @param service_id        ServiceID to be written.
 * @param attrib_index      Attribute index of characteristic.
 * @param length            length of value to be written.
 * @param p_value           value to be written.
 * @return Profile procedure result
*/
T_APP_RESULT bt_config_service_attr_write_cb(uint8_t conn_id, T_SERVER_ID service_id,
                                            uint16_t attrib_index, T_WRITE_TYPE write_type, uint16_t length, uint8_t *p_value,
                                            P_FUN_WRITE_IND_POST_PROC *p_write_ind_post_proc)
{
	//printf("[BT_CONFIG] Write %d\n\r", length);
	TBTCONFIG_CALLBACK_DATA callback_data;
	T_APP_RESULT  cause = APP_RESULT_SUCCESS;
	APP_PRINT_INFO1("bt_config_service_attr_write_cb write_type = 0x%x", write_type);
	*p_write_ind_post_proc = NULL;

	if (BT_CONFIG_SERVICE_CHAR_V1_READ_WRITE_INDEX == attrib_index) {
		/* Make sure written value size is valid. */
		if (p_value == NULL) {
			cause  = APP_RESULT_INVALID_VALUE_SIZE;
		} else {
			/* Notify Application. */
			callback_data.msg_type = SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE;
			callback_data.conn_id  = conn_id;
			callback_data.msg_data.write.opcode = BTCONFIG_WRITE_V1;
			callback_data.msg_data.write.write_type = write_type;
			callback_data.msg_data.write.len = length;
			callback_data.msg_data.write.p_value = p_value;

			//handle_bt_config_app_data(p_value, length);
			if (pfn_bt_config_service_cb) {
				pfn_bt_config_service_cb(service_id, (void *)&callback_data);
			}
		}
	} else {
			APP_PRINT_ERROR2("bt_config_service_attr_write_cb Error: attrib_index 0x%x, length %d",
			 attrib_index, length);
			cause = APP_RESULT_ATTR_NOT_FOUND;
	}

	return cause;
}

/**
 * @brief Simple ble Service Callbacks.
*/
const T_FUN_GATT_SERVICE_CBS bt_config_service_cbs =
{
    bt_config_service_attr_read_cb,  // Read callback function pointer
    bt_config_service_attr_write_cb, // Write callback function pointer
    NULL // CCCD update callback function pointer
};

/**
  * @brief Add simple BLE service to the BLE stack database.
  *
  * @param[in]   p_func  Callback when service attribute was read, write or cccd update.
  * @return Service id generated by the BLE stack: @ref T_SERVER_ID.
  * @retval 0xFF Operation failure.
  * @retval others Service id assigned by stack.
  *
  */
T_SERVER_ID bt_config_service_add_service(void *p_func)
{
    if (false == server_add_service(&bt_config_service_id,
                                    (uint8_t *)bt_config_service_tbl,
                                    sizeof(bt_config_service_tbl),
                                    bt_config_service_cbs))
    {
   // 	 DBG_8195A("bt_config_service_add_service: fail\n\r");
        APP_PRINT_ERROR0("bt_config_service_add_service: fail");
        bt_config_service_id = 0xff;
        return bt_config_service_id;
    }

    pfn_bt_config_service_cb = (P_FUN_SERVER_GENERAL_CB)p_func;
    return bt_config_service_id;
}
