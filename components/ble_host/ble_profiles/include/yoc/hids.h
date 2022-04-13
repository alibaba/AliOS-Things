/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef _BT_HIDS_H_
#define _BT_HIDS_H_

typedef void *hids_handle_t;

hids_handle_t hids_init(uint8_t     mode);
int set_data_map(uint8_t u_data[], uint16_t len, uint8_t u_type);
int hids_key_send(hids_handle_t handle, uint8_t *key_code, uint16_t us_len);
int hids_notify_send(hids_handle_t handle, uint8_t *key_code, uint16_t us_len);

typedef enum {
    HIDS_IDX_SVC,
    HIDS_IDX_REPORT_MAP_CHAR,
    HIDS_IDX_REPORT_MAP_VAL,
    HIDS_IDX_REPORT_INPUT_CHAR,
    HIDS_IDX_REPORT_INPUT_VAL,
    HIDS_IDX_REPORT_INPUT_REF,
    HIDS_IDX_REPORT_INPUT_CCC,
    HIDS_IDX_REPORT_OUTPUT_CHAR,
    HIDS_IDX_REPORT_OUTPUT_VAL,
    HIDS_IDX_REPORT_OUTPUT_REF,
    //HIDS_IDX_REPORT_FEATURE_CHAR,
    //HIDS_IDX_REPORT_FEATURE_VAL,
    //HIDS_IDX_REPORT_FEATURE_DES,
    HIDS_IDX_BOOT_KB_INPUT_REPORT_CHAR,
    HIDS_IDX_BOOT_KB_INPUT_REPORT_VAL,
    HIDS_IDX_BOOT_KB_INPUT_REPORT_CCC,
    HIDS_IDX_BOOT_KB_OUTPUT_REPORT_CHAR,
    HIDS_IDX_BOOT_KB_OUTPUT_REPORT_VAL,
    HIDS_IDX_INFO_CHAR,
    HIDS_IDX_INFO_VAL,
    HIDS_IDX_CTRL_CHAR,
    HIDS_IDX_CTRL_VAL,
    HIDS_IDX_PROTOCOL_MODE_CHAR,
    HIDS_IDX_PROTOCOL_MODE_VAL,

    HIDS_IDX_MAX,
} hids_event_e;

typedef void (*hids_event_cb)(hids_event_e event, void *event_data); //Event call back.
int  init_hids_call_func(int32_t idx, hids_event_cb cb_event);


enum {
    REPORT_MAP = 0x00,
    REPORT_INPUT = 0x01,
    REPORT_OUTPUT = 0x02,
    //REPORT_FEATURE = 0x03,

    REPORT_MAX,
};

enum {
    HIDS_BOOT_PROTOCOL_MODE = 0x00,
    HIDS_REPORT_PROTOCOL_MODE = 0x01,
};



#endif
