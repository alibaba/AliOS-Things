#ifndef TLS_WIRELESS_H
#define TLS_WIRELESS_H

#include "tls_common.h"
#include "wm_osal.h"
#include "list.h"
//#include "netif.h"

struct tls_wif;
struct wpa_supplicant;

/* Maximum size of the SSID  */
#define IW_SSID_MAX_SIZE	32
#if 0
#define IEEE80211_MODE_INFRA	0
#define IEEE80211_MODE_IBSS	    1
#define IEEE80211_MODE_AP	    2
#endif
//#define IEEE80211_RATE_MODE_B      BIT(0)
//#define IEEE80211_RATE_MODE_G      BIT(1)
//#define IEEE80211_RATE_MODE_BG     BIT(2)
//#define IEEE80211_RATE_MODE_N      BIT(3)
//#define IEEE80211_RATE_MODE_BGN    BIT(4)

enum ieee80211_wireless_mode {
	IEEE80211_MODE_11B = 0,
    IEEE80211_MODE_11G,
    IEEE80211_MODE_11NG_HT20,
    IEEE80211_MODE_11NG_HT40PLUS,
    IEEE80211_MODE_11NG_HT40MINUS,
    IEEE80211_MODE_MAX,
};

#define IW_AUTH_INDEX		0x0FFF
#define IW_AUTH_FLAGS		0xF000

#define IW_AUTH_WPA_VERSION		0
#define IW_AUTH_CIPHER_PAIRWISE		1
#define IW_AUTH_CIPHER_GROUP		2
#define IW_AUTH_KEY_MGMT		3
#define IW_AUTH_TKIP_COUNTERMEASURES	4
#define IW_AUTH_DROP_UNENCRYPTED	5
#define IW_AUTH_80211_AUTH_ALG		6
#define IW_AUTH_WPA_ENABLED		7
#define IW_AUTH_RX_UNENCRYPTED_EAPOL	8
#define IW_AUTH_ROAMING_CONTROL		9
#define IW_AUTH_PRIVACY_INVOKED		10
#define IW_AUTH_CIPHER_GROUP_MGMT	11
#define IW_AUTH_MFP			12

/* IW_AUTH_WPA_VERSION values (bit field) */
#define IW_AUTH_WPA_VERSION_DISABLED	0x00000001
#define IW_AUTH_WPA_VERSION_WPA		0x00000002
#define IW_AUTH_WPA_VERSION_WPA2	0x00000004

/* IW_AUTH_PAIRWISE_CIPHER, IW_AUTH_GROUP_CIPHER, and IW_AUTH_CIPHER_GROUP_MGMT
 * values (bit field) */
#define IW_AUTH_CIPHER_NONE	0x00000001
#define IW_AUTH_CIPHER_WEP40	0x00000002
#define IW_AUTH_CIPHER_TKIP	0x00000004
#define IW_AUTH_CIPHER_CCMP	0x00000008
#define IW_AUTH_CIPHER_WEP104	0x00000010
#define IW_AUTH_CIPHER_AES_CMAC	0x00000020

/* IW_AUTH_KEY_MGMT values (bit field) */
#define IW_AUTH_KEY_MGMT_802_1X	1
#define IW_AUTH_KEY_MGMT_PSK	2

/* IW_AUTH_80211_AUTH_ALG values (bit field) */
#define IW_AUTH_ALG_OPEN_SYSTEM	0x00000001
#define IW_AUTH_ALG_SHARED_KEY	0x00000002
#define IW_AUTH_ALG_LEAP	0x00000004

#define IW_ENCODE_ALG_NONE	0
#define IW_ENCODE_ALG_WEP	1
#define IW_ENCODE_ALG_TKIP	2
#define IW_ENCODE_ALG_CCMP	3
#define IW_ENCODE_ALG_PMK	4
#define IW_ENCODE_ALG_AES_CMAC	5

#define IW_MAX_FREQUENCIES	32

#define IW_SCAN_TYPE_ACTIVE   0
#define IW_SCAN_TYPE_PASSIVE  1

struct	iw_scan_req {
    u8		scan_type; /* IW_SCAN_TYPE_{ACTIVE,PASSIVE} */
	u8 ssid_len;
    u8		num_channels; /* num entries in channel_list;
                           * 0 = scan all allowed channels */
    u8		bssid[ETH_ALEN]; /* ff:ff:ff:ff:ff:ff for broadcast BSSID or
                              * individual address of a specific BSS */

    u8		ssid[IW_SSID_MAX_SIZE];
    u32     extra_ies_len;
    u8      extra_ies[0];

	//struct iw_freq	channel_list[IW_MAX_FREQUENCIES];
};

struct iw_bssid_ex {
    u32     length;
    u8      bssid[ETH_ALEN];
    u16     freq;
    u32     ssid_len;
    u8      ssid[IW_SSID_MAX_SIZE];
    u32     privacy;
    int     rssi;
    u8      tsf[8];
    u16     beacon_int;
    u16     capabilities;
    u32     ie_len;
    u8      ies[0];
} __attribute__ ((packed));

struct iw_bss_info {
    u8      bssid[ETH_ALEN];
    u8      mode;
    u8      channel;
    u8      privacy;
    u8      ssid_len;
    u8      rssi;
    u8      ssid[32]; 
};

struct iw_scan_results {
    u32     count;
    struct iw_bssid_ex   bssid[1];
};

struct iw_scan_bss {
    u32     count;
    u32     length; /* bss info total length */
    struct iw_bss_info bss[1];
};

/**
 * struct beacon_parameters - beacon parameters
 *
 * Used to configure the beacon for an interface.
 *
 * @head: head portion of beacon (before TIM IE)
 *     or %NULL if not changed
 * @tail: tail portion of beacon (after TIM IE)
 *     or %NULL if not changed
 * @interval: beacon interval or zero if not changed
 * @dtim_period: DTIM period or zero if not changed
 * @head_len: length of @head
 * @tail_len: length of @tail
 */
struct iw_beacon_parameters {
	u8 *head, *tail;
	int interval, dtim_period;
	int head_len, tail_len;
};


struct iw_ssid_params {
    u8      ssid[IW_SSID_MAX_SIZE];
    u8      ssid_len;
    u32     ie_len;
    u8      *ie;
};

struct iw_key_params {
    u8          key[32];
    u8          tsc[32];
    u32         key_len;
    u32         tsc_len;
    u16         cipher;
    bool        pairwise;
    bool        default_key;
    u32         key_idx;
    u8          addr[ETH_ALEN];
};

struct iw_sta_add_params {
	const u8 *addr;
	u16 aid;
	u16 capability;
	u16 listen_interval;
	u8  mode;
	const u8 *supp_rates;
	u32 supp_rates_len;
	u32 flags; /* bitmask of WPA_STA_* flags */
};

/**
 * struct beacon_parameters - beacon parameters
 *
 * Used to configure the beacon for an interface.
 *
 * @head: head portion of beacon (before TIM IE)
 *     or %NULL if not changed
 * @tail: tail portion of beacon (after TIM IE)
 *     or %NULL if not changed
 * @interval: beacon interval or zero if not changed
 * @dtim_period: DTIM period or zero if not changed
 * @head_len: length of @head
 * @tail_len: length of @tail
 */
struct beacon_parameters {
	u8 *head, *tail;
	int interval, dtim_period;
	int head_len, tail_len;
};

struct wl_event_reload {
    bool reload;
};

struct wl_event_join_ibss_info {
    u8  bssid[ETH_ALEN];
    u32 freq;
};

struct wl_event_mic_err {
    u8 bssid[ETH_ALEN];
    u8 addr[ETH_ALEN];
    bool group_mic_err;       /* TRUE: group, FALSE: pairwise */
};

struct wl_event_rx_eapol {
    u8 *src_addr;
    u8 *eapol_body;
    u32 eapol_body_len; 
#if TLS_CONFIG_AP
    u8 mode;
#endif
};

struct wl_event_rx_mgmt {
    u8 *mgmt;
    u32 mgmt_len;
};

struct wl_event_rx_from_unknown {
    u8 addr[ETH_ALEN]; 
};

struct wl_event_assoc_info {
    /**
     * reassoc - Flag to indicate association or reassociation
     */
    int reassoc;

    /**
     * req_ies - (Re)Association Request IEs
     *
     * If the driver generates WPA/RSN IE, this event data must be
     * returned for WPA handshake to have needed information. If
     * wpa_supplicant-generated WPA/RSN IE is used, this
     * information event is optional.
     *
     * This should start with the first IE (fixed fields before IEs
     * are not included).
     */
    const u8 *req_ies;

    /**
     * req_ies_len - Length of req_ies in bytes
     */
    u32 req_ies_len;

    /**
     * resp_ies - (Re)Association Response IEs
     *
     * Optional association data from the driver. This data is not
     * required WPA, but may be useful for some protocols and as
     * such, should be reported if this is available to the driver
     * interface.
     *
     * This should start with the first IE (fixed fields before IEs
     * are not included).
     */
    u8 *resp_ies;

    /**
     * resp_ies_len - Length of resp_ies in bytes
     */
    u32 resp_ies_len;

    /**
     * addr - Station address (for AP mode)
     */
    u8 *addr;

    u8 *bssid;

    /**
     * freq - Frequency of the operational channel in MHz
     */
    unsigned int freq;
};

struct tls_wl_event_ops {
    int (*ibss_joined)(struct tls_wif *wif, 
            struct wl_event_join_ibss_info *info);
    int (*assoc)(struct tls_wif *wif, 
            struct wl_event_assoc_info *info);
    int (*disassoc)(struct tls_wif *wif);
    int (*scan_completed)(struct tls_wif *wif);
    int (*mic_err)(struct tls_wif *wif,
            struct wl_event_mic_err *info);
    int (*rx_eapol)(struct tls_wif *wif, 
            struct wl_event_rx_eapol *eapol);
    int (*rx_mgmt)(struct tls_wif *wif,
            struct wl_event_rx_mgmt *mgmt);
	int (*mac_wdg)(struct tls_wif *wif);
#if TLS_CONFIG_AP_OPT_PS
    int (*beacon_done)(struct tls_wif *wif);
    int (*rx_ps)(struct tls_wif *wif,
            struct wl_event_rx_mgmt *mgmt);
    int (*rx_pspoll)(struct tls_wif *wif,
            struct wl_event_rx_from_unknown *rx_pspoll);
    int (*sta_active)(struct tls_wif *wif,
            struct wl_event_rx_from_unknown *rx_pspoll);
#endif
    int (*rx_from_unknown_sta)(struct tls_wif *wif,
            struct wl_event_rx_from_unknown *rx_from_unknown);
    int (*net_down)(struct tls_wif *wif);
	int (*net_fail)(struct tls_wif *wif);
    int (*net_up)(struct tls_wif *wif);
    int (*update_stat)(struct tls_wif *wif, void *cur_bss);/* struct ieee80211_bss *cur_bss */
};

/* sk_buff allocated by wlan driver  */
struct sk_buff {
    struct dl_list list;
    u8   *buf;
    u32   buf_len;
    u32   flags;
};

struct tls_wif {
    //void *priv;
    struct ieee80211_if_data *priv;
    struct wpa_supplicant *wpa_s;
    struct tls_wl_event_ops *ops;

    //struct netif *ethif;
    //bool   net_up;
    bool   wlan_create;
    int (*rx_data_cb)(const u8 *bssid, u8 *buf, u32 buf_len);
#if TLS_CONFIG_AP
    bool   wmm_set;
    void  *client_event_callback;
#if TLS_CONFIG_AP_OPT_FWD
    int (*rx_ip_cb)(const u8 *bssid, u8 *buf, u32 buf_len);
#endif
#endif

#if 0
    int (*send)(struct tls_wif *wif, struct sk_buff *skb);
    int (*rx_data_cb)(struct tls_wif *wif, struct sk_buff *skb);
    int (*tx_mgmt)(struct tls_wif *wif, struct sk_buff *skb);
    int (*send_eapol)(struct tls_wif *wif, struct sk_buff *skb);
    int (*send_beacon)(struct tls_wif *wif, struct sk_buff *skb);
#endif
};

void tls_wl_print_stats(struct tls_wif *wif);

int tls_wl_if_scan(struct tls_wif *wif,
        struct iw_scan_req *scan_req, u16 size);
int tls_wl_if_scan_result2(struct tls_wif *wif,
        u8 *ssid, u32 ssid_len, u8 *buf, u32 buf_size);
int tls_wl_if_scan_result(struct tls_wif *wif,  u8 *buf, u32 buf_size);
void tls_wl_if_sta_flush(struct tls_wif *wif, u8 mode);
int tls_wl_if_sta_deauth(struct tls_wif *wif, u8 *own_addr,
        const u8 *addr, int reason);
int tls_wl_if_sta_disassoc(struct tls_wif *wif, u8 *own_addr,
        const u8 *addr, int reason);
#if TLS_CONFIG_AP
int tls_wl_if_set_tx_queue_params(struct tls_wif *wif, int queue, 
        int aifs, int cw_min, int cw_max, int burst_time);
void tls_wl_if_set_sta_flags(struct tls_wif *wif, u8 *addr, u32 flags);
int tls_wl_if_send_channel_switch(struct tls_wif *wif, u8 *ownaddr, u8 newch);
void tls_wl_if_switch_channel_width(struct tls_wif *wif, u8 *ownaddr);
void tls_wl_if_channel_info_updata(struct tls_wif *wif);
#endif
int tls_wl_if_set_mode(struct tls_wif *wif, u16 mode);
int tls_wl_if_clear_mode(struct tls_wif *wif, u16 mode);
int tls_wl_if_set_bssid(struct tls_wif *wif, const u8 *addr, u32 mode);
int tls_wl_if_set_ssid(struct tls_wif *wif, struct iw_ssid_params *params);
int tls_wl_if_set_auth(struct tls_wif *wif, u16 flag, u32 value, u8 mode);
int tls_wl_if_set_ps_mode(struct tls_wif *wif, int powersave);
int tls_wl_if_set_freq(struct tls_wif *wif,
        int freq);
int tls_wl_if_send_eapol(struct tls_wif *wif,
        u8 *buf, u32 len, bool is_apsta);
int tls_wl_if_xmit(struct tls_wif *wif, void *buf, int len, bool is_apsta, bool not_delay);
int tls_wl_if_add_key(struct tls_wif *wif,
        struct iw_key_params *params);
int tls_wl_if_remove_key(struct tls_wif *wif,
        u32 cipher,
        u32 key_idx,
        bool pairwise,
        u8 *addr);
int tls_wl_if_send_mlme(struct tls_wif *wif,
        u8 *buf, u32 len);
int tls_wl_if_set_rate_mode(struct tls_wif *wif,
        u32 rate_mode);
int tls_wl_if_set_beacon(struct tls_wif *wif,
        struct iw_beacon_parameters *params);
int tls_wl_if_del_beacon(struct tls_wif *wif);
int tls_wl_if_sta_add(struct tls_wif *wif,
        struct iw_sta_add_params *params);
int tls_wl_if_sta_remove(struct tls_wif *wif, u8 *addr);
int tls_wl_if_get_inact_sec(struct tls_wif *wif, const u8 *addr);
int tls_wl_if_get_scan_res(struct tls_wif *wif, u8 *buf, u32 buf_size);
int tls_wl_if_disconnect(struct tls_wif *wif);
int tls_wl_if_tx(struct tls_wif *wif,
        u8 *buf, u32 buflen, bool last_packet, bool is_apsta, bool not_delay);
int tls_wl_if_set_max_rate(struct tls_wif *wif, u8 max_rate_idx);
int tls_wl_if_get_max_rate(struct tls_wif *wif, u8 *max_rate_idx);
int tls_wl_if_set_adhoc(struct tls_wif *wif, int adhoc_automode);
int tls_wl_if_ps(int wake_up);
int tls_wl_if_standby(int type, int delay, int wake_time);
int tls_wl_if_sleep(int type, int delay, int wake_time);
void tls_wl_if_set_errno(int eno);
int  tls_wl_if_get_errno(void);
void  tls_wl_if_perror(const char *info);
const char *tls_wl_if_get_errinfo(int eno);
#endif /* end of TLS_WIRELESS_H */
