/**
 * @file    wm_netif.h
 *
 * @brief   ETHERNET INIT Interface
 *
 * @author  winnermicro
 *
 * Copyright (c) 2015 Winner Microelectronics Co., Ltd.
 */
#ifndef WM_NETIF_H
#define WM_NETIF_H
#include "wm_config.h"
#if TLS_CONFIG_LWIP_VER2_0_3
#include "wm_netif2.0.3.h"
#else
#include "wm_wifi.h"
#include "wm_type_def.h"
#include "wm_params.h"

/** MACRO for callback EVENT to join AP or create soft-AP successfully  */
#define	NETIF_WIFI_JOIN_SUCCESS         0x1
/** MACRO for callback EVENT to fail to join AP */
#define NETIF_WIFI_JOIN_FAILED          0x2
/** MACRO for callback EVENT to disconnect from AP or destroy soft-AP */
#define	NETIF_WIFI_DISCONNECTED         0x3
/** MACRO for callbck EVENT to get IP address */
#define	NETIF_IP_NET_UP                 0x4
/** MACRO for callback EVNET to join AP successfully for APSTA */
#define	NETIF_WIFI_SOFTAP_SUCCESS    0x5
/** MACRO for callback EVNET to Create soft-AP successfully for APSTA */
#define	NETIF_WIFI_SOFTAP_FAILED     0x6
/** MACRO for callback EVNET to get IP successfully for APSTA's STA */
#define	NETIF_WIFI_SOFTAP_CLOSED          0x7
/** MACRO for callback EVNET to inform soft ap's net */
#define	NETIF_IP_NET2_UP                0x8
/** Structure for IP address */
struct ip_addr
{
  u32 addr;
};

/** Structure to save net info */
struct tls_ethif
{
    struct ip_addr ip_addr;    	/**< IP Address */
    struct ip_addr netmask;    	/**< NETMASK */
    struct ip_addr gw;    		/**< gateway */
    struct ip_addr dns1;    	/**< first DNS */
    struct ip_addr dns2;    	/**< second DNS */
    u8 status; 					/**< Net Status 0:net down,1:net up */
};

/** Type definition of struct ip_addr */
typedef struct ip_addr ip_addr_t;

/** Type definition of netif status changed callback */
typedef void (*tls_netif_status_event_fn)(u8 status);

/**
 * @defgroup APP_APIs APP APIs
 * @brief APP APIs
 */

/**
 * @addtogroup APP_APIs
 * @{
 */

/**
 * @defgroup NETIF_APIs NETIF APIs
 * @brief NET interface APIs
 */

/**
 * @addtogroup NETIF_APIs
 * @{
 */

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
struct tls_ethif *tls_netif_get_ethif(void);

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
s8 tls_dhcp_start(void);

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
s8 tls_dhcp_stop(void);

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
s8 tls_netif_set_addr(ip_addr_t *ipaddr, ip_addr_t *netmask, ip_addr_t *gw);

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
s8 tls_netif_set_up(void);

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
s8 tls_netif_set_down(void);

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
s8 tls_netif_add_status_event(tls_netif_status_event_fn event_fn);

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
s8 tls_netif_remove_status_event(tls_netif_status_event_fn event_fn);

/**
 * @brief          This function is used to get default net device interface

 *
 * @param[in]      None
 *
 * @retval         netif *     pointer to net device interface
 *
 * @note           None
 */
struct netif *tls_get_netif(void);

/**
 * @brief          This function is used to get softap's net device interface

 *
 * @param[in]      None
 *
 * @retval         netif *     pointer to net device interface
 *
 * @note           None
 */
struct tls_ethif * tls_netif_get_ethif2(void);


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
s8 tls_netif2_set_up(void);

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
s8 tls_netif2_set_down(void);

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
s8 tls_netif2_set_addr(ip_addr_t *ipaddr, ip_addr_t *netmask, ip_addr_t *gw);

/**
 * @}
 */

/**
 * @}
 */
#endif
#endif /* WM_NETIF_H */

