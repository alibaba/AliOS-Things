/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __GENIE_VENDOR_MODEL_H__
#define __GENIE_VENDOR_MODEL_H__

#define VENDOR_OP_ATTR_GET_STATUS 0xD0
#define VENDOR_OP_ATTR_SET_ACK 0xD1
#define VENDOR_OP_ATTR_SET_UNACK 0xD2
#define VENDOR_OP_ATTR_STATUS 0xD3
#define VENDOR_OP_ATTR_INDICATE 0xD4
#define VENDOR_OP_ATTR_CONFIME 0xD5
#define VENDOR_OP_ATTR_INDICATE_TG 0xDE
#define VENDOR_OP_ATTR_CONFIME_TG 0xDF
#define VENDOR_OP_ATTR_TRANS_MSG 0xCF
#define VENDOR_OP_ATTR_TRANS_INDICATE 0xCE
#define VENDOR_OP_ATTR_TRANS_ACK 0xCD

#define ATTR_TYPE_REPORT_VERSION 0xFF01
#define ATTR_TYPE_DEVICE_EVENT 0xF009

#define ATTR_TYPE_VOLTAGE_LEVEL 0x0105

#define VENDOR_MODEL_VERSION 10 //vendor model version:1.0
#define SDK_VERSION 13          //SDK version:1.3

/* Event List - Refer to Device Event List */
#define EL_FAULT_T 0x00    /* malfunction event */
#define EL_LOW_BAT_T 0x01  /* low battery event */
#define EL_DEV_UP_T 0x03   /* device power up */
#define EL_HW_RESET_T 0x23 /* hardware reset event */

/* Vendor timer error code */
#define ERR_CODE_UNIXTIME 0x80
#define ERR_CODE_NOTSUP_ATTR_OP 0x82
#define ERR_CODE_NOTSUP_ATTR_PARAM 0x83
#define ERR_CODE_TIMER_SETTING 0x84
#define ERR_CODE_TIMER_INDEX 0x85
#define ERR_CODE_TIMER_FULL 0x86
#define ERR_CODE_TIMER_PRIORDIC_PARAM 0x87

/* Vendor timer ali attr type */
#define ERROR_CODE_T 0x0000
#define TIMING_TIMEOUT_T 0xF009
#define TIMING_SETTING_T 0xF010
#define TIMING_PERIODIC_SETTING_T 0xF011
#define TIMING_DELETE_T 0xF012
#define TIMING_SYNC_T 0xF01D
#define TIMEZONE_SETTING_T 0xF01E
#define UNIX_TIME_T 0xF01F

#define EL_TIMING_TIMEOUT_T 0x11 /* timer timeout event */

#ifndef VENDOR_MODEL_MSG_MAX_RETRY_TIMES
#define VENDOR_MODEL_MSG_DFT_RETRY_TIMES 45
#endif

#ifndef VENDOR_MODEL_MSG_MAX_RETRY_TIMES
#define VENDOR_MODEL_MSG_MAX_RETRY_TIMES 45
#endif

#ifndef VENDOR_MODEL_MSG_RETRY_PERIOD
#define VENDOR_MODEL_MSG_RETRY_PERIOD 400
#endif

#define GENIE_MODEL_SEG_COUNT 8
#define GENIE_MODEL_MTU (GENIE_MODEL_SEG_COUNT * 12)

#define CONFIG_MESH_VENDOR_MODEL_SRV 0x0000

#define VENDOR_MODEL_OPC_NUM 9

extern struct bt_mesh_model_pub genie_model_pub;
extern const struct bt_mesh_model_op genie_model_op[VENDOR_MODEL_OPC_NUM];

#define MESH_MODEL_VENDOR_SRV(_user_data) BT_MESH_MODEL_VND(CONFIG_MESH_VENDOR_COMPANY_ID, CONFIG_MESH_VENDOR_MODEL_SRV, \
                                                            genie_model_op, &genie_model_pub, _user_data)

int genie_model_handle_mesg(genie_transport_model_param_t *p_msg);

#endif
