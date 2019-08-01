#ifndef _RW_MSG_PUB_H_
#define _RW_MSG_PUB_H_

typedef enum {
    WIFI_COUNTRY_POLICY_AUTO,   /**< Country policy is auto, use the country info of AP to which the station is connected */
    WIFI_COUNTRY_POLICY_MANUAL, /**< Country policy is manual, always use the configured country info */
} wifi_country_policy_t;

typedef enum {
	MSG_IDLE = 0,
	MSG_CONNECTING,
	MSG_PASSWD_WRONG,
	MSG_NO_AP_FOUND,
	MSG_CONN_FAIL,
	MSG_CONN_SUCCESS,
	MSG_GOT_IP,
}msg_sta_states;

/** @brief Structure describing WiFi country-based regional restrictions. */
typedef struct {
    char                  cc[3];          /**< country code string */
    uint8_t               schan;          /**< start channel */
    uint8_t               nchan;          /**< total channel number */
    int8_t                max_tx_power;   /**< maximum tx power */
    wifi_country_policy_t policy;         /**< country policy */
} wifi_country_t;

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

typedef struct scanu_rst_upload
{
    UINT8 scanu_num;
    struct sta_scan_res **res;
}SCAN_RST_UPLOAD_T, *SCAN_RST_UPLOAD_PTR;

extern void mhdr_set_station_status(msg_sta_states val);
extern msg_sta_states mhdr_get_station_status(void);
UINT32 rw_ieee80211_set_country(const wifi_country_t *country);
UINT32 rw_ieee80211_get_country(wifi_country_t *country);

#endif /*_RW_MSG_PUB_H_*/