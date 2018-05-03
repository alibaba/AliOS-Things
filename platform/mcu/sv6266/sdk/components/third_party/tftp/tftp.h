#ifndef TFTP_H
#define TFTP_H

#include "stdio.h"
#include <stdlib.h>
#include <string.h>
#include "lwip/sockets.h"
#include "lwip/err.h"
#include "packets.h"
#include "common.h"
#include "pong.h"
#include "netstack_def.h"

//port 6100 + group 6
#define SERV_UDP_PORT   69

#define SERV_HOST_ADDR  "192.168.1.6"

#define MAX_TFTP_CLIENTS 100
#define MAX_TFTP_TIMEOUTS 10
#define TFTP_TIMEOUT_DURATION 2 //seconds

//we only support octet mode
#define TFTP_SUPORTED_MODE "octet"

//max packet size
#define BUFSIZE 1024

//for testing
#define DEBUG true

//extern int l3_errno;
//#define errno l3_errno

int tftp_recv_req(char *servername, uint16_t server_port, char *filename);
int tftp_recv(PACKET *packet, char *filename);

#endif

