/* coap_io_lwip.h -- Network I/O functions for libcoap on lwIP
 *
 * Copyright (C) 2012,2014 Olaf Bergmann <bergmann@tzi.org>
 *               2014 chrysn <chrysn@fsfe.org>
 *
 * This file is part of the CoAP library libcoap. Please see
 * README for terms of use.
 */

#include "coap_config.h"
#include "mem.h"
#include "coap_io.h"
#include <lwip/udp.h>

#if NO_SYS
pthread_mutex_t lwprot_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_t lwprot_thread = (pthread_t)0xDEAD;
int lwprot_count = 0;
#endif

#if 0
void coap_packet_copy_source(coap_packet_t *packet, coap_address_t *target)
{
        target->port = packet->srcport;
        memcpy(&target->addr, ip_current_src_addr(), sizeof(ip_addr_t));
}
#endif
void coap_packet_get_memmapped(coap_packet_t *packet, unsigned char **address, size_t *length)
{
        LWIP_ASSERT("Can only deal with contiguous PBUFs to read the initial details", packet->pbuf->tot_len == packet->pbuf->len);
        *address = packet->pbuf->payload;
        *length = packet->pbuf->tot_len;
}
void coap_free_packet(coap_packet_t *packet)
{
        if (packet->pbuf)
                pbuf_free(packet->pbuf);
        coap_free_type(COAP_PACKET, packet);
}

struct pbuf *coap_packet_extract_pbuf(coap_packet_t *packet)
{
        struct pbuf *ret = packet->pbuf;
        packet->pbuf = NULL;
        return ret;
}


/** Callback from lwIP when a package was received.
 *
 * The current implementation deals this to coap_dispatch immediately, but
 * other mechanisms (as storing the package in a queue and later fetching it
 * when coap_read is called) can be envisioned.
 *
 * It handles everything coap_read does on other implementations.
 */
static void coap_recv(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
  coap_endpoint_t *ep = (coap_endpoint_t*)arg;
  coap_pdu_t *pdu = NULL;
  coap_session_t *session;
  coap_tick_t now;
  coap_packet_t *packet = coap_malloc_type(COAP_PACKET, sizeof(coap_packet_t));

  /* this is fatal because due to the short life of the packet, never should there be more than one coap_packet_t required */
  LWIP_ASSERT("Insufficient coap_packet_t resources.", packet != NULL);
  packet->pbuf = p;
  packet->src.port = port;
  packet->src.addr = *addr;
  packet->dst.port = upcb->local_port;
  packet->dst.addr = *ip_current_dest_addr();
  packet->ifindex = netif_get_index(ip_current_netif());

  pdu = coap_pdu_from_pbuf(p);
  if (!pdu)
    goto error;

  if (!coap_pdu_parse(ep->proto, p->payload, p->len, pdu)) {
    goto error;
  }

  coap_ticks(&now);
  session = coap_endpoint_get_session(ep, packet, now);
  if (!session)
    goto error;
  LWIP_ASSERT("Proto not supported for LWIP", COAP_PROTO_NOT_RELIABLE(session->proto));
  coap_dispatch(ep->context, session, pdu);

  coap_delete_pdu(pdu);
  packet->pbuf = NULL;
  coap_free_packet(packet);
  return;

error:
  /* FIXME: send back RST? */
  if (pdu) coap_delete_pdu(pdu);
  if (packet) {
    packet->pbuf = NULL;
    coap_free_packet(packet);
  }
  return;
}

coap_endpoint_t *
coap_new_endpoint(coap_context_t *context, const coap_address_t *addr, coap_proto_t proto) {
        coap_endpoint_t *result;
        err_t err;

        LWIP_ASSERT("Proto not supported for LWIP endpoints", proto == COAP_PROTO_UDP);

        result = coap_malloc_type(COAP_ENDPOINT, sizeof(coap_endpoint_t));
        if (!result) return NULL;

        result->sock.pcb = udp_new_ip_type(IPADDR_TYPE_ANY);
        if (result->sock.pcb == NULL) goto error;

        udp_recv(result->sock.pcb, coap_recv, (void*)result);
        err = udp_bind(result->sock.pcb, &addr->addr, addr->port);
        if (err) {
                udp_remove(result->sock.pcb);
                goto error;
        }

        result->default_mtu = COAP_DEFAULT_MTU;
        result->context = context;
        result->proto = proto;

        return result;

error:
        coap_free_type(COAP_ENDPOINT, result);
        return NULL;
}

void coap_free_endpoint(coap_endpoint_t *ep)
{
        udp_remove(ep->sock.pcb);
        coap_free_type(COAP_ENDPOINT, ep);
}

ssize_t
coap_socket_send_pdu(coap_socket_t *sock, coap_session_t *session,
  coap_pdu_t *pdu) {
  /* FIXME: we can't check this here with the existing infrastructure, but we
  * should actually check that the pdu is not held by anyone but us. the
  * respective pbuf is already exclusively owned by the pdu. */

  pbuf_realloc(pdu->pbuf, pdu->used_size + coap_pdu_parse_header_size(session->proto, pdu->pbuf->payload));
  udp_sendto(sock->pcb, pdu->pbuf, &session->remote_addr.addr,
    session->remote_addr.port);
  return pdu->used_size;
}

ssize_t
coap_socket_send(coap_socket_t *sock, coap_session_t *session,
  const uint8_t *data, size_t data_len ) {
  /* Not implemented, use coap_socket_send_pdu instead */
  return -1;
}

int
coap_socket_bind_udp(coap_socket_t *sock,
  const coap_address_t *listen_addr,
  coap_address_t *bound_addr) {
  return 0;
}

int
coap_socket_connect_udp(coap_socket_t *sock,
  const coap_address_t *local_if,
  const coap_address_t *server,
  int default_port,
  coap_address_t *local_addr,
  coap_address_t *remote_addr) {
  return 0;
}

int
coap_socket_connect_tcp1(coap_socket_t *sock,
                         const coap_address_t *local_if,
                         const coap_address_t *server,
                         int default_port,
                         coap_address_t *local_addr,
                         coap_address_t *remote_addr) {
  return 0;
}

int
coap_socket_connect_tcp2(coap_socket_t *sock,
                         coap_address_t *local_addr,
                         coap_address_t *remote_addr) {
  return 0;
}

int
coap_socket_bind_tcp(coap_socket_t *sock,
                     const coap_address_t *listen_addr,
                     coap_address_t *bound_addr) {
  return 0;
}

int
coap_socket_accept_tcp(coap_socket_t *server,
                        coap_socket_t *new_client,
                        coap_address_t *local_addr,
                        coap_address_t *remote_addr) {
  return 0;
}

ssize_t
coap_socket_write(coap_socket_t *sock, const uint8_t *data, size_t data_len) {
  return -1;
}

ssize_t
coap_socket_read(coap_socket_t *sock, uint8_t *data, size_t data_len) {
  return -1;
}

void coap_socket_close(coap_socket_t *sock) {
  return;
}

