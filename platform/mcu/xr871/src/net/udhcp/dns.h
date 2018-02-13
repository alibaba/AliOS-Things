/***
   * dns-server.c
   * Provided by: Paul Krzyzanowski
   * Group: Aditya Geria, Monisha Jain, Jeevana Lagisetty
   * Header file for dns-server.c
   * Provides the header structs for a DNS query exchange
  **/

#ifndef __DHCPD_DNS_H__
#define __DHCPD_DNS_H__

#include <stdio.h>
#include <stdlib.h>
#include "lwip/sockets.h"
#include <sys/types.h>
#include <string.h>

/*
	DNS structures
*/

/* dns header - see rfc1035 */
/* this is the main header of a DNS message */
/* it is followed by zero or more questions, answers, authorities, and additional sections */
/* the last four count fields tell you how many of each to expect */

typedef struct {
	unsigned short id;
	unsigned char rd:1;
	unsigned char tc:1;
	unsigned char aa:1;
	unsigned char opcode:4;
	unsigned char qr:1;
	unsigned char rcode:4;
	unsigned char cd:1;
	unsigned char ad:1;
	unsigned char z:1;
	unsigned char ra:1;
	unsigned short qd_count;
	unsigned short an_count;
	unsigned short ns_count;
	unsigned short ar_count;
} dns_header;

/* dns question section format. This is prepended with a name */
/* check the specs for the format of a name. Instead of components */
/* separated by dots, each component is prefixed with a byte containing */
/* the length of that component */

typedef struct {
	unsigned short qtype;
	unsigned short qclass;
} dns_question;

/* DNS resource record format */
/* The answer, authority, and additional sections all share this format. */
/* It is prepended with a name and suffixed with additional data */

typedef struct __attribute__ ((__packed__)) {
	unsigned short type;
	unsigned short class;
	unsigned int ttl;
	unsigned short data_len;
	//unsigned int rdata;
} dns_rrhdr;

#define DNS_BUF_SIZE 256

//#define DNS_SERVER_DBG

#ifdef DNS_SERVER_DBG
#define	DNS_DBG			printf
#define DNS_ERR(x...)	do {printf("[DNS ERR]"); printf(x); } while (0)
#else
#define	DNS_DBG(x...) NULL
#define DNS_ERR(x...) NULL
#endif /* DEBUG */

int dns_listen_socket(void);
int dns_server(int fd, char *recvbuf, int buflen);

#endif