/*
 * Copyright (C) 2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include "aos/kernel.h"
#include "parcel.h"
#include "netmgr_rpc_priv.h"

static netmgr_rpc_t g_netmgr_rpc = {0};

static int rpc_client_init(netmgr_rpc_t *hdl, const char *query_name, const char *resp_name, size_t buf_len)
{
    int ret = -1;

    if (hdl == NULL) {
        return -EINVAL;
    }
    memset(hdl, 0, sizeof(netmgr_rpc_t));

    ret = aos_queue_open(&hdl->query_queue, query_name, 0, 0, 0);
    if (ret != 0) {
        return -1;
    }

    ret = aos_queue_open(&hdl->resp_queue, resp_name, 0, 0, 0);
    if (ret != 0) {
        aos_queue_close(&hdl->query_queue);
        return -1;
    }

    hdl->recv_buff = aos_malloc(buf_len);
    if (hdl->recv_buff == NULL) {
        aos_queue_close(&hdl->query_queue);
        aos_queue_close(&hdl->resp_queue);
        return -ENOSPC;
    }
    hdl->buff_len = buf_len;

    return 0;
}

static int rpc_client_deinit(netmgr_rpc_t *hdl)
{

    if (hdl == NULL) {
        return -EINVAL;
    }

    aos_queue_close(&hdl->query_queue);
    aos_queue_close(&hdl->resp_queue);
    aos_free(hdl->recv_buff);
    hdl->recv_buff = NULL;
    hdl->buff_len = 0;
    return 0;
}

static int rpc_call_sync(netmgr_rpc_t *rpc, struct aos_parcel_t *reply, int nparams, ...)
{
    int ret;
    struct aos_parcel_t *rdata = NULL;
    size_t pblen = 0;
    va_list var_args;
    size_t arg_size;
    void *pbuf, *arg;
    int i;

    if (rpc == NULL || reply == NULL || nparams <= 0) {
        return -EINVAL;
    }

    /* compute length of proto buffer */
    i = 0;
    va_start(var_args, nparams);
    while (i++ < nparams) {
        arg_size = va_arg(var_args, size_t);
        pblen += arg_size + 4;

        arg = va_arg(var_args, void *);
    }
    va_end(var_args);

    /* alloc rpc_data_t and rpc_buffer */
    rdata = (struct aos_parcel_t *)aos_malloc(sizeof(*rdata));
    if (rdata == NULL) {
        return -ENOSPC;
    }

    pbuf = (char *)aos_malloc(pblen);
    if (pbuf == NULL) {
        aos_free(rdata);
        return -ENOSPC;
    }
    memset(pbuf, 0, pblen);
    aos_parcel_init(rdata, pbuf, pblen);

    /* serialize parameter list */
    i = 0;
    va_start(var_args, nparams);
    while (i++ < nparams) {
        arg_size = va_arg(var_args, size_t);
        arg = va_arg(var_args, void *);

        aos_parcel_put(AP_BUF, arg, arg_size, rdata);
    }
    va_end(var_args);

    ret = aos_queue_send(&rpc->query_queue, rdata->state, rdata->wbytes);

    aos_parcel_params_free(rdata);

    if (ret == 0) {
        pblen = rpc->buff_len;
        ret = aos_queue_recv(&rpc->resp_queue, AOS_WAIT_FOREVER, rpc->recv_buff, &pblen);
        if (ret == 0) {
            aos_parcel_init(reply, rpc->recv_buff, pblen);
        }
    }
    return ret;
}

int netmgr_service_init()
{
    int ret = -1;
    if (g_netmgr_rpc.recv_buff == NULL) {
        ret = rpc_client_init(&g_netmgr_rpc, NETMGR_QUEUE_QUERY_NAME, NETMGR_QUEUE_RESP_NAME, NETMGR_QUEUE_MAX_MSG_LEN);
    }

    return ret;
}

void netmgr_service_deinit()
{
    if (g_netmgr_rpc.recv_buff != NULL) {
        rpc_client_deinit(&g_netmgr_rpc);
    }
}

netmgr_hdl_t netmgr_get_dev(const char *name)
{
    int ret;
    size_t req_len;
    size_t resp_len;
    netmgr_rpc_tlv_t *req_msg;
    netmgr_api_arg_u *arg;
    netmgr_api_result_u *resp_msg;

    struct aos_parcel_t reply;

    if (name == NULL) {
        return -EINVAL;
    }

    req_len = sizeof(netmgr_rpc_tlv_t) + sizeof(netmgr_api_arg_u) + strlen(name) + 1;
    req_msg = aos_malloc(req_len);
    if (!req_msg) {
        return -ENOSPC;
    }

    req_msg->t = OPS_GET_DEV;
    req_msg->l = req_len - sizeof(netmgr_rpc_tlv_t);
    arg = (netmgr_api_arg_u *)req_msg->v;
    strncpy(arg->get_dev.path, name, req_msg->l - 1);

    ret = rpc_call_sync(&g_netmgr_rpc, &reply, 1, req_len, req_msg);

    aos_free(req_msg);
    if (ret < 0) {
        return ret;
    }

    /* parse response */
    resp_len = reply.max_size;

    resp_msg = (netmgr_api_result_u *)aos_malloc(resp_len);
    if (!resp_msg) {
        return -ENOSPC;
    }

    ret = aos_parcel_get(AP_BUF, resp_msg, &resp_len, &reply);
    if (ret < 0) {
        aos_free(resp_msg);
        return ret;
    }
    ret = resp_msg->comm.status;

    aos_free(resp_msg);

    return ret;
}

/**
 * @brief  set if config information
 * @param  [in] hdl
 * @param  [in] if config information
 * @return 0 on success, others on error
 */
int netmgr_set_ifconfig(netmgr_hdl_t hdl, netmgr_ifconfig_info_t *info)
{
    int ret;
    size_t req_len;
    size_t resp_len;
    netmgr_rpc_tlv_t *req_msg;
    netmgr_api_arg_u *arg;
    netmgr_api_result_u *resp_msg;
    struct aos_parcel_t reply;

    if (info == NULL) {
        return -EINVAL;
    }

    req_len = sizeof(netmgr_rpc_tlv_t) + sizeof(netmgr_api_arg_u) + sizeof(netmgr_ifconfig_info_t);
    req_msg = aos_malloc(req_len);
    if (!req_msg) {
        return -ENOSPC;
    }

    req_msg->t = OPS_SET_IFCONFIG;
    req_msg->l = req_len - sizeof(netmgr_rpc_tlv_t);
    arg = (netmgr_api_arg_u *)req_msg->v;
    arg->ifconfig.hdl = hdl;
    memcpy(arg->ifconfig.buf, info, sizeof(netmgr_ifconfig_info_t));

    ret = rpc_call_sync(&g_netmgr_rpc, &reply, 1, req_len, req_msg);

    aos_free(req_msg);
    if (ret < 0) {
        return ret;
    }

    /* parse response */
    resp_len = reply.max_size;

    resp_msg = (netmgr_api_result_u *)aos_malloc(resp_len);
    if (!resp_msg) {
        return -ENOSPC;
    }

    ret = aos_parcel_get(AP_BUF, resp_msg, &resp_len, &reply);
    if (ret < 0) {
        aos_free(resp_msg);
        return ret;
    }
    ret = resp_msg->comm.status;

    aos_free(resp_msg);

    return ret;
}

/**
 * @brief  get if config information
 * @param  [in] hdl
 * @param  [out] if config information
 * @return 0 on success, others on error
 */
int netmgr_get_ifconfig(netmgr_hdl_t hdl, netmgr_ifconfig_info_t *info)
{
    int ret;
    size_t req_len;
    size_t resp_len;
    netmgr_rpc_tlv_t *req_msg;
    netmgr_api_arg_u *arg;
    netmgr_api_result_u *resp_msg;
    struct aos_parcel_t reply;

    if (info == NULL) {
        return -EINVAL;
    }

    req_len = sizeof(netmgr_rpc_tlv_t) + sizeof(netmgr_api_arg_u);
    req_msg = aos_malloc(req_len);
    if (!req_msg) {
        return -ENOSPC;
    }

    req_msg->t = OPS_GET_IFCONFIG;
    req_msg->l = req_len - sizeof(netmgr_rpc_tlv_t);
    arg = (netmgr_api_arg_u *)req_msg->v;
    arg->ifconfig.hdl = hdl;

    ret = rpc_call_sync(&g_netmgr_rpc, &reply, 1, req_len, req_msg);

    aos_free(req_msg);
    if (ret < 0) {
        return ret;
    }

    /* parse response */
    resp_len = reply.max_size;

    resp_msg = (netmgr_api_result_u *)aos_malloc(resp_len);
    if (!resp_msg) {
        return -ENOSPC;
    }

    ret = aos_parcel_get(AP_BUF, resp_msg, &resp_len, &reply);
    if (ret < 0) {
        aos_free(resp_msg);
        return ret;
    }
    ret = resp_msg->comm.status;
    if (ret == 0) {
        memcpy(info, resp_msg->with_out_params.data, sizeof(netmgr_ifconfig_info_t));
    }

    aos_free(resp_msg);
    return ret;
}

/**
 * @brief  set auto reconnect flag
 * @param  [in] hdl
 * @param  [in] enable
 */
void netmgr_set_auto_reconnect(netmgr_hdl_t hdl, bool enable)
{
    size_t req_len;
    netmgr_rpc_tlv_t *req_msg;
    netmgr_api_arg_u *arg;
    struct aos_parcel_t reply;

    req_len = sizeof(netmgr_rpc_tlv_t) + sizeof(netmgr_api_arg_u);
    req_msg = aos_malloc(req_len);
    if (!req_msg) {
        return;
    }

    req_msg->t = OPS_SET_AUTO_RECON;
    req_msg->l = req_len - sizeof(netmgr_rpc_tlv_t);
    arg = (netmgr_api_arg_u *)req_msg->v;

    arg->set_auto_reconn.enable = enable;
    arg->set_auto_reconn.hdl = hdl;

    rpc_call_sync(&g_netmgr_rpc, &reply, 1, req_len, req_msg);

    aos_free(req_msg);
    return;
}

/**
 * @brief  get config information
 * @param  [in] hdl
 * @param  [out] config information
 * @return 0 on success, others on error
 */
int netmgr_get_config(netmgr_hdl_t hdl, netmgr_config_t *config)
{
    int ret;
    size_t req_len;
    size_t resp_len;
    netmgr_rpc_tlv_t *req_msg;
    netmgr_api_arg_u *arg;
    netmgr_api_result_u *resp_msg;
    struct aos_parcel_t reply;

    if (config == NULL) {
        return -EINVAL;
    }

    req_len = sizeof(netmgr_rpc_tlv_t) + sizeof(netmgr_api_arg_u);
    req_msg = aos_malloc(req_len);
    if (!req_msg) {
        return -ENOSPC;
    }

    req_msg->t = OPS_GET_CONFIG;
    req_msg->l = req_len - sizeof(netmgr_rpc_tlv_t);
    arg = (netmgr_api_arg_u *)req_msg->v;
    arg->get_config.hdl = hdl;

    ret = rpc_call_sync(&g_netmgr_rpc, &reply, 1, req_len, req_msg);

    aos_free(req_msg);
    if (ret < 0) {
        return ret;
    }

    /* parse response */
    resp_len = reply.max_size;

    resp_msg = (netmgr_api_result_u *)aos_malloc(resp_len);
    if (!resp_msg) {
        return -ENOSPC;
    }

    ret = aos_parcel_get(AP_BUF, resp_msg, &resp_len, &reply);
    if (ret < 0) {
        aos_free(resp_msg);
        return ret;
    }
    ret = resp_msg->comm.status;
    if (ret == 0) {
        memcpy(config, resp_msg->with_out_params.data, sizeof(netmgr_config_t));
    }

    aos_free(resp_msg);
    return ret;
}

/**
 * @brief  delete config information
 * @param  [in] hdl
 * @param  [in] config
 * @return 0 on success, others on error
 */
int netmgr_del_config(netmgr_hdl_t hdl, netmgr_del_config_t *config)
{
    int ret;
    size_t req_len;
    size_t resp_len;
    netmgr_rpc_tlv_t *req_msg;
    netmgr_api_arg_u *arg;
    netmgr_api_result_u *resp_msg;
    struct aos_parcel_t reply;

    if (config == NULL) {
        return -EINVAL;
    }

    req_len = sizeof(netmgr_rpc_tlv_t) + sizeof(netmgr_api_arg_u) + sizeof(netmgr_del_config_t);
    req_msg = aos_malloc(req_len);
    if (!req_msg) {
        return -ENOSPC;
    }

    req_msg->t = OPS_DEL_CONFIG;
    req_msg->l = req_len - sizeof(netmgr_rpc_tlv_t);
    arg = (netmgr_api_arg_u *)req_msg->v;
    arg->ifconfig.hdl = hdl;
    memcpy(arg->ifconfig.buf, config, sizeof(netmgr_del_config_t));

    ret = rpc_call_sync(&g_netmgr_rpc, &reply, 1, req_len, req_msg);

    aos_free(req_msg);
    if (ret < 0) {
        return ret;
    }

    /* parse response */
    resp_len = reply.max_size;

    resp_msg = (netmgr_api_result_u *)aos_malloc(resp_len);
    if (!resp_msg) {
        return -ENOSPC;
    }

    ret = aos_parcel_get(AP_BUF, resp_msg, &resp_len, &reply);
    if (ret < 0) {
        aos_free(resp_msg);
        return ret;
    }
    ret = resp_msg->comm.status;

    aos_free(resp_msg);
    return ret;
}

/**
 * @brief  get state of hdl
 * @param  [in] hdl
 * @return >=0 on success, others on error
 */
int netmgr_get_state(netmgr_hdl_t hdl)
{
    int ret;
    size_t req_len;
    size_t resp_len;
    netmgr_rpc_tlv_t *req_msg;
    netmgr_api_arg_u *arg;
    netmgr_api_result_u *resp_msg;
    struct aos_parcel_t reply;

    req_len = sizeof(netmgr_rpc_tlv_t) + sizeof(netmgr_api_arg_u);
    req_msg = aos_malloc(req_len);
    if (!req_msg) {
        return -ENOSPC;
    }

    req_msg->t = OPS_GET_STATE;
    req_msg->l = req_len - sizeof(netmgr_rpc_tlv_t);
    arg = (netmgr_api_arg_u *)req_msg->v;
    arg->ifconfig.hdl = hdl;
    // memcpy(arg->ifconfig.buf, info, sizeof(netmgr_ifconfig_info_t));

    ret = rpc_call_sync(&g_netmgr_rpc, &reply, 1, req_len, req_msg);

    aos_free(req_msg);
    if (ret < 0) {
        return ret;
    }

    /* parse response */
    resp_len = reply.max_size;

    resp_msg = (netmgr_api_result_u *)aos_malloc(resp_len);
    if (!resp_msg) {
        return -ENOSPC;
    }

    ret = aos_parcel_get(AP_BUF, resp_msg, &resp_len, &reply);
    if (ret < 0) {
        aos_free(resp_msg);
        return ret;
    }
    ret = resp_msg->comm.status;
    aos_free(resp_msg);

    return ret;
}

/**
 * @brief  netmgr connect network
 * @param  [in] hdl
 * @param  [in] params
 * @return 0 on success, others on error
 */
int netmgr_connect(netmgr_hdl_t hdl, netmgr_connect_params_t *params)
{
    int ret;
    size_t req_len;
    size_t resp_len;
    netmgr_rpc_tlv_t *req_msg;
    netmgr_api_arg_u *arg;
    netmgr_api_result_u *resp_msg;
    struct aos_parcel_t reply;

    if (params == NULL) {
        return -EINVAL;
    }

    req_len = sizeof(netmgr_rpc_tlv_t) + sizeof(netmgr_api_arg_u) + sizeof(netmgr_connect_params_t);
    req_msg = aos_malloc(req_len);
    if (!req_msg) {
        return -ENOSPC;
    }

    req_msg->t = OPS_CONNECT;
    req_msg->l = req_len - sizeof(netmgr_rpc_tlv_t);
    arg = (netmgr_api_arg_u *)req_msg->v;
    arg->ifconfig.hdl = hdl;
    memcpy(arg->ifconfig.buf, params, sizeof(netmgr_connect_params_t));

    ret = rpc_call_sync(&g_netmgr_rpc, &reply, 1, req_len, req_msg);

    aos_free(req_msg);
    if (ret < 0) {
        return ret;
    }

    /* parse response */
    resp_len = reply.max_size;

    resp_msg = (netmgr_api_result_u *)aos_malloc(resp_len);
    if (!resp_msg) {
        return -ENOSPC;
    }

    ret = aos_parcel_get(AP_BUF, resp_msg, &resp_len, &reply);
    if (ret < 0) {
        aos_free(resp_msg);
        return ret;
    }
    ret = resp_msg->comm.status;

    aos_free(resp_msg);

    return ret;
}

/**
 * @brief  netmgr disconnect network
 * @param  [in] hdl
 * @return 0 on success, others on error
 */
int netmgr_disconnect(netmgr_hdl_t hdl)
{
    int ret;
    size_t req_len;
    size_t resp_len;
    netmgr_rpc_tlv_t *req_msg;
    netmgr_api_arg_u *arg;
    netmgr_api_result_u *resp_msg;
    struct aos_parcel_t reply;

    req_len = sizeof(netmgr_rpc_tlv_t) + sizeof(netmgr_api_arg_u);
    req_msg = aos_malloc(req_len);
    if (!req_msg) {
        return -ENOSPC;
    }

    req_msg->t = OPS_DISCONNECT;
    req_msg->l = req_len - sizeof(netmgr_rpc_tlv_t);
    arg = (netmgr_api_arg_u *)req_msg->v;
    arg->ifconfig.hdl = hdl;

    ret = rpc_call_sync(&g_netmgr_rpc, &reply, 1, req_len, req_msg);

    aos_free(req_msg);
    if (ret < 0) {
        return ret;
    }

    /* parse response */
    resp_len = reply.max_size;

    resp_msg = (netmgr_api_result_u *)aos_malloc(resp_len);
    if (!resp_msg) {
        return -ENOSPC;
    }

    ret = aos_parcel_get(AP_BUF, resp_msg, &resp_len, &reply);
    if (ret < 0) {
        aos_free(resp_msg);
        return ret;
    }
    ret = resp_msg->comm.status;
    aos_free(resp_msg);

    return ret;
}

/**
 * @brief  netmgr save config
 * @param  [in] hdl
 * @return 0 on success, others on error
 */
int netmgr_save_config(netmgr_hdl_t hdl)
{
    int ret;
    size_t req_len;
    size_t resp_len;
    netmgr_rpc_tlv_t *req_msg;
    netmgr_api_arg_u *arg;
    netmgr_api_result_u *resp_msg;
    struct aos_parcel_t reply;

    req_len = sizeof(netmgr_rpc_tlv_t) + sizeof(netmgr_api_arg_u);
    req_msg = aos_malloc(req_len);
    if (!req_msg) {
        return -ENOSPC;
    }

    req_msg->t = OPS_SAVE_CONFIG;
    req_msg->l = req_len - sizeof(netmgr_rpc_tlv_t);
    arg = (netmgr_api_arg_u *)req_msg->v;
    arg->ifconfig.hdl = hdl;

    ret = rpc_call_sync(&g_netmgr_rpc, &reply, 1, req_len, req_msg);

    aos_free(req_msg);
    if (ret < 0) {
        return ret;
    }

    /* parse response */
    resp_len = reply.max_size;

    resp_msg = (netmgr_api_result_u *)aos_malloc(resp_len);
    if (!resp_msg) {
        return -ENOSPC;
    }

    ret = aos_parcel_get(AP_BUF, resp_msg, &resp_len, &reply);
    if (ret < 0) {
        aos_free(resp_msg);
        return ret;
    }
    ret = resp_msg->comm.status;
    aos_free(resp_msg);
    return ret;
}

/**
 * @brief  netmgr set connect params
 * @param  [in] hdl
 * @param  [in] ap_info
 * @param  [in] num
 * @param  [in] type
 * @return 0 on success, others on error
 */
int netmgr_set_connect_params(netmgr_hdl_t hdl, netmgr_connect_params_t *params)
{
    int ret;
    size_t req_len;
    size_t resp_len;
    netmgr_rpc_tlv_t *req_msg;
    netmgr_api_arg_u *arg;
    netmgr_api_result_u *resp_msg;
    struct aos_parcel_t reply;

    if (params == NULL) {
        return -EINVAL;
    }

    req_len = sizeof(netmgr_rpc_tlv_t) + sizeof(netmgr_api_arg_u) + sizeof(netmgr_connect_params_t);
    req_msg = aos_malloc(req_len);
    if (!req_msg) {
        return -ENOSPC;
    }

    req_msg->t = OPS_SET_CONN_PARAMS;
    req_msg->l = req_len - sizeof(netmgr_rpc_tlv_t);
    arg = (netmgr_api_arg_u *)req_msg->v;
    arg->ifconfig.hdl = hdl;
    memcpy(arg->ifconfig.buf, params, sizeof(netmgr_connect_params_t));

    ret = rpc_call_sync(&g_netmgr_rpc, &reply, 1, req_len, req_msg);

    aos_free(req_msg);
    if (ret < 0) {
        return ret;
    }

    /* parse response */
    resp_len = reply.max_size;

    resp_msg = (netmgr_api_result_u *)aos_malloc(resp_len);
    if (!resp_msg) {
        return -ENOSPC;
    }

    ret = aos_parcel_get(AP_BUF, resp_msg, &resp_len, &reply);
    if (ret < 0) {
        aos_free(resp_msg);
        return ret;
    }
    ret = resp_msg->comm.status;

    aos_free(resp_msg);
    return ret;
}

/* async api not implement yet */
int netmgr_set_msg_cb(netmgr_hdl_t hdl, netmgr_msg_cb_t cb)
{
    int ret = -1;
    return ret;
}

int netmgr_del_msg_cb(netmgr_hdl_t hdl, netmgr_msg_cb_t cb)
{
    int ret = -1;
    return ret;
}