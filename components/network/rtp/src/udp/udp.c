/**
 * @file udp.c  User Datagram Protocol
 *
 * Copyright (C) 2010 Creytiv.com
 */
#include <stdlib.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_IO_H
#include <io.h>
#endif
#if !defined(WIN32)
#define __USE_POSIX 1  /**< Use POSIX flag */
#define __USE_XOPEN2K 1/**< Use POSIX.1:2001 code */
#include <network/network.h>
#endif
#include <string.h>
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif
#ifdef __APPLE__
#include "TargetConditionals.h"
#endif
#include <re_types.h>
#include <re_fmt.h>
#include <re_mem.h>
#include <re_mbuf.h>
#include <re_list.h>
#include <re_main.h>
#include <re_sa.h>
#include <re_net.h>
#include <re_udp.h>
#include <aos/kernel.h>

#define DEBUG_MODULE "udp"
#define DEBUG_LEVEL 5
#include <re_dbg.h>


/** Platform independent buffer type cast */
#ifdef WIN32
#define BUF_CAST (char *)
#define SOK_CAST (int)
#define SIZ_CAST (int)
#define close closesocket
#else
#define BUF_CAST
#define SOK_CAST
#define SIZ_CAST
#endif

enum {
	UDP_RXSZ_DEFAULT = 8192
};

#define UDPSOCK_READ_SOCK_MAX_NUM  8
struct udpsock_read_sock{
    bool is_fd_used;
    bool is_fd6_used;
    struct udp_sock* us;
};

static struct udpsock_read_sock g_read_sock[UDPSOCK_READ_SOCK_MAX_NUM] = {0};

static bool g_udpsock_read_task_running = false;
static void udpsock_read(struct udp_sock *us, int fd);

void udpsock_read_task_handle(void)
{
    struct timeval timeselect;
    fd_set read_sets;
    int maxfdp = 0;
    int err;

    g_udpsock_read_task_running = true;

    /* set select timeout */
    timeselect.tv_sec  = 3;
    timeselect.tv_usec = 0;
    while(1) {
        int i;

        if(g_udpsock_read_task_running == false) {
            printf("udpsock read task exit\n");
            break;
        }

        maxfdp = 0;

        FD_ZERO(&read_sets);
        for(i = 0; i < UDPSOCK_READ_SOCK_MAX_NUM; i++) {
            if((g_read_sock[i].is_fd_used == true) && (g_read_sock[i].us->fd != -1)) {
                maxfdp = (maxfdp > g_read_sock[i].us->fd ? maxfdp : g_read_sock[i].us->fd);
                FD_SET(g_read_sock[i].us->fd, &read_sets);
            }

            if((g_read_sock[i].is_fd6_used == true) && (g_read_sock[i].us->fd6 != -1)) {
                maxfdp = (maxfdp > g_read_sock[i].us->fd6 ? maxfdp : g_read_sock[i].us->fd6);
                FD_SET(g_read_sock[i].us->fd6, &read_sets);
            }
        }

        if(maxfdp == 0) {
            g_udpsock_read_task_running = false;
            continue;
        }

        err = select(maxfdp + 1, &read_sets, NULL, NULL, &timeselect);
        if(err > 0) {
            for(i = 0; i < UDPSOCK_READ_SOCK_MAX_NUM; i++) {
                if((g_read_sock[i].is_fd_used == true)
                  && (FD_ISSET(g_read_sock[i].us->fd, &read_sets))) {
                    udpsock_read(g_read_sock[i].us, g_read_sock[i].us->fd);
                }
                if((g_read_sock[i].is_fd6_used == true)
                  && (FD_ISSET(g_read_sock[i].us->fd6, &read_sets))) {
                    udpsock_read(g_read_sock[i].us, g_read_sock[i].us->fd6);
                }
            }
        }
        else
        {
            if(errno == EINTR) {
#ifdef CONFIG_LINUX_IPSTACK
                aos_msleep(200);
#endif
            }
        }
    }
    aos_task_exit(0);
}

static void dummy_udp_recv_handler(const struct sa *src,
				   struct mbuf *mb, void *arg)
{
	(void)src;
	(void)mb;
	(void)arg;
}


static bool helper_send_handler(int *err, struct sa *dst,
				struct mbuf *mb, void *arg)
{
	(void)err;
	(void)dst;
	(void)mb;
	(void)arg;
	return false;
}


static bool helper_recv_handler(struct sa *src,
				struct mbuf *mb, void *arg)
{
	(void)src;
	(void)mb;
	(void)arg;
	return false;
}


static void udp_destructor(void *data)
{
	struct udp_sock *us = data;

	list_flush(&us->helpers);

    udpsock_thread_detach(us);
}


static void udpsock_read(struct udp_sock *us, int fd)
{
	struct mbuf *mb = mbuf_alloc(us->rxsz);
	struct sa src;
	struct le *le;
	int err = 0;
	ssize_t n;

	if (!mb)
		return;

	src.len = sizeof(src.u);
	n = recvfrom(fd, BUF_CAST mb->buf + us->rx_presz,
		     mb->size - us->rx_presz, 0,
		     &src.u.sa, &src.len);
	if (n < 0) {
		err = errno;

		if (EAGAIN == err)
			goto out;

#ifdef EWOULDBLOCK
		if (EWOULDBLOCK == err)
			goto out;
#endif

#if TARGET_OS_IPHONE
		if (ENOTCONN == err) {

			struct udp_sock *us_new;
			struct sa laddr;

			err = udp_local_get(us, &laddr);
			if (err)
				goto out;

			if (-1 != us->fd) {
				fd_close(us->fd);
				(void)close(us->fd);
				us->fd = -1;
			}

			if (-1 != us->fd6) {
				fd_close(us->fd6);
				(void)close(us->fd6);
				us->fd6 = -1;
			}

			err = udpsock_listen(&us_new, &laddr, NULL, NULL);
			if (err)
				goto out;

			us->fd  = us_new->fd;
			us->fd6 = us_new->fd6;

			us_new->fd  = -1;
			us_new->fd6 = -1;

			mem_deref(us_new);

			udpsock_thread_attach(us);

			goto out;
		}
#endif
		if (us->eh)
			us->eh(err, us->arg);

		goto out;
	}

	mb->pos = us->rx_presz;
	mb->end = n + us->rx_presz;

	(void)mbuf_resize(mb, mb->end);

	/* call helpers */
	le = us->helpers.head;
	while (le) {
		struct udp_helper *uh = le->data;
		bool hdld;

		le = le->next;

		hdld = uh->recvh(&src, mb, uh->arg);
		if (hdld)
			goto out;
	}

	us->rh(&src, mb, us->arg);

 out:
	mem_deref(mb);
}

/**
 * Create and listen on a UDP Socket
 *
 * @param usp   Pointer to returned UDP Socket
 * @param local Local network address
 * @param rh    Receive handler
 * @param arg   Handler argument
 *
 * @return 0 if success, otherwise errorcode
 */
int udpsock_listen(struct udp_sock **usp, const struct sa *local,
	       udp_recv_h *rh, void *arg)
{
	struct addrinfo hints, *res = NULL, *r;
	struct udp_sock *us = NULL;
	char addr[64];
	char serv[6] = "12345";
	int af, error, err = 0;

	if (!usp)
		return EINVAL;

	us = mem_zalloc(sizeof(*us), udp_destructor);
	if (!us)
		return ENOMEM;

	list_init(&us->helpers);

	us->fd  = -1;
	us->fd6 = -1;

	if (local) {
		af = sa_af(local);
		(void)re_snprintf(addr, sizeof(addr), "%H",
				  sa_print_addr, local);
		(void)re_snprintf(serv, sizeof(serv), "%u", sa_port(local));
	}
	else {
#ifdef HAVE_INET6
		af = AF_UNSPEC;
#else
		af = AF_INET;
#endif
	}

	memset(&hints, 0, sizeof(hints));
	/* set-up hints structure */
	hints.ai_family   = af;
	hints.ai_flags    = AI_PASSIVE | AI_NUMERICHOST;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;

	error = getaddrinfo(local ? addr : NULL, serv, &hints, &res);
	if (error) {
#ifdef WIN32
		DEBUG_WARNING("listen: getaddrinfo: wsaerr=%d\n",
			      WSAGetLastError());
#endif
		//DEBUG_WARNING("listen: getaddrinfo: %s:%s (%s)\n",
		//	      addr, serv, gai_strerror(error));
		err = EADDRNOTAVAIL;
		goto out;
	}

	for (r = res; r; r = r->ai_next) {
		int fd = -1;

		if (us->fd > 0)
			continue;

		DEBUG_INFO("listen: for: af=%d addr=%j\n",
			   r->ai_family, r->ai_addr);

		fd = SOK_CAST socket(r->ai_family, SOCK_DGRAM, IPPROTO_UDP);
		if (fd < 0) {
			err = errno;
			continue;
		}

		err = net_sockopt_blocking_set(fd, false);
		if (err) {
			DEBUG_WARNING("udp listen: nonblock set: %m\n", err);
			(void)close(fd);
			continue;
		}

		if (bind(fd, r->ai_addr, SIZ_CAST r->ai_addrlen) < 0) {
			err = errno;
			DEBUG_INFO("listen: bind(): %m (%J)\n", err, local);
			(void)close(fd);
			continue;
		}

		/* Can we do both IPv4 and IPv6 on same socket? */
		if (AF_INET6 == r->ai_family) {
			struct sa sa;
			int on = 1;  /* assume v6only */

#if defined (IPPROTO_IPV6) && defined (IPV6_V6ONLY)
			socklen_t on_len = sizeof(on);
			if (0 != getsockopt(fd, IPPROTO_IPV6, IPV6_V6ONLY,
					    (char *)&on, &on_len)) {
				on = 1;
			}
#endif
			/* Extra check for unspec addr - MAC OS X/Solaris */
			if (0==sa_set_sa(&sa, r->ai_addr) && sa_is_any(&sa)) {
				on = 1;
			}
			DEBUG_INFO("socket %d: IPV6_V6ONLY is %d\n", fd, on);
			if (on) {
				us->fd6 = fd;
				continue;
			}
		}

		/* OK */
		us->fd = fd;
		break;
	}

	freeaddrinfo(res);

	/* We must have at least one socket */
	if (-1 == us->fd && -1 == us->fd6) {
		if (0 == err)
			err = EADDRNOTAVAIL;
		goto out;
	}

    err = udpsock_thread_attach(us);
    if(err) {
        goto out;
    }

	us->rh   = rh ? rh : dummy_udp_recv_handler;
	us->arg  = arg;
	us->rxsz = UDP_RXSZ_DEFAULT;

 out:
	if (err)
		mem_deref(us);
	else
		*usp = us;

	return err;
}

/**
 * Connect a UDP Socket to a specific peer.
 * When connected, this UDP Socket will only receive data from that peer.
 *
 * @param us   UDP Socket
 * @param peer Peer network address
 *
 * @return 0 if success, otherwise errorcode
 */
int udpsock_connect(struct udp_sock *us, const struct sa *peer)
{
	int fd;

	if (!us || !peer)
		return EINVAL;

	/* choose a socket */
	if (AF_INET6 == sa_af(peer) && -1 != us->fd6)
		fd = us->fd6;
	else
		fd = us->fd;

	if (0 != connect(fd, &peer->u.sa, peer->len))
		return errno;

	us->conn = true;

	return 0;
}

static int udp_send_internal(struct udp_sock *us, const struct sa *dst,
			     struct mbuf *mb, struct le *le)
{
	struct sa hdst;
	int err = 0, fd;

	/* choose a socket */
	if (AF_INET6 == sa_af(dst) && -1 != us->fd6)
		fd = us->fd6;
	else
		fd = us->fd;

	/* call helpers in reverse order */
	while (le) {
		struct udp_helper *uh = le->data;

		le = le->prev;

		if (dst != &hdst) {
			sa_cpy(&hdst, dst);
			dst = &hdst;
		}

		if (uh->sendh(&err, &hdst, mb, uh->arg) || err)
			return err;
	}

	/* Connected socket? */
	if (us->conn) {
		if (send(fd, BUF_CAST mb->buf + mb->pos, mb->end - mb->pos,
			 0) < 0)
			return errno;
	}
	else {
		if (sendto(fd, BUF_CAST mb->buf + mb->pos, mb->end - mb->pos,
			   0, &dst->u.sa, dst->len) < 0)
			return errno;
	}

	return 0;
}

/**
 * Send a UDP Datagram to a peer
 *
 * @param us  UDP Socket
 * @param dst Destination network address
 * @param mb  Buffer to send
 *
 * @return 0 if success, otherwise errorcode
 */
int udpsock_send(struct udp_sock *us, const struct sa *dst, struct mbuf *mb)
{
	if (!us || !dst || !mb)
		return EINVAL;

	return udp_send_internal(us, dst, mb, us->helpers.tail);
}

/**
 * Send an anonymous UDP Datagram to a peer
 *
 * @param dst Destination network address
 * @param mb  Buffer to send
 *
 * @return 0 if success, otherwise errorcode
 */
int udpsock_send_anon(const struct sa *dst, struct mbuf *mb)
{
	struct udp_sock *us;
	int err;

	if (!dst || !mb)
		return EINVAL;
	err = udpsock_listen(&us, NULL, NULL, NULL);
	if (err)
		return err;

	err = udp_send_internal(us, dst, mb, NULL);
	mem_deref(us);

	return err;
}


/**
 * Get the local network address on the UDP Socket
 *
 * @param us    UDP Socket
 * @param local The returned local network address
 *
 * @return 0 if success, otherwise errorcode
 *
 * @todo bug no way to specify AF
 */
int udpsock_local_get(const struct udp_sock *us, struct sa *local)
{
	if (!us || !local)
		return EINVAL;

	local->len = sizeof(local->u);

	if (0 == getsockname(us->fd, &local->u.sa, &local->len))
		return 0;

	if (0 == getsockname(us->fd6, &local->u.sa, &local->len))
		return 0;

	return errno;
}


/**
 * Set socket options on the UDP Socket
 *
 * @param us      UDP Socket
 * @param level   Socket level
 * @param optname Option name
 * @param optval  Option value
 * @param optlen  Option length
 *
 * @return 0 if success, otherwise errorcode
 */
int udpsock_setsockopt(struct udp_sock *us, int level, int optname,
		   const void *optval, uint32_t optlen)
{
	int err = 0;

	if (!us)
		return EINVAL;

	if (-1 != us->fd) {
		if (0 != setsockopt(us->fd, level, optname,
				    BUF_CAST optval, optlen))
			err |= errno;
	}

	if (-1 != us->fd6) {
		if (0 != setsockopt(us->fd6, level, optname,
				    BUF_CAST optval, optlen))
			err |= errno;
	}

	return err;
}


/**
 * Set the send/receive buffer size on a UDP Socket
 *
 * @param us   UDP Socket
 * @param size Buffer size in bytes
 *
 * @return 0 if success, otherwise errorcode
 */
int udpsock_sockbuf_set(struct udp_sock *us, int size)
{
	int err = 0;

	if (!us)
		return EINVAL;

	err |= udpsock_setsockopt(us, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size));
	err |= udpsock_setsockopt(us, SOL_SOCKET, SO_SNDBUF, &size, sizeof(size));

	return err;
}


/**
 * Set the maximum receive chunk size on a UDP Socket
 *
 * @param us   UDP Socket
 * @param rxsz Maximum receive chunk size
 */
void udpsock_rxsz_set(struct udp_sock *us, size_t rxsz)
{
	if (!us)
		return;

	us->rxsz = rxsz;
}


/**
 * Set preallocated space on receive buffer.
 *
 * @param us       UDP Socket
 * @param rx_presz Size of preallocate space.
 */
void udpsock_rxbuf_presz_set(struct udp_sock *us, size_t rx_presz)
{
	if (!us)
		return;

	us->rx_presz = rx_presz;
}


/**
 * Set receive handler on a UDP Socket
 *
 * @param us  UDP Socket
 * @param rh  Receive handler
 * @param arg Handler argument
 */
void udpsock_handler_set(struct udp_sock *us, udp_recv_h *rh, void *arg)
{
	if (!us)
		return;

	us->rh  = rh ? rh : dummy_udp_recv_handler;
	us->arg = arg;
}


/**
 * Set error handler on a UDP Socket
 *
 * @param us  UDP Socket
 * @param eh  Error handler
 */
void udpsock_error_handler_set(struct udp_sock *us, udp_error_h *eh)
{
	if (!us)
		return;

	us->eh = eh;
}


/**
 * Get the File Descriptor from a UDP Socket
 *
 * @param us  UDP Socket
 * @param af  Address Family
 *
 * @return File Descriptor, or -1 for errors
 */
int udpsock_sock_fd(const struct udp_sock *us, int af)
{
	if (!us)
		return -1;

	switch (af) {

	default:
	case AF_INET:  return us->fd;
	case AF_INET6: return (us->fd6 != -1) ? us->fd6 : us->fd;
	}
}


/**
 * Attach the current thread to the UDP Socket
 *
 * @param us UDP Socket
 *
 * @return 0 if success, otherwise errorcode
 */
int udpsock_thread_attach(struct udp_sock *us)
{
    int err = 0;
    aos_task_t read_sock_task;

    /* create a task to read data */
    if(g_udpsock_read_task_running == false) {
        /*reset read sock information */
        memset(g_read_sock, 0, sizeof(g_read_sock));
        if(us->fd != -1) {
            g_read_sock[0].is_fd_used = true;
        }
        if(us->fd6 != -1) {
            g_read_sock[0].is_fd6_used = true;
        }
        g_read_sock[0].us = us;
        aos_task_new_ext(&read_sock_task, "rtp_udpsock_recv", udpsock_read_task_handle, NULL, 20*1024, 18);
    }
    else {
        int i;
        int found_sock = 0;
        int empty_sock = 0;

        for(i = 0; i < UDPSOCK_READ_SOCK_MAX_NUM; i++) {
            if((found_sock == 0)
              && (g_read_sock[i].is_fd_used == false)
              && (g_read_sock[i].is_fd6_used == false))
            {
                empty_sock = i;
            }
            if(g_read_sock[i].us == us)
            {
                found_sock = i;
                break;
            }
        }

        if(found_sock != 0) {
            printf("rtp udpsock:%p is already attach\n", us);
        }
        else if(empty_sock != 0) {
            if(us->fd != -1) {
                g_read_sock[i].is_fd_used = true;
            }
            if(us->fd6 != -1) {
                g_read_sock[i].is_fd6_used = true;
            }
            g_read_sock[i].us = us;
        }
        else {
            printf("rtp udpsock read sock is full\n");
            err = -1;
        }
    }

    return err;
}


/**
 * Detach the current thread from the UDP Socket
 *
 * @param us UDP Socket
 */
void udpsock_thread_detach(struct udp_sock *us)
{
    int i;

	if (!us)
		return;

    for(i = 0; i < UDPSOCK_READ_SOCK_MAX_NUM; i++) {
        if(g_read_sock[i].us == us)
        {
            break;
        }
    }

    if( i == UDPSOCK_READ_SOCK_MAX_NUM) {
        printf("no udp sock found \n");
        return ;
    }

    g_read_sock[i].is_fd_used = false;
    g_read_sock[i].is_fd6_used = false;
    g_read_sock[i].us = NULL;

	if (-1 != us->fd)
		fd_close(us->fd);

	if (-1 != us->fd6)
		fd_close(us->fd6);
}


static void helper_destructor(void *data)
{
	struct udp_helper *uh = data;

	list_unlink(&uh->le);
}


static bool sort_handler(struct le *le1, struct le *le2, void *arg)
{
	struct udp_helper *uh1 = le1->data, *uh2 = le2->data;
	(void)arg;

	return uh1->layer <= uh2->layer;
}


/**
 * Register a UDP protocol stack helper
 *
 * @param uhp   Pointer to allocated UDP helper object
 * @param us    UDP socket
 * @param layer Layer number; higher number means higher up in stack
 * @param sh    Send handler
 * @param rh    Receive handler
 * @param arg   Handler argument
 *
 * @return 0 if success, otherwise errorcode
 */
int udpsock_register_helper(struct udp_helper **uhp, struct udp_sock *us,
			int layer,
			udp_helper_send_h *sh, udp_helper_recv_h *rh,
			void *arg)
{
	struct udp_helper *uh;

	if (!us)
		return EINVAL;

	uh = mem_zalloc(sizeof(*uh), helper_destructor);
	if (!uh)
		return ENOMEM;

	list_append(&us->helpers, &uh->le, uh);

	uh->layer = layer;
	uh->sendh = sh ? sh : helper_send_handler;
	uh->recvh = rh ? rh : helper_recv_handler;
	uh->arg   = arg;

	list_sort(&us->helpers, sort_handler, NULL);

	if (uhp)
		*uhp = uh;

	return 0;
}


/**
 * Send a UDP Datagram to a remote peer bypassing this helper and
 * the helpers above it.
 *
 * @param us  UDP Socket
 * @param dst Destination network address
 * @param mb  Buffer to send
 * @param uh  UDP Helper
 *
 * @return 0 if success, otherwise errorcode
 */
int udpsock_send_helper(struct udp_sock *us, const struct sa *dst,
		    struct mbuf *mb, struct udp_helper *uh)
{
	if (!us || !dst || !mb || !uh)
		return EINVAL;

	return udp_send_internal(us, dst, mb, uh->le.prev);
}


/**
 * Find a UDP-helper on a UDP socket
 *
 * @param us    UDP socket
 * @param layer Layer number
 *
 * @return UDP-helper if found, NULL if not found
 */
struct udp_helper *udpsock_helper_find(const struct udp_sock *us, int layer)
{
	struct le *le;

	if (!us)
		return NULL;

	for (le = us->helpers.head; le; le = le->next) {

		struct udp_helper *uh = le->data;

		if (layer == uh->layer)
			return uh;
	}

	return NULL;
}
