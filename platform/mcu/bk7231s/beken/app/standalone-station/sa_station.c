#include "include.h"

#include "schedule_pub.h"

#include "sa_station.h"
#include "drv_model_pub.h"
#include "uart_pub.h"
#include "scanu_task.h"
#include "scan_task.h"
#include "rxu_task.h"
#include "mm_task.h"
#include "me_task.h"
#include "sm_task.h"
#include "rw_msg_tx.h"
#include "mac_ie.h"
#include "vif_mgmt.h"

#include "arm_arch.h"
#include "mem_pub.h"
#include "rw_pub.h"
#include "common.h"

#if CFG_USE_LWIP_NETSTACK
#include "lwip_intf.h"
#endif

#if CFG_USE_TEMPERATURE_DETECT
#include "temp_detect_pub.h"
#endif

#include "param_config.h"

#if (!CFG_SUPPORT_ALIOS)
#include "sys_rtos.h"
#endif
#include "rtos_pub.h"
#include "errno.h"
#include "error.h"
#include "errno-base.h"
#include "rw_ieee80211.h"

extern struct mac_scan_result *scanu_search_by_ssid(struct mac_ssid const *ssid);

/*---------------------------------------------------------------------------*/
int sa_station_send_associate_cmd(CONNECT_PARAM_T *connect_param)
{
    struct sm_connect_indication *conn_ind_ptr;
    struct mac_scan_result *desired_ap_ptr;
    struct sm_connect_cfm sm_connect_cfm;
    int ret;


    if(g_sta_param_ptr->fast_connect_set)
    {
        g_sta_param_ptr->fast_connect_set = 0;
        connect_param->chan.freq = rw_ieee80211_get_centre_frequency(g_sta_param_ptr->fast_connect.chann);
        connect_param->chan.band = 0;
        connect_param->chan.flags = 0;
        connect_param->chan.tx_power = 10;
    }
    else
    {
        desired_ap_ptr = scanu_search_by_ssid((void *)&connect_param->ssid);
        if(NULL == desired_ap_ptr)
        {
            return -1;
        }
        connect_param->chan = *(desired_ap_ptr->chan);
        if(0 == connect_param->chan.tx_power)
        {
            connect_param->chan.tx_power = 10;
        }
    }

    ret = rw_msg_send_sm_connect_req(connect_param, &sm_connect_cfm);
    if(ret)
        return ret;

    switch (sm_connect_cfm.status)
    {
    case CO_OK:
        ret = 0;
        break;

    case CO_BUSY:
        ret = -ERRINPROGRESS;
        break;

    case CO_OP_IN_PROGRESS:
        ret = -ERRALREADY;
        break;

    default:
        ret = -EERIO;
        break;
    }

    return ret;
}

/*---------------------------------------------------------------------------*/
static void sa_station_cfg80211_init(void)
{
    if (rwm_mgmt_is_vif_first_used() == NULL)
    {
        SASTA_PRT("[sa_sta]MM_RESET_REQ\r\n");
        rw_msg_send_reset();

        SASTA_PRT("[sa_sta]ME_CONFIG_REQ\r\n");
        rw_msg_send_me_config_req();

        SASTA_PRT("[sa_sta]ME_CHAN_CONFIG_REQ\r\n");
        rw_msg_send_me_chan_config_req();

        SASTA_PRT("[sa_sta]MM_START_REQ\r\n");
        rw_msg_send_start();
    }
}

#ifndef DISABLE_RECONNECT

#if (CFG_SUPPORT_ALIOS || CFG_SUPPORT_RTT)
beken_thread_t reconnect_thread_handle = NULL;
#else 
xTaskHandle  reconnect_thread_handle = NULL;
#endif
uint32_t  reconnect_stack_size = 2000;

void sa_reconnect_main(void *arg)
{
    sa_station_init();
    os_printf("sa_reconnect_main\r\n");

    rtos_delete_thread( NULL );
    reconnect_thread_handle = NULL;
}

void sa_reconnect_init(void)
{
    OSStatus ret;
    return; // try it;
    if(NULL == reconnect_thread_handle)
    {
        ret = rtos_create_thread(&reconnect_thread_handle,
                                 THD_RECONNECT_PRIORITY,
                                 "reconnect_thread",
                                 (beken_thread_function_t)sa_reconnect_main,
                                 (unsigned short)reconnect_stack_size,
                                 (beken_thread_arg_t)0);
        ASSERT(kNoErr == ret);
    }
    else
    {
        os_printf("sa_reconnect_init_strange\r\n");
    }
}
#endif

void sa_station_init(void)
{
    sa_station_cfg80211_init();
}

void sa_station_uninit(void)
{
}

// eof
