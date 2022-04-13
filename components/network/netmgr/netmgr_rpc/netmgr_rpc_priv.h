/*
 * Copyright (C) 2020 Alibaba Group Holding Limited
 */

#ifndef _NETMGR_RPC_PRIV_H_
#define _NETMGR_RPC_PRIV_H_

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>
#include <aos/kernel.h>
#include <netmgr.h>

#define NETMGR_QUEUE_QUERY_NAME  "/aos_netmgr_state_query"
#define NETMGR_QUEUE_RESP_NAME   "/aos_netmgr_state_resp"

#define NETMGR_QUEUE_MAX_MSG_LEN  512
#define NETMGR_QUEUE_BUF_SIZE     NETMGR_QUEUE_MAX_MSG_LEN * 2
#define NETMGR_IPC_TASK_STACK     4096

typedef struct {
    aos_hdl_t query_queue;
    aos_hdl_t resp_queue;
    void *recv_buff;
    size_t buff_len;
    uint8_t running;
} netmgr_rpc_t;

typedef enum netmgr_msg_id {
    OPS_MIN = 0,
    /* Application -> C */
    OPS_SRV_INIT,        /* 1 */
    OPS_SRV_DEINIT,      /* 2 */
    OPS_GET_DEV,         /* 3 */
    OPS_SET_IFCONFIG,    /* 4 */
    OPS_GET_IFCONFIG,    /* 5 */
    OPS_SET_AUTO_RECON,  /* 6 */
    OPS_GET_CONFIG,      /* 7 */
    OPS_DEL_CONFIG,      /* 8 */
    OPS_GET_STATE,       /* 9 */
    OPS_CONNECT,         /* 10 */
    OPS_DISCONNECT,      /* 11 */
    OPS_SAVE_CONFIG,     /* 12 */
    OPS_SET_CONN_PARAMS, /* 13 */
    OPS_SET_MSG_CB,      /* 14 */
    OPS_DEL_MSG_CB,      /* 15 */
    FOPS_MAX
} netmgr_msg_id_e;

typedef union netmgr_api_arg {

    struct {
        char path[0];
    } get_dev;

    struct {
        netmgr_hdl_t hdl;
        char buf[0];
        //netmgr_ifconfig_info_t *info;
    } ifconfig;

    struct {
        netmgr_hdl_t hdl;
        bool enable;
    } set_auto_reconn;

    struct {
        netmgr_hdl_t hdl;
        //netmgr_config_t *config;
    } get_config;

    struct {
        netmgr_hdl_t hdl;
        char buf[0];
        //netmgr_del_config_t *config;
    } del_config;

    struct {
        netmgr_hdl_t hdl;
        char buf[0];
        //netmgr_connect_params_t *params;
    } connect;
    struct {
        netmgr_hdl_t hdl;
    } disconnect;
    struct {
        netmgr_hdl_t hdl;
    } save_config;

    struct {
        netmgr_hdl_t hdl;
        char buf[0];
        //netmgr_connect_params_t *params;
    } connect_params;

    struct {
        netmgr_hdl_t hdl;
    } msg_cb;

    struct {
        void *farg;
    } priv;
} netmgr_api_arg_u;

typedef union netmgr_api_result {

    struct {
        int status;
    } comm;

    struct {
        int status;
        ssize_t len;
        char data[0];
    } with_out_params;
} netmgr_api_result_u;

typedef struct netmgr_rpc_tlv {
    unsigned short   t;
    unsigned short   l;
    netmgr_api_arg_u v[0];
} netmgr_rpc_tlv_t;

#ifdef __cplusplus
}
#endif

#endif
