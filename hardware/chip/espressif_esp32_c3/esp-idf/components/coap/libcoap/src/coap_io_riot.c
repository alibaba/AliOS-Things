/* coap_io_riot.c -- Default network I/O functions for libcoap on RIOT
 *
 * Copyright (C) 2019 Olaf Bergmann <bergmann@tzi.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * This file is part of the CoAP library libcoap. Please see
 * README for terms of use.
 */

#include "coap3/coap_internal.h"

#ifdef HAVE_STDIO_H
#  include <stdio.h>
#endif

#ifdef HAVE_SYS_SOCKET_H
# include <sys/socket.h>
# define OPTVAL_T(t)         (t)
# define OPTVAL_GT(t)        (t)
#endif
#ifdef HAVE_SYS_IOCTL_H
 #include <sys/ioctl.h>
#endif
#ifdef HAVE_NETINET_IN_H
# include <netinet/in.h>
#endif
#ifdef HAVE_SYS_UIO_H
# include <sys/uio.h>
#endif
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
#include <errno.h>

#include "net/gnrc.h"
#include "net/gnrc/ipv6.h"
#include "net/gnrc/netreg.h"
#include "net/udp.h"

#include "coap_riot.h"

ssize_t
coap_network_send(coap_socket_t *sock,
                  const coap_session_t *session,
                  const uint8_t *data,
                  size_t datalen) {
  ssize_t bytes_written = 0;

  if (!coap_debug_send_packet()) {
    bytes_written = (ssize_t)datalen;
  } else if (sock->flags & COAP_SOCKET_CONNECTED) {
    bytes_written = send(sock->fd, data, datalen, 0);
  } else {
    bytes_written = sendto(sock->fd, data, datalen, 0,
                           &session->addr_info.remote.addr.sa,
                           session->addr_info.remote.size);
  }

  if (bytes_written < 0)
    coap_log(LOG_CRIT, "coap_network_send: %s\n", coap_socket_strerror());

  return bytes_written;
}

static udp_hdr_t *
get_udp_header(gnrc_pktsnip_t *pkt) {
  gnrc_pktsnip_t *udp = gnrc_pktsnip_search_type(pkt, GNRC_NETTYPE_UDP);
  return udp ? (udp_hdr_t *)udp->data : NULL;
}

ssize_t
coap_network_read(coap_socket_t *sock, coap_packet_t *packet) {
  size_t len;
  ipv6_hdr_t *ipv6_hdr;
  /* The GNRC API currently only supports UDP. */
  gnrc_pktsnip_t *udp;
  udp_hdr_t *udp_hdr;
  const gnrc_nettype_t type = GNRC_NETTYPE_UDP;

  assert(sock);
  assert(packet);

  if ((sock->flags & COAP_SOCKET_CAN_READ) == 0) {
    coap_log(LOG_DEBUG, "coap_network_read: COAP_SOCKET_CAN_READ not set\n");
    return -1;
  } else {
    /* clear has-data flag */
    sock->flags &= ~COAP_SOCKET_CAN_READ;
  }

  /* Search for the transport header in the packet received from the
   * network interface driver. */
  udp = gnrc_pktsnip_search_type(sock->pkt, type);
  ipv6_hdr = gnrc_ipv6_get_header(sock->pkt);

  if (!ipv6_hdr || !udp || !(udp_hdr = (udp_hdr_t *)udp->data)) {
    coap_log(LOG_DEBUG, "no UDP header found in packet\n");
    return -EFAULT;
  }
  udp_hdr_print(udp_hdr);

  len = (size_t)gnrc_pkt_len_upto(sock->pkt, type) - sizeof(udp_hdr_t);
  coap_log(LOG_DEBUG, "coap_network_read: recvfrom got %zd bytes\n", len);
  if (len > COAP_RXBUFFER_SIZE) {
    coap_log(LOG_WARNING, "packet exceeds buffer size, truncated\n");
    len = COAP_RXBUFFER_SIZE;
  }
  packet->ifindex = sock->fd;

  assert(sizeof(struct in6_addr) == sizeof(ipv6_addr_t));
  packet->addr_info.remote.size = sizeof(struct sockaddr_in6);
  memset(&packet->addr_info.remote.addr, 0,
         sizeof(packet->addr_info.remote.addr));
  packet->addr_info.remote.addr.sin6.sin6_family = AF_INET6;
  memcpy(&packet->addr_info.remote.addr.sin6.sin6_addr,
         &ipv6_hdr->src, sizeof(ipv6_addr_t));
  memcpy(&packet->addr_info.remote.addr.sin6.sin6_port,
         &udp_hdr->src_port, sizeof(udp_hdr->src_port));

  packet->addr_info.local.size = sizeof(struct sockaddr_in6);
  memset(&packet->addr_info.local.addr, 0, sizeof(packet->addr_info.local.addr));
  packet->addr_info.local.addr.sin6.sin6_family = AF_INET6;
  memcpy(&packet->addr_info.local.addr.sin6.sin6_addr,
         &ipv6_hdr->dst, sizeof(ipv6_addr_t));
  memcpy(&packet->addr_info.local
         .addr.sin6.sin6_port, &udp_hdr->dst_port, sizeof(udp_hdr->src_port));

  packet->ifindex = sock->fd;
  packet->length = (len > 0) ? len : 0;
  memcpy(packet->payload, (uint8_t*)udp_hdr + sizeof(udp_hdr_t), len);
  if (LOG_DEBUG <= coap_get_log_level()) {
    unsigned char addr_str[INET6_ADDRSTRLEN + 8];

    if (coap_print_addr(&packet->addr_info.remote, addr_str, INET6_ADDRSTRLEN + 8)) {
      coap_log(LOG_DEBUG, "received %zd bytes from %s\n", len, addr_str);
    }
  }

  return len;
}

static msg_t _msg_q[LIBCOAP_MSG_QUEUE_SIZE];

void
coap_riot_startup(void) {
  msg_init_queue(_msg_q, LIBCOAP_MSG_QUEUE_SIZE);
}

/**
 * Returns the port of @p addr in network byte order or 0 on error.
 */
static uint16_t
get_port(const coap_address_t *addr) {
  if (addr) {
    switch (addr->addr.sa.sa_family) {
    case AF_INET: return addr->addr.sin.sin_port;
    case AF_INET6: return addr->addr.sin6.sin6_port;
    default:
      ;
    }
  }
  return 0;
}

static coap_socket_t *
find_socket(coap_fd_t fd, coap_socket_t *sockets[], unsigned int num_sockets) {
  for (unsigned int i = 0; i < num_sockets; i++) {
    if (fd == sockets[i]->fd)
      return sockets[i];
  }
  return NULL;
}

static bool
address_equals(const coap_address_t *a, const ipv6_addr_t *b) {
  assert(a);
  assert(b);
  return IN6_IS_ADDR_UNSPECIFIED(&a->addr.sin6.sin6_addr) ||
    (memcmp(&a->addr.sin6.sin6_addr, b->u8, sizeof(struct in6_addr)) == 0);
}

int
coap_io_process(coap_context_t *ctx, uint32_t timeout_ms) {
  coap_tick_t before, now;
  coap_socket_t *sockets[LIBCOAP_MAX_SOCKETS];
  unsigned int num_sockets = 0, timeout;
  gnrc_netreg_entry_t coap_reg =
    GNRC_NETREG_ENTRY_INIT_PID(GNRC_NETREG_DEMUX_CTX_ALL, thread_getpid());
  msg_t msg;
  bool found_port = false;

  coap_ticks(&before);

  timeout =
    coap_io_prepare_io(ctx, sockets, ARRAY_SIZE(sockets), &num_sockets, before);
  if (timeout == 0 || timeout_ms < timeout)
    timeout = timeout_ms;

  if (num_sockets > 0) {
    gnrc_netreg_register(GNRC_NETTYPE_UDP, &coap_reg);
  }

  if (timeout == 0 || timeout_ms < timeout)
    timeout = timeout_ms;

  xtimer_msg_receive_timeout(&msg, timeout_ms * US_PER_SEC);
  switch (msg.type) {
  case GNRC_NETAPI_MSG_TYPE_RCV: {
    coap_log(LOG_DEBUG, "coap_run_once: GNRC_NETAPI_MSG_TYPE_RCV\n");

    coap_session_t *s, *rtmp;
    udp_hdr_t *udp_hdr = get_udp_header((gnrc_pktsnip_t *)msg.content.ptr);
    ipv6_hdr_t *ip6_hdr =
      gnrc_ipv6_get_header((gnrc_pktsnip_t *)msg.content.ptr);
    if (!udp_hdr || !ip6_hdr)
      break;
    coap_log(LOG_DEBUG, "coap_run_once: found UDP header\n");

    /* Traverse all sessions and set COAP_SOCKET_CAN_READ if the
     * received packet's destination address matches. */
    SESSIONS_ITER(ctx->sessions, s, rtmp) {
      coap_log(LOG_DEBUG, "coap_run_once: check ctx->sessions %u == %u\n",
               ntohs(get_port(&s->addr_info.local)),
               ntohs(udp_hdr->dst_port.u16));
      if ((get_port(&s->addr_info.local) == udp_hdr->dst_port.u16) &&
          (address_equals(&s->addr_info.local, &ip6_hdr->dst))) {
        coap_socket_t *sock = find_socket(s->sock.fd, sockets, num_sockets);

        if (sock && (sock->flags & (COAP_SOCKET_WANT_READ))) {
          coap_log(LOG_DEBUG, "fd %d on port %u can read\n",
                   sock->fd, ntohs(get_port(&s->addr_info.local)));
          sock->flags |= COAP_SOCKET_CAN_READ;
          sock->pkt = msg.content.ptr;
          found_port = true;
          break;                /* found session, finish loop */
        }
      }
    }

    /* If no session was found for received packet, traverse all
     * endpoints and set COAP_SOCKET_CAN_READ if the received packet's
     * destination address matches the endpoint. */
    if (!found_port) {
      coap_endpoint_t *ep;
      LL_FOREACH(ctx->endpoint, ep) {
        if ((get_port(&ep->bind_addr) == udp_hdr->dst_port.u16) &&
            (address_equals(&ep->bind_addr, &ip6_hdr->dst))) {
          coap_socket_t *sock = find_socket(ep->sock.fd, sockets, num_sockets);

          if (sock && (sock->flags & (COAP_SOCKET_WANT_READ))) {
              coap_log(LOG_DEBUG, "fd %d on port %u can read\n",
                       sock->fd, ntohs(get_port(&ep->bind_addr)));
              sock->flags |= COAP_SOCKET_CAN_READ;
              sock->pkt = msg.content.ptr;
              found_port = true;
              break;            /* found session, finish loop */
            }
        }
      }
    }
    break;
  }
  case GNRC_NETAPI_MSG_TYPE_SND:
    break;
  case GNRC_NETAPI_MSG_TYPE_SET:
    /* fall through */
  case GNRC_NETAPI_MSG_TYPE_GET:
    break;
  default:
    break;
  }

  coap_ticks(&now);
  coap_io_do_io(ctx, now);

  /* cleanup */
  gnrc_netreg_unregister(GNRC_NETTYPE_UDP, &coap_reg);

  return (int)(((now - before) * 1000) / COAP_TICKS_PER_SECOND);
}
