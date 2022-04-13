/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <stdbool.h>
#include "aos/kernel.h"
#include "lwip/debug.h"
#include "lwip_rpc.h"
#include "rpc.h"

volatile static bool m_ulwip_service_init_flag = false;
aos_mutex_t g_lwip_rpc_mutex;

int ulwip_service_init(void)
{
    int m_ulwip_rpc_handle;
    int m_select_rpc_handle;
    int ret = AOS_SUCCESS;

    if (m_ulwip_service_init_flag == true)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]ulwip service has initialized\r\n", __FUNCTION__));
        return ret;
    }

    /* register ulwip rpc service into PM */
    ret = aos_rpc_regist(ULWIP_RPC_MAIN_NAME, ULWIP_RPC_BUFFER_SIZE, &m_ulwip_rpc_handle);
    if (ret != AOS_SUCCESS)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]register rpc service %s failed, ret:%d\r\n", __FUNCTION__, ULWIP_RPC_MAIN_NAME, ret));
        return ret;
    }

    LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]register rpc service %s succeed, handle:0x%x\r\n", __FUNCTION__, ULWIP_RPC_MAIN_NAME, m_ulwip_rpc_handle));

    /* create thread to process rpc message */
    ret = aos_rpc_run(m_ulwip_rpc_handle, ulwip_request_proccess, ulwip_request_callback);
    if (ret != AOS_SUCCESS)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]aos_rpc_run on service:%s fails, ret:%d\r\n", __FUNCTION__, ULWIP_RPC_MAIN_NAME, ret));
        /* NOTE: free resource but the risk is that rpc thread is put into the ready queue */
        aos_rpc_close(m_ulwip_rpc_handle);
        return ret;
    }

    /* register ulwip rpc service into PM */
    ret = aos_rpc_regist(USELECT_RPC_MAIN_NAME, USELECT_RPC_BUFFER_SIZE, &m_select_rpc_handle);
    if (ret != AOS_SUCCESS)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]register rpc service %s failed, ret:%d\r\n", __FUNCTION__, USELECT_RPC_MAIN_NAME, ret));
        aos_rpc_close(m_ulwip_rpc_handle);
        return ret;
    }

    LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]register rpc service %s succeed, handle:0x%x\r\n", __FUNCTION__, USELECT_RPC_MAIN_NAME, m_select_rpc_handle));

    /* create thread to process rpc message */
    ret = aos_rpc_run(m_select_rpc_handle, ulwip_request_proccess, ulwip_request_callback);
    if (ret != AOS_SUCCESS)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]aos_rpc_run on service:%s fails, ret:%d\r\n", __FUNCTION__, USELECT_RPC_MAIN_NAME, ret));
        aos_rpc_close(m_ulwip_rpc_handle);
        /* NOTE: free resource but the risk is that rpc thread is put into the ready queue */
        aos_rpc_close(m_select_rpc_handle);
        return ret;
    }

    ret = aos_mutex_new(&g_lwip_rpc_mutex);
    if (ret != AOS_SUCCESS)
    {
        LWIP_DEBUGF(LWIP_RPC_DEBUG, ("[%s]aos_mutex_new on service:%s fails, ret:%d\r\n", __FUNCTION__, USELECT_RPC_MAIN_NAME, ret));
        aos_rpc_close(m_ulwip_rpc_handle);
        aos_rpc_close(m_select_rpc_handle);
    }

    m_ulwip_service_init_flag = true;

    return ret;
}
