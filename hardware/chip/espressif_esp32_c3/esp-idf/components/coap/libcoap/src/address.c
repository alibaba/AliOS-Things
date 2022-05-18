/* address.c -- representation of network addresses
 *
 * Copyright (C) 2015-2016,2019 Olaf Bergmann <bergmann@tzi.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * This file is part of the CoAP library libcoap. Please see
 * README for terms of use.
 */

#include "coap3/coap_internal.h"

#if !defined(WITH_CONTIKI) && !defined(WITH_LWIP)
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef HAVE_WS2TCPIP_H
#include <ws2tcpip.h>
#endif

#ifdef RIOT_VERSION
/* FIXME */
#define IN_MULTICAST(Address) (0)
#endif /* RIOT_VERSION */

uint16_t
coap_address_get_port(const coap_address_t *addr) {
  assert(addr != NULL);
  switch (addr->addr.sa.sa_family) {
  case AF_INET: return ntohs(addr->addr.sin.sin_port);
  case AF_INET6: return ntohs(addr->addr.sin6.sin6_port);
  default: /* undefined */
    ;
  }
  return 0;
}

void
coap_address_set_port(coap_address_t *addr, uint16_t port) {
  assert(addr != NULL);
  switch (addr->addr.sa.sa_family) {
  case AF_INET:
    addr->addr.sin.sin_port = htons(port);
    break;
  case AF_INET6:
    addr->addr.sin6.sin6_port = htons(port);
    break;
  default: /* undefined */
    ;
  }
}

int
coap_address_equals(const coap_address_t *a, const coap_address_t *b) {
  assert(a); assert(b);

  if (a->size != b->size || a->addr.sa.sa_family != b->addr.sa.sa_family)
    return 0;

  /* need to compare only relevant parts of sockaddr_in6 */
 switch (a->addr.sa.sa_family) {
 case AF_INET:
   return
     a->addr.sin.sin_port == b->addr.sin.sin_port &&
     memcmp(&a->addr.sin.sin_addr, &b->addr.sin.sin_addr,
            sizeof(struct in_addr)) == 0;
 case AF_INET6:
   return a->addr.sin6.sin6_port == b->addr.sin6.sin6_port &&
     memcmp(&a->addr.sin6.sin6_addr, &b->addr.sin6.sin6_addr,
            sizeof(struct in6_addr)) == 0;
 default: /* fall through and signal error */
   ;
 }
 return 0;
}

int coap_is_mcast(const coap_address_t *a) {
  if (!a)
    return 0;

 switch (a->addr.sa.sa_family) {
 case AF_INET:
   return IN_MULTICAST(ntohl(a->addr.sin.sin_addr.s_addr));
 case  AF_INET6:
   return IN6_IS_ADDR_MULTICAST(&a->addr.sin6.sin6_addr) ||
       (IN6_IS_ADDR_V4MAPPED(&a->addr.sin6.sin6_addr) &&
           IN_MULTICAST(ntohl(a->addr.sin6.sin6_addr.s6_addr[12])));
 default:  /* fall through and signal error */
   ;
  }
 return 0;
}

#endif /* !defined(WITH_CONTIKI) && !defined(WITH_LWIP) */

void coap_address_init(coap_address_t *addr) {
  assert(addr);
  memset(addr, 0, sizeof(coap_address_t));
#if !defined(WITH_LWIP) && !defined(WITH_CONTIKI)
  /* lwip and Contiki have constant address sizes and don't need the .size part */
  addr->size = sizeof(addr->addr);
#endif
}

