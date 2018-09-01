#include "mmf_sink.h"

//rtsp header files
#include "rtsp/rtsp_api.h"
#include "sockets.h"
#include "lwip/netif.h"

static struct rtp_object rtp_payload;

// to get stream id,
// need mutex to protect this routine? i dont think it is necessary
#define STREAM_FLOW_ID_BASE 0
static u32 stream_flow_id_bitmap = 0;
static _mutex stream_flow_id_bitmap_lock = NULL;

//static u32 rtsp_tick_offset = 0;
//static u32 rtsp_stream_num = 0;
// codec map
//
static int codec_map_v[] = {AV_CODEC_ID_MJPEG, AV_CODEC_ID_H264, AV_CODEC_ID_MP4V_ES};
static int codec_map_a[] = {AV_CODEC_ID_PCMU, AV_CODEC_ID_PCMA, AV_CODEC_ID_MP4A_LATM};

void rtsp_mod_close(void* ctx)
{
	struct rtsp_context * rtsp_ctx = (struct rtsp_context *)ctx;
	if(rtsp_ctx->stream_ctx[0].codec)
		free(rtsp_ctx->stream_ctx[0].codec);
	rtsp_close(rtsp_ctx);
	rtsp_context_free(rtsp_ctx);
        if(stream_flow_id_bitmap_lock != NULL)
                rtw_mutex_free(&stream_flow_id_bitmap_lock);
        stream_flow_id_bitmap = 0;
}

void* rtsp_mod_open(void)
{
	int timeout = 10;
	struct rtsp_context *rtsp_ctx = rtsp_context_create(1);
	if(!rtsp_ctx)	return NULL;
	/*open rtsp service task*/
	if(rtsp_open(rtsp_ctx)<0){
		rtsp_context_free(rtsp_ctx);
		rtsp_ctx = NULL;
	}
	
	while(!rtsp_is_service_enabled(rtsp_ctx)){
		rtw_mdelay_os(1000);
		if(timeout--<=0){
			printf("\n\rwait rtsp service time out...");
			goto rtsp_mod_open_fail;
		}
	}  
	
	// init payload object
	rtp_object_init(&rtp_payload);
	rtsp_ctx->stream_ctx[0].codec = malloc(sizeof(struct codec_info));
	if(!rtsp_ctx->stream_ctx[0].codec)
		goto rtsp_mod_open_fail;
	
	//rtsp_clock_init(rtsp_ctx->stream_ctx[0].codec->clock_rate);		
	//rtsp_tick_offset = rtw_get_current_time();
	//rtsp_stream_num = 1;
	return (void*)rtsp_ctx; 
	
rtsp_mod_open_fail:
	rtp_object_deinit(&rtp_payload);
	rtsp_mod_close(rtsp_ctx);
	return NULL;
}

int rtsp_mod_set_param(void* ctx, int cmd, int arg)
{
	struct rtsp_context *rtsp_ctx = (struct rtsp_context *)ctx;
	struct stream_context *stream_ctx = &rtsp_ctx->stream_ctx[0];
	int *codec_map = NULL;
	
	switch(cmd){
	case CMD_SET_FRAMERATE:
		stream_ctx->framerate = arg;
		break;
	case CMD_SET_BITRATE:
		stream_ctx->bitrate = arg;
		break;
	case CMD_SET_SAMPLERATE:
		stream_ctx->samplerate = arg;
		break;
	case CMD_SET_CHANNEL:
		stream_ctx->channel = arg;
		break;
	// for H264
	case CMD_SET_SPS:				
		set_sps_string((char*)arg);
		break;
	case CMD_SET_PPS:
		set_pps_string((char*)arg);
		break;
	case CMD_SET_LEVEL:
		set_profile_lv_string((char*)arg);
		break;                
	case CMD_SET_APPLY:
		if(stream_flow_id_bitmap_lock == NULL)
			rtw_mutex_init(&stream_flow_id_bitmap_lock);
                if(stream_ctx->stream_id < 0)
                    stream_ctx->stream_id = rtsp_get_number(STREAM_FLOW_ID_BASE, &stream_flow_id_bitmap, &stream_flow_id_bitmap_lock);
		rtp_stream_statistics_sync(stream_ctx);	
		break;
	/*
	case CMD_SET_STREAMNUM:
		if(arg>1){	// arg = 1, do nothing, because has been created when open
			// free original stream
			free(stream_ctx);
			// malloc new steram and init
			rtsp_ctx->nb_streams = arg;
			rtsp_ctx->stream_ctx = malloc(arg * sizeof(struct stream_context));
			if(!rtsp_ctx->stream_ctx)	return -ENOMEM;
			for(int i = 0; i < arg; i++)
			{
				rtsp_ctx->rtpseq[i] = 0;
				rtsp_stream_context_init(rtsp_ctx, &rtsp_ctx->stream_ctx[i]);
			}
		}	
		break;
	*/
	case CMD_SET_STREAMMING:
		if(arg == ON){
                        rtsp_clock_init(rtsp_ctx->stream_ctx[0].codec->clock_rate);
			rtsp_start(rtsp_ctx);
			// insert to input queue
			rtp_object_in_stream_queue(&rtp_payload, &rtsp_ctx->stream_ctx[0]); 
		}else
			rtsp_stop(rtsp_ctx);
		break;
	case CMD_SET_CODEC:
		codec_map = ((arg&0xf0)==0x00)?codec_map_v:codec_map_a;
		get_codec_by_id(stream_ctx->codec, codec_map[arg&0xf]);
		stream_ctx->media_type = rtsp_parse_stream_media_type(stream_ctx->codec);
		rtp_load_o_handler_by_codec_id(&rtp_payload, codec_map[arg&0xf]);
		if(rtp_payload.rtp_object_handler == NULL)
			return -EINVAL;
		break;
	default:
		break;
	}
	return 0;
}

// private function
int rtsp_mod_wait_complete(void)
{
	return 1;	
}

// private function
int rtsp_mod_compelete_cb(void)
{
	return 1;
}

int rtsp_mod_handle(void* ctx, void* b)
{
	struct rtsp_context *rtsp_ctx = (struct rtsp_context *)ctx;
	exch_buf_t *exbuf = (exch_buf_t*)b;
	struct stream_context *stream_ctx = &rtsp_ctx->stream_ctx[0];
	struct rtp_object *payload = NULL;//&rtp_payload;
        
        if(exbuf->state != STAT_READY)
            return -EAGAIN;
        
        if(rtsp_ctx->state != RTSP_PLAYING)
            goto end;
	// wait output not empty and get one
	// Get payload from rtsp module
	do{
		payload = rtp_object_out_stream_queue(stream_ctx);
		if(payload==NULL)	vTaskDelay(1);
	}while(payload == NULL);
	
	// insert payload to rtsp_ctx stream
	//printf("%d\n\r", xTaskGetTickCount());
	/*fill payload*/
	payload->index = exbuf->index;
	payload->data = exbuf->data;
	payload->len = exbuf->len;
        payload->timestamp = exbuf->timestamp;
	if(payload->timestamp==0)
		payload->timestamp = rtsp_get_current_tick();
	else
		payload->timestamp = exbuf->timestamp;
	//printf("ts: %8x\n\r", payload->timestamp);
	/* because we will fill&send a complete frame in single rtp object, set both fs & fe to 1 and fd to 0*/
	rtp_object_set_fs(payload, 1);
	rtp_object_set_fe(payload, 1);
	rtp_object_set_fd(payload, 0);
	/* set payload state to READY to enable rtp task process this rtp object;*/
	payload->state = RTP_OBJECT_READY; 
	
	rtp_object_in_stream_queue(payload, stream_ctx);
	
	// TODO: use inter task communication
	// wait payload state to IDEL or USED
	rtsp_mod_wait_complete();
	while(list_empty(&stream_ctx->output_queue))
		vTaskDelay(1);
end:
        exbuf->state = STAT_USED;
	return 0;
}

msink_module_t rtsp_module = 
{
	.create = rtsp_mod_open,
	.destroy = rtsp_mod_close,
	.set_param = rtsp_mod_set_param,
	.handle = rtsp_mod_handle,
};