/*
note : Frank Yu (2012/12/21)
	This is the content of the previous 'regs.h' before reg modification.
Just for temp compatibily purpose. This file will be removed in the future.
*/
#ifndef _SSV_REGS_H_
#define _SSV_REGS_H_

#include <soc_types.h>
#include <net_types.h>
//#include <regs.h>

/**
 * struct DMA_CTRL_st - DMA register definition
 *
 * @ DMA_SRC[31:00]: Source address
 * @ DMA_DST[31:00]: Destination address
 * @ DMA_CTRL: DMA Control
 */
/*lint -save -e751 */
typedef struct DMA_CTRL_st {
    SSV6XXX_REG     DMA_SRC;   // 0x00
    SSV6XXX_REG     DMA_DST;   // 0x04
    SSV6XXX_REG     DMA_CTRL; // 0x08
    SSV6XXX_REG     DMA_INT;   // 0x0C

} DMA_CTRL, *PDMA_CTRL;
/*lint -restore */

/**
 * struct RTC_st - RTC register definition
 *
 * @ RTC_CTRL0[31:0]:
 * @ RTC_CTRL1[31:0]:
 * @ RTC_CNT[31:0]:
 * @ RTC_ALARM[31:0]:
 * @ RTC_RAM00[31:0] ~ RTC_RAM1f[31:0]:
 *
 */
typedef struct RTC_st {
    SSV6XXX_REG     RTC_CTRL0;
    SSV6XXX_REG     RTC_CTRL1;
    SSV6XXX_REG     RTC_CNT;
    SSV6XXX_REG     RTC_ALARM;
    SSV6XXX_REG     RTC_RAM00[1];
} RTC, *PRTC;


/**
 * struct WDT_st - Watch dog register definition
 *
 * @ WDT_TIMER[15:0]: Watch-dog timer value
 */
typedef struct WDT_st {
    SSV6XXX_REG     WDT_TIMER;

} WDT, *PWDT;



/**
* struct WSID_ENTRY_st - WSID Table Entry
*
* @ WINFO
* @ MAC00_31
*/
/*lint -save -e18 */
typedef struct WSID_ENTRY_st 
{
    SSV6XXX_REG     WINFO;
    SSV6XXX_REG     MAC00_31;
    SSV6XXX_REG     MAC32_47;

    struct {
    SSV6XXX_REG     UP_ACK_POLICY;
    SSV6XXX_REG     UP_TX_SEQCTL;
    } s[8];

    SSV6XXX_REG     RSVD;

} WSID_ENTRY, *PWSID_ENTRY;
/*lint -restore */


typedef struct WSID_RX_ENTRY_st 
{
   SSV6XXX_REG UP_RX_SEQCTL[8];

} WSID_RX_ENTRY, *PWSID_RX_ENTRY;


/**
* struct MRX_DECISION_st - MAC Rx Decision Table Entry.
*
* @ DECI_VAL
*/
typedef struct MRX_DECISION_st {
    SSV6XXX_REG     DECI_VAL[16];

} MRX_DECISION, *PMRX_DECISION;



/**
* struct MRX_DECISION_MASK_st - MAC Rx Decision Mask Table Entry.
*
* @ DECI_MSK
*/
typedef struct MRX_DECISION_MASK_st {
    SSV6XXX_REG      DECI_MSK[9];

} MRX_DECI_MASK, *PMRX_DECI_MASK;


/**
* struct GMFLT_ENTRY_st - Group MAC address filtering Table Entry.
* 
* @ 
*/
typedef struct GMFLT_ENTRY_st {
    SSV6XXX_REG     TB_MAC00_31;
    SSV6XXX_REG     TB_MAC32_47;
    SSV6XXX_REG     MK_MAC00_31;
    SSV6XXX_REG     MK_MAC32_47;
    SSV6XXX_REG     control;
} MRX_GMFLT_TBL, *PMRX_GMFLT_TBL;



/**
* struct PHYINFO_TBL_st - PHY Info Table (register table)
*
*/
typedef struct PHYINFO_TBL_st {
    SSV6XXX_REG     INFO;
} PHYINFO_TBL, *PPHYINFO_TBL;





/****************************************************************************
 *  Definition of Base Register Address
 ****************************************************************************/
#define REG_CPU_UART            ((REG_UART  *)(UART_REG_BASE))
#define REG_HCI_UART            ((REG_UART  *)(UART_REG_BASE+0x0100))


#define REG_WSID_ENTRY          ((WSID_ENTRY    *)ADR_WSID0)
#define REG_WSID_ENTRY_extra    ((WSID_ENTRY    *)ADR_WSID2)
#define REG_MRX_DECTBL          ((MRX_DECISION  *)ADR_MRX_FLT_TB0)
#define REG_MRX_DECMSK          ((MRX_DECI_MASK *)ADR_MRX_FLT_EN0)
#define REG_MRX_GMFLT           ((MRX_GMFLT_TBL *)ADR_MRX_MCAST_TB0_0)



















// 'REG_MODIFY_TEMP' -> Frank's temp use macro for reg modify job, will be removed in the future.
#if (defined REG_MODIFY_TEMP)
	extern void FCMD_dump();
	extern void TX_FLOW_dump();
	extern void GLBLE_SET_dump();
	extern void REG_MRX_DECTBL_dump();
	extern void REG_MRX_DECMSK_dump();
	extern void TX_ETHER_TYPE_dump();
	extern void RX_ETHER_TYPE_dump();
#else
	#define	FCMD_dump()
	#define TX_FLOW_dump()
	#define GLBLE_SET_dump()
	#define REG_MRX_DECTBL_dump()
	#define REG_MRX_DECMSK_dump()
	#define TX_ETHER_TYPE_dump()
	#define RX_ETHER_TYPE_dump()
#endif	/* CONFIG_SIM_PLATFORM */



#define TX_PKT_RES_BASE		16

#define MTX_QUEUE_MASK (MTX_HALT_Q_MB_MSK >> MTX_HALT_Q_MB_SFT)

#endif	/* _SSV_REGS_H_ */
