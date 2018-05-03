/*
 * Copyright (c) 2016 iComm Semiconductor Ltd.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
 
#ifndef TU_COMMON_H
#define TU_COMMON_H

#if defined(SYS_BUS_SPEED) && (SYS_BUS_SPEED == 80)
#define SYS_BUS_SETTING (0x8)
#elif defined(SYS_BUS_SPEED) && (SYS_BUS_SPEED == 40)
#define SYS_BUS_SETTING (0x4)
#else
#error Please defined SYS_BUS_SPEED
#endif

#define TURISMOA        

#if 0              
/**
* struct ssv6006_tx_desc - ssv6006 tx frame descriptor.
* This descriptor is shared with ssv6200 hardware and driver.
*/
struct ssv6006_tx_desc
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
    u32             ack_policy_obsolete:2;   /* See Table 8-6, IEEE 802.11 Spec. 2012 */
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
    u32             rpt_noctstrycnt0:4;     //new, report field for the number of tries excuted by MAC Tx failing to receive CTS.
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
    u32             rpt_noctstrycnt1:4;     //new, report field for the number of tries excuted by MAC Tx failing to receive CTS.
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
    u32             rpt_noctstrycnt2:4;     //new, report field for the number of tries excuted by MAC Tx failing to receive CTS.
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
    u32             rpt_noctstrycnt3:4;     //new, report field for the number of tries excuted by MAC Tx failing to receive CTS.
    /* The definition of WORD_15 */
    u32             ampdu_whole_length:16;  //new, for ampdu tx 1.3 whole ampdu length
    u32             ampdu_next_pkt:8;       //new, for ampdu tx 1.3 pointer for next pkt id
    u32             ampdu_last_pkt:1;       //new, for ampdu tx 1.3 last pkt indicator
    u32             rsvdtx_14a:3;
    u32             ampdu_dmydelimiter_num:4;   // new,v2.13 for ampdu tx 1.3
                                            
    /* The definition of WORD_16 */
    u32             ampdu_tx_bitmap_lw;     //new, for ampdu tx 1.3, bitmap low word for ampdu packet
    /* The definition of WORD_17 */
    u32             ampdu_tx_bitmap_hw;     //new, for ampdu tx 1.3, bitmap high word for ampdu packet    
    /* The definition of WORD_18~20: */
    u32             dummy0;
    u32             dummy1;
    u32             dummy2;
};

/**
* struct ssv6006_rx_desc - ssv6006 rx frame descriptor.
* This descriptor is shared with ssv6006 hardware and driver.
*/
struct ssv6006_rx_desc
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

};

struct ssv6006_rxphy_info {
    /* WORD 1: */
    u32             len:16;
    u32             phy_rate:8;   /* bit [7:6] = 0 : 11b ; 01 = reserved;
                                   *             10: 11g ; 11 = 11n;
                                   * bit [5] =   0 : 20M ;  1 = 40M;
                                   * bit [4] = 1/0 : Short/Long preamble in 11b /GI in 11n
                                   * bit [3] = 1/0 : GreenField/MixedMode in 11n
                                   * bit [2:0] =  1M, 2M, 5.5M, 11M in 11b
                                   *              6M, 9M, ~, 54M in 11g 
                                   *              MCS0, MCS1, ~, MCS7 in 11n
                                   */
    u32             smoothing:1;
    u32             no_sounding:1;
    u32             aggregate:1;
    u32             stbc:2;
    u32             fec:1;
    u32             n_ess:2;    

    /* WORD 2: */
    u32             l_length:12;
    u32             l_rate:3;
    u32             mrx_seqn:1;
    u32             rssi:8;
    u32             snr:8;

    /* WORD 3: */
    u32             rx_freq_offset:16;  // new; for v2.13
    u32             service:16;         // new; for v2.13

    /* WORD 4: */    
    u32             rx_time_stamp;

}; 
#endif

struct padpd_table{
    u32 addr;
    u32 mask0;
    u32 mask1;
};
  
extern int cal_ch_5g[4];
/* define for xtal type*/
enum {   
    XTAL16M = 0,
    XTAL24M,
    XTAL26M,
    XTAL40M,
    XTAL12M,    
    XTAL20M,
    XTAL25M,
    XTAL32M,
    XTALMAX,
};
enum {   
    NL80211_CHAN_NO_HT = 0,
    NL80211_CHAN_HT20,
    NL80211_CHAN_HT40MINUS,
    NL80211_CHAN_HT40PLUS,
};

/* define for supported band */
enum{
    G_BAND_ONLY = 0,
    AG_BAND_BOTH = 1,
};

enum {
    CAL_IDX_NONE,           
    CAL_IDX_WIFI2P4G_RXDC,  
    CAL_IDX_BT_RXDC,        
    CAL_IDX_BW20_RXRC,      
    CAL_IDX_WIFI2P4G_TXLO,  
    CAL_IDX_WIFI2P4G_TXIQ, 
    CAL_IDX_WIFI2P4G_RXIQ,  
    CAL_IDX_WIFI2P4G_PADPD,
    CAL_IDX_5G_NONE,        
    CAL_IDX_WIFI5G_RXDC,    
    CAL_IDX_5G_NONE2,       
    CAL_IDX_BW40_RXRC,      
    CAL_IDX_WIFI5G_TXLO,    
    CAL_IDX_WIFI5G_TXIQ,   
    CAL_IDX_WIFI5G_RXIQ,    
    CAL_IDX_WIFI5G_PADPD,   
};
   
enum {
    MODE_STANDBY,
    MODE_CALIBRATION,
    MODE_WIFI2P4G_TX,
    MODE_WIFI2P4G_RX,
    MODE_BT_TX,
    MODE_BT_RX,
    MODE_WIFI5G_TX,
    MODE_WIFI5G_RX,
};  
                                 
/*  The following code are common phy_RF routine over all platform.
    To use these routine , it should define several macro in other header file first.
    The following pre-defined macro are for SMAC
    
    // Macros for predefined register access.
    #define REG32(_addr)    REG32_R(_addr)
    #define REG32_R(_addr)   ({ u32 reg; SMAC_REG_READ(sh, _addr, &reg); reg;})
    #define REG32_W(_addr, _value)   do { SMAC_REG_WRITE(sh, _addr, _value); } while (0)
*/ 
    // Macros for turismo common header
    #define MSLEEP(_val)        OS_MsDelay(_val)
    #define UDELAY(_val)        OS_MsDelay(1)
    //#define PRINT               printf    
    #define PRINT                   

// For GeminiA
/* If DUT support HT40, it should TU_CHANGE_CHANNEL to set channel and bandwidth. 
   Otherwise, it can use TU_SET_CHANNEL to set channel only. It should set bandwith before
   setting channel.
*/
#endif
