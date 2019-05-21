#include "include.h"
#include "rw_msg_tx.h"
#include "ke_msg.h"
#include "rw_ieee80211.h"
#include "rw_pub.h"
#include "mem_pub.h"
#include "common.h"

#include "scanu_task.h"
#include "scan_task.h"
#include "mm_task.h"
#include "sm_task.h"
#include "me_task.h"
#include "apm_task.h"
#include "vif_mgmt.h"
#include "str_pub.h"
#include "mac_frame.h"
#include "lwip/def.h"
#include "scanu_task.h"
#include "wlan_ui_pub.h"
#include "param_config.h"
#if CFG_ROLE_LAUNCH
#include "role_launch.h"
#endif
#include "error.h"

extern int bmsg_ioctl_sender(void *arg);

int rw_msg_send(const void *msg_params, uint16_t reqid, void *cfm)
{
    int ret;
    struct ke_msg *msg = ke_param2msg(msg_params);
    uint8_t need_cfm = 0;
    MSG_SND_NODE_PTR tx_msg;

    GLOBAL_INT_DECLARATION();

    // Need wait cfm from full mac
    if(cfm != NULL)
    {
        tx_msg = os_malloc(sizeof(MSG_SND_NODE_ST));
        if(!tx_msg)
            return -1;

        tx_msg->msg = msg;
        tx_msg->reqid = reqid;
        tx_msg->cfm = cfm;

#if CFG_SUPPORT_ALIOS
        ret = rtos_init_semaphore(&tx_msg->semaphore, 0);
#else
        ret = rtos_init_semaphore(&tx_msg->semaphore, 1);
#endif
        ASSERT(0 == ret);

        GLOBAL_INT_DISABLE();
        co_list_push_back(&rw_msg_tx_head, &tx_msg->hdr);
        GLOBAL_INT_RESTORE();

        need_cfm = 1;
    }

    //ke_msg_send(msg_params);
    ret = bmsg_ioctl_sender((void*)msg_params);
    if (kNoErr != ret)
    {
        os_printf("%s failed send %d\n", __FUNCTION__, msg->id);
        os_free((void*)msg_params);

        goto failed_or_timeout;
    }
    else if (need_cfm)
    {
        ret = rtos_get_semaphore(&tx_msg->semaphore, 5 * MICROSECONDS);
        if (0 != ret)
        {
            os_printf("%s timeout for %d\n", __FUNCTION__, reqid);
            GLOBAL_INT_DISABLE();
            co_list_extract(&rw_msg_tx_head, &tx_msg->hdr);
            GLOBAL_INT_RESTORE();

            goto failed_or_timeout;
        }

        ret = rtos_deinit_semaphore(&tx_msg->semaphore);
        ASSERT(0 == ret);

        os_free(tx_msg);
    }

    return 0;

failed_or_timeout:
    if (need_cfm)
    {
        ret = rtos_deinit_semaphore(&tx_msg->semaphore);
        ASSERT(0 == ret);

        os_free(tx_msg);
    }
    return -1;
}

int rw_msg_send_reset(void)
{
    void *void_param;

    /* RESET REQ has no parameter */
    void_param = ke_msg_alloc(MM_RESET_REQ, TASK_MM, TASK_API, 0);
    if (!void_param)
        return -1;

    return rw_msg_send(void_param, MM_RESET_CFM, NULL);
}

int rw_msg_send_start(void)
{
    struct mm_start_req *start_req_param;

    /* Build the START REQ message */
    start_req_param = ke_msg_alloc(MM_START_REQ, TASK_MM, TASK_API,
                                   sizeof(struct mm_start_req));
    if (!start_req_param)
        return -1;

    /* Set parameters for the START message */
    start_req_param->phy_cfg.parameters[0] = 1;
    start_req_param->phy_cfg.parameters[1] = 0;
    start_req_param->uapsd_timeout = 300;
    start_req_param->lp_clk_accuracy = 20;

    /* Send the START REQ message to LMAC FW */
    return rw_msg_send(start_req_param, MM_START_CFM, NULL);
}

int rw_msg_send_me_config_req(void)
{
    struct me_config_req *req;

#if CFG_IEEE80211N
    WIPHY_T *wiphy = &g_wiphy;
    struct ieee80211_sta_ht_cap *ht_cap = &wiphy->bands[IEEE80211_BAND_2GHZ].ht_cap;
    struct ieee80211_sta_vht_cap *vht_cap = &wiphy->bands[IEEE80211_BAND_2GHZ].vht_cap;
    uint8_t *ht_mcs = (uint8_t *)&ht_cap->mcs;
    int i;
#endif // CFG_IEEE80211N    

    /* Build the ME_CONFIG_REQ message */
    req = ke_msg_alloc(ME_CONFIG_REQ, TASK_ME, TASK_API,
                       sizeof(struct me_config_req));
    if (!req)
        return -1;

    /* Set parameters for the ME_CONFIG_REQ message */
#if CFG_IEEE80211N
    req->ht_supp = ht_cap->ht_supported;
    req->vht_supp = vht_cap->vht_supported;
    req->ht_cap.ht_capa_info = cpu_to_le16(ht_cap->cap);
    req->ht_cap.a_mpdu_param = ht_cap->ampdu_factor |
                               (ht_cap->ampdu_density <<
                                IEEE80211_HT_AMPDU_PARM_DENSITY_SHIFT);

    for (i = 0; i < sizeof(ht_cap->mcs); i++)
    {
        req->ht_cap.mcs_rate[i] = ht_mcs[i];
    }

    req->ht_cap.ht_extended_capa = 0;
    req->ht_cap.tx_beamforming_capa = 0x64000000;
    req->ht_cap.asel_capa = 0x1;

    req->vht_cap.vht_capa_info = cpu_to_le32(vht_cap->cap);
    req->vht_cap.rx_highest = cpu_to_le16(vht_cap->vht_mcs.rx_highest);
    req->vht_cap.rx_mcs_map = cpu_to_le16(vht_cap->vht_mcs.rx_mcs_map);
    req->vht_cap.tx_highest = cpu_to_le16(vht_cap->vht_mcs.tx_highest);
    req->vht_cap.tx_mcs_map = cpu_to_le16(vht_cap->vht_mcs.tx_mcs_map);
#endif // CFG_IEEE80211N

    /* Send the ME_CONFIG_REQ message to LMAC FW */
    return rw_msg_send(req, ME_CONFIG_CFM, NULL);
}

int rw_msg_send_me_chan_config_req(void)
{
    struct me_chan_config_req *req;
    WIPHY_T *wiphy = &g_wiphy;
    int i;

    /* Build the ME_CHAN_CONFIG_REQ message */
    req = ke_msg_alloc(ME_CHAN_CONFIG_REQ, TASK_ME, TASK_API,
                       sizeof(struct me_chan_config_req));
    if (!req)
        return -1;

    req->chan2G4_cnt =  0;
    if (wiphy->bands[IEEE80211_BAND_2GHZ].num_channels)
    {
        struct ieee80211_supported_band *b = &wiphy->bands[IEEE80211_BAND_2GHZ];
        for (i = 0; i < b->num_channels; i++)
        {
            req->chan2G4[req->chan2G4_cnt].flags = 0;

            if (b->channels[i].flags & IEEE80211_CHAN_DISABLED)
                req->chan2G4[req->chan2G4_cnt].flags |= SCAN_DISABLED_BIT;

            if (b->channels[i].flags & IEEE80211_CHAN_NO_IR)
                req->chan2G4[req->chan2G4_cnt].flags |= SCAN_PASSIVE_BIT;

            req->chan2G4[req->chan2G4_cnt].band = IEEE80211_BAND_2GHZ;
            req->chan2G4[req->chan2G4_cnt].freq = b->channels[i].center_freq;
            req->chan2G4_cnt++;

            if (req->chan2G4_cnt == SCAN_CHANNEL_2G4)
                break;
        }
    }

    req->chan5G_cnt = 0;
    if (wiphy->bands[IEEE80211_BAND_5GHZ].num_channels)
    {
        struct ieee80211_supported_band *b = &wiphy->bands[IEEE80211_BAND_5GHZ];
        for (i = 0; i < b->num_channels; i++)
        {
            req->chan5G[req->chan5G_cnt].flags = 0;

            if (b->channels[i].flags & IEEE80211_CHAN_DISABLED)
                req->chan5G[req->chan5G_cnt].flags |= SCAN_DISABLED_BIT;

            if (b->channels[i].flags & IEEE80211_CHAN_NO_IR)
                req->chan5G[req->chan5G_cnt].flags |= SCAN_PASSIVE_BIT;

            req->chan5G[req->chan5G_cnt].band = IEEE80211_BAND_5GHZ;
            req->chan5G[req->chan5G_cnt].freq = b->channels[i].center_freq;
            req->chan5G_cnt++;
            if (req->chan5G_cnt == SCAN_CHANNEL_5G)
                break;
        }
    }
    /* Send the ME_CHAN_CONFIG_REQ message to LMAC FW */
    return rw_msg_send(req, ME_CHAN_CONFIG_CFM, NULL);
}

int rw_msg_send_add_if(const unsigned char *mac,
                       enum nl80211_iftype iftype, 
                       bool p2p, 
                       struct mm_add_if_cfm *cfm)
{
    struct mm_add_if_req *add_if_req_param;

    /* Build the ADD_IF_REQ message */
    add_if_req_param = ke_msg_alloc(MM_ADD_IF_REQ, TASK_MM, TASK_API,
                                    sizeof(struct mm_add_if_req));
    if (!add_if_req_param)
        return -1;

    /* Set parameters for the ADD_IF_REQ message */
    os_memcpy(&(add_if_req_param->addr.array[0]), mac, ETH_ALEN);
    switch (iftype)
    {
    case NL80211_IFTYPE_STATION:
        add_if_req_param->type = VIF_STA;
        break;

    case NL80211_IFTYPE_ADHOC:
        add_if_req_param->type = VIF_IBSS;
        break;

    case NL80211_IFTYPE_AP:
        add_if_req_param->type = VIF_AP;
        break;
		
    case NL80211_IFTYPE_MESH_POINT:
        add_if_req_param->type = VIF_MESH_POINT;
        break;

    default:
        add_if_req_param->type = VIF_STA;
        break;
    }

    add_if_req_param->p2p = p2p;

    /* Send the ADD_IF_REQ message to LMAC FW */
    return rw_msg_send( add_if_req_param, MM_ADD_IF_CFM, cfm);
}

int rw_msg_send_remove_if(u8 vif_index)
{
    struct mm_remove_if_req *remove_if_req;

    /* Build the MM_REMOVE_IF_REQ message */
    remove_if_req = ke_msg_alloc(MM_REMOVE_IF_REQ, TASK_MM, TASK_API,
                                 sizeof(struct mm_remove_if_req));
    if (!remove_if_req)
        return -1;

    /* Set parameters for the MM_REMOVE_IF_REQ message */
    remove_if_req->inst_nbr = vif_index;

    /* Send the MM_REMOVE_IF_REQ message to LMAC FW */
    return rw_msg_send(remove_if_req, MM_REMOVE_IF_CFM, NULL);
}

int rw_msg_send_apm_start_req(u8 vif_index, u8 channel,
                     struct apm_start_cfm *cfm)
{
    struct apm_start_req *req;

    /* Build the APM_START_REQ message */
    req = ke_msg_alloc(APM_START_REQ, TASK_APM, TASK_API,
                       sizeof(struct apm_start_req));
    if (!req)
        return -1;

    req->basic_rates.length = 4;
    req->basic_rates.array[0] = 130;
    req->basic_rates.array[1] = 132;
    req->basic_rates.array[2] = 139;
    req->basic_rates.array[3] = 150;

    req->chan.band = 0;
    req->chan.flags = 0;
	req->chan.freq = rw_ieee80211_get_centre_frequency(channel);
	req->center_freq1 = rw_ieee80211_get_centre_frequency(channel);
    req->center_freq2 = 0;
    req->ch_width = 0;

    req->bcn_addr = (UINT32)beacon;
    req->bcn_len = sizeof(beacon);
    req->tim_oft = 56;
    req->tim_len = 6;
    req->bcn_int = BEACON_INTERVAL;

    if(g_ap_param_ptr->cipher_suite > SECURITY_TYPE_WEP)
    {
        req->flags = WPA_WPA2_IN_USE;
        req->flags |= CONTROL_PORT_HOST;
    }
    else
    {
        req->flags = 0;
    }

    req->ctrl_port_ethertype = PP_HTONS(ETH_P_PAE);
    req->vif_idx = vif_index;

    os_printf("apm start with vif:%d\r\n", vif_index);

    /* Send the APM_START_REQ message to LMAC FW */
    return rw_msg_send(req, APM_START_CFM, cfm);
}

int rw_msg_send_apm_stop_req(u8 vif_index)
{
    struct apm_stop_req *req;
	struct apm_stop_cfm *cfm
			= (struct apm_stop_cfm *)os_malloc(sizeof(struct apm_stop_cfm));
	int ret;
    /* Build the APM_STOP_REQ message */
    req = ke_msg_alloc(APM_STOP_REQ, TASK_APM, TASK_API,
                       sizeof(struct apm_stop_req));
    if ((!req) || (!cfm))
    {
    	if(cfm)
			os_free(cfm);
		if(req)
			os_free(req);
        return -1;
    }

    /* Set parameters for the APM_STOP_REQ message */
    req->vif_idx = vif_index;
	bk_printf("[msg]rw_msg_send_apm_stop_req\n");

    /* Send the APM_STOP_REQ message to LMAC FW */
    //ret = rw_msg_send(req, APM_STOP_CFM, NULL);
    ret = rw_msg_send(req, APM_STOP_CFM, cfm);
	if(cfm->status)
	{
		bk_printf("[T]APM_STOP_CFM\n");
	}
	os_free(cfm);
	return ret;
}

int rw_msg_send_bcn_change(void *bcn_param)
{
    struct mm_bcn_change_req *req;
    BCN_PARAM_PTR param = (BCN_PARAM_PTR)bcn_param;

    /* Build the MM_BCN_CHANGE_REQ message */
    req = ke_msg_alloc(MM_BCN_CHANGE_REQ, TASK_MM, TASK_API,
                       sizeof(struct mm_bcn_change_req));
    if (!req)
        return -1;

    /* Set parameters for the MM_BCN_CHANGE_REQ message */
    req->bcn_ptr = (u32)(param->bcn_ptr);
    req->bcn_len = param->bcn_len;
    req->tim_len = param->tim_len;
    req->tim_oft = param->tim_oft;
    req->inst_nbr = param->vif_idx;
    req->bcn_ptr_malloc_flag = param->flag;

    if (param->csa_oft)
    {
        for (int i = 0; i < BCN_MAX_CSA_CPT; i++)
        {
            req->csa_oft[i] = param->csa_oft[i];
        }
    }

    /* Send the MM_BCN_CHANGE_REQ message to LMAC FW */
    return rw_msg_send(req, MM_BCN_CHANGE_CFM, NULL);
}

int rw_msg_send_me_sta_add(struct add_sta_st *param,
                           struct me_sta_add_cfm *cfm)
{
    struct me_sta_add_req *req;

    /* Build the MM_STA_ADD_REQ message */
    req = ke_msg_alloc(ME_STA_ADD_REQ, TASK_ME, TASK_API,
                       sizeof(struct me_sta_add_req));
    if (!req)
        return -1;

    /* Set parameters for the MM_STA_ADD_REQ message */
    os_memcpy(&(req->mac_addr.array[0]), param->sta_addr, ETH_ALEN);

    req->vif_idx = param->ap_vif_idx;
    req->aid = param->aid;
    req->flags = 0x01; // 1:STA_QOS_CAPA 2: STA_HT_CAPA

    os_printf("hapd_intf_sta_add:%d, vif:%d\r\n", req->aid, req->vif_idx);

    req->rate_set.length = 12;
    req->rate_set.array[0] = 130;
    req->rate_set.array[1] = 132;
    req->rate_set.array[2] = 139;
    req->rate_set.array[3] = 150;
    req->rate_set.array[4] = 36;
    req->rate_set.array[5] = 48;
    req->rate_set.array[6] = 72;
    req->rate_set.array[7] = 108;
    req->rate_set.array[8] = 12;
    req->rate_set.array[9] = 18;
    req->rate_set.array[10] = 24;
    req->rate_set.array[11] = 96;

    req->ht_cap.mcs_rate[0] = 255;

    /* Send the ME_STA_ADD_REQ message to LMAC FW */
    return rw_msg_send(req, ME_STA_ADD_CFM, cfm);
}

int rw_msg_send_me_sta_del(u8 sta_idx, bool tdls_sta)
{
    struct me_sta_del_req *req;

    /* Build the MM_STA_DEL_REQ message */
    req = ke_msg_alloc(ME_STA_DEL_REQ, TASK_ME, TASK_API,
                       sizeof(struct me_sta_del_req));
    if (!req)
        return -1;

    /* Set parameters for the MM_STA_DEL_REQ message */
    req->sta_idx = sta_idx;
    req->tdls_sta = tdls_sta;

    /* Send the ME_STA_DEL_REQ message to LMAC FW */
    return rw_msg_send(req, ME_STA_DEL_CFM, NULL);
}

int rw_msg_me_set_control_port_req(bool opened, u8 sta_idx)
{
    struct me_set_control_port_req *req;

    /* Build the ME_SET_CONTROL_PORT_REQ message */
    req = ke_msg_alloc(ME_SET_CONTROL_PORT_REQ, TASK_ME, TASK_API,
                       sizeof(struct me_set_control_port_req));
    if (!req)
        return -1;

    /* Set parameters for the ME_SET_CONTROL_PORT_REQ message */
    req->sta_idx = sta_idx;
    req->control_port_open = opened;

    /* Send the ME_SET_CONTROL_PORT_REQ message to LMAC FW */
    return rw_msg_send(req, ME_SET_CONTROL_PORT_CFM, NULL);
}

int rw_msg_send_key_add(KEY_PARAM_T *param, struct mm_key_add_cfm *cfm)
{
    struct mm_key_add_req *key_add_req;

    /* Build the MM_KEY_ADD_REQ message */
    key_add_req = ke_msg_alloc(MM_KEY_ADD_REQ, TASK_MM, TASK_API,
                               sizeof(struct mm_key_add_req));
    if (!key_add_req)
        return -1;

    /* Set parameters for the MM_KEY_ADD_REQ message */
    if (param->sta_idx != 0xFF)
    {
        /* Pairwise key */
        key_add_req->sta_idx = param->sta_idx;
    }
    else
    {
        /* Default key */
        key_add_req->sta_idx = param->sta_idx;
        key_add_req->key_idx = param->key_idx; /* only useful for default keys */
    }

    key_add_req->spp = 0;
    key_add_req->pairwise = 0;
    key_add_req->inst_nbr = param->inst_nbr;
    key_add_req->key.length = param->key.length;
    os_memcpy(&(key_add_req->key.array[0]), &(param->key.array[0]), param->key.length);

    key_add_req->cipher_suite = param->cipher_suite;

    /* Send the MM_KEY_ADD_REQ message to LMAC FW */
    return rw_msg_send(key_add_req, MM_KEY_ADD_CFM, cfm);
}

int rw_msg_send_key_del(u8 hw_key_idx)
{
    struct mm_key_del_req *key_del_req;

    /* Build the MM_KEY_DEL_REQ message */
    key_del_req = ke_msg_alloc(MM_KEY_DEL_REQ, TASK_MM, TASK_API,
                               sizeof(struct mm_key_del_req));
    if (!key_del_req)
        return -1;

    /* Set parameters for the MM_KEY_DEL_REQ message */
    key_del_req->hw_key_idx = hw_key_idx;

    /* Send the MM_KEY_DEL_REQ message to LMAC FW */
    return rw_msg_send(key_del_req, MM_KEY_DEL_CFM, NULL);
}

int rw_msg_send_scanu_req(SCAN_PARAM_T *scan_param)
{
    struct scanu_start_req *req;
    int i;
    UINT32 channel_id;
	
#if CFG_ROLE_LAUNCH
		if(rl_pre_sta_set_status(RL_STATUS_STA_SCANNING))
		{
			return 0;
		}
#endif   

    /* Build the SCANU_START_REQ message */
    req = ke_msg_alloc(SCANU_START_REQ, TASK_SCANU, TASK_API,
                       sizeof(struct scanu_start_req));
    if (!req)
        return -1;

    /* Set parameters */
    req->vif_idx = scan_param->vif_idx;
    req->no_cck = 0;

    channel_id = 0;
    for(i = 0; i < g_wiphy.bands[IEEE80211_BAND_2GHZ].num_channels; i ++)
    {
        req->chan[i].band = IEEE80211_BAND_2GHZ;
        req->chan[i].flags = 0;
        req->chan[i].freq = rw_ieee80211_get_centre_frequency(i + 1);

        channel_id ++;
    }

#ifdef ENABLE_5GHZ_IEEE80211
    for(i = 0; i < g_wiphy.bands[IEEE80211_BAND_5GHZ].num_channels; i ++)
    {
        req->chan[i].band = IEEE80211_BAND_5GHZ;
        req->chan[i].flags = 0;
        req->chan[i].freq = rw_ieee80211_get_centre_frequency(i
                            + SCAN_CHANNEL_2G4
                            + 1);

        ASSERT(req->chan[i].freq);
        channel_id ++;
    }
#endif

    req->chan_cnt = channel_id;

    os_memcpy(&req->bssid, &scan_param->bssid, sizeof(req->bssid));
    req->ssid_cnt = scan_param->num_ssids;
    for(i = 0; i < req->ssid_cnt; i++)
    {
        req->ssid[i].length = scan_param->ssids[i].length;
        os_memcpy(req->ssid[i].array, scan_param->ssids[i].array, req->ssid[i].length);
    }

    req->add_ies = 0;
    req->add_ie_len = 0;

    /* Send the SCANU_START_REQ message to LMAC FW */
    return rw_msg_send(req, SCANU_START_CFM, NULL);
}

int rw_msg_send_scanu_fast_req(FAST_SCAN_PARAM_T *fscan_param)
{
    struct scanu_fast_req *req;

    /* Build the SCANU_START_REQ message */
    req = ke_msg_alloc(SCANU_FAST_REQ, TASK_SCANU, TASK_API,
                       sizeof(struct scanu_fast_req));

    req->bssid = fscan_param->bssid;
    req->ch_nbr = fscan_param->ch_num;
    req->maxch_time = fscan_param->max_ch_time;
    req->minch_time = fscan_param->min_ch_time;
    req->probe_delay = fscan_param->probe_delay;
    req->ssid = fscan_param->ssid;

    return rw_msg_send(req, SCANU_FAST_CFM, NULL);
}

int rw_msg_send_connection_loss_ind(u8 vif_index)
{
    struct mm_connection_loss_ind *ind = ke_msg_alloc(MM_CONNECTION_LOSS_IND,
                                         TASK_SM, TASK_API, sizeof(struct mm_connection_loss_ind));

    // Fill-in the indication message parameters
    ind->inst_nbr = vif_index;

    // Send the indication to the upper layers
    return rw_msg_send(ind, 0, NULL);
}

int rw_msg_get_bss_info(u8 vif_idx, void *cfm)
{
    struct sm_get_bss_info_req *req = NULL;

    if(vif_idx >= NX_VIRT_DEV_MAX)
        return -1;

    req = ke_msg_alloc(SM_GET_BSS_INFO_REQ, TASK_SM, TASK_API,
                       sizeof(struct sm_get_bss_info_req));

    if (!req)
        return -1;

    req->vif_idx = vif_idx;

    return rw_msg_send(req, SM_GET_BSS_INFO_CFM, cfm);
}

int rw_msg_get_channel(void *cfm)
{
    struct phy_channel_info info;

    phy_get_channel(&info, 0);

    return info.info2;
}

int rw_msg_set_filter(uint32_t filter)
{
    struct mm_set_filter_req *set_filter_req_param;
    uint32_t rx_filter = 0;

    /* Build the MM_SET_FILTER_REQ message */
    set_filter_req_param =
        ke_msg_alloc(MM_SET_FILTER_REQ, TASK_MM, TASK_API,
                     sizeof(struct mm_set_filter_req));
    if (!set_filter_req_param)
        return -1;

    /* Now copy all the flags into the message parameter */
    set_filter_req_param->filter = rx_filter;


    /* Send the MM_SET_FILTER_REQ message to LMAC FW */
    return rw_msg_send(set_filter_req_param, MM_SET_FILTER_CFM, NULL);
}

int rw_msg_set_channel(uint32_t channel, uint32_t band_width, void *cfm)
{
    struct mm_set_channel_req *set_chnl_par;

    set_chnl_par = ke_msg_alloc(MM_SET_CHANNEL_REQ, TASK_MM, TASK_API,
                                sizeof(struct mm_set_channel_req));
    if (!set_chnl_par)
        return -ENOMEM;

    set_chnl_par->band = PHY_BAND_2G4;
    set_chnl_par->type = band_width;//PHY_CHNL_BW_20;
    set_chnl_par->prim20_freq = set_chnl_par->center1_freq
                                = rw_ieee80211_get_centre_frequency(channel);
    set_chnl_par->center2_freq = 0;
    set_chnl_par->index = PHY_PRIM;
    set_chnl_par->tx_power = 0;
    set_chnl_par->index = PHY_SEC;

    /* Send the MM_SET_CHANNEL_REQ REQ message to LMAC FW */
    return rw_msg_send(set_chnl_par, MM_SET_CHANNEL_CFM, cfm);
}

int rw_msg_send_scan_cancel_req(void *cfm)
{
    struct scan_cancel_req *req;

    /* Build the SCAN_CANCEL_REQ message */
    req = ke_msg_alloc(SCAN_CANCEL_REQ, TASK_SCAN, TASK_API,
                       sizeof(struct scan_cancel_req));
    if (!req)
        return -1;

    /* Send the SCAN_CANCEL_REQ message to LMAC FW */
    return rw_msg_send(req, SCAN_CANCEL_CFM, cfm);
}

int rw_msg_send_sm_disconnect_req(DISCONNECT_PARAM_T *param)
{
    struct sm_disconnect_req *req;

    /* Build the SM_DISCONNECT_REQ message */
    req = ke_msg_alloc(SM_DISCONNECT_REQ, TASK_SM, TASK_API,
                       sizeof(struct sm_disconnect_req));
    if (!req)
        return -1;

    /* Set parameters for the SM_DISCONNECT_REQ message */
    req->reason_code = param->reason_code;
    req->vif_idx = param->vif_idx;

    return rw_msg_send(req, SM_DISCONNECT_CFM, NULL);
}

int rw_msg_send_sm_connect_req( CONNECT_PARAM_T *sme, void *cfm)
{
    struct sm_connect_req *req;

    /* Build the SM_CONNECT_REQ message */
    req = ke_msg_alloc(SM_CONNECT_REQ, TASK_SM, TASK_API,
                       sizeof(struct sm_connect_req));
    if (!req)
        return -1;
	
	ke_msg_send_basic(SM_CONNCTION_START_IND, TASK_API, TASK_SM);
	
    /* Set parameters for the SM_CONNECT_REQ message */
    req->ssid.length = sme->ssid.length;
    os_memcpy(req->ssid.array, sme->ssid.array, sme->ssid.length);
    os_memcpy(&req->bssid, &sme->bssid, sizeof(sme->bssid));

    req->vif_idx = sme->vif_idx;
    req->chan.band = sme->chan.band;
    req->chan.flags = sme->chan.flags;
    req->chan.freq = sme->chan.freq;
    req->flags = sme->flags;
    req->ctrl_port_ethertype = PP_HTONS(ETH_P_PAE);
    req->ie_len = sme->ie_len;
    req->auth_type = sme->auth_type;
    os_memcpy((UINT8 *)req->ie_buf, (UINT8 *)sme->ie_buf, req->ie_len);

    /* Send the SM_CONNECT_REQ message to LMAC FW */
    return rw_msg_send(req, SM_CONNECT_CFM, cfm);
	
}

int rw_msg_send_tim_update(u8 vif_idx, u16 aid, u8 tx_status)
{
    struct mm_tim_update_req *req;

    /* Build the MM_TIM_UPDATE_REQ message */
    req = ke_msg_alloc(MM_TIM_UPDATE_REQ, TASK_MM, TASK_API,
                          sizeof(struct mm_tim_update_req));
    if (!req)
        return -1;

    /* Set parameters for the MM_TIM_UPDATE_REQ message */
    req->aid = aid;
    req->tx_avail = tx_status;
    req->inst_nbr = vif_idx;

    /* Send the MM_TIM_UPDATE_REQ message to LMAC FW */
    return rw_msg_send(req, MM_TIM_UPDATE_CFM, NULL);
}

int rw_msg_set_power(u8 vif_idx, u8 power)
{
    struct mm_set_power_req *req;

    /* Build the MM_SET_POWER_REQ message */
    req = ke_msg_alloc(MM_SET_POWER_REQ, TASK_MM, TASK_NONE,
                          sizeof(struct mm_set_power_req));
    if (!req)
        return -1;

    /* Set parameters for the MM_SET_POWER_REQ message */
    req->inst_nbr = vif_idx;
    req->power = power;

    /* Send the MM_SET_POWER_REQ message to LMAC FW */
    return rw_msg_send(req, MM_SET_POWER_CFM, NULL);
}
// eof

