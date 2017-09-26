#ifndef _RW_PUB_H_
#define _RW_PUB_H_

#include "ke_msg.h"
#include "uart_pub.h"

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
	FUNC_1PARAM_PTR cb;
	void *ctxt_arg;
}IND_CALLBACK_T;

extern void mt_msg_dispatch(UINT16 cmd, void *param);
extern void mr_kmsg_flush(void);
extern UINT32 mr_kmsg_fwd(struct ke_msg *msg);
extern UINT32 mr_kmsg_fuzzy_handle(void);
extern UINT32 mr_kmsg_exact_handle(UINT16 rsp);
extern UINT32 mr_kmsg_fetch(UINT8 *buf, UINT32 len);
extern UINT32 mr_kmsg_background_handle(void);
extern void mhdr_assoc_cfm_cb(FUNC_1PARAM_PTR ind_cb, void *ctxt);
extern void mhdr_scanu_reg_cb(FUNC_1PARAM_PTR ind_cb, void *ctxt);
extern UINT32 rw_ieee80211_init(void);
extern UINT32 rw_ieee80211_get_centre_frequency(UINT32 chan_id);
extern UINT8 rw_ieee80211_get_chan_id(UINT32 freq);
extern void *sr_get_scan_results(void);
extern void sr_release_scan_results(SCAN_RST_UPLOAD_PTR ptr);
extern UINT32 rwm_transfer(UINT8 *buf, UINT32 len);
extern UINT32 rwm_uploaded_data_handle(UINT8 *upper_buf, UINT32 len);
extern UINT32 rwm_get_rx_valid_node_len(void);

extern UINT8 beacon[149];

#endif //_RW_PUB_H_
// eof

