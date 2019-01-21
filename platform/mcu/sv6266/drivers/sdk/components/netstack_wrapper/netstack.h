/*
*  (C) Copyright 2014-2016 Shenzhen South Silicon Valley microelectronics co.,limited
*
*  All Rights Reserved
*/

#ifndef _NETSTACK_H_
#define _NETSTACK_H_
#include <soc_types.h>
#include <attrs.h>
#include <wificonf.h>
#ifdef TCPIPSTACK_EN
#include "lwip/tcpip.h"
#include "lwip/dns.h"
#include "lwip/netifapi.h"
#endif
#include "wifinetstack.h"

struct netdev
{
    char name[6];
    u16 mtu;
    u32 ipaddr;
    u32 netmask;
    u32 gw;
    u32 flags;    
    unsigned char hwmac[6];        
};

#define NS_OK           0   //Everything is fine
#define NS_ERR_MEM      -1  //Out of memory
#define NS_ERR_ARG      -2  //Invalid arguement
#define NS_ERR_IMP      -3  //Not implement yet
#define NS_ERR_CALLER  -4  //Not define error, need to check

// Transfer L2 packet to netstack
int netstack_input(char *ifname, void *data, u32 len) ATTRIBUTE_SECTION_FAST;

// Transfer netstack packet to L2
int netstack_output(char *name, void *data, u32 len) ATTRIBUTE_SECTION_FAST;

typedef struct{
    char hostname_if0[32]; //FOR IF0
    char hostname_if1[32]; //FOR IF1
}NETSTACK_CONFIG;

/** 
 *@ init netstack
 *@ config: input parameter
    config=NULL, use the default configuration
    config!=NULL, only can setup the host name now
*/
int netstack_init(NETSTACK_CONFIG *config);

#ifdef TCPIPSTACK_EN
//get dns server
int netstack_get_ipv4dnsaddr(u32 *dnsserver, u32 numdnssvr);
//set dns server
int netstack_set_ipv4dnsaddr(const u32 *dnsserver, u32 numdnssvr);

//Add device with specific setting
int netdev_init(struct netdev * pdev, bool dft_dev, bool init_up);

//Add device with specific setting 
int netdev_init_ex(struct netdev *pdev, bool dft_dev, bool init_up, char *hostName);

//get hw mac
int netdev_getmacaddr(const char *ifname, u8 *macaddr);
//get ipinfo
int netdev_getipv4info(const char *ifname, u32 *ip, u32 *gw, u32 *netmask);
//set ipinfo
int netdev_setipv4info(const char *ifname, u32 ip, u32 gw, u32 netmask);

//get interface status
int netdev_getifstatus(const char *ifname, u32 *flags);

//set interface up
int netdev_ifup(const char *ifname);
//set interface down
int netdev_ifdown(const char *ifname);
//interface link up cb
void netdev_link_up_cb(void *ifname);
//interface link down cb
void netdev_link_down_cb(void *ifname);
//get all netdev
u32 netdev_getallnetdev(struct netdev * pdev, u32 num);


//set dhcp client on dev
int dhcpc_wrapper_set(const char *ifname, const bool enable);

// UDP operation


//ARP operation
void netstack_arp_table_showall();

//DHCP SERVER operation
#if 0
int netstack_dhcpd_init(void *arg);
int netstack_dhcpd_exit(void);
int netstack_dhcps_info_set(void *if_dhcps, void *des_if_dhcps);
int netstack_dhcpd_start(void);
int netstack_dhcpd_stop(void);
#endif
void netstack_dhcpd_lease_showall();
#endif

#endif //#ifndef _NETSTACK_H_
