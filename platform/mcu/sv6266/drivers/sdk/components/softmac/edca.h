#ifndef _EDCA_H_
#define _EDCA_H_


typedef enum{
	EDCA_INFOFLAG_WAIT_BK_TX_DONE 	= 0,
	EDCA_INFOFLAG_WAIT_BE_TX_DONE	,
	EDCA_INFOFLAG_WAIT_VI_TX_DONE	, 
	EDCA_INFOFLAG_WAIT_VO_TX_DONE	, 
	
}EDCA_INFOFLAG;



#define EDCA_IS_ANY_ONE_WAIT_TX_DONE  (gEDCAInfo->info_flag)

#define EDCA_IS_WAIT_TX_DONE(_q) !!(gEDCAInfo->info_flag&(1<<_q))
#define EDCA_SET_Q_WAIT_TX_DONE(_q, _tf) \
    /*lint -save -e845 */ \
    do { \
        gEDCAInfo->info_flag = ((gEDCAInfo->info_flag & ~(1<<(_q)))| ((_tf)<<(_q))); \
    } while (0) /*lint -save -e845 */ \


// info for rate
struct phy_rate_info {
    u16 bitrate:7;      // values in IE
	u16 type:2;     // modulation
	u16 flag:1;     // for long/short preamble or long short GI
	u16 ackIndex:6; // ack rate hareware index
    u16 hkbps;          // Speed in hkbps, It is need to calculate perfect tx time
};

/*struct ecda_info {

	u32 info_flag;
	struct edca_txq   txq[EDCA_NUM_TX_QUEUES];

	
	

};


//s32 EDCA_init(void);

void EDCA_MacTxIntNotify(enum edca_tx_queue_subtype queuetype);*/

void EDCA_Handler(u8 id, PKT_TxInfo *pTxInfo, s32 reason);
void EDCA_FillRelatedDuration(PKT_TxInfo *pTxInfo, u16 fc);



#endif /* _EDCA_H_ */

