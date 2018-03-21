#ifndef _WIFI_WOWLAN_H_
#define _WIFI_WOWLAN_H_

#include <platform_stdlib.h>
#include <osdep_service.h>

#define WOWLAN_DBG 1

enum{
	WOWLAN_DBG_OFF = 0,
	WOWLAN_DBG_ALWAYS,
	WOWLAN_DBG_ERROR,
	WOWLAN_DBG_WARNING,
	WOWLAN_DBG_INFO
};

#if WOWLAN_DBG
	//#define WOWLAN_DUMP_MSG
	#define WOWLAN_DUMP_MSG_1 //dump packet when setting
	static unsigned char  gWowlanDbgLevel = WOWLAN_DBG_ERROR;
	#define WOWLAN_PRINTK(fmt, args...)		printf(fmt"\r\n",## args)
	#define _WOWLAN_PRINTK(fmt, args...)	printf(fmt,## args)
	#define WOWLAN_DBG_MSG(level, fmt, args...)					\
			do{														\
				if(level <= gWowlanDbgLevel){	\
					WOWLAN_PRINTK(fmt,## args);							\
				}													\
			}while(0)
#else
	#define WOWLAN_PRINTK(fmt, args...)
	#define WOWLAN_DBG_MSG(level, fmt, args...)	
#endif

#ifndef u8
typedef unsigned char		u8;
typedef unsigned short		u16;
typedef unsigned int		u32;
#endif

#ifndef BIT
#define BIT(x)	((u32)1 << (x))
#endif

#ifndef le16_to_cpu //need a general definition for the whole system
#define cpu_to_le32(x) ((u32)(x))
#define le32_to_cpu(x) ((u32)(x))
#define cpu_to_le16(x) ((u16)(x))
#define le16_to_cpu(x) ((u16)(x))
#endif

#ifndef IP_FMT
#define IP_FMT "%d.%d.%d.%d"
#endif

#ifndef IP_ARG
#define IP_ARG(x) ((u8*)(x))[0],((u8*)(x))[1],((u8*)(x))[2],((u8*)(x))[3]
#endif

#ifndef MAC_FMT
#define MAC_FMT "%02x:%02x:%02x:%02x:%02x:%02x"
#endif

#ifndef MAC_ARG
#define MAC_ARG(x) ((u8*)(x))[0],((u8*)(x))[1],((u8*)(x))[2],((u8*)(x))[3],((u8*)(x))[4],((u8*)(x))[5]
#endif

#ifndef ETH_ALEN
#define ETH_ALEN 6
#endif

#ifndef ethhdr
struct ethhdr 
{
	unsigned char	h_dest[ETH_ALEN];	/* destination eth addr	*/
	unsigned char	h_source[ETH_ALEN];	/* source ether addr	*/
	unsigned short	h_proto;		/* packet type ID field	*/
};
#endif

#ifndef wowlan_memcpy
	#define wowlan_memcpy(d, s, n)	rtw_memcpy((void*)(d), ((void*)(s)), (n))
#endif

#ifndef wowlan_malloc
	#define wowlan_malloc(sz)	rtw_malloc(sz)
#endif

#ifndef wowlan_zmalloc
	#define wowlan_zmalloc(sz)	rtw_zmalloc(sz)
#endif

#ifndef wowlan_memset
	#define wowlan_memset(pbuf, c, sz)	rtw_memset(pbuf, c, sz)
#endif

#ifndef wowlan_mfree
	#define wowlan_mfree(p, sz)	rtw_mfree(((u8*)(p)), (sz))
#endif

#ifndef wowlan_memcmp
	#define wowlan_memcmp(s1, s2, n) rtw_memcmp(((void*)(s1)), ((void*)(s2)), (n))
#endif

#ifndef wowlan_mdelay_os
	#define wowlan_mdelay_os(ms) rtw_mdelay_os(ms)
#endif

/*Mutex services*/
typedef _mutex _wowlock;

__inline static void _init_wowlock(_wowlock *plock)
{
	rtw_mutex_init(plock);
}

__inline static void _free_wowlock(_wowlock *plock)
{
	rtw_mutex_free(plock);
}

__inline static void _enter_wowlock(_wowlock *plock)
{
	rtw_mutex_get(plock);
}

__inline static void _exit_wowlock(_wowlock *plock)
{
	rtw_mutex_put(plock);
}

/*Timer services*/
typedef _timerHandle _wowTimer;
#define TMR_AUTO_RELOAD_EN		_TRUE
#define TMR_AUTO_RELOAD_DIS	_FALSE

__inline static void
_wowlan_init_timer(_wowTimer *ptimer, void *adapter, TIMER_FUN pfunc,void* cntx, const char *name, u32 auto_reload)
{
	*ptimer = rtw_timerCreate(
				(signed const char *)name, 		// Just a text name, not used by the RTOS kernel. 
				TIMER_MAX_DELAY,		// Timer Period, not 0
				auto_reload,		// Whether timer will auto-load themselves when expires
				cntx,		// Uniq id used to identify which timer expire.. 
				pfunc		// Timer callback							
			);
}

__inline static void 
_wowlan_set_timer(_wowTimer *ptimer, u32 delay_time_ms)
{
	if(rtw_timerChangePeriod(*ptimer, rtw_ms_to_systime(delay_time_ms), TIMER_MAX_DELAY) == _FAIL)
		WOWLAN_PRINTK("Fail to set timer period");
}

__inline static void 
_wowlan_cancel_timer(_wowTimer *ptimer)
{
	rtw_timerStop(*ptimer, TIMER_MAX_DELAY);
}

__inline static void 
_wowlan_del_timer(_wowTimer *ptimer)
{
	rtw_timerDelete(*ptimer, TIMER_MAX_DELAY);
}

__inline static void *
_wowlan_get_timer_cntx(_wowTimer timer)
{
#ifdef PLATFORM_FREERTOS
#include <FreeRTOS.h>
#include <timers.h>
	return pvTimerGetTimerID(timer);
#else
	#error "_wowlan_get_timer_cntx is not defined"
#endif
}

enum rtw_wowlan_wakeup_reason {
	RTW_WOWLAN_WAKEUP_BY_PATTERN				= BIT(0),
	RTW_WOWLAN_WAKEUP_BY_DISCONNECTION			= BIT(1),
	RTW_WOWLAN_WAKEUP_MAX 						= 0x7FFFFFFF
}; 

enum rtw_wowlan_cmd_id{
	RTW_WOWLAN_CMD_ENABLE = 0x01, // enable wowlan service
	RTW_WOWLAN_CMD_PATTERNS = 0x02, // wowlan pattern setting
	RTW_WOWLAN_CMD_PROT_OFFLOAD_CONFIG = 0x03, //ARP offload setting
	RTW_WOWLAN_CMD_GET_STATUS = 0x04, // get rtw_wowlan_status
	RTW_WOWLAN_CMD_CLEAR_ALL = 0x05, //clear wowlan content
	RTW_WOWLAN_CMD_KEEPALIVE = 0x06, //for keep alive packet setting
	RTW_WOWLAN_CMD_MAX = 0xff
};

#define RTW_WOWLAN_MAX_RX_FILTERS			(5)
#define RTW_WOWLAN_RX_FILTER_MAX_FIELDS	(8)
#define RTW_WOWLAN_ID_OFFSET				(100) //to match some application, ID starts from 100
#define RTW_WOWLAN_MIN_FILTERS_ID			(RTW_WOWLAN_ID_OFFSET)
#define RTW_WOWLAN_MAX_FILTERS_ID			(RTW_WOWLAN_ID_OFFSET+RTW_WOWLAN_MAX_RX_FILTERS-1)

struct rtw_wowlan_rx_filter_field {
	u16 offset;
	u8 len;
	u8 flags;
	u8 *mask;
	u8 *pattern;
};

struct rtw_wowlan_rx_filter {
	u8 action;
	u8 offset;
	u8 num_fields;
	struct rtw_wowlan_rx_filter_field fields[RTW_WOWLAN_RX_FILTER_MAX_FIELDS];
};

#if defined(__IAR_SYSTEMS_ICC__)|| defined (__GNUC__)
#pragma pack(1)
#else
#error "this structure needs to be packed!"
#endif
struct rtw_wowlan_status {
	u32 wakeup_reasons; //record wake up reason
	u32 filter_id; //record which pattern is matched
};
#if defined(__IAR_SYSTEMS_ICC__)|| defined (__GNUC__)
#pragma pack()
#else
#error "this structure needs to be packed!"
#endif

/**
 * struct rtw_wowlan_keepalive_packet
 *
 * @payload_len: data payload length
 * @payload: data payload buffer
 * @data_interval: interval at which to send data packets
**/
#define RTW_WOWLAN_MAX_KPALIVE_PKT 3
#define RTW_WOWLAN_MAX_KPALIVE_PKT_SZ 512
struct rtw_wowlan_keepalive_packet{
	u8 packet_id;
	int payload_len;
	u8 *payload;
	u32 data_interval;
	_wowTimer keepalive_tmr;
};

struct rtw_wowlan_ops {
	int (*DevWowlanInit)(void);	
	int (*DevWowlanEnable)(void);
	int (*DevWowlanDisable)(void);
	int (*DevWowlanWakeUp)(void);
	int (*DevWowlanSleep)(void);
};

/**
 * enum rtw_wowlan_proto_offloads - enabled protocol offloads
 * @RTW_WOWLAN_PROTO_OFFLOAD_ARP: ARP data is enabled
 */
enum rtw_wowlan_proto_offloads {
	RTW_WOWLAN_PROTO_OFFLOAD_ARP	= BIT(0),
	RTW_WOWLAN_PROTO_OFFLOAD_MAX	= 0x7FFFFFFF
};

/**
 * struct rtw_wowlan_proto_offload_common - ARP/NS offload common part
 * @enabled: enable flags
 * @remote_ipv4_addr: remote address to answer to (or zero if all)
 * @host_ipv4_addr: our IPv4 address to respond to queries for
 * @arp_mac_addr: our MAC address for ARP responses
 * @reserved: unused
 */
struct rtw_wowlan_proto_offload_common{
	int proto_enabled;
	u32 remote_ipv4_addr;
	u32 host_ipv4_addr;
	u8 host_mac_addr[ETH_ALEN];	
	u16 reserved;
};

struct rtw_wowlan {
	_wowlock wow_mutex;
	bool enabled;
	struct rtw_wowlan_status status;
	struct rtw_wowlan_ops ops;
	struct rtw_wowlan_proto_offload_common proto;
	bool proto_offload_enabled;
	struct rtw_wowlan_rx_filter *rx_filter[RTW_WOWLAN_MAX_RX_FILTERS];
	bool rx_filter_enabled[RTW_WOWLAN_MAX_RX_FILTERS];/* RX Data filter rule state - enabled/disabled */
	struct rtw_wowlan_keepalive_packet *tx_keepalive[RTW_WOWLAN_MAX_KPALIVE_PKT];
	bool tx_keepalive_enabled[RTW_WOWLAN_MAX_KPALIVE_PKT];/* TX keep avlive rule state - enabled/disabled */
};

#define eqMacAddr(a,b)						( ((a)[0]==(b)[0] && (a)[1]==(b)[1] && (a)[2]==(b)[2] && (a)[3]==(b)[3] && (a)[4]==(b)[4] && (a)[5]==(b)[5]) ? 1:0 )
#define cpMacAddr(des,src)					((des)[0]=(src)[0],(des)[1]=(src)[1],(des)[2]=(src)[2],(des)[3]=(src)[3],(des)[4]=(src)[4],(des)[5]=(src)[5])
#define cpIpAddr(des,src)					((des)[0]=(src)[0],(des)[1]=(src)[1],(des)[2]=(src)[2],(des)[3]=(src)[3])

#define RTW_WOWLAN_GET_ARP_PKT_OPERATION(__pHeader) 				ReadEF2Byte( ((u8*)(__pHeader)) + 6)
#define RTW_WOWLAN_GET_ARP_PKT_SENDER_MAC_ADDR(__pHeader, _val) 	cpMacAddr((u8*)(_val), ((u8*)(__pHeader))+8)
#define RTW_WOWLAN_GET_ARP_PKT_SENDER_IP_ADDR(__pHeader, _val) 		cpIpAddr((u8*)(_val), ((u8*)(__pHeader))+14)
#define RTW_WOWLAN_GET_ARP_PKT_TARGET_MAC_ADDR(__pHeader, _val) 	cpMacAddr((u8*)(_val), ((u8*)(__pHeader))+18)
#define RTW_WOWLAN_GET_ARP_PKT_TARGET_IP_ADDR(__pHeader, _val) 	cpIpAddr((u8*)(_val), ((u8*)(__pHeader))+24)

#define RTW_WOWLAN_SET_ARP_PKT_HW(__pHeader, __Value)  				WriteEF2Byte( ((u8*)(__pHeader)) + 0, __Value)
#define RTW_WOWLAN_SET_ARP_PKT_PROTOCOL(__pHeader, __Value)  			WriteEF2Byte( ((u8*)(__pHeader)) + 2, __Value)
#define RTW_WOWLAN_SET_ARP_PKT_HW_ADDR_LEN(__pHeader, __Value)  		WriteEF1Byte( ((u8*)(__pHeader)) + 4, __Value)
#define RTW_WOWLAN_SET_ARP_PKT_PROTOCOL_ADDR_LEN(__pHeader, __Value)  	WriteEF1Byte( ((u8*)(__pHeader)) + 5, __Value)
#define RTW_WOWLAN_SET_ARP_PKT_OPERATION(__pHeader, __Value) 		WriteEF2Byte( ((u8*)(__pHeader)) + 6, __Value)
#define RTW_WOWLAN_SET_ARP_PKT_SENDER_MAC_ADDR(__pHeader, _val) 	cpMacAddr(((u8*)(__pHeader))+8, (u8*)(_val))
#define RTW_WOWLAN_SET_ARP_PKT_SENDER_IP_ADDR(__pHeader, _val) 		cpIpAddr(((u8*)(__pHeader))+14, (u8*)(_val))
#define RTW_WOWLAN_SET_ARP_PKT_TARGET_MAC_ADDR(__pHeader, _val) 	cpMacAddr(((u8*)(__pHeader))+18, (u8*)(_val))
#define RTW_WOWLAN_SET_ARP_PKT_TARGET_IP_ADDR(__pHeader, _val) 		cpIpAddr(((u8*)(__pHeader))+24, (u8*)(_val))

#define RTW_WOWLAN_ARP_PKT_LEN		0x2A
#define RTW_WOWLAN_ARP_PKT_OPERATION_REQ		0x0100 //arp request
#define RTW_WOWLAN_ARP_PKT_OPERATION_RSP		0x0200 //arp response

extern u8 key_2char2num(u8 hch, u8 lch);
extern _LONG_CALL_ void __rtl_memDump_v1_00(const u8 *start, u32 size, char * strHeader);
#define rtw_wowlan_DumpForBytes(pData, Len) __rtl_memDump_v1_00(pData, Len, NULL)

#define PWOWLAN_TO_STATUS(pwowlan)			(&pwowlan->status)
#define PWOWLAN_TO_OPS(pwowlan)			(&pwowlan->ops)
#define PWOWLAN_TO_PROTO(pwowlan)			(&pwowlan->proto)
#define PWOWLAN_TO_RX_FILTER(pwowlan)		(pwowlan->rx_filter)
#define PWOWLAN_TO_TX_KEEPALIVE(pwowlan)	(pwowlan->tx_keepalive)

/**
 * rtw_wowlan_init: initialize wowlan service
 * arg: None
 * return: _SUCCESS or _FAIL
 */
extern int rtw_wowlan_init(void);

/**
 * cmd_wowlan_service: input commands to configure wowlan service
 * arg:
 * @argc: number of input parameter
 * @argv: content of input string
 * return: None
 */
extern void cmd_wowlan_service(int argc, char **argv);

/**
 * rtw_wowlan_process_rx_packet: entry for packet process in wowlan service once it starts
 * arg: 
 * @rx_pkt: receive packet from wlan/ethernet 
 * @pkt_len: receive packet length
 * return: _SUCCESS or _FAIL
 */
extern int rtw_wowlan_process_rx_packet(char *rx_pkt, u16 pkt_len);

/**
 * rtw_wowlan_wakeup_process: wake up process once the reasons are matched, 
 *       refer to enum rtw_wowlan_wakeup_reason
 * arg: 
 * @reason: wake up reason, refer to enum rtw_wowlan_wakeup_reason  
 * return: None
 */
extern void rtw_wowlan_wakeup_process(int reason);

/**
 * rtw_wowlan_is_enabled: if wowlan service is already enabled
 * this function is called in rx path and wifi_inidication when wowlan service is running
 * arg: None
 * return: _True if enable or _False if disable
 */
extern int rtw_wowlan_is_enabled(void);

/**
 * rtw_wowlan_get_wk_reason: query wake up reason, refer to enum rtw_wowlan_wakeup_reason
 * arg: None
 * return: wakeup_reason
 */
extern int rtw_wowlan_get_wk_reason(void);

/**
 * rtw_wowlan_dev_sleep: sleep process on Ameba side, pull control for example
 * this function is linked to dev_wowlan_sleep_process() in dev_wowlan.c
 * arg: None
 * return: None
 */
extern void rtw_wowlan_dev_sleep(void);

#endif
