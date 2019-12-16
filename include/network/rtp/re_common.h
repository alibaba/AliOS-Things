/**
 * @file re_sa.h  Interface to Socket Address
 *
 * Copyright (C) 2010 Creytiv.com
 */
#ifndef RE_COMMON_H
#define RE_COMMON_H
#include <network/network.h>

/**
 * @addtogroup aos_rtp_common rtp_common
 * @{
 */

/** Defines a pointer-length string type */
struct pl {
        const char *p;  /**< Pointer to string */
        size_t l;       /**< Length of string  */
};

/**
 * Defines the re_vhprintf() print handler
 *
 * @param p    String to print
 * @param size Size of string to print
 * @param arg  Handler argument
 *
 * @return 0 for success, otherwise errorcode
 */
typedef int(re_vprintf_h)(const char *p, size_t size, void *arg);

/** Defines a print backend */
struct re_printf {
	re_vprintf_h *vph; /**< Print handler   */
	void *arg;         /**< Handler agument */
};

/** Defines a Socket Address */
struct sa {
	union {
		struct sockaddr sa;
		struct sockaddr_in in;
#ifdef HAVE_INET6
		struct sockaddr_in6 in6;
#endif
		uint8_t padding[28];
	} u;
	socklen_t len;
};

typedef void (rtp_recv_h)(const struct sa *src, const struct rtp_header *hdr,
			  struct mbuf *mb, void *arg);
typedef void (rtcp_recv_h)(const struct sa *src, struct rtcp_msg *msg,
			   void *arg);

/** Defines an RTP Socket */
struct rtp_sock {
	/** Encode data */
	struct {
		uint16_t seq;   /**< Sequence number       */
		uint32_t ssrc;  /**< Synchronizing source  */
	} enc;
	int proto;              /**< Transport Protocol    */
	void *sock_rtp;         /**< RTP Socket            */
	void *sock_rtcp;        /**< RTCP Socket           */
	struct sa local;        /**< Local RTP Address     */
	struct sa rtcp_peer;    /**< RTCP address of Peer  */
	rtp_recv_h *recvh;      /**< RTP Receive handler   */
	rtcp_recv_h *rtcph;     /**< RTCP Receive handler  */
	void *arg;              /**< Handler argument      */
	struct rtcp_sess *rtcp; /**< RTCP Session          */
	bool rtcp_mux;          /**< RTP/RTCP multiplexing */
};

/** @} */
#endif /* RE_COMMON_H */
