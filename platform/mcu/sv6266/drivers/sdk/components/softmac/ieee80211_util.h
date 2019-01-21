#ifndef __IEEE80211_UTIL_H__
#define __IEEE80211_UTIL_H__

/*	WMM_Specification_1-1 : Table 14  802.1D Priority to AC mappings

	UP		Access Category
	-------------------------
	1, 2	AC_BK
	0, 3	AC_BE
	4, 5	AC_VI
	6, 7	AC_VO
*/
#define	AC_BK	 0
#define	AC_BE	 1
#define	AC_VI	 2
#define	AC_VO	 3

extern u8 ssv6xxx_get_pbuf_hdr80211(void *p,u32 i);
extern u8* ssv6xxx_get_addr_hdr80211(void *p,u32 i);
#define	GET_PBUF_HDR80211(p, i)               ssv6xxx_get_pbuf_hdr80211((void *)p,i)
#define	GET_ADDR_HDR80211(p, i)               ssv6xxx_get_addr_hdr80211((void *)p,i)

#define	SET_PBUF_HDR80211(p, i)				(*((u8 *)(p)+(p)->hdr_offset + (i)))

#define	GET_SC_SEQNUM(sc)				(((sc) & 0xfff0) >> 4)
#define	GET_SC_FRAGNUM(sc)				(((sc) & 0x000f)     )

#define GET_QC_TID(qc)					((qc) & 0x000f)
#define	GET_QC_UP(qc)					((qc) & 0x0007)
#define GET_QC_AC(qc) 					((GET_QC_UP(qc) == 0) ? AC_BE : \
										 (GET_QC_UP(qc) == 1) ? AC_BK : \
										 (GET_QC_UP(qc) == 2) ? AC_BK : \
										 (GET_QC_UP(qc) == 3) ? AC_BE : \
										 (GET_QC_UP(qc) == 4) ? AC_VI : \
										 (GET_QC_UP(qc) == 5) ? AC_VI : \
										 (GET_QC_UP(qc) == 6) ? AC_VO : AC_VO)


#define GET_HDR80211_FC(p)				(((p)->f80211==1) ? (((u16)GET_PBUF_HDR80211(p, 1) << 8) | GET_PBUF_HDR80211(p, 0)) : 0)
#define GET_HDR80211_FC_TYPE(p)			((GET_HDR80211_FC(p) & 0x0c) >> 2)
#define GET_HDR80211_FC_SUBTYPE(p)			((GET_HDR80211_FC(p) & 0xF0) >> 4)
#define GET_HDR80211_FC_TYPE_STR(t)		((t == 0) ? "Mgmt" : ((t == 1) ? "Control" : ((t == 2) ? "Data" : "Reserved")))
#define	GET_HDR80211_FC_VER(p)			((GET_HDR80211_FC(p) & M_FC_VER))
#define GET_HDR80211_FC_TODS(p)			((GET_HDR80211_FC(p) & M_FC_TODS)      >>  8)
#define GET_HDR80211_FC_FROMDS(p)		((GET_HDR80211_FC(p) & M_FC_FROMDS)    >>  9)
#define GET_HDR80211_FC_MOREFRAG(p)		((GET_HDR80211_FC(p) & M_FC_MOREFRAGS) >> 10)
#define	GET_HDR80211_FC_RETRY(p)	    ((GET_HDR80211_FC(p) & M_FC_RETRY)     >> 11)
#define	GET_HDR80211_FC_PWRMGNT(p)		((GET_HDR80211_FC(p) & M_FC_PWRMGMT)   >> 12)
#define	GET_HDR80211_FC_MOREDATA(p)		((GET_HDR80211_FC(p) & M_FC_MOREDATA)  >> 13)
#define	GET_HDR80211_FC_PROTECTED(p)	((GET_HDR80211_FC(p) & M_FC_PROTECTED) >> 14)
#define	GET_HDR80211_FC_ORDER(p)		((GET_HDR80211_FC(p) & M_FC_ORDER)     >> 15)

#define SET_HDR80211_FC_MOREFRAG(p)		(SET_PBUF_HDR80211(p, 1) |= (M_FC_MOREFRAGS>>8))
#define SET_HDR80211_FC_MOREDATA(p)		(SET_PBUF_HDR80211(p, 1) |= (M_FC_MOREDATA>>8))
#define SET_HDR80211_FC_FROMDS(p)       (SET_PBUF_HDR80211(p, 1) |= (M_FC_FROMDS>>8))
#define UNSET_HDR80211_FC_MOREFRAG(p)	(SET_PBUF_HDR80211(p, 1) &= 0xfb)

#define GET_HDR80211_SC(p)				((u16)GET_PBUF_HDR80211(p, 23) << 8 | (GET_PBUF_HDR80211(p, 22)))
#define GET_HDR80211_SC_SEQNUM(p)		((GET_HDR80211_SC(p) & 0xfff0) >> 4)
#define SET_HDR80211_SC_SEQNUM(p, seq)		(SET_PBUF_HDR80211(p,22) = (u8)((seq<<4)&0x00f0)); \
                                                (SET_PBUF_HDR80211(p,23) = (u8)(((seq<<4)&0xff00)>>8));
#define GET_HDR80211_SC_FRAGNUM(p)		((GET_HDR80211_SC(p) & 0x000f))
#define GET_HDR80211_Ether_Type(p)      (((p)->f80211==1) ? ((p->qos == 1)?(((u16)GET_PBUF_HDR80211(p, 32) << 8) | GET_PBUF_HDR80211(p, 33)):(((u16)GET_PBUF_HDR80211(p, 30) << 8) | GET_PBUF_HDR80211(p, 31))) : 0)

//
//	Function			ToDS	FromDS	Addr1	Addr2	Addr3	Addr4
//	-------------------------------------------------------------------------
//	IBSS			0		0		DA		SA		BSSID	Not_Used
//	To AP (infra.)		1		0		BSSID	SA		DA		Not_Used
//	From AP (infra.)	0		1		DA		BSSID	SA		Not_Used
//	WDS (bridge)		1		1		RA		TA		DA		SA
#define HAS_HDR80211_ADDRESS_4(p)		(GET_HDR80211_FC_TODS(p) & GET_HDR80211_FC_FROMDS(p))

// QoS Control Field
#define	GET_HDR80211_QC(p)				(((p)->qos == 1) ? (((u16)GET_PBUF_HDR80211(p, 25 + (HAS_HDR80211_ADDRESS_4(p)*6)) << 8) | GET_PBUF_HDR80211(p, 24 + (HAS_HDR80211_ADDRESS_4(p)*6))) : 0)
#define	SET_HDR80211_QC(p,v)    \
        SET_PBUF_HDR80211(p, 24)=v&0xFF;\
        SET_PBUF_HDR80211(p, 25)=(v>>8)&0xFF;
#define GET_HDR80211_ADDRESS_1(a, p)	memcpy((a), ((u8 *)(p)+(p)->hdr_offset +  4), 6)
#define SET_HDR80211_ADDRESS_1(a, p)	memcpy(((u8 *)(p)+(p)->hdr_offset +  4),(a), 6)
#define GET_HDR80211_ADDRESS_2(a, p)	memcpy((a), ((u8 *)(p)+(p)->hdr_offset + 10), 6)
#define SET_HDR80211_ADDRESS_2(a, p)	memcpy(((u8 *)(p)+(p)->hdr_offset +  10),(a), 6)
#define GET_HDR80211_ADDRESS_3(a, p)	memcpy((a), ((u8 *)(p)+(p)->hdr_offset + 16), 6)
#define SET_HDR80211_ADDRESS_3(a, p)    memcpy(((u8 *)(p)+(p)->hdr_offset + 16),(a), 6)

#endif // __IEEE80211_UTIL_H__
