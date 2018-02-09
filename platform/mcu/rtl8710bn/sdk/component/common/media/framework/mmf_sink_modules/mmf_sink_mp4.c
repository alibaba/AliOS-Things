#include "ff.h"
#include <fatfs_ext/inc/ff_driver.h>
#include <platform/platform_stdlib.h>
#include "basic_types.h"
#include "section_config.h"
#include "sdio_host.h"
#include <disk_if/inc/sdcard.h>


#include "FreeRTOS.h"
#include "task.h"
#include <platform/platform_stdlib.h>
#include "basic_types.h"
#include "platform_opts.h"

#include "mmf_sink.h"

#include "sockets.h"
#include "lwip/netif.h"
#include "mp4_encap.h"

#define AUDIO_SIZE     1024*10
#define VIDEO_SIZE     1024*10
#define MOOV_BOX_SIZE  1024*64
#define H264_BUF_SIZE  1024*224//must 32 kb multiple 
SDRAM_DATA_SECTION unsigned char moov_box_[MOOV_BOX_SIZE];
SDRAM_DATA_SECTION int video_buffer_index[VIDEO_SIZE];
SDRAM_DATA_SECTION int video_buffer_size[VIDEO_SIZE];
SDRAM_DATA_SECTION int audio_buffer_index[AUDIO_SIZE];
SDRAM_DATA_SECTION int audio_buffer_size[AUDIO_SIZE];
SDRAM_DATA_SECTION unsigned char h264_buffer[H264_BUF_SIZE];

int fatfs_init(void* ctx){
	char path[64];
	
	int test_result = 1;
	int ret = 0;
        pmp4_context mp4_ctx = (pmp4_context)ctx;
	//1 init disk drive
	DBG_8195A("Init Disk driver.\n");
	// Set sdio debug level
	DBG_INFO_MSG_OFF(_DBG_SDIO_);  
	DBG_WARN_MSG_OFF(_DBG_SDIO_);
	DBG_ERR_MSG_ON(_DBG_SDIO_);
	
	if(sdio_init_host()!=0){
		DBG_8195A("SDIO host init fail.\n");
		return -1;
	}

	//1 register disk driver to fatfs
	DBG_8195A("Register disk driver to Fatfs.\n");
	mp4_ctx->drv_num = FATFS_RegisterDiskDriver(&SD_disk_Driver);
        
	if(mp4_ctx->drv_num < 0){
		DBG_8195A("Rigester disk driver to FATFS fail.\n");
                goto fail;
	}else{
                mp4_ctx->_drv[0] = mp4_ctx->drv_num + '0';
		mp4_ctx->_drv[1] = ':';
                mp4_ctx->_drv[2] = '/';
                mp4_ctx->_drv[3] = 0;
	}
        
        DBG_8195A("FatFS Write begin......\n\n");
        printf("\r\ndrv(%s)", mp4_ctx->_drv);
        if(f_mount(&mp4_ctx->m_fs,mp4_ctx->_drv, 1)!= FR_OK){
                DBG_8195A("FATFS mount logical drive fail.\n");
                goto fail;
        }

	return 0;
fail:
	sdio_deinit_host();
	return -1;
}

int fatfs_close(void *ctx){
        pmp4_context mp4_ctx = (pmp4_context)ctx;

        if(f_mount(NULL,mp4_ctx->_drv, 1) != FR_OK){
                DBG_8195A("FATFS unmount logical drive fail.\n");
        }
        
        if(FATFS_UnRegisterDiskDriver(mp4_ctx->drv_num))	
                DBG_8195A("Unregister disk driver from FATFS fail.\n");
	
	sdio_deinit_host();
	return 0;
}

void mp4_mod_close(void* ctx)
{
	pmp4_context mp4_ctx = (pmp4_context)ctx;
        mp4_muxer_close(mp4_ctx);
        fatfs_close(mp4_ctx);
	free(mp4_ctx);
}

void* mp4_mod_open(void)
{
	pmp4_context mp4_ctx = (pmp4_context)malloc(sizeof(mp4_context));
        if(!mp4_ctx){
          printf("malloc failed\r\n");
          return NULL;
        }
        memset(mp4_ctx,0,sizeof(mp4_context));
        mp4_muxer_init(mp4_ctx);
        if(fatfs_init(mp4_ctx)<0){
          free(mp4_ctx);
          return NULL;
        }
        set_mp4_audio_buffer(mp4_ctx,audio_buffer_index,audio_buffer_size,AUDIO_SIZE);
        set_mp4_video_buffer(mp4_ctx,video_buffer_index,video_buffer_size,VIDEO_SIZE);
        set_mp4_moov_buffer(mp4_ctx,moov_box_,MOOV_BOX_SIZE);
        set_mp4_write_buffer(mp4_ctx,h264_buffer,H264_BUF_SIZE);
        printf("mp4_mod_open\r\n");
	return (void*)mp4_ctx; 
}

int mp4_mod_set_param(void* ctx, int cmd, int arg)
{
        int ret = 0;
        pmp4_context mp4_ctx = (pmp4_context)ctx;
	
	switch(cmd){
	case CMD_SET_HEIGHT:
                mp4_ctx->height=arg;
		break;
	case CMD_SET_WIDTH:
                mp4_ctx->width = arg;
		break;
	case CMD_SET_FRAMERATE:
		mp4_ctx->frame_rate = arg;
		break;
	case CMD_SET_CHANNEL:
		mp4_ctx->channel_count = arg;
		break;
	case CMD_SET_SAMPLERATE:
		mp4_ctx->sample_rate = arg;
		break;
        case CMD_SET_ST_PERIOD:
                mp4_ctx->period_time = arg*1000;
                break;
        case CMD_SET_ST_TOTAL:
                mp4_ctx->file_total = arg;
                break;
        case CMD_SET_ST_TYPE:
                mp4_ctx->type = arg;
                break;
        case CMD_SET_ST_FILENAME:
                memset(mp4_ctx->filename,0,sizeof(mp4_ctx->filename));
                strcpy(mp4_ctx->filename,(char*)arg);
                break;
        case CMD_SET_ST_START:
                mp4_set_start_status(mp4_ctx);
                break;
	default:
		ret = EINVAL;
		break;
	} 
	return ret;
}
#if 0
int mp4_mod_handle(void* ctx, void* b)
{
	pmp4_context mp4_ctx = (pmp4_context)ctx;
	exch_buf_t *exbuf = (exch_buf_t*)b;
        int status = 0;
        if((exbuf->state == STAT_INIT) || (exbuf->state == STAT_USED))
            return -EAGAIN;   
        
        if(exbuf->codec_fmt == FMT_A_MP4A_LATM || exbuf->codec_fmt == FMT_V_MP4V_ES){
            mp4_handle(mp4_ctx,exbuf->data,exbuf->len,exbuf->codec_fmt);
        }
        exbuf->state=STAT_USED;

	return 0;
}
#endif
int mp4_mod_handle(void* ctx, void* b)
{
	pmp4_context mp4_ctx = (pmp4_context)ctx;
	exch_buf_t *exbuf = (exch_buf_t*)b;
        int status = 0;
        if((exbuf->state == STAT_INIT) || (exbuf->state == STAT_USED))
            return -EAGAIN;    
        if(exbuf->state == STAT_RESERVED){
            if(mp4_ctx->buffer_write_status == 0)
              exbuf->state=STAT_USED;
            return 0;
        }  
        
        if(exbuf->codec_fmt == FMT_A_MP4A_LATM || exbuf->codec_fmt == FMT_V_MP4V_ES){
            mp4_handle(mp4_ctx,exbuf->data,exbuf->len,exbuf->codec_fmt);
        }
        if((mp4_ctx->buffer_write_status == 1) && (mp4_ctx->iframe == 1) && (exbuf->codec_fmt == FMT_V_MP4V_ES))
            exbuf->state=STAT_RESERVED;
        else
            exbuf->state=STAT_USED;

	return 0;
}
msink_module_t mp4_module = 
{
	.create = mp4_mod_open,
	.destroy = mp4_mod_close,
	.set_param = mp4_mod_set_param,
	.handle = mp4_mod_handle,
};