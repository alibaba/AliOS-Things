/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
//#include <aos/osal_debug.h>
#include <aos/ble.h>
#include <yoc/dis.h>

#define TAG "DIS"

enum {
    DIS_IDX_SVC,
    DIS_IDX_MANU_NUM_CHAR,
    DIS_IDX_MANU_NUM_VAL,
    DIS_IDX_MODEL_NUM_CHAR,
    DIS_IDX_MODEL_NUM_VAL,
    DIS_IDX_SER_NUM_CHAR,
    DIS_IDX_SER_NUM_VAL,
    DIS_IDX_HW_REV_CHAR,
    DIS_IDX_HW_REV_VAL,
    DIS_IDX_FW_REV_CHAR,
    DIS_IDX_FW_REV_VAL,
    DIS_IDX_SW_REV_CHAR,
    DIS_IDX_SW_REV_VAL,
    DIS_IDX_SYS_ID_CHAR,
    DIS_IDX_SYS_ID_VAL,
    DIS_IDX_REGU_LIST_CHAR,
    DIS_IDX_REGU_LIST_VAL,
    DIS_IDX_PNP_ID_CHAR,
    DIS_IDX_PNP_ID_VAL,

    DIS_IDX_MAX,
};

typedef struct _dis_t {
    uint16_t conn_handle;
    dis_info_t *info;
    uint16_t dis_svc_handle;
    uint16_t manu_num_char_val_handle;
    uint16_t model_num_char_val_handle;
    uint16_t ser_num_char_val_handle;
    uint16_t hw_rev_char_val_handle;
    uint16_t fw_rev_char_val_handle;
    uint16_t sw_rev_char_val_handle;
    uint16_t regu_list_char_val_handle;
    uint16_t sys_id_char_val_handle;
    uint16_t pnp_id_char_val_handle;
} dis_t;

dis_t g_dis;
gatt_service dis_service;

static  gatt_attr_t dis_attrs[DIS_IDX_MAX] = {
    [DIS_IDX_SVC] = GATT_PRIMARY_SERVICE_DEFINE(UUID_DIS),
    [DIS_IDX_MANU_NUM_CHAR] = GATT_CHAR_DEFINE(UUID_DIS_MANUFACTURER_NAME,  GATT_CHRC_PROP_READ),
    [DIS_IDX_MANU_NUM_VAL] = GATT_CHAR_VAL_DEFINE(UUID_DIS_MANUFACTURER_NAME, GATT_PERM_READ),
    [DIS_IDX_MODEL_NUM_CHAR] = GATT_CHAR_DEFINE(UUID_DIS_MODEL_NUMBER,  GATT_CHRC_PROP_READ),
    [DIS_IDX_MODEL_NUM_VAL] = GATT_CHAR_VAL_DEFINE(UUID_DIS_MODEL_NUMBER, GATT_PERM_READ),
    [DIS_IDX_SER_NUM_CHAR] = GATT_CHAR_DEFINE(UUID_DIS_SERIAL_NUMBER,  GATT_CHRC_PROP_READ),
    [DIS_IDX_SER_NUM_VAL] = GATT_CHAR_VAL_DEFINE(UUID_DIS_SERIAL_NUMBER, GATT_PERM_READ),
    [DIS_IDX_HW_REV_CHAR] = GATT_CHAR_DEFINE(UUID_DIS_HARDWARE_REVISION,  GATT_CHRC_PROP_READ),
    [DIS_IDX_HW_REV_VAL] = GATT_CHAR_VAL_DEFINE(UUID_DIS_HARDWARE_REVISION, GATT_PERM_READ),
    [DIS_IDX_FW_REV_CHAR] = GATT_CHAR_DEFINE(UUID_DIS_FIRMWARE_REVISION,  GATT_CHRC_PROP_READ),
    [DIS_IDX_FW_REV_VAL] = GATT_CHAR_VAL_DEFINE(UUID_DIS_FIRMWARE_REVISION, GATT_PERM_READ),
    [DIS_IDX_SW_REV_CHAR] = GATT_CHAR_DEFINE(UUID_DIS_SOFTWARE_REVISION,  GATT_CHRC_PROP_READ),
    [DIS_IDX_SW_REV_VAL] = GATT_CHAR_VAL_DEFINE(UUID_DIS_SOFTWARE_REVISION, GATT_PERM_READ),
    [DIS_IDX_SYS_ID_CHAR] = GATT_CHAR_DEFINE(UUID_DIS_SYSTEM_ID,  GATT_CHRC_PROP_READ),
    [DIS_IDX_SYS_ID_VAL] = GATT_CHAR_VAL_DEFINE(UUID_DIS_SYSTEM_ID, GATT_PERM_READ),
    [DIS_IDX_REGU_LIST_CHAR] = GATT_CHAR_DEFINE(UUID_IEEE_REGULATORY_CERTIFICATION_DATA_LIST,  GATT_CHRC_PROP_READ),
    [DIS_IDX_REGU_LIST_VAL] = GATT_CHAR_VAL_DEFINE(UUID_IEEE_REGULATORY_CERTIFICATION_DATA_LIST, GATT_PERM_READ),
    [DIS_IDX_PNP_ID_CHAR] = GATT_CHAR_DEFINE(UUID_DIS_PNP_ID,  GATT_CHRC_PROP_READ),
    [DIS_IDX_PNP_ID_VAL] = GATT_CHAR_VAL_DEFINE(UUID_DIS_PNP_ID, GATT_PERM_READ),
};

static inline void read_dis_info(evt_data_gatt_char_read_t *e, void *data, uint16_t len)
{
    if (data == NULL || len == 0) {
        return ;
    }

    e->data = data;
    e->len = len;
}

static inline void read_pnp_id(evt_data_gatt_char_read_t *e, pnp_id_t *pnp_id)
{
    static uint8_t data[7] = {0};

    if (e && pnp_id) {
        data[0] = pnp_id->vendor_id_source;
        memcpy(&data[1], &pnp_id->vendor_id, 6);

        e->data = data;
        e->len = 7;
    }
}

static void event_char_read(ble_event_en event, void *event_data)
{
    evt_data_gatt_char_read_t *e = (evt_data_gatt_char_read_t *)event_data;

    if (e->char_handle < g_dis.dis_svc_handle || e->char_handle >= g_dis.dis_svc_handle + DIS_IDX_MAX) {
        return;
    }

    uint16_t dis_char_idx = e->char_handle - g_dis.dis_svc_handle;

    switch (dis_char_idx) {
        case DIS_IDX_MANU_NUM_VAL:
            read_dis_info(e, g_dis.info->manufacturer_name, strlen(g_dis.info->manufacturer_name));
            break;
        case DIS_IDX_MODEL_NUM_VAL:
            read_dis_info(e, g_dis.info->model_number, strlen(g_dis.info->model_number));
            break;

        case DIS_IDX_SER_NUM_VAL:
            read_dis_info(e, g_dis.info->serial_number, strlen(g_dis.info->serial_number));
            break;

        case DIS_IDX_HW_REV_VAL:
            read_dis_info(e, g_dis.info->hardware_revison, strlen(g_dis.info->hardware_revison));
            break;

        case DIS_IDX_FW_REV_VAL:
            read_dis_info(e, g_dis.info->firmware_revision, strlen(g_dis.info->firmware_revision));
            break;

        case DIS_IDX_SW_REV_VAL:
            read_dis_info(e, g_dis.info->software_revision, strlen(g_dis.info->software_revision));
            break;

        case DIS_IDX_SYS_ID_VAL:
            read_dis_info(e, g_dis.info->system_id, g_dis.info->system_id ? sizeof(*g_dis.info->system_id) : 0);
            break;

        case DIS_IDX_REGU_LIST_VAL:
            read_dis_info(e, g_dis.info->regu_cert_data_list ? g_dis.info->regu_cert_data_list->list : NULL,
                          g_dis.info->regu_cert_data_list ? g_dis.info->regu_cert_data_list->list_len : 0);
            break;
        case DIS_IDX_PNP_ID_VAL: {
            read_pnp_id(e, g_dis.info->pnp_id);
        }
        break;
    }

    if (e->char_handle == g_dis.manu_num_char_val_handle) {
        read_dis_info(e, g_dis.info->manufacturer_name, strlen(g_dis.info->manufacturer_name));
    } else if (e->char_handle == g_dis.model_num_char_val_handle) {
        read_dis_info(e, g_dis.info->model_number, strlen(g_dis.info->model_number));
    } else if (e->char_handle == g_dis.ser_num_char_val_handle) {
        read_dis_info(e, g_dis.info->serial_number, strlen(g_dis.info->serial_number));
    } else if (e->char_handle == g_dis.hw_rev_char_val_handle) {
        read_dis_info(e, g_dis.info->hardware_revison, strlen(g_dis.info->hardware_revison));
    } else if (e->char_handle == g_dis.fw_rev_char_val_handle) {
        read_dis_info(e, g_dis.info->firmware_revision, strlen(g_dis.info->firmware_revision));
    } else if (e->char_handle == g_dis.sw_rev_char_val_handle) {
        read_dis_info(e, g_dis.info->software_revision, strlen(g_dis.info->software_revision));
    } else if (e->char_handle == g_dis.sys_id_char_val_handle) {
        read_dis_info(e, g_dis.info->system_id, sizeof(*g_dis.info->system_id));
    } else if (e->char_handle == g_dis.regu_list_char_val_handle) {
        read_dis_info(e, g_dis.info->regu_cert_data_list->list, g_dis.info->regu_cert_data_list->list_len);
    } else if (e->char_handle == g_dis.pnp_id_char_val_handle) {
        read_dis_info(e, g_dis.info->pnp_id, sizeof(*g_dis.info->pnp_id));
    }

}

static void conn_change(ble_event_en event, void *event_data)
{
    evt_data_gap_conn_change_t *e = (evt_data_gap_conn_change_t *)event_data;

    if (e->connected == CONNECTED) {
        g_dis.conn_handle = e->conn_handle;
    } else {
        g_dis.conn_handle = 0xFFFF;
    }
}

static int dis_event_callback(ble_event_en event, void *event_data)
{
    switch (event) {
        case EVENT_GAP_CONN_CHANGE:
            conn_change(event, event_data);
            break;

        case EVENT_GATT_CHAR_READ:
            event_char_read(event, event_data);
            break;

        default:
            break;
    }

    return 0;
}

static ble_event_cb_t ble_cb = {
    .callback = dis_event_callback,
};

dis_handle_t dis_init(dis_info_t *info)
{
    int ret = 0;

    if (g_dis.info) {
        return 0;
    }

    memset(&g_dis, 0xFF, sizeof(g_dis));
    g_dis.info = NULL;

    ret = ble_stack_event_register(&ble_cb);

    if (ret) {
        goto err;
    }


    ret = ble_stack_gatt_registe_service(&dis_service,dis_attrs, DIS_IDX_MAX);

    if (ret < 0) {
        goto err;
    }

    g_dis.dis_svc_handle = ret;
#if 0

    if (info->manufacturer_name) {
        ret = ble_stack_gatt_registe_service(&bas_attrs[DIS_IDX_MANU_NUM_CHAR], 2);

        if (ret < 0) {
            goto err;
        }

        g_dis.manu_num_char_val_handle = ret + 1;
    }

    if (info->model_number) {
        ret = ble_stack_gatt_registe_service(&bas_attrs[DIS_IDX_MODEL_NUM_CHAR], 2);

        if (ret < 0) {
            goto err;
        }

        g_dis.model_num_char_val_handle = ret + 1;
    }

    if (info->serial_number) {
        ret = ble_stack_gatt_registe_service(&bas_attrs[DIS_IDX_SER_NUM_CHAR], 2);

        if (ret < 0) {
            goto err;
        }

        g_dis.ser_num_char_val_handle = ret + 1;
    }

    if (info->hardware_revison) {
        ret = ble_stack_gatt_registe_service(&bas_attrs[DIS_IDX_HW_REV_CHAR], 2);

        if (ret < 0) {
            goto err;
        }

        g_dis.hw_rev_char_val_handle = ret + 1;
    }

    if (info->firmware_revision) {
        ret = ble_stack_gatt_registe_service(&bas_attrs[DIS_IDX_FW_REV_CHAR], 2);

        if (ret < 0) {
            goto err;
        }

        g_dis.fw_rev_char_val_handle = ret + 1;
    }

    if (info->software_revision) {
        ret = ble_stack_gatt_registe_service(&bas_attrs[DIS_IDX_SW_REV_CHAR], 2);

        if (ret < 0) {
            goto err;
        }

        g_dis.sw_rev_char_val_handle = ret + 1;
    }

    if (info->system_id) {
        ret = ble_stack_gatt_registe_service(&bas_attrs[DIS_IDX_SYS_ID_CHAR], 2);

        if (ret < 0) {
            goto err;
        }

        g_dis.sys_id_char_val_handle = ret + 1;
    }

    if (info->regu_cert_data_list) {
        ret = ble_stack_gatt_registe_service(&bas_attrs[DIS_IDX_REGU_LIST_CHAR], 2);

        if (ret < 0) {
            goto err;
        }

        g_dis.regu_list_char_val_handle = ret + 1;
    }

    if (info->pnp_id) {
        ret = ble_stack_gatt_registe_service(&bas_attrs[DIS_IDX_PNP_ID_CHAR], 2);

        if (ret < 0) {
            goto err;
        }

        g_dis.pnp_id_char_val_handle = ret + 1;
    }

#endif
    g_dis.info = info;

    return &g_dis;

err:
    //LOGE(TAG, "dis init err %d\n", ret);
    return NULL;
}

