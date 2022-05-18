/*
 * coap_tcp.c -- TCP functions for libcoap
 *
 * Copyright (C) 2019 Olaf Bergmann <bergmann@tzi.org> and others
 *
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * This file is part of the CoAP library libcoap. Please see README for terms
 * of use.
 */

#include "coap3/coap_internal.h"

#include <errno.h>
#include <sys/types.h>
#ifdef HAVE_SYS_SOCKET_H
# include <sys/socket.h>
# define OPTVAL_T(t)         (t)
# define OPTVAL_GT(t)        (t)
#endif
#ifdef HAVE_SYS_IOCTL_H
 #include <sys/ioctl.h>
#endif
#ifdef HAVE_WS2TCPIP_H
#include <ws2tcpip.h>
# define OPTVAL_T(t)         (const char*)(t)
# define OPTVAL_GT(t)        (char*)(t)
# undef CMSG_DATA
# define CMSG_DATA WSA_CMSG_DATA
#endif

int
coap_tcp_is_supported(void) {
  return !COAP_DISABLE_TCP;
}

#if !COAP_DISABLE_TCP
int
coap_socket_connect_tcp1(coap_socket_t *sock,
                         const coap_address_t *local_if,
                         const coap_address_t *server,
                         int default_port,
                         coap_address_t *local_addr,
                         coap_address_t *remote_addr) {
  int on = 1;
#ifndef RIOT_VERSION
  int off = 0;
#endif /* RIOT_VERSION */
#ifdef _WIN32
  u_long u_on = 1;
#endif
  coap_address_t connect_addr;
  coap_address_copy( &connect_addr, server );

  sock->flags &= ~COAP_SOCKET_CONNECTED;
  sock->fd = socket(server->addr.sa.sa_family, SOCK_STREAM, 0);

  if (sock->fd == COAP_INVALID_SOCKET) {
    coap_log(LOG_WARNING,
             "coap_socket_connect_tcp1: socket: %s\n",
             coap_socket_strerror());
    goto error;
  }

#ifndef RIOT_VERSION
#ifdef _WIN32
  if (ioctlsocket(sock->fd, FIONBIO, &u_on) == COAP_SOCKET_ERROR) {
#else
  if (ioctl(sock->fd, FIONBIO, &on) == COAP_SOCKET_ERROR) {
#endif
    coap_log(LOG_WARNING,
             "coap_socket_connect_tcp1: ioctl FIONBIO: %s\n",
             coap_socket_strerror());
  }
#endif /* RIOT_VERSION */

  switch (server->addr.sa.sa_family) {
  case AF_INET:
    if (connect_addr.addr.sin.sin_port == 0)
      connect_addr.addr.sin.sin_port = htons(default_port);
    break;
  case AF_INET6:
    if (connect_addr.addr.sin6.sin6_port == 0)
      connect_addr.addr.sin6.sin6_port = htons(default_port);
#ifndef RIOT_VERSION
    /* Configure the socket as dual-stacked */
    if (setsockopt(sock->fd, IPPROTO_IPV6, IPV6_V6ONLY, OPTVAL_T(&off), sizeof(off)) == COAP_SOCKET_ERROR)
      coap_log(LOG_WARNING,
               "coap_socket_connect_tcp1: setsockopt IPV6_V6ONLY: %s\n",
               coap_socket_strerror());
#endif /* RIOT_VERSION */
    break;
  default:
    coap_log(LOG_ALERT, "coap_socket_connect_tcp1: unsupported sa_family\n");
    break;
  }

  if (local_if && local_if->addr.sa.sa_family) {
    coap_address_copy(local_addr, local_if);
    if (setsockopt(sock->fd, SOL_SOCKET, SO_REUSEADDR, OPTVAL_T(&on), sizeof(on)) == COAP_SOCKET_ERROR)
      coap_log(LOG_WARNING,
               "coap_socket_connect_tcp1: setsockopt SO_REUSEADDR: %s\n",
               coap_socket_strerror());
    if (bind(sock->fd, &local_if->addr.sa,
             local_if->addr.sa.sa_family == AF_INET ?
              (socklen_t)sizeof(struct sockaddr_in) :
              (socklen_t)local_if->size) == COAP_SOCKET_ERROR) {
      coap_log(LOG_WARNING, "coap_socket_connect_tcp1: bind: %s\n",
               coap_socket_strerror());
      goto error;
    }
  } else {
    local_addr->addr.sa.sa_family = server->addr.sa.sa_family;
  }

  if (connect(sock->fd, &connect_addr.addr.sa, connect_addr.size) == COAP_SOCKET_ERROR) {
#ifdef _WIN32
    if (WSAGetLastError() == WSAEWOULDBLOCK) {
#else
    if (errno == EINPROGRESS) {
#endif
      /*
       * COAP_SOCKET_CONNECTED needs to be set here as there will be reads/writes
       * by underlying TLS libraries during connect() and we do not want to
       * assert() in coap_read_session() or coap_write_session() when called by coap_read()
       */
      sock->flags |= COAP_SOCKET_WANT_CONNECT | COAP_SOCKET_CONNECTED;
      return 1;
    }
    coap_log(LOG_WARNING, "coap_socket_connect_tcp1: connect: %s\n",
             coap_socket_strerror());
    goto error;
  }

  if (getsockname(sock->fd, &local_addr->addr.sa, &local_addr->size) == COAP_SOCKET_ERROR) {
    coap_log(LOG_WARNING, "coap_socket_connect_tcp1: getsockname: %s\n",
             coap_socket_strerror());
  }

  if (getpeername(sock->fd, &remote_addr->addr.sa, &remote_addr->size) == COAP_SOCKET_ERROR) {
    coap_log(LOG_WARNING, "coap_socket_connect_tcp1: getpeername: %s\n",
             coap_socket_strerror());
  }

  sock->flags |= COAP_SOCKET_CONNECTED;
  return 1;

error:
  coap_socket_close(sock);
  return 0;
}

int
coap_socket_connect_tcp2(coap_socket_t *sock,
                         coap_address_t *local_addr,
                         coap_address_t *remote_addr) {
  int error = 0;
#ifdef _WIN32
  int optlen = (int)sizeof( error );
#else
  socklen_t optlen = (socklen_t)sizeof( error );
#endif

  sock->flags &= ~(COAP_SOCKET_WANT_CONNECT | COAP_SOCKET_CAN_CONNECT);

  if (getsockopt(sock->fd, SOL_SOCKET, SO_ERROR, OPTVAL_GT(&error),
    &optlen) == COAP_SOCKET_ERROR) {
    coap_log(LOG_WARNING, "coap_socket_finish_connect_tcp: getsockopt: %s\n",
      coap_socket_strerror());
  }

  if (error) {
    coap_log(LOG_WARNING,
             "coap_socket_finish_connect_tcp: connect failed: %s\n",
             coap_socket_format_errno(error));
    coap_socket_close(sock);
    return 0;
  }

  if (getsockname(sock->fd, &local_addr->addr.sa, &local_addr->size) == COAP_SOCKET_ERROR) {
    coap_log(LOG_WARNING, "coap_socket_connect_tcp: getsockname: %s\n",
             coap_socket_strerror());
  }

  if (getpeername(sock->fd, &remote_addr->addr.sa, &remote_addr->size) == COAP_SOCKET_ERROR) {
    coap_log(LOG_WARNING, "coap_socket_connect_tcp: getpeername: %s\n",
             coap_socket_strerror());
  }

  return 1;
}

int
coap_socket_bind_tcp(coap_socket_t *sock,
                     const coap_address_t *listen_addr,
                     coap_address_t *bound_addr) {
  int on = 1;
#ifndef RIOT_VERSION
  int off = 0;
#endif /* RIOT_VERSION */
#ifdef _WIN32
  u_long u_on = 1;
#endif

  sock->fd = socket(listen_addr->addr.sa.sa_family, SOCK_STREAM, 0);

  if (sock->fd == COAP_INVALID_SOCKET) {
    coap_log(LOG_WARNING, "coap_socket_bind_tcp: socket: %s\n",
             coap_socket_strerror());
    goto error;
  }

#ifndef RIOT_VERSION
#ifdef _WIN32
  if (ioctlsocket(sock->fd, FIONBIO, &u_on) == COAP_SOCKET_ERROR) {
#else
  if (ioctl(sock->fd, FIONBIO, &on) == COAP_SOCKET_ERROR) {
#endif
    coap_log(LOG_WARNING, "coap_socket_bind_tcp: ioctl FIONBIO: %s\n",
                           coap_socket_strerror());
  }
#endif /* RIOT_VERSION */
  if (setsockopt (sock->fd, SOL_SOCKET, SO_KEEPALIVE, OPTVAL_T(&on),
                  sizeof (on)) == COAP_SOCKET_ERROR)
    coap_log(LOG_WARNING,
             "coap_socket_bind_tcp: setsockopt SO_KEEPALIVE: %s\n",
             coap_socket_strerror());

  if (setsockopt(sock->fd, SOL_SOCKET, SO_REUSEADDR, OPTVAL_T(&on),
                 sizeof(on)) == COAP_SOCKET_ERROR)
    coap_log(LOG_WARNING,
             "coap_socket_bind_tcp: setsockopt SO_REUSEADDR: %s\n",
             coap_socket_strerror());

  switch (listen_addr->addr.sa.sa_family) {
  case AF_INET:
    break;
  case AF_INET6:
#ifndef RIOT_VERSION
    /* Configure the socket as dual-stacked */
    if (setsockopt(sock->fd, IPPROTO_IPV6, IPV6_V6ONLY, OPTVAL_T(&off), sizeof(off)) == COAP_SOCKET_ERROR)
      coap_log(LOG_ALERT,
               "coap_socket_bind_tcp: setsockopt IPV6_V6ONLY: %s\n",
               coap_socket_strerror());
#endif /* RIOT_VERSION */
    break;
  default:
    coap_log(LOG_ALERT, "coap_socket_bind_tcp: unsupported sa_family\n");
  }

  if (bind(sock->fd, &listen_addr->addr.sa,
           listen_addr->addr.sa.sa_family == AF_INET ?
            (socklen_t)sizeof(struct sockaddr_in) :
            (socklen_t)listen_addr->size) == COAP_SOCKET_ERROR) {
    coap_log(LOG_ALERT, "coap_socket_bind_tcp: bind: %s\n",
             coap_socket_strerror());
    goto error;
  }

  bound_addr->size = (socklen_t)sizeof(*bound_addr);
  if (getsockname(sock->fd, &bound_addr->addr.sa, &bound_addr->size) < 0) {
    coap_log(LOG_WARNING, "coap_socket_bind_tcp: getsockname: %s\n",
             coap_socket_strerror());
    goto error;
  }

  if (listen(sock->fd, 5) == COAP_SOCKET_ERROR) {
    coap_log(LOG_ALERT, "coap_socket_bind_tcp: listen: %s\n",
             coap_socket_strerror());
    goto  error;
  }

  return 1;

error:
  coap_socket_close(sock);
  return 0;
}

int
coap_socket_accept_tcp(coap_socket_t *server,
                       coap_socket_t *new_client,
                       coap_address_t *local_addr,
                       coap_address_t *remote_addr) {
#ifndef RIOT_VERSION
#ifdef _WIN32
  u_long u_on = 1;
#else
  int on = 1;
#endif
#endif /* RIOT_VERSION */

  server->flags &= ~COAP_SOCKET_CAN_ACCEPT;

  new_client->fd = accept(server->fd, &remote_addr->addr.sa,
                          &remote_addr->size);
  if (new_client->fd == COAP_INVALID_SOCKET) {
    coap_log(LOG_WARNING, "coap_socket_accept_tcp: accept: %s\n",
             coap_socket_strerror());
    return 0;
  }

  if (getsockname( new_client->fd, &local_addr->addr.sa, &local_addr->size) < 0)
    coap_log(LOG_WARNING, "coap_socket_accept_tcp: getsockname: %s\n",
             coap_socket_strerror());

#ifndef RIOT_VERSION
  #ifdef _WIN32
  if (ioctlsocket(new_client->fd, FIONBIO, &u_on) == COAP_SOCKET_ERROR) {
#else
  if (ioctl(new_client->fd, FIONBIO, &on) == COAP_SOCKET_ERROR) {
#endif
    coap_log(LOG_WARNING, "coap_socket_accept_tcp: ioctl FIONBIO: %s\n",
             coap_socket_strerror());
  }
#endif /* RIOT_VERSION */
  return 1;
}
#endif /* !COAP_DISABLE_TCP */
