/**
 * @file pkt.c  RTCP Packet handling
 *
 * Copyright (C) 2010 Creytiv.com
 */
#include <string.h>
#include <re_types.h>
#include <re_fmt.h>
#include <re_mem.h>
#include <re_mbuf.h>
#include <re_list.h>
#include <re_sys.h>
#include <re_sa.h>
#include <re_rtp.h>
#include "rtcp.h"


#define DEBUG_MODULE "rtcp_pkt"
#define DEBUG_LEVEL 5
#include <re_dbg.h>


static void rtcp_destructor(void *data)
{
	struct rtcp_msg *msg = data;
	size_t i, j;

	switch (msg->hdr.pt) {

	case RTCP_SR:
		mem_deref(msg->r.sr.rrv);
		break;

	case RTCP_RR:
		mem_deref(msg->r.rr.rrv);
		break;

	case RTCP_SDES:
		if (!msg->r.sdesv)
			break;

		for (i=0; i<msg->hdr.count; i++) {
			struct rtcp_sdes *sdes = &msg->r.sdesv[i];

			for (j=0; j<sdes->n; j++) {

				mem_deref(sdes->itemv[j].data);
			}
			mem_deref(sdes->itemv);
		}
		mem_deref(msg->r.sdesv);
		break;

	case RTCP_BYE:
		mem_deref(msg->r.bye.srcv);
		mem_deref(msg->r.bye.reason);
		break;

	case RTCP_APP:
		mem_deref(msg->r.app.data);
		break;

	case RTCP_RTPFB:
	case RTCP_PSFB:
		mem_deref(msg->r.fb.fci.p);
		break;

	default:
		/* nothing allocated */
		break;
	}
}


/**
 * Encode the RTCP Header
 *
 * @param mb     Buffer to encode into
 * @param count  Number of sub-elemements
 * @param type   RTCP Packet type
 * @param length Packet length in words
 *
 * @return 0 for success, otherwise errorcode
 */
int rtcp_hdr_encode(struct mbuf *mb, uint8_t count, enum rtcp_type type,
		    uint16_t length)
{
	int err;

	if (!mb)
		return EINVAL;

	err  = mbuf_write_u8(mb, RTCP_VERSION<<6 | count);
	err |= mbuf_write_u8(mb, type);
	err |= mbuf_write_u16(mb, htons(length));

	return err;
}


/**
 * Decode the RTCP Header
 *
 * @param mb  Buffer to decode from
 * @param hdr RTCP Header to decode into
 *
 * @return 0 for success, otherwise errorcode
 */
int rtcp_hdr_decode(struct mbuf *mb, struct rtcp_hdr *hdr)
{
	uint8_t b;

	if (!hdr)
		return EINVAL;
	if (mbuf_get_left(mb) < RTCP_HDR_SIZE)
		return EBADMSG;

	b = mbuf_read_u8(mb);
	hdr->pt = mbuf_read_u8(mb);
	hdr->length = ntohs(mbuf_read_u16(mb));

	hdr->version = (b >> 6) & 0x3;
	hdr->p       = (b >> 5) & 0x1;
	hdr->count   = (b >> 0) & 0x1f;

	return 0;
}


int rtcp_vencode(struct mbuf *mb, enum rtcp_type type, uint32_t count,
		 va_list ap)
{
	size_t i, pos;
	uint16_t len;
	const uint8_t *data;
	size_t data_len;
	const uint32_t *srcv;
	const char *reason;
	rtcp_encode_h *ench;
	void *arg;
	int err = 0;

	if (!mb)
		return EINVAL;

	pos = mb->pos;

	/* Skip header - encoded last */
	mb->pos = mb->end = (mb->pos + RTCP_HDR_SIZE);

	switch (type) {

	case RTCP_SR:
		for (i=0; i<6; i++)
			err |= mbuf_write_u32(mb, htonl(va_arg(ap, uint32_t)));
		ench = va_arg(ap, rtcp_encode_h *);
		arg = va_arg(ap, void *);
		if (ench)
			err |= ench(mb, arg);
		break;

	case RTCP_RR:
		err = mbuf_write_u32(mb, htonl(va_arg(ap, uint32_t)));
		ench = va_arg(ap, rtcp_encode_h *);
		arg = va_arg(ap, void *);
		if (ench)
			err |= ench(mb, arg);
		break;

	case RTCP_SDES:
		ench = va_arg(ap, rtcp_encode_h *);
		arg = va_arg(ap, void *);
		if (ench)
			err |= ench(mb, arg);
		break;

	case RTCP_BYE:
		srcv   = va_arg(ap, uint32_t *);
		reason = va_arg(ap, char *);
		for (i=0; i<count && !err; i++) {
			err = mbuf_write_u32(mb, htonl(srcv[i]));
		}
		if (reason) {
			err |= mbuf_write_u8(mb, strlen(reason));
			err |= mbuf_write_str(mb, reason);
		}
		break;

	case RTCP_APP:
		err  = mbuf_write_u32(mb, htonl(va_arg(ap, uint32_t)));
		err |= mbuf_write_mem(mb, va_arg(ap, uint8_t *), 4);
		data = va_arg(ap, const uint8_t *);
		data_len = va_arg(ap, size_t);
		if (data) {
			if (data_len % 4) {
				DEBUG_WARNING("not a multiple of 32bits\n");
				return EBADMSG;
			}
			err |= mbuf_write_mem(mb, data, data_len);
		}
		break;

	case RTCP_FIR:
		err  = mbuf_write_u32(mb, htonl(va_arg(ap, uint32_t)));
		break;

	case RTCP_NACK:
		err  = mbuf_write_u32(mb, htonl(va_arg(ap, uint32_t)));
		err |= mbuf_write_u16(mb, htons(va_arg(ap, uint32_t)));
		err |= mbuf_write_u16(mb, htons(va_arg(ap, uint32_t)));
		break;

	case RTCP_RTPFB:
	case RTCP_PSFB:
		err  = mbuf_write_u32(mb, htonl(va_arg(ap, uint32_t)));
		err |= mbuf_write_u32(mb, htonl(va_arg(ap, uint32_t)));
		ench = va_arg(ap, rtcp_encode_h *);
		arg = va_arg(ap, void *);
		if (ench)
			err |= ench(mb, arg);
		break;

	default:
		return EINVAL;
	}
	if (err)
		return err;

	/* padding to 32 bits */
	while ((mb->end - pos) & 0x3)
		err |= mbuf_write_u8(mb, 0x00);
	if (err)
		return err;

	/* Encode RTCP Header */
	mb->pos = pos;
	len = (mb->end - pos - RTCP_HDR_SIZE)/sizeof(uint32_t);
	err = rtcp_hdr_encode(mb, count, type, len);
	if (err)
		return err;

	mb->pos = mb->end;

	return 0;
}


/**
 * Encode an RTCP Packet into a buffer
 *
 * @param mb    Buffer to encode into
 * @param type  RTCP Packet type
 * @param count Packet-specific count
 *
 * @return 0 for success, otherwise errorcode
 */
int rtcp_encode(struct mbuf *mb, enum rtcp_type type, uint32_t count, ...)
{
	va_list ap;
	int err;

	va_start(ap, count);
	err = rtcp_vencode(mb, type, count, ap);
	va_end(ap);

	return err;
}


/**
 * Decode one RTCP message from a buffer
 *
 * @param msgp Pointer to allocated RTCP Message
 * @param mb   Buffer to decode from
 *
 * @return 0 for success, otherwise errorcode
 */
int rtcp_decode(struct rtcp_msg **msgp, struct mbuf *mb)
{
	struct rtcp_msg *msg = NULL;
	size_t start, i, sz, count, rem;
	int err;

	if (!msgp)
		return EINVAL;
	if (mbuf_get_left(mb) < RTCP_HDR_SIZE)
		return EBADMSG;

	msg = mem_zalloc(sizeof(*msg), rtcp_destructor);
	if (!msg)
		return ENOMEM;

	start = mb->pos;

	/* decode and check header */
	err = rtcp_hdr_decode(mb, &msg->hdr);
	if (err)
		goto out;

	if (msg->hdr.version != RTCP_VERSION)
		goto badmsg;

	/* check length and remaining */
	rem = msg->hdr.length * sizeof(uint32_t);
	if (mbuf_get_left(mb) < rem)
		goto badmsg;

	count = msg->hdr.count;

	switch (msg->hdr.pt) {

	case RTCP_SR:
		if (mbuf_get_left(mb) < (RTCP_SRC_SIZE + RTCP_SR_SIZE))
			goto badmsg;
		msg->r.sr.ssrc     = ntohl(mbuf_read_u32(mb));
		msg->r.sr.ntp_sec  = ntohl(mbuf_read_u32(mb));
		msg->r.sr.ntp_frac = ntohl(mbuf_read_u32(mb));
		msg->r.sr.rtp_ts   = ntohl(mbuf_read_u32(mb));
		msg->r.sr.psent    = ntohl(mbuf_read_u32(mb));
		msg->r.sr.osent    = ntohl(mbuf_read_u32(mb));

		err = rtcp_rr_alloc(&msg->r.sr.rrv, count);
		if (err)
			goto out;
		for (i=0; i<count && !err; i++)
			err = rtcp_rr_decode(mb, &msg->r.sr.rrv[i]);
		break;

	case RTCP_RR:
		if (mbuf_get_left(mb) < RTCP_SRC_SIZE)
			goto badmsg;
		msg->r.rr.ssrc = ntohl(mbuf_read_u32(mb));

		err = rtcp_rr_alloc(&msg->r.rr.rrv, count);
		if (err)
			goto out;
		for (i=0; i<count && !err; i++)
			err = rtcp_rr_decode(mb, &msg->r.rr.rrv[i]);
		break;

	case RTCP_SDES:
		if (count == 0)
			break;

		sz = count * sizeof(*msg->r.sdesv);
		msg->r.sdesv = mem_zalloc(sz, NULL);
		if (!msg->r.sdesv) {
			err = ENOMEM;
			goto out;
		}

		for (i=0; i<msg->hdr.count && !err; i++)
			err = rtcp_sdes_decode(mb, &msg->r.sdesv[i]);
		break;

	case RTCP_BYE:
		sz = count * sizeof(*msg->r.bye.srcv);
		msg->r.bye.srcv = mem_alloc(sz, NULL);
		if (!msg->r.bye.srcv) {
			err = ENOMEM;
			goto out;
		}
		if (mbuf_get_left(mb) < sz)
			goto badmsg;
		for (i=0; i<count; i++)
			msg->r.bye.srcv[i] = ntohl(mbuf_read_u32(mb));

		/* decode reason (optional) */
		if (rem > count*sizeof(uint32_t)) {
			const size_t len = mbuf_read_u8(mb);
			if (mbuf_get_left(mb) < len)
				goto badmsg;

			err = mbuf_strdup(mb, &msg->r.bye.reason, len);
		}
		break;

	case RTCP_APP:
		if (mbuf_get_left(mb) < RTCP_APP_SIZE)
			goto badmsg;
		msg->r.app.src = ntohl(mbuf_read_u32(mb));
		(void)mbuf_read_mem(mb, (uint8_t *)msg->r.app.name,
				    sizeof(msg->r.app.name));
		if (rem > RTCP_APP_SIZE) {
			msg->r.app.data_len = rem - RTCP_APP_SIZE;
			msg->r.app.data = mem_alloc(msg->r.app.data_len, NULL);
			if (!msg->r.app.data) {
				err = ENOMEM;
				goto out;
			}
			if (mbuf_get_left(mb) < msg->r.app.data_len)
				goto badmsg;
			(void)mbuf_read_mem(mb, msg->r.app.data,
					    msg->r.app.data_len);
		}
		break;

	case RTCP_FIR:
		if (mbuf_get_left(mb) < RTCP_FIR_SIZE)
			goto badmsg;
		msg->r.fir.ssrc = ntohl(mbuf_read_u32(mb));
		break;

	case RTCP_NACK:
		if (mbuf_get_left(mb) < RTCP_NACK_SIZE)
			goto badmsg;
		msg->r.nack.ssrc = ntohl(mbuf_read_u32(mb));
		msg->r.nack.fsn  = ntohs(mbuf_read_u16(mb));
		msg->r.nack.blp  = ntohs(mbuf_read_u16(mb));
		break;

	case RTCP_RTPFB:
		if (mbuf_get_left(mb) < RTCP_FB_SIZE)
			goto badmsg;

		if (msg->hdr.length < 2)
			goto badmsg;

		msg->r.fb.ssrc_packet = ntohl(mbuf_read_u32(mb));
		msg->r.fb.ssrc_media = ntohl(mbuf_read_u32(mb));
		msg->r.fb.n = msg->hdr.length - 2;

		err = rtcp_rtpfb_decode(mb, msg);
		break;

	case RTCP_PSFB:
		if (mbuf_get_left(mb) < RTCP_FB_SIZE)
			goto badmsg;

		if (msg->hdr.length < 2)
			goto badmsg;

		msg->r.fb.ssrc_packet = ntohl(mbuf_read_u32(mb));
		msg->r.fb.ssrc_media = ntohl(mbuf_read_u32(mb));
		msg->r.fb.n = msg->hdr.length - 2;

		err = rtcp_psfb_decode(mb, msg);
		break;

	default:
		/* unknown message type */
		mbuf_advance(mb, rem);
		break;
	}
	if (err)
		goto out;

	/* slurp padding */
	while ((mb->pos - start) & 0x3 && mbuf_get_left(mb))
		++mb->pos;

 out:
	if (err)
		mem_deref(msg);
	else
		*msgp = msg;

	return err;

 badmsg:
	mem_deref(msg);
	return EBADMSG;
}
