#include "include.h"
#include "hostapd_intf.h"
#include "sk_intf.h"
#include "mem_pub.h"
#include "me_task.h"
#include "ke_event.h"
#include "fake_socket.h"
#include "uart_pub.h"
#include "ieee802_11_defs.h"
#include "driver.h"
#include "driver_beken.h"
#include "hostapd_cfg.h"
#include "rw_pub.h"
#include "drv_model_pub.h"
#include "sta_mgmt.h"
#include "vif_mgmt.h"
#include "sa_station.h"
#include "sa_ap.h"
#include "wlan_ui_pub.h"
#include "param_config.h"
#include "sm_task.h"
#include "rw_msg_rx.h"
#include "mm_bcn.h"

#if CFG_ROLE_LAUNCH
#include "role_launch.h"
#endif
#include "mcu_ps_pub.h"
#include "power_save_pub.h"

struct scanu_rst_upload *s_scan_result_upload_ptr;

struct mm_bcn_change_req *hadp_intf_get_bcn_change_req(uint8_t vif_id, struct beacon_data *bcn_info)
{
#define BCN_TIM_IE_LEN                   6
    uint8_t tim_ie[BCN_TIM_IE_LEN] = {5, 4, 0, 2, 0, 0};
    uint8_t *beacon_ptr = 0;
    uint32_t bcn_len;
    struct mm_bcn_change_req *req = 0;

#ifdef ADD_BEACON_IES_ENABLE
    bcn_len = bcn_info->head_len + BCN_TIM_IE_LEN + bcn_info->tail_len
              + bcn_info->beacon_ies_len;
#else
    bcn_len = bcn_info->head_len + BCN_TIM_IE_LEN + bcn_info->tail_len;
#endif

    beacon_ptr = (uint8_t *)os_malloc(bcn_len);
    if (!beacon_ptr)
        goto exit_get_failed;

    req = ke_msg_alloc(MM_BCN_CHANGE_REQ, TASK_MM, TASK_API,
                       sizeof(struct mm_bcn_change_req));
    if (!req)
        goto exit_get_failed;

    if(bcn_info->head && bcn_info->head_len)
    {
        os_memcpy(beacon_ptr, bcn_info->head, bcn_info->head_len);
    }

    os_memcpy((void *)((uint32_t)beacon_ptr + bcn_info->head_len), tim_ie, BCN_TIM_IE_LEN);

    if(bcn_info->tail && bcn_info->tail_len)
    {
        os_memcpy((void *)((uint32_t)beacon_ptr + bcn_info->head_len + BCN_TIM_IE_LEN),
                  bcn_info->tail, bcn_info->tail_len);
    }

#ifdef ADD_BEACON_IES_ENABLE
    if(bcn_info->beacon_ies && bcn_info->beacon_ies_len)
    {
        os_memcpy((void *)((uint32_t)beacon_ptr + bcn_info->head_len + bcn_info->tail_len),
                  bcn_info->beacon_ies, bcn_info->beacon_ies_len);
    }
#endif // ADD_BEACON_IES_ENABLE

    req->bcn_ptr = (uint32_t)(beacon_ptr);
    req->bcn_len = bcn_len;
    req->tim_len = BCN_TIM_IE_LEN;
    req->tim_oft = bcn_info->head_len;
    req->inst_nbr = vif_id;
    req->bcn_ptr_malloc_flag = 1;

    return req;

exit_get_failed:
    if(req)
        os_free(req);
    if(beacon_ptr)
        os_free(beacon_ptr);

    return NULL;
}

struct mm_bcn_change_req *hadp_intf_get_csa_bcn_req(struct csa_settings *settings,
        uint8_t vif_id, struct beacon_data *bcn_info)
{
    uint16_t csa_cnt_offset;
    struct mm_bcn_change_req *req;

    req = hadp_intf_get_bcn_change_req(vif_id, bcn_info);

    csa_cnt_offset = settings->counter_offset_beacon + 6 + bcn_info->head_len; // tim_ie_len == 6
    req->csa_oft[0] =  csa_cnt_offset & 0xff;
    req->csa_oft[1] = (csa_cnt_offset & 0xff00) >> 8;

    return req;
}

struct mm_bcn_change_req *hadp_intf_get_csa_after_bcn_req(uint8_t vif_id, struct beacon_data *bcn_info)
{
    return hadp_intf_get_bcn_change_req(vif_id, bcn_info);
}

int wpa_intf_channel_switch(struct prism2_hostapd_param *param, int len)
{
    uint8_t vif_id;
    uint32_t freq;
    uint32_t csa_count;
    struct vif_info_tag *vif;
    struct csa_settings *settings;
    struct mm_bcn_change_req *param_csa;
    struct mm_bcn_change_req *param_csa_after;

    freq = param->u.chan_switch.freq;
    csa_count = param->u.chan_switch.csa_cnt;

    vif_id = param->vif_idx;
    vif = vif_mgmt_get_entry(vif_id);

#if CFG_ROLE_LAUNCH
    if(rl_pre_sta_set_status(RL_STATUS_STA_CHANNEL_SWITCHING))
    {
        return 0;
    }
#endif
    if(freq == bk_wlan_ap_get_channel_config())
    {
        os_printf("csa_over_same_channel\r\n");
        return 0;
    }

    mm_channel_switch_init(vif, freq, csa_count);

    os_printf("h_channel_switch:%x:%d:%d\r\n", vif, param->vif_idx, csa_count);
    settings = param->u.chan_switch.settings;
    param_csa = hadp_intf_get_csa_bcn_req(settings, vif_id, &settings->beacon_csa);
    param_csa_after = hadp_intf_get_csa_after_bcn_req(vif_id, &settings->beacon_after);
    mm_csa_beacon_change(param_csa, param_csa_after);

    return 0;
}

int hapd_intf_sta_add(struct prism2_hostapd_param *param, int len)
{
    int ret;
    struct add_sta_st add_sta;
    struct me_sta_add_cfm *cfm
        = (struct me_sta_add_cfm *)os_malloc(sizeof(struct me_sta_add_cfm));

    if(NULL == cfm)
    {
        WPAS_PRT("sta_add buf_failed\r\n");
        return HINTF_FAILURE;
    }

    add_sta.aid = param->u.add_sta.aid;
    add_sta.ap_vif_idx = param->vif_idx;
    add_sta.sta_addr = param->sta_addr;
    add_sta.capability = param->u.add_sta.capability;
    add_sta.tx_supp_rates = param->u.add_sta.tx_supp_rates;

    ret = rw_msg_send_me_sta_add(&add_sta, cfm);
    if(!ret && (cfm->status == CO_OK))
    {
        WPAS_PRT("sta_idx:%d, pm_state:%d\r\n", cfm->sta_idx, cfm->pm_state);

#if CFG_USE_AP_PS
        rwm_flush_txing_list(cfm->sta_idx);
#endif
    }
    os_free(cfm);

    return ret;
}

int hapd_intf_sta_del(struct prism2_hostapd_param *param, int len)
{
    bool tdls_sta = 0;
    u8 sta_idx;
    sta_idx = rwm_mgmt_sta_mac2idx(param->sta_addr);

    if(sta_idx == 0xff)
        return 0;

    WPAS_PRT("hapd_intf_sta_del:%d\r\n", sta_idx);

#if CFG_USE_AP_PS
    rwm_flush_txing_list(sta_idx);
#endif

    return rw_msg_send_me_sta_del(sta_idx, tdls_sta);
}

int hapd_intf_add_key(struct prism2_hostapd_param *param, int len)
{
    int ret;
    KEY_PARAM_T key_param;
    struct mm_key_add_cfm *cfm
        = (struct mm_key_add_cfm *)os_malloc(sizeof(struct mm_key_add_cfm));

    if(NULL == cfm)
    {
        WPAS_PRT("key_add buf_failed\r\n");
        return HINTF_FAILURE;
    }

    if(os_strcmp((char *)param->u.crypt.alg, "WEP40") == 0)
    {
        WPAS_PRT("add WEP40\r\n");
        key_param.cipher_suite = MAC_RSNIE_CIPHER_WEP40;
    }
    else if(os_strcmp((char *)param->u.crypt.alg, "WEP104") == 0)
    {
        WPAS_PRT("add WEP104\r\n");
        key_param.cipher_suite = MAC_RSNIE_CIPHER_WEP104;
    }
    else if(os_strcmp((char *)param->u.crypt.alg, "TKIP") == 0)
    {
        WPAS_PRT("add TKIP\r\n");
        key_param.cipher_suite = MAC_RSNIE_CIPHER_TKIP;
    }
    else if(os_strcmp((char *)param->u.crypt.alg, "CCMP") == 0)
    {
        WPAS_PRT("hapd_intf_add_key CCMP\r\n");
        key_param.cipher_suite = MAC_RSNIE_CIPHER_CCMP;
    }

    if(is_broadcast_ether_addr(param->sta_addr))
    {
        WPAS_PRT("add is_broadcast_ether_addr\r\n");
        key_param.sta_idx = 0xFF;
        key_param.inst_nbr = param->vif_idx;
    }
    else
    {
        WPAS_PRT("add sta_mgmt_get_sta\r\n");
        key_param.sta_idx = rwm_mgmt_sta_mac2idx(param->sta_addr);
        key_param.inst_nbr = param->vif_idx;
    }

    key_param.key_idx = param->u.crypt.idx;
    key_param.key.length = param->u.crypt.key_len;
    os_memcpy((u8 *) & (key_param.key.array[0]), (u8 *)&param[1], key_param.key.length);

    WPAS_PRT("sta:%d, vif:%d, key:%d\r\n",
             key_param.sta_idx, key_param.inst_nbr, key_param.key_idx);

#if CFG_USE_STA_PS
    bk_wlan_dtim_rf_ps_mode_do_wakeup();
#endif
    ps_set_key_prevent();
    mcu_prevent_set(MCU_PS_ADD_KEY);

    ret = rw_msg_send_key_add(&key_param, cfm);
    if(!ret && (cfm->status == CO_OK))
    {
        WPAS_PRT("add hw key idx:%d\r\n", cfm->hw_key_idx);
    }

    os_free(cfm);

    return ret;
}


int hapd_intf_del_key(struct prism2_hostapd_param *param, int len)
{
    u8 hw_key_idx = 0;

    if( (param->sta_addr == NULL) || is_broadcast_ether_addr(param->sta_addr))
    {
        hw_key_idx = rwm_mgmt_get_hwkeyidx(param->vif_idx, 0xff);
    }
    else
    {
        u8 staid = rwm_mgmt_sta_mac2idx(param->sta_addr);
        if(staid == 0xff)
            return 0;
        hw_key_idx = rwm_mgmt_get_hwkeyidx(param->vif_idx, staid);
    }

    if(hw_key_idx > MM_SEC_MAX_KEY_NBR)
        return 0;

    WPAS_PRT("del hw key idx:%d\r\n", hw_key_idx);

    return rw_msg_send_key_del(hw_key_idx);

}

int hapd_intf_add_vif(struct prism2_hostapd_param *param, int len)
{
    int ret;
    struct mm_add_if_cfm *cfm
        = (struct mm_add_if_cfm *)os_malloc(sizeof(struct mm_add_if_cfm));

    if(NULL == cfm)
    {
        WPAS_PRT("vif_add buf_failed\r\n");
        return HINTF_FAILURE;
    }

    ret = rw_msg_send_add_if((const unsigned char *)&param->sta_addr,
                             param->u.add_if.type, 0, cfm);

    if(ret || (cfm->status != CO_OK))
    {
        SAAP_PRT("MM_ADD_IF_REQ failed!\r\n");
        os_free(cfm);
        return -1;
    }
    SAAP_PRT("hapd_intf_add_vif,type:%d, s:%d, id:%d\r\n",
             param->u.add_if.type, cfm->status, cfm->inst_nbr);

    if(cfm->inst_nbr >= NX_VIRT_DEV_MAX)
        return -1;

    *(param->u.add_if.indx_ptr) = cfm->inst_nbr;

    os_free(cfm);

    return 0;
}

int hapd_intf_remove_vif(struct prism2_hostapd_param *param, int len)
{
    u8 vif_index = param->vif_idx;

    if(vif_index >= NX_VIRT_DEV_MAX)
        return -1;

    WPAS_PRT("wpa/host apd remove_if:%d\r\n", vif_index);

    return rw_msg_send_remove_if(vif_index);
}

int hapd_intf_start_apm(struct prism2_hostapd_param *param, int len)
{
    int ret;
    struct apm_start_cfm *cfm
        = (struct apm_start_cfm *)os_malloc(sizeof(struct apm_start_cfm));

    if(NULL == cfm)
    {
        WPAS_PRT("apm_start buf_failed\r\n");
        return HINTF_FAILURE;
    }

    ret = rw_msg_send_apm_start_req(param->vif_idx, g_ap_param_ptr->chann, cfm);
    if(ret)
    {
        SAAP_PRT("hapd_intf_start_apm failed!\r\n");
        os_free(cfm);
        return -1;
    }

    if(cfm->status == CO_OK)
    {
        SAAP_PRT("vif_idx:%d, ch_idx:%d, bcmc_idx:%d\r\n", cfm->vif_idx,
                 cfm->ch_idx, cfm->bcmc_idx);
    }
    os_free(cfm);
    return 0;
}

int hapd_intf_stop_apm(struct prism2_hostapd_param *param, int len)
{
    int ret;
    ret = rw_msg_send_apm_stop_req(param->vif_idx);
    if(ret)
    {
        SAAP_PRT("hapd_intf_start_apm failed!\r\n");
        return -1;
    }
    return 0;
}

int hapd_intf_set_ap_bcn(struct prism2_hostapd_param *param, int len)
{
    char *bcn_buf;
    BCN_PARAM_ST bcn_param;

#if CFG_ROLE_LAUNCH
    if(rl_pre_ap_set_status(RL_STATUS_AP_UPDATING_BCN))
    {
        return 0;
    }
#endif

    if(param->u.bcn_change.bcn_len == 0)
    {
        WPAS_WPRT("hapd_intf_set_ap bcn len = 0\r\n");
        return 0;
    }

    bcn_buf = (char *)os_malloc(param->u.bcn_change.bcn_len);
    if(!bcn_buf)
    {
        WPAS_WPRT("hapd_intf_set_ap no buffer\r\n");
        return -1;
    }

    os_memcpy(bcn_buf, param->u.bcn_change.beacon, param->u.bcn_change.bcn_len);
    os_memset(&bcn_param, 0, sizeof(BCN_PARAM_ST));

    bcn_param.bcn_ptr = (u32 *)bcn_buf;
    bcn_param.bcn_len = param->u.bcn_change.bcn_len;
    bcn_param.tim_len = param->u.bcn_change.tim_len;
    bcn_param.tim_oft = param->u.bcn_change.head_len;
    bcn_param.vif_idx = param->vif_idx;
    bcn_param.csa_oft = NULL;
    bcn_param.flag = 1;

    return rw_msg_send_bcn_change((void *)&bcn_param);
}

void wpa_buffer_scan_results(void)
{
    s_scan_result_upload_ptr = sr_get_scan_results();
}

void wpa_clear_scan_results(void)
{
    s_scan_result_upload_ptr = 0;
}

int wpa_reg_assoc_cfm_callback(struct prism2_hostapd_param *param, int len)
{
    ASSERT(param);
    mhdr_assoc_cfm_cb(param->u.reg_assoc_cfm.cb,
                      param->u.reg_assoc_cfm.arg);
}

int wpa_reg_scan_cfm_callback(struct prism2_hostapd_param *param, int len)
{
    ASSERT(param);
    mhdr_scanu_reg_cb(param->u.reg_scan_cfm.cb,
                      param->u.reg_scan_cfm.arg);
}

int wpa_reg_deassoc_evt_callback(struct prism2_hostapd_param *param, int len)
{
    ASSERT(param);
    mhdr_deassoc_evt_cb(param->u.reg_disassoc_evt.cb,
                        param->u.reg_disassoc_evt.arg);
}

int wpa_reg_deauth_evt_callback(struct prism2_hostapd_param *param, int len)
{
    ASSERT(param);
    mhdr_deauth_evt_cb(param->u.reg_deauth_evt.cb,
                       param->u.reg_deauth_evt.arg);
}

int wpa_send_scan_req(struct prism2_hostapd_param *param, int len)
{
    UINT8 i;
    SCAN_PARAM_T scan_param;

    WPAS_PRT("wpa_send_scan_req\r\n");
    scan_param.num_ssids = param->u.scan_req.ssids_num;
    for(i = 0; i < scan_param.num_ssids; i++)
    {
        scan_param.ssids[i].length = param->u.scan_req.ssids[i].ssid_len;
        os_memcpy(scan_param.ssids[i].array, param->u.scan_req.ssids[i].ssid, scan_param.ssids[i].length);
    }
    scan_param.bssid = mac_addr_bcst;
    scan_param.vif_idx = param->vif_idx;

    return rw_msg_send_scanu_req(&scan_param);
}

int wpa_get_scan_rst(struct prism2_hostapd_param *param, int len)
{
    struct wpa_scan_results *results = param->u.scan_rst;
    struct sta_scan_res *scan_rst_ptr;
    struct wpa_scan_res *r;
    int i, ret = 0;

    if(NULL == s_scan_result_upload_ptr)
    {
        WPAS_PRT("get_scan_rst_null\r\n");
        mhdr_set_station_status(MSG_NO_AP_FOUND);

        return -1;
    }

#if CFG_ROLE_LAUNCH
    if(rl_pre_sta_set_status(RL_STATUS_STA_CONNECTING))
    {
        return -1;
    }
#endif

    WPAS_PRT("wpa_get_scan_rst:%d\r\n", s_scan_result_upload_ptr->scanu_num);
    for(i = 0; i < s_scan_result_upload_ptr->scanu_num; i++)
    {
        scan_rst_ptr = s_scan_result_upload_ptr->res[i];
        r = os_zalloc(sizeof(*r) + scan_rst_ptr->ie_len);
        if (r == NULL)
        {
            ret = -1;
            WPAS_PRT("wpa_get_scan_rst break;\r\n");
            break;
        }

        os_memcpy(r->bssid, scan_rst_ptr->bssid, ETH_ALEN);
        r->level = scan_rst_ptr->level;
        r->freq = rw_ieee80211_get_centre_frequency(scan_rst_ptr->channel);
        r->beacon_int = scan_rst_ptr->beacon_int;
        r->caps = scan_rst_ptr->caps;
        r->tsf = WPA_GET_BE64(scan_rst_ptr->tsf);
        r->ie_len = scan_rst_ptr->ie_len;
        os_memcpy(r + 1, scan_rst_ptr + 1, scan_rst_ptr->ie_len);

        results->res[results->num++] = r;
    }

    return ret;
}

int wpa_send_assoc_req(struct prism2_hostapd_param *param, int len)
{
    CONNECT_PARAM_T connect_param = {0};
    int ret;

    os_memcpy((UINT8 *)&connect_param.bssid, param->u.assoc_req.bssid, ETH_ALEN);
    connect_param.flags = CONTROL_PORT_HOST;
    if(param->u.assoc_req.proto & (WPA_PROTO_WPA | WPA_PROTO_RSN))
    {
        connect_param.flags |= WPA_WPA2_IN_USE;
    }

    connect_param.vif_idx = param->vif_idx;
    connect_param.ssid.length = param->u.assoc_req.ssid_len;
    os_memcpy(connect_param.ssid.array, param->u.assoc_req.ssid, connect_param.ssid.length);
    connect_param.ie_len = param->u.assoc_req.ie_len;
    os_memcpy((UINT8 *)connect_param.ie_buf, (UINT8 *)param->u.assoc_req.ie_buf, connect_param.ie_len);

    connect_param.auth_type = param->u.assoc_req.auth_alg;
    ret = sa_station_send_associate_cmd(&connect_param);

    if(s_scan_result_upload_ptr)
    {
        sr_release_scan_results(s_scan_result_upload_ptr);
        s_scan_result_upload_ptr = NULL;
    }

    return ret;
}

int wpa_send_disconnect_req(struct prism2_hostapd_param *param, int len)
{
    DISCONNECT_PARAM_T disconnect_param = {0};
    disconnect_param.vif_idx = param->vif_idx;
    disconnect_param.reason_code = param->u.disconnect_req.reason;

    return rw_msg_send_sm_disconnect_req(&disconnect_param);
}

int wpa_get_bss_info(struct prism2_hostapd_param *param, int len)
{
    int ret, ssid_len;
    struct sm_get_bss_info_cfm *cfm;

    cfm = os_malloc(sizeof(struct sm_get_bss_info_cfm));

    if(NULL == cfm)
    {
        WPAS_PRT("bss_get_info buf_failed\r\n");
        return HINTF_FAILURE;
    }

    ret = rw_msg_get_bss_info(param->vif_idx, (void *)cfm);
    if(ret)
        return -1;

    os_memcpy(param->u.bss_info.bssid, cfm->bssid, ETH_ALEN);
    ssid_len = MIN(SSID_MAX_LEN, os_strlen(cfm->ssid));
    os_memcpy(param->u.bss_info.ssid, cfm->ssid, ssid_len);
    os_free(cfm);
}

int wpa_hostapd_set_sta_flag(struct prism2_hostapd_param *param, int len)
{
    u32 set_flag = param->u.set_flags_sta.flags_or;
    u32 mask = param->u.set_flags_sta.flags_and;
    u32 flag = 0;
    bool opened = 0;
    u8 sta_idx;

    flag |= set_flag;
    flag &= mask;

    if(!flag)
        return 0;

    if( !(flag & WPA_STA_AUTHENTICATED))
        return 0;

    sta_idx = rwm_mgmt_sta_mac2idx(param->sta_addr);
    if(sta_idx == 0xff)
        return 0;

    opened = 1;

    return rw_msg_me_set_control_port_req(opened, sta_idx);
}

void wpa_hostapd_release_scan_rst(void)
{
    if(s_scan_result_upload_ptr)
    {
        sr_release_scan_results(s_scan_result_upload_ptr);
        s_scan_result_upload_ptr = NULL;
    }
    else
    {
        SCAN_RST_UPLOAD_T *rst = sr_get_scan_results();
        sr_release_scan_results(rst);
    }
}

int hapd_intf_ioctl(unsigned long arg)
{
    u32 cmd;
    int len, ret = 0;
    struct iwreq *iwr_ptr;
    struct prism2_hostapd_param *param;

    if(0 == arg)
    {
        ret = EINVAL;
        goto ioctl_exit;
    }

    iwr_ptr = (struct iwreq *)arg;
    len = iwr_ptr->u.data.length;
    param = iwr_ptr->u.data.pointer;

    cmd = param->cmd;
    switch(cmd)
    {
    case PRISM2_HOSTAPD_CHANNEL_SWITCH:
        wpa_intf_channel_switch(param, len);
        break;

    case PRISM2_HOSTAPD_WPA_INIT_VIF:
        ret = hapd_intf_add_vif(param, len);
        break;

    case PRISM2_HOSTAPD_SET_FLAGS_STA:
        ret = wpa_hostapd_set_sta_flag(param, len);
        break;

    case PRISM2_HOSTAPD_START_APM:
        ret = hapd_intf_start_apm(param, len);
        break;

    case PRISM2_HOSTAPD_STOP_APM:
        ret = hapd_intf_stop_apm(param, len);
        break;

    case PRISM2_HOSTAPD_SET_AP_BCN:
        ret = hapd_intf_set_ap_bcn(param, len);
        break;

    case PRISM2_SET_ENCRYPTION:
        if(os_strcmp((const char *)param->u.crypt.alg, "NONE") == 0)
        {
            ret = hapd_intf_del_key(param, len);
            mcu_prevent_clear(MCU_PS_ADD_KEY);
        }
        else
        {
            ret = hapd_intf_add_key(param, len);
        }
        break;

    case PRISM2_GET_ENCRYPTION:
        break;

    case PRISM2_HOSTAPD_FLUSH:
        break;

    case PRISM2_HOSTAPD_ADD_STA:
        ret = hapd_intf_sta_add(param, len);
        break;

    case PRISM2_HOSTAPD_REMOVE_STA:
        ret = hapd_intf_sta_del(param, len);
        break;

    case PRISM2_HOSTAPD_GET_INFO_STA:
        break;

    case PRISM2_HOSTAPD_STA_CLEAR_STATS:
        break;

    case PRISM2_HOSTAPD_SET_GENERIC_ELEMENT:
        break;

    case PRISM2_HOSTAPD_REG_SCAN_CALLBACK:
        wpa_reg_scan_cfm_callback(param, len);
        break;

    case PRISM2_HOSTAPD_REG_CSA_CALLBACK:
        ASSERT(param);
        mm_csa_event_cb(param->u.reg_csa_event.cb,
                        param->u.reg_csa_event.arg);
        break;

    case PRISM2_HOSTAPD_REG_ASSOC_CALLBACK:
        wpa_reg_assoc_cfm_callback(param, len);
        break;
    case PRISM2_HOSTAPD_SCAN_REQ:
        ret = wpa_send_scan_req(param, len);
        break;

    case PRISM2_HOSTAPD_GET_SCAN_RESULT:
        ret = wpa_get_scan_rst(param, len);
        break;

    case PRISM2_HOSTAPD_ASSOC_REQ:
        ret = wpa_send_assoc_req(param, len);
        break;

    case PRISM2_HOSTAPD_ASSOC_ACK:
        break;

    case PRISM2_HOSTAPD_REG_DISASSOC_CALLBACK:
        wpa_reg_deassoc_evt_callback(param, len);
        break;

    case PRISM2_HOSTAPD_REG_DEAUTH_CALLBACK:
        wpa_reg_deauth_evt_callback(param, len);
        break;

    case PRISM2_HOSTAPD_REG_APP_START:
        break;

    case PRISM2_HOSTAPD_DISCONN_REQ:
        wpa_send_disconnect_req(param, len);
        break;

    case PRISM2_HOSTAPD_WPA_DEINIT_VIF:
        ret = hapd_intf_remove_vif(param, len);
        break;

    case PRISM2_HOSTAPD_GET_BSS_INFO:
        ret = wpa_get_bss_info(param, len);
        break;

    default:
        break;
    }

ioctl_exit:
    return ret;
}

void hapd_intf_ke_rx_handle(int dummy)
{
    int payload_size;
    unsigned char *buf;
    S_TYPE_PTR type_ptr = (S_TYPE_PTR)dummy;

    if(type_ptr->type == HOSTAPD_MGMT)
    {
        int param_len;
        struct ke_msg *kmsg_dst;
        struct me_mgmt_tx_req *mgmt_tx_ptr;

        payload_size = ke_mgmt_peek_rxed_next_payload_size(type_ptr->vif_index);
        ASSERT(payload_size);
        buf = (unsigned char *)os_malloc(payload_size);
        if(0 == buf)
        {
            goto exit;
        }
        ke_mgmt_packet_rx(buf, payload_size, type_ptr->vif_index);
        param_len = sizeof(struct me_mgmt_tx_req);
        kmsg_dst = (struct ke_msg *)os_malloc(sizeof(struct ke_msg)
                                              + param_len);
        if(0 == kmsg_dst)
        {
            goto kmsg_malloc_fail;
        }
        os_memset(kmsg_dst, 0, (sizeof(struct ke_msg) + param_len));

        kmsg_dst->id = ME_MGMT_TX_REQ;
        kmsg_dst->dest_id = TASK_ME;
        kmsg_dst->src_id  = 100;
        kmsg_dst->param_len = param_len;

        mgmt_tx_ptr = (struct me_mgmt_tx_req *)kmsg_dst->param;
        mgmt_tx_ptr->addr = (UINT32)buf;
        mgmt_tx_ptr->hostid = (UINT32)buf;
        mgmt_tx_ptr->len = payload_size;

        mgmt_tx_ptr->req_malloc_flag = 1;
        mgmt_tx_ptr->vif_idx = type_ptr->vif_index;

        ke_msg_send(ke_msg2param(kmsg_dst));
    }
    else if(type_ptr->type == HOSTAPD_DATA)
    {
        UINT8 *pd_ptr;
        void *txnode = NULL;

        payload_size = ke_data_peek_rxed_next_payload_size(type_ptr->vif_index);
        pd_ptr = (UINT8 *)os_malloc(payload_size);
        if(NULL == pd_ptr)
        {
            os_printf("hapd_intf_tx_error\r\n");
            goto exit;
        }

        ke_l2_packet_rx(pd_ptr, payload_size, type_ptr->vif_index);

        ps_set_data_prevent();
#if CFG_USE_STA_PS
        bk_wlan_dtim_rf_ps_mode_do_wakeup();
#endif

        rwm_transfer(type_ptr->vif_index, pd_ptr, payload_size);

        os_free(pd_ptr);
    }

    os_free(type_ptr);
    return;

kmsg_malloc_fail:
    os_free(buf);

exit:
    os_free(type_ptr);
    return;
}

// eof

