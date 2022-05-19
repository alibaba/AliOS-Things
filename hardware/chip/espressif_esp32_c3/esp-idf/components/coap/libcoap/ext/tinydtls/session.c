/*******************************************************************************
 *
 * Copyright (c) 2011, 2012, 2013, 2014, 2015 Olaf Bergmann (TZI) and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v. 1.0 which accompanies this distribution.
 *
 * The Eclipse Public License is available at http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at 
 * http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Olaf Bergmann  - initial API and implementation
 *
 *******************************************************************************/

#include "session.h"

#ifdef HAVE_ASSERT_H
#include <assert.h>
#else
#ifndef assert
#warning "assertions are disabled"
#  define assert(x)
#endif
#endif

#if defined(WITH_CONTIKI)
#define _dtls_address_equals_impl(A,B)				\
  ((A)->size == (B)->size					\
   && (A)->port == (B)->port					\
   && uip_ipaddr_cmp(&((A)->addr),&((B)->addr))			\
   && (A)->ifindex == (B)->ifindex)
#elif defined(WITH_RIOT_GNRC)
#define _dtls_address_equals_impl(A,B)                          \
  ((A)->size == (B)->size                                       \
   && (A)->port == (B)->port                                    \
   && ipv6_addr_equal(&((A)->addr),&((B)->addr))                \
   && (A)->ifindex == (B)->ifindex)
#else /* WITH_CONTIKI */

static inline int 
_dtls_address_equals_impl(const session_t *a,
			  const session_t *b) {
  if (a->ifindex != b->ifindex ||
      a->size != b->size || a->addr.sa.sa_family != b->addr.sa.sa_family)
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
#endif /* WITH_CONTIKI */

void
dtls_session_init(session_t *sess) {
  assert(sess);
  memset(sess, 0, sizeof(session_t));
  sess->size = sizeof(sess->addr);
}

/* These functions are primarly needed for the tinydtls ruby gem.
 *
 * They are not implemented on Contiki and RIOT because these operating
 * system don't supply malloc(3). This could be fixed by fixed by using
 * memory pools on these operating system as in `peer.c`. However, the
 * downside of this approach is that the memory pools reserve memory
 * even if the `dtls_new_session` isn't used and usually memory for the
 * `session_t` type is already resevered in the `peer_t` struct.
 * Therefore it would introduces quite some overhead on these
 * constrained platforms.
 *
 * In the long run we probably want to create two seperate memory pools
 * for sessions and peers and store a pointer to a session in the peer
 * struct.
 */
#if !(defined (WITH_CONTIKI)) && !(defined (RIOT_VERSION))
session_t*
dtls_new_session(struct sockaddr *addr, socklen_t addrlen) {
  session_t *sess;

  sess = malloc(sizeof(session_t));
  if (!sess)
    return NULL;
  dtls_session_init(sess);

  sess->size = addrlen;
  memcpy(&sess->addr.sa, addr, sess->size);

  return sess;
}

void
dtls_free_session(session_t *sess) {
  free(sess);
}

struct sockaddr*
dtls_session_addr(session_t *sess, socklen_t *addrlen) {
  if (!sess)
    return NULL;

  *addrlen = sess->size;
  return &sess->addr.sa;
}
#endif /* !(defined (WITH_CONTIKI)) && !(defined (RIOT_VERSION)) */

int
dtls_session_equals(const session_t *a, const session_t *b) {
  assert(a); assert(b);
  return _dtls_address_equals_impl(a, b);
}
