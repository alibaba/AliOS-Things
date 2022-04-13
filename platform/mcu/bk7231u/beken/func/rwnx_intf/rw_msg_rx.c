#include "include.h"
#include "rw_msg_rx.h"
#include "rw_pub.h"
#include "ke_msg.h"
#include "mem_pub.h"
#include "mac_common.h"
#include "scanu_task.h"
#include "sa_station.h"
#include "apm_task.h"
#include "me_task.h"
#include "sm_task.h"
#include "hostapd_intf_pub.h"
#include "mac_ie.h"
#include "ieee802_11_defs.h"
#include "wlan_ui_pub.h"
#include "mcu_ps_pub.h"


uint32_t scan_cfm = 0;
uint8_t *ind_buf_ptr = 0;
struct co_list rw_msg_rx_head;
struct co_list rw_msg_tx_head;
msg_sta_states connect_flag = MSG_IDLE;

SCAN_RST_UPLOAD_T *scan_rst_set_ptr = 0;
IND_CALLBACK_T scan_cfm_cb = {0};
IND_CALLBACK_T assoc_cfm_cb = {0};
IND_CALLBACK_T deassoc_evt_cb = {0};
IND_CALLBACK_T deauth_evt_cb = {0};
IND_CALLBACK_T wlan_connect_user_cb = {0};

FUNC_1PARAM_PTR connection_lost_cb =NULL;
FUNC_1PARAM_PTR	auth_fail_cb = NULL;
FUNC_1PARAM_PTR assoc_fail_cb = NULL;
FUNC_1PARAM_PTR sta_connect_start_cb = NULL;
rw_event_handler rw_event_handlers[RW_EVT_MAX] = {0};

extern void app_set_sema(void);
extern int get_security_type_from_ie(u8 *, int, u16);

#if 1 /* scan result*/
UINT8 *sr_malloc_result_item(UINT32 vies_len)
{
    return os_zalloc(vies_len + sizeof(struct sta_scan_res));
}

void sr_free_result_item(UINT8 *item_ptr)
{
    os_free(item_ptr);
}

UINT8 *sr_malloc_shell(void)
{
    UINT8 *ptr;
    UINT32 layer1_space_len;
    UINT32 layer2_space_len;

    layer1_space_len = sizeof(SCAN_RST_UPLOAD_T);
    layer2_space_len = MAX_BSS_LIST * sizeof(struct sta_scan_res *);
    ptr = os_zalloc(layer1_space_len + layer2_space_len);

    ASSERT(ptr);

    return ptr;
}

void sr_free_shell(UINT8 *shell_ptr)
{
    os_free(shell_ptr);
}

void sr_free_all(SCAN_RST_UPLOAD_T *scan_rst)
{
    UINT32 i;

    for(i = 0; i < scan_rst->scanu_num; i ++)
    {
        sr_free_result_item((UINT8 *)scan_rst->res[i]);
        scan_rst->res[i] = 0;
    }
    scan_rst->scanu_num = 0;

    sr_free_shell((UINT8 *)scan_rst);
}

void *sr_get_scan_results(void)
{
    return scan_rst_set_ptr;
}

void sr_release_scan_results(SCAN_RST_UPLOAD_PTR ptr)
{
    if(ptr)
    {
        sr_free_all(ptr);
    }
    scan_rst_set_ptr = 0;
	wpa_clear_scan_results();
}
#endif

#if 2
void mr_kmsg_init(void)
{
    co_list_init(&rw_msg_tx_head);
    co_list_init(&rw_msg_rx_head);
}

UINT32 mr_kmsg_fwd(struct ke_msg *msg)
{
    GLOBAL_INT_DECLARATION();

    GLOBAL_INT_DISABLE();
    co_list_push_back(&rw_msg_rx_head, &msg->hdr);
    GLOBAL_INT_RESTORE();

    app_set_sema();

    return 0;
}

void mr_kmsg_flush(void)
{
    while(mr_kmsg_fuzzy_handle())
    {
        ;
    }
}

UINT32 mr_kmsg_fuzzy_handle(void)
{
    UINT32 ret = 0;
    struct ke_msg *msg;
    struct co_list_hdr *node;

    GLOBAL_INT_DECLARATION();

    GLOBAL_INT_DISABLE();
    node = co_list_pop_front(&rw_msg_rx_head);
    GLOBAL_INT_RESTORE();

    if(node)
    {
        msg = (struct ke_msg *)node;
        ke_msg_free(msg);

        ret = 1;
    }

    return ret;
}

UINT32 mr_kmsg_exact_handle(UINT16 rsp)
{
    UINT32 ret = 0;
    struct ke_msg *msg;
    struct co_list_hdr *node;

    GLOBAL_INT_DECLARATION();

    GLOBAL_INT_DISABLE();
    node = co_list_pop_front(&rw_msg_rx_head);
    GLOBAL_INT_RESTORE();

    if(node)
    {
        msg = (struct ke_msg *)node;
        if(rsp == msg->id)
        {
            ret = 1;
        }
        ke_msg_free(msg);
    }

    return ret;
}

#endif

void user_callback_func_register(FUNC_1PARAM_PTR sta_connect_start_func,
								 FUNC_1PARAM_PTR connection_lost_func,
								 FUNC_1PARAM_PTR auth_fail_func,
								 FUNC_1PARAM_PTR assoc_fail_func )		
{
	sta_connect_start_cb = sta_connect_start_func;
	connection_lost_cb = connection_lost_func;
	auth_fail_cb = auth_fail_func;
	assoc_fail_cb = assoc_fail_func;
	
}
//
void user_callback_func_unregister(void)
{
    sta_connect_start_cb = NULL;
	connection_lost_cb = NULL;
	auth_fail_cb = NULL;
	assoc_fail_cb = NULL;
}

void rw_evt_set_callback(enum rw_evt_type evt_type, rw_event_handler handler)
{
    if ((RW_EVT_STA_CONNECTED <= evt_type) && (evt_type < RW_EVT_MAX))
    {
        rw_event_handlers[evt_type] = handler;
    }
}

void mhdr_connect_user_cb(FUNC_2PARAM_PTR ind_cb, void *ctxt)
{
    wlan_connect_user_cb.cb = ind_cb;
    wlan_connect_user_cb.ctxt_arg = ctxt;
}

void mhdr_assoc_cfm_cb(FUNC_2PARAM_PTR ind_cb, void *ctxt)
{
    assoc_cfm_cb.cb = ind_cb;
    assoc_cfm_cb.ctxt_arg = ctxt;
}

void mhdr_scanu_reg_cb(FUNC_2PARAM_PTR ind_cb, void *ctxt)
{
    scan_cfm_cb.cb = ind_cb;
    scan_cfm_cb.ctxt_arg = ctxt;
}

void mhdr_deauth_evt_cb(FUNC_2PARAM_PTR ind_cb, void *ctxt)
{
    deauth_evt_cb.cb = ind_cb;
    deauth_evt_cb.ctxt_arg = ctxt;
}

void mhdr_deassoc_evt_cb(FUNC_2PARAM_PTR ind_cb, void *ctxt)
{
    deassoc_evt_cb.cb = ind_cb;
    deassoc_evt_cb.ctxt_arg = ctxt;
}

void mhdr_disconnect_ind(void *msg)
{
    struct ke_msg *msg_ptr;
    struct sm_disconnect_ind *disc;

    msg_ptr = (struct ke_msg *)msg;
    disc = (struct sm_disconnect_ind *)msg_ptr->param;

    mhdr_set_station_status(MSG_CONN_FAIL);

    sa_reconnect_init();

    if(deassoc_evt_cb.cb)
    {
        (*deassoc_evt_cb.cb)(deassoc_evt_cb.ctxt_arg, disc->vif_idx);
    }
}

void mhdr_connect_ind(void *msg, UINT32 len)
{
    struct ke_msg *msg_ptr;
    struct sm_connect_indication *conn_ind_ptr;

    msg_ptr = (struct ke_msg *)msg;
    conn_ind_ptr = (struct sm_connect_indication *)msg_ptr->param;
    if(0 == conn_ind_ptr->status_code)
    {
        os_printf("---------SM_CONNECT_IND_ok\r\n");
        mhdr_set_station_status(MSG_CONN_SUCCESS);

        if(assoc_cfm_cb.cb)
        {
            (*assoc_cfm_cb.cb)(assoc_cfm_cb.ctxt_arg, conn_ind_ptr->vif_idx);
        }

        if(wlan_connect_user_cb.cb)
        {
            (*wlan_connect_user_cb.cb)(wlan_connect_user_cb.ctxt_arg, 0);
        }
    }
    else
    {
        os_printf("---------SM_CONNECT_IND_fail\r\n");
        mhdr_set_station_status(MSG_CONN_FAIL);

        sa_reconnect_init();
  
        if(deassoc_evt_cb.cb)
        {
            (*deassoc_evt_cb.cb)(deassoc_evt_cb.ctxt_arg, conn_ind_ptr->vif_idx);
        }
    }

    mcu_prevent_clear(MCU_PS_CONNECT);
#if CFG_USE_BLE_PS
    rf_can_share_for_ble();
#endif
}

void mhdr_set_station_status(msg_sta_states val)
{
    connect_flag = val;
}

msg_sta_states mhdr_get_station_status(void)
{
    return connect_flag;
}

static void sort_scan_result(SCAN_RST_UPLOAD_T *ap_list)
{
    int i, j;
    struct sta_scan_res *tmp;

    if (ap_list->scanu_num == 0)
        return;

    for(i = 0; i < (ap_list->scanu_num - 1); i++)
    {
        for(j = i + 1; j < ap_list->scanu_num; j++)
        {
            if (ap_list->res[j]->level > ap_list->res[i]->level)
            {
                tmp = ap_list->res[j];
                ap_list->res[j] = ap_list->res[i];
                ap_list->res[i] = tmp;
            }
        }
    }
}

UINT32 mhdr_scanu_start_cfm(void *msg, SCAN_RST_UPLOAD_T *ap_list)
{
    struct scanu_start_cfm *cfm;
    struct ke_msg *msg_ptr;

    msg_ptr = (struct ke_msg *)msg;
    cfm = (struct scanu_start_cfm *)msg_ptr->param;

    if(ap_list)
    {
        sort_scan_result(ap_list);
        wpa_buffer_scan_results();
    }

    if(scan_cfm_cb.cb)
    {
        (*scan_cfm_cb.cb)(scan_cfm_cb.ctxt_arg, cfm->vif_idx);
    }

    return RW_SUCCESS;
}


UINT32 mhdr_scanu_result_ind(SCAN_RST_UPLOAD_T *scan_rst, void *msg, UINT32 len)
{
    UINT32 ret, chann;
    UINT8 *elmt_addr;
    UINT32 vies_len, i;
    UINT8 *var_part_addr;
    struct ke_msg *msg_ptr;
    SCAN_RST_ITEM_PTR item;
    SCAN_RST_UPLOAD_PTR result_ptr;
    SCAN_IND_PTR scanu_ret_ptr;
    IEEE802_11_PROBE_RSP_PTR probe_rsp_ieee80211_ptr;
    char on_channel;
    int replace_index = -1;

    ret = RW_SUCCESS;
    result_ptr = scan_rst;

    if (result_ptr->scanu_num >= MAX_BSS_LIST)
        goto scan_rst_exit;

    msg_ptr = (struct ke_msg *)msg;
    scanu_ret_ptr = (SCAN_IND_PTR)msg_ptr->param;
    probe_rsp_ieee80211_ptr =  (IEEE802_11_PROBE_RSP_PTR)scanu_ret_ptr->payload;
    vies_len = scanu_ret_ptr->length - MAC_BEACON_VARIABLE_PART_OFT;
    var_part_addr = probe_rsp_ieee80211_ptr->rsp.variable;

    elmt_addr = (UINT8 *)mac_ie_find((UINT32)var_part_addr, (UINT16)vies_len, MAC_ELTID_DS);
    if(elmt_addr) // adjust channel
    {
        chann = *(elmt_addr + MAC_DS_CHANNEL_OFT);
        if (chann == rw_ieee80211_get_chan_id(scanu_ret_ptr->center_freq))
            on_channel = 1;
        else
            on_channel = 0;
    }
    else
    {
        chann = rw_ieee80211_get_chan_id(scanu_ret_ptr->center_freq);
        on_channel = 0;
    }

    /* check the duplicate bssid*/
    do
    {
        for(i = 0; i < result_ptr->scanu_num; i ++)
        {
            if(!os_memcmp(probe_rsp_ieee80211_ptr->bssid, result_ptr->res[i]->bssid, ETH_ALEN))
            {
                if ((result_ptr->res[i]->on_channel == 1) || (on_channel == 0))
                {
                    goto scan_rst_exit;
                }
                else
                {
                    replace_index = i; // should replace it.
                }
            }
        }
    }
    while(0);

    item = (SCAN_RST_ITEM_PTR)sr_malloc_result_item(vies_len);
    if (item == NULL)
        goto scan_rst_exit;

    elmt_addr = (UINT8 *)mac_ie_find((UINT32)var_part_addr,
                                     (UINT16)vies_len,
                                     MAC_ELTID_SSID);
    if(elmt_addr)
    {
        UINT8 ssid_len = *(elmt_addr + MAC_SSID_LEN_OFT);

        if (ssid_len > MAC_SSID_LEN)
            ssid_len = MAC_SSID_LEN;

        os_memcpy(item->ssid, elmt_addr + MAC_SSID_SSID_OFT, ssid_len);
    }
    else
    {
        os_printf("NoSSid\r\n");
    }

    os_memcpy(item->bssid, probe_rsp_ieee80211_ptr->bssid, ETH_ALEN);
    item->channel = chann;
    item->beacon_int = probe_rsp_ieee80211_ptr->rsp.beacon_int;
    item->caps = probe_rsp_ieee80211_ptr->rsp.capab_info;
    item->level = scanu_ret_ptr->rssi;
    item->on_channel = on_channel;

    os_memcpy(item->tsf, probe_rsp_ieee80211_ptr->rsp.timestamp, 8);

    item->ie_len = vies_len;
    os_memcpy(item + 1, var_part_addr, vies_len);

    item->security = get_security_type_from_ie((u8 *)var_part_addr, vies_len, item->caps);

    if (replace_index >= 0)
    {
        sr_free_result_item((UINT8 *)result_ptr->res[replace_index]);
        result_ptr->res[replace_index] = item;
    }
    else
    {
        result_ptr->res[result_ptr->scanu_num] = item;
        result_ptr->scanu_num ++;
    }

scan_rst_exit:
    return ret;
}

void rwnx_handle_recv_msg(struct ke_msg *rx_msg)
{
    switch(rx_msg->id)
    {
    case SCANU_START_CFM:
        scan_cfm = 1;
        mhdr_scanu_start_cfm(rx_msg, scan_rst_set_ptr);
        break;

    case SCANU_RESULT_IND:
        if(scan_cfm && scan_rst_set_ptr)
        {
            sr_release_scan_results(scan_rst_set_ptr);
            scan_rst_set_ptr = 0;
            scan_cfm = 0;
        }

        if(0 == scan_rst_set_ptr)
        {
            scan_rst_set_ptr = (SCAN_RST_UPLOAD_T *)sr_malloc_shell();
            ASSERT(scan_rst_set_ptr);
            scan_rst_set_ptr->scanu_num = 0;
            scan_rst_set_ptr->res = (SCAN_RST_ITEM_PTR*)&scan_rst_set_ptr[1];
        }

        mhdr_scanu_result_ind(scan_rst_set_ptr, rx_msg, rx_msg->param_len);
        break;

    case SM_CONNECT_IND:
        if(scan_rst_set_ptr)
        {
            sr_release_scan_results(scan_rst_set_ptr);
            scan_rst_set_ptr = 0;
        }

        mhdr_connect_ind(rx_msg, rx_msg->param_len);
        break;

    case SM_DISCONNECT_IND:
        os_printf("SM_DISCONNECT_IND\r\n");
        mhdr_disconnect_ind(rx_msg);
        break;

	case SM_CONNCTION_START_IND:
		if(sta_connect_start_cb)
		{
			(*sta_connect_start_cb)(rx_msg->param);
		}
		break;
		
	case SM_BEACON_LOSE_IND:
        #if CFG_USE_BLE_PS
        rf_not_share_for_ble();
        #endif
        
		if(connection_lost_cb)
		{
			(*connection_lost_cb)(0);
		}
		break;
		
	case SM_AUTHEN_FAIL_IND:
		if(auth_fail_cb)
		{
			(*auth_fail_cb)(rx_msg->param);
		}
		break;
		
	case SM_ASSOC_FAIL_INID:
		if(assoc_fail_cb)
		{
			(*assoc_fail_cb)(rx_msg->param);
		}
		break;

    case SM_ASSOC_IND:
        if (rw_event_handlers[RW_EVT_STA_CONNECTED])
        {
            struct rw_evt_payload evt_payload;
            struct sm_assoc_ind *sta_assoc = (struct sm_assoc_ind *)rx_msg->param;
            os_memcpy(evt_payload.mac, sta_assoc->mac, sizeof(sta_assoc->mac));
            (*rw_event_handlers[RW_EVT_STA_CONNECTED])(RW_EVT_STA_CONNECTED, (void *)&evt_payload);
        }
        break;

    case SM_DEASSOC_IND:
        if (rw_event_handlers[RW_EVT_STA_DISCONNECTED])
        {
            struct rw_evt_payload evt_payload;
            struct sm_deassoc_ind *sta_deassoc = (struct sm_deassoc_ind *)rx_msg->param;
            os_memcpy(evt_payload.mac, sta_deassoc->mac, sizeof(sta_deassoc->mac));
            (*rw_event_handlers[RW_EVT_STA_DISCONNECTED])(RW_EVT_STA_DISCONNECTED, (void *)&evt_payload);
        }
        break;

    case SM_ASSOC_FAILED_IND:
        if (rw_event_handlers[RW_EVT_STA_CONNECT_FAILED])
        {
            struct rw_evt_payload evt_payload;
            struct sm_assoc_failed_ind *sta_assoc_failed = (struct sm_assoc_failed_ind *)rx_msg->param;
            os_memcpy(evt_payload.mac, sta_assoc_failed->mac, sizeof(sta_assoc_failed->mac));
            (*rw_event_handlers[RW_EVT_STA_CONNECT_FAILED])(RW_EVT_STA_CONNECT_FAILED, (void *)&evt_payload);
        }
        break;

    case APM_ASSOC_IND:
        if (rw_event_handlers[RW_EVT_AP_CONNECTED])
        {
            struct rw_evt_payload evt_payload;
            struct apm_assoc_ind *sta_assoc = (struct apm_assoc_ind *)rx_msg->param;
            os_memcpy(evt_payload.mac, sta_assoc->mac, sizeof(sta_assoc->mac));
            (*rw_event_handlers[RW_EVT_AP_CONNECTED])(RW_EVT_AP_CONNECTED, (void *)&evt_payload);
        }
        break;

    case APM_DEASSOC_IND:
        if (rw_event_handlers[RW_EVT_AP_DISCONNECTED])
        {
            struct rw_evt_payload evt_payload;
            struct apm_deassoc_ind *sta_deassoc = (struct apm_deassoc_ind *)rx_msg->param;
            os_memcpy(evt_payload.mac, sta_deassoc->mac, sizeof(sta_deassoc->mac));
            (*rw_event_handlers[RW_EVT_AP_DISCONNECTED])(RW_EVT_AP_DISCONNECTED, (void *)&evt_payload);
        }
        break;

    case APM_ASSOC_FAILED_IND:
        if (rw_event_handlers[RW_EVT_AP_CONNECT_FAILED])
        {
            struct rw_evt_payload evt_payload;
            struct apm_assoc_failed_ind *sta_assoc_failed = (struct apm_assoc_failed_ind *)rx_msg->param;
            os_memcpy(evt_payload.mac, sta_assoc_failed->mac, sizeof(sta_assoc_failed->mac));
            (*rw_event_handlers[RW_EVT_AP_CONNECT_FAILED])(RW_EVT_AP_CONNECT_FAILED, (void *)&evt_payload);
        }
        break;

#if CFG_USE_AP_PS
    case MM_PS_CHANGE_IND:
        rwm_msdu_ps_change_ind_handler(rx_msg);
        break;
#endif

    default:
        break;
    }
}

void rwnx_recv_msg(void)
{
    struct ke_msg *rx_msg;
    MSG_SND_NODE_PTR tx_msg;
    struct co_list_hdr *rx_node, *tx_node;

    GLOBAL_INT_DECLARATION();

    while(1)
    {
        uint8_t find = 0;

        rx_node = co_list_pop_front(&rw_msg_rx_head);
        if(!rx_node)
            break;

        rx_msg = (struct ke_msg *)rx_node;

        GLOBAL_INT_DISABLE();
        tx_node = co_list_pick(&rw_msg_tx_head);
        GLOBAL_INT_RESTORE();

        while(tx_node)
        {
            tx_msg = (MSG_SND_NODE_PTR)tx_node;
            if(rx_msg->id == tx_msg->reqid)
            {
                find = 1;
                break;
            }

            GLOBAL_INT_DISABLE();
            tx_node = co_list_next(tx_node);
            GLOBAL_INT_RESTORE();
        }

        if(find)
        {
            int ret;
            GLOBAL_INT_DISABLE();
            co_list_extract(&rw_msg_rx_head, rx_node);
            co_list_extract(&rw_msg_tx_head, tx_node);
            GLOBAL_INT_RESTORE();

            if(tx_msg->cfm && rx_msg->param_len)
                os_memcpy(tx_msg->cfm, &rx_msg->param[0], rx_msg->param_len);

            ret = rtos_set_semaphore(&tx_msg->semaphore);
            ASSERT(0 == ret);
        }
        else
        {
            rwnx_handle_recv_msg(rx_msg);
        }

        ke_msg_free(rx_msg);
    }
}

// eof

