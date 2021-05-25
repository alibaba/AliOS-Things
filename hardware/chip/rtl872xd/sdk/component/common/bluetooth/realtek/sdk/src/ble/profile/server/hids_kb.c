/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     hids_kb.c
  * @brief    Source file for using Human Interface Device Service.
  * @details  Global data and function implement.
  * @author   Jeff_Zheng
  * @date     2017-12-01
  * @version  v1.0
  * *************************************************************************************
  */

#include <string.h>
#include "trace_app.h"
#include "profile_server.h"
#include "hids_kb.h"


#define GATT_UUID_HID                           0x1812
#define GATT_UUID_CHAR_PROTOCOL_MODE            0x2A4E
#define GATT_UUID_CHAR_REPORT                   0x2A4D
#define GATT_UUID_CHAR_REPORT_MAP               0x2A4B
#define GATT_UUID_CHAR_BOOT_KB_IN_REPORT        0x2A22
#define GATT_UUID_CHAR_BOOT_KB_OUT_REPORT       0x2A32
#define GATT_UUID_CHAR_HID_INFO                 0x2A4A
#define GATT_UUID_CHAR_HID_CONTROL_POINT        0x2A4C

/* Report ID for General Keyboard, change: 0x03 to 0x01 */
#define HOGP_KB_REPORT_ID                       0x03
#define HOGP_MM_REPORT_ID                       0x04
#define MULTIMEDIA_KEYBOARD
T_HID_INFO hid_info = {0, 0, 0x0100};
T_HID_PROTOCOL_MODE hid_protocol_mode = BOOT_PROTOCOL_MODE;
uint8_t hid_suspand_mode = 0;
uint16_t external_report_refer = 0;

const uint8_t hids_report_descriptor[] =
{
    0x05, 0x01,     /* USAGE_PAGE       (Generic Desktop) */
    0x09, 0x06,     /* USAGE            (Keyboard) */
    0xa1, 0x01,     /* COLLECTION       (Application) */
    0x85, HOGP_KB_REPORT_ID,    /* REPORT_ID        (3) */
    0x05, 0x07,     /* USAGE_PAGE       (Keyboard) */
    0x19, 0xe0,     /* USAGE_MINIMUM    (Keyboard Left Control) */
    0x29, 0xe7,     /* USAGE_MAXIMUM    (Keyboard Right GUI) */
    0x15, 0x00,     /* LOGICAL_MINIMUM  (0) */
    0x25, 0x01,     /* LOGICAL_MAXIMUM  (1) */
    0x75, 0x01,     /* REPORT_SIZE      (1) */
    0x95, 0x08,     /* REPORT_COUNT     (8) */
    0x81, 0x02,     /* INPUT            (Data,Var,Abs) */
    0x95, 0x01,     /* REPORT_COUNT     (1) */
    0x75, 0x08,     /* REPORT_SIZE      (8) */
    0x81, 0x01,     /* INPUT            (Cnst,Var,Abs) */
    0x95, 0x05,     /* REPORT_COUNT     (5) */
    0x75, 0x01,     /* REPORT_SIZE      (1) */
    0x05, 0x08,     /* USAGE_PAGE       (LEDs) */
    0x19, 0x01,     /* USAGE_MINIMUM    (Num Lock) */
    0x29, 0x05,     /* USAGE_MAXIMUM    (Kana) */
    0x91, 0x02,     /* OUTPUT           (Data,Var,Abs) */
    0x95, 0x01,     /* REPORT_COUNT     (1) */
    0x75, 0x03,     /* REPORT_SIZE      (3) */
    0x91, 0x01,     /* OUTPUT           (Cnst,Var,Abs) */
    0x95, 0x06,     /* REPORT_COUNT     (6) */
    0x75, 0x08,     /* REPORT_SIZE      (8) */
    0x15, 0x00,     /* LOGICAL_MINIMUM  (0) */
    0x25, 0xa4,     /* LOGICAL_MAXIMUM  (164) */    /* Can be 255 */
    0x05, 0x07,     /* USAGE_PAGE       (Keyboard) */
    0x19, 0x00,     /* USAGE_MINIMUM    (Reserved-no event indicated) */
    0x29, 0xa4,     /* USAGE_MAXIMUM    (Keyboard Application) */       /* Can be 255 */
    0x81, 0x00,     /* INPUT            (Data,Ary,Abs) */
    0xc0,           /* END_COLLECTION */
#ifdef MULTIMEDIA_KEYBOARD
    0x05, 0x0c,     /* USAGE_PAGE       (Consumer) */
    0x09, 0x01,     /* USAGE            (Consumer Control) */
    0xa1, 0x01,     /* COLLECTION       (Application) */
    0x85, HOGP_MM_REPORT_ID,     /* REPORT_ID        (4) */
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
    0xc0            /* END_COLLECTION */
#endif
};

static P_FUN_SERVER_GENERAL_CB pfn_hids_cb = NULL;

static const T_ATTRIB_APPL hids_attr_tbl[] =
{
    /* <<Primary Service>>, .. 0*/
    {
        (ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_LE),  /* wFlags     */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_PRIMARY_SERVICE),
            HI_WORD(GATT_UUID_PRIMARY_SERVICE),
            LO_WORD(GATT_UUID_HID),               /* service UUID */
            HI_WORD(GATT_UUID_HID)
        },
        UUID_16BIT_SIZE,                            /* bValueLen     */
        NULL,                                       /* pValueContext */
        GATT_PERM_READ                              /* wPermissions  */
    },

    /* <<Characteristic>>, .. 1*/
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_READ | GATT_CHAR_PROP_WRITE_NO_RSP,                    /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },

    /*  HID Protocol Mode characteristic value ..2*/
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_PROTOCOL_MODE),
            HI_WORD(GATT_UUID_CHAR_PROTOCOL_MODE)
        },
        0,                                          /* variable size */
        (void *)NULL,
        GATT_PERM_READ_AUTHEN_REQ | GATT_PERM_WRITE_AUTHEN_REQ                        /* wPermissions */
    },

    /* <<Characteristic>>, .. 3*/
    {
        ATTRIB_FLAG_VALUE_INCL,                   /* wFlags */
        {                                       /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_READ | GATT_CHAR_PROP_WRITE | GATT_CHAR_PROP_NOTIFY,           /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                        /* bValueLen */
        NULL,
        GATT_PERM_READ                            /* wPermissions */
    },

    /*  HID Report characteristic value .. 4*/
    {
        ATTRIB_FLAG_VALUE_APPL,                   /* wFlags */
        {                                       /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_REPORT),
            HI_WORD(GATT_UUID_CHAR_REPORT)
        },
        0,                                        /* variable size */
        (void *)NULL,
        GATT_PERM_READ_AUTHEN_REQ | GATT_PERM_WRITE_AUTHEN_REQ /* wPermissions */
    },

    /* client characteristic configuration .. 5*/
    {
        (ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_CCCD_APPL),                  /* wFlags */
        {                                         /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            HI_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            /* NOTE: this value has an instantiation for each client, a write to */
            /* this attribute does not modify this default value:                */
            LO_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT), /* client char. config. bit field */
            HI_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT)
        },
        2,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ_AUTHEN_REQ | GATT_PERM_WRITE_AUTHEN_REQ)          /* wPermissions */
    },

    /*report ID map reference descriptor .. 6*/
    {
        (ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_CCCD_APPL),                  /* wFlags */
        {                                         /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_REPORT_REFERENCE),
            HI_WORD(GATT_UUID_CHAR_REPORT_REFERENCE),
            HOGP_KB_REPORT_ID,
            HID_INPUT_TYPE,
        },
        2,                                          /* bValueLen */
        NULL,//(void*)&cPointerInputReportIdMap,
        (GATT_PERM_READ_AUTHEN_REQ)        /* wPermissions */
    },

    /* <<Characteristic>>, .. 7*/
    {
        ATTRIB_FLAG_VALUE_INCL,                   /* wFlags */
        {                                         /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_READ | GATT_CHAR_PROP_WRITE | GATT_CHAR_PROP_WRITE_NO_RSP,   /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                        /* bValueLen */
        NULL,
        GATT_PERM_READ                            /* wPermissions */
    },

    /*  HID Report characteristic value .. 8*/
    {
        ATTRIB_FLAG_VALUE_APPL,                   /* wFlags */
        {                                         /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_REPORT),
            HI_WORD(GATT_UUID_CHAR_REPORT)
        },
        0,                                        /* variable size */
        (void *)NULL,
        GATT_PERM_READ_AUTHEN_REQ | GATT_PERM_WRITE_AUTHEN_REQ                          /* wPermissions */
    },

    /*report ID map reference descriptor .. 9*/
    {
        (ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_CCCD_APPL),                   /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_REPORT_REFERENCE),
            HI_WORD(GATT_UUID_CHAR_REPORT_REFERENCE),
            HOGP_KB_REPORT_ID,
            HID_OUTPUT_TYPE
        },
        2,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ_AUTHEN_REQ)          /* wPermissions */
    },

    /* <<Characteristic>>, .. 10*/
    {
        ATTRIB_FLAG_VALUE_INCL,                   /* wFlags */
        {                                         /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_READ | GATT_CHAR_PROP_WRITE, /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                        /* bValueLen */
        NULL,
        GATT_PERM_READ                            /* wPermissions */
    },

    /*  HID Report characteristic value .. 11*/
    {
        ATTRIB_FLAG_VALUE_APPL,                   /* wFlags */
        {                                         /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_REPORT),
            HI_WORD(GATT_UUID_CHAR_REPORT)
        },
        0,                                        /* variable size */
        (void *)NULL,
        GATT_PERM_READ_AUTHEN_REQ | GATT_PERM_WRITE_AUTHEN_REQ                          /* wPermissions */
    },

    /*report ID map reference descriptor .. 12*/
    {
        (ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_CCCD_APPL),                   /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_REPORT_REFERENCE),
            HI_WORD(GATT_UUID_CHAR_REPORT_REFERENCE),
            /* NOTE: this value has an instantiation for each client, a write to */
            /* this attribute does not modify this default value:                */
            HOGP_KB_REPORT_ID,
            HID_FEATURE_TYPE
        },
        2,                                          /* bValueLen */
        (void *)NULL,
        (GATT_PERM_READ_AUTHEN_REQ)          /* wPermissions */
    },

    /* <<Characteristic>>, .. 13*/
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_READ,                  /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },

    /*  HID report map characteristic value .. 14*/
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_REPORT_MAP),
            HI_WORD(GATT_UUID_CHAR_REPORT_MAP)
        },
        0,                                          /* variable size */
        (void *)NULL,
        GATT_PERM_READ_AUTHEN_REQ                              /* wPermissions */
    },

    /* <<EXTERNAL_REPORT_REFERENCE>>, .. 15*/
    {
        ATTRIB_FLAG_VALUE_APPL,                                 /* flags */
        {                                                       /* type_value */
            LO_WORD(GATT_UUID_CHAR_EXTERNAL_REPORT_REFERENCE),
            HI_WORD(GATT_UUID_CHAR_EXTERNAL_REPORT_REFERENCE),
        },
        0,                                                      /* bValueLen */
        (void *)NULL,
        GATT_PERM_READ_AUTHEN_REQ                                          /* permissions */
    },

    /* <<Characteristic>>, .. 16*/
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_READ | GATT_CHAR_PROP_WRITE | GATT_CHAR_PROP_NOTIFY, /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },

    /*  HID boot keyboard input characteristic value .. 17*/
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHAR_BOOT_KB_IN_REPORT),
            HI_WORD(GATT_UUID_CHAR_BOOT_KB_IN_REPORT)
        },
        0,                                          /* variable size */
        (void *)NULL,
        GATT_PERM_READ_AUTHEN_REQ | GATT_PERM_WRITE_AUTHEN_REQ /* permissions */
    },

    /* client characteristic configuration .. 18*/
    {
        (ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_CCCD_APPL),            /* flags */
        {                                         /* type_value */
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

    /* <<Characteristic>>, .. 19*/
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_READ | GATT_CHAR_PROP_WRITE | GATT_CHAR_PROP_WRITE_NO_RSP,                 /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },

    /*  HID boot keyboard output characteristic value .. 20*/
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHAR_BOOT_KB_OUT_REPORT),
            HI_WORD(GATT_UUID_CHAR_BOOT_KB_OUT_REPORT)
        },
        0,                                          /* variable size */
        (void *)NULL,
        GATT_PERM_READ_AUTHEN_REQ | GATT_PERM_WRITE_AUTHEN_REQ                        /* permissions */
    },

    /* <<Characteristic>>, .. 21*/
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_READ,                    /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },

    /*  HID Information characteristic value .. 22*/
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_HID_INFO),
            HI_WORD(GATT_UUID_CHAR_HID_INFO)
        },
        0,                                          /* variable size */
        (void *)NULL,
        GATT_PERM_READ_AUTHEN_REQ                              /* wPermissions */
    },

    /* <<Characteristic>>, .. 23*/
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_WRITE_NO_RSP,                      /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },

    /*  HID controlPoint characteristic value .. 24*/
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_HID_CONTROL_POINT),
            HI_WORD(GATT_UUID_CHAR_HID_CONTROL_POINT)
        },
        0,                                          /* variable size */
        (void *)NULL,
        GATT_PERM_READ_AUTHEN_REQ | GATT_PERM_WRITE_AUTHEN_REQ            /* wPermissions */
    },

#ifdef MULTIMEDIA_KEYBOARD
    /* <<Characteristic>>, .. 25*/
    {
        ATTRIB_FLAG_VALUE_INCL,                   /* wFlags */
        {                                         /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_NOTIFY | GATT_CHAR_PROP_READ | GATT_CHAR_PROP_WRITE,           /* characteristic properties */
            //XXXXMJMJ GATT_CHAR_PROP_INDICATE,                  /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                        /* bValueLen */
        NULL,
        GATT_PERM_READ                            /* wPermissions */
    },

    /*  HID Report characteristic value ,multimedia keyboard Input 26*/
    {
        ATTRIB_FLAG_VALUE_APPL,                   /* wFlags */
        {                                         /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_REPORT),
            HI_WORD(GATT_UUID_CHAR_REPORT)
        },
        3,                                        /* variable size */
        (void *)NULL,
        GATT_PERM_READ_AUTHEN_REQ | GATT_PERM_WRITE_AUTHEN_REQ                          /* wPermissions */
    },

    /* client characteristic configuration 27*/
    {
        (ATTRIB_FLAG_VALUE_INCL |                   /* wFlags */
         ATTRIB_FLAG_CCCD_APPL),
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            HI_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            /* NOTE: this value has an instantiation for each client, a write to */
            /* this attribute does not modify this default value:                */
            LO_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT), /* client char. config. bit field */
            HI_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT)
        },
        2,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ_AUTHEN_REQ | GATT_PERM_WRITE_AUTHEN_REQ)          /* wPermissions */
    },

    /*report ID map reference descriptor 28*/
    {
        (ATTRIB_FLAG_VALUE_INCL |                   /* wFlags */
         ATTRIB_FLAG_CCCD_APPL),
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_REPORT_REFERENCE),
            HI_WORD(GATT_UUID_CHAR_REPORT_REFERENCE),
            /* NOTE: this value has an instantiation for each client, a write to */
            /* this attribute does not modify this default value:                */
            HOGP_MM_REPORT_ID, /* client char. config. bit field */
            HID_INPUT_TYPE
        },
        2,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ_AUTHEN_REQ)          /* wPermissions */
    }
#endif

};

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
bool hids_set_parameter(T_HIDS_PARAM_TYPE param_type, uint8_t length, void *value_ptr)
{
    bool ret = true;

    switch (param_type)
    {
    case HID_PROTOCOL_MODE:
        {
            hid_protocol_mode = (T_HID_PROTOCOL_MODE) * ((uint8_t *)value_ptr);
        }
        break;

    case HID_REPORT_INPUT:
        break;

    case HID_REPORT_OUTPUT:
        break;

    case HID_REPORT_FEATURE:
        break;

    case HID_REPORT_MAP:
        break;

    case HID_EXTERNAL_REPORT_REFER:
        {
            external_report_refer = *(uint16_t *)value_ptr;
        }
        break;

    case HID_BOOT_KB_IN_REPORT:
        break;

    case HID_BOOT_KB_OUT_REPORT:
        break;

    case HID_INFO:
        {
            memcpy((void *)&hid_info, value_ptr, length);
        }
        break;

    case HID_CONTROL_POINT:
        hid_suspand_mode = *((uint8_t *)value_ptr);
        break;

    default:
        ret = false;
        break;
    }
    return ret;
}


static T_APP_RESULT hids_attr_read_cb(uint8_t conn_id, T_SERVER_ID service_id,
                                      uint16_t attrib_index,
                                      uint16_t offset, uint16_t *p_length, uint8_t **pp_value)
{
    T_APP_RESULT cause = APP_RESULT_SUCCESS;
    T_HID_CALLBACK_DATA callback_data;
    callback_data.msg_type = SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE;
    callback_data.conn_id = conn_id;

    callback_data.msg_type = SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE;

    switch (attrib_index)
    {
    default:
        cause = APP_RESULT_ATTR_NOT_FOUND;
        break;

    case GATT_SVC_HID_PROTOCOL_MODE_INDEX:
        callback_data.msg_data.read_value_index = GATT_SVC_HID_PROTOCOL_MODE_INDEX;
        cause = pfn_hids_cb(service_id, (void *)&callback_data);
        *pp_value = (uint8_t *)&hid_protocol_mode;
        *p_length = sizeof(hid_protocol_mode);
        break;

    case GATT_SVC_HID_REPORT_INPUT_INDEX:
        break;

    case GATT_SVC_HID_REPORT_OUTPUT_INDEX:
        break;

    case GATT_SVC_HID_REPORT_FEATURE_INDEX:
        break;

    case GATT_SVC_HID_REPORT_MAP_INDEX:
        *pp_value = (uint8_t *)hids_report_descriptor;
        *p_length = sizeof(hids_report_descriptor);
        break;

    case GATT_SVC_HID_EXTERNAL_REPORT_REFER_INDEX:
        callback_data.msg_data.read_value_index = GATT_SVC_HID_EXTERNAL_REPORT_REFER_INDEX;
        cause = pfn_hids_cb(service_id, (void *)&callback_data);
        *pp_value = (uint8_t *)&external_report_refer;
        *p_length = sizeof(external_report_refer);
        break;

    case GATT_SVC_HID_BOOT_KB_IN_REPORT_INDEX:
        break;

    case GATT_SVC_HID_BOOT_KB_OUT_REPORT_INDEX:
        break;

    case GATT_SVC_HID_INFO_INDEX:
        callback_data.msg_data.read_value_index = GATT_SVC_HID_INFO_INDEX;
        cause = pfn_hids_cb(service_id, (void *)&callback_data);
        *pp_value = (uint8_t *)&hid_info;
        *p_length = sizeof(hid_info);
        break;

    case GATT_SVC_HID_CONTROL_POINT_INDEX:
        break;
    }

    return cause;
}


static T_APP_RESULT hids_attr_write_cb(uint8_t conn_id, T_SERVER_ID service_id,
                                       uint16_t attrib_index, T_WRITE_TYPE write_type,
                                       uint16_t length, uint8_t *p_value, P_FUN_WRITE_IND_POST_PROC *p_write_ind_post_proc)
{
    T_APP_RESULT cause  = APP_RESULT_SUCCESS;
    T_HID_CALLBACK_DATA callback_data;
    callback_data.msg_type = SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE;

    if (!p_value)
    {
        cause = APP_RESULT_INVALID_PDU;
        return cause;
    }

    switch (attrib_index)
    {
    default:
        cause = APP_RESULT_ATTR_NOT_FOUND;
        break;

    case GATT_SVC_HID_PROTOCOL_MODE_INDEX:
        callback_data.msg_data.read_value_index = GATT_SVC_HID_PROTOCOL_MODE_INDEX;
        callback_data.msg_data.write_msg.write_type = write_type;
        callback_data.msg_data.write_msg.write_parameter.protocol_mode = *p_value;
        hids_set_parameter(HID_PROTOCOL_MODE, length, p_value);
        break;

    case GATT_SVC_HID_REPORT_INPUT_INDEX:
        break;

    case GATT_SVC_HID_REPORT_OUTPUT_INDEX:
        callback_data.msg_data.read_value_index = GATT_SVC_HID_REPORT_OUTPUT_INDEX;
        callback_data.msg_data.write_msg.write_type = write_type;
        callback_data.msg_data.write_msg.write_parameter.output = *p_value;
        break;

    case GATT_SVC_HID_REPORT_FEATURE_INDEX:
        break;

    case GATT_SVC_HID_REPORT_MAP_INDEX:
        break;

    case GATT_SVC_HID_BOOT_KB_IN_REPORT_INDEX:
        break;

    case GATT_SVC_HID_BOOT_KB_OUT_REPORT_INDEX:
        break;

    case GATT_SVC_HID_INFO_INDEX:
        break;

    case GATT_SVC_HID_CONTROL_POINT_INDEX:
        break;
    }

    if (pfn_hids_cb && (cause == APP_RESULT_SUCCESS))
    {
        pfn_hids_cb(service_id, (void *)&callback_data);
    }

    return cause;

}

void hids_cccd_update_cb(uint8_t conn_id, T_SERVER_ID service_id, uint16_t index, uint16_t ccc_bits)
{
    bool cause = true;
    T_HID_CALLBACK_DATA callback_data;
    callback_data.conn_id = conn_id;
    callback_data.msg_type = SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION;

    APP_PRINT_INFO2("hids_cccd_update_cb index = %d ccc_bits %x", index, ccc_bits);

    switch (index)
    {
    default:
        cause = false;
        break;

    case GATT_SVC_HID_REPORT_INPUT_CCCD_INDEX:
        {
            callback_data.msg_data.not_ind_data.index = GATT_SVC_HID_REPORT_INPUT_CCCD_INDEX;
            if (ccc_bits & GATT_PDU_TYPE_NOTIFICATION)
            {
                callback_data.msg_data.not_ind_data.value = NOTIFY_ENABLE;
            }
            else
            {
                callback_data.msg_data.not_ind_data.value = NOTIFY_DISABLE;
            }
            break;
        }

    case GATT_SVC_HID_BOOT_KB_IN_REPORT_CCCD_INDEX:
        {
            callback_data.msg_data.not_ind_data.index = GATT_SVC_HID_BOOT_KB_IN_REPORT_CCCD_INDEX;
            if (ccc_bits & GATT_PDU_TYPE_NOTIFICATION)
            {
                callback_data.msg_data.not_ind_data.value = NOTIFY_ENABLE;
            }
            else
            {
                callback_data.msg_data.not_ind_data.value = NOTIFY_DISABLE;
            }
            break;
        }

    }

    if (pfn_hids_cb && (cause == true))
    {
        pfn_hids_cb(service_id, (void *)&callback_data);
    }

    return;
}


/**
 * @brief       Send HIDS notification data .
 *
 *
 * @param[in]   conn_id  Connection id.
 * @param[in]   service_id  Service id.
 * @param[in]   index  hids characteristic index.
 * @param[in]   p_data report value pointer.
 * @param[in]   data_len length of report data.
 * @return Operation result.
 * @retval true Operation success.
 * @retval false Operation failure.
 *
 * <b>Example usage</b>
 * \code{.c}
    void test(void)
    {
        uint8_t conn_id = 0;
        T_SERVER_ID service_id = hids_id;
        uint8_t hid_report_input[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};
        hids_send_report(conn_id, service_id, GATT_SVC_HID_REPORT_INPUT_INDEX, hid_report_input, sizeof(hid_report_input));
    }
 * \endcode
 */
bool hids_send_report(uint8_t conn_id, T_SERVER_ID service_id, uint16_t index, uint8_t *p_data,
                      uint16_t data_len)
{
    APP_PRINT_INFO1("hids_send_report data_len %d", data_len);
    return server_send_data(conn_id, service_id, index, p_data, data_len, GATT_PDU_TYPE_NOTIFICATION);
}



uint16_t hids_attr_tbl_len = sizeof(hids_attr_tbl);

const T_FUN_GATT_SERVICE_CBS hids_cbs =
{
    hids_attr_read_cb,      // Read callback function pointer
    hids_attr_write_cb,     // Write callback function pointer
    hids_cccd_update_cb,    // Authorization callback function pointer
};

/**
  * @brief       Add HID service to the BLE stack database.
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
         hids_id = hids_add_service(app_handle_profile_message);
     }
  * \endcode
  */
T_SERVER_ID hids_add_service(void *p_func)
{
    T_SERVER_ID service_id;
    if (false == server_add_service(&service_id, (uint8_t *)hids_attr_tbl, hids_attr_tbl_len, hids_cbs))
    {
        APP_PRINT_ERROR1("hids_add_service: ServiceId %d", service_id);
        service_id = 0xff;
    }

    pfn_hids_cb = (P_FUN_SERVER_GENERAL_CB)p_func;
    return service_id;
}

