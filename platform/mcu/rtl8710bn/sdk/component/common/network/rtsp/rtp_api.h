#ifndef _RTP_API_H_
#define _RTP_API_H_

#include "dlist.h"
#include "basic_types.h"
#include "osdep_service.h"
//#include "osdep_api.h"
#include "avcodec.h"
#include <lwip/def.h> //for host network byte order convertion

/* from error_base.h */
#define	EIO		 5	/* I/O error */
#define	EAGAIN		11	/* Try again */
#define	ENOMEM		12	/* Out of memory */
#define	EINVAL		22	/* Invalid argument */

#define RTP_BIG_ENDIAN 0
#define RTP_HDR_SZ 12

#define RTP_SERVER_PORT_BASE 55608
#define RTP_PORT_BASE 50020
#define RTP_CLIENT_PORT_BASE 51020
/*
 * RTP data header from RFC1889
 */
/*
 *
 *
 *    The RTP header has the following format:
 *
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |V=2|P|X|  CC   |M|     PT      |       sequence number         |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                           timestamp                           |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |           synchronization source (SSRC) identifier            |
 * +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
 * |            contributing source (CSRC) identifiers             |
 * |                             ....                              |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 * RTP data header
 */

typedef struct {
#if RTP_BIG_ENDIAN
        u16 version:2;   /* protocol version */
        u16 p:1;         /* padding flag */
        u16 x:1;         /* header extension flag */
        u16 cc:4;        /* CSRC count */
        u16 m:1;         /* marker bit */
        u16 pt:7;        /* payload type */
#else /*RTP_LITTLE_ENDIAN*/
        u16 cc:4;        /* CSRC count */
        u16 x:1;         /* header extension flag */
        u16 p:1;         /* padding flag */
        u16 version:2;   /* protocol version */
        u16 pt:7;        /* payload type */
        u16 m:1;         /* marker bit */
#endif
        u16 seq;              /* sequence number */
        u32 ts;               /* timestamp */
        u32 ssrc;             /* synchronization source */
        u32 *csrc;          /* optional CSRC list, skip if cc is set to 0 here*/
} rtp_hdr_t;

/*sturcture to hold connect info*/
struct connect_context
{
	int socket_id;	
	u8 *server_ip;
	u16 server_port;
        u8 *remote_ip;
	u16 remote_port;
};

struct rtp_statistics
{
        u32 rtp_tick;
        u32 rtp_tick_inc; 
        u32 base_timestamp;
        /*for flow control*/
        u32 delay_threshold; //in ms
        u32 timer1; //time before send
        u32 timer2; //time after sent
        u32 delta_timer;
        u8 do_start_check; //indicate if start check need to be done
        u32 sent_packet;
        u32 drop_packet;
};

enum rtp_object_state
{
    RTP_OBJECT_IDLE = 0,
    RTP_OBJECT_READY,
    RTP_OBJECT_INUSE,
    RTP_OBJECT_USED,
};

struct stream_context;
struct rtp_object;

struct rtp_object
{
        struct list_head rtp_list;
        _mutex list_lock;
        rtp_hdr_t *rtphdr;
        void *extra;     //pointer to type specific structure
        int index;      //respective internal buffer index 
        u8 *data; // respective internal buffer data addr
        int len;        //one complete frame data length
        u32 timestamp; //timestamp        
        u32 fs:1; //fragment start
        u32 fe:1; //fragment end
        u32 fk:1;    //fragment keep indicator so that cannot avoid sending by flow control        
        u32 fd:29; //fragment data size (max size of 2^29-1)
        enum rtp_object_state state;    
        struct connect_context connect_ctx;
        int (*rtp_object_handler)(struct stream_context *stream_ctx, struct rtp_object *payload);
};

void rtp_object_init(struct rtp_object *payload);
void rtp_object_deinit(struct rtp_object *payload);
void rtp_object_set_fs(struct rtp_object *payload, int flag);
void rtp_object_set_fe(struct rtp_object *payload, int flag);
void rtp_object_set_fk(struct rtp_object *payload, int flag);
void rtp_object_set_fd(struct rtp_object *payload, int size);
void rtp_load_o_handler_by_codec_id(struct rtp_object *payload, int id);
void rtp_fill_header(rtp_hdr_t *rtphdr, int version, int padding, int extension, int cc, int marker, int pt, u16 seq, u32 ts, u32 ssrc);
int rtp_parse_header(u8 *src, rtp_hdr_t *rtphdr, int is_nbo);
void rtp_dump_header(rtp_hdr_t *rtphdr, int is_nbo);
#endif