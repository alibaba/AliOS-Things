#ifndef _SSV_PKTDEF_H_
#define _SSV_PKTDEF_H_

#include <soc_types.h>
//#include <ssv6006_mac.h>
//#include <ieee80211_util.h>

/**
 *  The flag definition for c_type (command type) field of PKTInfo:
 *
 *      @ M0_TXREQ:
 *      @ M1_TXREQ
 *      @ M2_TXREQ
 *      @ M0_RXEVENT
 *      @ M1_RXEVENT
 *      @ HOST_CMD
 *      @ HOST_EVENT
 *
 */
#define M0_TXREQ                            0
#define M1_TXREQ                            1
#define M2_TXREQ                            2
#define M0_RXEVENT                          3
#define M2_RXEVENT                          4
#define HOST_CMD                            5
#define HOST_EVENT                          6
#define RATE_RPT                            7


#define IS_RX_PKT(_p)       ((_p)->c_type==M0_RXEVENT)
#define IS_TX_PKT(_p)       (/*((_p)->c_type>=M0_TXREQ)&&*/((_p)->c_type<=M2_TXREQ))
#define IS_TX_TEST_CMD(_p)	((_p)->c_type==TEST_CMD)
#define IS_STA_RX_BMCAST(_p)    ((*((u8 *)(_p)+(_p)->hdr_offset +  4))&0x01)


/**
* struct ssv6006_tx_desc - ssv6006 tx frame descriptor.
* This descriptor is shared with ssv6200 hardware and driver.
*/
 struct cfg_host_txreq0 {
    /* The definition of WORD_1: */
    u32             len:16;
    u32             c_type:3;
    u32             f80211:1;
    u32             qos:1;          /* 0: without qos control field, 1: with qos control field */
    u32             ht:1;           /* 0: without ht control field, 1: with ht control field */
    u32             use_4addr:1;
    u32             rvdtx_0:1;//used for rate control report event.
    u32             padding:2;
    u32             bc_que:1;
    u32             security:1;
    u32             more_data:1;
    u32             stype_b5b4:2;
    u32             extra_info:1;   /* 0: don't trap to cpu after parsing, 1: trap to cpu after parsing */


    /* The definition of WORD_2: */
    u32             fCmd;

    /* The definition of WORD_3: */
    u32             hdr_offset:8;
    u32             frag:1;
    u32             unicast:1;
    u32             hdr_len:6;
    u32             no_pkt_buf_reduction:1;
    u32             tx_burst_obsolete:1;     /* 0: normal, 1: burst tx */
    u32             ack_policy_obsolete:2;   /* See Table 8-6, IEEE 802.11 Spec. 2012 */
    u32             aggr:2;
    u32             rsvdtx_1:1;              // for sw retry ampdu
    u32             is_rate_stat_sample_pkt:1;
                                             // new , v2.13
    u32             bssidx:2;                // new , v2.13, change location
    
};

struct cfg_host_rxpkt {

    /* The definition of WORD_1: */
    u32             len:16;
    u32             c_type:3;
    u32             f80211:1;
    u32             qos:1;          /* 0: without qos control field, 1: with qos control field */
    u32             ht:1;           /* 0: without ht control field, 1: with ht control field */
    u32             use_4addr:1;
    u32             rsvdrx0_1:1;    // v2.14
    u32             running_no:4;   // v2.14
    u32             psm:1;
    u32             stype_b5b4:2;
    u32             rsvdrx0_2:1;

    /* The definition of WORD_2: */
    union{
        u32             fCmd;
        u32             edca0_used:4;
        u32             edca1_used:5;
        u32             edca2_used:5;
        u32             edca3_used:5;
        u32             mng_used:4;
        u32             tx_page_used:9;
    };
    /* The definition of WORD_3: */
    u32             hdr_offset:8;
    u32             frag:1;
    u32             unicast:1;
    u32             hdr_len:6;
    u32             RxResult:8;
    u32             bssid:2;        // new; for v2.13
    u32             reason:6;

    /* The definition of WORD_4: */
    u32             channel:8;      // v2.14
    u32             rx_pkt_run_no:8;      // new; for v2.14; from tx_pkt_run_no at BA
    u32             fCmdIdx:3;
    u32             wsid:4;
    u32             rsvd_rx_3b:9;

	/* The definition of WORD_5: */
	u32 			phy_packet_length:16;
	u32				phy_rate:8;
    u32             phy_smoothing:1;
    u32             phy_no_sounding:1;
    u32             phy_aggregation:1;
    u32             phy_stbc:2;
    u32             phy_fec:1;
    u32             phy_n_ess:2;

	/* The definition of WORD_6: */
	u32				phy_l_length:12;
	u32				phy_l_rate:3;
	u32				phy_mrx_seqn:1;
	u32				phy_rssi:8;
	u32				phy_snr:8;

	/* The definition of WORD_7: */
	u32				phy_rx_freq_offset:16;
	u32				phy_service:6;

};

typedef struct PKT_Info_st
{
    /* The definition of WORD_1: */
    u32             len:16;
    u32             c_type:3;
    u32             f80211:1;
    u32             qos:1;          /* 0: without qos control field, 1: with qos control field */
    u32             ht:1;           /* 0: without ht control field, 1: with ht control field */
    u32             use_4addr:1;
    u32             rsvd_0:6;
    u32             stype_b5b4:2;
    u32             extra_info:1;   /* 0: don't trap to cpu after parsing, 1: trap to cpu after parsing */

    /* The definition of WORD_2: */
    u32             fCmd;

    /* The definition of WORD_3: */
    u32             hdr_offset:8;
    u32             frag:1;
    u32             unicast:1;
    u32             hdr_len:6;
    u32             rsvd_1:10;
    u32             reason:6;

    /* The definition of WORD_4: */
    u32             rsvd_2:16;
    u32             fCmdIdx:3;
    u32             wsid:4;
    u32             rsvd_3:9;
}PKT_Info, *PPKT_Info;


/**
* struct ssv6006_tx_desc - ssv6006 tx frame descriptor.
* This descriptor is shared with ssv6200 hardware and driver.
*/
typedef struct PKT_TxInfo_st
{
    /* The definition of WORD_1: */
    u32             len:16;
    u32             c_type:3;
    u32             f80211:1;
    u32             qos:1;          /* 0: without qos control field, 1: with qos control field */
    u32             ht:1;           /* 0: without ht control field, 1: with ht control field */
    u32             use_4addr:1;
    u32             rvdtx_0:3;//used for rate control report event.
    u32             bc_que:1;
    u32             security:1;
    u32             more_data:1;
    u32             stype_b5b4:2;
    u32             extra_info:1;   /* 0: don't trap to cpu after parsing, 1: trap to cpu after parsing */

    /* The definition of WORD_2: */
    u32             fCmd;

    /* The definition of WORD_3: */
    u32             hdr_offset:8;
    u32             frag:1;
    u32             unicast:1;
    u32             hdr_len:6;
    u32             no_pkt_buf_reduction:1;
    u32             tx_burst_obsolete:1;     /* 0: normal, 1: burst tx */
    u32             ack_policy_obsolete:2; //ack_policy_obsolete:2;   /* See Table 8-6, IEEE 802.11 Spec. 2012 */
    u32             aggr:2; 
    u32             rsvdtx_1:1;              // for sw retry ampdu
    u32             is_rate_stat_sample_pkt:1;
                                             // new , v2.13
    u32             bssidx:2;                // new , v2.13, change location
    u32             reason:6;

    /* The definition of WORD_4: */
    u32             payload_offset_obsolete:8;
    u32             tx_pkt_run_no:8;         // new, v2.13
    u32             fCmdIdx:3;
    u32             wsid:4;
    u32             txq_idx:3;
    u32             TxF_ID:6;

    /* The definition of WORD_5: */
    u32             rateidx1_data_duration:16;
    u32             rateidx2_data_duration:16;

    /* The definition of WORD_6: */
    u32             rateidx3_data_duration:16;
    u32             rsvd_tx05 :2;
    u32             rate_rpt_mode:2;        //new
    u32             ampdu_tx_ssn:12;        //new for ampdu 1.3 not used in turismo
    /* The definition of WORD_7 */
    u32             drate_idx0:8;           //new, normal rate setting for series 0
    u32             crate_idx0:8;           //new, control rate setting(cts/rts) for series 0
    u32             rts_cts_nav0:16;        //new, rts_cts_nav for series 0
    /* The definition of WORD_8 */
    u32             dl_length0:12;          //new, dl_length for series 0
    u32             try_cnt0:4;             //new, retry count for series 0
    u32             ack_policy0:2;          //new, ack policy for series 0
    u32             do_rts_cts0:2;          //new, do rts_cts for series 0
    u32             is_last_rate0:1;        //new, is v2.13 last rate for rate control series
    u32             rsvdtx_07b:1;
    u32             rpt_result0:2;          /*new, for v2.13;
                                            //     0: typical maxtry fail ,     1: typical success
                                            //     2: by-packet rts maxtry fail 3: peer-ps reject tx*/

    u32             rpt_trycnt0:4;          //new, report field for the number of tries excuted by MAC Tx
    #if 0    
    u32             rpt_noctstrycnt0:4;     //new, report field for the number of tries excuted by MAC Tx failing to receive CTS.
    #else
    u32             no_rsp_rxbusyfall0:1;
    u32             no_rsp_rxreqcutin0:1;    
    u32             no_rsp_timeout0:1;        
    u32             no_rsp0:1;    
    #endif
    /* The definition of WORD_9 */
    u32             drate_idx1:8;           //new, normal rate setting for series 1
    u32             crate_idx1:8;           //new, control rate setting(cts/rts) for series 1
    u32             rts_cts_nav1:16;        //new, rts_cts_nav for series 1
    /* The definition of WORD_10 */
    u32             dl_length1:12;          //new, dl_length for series 1
    u32             try_cnt1:4;             //new, retry count for series 1
    u32             ack_policy1:2;          //new, ack policy for series 1
    u32             do_rts_cts1:2;          //new, do rts_cts for series 1
    u32             is_last_rate1:1;        //new, is v2.13 last rate for rate control series
    u32             rsvdtx_09b:1;
    u32             rpt_result1:2;          /*new, for v2.13;
                                            //     0: typical maxtry fail ,     1: typical success
                                            //     2: by-packet rts maxtry fail 3: peer-ps reject tx*/
    u32             rpt_trycnt1:4;          //new, report field for the number of tries excuted by MAC Tx
    #if 0    
    u32             rpt_noctstrycnt1:4;     //new, report field for the number of tries excuted by MAC Tx failing to receive CTS.
    #else
    u32             no_rsp_rxbusyfall1:1;
    u32             no_rsp_rxreqcutin1:1;    
    u32             no_rsp_timeout1:1;        
    u32             no_rsp1:1;    
    #endif
     /* The definition of WORD_11 */
    u32             drate_idx2:8;           //new, normal rate setting for series 2
    u32             crate_idx2:8;           //new, control rate setting(cts/rts) for series 2
    u32             rts_cts_nav2:16;        //new, rts_cts_nav for series 2
    /* The definition of WORD_12 */
    u32             dl_length2:12;          //new, dl_length for series 2
    u32             try_cnt2:4;             //new, retry count for series 2
    u32             ack_policy2:2;          //new, ack policy for series 2
    u32             do_rts_cts2:2;          //new, do rts_cts for series 2
    u32             is_last_rate2:1;        //new, is v2.13 last rate for rate control series
    u32             rsvdtx_11b:1;
    u32             rpt_result2:2;          /*new, for v2.13;
                                            //     0: typical maxtry fail ,     1: typical success
                                            //     2: by-packet rts maxtry fail 3: peer-ps reject tx*/
    u32             rpt_trycnt2:4;          //new, report field for the number of tries excuted by MAC Tx
    #if 0
    u32             rpt_noctstrycnt2:4;     //new, report field for the number of tries excuted by MAC Tx failing to receive CTS.
    #else
    u32             no_rsp_rxbusyfall2:1;
    u32             no_rsp_rxreqcutin2:1;    
    u32             no_rsp_timeout2:1;        
    u32             no_rsp2:1;        
    #endif
    /* The definition of WORD_13 */
    u32             drate_idx3:8;           //new, normal rate setting for series 3
    u32             crate_idx3:8;           //new, control rate setting(cts/rts) for series 3
    u32             rts_cts_nav3:16;        //new, rts_cts_nav for series 3
    /* The definition of WORD_14 */
    u32             dl_length3:12;          //new, dl_length for series 3
    u32             try_cnt3:4;             //new, retry count for series 3
    u32             ack_policy3:2;          //new, ack policy for series 3
    u32             do_rts_cts3:2;          //new, do rts_cts for series 3
    u32             is_last_rate3:1;        //new, is v2.13 last rate for rate control series
    u32             rsvdtx_13b:1;
    u32             rpt_result3:2;          /*new, for v2.13;
                                            //     0: typical maxtry fail ,     1: typical success
                                            //     2: by-packet rts maxtry fail 3: peer-ps reject tx*/
    u32             rpt_trycnt3:4;          //new, report field for the number of tries excuted by MAC Tx
    #if 0
    u32             rpt_noctstrycnt3:4;     //new, report field for the number of tries excuted by MAC Tx failing to receive CTS.
    #else
    u32             no_rsp_rxbusyfall3:1;
    u32             no_rsp_rxreqcutin3:1;    
    u32             no_rsp_timeout3:1;        
    u32             no_rsp3:1;        
    #endif
    /* The definition of WORD_15 */
    u32             ampdu_whole_length:16;  //new, for ampdu tx 1.3 whole ampdu length
    u32             ampdu_next_pkt:8;       //new, for ampdu tx 1.3 pointer for next pkt id
    u32             ampdu_last_pkt:1;       //new, for ampdu tx 1.3 last pkt indicator
    u32             rsvdtx_14a:3;                //rsvdtx_14a:3;
    u32             ampdu_dmydelimiter_num:4;   // new,v2.13 for ampdu tx 1.3

    /* The definition of WORD_16 */
    u32             ampdu_tx_bitmap_lw;     //new, for ampdu tx 1.3, bitmap low word for ampdu packet
    /* The definition of WORD_17 */
    u32             ampdu_tx_bitmap_hw;     //new, for ampdu tx 1.3, bitmap high word for ampdu packet
    /* The definition of WORD_18~20: */
    u32             dummy0;
    u32             dummy1;
    u32             dummy2;
}PKT_TxInfo, *PPKT_TxInfo;


/**
* struct ssv6006_rx_desc - ssv6006 rx frame descriptor.
* This descriptor is shared with ssv6006 hardware and driver.
*/
typedef struct PKT_RxInfo_st
{
    /* The definition of WORD_1: */
    u32             len:16;
    u32             c_type:3;
    u32             f80211:1;
    u32             qos:1;          /* 0: without qos control field, 1: with qos control field */
    u32             ht:1;           /* 0: without ht control field, 1: with ht control field */
    u32             use_4addr:1;
    u32             rsvdrx0_1:1;    // v2.14
    u32             running_no:4;   // v2.14
    u32             psm:1;
    u32             stype_b5b4:2;
    u32             rsvdrx0_2:1;

    /* The definition of WORD_2: */
    union{
        u32             fCmd;
        u32             edca0_used:4;
        u32             edca1_used:5;
        u32             edca2_used:5;
        u32             edca3_used:5;
        u32             mng_used:4;
        u32             tx_page_used:9;
    };
    /* The definition of WORD_3: */
    u32             hdr_offset:8;
    u32             frag:1;
    u32             unicast:1;
    u32             hdr_len:6;
    u32             RxResult:8;
    u32             bssid:2;        // new; for v2.13
    u32             reason:6;

    /* The definition of WORD_4: */
    u32             channel:8;      // v2.14
    u32             rx_pkt_run_no:8;      // new; for v2.14; from tx_pkt_run_no at BA
    u32             fCmdIdx:3;
    u32             wsid:4;
    u32             rsvd_rx_3b:9;

	/* The definition of WORD_5: */
	u32 			phy_packet_length:16;
	u32				phy_rate:8;
    u32             phy_smoothing:1;
    u32             phy_no_sounding:1;
    u32             phy_aggregation:1;
    u32             phy_stbc:2;
    u32             phy_fec:1;
    u32             phy_n_ess:2;

	/* The definition of WORD_6: */
	u32				phy_l_length:12;
	u32				phy_l_rate:3;
	u32				phy_mrx_seqn:1;
	u32				phy_rssi:8;
	u32				phy_snr:8;

	/* The definition of WORD_7: */
	u32				phy_rx_freq_offset:16;
	u32				phy_service:6;

}PKT_RxInfo, *PPKT_RxInfo;

#define GET_PKT_PAYLOAD_OFFSET(p) (((PKT_Info *)p)->hdr_offset+((PKT_Info *)p)->hdr_len)

#define IS_AMPDU_RETRY_PACKET(p) (p->rsvdtx_14a)

typedef struct PKT_RSInfo_st
{
    /* The definition of WORD_1: */
    u32             len:16;
    u32             c_type:3;
    u32             rsvd0:5;
    u32             fini:8;

    /* The definition of WORD_2: */
    u32             tab_type:16;
    u32             rsvd1:16;

    /* The definition of WORD_3: */
    /* The definition of WORD_4: */
    u8              wsid_ptr[8];

}PKT_RSInfo, *PPKT_RSInfo;

#endif	/* _SSV_PKTDEF_H_ */

