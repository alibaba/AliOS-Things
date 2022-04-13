/*
 * Copyright (C) 2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdbool.h>
#include "aos/kernel.h"
#include "parcel.h"
#include "netmgr.h"
#include "netmgr_rpc_priv.h"

static netmgr_rpc_t g_netmgr_rpc = {0};

static int netmgr_rpc_srv_init(netmgr_rpc_t *hdl, const char *query_name, const char *resp_name)
{
    int ret = -1;

    if (hdl == NULL) {
        return -EINVAL;
    }
    memset(hdl, 0, sizeof(netmgr_rpc_t));

    ret = aos_queue_open(&hdl->query_queue, query_name, O_CREAT, NETMGR_QUEUE_BUF_SIZE, NETMGR_QUEUE_MAX_MSG_LEN);
    if (ret != 0) {
        return ret;
    }

    ret = aos_queue_open(&hdl->resp_queue, resp_name, O_CREAT, NETMGR_QUEUE_BUF_SIZE, NETMGR_QUEUE_MAX_MSG_LEN);
    if (ret != 0) {
        aos_queue_close(&hdl->query_queue);
        aos_queue_unlink(NETMGR_QUEUE_QUERY_NAME);
        return ret;
    }

    hdl->recv_buff = aos_malloc(NETMGR_QUEUE_MAX_MSG_LEN);
    if (hdl->recv_buff == NULL) {
        aos_queue_close(&hdl->query_queue);
        aos_queue_close(&hdl->resp_queue);
        aos_queue_unlink(NETMGR_QUEUE_QUERY_NAME);
        aos_queue_unlink(NETMGR_QUEUE_RESP_NAME);
        return -ENOSPC;
    }
    hdl->buff_len = NETMGR_QUEUE_MAX_MSG_LEN;

    return 0;
}

static int netmgr_rpc_srv_deinit(netmgr_rpc_t *hdl)
{

    if (hdl == NULL) {
        return -EINVAL;
    }

    aos_queue_close(&hdl->query_queue);
    aos_queue_close(&hdl->resp_queue);
    aos_queue_unlink(NETMGR_QUEUE_QUERY_NAME);
    aos_queue_unlink(NETMGR_QUEUE_RESP_NAME);
    aos_free(hdl->recv_buff);
    hdl->recv_buff = NULL;
    hdl->buff_len = 0;
    return 0;
}

int netmgr_rpc_resp(netmgr_rpc_t *hdl, int nparams, ...)
{
    struct aos_parcel_t *rsp_data = NULL;
    size_t pblen = 0;
    va_list var_args;
    size_t arg_size;
    void *pbuf, *arg;
    int res;
    int i;

    if (hdl == 0 || nparams <= 0) {
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
    rsp_data = (struct aos_parcel_t *)aos_malloc(sizeof(*rsp_data));
    if (rsp_data == NULL) {
        return -ENOSPC;
    }

    pbuf = (char *)aos_malloc(pblen);
    if (pbuf == NULL) {
        aos_free(rsp_data);
        return -ENOSPC;
    }

    aos_parcel_init(rsp_data, pbuf, pblen);

    /* serialize parameter list */
    i = 0;
    va_start(var_args, nparams);
    while (i++ < nparams) {
        arg_size = va_arg(var_args, size_t);
        arg = va_arg(var_args, void *);

        aos_parcel_put(AP_BUF, arg, arg_size, rsp_data);
    }
    va_end(var_args);

    res = aos_queue_send(&hdl->resp_queue, rsp_data->state, rsp_data->wbytes);
    aos_parcel_params_free(rsp_data);

    return res;
}

static int netmgr_rpc_srv_deal_msg(netmgr_rpc_t *rpc, netmgr_rpc_tlv_t *tlv, size_t size)
{
    int ret;
    int len;
    netmgr_api_arg_u *arg;
    netmgr_api_result_u result;

    memset(&result, 0, sizeof(netmgr_api_result_u));
    len = tlv->l;
    arg = (netmgr_api_arg_u *)tlv->v;

    if (len + sizeof(netmgr_rpc_tlv_t) > size) {
        printf("tlv len overflow : tlv len:%d, buf len :%d, %s:%d\r\n", len, size - sizeof(netmgr_rpc_tlv_t), __func__,
               __LINE__);
        return -ENOMEM;
    }
    switch (tlv->t) {
        case OPS_SRV_INIT: {

        }
        break;
        case OPS_SRV_DEINIT:
            break;
        case OPS_GET_DEV: {
            if (len <= 0) {
                result.comm.status = -EINVAL;
                printf("err:paras too short:%d, %s:%d\r\n", len, __func__, __LINE__);
                break;
            }
            const char *name = arg->get_dev.path;
            int hdl = netmgr_get_dev(name);
            result.comm.status = hdl;
        }
        break;
        case OPS_SET_IFCONFIG: {
            int hdl;
            netmgr_ifconfig_info_t *info;
            if (len <= sizeof(netmgr_ifconfig_info_t) + sizeof(netmgr_hdl_t)) {
                printf("err:paras too short:%d, %s:%d\r\n", len, __func__, __LINE__);
                result.comm.status = -EINVAL;
                break;
            }
            hdl = arg->ifconfig.hdl;
            info = (netmgr_ifconfig_info_t *)arg->ifconfig.buf;
            ret = netmgr_set_ifconfig(hdl, info);
            result.comm.status = ret;
        }
        break;
        case OPS_GET_IFCONFIG: {
            int hdl;
            netmgr_api_result_u *resp_buf;
            int resp_buf_len;
            netmgr_ifconfig_info_t info = {0};


            if (len <= sizeof(netmgr_hdl_t)) {
                printf("err:paras too short:%d, %s:%d\r\n", len, __func__, __LINE__);
                result.comm.status = -EINVAL;
                break;
            }
            resp_buf_len = sizeof(netmgr_api_result_u) + sizeof(netmgr_ifconfig_info_t);

            hdl = arg->ifconfig.hdl;
            ret = netmgr_get_ifconfig(hdl, &info);
            if (ret < 0) {
                result.comm.status = ret;
                break;
            }
            resp_buf = aos_malloc(resp_buf_len);
            if (resp_buf == NULL) {
                result.comm.status = -ENOSPC;
                break;
            }
            resp_buf->with_out_params.status = ret;
            resp_buf->with_out_params.len = sizeof(info);
            memcpy(resp_buf->with_out_params.data, &info, sizeof(info));
            ret = netmgr_rpc_resp(rpc, 1, resp_buf_len, resp_buf);
            aos_free(resp_buf);
            return ret;
        }
        break;
        case OPS_SET_AUTO_RECON: {
            int hdl;
            bool enable;
            if (len <= sizeof(bool) + sizeof(netmgr_hdl_t)) {
                printf("err:paras too short:%d, %s:%d\r\n", len, __func__, __LINE__);
                result.comm.status = -EINVAL;
                break;
            }
            hdl = arg->set_auto_reconn.hdl;
            enable = arg->set_auto_reconn.enable;
            netmgr_set_auto_reconnect(hdl, enable);
            result.comm.status = 0;
        }
        break;
        case OPS_GET_CONFIG: {
            int hdl;
            netmgr_api_result_u *resp_buf;
            int resp_buf_len;
            netmgr_config_t info = {0};


            if (len <= sizeof(netmgr_hdl_t)) {
                printf("err:paras too short:%d, %s:%d\r\n", len, __func__, __LINE__);
                result.comm.status = -EINVAL;
                break;
            }
            resp_buf_len = sizeof(netmgr_api_result_u) + sizeof(netmgr_config_t);

            hdl = arg->ifconfig.hdl;
            ret = netmgr_get_config(hdl, &info);
            if (ret < 0) {
                result.comm.status = ret;
                break;
            }
            resp_buf = aos_malloc(resp_buf_len);
            if (resp_buf == NULL) {
                result.comm.status = -ENOSPC;
                break;
            }
            resp_buf->with_out_params.status = ret;
            resp_buf->with_out_params.len = sizeof(info);
            memcpy(resp_buf->with_out_params.data, &info, sizeof(info));
            ret = netmgr_rpc_resp(rpc, 1, resp_buf_len, resp_buf);
            aos_free(resp_buf);
            printf("netmgr_rpc_resp ret = %d\r\n ", ret);
            return ret;
        }
        break;
        case OPS_DEL_CONFIG: {
            int hdl;
            netmgr_del_config_t *info;
            if (len <= sizeof(netmgr_del_config_t) + sizeof(netmgr_hdl_t)) {
                printf("err:paras too short:%d, %s:%d\r\n", len, __func__, __LINE__);
                result.comm.status = -EINVAL;
                break;
            }
            hdl = arg->del_config.hdl;
            info = (netmgr_del_config_t *)arg->del_config.buf;
            ret = netmgr_del_config(hdl, info);
            result.comm.status = ret;
        }
        break;
        case OPS_GET_STATE: {
            int hdl;
            if (len <= sizeof(netmgr_hdl_t)) {
                printf("err:paras too short:%d, %s:%d\r\n", len, __func__, __LINE__);
                result.comm.status = -EINVAL;
                break;
            }
            hdl = arg->get_config.hdl;
            ret = netmgr_get_state(hdl);
            result.comm.status = ret;
        }
        break;
        case OPS_CONNECT: {
            int hdl;
            netmgr_connect_params_t *info;
            if (len <= sizeof(netmgr_connect_params_t) + sizeof(netmgr_hdl_t)) {
                printf("err:paras too short:%d, %s:%d\r\n", len, __func__, __LINE__);
                result.comm.status = -EINVAL;
                break;
            }
            hdl = arg->connect.hdl;
            info = (netmgr_connect_params_t *)arg->connect.buf;
            ret = netmgr_connect(hdl, info);

            result.comm.status = ret;
        }

        break;
        case OPS_DISCONNECT: {
            int hdl;
            if (len <= sizeof(netmgr_hdl_t)) {
                printf("err:paras too short:%d, %s:%d\r\n", len, __func__, __LINE__);
                result.comm.status = -EINVAL;
                break;
            }
            hdl = arg->get_config.hdl;
            ret = netmgr_disconnect(hdl);
            result.comm.status = ret;
        }
        break;
        case OPS_SAVE_CONFIG: {
            int hdl;
            if (len <= sizeof(netmgr_hdl_t)) {
                printf("err:paras too short:%d, %s:%d\r\n", len, __func__, __LINE__);
                result.comm.status = -EINVAL;
                break;
            }
            hdl = arg->get_config.hdl;
            ret = netmgr_save_config(hdl);
            result.comm.status = ret;
        }
        break;
        case OPS_SET_CONN_PARAMS: {
            int hdl;
            netmgr_connect_params_t *info;
            if (len <= sizeof(netmgr_connect_params_t) + sizeof(netmgr_hdl_t)) {
                printf("err:paras too short:%d, %s:%d\r\n", len, __func__, __LINE__);
                result.comm.status = -EINVAL;
                break;
            }
            hdl = arg->connect_params.hdl;
            info = (netmgr_connect_params_t *)arg->connect_params.buf;
            ret = netmgr_set_connect_params(hdl, info);
            result.comm.status = ret;
        }
        break;
        case OPS_SET_MSG_CB:
            break;
        case OPS_DEL_MSG_CB:
            break;
        default:
            printf("unknow tlv type:%d \r\n", tlv->t);
            break;
    }
    ret = netmgr_rpc_resp(rpc, 1, sizeof(result), &result);
    return ret;
}
static int netmgr_rpc_srv_deal_pkg(netmgr_rpc_t *rpc, aos_parcel_t *pkg)
{
    size_t len;
    void *buf;
    int ret;

    if (pkg->max_size == 0) {
        return -EINVAL;
    }
    len = pkg->max_size;
    buf = aos_malloc(len);
    if (!buf) {
        printf("malloc buf for rpc msg failed\r\n");
        return -ENOMEM;
    }
    ret = aos_parcel_get(AP_BUF, buf, &len, pkg);
    if (ret < 0 || len <= 0) {
        printf("rpc receive data failed, ret:%d\r\n", ret);
        aos_free(buf);
        return ret;
    }

    ret = netmgr_rpc_srv_deal_msg(rpc, (netmgr_rpc_tlv_t *)buf, len);
    aos_free(buf);
    return ret;
}

static void netmgr_srv_running_func(void *data)
{
    size_t get_len = 0;
    netmgr_rpc_t *rpc = (netmgr_rpc_t *)data;
    struct aos_parcel_t pkt;
    if (data == NULL) {
        return;
    }

    while (rpc->running) {
        int ret = aos_queue_recv(&rpc->query_queue, AOS_WAIT_FOREVER, rpc->recv_buff, &get_len);
        //deal msg
        if (ret != 0) {
            rpc->running = 0;
            printf("netmgr_srv_running_func ,recv err %d \r\n", ret);
            break;
        }
        aos_parcel_init(&pkt, rpc->recv_buff, get_len);
        netmgr_rpc_srv_deal_pkg(rpc, &pkt);
    }
    return;
}

int netmgr_rpc_service_start()
{
    int ret;
    aos_task_t task;

    ret = netmgr_rpc_srv_init(&g_netmgr_rpc, NETMGR_QUEUE_QUERY_NAME, NETMGR_QUEUE_RESP_NAME);
    if (ret < 0) {
        return ret;
    }
    g_netmgr_rpc.running = 1;
    ret = aos_task_new_ext(&task, "netmgr rpc task", netmgr_srv_running_func,
                           &g_netmgr_rpc, NETMGR_IPC_TASK_STACK, AOS_DEFAULT_APP_PRI - 1, 1);
    if (ret != 0) {
        g_netmgr_rpc.running = 0;
        netmgr_rpc_srv_deinit(&g_netmgr_rpc);
    }

    return ret;
}

int netmgr_rpc_service_stop()
{
    netmgr_rpc_t *rpc = &g_netmgr_rpc;

    rpc->running = 0;

    netmgr_rpc_srv_deinit(rpc);
    return 0;
}
