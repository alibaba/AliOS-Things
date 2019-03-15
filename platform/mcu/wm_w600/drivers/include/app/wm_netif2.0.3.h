/**
 * @file    wm_netif2.0.3.h
 *
 * @brief   netif203 module
 *
 * @author  dave
 *
 * Copyright (c) 2014 Winner Microelectronics Co., Ltd.
 */
 
#ifndef WM_NETIF2_0_3_H
#define WM_NETIF2_0_3_H

#include "wm_config.h"
#include "wm_type_def.h"
#include "wm_sockets.h"
#include "wm_wifi.h"
#include "wm_params.h"

/** MACRO for callback EVENT to join AP or create soft-AP successfully  */
#define	NETIF_WIFI_JOIN_SUCCESS         0x1
/** MACRO for callback EVENT to fail to join AP */
#define  NETIF_WIFI_JOIN_FAILED          0x2
/** MACRO for callback EVENT to disconnect from AP or destroy soft-AP */
#define	NETIF_WIFI_DISCONNECTED         0x3
/** MACRO for callbck EVENT to get IP address */
#define	NETIF_IP_NET_UP                 0x4
/** MACRO for callback EVNET to create AP successfully */
#define	NETIF_WIFI_SOFTAP_SUCCESS    0x5
/** MACRO for callback EVNET to create soft-AP failed */
#define	NETIF_WIFI_SOFTAP_FAILED     0x6
/** MACRO for callback EVNET to close soft-AP */
#define	NETIF_WIFI_SOFTAP_CLOSED          0x7
/** MACRO for callback EVNET to inform soft ap's net */
#define	NETIF_IP_NET2_UP                0x8

#define NETIF_IPV6_NET_UP           0x9

/** These are the values for ip_addr_t.type */
#define IPADDR_TYPE_V4                0U
#define IPADDR_TYPE_V6                6U
#define IPADDR_TYPE_ANY               46U

#define IPV6_ADDR_MAX_NUM       3       

#if 0
struct ip_addr {
  u32_t addr;
};

typedef struct ip_addr ip_addr_t;	
#endif
#if 0
struct ip4_addr {
  u32_t addr;
};
typedef struct ip4_addr ip4_addr_t;

struct ip6_addr {
  u32_t addr[4];
};
typedef struct ip6_addr ip6_addr_t;

#if (TLS_CONFIG_IPV4 && TLS_CONFIG_IPV6)
typedef struct _ip_addr {
  union {
    ip6_addr_t ip6;
    ip4_addr_t ip4;
  } u_addr;
  u8_t type;
} ip_addr_t;
#else
#if TLS_CONFIG_IPV4
typedef ip4_addr_t ip_addr_t;
#else	  
typedef ip6_addr_t ip_addr_t;
#endif
#endif
#endif
struct tls_ethif {
    ip_addr_t ip_addr;
    ip_addr_t netmask;
    ip_addr_t gw;
#if TLS_CONFIG_IPV6
    ip_addr_t ip6_addr[IPV6_ADDR_MAX_NUM];
#endif
    ip_addr_t dns1;
    ip_addr_t dns2;
    u8 status;              //0:net down; 1:net up
#if TLS_CONFIG_IPV6
    u8 ipv6_status[IPV6_ADDR_MAX_NUM];      //0:net down; 1:net up
#endif
};

//type defination of netif status changed callback.
typedef void (*tls_netif_status_event_fn)(u8 status);

/**
 * @brief          This function is used to initialize TCP/IP Stack
 *
 * @param[in]      None
 *
 * @retval         0     success
 * @retval         other failed
 *
 * @note           None
 */
int tls_ethernet_init(void);

/**
 * @brief          This function is used to get IP parameters stored in
                   tls_ethif struct
 *
 * @param[in]      None
 *
 * @retval         tls_ethif *     Pointer to struct tls_ethif
 *
 * @note           None
 */
struct tls_ethif * tls_netif_get_ethif(void);

/**
 * @brief          This function is used to start DHCP Client
 *
 * @param[in]      None
 *
 * @retval         0     success
 * @retval         Minus failed
 *
 * @note           None
 */
err_t tls_dhcp_start(void);

/**
 * @brief          This function is used to stop DHCP client
 *
 * @param[in]      None
 *
 * @retval         0     success
 * @retval         Minus failed
 *
 * @note           None
 */
err_t tls_dhcp_stop(void);

/**
 * @brief          This function is used to change IP address configuration
 *
 * @param[in]      *ipaddr     IP address
 * @param[in]      *netmask    netmask
 * @param[in]      *gw         default gateway
 *
 * @retval         0     success
 * @retval         Minus failed
 *
 * @note           None
 */
err_t tls_netif_set_addr(ip_addr_t *ipaddr,
                        ip_addr_t *netmask,
                        ip_addr_t *gw);

/**
 * @brief          This function is used to set dns servers
 *
 * @param[in]      numdns     index of the DNS server to set
 							  must be < DNS_MAX_SERVERS
 * @param[in]      *dnsserver IP address of the DNS server to set
 *
 * @return         None
 *
 * @note           None
 */ 
void tls_netif_dns_setserver(u8 numdns, ip_addr_t *dnsserver);

/**
 * @brief          This function is used to bring up an interface,available
                   for processing traffic
 *
 * @param[in]      None
 *
 * @retval         0     success
 * @retval         Minus failed
 *
 * @note           None
 */
err_t tls_netif_set_up(void);

/**
 * @brief          This function is used to bring down an interface,disabling
 				   any traffic processing
 *
 * @param[in]      None
 *
 * @retval         0     success
 * @retval         Minus failed
 *
 * @note           None
 */ 
err_t tls_netif_set_down(void);

/**
 * @brief          This function is used to add netif status changed callback
                   to event list,if exists, do nothing
 *
 * @param[in]      event_fn    pointer to tls_netif_status_event_fn
 *
 * @retval         0     success
 * @retval         Minus failed
 *
 * @note           None
 */
err_t tls_netif_add_status_event(tls_netif_status_event_fn event_fn);

/**
 * @brief          This function is used to remove netif status changed
                   callback from event list,if not exists, do nothing
 *
 * @param[in]      event_fn    pointer to tls_netif_status_event_fn
 *
 * @retval         0     success
 * @retval         Minus failed
 *
 * @note           None
 */
err_t tls_netif_remove_status_event(tls_netif_status_event_fn event_fn);

struct netif *tls_get_netif(void);

#if TLS_CONFIG_AP
/**
 * @brief          Start DHCP Server for a network interface
 * *
 * @retval         DHCPS_ERR_SUCCESS - No error
 * @retval         DHCPS_ERR_MEM - Out of memory
 * @retval         DHCPS_ERR_LINKDOWN - The NI is inactive
 *
 * @note           None
 */
INT8S tls_dhcps_start(void);

/**
 * @brief          This function is used to stop DHCP Server
 *
 * @param[in]      None
 *
 * @retval         None
 *
 * @note           None
 */
void tls_dhcps_stop(void);

/**
 * @brief          Start the dns server's service
 * *
 * @retval         DHCPS_ERR_SUCCESS - No error
 * @retval         DHCPS_ERR_MEM - Out of memory
 * @retval         DHCPS_ERR_LINKDOWN - The NI is inactive
 * @retval		   DNSS_ERR_PARAM - Input parameter error
 *
 * @note           None
 */
INT8S tls_dnss_start(INT8U * DnsName);

/**
 * @brief          Stop the dns server's service
 *
 * @param[in]      None
 *
 * @retval         None
 *
 * @note           None
 */
void tls_dnss_stop(void);

/**
 * @brief          Get station's ip address by mac address
 *
 * @param[in]      mac    station's mac address
 *
 * @retval         ip_addr   station's ip address
 *
 * @note           None
 */
ip_addr_t *tls_dhcps_getip(const u8_t *mac);
#endif //TLS_CONFIG_AP

#if TLS_CONFIG_RMMS
/**
 * @brief          Start remote manager server.
 * *
 * @retval         DHCPS_ERR_SUCCESS - No error
 * @retval         DHCPS_ERR_MEM - Out of memory
 * @retval         DHCPS_ERR_LINKDOWN - The NIF is inactive
 *
 * @note           None
 */
INT8S tls_rmms_start(void);

/**
 * @brief          Disable remote manager server
 *
 * @param[in]      None
 *
 * @retval         None
 *
 * @note           None
 */
void tls_rmms_stop(void);
#endif

#if TLS_CONFIG_AP
struct tls_ethif * tls_netif_get_softap_ethif(void);
/**
 * @brief          This is used to bring up an interface for APSTA,available
                   for processing traffic
 *
 * @param[in]      None
 *
 * @retval         0     success
 * @retval         Minus failed
 *
 * @note           Only APSTA used
 */
err_t tls_netif2_set_up(void);

/**
 * @brief          This is used to bring down an interface for APSTA, disab-
                   ling any traffic processing
 *
 * @param[in]      None
 *
 * @retval         0     success
 * @retval         Minus failed
 *
 * @note           Only APSTA used
 */
err_t tls_netif2_set_down(void);

/**
 * @brief          This function is used to change IP configuration for
                   a network interface for APSTA
 *
 * @param[in]      *ipaddr     IP address
 * @param[in]      *netmask    netmask
 * @param[in]      *gw         default gateway
 *
 * @retval         0     success
 * @retval         Minus failed
 *
 * @note           Only APSTA used
 */
err_t tls_netif2_set_addr(ip_addr_t *ipaddr,
                          ip_addr_t *netmask,
                          ip_addr_t *gw);
/*************************************************************************** 
* Function: tls_dhcps_setdns
* Description: Set dhcp server's dns address.
* 
* Input:  numdns:     the index of the DNS server to set must be < DNS_MAX_SERVERS
* 
* Output: None
* 
* Return: None
* 
* Date : 2015-3-10
****************************************************************************/
/**
 * @brief          Set dhcp server's dns address
 *
 * @param[in]      numdns the index of the DNS server to set must be < DNS_MAX_SERVERS
 *
 * @retval         None
 *
 * @note           Only APSTA used
 */
void tls_dhcps_setdns(u8_t numdns);
#endif

#endif //WM_NETIF_H
