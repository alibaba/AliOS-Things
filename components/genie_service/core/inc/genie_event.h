/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __GENIE_EVENT_H__
#define __GENIE_EVENT_H__

#include <stdint.h>
#include <stdbool.h>

#ifndef GENIE_PROV_SUCCESS_REPORT_TIMEOUT
#define GENIE_PROV_SUCCESS_REPORT_TIMEOUT 30
#endif

typedef enum
{
    GENIE_EVT_NONE = 0,
    GENIE_EVT_SW_RESET,       /* triggered from cloud */
    GENIE_EVT_HW_RESET_START, /* triggered from user */
    GENIE_EVT_BT_READY,
    GENIE_EVT_MESH_READY, // Used to sync device's state with cloud

    GENIE_EVT_SDK_AIS_DISCON = 10,
    GENIE_EVT_SDK_AIS_CONNECT,
    GENIE_EVT_SDK_MESH_PBADV_START,
    GENIE_EVT_SDK_MESH_PBADV_TIMEOUT,
    GENIE_EVT_SDK_MESH_SILENT_START,

    GENIE_EVT_SDK_MESH_PROV_START = 20,
    GENIE_EVT_SDK_MESH_PROV_DATA,
    GENIE_EVT_SDK_MESH_PROV_TIMEOUT,
    GENIE_EVT_SDK_MESH_PROV_SUCCESS,
    GENIE_EVT_SDK_MESH_PROV_FAIL,

    GENIE_EVT_SDK_APPKEY_ADD = 30,
    GENIE_EVT_SDK_APPKEY_DEL,
    GENIE_EVT_SDK_APPKEY_UPDATE,
    GENIE_EVT_SDK_NETKEY_ADD,
    GENIE_EVT_SDK_NETKEY_DEL,
    GENIE_EVT_SDK_NETKEY_UPDATE,

    GENIE_EVT_SDK_HB_SET = 40,
#ifdef CONFIG_BT_MESH_CTRL_RELAY
    GENIE_EVT_SDK_CTRL_RELAY_SET,
#endif
    GENIE_EVT_SDK_SEQ_UPDATE,

    GENIE_EVT_TIMEOUT = 50,
    GENIE_EVT_DOWN_MSG,
    GENIE_EVT_VENDOR_MODEL_MSG,
    GENIE_EVT_SIG_MODEL_MSG,
    GENIE_EVT_USER_TRANS_CYCLE,
    GENIE_EVT_USER_ACTION_DONE
} genie_event_e;

typedef enum _genie_down_mesg_type
{
    GENIE_DOWN_MESG_VENDOR_TYPE = 0,
    GENIE_DOWN_MESG_SIG_TYPE
} genie_down_mesg_type;

typedef struct _sig_model_msg
{
    uint8_t element_id;
    uint16_t opcode;
    uint16_t len;
    uint8_t *data;
} sig_model_msg;

typedef struct _genie_down_msg_s
{
    uint8_t element_id;
    uint16_t len;
    uint8_t *data;
} genie_down_msg_t;

/**
 * @brief The handler for the underlying events. If necessary
 *        this handler dispatch the user events to applications.
 * @param[in] event refers to the event details.
 * @param[in] args refers to the additional information for the event.
 */
void genie_event(genie_event_e event, void *args);

int genie_down_msg(genie_down_mesg_type msg_type, uint32_t opcode, void *p_msg);

#endif
