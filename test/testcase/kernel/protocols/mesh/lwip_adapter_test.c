/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "yunit.h"

#include "lwip/ip6_addr.h"
#include "lwip/sockets.h"

#include "umesh_types.h"
#include "utilities/message.h"
#include "ip/ip6.h"
#include "ip/lwip_adapter.h"

#if LWIP_IPV6
extern struct netif *ur_adapter_ip6_route(const ip6_addr_t *src, const ip6_addr_t *dest);
extern bool ur_adapter_is_mcast_subscribed(const ip6_addr_t *addr);
#endif
extern void interface_start(void);
extern void interface_stop(void);

void test_uradar_lwip_adapter_case(void)
{
    YUNIT_ASSERT(UR_ERROR_NONE == ur_adapter_interface_update());
    interface_start();
    YUNIT_ASSERT(UR_ERROR_NONE == ur_adapter_interface_up());

#if LWIP_IPV6
    YUNIT_ASSERT(NULL != ur_adapter_ip6_route(NULL, NULL));

    ip6_addr_t ip6addr;
    memset(ip6addr.addr, 0x00, sizeof(ip6addr));
    ip6addr.addr[0] = 0x00110011;
    YUNIT_ASSERT(false == ur_adapter_is_mcast_subscribed(&ip6addr));
#endif

    message_t *message;
    message = message_alloc(60, UT_MSG);
    ur_ip6_header_t *ip6_header;
    ip6_header = (ur_ip6_header_t *)message_get_payload(message);
    memset(ip6_header, 0x00, UR_IP6_HLEN);
    ip6_header->v_tc_fl = htonl(6<<24);
    ip6_header->len = 60 - UR_IP6_HLEN;
    ip6_header->next_header = 17;
    ip6_header->hop_lim = 255;
    YUNIT_ASSERT(UR_ERROR_NONE == ur_adapter_input(message->data));

    int socket;
    ur_udp_header_t *header;
    ur_ip6_addr_t dst;
    header = (ur_udp_header_t *)malloc(16);
    header->src_port = htons(2016);
    header->dst_port = htons(2016);
    header->length = htons(16);
    header->chksum = 0;
    string_to_ip6_addr("F'00::0", &dst);
    string_to_ip6_addr("fc00::0", &dst);
    socket = lwip_socket(AF_INET6, SOCK_RAW, IPPROTO_UDP);
    ip6_sendto(socket, (const uint8_t *)header, 16, &dst, 0);
    /* TODO: sleep to wait send actually happen */
    free(header);


    interface_stop();
    YUNIT_ASSERT(UR_ERROR_NONE == ur_adapter_interface_down());
}


