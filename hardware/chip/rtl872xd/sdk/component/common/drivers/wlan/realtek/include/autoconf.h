/******************************************************************************
 * Copyright (c) 2013-2016 Realtek Semiconductor Corp.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ******************************************************************************/


#ifndef WLANCONFIG_H
#define WLANCONFIG_H

/*
 * Include user defined options first. Anything not defined in these files
 * will be set to standard values. Override anything you dont like!
 */
#include "platform_opts.h"

#if defined(CONFIG_PLATFORM_8195A) || defined(CONFIG_PLATFORM_8711B) || defined(CONFIG_PLATFORM_8721D) || defined(CONFIG_PLATFORM_8195BHP) || defined(CONFIG_PLATFORM_8710C)
#ifndef CONFIG_PLATFORM_AMEBA_X
#define CONFIG_PLATFORM_AMEBA_X 1
#endif
#else
#define CONFIG_PLATFORM_AMEBA_X 0
#endif

#if (CONFIG_PLATFORM_AMEBA_X == 1)
#if defined(CONFIG_PLATFORM_8195BHP) || defined(CONFIG_PLATFORM_8710C)
	#define CONFIG_AXI_HCI
	#else
	#define CONFIG_LX_HCI
	#endif
#else
	#if defined(CONFIG_PLATFOMR_CUSTOMER_RTOS)
	#define PLATFORM_CUSTOMER_RTOS 1
	#define CONFIG_LWIP_LAYER 0
	#else
	#define PLATFORM_FREERTOS	1
	#endif
#if 1
	#undef PLATFORM_FREERTOS
	//#undef PLATFORM_ALIOS
	#define PLATFORM_ALIOS	1
#endif
	#ifdef USE_SDIO_INTERFACE
	#define CONFIG_SDIO_HCI
	#else
	#define CONFIG_GSPI_HCI
#endif
#endif // #if (CONFIG_PLATFORM_AMEBA_X == 1)


#if defined(CONFIG_HARDWARE_8188F) || defined(CONFIG_HARDWARE_8192E)|| defined(CONFIG_HARDWARE_8723D) || defined(CONFIG_HARDWARE_8821C) || defined(CONFIG_PLATFORM_8195BHP) || defined(CONFIG_HARDWARE_8188E) || defined(CONFIG_PLATFORM_8721D) || defined(CONFIG_PLATFORM_8710C)
#define CONFIG_FW_C2H_PKT
#define PHYDM_LINUX_CODING_STYLE 1
#else
#define PHYDM_LINUX_CODING_STYLE 0
#endif

#if (PHYDM_LINUX_CODING_STYLE == 1)
#define PHYDM_NEW_INTERFACE 1
#else
#define PHYDM_NEW_INTERFACE 0
#endif

#ifndef CONFIG_INIC_EN
#define CONFIG_INIC_EN 0 //For iNIC project
#endif

#if CONFIG_INIC_EN
#define CONFIG_LWIP_LAYER    0
#endif

#ifndef CONFIG_WIFI_CRITICAL_CODE_SECTION
#define CONFIG_WIFI_CRITICAL_CODE_SECTION
#endif
#define CONFIG_LITTLE_ENDIAN
#define CONFIG_80211N_HT
#define CONFIG_RECV_REORDERING_CTRL
#define RTW_NOTCH_FILTER 0
#define CONFIG_EMBEDDED_FWIMG
#define CONFIG_PHY_SETTING_WITH_ODM
#if (CONFIG_PLATFORM_AMEBA_X == 0)
#define HAL_MAC_ENABLE 1
#define HAL_BB_ENABLE 1
#define HAL_RF_ENABLE 1
#endif
#if (CONFIG_PLATFORM_AMEBA_X == 1)
/* Patch when dynamic mechanism is not ready */
//#define CONFIG_DM_PATCH
#endif

//#define CONFIG_DEBUG
//#define CONFIG_DEBUG_RTL871X
#if (CONFIG_PLATFORM_AMEBA_X == 1)
	#define CONFIG_MEM_MONITOR	MEM_MONITOR_SIMPLE
	#define WLAN_INTF_DBG		0 
	//#define CONFIG_DEBUG_DYNAMIC
	//#define DBG_TX 1
	//#define DBG_XMIT_BUF 1
	//#define DBG_XMIT_BUF_EXT 1
	#define DBG_TX_DROP_FRAME
#else
	#define CONFIG_MEM_MONITOR	MEM_MONITOR_SIMPLE
	//#define CONFIG_TRACE_SKB
	//#define WLAN_INTF_DBG
#endif // CONFIG_PLATFORM_AMEBA_X

//#define CONFIG_DONT_CARE_TP
//#define CONFIG_HIGH_TP
//#define CONFIG_MEMORY_ACCESS_ALIGNED

#define CONFIG_POWER_SAVING
#ifdef CONFIG_POWER_SAVING
	#define CONFIG_IPS
	#define CONFIG_LPS
	#ifdef CONFIG_LPS
		#define CONFIG_LPS_CHK_BY_TP
	#endif
	//#define CONFIG_LPS_LCLK
#if (CONFIG_PLATFORM_AMEBA_X == 0)
#ifdef CONFIG_LPS_LCLK
	#define CONFIG_DETECT_CPWM_BY_POLLING
	#define LPS_RPWM_WAIT_MS 300
#endif
#else
	#define CONFIG_LPS_32K
	#define TDMA_POWER_SAVING
#endif
	#define CONFIG_WAIT_PS_ACK
	#define CONFIG_FW_PSTIMEOUT
#endif

#define BAD_MIC_COUNTERMEASURE 1
#define DEFRAGMENTATION 1

#define WIFI_LOGO_CERTIFICATION 0
#if WIFI_LOGO_CERTIFICATION
    #define RX_AGGREGATION 1
	#define RX_AMSDU 1
#else
	#ifdef CONFIG_HIGH_TP_TEST
    		#define RX_AGGREGATION 1
	#else
		#define RX_AGGREGATION 1
	#endif
	#define RX_AMSDU 0
#endif

#if defined(CONFIG_PLATFORM_8711B)
	#define CONFIG_FW_C2H_PKT
#endif

#if (CONFIG_PLATFORM_AMEBA_X == 1)
	#if defined(CONFIG_PLATFORM_8195A)
		#define CONFIG_USE_TCM_HEAP 1					/* USE TCM HEAP */
	#endif
	#define CONFIG_RECV_TASKLET_THREAD
	#define CONFIG_XMIT_TASKLET_THREAD
#else
	#define CONFIG_XMIT_THREAD_MODE
#endif // CONFIG_PLATFORM_AMEBA_X
//#define CONFIG_RECV_THREAD_MODE				/* Wlan IRQ Polling  Mode*/
//#define CONFIG_ISR_THREAD_MODE_POLLING		/* Wlan IRQ Polling  Mode*/

//1 Chris
#ifndef CONFIG_SDIO_HCI
#define CONFIG_ISR_THREAD_MODE_INTERRUPT	/* Wlan IRQ Interrupt  Mode*/
#endif

#if defined(CONFIG_ISR_THREAD_MODE_POLLING) && defined(CONFIG_ISR_THREAD_MODE_INTERRUPT)
#error "CONFIG_ISR_THREAD_MODE_POLLING and CONFIG_ISR_THREAD_MODE_INTERRUPT are mutually exclusive. "
#endif

//#define CONFIG_RECV_TASK_THREAD_MODE

#if (CONFIG_PLATFORM_AMEBA_X == 1)
/* CRC DMEM optimized mode consume 1k less SRM memory consumption */
#define CRC_IMPLEMENTATION_MODE CRC_IMPLEMENTATION_DMEM_OPTIMIZED
#endif

/* AES DMEM optimized mode comsume 10k less memory compare to 
     IMEM optimized mode AES_IMPLEMENTATION_IMEM_OPTIMIZED */
#define AES_IMPLEMENTATION_MODE AES_IMPLEMENTATION_DMEM_OPTIMIZED

#define USE_SKB_AS_XMITBUF	1
#if (CONFIG_PLATFORM_AMEBA_X == 1)
#define USE_XMIT_EXTBUFF	1
#else
#define USE_XMIT_EXTBUFF	0
#endif
#define USE_MUTEX_FOR_SPINLOCK	1

// remove function to reduce code
#define NOT_SUPPORT_5G
#define SUPPORT_5G_CHANNEL	0
#if !defined(CONFIG_HARDWARE_8192E)
#define NOT_SUPPORT_RF_MULTIPATH
#endif
#define NOT_SUPPORT_VHT
//#define NOT_SUPPORT_40M
#define NOT_SUPPORT_80M
#if defined(CONFIG_PLATFORM_8195A)
#define NOT_SUPPORT_BBSWING
#endif
#ifdef CONFIG_HIGH_TP_TEST
	#undef NOT_SUPPORT_40M
#endif
#define NOT_SUPPORT_OLD_CHANNEL_PLAN
#define NOT_SUPPORT_BT

#define CONFIG_WIFI_SPEC	0
#define CONFIG_FAKE_EFUSE	0
#if CONFIG_FAKE_EFUSE
	#define FAKE_CHIPID		CHIPID_8710BN
#endif

#define CONFIG_AUTO_RECONNECT 1
#define ENABLE_HWPDN_PIN
#define SUPPORT_SCAN_BUF	1
#if (CONFIG_PLATFORM_AMEBA_X == 0)
#define BE_I_CUT			1
#endif

/* For WPA2 */
#define CONFIG_INCLUDE_WPA_PSK
#ifdef CONFIG_INCLUDE_WPA_PSK
#define CONFIG_MULTIPLE_WPA_STA
#define PSK_SUPPORT_TKIP	1
#endif

//#define CONFIG_PMKSA_CACHING

/* For WPA3 */
#define CONFIG_IEEE80211W
#define CONFIG_SAE_SUPPORT
#ifdef CONFIG_SAE_SUPPORT
#define CONFIG_SAE_DH_SUPPORT 1
#define ALL_DH_GROUPS
#endif

/* For promiscuous mode */
#define CONFIG_PROMISC

#define PROMISC_DENY_PAIRWISE	0

/* For Simple Link */
#ifndef CONFIG_INCLUDE_SIMPLE_CONFIG
//#define CONFIG_INCLUDE_SIMPLE_CONFIG 1
#endif

// for probe request with custom vendor specific IE
#define CONFIG_CUSTOM_IE

#if (CONFIG_PLATFORM_AMEBA_X == 0)
/* For multicast */
#define CONFIG_MULTICAST
#endif

/* For STA+AP Concurrent MODE */
#define CONFIG_CONCURRENT_MODE
#ifdef CONFIG_CONCURRENT_MODE
//#define CONFIG_MCC_MODE
  #if defined(CONFIG_PLATFORM_8195A) || defined(CONFIG_PLATFORM_8195BHP) || defined(CONFIG_PLATFORM_8710C)
    #define CONFIG_RUNTIME_PORT_SWITCH
  #endif
  #ifdef CONFIG_BRIDGE
    #define NET_IF_NUM ((CONFIG_ETHERNET) + (CONFIG_BRIDGE) + (CONFIG_WLAN) + 1)
  #else
    #define NET_IF_NUM ((CONFIG_ETHERNET) + (CONFIG_WLAN) + 1)
  #endif
#else
  #define NET_IF_NUM ((CONFIG_ETHERNET) + (CONFIG_WLAN))
#endif


/****************** For EAP auth configurations *******************/
#define CONFIG_TLS	1
#define CONFIG_PEAP	1
#define CONFIG_TTLS	1

// DO NOT change the below config of EAP
#ifdef PRE_CONFIG_EAP
#undef CONFIG_TLS
#define CONFIG_TLS	1
#undef CONFIG_PEAP
#define CONFIG_PEAP	1
#undef CONFIG_TTLS
#define CONFIG_TTLS	1
#endif

// enable 1X code in lib_wlan as default (increase 380 bytes)
#define CONFIG_EAP

#if CONFIG_TLS || CONFIG_PEAP || CONFIG_TTLS
#define EAP_REMOVE_UNUSED_CODE 1
#endif	     
	     
#define EAP_SSL_VERIFY_SERVER

#if CONFIG_TLS
#define EAP_SSL_VERIFY_CLIENT
#endif

#if CONFIG_TTLS
#define EAP_MSCHAPv2
#define EAP_TTLS_MSCHAPv2
//#define EAP_TTLS_EAP
//#define EAP_TTLS_MSCHAP
//#define EAP_TTLS_PAP
//#define EAP_TTLS_CHAP
#endif
/****************** End of EAP configurations *******************/

/* For WPS and P2P */
#define CONFIG_WPS
#if 0
#define CONFIG_WPS_AP
#define CONFIG_P2P_NEW
#if (!defined(SUPPORT_SCAN_BUF)||!defined(CONFIG_WPS_AP)) && defined(CONFIG_P2P_NEW)
#error "If CONFIG_P2P_NEW, need to SUPPORT_SCAN_BUF"
#endif
#endif

#define CONFIG_NEW_SIGNAL_STAT_PROCESS
#define CONFIG_SKIP_SIGNAL_SCALE_MAPPING

/* For AP_MODE */
#define CONFIG_AP_MODE
extern unsigned char g_user_ap_sta_num;
#define USER_AP_STA_NUM g_user_ap_sta_num
#if (CONFIG_PLATFORM_AMEBA_X == 1)
#if defined(CONFIG_PLATFORM_8721D)
#define AP_STA_NUM	5
#else
#define AP_STA_NUM	3	//2014/10/27 modify to 3
#endif
#define USE_DEDICATED_BCN_TX	0
#if USE_DEDICATED_BCN_TX
#error "WLAN driver for Ameba should not enable USE_DEDICATED_BCN_TX"
#endif
#else
extern unsigned int g_ap_sta_num;
#define AP_STA_NUM 3//g_ap_sta_num
#endif
#ifdef CONFIG_AP_MODE
#if defined(CONFIG_PLATFORM_8195A)  
	 //softap sent qos null0 polling client alive or not
	#define CONFIG_AP_POLLING_CLIENT_ALIVE 
#endif
	#define CONFIG_NATIVEAP_MLME
#if (CONFIG_PLATFORM_AMEBA_X == 1)
	#define CONFIG_INTERRUPT_BASED_TXBCN
#endif
	#ifdef CONFIG_INTERRUPT_BASED_TXBCN
		//#define CONFIG_INTERRUPT_BASED_TXBCN_EARLY_INT
		#define CONFIG_INTERRUPT_BASED_TXBCN_BCN_OK_ERR
	#endif
//	#define CONFIG_GK_REKEY
#if (CONFIG_PLATFORM_AMEBA_X == 0)
	#define USE_DEDICATED_BCN_TX	1
#endif
#if CONFIG_INIC_EN
//	#define REPORT_STA_EVENT //useless
#endif
#else
#if (CONFIG_PLATFORM_AMEBA_X == 0)
	#define USE_DEDICATED_BCN_TX	0
#endif
#endif

#if defined(CONFIG_AP_MODE) && defined(CONFIG_GK_REKEY) && !defined(CONFIG_MULTIPLE_WPA_STA)
#error "If CONFIG_GK_REKEY when CONFIG_AP_MODE, need to CONFIG_MULTIPLE_WPA_STA"
#endif

#if (CONFIG_PLATFORM_AMEBA_X == 0)
#if !defined(CONFIG_AP_MODE) && defined(CONFIG_CONCURRENT_MODE)
#error "If CONFIG_CONCURRENT_MODEE, need to CONFIG_AP_MODE"
#endif
#endif

/* For efuse or flash config */
#if (CONFIG_PLATFORM_AMEBA_X == 1)
	#define CONFIG_RW_PHYSICAL_EFUSE			0	// Mask efuse user blocks
	#define CONFIG_HIDE_PROTECT_EFUSE			1
	#define CONFIG_ADAPTOR_INFO_CACHING_FLASH	1
	#define CHECK_FLASH_VALID_MASK				1
	#define CHECK_EFUSE_VALID_MASK				1
	/* For K-free */
	#define CONFIG_RF_GAIN_OFFSET
#endif	// CONFIG_PLATFORM_AMEBA_X

/* For MP_MODE */
//#define CONFIG_MP_INCLUDED
#ifdef CONFIG_MP_INCLUDED
	#define MP_DRIVER		1
	#define CONFIG_MP_IWPRIV_SUPPORT
//	#define HAL_EFUSE_MEMORY
	#if (CONFIG_PLATFORM_AMEBA_X == 1)
		#define MP_REG_TEST
	#endif
#else
	#define MP_DRIVER		0
	#if defined(CONFIG_PLATFORM_8195A)
		//Control wifi mcu function
		#define CONFIG_LITTLE_WIFI_MCU_FUNCTION_THREAD
		#define CONFIG_ODM_REFRESH_RAMASK
		#define CONFIG_ANTENNA_DIVERSITY
		//#define CONFIG_ANTENNA_DIVERSITY_FORCE_ON
		//#define CONFIG_BT_COEXIST
	#endif
	#if defined(CONFIG_PLATFORM_8721D)
		#define CONFIG_ANTENNA_DIVERSITY
		//#define CONFIG_BT_COEXIST
		//#define CONFIG_SW_MAILBOX_EN
		//#define NEW_BT_COEX
		#define CONFIG_BT_COEXIST_SOC
	#endif
	#if defined(CONFIG_PLATFORM_8710C)
		//#define CONFIG_ANTENNA_DIVERSITY
		//#define CONFIG_BT_COEXIST
		//#define CONFIG_SW_MAILBOX_EN
		//#define NEW_BT_COEX
	#endif
#endif // #ifdef CONFIG_MP_INCLUDED

#ifdef CONFIG_BT_COEXIST
	#undef NOT_SUPPORT_BT
	#define CONFIG_BT_MAILBOX
	#define CONFIG_BT_EFUSE
	//#define CONFIG_BT_TWO_ANTENNA
#endif

// for Debug message
#define DBG 0
#if (CONFIG_PLATFORM_AMEBA_X == 1)
#if(DBG == 0)
	#define ROM_E_RTW_MSG 1
	#define ROM_F_RTW_MSG 1
#if (CONFIG_INIC_EN == 0) && (PHYDM_LINUX_CODING_STYLE == 0)
	/* For DM debug*/
	// BB
	#define DBG_RX_INFO 1
	#define DBG_DM_DIG 1			// DebugComponents: bit0
	#define DBG_DM_RA_MASK 1		// DebugComponents: bit1
	#define DBG_DM_ANT_DIV 1		// DebugComponents: bit6
	#define DBG_TX_RATE 1			// DebugComponents: bit9
	#define DBG_DM_RA 1				// DebugComponents: bit9
	#define DBG_DM_ADAPTIVITY 1		// DebugComponents: bit17
	// RF
	#define DBG_PWR_TRACKING 1		// DebugComponents: bit24
	#define DBG_RF_IQK 1			// DebugComponents: bit26
	#define DBG_RF_DPK 1			// DebugComponents: 
	// Common
	#define DBG_PWR_INDEX 1			// DebugComponents: bit30
#endif
#endif
#endif

#if (CONFIG_PLATFORM_AMEBA_X == 1)
	#if defined(CONFIG_PLATFORM_8195A)
		#undef CONFIG_RTL8195A
		#define CONFIG_RTL8195A
	#endif
	#if defined(CONFIG_PLATFORM_8711B)
		#ifndef CONFIG_RTL8711B 
			#define CONFIG_RTL8711B
		#endif
		#undef CONFIG_ADAPTOR_INFO_CACHING_FLASH
		#define CONFIG_ADAPTOR_INFO_CACHING_FLASH 0
		//#undef CONFIG_EAP
		//#undef CONFIG_IPS
		#define CONFIG_8710B_MOVE_TO_ROM
		#define CONFIG_EFUSE_SEPARATE
		#define CONFIG_MOVE_PSK_TO_ROM
		#define CONFIG_WOWLAN
		#define CONFIG_TRAFFIC_PROTECT
		#define CONFIG_FABVERSION_UMC 	1
		#if (CONFIG_INIC_EN == 1)
			#undef CONFIG_PROMISC
			#undef CONFIG_WPS
			#undef CONFIG_AP_MODE
			#undef CONFIG_NATIVEAP_MLME
			#undef CONFIG_INTERRUPT_BASED_TXBCN
			#undef CONFIG_INTERRUPT_BASED_TXBCN_BCN_OK_ERR
			#undef USE_DEDICATED_BCN_TX
			//#undef SUPPORT_SCAN_BUF
			#undef CONFIG_CONCURRENT_MODE
			#undef CONFIG_AUTO_RECONNECT
		#endif
	#endif
	#if defined(CONFIG_PLATFORM_8721D)
	#define CONFIG_EMPTY_EFUSE_PG_ENABLE
		#define CONFIG_APP_CTRL_RF_ONOFF
		#ifndef CONFIG_RTL8721D 
			#define CONFIG_RTL8721D
		#endif
		#undef NOT_SUPPORT_5G
		#undef CONFIG_ADAPTOR_INFO_CACHING_FLASH
		#define CONFIG_ADAPTOR_INFO_CACHING_FLASH 0
		#define CONFIG_EFUSE_SEPARATE
		#define CONFIG_WOWLAN
		//#define CONFIG_TRAFFIC_PROTECT
		#undef SUPPORT_5G_CHANNEL
		#define SUPPORT_5G_CHANNEL	1
		#define CONFIG_DFS
		#define CONFIG_XMIT_ACK
		#define TXBD_PRE_MALLOC
		#define USER_CTL_POWER_SAVE
		#define RTL8721D_SPECIFIC		//Patch for rtl8721d, avoid misunderstanding when porting to next new IC
		#ifdef CONFIG_DFS
			#define CONFIG_DFS_ACTION
		#endif
		#define DBG_DM_DIG 0			// DebugComponents: bit0
		//#define CONFIG_SUPPORT_DYNAMIC_TXPWR   //rtw_phydm_fill_desc_dpt todo
		#if (CONFIG_INIC_EN == 1)
			#undef CONFIG_PROMISC
			#undef CONFIG_WPS
			#undef CONFIG_AP_MODE
			#undef CONFIG_NATIVEAP_MLME
			#undef CONFIG_INTERRUPT_BASED_TXBCN
			#undef CONFIG_INTERRUPT_BASED_TXBCN_BCN_OK_ERR
			#undef USE_DEDICATED_BCN_TX
			//#undef SUPPORT_SCAN_BUF
			#undef CONFIG_CONCURRENT_MODE
			#undef CONFIG_AUTO_RECONNECT
		#endif
	//#define CONFIG_WLAN_SWITCH_MODE         //save memory while switching mode without driver re-init
	//#define LOW_POWER_WIFI_CONNECT
	//#define LONG_PERIOD_TICKLESS
	#endif
	#if defined(CONFIG_PLATFORM_8195BHP)
		#define CONFIG_RTL8195B
		#undef CONFIG_EAP
//		#undef CONFIG_ADAPTOR_INFO_CACHING_FLASH
//		#define CONFIG_ADAPTOR_INFO_CACHING_FLASH 0
		#undef CHECK_FLASH_VALID_MASK
		#define CHECK_FLASH_VALID_MASK				0
		#undef CHECK_EFUSE_VALID_MASK
		#define CHECK_EFUSE_VALID_MASK				0
		#undef CONFIG_RW_PHYSICAL_EFUSE
		#define CONFIG_RW_PHYSICAL_EFUSE			1	// efuse_get realraw
		#undef NOT_SUPPORT_5G
		#undef NOT_SUPPORT_VHT
//		#undef NOT_SUPPORT_40M
//		#undef NOT_SUPPORT_80M
		#undef DBG
		#define DBG 1
		#ifdef CONFIG_POWER_SAVING
			#define CONFIG_LPS_LCLK
			#ifdef CONFIG_LPS_LCLK
				#define CONFIG_DETECT_CPWM_BY_POLLING
				#define LPS_RPWM_WAIT_MS 300
			#endif
			#define CONFIG_LPS_PG
		#endif
		#define CONFIG_80211AC_VHT
		#undef CONFIG_IPS
//		#define CONFIG_NO_FW
		#define CONFIG_EX_FW_BIN
		#define CONFIG_WOWLAN
                #define CONFIG_WOWLAN_HW_CAM
		#define CONFIG_WOWLAN_CUSTOM_PATTERN
		#ifdef CONFIG_WOWLAN
			#define CONFIG_WOWLAN_TCP_KEEP_ALIVE
		#endif
		#define LOAD_FW_HEADER_FROM_DRIVER
		#define FW_IQK
//		#define RTW_IQK_FW_OFFLOAD
		#define CONFIG_PHY_CAPABILITY_QUERY
		#define CONFIG_ISR_THREAD_MODE_INTERRUPT	/* Wlan IRQ Interrupt  Mode*/
//		#define CONFIG_WLAN_RF_CNTL
		#define SUPPORT_5G_CHANNEL	1
		#define CONFIG_DFS
		#ifdef CONFIG_DFS
			#define CONFIG_DFS_ACTION
		#endif
			
		#define DBG_DM_DIG 0			// DebugComponents: bit0
//		#define CONFIG_DEBUG

		#define RTW_HALMAC		/* Use HALMAC architecture */
		#define RTW_HALMAC_MU_BF	0
		#define RTW_HALMAC_SU_BF	0
		#define RTW_HALMAC_BT_COEX	0
		#define RTW_HALMAC_DUMP_INFO  0
		#define RTW_HALMAC_TXBF		0
		#define RTW_HALMAC_FW_OFFLOAD  0
		#define RTW_HALMAC_PHYSICAL_EFUSE  0
		#define RTW_HALMAC_SIZE_OPTIMIZATION 1
		#define RTW_HALMAC_SDIO_CIA_READ 0
		#define RTW_HALMAC_LTE_COEX 0

		#define CONFIG_MAC_LOOPBACK_DRIVER_RTL8195B 0
	#endif
	#if defined(CONFIG_PLATFORM_8710C)
		#ifndef CONFIG_RTL8710C 
			#define CONFIG_RTL8710C
		#endif
		#undef CONFIG_EAP
		#undef CONFIG_ADAPTOR_INFO_CACHING_FLASH
		#undef CHECK_FLASH_VALID_MASK
		#undef NOT_SUPPORT_40M
		#define NOT_SUPPORT_40M
		#define CONFIG_ADAPTOR_INFO_CACHING_FLASH 0
		#undef CONFIG_RW_PHYSICAL_EFUSE
		#define CONFIG_RW_PHYSICAL_EFUSE		1// efuse_get realraw enable physical read/write
		#define LOAD_FW_HEADER_FROM_DRIVER
		//#define CONFIG_EFUSE_SEPARATE
		#define CONFIG_TRAFFIC_PROTECT	
		#ifdef CONFIG_POWER_SAVING
			#define CONFIG_WOWLAN
			//#define CONFIG_LPS_LCLK
			#ifdef CONFIG_LPS_LCLK
				//#define CONFIG_DETECT_CPWM_BY_POLLING
				#define LPS_RPWM_WAIT_MS 300
			#endif
			//#define CONFIG_LPS_PG
			//#define CONFIG_LPS_POFF
			#ifdef CONFIG_WOWLAN
				//#define CONFIG_WOWLAN_TCP_KEEP_ALIVE
			#endif
		#endif
		#define DBG_DM_DIG 0			// DebugComponents: bit0
//		#define CONFIG_DEBUG

		//#define RTW_HALMAC		/* Use HALMAC architecture */
		//#define RTW_HALMAC_MU_BF	0
		//#define RTW_HALMAC_SU_BF	0
		//#define RTW_HALMAC_BT_COEX	0
		//#define RTW_HALMAC_DUMP_INFO  0
		//#define RTW_HALMAC_TXBF		0
		//#define RTW_HALMAC_FW_OFFLOAD  0
		//#define RTW_HALMAC_PHYSICAL_EFUSE  0
		//#define RTW_HALMAC_SIZE_OPTIMIZATION 1
		//#define RTW_HALMAC_SDIO_CIA_READ 0
		//#define RTW_HALMAC_LTE_COEX 0

		//#define CONFIG_MAC_LOOPBACK_DRIVER_RTL8710C 1 // 1: HAL+MAC LOOPBACK, 2: HAL+MAC+BB LOOPBACK 3: DRV+HAL+MAC LOOPBACK
		#if defined(CONFIG_MAC_LOOPBACK_DRIVER_RTL8710C) && (CONFIG_MAC_LOOPBACK_DRIVER_RTL8710C == 3)
		#define CONFIG_MAC_LOOPBACK_DRIVER_AMEBA
		#endif
		#define CONFIG_WLAN_SWITCH_MODE         //save memory while switching mode without driver re-init
	#endif
#elif defined(CONFIG_HARDWARE_8188F)
#define CONFIG_RTL8188F
#elif defined(CONFIG_HARDWARE_8192E)
#define CONFIG_RTL8192E
#elif defined(CONFIG_HARDWARE_8821C)
#define CONFIG_RTL8821C
#elif defined(CONFIG_HARDWARE_8723D)
#define CONFIG_RTL8723D
#elif defined(CONFIG_HARDWARE_8188E)
#define CONFIG_RTL8188E
#else
#define CONFIG_RTL8188E
#endif

#define RTL8192E_SUPPORT 0
#define RTL8812A_SUPPORT 0
#define RTL8821A_SUPPORT 0
#define RTL8723B_SUPPORT 0
#define RTL8195A_SUPPORT 0
#define RTL8188E_SUPPORT 0
#define RTL8188F_SUPPORT 0
#define RTL8711B_SUPPORT 0
#define RTL8721D_SUPPORT 0
#define RTL8821C_SUPPORT 0
#define RTL8723D_SUPPORT 0
#define RTL8195B_SUPPORT 0
#define RTL8710C_SUPPORT 0
#if defined(CONFIG_PLATFORM_8195A)
#undef RTL8195A_SUPPORT
#define RTL8195A_SUPPORT 1
#elif defined(CONFIG_PLATFORM_8711B)
#undef RTL8711B_SUPPORT
#define RTL8711B_SUPPORT 1
#elif defined(CONFIG_PLATFORM_8721D)
#undef RTL8721D_SUPPORT
#define RTL8721D_SUPPORT 1
#elif defined(CONFIG_PLATFORM_8195BHP)
#undef RTL8195B_SUPPORT
#define RTL8195B_SUPPORT 1
#elif defined(CONFIG_PLATFORM_8710C)
#undef RTL8710C_SUPPORT
#define RTL8710C_SUPPORT 1
#elif defined(CONFIG_HARDWARE_8188F)
#undef	RTL8188F_SUPPORT
#define RTL8188F_SUPPORT 1
#elif defined(CONFIG_HARDWARE_8192E)
#undef RTL8192E_SUPPORT
#define RTL8192E_SUPPORT 1
#elif defined(CONFIG_HARDWARE_8821C)
#undef	RTL8821C_SUPPORT
#define RTL8821C_SUPPORT 1
#elif defined(CONFIG_HARDWARE_8723D)
#undef	RTL8723D_SUPPORT
#define RTL8723D_SUPPORT 1
#elif defined(CONFIG_HARDWARE_8188E)
#undef RTL8188E_SUPPORT
#define RTL8188E_SUPPORT 1
#else
#undef RTL8188E_SUPPORT
#define RTL8188E_SUPPORT 1
#endif

/* For DM support */
#if defined(CONFIG_RTL8188F) 
#define RATE_ADAPTIVE_SUPPORT 0
#elif defined(CONFIG_RTL8821C)
#define RATE_ADAPTIVE_SUPPORT 0
#elif defined(CONFIG_RTL8192E)
#define RATE_ADAPTIVE_SUPPORT 0
#elif defined(CONFIG_RTL8723D)
#define RATE_ADAPTIVE_SUPPORT 0
#elif defined(CONFIG_PLATFORM_8711B)
#define RATE_ADAPTIVE_SUPPORT 0
#define CONFIG_ODM_REFRESH_RAMASK
#elif defined(CONFIG_PLATFORM_8721D)
#define RATE_ADAPTIVE_SUPPORT 0
//#define CONFIG_ODM_REFRESH_RAMASK
#elif defined(CONFIG_PLATFORM_8710C)
#define RATE_ADAPTIVE_SUPPORT 0
//#define CONFIG_ODM_REFRESH_RAMASK
#else
#define RATE_ADAPTIVE_SUPPORT 1
#endif
// adaptivity
#define RTW_ADAPTIVITY_EN_DISABLE			0
#define RTW_ADAPTIVITY_EN_ENABLE			1
#define CONFIG_RTW_ADAPTIVITY_EN			RTW_ADAPTIVITY_EN_DISABLE
#define RTW_ADAPTIVITY_MODE_NORMAL			0
#define RTW_ADAPTIVITY_MODE_CARRIER_SENSE	1
#define CONFIG_RTW_ADAPTIVITY_MODE			RTW_ADAPTIVITY_MODE_CARRIER_SENSE
#define CONFIG_RTW_ADAPTIVITY_DML			0


#if (CONFIG_PLATFORM_AMEBA_X == 1)
	#define CONFIG_POWER_TRAINING_WIL 0 // in RA
#else
	#define POWER_BY_RATE_SUPPORT 0
#endif

#if (CONFIG_PLATFORM_AMEBA_X == 1)
#define RTL8195A_FOR_TEST_CHIP 0

//#define CONFIG_WIFI_TEST 1
//#define CONFIG_MAC_LOOPBACK_DRIVER 1
//#define CONFIG_WLAN_HAL_TEST 1
//#define SKB_PRE_ALLOCATE_TX 1
#ifdef CONFIG_HIGH_TP_TEST
	#define SKB_PRE_ALLOCATE_RX	1
#else
	#define SKB_PRE_ALLOCATE_RX	0
#endif

#if (!defined(CONFIG_PLATFORM_8721D))
	#define TX_CHECK_DSEC_ALWAYS 1
#endif

#define CONFIG_DBG_DISABLE_RDU_INTERRUPT
//#define CONFIG_WLAN_HAL_RX_TASK
#if (SKB_PRE_ALLOCATE_RX == 1)
	#ifdef CONFIG_HIGH_TP_TEST
		#define EXCHANGE_LXBUS_RX_SKB 1
	#else
		#define EXCHANGE_LXBUS_RX_SKB 0
	#endif
#endif
#if (defined(CONFIG_FPGA) && !defined(CONFIG_PLATFORM_8710C))\
	|| (defined(CONFIG_PLATFORM_8710C) && defined(CONFIG_MAC_LOOPBACK_DRIVER_RTL8710C) && (CONFIG_MAC_LOOPBACK_DRIVER_RTL8710C == 1))
	//Enable mac loopback for test mode (Ameba)
	#ifdef CONFIG_WIFI_NORMAL
		#define CONFIG_TWO_MAC_DRIVER // for test mode
	#else //CONFIG_WIFI_VERIFY
		#define ENABLE_MAC_LB_FOR_TEST_MODE
	#endif

	#define AP_PSK_SUPPORT_TKIP
#endif

#ifdef ENABLE_MAC_LB_FOR_TEST_MODE
	#define CONFIG_SUDO_PHY_SETTING
	#define INT_HANDLE_IN_ISR 1
#ifdef CONFIG_LWIP_LAYER
        #undef CONFIG_LWIP_LAYER
	#define CONFIG_LWIP_LAYER 0
#else
        #define CONFIG_LWIP_LAYER 0
#endif
	#define CONFIG_WLAN_HAL_TEST
	#define CONFIG_WLAN_HAL_RX_TASK
	#define CONFIG_MAC_LOOPBACK_DRIVER_AMEBA 1
	#define HAL_MAC_ENABLE 1
	#if !defined(CONFIG_PLATFORM_8710C)
	#define CONFIG_TWO_MAC_TEST_MODE 
	#endif
	#if defined(CONFIG_MAC_LOOPBACK_DRIVER_RTL8710C) && (CONFIG_MAC_LOOPBACK_DRIVER_RTL8710C == 2)
	// Enable BB loopback test
	#define HAL_BB_ENABLE 1
	#define HAL_RF_ENABLE 1
	#define DISABLE_BB_RF 0
	#else
	#define DISABLE_BB_RF 1
	#endif
#else
	//#define CONFIG_TWO_MAC_DRIVER //for mornal driver; two mac
	#if defined(CONFIG_TWO_MAC_DRIVER) || defined(CONFIG_MAC_LOOPBACK_DRIVER_AMEBA)
		#define CONFIG_SUDO_PHY_SETTING
		#define HAL_MAC_ENABLE 1
		#define DISABLE_BB_RF 1
	#else
		#define HAL_MAC_ENABLE 1
		#define HAL_BB_ENABLE 1
		#define HAL_RF_ENABLE 1
		#define DISABLE_BB_RF 0
	#endif
	//#define INT_HANDLE_IN_ISR 1
#endif
#endif // CONFIG_PLATFORM_AMEBA_X

#ifndef CONFIG_LWIP_LAYER
#define CONFIG_LWIP_LAYER 1
#endif
#define CONFIG_MAC_ADDRESS 0
//fast reconnection
//#define CONFIG_FAST_RECONNECTION 1
#if defined(CONFIG_INIC_EN)&&(CONFIG_INIC_EN==1)
#define CONFIG_RECV_REORDERING_CTRL //enable reordering for iNIC high throughput
#undef RX_AGGREGATION
#define RX_AGGREGATION 1
#undef NOT_SUPPORT_40M
#undef CONFIG_CONCURRENT_MODE
#endif

#if defined(CONFIG_HARDWARE_8821C)
#define FW_IQK
#define RTW_HALMAC
#define LOAD_FW_HEADER_FROM_DRIVER
#define RTW_HALMAC_SIZE_OPTIMIZATION 1
//#define CONFIG_NO_FW
#ifdef NOT_SUPPORT_5G
#undef NOT_SUPPORT_5G
#define SUPPORT_5G_CHANNEL	1
#endif
#endif

//#define CONFIG_ADDRESS_ALIGNMENT
#ifdef CONFIG_ADDRESS_ALIGNMENT
#define ALIGNMENT_SIZE 32
#endif

#define CONFIG_DFS
//#define CONFIG_EMPTY_EFUSE_PG_ENABLE

#define WLAN_WRAPPER_VERSION 1

#define TIME_THRES	20

/* 80211 - K V R */
#define CONFIG_IEEE80211K
#define CONFIG_LAYER2_ROAMING
#ifdef CONFIG_LAYER2_ROAMING
    #define CONFIG_RTW_WNM
    #define CONFIG_IEEE80211R
#endif

#endif //WLANCONFIG_H
