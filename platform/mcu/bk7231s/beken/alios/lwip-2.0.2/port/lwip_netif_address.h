#ifndef _LWIP_NETIF_ADDR_H_
#define _LWIP_NETIF_ADDR_H_

/** MLAN BSS type */
typedef enum _wifi_interface_type
{
    WIFI_INTERFACE_TYPE_STA = 0,
    WIFI_INTERFACE_TYPE_UAP = 1,

    WIFI_INTERFACE_TYPE_ANY = 0xff,
} wifi_interface_type;

#define ADDR_TYPE_STATIC 1
#define ADDR_TYPE_DHCP   0

/** This data structure represents an IPv4 address */
struct ipv4_config {
	/** DHCP_Disable DHCP_CLIENT DHCP_Server */
	unsigned addr_type;
	/** The system's IP address in network order. */
	unsigned address;
	/** The system's default gateway in network order. */
	unsigned gw;
	/** The system's subnet mask in network order. */
	unsigned netmask;
	/** The system's primary dns server in network order. */
	unsigned dns1;
	/** The system's secondary dns server in network order. */
	unsigned dns2;
};

/** Network IP configuration.
 *
 *  This data structure represents the network IP configuration
 *  for IPv4 as well as IPv6 addresses
 */
struct wlan_ip_config {
#ifdef CONFIG_IPV6
	/** The network IPv6 address configuration that should be
	 * associated with this interface. */
	struct ipv6_config ipv6[MAX_IPV6_ADDRESSES];
#endif
	/** The network IPv4 address configuration that should be
	 * associated with this interface. */
	struct ipv4_config ipv4;
};

#endif
