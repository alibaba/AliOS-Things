#include "include.h"
#include "arm_arch.h"

#include "sdio_pub.h"

#include "sdio_intf.h"
#include "sdio_intf_pub.h"

#include "sdio_pub.h"
#include "drv_model_pub.h"

#include "mem_pub.h"

#include "mac.h"
#include "ke_event.h"
#include "llc.h"
#include "txu_cntrl.h"
#include "txl_cntrl.h"

#include "mem_pub.h"
#include "co_utils.h"
#include "scan_task.h"
#include "scanu_task.h"
#include "me_task.h"
#include "mm_task.h"
#include "sm_task.h"
#include "vif_mgmt.h"
#include "apm_task.h"
#include "rxu_task.h"

#include "uart_pub.h"
#include "mem_pub.h"
#include "apm_task.h"
#include "mac_ie.h"

#include "hostapd_cfg.h"
#include "sa_station.h"

#if CFG_USE_TEMPERATURE_DETECT
#include "temp_detect_pub.h"
#endif

#include "common.h"
#include "rwnx.h"
#include "rw_pub.h"



#if CFG_SDIO
LIST_HEADER_T inbound_list;

UINT16 freq_2_4_G[WIFI_2_4_G_CHANNEL_NUM] =
{
    2412,
    2417,
    2422,
    2427,
    2432,
    2437,
    2442,
    2447,
    2452,
    2457,
    2462,
    2467,
    2472,
    2484
};

UINT32 scan_start_flag = 0;
UINT32 scan_resp_cmd_sn = 0;
SDIO_NODE_PTR temp_mem_node_ptr;

SDIO_NODE_PTR sdio_get_rxed_node(void)
{
    UINT32 status;
    UINT32 rd_sta;
    DD_HANDLE sdio_hdl;
    SDIO_NODE_PTR ret = 0;
    SDIO_NODE_PTR mem_node_ptr;

    sdio_hdl = ddev_open(SDIO_DEV_NAME, &status, 0);
    if(DD_HANDLE_UNVALID == sdio_hdl)
    {
        goto rxed_exception;
    }

    rd_sta = ddev_read(sdio_hdl, (char *)SDIO_DUMMY_BUFF_ADDR, SDIO_DUMMY_LENGTH, H2S_RD_SPECIAL);
    if(SDIO_FAILURE == rd_sta)
    {
        goto rd_fail;
    }
    else
    {
        mem_node_ptr = (SDIO_NODE_PTR)rd_sta;
        ret = mem_node_ptr;
    }

rd_fail:
    //ddev_close(sdio_hdl);

rxed_exception:
    return ret;
}

UINT32 sdio_get_free_node_count(void)
{
    UINT32 status;
    UINT32 count = 0;
    DD_HANDLE sdio_hdl;

    sdio_hdl = ddev_open(SDIO_DEV_NAME, &status, 0);
    if(DD_HANDLE_UNVALID == sdio_hdl)
    {
        count = 0;

        goto rxed_exception;
    }

    ddev_control(sdio_hdl, SDIO_CMD_GET_CNT_FREE_NODE, &count);

rxed_exception:
    return count;
}

UINT32 sdio_get_free_node(UINT8 **buf_pptr, UINT32 buf_size)
{
    UINT32 size;
    UINT32 status;
    DD_HANDLE sdio_hdl;
    UINT32 ret = SDIO_INTF_SUCCESS;

    sdio_hdl = ddev_open(SDIO_DEV_NAME, &status, 0);
    if(DD_HANDLE_UNVALID == sdio_hdl)
    {
        ret = SDIO_INTF_FAILURE;

        goto rxed_exception;
    }

    size = buf_size;
    temp_mem_node_ptr = (SDIO_NODE_PTR)ddev_control(sdio_hdl, SDIO_CMD_GET_FREE_NODE, &size);
    if(temp_mem_node_ptr)
    {
        *buf_pptr = temp_mem_node_ptr->addr;
        temp_mem_node_ptr->length = size;
    }
    else
    {
        *buf_pptr = 0;
    }

rxed_exception:
    return ret;
}

UINT32 sdio_release_one_node(SDIO_NODE_PTR mem_node_ptr)
{
    UINT32 status;
    DD_HANDLE sdio_hdl;
    UINT32 ret = SDIO_INTF_SUCCESS;

    sdio_hdl = ddev_open(SDIO_DEV_NAME, &status, 0);
    if(DD_HANDLE_UNVALID == sdio_hdl)
    {
        ret = SDIO_INTF_FAILURE;

        goto rxed_exception;
    }

    ddev_control(sdio_hdl, SDIO_CMD_PUSH_FREE_NODE, (void *)mem_node_ptr);

rxed_exception:
    return ret;
}

UINT32 sdio_emb_get_tx_info(UINT8 *buf, UINT8 *tid)
{
    *tid = 0xFF;

    return SDIO_INTF_SUCCESS;
}

UINT32 sdio_emb_get_hwqueue_id(UINT8 tid)
{
    return AC_VI;
}

void sdio_emb_txdesc_copy(struct txdesc *dst_local, ETHHDR_PTR eth_hdr_ptr)
{
    struct hostdesc *host_ptr;

    host_ptr = &dst_local->host;

    os_memcpy(&host_ptr->eth_dest_addr, &eth_hdr_ptr->h_dest, sizeof(host_ptr->eth_dest_addr));
    os_memcpy(&host_ptr->eth_src_addr, &eth_hdr_ptr->h_src, sizeof(host_ptr->eth_src_addr));
}

UINT32 outbound_upload_data(RW_RXIFO_PTR rx_info)
{
    UINT32 status;
    DD_HANDLE sdio_hdl;
    UINT32 ret = SDIO_INTF_SUCCESS;
    SDIO_NODE_PTR node = temp_mem_node_ptr;

    ASSERT(rx_info->data == node->addr);

#if 1
    STM32_FRAME_HDR *frm_hdr_ptr;
    STM32_RXPD_PTR rx_ptr;

    ASSERT(node->length);
    frm_hdr_ptr = (STM32_FRAME_HDR *)node->addr;
    frm_hdr_ptr->len = node->length;
    frm_hdr_ptr->type = MVMS_DAT;

    rx_ptr = (STM32_RXPD_PTR)&frm_hdr_ptr[1];
    os_memset(rx_ptr, 0, sizeof(STM32_RXPD_S));
    rx_ptr->pkt_ptr = 0x36; // 0x4e;
#endif

    sdio_hdl = ddev_open(SDIO_DEV_NAME, &status, 0);
    if(DD_HANDLE_UNVALID == sdio_hdl)
    {
        ret = SDIO_INTF_FAILURE;

        goto rxed_exception;
    }

    ddev_write(sdio_hdl, (char *)node, SDIO_DUMMY_LENGTH, S2H_WR_SPECIAL);

rxed_exception:

    return ret;
}

void inbound_cfm(void)
{
    LIST_HEADER_T *tmp;
    LIST_HEADER_T *pos;
    SDIO_NODE_PTR node;

    GLOBAL_INT_DECLARATION();

    //SDIO_INTF_PRT("inbound_cfm\r\n");

    GLOBAL_INT_DISABLE();

    node = NULLPTR;
    list_for_each_safe(pos, tmp, &inbound_list)
    {
        list_del(pos);
        node = list_entry(pos, SDIO_NODE_T, node_list);

        if(node->callback)
        {
            (*(node->callback))(node->Lparam, node->Rparam);
        }

        node->callback = NULLPTR;
        node->Lparam   = NULL;
        node->Rparam   = NULL;

        sdio_release_one_node(node);

        break;
    }

    GLOBAL_INT_RESTORE();

    return;
}

#if 1
UINT32 resp_inc_seqnum(void)
{
    static UINT32 seq = 0;

    seq ++;

    return seq;
}

UINT32 resp_conversion_generic_cfm(struct ke_msg *msg, STM32_FRAME_HDR **frm_pptr, UINT16 ret_cmd)
{
    UINT32 len;
    STM32_FRAME_HDR *frm;
    STM32_CMD_HDR_PTR hdr_ptr;

    len = sizeof(STM32_FRAME_HDR) + sizeof(STM32_CMD_HDR_S);
    frm = (STM32_FRAME_HDR *)os_malloc(len);
    ASSERT(frm);

    frm->len = len;
    frm->type = MVMS_CMD;
    *frm_pptr = frm;

    hdr_ptr = (STM32_CMD_HDR_PTR)((UINT32)frm + sizeof(STM32_FRAME_HDR));
    hdr_ptr->command = CMD_RET(ret_cmd);
    hdr_ptr->result  = 0;
    hdr_ptr->seqnum  = resp_inc_seqnum();
    hdr_ptr->size    = len - sizeof(STM32_FRAME_HDR);

    *frm_pptr = frm;

    return len;
}

UINT32 resp_conversion_rxu_mgt_ind(struct ke_msg *msg, STM32_FRAME_HDR **frm_pptr)
{
    UINT32 len;
    UINT16 cmd = 0;
    struct rxu_mgt_ind *mgt;
    STM32_TX_MGMT_S *frm;

    mgt = (struct rxu_mgt_ind *)msg->param;

    if((mgt->framectrl & MAC_FCTRL_TYPESUBTYPE_MASK) == MAC_FCTRL_ASSOCREQ)
    {
        SDIO_INTF_PRT("802_11_ASSOCIATE\r\n");
        cmd = SCMD_802_11_ASSOCIATE;
    }
    else if((mgt->framectrl & MAC_FCTRL_TYPESUBTYPE_MASK) == MAC_FCTRL_AUTHENT)
    {
        SDIO_INTF_PRT("802_11_AUTHENTICATE\r\n");
        cmd = SCMD_802_11_AUTHENTICATE;
    }

    len = sizeof(STM32_TX_MGMT_S);
    frm = (STM32_TX_MGMT_S *)os_malloc(len);
    ASSERT(frm);

    *frm_pptr = (STM32_FRAME_HDR *)frm;

    frm->len = len;
    frm->type = MVMS_CMD;
    frm->hdr.command = cmd;
    frm->hdr.result = 0;
    frm->hdr.size = len  - sizeof(STM32_FRAME_HDR);

    return len;
}

UINT32 resp_conversion_start_apm_cfm(struct ke_msg *msg, STM32_FRAME_HDR **frm_pptr)
{
    return resp_conversion_generic_cfm(msg, frm_pptr, SCMD_APM_START);
}

UINT32 resp_conversion_bcn_change_cfm(struct ke_msg *msg, STM32_FRAME_HDR **frm_pptr)
{
    return resp_conversion_generic_cfm(msg, frm_pptr, SCMD_BCN_CHANGE);
}

UINT32 resp_conversion_mgmt_tx_cfm(struct ke_msg *msg, STM32_FRAME_HDR **frm_pptr)
{
    UINT32 len;
    STM32_FRAME_HDR *frm;
    STM32_CMD_HDR_PTR hdr_ptr;
#if CFG_WIFI_AP_MODE
    struct me_mgmt_tx_cfm *cfm_ptr;
#endif

    len = sizeof(STM32_FRAME_HDR) + sizeof(STM32_CMD_HDR_S);
    frm = (STM32_FRAME_HDR *)os_malloc(len);
    ASSERT(frm);

    frm->len = len;
    frm->type = MVMS_CMD;
    *frm_pptr = frm;

    hdr_ptr = (STM32_CMD_HDR_PTR)((UINT32)frm + sizeof(STM32_FRAME_HDR));
    hdr_ptr->command = CMD_RET(SCMD_MGMT_TX_REQ);
    hdr_ptr->result  = 0;
    hdr_ptr->seqnum  = resp_inc_seqnum();
    hdr_ptr->size    = len - sizeof(STM32_FRAME_HDR);

    *frm_pptr = frm;

#if CFG_WIFI_AP_MODE
    cfm_ptr = (struct me_mgmt_tx_cfm *)msg->param;
    if(CO_OK == cfm_ptr->status)
    {
        extern int ke_mgmt_packet_tx(unsigned char * buf, int len, int flag);

        ke_mgmt_packet_tx((unsigned char *)cfm_ptr->hostid, cfm_ptr->length, 0);
        if(cfm_ptr->rsp_frm_new)
        {
            os_free((void *)cfm_ptr->hostid);
        }
    }
#endif

    return len;
}


UINT32 resp_conversion_add_sta_cfm(struct ke_msg *msg, STM32_FRAME_HDR **frm_pptr)
{
    return resp_conversion_generic_cfm(msg, frm_pptr, SCMD_ADD_STA_REQ);
}

UINT32 resp_conversion_reset_cfm(struct ke_msg *msg, STM32_FRAME_HDR **frm_pptr)
{
    return resp_conversion_generic_cfm(msg, frm_pptr, SCMD_RESET_REQ);
}

UINT32 resp_conversion_me_config_cfm(struct ke_msg *msg, STM32_FRAME_HDR **frm_pptr)
{
    return resp_conversion_generic_cfm(msg, frm_pptr, SCMD_ME_CONFIG_REQ);
}

UINT32 resp_conversion_me_chan_config_cfm(struct ke_msg *msg, STM32_FRAME_HDR **frm_pptr)
{
    return resp_conversion_generic_cfm(msg, frm_pptr, SCMD_ME_CHAN_CONFIG_REQ);
}

UINT32 resp_conversion_start_cfm(struct ke_msg *msg, STM32_FRAME_HDR **frm_pptr)
{
    return resp_conversion_generic_cfm(msg, frm_pptr, SCMD_START_REQ);
}

UINT32 resp_conversion_add_if_cfm(struct ke_msg *msg, STM32_FRAME_HDR **frm_pptr)
{
    return resp_conversion_generic_cfm(msg, frm_pptr, SCMD_ADD_IF_REQ);
}

UINT32 resp_conversion_set_port_cfm(struct ke_msg *msg, STM32_FRAME_HDR **frm_pptr)
{
    return resp_conversion_generic_cfm(msg, frm_pptr, SCMD_SET_PORT_REQ);
}

UINT32 resp_conversion_key_add_cfm(struct ke_msg *msg, STM32_FRAME_HDR **frm_pptr)
{
    return resp_conversion_generic_cfm(msg, frm_pptr, SCMD_802_11_KEY_MATERIAL);
}

UINT32 resp_conversion_connect_ind(struct ke_msg *msg, STM32_FRAME_HDR **frm_pptr)
{
    UINT32 len;
    STM32_FRAME_HDR *frm;
    ASSOC_RESP_PTR assoc_rsp_ptr;
    struct sm_connect_indication *conn_ind_ptr;
#if CFG_REAL_SDIO
    len = sizeof(STM32_FRAME_HDR) + sizeof(STM32_CMD_HDR_S) + msg->param_len;
#else
    len = sizeof(STM32_FRAME_HDR) + sizeof(ASSOC_RESP_S);
#endif
    frm = (STM32_FRAME_HDR *)os_malloc(len);
    ASSERT(frm);

    frm->len = len;
    frm->type = MVMS_CMD;
    *frm_pptr = frm;

    conn_ind_ptr = (struct sm_connect_indication *)msg->param;
    SDIO_INTF_PRT("connect_ind:%x\r\n", conn_ind_ptr->status_code);

    assoc_rsp_ptr = (ASSOC_RESP_PTR)((UINT32)frm + sizeof(STM32_FRAME_HDR));
    assoc_rsp_ptr->hdr.command = CMD_RET(SCMD_802_11_ASSOCIATE);

#if CFG_REAL_SDIO
    assoc_rsp_ptr->hdr.result  = conn_ind_ptr->status_code;
#else
    assoc_rsp_ptr->hdr.result  = 0;
#endif
    assoc_rsp_ptr->hdr.seqnum  = resp_inc_seqnum();
    assoc_rsp_ptr->hdr.size    = len - sizeof(STM32_FRAME_HDR);
#if CFG_REAL_SDIO
    os_memcpy((void *)((UINT32)assoc_rsp_ptr + sizeof(STM32_CMD_HDR_S)), msg->param, msg->param_len);
#else
    assoc_rsp_ptr->statuscode = conn_ind_ptr->status_code;
#endif
    return len;
}

UINT32 resp_conversion_scanu_cfm(struct ke_msg *msg, STM32_FRAME_HDR **frm_pptr)
{
    UINT32 len;
    STM32_FRAME_HDR *frm;
    STM32_SCAN_RSP_PTR stm32_scan_rsp_ptr;
#if CFG_REAL_SDIO
    len = sizeof(STM32_FRAME_HDR) + sizeof(STM32_CMD_HDR_S);
#else
    len = sizeof(STM32_FRAME_HDR) + sizeof(STM32_SCAN_RSP_S);
#endif

    frm = (STM32_FRAME_HDR *)os_malloc(len);
    ASSERT(frm);

    frm->len = len;
    frm->type = MVMS_CMD;
    *frm_pptr = frm;

    stm32_scan_rsp_ptr = (STM32_SCAN_RSP_PTR)((UINT32)frm + sizeof(STM32_FRAME_HDR));
    stm32_scan_rsp_ptr->hdr.command = CMD_RET(SCMD_802_11_SCAN);
    stm32_scan_rsp_ptr->hdr.result  = 0;
    stm32_scan_rsp_ptr->hdr.seqnum  = scan_resp_cmd_sn;
#if CFG_REAL_SDIO
    stm32_scan_rsp_ptr->hdr.size    = 0;
#else
    stm32_scan_rsp_ptr->hdr.size    = len - sizeof(STM32_FRAME_HDR);
    stm32_scan_rsp_ptr->bssdescriptsize = sizeof(BEACON_INFO_S);
    stm32_scan_rsp_ptr->nr_sets      = 0;
#endif
    return len;
}

UINT32 resp_conversion_scanu_ret(struct ke_msg *msg, STM32_FRAME_HDR **frm_pptr)
{
    UINT32 len;
    STM32_FRAME_HDR *frm;
    SCANU_RET_IND_PTR scanu_ret_ptr;
    STM32_SCAN_RSP_PTR stm32_scan_rsp_ptr;
    IEEE80211_PROBE_RSP_PTR probe_rsp_ieee80211_ptr;
#if (! CFG_REAL_SDIO)
    UINT32 vies_len;
    BEACON_INFO_PTR beacon_ptr;
    UINT8 *var_part_addr;
    UINT8 *elmt_addr;
    char ssid[MAC_SSID_LEN + 1];
#endif

    scanu_ret_ptr = (SCANU_RET_IND_PTR)msg->param;
    SDIO_INTF_PRT("____center:%d\r\n", scanu_ret_ptr->center_freq);
    SDIO_INTF_PRT("____band:%d\r\n", scanu_ret_ptr->band);
    SDIO_INTF_PRT("____rssi:%d\r\n", scanu_ret_ptr->rssi);

    probe_rsp_ieee80211_ptr = (IEEE80211_PROBE_RSP_PTR)scanu_ret_ptr->payload;
    SDIO_INTF_PRT("____bssid:%x:%x:%x:%x:%x:%x\r\n", probe_rsp_ieee80211_ptr->bssid[0]
                  , probe_rsp_ieee80211_ptr->bssid[1]
                  , probe_rsp_ieee80211_ptr->bssid[2]
                  , probe_rsp_ieee80211_ptr->bssid[3]
                  , probe_rsp_ieee80211_ptr->bssid[4]
                  , probe_rsp_ieee80211_ptr->bssid[5]);

#if CFG_REAL_SDIO
    len = sizeof(STM32_FRAME_HDR) + sizeof(STM32_CMD_HDR_S)
          + msg->param_len;
#else
    vies_len = scanu_ret_ptr->length - (sizeof(SCANU_RET_IND_S) - (SCANU_RET_PAYLOAD_LEN << 2))
               - (sizeof(IEEE80211_PROBE_RSP_S) - 1) + 10;
    len = sizeof(STM32_FRAME_HDR) + sizeof(STM32_SCAN_RSP_S) - 1
          + sizeof(BEACON_INFO_S)
          + vies_len;

    var_part_addr = probe_rsp_ieee80211_ptr->rsp.variable;
    elmt_addr = (UINT8 *)mac_ie_find((UINT32)var_part_addr, (UINT16)vies_len, MAC_ELTID_SSID);
    if (elmt_addr)
    {
        UINT8 ssid_len = *(elmt_addr + MAC_SSID_LEN_OFT);
        if (ssid_len > MAC_SSID_LEN)
            ssid_len = MAC_SSID_LEN;

        os_memcpy(&ssid[0], elmt_addr + MAC_SSID_SSID_OFT, ssid_len);
        ssid[ssid_len] = '\0';
        SDIO_INTF_PRT("____ssid:%s\r\n", ssid);
    }
#endif
    SDIO_INTF_PRT("\r\n");

    frm = (STM32_FRAME_HDR *)os_malloc(len);
    ASSERT(frm);

    *frm_pptr = frm;

    frm->len = len;
    frm->type = MVMS_CMD;

    stm32_scan_rsp_ptr = (STM32_SCAN_RSP_PTR)((UINT32)frm + sizeof(STM32_FRAME_HDR));
    stm32_scan_rsp_ptr->hdr.command = CMD_RET(SCMD_802_11_SCAN);
    stm32_scan_rsp_ptr->hdr.result  = 0;
    stm32_scan_rsp_ptr->hdr.seqnum  = scan_resp_cmd_sn;
#if CFG_REAL_SDIO
    stm32_scan_rsp_ptr->hdr.size    = msg->param_len;
    os_memcpy((void *)((UINT32)stm32_scan_rsp_ptr + sizeof(STM32_CMD_HDR_S)) , msg->param, msg->param_len);
#else
    stm32_scan_rsp_ptr->hdr.size    = len - sizeof(STM32_FRAME_HDR);
    stm32_scan_rsp_ptr->bssdescriptsize = sizeof(BEACON_INFO_S) + vies_len;
    stm32_scan_rsp_ptr->nr_sets     = 1;

    beacon_ptr = &stm32_scan_rsp_ptr->beacon_info;
    beacon_ptr->len = sizeof(BEACON_INFO_S) + vies_len - 2; // 2-->len
    os_memcpy(beacon_ptr->bssid, probe_rsp_ieee80211_ptr->bssid, ETH_ALEN);
    beacon_ptr->rssi = scanu_ret_ptr->rssi;
    os_memcpy(beacon_ptr->timestamp, probe_rsp_ieee80211_ptr->rsp.timestamp, BEACON_TIMESTAMP_LEN);
    beacon_ptr->beacon_interval = probe_rsp_ieee80211_ptr->rsp.beacon_int;
    beacon_ptr->capability = probe_rsp_ieee80211_ptr->rsp.capab_info;

    os_memcpy(stm32_scan_rsp_ptr->tlvbuffer, probe_rsp_ieee80211_ptr->rsp.variable, vies_len);
#endif

    return len;
}

UINT32 resp_conversion_sm_disconnect_ind(struct ke_msg *msg,
        STM32_FRAME_HDR **frm_pptr)
{
    struct sm_disconnect_ind *disc = (struct sm_disconnect_ind *)msg->param;

    *frm_pptr = NULL;
    SDIO_INTF_PRT("reason:%d, vif_idx:%d\r\n", disc->reason_code, disc->vif_idx);

#if CFG_WIFI_STATION_MODE
    SDIO_INTF_PRT("exit all application process\r\n");
    SDIO_INTF_PRT("polling sa_station_process to scan and associate\r\n");
    sa_station_set_reconnect_timer();
#endif

    return 0;
}

UINT32 sdio_resp_conversion(struct ke_msg *msg, STM32_FRAME_HDR **frm_pptr)
{
    UINT32 len = 0;

    switch (msg->id)
    {
    case ME_MGMT_TX_CFM:
        SDIO_INTF_PRT("mgmt_tx_cfm\r\n");
        len = resp_conversion_mgmt_tx_cfm(msg, frm_pptr);
        break;

    case MM_KEY_ADD_CFM:
        SDIO_INTF_PRT("mm_key_add_cfm\r\n");
        len = resp_conversion_key_add_cfm(msg, frm_pptr);
        break;

    case RXU_MGT_IND:
        SDIO_INTF_PRT("rxu_mgt_ind\r\n");
        len = resp_conversion_rxu_mgt_ind(msg, frm_pptr);
        break;

    case ME_STA_ADD_CFM:
        SDIO_INTF_PRT("add_station_cfm\r\n");
        len = resp_conversion_add_sta_cfm(msg, frm_pptr);
        break;

    case MM_BCN_CHANGE_CFM:
        SDIO_INTF_PRT("bcn_change_cfm\r\n");
        len = resp_conversion_bcn_change_cfm(msg, frm_pptr);
        break;

    case APM_START_CFM:
        SDIO_INTF_PRT("apm_start_cfm\r\n");
        len = resp_conversion_start_apm_cfm(msg, frm_pptr);
        break;

    case SCANU_RESULT_IND:
        SDIO_INTF_PRT("scanu-result-ind\r\n");
        len = resp_conversion_scanu_ret(msg, frm_pptr);
        break;

    case SCANU_START_CFM:
        scan_start_flag = 0;
        SDIO_INTF_PRT("scanu-start-cfm\r\n");
        len = resp_conversion_scanu_cfm(msg, frm_pptr);
        break;

    case MM_RESET_CFM:
        SDIO_INTF_PRT("mm-reset-cfm\r\n");
        len = resp_conversion_reset_cfm(msg, frm_pptr);
        break;

    case ME_CONFIG_CFM:
        SDIO_INTF_PRT("me-config-cfm\r\n");
        len = resp_conversion_me_config_cfm(msg, frm_pptr);
        break;

    case ME_CHAN_CONFIG_CFM:
        SDIO_INTF_PRT("me-channel-config-cfm\r\n");
        len = resp_conversion_me_chan_config_cfm(msg, frm_pptr);
        break;

    case MM_START_CFM:
        SDIO_INTF_PRT("mm-start-cfm\r\n");
        len = resp_conversion_start_cfm(msg, frm_pptr);
        break;

    case MM_ADD_IF_CFM:
        SDIO_INTF_PRT("mm-add-if-cfm\r\n");
        len = resp_conversion_add_if_cfm(msg, frm_pptr);
        break;

    case SM_CONNECT_CFM:
        SDIO_INTF_PRT("sm-connect-cfm\r\n");
        break;

    case SM_CONNECT_IND:
        SDIO_INTF_PRT("sm-connect-ind\r\n");
        len = resp_conversion_connect_ind(msg, frm_pptr);
        break;

    case ME_SET_CONTROL_PORT_CFM:
        SDIO_INTF_PRT("mm-set-control-port-cfm\r\n");
        len = resp_conversion_set_port_cfm(msg, frm_pptr);
        break;

    case MM_CHANNEL_SWITCH_IND:
        SDIO_INTF_PRT("mm-channel-switch-ind\r\n");
        break;

    case MM_PS_CHANGE_IND:
        break;

    case SM_DISCONNECT_IND:
        SDIO_INTF_PRT("sm-disconnect-ind\r\n");
        len = resp_conversion_sm_disconnect_ind(msg, frm_pptr);
        break;

    default:
        SDIO_INTF_PRT("resp-unkown\r\n");
        break;
    }

    return len;
}

UINT32 sdio_emb_kmsg_fwd(struct ke_msg *msg)
{
    UINT32 ret;
    UINT32 len;
    UINT32 status;
    UINT32 wr_sta;
    DD_HANDLE sdio_hdl;
    STM32_FRAME_HDR *frm_ptr;

    ret = SDIO_INTF_FAILURE;

    /*prepare for response content*/
    len = sdio_resp_conversion(msg, &frm_ptr);

    ke_msg_free(msg);

    if(0 == len)
    {
        goto tx_ok;
    }

    /*forward the new message*/
    sdio_hdl = ddev_open(SDIO_DEV_NAME, &status, 0);
    if(DD_HANDLE_UNVALID == sdio_hdl)
    {
        goto tx_exception;
    }

    wr_sta = ddev_write(sdio_hdl, (char *)frm_ptr, len, S2H_WR_SYNC);
    if(SDIO_FAILURE == wr_sta)
    {
        goto tx_exception;
    }

    if(frm_ptr)
    {
        os_free(frm_ptr);
    }

tx_ok:
    ret = SDIO_INTF_SUCCESS;

tx_exception:

    return ret;
}
#endif

#if 2
#if CFG_WIFI_AP_MODE
UINT8 chan_connect = CFG_CHANNEL_AP;
#else
UINT8 chan_connect = 10;
#endif

UINT32 ie_info[32] = {0x500016dd, 0x101f2, 0x2f25000, 0x50000001, 0x102f2, 0x2f25000};

void cmd_conversion_add_wep_key(STM32_FRAME_HDR *frm_ptr, struct ke_msg **kmsg_pptr)
{
    extern int hexstr2bin(const char * hex, u8 * buf, size_t len);

    UINT32 param_len;
    struct ke_msg *kmsg_dst;
    struct mm_key_add_req *add_wep_key_ptr;

    param_len = sizeof(struct mm_key_add_req);
    kmsg_dst = (struct ke_msg *)os_malloc(sizeof(struct ke_msg)
                                          + param_len);
    ASSERT(kmsg_dst);
    os_memset(kmsg_dst, 0, (sizeof(struct ke_msg) + param_len));

    kmsg_dst->id = MM_KEY_ADD_REQ;
    kmsg_dst->dest_id = TASK_MM;
    kmsg_dst->src_id  = DRV_TASK_ID;
    kmsg_dst->param_len = param_len;

    *kmsg_pptr = kmsg_dst;
    add_wep_key_ptr = (struct mm_key_add_req *)kmsg_dst->param;

    add_wep_key_ptr->cipher_suite = MAC_RSNIE_CIPHER_WEP40;
    add_wep_key_ptr->sta_idx = 0xff;
    add_wep_key_ptr->inst_nbr = 0;

    add_wep_key_ptr->key.length = os_strlen(CFG_WEP_KEY);
    hexstr2bin(CFG_WEP_KEY, (u8 *)&add_wep_key_ptr->key.array[0], add_wep_key_ptr->key.length);

    add_wep_key_ptr->key_idx = 0;
    add_wep_key_ptr->spp = 0;

    return;
}

void cmd_conversion_start_apm(STM32_FRAME_HDR *frm_ptr, struct ke_msg **kmsg_pptr)
{
    UINT32 param_len;
    struct ke_msg *kmsg_dst;
    struct apm_start_req *start_req_ptr;

    param_len = sizeof(struct apm_start_req);
    kmsg_dst = (struct ke_msg *)os_malloc(sizeof(struct ke_msg)
                                          + param_len);
    ASSERT(kmsg_dst);
    os_memset(kmsg_dst, 0, (sizeof(struct ke_msg) + param_len));

    kmsg_dst->id = APM_START_REQ;
    kmsg_dst->dest_id = TASK_APM;
    kmsg_dst->src_id  = DRV_TASK_ID;
    kmsg_dst->param_len = param_len;

    *kmsg_pptr = kmsg_dst;
    start_req_ptr = (struct apm_start_req *)kmsg_dst->param;
    start_req_ptr->basic_rates.length = 4;
    start_req_ptr->basic_rates.array[0] = 130;
    start_req_ptr->basic_rates.array[1] = 132;
    start_req_ptr->basic_rates.array[2] = 139;
    start_req_ptr->basic_rates.array[3] = 150;
    start_req_ptr->chan.band = 0;
    start_req_ptr->chan.freq = freq_2_4_G[chan_connect - 1];
    start_req_ptr->chan.flags = 0;
    start_req_ptr->center_freq1 = freq_2_4_G[chan_connect - 1];
    start_req_ptr->center_freq2 = 0;
    start_req_ptr->ch_width = 0;
    start_req_ptr->bcn_addr = (UINT32)beacon;
    start_req_ptr->bcn_len = sizeof(beacon);
    start_req_ptr->tim_oft = 56;
    start_req_ptr->tim_len = 6;
    start_req_ptr->bcn_int = 100;
#if CFG_WIFI_WPA
    start_req_ptr->flags = 8;
#else
    start_req_ptr->flags = 0;
#endif
    start_req_ptr->ctrl_port_ethertype = 36488;
    start_req_ptr->vif_idx = 0;

    return;
}

void cmd_conversion_beacon_change(STM32_FRAME_HDR *frm_ptr, struct ke_msg **kmsg_pptr)
{
    UINT32 param_len;
    struct ke_msg *kmsg_dst;
    struct mm_bcn_change_req *bcn_change_ptr;

    param_len = sizeof(struct mm_bcn_change_req);
    kmsg_dst = (struct ke_msg *)os_malloc(sizeof(struct ke_msg)
                                          + param_len);
    ASSERT(kmsg_dst);
    os_memset(kmsg_dst, 0, (sizeof(struct ke_msg) + param_len));

    kmsg_dst->id = MM_BCN_CHANGE_REQ;
    kmsg_dst->dest_id = TASK_MM;
    kmsg_dst->src_id  = DRV_TASK_ID;
    kmsg_dst->param_len = param_len;

    *kmsg_pptr = kmsg_dst;
    bcn_change_ptr = (struct mm_bcn_change_req *)kmsg_dst->param;
    bcn_change_ptr->bcn_ptr = (UINT32)beacon;
    bcn_change_ptr->bcn_len = sizeof(beacon);
    bcn_change_ptr->tim_len = 6;
    bcn_change_ptr->tim_oft = 56;

    return;
}

/* hard code: dest addr, src addr, ap ssid, ap mac addr*/
UINT8 auth_rsp[] =
{
    0xb0, 0x00, 0x40, 0x01, 0x48, 0x5a, 0xb6, 0xc1
    , 0xea, 0xe1, 0x12, 0x71, 0x11, 0x71, 0x11
    , 0x71, 0x12, 0x71, 0x11, 0x71, 0x11, 0x71
    , 0xe0, 0x04,
    0, 0, 2, 0, 0, 0
};
UINT8 assoc_rsp[] =
{
    0x10, 0x00, 0x40, 0x01, 0x48, 0x5a, 0xb6, 0xc1
    , 0xea, 0xe1, 0x12, 0x71, 0x11, 0x71, 0x11
    , 0x71, 0x12, 0x71, 0x11, 0x71, 0x11, 0x71
    , 0xf0, 0x04,
    0x21, 0x04, 0x00, 0x00, 0x01, 0xc0, 0x01, 0x08
    , 0x82, 0x84, 0x8B, 0x96, 0x0C, 0x12, 0x18, 0x24
    , 0x32, 0x04, 0x30, 0x48, 0x60, 0x6C, 0x2D, 0x1A
    , 0x0C, 0x00, 0x1B, 0xFF, 0x00, 0x00, 0x00, 0x01
    , 0x00, 0x00, 0x00, 0x00, 0x00, 0x96, 0x00, 0x01
    , 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    , 0x00, 0x00, 0x3D, 0x16, 0x08, 0x00, 0x04, 0x00
    , 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    , 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    , 0x00, 0x00, 0xDD, 0x18, 0x00, 0x50, 0xF2, 0x02
    , 0x01, 0x01, 0x00, 0x00, 0x03, 0xA4, 0x00, 0x00
    , 0x27, 0xA4, 0x00, 0x00, 0x42, 0x43, 0x5E, 0x00
    , 0x62, 0x32, 0x2F, 0x00
};

void cmd_conversion_mgmt_tx_req(STM32_FRAME_HDR *frm_ptr, struct ke_msg **kmsg_pptr)
{
    UINT32 param_len;
    struct ke_msg *kmsg_dst;
    STM32_TX_MGMT_PTR tx_mgmt_ptr;
    struct me_mgmt_tx_req *mgmt_tx_ptr;

    param_len = sizeof(struct me_mgmt_tx_req);
    kmsg_dst = (struct ke_msg *)os_malloc(sizeof(struct ke_msg)
                                          + param_len);
    ASSERT(kmsg_dst);
    os_memset(kmsg_dst, 0, (sizeof(struct ke_msg) + param_len));

    kmsg_dst->id = ME_MGMT_TX_REQ;
    kmsg_dst->dest_id = TASK_ME;
    kmsg_dst->src_id  = DRV_TASK_ID;
    kmsg_dst->param_len = param_len;

    *kmsg_pptr = kmsg_dst;
    mgmt_tx_ptr = (struct me_mgmt_tx_req *)kmsg_dst->param;

    tx_mgmt_ptr = (STM32_TX_MGMT_PTR)frm_ptr;
    if(SCMD_802_11_AUTHENTICATE == tx_mgmt_ptr->private)
    {
        mgmt_tx_ptr->addr = (UINT32)auth_rsp;
        mgmt_tx_ptr->hostid = (UINT32)auth_rsp;
        mgmt_tx_ptr->len = sizeof(auth_rsp);
    }
    else if(SCMD_802_11_ASSOCIATE == tx_mgmt_ptr->private)
    {
        mgmt_tx_ptr->addr = (UINT32)assoc_rsp;
        mgmt_tx_ptr->hostid = (UINT32)assoc_rsp;
        mgmt_tx_ptr->len = sizeof(assoc_rsp);
    }

    return;
}


void cmd_conversion_add_sta_req(STM32_FRAME_HDR *frm_ptr, struct ke_msg **kmsg_pptr)
{
    UINT32 param_len;
    struct ke_msg *kmsg_dst;
    struct me_sta_add_req *sta_add_ptr;

    param_len = sizeof(struct me_sta_add_req);
    kmsg_dst = (struct ke_msg *)os_malloc(sizeof(struct ke_msg)
                                          + param_len);
    ASSERT(kmsg_dst);
    os_memset(kmsg_dst, 0, (sizeof(struct ke_msg) + param_len));

    kmsg_dst->id = ME_STA_ADD_REQ;
    kmsg_dst->dest_id = TASK_ME;
    kmsg_dst->src_id  = DRV_TASK_ID;
    kmsg_dst->param_len = param_len;

    *kmsg_pptr = kmsg_dst;
    sta_add_ptr = (struct me_sta_add_req *)kmsg_dst->param;

    sta_add_ptr->vif_idx = 0;
    sta_add_ptr->aid = 1;
    sta_add_ptr->flags = 0x03;

    sta_add_ptr->rate_set.length = 12;
    sta_add_ptr->rate_set.array[0] = 130;
    sta_add_ptr->rate_set.array[1] = 132;
    sta_add_ptr->rate_set.array[2] = 139;
    sta_add_ptr->rate_set.array[3] = 150;
    sta_add_ptr->rate_set.array[4] = 108;
    sta_add_ptr->rate_set.array[5] = 36;
    sta_add_ptr->rate_set.array[6] = 48;
    sta_add_ptr->rate_set.array[7] = 72;
    sta_add_ptr->rate_set.array[8] = 12;
    sta_add_ptr->rate_set.array[9] = 18;
    sta_add_ptr->rate_set.array[10] = 24;
    sta_add_ptr->rate_set.array[11] = 96;

    sta_add_ptr->ht_cap.mcs_rate[0] = 255;

    return;
}

void cmd_conversion_reset_req(STM32_FRAME_HDR *frm_ptr, struct ke_msg **kmsg_pptr)
{
    UINT32 param_len;
    struct ke_msg *kmsg_dst;

    param_len = 0;
    kmsg_dst = (struct ke_msg *)os_malloc(sizeof(struct ke_msg)
                                          + param_len);
    ASSERT(kmsg_dst);
    os_memset(kmsg_dst, 0, (sizeof(struct ke_msg) + param_len));

    kmsg_dst->id = MM_RESET_REQ;
    kmsg_dst->dest_id = TASK_MM;
    kmsg_dst->src_id  = TASK_API;
    kmsg_dst->param_len = param_len;

    *kmsg_pptr = kmsg_dst;

    return;
}

void cmd_conversion_me_config_req(STM32_FRAME_HDR *frm_ptr, struct ke_msg **kmsg_pptr)
{
    UINT32 param_len;
    struct ke_msg *kmsg_dst;

    param_len = sizeof(struct me_config_req);
    kmsg_dst = (struct ke_msg *)os_malloc(sizeof(struct ke_msg)
                                          + param_len);
    ASSERT(kmsg_dst);
    os_memset(kmsg_dst, 0, (sizeof(struct ke_msg) + param_len));

    kmsg_dst->id = ME_CONFIG_REQ;
    kmsg_dst->dest_id = TASK_ME;
    kmsg_dst->src_id  = TASK_API;
    kmsg_dst->param_len = param_len;

    *kmsg_pptr = kmsg_dst;

    return;
}

void cmd_conversion_me_chan_config_req(STM32_FRAME_HDR *frm_ptr, struct ke_msg **kmsg_pptr)
{
    UINT32 i;
    UINT32 param_len;
    struct ke_msg *kmsg_dst;
    struct me_chan_config_req *me_chan_cfg_ptr;

    param_len = sizeof(struct me_chan_config_req);
    kmsg_dst = (struct ke_msg *)os_malloc(sizeof(struct ke_msg)
                                          + param_len);
    ASSERT(kmsg_dst);
    os_memset(kmsg_dst, 0, (sizeof(struct ke_msg) + param_len));

    kmsg_dst->id = ME_CHAN_CONFIG_REQ;
    kmsg_dst->dest_id = TASK_ME;
    kmsg_dst->src_id  = TASK_API;
    kmsg_dst->param_len = param_len;

    *kmsg_pptr = kmsg_dst;

    me_chan_cfg_ptr = (struct me_chan_config_req *)kmsg_dst->param;

    for(i = 0; i < WIFI_2_4_G_CHANNEL_NUM; i ++)
    {
        me_chan_cfg_ptr->chan2G4[i].band = WIFI_2_4_G_BAND;
        me_chan_cfg_ptr->chan2G4[i].flags = 0;
        me_chan_cfg_ptr->chan2G4[i].freq = freq_2_4_G[i];
    }

    me_chan_cfg_ptr->chan2G4_cnt = WIFI_2_4_G_CHANNEL_NUM;
    me_chan_cfg_ptr->chan5G_cnt = 0;

    return;
}
void cmd_conversion_set_port_req(STM32_FRAME_HDR *frm_ptr, struct ke_msg **kmsg_pptr)
{
    UINT32 param_len;
    struct ke_msg *kmsg_dst;
    struct me_set_control_port_req *set_port_ptr;

    param_len = sizeof(struct me_set_control_port_req);
    kmsg_dst = (struct ke_msg *)os_malloc(sizeof(struct ke_msg)
                                          + param_len);
    ASSERT(kmsg_dst);
    os_memset(kmsg_dst, 0, (sizeof(struct ke_msg) + param_len));

    kmsg_dst->id = ME_SET_CONTROL_PORT_REQ;
    kmsg_dst->dest_id = TASK_ME;
    kmsg_dst->src_id  = TASK_API;
    kmsg_dst->param_len = param_len;

    *kmsg_pptr = kmsg_dst;

    set_port_ptr = (struct me_set_control_port_req *)kmsg_dst->param;
    set_port_ptr->control_port_open = 1;
    set_port_ptr->sta_idx = 0;

    return;
}

void cmd_conversion_start_req(STM32_FRAME_HDR *frm_ptr, struct ke_msg **kmsg_pptr)
{
    UINT32 param_len;
    struct ke_msg *kmsg_dst;
    struct mm_start_req *mm_start_ptr;

    param_len = sizeof(struct mm_start_req);
    kmsg_dst = (struct ke_msg *)os_malloc(sizeof(struct ke_msg)
                                          + param_len);
    ASSERT(kmsg_dst);
    os_memset(kmsg_dst, 0, (sizeof(struct ke_msg) + param_len));

    kmsg_dst->id = MM_START_REQ;
    kmsg_dst->dest_id = TASK_MM;
    kmsg_dst->src_id  = TASK_API;
    kmsg_dst->param_len = param_len;

    *kmsg_pptr = kmsg_dst;

    mm_start_ptr = (struct mm_start_req *)kmsg_dst->param;

    mm_start_ptr->phy_cfg.parameters[0] = 1;
    mm_start_ptr->phy_cfg.parameters[1] = 0;
    mm_start_ptr->uapsd_timeout = 300;

    return;
}

void cmd_conversion_assoc_req(STM32_FRAME_HDR *frm_ptr, struct ke_msg **kmsg_pptr)
{
    UINT32 param_len;
    struct ke_msg *kmsg_dst;
    ASSOC_REQ_PTR asso_ptr;
    struct sm_connect_req *sm_connect_req_ptr;
#if CFG_REAL_SDIO
    char *temp;
#endif

    asso_ptr = (ASSOC_REQ_PTR)&frm_ptr[1];

    param_len = sizeof(struct sm_connect_req);
    kmsg_dst  = (struct ke_msg *)os_malloc(sizeof(struct ke_msg)
                                           + param_len);
    ASSERT(kmsg_dst);
    os_memset(kmsg_dst, 0, (sizeof(struct ke_msg) + param_len));

    kmsg_dst->id        = SM_CONNECT_REQ;
    kmsg_dst->dest_id   = TASK_SM;
    kmsg_dst->src_id    = TASK_API;
    kmsg_dst->param_len = param_len;

    *kmsg_pptr = kmsg_dst;
    sm_connect_req_ptr = (struct sm_connect_req *)kmsg_dst->param;
#if CFG_REAL_SDIO
    temp = (char *)((UINT32)asso_ptr + sizeof(STM32_CMD_HDR_S));
    os_memcpy((char *)&sm_connect_req_ptr->ssid, temp, sizeof(struct mac_ssid));
    os_memcpy((char *)&sm_connect_req_ptr->bssid,
              temp + sizeof(struct mac_ssid) + 1, sizeof(struct sm_connect_req) - sizeof(struct mac_ssid));

#if CFG_WIFI_WEP
    sm_connect_req_ptr->auth_type = MAC_AUTH_ALGO_SHARED;
#else
    sm_connect_req_ptr->auth_type = MAC_AUTH_ALGO_OPEN;
#endif
#else
#if (0 == CFG_WIFI_AP_MODE)
    sm_connect_req_ptr->ssid.length = os_strlen((const char *)CFG_OOB_CONNECT_SSID);
    os_memcpy(sm_connect_req_ptr->ssid.array, CFG_OOB_CONNECT_SSID, sm_connect_req_ptr->ssid.length);
#endif

    os_memcpy(&sm_connect_req_ptr->bssid, &asso_ptr->bssid, sizeof(asso_ptr->bssid));

    sm_connect_req_ptr->chan.band = 0;
    sm_connect_req_ptr->chan.flags = 0;
    sm_connect_req_ptr->chan.freq = freq_2_4_G[chan_connect - 1];
    sm_connect_req_ptr->flags = 1; // wpa 5
    sm_connect_req_ptr->ctrl_port_ethertype = 36488;
    sm_connect_req_ptr->ie_len = 0; // wpa 24

#if CFG_WIFI_WEP
    sm_connect_req_ptr->auth_type = MAC_AUTH_ALGO_SHARED | MAC_AUTH_ALGO_OPEN;
#else
    sm_connect_req_ptr->auth_type = MAC_AUTH_ALGO_OPEN;
#endif

    sm_connect_req_ptr->vif_idx = 0;
    os_memcpy(sm_connect_req_ptr->ie_buf, ie_info, sm_connect_req_ptr->ie_len);
#endif
    return;
}

void cmd_conversion_key_material_req(STM32_FRAME_HDR *frm_ptr, struct ke_msg **kmsg_pptr)
{
    UINT32 param_len;
    struct ke_msg *kmsg_dst;
    struct mm_key_add_req *key_add_req_ptr;
    STM32_KEY_MATERIAL_PTR key_material_ptr;

    key_material_ptr = (STM32_KEY_MATERIAL_PTR)&frm_ptr[1];

    param_len = sizeof(struct mm_key_add_req);
    kmsg_dst  = (struct ke_msg *)os_malloc(sizeof(struct ke_msg)
                                           + param_len);
    ASSERT(kmsg_dst);
    os_memset(kmsg_dst, 0, (sizeof(struct ke_msg) + param_len));

    kmsg_dst->id        = MM_KEY_ADD_REQ;
    kmsg_dst->dest_id   = TASK_MM;
    kmsg_dst->src_id    = TASK_API;
    kmsg_dst->param_len = param_len;

    *kmsg_pptr = kmsg_dst;
    key_add_req_ptr = (struct mm_key_add_req *)kmsg_dst->param;

    if(key_material_ptr->action)
    {
        key_add_req_ptr->sta_idx = 255;
    }
    else
    {
        key_add_req_ptr->sta_idx = 0;
    }

    key_add_req_ptr->key_idx = 0;
    key_add_req_ptr->key.length = os_strlen(CFG_WEP_KEY);
    hexstr2bin(CFG_WEP_KEY, (u8 *)&key_add_req_ptr->key.array[0], key_add_req_ptr->key.length);
    key_add_req_ptr->cipher_suite = 1;
    key_add_req_ptr->inst_nbr = 0;
    key_add_req_ptr->spp = 0;

    return;
}

void cmd_conversion_add_if_req(STM32_FRAME_HDR *frm_ptr, struct ke_msg **kmsg_pptr)
{
    UINT32 param_len;
    struct ke_msg *kmsg_dst;
    struct mm_add_if_req *mm_add_if_ptr;

    param_len = sizeof(struct mm_add_if_req);
    kmsg_dst = (struct ke_msg *)os_malloc(sizeof(struct ke_msg)
                                          + param_len);
    ASSERT(kmsg_dst);
    os_memset(kmsg_dst, 0, (sizeof(struct ke_msg) + param_len));

    kmsg_dst->id = MM_ADD_IF_REQ;
    kmsg_dst->dest_id = TASK_MM;
    kmsg_dst->src_id  = TASK_API;
    kmsg_dst->param_len = param_len;

    *kmsg_pptr = kmsg_dst;

    mm_add_if_ptr = (struct mm_add_if_req *)kmsg_dst->param;

#if CFG_WIFI_AP_MODE
    SDIO_INTF_PRT("vif_ap\r\n");
    mm_add_if_ptr->type = VIF_AP;
#else
    SDIO_INTF_PRT("vif_sta\r\n");
    mm_add_if_ptr->type = VIF_STA;
#endif

    return;
}

void cmd_conversion_802_11_scan(STM32_FRAME_HDR *frm_ptr, struct ke_msg **kmsg_pptr)
{
    UINT32 param_len;
    struct ke_msg *kmsg_dst;
    STM32_CMD_HDR_PTR scmd_hdr_ptr;
    struct scan_start_req *scan_start_ptr;
#if (! CFG_REAL_SDIO)
    UINT32 ssid_id;
    UINT32 channel_id;
    UINT32 ie_total_len;
    MRVL_IE_HDR_PTR mrvl_ie_ptr;
    STM32_CMD_802_11_SCAN_PTR scan_cmd_ptr;
#endif

    kmsg_dst = NULLPTR;
    scmd_hdr_ptr = (STM32_CMD_HDR_PTR)&frm_ptr[1];

    param_len = sizeof(struct scan_start_req);
    kmsg_dst = (struct ke_msg *)os_malloc(sizeof(struct ke_msg)
                                          + param_len);
    ASSERT(kmsg_dst);
    os_memset(kmsg_dst, 0, (sizeof(struct ke_msg) + param_len));

    /*directly set value*/
    kmsg_dst->id        = SCANU_START_REQ;
    kmsg_dst->dest_id   = TASK_SCANU;
    kmsg_dst->param_len = param_len;

    kmsg_dst->hdr.next  = NULL;
    kmsg_dst->src_id    = TASK_API;

#if (! CFG_REAL_SDIO)
    /* conversion*/
    ssid_id = 0;
    channel_id = 0;

    scan_start_ptr = (struct scan_start_req *)kmsg_dst->param;
    scan_cmd_ptr = (STM32_CMD_802_11_SCAN_PTR)scmd_hdr_ptr;
    mrvl_ie_ptr  = (MRVL_IE_HDR_PTR)scan_cmd_ptr->tlvbuffer;
    ie_total_len =  scmd_hdr_ptr->size - ((UINT32)mrvl_ie_ptr - (UINT32)scmd_hdr_ptr);

    while(ie_total_len)
    {
        ie_total_len -= (mrvl_ie_ptr->len + sizeof(MRVL_IE_HDR_S));
        switch(mrvl_ie_ptr->type)
        {
        case TLV_TYPE_SSID:
        {
            MRVL_IE_SSID_PARAM_PTR ssid;

            ssid = (MRVL_IE_SSID_PARAM_PTR)mrvl_ie_ptr;
            scan_start_ptr->ssid[ssid_id].length = ssid->header.len;
            os_memcpy(scan_start_ptr->ssid[ssid_id].array, ssid->ssid, ssid->header.len);

            ssid_id ++;
            scan_start_ptr->ssid_cnt = ssid_id;

            break;
        }

        case TLV_TYPE_CHANLIST:
        {
            UINT32 i;

            for(i = 0; i < WIFI_2_4_G_CHANNEL_NUM; i ++)
            {
                scan_start_ptr->chan[i].band = WIFI_2_4_G_BAND;
                scan_start_ptr->chan[i].flags = 0;
                scan_start_ptr->chan[i].freq = freq_2_4_G[i];

                channel_id ++;
            }

            scan_start_ptr->chan_cnt = channel_id;

            break;
        }

        case TLV_TYPE_RATES:
        {
            break;
        }

        default:
            break;
        }

        mrvl_ie_ptr = (MRVL_IE_HDR_PTR)((UINT32)scan_cmd_ptr->tlvbuffer
                                        + (UINT32)mrvl_ie_ptr->len
                                        + sizeof(MRVL_IE_HDR_S));
    }

    os_memset(&scan_start_ptr->bssid, 0xff, ETH_ALEN);

    scan_start_ptr->ssid_cnt = 1;
    scan_start_ptr->ssid[0].length = 0;
    scan_start_ptr->add_ies = 0;
    scan_start_ptr->add_ie_len = 0;
#endif

    ASSERT_ERR(ke_task_local(kmsg_dst->dest_id));

    *kmsg_pptr = kmsg_dst;

#if CFG_REAL_SDIO
    scan_start_ptr = (struct scan_start_req *)kmsg_dst->param;

    os_memcpy(scan_start_ptr, (void *)&scmd_hdr_ptr[1], kmsg_dst->param_len);
#endif
    scan_resp_cmd_sn = resp_inc_seqnum();

    return;
}

UINT32 sdio_h2e_msg_conversion(STM32_FRAME_HDR *frm_ptr, struct ke_msg **kmsg_pptr)
{
    UINT32 ret = SDIO_INTF_SUCCESS;
    STM32_CMD_HDR_PTR scmd_hdr_ptr;

    scmd_hdr_ptr = (STM32_CMD_HDR_PTR)&frm_ptr[1];
    if(frm_ptr->len <= sizeof(STM32_FRAME_HDR))
    {
        ret = SDIO_INTF_FAILURE;
        goto conv_exit;
    }

    SDIO_INTF_PRT("cmd:%x\r\n", scmd_hdr_ptr->command);
    switch(scmd_hdr_ptr->command)
    {
    case SCMD_ADD_WEP_KEY:
        cmd_conversion_add_wep_key(frm_ptr, kmsg_pptr);
        break;

    case SCMD_APM_START:
        SDIO_INTF_PRT("start-ap-mode\r\n");
        cmd_conversion_start_apm(frm_ptr, kmsg_pptr);
        break;

    case SCMD_BCN_CHANGE:
        SDIO_INTF_PRT("beacon-change\r\n");
        break;

    case SCMD_MGMT_TX_REQ:
        SDIO_INTF_PRT("mgmt_tx_req\r\n");
        cmd_conversion_mgmt_tx_req(frm_ptr, kmsg_pptr);
        break;

    case SCMD_ADD_STA_REQ:
        SDIO_INTF_PRT("add-sta\r\n");
        cmd_conversion_add_sta_req(frm_ptr, kmsg_pptr);
        break;

    case SCMD_802_11_SCAN:
#if CFG_REAL_SDIO
        if(1)
#else
        if(0 == scan_start_flag)
#endif
        {
            scan_start_flag = 1;
            SDIO_INTF_PRT("80211-scan\r\n");
            cmd_conversion_802_11_scan(frm_ptr, kmsg_pptr);
        }
        else
        {
            SDIO_INTF_PRT("discard_scan\r\n");
            ret = SDIO_INTF_FAILURE;
            goto conv_exit;
        }
        break;

    case SCMD_RESET_REQ:
        SDIO_INTF_PRT("reset-mm\r\n");
        cmd_conversion_reset_req(frm_ptr, kmsg_pptr);
        break;

    case SCMD_ME_CONFIG_REQ:
        SDIO_INTF_PRT("me-config\r\n");
        cmd_conversion_me_config_req(frm_ptr, kmsg_pptr);
        break;

    case SCMD_ME_CHAN_CONFIG_REQ:
        SDIO_INTF_PRT("me-channel-config\r\n");
        cmd_conversion_me_chan_config_req(frm_ptr, kmsg_pptr);
        break;

    case SCMD_START_REQ:
        SDIO_INTF_PRT("mm-start\r\n");
        cmd_conversion_start_req(frm_ptr, kmsg_pptr);
        break;

    case SCMD_ADD_IF_REQ:
        SDIO_INTF_PRT("add-if");
        cmd_conversion_add_if_req(frm_ptr, kmsg_pptr);
        break;

    case SCMD_802_11_KEY_MATERIAL:
        SDIO_INTF_PRT("mm-key-add\r\n");
        cmd_conversion_key_material_req(frm_ptr, kmsg_pptr);
        break;

    case SCMD_802_11_ASSOCIATE:
        SDIO_INTF_PRT("sm-connect\r\n");
        cmd_conversion_assoc_req(frm_ptr, kmsg_pptr);
        break;

    case SCMD_SET_PORT_REQ:
        SDIO_INTF_PRT("set-port\r\n");
        cmd_conversion_set_port_req(frm_ptr, kmsg_pptr);
        break;

    default:
        SDIO_INTF_PRT("default-unkown\r\n");
        ret = SDIO_INTF_FAILURE;
        break;
    }

conv_exit:
    return ret;
}

void sdio_h2e_kmsg_hdlr(struct ke_msg *kmsg_ptr)
{
    ke_msg_send(ke_msg2param(kmsg_ptr));
}
#endif

void sdio_emb_rxed_evt(int dummy)
{
    UINT32 status;
    UINT32 queue_idx;
    UINT8 *content_ptr;
    bool pushed = false;
    SDIO_NODE_PTR mem_node_ptr;
    STM32_TX_FRAME *frm_tx_ptr;
    STM32_FRAME_HDR *frm_hdr_ptr;
#if CFG_REAL_SDIO
    UINT8 i = 0;
#else
    STM32_TXPD_S *txpd;
#endif

    queue_idx = AC_VI;
    mem_node_ptr = sdio_get_rxed_node();

    while(mem_node_ptr)
    {
        frm_hdr_ptr = (STM32_FRAME_HDR *)mem_node_ptr->addr;
        switch(frm_hdr_ptr->type)
        {
        case MVMS_DAT:
        {
            UINT8 tid;
            ETHHDR_PTR eth_hdr_ptr;
            struct txdesc *txdesc_new;
            GLOBAL_INT_DECLARATION();

            GLOBAL_INT_DISABLE();
            list_add_tail(&mem_node_ptr->node_list, &inbound_list);
            GLOBAL_INT_RESTORE();

            frm_tx_ptr = (STM32_TX_FRAME *)mem_node_ptr->addr;
#if CFG_REAL_SDIO
            content_ptr = (UINT8 *)&frm_tx_ptr->frm + 2;
#else
            txpd = &frm_tx_ptr->frm;
            content_ptr = (UINT8 *)&txpd[1];
#endif

            eth_hdr_ptr = (ETHHDR_PTR)content_ptr;

            status = sdio_emb_get_tx_info(content_ptr, &tid);
            if(SDIO_INTF_SUCCESS != status)
            {
                goto rxed_exception;
            }

            queue_idx = sdio_emb_get_hwqueue_id(tid);

            /*allocation of the tx descriptor*/
            txdesc_new = tx_txdesc_prepare(queue_idx);
            if(TXDESC_STA_USED == txdesc_new->status)
            {
                fatal_prf("TFull\r\n");

                inbound_cfm();

                goto rxed_exception;
            }

            txdesc_new->status = TXDESC_STA_USED;
            sdio_emb_txdesc_copy(txdesc_new, eth_hdr_ptr);

            txdesc_new->host.flags            = 0;
            txdesc_new->host.orig_addr        = (UINT32)mem_node_ptr->orig_addr;
            txdesc_new->host.packet_addr      = (UINT32)content_ptr + 14;
#if CFG_REAL_SDIO
            txdesc_new->host.packet_len       = frm_hdr_ptr->len - 14 - sizeof(STM32_FRAME_HDR) - 2;
            txdesc_new->host.staid            = 0;
#else
            txdesc_new->host.packet_len       = frm_hdr_ptr->len - 14 - sizeof(STM32_FRAME_HDR) - sizeof(STM32_TXPD_S);
#endif
            txdesc_new->host.status_desc_addr = (UINT32)content_ptr + 14;
            txdesc_new->host.ethertype        = eth_hdr_ptr->h_proto;
            txdesc_new->host.tid              = tid;
            txdesc_new->host.vif_idx          = 0;

#if (! CFG_REAL_SDIO)
#if CFG_WIFI_AP_MODE
            if(is_broadcast_ether_addr(eth_hdr_ptr->h_dest))
            {
                txdesc_new->host.staid        = VIF_TO_BCMC_IDX(txdesc_new->host.vif_idx);
            }
            else
            {
                txdesc_new->host.staid	      = 0;
            }
#else
            txdesc_new->host.staid		      = 0;
#endif
#endif
            // Initialize some fields of the descriptor
            txdesc_new->lmac.agg_desc = NULL;
            txdesc_new->lmac.hw_desc->cfm.status = 0;

#if NX_POWERSAVE
            txl_cntrl_inc_pck_cnt();
#endif
            txu_cntrl_push(txdesc_new, queue_idx);

            pushed = true;

#if FOR_SDIO_BLK_512
            i++;
#endif
            break;
        }

        case MVMS_CMD:
        {
            UINT32 ret;
            struct ke_msg *kmsg;

            ret = sdio_h2e_msg_conversion(frm_hdr_ptr, &kmsg);
            if(SDIO_INTF_SUCCESS == ret)
            {
                sdio_h2e_kmsg_hdlr(kmsg);
            }

            sdio_release_one_node(mem_node_ptr);

            break;
        }

        case MVMS_EVENT:
            break;

        case MVMS_TXDONE:
            break;

        default:
            break;
        }


        mem_node_ptr = sdio_get_rxed_node();

#if FOR_SDIO_BLK_512
        if(i > 10)
        {
            mem_node_ptr = 0;
        }
#endif
    }

    if(0 == mem_node_ptr)
    {
        ke_evt_clear(KE_EVT_SDIO_RXED_DATA_BIT);
    }

    if (pushed)
    {
#if (NX_AMPDU_TX)
        // Check if the current A-MPDU under construction has to be closed
        txl_aggregate_check(queue_idx);
#endif //(NX_AMPDU_TX)
    }

rxed_exception:
    return;
}

void sdio_rxed_trigger_evt(void)
{
    ke_evt_set(KE_EVT_SDIO_RXED_DATA_BIT);
}

UINT32 sdio_intf_init(void)
{
    UINT32 ret;
    UINT32 status;
    DD_HANDLE sdio_hdl;
    RW_CONNECTOR_T intf;

    ret = SDIO_INTF_FAILURE;

    intf.msg_outbound_func = sdio_emb_kmsg_fwd;
    intf.data_outbound_func = outbound_upload_data;
    intf.rx_alloc_func = sdio_get_free_node;
    intf.get_rx_valid_status_func = sdio_get_free_node_count;
    intf.tx_confirm_func = inbound_cfm;
    rwnxl_register_connector(&intf);

    INIT_LIST_HEAD(&inbound_list);

    sdio_hdl = ddev_open(SDIO_DEV_NAME, &status, 0);
    if(DD_HANDLE_UNVALID == sdio_hdl)
    {
        goto init_exception;
    }

    ddev_control(sdio_hdl, SDIO_CMD_REG_RX_CALLBACK, (void *)sdio_rxed_trigger_evt);

    SDIO_INTF_PRT("sdio_intf_init\r\n");
    ret = SDIO_INTF_SUCCESS;

init_exception:

    return ret;
}
#endif // CFG_SDIO
// EOF

