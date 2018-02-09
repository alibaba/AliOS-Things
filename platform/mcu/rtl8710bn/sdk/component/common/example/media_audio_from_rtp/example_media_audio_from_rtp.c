#include "FreeRTOS.h"
#include "task.h"
#include "example_media_audio_from_rtp.h"

#include "mmf_source.h"
#include "mmf_sink.h"

void example_media_audio_from_rtp_main(void* param)
{
 
        int con = 1;
	msink_context *msink_ctx;
	msrc_context *msrc_ctx;
	
	xQueueHandle src2sink_qid;
	xQueueHandle sink2src_qid;
	
	src2sink_qid = xQueueCreate(3, sizeof(exch_buf_t));
	sink2src_qid = xQueueCreate(3, sizeof(exch_buf_t));

	// open and setup stream
#if CONFIG_EXAMPLE_MP3_STREAM_RTP  
	if( (msink_ctx = mmf_sink_open(&mp3_sink_module))==NULL)
		goto fail;	
	mmf_sink_ctrl(msink_ctx, CMD_SET_INPUT_QUEUE, (int)src2sink_qid);
	mmf_sink_ctrl(msink_ctx, CMD_SET_OUTPUT_QUEUE, (int)sink2src_qid);
	mmf_sink_ctrl(msink_ctx, CMD_SET_STREAMMING, ON);
        mmf_sink_ctrl(msink_ctx, CMD_SET_TASK_ON, 512);
#else
        if( (msink_ctx = mmf_sink_open(&i2s_sink_module))==NULL)
		goto fail;	
	mmf_sink_ctrl(msink_ctx, CMD_SET_INPUT_QUEUE, (int)src2sink_qid);
	mmf_sink_ctrl(msink_ctx, CMD_SET_OUTPUT_QUEUE, (int)sink2src_qid);
	mmf_sink_ctrl(msink_ctx, CMD_SET_STREAMMING, ON);
        mmf_sink_ctrl(msink_ctx, CMD_SET_TASK_ON, 0);
#endif
        
        if((msrc_ctx = mmf_source_open(&rtp_src_module))==NULL)
                goto fail;
        mmf_source_ctrl(msrc_ctx, CMD_SET_PRIV_BUF, 1);
        mmf_source_ctrl(msrc_ctx, CMD_SET_INPUT_QUEUE, (int)sink2src_qid);
	mmf_source_ctrl(msrc_ctx, CMD_SET_OUTPUT_QUEUE, (int)src2sink_qid);
	mmf_source_ctrl(msrc_ctx, CMD_SET_STREAMMING, ON);
        mmf_source_ctrl(msrc_ctx, CMD_SET_TASK_ON, 0);
        
        while(con)
        {
            vTaskDelay(100);
        }
	mmf_sink_ctrl(msink_ctx, CMD_SET_STREAMMING, OFF);
        mmf_source_ctrl(msrc_ctx, CMD_SET_PRIV_BUF, 0);        
	mmf_source_ctrl(msrc_ctx, CMD_SET_STREAMMING, OFF);        
fail:
	mmf_sink_close(msink_ctx);
	mmf_source_close(msrc_ctx);
	vTaskDelete(NULL);        
}

void example_media_audio_from_rtp(void)
{
	/*user can start their own task here*/
	if(xTaskCreate(example_media_audio_from_rtp_main, ((const char*)"example_media_audio_from_rtp_main"), 512, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS) {
		printf("\r\n example_media_audio_from_rtp_main: Create Task Error\n");
	}	
}