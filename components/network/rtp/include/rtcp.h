/**
 * @file rtcp.h  Internal interface to RTCP
 *
 * Copyright (C) 2010 Creytiv.com
 */


/** RTCP protocol values */
enum {
	RTCP_HDR_SIZE  =   4,  /**< Size of common RTCP header   */
	RTCP_SRC_SIZE  =   4,  /**< Size of Source field         */
	RTCP_SR_SIZE   =  20,  /**< Size of Sender Information   */
	RTCP_RR_SIZE   =  24,  /**< Size of Report Block         */
	RTCP_APP_SIZE  =   8,  /**< Size of Application packet   */
	RTCP_FIR_SIZE  =   4,  /**< Size of FIR packet           */
	RTCP_NACK_SIZE =   8,  /**< Size of NACK packet          */
	RTCP_FB_SIZE   =   8,  /**< Size of Feedback packets     */
	RTCP_MAX_SDES  = 255,  /**< Maximum text length for SDES */
	RTCP_HEADROOM  =   4,  /**< Headroom in RTCP packets     */
};

/** NTP Time */
struct ntp_time {
	uint32_t hi;  /**< Seconds since 0h UTC on 1 January 1900 */
	uint32_t lo;  /**< Fraction of seconds                    */
};

struct hash;

/** Per-source state information */
struct rtp_source {
	struct sa rtp_peer;       /**< IP-address of the RTP source        */
	uint16_t max_seq;         /**< Highest seq. number seen            */
	uint32_t cycles;          /**< Shifted count of seq. number cycles */
	uint32_t base_seq;        /**< Base seq number                     */
	uint32_t bad_seq;         /**< Last 'bad' seq number + 1           */
	uint32_t probation;       /**< Sequ. packets till source is valid  */
	uint32_t received;        /**< Packets received                    */
	uint32_t expected_prior;  /**< Packet expected at last interval    */
	uint32_t received_prior;  /**< Packet received at last interval    */
	int transit;              /**< Relative trans time for prev pkt    */
	uint32_t jitter;          /**< Estimated jitter                    */
	size_t rtp_rx_bytes;      /**< Number of RTP bytes received        */
	uint64_t sr_recv;         /**< When the last SR was received       */
	struct ntp_time last_sr;  /**< NTP Timestamp from last SR received */
	uint32_t rtp_ts;          /**< RTP timestamp                       */
	uint32_t psent;           /**< RTP packets sent                    */
	uint32_t osent;           /**< RTP octets sent                     */
};

/** RTP Member */
struct rtp_member {
	struct le le;             /**< Hash-table element                  */
	struct rtp_source *s;     /**< RTP source state                    */
	uint32_t src;             /**< Source - used for hash-table lookup */
	int cum_lost;             /**< Cumulative number of packets lost   */
	uint32_t jit;             /**< Jitter in [us]                      */
	uint32_t rtt;             /**< Round-trip time in [us]             */
};


/* Member */
struct rtp_member *member_add(struct hash *ht, uint32_t src);
struct rtp_member *member_find(struct hash *ht, uint32_t src);

/* Source */
void source_init_seq(struct rtp_source *s, uint16_t seq);
int  source_update_seq(struct rtp_source *s, uint16_t seq);
void source_calc_jitter(struct rtp_source *s, uint32_t rtp_ts,
			uint32_t arrival);
int  source_calc_lost(const struct rtp_source *s);
uint8_t source_calc_fraction_lost(struct rtp_source *s);

/* RR (Reception report) */
int rtcp_rr_alloc(struct rtcp_rr **rrp, size_t count);
int rtcp_rr_encode(struct mbuf *mb, const struct rtcp_rr *rr);
int rtcp_rr_decode(struct mbuf *mb, struct rtcp_rr *rr);

/* SDES (Source Description) */
int rtcp_sdes_decode(struct mbuf *mb, struct rtcp_sdes *sdes);

/* RTCP Feedback */
int rtcp_rtpfb_gnack_encode(struct mbuf *mb, uint16_t pid, uint16_t blp);
int rtcp_psfb_sli_encode(struct mbuf *mb, uint16_t first, uint16_t number,
			 uint8_t picid);
int rtcp_rtpfb_decode(struct mbuf *mb, struct rtcp_msg *msg);
int rtcp_psfb_decode(struct mbuf *mb, struct rtcp_msg *msg);

/** NTP Time */
struct timeval;
void unix2ntp(struct ntp_time *ntp, const struct timeval *tv);
void ntp2unix(struct timeval *tv, const struct ntp_time *ntp);
int  ntp_time_get(struct ntp_time *ntp);
uint32_t ntp_compact(const struct ntp_time *ntp);
uint64_t ntp_compact2us(uint32_t ntpc);

/* RTP Socket */
struct rtcp_sess *rtp_rtcp_sess(const struct rtp_sock *rs);

/* RTCP message */
typedef int (rtcp_encode_h)(struct mbuf *mb, void *arg);

int rtcp_hdr_encode(struct mbuf *mb, uint8_t count, enum rtcp_type type,
		    uint16_t length);
int rtcp_hdr_decode(struct mbuf *mb, struct rtcp_hdr *hdr);
int rtcp_vencode(struct mbuf *mb, enum rtcp_type type, uint32_t count,
		 va_list ap);

/* RTCP Session */
struct rtcp_sess;

int  rtcp_sess_alloc(struct rtcp_sess **sessp, struct rtp_sock *rs);
int  rtcp_enable(struct rtcp_sess *sess, bool enabled, const char *cname);
int  rtcp_send(struct rtp_sock *rs, struct mbuf *mb);
void rtcp_handler(struct rtcp_sess *sess, struct rtcp_msg *msg);
void rtcp_sess_tx_rtp(struct rtcp_sess *sess, uint32_t ts,
		      size_t payload_size);
void rtcp_sess_rx_rtp(struct rtcp_sess *sess, uint16_t seq, uint32_t ts,
		      uint32_t src, size_t payload_size,
		      const struct sa *peer);
