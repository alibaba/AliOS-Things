#ifndef _RW_PUB_H_
#define _RW_PUB_H_

#include "ke_msg.h"
#include "uart_pub.h"
#include "rw_ieee80211.h"
#include "rtos_pub.h"
#include "rw_ieee80211.h"
#include "apm_task.h"
#include "me_task.h"
#include "mm_task.h"
#include "mm.h"
#include "lwip/netif.h"
#include "vif_mgmt.h"

#define RWI_DEBUG

#ifdef RWI_DEBUG
#define RWI_PRT                 os_printf
#define RWI_WPRT                warning_prf
#else
#define RWI_PRT                 os_null_printf
#define RWI_WPRT                os_null_printf
#endif

#define RW_SUCCESS                 (0)
#define RW_FAILURE                 ((UINT32)-1)

typedef struct cfg80211_connect_params
{
    uint32_t flags;
    uint32_t vif_idx;
	uint8_t auth_type;
    struct mac_addr bssid;
    struct mac_ssid ssid;
    struct scan_chan_tag chan;
    uint16_t ie_len;
    uint32_t ie_buf[64];
} CONNECT_PARAM_T;

typedef struct cfg80211_scan_params
{
    uint8_t num_ssids;
    uint8_t vif_idx;
    struct mac_ssid ssids[SCAN_SSID_MAX];
    struct mac_addr bssid;
} SCAN_PARAM_T;

typedef struct cfg80211_fast_scan_params
{
    struct mac_ssid ssid;
    struct mac_addr bssid;
	
    uint16_t probe_delay;
    uint16_t min_ch_time;
    uint16_t max_ch_time;
    uint16_t ch_num;
}FAST_SCAN_PARAM_T;

typedef struct cfg80211_disconnect_params
{
    uint16_t reason_code;
    uint8_t vif_idx;
}DISCONNECT_PARAM_T;

typedef struct scanu_rst_upload
{
    UINT8 scanu_num;
    struct sta_scan_res **res;
}SCAN_RST_UPLOAD_T, *SCAN_RST_UPLOAD_PTR;


typedef struct sta_scan_res
{
    UINT8 bssid[6];
    char ssid[32];  /**< The SSID of an access point. */
    char on_channel; // 1: ds IE channel=center_freq, 0: !=   
    char channel;
    UINT16 beacon_int;
    UINT16 caps;
    int level;
	int security; // security type
    UINT8 tsf[8];
    UINT32 ie_len;
    /* Followed by ie_len of IE data */
}SCAN_RST_ITEM_T, *SCAN_RST_ITEM_PTR;


typedef struct 
{  
    char ssid[MAC_SSID_LEN]; 
    char ap_power; 
}AP_INFO_T;

typedef  struct
{  
	uint16_t ap_num;   
	uint16_t ap_max;  
  
   AP_INFO_T *ap_list; 
} SCAN_RESULT_SET; 

typedef struct
{
	FUNC_2PARAM_PTR cb;
	void *ctxt_arg;
}IND_CALLBACK_T;

typedef struct
{
    int8_t rssi;
    uint8_t ssid[32];
    uint8_t bssid[6];
    uint16_t freq;
}BSS_INFO_T;

typedef enum {
	MSG_IDLE = 0,
	MSG_CONNECTING,
	MSG_PASSWD_WRONG,
	MSG_NO_AP_FOUND,
	MSG_CONN_FAIL,
	MSG_CONN_SUCCESS,
	MSG_GOT_IP,
}msg_sta_states;

enum nl80211_iftype {
	NL80211_IFTYPE_UNSPECIFIED,
	NL80211_IFTYPE_ADHOC,
	NL80211_IFTYPE_STATION,
	NL80211_IFTYPE_AP,
	NL80211_IFTYPE_AP_VLAN,
	NL80211_IFTYPE_WDS,
	NL80211_IFTYPE_MONITOR,
	NL80211_IFTYPE_MESH_POINT,
	NL80211_IFTYPE_P2P_CLIENT,
	NL80211_IFTYPE_P2P_GO,
	NL80211_IFTYPE_P2P_DEVICE,
	NL80211_IFTYPE_OCB,

	/* keep last */
	NUM_NL80211_IFTYPES,
	NL80211_IFTYPE_MAX = NUM_NL80211_IFTYPES - 1
};

enum rw_evt_type
{
    /* for station mode */
    RW_EVT_STA_CONNECTED = 0,    /* authentication success */
    RW_EVT_STA_DISCONNECTED,    /* disconnect with server */
    RW_EVT_STA_CONNECT_FAILED, /* authentication failed */
    /* for softap mode */
    RW_EVT_AP_CONNECTED,          /* a client association success */
    RW_EVT_AP_DISCONNECTED,    /* a client disconnect */
    RW_EVT_AP_CONNECT_FAILED, /* a client association failed */
    RW_EVT_MAX
};

typedef int (*rw_event_handler)(enum rw_evt_type evt_type, void *data);

struct rw_evt_payload
{
    uint8_t mac[6];
};

struct add_sta_st {
	u16 aid;
	u16 capability;
    void *sta_addr;
	u8 tx_supp_rates;
    u8 ap_vif_idx;
} ;

typedef struct cfg80211_key_params
{
    uint8_t cipher_suite;
    uint8_t sta_idx;
    uint8_t inst_nbr;
    uint8_t key_idx;
    uint8_t hw_key_idx;
    struct mac_sec_key key;
} KEY_PARAM_T;

typedef struct msg_send_node
{
    struct co_list_hdr hdr;
    struct ke_msg *msg;
    void *cfm;
    beken_semaphore_t semaphore;
    uint16_t reqid;
}MSG_SND_NODE_ST, *MSG_SND_NODE_PTR;

typedef struct bcn_param_st {
    u32 *bcn_ptr;
    u16 bcn_len;
    u16 tim_oft;
    
    u8 tim_len;
    u8 vif_idx; 
    u16 flag;  
    
    u8* csa_oft;
} BCN_PARAM_ST, *BCN_PARAM_PTR;

typedef struct vif_info_tag VIF_INF_ST;
typedef struct vif_info_tag* VIF_INF_PTR;

typedef struct sta_info_tag STA_INF_ST;
typedef struct sta_info_tag* STA_INF_PTR;

extern struct co_list rw_msg_tx_head;
extern struct co_list rw_msg_rx_head;

extern void mt_msg_dispatch(UINT16 cmd, void *param);
extern void mr_kmsg_flush(void);
extern UINT32 mr_kmsg_fwd(struct ke_msg *msg);
extern UINT32 mr_kmsg_fuzzy_handle(void);
extern UINT32 mr_kmsg_exact_handle(UINT16 rsp);
extern void mhdr_assoc_cfm_cb(FUNC_2PARAM_PTR ind_cb, void *ctxt);
extern void mhdr_scanu_reg_cb(FUNC_2PARAM_PTR ind_cb, void *ctxt);
extern void mhdr_connect_user_cb(FUNC_2PARAM_PTR ind_cb, void *ctxt);
extern UINT32 rw_ieee80211_init(void);
extern UINT32 rw_ieee80211_get_centre_frequency(UINT32 chan_id);
extern UINT8 rw_ieee80211_get_chan_id(UINT32 freq);
extern void *sr_get_scan_results(void);
extern void sr_release_scan_results(SCAN_RST_UPLOAD_PTR ptr);
extern UINT32 rwm_transfer(UINT8 vif_idx, UINT8 *buf, UINT32 len);
extern void* rwm_transfer_pre(UINT8 vif_idx, UINT8 *buf, UINT32 len);
extern UINT32 rwm_uploaded_data_handle(UINT8 *upper_buf, UINT32 len);
extern UINT32 rwm_get_rx_valid_node_len(void);
extern void mhdr_set_station_status(msg_sta_states val);
extern msg_sta_states mhdr_get_station_status(void);

//
extern void user_callback_func_register(FUNC_1PARAM_PTR sta_connect_start_func,
								 FUNC_1PARAM_PTR connection_lost_func,
								 FUNC_1PARAM_PTR auth_fail_func,
								 FUNC_1PARAM_PTR assoc_fail_func );
extern void user_callback_func_unregister(void);
extern void rw_evt_set_callback(enum rw_evt_type evt_type, rw_event_handler handler);


extern int rw_msg_send(const void *msg_params, uint16_t reqid, void *cfm);
extern int rw_msg_send_reset(void);
extern int rw_msg_send_start(void);
extern int rw_msg_send_me_config_req(void);
extern int rw_msg_send_me_chan_config_req(void);
extern int rw_msg_send_add_if(const unsigned char *mac,
                     enum nl80211_iftype iftype, bool p2p, struct mm_add_if_cfm *cfm);
extern int rw_msg_send_remove_if(u8 vif_index);
extern int rw_msg_send_apm_start_req(u8 vif_index, u8 channel,
                     struct apm_start_cfm *cfm);
extern int rw_msg_send_bcn_change(void *bcn_param);
extern int rw_msg_send_me_sta_add(struct add_sta_st *param, 
                     struct me_sta_add_cfm *cfm);
extern int rw_msg_send_me_sta_del(u8 sta_idx, bool tdls_sta);
extern int rw_msg_me_set_control_port_req(bool opened, u8 sta_idx);
extern int rw_msg_send_key_add(KEY_PARAM_T *param, struct mm_key_add_cfm *cfm);
extern int rw_msg_send_key_del(u8 hw_key_idx);
extern int rw_msg_send_scanu_req(SCAN_PARAM_T *scan_param);
extern int rw_msg_send_connection_loss_ind(u8 vif_index);
extern int rw_msg_get_bss_info(u8 vif_idx, void *cfm);
extern int rw_msg_get_channel(void *cfm);
extern int rw_msg_set_filter(uint32_t filter);
extern int rw_msg_set_channel(uint32_t channel, uint32_t band_width, void *cfm);
extern int rw_msg_send_scan_cancel_req(void *cfm);
extern int rw_msg_send_sm_disconnect_req(DISCONNECT_PARAM_T *param);
extern int rw_msg_send_sm_connect_req( CONNECT_PARAM_T *sme, void *cfm);
extern int rw_msg_send_tim_update(u8 vif_idx, u16 aid, u8 tx_status);
extern int rw_msg_send_apm_stop_req(u8 vif_index);
extern int rw_msg_set_power(u8 vif_idx, u8 power);



VIF_INF_PTR rwm_mgmt_vif_idx2ptr(UINT8 vif_idx);
VIF_INF_PTR rwm_mgmt_vif_type2ptr(UINT8 vif_type);
STA_INF_PTR rwm_mgmt_sta_idx2ptr(UINT8 staid);
STA_INF_PTR rwm_mgmt_sta_mac2ptr(void *mac);
UINT8 rwm_mgmt_sta_mac2idx(void *mac);
UINT8 rwm_mgmt_vif_mac2idx(void *mac);
UINT8 rwm_mgmt_vif_name2idx(char *name);
UINT8 rwm_mgmt_get_hwkeyidx(UINT8 vif_idx, UINT8 staid);
void rwm_mgmt_set_vif_netif(struct netif *net_if);
struct netif *rwm_mgmt_get_vif2netif(UINT8 vif_idx);
UINT8 rwm_mgmt_get_netif2vif(struct netif *netif);
UINT8 rwm_mgmt_tx_get_staidx(UINT8 vif_idx, void *dstmac);
u8 rwn_mgmt_is_only_sta_role_add(void);

void rwm_msdu_init(void);
void rwm_flush_txing_list(UINT8 sta_idx);
void rwm_msdu_ps_change_ind_handler(void *msg) ;
void rwm_msdu_send_txing_node(UINT8 sta_idx);

__INLINE u8 rwm_mgmt_is_ap_inface(u8 vif_idx)
{
    return (vif_info_tab[vif_idx].type == VIF_AP);
}

__INLINE u8 rwm_mgmt_is_sta_inface(u8 vif_idx)
{
    return (vif_info_tab[vif_idx].type == VIF_STA);
}

__INLINE void *rwm_mgmt_is_vif_first_used(void)
{
    return((void *)co_list_pick(&vif_mgmt_env.used_list));
}

__INLINE void *rwm_mgmt_next(VIF_INF_PTR vif_entry)
{
    return((void *)co_list_next(&vif_entry->list_hdr));
}

__INLINE u8 rwm_mgmt_get_type(u8 vif_idx)
{
    return (vif_info_tab[vif_idx].type);
}

__INLINE void *rwm_mgmt_get_addr(u8 vif_idx)
{
    return (&vif_info_tab[vif_idx].mac_addr);
}


extern UINT8 beacon[149];


#endif //_RW_PUB_H_
// eof

