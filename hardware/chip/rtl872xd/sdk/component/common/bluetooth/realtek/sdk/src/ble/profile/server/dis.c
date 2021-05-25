/**
*********************************************************************************************************
*               Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     dis.c
* @brief    Device Information service source file.
* @details  Interfaces to access Device Information service.
* @author
* @date
* @version  v1.0
*********************************************************************************************************
*/
#include "stdint.h"
#include "stddef.h"
#include "string.h"
#include "trace_app.h"
#include "profile_server.h"
#include "dis.h"
#include "dis_config.h"
#include "trace_app.h"

/********************************************************************************************************
* local static variables defined here, only used in this source file.
********************************************************************************************************/


///@cond
/** @brief  IEEE 11073 authoritative body values. */
#define DIS_IEEE_11073_BODY_EMPTY           0
#define DIS_IEEE_11073_BODY_IEEE            1
#define DIS_IEEE_11073_BODY_CONTINUA        2
#define DIS_IEEE_11073_BODY_EXP             254

/** @brief DIS related UUIDs */
#define GATT_UUID_DEVICE_INFORMATION_SERVICE   0x180A

#define GATT_UUID_CHAR_SYSTEM_ID               0x2A23
#define GATT_UUID_CHAR_MODEL_NUMBER            0x2A24
#define GATT_UUID_CHAR_SERIAL_NUMBER           0x2A25
#define GATT_UUID_CHAR_FIRMWARE_REVISION       0x2A26
#define GATT_UUID_CHAR_HARDWARE_REVISION       0x2A27
#define GATT_UUID_CHAR_SOFTWARE_REVISION       0x2A28
#define GATT_UUID_CHAR_MANUFACTURER_NAME       0x2A29
#define GATT_UUID_CHAR_IEEE_CERTIF_DATA_LIST   0x2A2A
#define GATT_UUID_CHAR_PNP_ID                  0x2A50
///@endcond



static P_FUN_SERVER_GENERAL_CB pfn_dis_cb = NULL;


#if DIS_CHAR_SYSTEM_ID_SUPPORT
/**<  DIS service System ID. */
static uint8_t dis_system_id[DIS_SYSTEM_ID_LENGTH] = {0, 1, 2, 0, 0, 3, 4, 5};
#endif


#if DIS_CHAR_PNP_ID_SUPPORT
/**<  DIS service PnP ID. */
static uint8_t dis_pnp_id[DIS_PNP_ID_LENGTH] =
{
    1,                                  // Vendor ID source (1=Bluetooth SIG)
    LO_WORD(0x005D), HI_WORD(0x005D),   // Vendor ID (Realtek Semiconductor Corporation)
    LO_WORD(0x0000), HI_WORD(0x0000),   // Product ID (vendor-specific)
    LO_WORD(0x0100), HI_WORD(0x0100)    // Product version (vendor-assigned-JJ.M.N)
};
#endif

#if DIS_CHAR_MANUFACTURER_NAME_SUPPORT
/**<  DIS service Manufacturer Name. A null-terminated byte is reserved*/
static uint8_t dis_manufacturer_name[DIS_CHAR_MANUFACTURER_NAME_STR_MAX_LENGTH] = "";
uint8_t dis_manufacturer_name_len = 0;
#endif

#if DIS_CHAR_MODEL_NUMBER_SUPPORT
/**<  DIS service Model Number. A null-terminated byte is reserved*/
static uint8_t dis_model_number[DIS_CHAR_MODEL_NUMBER_STR_MAX_LENGTH] = "";
uint8_t dis_model_number_len = 0;
#endif

#if DIS_CHAR_SERIAL_NUMBER_SUPPORT
/**<  DIS service Serial Number. A null-terminated byte is reserved*/
static uint8_t dis_serial_number[DIS_CHAR_SERIAL_NUMBER_STR_MAX_LENGTH] = "";
uint8_t dis_serial_number_len = 0;
#endif

#if DIS_CHAR_HARDWARE_REVISION_SUPPORT
/**<  DIS service Hardware Revision. A null-terminated byte is reserved*/
static uint8_t dis_hardware_rev[DIS_CHAR_HARDWARE_REVISION_STR_MAX_LENGTH] = "";
uint8_t dis_hardware_rev_len = 0;
#endif

#if DIS_CHAR_FIRMWARE_REVISION_SUPPORT
/**<  DIS service Firmware Revision. A null-terminated byte is reserved*/
static uint8_t dis_firmware_rev[DIS_CHAR_FIRMWARE_REVISION_STR_MAX_LENGTH] = "";
uint8_t dis_firmware_rev_len = 0;
#endif

#if DIS_CHAR_SOFTWARE_REVISION_SUPPORT
/**<  DIS service Software Revision. A null-terminated byte is reserved*/
static uint8_t dis_software_rev[DIS_CHAR_SOFTWARE_REVISION_STR_MAX_LENGTH] = "";
uint8_t dis_software_rev_len = 0;
#endif

#if DIS_CHAR_IEEE_CERTIF_DATA_LIST_SUPPORT
/**<  DIS service IEEE 11073-20601 Regulatory Certification Data List. */
static uint8_t dis_ieee_data_list[DIS_CHAR_IEEE_CERTIF_DATA_LIST_MAX_LENGTH] =
{
    DIS_IEEE_11073_BODY_EXP,      // Authoritative body type
    0x00,                         // Authoritative body structure type
    // Authoritative body data follows below:
    'e', 'x', 'p', 'e', 'r', 'i', 'm', 'e', 'n', 't', 'a', 'l'
};
uint8_t dis_ieee_data_list_len = 14;
#endif

/**< @brief  profile/service definition.  */
static const T_ATTRIB_APPL dis_attr_tbl[] =
{
    /*----------------- Device Information Service -------------------*/
    /* <<Primary Service>> */
    {
        (ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_LE), /* flags     */
        {                                         /* type_value */
            LO_WORD(GATT_UUID_PRIMARY_SERVICE),
            HI_WORD(GATT_UUID_PRIMARY_SERVICE),
            LO_WORD(GATT_UUID_DEVICE_INFORMATION_SERVICE),  /* service UUID */
            HI_WORD(GATT_UUID_DEVICE_INFORMATION_SERVICE)
        },
        UUID_16BIT_SIZE,                              /* bValueLen     */
        NULL,                                     /* p_value_context */
        GATT_PERM_READ                            /* permissions  */
    }

#if DIS_CHAR_MANUFACTURER_NAME_SUPPORT
    ,
    /* <<Characteristic>> */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_READ                       /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    /* Manufacturer Name String characteristic value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHAR_MANUFACTURER_NAME),
            HI_WORD(GATT_UUID_CHAR_MANUFACTURER_NAME)
        },
        0,                                          /* variable size */
        (void *)NULL,
        GATT_PERM_READ                              /* permissions */
    }
#endif

#if DIS_CHAR_MODEL_NUMBER_SUPPORT
    ,
    /* <<Characteristic>> */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_READ                       /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    /* Model Number characteristic value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHAR_MODEL_NUMBER),
            HI_WORD(GATT_UUID_CHAR_MODEL_NUMBER)
        },
        0,                                          /* variable size */
        (void *)NULL,
        GATT_PERM_READ                              /* permissions */
    }
#endif

#if DIS_CHAR_SERIAL_NUMBER_SUPPORT
    ,
    /* <<Characteristic>> */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_READ                     /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    /* Serial Number String String characteristic value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHAR_SERIAL_NUMBER),
            HI_WORD(GATT_UUID_CHAR_SERIAL_NUMBER)
        },
        0,                                          /* variable size */
        (void *)NULL,
        GATT_PERM_READ                              /* permissions */
    }
#endif

#if DIS_CHAR_HARDWARE_REVISION_SUPPORT
    ,
    /* <<Characteristic>> */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_READ                     /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    /* Manufacturer Name String characteristic value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHAR_HARDWARE_REVISION),
            HI_WORD(GATT_UUID_CHAR_HARDWARE_REVISION)
        },
        0,                                          /* variable size */
        (void *)NULL,
        GATT_PERM_READ                              /* permissions */
    }
#endif

#if DIS_CHAR_FIRMWARE_REVISION_SUPPORT
    ,
    /* <<Characteristic>> */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_READ                     /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    /* Firmware revision String characteristic value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHAR_FIRMWARE_REVISION),
            HI_WORD(GATT_UUID_CHAR_FIRMWARE_REVISION)
        },
        0,                                          /* variable size */
        (void *)NULL,
        GATT_PERM_READ                              /* permissions */
    }
#endif

#if DIS_CHAR_SOFTWARE_REVISION_SUPPORT
    ,
    /* <<Characteristic>> */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_READ                     /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    /* Manufacturer Name String characteristic value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHAR_SOFTWARE_REVISION),
            HI_WORD(GATT_UUID_CHAR_SOFTWARE_REVISION)
        },
        0,                                          /* variable size */
        (void *)NULL,
        GATT_PERM_READ                              /* permissions */
    }
#endif

#if DIS_CHAR_SYSTEM_ID_SUPPORT
    ,
    /* <<Characteristic>> */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_READ                       /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    /* System ID String characteristic value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHAR_SYSTEM_ID),
            HI_WORD(GATT_UUID_CHAR_SYSTEM_ID)
        },
        0,                                          /* variable size */
        (void *)NULL,
        GATT_PERM_READ                              /* permissions */
    }
#endif

#if DIS_CHAR_IEEE_CERTIF_DATA_LIST_SUPPORT
    ,

    /* <<Characteristic>> */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_READ                     /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    /* Manufacturer Name String characteristic value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHAR_IEEE_CERTIF_DATA_LIST),
            HI_WORD(GATT_UUID_CHAR_IEEE_CERTIF_DATA_LIST)
        },
        0,                                          /* variable size */
        (void *)NULL,
        GATT_PERM_READ                              /* permissions */
    }
#endif

#if DIS_CHAR_PNP_ID_SUPPORT
    ,
    /* <<Characteristic>> */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_READ                     /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    /* Manufacturer Name String characteristic value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHAR_PNP_ID),
            HI_WORD(GATT_UUID_CHAR_PNP_ID)
        },
        0,                                          /* variable size */
        (void *)NULL,
        GATT_PERM_READ                              /* permissions */
    }
#endif
};
/**< @brief  DIS service size definition.  */
const uint16_t dis_attr_tbl_size = sizeof(dis_attr_tbl);

/**
 * @brief       Set a device information service parameter.
 *
 *              NOTE: You can call this function with a device information service parameter type and it will set the
 *                      device information service parameter.  Device information service parameters are defined in @ref T_DIS_PARAM_TYPE.
 *                      If the "len" field sets to the size of a "uint16_t" ,the
 *                      "p_value" field must point to a data with type of "uint16_t".
 *
 * @param[in]   param_type   Device information service parameter type: @ref T_DIS_PARAM_TYPE
 * @param[in]   length       Length of data to write
 * @param[in]   p_value Pointer to data to write.  This is dependent on
 *                      the parameter type and WILL be cast to the appropriate
 *                      data type (For example: if data type of param is uint16_t, p_value will be cast to
 *                      pointer of uint16_t).
 *
 * @return Operation result.
 * @retval true Operation success.
 * @retval false Operation failure.
 *
 * <b>Example usage</b>
 * \code{.c}
    void test(void)
    {
        const uint8_t dis_manufacture_name[] = "Realtek BT";
        dis_set_parameter(DIS_PARAM_MANUFACTURER_NAME,
                      sizeof(dis_manufacture_name),
                      (void *)dis_manufacture_name);
    }
 * \endcode
 */
bool dis_set_parameter(T_DIS_PARAM_TYPE param_type, uint8_t length, void *p_value)
{
    bool ret = true;

    switch (param_type)
    {
    default:
        ret = false;
        break;

#if DIS_CHAR_MANUFACTURER_NAME_SUPPORT
    case DIS_PARAM_MANUFACTURER_NAME:
        if (length > DIS_CHAR_MANUFACTURER_NAME_STR_MAX_LENGTH)
        {
            length = DIS_CHAR_MANUFACTURER_NAME_STR_MAX_LENGTH;
        }
        dis_manufacturer_name_len = length;
        memcpy(dis_manufacturer_name, p_value, length);
        break;
#endif

#if DIS_CHAR_MODEL_NUMBER_SUPPORT
    case DIS_PARAM_MODEL_NUMBER:
        if (length > DIS_CHAR_MODEL_NUMBER_STR_MAX_LENGTH)
        {
            length = DIS_CHAR_MODEL_NUMBER_STR_MAX_LENGTH;
        }
        dis_model_number_len = length;
        memcpy(dis_model_number, p_value, length);
        break;
#endif

#if DIS_CHAR_SERIAL_NUMBER_SUPPORT
    case DIS_PARAM_SERIAL_NUMBER:
        if (length > DIS_CHAR_SERIAL_NUMBER_STR_MAX_LENGTH)
        {
            length = DIS_CHAR_SERIAL_NUMBER_STR_MAX_LENGTH;
        }
        dis_serial_number_len = length;
        memcpy(dis_serial_number, p_value, length);

        break;
#endif

#if DIS_CHAR_HARDWARE_REVISION_SUPPORT
    case DIS_PARAM_HARDWARE_REVISION:
        if (length > DIS_CHAR_HARDWARE_REVISION_STR_MAX_LENGTH)
        {
            length = DIS_CHAR_HARDWARE_REVISION_STR_MAX_LENGTH;
        }
        dis_hardware_rev_len = length;
        memcpy(dis_hardware_rev, p_value, length);
        break;
#endif

#if DIS_CHAR_FIRMWARE_REVISION_SUPPORT
    case DIS_PARAM_FIRMWARE_REVISION:
        if (length > DIS_CHAR_FIRMWARE_REVISION_STR_MAX_LENGTH)
        {
            length = DIS_CHAR_FIRMWARE_REVISION_STR_MAX_LENGTH;
        }
        dis_firmware_rev_len = length;
        memcpy(dis_firmware_rev, p_value, length);
        break;
#endif

#if DIS_CHAR_SOFTWARE_REVISION_SUPPORT
    case DIS_PARAM_SOFTWARE_REVISION:
        if (length > DIS_CHAR_SOFTWARE_REVISION_STR_MAX_LENGTH)
        {
            length = DIS_CHAR_SOFTWARE_REVISION_STR_MAX_LENGTH;
        }
        dis_software_rev_len = length;
        memcpy(dis_software_rev, p_value, length);
        break;
#endif

#if DIS_CHAR_SYSTEM_ID_SUPPORT
    case DIS_PARAM_SYSTEM_ID:
        if (length > DIS_SYSTEM_ID_LENGTH)
        {
            length = DIS_SYSTEM_ID_LENGTH;
        }
        memcpy(dis_system_id, p_value, length);
        break;
#endif

#if DIS_CHAR_IEEE_CERTIF_DATA_LIST_SUPPORT
    case DIS_PARAM_IEEE_DATA_LIST:
        if (length > DIS_CHAR_IEEE_CERTIF_DATA_LIST_MAX_LENGTH)
        {
            length = DIS_CHAR_IEEE_CERTIF_DATA_LIST_MAX_LENGTH;
        }
        dis_ieee_data_list_len = length;
        memcpy(dis_ieee_data_list, p_value, length);
        break;
#endif

#if DIS_CHAR_PNP_ID_SUPPORT
    case DIS_PARAM_PNP_ID:
        if (length > DIS_PNP_ID_LENGTH)
        {
            length = DIS_PNP_ID_LENGTH;
        }
        memcpy(dis_pnp_id, p_value, length);
        break;
#endif
    }

    if (!ret)
    {
        APP_PRINT_ERROR0("DIS parameter set failed");
    }

    return ret;
}

/**
 * @brief Read characteristic data from service.
 *
 * @param[in]   conn_id  Connection id.
 * @param[in] service_id            ServiceID to be read.
 * @param[in] attrib_index          Attribute index of getting characteristic data.
 * @param[in] offset                offset of characteritic to be read.
 * @param[in,out] length_ptr            length of getting characteristic data.
 * @param[in,out] pp_value              pointer to pointer of characteristic value to be read.
 * @return T_APP_RESULT
*/
T_APP_RESULT dis_attr_read_cb(uint8_t conn_id, T_SERVER_ID service_id, uint16_t attrib_index,
                              uint16_t offset, uint16_t *length_ptr, uint8_t **pp_value)
{
    T_DIS_CALLBACK_DATA callback_data;
    T_APP_RESULT  cause  = APP_RESULT_SUCCESS;
    *length_ptr = 0;
    callback_data.msg_type = SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE;
    callback_data.conn_id = conn_id;

    switch (attrib_index)
    {
    default:
        APP_PRINT_ERROR1("dis_attr_read_cb: attrib_index %d", attrib_index);
        cause = APP_RESULT_ATTR_NOT_FOUND;
        break;

#if DIS_CHAR_SYSTEM_ID_SUPPORT
    case GATT_SVC_DIS_SYSTEM_ID_INDEX:
        callback_data.msg_data.read_value_index = DIS_READ_SYSTEM_ID_INDEX;
        pfn_dis_cb(service_id, (void *)&callback_data);
        *pp_value    = (uint8_t *)&dis_system_id;
        *length_ptr = sizeof(dis_system_id);
        break;
#endif

#if DIS_CHAR_MANUFACTURER_NAME_SUPPORT
    case GATT_SVC_DIS_MANU_NAME_INDEX:
        callback_data.msg_data.read_value_index = DIS_READ_MANU_NAME_INDEX;
        pfn_dis_cb(service_id, (void *)&callback_data);
        *pp_value    = (uint8_t *)&dis_manufacturer_name;
        *length_ptr = dis_manufacturer_name_len;
        break;
#endif

#if DIS_CHAR_SERIAL_NUMBER_SUPPORT
    case GATT_SVC_DIS_SERIAL_NUM_INDEX:
        callback_data.msg_data.read_value_index = DIS_READ_SERIAL_NUM_INDEX;
        pfn_dis_cb(service_id, (void *)&callback_data);
        *pp_value    = (uint8_t *)&dis_serial_number;
        *length_ptr = dis_serial_number_len;
        break;
#endif

#if DIS_CHAR_FIRMWARE_REVISION_SUPPORT
    case GATT_SVC_DIS_FIRMWARE_REV_INDEX:
        callback_data.msg_data.read_value_index = DIS_READ_FIRMWARE_REV_INDEX;
        pfn_dis_cb(service_id, (void *)&callback_data);
        *pp_value    = (uint8_t *)&dis_firmware_rev;
        *length_ptr = dis_firmware_rev_len;
        break;
#endif

#if DIS_CHAR_HARDWARE_REVISION_SUPPORT
    case GATT_SVC_DIS_HARDWARE_REV_INDEX:
        callback_data.msg_data.read_value_index = DIS_READ_HARDWARE_REV_INDEX;
        pfn_dis_cb(service_id, (void *)&callback_data);
        *pp_value    = (uint8_t *)&dis_hardware_rev;
        *length_ptr = dis_hardware_rev_len;
        break;
#endif

#if DIS_CHAR_SOFTWARE_REVISION_SUPPORT
    case GATT_SVC_DIS_SOFTWARE_REV_INDEX:
        callback_data.msg_data.read_value_index = DIS_READ_SOFTWARE_REV_INDEX;
        pfn_dis_cb(service_id, (void *)&callback_data);
        *pp_value    = (uint8_t *)&dis_software_rev;
        *length_ptr = dis_software_rev_len;
        break;
#endif

#if DIS_CHAR_IEEE_CERTIF_DATA_LIST_SUPPORT
    case GATT_SVC_DIS_IEEE_CERT_STR_INDEX:
        callback_data.msg_data.read_value_index = DIS_READ_IEEE_CERT_STR_INDEX;
        pfn_dis_cb(service_id, (void *)&callback_data);
        *pp_value    = (uint8_t *)&dis_ieee_data_list;
        *length_ptr = dis_ieee_data_list_len;
        break;
#endif

#if DIS_CHAR_PNP_ID_SUPPORT
    case GATT_SVC_DIS_PNP_ID_INDEX:
        callback_data.msg_data.read_value_index = DIS_READ_PNP_ID_INDEX;
        pfn_dis_cb(service_id, (void *)&callback_data);
        *pp_value    = (uint8_t *)&dis_pnp_id;
        *length_ptr = sizeof(dis_pnp_id);
        break;
#endif

#if DIS_CHAR_MODEL_NUMBER_SUPPORT
    case GATT_SVC_DIS_MODEL_NUM_INDEX:
        callback_data.msg_data.read_value_index = DIS_READ_MODEL_NUM_INDEX;
        pfn_dis_cb(service_id, (void *)&callback_data);
        *pp_value    = (uint8_t *)&dis_model_number;
        *length_ptr = dis_model_number_len;
        break;
#endif

    }

    APP_PRINT_INFO2("dis_attr_read_cb: attrib_index %d, *length_ptr %d",
                    attrib_index,
                    *length_ptr);
    return (cause);
}

const T_FUN_GATT_SERVICE_CBS dis_cbs =
{
    dis_attr_read_cb,  // Read callback function pointer
    NULL, // Write callback function pointer
    NULL  // Authorization callback function pointer
};

/**
  * @brief Add device information service to the BLE stack database.
  *
  * @param[in]   p_func  Callback when service attribute was read, write or cccd update.
  * @return Service id generated by the BLE stack: @ref T_SERVER_ID.
  * @retval 0xFF Operation failure.
  * @retval others Service id assigned by stack.
  *
  * <b>Example usage</b>
  * \code{.c}
    void profile_init()
    {
        server_init(1);
        dis_id = dis_add_service(app_handle_profile_message);
    }
  * \endcode
  */
T_SERVER_ID dis_add_service(void *p_func)
{
    T_SERVER_ID service_id;
    if (false == server_add_service(&service_id,
                                    (uint8_t *)dis_attr_tbl,
                                    dis_attr_tbl_size,
                                    dis_cbs))
    {
        APP_PRINT_ERROR1("dis_add_service: service_id %d", service_id);
        service_id = 0xff;
    }
    pfn_dis_cb = (P_FUN_SERVER_GENERAL_CB)p_func;
    return service_id;
}

