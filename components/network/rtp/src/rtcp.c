/**
 * @file rtcp.c  Real-time Transport Control Protocol
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


static int rtcp_quick_send(struct rtp_sock *rs, enum rtcp_type type,
			   uint32_t count, ...)
{
	struct mbuf *mb;
	va_list ap;
	int err;

	mb = mbuf_alloc(32);
	if (!mb)
		return ENOMEM;

	mb->pos = RTCP_HEADROOM;

	va_start(ap, count);
	err = rtcp_vencode(mb, type, count, ap);
	va_end(ap);

	mb->pos = RTCP_HEADROOM;

	if (!err)
		err = rtcp_send(rs, mb);

	mem_deref(mb);

	return err;
}


/**
 * Send an RTCP Application-Defined (APP) packet
 *
 * @param rs   RTP Socket
 * @param name Ascii name (4 octets)
 * @param data Application-dependent data
 * @param len  Number of bytes of data
 *
 * @return 0 for success, otherwise errorcode
 */
int rtcp_send_app(struct rtp_sock *rs, const char name[4],
		  const uint8_t *data, size_t len)
{
	return rtcp_quick_send(rs, RTCP_APP, 0, rtp_sess_ssrc(rs),
			       name, data, len);
}


/**
 * Send a Full INTRA-frame Request (FIR) packet
 *
 * @param rs   RTP Socket
 * @param ssrc Synchronization source identifier for the sender of this packet
 *
 * @return 0 for success, otherwise errorcode
 */
int rtcp_send_fir(struct rtp_sock *rs, uint32_t ssrc)
{
	return rtcp_quick_send(rs, RTCP_FIR, 0, ssrc);
}


/**
 * Send an RTCP NACK packet
 *
 * @param rs   RTP Socket
 * @param fsn  First Sequence Number lost
 * @param blp  Bitmask of lost packets
 *
 * @return 0 for success, otherwise errorcode
 */
int rtcp_send_nack(struct rtp_sock *rs, uint16_t fsn, uint16_t blp)
{
	return rtcp_quick_send(rs, RTCP_NACK, 0, rtp_sess_ssrc(rs), fsn, blp);
}


/**
 * Send an RTCP Picture Loss Indication (PLI) packet
 *
 * @param rs      RTP Socket
 * @param fb_ssrc Feedback SSRC
 *
 * @return 0 for success, otherwise errorcode
 */
int rtcp_send_pli(struct rtp_sock *rs, uint32_t fb_ssrc)
{
	return rtcp_quick_send(rs, RTCP_PSFB, RTCP_PSFB_PLI,
			       rtp_sess_ssrc(rs), fb_ssrc, NULL, NULL);
}


const char *rtcp_type_name(enum rtcp_type type)
{
	switch (type) {

	case RTCP_FIR:   return "FIR";
	case RTCP_NACK:  return "NACK";
	case RTCP_SR:    return "SR";
	case RTCP_RR:    return "RR";
	case RTCP_SDES:  return "SDES";
	case RTCP_BYE:   return "BYE";
	case RTCP_APP:   return "APP";
	case RTCP_RTPFB: return "RTPFB";
	case RTCP_PSFB:  return "PSFB";
	case RTCP_XR:    return "XR";
	case RTCP_AVB:   return "AVB";
	default:         return "?";
	}
}


const char *rtcp_sdes_name(enum rtcp_sdes_type sdes)
{
	switch (sdes) {

	case RTCP_SDES_END:    return "END";
	case RTCP_SDES_CNAME:  return "CNAME";
	case RTCP_SDES_NAME:   return "NAME";
	case RTCP_SDES_EMAIL:  return "EMAIL";
	case RTCP_SDES_PHONE:  return "PHONE";
	case RTCP_SDES_LOC:    return "LOC";
	case RTCP_SDES_TOOL:   return "TOOL";
	case RTCP_SDES_NOTE:   return "NOTE";
	case RTCP_SDES_PRIV:   return "PRIV";
	default:               return "?";
	}
}


/**
 * Print an RTCP Message
 *
 * @param pf  Print handler for debug output
 * @param msg RTCP Message
 *
 * @return 0 if success, otherwise errorcode
 */
int rtcp_msg_print(struct re_printf *pf, const struct rtcp_msg *msg)
{
	size_t i, j;
	int err;

	if (!msg)
		return 0;

	err = re_hprintf(pf, "%8s pad=%d count=%-2d pt=%-3d len=%u ",
			 rtcp_type_name((enum rtcp_type)msg->hdr.pt),
			 msg->hdr.p,
			 msg->hdr.count, msg->hdr.pt, msg->hdr.length);
	if (err)
		return err;

	switch (msg->hdr.pt) {

	case RTCP_SR:
		err = re_hprintf(pf, "%08x %u %u %u %u %u",
				 msg->r.sr.ssrc,
				 msg->r.sr.ntp_sec,
				 msg->r.sr.ntp_frac,
				 msg->r.sr.rtp_ts,
				 msg->r.sr.psent,
				 msg->r.sr.osent);
		for (i=0; i<msg->hdr.count && !err; i++) {
			const struct rtcp_rr *rr = &msg->r.sr.rrv[i];
			err = re_hprintf(pf, " {%08x %u %d %u %u %u %u}",
					 rr->ssrc, rr->fraction, rr->lost,
					 rr->last_seq, rr->jitter,
					 rr->lsr, rr->dlsr);
		}
		break;

	case RTCP_RR:
		err = re_hprintf(pf, "%08x", msg->r.rr.ssrc);
		for (i=0; i<msg->hdr.count && !err; i++) {
			const struct rtcp_rr *rr = &msg->r.rr.rrv[i];
			err = re_hprintf(pf, " {0x%08x %u %d %u %u %u %u}",
					 rr->ssrc, rr->fraction, rr->lost,
					 rr->last_seq, rr->jitter,
					 rr->lsr, rr->dlsr);
		}
		break;

	case RTCP_SDES:
		for (i=0; i<msg->hdr.count; i++) {
			const struct rtcp_sdes *sdes = &msg->r.sdesv[i];

			err = re_hprintf(pf, " {0x%08x n=%u",
					 sdes->src, sdes->n);
			for (j=0; j<sdes->n && !err; j++) {
				const struct rtcp_sdes_item *item;
				item = &sdes->itemv[j];
				err = re_hprintf(pf, " <%s:%b>",
						 rtcp_sdes_name(item->type),
						 item->data,
						 (size_t)item->length);
			}
			err |= re_hprintf(pf, "}");
		}
		break;

	case RTCP_BYE:
		err = re_hprintf(pf, "%u srcs:", msg->hdr.count);
		for (i=0; i<msg->hdr.count && !err; i++) {
			err = re_hprintf(pf, " %08x",
					 msg->r.bye.srcv[i]);
		}
		err |= re_hprintf(pf, " '%s'", msg->r.bye.reason);
		break;

	case RTCP_APP:
		err = re_hprintf(pf, "src=%08x '%b' data=%zu",
				 msg->r.app.src,
				 msg->r.app.name, sizeof(msg->r.app.name),
				 msg->r.app.data_len);
		break;

	case RTCP_FIR:
		err = re_hprintf(pf, "ssrc=%08x", msg->r.fir.ssrc);
		break;

	case RTCP_NACK:
		err = re_hprintf(pf, "ssrc=%08x fsn=%04x blp=%04x",
				 msg->r.nack.ssrc, msg->r.nack.fsn,
				 msg->r.nack.blp);
		break;

	case RTCP_RTPFB:
		err = re_hprintf(pf, "pkt=%08x med=%08x n=%u",
				 msg->r.fb.ssrc_packet,
				 msg->r.fb.ssrc_media,
				 msg->r.fb.n);
		if (msg->hdr.count == RTCP_RTPFB_GNACK) {
			err |= re_hprintf(pf, " GNACK");
			for (i=0; i<msg->r.fb.n; i++) {
				err |= re_hprintf(pf, " {%04x %04x}",
						  msg->r.fb.fci.gnackv[i].pid,
						  msg->r.fb.fci.gnackv[i].blp);
			}
		}
		break;

	case RTCP_PSFB:
		err = re_hprintf(pf, "pkt=%08x med=%08x n=%u",
				 msg->r.fb.ssrc_packet,
				 msg->r.fb.ssrc_media,
				 msg->r.fb.n);
		if (msg->hdr.count == RTCP_PSFB_SLI) {
			err |= re_hprintf(pf, " SLI");
			for (i=0; i<msg->r.fb.n; i++) {
				err |= re_hprintf(pf, " {%04x %04x %02x}",
						  msg->r.fb.fci.sliv[i].first,
						  msg->r.fb.fci.sliv[i].number,
						  msg->r.fb.fci.sliv[i].picid);
			}
		}
		else if (msg->hdr.count == RTCP_PSFB_AFB) {
			err |= re_hprintf(pf, " AFB %u bytes",
					  msg->r.fb.n * 4);
		}
		break;

	default:
		err = re_hprintf(pf, "<len=%u>", msg->hdr.length);
		break;
	}

	err |= re_hprintf(pf, "\n");

	return err;
}
