/**
 * @file re_udp.h  Interface to User Datagram Protocol
 *
 * Copyright (C) 2010 Creytiv.com
 */

#ifndef RE_UDP_H
#define RE_UDP_H
#include "re_common.h"
#include "re_sa.h"

/**
 * @addtogroup aos_rtp_udp rtp_udp
 * @{
 */

/**
 * Defines the UDP Receive handler
 *
 * @param src Source address
 * @param mb  Datagram buffer
 * @param arg Handler argument
 */
typedef void (udp_recv_h)(const struct sa *src, struct mbuf *mb, void *arg);
/**
 * Defines the UDP Receive handler
 *
 * @param err error number
 * @param arg Handler argument
 */
typedef void (udp_error_h)(int err, void *arg);

/* Helper API */
/* @brief udp send helper */
typedef bool (udp_helper_send_h)(int *err, struct sa *dst,
				 struct mbuf *mb, void *arg);
/* @brief udp recv helper */
typedef bool (udp_helper_recv_h)(struct sa *src,
				 struct mbuf *mb, void *arg);
/** @brief Defines a UDP socket */
struct udp_sock {
	struct list helpers; /**< List of UDP Helpers         */
	udp_recv_h *rh;      /**< Receive handler             */
	udp_error_h *eh;     /**< Error handler               */
	void *arg;           /**< Handler argument            */
	int fd;              /**< Socket file descriptor      */
	int fd6;             /**< IPv6 socket file descriptor */
	bool conn;           /**< Connected socket flag       */
	size_t rxsz;         /**< Maximum receive chunk size  */
	size_t rx_presz;     /**< Preallocated rx buffer size */
};

/** @brief Defines a UDP helper */
struct udp_helper {
	struct le le;
	int layer;
	udp_helper_send_h *sendh;
	udp_helper_recv_h *recvh;
	void *arg;
};

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
int  udpsock_listen(struct udp_sock **usp, const struct sa *local,
		udp_recv_h *rh, void *arg);

/**
 * Connect a UDP Socket to a specific peer.
 * When connected, this UDP Socket will only receive data from that peer.
 *
 * @param us   UDP Socket
 * @param peer Peer network address
 *
 * @return 0 if success, otherwise errorcode
 */
int  udpsock_connect(struct udp_sock *us, const struct sa *peer);

/**
 * Send a UDP Datagram to a peer
 *
 * @param us  UDP Socket
 * @param dst Destination network address
 * @param mb  Buffer to send
 *
 * @return 0 if success, otherwise errorcode
 */
int  udpsock_send(struct udp_sock *us, const struct sa *dst, struct mbuf *mb);

/**
 * Send an anonymous UDP Datagram to a peer
 *
 * @param dst Destination network address
 * @param mb  Buffer to send
 *
 * @return 0 if success, otherwise errorcode
 */
int  udpsock_send_anon(const struct sa *dst, struct mbuf *mb);

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
int  udpsock_local_get(const struct udp_sock *us, struct sa *local);

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
int  udpsock_setsockopt(struct udp_sock *us, int level, int optname,
		    const void *optval, uint32_t optlen);

/**
 * Set the send/receive buffer size on a UDP Socket
 *
 * @param us   UDP Socket
 * @param size Buffer size in bytes
 *
 * @return 0 if success, otherwise errorcode
 */
int  udpsock_sockbuf_set(struct udp_sock *us, int size);

/**
 * Set the maximum receive chunk size on a UDP Socket
 *
 * @param us   UDP Socket
 * @param rxsz Maximum receive chunk size
 */
void udpsock_rxsz_set(struct udp_sock *us, size_t rxsz);

/**
 * Set preallocated space on receive buffer.
 *
 * @param us       UDP Socket
 * @param rx_presz Size of preallocate space.
 */
void udpsock_rxbuf_presz_set(struct udp_sock *us, size_t rx_presz);

/**
 * Set receive handler on a UDP Socket
 *
 * @param us  UDP Socket
 * @param rh  Receive handler
 * @param arg Handler argument
 */
void udpsock_handler_set(struct udp_sock *us, udp_recv_h *rh, void *arg);

/**
 * Set error handler on a UDP Socket
 *
 * @param us  UDP Socket
 * @param eh  Error handler
 */
void udpsock_error_handler_set(struct udp_sock *us, udp_error_h *eh);

/**
 * Get the File Descriptor from a UDP Socket
 *
 * @param us  UDP Socket
 * @param af  Address Family
 *
 * @return File Descriptor, or -1 for errors
 */
int  udpsock_sock_fd(const struct udp_sock *us, int af);

/**
 * Attach the current thread to the UDP Socket
 *
 * @param us UDP Socket
 *
 * @return 0 if success, otherwise errorcode
 */
int  udpsock_thread_attach(struct udp_sock *us);

/**
 * Detach the current thread from the UDP Socket
 *
 * @param us UDP Socket
 */
void udpsock_thread_detach(struct udp_sock *us);

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
			void *arg);

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
		    struct mbuf *mb, struct udp_helper *uh);

/**
 * Find a UDP-helper on a UDP socket
 *
 * @param us    UDP socket
 * @param layer Layer number
 *
 * @return UDP-helper if found, NULL if not found
 */
struct udp_helper *udpsock_helper_find(const struct udp_sock *us, int layer);

/** @} */
#endif /* RE_UDP_H */
