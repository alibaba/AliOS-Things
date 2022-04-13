/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include "k_api.h"
#include "aos/kernel.h"
#include "lwip/debug.h"
#include "lwip_rpc.h"

void ulwip_request_proccess(struct aos_parcel_t *p_pkt)
{
    u_socket_reponse_t *p_out = NULL;
    u_socket_request_t *p_in = NULL;
    int m_reponse_handle = 0;
    size_t m_len = sizeof(m_reponse_handle);
    char *p_in_buf = NULL;
    int ret = AOS_FAILED;

    if ((NULL == p_pkt) || (0 == p_pkt->max_size))
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]invalid pkt:%p or max_size:%d\r\n", __FUNCTION__, p_pkt, ((p_pkt != NULL) ? p_pkt->max_size : 0)));
        goto reponse;
    }

    ret = aos_parcel_get(AP_UVARINT, &m_reponse_handle, &m_len, p_pkt);
    if (ret != AOS_SUCCESS)
    {
        goto reponse;
    }

    m_len = p_pkt->max_size;

    LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]reply_handle:%x, package max size:%x\r\n", __FUNCTION__, m_reponse_handle, m_len));

    p_in_buf = malloc(m_len);
    if (NULL == p_in_buf)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]malloc buf for rpc msg failed\r\n", __FUNCTION__));
        goto reponse;
    }

    ret = aos_parcel_get(AP_ALL, p_in_buf, &m_len, p_pkt);
    if ((ret != AOS_SUCCESS) || (m_len <= 0))
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]rpc receive data failed, ret:%d\r\n", __FUNCTION__, ret));
        goto reponse;
    }

    p_in = (u_socket_request_t *)p_in_buf;

    if ((EVT_ST_MAX <= p_in->m_evt) || (0 > p_in->m_evt))
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("p_msg->evt:%d >= DEV_MSG_MAX:%d\r\n", p_in->m_evt, EVT_ST_MAX));
        goto reponse;
    }

    if (m_socket_opt[p_in->m_evt] == NULL)
    {
        goto reponse;
    }

    LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]start proccess event %d\r\n", __FUNCTION__, p_in->m_evt));

    m_socket_opt[p_in->m_evt](p_in, &p_out);

    LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]proccess event %d success!\r\n", __FUNCTION__, p_in->m_evt));

reponse:

    /* Only support response the rpc message after 
     * getting the return value calling back from lwip socket interface .
     * If we cann't get object of p_out, the reason is caused by system error.
     */
    if (p_out != NULL)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]set event %d response 0x%x!\r\n", __FUNCTION__, (p_in != NULL)?p_in->m_evt:(-1), m_reponse_handle));

        ret = aos_rpc_resp(m_reponse_handle, 1, p_out->m_len, p_out);
        if (ret != AOS_SUCCESS)
        {
            LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]reply failed - %d\n", __FUNCTION__, m_reponse_handle));
        }

        free(p_out);
    }

    if (NULL != p_in_buf)
    {
        free(p_in_buf);
    }
}

void ulwip_request_callback(int eid, void *val)
{
    int reason;
    struct aos_revt_param_t *p_param;

    if (NULL == val)
    {
        return;
    }

    p_param = (struct aos_revt_param_t *)val;
    if (NULL == p_param->param)
    {
        return;
    }

    reason = *(int *)p_param->param;

    if (reason != RPC_EID_STOP)
    {
        aos_rpc_close(p_param->srpc_handle);
    }
}
