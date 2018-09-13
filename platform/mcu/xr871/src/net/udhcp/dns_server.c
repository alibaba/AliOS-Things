/***
   * dns-server.c
   * Authors: Aditya Geria, Monisha Jain, Jeevana Lagisetty
   * Version 1.0.4 (April 1, 2016)
   * Fixed debug information, cleaned up messages shown
   * Host a simple DNS server which will resolve hosts from a given
   * hosts.txt file, and provide an IP address.
  **/

#include "dns.h"
#include "lwip/netdb.h"
#include <ctype.h>
#include <stdio.h>

int process_request (int fd, void* recvbuf, int recvlen, struct sockaddr *to)
{
	int ret;
	dns_header* header = (dns_header*) recvbuf;
	socklen_t addrlen = sizeof(*to);

	if(header->qr == 0) {
		DNS_DBG("Correct query field - identified question\n");
		DNS_DBG("Query count: %d\n", htons(header->qd_count));

		//char* query = (char*)recvbuf + sizeof(dns_header); //gets the query name
		//DNS_DBG("Query length: %d\n", strlen(query));

		header->qr = 1;
		header->aa = 0;
		header->tc = 0;
		header->ra = 1;
		header->rcode = 5; //refuse;
		//header->qd_count = htons(1);
		//header->an_count = 0;
		//header->ns_count = 0;
		//header->ar_count = 0;

		//no need to add answer record, or query
		if((ret = sendto(fd, recvbuf, recvlen, 0, to, addrlen)) < 0) {
			DNS_ERR("Failed to send, line %d\n", __LINE__);
			return -1;
		}
		return ret;
	}else {
		return -1;
	}
}

/**
 * @Method dump
 * @param buf - thing to view/dump
 * @param len - length of buf
 * Shows contents of (buf) in bitwise manner
 */
void dump (char* buf, int len) {
	int i = 0;

	for(i=0; i < len; ++i)
		DNS_DBG("%02x ", buf[i]);  // dump hex values
	DNS_DBG("\n");

	for(i=0; i < len; ++i) {
		if((u8_t)buf[i] >= 0x20 && (u8_t)buf[i] <= 0x7f)
			DNS_DBG("'%c' ", buf[i]);  // dump a character if it’s printable
		else
			DNS_DBG("0x%02x ", buf[i]);  // else dump a hex value
	}
	DNS_DBG("\n");
	return;
}

int dns_listen_socket(void) {
	struct sockaddr_in myaddr; // our address
	int fd = -1; // our socket
	int port = 53;
	int n = 1;

	DNS_DBG("Opening listen socket on port %d for dns server\n", port);
    /* create a UDP socket */
    if ((fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        DNS_ERR("cannot create socket\n");
        return -1;
    }

	if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *) &n, sizeof(n)) < 0) {
		closesocket(fd);
		return -1;
	}

    /* bind the socket to any valid IP address and a specific port */
	memset((char *)&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(port);

    if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
        closesocket(fd);
        return -1;
    }
	return fd;
}

int dns_server(int fd, char *recvbuf, int buflen)
{
	struct sockaddr_in remaddr; // remote address
	socklen_t addrlen = sizeof(remaddr); // length of addresses
	int ret; //bytes received

	ret = recvfrom(fd, recvbuf, buflen, 0, (struct sockaddr *)&remaddr, &addrlen);
	if (ret < 0 || ret < sizeof(dns_header)) {
		DNS_ERR("dns packet recv err\n");
		return -1;
	}
	DNS_DBG("dns server received %d bytes\n", ret);
	//dump(recvbuf, ret); //debugging dump
	ret = process_request(fd, recvbuf, ret, (struct sockaddr *)&remaddr);
	return ret;
}


