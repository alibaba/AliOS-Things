/**
 * @file re_rtp.h  Interface to Real-time Transport Protocol and RTCP
 *
 * Copyright (C) 2010 Creytiv.com
 */

#ifndef RE_RTP_H
#define RE_RTP_H

#include <stdbool.h>
#include "re_common.h"
#include "re_sa.h"

/**
 * @addtogroup aos_rtp rtp
 * @{
 */

/** RTP protocol values */
enum {
	RTP_VERSION     =  2,  /**< Defines the RTP version we support */
	RTCP_VERSION    =  2,  /**< Supported RTCP Version             */
	RTP_HEADER_SIZE = 12   /**< Number of bytes in RTP Header      */
};


/** Defines the RTP header */
struct rtp_header {
	uint8_t  ver;       /**< RTP version number     */
	bool     pad;       /**< Padding bit            */
	bool     ext;       /**< Extension bit          */
	uint8_t  cc;        /**< CSRC count             */
	bool     m;         /**< Marker bit             */
	uint8_t  pt;        /**< Payload type           */
	uint16_t seq;       /**< Sequence number        */
	uint32_t ts;        /**< Timestamp              */
	uint32_t ssrc;      /**< Synchronization source */
	uint32_t csrc[16];  /**< Contributing sources   */
	struct {
		uint16_t type;  /**< Defined by profile     */
		uint16_t len;   /**< Number of 32-bit words */
	} x;
};

/** RTCP Packet Types */
enum rtcp_type {
	RTCP_FIR   = 192,  /**< Full INTRA-frame Request (RFC 2032)    */
	RTCP_NACK  = 193,  /**< Negative Acknowledgement (RFC 2032)    */
	RTCP_SR    = 200,  /**< Sender Report                          */
	RTCP_RR    = 201,  /**< Receiver Report                        */
	RTCP_SDES  = 202,  /**< Source Description                     */
	RTCP_BYE   = 203,  /**< Goodbye                                */
	RTCP_APP   = 204,  /**< Application-defined                    */
	RTCP_RTPFB = 205,  /**< Transport layer FB message (RFC 4585)  */
	RTCP_PSFB  = 206,  /**< Payload-specific FB message (RFC 4585) */
	RTCP_XR    = 207,  /**< Extended Report (RFC 3611)             */
	RTCP_AVB   = 208,  /**< AVB RTCP Packet (IEEE1733)             */
};

/** SDES Types */
enum rtcp_sdes_type {
	RTCP_SDES_END   = 0,  /**< End of SDES list               */
	RTCP_SDES_CNAME = 1,  /**< Canonical name                 */
	RTCP_SDES_NAME  = 2,  /**< User name                      */
	RTCP_SDES_EMAIL = 3,  /**< User's electronic mail address */
	RTCP_SDES_PHONE = 4,  /**< User's phone number            */
	RTCP_SDES_LOC   = 5,  /**< Geographic user location       */
	RTCP_SDES_TOOL  = 6,  /**< Name of application or tool    */
	RTCP_SDES_NOTE  = 7,  /**< Notice about the source        */
	RTCP_SDES_PRIV  = 8   /**< Private extension              */
};

/** Transport Layer Feedback Messages */
enum rtcp_rtpfb {
	RTCP_RTPFB_GNACK = 1  /**< Generic NACK */
};

/** Payload-Specific Feedback Messages */
enum rtcp_psfb {
	RTCP_PSFB_PLI  = 1,   /**< Picture Loss Indication (PLI) */
	RTCP_PSFB_SLI  = 2,   /**< Slice Loss Indication (SLI)   */
	RTCP_PSFB_AFB  = 15,  /**< Application layer Feedback Messages */
};

/** Reception report block */
struct rtcp_rr {
	uint32_t ssrc;            /**< Data source being reported      */
	unsigned int fraction:8;  /**< Fraction lost since last SR/RR  */
	int lost:24;              /**< Cumul. no. pkts lost (signed!)  */
	uint32_t last_seq;        /**< Extended last seq. no. received */
	uint32_t jitter;          /**< Interarrival jitter             */
	uint32_t lsr;             /**< Last SR packet from this source */
	uint32_t dlsr;            /**< Delay since last SR packet      */
};

/** SDES item */
struct rtcp_sdes_item {
	enum rtcp_sdes_type type; /**< Type of item (enum rtcp_sdes_type) */
	uint8_t length;           /**< Length of item (in octets)         */
	char *data;               /**< Text, not null-terminated          */
};

/** One RTCP Message */
struct rtcp_msg {
	/** RTCP Header */
	struct rtcp_hdr {
		unsigned int version:2;  /**< Protocol version       */
		unsigned int p:1;        /**< Padding flag           */
		unsigned int count:5;    /**< Varies by packet type  */
		unsigned int pt:8;       /**< RTCP packet type       */
		uint16_t length;         /**< Packet length in words */
	} hdr;
	union {
		/** Sender report (SR) */
		struct {
			uint32_t ssrc;        /**< Sender generating report  */
			uint32_t ntp_sec;     /**< NTP timestamp - seconds   */
			uint32_t ntp_frac;    /**< NTP timestamp - fractions */
			uint32_t rtp_ts;      /**< RTP timestamp             */
			uint32_t psent;       /**< RTP packets sent          */
			uint32_t osent;       /**< RTP octets sent           */
			struct rtcp_rr *rrv;  /**< Reception report blocks   */
		} sr;

		/** Reception report (RR) */
		struct {
			uint32_t ssrc;        /**< Receiver generating report*/
			struct rtcp_rr *rrv;  /**< Reception report blocks   */
		} rr;

		/** Source Description (SDES) */
		struct rtcp_sdes {
			uint32_t src;         /**< First SSRC/CSRC           */
			struct rtcp_sdes_item *itemv;  /**< SDES items       */
			uint32_t n;           /**< Number of SDES items      */
		} *sdesv;

		/** BYE */
		struct {
			uint32_t *srcv;    /**< List of sources              */
			char *reason;      /**< Reason for leaving (opt.)    */
		} bye;

		/** Application-defined (APP) */
		struct {
			uint32_t src;      /**< SSRC/CSRC                  */
			char name[4];      /**< Name (ASCII)               */
			uint8_t *data;     /**< Application data (32 bits) */
			size_t data_len;   /**< Number of data bytes       */
		} app;

		/** Full INTRA-frame Request (FIR) packet */
		struct {
			uint32_t ssrc;  /**< SSRC for sender of this packet */
		} fir;

		/** Negative ACKnowledgements (NACK) packet */
		struct {
			uint32_t ssrc;  /**< SSRC for sender of this packet */
			uint16_t fsn;   /**< First Sequence Number lost     */
			uint16_t blp;   /**< Bitmask of lost packets        */
		} nack;

		/** Feedback (RTPFB or PSFB) packet */
		struct {
			uint32_t ssrc_packet;
			uint32_t ssrc_media;
			uint32_t n;
			/** Feedback Control Information (FCI) */
			union {
				struct gnack {
					uint16_t pid;
					uint16_t blp;
				} *gnackv;
				struct sli {
					uint16_t first;
					uint16_t number;
					uint8_t picid;
				} *sliv;
				struct mbuf *afb;
				void *p;
			} fci;
		} fb;
	} r;
};

/** RTCP Statistics */
struct rtcp_stats {
	struct {
		uint32_t sent;  /**< Tx RTP Packets                  */
		int lost;       /**< Tx RTP Packets Lost             */
		uint32_t jit;   /**< Tx Inter-arrival Jitter in [us] */
	} tx;
	struct {
		uint32_t sent;  /**< Rx RTP Packets                  */
		int lost;       /**< Rx RTP Packets Lost             */
		uint32_t jit;   /**< Rx Inter-Arrival Jitter in [us] */
	} rx;
	uint32_t rtt;           /**< Current Round-Trip Time in [us] */
};

/* RTP api */

/**
 * Allocate a new RTP socket
 *
 * @param rsp Pointer to returned RTP socket
 *
 * @return 0 for success, otherwise errorcode
 */
int   rtp_alloc(struct rtp_sock **rsp);

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
int   rtp_listen(struct rtp_sock **rsp, int proto, const struct sa *ip,
		 uint16_t min_port, uint16_t max_port, bool enable_rtcp,
		 rtp_recv_h *recvh, rtcp_recv_h *rtcph, void *arg);

/**
 * Encode the RTP header into a buffer
 *
 * @param mb  Buffer to encode into
 * @param hdr RTP Header to be encoded
 *
 * @return 0 if success, otherwise errorcode
 */
int   rtp_hdr_encode(struct mbuf *mb, const struct rtp_header *hdr);

/**
 * Decode an RTP header from a buffer
 *
 * @param hdr RTP Header to decode into
 * @param mb  Buffer to decode from
 *
 * @return 0 if success, otherwise errorcode
 */
int   rtp_hdr_decode(struct rtp_header *hdr, struct mbuf *mb);

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
int   rtp_encode(struct rtp_sock *rs, bool ext, bool marker, uint8_t pt,
		 uint32_t ts, struct mbuf *mb);

/**
 * Decode an RTP packet and return decoded RTP header and payload
 *
 * @param rs     RTP Socket
 * @param mb     Memory buffer containing RTP packet
 * @param hdr    RTP header (set on return)
 *
 * @return 0 for success, otherwise errorcode
 */
int   rtp_decode(struct rtp_sock *rs, struct mbuf *mb, struct rtp_header *hdr);

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
int   rtp_send(struct rtp_sock *rs, const struct sa *dst, bool ext,
	       bool marker, uint8_t pt, uint32_t ts, struct mbuf *mb);

/**
 * RTP Debug handler, use with fmt %H
 *
 * @param pf Print function
 * @param rs RTP Socket
 *
 * @return 0 if success, otherwise errorcode
 */
int   rtp_debug(struct re_printf *pf, const struct rtp_sock *rs);

/**
 * Get the RTP transport socket from an RTP/RTCP Socket
 *
 * @param rs RTP Socket
 *
 * @return Transport socket for RTP
 */
void *rtp_sock(const struct rtp_sock *rs);

/**
 * Get the Synchronizing source for an RTP/RTCP Socket
 *
 * @param rs RTP Socket
 *
 * @return Synchronizing source
 */
uint32_t rtp_sess_ssrc(const struct rtp_sock *rs);

/**
 * Get the local RTP address for an RTP/RTCP Socket
 *
 * @param rs RTP Socket
 *
 * @return Local RTP address
 */
const struct sa *rtp_local(const struct rtp_sock *rs);

/* RTCP session api */
/**
 * Start the RTCP Session
 *
 * @param rs    RTP Socket
 * @param cname Canonical Name
 * @param peer  IP-Address of RTCP Peer
 */
void  rtcp_start(struct rtp_sock *rs, const char *cname,
		 const struct sa *peer);

/**
 * Enable RTCP-multiplexing on RTP-port
 *
 * @param rs      RTP Socket
 * @param enabled True to enable, false to disable
 */
void  rtcp_enable_mux(struct rtp_sock *rs, bool enabled);

/**
 * Set the Sampling-rate on an RTCP Session
 *
 * @param rs       RTP Socket
 * @param srate_tx Transmit samplerate
 * @param srate_rx Receive samplerate
 */
void  rtcp_set_srate(struct rtp_sock *rs, uint32_t sr_tx, uint32_t sr_rx);

/**
 * Set the transmit Sampling-rate on an RTCP Session
 *
 * @param rs       RTP Socket
 * @param srate_tx Transmit samplerate
 */
void  rtcp_set_srate_tx(struct rtp_sock *rs, uint32_t srate_tx);

/**
 * Set the receive Sampling-rate on an RTCP Session
 *
 * @param rs       RTP Socket
 * @param srate_rx Receive samplerate
 */
void  rtcp_set_srate_rx(struct rtp_sock *rs, uint32_t srate_rx);

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
int   rtcp_send_app(struct rtp_sock *rs, const char name[4],
		    const uint8_t *data, size_t len);

/**
 * Send a Full INTRA-frame Request (FIR) packet
 *
 * @param rs   RTP Socket
 * @param ssrc Synchronization source identifier for the sender of this packet
 *
 * @return 0 for success, otherwise errorcode
 */
int   rtcp_send_fir(struct rtp_sock *rs, uint32_t ssrc);

/**
 * Send an RTCP NACK packet
 *
 * @param rs   RTP Socket
 * @param fsn  First Sequence Number lost
 * @param blp  Bitmask of lost packets
 *
 * @return 0 for success, otherwise errorcode
 */
int   rtcp_send_nack(struct rtp_sock *rs, uint16_t fsn, uint16_t blp);

/**
 * Send an RTCP Picture Loss Indication (PLI) packet
 *
 * @param rs      RTP Socket
 * @param fb_ssrc Feedback SSRC
 *
 * @return 0 for success, otherwise errorcode
 */
int   rtcp_send_pli(struct rtp_sock *rs, uint32_t fb_ssrc);

/**
 * RTCP Debug handler, use with fmt %H
 *
 * @param pf Print function
 * @param rs RTP Socket
 *
 * @return 0 if success, otherwise errorcode
 */
int   rtcp_debug(struct re_printf *pf, const struct rtp_sock *rs);

/**
 * Get the RTCP transport socket from an RTP/RTCP Socket
 *
 * @param rs RTP Socket
 *
 * @return Transport socket for RTCP
 */
void *rtcp_sock(const struct rtp_sock *rs);

/**
 * Get the RTCP Statistics for a source
 *
 * @param rs    RTP Socket
 * @param ssrc  Synchronization source
 * @param stats RTCP Statistics, set on return
 *
 * @return 0 if success, otherwise errorcode
 */
int   rtcp_stats(struct rtp_sock *rs, uint32_t ssrc, struct rtcp_stats *stats);

/* RTCP utils */
/**
 * Encode an RTCP Packet into a buffer
 *
 * @param mb    Buffer to encode into
 * @param type  RTCP Packet type
 * @param count Packet-specific count
 *
 * @return 0 for success, otherwise errorcode
 */
int   rtcp_encode(struct mbuf *mb, enum rtcp_type type, uint32_t count, ...);

/**
 * Decode one RTCP message from a buffer
 *
 * @param msgp Pointer to allocated RTCP Message
 * @param mb   Buffer to decode from
 *
 * @return 0 for success, otherwise errorcode
 */
int   rtcp_decode(struct rtcp_msg **msgp, struct mbuf *mb);

/**
 * Print an RTCP Message
 *
 * @param pf  Print handler for debug output
 * @param msg RTCP Message
 *
 * @return 0 if success, otherwise errorcode
 */
int   rtcp_msg_print(struct re_printf *pf, const struct rtcp_msg *msg);

/**
 * Encode one SDES chunk into mbuffer
 *
 * @param mb    Buffer to encode into
 * @param src   First SSRC/CSRC
 * @param itemc Number of SDES items to encode
 *
 * @return 0 if success, otherwise errorcode
 */
int   rtcp_sdes_encode(struct mbuf *mb, uint32_t src, uint32_t itemc, ...);

/**
 * Get rtcp type name string
 *
 * @param type    Rtcp type
 *
 * @return "?" if failure, otherwise success
 */
const char *rtcp_type_name(enum rtcp_type type);

/**
 * Get rtcp sdes name string
 *
 * @param type    Rtcp sdes type
 *
 * @return "?" if failure, otherwise success
 */
const char *rtcp_sdes_name(enum rtcp_sdes_type sdes);

/** @} */
#endif /* RE_RTP_H */
