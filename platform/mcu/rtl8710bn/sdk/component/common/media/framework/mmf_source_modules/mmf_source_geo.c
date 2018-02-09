#include "mmf_source.h"

//uvc/v4l2 header files
#include "videodev2.h"
#include "uvcvideo.h"
#include "v4l2_intf.h"

#include "driver/geo/patch_uvc_geo.h"

#define CHANNEL_CNT     4
static int chl_table[CHANNEL_CNT] = {0};

extern mux_stream_t mux_stream;
#define CLEAR(x) memset(&(x), 0, sizeof(x))

typedef struct __mxuvc_context{
	int width;
	int height;
	int frame_rate;
	int fmt_type;
	int compression_ratio;
}mxuvc_context;

//------------------------------------------------------------------------------
void* geo_mod_open(void)
{
	/*init usb driver prior to init uvc driver*/
	_usb_init();
	if(wait_usb_ready() < 0)
		return NULL;
	
	/*init uvc driver*/
	if(uvc_stream_init() < 0)
		// Maybe not correct
		return NULL;
	
	mxuvc_context *mxuvc_ctx = malloc(sizeof(mxuvc_context));
	if(!mxuvc_ctx)	return NULL;
	
	// default value
	mxuvc_ctx->fmt_type = V4L2_PIX_FMT_H264;
	mxuvc_ctx->width = 1280;
	mxuvc_ctx->height = 720;
	mxuvc_ctx->frame_rate = 30;  
	mxuvc_ctx->compression_ratio = 0; 
	return mxuvc_ctx;
}

void geo_mod_close(void* ctx)
{
        uvc_stream_free();
	free((struct mxuvc_context *)ctx);
	_usb_deinit();
}

int geo_mod_set_param(void* ctx, int cmd, int arg)
{
	int i, ret = 0;
        unsigned int temp = 0;
        unsigned int frame = 0;
        unsigned short width = 0;
	unsigned short high = 0;
        video_format_t format = 0;
        mxuvc_context* mxuvc_ctx = (mxuvc_context*)ctx;

	// format mapping
	//				 FMT_V_MJPG		   FMT_V_H264
	int codec_map[]={V4L2_PIX_FMT_MJPEG, V4L2_PIX_FMT_H264};
	
	switch(cmd){
        case CMD_ENABLE_CHANNEL:
                if(arg >= 0 && arg <= CHANNEL_CNT)
                    chl_table[arg] = 1;
                break;
        case CMD_DISABLE_CHANNEL:
                if(arg >= 0 && arg <= CHANNEL_CNT)
                    chl_table[arg] = 0;
				break;
	case CMD_SET_HEIGHT:
		mxuvc_ctx->height = arg;
		break;
	case CMD_SET_WIDTH:
		mxuvc_ctx->width = arg;
		break;
	case CMD_SET_FRAMERATE:
		mxuvc_ctx->frame_rate = arg;
		break;
	case CMD_SET_CPZRATIO:
		mxuvc_ctx->compression_ratio = arg;
		break;
	case CMD_SET_FRAMETYPE:
		mxuvc_ctx->fmt_type = codec_map[arg&0xF];
		break;
	case CMD_SET_APPLY:
		//if(uvc_set_param(uvc_ctx->fmt_type, &uvc_ctx->width, &uvc_ctx->height, &uvc_ctx->frame_rate, &uvc_ctx->compression_ratio) < 0)
		//	ret = EIO;  
		break;
	case CMD_SET_STREAMMING:
		if(arg == ON){	// stream on
                        for(i = 0; i < CHANNEL_CNT; i++)
                        {
                            if(chl_table[i])
                            {
                                if(mxuvc_video_start(i) < 0)
                                        ret = -EIO;   
                            }
                        }
		}else{			// stream off
                        for(i = 0; i < CHANNEL_CNT; i++)
                        {
                            if(!chl_table[i])
                            {
                                if(mxuvc_video_stop(i) < 0)
                                        ret = -EIO;   
                            }
                        }                            
		}	
		break;
	default:
		ret = ENOTSUPP;
		break;
	} 
	
	return ret;
}

//static int first_packet = 0;	//VLC need timestamps of each stream at the beginning of streaming
/*			first_packet   		 	|  	state
* 				0					|	init state: allow all packets
*   get audio /   \ get video		|
*			 1	   2				|	1: allow video;  2: allow audio 
*   get video \   / get audio		|
*				3					|	final: allow all packets
*/

int geo_mod_handle(void* ctx, void* b)
{
	int ret = 0;
	exch_buf_t *exbuf = (exch_buf_t*)b;
	
	video_info_t info;
	int channel_id;
	uint8_t *data_buf;
	uint32_t data_size; 
	uint64_t ts;
	
	struct v4l2_buffer buf;	
	
	if(exbuf->state==STAT_USED){
		memset(&buf, 0, sizeof(struct v4l2_buffer));
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP /*V4L2_MEMORY_DMABUF*/;
		buf.index = exbuf->index;
		buf.field = /*V4L2_FIELD_ANY*/V4L2_FIELD_NONE;
		buf.timestamp = rtw_get_current_time();
		ret = v4l_usr_ioctl(mux_stream.fd, VIDIOC_QBUF, (void *)&buf);                   
		if (ret < 0){
			printf("\n\rread_frame mmap method enqueue buffer failed 1");
			//mxuvc_video_stop(CH1);
			return -EIO;
		}
                exbuf->state = STAT_INIT;
	}

	if(exbuf->state!=STAT_READY){
	
recap:	
		memset(&buf, 0, sizeof(struct v4l2_buffer));
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP /*V4L2_MEMORY_DMABUF*/;        
		ret = v4l_usr_ioctl(mux_stream.fd, VIDIOC_DQBUF, (void *)&buf);                              
		if (ret < 0){
			printf("\r\nread_frame mmap method dequeue buffer failed 2");
			//mxuvc_video_stop(CH1);
			return -EIO;
		}

		/*manipulate buf data if necessary*/
		CLEAR(info);
		ret = qbox_parse_header((unsigned char*)buf.m.offset, &channel_id, &info.format, &data_buf, &data_size, &ts, &info.metadata);
		//if((ret<0)||(channel_id == CH2)||(channel_id==CH1&&first_packet==2)||(channel_id==8&&first_packet==1)) {//can filter unwanted here    //do not process H264 data from ch2
                //if((ret<0)||(channel_id == CH2)||(channel_id==8)) {//can filter unwanted here    //do not process H264 data from ch2
                if((ret<0)||(channel_id == CH2)) {
			ret = v4l_usr_ioctl(mux_stream.fd, VIDIOC_QBUF, (void *)&buf);                   
			if (ret < 0){
				printf("\r\nread_frame mmap method enqueue buffer failed 3");
				//mxuvc_video_stop(CH1);
				return -EIO;
			}        
			goto recap;
		}

		int this_id = FMT_AV_UNKNOWN;
		switch(info.format){
			case VID_FORMAT_AAC_RAW:
				this_id = FMT_A_MP4A_LATM;
				break;
			case VID_FORMAT_H264_TS:
			case VID_FORMAT_H264_RAW:
				this_id = FMT_V_H264;
				break;
			default:
				printf("\r\nUnsupport format is met");
				return -EOPNOTSUPP;
		}

		switch(this_id){		
			case FMT_A_MP4A_LATM:
				//printf("AAC");
				get_adts_header(data_size, data_buf);
				exbuf->codec_fmt = FMT_A_MP4A_LATM;
				break;		
			case FMT_V_H264:
				//printf("H264");
				exbuf->codec_fmt = FMT_V_H264;
				break;
				
			default:
				printf("\r\nUnexpected error in event_handler");
				break;
		}
		/*fill payload*/
		exbuf->index = buf.index;
		exbuf->data = data_buf;
		exbuf->len = data_size;
		if(this_id == FMT_A_MP4A_LATM){
			exbuf->timestamp = (u32)( ts*16/90);//ts*16000/1000/90
		}
		else{
			exbuf->timestamp = (u32) ts;
		}
		exbuf->state = STAT_READY;  
	}
	return 0;
}

msrc_module_t geo_module =
{
	.create = geo_mod_open,
	.destroy = geo_mod_close,
	.set_param = geo_mod_set_param,
	.handle = geo_mod_handle,
};