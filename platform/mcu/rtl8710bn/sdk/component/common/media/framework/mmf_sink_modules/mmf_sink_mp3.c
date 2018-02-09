#include "mmf_sink_mp3.h"

/*SDRAM_DATA_SECTION*/ //unsigned char MP3_ip[640];
signed short WAV_op[4608];

#define I2S_DMA_PAGE_SIZE	4608   // 2 ~ 4096
#define I2S_DMA_PAGE_NUM    2   // Vaild number is 2~4

static u8 i2s_tx_buf[I2S_DMA_PAGE_SIZE*I2S_DMA_PAGE_NUM];
static u8 i2s_rx_buf[I2S_DMA_PAGE_SIZE*I2S_DMA_PAGE_NUM];

#define I2S_SCLK_PIN            PC_1
#define I2S_WS_PIN              PC_0
#define I2S_SD_PIN              PC_2

static i2s_t i2s_obj;

mp3_decoder_t mp3;
mp3_info_t info;
int frame_size = 0;
int *ptx_buf;


static void i2s_tx_complete(void *data, char *pbuf)
{   
	//
}

static void i2s_rx_complete(void *data, char* pbuf)
{
	//
}

void cleanup()
{
	mp3_done(mp3);
}

void mp3_sink_mod_close(void* ctx)
{
	cleanup();
}

void* mp3_sink_mod_open(void)
{
	i2s_obj.channel_num = CH_STEREO;
	i2s_obj.sampling_rate = I2S_SR_44p1KHZ;
	i2s_obj.word_length = WL_16b;
	i2s_obj.direction = I2S_DIR_TXRX;	 
	i2s_init(&i2s_obj, I2S_SCLK_PIN, I2S_WS_PIN, I2S_SD_PIN);
	i2s_set_dma_buffer(&i2s_obj, (char*)i2s_tx_buf, (char*)i2s_rx_buf, \
		I2S_DMA_PAGE_NUM, I2S_DMA_PAGE_SIZE);
	i2s_tx_irq_handler(&i2s_obj, (i2s_irq_handler)i2s_tx_complete, (uint32_t)&i2s_obj);
	i2s_rx_irq_handler(&i2s_obj, (i2s_irq_handler)i2s_rx_complete, (uint32_t)&i2s_obj);
	mp3 = mp3_create();
	if(!mp3)
	{
		printf("mp3 context create fail\n");
		cleanup();
	}
        
#if CONFIG_EXAMPLE_MP3_STREAM_SGTL5000
        sgtl5000_enable();
        sgtl5000_setVolume(0.5);
#else
	alc5651_init();
	alc5651_init_interface2();    
#endif
        return (void*)1;
}

int mp3_sink_mod_set_param(void* ctx, int cmd, int arg)
{
  return 1;
}


int mp3_sink_mod_handle(void* ctx, void* b)
{
        exch_buf_t *exbuf = (exch_buf_t*)b;
	if(exbuf->state != STAT_READY)
        return -EAGAIN;

        /* Read a block */
        frame_size = mp3_decode(mp3, (exbuf->data), (exbuf->len), WAV_op, &info);
        retry:
        ptx_buf = i2s_get_tx_page(&i2s_obj);
        if(ptx_buf){
                if((frame_size>0)&&(info.audio_bytes>0))
                {
                        memcpy((void*)ptx_buf, (void*)WAV_op, info.audio_bytes);
                        i2s_send_page(&i2s_obj, (uint32_t*)ptx_buf);
                }
        }else{
                vTaskDelay(1);
                goto retry;
        }
   exbuf->state = STAT_USED;
   return 1;

}

msink_module_t mp3_sink_module = 
{
	.create = mp3_sink_mod_open,
	.destroy = mp3_sink_mod_close,
	.set_param = mp3_sink_mod_set_param,
	.handle = mp3_sink_mod_handle,
};
