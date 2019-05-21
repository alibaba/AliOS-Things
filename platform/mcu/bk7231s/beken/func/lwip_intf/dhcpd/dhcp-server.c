/** dhcp-server.c: The DHCP Server
 */
#include <string.h>
#include "rtos_pub.h"
#include "dhcp-bootp.h"
#include "dns.h"
#include "dhcp-priv.h"
#include "str_pub.h"
#include "mem_pub.h"

#define os_mem_alloc os_malloc
#define os_mem_free  os_free  
#define SEND_RESPONSE(w,x,y,z)	send_response(w,x,y,z)

#define DEFAULT_DHCP_ADDRESS_TIMEOUT	(60U*60U*1U) /* 1 hour */
#define CLIENT_IP_NOT_FOUND             0x00000000

uint32_t dhcp_address_timeout = DEFAULT_DHCP_ADDRESS_TIMEOUT;
static beken_mutex_t dhcpd_mutex;
static int (*dhcp_nack_dns_server_handler)(char *msg, int len,
					   struct sockaddr_in *fromaddr);

struct dhcp_server_data dhcps;

extern int net_get_if_macaddr(void *macaddr, void *intrfc_handle);
extern int net_get_if_ip_addr(uint32_t *ip, void *intrfc_handle);
extern int net_get_if_ip_mask(uint32_t *nm, void *intrfc_handle);
static void get_broadcast_addr(struct sockaddr_in *addr);
static int get_ip_addr_from_interface(uint32_t *ip, void *interface_handle);
static int get_netmask_from_interface(uint32_t *nm, void *interface_handle);
static int get_mac_addr_from_interface(void *mac, void *interface_handle);
static int send_gratuitous_arp(uint32_t ip);
static bool ac_add(uint8_t *chaddr, uint32_t client_ip);
static uint32_t ac_lookup_mac(uint8_t *chaddr);
static uint8_t *ac_lookup_ip(uint32_t client_ip);
static bool ac_not_full();

static bool ac_add(uint8_t *chaddr, uint32_t client_ip)
{
	/* adds ip-mac mapping in cache */
	if (ac_not_full()) {
		dhcps.ip_mac_mapping[dhcps.count_clients].client_mac[0]
			= chaddr[0];
		dhcps.ip_mac_mapping[dhcps.count_clients].client_mac[1]
			= chaddr[1];
		dhcps.ip_mac_mapping[dhcps.count_clients].client_mac[2]
			= chaddr[2];
		dhcps.ip_mac_mapping[dhcps.count_clients].client_mac[3]
			= chaddr[3];
		dhcps.ip_mac_mapping[dhcps.count_clients].client_mac[4]
			= chaddr[4];
		dhcps.ip_mac_mapping[dhcps.count_clients].client_mac[5]
			= chaddr[5];
		dhcps.ip_mac_mapping[dhcps.count_clients].client_ip = client_ip;
		dhcps.count_clients++;
		return 0;
	}
	return -1;
}

static uint32_t ac_lookup_mac(uint8_t *chaddr)
{
	/* returns ip address, if mac address is present in cache */
	int i;
	for (i = 0; i < dhcps.count_clients && i < MAC_IP_CACHE_SIZE; i++) {
		if ((dhcps.ip_mac_mapping[i].client_mac[0] == chaddr[0]) &&
		    (dhcps.ip_mac_mapping[i].client_mac[1] == chaddr[1]) &&
		    (dhcps.ip_mac_mapping[i].client_mac[2] == chaddr[2]) &&
		    (dhcps.ip_mac_mapping[i].client_mac[3] == chaddr[3]) &&
		    (dhcps.ip_mac_mapping[i].client_mac[4] == chaddr[4]) &&
		    (dhcps.ip_mac_mapping[i].client_mac[5] == chaddr[5])) {
			return dhcps.ip_mac_mapping[i].client_ip;
		}
	}
	return CLIENT_IP_NOT_FOUND;
}

static uint8_t *ac_lookup_ip(uint32_t client_ip)
{
	/* returns mac address, if ip address is present in cache */
	int i;
	for (i = 0; i < dhcps.count_clients && i < MAC_IP_CACHE_SIZE; i++) {
		if ((dhcps.ip_mac_mapping[i].client_ip)	== client_ip) {
			return dhcps.ip_mac_mapping[i].client_mac;
		}
	}
	return NULL;
}

static bool ac_not_full()
{
	/* returns true if cache is not full */
	return (dhcps.count_clients < MAC_IP_CACHE_SIZE);
}

static bool ac_valid_ip(uint32_t requested_ip)
{
	/* skip over our own address, the network address or the
	 * broadcast address
	 */
	if (requested_ip == ntohl(dhcps.my_ip) ||
	       (requested_ip == ntohl(dhcps.my_ip &
					  dhcps.netmask)) ||
	       (requested_ip == ntohl((dhcps.my_ip |
					   (0xffffffff & ~dhcps.netmask))))) {
		return false;
	}
	if (ac_lookup_ip(htonl(requested_ip)) != NULL)
		return false;
	return true;
}

static void write_u32(char *dest, uint32_t be_value)
{
	*dest++ = be_value & 0xFF;
	*dest++ = (be_value >> 8) & 0xFF;
	*dest++ = (be_value >> 16) & 0xFF;
	*dest = be_value >> 24;
}

/* Configure the DHCP dynamic IP lease time*/
int dhcp_server_lease_timeout(uint32_t val)
{
	if ((val == 0) || (val > (60U*60U*24U*49700U))) {
		return -EINVAL;
	} else {
		dhcp_address_timeout = val;
		return 0;
	}
}

/* calculate the address to give out to the next DHCP DISCOVER request
 *
 * DHCP clients will be assigned addresses in sequence in the subnet's address space.
 */
static unsigned int next_yiaddr()
{
#ifdef CONFIG_DHCP_SERVER_DEBUG
	struct in_addr ip;
#endif
	uint32_t new_ip;
	struct bootp_header *hdr = (struct bootp_header *)dhcps.msg;

	/* if device requesting for ip address is already registered,
	 * if yes, assign previous ip address to it
	 */
	new_ip = ac_lookup_mac(hdr->chaddr);
	if (new_ip == (CLIENT_IP_NOT_FOUND)) {
		/* next IP address in the subnet */
		dhcps.current_ip = ntohl(dhcps.my_ip & dhcps.netmask) |
			((dhcps.current_ip + 1) & ntohl(~dhcps.netmask));
		while (!ac_valid_ip(dhcps.current_ip)) {
			dhcps.current_ip = ntohl(dhcps.my_ip & dhcps.netmask) |
				((dhcps.current_ip + 1) &
				 ntohl(~dhcps.netmask));
		}

		new_ip = htonl(dhcps.current_ip);

		if (ac_add(hdr->chaddr, new_ip) !=
		    0)
			dhcp_w("No space to store new mapping..");
	}

#ifdef CONFIG_DHCP_SERVER_DEBUG
	ip.s_addr = new_ip;
	dhcp_d("New client IP will be %s", inet_ntoa(ip));
	ip.s_addr = dhcps.my_ip & dhcps.netmask;
#endif

	return new_ip;
}

static unsigned int make_response(char *msg, enum dhcp_message_type type)
{
	struct bootp_header *hdr;
	struct bootp_option *opt;
	char *offset = msg;

	hdr = (struct bootp_header *)offset;
	hdr->op = BOOTP_OP_RESPONSE;
	hdr->htype = 1;
	hdr->hlen = 6;
	hdr->hops = 0;
	hdr->ciaddr = 0;
	hdr->yiaddr = (type == DHCP_MESSAGE_ACK) ? dhcps.client_ip : 0;
	hdr->yiaddr = (type == DHCP_MESSAGE_OFFER) ?
	    next_yiaddr() : hdr->yiaddr;
	hdr->siaddr = 0;
	hdr->riaddr = 0;
	offset += sizeof(struct bootp_header);

	opt = (struct bootp_option *)offset;
	opt->type = BOOTP_OPTION_DHCP_MESSAGE;
	*(uint8_t *) opt->value = type;
	opt->length = 1;
	offset += sizeof(struct bootp_option) + opt->length;

	if (type == DHCP_MESSAGE_NAK)
		return (unsigned int)(offset - msg);

	opt = (struct bootp_option *)offset;
	opt->type = BOOTP_OPTION_SUBNET_MASK;
	write_u32(opt->value, dhcps.netmask);
	opt->length = 4;
	offset += sizeof(struct bootp_option) + opt->length;

	opt = (struct bootp_option *)offset;
	opt->type = BOOTP_OPTION_ADDRESS_TIME;
	write_u32(opt->value, htonl(dhcp_address_timeout));
	opt->length = 4;
	offset += sizeof(struct bootp_option) + opt->length;

	opt = (struct bootp_option *)offset;
	opt->type = BOOTP_OPTION_DHCP_SERVER_ID;
	write_u32(opt->value, dhcps.my_ip);
	opt->length = 4;
	offset += sizeof(struct bootp_option) + opt->length;

	opt = (struct bootp_option *)offset;
	opt->type = BOOTP_OPTION_ROUTER;
	write_u32(opt->value, dhcps.my_ip);
	opt->length = 4;
	offset += sizeof(struct bootp_option) + opt->length;

	opt = (struct bootp_option *)offset;
	opt->type = BOOTP_OPTION_NAMESERVER;
	if (dhcp_nack_dns_server_handler)
		write_u32(opt->value, dhcps.my_ip);
	else
		write_u32(opt->value, 0);
	opt->length = 4;
	offset += sizeof(struct bootp_option) + opt->length;

	opt = (struct bootp_option *)offset;
	opt->type = BOOTP_END_OPTION;
	offset++;

	return (unsigned int)(offset - msg);
}

int dhcp_get_ip_from_mac(uint8_t *client_mac, uint32_t *client_ip)
{
	*client_ip = ac_lookup_mac(client_mac);
	if (*client_ip == CLIENT_IP_NOT_FOUND) {
		return -1;
	}
	return 0;
}

extern void ap_set_default_netif(void);
extern void reset_default_netif(void);

static int send_response(int sock, struct sockaddr *addr, char *msg, int len)
{
	int nb;
	unsigned int sent = 0;
    // // in AP+STA mode, should set ap-netif as default
    int dest_ip = ((struct sockaddr_in *)addr)->sin_addr.s_addr;
    if((dest_ip == IPADDR_ANY) || (dest_ip == IPADDR_BROADCAST))
        ap_set_default_netif();

	while (sent < len) {
		nb = lwip_sendto(sock, msg + sent, len - sent, 0, addr,
			    sizeof(struct sockaddr_in));
		if (nb < 0) {
			dhcp_e("failed to send response, addr %p:%d", 
				((struct sockaddr_in *)addr)->sin_addr,
				((struct sockaddr_in *)addr)->sin_port);
			return -1;
		}
		sent += nb;
	}

    // rest default netif to sta's netif
    if((dest_ip == IPADDR_ANY) || (dest_ip == IPADDR_BROADCAST))
        reset_default_netif();

	dhcp_d("sent response, %d bytes %s", sent,
	    inet_ntoa(((struct sockaddr_in *)addr)->sin_addr));
	return 0;
}

#define ERROR_REFUSED 5
static int process_dns_message(char *msg, int len, struct sockaddr_in *fromaddr)
{
	struct dns_header *hdr;
	char *endp = msg + len;
	int nq;

	if (len < sizeof(struct dns_header)) {
		dhcp_e("DNS request is not complete, hence ignoring it");
		return -1;
	}

	hdr = (struct dns_header *)msg;

	dhcp_d("DNS transaction id: 0x%x", htons(hdr->id));

	if (hdr->flags.fields.qr) {
		dhcp_e("ignoring this dns message (not a query)");
		return -1;
	}

	nq = ntohs(hdr->num_questions);
	dhcp_d("we were asked %d questions", nq);

	if (nq <= 0) {
		dhcp_e("ignoring this dns msg (not a query or 0 questions)");
		return -1;
	}

	/* make the header represent a response */
	hdr->flags.fields.qr = 1;
	hdr->flags.fields.opcode = 0;
	/* Errors are never authoritative (unless they are
	   NXDOMAINS, which this is not) */
	hdr->flags.fields.aa = 0;
	hdr->flags.fields.tc = 0;
	hdr->flags.fields.rd = 1;
	hdr->flags.fields.ra = 0;
	hdr->flags.fields.rcode = ERROR_REFUSED;
	hdr->flags.num = htons(hdr->flags.num);
	/* number of entries in questions section */
	hdr->num_questions  = htons(0x01);
	hdr->answer_rrs = 0; /* number of resource records in answer section */
	hdr->authority_rrs = 0;
	hdr->additional_rrs = 0;
	SEND_RESPONSE(dhcps.dnssock, (struct sockaddr *)fromaddr,
		      msg, endp - msg);

	return -1;
}

static int process_dhcp_message(char *msg, int len)
{
	struct bootp_header *hdr;
	struct bootp_option *opt;
	uint8_t response_type = DHCP_NO_RESPONSE;
	unsigned int consumed = 0;
	bool got_ip = 0;
	bool need_ip = 0;
	int ret = 0;
	bool got_client_ip = 0;
	uint32_t new_ip;

	if (!msg ||
	    len < sizeof(struct bootp_header) + sizeof(struct bootp_option) + 1)
		return -1;

	hdr = (struct bootp_header *)msg;

	switch (hdr->op) {
	case BOOTP_OP_REQUEST:
		dhcp_d("bootp request");
		break;
	case BOOTP_OP_RESPONSE:
		dhcp_d("bootp response");
		break;
	default:
		dhcp_e("invalid op code: %d", hdr->op);
		return -1;
	}

	if (hdr->htype != 1 || hdr->hlen != 6) {
		dhcp_e("invalid htype or hlen");
		return -1;
	}

	dhcp_d("client MAC: %02X:%02X:%02X:%02X:%02X:%02X", hdr->chaddr[0],
	    hdr->chaddr[1], hdr->chaddr[2], hdr->chaddr[3], hdr->chaddr[4],
	    hdr->chaddr[5]);

	dhcp_d("magic cookie: 0x%X", hdr->cookie);

	len -= sizeof(struct bootp_header);
	opt = (struct bootp_option *)(msg + sizeof(struct bootp_header));
	while (len > 0 && opt->type != BOOTP_END_OPTION) {
		if (opt->type == BOOTP_OPTION_DHCP_MESSAGE && opt->length == 1) {
			dhcp_d("found DHCP message option");
			switch (*(uint8_t *) opt->value) {
			case DHCP_MESSAGE_DISCOVER:
				dhcp_d("DHCP discover");
				response_type = DHCP_MESSAGE_OFFER;
				break;

			case DHCP_MESSAGE_REQUEST:
				dhcp_d("DHCP request");
				need_ip = 1;
				if (hdr->ciaddr != 0x0000000) {
					dhcps.client_ip = hdr->ciaddr;
					got_client_ip = 1;
				}
				break;

			default:
				dhcp_d("ignoring message type %d",
				    *(uint8_t *) opt->value);
				break;
			}
		}
		if (opt->type == BOOTP_OPTION_REQUESTED_IP && opt->length == 4) {
			dhcp_d("found REQUESTED IP option %hhu.%hhu.%hhu.%hhu",
			    opt->value[0],
			    opt->value[1], opt->value[2], opt->value[3]);
			memcpy((uint8_t *) &dhcps.client_ip,
			       (uint8_t *) opt->value, 4);
			got_client_ip = 1;
		}

		if (got_client_ip) {
			/* requested address outside of subnet */
			if ((dhcps.client_ip & dhcps.netmask) ==
			    (dhcps.my_ip & dhcps.netmask)) {

				/* When client requests an IP address,
				 * DHCP-server checks if the valid
				 * IP-MAC entry is present in the
				 * ip-mac cache, if yes, also checks
				 * if the requested IP is same as the
				 * IP address present in IP-MAC entry,
				 * if yes, it allows the device to
				 * continue with the requested IP
				 * address.
				 */
				new_ip = ac_lookup_mac(hdr->chaddr);
				if (new_ip != (CLIENT_IP_NOT_FOUND)) {
					/* if new_ip is equal to requested ip */
					if (new_ip == dhcps.client_ip) {
						got_ip = 1;
					} else {
						got_ip = 0;
					}
				} else if (ac_valid_ip
					   (ntohl(dhcps.client_ip))) {
					/* When client requests with an IP
					 * address that is within subnet range
					 * and not assigned to any other client,
					 * then dhcp-server allows that device
					 * to continue with that IP address.
					 * And if IP-MAC cache is not full then
					 * adds this entry in cache.
					 */
					if (ac_not_full()) {
						ac_add(hdr->chaddr,
						       dhcps.client_ip);
					} else {
						dhcp_w("No space to store new "
						       "mapping..");
					}
					got_ip = 1;
				}
			}
		}

		/* look at the next option (if any) */
		consumed = sizeof(struct bootp_option) + opt->length;
		len -= consumed;
		opt = (struct bootp_option *)((char *)opt + consumed);
		if (need_ip)
			response_type = got_ip ? DHCP_MESSAGE_ACK :
			    DHCP_MESSAGE_NAK;
	}

	if (response_type != DHCP_NO_RESPONSE) {
		ret = make_response(msg, (enum dhcp_message_type)response_type);
		ret = SEND_RESPONSE(dhcps.sock,
				    (struct sockaddr *)&dhcps.baddr, msg, ret);
		if (response_type == DHCP_MESSAGE_ACK)
			send_gratuitous_arp(dhcps.my_ip);
		return 0;
	}

	dhcp_d("ignoring DHCP packet");
	return 0;
}

static void dhcp_clean_sockets()
{
	int ret;

	if (dhcps.sock != -1) {
		ret = lwip_close(dhcps.sock);
		if (ret != 0) {
			dhcp_w("Failed to close dhcp socket");
		}
		dhcps.sock = -1;
	}
	if ( dhcps.ctrlsock != -1 )
	{
	    ret = lwip_close(dhcps.ctrlsock);
		if ( ret != 0 )
		{
			dhcp_w("Failed to close ctrol port socket");
		}
		dhcps.ctrlsock = -1;
	}
}

void dhcp_server(void* data)
{
	int ret;
	struct sockaddr_in caddr;
	struct sockaddr_in ctrl_listen;
	int addr_len;
	int max_sock;
	int len;
	socklen_t flen = sizeof(caddr);
	fd_set rfds;


	rtos_lock_mutex(&dhcpd_mutex);

	while (1) {
		FD_ZERO(&rfds);
		FD_SET(dhcps.sock, &rfds);
		max_sock = dhcps.sock;

		if (dhcp_nack_dns_server_handler) {
			FD_SET(dhcps.dnssock, &rfds);
			max_sock = (dhcps.sock > dhcps.dnssock ?
				    dhcps.sock : dhcps.dnssock);
		}

		FD_SET(dhcps.ctrlsock, &rfds);
		max_sock = (dhcps.sock > dhcps.ctrlsock ?
					dhcps.sock : dhcps.ctrlsock);

		ret = lwip_select(max_sock + 1, &rfds, NULL, NULL, NULL);

		/* Error in select? */
		if (ret < 0) {
			dhcp_e("select failed", -1);
			goto done;
		}

		if (FD_ISSET(dhcps.sock, &rfds)) {
			len = lwip_recvfrom(dhcps.sock, dhcps.msg,
				       SERVER_BUFFER_SIZE,
				       0, (struct sockaddr *)&caddr, &flen);
			if (len > 0) {
				dhcp_d("recved msg on dhcp sock len: %d", len);
				process_dhcp_message(dhcps.msg, len);
			}
		}

		if ( dhcp_nack_dns_server_handler )
		{
			if (FD_ISSET(dhcps.dnssock, &rfds)) {
				len = lwip_recvfrom(dhcps.dnssock, dhcps.msg,
						   SERVER_BUFFER_SIZE,
						   0, (struct sockaddr *)&caddr, &flen);
				if (len > 0) {
					dhcp_d("recved msg on dns sock len: %d", len);
					dhcp_nack_dns_server_handler(dhcps.msg, len,
									 &caddr);
				}
			}
		}

		if ( FD_ISSET(dhcps.ctrlsock, &rfds) )
		{
		    len = lwip_recvfrom(dhcps.ctrlsock, dhcps.msg,
						SERVER_BUFFER_SIZE,
						0, (struct sockaddr *)&caddr, &flen);
			if ( len > 0 )
			{
				if ( os_strcmp(dhcps.msg, "HALT") == 0 )
				{
					goto done;
				}
			}
		}
	}

done:
	dhcp_clean_sockets();
	rtos_unlock_mutex(&dhcpd_mutex);
	rtos_delete_thread(NULL);
}

static int create_and_bind_udp_socket(struct sockaddr_in *address,
					void *intrfc_handle)
{
	int one = 1;
	int ret;

	int sock = lwip_socket(PF_INET, SOCK_DGRAM, 0);
	if (sock == -1) {
		dhcp_e("failed to create a socket");
		return -1;
	}

	ret = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&one,
			 sizeof(int));
	if (ret == -1) {
		/* This is unimplemented in lwIP, hence do not return */
		dhcp_e("failed to set SO_REUSEADDR");
	}

	if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST,
		       (char *)&one, sizeof(one)) == -1) {
		dhcp_e("failed to set SO_BROADCAST");
		lwip_close(sock);
		return -1;
	}

	ret = lwip_bind(sock, (struct sockaddr *)address,
					sizeof(struct sockaddr));

	if (ret) {
		dhcp_e("failed to bind server socket");
		dhcp_e("socket err: %d", errno);
		lwip_close(sock);
		return -1;
	}
	return sock;
}

int dhcp_server_init(void *intrfc_handle)
{
	int ret = 0;

	memset(&dhcps, 0, sizeof(dhcps));
	dhcps.msg = (char*)os_mem_alloc(SERVER_BUFFER_SIZE);
	if (dhcps.msg == NULL)
		return -1;
	ret = rtos_init_mutex(&dhcpd_mutex);
	if (ret != 0)
		return -1;

	get_broadcast_addr(&dhcps.baddr);
	dhcps.baddr.sin_port = htons(DHCP_CLIENT_PORT);

	if (get_ip_addr_from_interface(&dhcps.my_ip, intrfc_handle) < 0) {
		dhcp_e("failed to look up our IP address from interface");
		ret = -1;
		goto out;
	}

	if (get_netmask_from_interface(&dhcps.netmask, intrfc_handle) < 0) {
		dhcp_e("failed to look up our netmask from interface");
		ret = -1;
		goto out;
	}

	dhcps.saddr.sin_family = AF_INET;
	dhcps.saddr.sin_addr.s_addr = INADDR_ANY;
	dhcps.saddr.sin_port = htons(DHCP_SERVER_PORT);
	dhcps.sock = create_and_bind_udp_socket(&dhcps.saddr, intrfc_handle);

	if (dhcps.sock < 0) {
		ret = -1;
		goto out;
	}

	if (dhcp_nack_dns_server_handler) {
		dhcps.dnsaddr.sin_family = AF_INET;
		dhcps.dnsaddr.sin_addr.s_addr = INADDR_ANY;
		dhcps.dnsaddr.sin_port = htons(NAMESERVER_PORT);
		dhcps.dnssock = create_and_bind_udp_socket(&dhcps.dnsaddr,
							   intrfc_handle);
		if (dhcps.dnssock < 0)
			return -1;
	}

	dhcps.ctrladdr.sin_family = AF_INET;
	dhcps.ctrladdr.sin_addr.s_addr = inet_addr("127.0.0.1");
	dhcps.ctrladdr.sin_port = htons(CTRL_PORT);
	dhcps.ctrlsock = create_and_bind_udp_socket(&dhcps.ctrladdr,
							intrfc_handle);
	if ( dhcps.ctrlsock < 0 )
	{
	    ret  = -1;
		goto out;
	}

    dhcps.prv = intrfc_handle;

	return 0;

out:
	os_mem_free(dhcps.msg);
	dhcps.msg = NULL;
	rtos_deinit_mutex(&dhcpd_mutex);
	return ret;
}

void dhcp_enable_nack_dns_server()
{
	dhcp_nack_dns_server_handler = process_dns_message;
}

static int send_ctrl_msg(const char *msg)
{
	int ret;

	ret = lwip_sendto(dhcps.ctrlsock, msg, os_strlen(msg)+1, 0, 
		(struct sockaddr *)&dhcps.ctrladdr, sizeof(struct sockaddr_in));

	return ret;
}

int dhcp_send_halt(void)
{
	int ret;

	ret = send_ctrl_msg("HALT");
	if ( ret < 0 )
	{
	    dhcp_w("Failed to send HALT: %d.", ret);
		return -1;
	}
	
	ret = dhcp_free_allocations();

	return ret;
}

int dhcp_free_allocations(void)
{
	/* Wait for 10 seconds */
	rtos_lock_mutex(&dhcpd_mutex);

	dhcp_clean_sockets();

	rtos_unlock_mutex(&dhcpd_mutex);

	if (dhcps.msg) {
		os_mem_free(dhcps.msg);
		dhcps.msg = NULL;
	}
	return rtos_deinit_mutex(&dhcpd_mutex);
}

static int send_gratuitous_arp(uint32_t ip)
{
	int sock;
	struct arp_packet pkt;
	struct sockaddr_in to_addr;
	to_addr.sin_family = AF_INET;
	to_addr.sin_addr.s_addr = ip;
	pkt.frame_type = htons(ARP_FRAME_TYPE);
	pkt.hw_type = htons(ETHER_HW_TYPE);
	pkt.prot_type = htons(IP_PROTO_TYPE);
	pkt.hw_addr_size = ETH_HW_ADDR_LEN;
	pkt.prot_addr_size = IP_ADDR_LEN;
	pkt.op = htons(OP_ARP_REQUEST);

	write_u32(pkt.sndr_ip_addr, ip);
	write_u32(pkt.rcpt_ip_addr, ip);

	memset(pkt.targ_hw_addr, 0xff, ETH_HW_ADDR_LEN);
	memset(pkt.rcpt_hw_addr, 0xff, ETH_HW_ADDR_LEN);
    get_mac_addr_from_interface((void*)pkt.sndr_hw_addr, dhcps.prv);
    get_mac_addr_from_interface((void*)pkt.src_hw_addr, dhcps.prv);   
    
	sock = lwip_socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0) {
		dhcp_e("Could not open socket to send Gratuitous ARP");
		return -1;
	}
	memset(pkt.padding, 0, sizeof(pkt.padding));

	if (lwip_sendto(sock, (char *)&pkt, sizeof(pkt), 0,
		   (struct sockaddr *)&to_addr, sizeof(to_addr)) < 0) {
		dhcp_e("Failed to send Gratuitous ARP");
		lwip_close(sock);
		return -1;
	}
	dhcp_d("Gratuitous ARP sent");
	lwip_close(sock);
	return 0;
}

static void get_broadcast_addr(struct sockaddr_in *addr)
{
	addr->sin_family = AF_INET;
	/* limited broadcast addr (255.255.255.255) */
	addr->sin_addr.s_addr = 0xffffffff;
	addr->sin_len = sizeof(struct sockaddr_in);
}

static int get_mac_addr_from_interface(void *mac, void *interface_handle)
{
	return net_get_if_macaddr(mac, interface_handle);
}

static int get_ip_addr_from_interface(uint32_t *ip, void *interface_handle)
{
	return net_get_if_ip_addr(ip, interface_handle);
}

static int get_netmask_from_interface(uint32_t *nm, void *interface_handle)
{
	return net_get_if_ip_mask(nm, interface_handle);
}

uint8_t* dhcp_lookup_mac(uint8_t *chaddr)
{
	/* returns ip address, if mac address is present in cache */
	int i;
    struct in_addr ip;
	for (i = 0; i < dhcps.count_clients && i < MAC_IP_CACHE_SIZE; i++) {
		if ((dhcps.ip_mac_mapping[i].client_mac[0] == chaddr[0]) &&
		    (dhcps.ip_mac_mapping[i].client_mac[1] == chaddr[1]) &&
		    (dhcps.ip_mac_mapping[i].client_mac[2] == chaddr[2]) &&
		    (dhcps.ip_mac_mapping[i].client_mac[3] == chaddr[3]) &&
		    (dhcps.ip_mac_mapping[i].client_mac[4] == chaddr[4]) &&
		    (dhcps.ip_mac_mapping[i].client_mac[5] == chaddr[5])) {

            ip.s_addr = dhcps.ip_mac_mapping[i].client_ip;
 
			return inet_ntoa(ip);
		}
	}
	return 0;
}