/**
 * @file rtp.c  Real-time Transport Protocol
 *
 * Copyright (C) 2010 Creytiv.com
 */
#include <string.h>
#include <re_types.h>
#include <re_fmt.h>
#include <re_mem.h>
#include <re_mbuf.h>
#include <re_list.h>
#include <re_sa.h>
#include <re_sys.h>
#include <re_net.h>
#include <re_udp.h>
#include <re_rtp.h>
#include "rtcp.h"


#define DEBUG_MODULE "rtp"
#define DEBUG_LEVEL 5
#include <re_dbg.h>

/**
 * Encode the RTP header into a buffer
 *
 * @param mb  Buffer to encode into
 * @param hdr RTP Header to be encoded
 *
 * @return 0 if success, otherwise errorcode
 */
int rtp_hdr_encode(struct mbuf *mb, const struct rtp_header *hdr)
{
	uint8_t buf[2];
	int err, i;

	if (!mb || !hdr)
		return EINVAL;

	buf[0]  = (hdr->ver & 0x02) << 6;
	buf[0] |= (hdr->pad & 0x01) << 5;
	buf[0] |= (hdr->ext & 0x01) << 4;
	buf[0] |= (hdr->cc  & 0x0f) << 0;
	buf[1]  = (hdr->m   & 0x01) << 7;
	buf[1] |= (hdr->pt  & 0x7f) << 0;

	err  = mbuf_write_mem(mb, buf, sizeof(buf));
	err |= mbuf_write_u16(mb, htons(hdr->seq));
	err |= mbuf_write_u32(mb, htonl(hdr->ts));
	err |= mbuf_write_u32(mb, htonl(hdr->ssrc));

	for (i=0; i<hdr->cc; i++) {
		err |= mbuf_write_u32(mb, htonl(hdr->csrc[i]));
	}

	return err;
}


/**
 * Decode an RTP header from a buffer
 *
 * @param hdr RTP Header to decode into
 * @param mb  Buffer to decode from
 *
 * @return 0 if success, otherwise errorcode
 */
int rtp_hdr_decode(struct rtp_header *hdr, struct mbuf *mb)
{
	uint8_t buf[2];
	int err, i;
	size_t header_len;

	if (!hdr || !mb)
		return EINVAL;

	if (mbuf_get_left(mb) < RTP_HEADER_SIZE)
		return EBADMSG;

	err = mbuf_read_mem(mb, buf, sizeof(buf));
	if (err)
		return err;

	hdr->ver  = (buf[0] >> 6) & 0x03;
	hdr->pad  = (buf[0] >> 5) & 0x01;
	hdr->ext  = (buf[0] >> 4) & 0x01;
	hdr->cc   = (buf[0] >> 0) & 0x0f;
	hdr->m    = (buf[1] >> 7) & 0x01;
	hdr->pt   = (buf[1] >> 0) & 0x7f;

	hdr->seq  = ntohs(mbuf_read_u16(mb));
	hdr->ts   = ntohl(mbuf_read_u32(mb));
	hdr->ssrc = ntohl(mbuf_read_u32(mb));

	header_len = hdr->cc*sizeof(uint32_t);
	if (mbuf_get_left(mb) < header_len)
		return EBADMSG;

	for (i=0; i<hdr->cc; i++) {
		hdr->csrc[i] = ntohl(mbuf_read_u32(mb));
	}

	if (hdr->ext) {
		if (mbuf_get_left(mb) < 4)
			return EBADMSG;

		hdr->x.type = ntohs(mbuf_read_u16(mb));
		hdr->x.len  = ntohs(mbuf_read_u16(mb));

		if (mbuf_get_left(mb) < hdr->x.len*sizeof(uint32_t))
			return EBADMSG;

		mb->pos += hdr->x.len*sizeof(uint32_t);
	}

	return 0;
}


static void destructor(void *data)
{
	struct rtp_sock *rs = data;

	switch (rs->proto) {

	case IPPROTO_UDP:
		udpsock_handler_set(rs->sock_rtp, NULL, NULL);
		udpsock_handler_set(rs->sock_rtcp, NULL, NULL);
		break;

	default:
		break;
	}

	/* Destroy RTCP Session now */
	mem_deref(rs->rtcp);

	mem_deref(rs->sock_rtp);
	mem_deref(rs->sock_rtcp);
}


static void rtcp_recv_handler(const struct sa *src, struct mbuf *mb, void *arg)
{
	struct rtp_sock *rs = arg;
	struct rtcp_msg *msg;

	while (0 == rtcp_decode(&msg, mb)) {

		/* handle internally first */
		rtcp_handler(rs->rtcp, msg);

		/* then relay to application */
		if (rs->rtcph)
			rs->rtcph(src, msg, rs->arg);

		mem_deref(msg);
	}
}


static void udp_recv_handler(const struct sa *src, struct mbuf *mb, void *arg)
{
	struct rtp_sock *rs = arg;
	struct rtp_header hdr;
	int err;

	/* Handle RTCP multiplexed on RTP-port */
	if (rs->rtcp_mux) {
		uint8_t pt;

		if (mbuf_get_left(mb) < 2)
			return;

		pt = mbuf_buf(mb)[1] & 0x7f;

		if (64 <= pt && pt <= 95) {
			rtcp_recv_handler(src, mb, arg);
			return;
		}
	}

	err = rtp_decode(rs, mb, &hdr);
	if (err)
		return;

	if (rs->rtcp) {
		rtcp_sess_rx_rtp(rs->rtcp, hdr.seq, hdr.ts,
				 hdr.ssrc, mbuf_get_left(mb), src);
	}

	if (rs->recvh)
		rs->recvh(src, &hdr, mb, rs->arg);
}


static int udp_range_listen(struct rtp_sock *rs, const struct sa *ip,
			    uint16_t min_port, uint16_t max_port)
{
	struct sa rtcp;
	int tries = 64;
	int err = 0;

	rs->local = rtcp = *ip;

	/* try hard */
	while (tries--) {
		struct udp_sock *us_rtp, *us_rtcp;
		uint16_t port;

		port = (min_port + (rand_u16() % (max_port - min_port)));
		port &= 0xfffe;

		sa_set_port(&rs->local, port);
		err = udpsock_listen(&us_rtp, &rs->local, udp_recv_handler, rs);
		if (err)
			continue;

		sa_set_port(&rtcp, port + 1);
		err = udpsock_listen(&us_rtcp, &rtcp, rtcp_recv_handler, rs);
		if (err) {
			mem_deref(us_rtp);
			continue;
		}

		/* OK */
		rs->sock_rtp = us_rtp;
		rs->sock_rtcp = us_rtcp;
		break;
	}

	return err;
}


/**
 * Allocate a new RTP socket
 *
 * @param rsp Pointer to returned RTP socket
 *
 * @return 0 for success, otherwise errorcode
 */
int rtp_alloc(struct rtp_sock **rsp)
{
	struct rtp_sock *rs;

	if (!rsp)
		return EINVAL;

	rs = mem_zalloc(sizeof(*rs), destructor);
	if (!rs)
		return ENOMEM;

	sa_init(&rs->rtcp_peer, AF_UNSPEC);

	rs->enc.seq  = rand_u16() & 0x7fff;
	rs->enc.ssrc = rand_u32();

	*rsp = rs;

	return 0;
}


/**
 * Listen on an RTP/RTCP Socket
 *
 * @param rsp         Pointer to returned RTP socket
 * @param proto       Transport protocol
 * @param ip          Local IP address
 * @param min_port    Minimum port range
 * @param max_port    Maximum port range
 * @param enable_rtcp True to enable RTCP Session
 * @param recvh       RTP Receive handler
 * @param rtcph       RTCP Receive handler
 * @param arg         Handler argument
 *
 * @return 0 for success, otherwise errorcode
 */
int rtp_listen(struct rtp_sock **rsp, int proto, const struct sa *ip,
	       uint16_t min_port, uint16_t max_port, bool enable_rtcp,
	       rtp_recv_h *recvh, rtcp_recv_h *rtcph, void *arg)
{
	struct rtp_sock *rs;
	int err;

	if (!ip || min_port >= max_port || !recvh)
		return EINVAL;

	err = rtp_alloc(&rs);
	if (err)
		return err;

	rs->proto = proto;
	rs->recvh = recvh;
	rs->rtcph = rtcph;
	rs->arg   = arg;

	/* Optional RTCP */
	if (enable_rtcp) {
		err = rtcp_sess_alloc(&rs->rtcp, rs);
		if (err)
			goto out;
	}

	switch (proto) {

	case IPPROTO_UDP:
		err = udp_range_listen(rs, ip, min_port, max_port);
		break;

	default:
		err = EPROTONOSUPPORT;
		break;
	}

 out:
	if (err)
		mem_deref(rs);
	else
		*rsp = rs;

	return err;
}


/**
 * Encode a new RTP header into the beginning of the buffer
 *
 * @param rs     RTP Socket
 * @param ext    Extension bit
 * @param marker Marker bit
 * @param pt     Payload type
 * @param ts     Timestamp
 * @param mb     Memory buffer
 *
 * @return 0 for success, otherwise errorcode
 *
 * @note The buffer must have enough space for the RTP header
 */
int rtp_encode(struct rtp_sock *rs, bool ext, bool marker, uint8_t pt,
	       uint32_t ts, struct mbuf *mb)
{
	struct rtp_header hdr;

	if (!rs || pt&~0x7f || !mb)
		return EINVAL;

	hdr.ver  = RTP_VERSION;
	hdr.pad  = false;
	hdr.ext  = ext;
	hdr.cc   = 0;
	hdr.m    = marker ? 1 : 0;
	hdr.pt   = pt;
	hdr.seq  = rs->enc.seq++;
	hdr.ts   = ts;
	hdr.ssrc = rs->enc.ssrc;

	return rtp_hdr_encode(mb, &hdr);
}


/**
 * Decode an RTP packet and return decoded RTP header and payload
 *
 * @param rs     RTP Socket
 * @param mb     Memory buffer containing RTP packet
 * @param hdr    RTP header (set on return)
 *
 * @return 0 for success, otherwise errorcode
 */
int rtp_decode(struct rtp_sock *rs, struct mbuf *mb,
	       struct rtp_header *hdr)
{
	int err;

	if (!rs || !mb || !hdr)
		return EINVAL;

	memset(hdr, 0, sizeof(*hdr));
	err = rtp_hdr_decode(hdr, mb);
	if (err)
		return err;

	if (RTP_VERSION != hdr->ver)
		return EBADMSG;

	return 0;
}


/**
 * Send an RTP packet to a peer
 *
 * @param rs     RTP Socket
 * @param dst    Destination address
 * @param ext    Extension bit
 * @param marker Marker bit
 * @param pt     Payload type
 * @param ts     Timestamp
 * @param mb     Payload buffer
 *
 * @return 0 for success, otherwise errorcode
 */
int rtp_send(struct rtp_sock *rs, const struct sa *dst, bool ext,
	     bool marker, uint8_t pt, uint32_t ts, struct mbuf *mb)
{
	size_t pos;
	int err;

	if (!rs || !mb)
		return EINVAL;

	if (mb->pos < RTP_HEADER_SIZE) {
		DEBUG_WARNING("rtp_send: buffer must have space for"
			      " rtp header (pos=%u, end=%u)\n",
			      mb->pos, mb->end);
		return EBADMSG;
	}

	mbuf_advance(mb, -RTP_HEADER_SIZE);

	pos = mb->pos;

	err = rtp_encode(rs, ext, marker, pt, ts, mb);
	if (err)
		return err;

	if (rs->rtcp)
		rtcp_sess_tx_rtp(rs->rtcp, ts, mbuf_get_left(mb));

	mb->pos = pos;

	return udpsock_send(rs->sock_rtp, dst, mb);
}


/**
 * Get the RTP transport socket from an RTP/RTCP Socket
 *
 * @param rs RTP Socket
 *
 * @return Transport socket for RTP
 */
void *rtp_sock(const struct rtp_sock *rs)
{
	return rs ? rs->sock_rtp : NULL;
}


/**
 * Get the RTCP transport socket from an RTP/RTCP Socket
 *
 * @param rs RTP Socket
 *
 * @return Transport socket for RTCP
 */
void *rtcp_sock(const struct rtp_sock *rs)
{
	return rs ? rs->sock_rtcp : NULL;
}


/**
 * Get the local RTP address for an RTP/RTCP Socket
 *
 * @param rs RTP Socket
 *
 * @return Local RTP address
 */
const struct sa *rtp_local(const struct rtp_sock *rs)
{
	return rs ? &rs->local : NULL;
}


/**
 * Get the Synchronizing source for an RTP/RTCP Socket
 *
 * @param rs RTP Socket
 *
 * @return Synchronizing source
 */
uint32_t rtp_sess_ssrc(const struct rtp_sock *rs)
{
	return rs ? rs->enc.ssrc : 0;
}


/**
 * Get the RTCP-Session for an RTP/RTCP Socket
 *
 * @param rs RTP Socket
 *
 * @return RTCP-Session
 */
struct rtcp_sess *rtp_rtcp_sess(const struct rtp_sock *rs)
{
	return rs ? rs->rtcp : NULL;
}


/**
 * Start the RTCP Session
 *
 * @param rs    RTP Socket
 * @param cname Canonical Name
 * @param peer  IP-Address of RTCP Peer
 */
void rtcp_start(struct rtp_sock *rs, const char *cname,
		const struct sa *peer)
{
	if (!rs)
		return;

	if (peer)
		rs->rtcp_peer = *peer;

	(void)rtcp_enable(rs->rtcp, peer != NULL, cname);
}


/**
 * Enable RTCP-multiplexing on RTP-port
 *
 * @param rs      RTP Socket
 * @param enabled True to enable, false to disable
 */
void rtcp_enable_mux(struct rtp_sock *rs, bool enabled)
{
	if (!rs)
		return;

	rs->rtcp_mux = enabled;
}


/**
 * Send RTCP packet(s) to the Peer
 *
 * @param rs RTP Socket
 * @param mb Buffer containing the RTCP Packet(s)
 *
 * @return 0 for success, otherwise errorcode
 */
int rtcp_send(struct rtp_sock *rs, struct mbuf *mb)
{
	if (!rs || !rs->sock_rtcp || !sa_isset(&rs->rtcp_peer, SA_ALL))
		return EINVAL;

	return udpsock_send(rs->rtcp_mux ? rs->sock_rtp : rs->sock_rtcp,
			&rs->rtcp_peer, mb);
}


/**
 * RTP Debug handler, use with fmt %H
 *
 * @param pf Print function
 * @param rs RTP Socket
 *
 * @return 0 if success, otherwise errorcode
 */
int rtp_debug(struct re_printf *pf, const struct rtp_sock *rs)
{
	int err;

	if (!rs || !pf)
		return EINVAL;

	err  = re_hprintf(pf, "RTP debug:\n");
	err |= re_hprintf(pf, " Encode: seq=%u ssrc=0x%lx\n",
			  rs->enc.seq, rs->enc.ssrc);

	if (rs->rtcp)
		err |= rtcp_debug(pf, rs);

	return err;
}
