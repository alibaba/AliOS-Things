/**
 * @file sdes.c  RTCP Source Description
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
#include <re_rtp.h>
#include "rtcp.h"


#define DEBUG_MODULE "rtcp_sdes"
#define DEBUG_LEVEL 5
#include <re_dbg.h>


enum {
	RTCP_SDES_MIN_SIZE = 1,
};


/**
 * Encode one SDES chunk into mbuffer
 *
 * @param mb    Buffer to encode into
 * @param src   First SSRC/CSRC
 * @param itemc Number of SDES items to encode
 *
 * @return 0 if success, otherwise errorcode
 */
int rtcp_sdes_encode(struct mbuf *mb, uint32_t src, uint32_t itemc, ...)
{
	va_list ap;
	size_t start;
	int err = 0;

	if (!mb || !itemc)
		return EINVAL;

	va_start(ap, itemc);

	start = mb->pos;
	err = mbuf_write_u32(mb, htonl(src));

	/* add all SDES items */
	while (itemc-- && !err) {
		const uint8_t type = va_arg(ap, int);
		const char *v = va_arg(ap, const char *);
		size_t len;
		if (!v)
			continue;

		len = strlen(v); /* note: max 255 chars */
		if (len > 255) {
			err = EINVAL;
			goto out;
		}

		err  = mbuf_write_u8(mb, type);
		err |= mbuf_write_u8(mb, len & 0xff);
		err |= mbuf_write_mem(mb, (uint8_t *)v, len);
	}

	/* END padding */
	err |= mbuf_write_u8(mb, RTCP_SDES_END);
	while ((mb->pos - start) & 0x3)
		err |= mbuf_write_u8(mb, RTCP_SDES_END);

 out:
	va_end(ap);

	return err;
}


/**
 * Decode SDES items from a buffer
 *
 * @param mb   Buffer to decode from
 * @param sdes RTCP SDES to decode into
 *
 * @return 0 if success, otherwise errorcode
 */
int rtcp_sdes_decode(struct mbuf *mb, struct rtcp_sdes *sdes)
{
	size_t start;

	if (!sdes)
		return EINVAL;
	if (mbuf_get_left(mb) < RTCP_SRC_SIZE)
		return EBADMSG;

	start = mb->pos;
	sdes->src = ntohl(mbuf_read_u32(mb));

	/* Decode all SDES items */
	while (mbuf_get_left(mb) >= RTCP_SDES_MIN_SIZE) {
		uint8_t type;
		struct rtcp_sdes_item *item;

		type = mbuf_read_u8(mb);
		if (type == RTCP_SDES_END)
			break;

		if (mbuf_get_left(mb) < 1)
			return EBADMSG;

		if (!sdes->itemv) {
			sdes->itemv = mem_alloc(sizeof(*sdes->itemv), NULL);
			if (!sdes->itemv)
				return ENOMEM;
		}
		else {
			const size_t sz = (sdes->n + 1) * sizeof(*sdes->itemv);
			struct rtcp_sdes_item *itemv;

			itemv = mem_realloc(sdes->itemv, sz);
			if (!itemv)
				return ENOMEM;

			sdes->itemv = itemv;
		}

		item = &sdes->itemv[sdes->n];

		item->type = (enum rtcp_sdes_type)type;
		item->length = mbuf_read_u8(mb);
		if (mbuf_get_left(mb) < item->length)
			return EBADMSG;
		item->data = mem_alloc(item->length, NULL);
		if (!item->data)
			return ENOMEM;
		(void)mbuf_read_mem(mb, (uint8_t *)item->data, item->length);

		sdes->n++;
	}

	/* slurp padding */
	while ((mb->pos - start) & 0x3 && mbuf_get_left(mb))
		++mb->pos;

	return 0;
}
