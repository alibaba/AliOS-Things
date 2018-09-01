
/*
* uvc video capture example
*/

#include "FreeRTOS.h"
#include "task.h"
#include "example_media_ss.h"

#include "mmf_source.h"
#include "mmf_sink.h"

// select source
#define SRC_UVC			1
#define SRC_MJPG_FILE	0
#define SRC_H264_FILE	0
#define SRC_H264_UNIT	0
#define SRC_AAC_FILE	0
#define SRC_PCMU_FILE	0
#define SRC_I2S         0
//---------------------------------------------------------------------------------------
void example_media_ss_main(void *param)
{
        rtw_mdelay_os(1000);
        int con = 100;
	msink_context *msink_ctx;
	msrc_context *msrc_ctx;
	
	xQueueHandle src2sink_qid;
	xQueueHandle sink2src_qid;
	
	src2sink_qid = xQueueCreate(2, sizeof(exch_buf_t));
	sink2src_qid = xQueueCreate(2, sizeof(exch_buf_t));

	// open and setup stream
	if( (msink_ctx = mmf_sink_open(&rtsp_module))==NULL)
		goto fail;
        //audio will ignore these settings
	mmf_sink_ctrl(msink_ctx, CMD_SET_FRAMERATE, 30);
	mmf_sink_ctrl(msink_ctx, CMD_SET_BITRATE, 0);
        
#if SRC_UVC==1 || SRC_MJPG_FILE==1	
	mmf_sink_ctrl(msink_ctx, CMD_SET_CODEC, FMT_V_MJPG);
#elif SRC_H264_FILE==1
	mmf_sink_ctrl(msink_ctx, CMD_SET_CODEC, FMT_V_H264);
	mmf_sink_ctrl(msink_ctx, CMD_SET_SPS,(int)"Z0LADZp0BsHvN4CIAAADAAgAAAMBlHihVQ==");
	mmf_sink_ctrl(msink_ctx, CMD_SET_PPS,(int)"aM48gA==");
	mmf_sink_ctrl(msink_ctx, CMD_SET_LEVEL,(int)"42c00d");
#elif SRC_H264_UNIT==1
	mmf_sink_ctrl(msink_ctx, CMD_SET_CODEC, FMT_V_H264);
	mmf_sink_ctrl(msink_ctx, CMD_SET_SPS,(int)"Z0LADZp0BsHvN4CIAAADAAgAAAMBlHihVQ==");
	mmf_sink_ctrl(msink_ctx, CMD_SET_PPS,(int)"aM48gA==");
	mmf_sink_ctrl(msink_ctx, CMD_SET_LEVEL,(int)"42c00d");
#elif SRC_AAC_FILE==1
	mmf_sink_ctrl(msink_ctx, CMD_SET_SAMPLERATE, 44100);
	mmf_sink_ctrl(msink_ctx, CMD_SET_CHANNEL, 2);        
	mmf_sink_ctrl(msink_ctx, CMD_SET_CODEC, FMT_A_MP4A_LATM);
#elif SRC_PCMU_FILE==1
	mmf_sink_ctrl(msink_ctx, CMD_SET_SAMPLERATE, 8000);
	mmf_sink_ctrl(msink_ctx, CMD_SET_CHANNEL, 1);         
	mmf_sink_ctrl(msink_ctx, CMD_SET_CODEC, FMT_A_PCMU);
#elif SRC_I2S==1
	mmf_sink_ctrl(msink_ctx, CMD_SET_SAMPLERATE, 8000);
	mmf_sink_ctrl(msink_ctx, CMD_SET_CHANNEL, 1);        
        mmf_sink_ctrl(msink_ctx, CMD_SET_CODEC, FMT_A_PCMA);
#else
	#error NO SINK FORMAT
#endif
//apply all above setting
	mmf_sink_ctrl(msink_ctx, CMD_SET_APPLY, 0);

	mmf_sink_ctrl(msink_ctx, CMD_SET_INPUT_QUEUE, (int)src2sink_qid);
	mmf_sink_ctrl(msink_ctx, CMD_SET_OUTPUT_QUEUE, (int)sink2src_qid);       
	mmf_sink_ctrl(msink_ctx, CMD_SET_STREAMMING, ON);
        mmf_sink_ctrl(msink_ctx, CMD_SET_TASK_ON, 0);
	// open and setup media
#if SRC_UVC==1
	if((msrc_ctx = mmf_source_open(&uvc_module))==NULL)
#elif SRC_MJPG_FILE==1
	if((msrc_ctx = mmf_source_open(&mjpgf_module))==NULL)
#elif SRC_H264_FILE==1		
	if((msrc_ctx = mmf_source_open(&h264f_module))==NULL)
#elif SRC_H264_UNIT==1
	if((msrc_ctx = mmf_source_open(&h264_unit_module))==NULL)
#elif SRC_AAC_FILE==1		
	if((msrc_ctx = mmf_source_open(&aacf_module))==NULL)
#elif SRC_PCMU_FILE==1		
	if((msrc_ctx = mmf_source_open(&pcmuf_module))==NULL)
#elif SRC_I2S==1
        if((msrc_ctx = mmf_source_open(&i2s_module))==NULL)
#else
	#error NO SOURCE SELECTED
	if(1)
#endif
		goto fail;
	mmf_source_ctrl(msrc_ctx, CMD_SET_FRAMETYPE, FMT_V_MJPG);
	mmf_source_ctrl(msrc_ctx, CMD_SET_HEIGHT, 480);
	mmf_source_ctrl(msrc_ctx, CMD_SET_WIDTH, 640);
	mmf_source_ctrl(msrc_ctx, CMD_SET_FRAMERATE, 30);	
	mmf_source_ctrl(msrc_ctx, CMD_SET_CPZRATIO, 0);
	mmf_source_ctrl(msrc_ctx, CMD_SET_APPLY, 0);

	mmf_source_ctrl(msrc_ctx, CMD_SET_INPUT_QUEUE, (int)sink2src_qid);
	mmf_source_ctrl(msrc_ctx, CMD_SET_OUTPUT_QUEUE, (int)src2sink_qid);
	mmf_source_ctrl(msrc_ctx, CMD_SET_STREAMMING, ON);
        mmf_source_ctrl(msrc_ctx, CMD_SET_TASK_ON, 0);
//disable wifi power save        
        wifi_disable_powersave();
#if 0
	while(1){
		mmf_source_get_frame(msrc_ctx, &exbuf[0]);
		mmf_sink_put_frame(msink_ctx, &exbuf[0]);
		vTaskDelay(33);	
	}
#endif
    //    int toggle = 1;
	// TODO: exit condition or signal
	while(con)
        {
          vTaskDelay(1000);	
#if 0
          con--;
          if(con == 1)
          {
              con = 100;
              if(toggle>0)
              {
                  mmf_sink_ctrl(msink_ctx, CMD_SET_TASK_FROZEN, 0);
                  mmf_source_ctrl(msrc_ctx, CMD_SET_TASK_FROZEN, 0);
                  mmf_source_ctrl(msrc_ctx, CMD_SET_STREAMMING, OFF);                  
              }else{
                  mmf_source_ctrl(msrc_ctx, CMD_SET_STREAMMING, ON); 
                  mmf_source_ctrl(msrc_ctx, CMD_SET_TASK_ON, 0);
                  mmf_sink_ctrl(msink_ctx, CMD_SET_TASK_ON, 0);
              }
              toggle *= -1;
          }
#endif
          //if(con <= 0)
          //  break;
        }
	mmf_sink_ctrl(msink_ctx, CMD_SET_STREAMMING, OFF);
	mmf_source_ctrl(msrc_ctx, CMD_SET_STREAMMING, OFF);
	
fail:	
	mmf_sink_close(msink_ctx);
	mmf_source_close(msrc_ctx);
	vTaskDelete(NULL);
}


void example_media_ss(void)
{
	/*user can start their own task here*/
	if(xTaskCreate(example_media_ss_main, ((const char*)"example_media_ss"), 512, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS) {
		printf("\r\n example_media_ss_main: Create Task Error\n");
	}	
}


/************************************************************end of rtsp/rtp with motion-jpeg************************************************/
