/******************************************************************************
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved. 
  *
******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __NETCONF_H
#define __NETCONF_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "lwip/tcpip.h"
/* Includes ------------------------------------------------------------------*/
#include <platform/platform_stdlib.h>
#include "platform_opts.h"
#include "autoconf.h"

// macros
/* Give default value if not defined */
#ifndef NET_IF_NUM
  #ifdef CONFIG_CONCURRENT_MODE
    #define NET_IF_NUM ((CONFIG_ETHERNET) + (CONFIG_WLAN) + 1)
  #else
    #define NET_IF_NUM ((CONFIG_ETHERNET) + (CONFIG_WLAN))
  #endif  // end of CONFIG_CONCURRENT_MODE
#endif  // end of NET_IF_NUM

/* Private typedef -----------------------------------------------------------*/
typedef enum 
{ 
	DHCP_START=0,
	DHCP_WAIT_ADDRESS,
	DHCP_ADDRESS_ASSIGNED,
	DHCP_RELEASE_IP,
	DHCP_STOP,
	DHCP_TIMEOUT
} DHCP_State_TypeDef;

/* Extern functions ------------------------------------------------------------*/
void wifi_rx_beacon_hdl( char* buf, int buf_len, int flags, void* userdata);


/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void LwIP_Init(void);
uint8_t LwIP_DHCP(uint8_t idx, uint8_t dhcp_state);
unsigned char* LwIP_GetMAC(struct netif *pnetif);
unsigned char* LwIP_GetIP(struct netif *pnetif);
unsigned char* LwIP_GetGW(struct netif *pnetif);
uint8_t* LwIP_GetMASK(struct netif *pnetif);
uint8_t* LwIP_GetBC(struct netif *pnetif);
#if LWIP_DNS
void LwIP_GetDNS(ip_addr_t* dns);
void LwIP_SetDNS(ip_addr_t* dns);
#endif
void LwIP_UseStaticIP(struct netif *pnetif);
#if LWIP_AUTOIP
void LwIP_AUTOIP(struct netif *pnetif);
#endif
#if LWIP_IPV6
void LwIP_AUTOIP_IPv6(struct netif *pnetif);
#endif
uint32_t LWIP_Get_Dynamic_Sleep_Interval();
#ifdef __cplusplus
}
#endif

#endif /* __NETCONF_H */


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
