#ifndef _SOC_GLOBAL_H_
#define _SOC_GLOBAL_H_

/*#include <phy/drv_phy.h>
#include <security/drv_security.h>
#include <rtos.h>*/

struct soc_mib_st
{
	/* SW MIB counter for soft-mac */
	u32			sw_mac_rx_cnt;
	u32 		sw_mac_drop_cnt;

	/* SW MIB counter for CPU reason dispatcher */
	u32			sw_mac_handle_cnt;
	struct {
		u32		hdl_defrag_rx_cnt;
		u32     hdl_defrag_accept;

	} hdl;

	/* SW MIB counter for cmd-engine */
	u32			sw_cmd_rx_cnt;

	/* SW MIB counter for MLME */
	struct {
		u32		sw_mlme_rx_cnt;

	} mlme;

};

//record rate set
struct associate_req_rate{
    u16 rate_set;
    u32 rate_set_len;
    u32 basic_rate_set;
};

extern ETHER_ADDR WILDCARD_ADDR;

extern u32 gTxFlowDataReason;
extern u32 gRxFlowDataReason;
extern u16 gTxFlowMgmtReason;
extern u16 gRxFlowMgmtReason;
extern u16 gRxFlowCtrlReason;

extern u32 g_free_msgevt_cnt;

#if 0
extern u32 g_sec_pbuf_addr;
extern u32 g_phy_pbuf_addr;
#endif


//extern struct task_info_st g_soc_task_info[];
extern struct soc_mib_st g_soc_mib;
extern struct associate_req_rate assoc_req_rate;
#if (CLI_HISTORY_ENABLE==1)

char gCmdHistoryBuffer[CLI_HISTORY_NUM][CLI_BUFFER_SIZE+1];
char gCmdHistoryIdx;
char gCmdHistoryCnt;

#endif//CLI_HISTORY_ENABLE


extern u32 gSoftTaskCount;
extern u32 gMLMETaskCount;


#endif /* _SOC_GLOBAL_ */

