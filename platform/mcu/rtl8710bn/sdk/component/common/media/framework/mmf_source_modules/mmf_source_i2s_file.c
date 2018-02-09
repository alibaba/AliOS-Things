#include "mmf_source_i2s_file.h"

static i2s_buf_context i2s_buf;
static u8 i2s_tx_buf[I2S_DMA_PAGE_SIZE*I2S_DMA_PAGE_NUM];
static u8 i2s_rx_buf[I2S_DMA_PAGE_SIZE*I2S_DMA_PAGE_NUM];
static unsigned char encoder_buf[G711_FSIZE];
static i2s_buf_context * buf_ctx=NULL;
static i2s_t i2s_obj;

static void set_i2s_buf_context(i2s_buf_context * des){
	buf_ctx = des;
	buf_ctx -> raw_data = i2s_rx_buf;
}

int i2s_mod_handle(void* ctx, void* b)
{
	int ret = 0;
        static unsigned int time_count = 0;
	
	(void)ctx;
	
	exch_buf_t *exbuf = (exch_buf_t*)b;
	
	unsigned char * source =i2s_buf.raw_data;
	/*get uvc buffer for new data*/
	if(exbuf->state!=STAT_READY){	
		G711_encoder((short *)(i2s_buf.raw_data+i2s_buf.data_start), encoder_buf, i2s_buf.mode, G711_FSIZE);	
		i2s_buf.data_start=(i2s_buf.data_start+2*G711_FSIZE)%(I2S_DMA_PAGE_SIZE*RECV_PAGE_NUM);
		i2s_buf.len = (i2s_buf.data_end-i2s_buf.data_start+(I2S_DMA_PAGE_SIZE*RECV_PAGE_NUM))%(I2S_DMA_PAGE_SIZE*RECV_PAGE_NUM);
		exbuf->len =  G711_FSIZE;	       
		exbuf->index = 0;
		exbuf->data = encoder_buf;
                time_count+=160;
		exbuf->timestamp = time_count;
		exbuf->state = STAT_READY;
	}
	
	return 0;
}


static void i2s_tx_complete(void *data, char* pbuf){
	return ;
}

static void i2s_rx_complete(void *data, char* pbuf)
{
        i2s_t *obj = (i2s_t *)data;


        if(buf_ctx != NULL){// i2s write to buffer, rtsp read from buffer, so keeps len below half of the buffer

                if(buf_ctx->len > I2S_DMA_PAGE_SIZE*RECV_PAGE_NUM/2){
                        buf_ctx->data_start = (buf_ctx->data_start+I2S_DMA_PAGE_SIZE) % (I2S_DMA_PAGE_SIZE*RECV_PAGE_NUM);
                        buf_ctx->len -=I2S_DMA_PAGE_SIZE;
                }
                buf_ctx->data_end = (buf_ctx->data_end+I2S_DMA_PAGE_SIZE) % (I2S_DMA_PAGE_SIZE*RECV_PAGE_NUM);
                buf_ctx->len +=I2S_DMA_PAGE_SIZE;
                
        }


        i2s_recv_page(obj);    // submit a new page for receive  
            return ;
}

void i2s_mod_init()
{
        //i2s_t i2s_obj;
	i2s_buf.len = i2s_buf.data_start = i2s_buf.data_end = 0;
	set_i2s_buf_context(&i2s_buf);	
        
        vTaskDelay(1000);
        
	alc5651_init();
	alc5651_init_interface2();	// connect to ALC interface 2

	i2s_obj.channel_num = CH_MONO;
	i2s_obj.sampling_rate = SR_8KHZ;
	i2s_obj.word_length = WL_16b;
	i2s_obj.direction = I2S_DIR_RX;//TODO: need to check direction before i2s_set_dma_buffer
	i2s_init(&i2s_obj, I2S_SCLK_PIN, I2S_WS_PIN, I2S_SD_PIN);
	i2s_set_dma_buffer(&i2s_obj, (char*)i2s_tx_buf, (char*)i2s_rx_buf, \
        I2S_DMA_PAGE_NUM, I2S_DMA_PAGE_SIZE);
        i2s_tx_irq_handler(&i2s_obj, (i2s_irq_handler)i2s_tx_complete, (uint32_t)&i2s_obj);
        i2s_rx_irq_handler(&i2s_obj, (i2s_irq_handler)i2s_rx_complete, (uint32_t)&i2s_obj);
	/* rx need clock, let tx out first */
	switch(i2s_obj.direction){
	case I2S_DIR_TX:
		i2s_send_page(&i2s_obj, (uint32_t*)i2s_get_tx_page(&i2s_obj));
		break;
	case I2S_DIR_TXRX:
		i2s_send_page(&i2s_obj, (uint32_t*)i2s_get_tx_page(&i2s_obj));//don't break, need i2s_recv_page
	case I2S_DIR_RX:
		i2s_recv_page(&i2s_obj);
		break;
	default:
		break;
	}	
}

void* i2s_mod_open(void)
{
        i2s_mod_init();
        return (void*)1;
}

void i2s_mod_close(void* ctx)
{
        (void)ctx;
        return;
}

int i2s_mod_set_param(void* ctx, int cmd, int arg)
{
  	int ret = 0;
	
	switch(cmd){
	case CMD_SET_FRAMETYPE:
                if(arg == FMT_A_PCMU)
                  i2s_buf.mode = I2S_MODE_G711U;
                 else if(arg == FMT_A_PCMA)
                  i2s_buf.mode = I2S_MODE_G711A;
                 else
                  i2s_buf.mode = I2S_MODE_G711A;//default
		break;
	default:
		ret = -EINVAL;
		break;
	}
	return ret;
}

msrc_module_t i2s_module =
{
	.create = i2s_mod_open,
	.destroy = i2s_mod_close,
	.set_param = i2s_mod_set_param,
	.handle = i2s_mod_handle,
};