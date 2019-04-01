#ifndef _DRV_MAC_H_
#define _DRV_MAC_H_

#include <turismo/regs.h>
#include <wificonf.h>
#include <attrs.h>
//#include <hw_regs_api.h>

#define RX_QUEUE_LEN (64)   //it must be power of 2
#define RX_QUEUE_MASK (RX_QUEUE_LEN-1)
#define PKT_IRQHANDLE 1
#define KRACK 1

#define FTYPE_BEACON         0x80

typedef struct rx_info
{
  u8* pbuf_addr;
//  u32 pktlen;
  unsigned long packettime;
  u8 isoccupied;
} rx_info_t;
 
typedef struct rx_queue
{
  rx_info_t queue[RX_QUEUE_LEN];
  u8 save_index;
  u8 proc_index;
  u8 clear_index;
} rx_queue_t;

struct sk_pbuff {
	u8 *data;
    unsigned long packettime;
	u16 len;
	u8 rssi;
	u8 bssid;
};
/**
* enum switch_ops_en (sw_ops) - define enable/disable value.
*
* @ SW_DISABLE:
* @ SW_ENABLE:
*/
typedef enum switch_ops_en {
    SW_DISABLE = 0,
    SW_ENABLE = 1,
} sw_ops;

/**
* enum sta_mode_en(sta_mode) - define the station operation mode.
*
* @ OP_MODE_STA: Station mode
* @ OP_MODE_AP: AP mode
* @ OP_MODE_IBSS: IBSS (ad-hoc) mode
* @ OP_MODE_WDS: Wireless Distribution System mode
*/
typedef enum sta_mode_en {
    OP_MODE_STA = 0,
    OP_MODE_AP,
    OP_MODE_IBSS,
    OP_MODE_WDS,

} sta_mode;

#define drv_mac_set_op_mode(sm)             SET_OP_MODE(sm)
#define drv_mac_get_op_mode()               GET_OP_MODE




/**
* enum ht_mode_en (ht_mode) - HT mode setting.
*
* @ HT_NONE: Non-HT mode
* @ HT_MF: HT Mixed mode
* @ HT_GF: HT Greenfield mode
*/
typedef enum ht_mode_en {
    M_HT_NONE = 0,
    M_HT_MF,
    M_HT_GF,

} ht_mode;

#define drv_mac_set_ht_mode(hm)             SET_HT_MODE(hm)
#define drv_mac_get_ht_mode()               GET_HT_MODE




#define DRV_MAC_GET_STA_TX_SEQ(wsid, tx_tid) (REG_WSID_ENTRY[wsid].s[tx_tid].UP_TX_SEQCTL)


#define drv_mac_set_pbuf_offset(s)          SET_PB_OFFSET(s)
#define drv_mac_get_pbuf_offset()           GET_PB_OFFSET

#define drv_mac_set_dup_detect(s)           SET_DUP_FLT(s)
#define drv_mac_get_dup_detect()            GET_DUP_FLT


#define drv_mac_set_promis_mode(s)
#define drv_mac_get_promis_mode()


#define drv_mac_set_null_trap(s)
#define drv_mac_get_null_trap()


#define drv_mac_set_hdr_strip(s)            SET_HDR_STRIP(s)
#define drv_mac_get_hdr_strip()             GET_HDR_STRIP


#define drv_mac_set_rx2host(s)              SET_RX_2_HOST(s)
#define drv_mac_get_rx2host()               GET_RX_2_HOST


//#define drv_mac_set_rx2host_m2(s)           SET_RX2HOST_M2_ENA(s)
//#define drv_mac_get_rx2host_m2()            GET_RX2HOST_M2_ENA




#define drv_mac_set_group_seq_ctrl(s)       SET_SEQ_CTRL(s)
#define drv_mac_get_group_seq_ctrl()        GET_SEQ_CTRL

#define drv_mac_set_tx_ether_trap_en(s)     SET_TX_ETHER_TRAP_EN(s)
#define drv_mac_get_tx_ether_trap_en()        GET_TX_ETHER_TRAP_EN

#define drv_mac_set_rx_ether_trap_en(s)     SET_RX_ETHER_TRAP_EN(s)
#define drv_mac_get_rx_ether_trap_en()        GET_RX_ETHER_TRAP_EN


#define drv_mac_set_group_ack_policy(s)
#define drv_mac_get_group_ack_policy



#define drv_mac_set_rx_null_data_trap_en(s)            SET_RX_NULL_TRAP_EN(s)
#define drv_mac_get_rx_null_data_trap_en()             GET_RX_NULL_TRAP_EN







/**
* enum erp_protect_type_en (erp_protect_type) - define erp protect type.
*
* @ ERP_T_NONE: no erp protect
* @ ERP_T_RTSCTS: RTS/CTS mechanism
* @ ERP_T_CTS2SELF: CTS-to-Self mechnism
*/
typedef enum erp_protect_type_en {
    ERP_T_NONE = 0,
    ERP_T_RTSCTS = 1,
    ERP_T_CTS2SELF = 2,
    ERP_T_RSVD = 3,

} erp_protect_type;

#define drv_mac_set_erp_protect(s)          SET_ERP_PROTECT(s)
#define drv_mac_get_erp_protect()           GET_ERP_PROTECT

#define drv_mac_set_rx_info_size(s)       SET_RX_INFO_SIZE(s)
#define drv_mac_get_rx_info_size()        GET_RX_INFO_SIZE

#define drv_mac_set_tx_pboffset(s)       SET_TX_PBOFFSET(s)
#define drv_mac_get_tx_pboffset()        GET_TX_PBOFFSET

#define drv_mac_set_tx_info_size(s)       SET_TX_INFO_SIZE(s)
#define drv_mac_get_tx_info_size()        GET_TX_INFO_SIZE

#define drv_mac_set_rx_last_phy_size(s)       SET_RX_LAST_PHY_SIZE(s)
#define drv_mac_get_rx_info_last_phy_size()        GET_RX_LAST_PHY_SIZE



#define drv_mac_set_auto_seqno(s)           SET_AUTO_SEQNO(s)
#define drv_mac_get_auto_seqno()            GET_AUTO_SEQNO

#define drv_mac_set_rts_threshold(s)        SET_DOT11RTSTHRESHOLD(s)
#define drv_mac_get_rts_threshold()         GET_DOT11RTSTHRESHOLD

#define drv_mac_set_txpkt_rsvd(s)            SET_TX_PKT_RSVD(s)
#define drv_mac_get_txpkt_rsvd()             GET_TX_PKT_RSVD

#define drv_mac_set_nav(s)                   SET_MTX_NAV(s)
#define drv_mac_get_nav()                    GET_MTX_NAV


#define drv_mac_set_bypass_tx_parser_encap(s)     SET_BYPASSS_TX_PARSER_ENCAP(s)
#define drv_mac_get_bypass_tx_parser_encap()      GET_BYPASSS_TX_PARSER_ENCAP

#define drv_mac_set_pro_ver(s)     SET_PRO_VER(s)
#define drv_mac_get_pro_ver()      GET_PRO_VER



/**
*
*/
#define drv_mac_set_global_ack_policy(s)
#define drv_mac_get_global_ack_policy()

#define drv_mac_set_txf_id(s)               SET_TXF_ID(s)
#define drv_mac_get_txf_id()                GET_TXF_ID

#define drv_mac_set_mgmt_txqid(s)           SET_TXQ_ID0(s)
#define drv_mac_get_mgmt_txqid()            GET_TXQ_ID0

#define drv_mac_set_nonqos_txqid(s)         SET_TXQ_ID1(s)
#define drv_mac_get_nonqos_txqid()          GET_TXQ_ID1

#define drv_mac_set_mac_loopback(s)         SET_RG_MAC_LPBK(s)
#define drv_mac_get_mac_loopback()          GET_RG_MAC_LPBK

#define drv_mac_set_mac_loopback_without_rx_check(s) SET_RG_LPBK_RX_EN(s)
#define drv_mac_get_mac_loopback_without_rx_check() GET_RG_LPBK_RX_EN


#define drv_mac_set_mac_m2m(s)              SET_RG_MAC_M2M(s)
#define drv_mac_get_mac_m2m()               GET_RG_MAC_M2M

#define drv_mac_set_phy_loopback(s)         SET_RG_PHY_LPBK(s)
#define drv_mac_get_phy_loopback()          GET_RG_PHY_LPBK

#define drv_mac_set_sniffer_mode(s)         SET_SNIFFER_MODE(s)
#define drv_mac_set_ampdu_sniffer(s)        SET_AMPDU_SNIFFER(s)

/* obsolete: */
#define drv_mac_set_qos_en(s)                SET_QOS_EN(s)
#define drv_mac_get_qos_en()                 GET_QOS_EN

void drv_mactrx_init();
void drv_macfilter_setting(WIFI_OPMODE mode, SNIFFER_RECVINDEX recvsetting);
void RATE_RPT_handler(u8 *pbuf_addr) ATTRIBUTE_SECTION_FAST;

/**
* STA Infomation Definition:
*
* @ STA_INFO_MODE()
* @ STA_INFO_HT()
* @ STA_INFO_QOS()
*/
#define STA_INFO_MODE(x)            ((x)&(3<<0))
#define STA_INFO_HT(x)              ((x)&(3<<2))
#define STA_INFO_QOS(x)             ((x)&(1<<4))



/**
* Define flags for WSID INFO field:
*
* @ WSID_INFO_VALID
*/
#define SET_WSID_INFO_VALID(s, v)       do {(s)->info = (((s)->info & ~(1<<0)) | ((v)<<0)); } while (0)
#define SET_WSID_INFO_QOS_EN(s, v)      do {(s)->info = (((s)->info & ~(1<<1)) | ((v)<<1)); } while (0)
#define SET_WSID_INFO_OP_MODE(s, v)     do {(s)->info = (((s)->info & ~(3<<2)) | ((v)<<2)); } while (0)
#define SET_WSID_INFO_HT_MODE(s, v)     do {(s)->info = (((s)->info & ~(3<<4)) | ((v)<<4)); } while (0)

#define GET_WSID_INFO_VALID(s)          ((s)->info&0x01)
#define GET_WSID_INFO_QOS_EN(s)         (((s)->info >>1) & 0x01)
#define GET_WSID_INFO_OP_MODE(s)        (((s)->info >>2) & 0x03)
#define GET_WSID_INFO_HT_MODE(s)        (((s)->info >>3) & 0x03)



/**
* struct mac_wsid_tabl_st - define wsid (wiress session ID) entry.
*
* @
*/
typedef struct mac_wsid_entry_st {
    u32             info;
    ETHER_ADDR      sta_mac;
    u16             tx_ack_policy;
    u16             tx_seq_ctrl[8];
    u16             rx_seq_ctrl[8];

} mwsid_entry;

s32 drv_mac_set_wsid_table(s32 index, const mwsid_entry *wen);
s32 drv_mac_get_wsid_table(s32 index, mwsid_entry *wen);
s32 drv_mac_get_wsid_peer_mac (s32 index, ETHER_ADDR *mac);
s32 drv_mac_set_regmac(u32 macreg, const u8 *mac);

/**
* struct gmfilter_tbl_st - define group address filter entry
*
* @
*/
typedef struct gmfilter_st {
    u32             ctrl; /* 0/1: disable, 2: mask mode, 3: single mode */
    ETHER_ADDR      mac;
    ETHER_ADDR      msk;

} gmfilter;


s32 drv_mac_set_ba_switch(u8 enable);
s32 drv_mac_get_ba_switch(void);
s32 drv_mac_set_ba_winsize(u8 size);
u16 drv_mac_get_ba_winsize(void);
s32 drv_mac_set_ba_ta(u8 *mac);
s32 drv_mac_get_ba_ta(u8 *mac);
s32 drv_mac_set_gmfilter(s32 index, const gmfilter *gm_entry);
s32 drv_mac_get_gmfilter(s32 index, gmfilter *gm_entry);
s32 drv_mac_set_decision_table(const u16 *raw_dat);
s32 drv_mac_get_decision_table(u16 *raw_dat);
s32 drv_mac_get_decision_table_value(u16 *raw_dat);
s32 drv_mac_get_decision_table_mask(u16 *raw_dat);
s32 drv_mac_set_cmd_flow(const u32 *cflow);
s32 drv_mac_get_cmd_flow(u32 *cflow);
s32 drv_mac_set_sta_mac(const u8 *mac);
s32 drv_mac_get_sta_mac(u8 *mac);
s32 drv_mac_set_bssid(const u8 *mac);
s32 drv_mac_get_bssid(u8 *mac);
s32 drv_mac_set_trap_mask(const u32 *trap_mask);
s32 drv_mac_get_trap_mask(u32 *trap_mask);
s32 drv_mac_init(void);



s32 drv_mac_set_tx_flow_data(const u32 *cflow);
s32 drv_mac_get_tx_flow_data(u32 *cflow);
s32 drv_mac_set_tx_flow_mgmt(const u32 *cflow);
s32 drv_mac_get_tx_flow_mgmt(u32 *cflow);
s32 drv_mac_set_tx_flow_ctrl(const u32 *cflow);
s32 drv_mac_get_tx_flow_ctrl(u32 *cflow);
s32 drv_mac_set_rx_flow_data(const u32 *cflow);
s32 drv_mac_get_rx_flow_data(u32 *cflow);
s32 drv_mac_set_rx_flow_mgmt(const u32 *cflow);
s32 drv_mac_get_rx_flow_mgmt(u32 *cflow);
s32 drv_mac_set_rx_flow_ctrl(const u32 *cflow);
s32 drv_mac_get_rx_flow_ctrl(u32 *cflow);
s32 drv_mac_set_tx_ether_trap(s32 index, u16 eth_type);
s32 drv_mac_get_tx_ether_trap(s32 index, u16 *eth_type);
s32 drv_mac_set_rx_ether_trap(s32 index, u16 eth_type);
s32 drv_mac_get_rx_ether_trap(s32 index, u16 *eth_type);

s32 drv_mac_hw_queue_status(void);

//HW MIB counter
int drv_mac_mib_reset(void);
int drv_mac_mib_on(void);
int drv_mac_mib_off(void);
int drv_mac_mib_rx(void);
int drv_mac_mib_tx(void);

void set_macisr_status(u8 stat) ATTRIBUTE_SECTION_FAST;

#endif /* _DRV_MAC_H_ */


