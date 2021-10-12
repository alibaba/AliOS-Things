/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */
#ifndef __GENIE_AT_H__
#define __GENIE_AT_H__

typedef enum
{
    AT_CMD_NO_ADD = 0x1,
    AT_CMD_ADD = 0x2,
    AT_CMD_QUERY = 0x4,
    AT_CMD_DOU = 0x8,
    AT_CMD_FIND = 0x10,
} AT_CMD_TYPE_e;

typedef enum
{
    AT_OP_EXECUTE = 0x1,
    AT_OP_FCURRENT = 0x2,
    AT_OP_HLOOKUP = 0x3,
    AT_OP_HELP = 0x4,
} AT_OP_CODE_e;

typedef enum
{
    AT_ERR_NOT_FOUND = -1,
    AT_ERR_FORMAT = -2,
    AT_ERR_EXE = -3,
} AT_OP_ERROR_e;

typedef enum
{
    MESH_TEST_RSSI = 0x00,
} MESH_TEST_OP_CODE_e;

typedef enum
{
    MESH_EVENT_PROV_SUCCESS = 0x00,
    MESH_EVENT_PROV_FAILED = 0x05,
    MESH_EVENT_DEV_READY = 0x04,
    MESH_EVENT_UNKNOW = 0xFF,
} MESH_EVENT_VAL_e;

typedef enum
{
    MESH_ADV_CLOSE = 0x00,
    MESH_ADV_START = 0x01,
    MESH_ADV_SILENT = 0x02,
} MESH_ADV_TYPE_e;

typedef int (*at_cmd_function_t)(int argc, char *argv[]);

typedef struct at_cmd_s
{
    const char *cmd_name;
    at_cmd_function_t cb;
    const char *help;
    at_cmd_function_t fcb;
} at_cmd_t;

#define AT_MAX_ARGC 10
#define AT_MAX_ARGV_LEN 32

#define AT_CME_ERROR_STR "+ERROR:%d\r\n"
#define AT_RESPONSE_STR "+%s:"
#define AT_OK_STR "OK\r\n"

// control cmd
#define BLE_MESH_ADV "MESHADV"          // start adv
#define BLE_MESH_REBOOT "REBOOT"        // reboot
#define BLE_MESH_RESET "MESHRST"        // mesh reset
#define BLE_MESH_INFO "MESHINF"         // mesh info
#define BLE_MESH_VER "MESHVER"          // mesh version
#define BLE_MESH_TEST "MESHTEST"        // mesh test
#define BLE_MESH_GROUP "MESHGRP"        // mesh group
#define BLE_MESH_MSG_TX "MESHMSGTX"     // mesh trans msg send
#define BLE_MESH_MAC "MESHMAC"          // mesh MAC
#define BLE_MESH_FACTY_TEST "FACTYTEST" // factory test
#define BLE_MESH_LOG "MESHLOG"          // mesh log

//event
#define BLE_MESH_EVENT "MESHEVT"    // mesh event
#define BLE_MESH_MSG_RX "MESHMSGRX" // mesh trans msg recv

int genie_at_cmd_reboot(int argc, char *argv[]);
int genie_at_cmd_adv_start(int argc, char *argv[]);
int genie_at_cmd_adv_query(int argc, char *argv[]);
int genie_at_cmd_mesh_reset(int argc, char *argv[]);
int genie_at_cmd_mesh_info(int argc, char *argv[]);
int genie_at_cmd_mesh_ver(int argc, char *argv[]);
int genie_at_cmd_mesh_mac(int argc, char *argv[]);
int genie_at_cmd_facty_test(int argc, char *argv[]);
int genie_at_cmd_mesh_test(int argc, char *argv[]);
int genie_at_cmd_mesh_group(int argc, char *argv[]);
int genie_at_cmd_mesh_group_query(int argc, char *argv[]);
int genie_at_cmd_mesh_trans_send(int argc, char *argv[]);
int genie_at_cmd_mesh_log(int argc, char *argv[]);
int genie_at_cmd_mesh_log_query(int argc, char *argv[]);

int genie_at_output_event(genie_event_e event, void *p_arg);
int genie_at_cmd_send_data_to_mcu(uint8_t *p_data, uint16_t data_len);

#endif /*--_GENIE_AT_CMDS_H_--*/
