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
 *    Hauke Mehrtens - memory optimization, ECC integration
 *
 *******************************************************************************/

#include "global.h"
#include "peer.h"
#include "dtls_debug.h"

#if !(defined (WITH_CONTIKI)) && !(defined (RIOT_VERSION))
void peer_init(void)
{
}

static inline dtls_peer_t *
dtls_malloc_peer(void) {
  return (dtls_peer_t *)malloc(sizeof(dtls_peer_t));
}

void
dtls_free_peer(dtls_peer_t *peer) {
  dtls_handshake_free(peer->handshake_params);
  dtls_security_free(peer->security_params[0]);
  dtls_security_free(peer->security_params[1]);
  free(peer);
}
#elif defined (WITH_CONTIKI) /* WITH_CONTIKI */

#include "memb.h"
MEMB(peer_storage, dtls_peer_t, DTLS_PEER_MAX);

void
peer_init(void) {
  memb_init(&peer_storage);
}

static inline dtls_peer_t *
dtls_malloc_peer(void) {
  return memb_alloc(&peer_storage);
}

void
dtls_free_peer(dtls_peer_t *peer) {
  dtls_handshake_free(peer->handshake_params);
  dtls_security_free(peer->security_params[0]);
  dtls_security_free(peer->security_params[1]);
  memb_free(&peer_storage, peer);
}

#elif defined (RIOT_VERSION)
# include <memarray.h>

dtls_peer_t peer_storage_data[DTLS_PEER_MAX];
memarray_t peer_storage;

void
peer_init(void) {
  memarray_init(&peer_storage, peer_storage_data, sizeof(dtls_peer_t), DTLS_PEER_MAX);
}

static inline dtls_peer_t *
dtls_malloc_peer(void) {
  return memarray_alloc(&peer_storage);
}

void
dtls_free_peer(dtls_peer_t *peer) {
  dtls_handshake_free(peer->handshake_params);
  dtls_security_free(peer->security_params[0]);
  dtls_security_free(peer->security_params[1]);
  memarray_free(&peer_storage, peer);
}

#endif /* WITH_CONTIKI */

dtls_peer_t *
dtls_new_peer(const session_t *session) {
  dtls_peer_t *peer;

  peer = dtls_malloc_peer();
  if (peer) {
    memset(peer, 0, sizeof(dtls_peer_t));
    memcpy(&peer->session, session, sizeof(session_t));
    peer->security_params[0] = dtls_security_new();

    if (!peer->security_params[0]) {
      dtls_free_peer(peer);
      return NULL;
    }

    dtls_dsrv_log_addr(DTLS_LOG_DEBUG, "dtls_new_peer", session);
  }

  return peer;
}
