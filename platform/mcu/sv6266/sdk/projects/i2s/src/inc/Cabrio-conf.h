/* 
 * Copyright (c) 2010, Swedish Institute of Computer Science. 
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met: 
 * 1. Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer. 
 * 2. Redistributions in binary form must reproduce the above copyright 
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the distribution. 
 * 3. Neither the name of the Institute nor the names of its contributors 
 *    may be used to endorse or promote products derived from this software 
 *    without specific prior written permission. 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND 
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE 
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS 
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY 
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
 * SUCH DAMAGE. 
 * 
 */

/** 
 * \file 
 *         Common code between collect client and server 
 * \author 
 *         Niclas Finne <nfi@sics.se> 
 */

#ifndef __CABRIO_CONF_H__
#define __CABRIO_CONF_H__

//#include "contiki.h"
//#include "contiki-net.h"
//#include "svn_ver.h"

#include "types.h"

/* Define debug flag */
/**
#define DEBUG_LOG	1
#if DEBUG_LOG
#define DEBUG_MSG	printf(const char * fmt,...)
#else
#define DEBUG_MSG
*/

/* Define if ATE mode involve into this process */
#define OPMODE_ATE			1
#define AT_ECHO				0
#define CURRENT_TEST		(0)

#define MAX_SEND_BUFFER 	1472

#define CABRIO_MANUFACTURE	"iComm Company"
//#define CABRIO_VERISON	"SSV6060.Z0.1001.150522"
#define CABRIO_VERISON		"SSV6060.P2" SVN_VERSION ".0" 

/* ATE command definition section */
/* ATE command for receiving from host UART */
#define ATCMD_REBOOT                "AT+REBOOT"
#define ATCMD_VERSION               "AT+VERSION"
#define ATCMD_MINFO                 "AT+MF_INFO"
#define ATCMD_GETCONFIG             "AT+GET_CONFIG"
#define ATCMD_SETIFCONFIG           "AT+SET_IFCONFIG"
#define ATCMD_SETWIFICONFIG	        "AT+SET_WIFICONFIG"
#define ATCMD_NETSTATUS             "AT+NETSTATUS"
#define ATCMD_NETSCAN               "AT+SCAN"
#define ATCMD_CLEAN_LISTS           "AT+CLEAN_LISTS"
#define ATCMD_NETSCAN_CUSTOM        "AT+SCAN_CUSTOM"
#define ATCMD_SHOWCONNECTAP         "AT+SHOWCONNECTAP"
#define ATCMD_SHOWALLAP             "AT+SHOWALLAP"
#define ATCMD_CONNECT               "AT+WIFICONNECT"
#define ATCMD_RECONNECT             "AT+WIFIRECONNECT"
#define ATCMD_SEND                  "AT+SEND"
#define ATCMD_DISCONNECT            "AT+WIFIDISCONNECT"
#define ATCMD_SET_DEBUGINFO         "AT+SET_DEBUGINFO"

//TCP
#define ATCMD_TCPCHANGETIMEOUT      "AT+TCPCHANGE_TIMEOUT"
#define ATCMD_TCPCONNECT            "AT+TCPCONNECT"
#define ATCMD_TCPSEND               "AT+TCPSEND"
#define ATCMD_TCPSEND6              "AT+IPV6SEND"
#define ATCMD_TCPDISCONNECT         "AT+TCPDISCONNECT"
#define ATCMD_TCPLISTEN	            "AT+TCPLISTEN"
#define ATCMD_TCPLISTEN6	        "AT+IPV6LISTEN"
#define ATCMD_TCPUNLISTEN           "AT+TCPUNLISTEN"

#define ATCMD_TCPSEND_CURR          "AT+TCPSEND_CURRENT"
#define ATCMD_TCPDISCONNECT_CURR    "AT+TCPDISCONNECT_CURRENT"

#define ATCMD_SSLCONNECT            "AT+SSLCONNECT"
#define ATCMD_SSLSEND               "AT+SSLSEND"
#define ATCMD_SSLDISCONNECT         "AT+SSLDISCONNECT"

#define ATCMD_UDPCREATE             "AT+UDPCREATE"
#define ATCMD_UDPSEND               "AT+UDPSEND"
#define ATCMD_UDPCLOSE              "AT+UDPCLOSE"

#define ATCMD_HTTPTESTSTART         "AT+HTTPTESTSTART"
#define ATCMD_HTTPTESTSTOP          "AT+HTTPTESTSTOP"

#define ATCMD_SET_GPIO              "AT+SET_GPIO"
#define ATCMD_WRITE_GPIO            "AT+WRITE_GPIO"
#define ATCMD_READ_GPIO             "AT+READ_GPIO"
#define ATCMD_SET_PWM               "AT+SET_PWM"
#define ATCMD_POWERSAVE             "AT+POWERSAVE"
#define ATCMD_SET_RATE              "AT+SETRATE"
#define ATCMD_SET_PWMSPEC           "AT+SET_PWMSPEC"
#define ATCMD_SHOWPINGCNT           "AT+SHOWPINGCOUNT"
#define ATCMD_CLEARPINGCNT          "AT+CLEARPINGCOUNT"

//ota cmd
#define ATCMD_OTASETSERVER          "AT+OTASETSERVER"
#define ATCMD_OTASETPARAM           "AT+OTASETPARAM"
#define ATCMD_OTASTART              "AT+OTASTART"

//demo cmd
#define ATCMD_STACONNECT_DEMO       "AT+STACONNECT_DEMO"
#define ATCMD_USERSMART_DEMO        "AT+USERSMART_DEMO"
#define ATCMD_TCPSERVER_DEMO        "AT+TCPSERVER_DEMO"
#define ATCMD_WIFIUART_DEMO         "AT+WIFIUART_DEMO"

//cloud cmd
#define ATCMD_BUKET_CREATE		"AT+BUKET_CREATE"
#define ATCMD_BUKET_DELETE		"AT+BUKET_DELETE"
#define ATCMD_OBJECT_DOWNLOAD		"AT+OBJECT_DOWNLOAD"
#define ATCMD_OBJECT_UPDATE		"AT+OBJECT_UPDATE"

//send raw packet cmd
#define ATCMD_SEND_RAW_INIT       "AT+SNED_RAW_INIT"
#define ATCMD_SEND_RAW_PACKET       "AT+SNED_RAW_PACKET"
#define ATCMD_SET_RAW_REMOTE_MAC    "AT+SET_RAW_REMOTE_MAC"

#define ATCMD_ENABLE_SMARTMODE      "AT+ENABLE_SMARTMODE"
#define ATCMD_ENABLE_SMARTREBOOT    "AT+ENABLE_SMARTREBOOT"
#define ATCMD_SET_RFCALLBACK        "AT+SET_RFCALLBACK"

//### following are not AT CMD,they are for debugging ###
#define ATCMD_HELP                  "help"
#define ATCMD_TEST_PERIPHERAL       "testp"
#define ATCMD_SYSINFO				"sysinfo"
#define ATCMD_WRITE_REG             "regw"
#define ATCMD_READ_REG              "regr"
#define ATCMD_SET_CHANNEL           "setchannel"
#define ATCMD_RF                    "rf"
#define ATCMD_APSTATUS              "ap"
#define ATCMD_SOFTAP_STATUS         "softap"
#define ATCMD_WIFISTATUS            "wifi"
#define ATCMD_MRX                   "mrx"
#define ATCMD_PING                  "ping"
#define ATCMD_NETSTAT               "netstat"
#define ATCMD_IPCONF                "ifconfig"
#define ATCMD_FLASH_WRITE           "flashw"
#define ATCMD_FLASH_READ            "flashr"
#define ATCMD_FLASH_ERASE           "flashe"
#define ATCMD_FLASH_ERASE32K        "flash32ke"

#define ATCMD_CFS_LS                "cfsls"
#define ATCMD_CFS_INFO              "cfsinfo"
#define ATCMD_CFS_WRITE             "cfsw"
#define ATCMD_CFS_READ              "cfsrd"
#define ATCMD_CFS_REMOVE            "cfsrm"

#define ATCMD_BENCH_FLASH_READ      "bflashr"
#define ATCMD_BENCH_FLASH_WRITE     "bflashw"


#define ATCMD_PHY_COUNT             "phycount"
#define ATCMD_MEM_READ              "memr"
#define ATCMD_MEM_INFO              "meminfo"
#define ATCMD_MBOX                  "mbox"
#define ATCMD_PBUF                  "pbuf"
#define ATCMD_WIFI_RESET            "wifi_reset"
#define ATCMD_NTP_TIME              "ntptime"


//watch dog 

#define ATCMD_WDOG_START            "wdog_start"
#define ATCMD_WDOG_STOP             "wdog_stop"


//#if CURRENT_TEST
#if 0
#define ATCMD_POWERON				"AT+POWERON"
#define ATCMD_RX					"AT+RX"
#define ATCMD_TX					"AT+TX"
#define ATCMD_POWERSAVING			"AT+POWERSAVING="
#endif

#define ATCMD_REMOVE_CONF           "AT+REMOVE_CONF"
#define ATCMD_GET_LOCALMAC          "AT+GET_LOCALMAC"
#define ATCMD_DEBUG_PRINTF          "log"
#define ATCMD_PROCESS_DUMP          "ppd"
#define ATCMD_PROCESS_KILL          "ppk"
#define ATCMD_PROCESS_QUE_DUMP      "pqd"
#define ATCMD_PROCESS_QUE_KILL      "pqk"

//
#define ATCMD_BMODE_RESET           "breset"
#define ATCMD_BMODE_COUNT           "bcount"
#define ATCMD_GMODE_RESET           "greset"
#define ATCMD_GMODE_COUNT           "gcount"
#define ATCMD_FILTER_RESET          "freset"
#define ATCMD_FILTER_COUNT          "fcount"

#define ATCMD_NSLOOKUP              "AT+NSLOOKUP"
/* ATE response command for sending to host uart */
#define	ATRSP_OK                    "+OK\r\n"
#define	ATRSP_ERROR                 "+ERROR:%d\r\n"
#define	ATRSP_ERROR_STR           	"+ERROR:%s\r\n"
#define	ATRSP_VERSION               "+VERSION:%s\r\n"
#define	ATRSP_MANUFACTURE           "+MF_INFO:%s\r\n"
#define	ATRSP_CONFIGURE             "+GET_CONFIG=%d,%s,%s,%d,%d,%d.%d.%d.%d,%d.%d.%d.%d,%d.%d.%d.%d\r\n"
#define	ATRSP_NETSTATUS             "+NETSTATUS=%d,%d,%d\r\n"
#define ATRSP_GPIO                   "+GPIO=%d\r\n"

/*ATE indication command for sending to host uart */
#define	ATIND_SCAN                  "+SCAN:%s\r\n"
#define	ATIND_RECV                  "+RECV:%s\r\n"
#define	ATIND_CONNECT               "+CONNECT:%s\r\n"
#define	ATIND_DISCONNECT            "+DISCONNECT:%s\r\n"
#define	ATIND_GPIO                  "+GPIO:%d\r\n"

//ATE phy calibration command
#define ATCMD_PHY_INI               "AT+PHYINI"
#define ATCMD_PHY_CALI              "AT+PHYCALI"
#define ATCMD_EVM                   "AT+EVM"
#define ATCMD_TONE                  "AT+TONE"
#define ATCMD_SETCH                 "AT+SETCH"

//SOFT_AP TEST
#define ATCMD_WIFISTART             "AT+WIFISTART"
#define ATCMD_AP                    ("AT+AP")
#define ATCMD_HKAP                  ("AT+HKAP")
#define ATCMD_SET_APCONFIG          ("AT+SET_APCONFIG")
#define ATCMD_GET_APCONFIG          ("AT+GET_APCONFIG")
#define ATCMD_GET_APMODE            ("AT+GET_APMODE")
#define ATCMD_DBG_LEASETIME         ("AT+DBG_LEASETIME")
#define ATCMD_AP_EXIT               ("AT+AP_EXIT")

#define ATCMD_SET_AP_SSID           ("AT+SET_AP_SSID")      //set ssid
#define ATCMD_SET_AUTO_CONNECT      ("AT+SET_AUTO_CONNECT")      //auto-connect
#define ATCMD_SAVE_CONFIG           ("AT+SAVE_CONFIG")      //save config

//RF Test AT Command
#define ATCMD_RADIO_CHANNEL			"AT+RADIO_CHANNEL"
#define ATCMD_RADIO_RF_START			"AT+RADIO_RF_START"
#define ATCMD_RADIO_RF_RATE			"AT+RADIO_RF_RATE"
#define ATCMD_RADIO_RF_BGAIN			"AT+RADIO_RF_BGAIN"
#define ATCMD_RADIO_RF_GNGAIN			"AT+RADIO_RF_GNGAIN"
#define ATCMD_RADIO_RF_IQPHASE		"AT+RADIO_RF_IQPHASE"
#define ATCMD_RADIO_RF_IQAMP			"AT+RADIO_RF_IQAMP"
#define ATCMD_RADIO_RF_STOP			"AT+RADIO_RF_STOP"

#define ATCMD_RADIO_RF_RESET			"AT+RADIO_RF_RESET"
#define ATCMD_RADIO_RF_COUNT			"AT+RADIO_RF_COUNT"

#define ATCMD_RADIO_RF_FREQOFFSET	"AT+RADIO_RF_FREQOFFSET"
#define ATCMD_RADIO_RF_TEMPCS_HT		"AT+RADIO_RF_TEMPCS_HT"
#define ATCMD_RADIO_RF_TEMPCS_RT		"AT+RADIO_RF_TEMPCS_RT"
#define ATCMD_RADIO_RF_TEMPCS_LT		"AT+RADIO_RF_TEMPCS_LT"
#define ATCMD_RADIO_RF_READ_TEMPCS	"AT+RADIO_RF_READ_TEMPCS"
#define ATCMD_RADIO_RF_ENABLE_TCSR	"AT+RADIO_RF_ENABLE_TCSR"
#define ATCMD_RADIO_RF_SET_HT			"AT+RADIO_RF_SET_HT"
#define ATCMD_RADIO_RF_SET_RT			"AT+RADIO_RF_SET_RT"
#define ATCMD_RADIO_RF_SET_LT			"AT+RADIO_RF_SET_LT"
#define ATCMD_RADIO_RF_GET_HT			"AT+RADIO_RF_GET_HT"
#define ATCMD_RADIO_RF_GET_RT			"AT+RADIO_RF_GET_RT"
#define ATCMD_RADIO_RF_GET_LT			"AT+RADIO_RF_GET_LT"
#define ATCMD_RADIO_RF_LDO			"AT+RADIO_RF_LDO"
#define ATCMD_RADIO_RF_PA1			"AT+RADIO_RF_PA1"
#define ATCMD_RADIO_RF_PA2			"AT+RADIO_RF_PA2"
#define ATCMD_RADIO_RF_PA3			"AT+RADIO_RF_PA3"

#define ATCMD_RADIO_RF_XTAL			"AT+RADIO_RF_XTAL"
#define ATCMD_RADIO_RF_FLASH_XTAL	"AT+RADIO_RF_SET_XTAL"
#define ATCMD_RADIO_TEMP_BOUNDARY	"AT+RADIO_TEMP_BOUNDARY"
#define ATCMD_RADIO_RF_SINGLE			"AT+RADIO_RF_SINGLE"

#define ATCMD_RADIO_WRITE_MAC		"AT+RADIO_WRITE_MAC"
#define ATCMD_RADIO_READ_MAC			"AT+RADIO_READ_MAC"
#define ATCMD_RADIO_WRITE_SERIAL		"AT+RADIO_WRITE_SERIAL"
#define ATCMD_RADIO_READ_SERIAL		"AT+RADIO_READ_SERIAL"

#define ATCMD_RADIO_RF_DUMP			"AT+RADIO_RF_DUMP"
#define ATCMD_RADIO_RF_REG_DUMP		"AT+RADIO_RF_REG_DUMP"
#define ATCMD_RADIO_PHY_REG_DUMP		"AT+RADIO_PHY_REG_DUMP"
#define ATCMD_RADIO_SELF_CHECK		"AT+RADIO_CHECK"
#define ATCMD_RADIO_HELP	                	"rf"

#define ATCMD_ENABLE_DCDC		"AT+ENABLE_DCDC"

//#ifdef IPERF_ENABLE
#define ATCMD_IPERF                     "iperf"
//#endif
#define ATCMD_TCPST              "tcpdbg"

typedef enum t_TAG_WIFIOPMODE
{
	WIFI_B 				= 0,
	WIFI_G,
	WIFI_N,
	WIFI_GREENFIELD
} TAG_WIFIOPMODE;

typedef enum t_TAG_11N
{
	GI 		= 0,
	AMPDU,
	HT20,
	HT40
} TAG_11N;

typedef enum t_TAG_AIFS
{
	VO = 0,
	VI,
	BE,
	BK
} TAG_AIFS;

typedef enum t_TAG_APSTA
{
	AP 	= 0,
	STA
} TAG_APSTA;

typedef enum t_TAG_FRAMETYPE
{
	DATA = 0,
	CTRL,
	MGMT
} TAG_FRAMETYPE;

typedef enum t_TAG_TXDATA
{
	NORMAL = 0,
	CW_WAVE
} TAG_TXDATA;

#if 0
typedef struct t_TAG_NET_STATUS
{
	u8_t tcp_connect;
	u8_t udp_connect;
	uip_ip4addr_t	remote_ip_addr;
	u16_t			port;
	u16_t			listenport;
	u16_t			udpport;
} TAG_NET_STATUS;
#endif

#endif /* __CABRIO_CONF_H__ */
