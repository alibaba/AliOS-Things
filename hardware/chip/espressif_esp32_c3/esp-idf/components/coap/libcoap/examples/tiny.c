/* tiny -- tiny sender
 *
 * Copyright (C) 2010,2011 Olaf Bergmann <bergmann@tzi.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * This file is part of the CoAP library libcoap. Please see
 * README for terms of use.
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <limits.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>

#include <coap3/coap.h>

#define Nn 8  /* duplicate definition of N if built on sky motes */
#define ENCODE_HEADER_SIZE 4
#define HIBIT (1 << (Nn - 1))
#define EMASK ((1 << ENCODE_HEADER_SIZE) - 1)
#define MMASK ((1 << Nn) - 1 - EMASK)
#define MAX_VALUE ( (1 << Nn) - (1 << ENCODE_HEADER_SIZE) ) * (1 << ((1 << ENCODE_HEADER_SIZE) - 1))

#define COAP_PSEUDOFP_DECODE_8_4(r) (r < HIBIT ? r : (r & MMASK) << (r & EMASK))

/* ls and s must be integer variables */
/* #define COAP_PSEUDOFP_ENCODE_8_4_DOWN(v,ls) (v < HIBIT ? v : (ls = coap_fls(v) - Nn, (v >> ls) & MMASK) + ls) */
COAP_STATIC_INLINE unsigned char
COAP_PSEUDOFP_ENCODE_8_4_DOWN(unsigned int v, int *ls) {
  if (v < HIBIT) return v;
  *ls = coap_fls(v) - Nn;
  return ((v >> *ls) & MMASK) + *ls;
}
#define COAP_PSEUDOFP_ENCODE_8_4_UP(v,ls,s) (v < HIBIT ? v : (ls = coap_fls(v) - Nn, (s = (((v + ((1<<ENCODE_HEADER_SIZE<<ls)-1)) >> ls) & MMASK)), s == 0 ? HIBIT + ls + 1 : s + ls))

static int quit = 0;

/* SIGINT handler: set quit to 1 for graceful termination */
static void
handle_sigint(int signum COAP_UNUSED) {
  quit = 1;
}

static coap_pdu_t *
make_pdu( coap_session_t *session, unsigned int value ) {
  coap_pdu_t *pdu;
  unsigned char enc;
  static unsigned char buf[20];
  int len, ls;

  if (!(pdu = coap_pdu_init(COAP_MESSAGE_NON, COAP_REQUEST_CODE_POST,
                            coap_new_message_id(session), COAP_DEFAULT_MTU)))
    return NULL;

  enc = COAP_PSEUDOFP_ENCODE_8_4_DOWN(value, &ls);

  len = sprintf((char *)buf, "%c%u", enc, COAP_PSEUDOFP_DECODE_8_4(enc));
  coap_add_data( pdu, len, buf );

  return pdu;
}

static void
usage( const char *program ) {
  const char *p;

  p = strrchr( program, '/' );
  if ( p )
    program = ++p;

  fprintf( stderr, "%s -- tiny fake sensor\n"
           "(c) 2010 Olaf Bergmann <bergmann@tzi.org>\n\n"
           "usage: %s [group address]\n"
           "\n\nSends some fake sensor values to specified multicast group\n",
           program, program );
}

static coap_session_t *
get_session(coap_context_t *ctx, const char *group) {
  int s;
  struct addrinfo hints;
  struct addrinfo *result, *rp;
  coap_session_t *session;
  int hops = 16;

  if (!ctx)
    return NULL;

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
  hints.ai_socktype = SOCK_DGRAM; /* Coap uses UDP */
  hints.ai_flags = AI_PASSIVE | AI_NUMERICHOST | AI_NUMERICSERV | AI_ALL;

  s = getaddrinfo(group, NULL, &hints, &result);
  if ( s != 0 ) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
    return NULL;
  }

  /* iterate through results until success */
  for (rp = result; rp != NULL; rp = rp->ai_next) {
    coap_address_t addr;
    coap_address_init(&addr);
    addr.size = rp->ai_addrlen;
    memcpy(&addr.addr, rp->ai_addr, rp->ai_addrlen);

    session = coap_new_client_session(ctx, NULL, &addr, COAP_PROTO_UDP);
    if (!session)
      continue;

    if (IN6_IS_ADDR_MULTICAST(&addr.addr.sin6.sin6_addr) ) {
      /* set socket options for multicast */
      if (!coap_mcast_set_hops(session, hops))
        perror("setsockopt: IPV6_MULTICAST_HOPS");

    }
    freeaddrinfo(result);
    return session;
  }

  fprintf(stderr, "no session available for group '%s'\n", group);
  freeaddrinfo(result);
  return NULL;
}

int
main(int argc, char **argv) {
  struct timeval tv;
  coap_pdu_t  *pdu;
  coap_session_t *session;
  struct sigaction sa;
  coap_context_t *ctx;

  if ( argc > 1 && strncmp(argv[1], "-h", 2) == 0 ) {
    usage( argv[0] );
    exit( 1 );
  }

  ctx = coap_new_context(NULL);
  if (!ctx)
    return -1;

  session = get_session(ctx, argc > 1 ? argv[1] : "::1");

  if ( !session )
    return -1;

  memset (&sa, 0, sizeof(sa));
  sigemptyset(&sa.sa_mask);
  sa.sa_handler = handle_sigint;
  sa.sa_flags = 0;
  sigaction (SIGINT, &sa, NULL);
  sigaction (SIGTERM, &sa, NULL);
  /* So we do not exit on a SIGPIPE */
  sa.sa_handler = SIG_IGN;
  sigaction (SIGPIPE, &sa, NULL);

  while ( !quit ) {

    if (! (pdu = make_pdu( session, rand() & 0xfff ) ) )
      break;

    coap_send(session, pdu);

    tv.tv_sec = 5; tv.tv_usec = 0;

    select( 0, 0, 0, 0, &tv );

  }

  coap_free_context(ctx);

  return 0;
}
