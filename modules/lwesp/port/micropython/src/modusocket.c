/*
 * =====================================================================================
 *
 *       Filename:  modsocket.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/11/2022 08:42:07 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "py/runtime0.h"
#include "py/nlr.h"
#include "py/objlist.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/stream.h"
#include "py/mperrno.h"
#include "shared/netutils/netutils.h"

#include "lwesp/lwesp.h"
#define SOCKET_POLL_US (100000)
#define MDNS_QUERY_TIMEOUT_MS (5000)
#define MDNS_LOCAL_SUFFIX ".local"

#define SOCK_STREAM     1
#define SOCK_DGRAM      2
#define SOCK_RAW        3

#define AF_UNSPEC       0
#define AF_INET         2
#define AF_PACKET       17
#define PF_INET         AF_INET
#define PF_UNSPEC       AF_UNSPEC
#define PF_PACKET       AF_PACKET

#define IPPROTO_IP      0
#define IPPROTO_ICMP    1
#define IPPROTO_TCP     6
#define IPPROTO_UDP     17

#define  SOL_SOCKET  0xfff    /* options for socket level */

#define IP_ADD_MEMBERSHIP 0x400
#define SO_REUSEADDR   0x0004
struct lwesp_addrinfo {
    int               family;     /* Address family of socket. */
    int               socktype;   /* Socket type. */
    int               protocol;   /* Protocol of socket. */
    lwesp_ip_t        ip;
    uint16_t port;
};

static inline int lwesp2mperr(lwespr_t ret)
{
    switch (ret) {
    case lwespOK:
        return 0;
    case lwespERR:
        return -MP_EIO;
    case lwespPARERR:
        return -MP_EINVAL;
    case lwespERRMEM:
        return -MP_ENOMEM;
    case lwespTIMEOUT:
        return -MP_ETIMEDOUT;
    case lwespCLOSED:
        return -MP_ENOENT;
    case lwespINPROG:
        return -MP_EINPROGRESS;
    case lwespERRNODEVICE:
        return -MP_ENXIO;
    case lwespEAGAIN:
        return -MP_EAGAIN; 
    default:
        return -1;
    }

    return -1;
}


char* lwesp_ip4addr_ntoa_r(const unsigned char *addr, char *buf, int buflen)
{
  uint32_t s_addr;
  char inv[3];
  char *rp;
  const uint8_t *ap;
  uint8_t rem;
  uint8_t n;
  uint8_t i;
  int len = 0;
  uint8_t tmp;

  rp = buf;
  ap = addr;
  for (n = 0; n < 4; n++) {
    i = 0;
    tmp = *ap;
    do {
      rem = tmp % (uint8_t)10;
      tmp /= (uint8_t)10;
      inv[i++] = '0' + rem;
    } while (tmp);
    while (i--) {
      if (len++ >= buflen) {
        return NULL;
      }
      *rp++ = inv[i];
    }
    if (len++ >= buflen) {
      return NULL;
    }
    *rp++ = '.';
    ap++;
  }
  *--rp = 0;
  return buf;
}
#define lwesp_in_range(c, lo, up)  ((uint8_t)(c) >= (lo) && (uint8_t)(c) <= (up))
#define lwesp_isdigit(c)           lwesp_in_range((c), '0', '9')
#define lwesp_isxdigit(c)          (lwesp_isdigit(c) || lwesp_in_range((c), 'a', 'f') || lwesp_in_range((c), 'A', 'F'))
#define lwesp_islower(c)           lwesp_in_range((c), 'a', 'z')
#define lwesp_isspace(c)           ((c) == ' ' || (c) == '\f' || (c) == '\n' || (c) == '\r' || (c) == '\t' || (c) == '\v')
#define lwesp_htonl(x) ((((x) & (uint32_t)0x000000ffUL) << 24) | \
                     (((x) & (uint32_t)0x0000ff00UL) <<  8) | \
                     (((x) & (uint32_t)0x00ff0000UL) >>  8) | \
                     (((x) & (uint32_t)0xff000000UL) >> 24))

int lwesp_ip4addr_aton_r(const char *cp,  uint32_t*addr)
{
  uint32_t val;
  uint8_t base;
  char c;
  uint32_t parts[4];
  uint32_t *pp = parts;

  c = *cp;
  for (;;) {
    /*
     * Collect number up to ``.''.
     * Values are specified as for C:
     * 0x=hex, 0=octal, 1-9=decimal.
     */
    if (!lwesp_isdigit(c)) {
      return 0;
    }
    val = 0;
    base = 10;
    if (c == '0') {
      c = *++cp;
      if (c == 'x' || c == 'X') {
        base = 16;
        c = *++cp;
      } else {
        base = 8;
      }
    }
    for (;;) {
      if (lwesp_isdigit(c)) {
        val = (val * base) + (uint32_t)(c - '0');
        c = *++cp;
      } else if (base == 16 && lwesp_isxdigit(c)) {
        val = (val << 4) | (uint32_t)(c + 10 - (lwesp_islower(c) ? 'a' : 'A'));
        c = *++cp;
      } else {
        break;
      }
    }
    if (c == '.') {
      /*
       * Internet format:
       *  a.b.c.d
       *  a.b.c   (with c treated as 16 bits)
       *  a.b (with b treated as 24 bits)
       */
      if (pp >= parts + 3) {
        return 0;
      }
      *pp++ = val;
      c = *++cp;
    } else {
      break;
    }
  }
  /*
   * Check for trailing characters.
   */
  if (c != '\0' && !lwesp_isspace(c)) {
    return 0;
  }
  /*
   * Concoct the address according to
   * the number of parts specified.
   */
  switch (pp - parts + 1) {

    case 0:
      return 0;       /* initial nondigit */

    case 1:             /* a -- 32 bits */
      break;

    case 2:             /* a.b -- 8.24 bits */
      if (val > 0xffffffUL) {
        return 0;
      }
      if (parts[0] > 0xff) {
        return 0;
      }
      val |= parts[0] << 24;
      break;

    case 3:             /* a.b.c -- 8.8.16 bits */
      if (val > 0xffff) {
        return 0;
      }
      if ((parts[0] > 0xff) || (parts[1] > 0xff)) {
        return 0;
      }
      val |= (parts[0] << 24) | (parts[1] << 16);
      break;

    case 4:             /* a.b.c.d -- 8.8.8.8 bits */
      if (val > 0xff) {
        return 0;
      }
      if ((parts[0] > 0xff) || (parts[1] > 0xff) || (parts[2] > 0xff)) {
        return 0;
      }
      val |= (parts[0] << 24) | (parts[1] << 16) | (parts[2] << 8);
      break;
    default:
      break;
  }
  if (addr) {
      *addr = lwesp_htonl(val);
  }
  return 1;
}


enum {
    SOCKET_STATE_NEW,
    SOCKET_STATE_CONNECTED,
    SOCKET_STATE_PEER_CLOSED,
    SOCKET_STATE_LISTENING,
};

typedef struct _socket_obj_t {
    mp_obj_base_t base;
    lwesp_netconn_p nc;
    uint8_t domain;
    uint8_t type;
    uint8_t proto;
    uint8_t state;
    uint16_t local_port;
    unsigned int retries;
    void *lastdata;
    uint16_t lastoffset;
    #if MICROPY_PY_USOCKET_EVENTS
    mp_obj_t events_callback;
    struct _socket_obj_t *events_next;
    #endif
} socket_obj_t;

#if MICROPY_PY_USOCKET_EVENTS
// Support for callbacks on asynchronous socket events (when socket becomes readable)

// This divisor is used to reduce the load on the system, so it doesn't poll sockets too often
#define USOCKET_EVENTS_DIVISOR (8)

STATIC uint8_t usocket_events_divisor;
STATIC socket_obj_t *usocket_events_head;

void usocket_events_deinit(void) {
    usocket_events_head = NULL;
}

// Assumes the socket is not already in the linked list, and adds it
STATIC void usocket_events_add(socket_obj_t *sock) {
    sock->events_next = usocket_events_head;
    usocket_events_head = sock;
}

// Assumes the socket is already in the linked list, and removes it
STATIC void usocket_events_remove(socket_obj_t *sock) {
    for (socket_obj_t **s = &usocket_events_head;; s = &(*s)->events_next) {
        if (*s == sock) {
            *s = (*s)->events_next;
            return;
        }
    }
}

// Polls all registered sockets for readability and calls their callback if they are readable
void usocket_events_handler(void) {
}

#endif // MICROPY_PY_USOCKET_EVENTS

static inline void check_for_exceptions(void) {
    mp_handle_pending(true);
}

static int _socket_getaddrinfo2(const mp_obj_t host, const mp_obj_t portx, struct lwesp_addrinfo *resp) {
    const char *host_str = mp_obj_str_get_str(host);

    if (host_str[0] == '\0') {
        // a host of "" is equivalent to the default/all-local IP address
        host_str = "0.0.0.0";
    }

    MP_THREAD_GIL_EXIT();
    int res = lwesp_dns_gethostbyname(host_str, &(resp->ip), NULL, NULL, 1); 
    MP_THREAD_GIL_ENTER();
    
    if (res != 0) {
        mp_raise_OSError(res > 0 ? -res : res);
    } else {
        resp->family = AF_INET; 
        resp->socktype = SOCK_STREAM;
        resp->protocol = 0;
        if (mp_obj_is_small_int(portx)) {
            resp->port = MP_OBJ_SMALL_INT_VALUE(portx);     
        }
    }
    return res;
}

STATIC mp_obj_t socket_make_new(const mp_obj_type_t *type_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 3, false);

    socket_obj_t *sock = m_new_obj_with_finaliser(socket_obj_t);
    sock->base.type = type_in;
    sock->domain = AF_INET;
    sock->type = SOCK_STREAM;
    sock->proto = 0;
    sock->local_port = 0;
    if (n_args > 0) {
        sock->domain = mp_obj_get_int(args[0]);
        if (n_args > 1) {
            sock->type = mp_obj_get_int(args[1]);
            if (n_args > 2) {
                sock->proto = mp_obj_get_int(args[2]);
            }
        }
    }

    sock->state = SOCKET_STATE_NEW;
    LWESP_DEBUGF(LWESP_CFG_DBG_INIT | LWESP_DBG_LVL_SEVERE | LWESP_DBG_TYPE_TRACE,
        "%s:%d\r\n", __func__, __LINE__);
 
    if (sock->type == SOCK_STREAM) {
        MP_THREAD_GIL_EXIT();
        sock->nc = lwesp_netconn_new(LWESP_NETCONN_TYPE_TCP);
        MP_THREAD_GIL_ENTER();
        if (sock->nc == NULL) {
            mp_raise_ValueError(MP_ERROR_TEXT("lwesp_netconn_new tcp failed"));
        }
    } else if (sock->type == SOCK_DGRAM) {
        MP_THREAD_GIL_EXIT();
        sock->nc = lwesp_netconn_new(LWESP_NETCONN_TYPE_UDP);
        MP_THREAD_GIL_ENTER();
        if (sock->nc == NULL) {
            mp_raise_ValueError(MP_ERROR_TEXT("lwesp_netconn_new udp failed"));
        }
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("not supported"));
    }

    return MP_OBJ_FROM_PTR(sock);
}

STATIC mp_obj_t socket_bind(const mp_obj_t arg0, const mp_obj_t arg1) {
    socket_obj_t *self = MP_OBJ_TO_PTR(arg0);
    lwesp_ip_t ip;
    int r = lwespOK;
    const lwesp_port_t port = (lwesp_port_t)netutils_parse_inet_addr(arg1, (uint8_t *)ip.addr.ip4.addr, NETUTILS_BIG);
    LWESP_DEBUGF(LWESP_CFG_DBG_INIT | LWESP_DBG_LVL_SEVERE | LWESP_DBG_TYPE_TRACE,
        "bind addr:%d.%d.%d.%d port:%d\r\n", ip.addr.ip4.addr[0], ip.addr.ip4.addr[1], ip.addr.ip4.addr[2],
        ip.addr.ip4.addr[3], port);

    if (self->type == SOCK_STREAM) {
        MP_THREAD_GIL_EXIT();
        r = lwesp_netconn_bind(self->nc, port);
        MP_THREAD_GIL_ENTER();
        self->local_port = port;
    } else if (self->type == SOCK_DGRAM){
        self->local_port = port; 
    } else {
        r = lwespPARERR;
    }
    if (r != lwespOK) {
        mp_raise_OSError(-lwesp2mperr(r));  
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(socket_bind_obj, socket_bind);

STATIC mp_obj_t socket_listen(const mp_obj_t arg0, const mp_obj_t arg1) {
    socket_obj_t *self = MP_OBJ_TO_PTR(arg0);
    int backlog = mp_obj_get_int(arg1);
    MP_THREAD_GIL_EXIT();
    int r =  lwesp_netconn_listen_with_max_conn(self->nc, backlog);
    MP_THREAD_GIL_ENTER();
    if (r != lwespOK) {
        mp_raise_OSError(-lwesp2mperr(r));  
    }
    self->state = SOCKET_STATE_LISTENING;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(socket_listen_obj, socket_listen);

STATIC mp_obj_t socket_accept(const mp_obj_t arg0) {
    socket_obj_t *self = MP_OBJ_TO_PTR(arg0);
    lwesp_netconn_p nc; 

    lwespr_t ret;
    MP_THREAD_GIL_EXIT();
    ret = lwesp_netconn_accept(self->nc, &nc);
    MP_THREAD_GIL_ENTER();

    if (ret != lwespOK) {
        mp_raise_OSError(-lwesp2mperr(ret));
    }

    // create new socket object
    socket_obj_t *sock = m_new_obj_with_finaliser(socket_obj_t);
    sock->base.type = self->base.type;
    sock->nc = nc;
    sock->domain = self->domain;
    sock->type = self->type;
    sock->proto = self->proto;
    sock->state = SOCKET_STATE_CONNECTED;

    // make the return value
    uint8_t *ip = NULL;
    mp_uint_t port = 0;
    mp_obj_tuple_t *client = mp_obj_new_tuple(2, NULL);
    client->items[0] = sock;
    // client->items[1] = netutils_format_inet_addr(ip, port, NETUTILS_BIG);

    return client;

}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(socket_accept_obj, socket_accept);

STATIC mp_obj_t socket_connect(const mp_obj_t arg0, const mp_obj_t arg1) {
    socket_obj_t *self = MP_OBJ_TO_PTR(arg0);
    mp_obj_t *elem;
    mp_obj_t mp_host;
    mp_obj_t mp_port;
    
    mp_obj_get_array_fixed_n(arg1, 2, &elem);
    mp_host = elem[0];
    mp_port = elem[1];
    int r = lwespOK;  
    const char *host = mp_obj_str_get_str(mp_host);
    const lwesp_port_t port = (lwesp_port_t)MP_OBJ_SMALL_INT_VALUE(mp_port);

    LWESP_DEBUGF(LWESP_CFG_DBG_INIT | LWESP_DBG_LVL_SEVERE | LWESP_DBG_TYPE_TRACE,
        "connecting host:%s port:%d\r\n", host, port);
    
    if (self->local_port > 0) { 
        MP_THREAD_GIL_EXIT();
        r = lwesp_netconn_connect_ex(self->nc, host, port, 0, NULL, self->local_port, 0);
        MP_THREAD_GIL_ENTER();
    } else {
         MP_THREAD_GIL_EXIT();
         r = lwesp_netconn_connect(self->nc, host, port);
         MP_THREAD_GIL_ENTER();
    }

    if (r != lwespOK) {
        LWESP_DEBUGF(LWESP_CFG_DBG_INIT | LWESP_DBG_LVL_SEVERE | LWESP_DBG_TYPE_TRACE,
    "socket_connect res:%d\r\n", r);
        mp_raise_OSError(-lwesp2mperr(r));
    }

    self->state = SOCKET_STATE_CONNECTED;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(socket_connect_obj, socket_connect);

STATIC mp_obj_t socket_setsockopt(size_t n_args, const mp_obj_t *args) {
    (void)n_args; // always 4
    socket_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(socket_setsockopt_obj, 4, 4, socket_setsockopt);

STATIC mp_obj_t socket_settimeout(const mp_obj_t arg0, const mp_obj_t arg1) {
    socket_obj_t *self = MP_OBJ_TO_PTR(arg0);
    uint32_t timeout;
    if (arg1 == mp_const_none) {
         timeout = 0; 
    } else {
         timeout= mp_obj_get_int(arg1) * 1000;
    }
#if LWESP_CFG_NETCONN_RECEIVE_TIMEOUT || __DOXYGEN__
    lwesp_netconn_set_receive_timeout(self->nc, timeout);
#endif

    lwesp_netconn_set_accept_timeout(self->nc, timeout);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(socket_settimeout_obj, socket_settimeout);

STATIC mp_obj_t socket_setblocking(const mp_obj_t arg0, const mp_obj_t arg1) {
    socket_obj_t *self = MP_OBJ_TO_PTR(arg0);
    uint32_t timeout;
    
    if (mp_obj_is_true(arg1)) {
        timeout = 0; 
    } else {
        timeout = LWESP_NETCONN_RECEIVE_NO_WAIT; 
    }
#if LWESP_CFG_NETCONN_RECEIVE_TIMEOUT || __DOXYGEN__
    lwesp_netconn_set_receive_timeout(self->nc, timeout);
#endif
    lwesp_netconn_set_accept_timeout(self->nc, timeout);
    lwesp_netconn_set_connect_timeout(self->nc, timeout);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(socket_setblocking_obj, socket_setblocking);

// XXX this can end up waiting a very long time if the content is dribbled in one character
// at a time, as the timeout resets each time a recvfrom succeeds ... this is probably not
// good behaviour.
STATIC mp_uint_t _socket_read_data(mp_obj_t self_in, void *mem, size_t size,
    void *from, void *from_len, int *errcode) {
    socket_obj_t *sock = MP_OBJ_TO_PTR(self_in);
    lwesp_pbuf_p pbuf;
    void *buf = NULL;
    lwesp_pbuf_p p;
    uint8_t done = 0;
    uint16_t buflen, copylen;
    int off = 0;
    // A new socket cannot be read from.
    if (sock->state == SOCKET_STATE_NEW) {
        *errcode = MP_ENOTCONN;
        return MP_STREAM_ERROR;
    }

    // If the peer closed the connection then the lwIP socket API will only return "0" once
    // from lwip_recvfrom and then block on subsequent calls.  To emulate POSIX behaviour,
    // which continues to return "0" for each call on a closed socket, we set a flag when
    // the peer closed the socket.
    if (sock->state == SOCKET_STATE_PEER_CLOSED) {
        return 0;
    }
    if (sock->type == SOCK_STREAM) {
        do {
            if (sock->lastdata) {
                
                buf = sock->lastdata;
                LWESP_DEBUGF(LWESP_CFG_DBG_INIT | LWESP_DBG_LVL_SEVERE | LWESP_DBG_TYPE_TRACE,
    "_socket_read_data buf from lastdata:%p\r\n", buf);

            } else {
                MP_THREAD_GIL_EXIT();
                int r = lwesp_netconn_receive(sock->nc, &pbuf);
                MP_THREAD_GIL_ENTER();
                if (r == lwespCLOSED) {
                    sock->state = SOCKET_STATE_PEER_CLOSED;
                    return 0;
                } else if (r == lwespOK && pbuf != NULL) {
                    buf = (void *)pbuf;
                    LWESP_DEBUGF(LWESP_CFG_DBG_INIT | LWESP_DBG_LVL_SEVERE | LWESP_DBG_TYPE_TRACE,
    "_socket_read_data buf from lwesp_netconn_receive:%p\r\n", buf);

                } else {
                     if (off > 0) {
                        if (errcode != NULL) {
                            *errcode = 0;
                        }
                        return off;
                     } else {
                        if (errcode != NULL) {
                            *errcode = -lwesp2mperr(r);
                        }
                        return MP_STREAM_ERROR;
                     }
                }
            }

            p = (lwesp_pbuf_p)buf;
            buflen = (size_t)lwesp_pbuf_length(p, 1);
            buflen -= sock->lastoffset;
            if (size > buflen) {
                copylen = buflen;
            } else {
                copylen = (uint16_t)size;
            }
            lwesp_pbuf_copy(p, (uint8_t *)mem + off, copylen, sock->lastoffset);
            off += copylen;
            
            size -= copylen;
            if (size <= 0) {
                done = 1;
            }
            if (buflen - copylen > 0) {
                LWESP_DEBUGF(LWESP_CFG_DBG_INIT | LWESP_DBG_LVL_SEVERE | LWESP_DBG_TYPE_TRACE,
    "_socket_read_data buf to lastdata:%p\r\n", buf);

                sock->lastdata  = buf;
                sock->lastoffset += copylen;
            } else {
                LWESP_DEBUGF(LWESP_CFG_DBG_INIT | LWESP_DBG_LVL_SEVERE | LWESP_DBG_TYPE_TRACE,
    "_socket_read_data null to lastdata\r\n");

                sock->lastdata = NULL;
                sock->lastoffset = 0;
                lwesp_pbuf_free((lwesp_pbuf_p)buf);
                buf = NULL;
            }
        } while(!done);
    } else if (sock->type == SOCK_DGRAM) {
        MP_THREAD_GIL_EXIT();
        int r = lwesp_netconn_receive(sock->nc, &pbuf);
        MP_THREAD_GIL_ENTER();
        if (r == lwespCLOSED) {
            sock->state = SOCKET_STATE_PEER_CLOSED;
            return 0;
        } else if (r == lwespOK && pbuf != NULL) {
            buf = (void *)pbuf;
        } else {
            if (errcode != NULL) {
                *errcode = -lwesp2mperr(r); 
            }
            return MP_STREAM_ERROR;
        }
        p = (lwesp_pbuf_p)buf;
        buflen = (size_t)lwesp_pbuf_length(pbuf, 1);
        if (size > buflen) {
            copylen = buflen;
        } else {
            copylen = (uint16_t)size;
        }
        lwesp_pbuf_copy(p, (uint8_t *)mem + off, copylen, sock->lastoffset);
        lwesp_pbuf_free((lwesp_pbuf_p)buf);     
        off += copylen;
    } else {
        off = -MP_EAFNOSUPPORT; 
    }
    return off;
}

mp_obj_t _socket_recvfrom(mp_obj_t self_in, mp_obj_t len_in,
    void *from, void *from_len) {
    size_t len = mp_obj_get_int(len_in);
    vstr_t vstr;
    vstr_init_len(&vstr, len);
    int errcode;
   
    mp_uint_t ret = _socket_read_data(self_in, vstr.buf, len, NULL, NULL, &errcode);
    
    if (ret == MP_STREAM_ERROR) {
        mp_raise_OSError(errcode);
    }

    vstr.len = ret;
    return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
}

STATIC mp_obj_t socket_recv(mp_obj_t self_in, mp_obj_t len_in) {
    return _socket_recvfrom(self_in, len_in, NULL, NULL);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(socket_recv_obj, socket_recv);

STATIC mp_obj_t socket_recvfrom(mp_obj_t self_in, mp_obj_t len_in) {

    mp_obj_t tuple[2];
    tuple[0] = _socket_recvfrom(self_in, len_in, NULL, NULL);

    return mp_obj_new_tuple(2, tuple);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(socket_recvfrom_obj, socket_recvfrom);

int _socket_send(socket_obj_t *sock, const char *data, size_t datalen) {
    int sentlen = 0;
    
    if (sock->type == SOCK_STREAM) {
        MP_THREAD_GIL_EXIT();
        int r = lwesp_netconn_write(sock->nc, data, datalen);
        MP_THREAD_GIL_ENTER();
        if (r == lwespOK) {
            MP_THREAD_GIL_EXIT();
            r = lwesp_netconn_flush(sock->nc);
            MP_THREAD_GIL_ENTER();
            if (r == lwespOK) {
                r = datalen - sentlen;
            } else {
                r = lwesp2mperr(r);
            }
        } else {
            r = lwesp2mperr(r);
        }
            // lwip returns EINPROGRESS when trying to send right after a non-blocking connect
        if (r >= 0) {
            sentlen += r;
        } else {
            return r;
        }   
    } else if (sock->type == SOCK_DGRAM) { 
        int r = lwesp_netconn_send(sock->nc, data, datalen);
        if (r == lwespOK) {
            sentlen += datalen;    
        } else {
            return lwesp2mperr(r); 
        }
    } else {
        sentlen = -MP_EAFNOSUPPORT; 
    }
    return sentlen;
}

STATIC mp_obj_t socket_send(const mp_obj_t arg0, const mp_obj_t arg1) {
    socket_obj_t *sock = MP_OBJ_TO_PTR(arg0);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(arg1, &bufinfo, MP_BUFFER_READ);
    int r = _socket_send(sock, bufinfo.buf, bufinfo.len);
    if (r < 0) {
        mp_raise_OSError(-r);
    }
    return mp_obj_new_int(r);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(socket_send_obj, socket_send);

STATIC mp_obj_t socket_sendall(const mp_obj_t arg0, const mp_obj_t arg1) {
    // XXX behaviour when nonblocking (see extmod/modlwip.c)
    // XXX also timeout behaviour.
    socket_obj_t *sock = MP_OBJ_TO_PTR(arg0);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(arg1, &bufinfo, MP_BUFFER_READ);
    int r = _socket_send(sock, bufinfo.buf, bufinfo.len);
    if (r < bufinfo.len) {
        mp_raise_OSError(-r);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(socket_sendall_obj, socket_sendall);

STATIC mp_obj_t socket_sendto(mp_obj_t self_in, mp_obj_t data_in, mp_obj_t addr_in) {
    socket_obj_t *self = MP_OBJ_TO_PTR(self_in);
    lwespr_t r = lwespOK;
    int err = 0;
    // get the buffer to send
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(data_in, &bufinfo, MP_BUFFER_READ);
    if (self->type == SOCK_STREAM) {
        r = _socket_send(self, bufinfo.buf, bufinfo.len);
        if (r < 0) {
            mp_raise_OSError(-r);
            return mp_const_none;
        }
        return mp_obj_new_int(r);
    } else if (self->type == SOCK_DGRAM) {
        lwesp_ip_t ip;
        const lwesp_port_t port = (lwesp_port_t)netutils_parse_inet_addr(addr_in, (uint8_t *)ip.addr.ip4.addr, NETUTILS_BIG);
        LWESP_DEBUGF(LWESP_CFG_DBG_INIT | LWESP_DBG_LVL_SEVERE | LWESP_DBG_TYPE_TRACE,
    "udp send to addr:%d.%d.%d.%d port:%d\r\n", ip.addr.ip4.addr[0], ip.addr.ip4.addr[1], ip.addr.ip4.addr[2],
    ip.addr.ip4.addr[3], port);
        if (self->state == SOCKET_STATE_NEW) {
            mp_obj_t mp_host;
            mp_obj_t mp_port;
            mp_obj_t *elem;

            mp_obj_get_array_fixed_n(addr_in, 2, &elem);
            mp_host = elem[0];
            mp_port = elem[1];
     
            const char *host = mp_obj_str_get_str(mp_host);
            const lwesp_port_t c_port = (lwesp_port_t)MP_OBJ_SMALL_INT_VALUE(mp_port);

            LWESP_DEBUGF(LWESP_CFG_DBG_INIT | LWESP_DBG_LVL_SEVERE | LWESP_DBG_TYPE_TRACE,
        "connecting host:%s port:%d\r\n", host, c_port);
    
            MP_THREAD_GIL_EXIT();
            int r = lwesp_netconn_connect_ex(self->nc, host, c_port, 0, NULL, self->local_port, 0);
            MP_THREAD_GIL_ENTER();
            self->state = SOCKET_STATE_CONNECTED;
        }
        if (r == lwespOK) {
            MP_THREAD_GIL_EXIT();
            r = lwesp_netconn_sendto(self->nc, &ip, port, bufinfo.buf, bufinfo.len);
            MP_THREAD_GIL_ENTER();
        }
        if (r == lwespOK) {
             return mp_obj_new_int_from_uint(bufinfo.len);
        } else {
            err = -lwesp2mperr(r);
            mp_raise_OSError(err);
        }
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("not supported"));
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(socket_sendto_obj, socket_sendto);

STATIC mp_obj_t socket_fileno(const mp_obj_t arg0) {
    socket_obj_t *self = MP_OBJ_TO_PTR(arg0);
    return mp_obj_new_int((unsigned long)self->nc);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(socket_fileno_obj, socket_fileno);

STATIC mp_obj_t socket_makefile(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    return args[0];
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(socket_makefile_obj, 1, 3, socket_makefile);

STATIC mp_uint_t socket_stream_read(mp_obj_t self_in, void *buf, mp_uint_t size, int *errcode) {
    return _socket_read_data(self_in, buf, size, NULL, NULL, errcode);
}

STATIC mp_uint_t socket_stream_write(mp_obj_t self_in, const void *buf, mp_uint_t size, int *errcode) {
    socket_obj_t *sock = self_in;

    int r = _socket_send(sock, buf, size);
    if (r >= 0) {
        return r;
    } else {
        if (errcode) {
            *errcode = -r;
        }
        return MP_STREAM_ERROR;
    }
    *errcode =  MP_ETIMEDOUT;
    return MP_STREAM_ERROR;
}

STATIC mp_uint_t socket_stream_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    socket_obj_t *socket = self_in;
    if (request == MP_STREAM_POLL) {
        uintptr_t flags = arg;
        int ret = 0;

        if (flags & MP_STREAM_POLL_RD) {
            if (socket->state == SOCKET_STATE_LISTENING) {
                // Listening TCP socket may have one or multiple connections waiting
                if (lwesp_netconn_accept_ready(socket->nc)) {
                    ret |= MP_STREAM_POLL_RD;
                }
            } else {
                // Otherwise there is just one slot for incoming data
                if (lwesp_netconn_receive_ready(socket->nc) || socket->lastdata != NULL) {
                    ret |= MP_STREAM_POLL_RD;
                }
            }
        }

        if (flags & MP_STREAM_POLL_WR) {
            ret |= MP_STREAM_POLL_WR;
        }

        if (socket->state == SOCKET_STATE_NEW) {
            // New sockets are not connected so set HUP
            ret |= MP_STREAM_POLL_HUP;
        } else if (socket->state == SOCKET_STATE_PEER_CLOSED) {
            // Peer-closed socket is both readable and writable: read will
            // return EOF, write - error. Without this poll will hang on a
            // socket which was closed by peer.
            ret |= flags & (MP_STREAM_POLL_RD | MP_STREAM_POLL_WR);
        }
        return ret;
    }  else if (request == MP_STREAM_CLOSE) {
        if (socket->nc != NULL) {
            int ret = lwesp_netconn_close(socket->nc);
            
            if (ret != lwespOK && ret != lwespPARERR) {
                LWESP_DEBUGF(LWESP_CFG_DBG_INIT | LWESP_DBG_LVL_SEVERE | LWESP_DBG_TYPE_TRACE,
    "lwesp_netconn_close ret:%d\r\n", ret);

                *errcode = -lwesp2mperr(ret);
                return MP_STREAM_ERROR;
            }
            ret = lwesp_netconn_delete(socket->nc);
            if (ret != lwespOK) {
                LWESP_DEBUGF(LWESP_CFG_DBG_INIT | LWESP_DBG_LVL_SEVERE | LWESP_DBG_TYPE_TRACE,
    "lwesp_netconn_delete ret:%d\r\n", ret);
                *errcode = -lwesp2mperr(ret);

                return MP_STREAM_ERROR;
            }
            socket->nc = NULL;
        }
        *errcode = 0;
        return 0;
    }

    *errcode = MP_EINVAL;
    return MP_STREAM_ERROR;
}

STATIC const mp_rom_map_elem_t socket_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&mp_stream_close_obj) },
    { MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&mp_stream_close_obj) },
    { MP_ROM_QSTR(MP_QSTR_bind), MP_ROM_PTR(&socket_bind_obj) },
    { MP_ROM_QSTR(MP_QSTR_listen), MP_ROM_PTR(&socket_listen_obj) },
    { MP_ROM_QSTR(MP_QSTR_accept), MP_ROM_PTR(&socket_accept_obj) },
    { MP_ROM_QSTR(MP_QSTR_connect), MP_ROM_PTR(&socket_connect_obj) },
    { MP_ROM_QSTR(MP_QSTR_send), MP_ROM_PTR(&socket_send_obj) },
    { MP_ROM_QSTR(MP_QSTR_sendall), MP_ROM_PTR(&socket_sendall_obj) },
    { MP_ROM_QSTR(MP_QSTR_sendto), MP_ROM_PTR(&socket_sendto_obj) },
    { MP_ROM_QSTR(MP_QSTR_recv), MP_ROM_PTR(&socket_recv_obj) },
    { MP_ROM_QSTR(MP_QSTR_recvfrom), MP_ROM_PTR(&socket_recvfrom_obj) },
    { MP_ROM_QSTR(MP_QSTR_setsockopt), MP_ROM_PTR(&socket_setsockopt_obj) },
    { MP_ROM_QSTR(MP_QSTR_settimeout), MP_ROM_PTR(&socket_settimeout_obj) },
    { MP_ROM_QSTR(MP_QSTR_setblocking), MP_ROM_PTR(&socket_setblocking_obj) },
    { MP_ROM_QSTR(MP_QSTR_makefile), MP_ROM_PTR(&socket_makefile_obj) },
    { MP_ROM_QSTR(MP_QSTR_fileno), MP_ROM_PTR(&socket_fileno_obj) },

    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mp_stream_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&mp_stream_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_readline), MP_ROM_PTR(&mp_stream_unbuffered_readline_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&mp_stream_write_obj) },
};
STATIC MP_DEFINE_CONST_DICT(socket_locals_dict, socket_locals_dict_table);

STATIC const mp_stream_p_t socket_stream_p = {
    .read = socket_stream_read,
    .write = socket_stream_write,
    .ioctl = socket_stream_ioctl
};

STATIC const mp_obj_type_t socket_type = {
    { &mp_type_type },
    .name = MP_QSTR_socket,
    .make_new = socket_make_new,
    .protocol = &socket_stream_p,
    .locals_dict = (mp_obj_t)&socket_locals_dict,
};

STATIC mp_obj_t lwesp_socket_getaddrinfo(size_t n_args, const mp_obj_t *args) 
{
    // TODO support additional args beyond the first two

    struct lwesp_addrinfo res = {0};
    struct lwesp_addrinfo *res_p = &res;

    _socket_getaddrinfo2(args[0], args[1], res_p);
    mp_obj_t ret_list = mp_obj_new_list(0, NULL);

    mp_obj_t addrinfo_objs[5] = {
            mp_obj_new_int(res_p->family),
            mp_obj_new_int(res_p->socktype),
            mp_obj_new_int(res_p->protocol),
            mp_const_none,
            mp_const_none
    };

    if (res_p->family == AF_INET) {
        char buf[16];
        lwesp_ip4addr_ntoa_r(res_p->ip.addr.ip4.addr, buf, sizeof(buf));
        mp_obj_t inaddr_objs[2] = {
            mp_obj_new_str(buf, strlen(buf)),
            mp_obj_new_int(res_p->port)
        };
        addrinfo_objs[4] = mp_obj_new_tuple(2, inaddr_objs);
    }
    mp_obj_list_append(ret_list, mp_obj_new_tuple(5, addrinfo_objs));

    return ret_list;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(lwesp_socket_getaddrinfo_obj, 2, 6, lwesp_socket_getaddrinfo);

STATIC mp_obj_t lwesp_socket_initialize() {
    static int initialized = 0;
    if (!initialized) {
        initialized = 1;
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(lwesp_socket_initialize_obj, lwesp_socket_initialize);

STATIC mp_obj_t mod_socket_inet_pton(mp_obj_t family_in, mp_obj_t addr_in)
{
    int family = mp_obj_get_int(family_in);
    uint32_t addr;
    int r = lwesp_ip4addr_aton_r(mp_obj_str_get_str(addr_in),  &addr);
    if (r == 0) {
        mp_raise_OSError(MP_EINVAL);
    }
    int binaddr_len = 0;
    binaddr_len = sizeof(addr);
    return mp_obj_new_bytes((const byte *)&addr, binaddr_len);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mod_socket_inet_pton_obj, mod_socket_inet_pton);

STATIC mp_obj_t mod_socket_inet_ntop(mp_obj_t family_in, mp_obj_t binaddr_in)
{
    int family = mp_obj_get_int(family_in);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(binaddr_in, &bufinfo, MP_BUFFER_READ);
    vstr_t vstr;
    vstr_init_len(&vstr, 16);
    memset(vstr.buf, 0, vstr.len);
    if (lwesp_ip4addr_ntoa_r(bufinfo.buf, vstr.buf, vstr.len) == NULL) { 
        mp_raise_OSError(MP_EINVAL);
    }
    vstr.len = strlen(vstr.buf);
    return mp_obj_new_str_from_vstr(&mp_type_str, &vstr);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mod_socket_inet_ntop_obj, mod_socket_inet_ntop);


STATIC const mp_rom_map_elem_t mp_module_socket_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_usocket) },
    { MP_ROM_QSTR(MP_QSTR___init__), MP_ROM_PTR(&lwesp_socket_initialize_obj) },
    { MP_ROM_QSTR(MP_QSTR_socket), MP_ROM_PTR(&socket_type) },
    { MP_ROM_QSTR(MP_QSTR_getaddrinfo), MP_ROM_PTR(&lwesp_socket_getaddrinfo_obj) },
    { MP_ROM_QSTR(MP_QSTR_inet_pton), MP_ROM_PTR(&mod_socket_inet_pton_obj) },
    { MP_ROM_QSTR(MP_QSTR_inet_ntop), MP_ROM_PTR(&mod_socket_inet_ntop_obj) },

    { MP_ROM_QSTR(MP_QSTR_AF_INET), MP_ROM_INT(AF_INET) },
    { MP_ROM_QSTR(MP_QSTR_SOCK_STREAM), MP_ROM_INT(SOCK_STREAM) },
    { MP_ROM_QSTR(MP_QSTR_SOCK_DGRAM), MP_ROM_INT(SOCK_DGRAM) },
    { MP_ROM_QSTR(MP_QSTR_SOCK_RAW), MP_ROM_INT(SOCK_RAW) },
    { MP_ROM_QSTR(MP_QSTR_IPPROTO_TCP), MP_ROM_INT(IPPROTO_TCP) },
    { MP_ROM_QSTR(MP_QSTR_IPPROTO_UDP), MP_ROM_INT(IPPROTO_UDP) },
    { MP_ROM_QSTR(MP_QSTR_IPPROTO_IP), MP_ROM_INT(IPPROTO_IP) },
    { MP_ROM_QSTR(MP_QSTR_SOL_SOCKET), MP_ROM_INT(SOL_SOCKET) },
    { MP_ROM_QSTR(MP_QSTR_SO_REUSEADDR), MP_ROM_INT(SO_REUSEADDR) },
    { MP_ROM_QSTR(MP_QSTR_IP_ADD_MEMBERSHIP), MP_ROM_INT(IP_ADD_MEMBERSHIP) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_socket_globals, mp_module_socket_globals_table);

const mp_obj_module_t mp_module_lwesp_usocket = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_socket_globals,
};
