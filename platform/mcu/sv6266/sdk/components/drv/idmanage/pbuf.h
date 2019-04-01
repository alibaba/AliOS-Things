#ifndef _PBUF_H_
#define _PBUF_H_

#include "ssv_pktdef.h"
//#include <ssv6006_mac.h>
#include "attrs.h"

typedef enum __PBuf_Type_E {
    NOTYPE_BUF  = 0,
    TX_BUF      = 1,
    RX_BUF      = 2
} PBuf_Type_E;

struct tx_pktbuf {
	u32 *pktbuf;
	u8 *databuf;
};

#define HOST_PBUF TX_BUF
#define SOC_PBUF RX_BUF

bool PBUF_is_reusable(void *old_packet, u32 new_packet_len);
void        *PBUF_MAlloc_Raw(u32 size, u32 need_header, PBuf_Type_E buf_type) ATTRIBUTE_SECTION_FAST;
void		_PBUF_MFree(void *PKTMSG) ;
void *_PBUF_MReAlloc(void **_pp, u32 new_size);

#define PBUF_SET_REUSE(pp) ((*pp)=NULL) //parameter type is ponit to point


#define PBUF_BASE_ADDR	            0x80000000
#define PBUF_ADDR_SHIFT	            16
#define PBUF_ADDR_MAX               128
#define PBUF_PAGE_SIZE              256
#define TX_PKT_RES_BASE		    16
#define TX_PKT_RES_SHIFT            4       // <<4 = 16

//#define PBUF_GetPktByID(_ID)        (PBUF_BASE_ADDR|((_ID)<<PBUF_ADDR_SHIFT))
//#define PBUF_GetPktID(_PKTMSG)      (((u32)_PKTMSG&0x0FFFFFFF)>>PBUF_ADDR_SHIFT)

#define PBUF_isPkt(addr)    (((u32)(addr) >= (u32)PBUF_BASE_ADDR) && \
                                ((u32)(addr) <= (u32)PBUF_MapIDtoPkt(PBUF_ADDR_MAX-1)) && \
                                ((((u32)(addr))&0xFFFF)==0))

#define PBUF_MAlloc(size, type)  PBUF_MAlloc_Raw(size, 1, type);
#define PBUF_MFree(PKTMSG) _PBUF_MFree(PKTMSG)

/**
 * This function only support to reuse a pbuf, and this pbuf is a HOST_CMD, PKT_TxInfo or Pkt_RxInfo
 */
#define PBUF_MReAlloc(pp,new_size)  _PBUF_MReAlloc((void **)pp,new_size)

s32		PBUF_Init(void);
s32 PBUF_Check_Init(void);
s32 getpacketbuffer(struct tx_pktbuf *buf, int payload_len) ATTRIBUTE_SECTION_FAST;
void setpacketbuf_len(u32 *buf, int payload_len) ATTRIBUTE_SECTION_FAST;

#endif /* _PBUF_H_ */





