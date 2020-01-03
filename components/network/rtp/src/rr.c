/**
 * @file rtp/rr.c  RTCP Reception report
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
#include <re_rtp.h>
#include "rtcp.h"


int rtcp_rr_alloc(struct rtcp_rr **rrp, size_t count)
{
	struct rtcp_rr *rr;

	if (!rrp)
		return EINVAL;

	rr = mem_alloc(count * sizeof(*rr), NULL);
	if (!rr)
		return ENOMEM;

	*rrp = rr;
	return 0;
}


int rtcp_rr_encode(struct mbuf *mb, const struct rtcp_rr *rr)
{
	int err;

	if (!mb || !rr)
		return EINVAL;

	err  = mbuf_write_u32(mb, htonl(rr->ssrc));
	err |= mbuf_write_u32(mb, htonl(rr->fraction<<24 |
					(rr->lost & 0x00ffffff)));
	err |= mbuf_write_u32(mb, htonl(rr->last_seq));
	err |= mbuf_write_u32(mb, htonl(rr->jitter));
	err |= mbuf_write_u32(mb, htonl(rr->lsr));
	err |= mbuf_write_u32(mb, htonl(rr->dlsr));

	return err;
}


int rtcp_rr_decode(struct mbuf *mb, struct rtcp_rr *rr)
{
	uint32_t w;

	if (!rr)
		return EINVAL;
	if (mbuf_get_left(mb) < RTCP_RR_SIZE)
		return EBADMSG;

	rr->ssrc     = ntohl(mbuf_read_u32(mb));
	w = ntohl(mbuf_read_u32(mb));
	rr->fraction = w>>24; rr->lost = w & 0x00ffffffU;
	rr->last_seq = ntohl(mbuf_read_u32(mb));
	rr->jitter   = ntohl(mbuf_read_u32(mb));
	rr->lsr      = ntohl(mbuf_read_u32(mb));
	rr->dlsr     = ntohl(mbuf_read_u32(mb));

	return 0;
}
