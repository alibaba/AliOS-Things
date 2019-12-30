/**
 * @file rtp.c RTP/RTCP Testcode
 *
 * Copyright (C) 2010 Creytiv.com
 */
#include <string.h>
#include <stddef.h>
#include <re_sa.h>
#include <re_mbuf.h>
#include <re_udp.h>
#include <re_rtp.h>
#include <stdio.h>

#include "aos/cli.h"
#include "aos/kernel.h"
#include "aos/yloop.h"
#include "ulog/ulog.h"
#include <network/network.h>
#include <netmgr.h>

#include "yunit/yunit.h"
#include "yunit/yts.h"

#define DEBUG_MODULE "rtptest"

#define DEBUG_WARNING(fmt, ...) LOGW(DEBUG_MODULE, fmt, ##__VA_ARGS__);

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a) ((sizeof(a))/(sizeof((a)[0])))
#endif

#define TEST_EQUALS(expected, actual)                           \
    if ((expected) != (actual)) {                               \
        (void)re_fprintf(stderr, "\n");                         \
        DEBUG_WARNING("TEST_EQUALS: %s:%u: %s():"               \
                      " expected=%d(0x%x), actual=%d(0x%x)\n",  \
                      __FILE__, __LINE__, __func__,             \
                      (expected), (expected),                   \
                      (actual), (actual));                      \
        err = EINVAL;                                           \
        goto out;                                               \
    }

enum {PAYLOAD_SIZE = 160};

/* @brief rtp testcase */
int yts_rtp(void)
{
    struct rtp_sock *rtp = NULL;
    struct mbuf *mb = NULL;
    static const uint8_t payload[PAYLOAD_SIZE];
    int j;
    int err;

    mb = mbuf_alloc(RTP_HEADER_SIZE);
    YUNIT_ASSERT( 0 != mb );
    if (!mb)
        return ENOMEM;

    err = rtp_alloc(&rtp);
    YUNIT_ASSERT( 0 == err );
    if (err)
        goto out;

    for (j=0; j<100; j++) {
        struct rtp_header hdr, hdr2;

        memset(&hdr, 0, sizeof(hdr));

        hdr.m  = j & 0x01;
        hdr.pt = j & 0x7f;
        hdr.ts = 160 + j;

        mb->pos = mb->end = RTP_HEADER_SIZE;
        err = mbuf_write_mem(mb, payload, sizeof(payload));
        YUNIT_ASSERT( 0 == err );
        if (err)
            break;

        mb->pos = 0;
        err = rtp_encode(rtp, false, hdr.m, hdr.pt, hdr.ts, mb);
        YUNIT_ASSERT( 0 == err );
        if (err)
            break;

        mb->pos = 0;

        err = rtp_decode(rtp, mb, &hdr2);
        YUNIT_ASSERT( 0 == err );
        if (err)
            break;

        if (hdr.m != hdr2.m) {
            DEBUG_WARNING("marker bit mismatch (%d != %d)\n", hdr.m, hdr2.m);
            err = EBADMSG;
            break;
        }

        if (hdr.pt != hdr2.pt) {
            DEBUG_WARNING("payload type mismatch (%u != %u)\n", hdr.pt, hdr2.pt);
            err = EBADMSG;
            break;
        }

        if (hdr.ts != hdr2.ts) {
            DEBUG_WARNING("timestamp mismatch (%lu != %lu)\n", hdr.ts, hdr2.ts);
            err = EBADMSG;
            break;
        }

        if (hdr2.pad) {
            DEBUG_WARNING("unexpected padding bit\n");
            err = EBADMSG;
            break;
        }

        if (hdr2.ext) {
            DEBUG_WARNING("unexpected extension bit\n");
            err = EBADMSG;
            break;
        }

        if (RTP_HEADER_SIZE != mb->pos || (RTP_HEADER_SIZE + PAYLOAD_SIZE) != mb->end) {
            DEBUG_WARNING("invalid mbuf size (pos=%u end=%u)\n", mb->pos, mb->end);
	    err = EBADMSG;
	    break;
        }

        if (0 != memcmp(mbuf_buf(mb), payload, sizeof(payload))) {
            DEBUG_WARNING("RTP payload mismatch\n");
            err = EBADMSG;
            break;
        }
    }

out:
    mem_deref(rtp);
    mem_deref(mb);

    return err;
}

static const uint8_t rtcp_msg[] =
	/* SR */
	"\x81\xc8\x00\x0c"
	"\x12\x34\x56\x78"
	"\x00\x11\x22\x33"
	"\x44\x55\x66\x77"
	"\x22\x33\x22\x33"
	"\x00\x00\x11\x11"
	"\x00\x00\x22\x22"
	"\x12\x34\x56\x78"
	"\x12\x34\x56\x78"
	"\x12\x34\x56\x78"
	"\x12\x34\x56\x78"
	"\x12\x34\x56\x78"
	"\x12\x34\x56\x78"

	/* RR */
	"\x81\xc9\x00\x07"
	"\x12\x34\x56\x78"
	"\x12\x34\x56\x78"
	"\x12\x34\x56\x78"
	"\x12\x34\x56\x78"
	"\x12\x34\x56\x78"
	"\x12\x34\x56\x78"
	"\x12\x34\x56\x78"

	/* SDES */
	"\x81\xca\x00\x03"
	"\xde\xad\xbe\xef"
	"\x01\x03\x61\x62"
	"\x63\x00\x00\x00"

	/* BYE */
	"\x82\xcb\x00\x04"
	"\x12\x34\x56\x78"
	"\x00\xab\xcd\xef"
	"\x04\x63\x69\x61"
	"\x6f\x00\x00\x00"

	/* APP */
	"\x80\xcc\x00\x03"
	"\x12\x34\x56\x78"
	"\x6e\x61\x6d\x65"
	"\x64\x61\x74\x61"

	/* FIR */
	"\x80\xc0\x00\x01"
	"\x12\x34\x56\x78"

	/* NACK */
	"\x80\xc1\x00\x02"
	"\x12\x34\x56\x78"
	"\x89\xab\xcd\xef"

	/* RTPFB */
	"\x81\xcd\x00\x05"
	"\x12\x34\x56\x78"
	"\xfe\xdc\xba\x98"
	"\x01\x23\x04\x56"
	"\x01\x23\x04\x56"
	"\x01\x23\x04\x56"

	/* PSFB - PLI */
	"\x81\xce\x00\x02"
	"\x12\x34\x56\x78"
	"\xfe\xdc\xba\x98"

	/* PSFB - SLI */
	"\x82\xce\x00\x04"
	"\x12\x34\x56\x78"
	"\xfe\xdc\xba\x98"
	"\xca\xfe\xca\xfe"
	"\xca\xfe\xca\xfe"

	"";

/* @brief encode handler*/
static int encode_handler(struct mbuf *mb, void *arg)
{
    int err = 0;
    size_t i;

    (void)arg;

    for (i=0; i<6 && !err; i++) {
        err = mbuf_write_u32(mb, htonl(0x12345678));
    }
    YUNIT_ASSERT( 0 == err );
    return err;
}

/* @brief sdes encode handler*/
static int sdes_encode_handler(struct mbuf *mb, void *arg)
{
    (void)arg;
    return rtcp_sdes_encode(mb, 0xdeadbeef, 1, RTCP_SDES_CNAME, "abc");
}

/* @brief gnack encode handler*/
static int gnack_encode(struct mbuf *mb, void *arg)
{
    int err = 0, n=3;
    (void)arg;

    while (n--) {
        err |= mbuf_write_u16(mb, htons(0x0123));
        err |= mbuf_write_u16(mb, htons(0x0456));
    }

    YUNIT_ASSERT( 0 == err );
    return err;
}

/* @brief sli encode handler*/
static int sli_encode(struct mbuf *mb, void *arg)
{
    int err = 0, n=2;
    (void)arg;

    while (n--) {
        err |= mbuf_write_u32(mb, htonl(0xcafecafe));
    }

    YUNIT_ASSERT( 0 == err );
    return err;
}

/* @brief rtcp encode testcase */
int yts_rtcp_encode(void)
{
    struct mbuf *mb;
    const size_t sz = sizeof(rtcp_msg) - 1;
    const uint32_t srcv[2] = {0x12345678, 0x00abcdef};
    int err = 0;

    mb = mbuf_alloc(512);
    YUNIT_ASSERT( 0 != mb );
    if (!mb)
        return ENOMEM;

    err |= rtcp_encode(mb, RTCP_SR, 1, 0x12345678, 0x00112233,
                       0x44556677, 0x22332233, 0x00001111, 0x00002222,
                       encode_handler, 0);
    err |= rtcp_encode(mb, RTCP_RR, 1, 0x12345678, encode_handler, 0);
    err |= rtcp_encode(mb, RTCP_SDES, 1, sdes_encode_handler, 0);
    err |= rtcp_encode(mb, RTCP_BYE, 2, srcv, "ciao");
    err |= rtcp_encode(mb, RTCP_APP, 0, 0x12345678, "name", "data",
			   (size_t)4);
    err |= rtcp_encode(mb, RTCP_FIR, 0, 0x12345678);
    err |= rtcp_encode(mb, RTCP_NACK, 0, 0x12345678, 0x89ab, 0xcdef);
    err |= rtcp_encode(mb, RTCP_RTPFB, RTCP_RTPFB_GNACK,
			   0x12345678, 0xfedcba98, gnack_encode, 0);
    err |= rtcp_encode(mb, RTCP_PSFB, RTCP_PSFB_PLI,
			   0x12345678, 0xfedcba98, NULL, 0);
    err |= rtcp_encode(mb, RTCP_PSFB, RTCP_PSFB_SLI,
			   0x12345678, 0xfedcba98, sli_encode, 0);
    YUNIT_ASSERT( 0 == err );
    if (err)
        goto out;

    if (mb->end != sz) {
        err = EPROTO;
    }
    if (0 != memcmp(mb->buf, rtcp_msg, mb->end)) {
        err = EBADMSG;
    }

    if (err) {
        DEBUG_WARNING("encode error: %m\n", err);
        hexdump(stderr, mb->buf, mb->end);
    }

    mb->pos = 0;
    while (mbuf_get_left(mb) >= 4 && !err) {
        struct rtcp_msg *msg = NULL;
        err = rtcp_decode(&msg, mb);
        msg = mem_deref(msg);
    }
    YUNIT_ASSERT( 0 == err );
    if (err)
        goto out;

    /* verify that rtcp_decode() read the whole buffer */
    TEST_EQUALS(mb->end, mb->pos);

out:
    mem_deref(mb);
    return err;
}

static const uint8_t rtcp_sdes[] =
	/* SDES */
	"\x83\xca\x00\x09"
	"\x11\x22\x33\x44"
	"\x01\x02\x41\x61"  /* cname */
	"\x00\x00\x00\x00"
	"\x55\x66\x77\x88"
	"\x07\x02\x42\x62"  /* note */
	"\x02\x01\x43\x00"  /* name */
	"\xaa\xbb\xcc\xdd"
	"\x04\x05\x31\x32"  /* phone */
	"\x33\x34\x35\x00"

	/* APP */
	"\x80\xcc\x00\x03"
	"\x12\x34\x56\x78"
	"\x6e\x61\x6d\x65"
	"\x64\x61\x74\x61"
	"";


static int test_rtcp_decode_badmsg(void)
{
    struct rtcp_msg *msg = NULL;
    uint32_t ssrc = 0xcafebabe;
    struct mbuf *mb;
    int err = 0;

    mb = mbuf_alloc(128);
    YUNIT_ASSERT( 0 != mb );
    if (!mb) {
        err = ENOMEM;
        goto out;
    }

    err = rtcp_encode(mb, RTCP_PSFB, RTCP_PSFB_SLI,
                      ssrc, ssrc, NULL, NULL);
    YUNIT_ASSERT( 0 == err );
    if (err)
        goto out;

    /* simulate a corrupt RTCP packet */
    mb->pos = 2;
    (void)mbuf_write_u16(mb, htons(0));

    mb->pos = 0;

    err = rtcp_decode(&msg, mb);
    YUNIT_ASSERT( EBADMSG == err );

    if (EBADMSG != err) {
        err = EBADMSG;
        goto out;
    }

out:
    mem_deref(msg);
    mem_deref(mb);

    return err;
}

int yts_rtcp_decode(void)
{
    struct rtcp_msg *msg = NULL;
    struct mbuf *mb;
    int err = 0;

    mb = mbuf_alloc(512);
    YUNIT_ASSERT( NULL != mb );
    if (!mb)
        return ENOMEM;

    err |= mbuf_write_u8(mb, 0x55); /* overhead -- test padding */
    err |= mbuf_write_mem(mb, rtcp_sdes, sizeof(rtcp_sdes));
    err |= mbuf_write_u8(mb, 0xaa); /* junk */
    mb->pos = 1;

    YUNIT_ASSERT( 0 == err );
    while (mbuf_get_left(mb) >= 4 && !err) {
        err = rtcp_decode(&msg, mb);
        msg = mem_deref(msg);
    }

    mem_deref(msg);
    mem_deref(mb);

    YUNIT_ASSERT( 0 == err );
    if (err)
        return err;

    err = test_rtcp_decode_badmsg();
    YUNIT_ASSERT( 0 == err );
    if (err)
        return err;

    return err;
}

static int afb_encode_handler(struct mbuf *mb, void *arg)
{
    return mbuf_write_str(mb, arg);
}

int yts_rtcp_encode_afb(void)
{
    uint32_t ssrc_packet_sender, ssrc_media_source;
    const char *afb_payload = "AFB tull";
    struct rtcp_msg *msg = NULL;
    struct mbuf *mb;
    int err = 0;

    mb = mbuf_alloc(512);
    YUNIT_ASSERT( NULL != mb );
    if (!mb)
        return ENOMEM;

    ssrc_packet_sender = 0xbad00bad;
    ssrc_media_source = 0; /* always 0 */
    err = rtcp_encode(mb, RTCP_PSFB, RTCP_PSFB_AFB,
                      ssrc_packet_sender, ssrc_media_source,
                      afb_encode_handler, afb_payload);
    YUNIT_ASSERT( 0 == err );
    if (err)
        goto out;

    mb->pos = 0;
    err = rtcp_decode(&msg, mb);
    YUNIT_ASSERT( 0 == err );
    if (err)
        goto out;

    YUNIT_ASSERT( RTCP_PSFB_AFB == msg->hdr.count );
    if (msg->hdr.count != RTCP_PSFB_AFB) {
        DEBUG_WARNING("expected AFB, got fmt=%u\n", msg->hdr.count);
        err = EPROTO;
        goto out;
    }

    YUNIT_ASSERT( (ssrc_packet_sender == msg->r.fb.ssrc_packet)
                 || (ssrc_media_source == msg->r.fb.ssrc_media) );
    if (msg->r.fb.ssrc_packet != ssrc_packet_sender ||
        msg->r.fb.ssrc_media  != ssrc_media_source) {
        DEBUG_WARNING("error in SSRC encoding\n");
        err = EBADMSG;
        goto out;
    }

    YUNIT_ASSERT( (0 == msg->r.fb.fci.afb)
                 || (strlen(afb_payload) == mbuf_get_left(msg->r.fb.fci.afb)) );
    if (!msg->r.fb.fci.afb ||
        mbuf_get_left(msg->r.fb.fci.afb) != strlen(afb_payload)) {
        DEBUG_WARNING("error in AFB mbuf (left=%u, size=%u)\n",
                      mbuf_get_left(msg->r.fb.fci.afb),strlen(afb_payload));
        err = EBADMSG;
        goto out;
    }

    YUNIT_ASSERT( 0 == memcmp(mbuf_buf(msg->r.fb.fci.afb), afb_payload,
                     strlen(afb_payload)) );
    if (0 != memcmp(mbuf_buf(msg->r.fb.fci.afb), afb_payload,
                     strlen(afb_payload))) {
        DEBUG_WARNING("error in AFB mbuf content\n");
        err = EBADMSG;
        goto out;
    }

    /* verify that rtcp_decode() read the whole buffer */
    TEST_EQUALS(mb->end, mb->pos);

out:
    mem_deref(mb);
    mem_deref(msg);
    return err;
}

/*
 *  .------.       RTP
 *  | RTP  | <--------------- [ Packet Generator ]
 *  | RTCP |
 *  '------'
 */


#define GENERATOR_SSRC 0x00000001

#define TEST_ERR(err)                                       \
        if ((err)) {                                        \
            (void)re_fprintf(stderr, "\n");                 \
            DEBUG_WARNING("TEST_ERR: %s:%u:"                \
                          " (%m)\n",                        \
                           __FILE__, __LINE__,              \
                          (err));                           \
            goto out;                                       \
        }

struct fixture {
    struct rtp_sock *rtp;
    struct sa rtp_addr;
    size_t n_recv;
    size_t num_packets;
};

static void signal_handler(int sig)
{
    re_fprintf(stderr, "test interrupted by signal %d\n", sig);
    re_cancel();
}

static void oom_watchdog_timeout(void *arg1, void *arg2)
{
    re_cancel();
}

static uint32_t timeout_override = 10000;

int re_main_timeout(uint32_t timeout_ms)
{
    aos_timer_t tmr;
    int ret = 0;

#ifdef TEST_TIMEOUT
    timeout_ms = TEST_TIMEOUT;
#endif
    if (timeout_override != 0)
        timeout_ms = timeout_override;

    ret = aos_timer_new(&tmr, oom_watchdog_timeout, NULL, timeout_ms, 0);
    if(ret != 0) {
        goto exit;
    }

    ret = aos_timer_start(&tmr);
    if(ret != 0) {
        goto exit;
    }

    (void)re_main(signal_handler);

    ret = aos_timer_stop(&tmr);
    if(ret != 0) {
       goto exit;
    }
exit:
    aos_timer_free(&tmr);
    return ret;
}

static int send_rtp_packet(const struct sa *dst, uint16_t seq, uint32_t ssrc)
{
    struct rtp_header hdr;
    struct mbuf *mb = mbuf_alloc(256);
    int err;

    YUNIT_ASSERT( NULL != mb ); 
    if (!mb)
        return ENOMEM;

    memset(&hdr, 0, sizeof(hdr));
    hdr.ver  = RTP_VERSION;
    hdr.seq  = seq;
    hdr.ts   = 0;
    hdr.ssrc = ssrc;

    err = rtp_hdr_encode(mb, &hdr);
    YUNIT_ASSERT( 0 == err ); 
    if (err)
        goto out;
    mbuf_fill(mb, 160, 0x00);
    mb->pos = 0;

    err = udpsock_send_anon(dst, mb);
    YUNIT_ASSERT( 0 == err ); 
    if (err)
        goto out;

out:
    mem_deref(mb);
    return err;
}

static void rtp_recv(const struct sa *src, const struct rtp_header *hdr,
		     struct mbuf *mb, void *arg)
{
    struct fixture *f = arg;
    (void)src;
    (void)hdr;
    (void)mb;

    ++f->n_recv;

    if (f->n_recv >= f->num_packets) {
        re_cancel();
    }
}

static int fixture_init(struct fixture *f)
{
    int err;

    memset(f, 0, sizeof(*f));

    err = sa_set_str(&f->rtp_addr, "127.0.0.1", 0);
    YUNIT_ASSERT( 0 == err ); 
    TEST_ERR(err);

    err = rtp_listen(&f->rtp, IPPROTO_UDP, &f->rtp_addr, 10000, 49152,
			 true, rtp_recv, NULL, f);
    YUNIT_ASSERT( 0 == err ); 
    if (err)
        goto out;

    err = udpsock_local_get(rtp_sock(f->rtp), &f->rtp_addr);
    YUNIT_ASSERT( 0 == err ); 
    TEST_ERR(err);

out:
    return err;
}

static void fixture_close(struct fixture *f)
{
    mem_deref(f->rtp);
}


static int test_loss(const uint16_t *seqv, size_t seqc,
		     int exp_lost)
{
    struct fixture fix, *f = &fix;
    struct rtcp_stats stats;
    uint32_t ssrc = GENERATOR_SSRC;
    unsigned i;
    int err, e;

    err = fixture_init(f);
    if (err)
        goto out;

    f->num_packets = seqc;

    /* no sources exist yet */
    e = rtcp_stats(f->rtp, ssrc, &stats);
    TEST_EQUALS(ENOENT, e);

    /* start the RTP packet generator, send X number of RTP packets
     * to the RTP-stack (fixture)
     */
    for (i=0; i<seqc; i++) {
        uint16_t seq = seqv[i];

        err = send_rtp_packet(&f->rtp_addr, seq, ssrc);
        YUNIT_ASSERT( 0 == err ); 
        if (err)
            goto out;
    }

    err = re_main_timeout(200);
    TEST_ERR(err);

    err = rtcp_stats(f->rtp, ssrc, &stats);
    YUNIT_ASSERT( 0 == err ); 
    TEST_ERR(err);

    /* in OOM-test, detect if member/sender was not allocated */
    if (stats.rx.sent == 0 && stats.rx.lost == 0 && stats.rx.jit == 0) {
        err = ENOMEM;
        goto out;
    }

    /* verify expected packets sent and packet loss */
    TEST_EQUALS(seqc, stats.rx.sent);
    TEST_EQUALS(exp_lost, stats.rx.lost);

    TEST_EQUALS(seqc, f->n_recv);

out:
    fixture_close(f);
    return err;
}

int yts_rtcp_packetloss(void)
{
    static const uint16_t seqv1[] = {0, 1, 2};
    static const uint16_t seqv2[] = {0,1,3,2,5,4};
    static const uint16_t seqv3[] = {65534, 65535, 0, 1};
    static const uint16_t seqv4[] = {65534, 0, 1};
    static const uint16_t seqv5[] = {65534, 1, 2};
    static const uint16_t seqv6[] = {65534, 1, 2, 65535};
    static const uint16_t seqv7[] = {1,2,8,9,10};
    int err = 0;

    err |= test_loss(seqv1, ARRAY_SIZE(seqv1), 0);
    err |= test_loss(seqv2, ARRAY_SIZE(seqv2), 0);
    err |= test_loss(seqv3, ARRAY_SIZE(seqv3), 0);
    err |= test_loss(seqv4, ARRAY_SIZE(seqv4), 1);
    err |= test_loss(seqv5, ARRAY_SIZE(seqv5), 2);
    err |= test_loss(seqv6, ARRAY_SIZE(seqv6), 1);
    err |= test_loss(seqv7, ARRAY_SIZE(seqv7), 5);

    YUNIT_ASSERT( 0 == err ); 
    return err;
}

static void init(void)
{
}

static int cleanup(void)
{
    return 0;
}

static void setup(void)
{
}

static void teardown(void)
{
}
/*
 * NULL terminated array of tests.
 * The first item in the array is a char[] which give some information about
 * what is being tested and is displayed to the user during runtime, the second
 * item is the test function.
 */
static yunit_test_case_t aos_rtp_testcases[] = {
    { "rtp test",
      yts_rtp },
    { "rtcp encode test",
      yts_rtcp_encode },
    { "rtcp decode test",
      yts_rtcp_decode },
    { "rtcp encode afb test",
      yts_rtcp_encode_afb },
    { "rtcp packetloss test",
      yts_rtcp_packetloss },
    YUNIT_TEST_CASE_NULL
};

static yunit_test_suite_t suites[] = {
    { "rtp", init, cleanup, setup, teardown, aos_rtp_testcases },
    YUNIT_TEST_SUITE_NULL
};

void test_rtp(void)
{
    yunit_add_test_suites(suites);
}

AOS_TESTCASE(test_rtp);

