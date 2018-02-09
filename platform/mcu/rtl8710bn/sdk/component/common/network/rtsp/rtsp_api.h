#ifndef _RTSP_API_H_
#define _RTSP_API_H_

#include "dlist.h"
#include "basic_types.h"
#include "osdep_service.h"
//#include "osdep_api.h"
#include "rtsp/rtp_api.h"
#include "avcodec.h"
#include "rtsp/sdp.h"

#define RTSP_DEBUG 0

#if RTSP_DEBUG
#define RTSP_PRINTF(fmt, args...)    printf("\n\r%s: " fmt, __FUNCTION__, ## args)
#define RTSP_ERROR(fmt, args...)    printf("\n\r%s: " fmt, __FUNCTION__, ## args)
#else
#define RTSP_PRINTF(fmt, args...)
#define RTSP_ERROR(fmt, args...)    printf("\n\r%s: " fmt, __FUNCTION__, ## args)
#endif

/* clock usage */
#define RTSP_DEPEND_CLK_HZ      configTICK_RATE_HZ      

#define RTSP_SERVICE_PRIORITY    2
#define RTSP_MAX_STREAM_NUM	2
#define RTSP_REQUEST_BUF_SIZE   512
#define RTSP_RESPONSE_BUF_SIZE 	MAX_SDP_SIZE //max size for response buffer  
#define DEF_RTSP_PORT 554
#define DEF_HTTP_PORT 5008

#define RTSP_SELECT_SOCK 8 

/*rtsp request type list*/
#define REQUEST_OPTIONS 1
#define REQUEST_DESCRIBE 2
#define REQUEST_SETUP 3
#define REQUEST_TEARDOWN 4
#define REQUEST_PLAY 5
#define REQUEST_PAUSE 6
#define REQUEST_GET_PARAM 7

/*rtsp cast mode list*/
#define UNICAST_UDP_MODE 1
#define UNICAST_TCP_MODE 2
#define MULTICAST_MODE 3

#define BOUNDARY "amebaimagetest"

/*RTSP KEEPALIVE TIMEOUT ENABLE*/
#define KEEPALIVE_TIMEOUT_ENABLE


enum _rtsp_state {
    RTSP_INIT = 0,
    RTSP_READY = 1,
    RTSP_PLAYING = 2,
};
typedef enum _rtsp_state rtsp_state;

struct rtsp_session
{
        u32 id;
        u8 version;
        u32 start_time;
        u32 end_time;
        u8 *user;
        u8 *name;
};

struct rtsp_transport
{
	u8 isRtp; //transport protocol
	u8 isTcp; //lower transport protocol
	u8 castMode; //unicast UDP(1) or unicast TCP(2) or multicast(3)
	int port_low;
	int port_high;
	int clientport_low;
	int clientport_high;
	int serverport_low;
	int serverport_high;
	u8 ttl; //multicast time to live
	//to be added if necessary
};

struct stream_context
{
	struct rtsp_context *parent;
	int stream_id; //sync with stream_flow id
	struct list_head input_queue;
	_mutex input_lock;
	struct list_head output_queue;
	_mutex output_lock;
	struct codec_info *codec;
	u8 media_type;
	u8 framerate;
        u32 bitrate;
	u32 samplerate;
	u32 channel;
	struct rtp_statistics statistics;
};

struct rtsp_context
{
        int id;
	u8 allow_stream;
	rtsp_state state;
	u8 request_type;
	u32 CSeq;
	u8 *response;
	struct connect_context connect_ctx;
	struct rtsp_transport transport[RTSP_MAX_STREAM_NUM];
	struct rtsp_session session;
	u16 rtpseq[RTSP_MAX_STREAM_NUM];
	u8 is_rtsp_start;
	_sema start_rtsp_sema;
	u8 is_rtp_start;
	_sema start_rtp_sema;
	void (* rtp_service_handle) (struct rtsp_context* rtsp_ctx);
#ifdef SUPPORT_RTCP
        u8 is_rtcp_start;
        _sema start_rtcp_sema;
	void (* rtcp_service_handle) (struct rtsp_context* rtsp_ctx);
#endif
#ifdef SUPPORT_HTTP
    //to be added
#endif
        u8 nb_streams_setup;
	u8 nb_streams;
	struct stream_context *stream_ctx;
        u32 pre_filter_packet;
        	// callback
	int (*cb_start)(void*);		// play
	int (*cb_stop)(void*);		// teardown
	int (*cb_pause)(void*);		// pause
	int (*cb_custom)(void*);	// setparam
};

int rtsp_clock_init(u32 clock_hz);
void rtsp_clock_deinit();
u32 rtsp_get_current_tick();

int rtsp_get_number(int number_base, u32 *number_bitmap, _mutex *bitmap_lock);
void rtsp_put_number(int number, int number_base, u32 *number_bitmap, _mutex *bitmap_lock);
struct rtsp_context *rtsp_context_create(u8 nb_streams);
void rtsp_context_free(struct rtsp_context *rtsp_ctx);
int rtsp_is_stream_enabled(struct rtsp_context *rtsp_ctx);
int rtsp_is_service_enabled(struct rtsp_context *rtsp_ctx);
int rtsp_open(struct rtsp_context *rtsp_ctx);
void rtsp_close(struct rtsp_context *rtsp_ctx);
void rtsp_start(struct rtsp_context *rtsp_ctx);
void rtsp_stop(struct rtsp_context *rtsp_ctx);
void rtp_object_in_stream_queue(struct rtp_object *payload, struct stream_context *stream_ctx);
struct rtp_object *rtp_object_out_stream_queue(struct stream_context *stream_ctx);

void set_profile_lv_string(char * plid);
void set_sps_string(char * sps);
void set_pps_string(char * pps);
int rtsp_parse_stream_media_type(struct codec_info *codec);
void rtsp_stream_context_init(struct rtsp_context *rtsp_ctx, struct stream_context *stream_ctx);
void set_prefilter_packet(struct rtsp_context *rtsp_ctx,u32 num);
void time_sync_disable(void);
void time_sync_enable(void);
#endif