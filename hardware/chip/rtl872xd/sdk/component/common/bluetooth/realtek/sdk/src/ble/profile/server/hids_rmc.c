/**
*********************************************************************************************************
*               Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     hids_rmc.c
* @brief    Remote control HID service source file.
* @details  Interfaces to access HIDS service.
* @author
* @date
* @version  v1.0
*********************************************************************************************************
*/
#include "profile_server.h"
#include "hids_rmc.h"
#include <string.h>
#include <trace_app.h>

/********************************************************************************************************
* local static variables defined here, only used in this source file.
********************************************************************************************************/
static P_FUN_SERVER_GENERAL_CB pfn_hids_cb = NULL;

static const uint8_t hids_report_descriptor[] =
{
    0x05, 0x01,  /* Usage Page(Generic Desktop) */
    0x09, 0x06,  /* Usage (Keyboard) */
    0xa1, 0x01,  /* Collection Application */

    0x05, 0x07,  /* Usage Page (Keyboard) */
    0x09, 0x06,  /* Usage (Keyboard) */
    0xa1, 0x01,  /* Collection Application */
    0x85, HIDS_KB_REPORT_ID,  /* Report Id */
    0x95, 0x08,  /* Report Count */
    0x75, 0x08,  /* Report Size */
    0x15, 0x00,  /* Logical Minimum */
    0x25, 0xff,  /* Logical Maximum */
    0x19, 0x00,  /* Usage Minimum */
    0x29, 0xff,  /* Usage Maximun */
    0x81, 0x00,  /* Input */
    0xc0,  /* End Collection */

#if FEATURE_SUPPORT_MULTIMEDIA_KEYBOARD
    0x05, 0x0c,     /* USAGE_PAGE       (Consumer) */
    0x09, 0x01,     /* USAGE            (Consumer Control) */
    0xa1, 0x01,     /* COLLECTION       (Application) */
    0x85, HIDS_MM_KB_REPORT_ID,     /* REPORT_ID        (4) */
    0x15, 0x00,     /* LOGICAL_MINIMUM  (0) */
    0x25, 0x01,     /* LOGICAL_MAXIMUM  (1) */
    0x75, 0x01,     /* REPORT_SIZE      (1) */
    0x95, 0x18,     /* REPORT_COUNT     (24) */
    0x09, 0xb5,     /* USAGE            (Scan Next Track) */
    0x09, 0xb6,     /* USAGE            (Scan Previous Track) */
    0x09, 0xb7,     /* USAGE            (Stop) */
    0x09, 0xcd,     /* USAGE            (Play/Pause) */
    0x09, 0xe2,     /* USAGE            (Mute) */
    0x09, 0xe5,     /* USAGE            (Bass Boost) */
    0x09, 0xe7,     /* USAGE            (Loudness) */
    0x09, 0xe9,     /* USAGE            (Volume Increment) */
    0x09, 0xea,     /* USAGE            (Volume Decrement) */
    0x0a, 0x52, 0x01,     /* USAGE      (Bass Increment) */
    0x0a, 0x53, 0x01,     /* USAGE      (Bass Decrement) */
    0x0a, 0x54, 0x01,     /* USAGE      (Treble Increment) */
    0x0a, 0x55, 0x01,     /* USAGE      (Treble Decrement) */
    0x0a, 0x83, 0x01,     /* USAGE      (AL Consumer Control Configuration) */
    0x0a, 0x8a, 0x01,     /* USAGE      (AL Email Reader) */
    0x0a, 0x92, 0x01,     /* USAGE      (AL Calculator) */
    0x0a, 0x94, 0x01,     /* USAGE      (AL Local Machine Browser) */
    0x0a, 0x21, 0x02,     /* USAGE      (AC Search) */
    0x0a, 0x23, 0x02,     /* USAGE      (AC Home) */
    0x0a, 0x24, 0x02,     /* USAGE      (AC Back) */
    0x0a, 0x25, 0x02,     /* USAGE      (AC Forward) */
    0x0a, 0x26, 0x02,     /* USAGE      (AC Stop) */
    0x0a, 0x27, 0x02,     /* USAGE      (AC Refresh) */
    0x0a, 0x2a, 0x02,     /* USAGE      (AC Bookmarks) */
    0x81, 0x02,     /* INPUT            (Data,Var,Abs) */
    0xc0,            /* END_COLLECTION */
#endif

#if RCU_VOICE_EN
    0x06, 0x12, 0xff,  /* USAGE_PAGE (Undefine) */
    0x0a, 0x12, 0xff,  /* USAGE (Undefine) */
    0xa1, 0x01,  /* COLLECTION (Application) */
    0x85, HIDS_VOICE_REPORT_ID,  /* REPORT_ID (RMC_VENDOR_REPORT_ID_1) */
    0x95, 0xff,  /* Report Count */
    0x75, 0x08,  /* Report Size */
    0x15, 0x00,  /* Logical Minimum */
    0x25, 0xff,  /* Logical Maximum */
    0x19, 0x00,  /* Usage Minimum */
    0x29, 0xff,  /* Usage Maximun */
    0x81, 0x00,  /* Input */
    0xc0,  /* End Collection */
#endif

    0xc0  /* End Collection */
};

static uint8_t hids_protocol_mode = REPORT_PROCOCOL_MODE;
static uint8_t hids_suspand_mode = 0;
static HID_INFO_ATTRB hids_info = {0, 0, 0x0100};
static uint8_t *p_report_data = NULL;
static uint16_t RmcDataLen = 0;


const T_ATTRIB_APPL hids_attr_table[] =
{
    /*--------------------------HID Service ---------------------------*/
    /* <<Primary Service>> 0 */
    {
        (ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_LE),  /* flags     */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_PRIMARY_SERVICE),
            HI_WORD(GATT_UUID_PRIMARY_SERVICE),
            LO_WORD(GATT_UUID_HID),               /* service UUID */
            HI_WORD(GATT_UUID_HID)
        },
        UUID_16BIT_SIZE,                            /* bValueLen     */
        NULL,                                       /* p_value_context */
        GATT_PERM_READ                              /* permissions  */
    },

    /* <<characteristic>> 1 */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_READ,                    /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },

    /*  HID Information characteristic 2 */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHAR_HID_INFO),
            HI_WORD(GATT_UUID_CHAR_HID_INFO)
        },
        4,                                          /* variable size */
        (void *)NULL,
        GATT_PERM_READ_AUTHEN_REQ                              /* permissions */
    },

    /* <<Characteristic>> 3 */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_WRITE_NO_RSP,                      /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },

    /* HID controlPoint characteristic 4 */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHAR_HID_CONTROL_POINT),
            HI_WORD(GATT_UUID_CHAR_HID_CONTROL_POINT)
        },
        1,                                          /* variable size */
        (void *)NULL,
        GATT_PERM_READ_AUTHEN_REQ | GATT_PERM_WRITE_AUTHEN_REQ            /* permissions */
    },

    /* <<Characteristic>> 5 */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_READ | GATT_CHAR_PROP_WRITE_NO_RSP,                    /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },

    /* HID Protocol Mode characteristic 6 */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHAR_PROTOCOL_MODE),
            HI_WORD(GATT_UUID_CHAR_PROTOCOL_MODE)
        },
        1,                                          /* variable size */
        (void *)NULL,
        GATT_PERM_READ_AUTHEN_REQ | GATT_PERM_WRITE_AUTHEN_REQ                        /* permissions */
    },

    /* <<Characteristic>> 7 */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_READ,                  /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },

    /* HID Report Map characteristic 8 */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHAR_REPORT_MAP),
            HI_WORD(GATT_UUID_CHAR_REPORT_MAP)
        },
        0,                                          /* variable size */
        (void *)NULL,
        GATT_PERM_READ_AUTHEN_REQ                              /* permissions */
    },

    /* <<Characteristic>> 9 */
    {
        ATTRIB_FLAG_VALUE_INCL,                   /* flags */
        {                                         /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_NOTIFY | GATT_CHAR_PROP_READ | GATT_CHAR_PROP_WRITE,           /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                        /* bValueLen */
        NULL,
        GATT_PERM_READ                            /* permissions */
    },

    /* Keyboard Input Report characteristic 10 */
    {
        ATTRIB_FLAG_VALUE_APPL,                   /* flags */
        {                                         /* type_value */
            LO_WORD(GATT_UUID_CHAR_REPORT),
            HI_WORD(GATT_UUID_CHAR_REPORT)
        },
        8,                                        /* variable size */
        (void *)NULL,
        GATT_PERM_READ_AUTHEN_REQ | GATT_PERM_WRITE_AUTHEN_REQ                          /* permissions */
    },

    /* client characteristic configuration 11 */
    {
        (ATTRIB_FLAG_VALUE_INCL |                   /* flags */
         ATTRIB_FLAG_CCCD_APPL),
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            HI_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            /* NOTE: this value has an instantiation for each client, a write to */
            /* this attribute does not modify this default value:                */
            LO_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT), /* client char. config. bit field */
            HI_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT)
        },
        2,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ_AUTHEN_REQ | GATT_PERM_WRITE_AUTHEN_REQ)          /* permissions */
    },

    /* Keyboard Input Report reference descriptor 12 */
    {
        (ATTRIB_FLAG_VALUE_INCL |                   /* flags */
         ATTRIB_FLAG_CCCD_APPL),
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHAR_REPORT_REFERENCE),
            HI_WORD(GATT_UUID_CHAR_REPORT_REFERENCE),
            /* NOTE: this value has an instantiation for each client, a write to */
            /* this attribute does not modify this default value:                */
            HIDS_KB_REPORT_ID, /* client char. config. bit field */
            1
        },
        2,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ_AUTHEN_REQ)          /* permissions */
    },

#if RCU_VOICE_EN
    /* <<Characteristic>> 13 */
    {
        ATTRIB_FLAG_VALUE_INCL,                   /* flags */
        {                                        /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_NOTIFY | GATT_CHAR_PROP_READ | GATT_CHAR_PROP_WRITE,           /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                        /* bValueLen */
        NULL,
        GATT_PERM_READ                            /* permissions */
    },

    /*  HID Report characteristic value, Voice Input 14 */
    {
        ATTRIB_FLAG_VALUE_APPL,                   /* flags */
        {                                        /* type_value */
            LO_WORD(GATT_UUID_CHAR_REPORT),
            HI_WORD(GATT_UUID_CHAR_REPORT)
        },
        255,                                          /* variable size */
        (void *)NULL,
        GATT_PERM_READ_AUTHEN_REQ | GATT_PERM_WRITE_AUTHEN_REQ                          /* permissions */
    },

    /* client characteristic configuration 15 */
    {
        (ATTRIB_FLAG_VALUE_INCL |                   /* flags */
         ATTRIB_FLAG_CCCD_APPL),
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            HI_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            /* NOTE: this value has an instantiation for each client, a write to */
            /* this attribute does not modify this default value:                */
            LO_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT), /* client char. config. bit field */
            HI_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT)
        },
        2,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ_AUTHEN_REQ | GATT_PERM_WRITE_AUTHEN_REQ)          /* permissions */
    },

    /* Voice Input reference descriptor 16 */
    {
        (ATTRIB_FLAG_VALUE_INCL |                   /* flags */
         ATTRIB_FLAG_CCCD_APPL),
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHAR_REPORT_REFERENCE),
            HI_WORD(GATT_UUID_CHAR_REPORT_REFERENCE),
            /* NOTE: this value has an instantiation for each client, a write to */
            /* this attribute does not modify this default value:                */
            HIDS_VOICE_REPORT_ID, /* client char. config. bit field */
            HID_INPUT_TYPE
        },
        2,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ_AUTHEN_REQ)          /* permissions */
    },

    /* <<Characteristic>> 17 */
    {
        ATTRIB_FLAG_VALUE_INCL,                   /* flags */
        {                                         /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_READ | GATT_CHAR_PROP_WRITE | GATT_CHAR_PROP_WRITE_NO_RSP,             /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                        /* bValueLen */
        NULL,
        GATT_PERM_READ                            /* permissions */
    },

    /*  HID Report characteristic value, Voice Output 18 */
    {
        ATTRIB_FLAG_VALUE_APPL,                   /* flags */
        {                                         /* type_value */
            LO_WORD(GATT_UUID_CHAR_REPORT),
            HI_WORD(GATT_UUID_CHAR_REPORT)
        },
        0,                                        /* variable size */
        (void *)NULL,
        GATT_PERM_READ_AUTHEN_REQ | GATT_PERM_WRITE_AUTHEN_REQ                          /* permissions */
    },

    /* Voice Output reference descriptor 19 */
    {
        (ATTRIB_FLAG_VALUE_INCL |                   /* flags */
         ATTRIB_FLAG_CCCD_APPL),
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHAR_REPORT_REFERENCE),
            HI_WORD(GATT_UUID_CHAR_REPORT_REFERENCE),
            /* NOTE: this value has an instantiation for each client, a write to */
            /* this attribute does not modify this default value:                */
            HIDS_VOICE_REPORT_ID, /* client char. config. bit field */
            HID_OUTPUT_TYPE
        },
        2,                                          /* bValueLen */
        (void *)NULL,
        (GATT_PERM_READ_AUTHEN_REQ)          /* permissions */
    },
#endif

#if FEATURE_SUPPORT_MULTIMEDIA_KEYBOARD
    /* <<Characteristic>> 20 */
    {
        ATTRIB_FLAG_VALUE_INCL,                   /* flags */
        {                                         /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_NOTIFY | GATT_CHAR_PROP_READ | GATT_CHAR_PROP_WRITE,           /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                        /* bValueLen */
        NULL,
        GATT_PERM_READ                            /* permissions */
    },

    /*  HID Report characteristic value, multimedia keyboard Input 21 */
    {
        ATTRIB_FLAG_VALUE_APPL,                   /* flags */
        {                                         /* type_value */
            LO_WORD(GATT_UUID_CHAR_REPORT),
            HI_WORD(GATT_UUID_CHAR_REPORT)
        },
        3,                                        /* variable size */
        (void *)NULL,
        GATT_PERM_READ_AUTHEN_REQ | GATT_PERM_WRITE_AUTHEN_REQ                          /* permissions */
    },

    /* client characteristic configuration 22 */
    {
        (ATTRIB_FLAG_VALUE_INCL |                   /* flags */
         ATTRIB_FLAG_CCCD_APPL),
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            HI_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            /* NOTE: this value has an instantiation for each client, a write to */
            /* this attribute does not modify this default value:                */
            LO_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT), /* client char. config. bit field */
            HI_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT)
        },
        2,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ_AUTHEN_REQ | GATT_PERM_WRITE_AUTHEN_REQ)          /* permissions */
    },

    /* multimedia keyboard Input reference descriptor 23 */
    {
        (ATTRIB_FLAG_VALUE_INCL |                   /* flags */
         ATTRIB_FLAG_CCCD_APPL),
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHAR_REPORT_REFERENCE),
            HI_WORD(GATT_UUID_CHAR_REPORT_REFERENCE),
            /* NOTE: this value has an instantiation for each client, a write to */
            /* this attribute does not modify this default value:                */
            HIDS_MM_KB_REPORT_ID, /* client char. config. bit field */
            HID_INPUT_TYPE
        },
        2,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ_AUTHEN_REQ)          /* permissions */
    }
#endif
};

uint16_t hids_attr_tbl_size = sizeof(hids_attr_table);

/**
 * @brief       Set a HID service parameter.
 *
 *              NOTE: You can call this function with a HID service parameter type and it will set the
 *                      HID service parameter.  HID service parameters are defined in @ref T_HIDS_PARAM_TYPE.
 *
 * @param[in]   param_type  HID service parameter type: @ref T_HIDS_PARAM_TYPE
 * @param[in]   len         Length of data to write
 * @param[in]   p_value Pointer to data to write.  This is dependent on
 *                      the parameter type and WILL be cast to the appropriate
 *                      data type
 *
 * @return Operation result.
 * @retval true Operation success.
 * @retval false Operation failure.
 *
 * <b>Example usage</b>
 * \code{.c}
    void test(void)
    {
        uint8_t mode = 1;
        hids_set_parameter(HID_PROTOCOL_MODE, 1, &mode);
    }
 * \endcode
 */
bool hids_set_parameter(uint8_t param_type, uint8_t length, void *value_ptr)
{
    bool ret = true;

    switch (param_type)
    {
    case HIDS_PARAM_PROTOCOL_MODE:
        {
            hids_protocol_mode = *((uint8_t *)value_ptr);
        }
        break;
    case HIDS_PARAM_SUSPEND_MODE:
        {
            hids_suspand_mode = *((uint8_t *)value_ptr);
        }
        break;
    case HIDS_PARAM_HID_INFO:
        {
            memcpy((void *)&hids_info, value_ptr, length);
        }
        break;
    case HIDS_PARAM_REPORT:
        {
            RmcDataLen = length;
            p_report_data = value_ptr;
        }
        break;

    default:
        ret = false;
        break;

    }
    return ret;
}

/**
 * @brief read characteristic data from service.
 *
 * @param conn_id           Connection ID.
 * @param service_id        ServiceID to be read.
 * @param attrib_index      Attribute index of getting characteristic data.
 * @param offset            Offset of characteritic to be read.
 * @param p_length          Length of getting characteristic data.
 * @param pp_value          Pointer to pointer of characteristic value to be read.
 * @return T_APP_RESULT
*/
T_APP_RESULT hids_attr_read_cb(uint8_t conn_id, T_SERVER_ID service_id, uint16_t attrib_index,
                               uint16_t offset, uint16_t *p_length, uint8_t **pp_value)
{
    T_APP_RESULT  cause  = APP_RESULT_SUCCESS;
    T_HID_CALLBACK_DATA callback_data;
    *p_length = 0;

    APP_PRINT_INFO2("hids_attr_read_cb attrib_index = %d offset %x", attrib_index, offset);

    callback_data.msg_type = SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE;
    switch (attrib_index)
    {
    default:
        cause = APP_RESULT_ATTR_NOT_FOUND;
        break;

    case GATT_SRV_HID_KB_INPUT_INDEX:
#if RCU_VOICE_EN
    case GATT_SRV_HID_VOICE_INPUT_INDEX:
    case GATT_SRV_HID_VOICE_OUTPUT_INDEX:
#endif
#if FEATURE_SUPPORT_MULTIMEDIA_KEYBOARD
    case GATT_SRV_HID_MM_KB_INPUT_INDEX:
#endif
        {
            callback_data.msg_data.read_value_index = HIDS_READ_PARAM_REPORT;
            if (pfn_hids_cb)
            {
                pfn_hids_cb(service_id, (void *)&callback_data);
            }
            *pp_value = p_report_data;
            *p_length = RmcDataLen;
            if ((*p_length == 0) || (NULL == p_report_data))
            {
                cause = APP_RESULT_APP_ERR;
            }
        }
        break;

    case GATT_SVC_HID_INFO_INDEX:
        {
            callback_data.msg_data.read_value_index = HIDS_READ_PARAM_HID_INFO;
            if (pfn_hids_cb)
            {
                pfn_hids_cb(service_id, (void *)&callback_data);
            }
            *pp_value = (uint8_t *)&hids_info;
            *p_length = sizeof(hids_info);
        }
        break;
    case GATT_SVC_HID_CONTROL_POINT_INDEX:
        {
            callback_data.msg_data.read_value_index = HIDS_READ_PARAM_SUSPEND_MODE;
            if (pfn_hids_cb)
            {
                pfn_hids_cb(service_id, (void *)&callback_data);
            }
            *pp_value = &hids_suspand_mode;
            *p_length = 1;
        }
        break;
    case GATT_SVC_HID_PROTOCOL_MODE_INDEX:
        {
            callback_data.msg_data.read_value_index = HIDS_READ_PARAM_PROTOCOL_MODE;
            if (pfn_hids_cb)
            {
                pfn_hids_cb(service_id, (void *)&callback_data);
            }
            *pp_value = &hids_protocol_mode;
            *p_length = sizeof(hids_protocol_mode);
        }
        break;
    case GATT_SVC_HID_REPORT_MAP_INDEX:
        {
            *pp_value = (uint8_t *)hids_report_descriptor;
            *p_length = sizeof(hids_report_descriptor);
        }
        break;
    }

    return cause;
}

/**
 * @brief write characteristic data from service.
 *
 * @param[in] conn_id       Connection id.
 * @param[in] service_id          ServiceID to be written.
 * @param[in] attrib_index          Attribute index of characteristic.
 * @param[in] write_type          Write type.
 * @param[in] length            Length of writing characteristic data.
 * @param[in] p_value            Pointer to characteristic data.
 * @param[in] p_write_ind_post_proc      Function pointer after ias_attr_write_cb.
 * @return TProfileResult
*/
T_APP_RESULT hids_attr_write_cb(uint8_t conn_id, uint8_t service_id, uint16_t attrib_index,
                                T_WRITE_TYPE write_type,
                                uint16_t length, uint8_t *p_value, P_FUN_WRITE_IND_POST_PROC *p_write_ind_post_proc)
{
    T_APP_RESULT  cause  = APP_RESULT_SUCCESS;
    T_HID_CALLBACK_DATA callback_data;
    if (!p_value)
    {
        APP_PRINT_ERROR2("hids_attr_write_cb: p_value %p length= 0x%x", p_value, length);
        cause = APP_RESULT_INVALID_PDU;
        return cause;
    }

    switch (attrib_index)
    {
    default:
        cause = APP_RESULT_ATTR_NOT_FOUND;
        break;
#if RCU_VOICE_EN
    case GATT_SRV_HID_VOICE_OUTPUT_INDEX:
        APP_PRINT_INFO1("--> host write voice output 0x%x ", *p_value);
        callback_data.msg_type = SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE;
        callback_data.msg_data.write.write_type = HID_WRITE_VIOCE_CMD;
        callback_data.msg_data.write.write_parameter.voice_enable = *p_value;
        break;
#endif
    case GATT_SVC_HID_PROTOCOL_MODE_INDEX:
        {
            hids_protocol_mode = *p_value;
            callback_data.msg_type = SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE;
            callback_data.msg_data.write.write_type = HID_WRITE_PROTOCOL_MODE;
            callback_data.msg_data.write.write_parameter.protocol_mode = *p_value;

            cause = APP_RESULT_SUCCESS;
            APP_PRINT_INFO1("--> host write hids_protocol_mode %d ",
                            hids_protocol_mode);
        }
        break;
    case GATT_SVC_HID_CONTROL_POINT_INDEX:
        {
            hids_suspand_mode = *p_value;
            callback_data.msg_type = SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE;
            callback_data.msg_data.write.write_type = HID_WRITE_SUSPEND_MODE;
            callback_data.msg_data.write.write_parameter.suspend_mode = *p_value;

            cause = APP_RESULT_SUCCESS;
            APP_PRINT_INFO1("--> host write hids_suspand_mode %d",
                            hids_suspand_mode);
        }
        break;
    case GATT_SRV_HID_KB_INPUT_INDEX:
#if RCU_VOICE_EN
    case GATT_SRV_HID_VOICE_INPUT_INDEX:
#endif
#if FEATURE_SUPPORT_MULTIMEDIA_KEYBOARD
    case GATT_SRV_HID_MM_KB_INPUT_INDEX:
#endif
        {
            callback_data.msg_type = SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE;
            callback_data.msg_data.write.write_type = HID_WRITE_INPUT_REPORT;
            callback_data.msg_data.write.write_parameter.report_data.reportLen = length;
            callback_data.msg_data.write.write_parameter.report_data.report = p_value;
            cause = APP_RESULT_SUCCESS;
        }
        break;
    }

    if (pfn_hids_cb && (cause == APP_RESULT_SUCCESS))
    {
        pfn_hids_cb(service_id, (void *)&callback_data);
    }

    return cause;

}

/**
 * @brief update CCCD bits from stack.
 *
 * @param conn_id           Connection ID.
 * @param service_id        Service ID.
 * @param index             Attribute index of characteristic data.
 * @param ccc_bits          CCCD bits from stack.
 * @return None
*/
void hids_cccd_update_cb(uint8_t conn_id, T_SERVER_ID service_id, uint16_t index, uint16_t ccc_bits)
{
    T_HID_CALLBACK_DATA callback_data;
    callback_data.msg_type = SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION;
    bool handle = true;

    APP_PRINT_INFO2("bas_cccd_update_cb index = %d ccc_bits %x", index, ccc_bits);
    switch (index)
    {
    case GATT_SRV_HID_KB_CCCD_INDEX:
        {
            if (ccc_bits & GATT_CLIENT_CHAR_CONFIG_NOTIFY)
            {
                callback_data.msg_data.notification_indification_index = HID_NOTIFY_INDICATE_KB_ENABLE;
            }
            else
            {
                callback_data.msg_data.notification_indification_index = HID_NOTIFY_INDICATE_KB_DISABLE;
            }
        }
        break;
#if RCU_VOICE_EN
    case (GATT_SRV_HID_VOICE_INPUT_CCCD_INDEX):
        {
            if (ccc_bits & GATT_CLIENT_CHAR_CONFIG_NOTIFY)
            {
                callback_data.msg_data.notification_indification_index = HID_NOTIFY_INDICATE_VOICE_ENABLE;
            }
            else if ((ccc_bits & GATT_CLIENT_CHAR_CONFIG_NOTIFY) == 0)
            {
                callback_data.msg_data.notification_indification_index = HID_NOTIFY_INDICATE_VOICE_DISABLE;
            }
        }
        break;
#endif

#if FEATURE_SUPPORT_MULTIMEDIA_KEYBOARD
    case GATT_SRV_HID_MM_KB_INPUT_CCCD_INDEX:
        if (ccc_bits & GATT_CLIENT_CHAR_CONFIG_NOTIFY)
        {
            callback_data.msg_data.notification_indification_index = HID_NOTIFY_INDICATE_MM_KB_ENABLE;

        }
        else
        {
            callback_data.msg_data.notification_indification_index = HID_NOTIFY_INDICATE_MM_KB_DISABLE;
        }
        break;
#endif
    default:
        handle = false;
        break;
    }

    if (pfn_hids_cb && (handle == true))
    {
        pfn_hids_cb(service_id, (void *)&callback_data);
    }

    return;
}

/**
 * @brief HID Service Callbacks.
*/
const T_FUN_GATT_SERVICE_CBS hids_cbs =
{
    hids_attr_read_cb,  // Read callback function pointer
    hids_attr_write_cb, // Write callback function pointer
    hids_cccd_update_cb  // Authorization callback function pointer
};

/**
  * @brief       Add HIDS service to the BLE stack database.
  *
  *
  * @param[in]   p_func  Callback when service attribute was read, write or cccd update.
  * @return Service id generated by the BLE stack: @ref T_SERVER_ID.
  * @retval 0xFF Operation failure.
  * @retval Others Service id assigned by stack.
  *
  * <b>Example usage</b>
  * \code{.c}
     void profile_init()
     {
         server_init(1);
         bas_id = hids_add_service(app_handle_profile_message);
     }
  * \endcode
  */
uint8_t hids_add_service(void *p_func)
{
    uint8_t service_id;
    if (false == server_add_service(&service_id,
                                    (uint8_t *)hids_attr_table,
                                    hids_attr_tbl_size,
                                    hids_cbs))
    {
        APP_PRINT_ERROR1("hids_add_service: ServiceId %d", service_id);
        service_id = 0xff;
    }

    pfn_hids_cb = (P_FUN_SERVER_GENERAL_CB)p_func;
    return service_id;
}
