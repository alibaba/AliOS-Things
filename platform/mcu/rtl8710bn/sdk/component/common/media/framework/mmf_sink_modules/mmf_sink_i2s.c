#include "mmf_sink.h"
#include "i2s_api.h"
#include "g711/g711_codec.h"

#define BUF_BLK 3 //MUST BE GREATER OR EQUAL TO 2
static u8 i2s_dec_buf[320]; //store decoded data
static u8 i2s_chl_buf[640*(BUF_BLK+1)]; //store mono2stereo data
#define I2S_DMA_PAGE_NUM 2
#define I2S_DMA_PAGE_SIZE (640) 

static u8 i2s_tx_buf[I2S_DMA_PAGE_SIZE*I2S_DMA_PAGE_NUM];
static u8 i2s_rx_buf[I2S_DMA_PAGE_SIZE*I2S_DMA_PAGE_NUM];

#define I2S_SCLK_PIN            PC_1
#define I2S_WS_PIN              PC_0
#define I2S_SD_PIN              PC_2

static u32 update_flag = 0;
static u32 update_token = 0;
static u32 prev_token = 0;

u8 *play_ptr = i2s_chl_buf;
static void i2s_tx_complete(void *data, char* pbuf){
    i2s_t *obj = (i2s_t *)data;
    int *ptx_buf; 
    ptx_buf = i2s_get_tx_page(obj);
    if(update_token > prev_token)
        update_flag = 1;
    else
        update_flag = 0;    
    if(update_flag)
    {
        _memcpy((void*)ptx_buf, (void*)play_ptr, I2S_DMA_PAGE_SIZE);

        play_ptr += I2S_DMA_PAGE_SIZE;
        if(play_ptr >= i2s_chl_buf + BUF_BLK*I2S_DMA_PAGE_SIZE)
        {

            play_ptr = i2s_chl_buf;
        }        
        update_token--;
    }
    else
        _memset((void*)ptx_buf, 0, I2S_DMA_PAGE_SIZE);
    i2s_send_page(obj, (uint32_t*)ptx_buf);    
    //printf("\n\r%d", update_token);  
}

static void i2s_rx_complete(void *data, char* pbuf){
    return;
}

void i2s_sink_mod_close(void* ctx)
{
    i2s_t* i2s_obj = (i2s_t*) ctx;
    i2s_deinit(i2s_obj);
    free(i2s_obj);
}

void* i2s_sink_mod_open(void)
{
    i2s_t* i2s_obj = malloc(sizeof(i2s_t));
    if(i2s_obj == NULL)
      return NULL;
    
    alc5651_init();
    alc5651_init_interface2();	// connect to ALC interface 2
    //default setting i2s obj
    i2s_obj->channel_num = /*CH_MONO*/CH_STEREO;
    i2s_obj->sampling_rate = I2S_SR_8KHZ;
    i2s_obj->word_length = WL_16b;
    i2s_obj->direction = I2S_DIR_TX;
    
    i2s_init(i2s_obj, I2S_SCLK_PIN, I2S_WS_PIN, I2S_SD_PIN);
    i2s_set_dma_buffer(i2s_obj, (char*)i2s_tx_buf, (char*)i2s_rx_buf, \
        I2S_DMA_PAGE_NUM, I2S_DMA_PAGE_SIZE);
    i2s_tx_irq_handler(i2s_obj, (i2s_irq_handler)i2s_tx_complete, (uint32_t)i2s_obj);
    i2s_rx_irq_handler(i2s_obj, (i2s_irq_handler)i2s_rx_complete, (uint32_t)i2s_obj);
    return i2s_obj;
}

int i2s_sink_mod_set_param(void* ctx, int cmd, int arg)
{
    i2s_t* i2s_obj = (i2s_t*) ctx;
    switch(cmd){
        case CMD_SET_STREAMMING:
            if(arg == ON){
                i2s_send_page(i2s_obj, (uint32_t*)i2s_get_tx_page(i2s_obj));
            }else{
            
            }
            break;
        default:
            break;
    }
    return 0;
}

//send audio data here
static int cache_byte = 0;
u8 *cache_ptr = i2s_chl_buf;
int i2s_sink_mod_handle(void* ctx, void* b)
{
    i2s_t* i2s_obj = (i2s_t*) ctx; 
    exch_buf_t *exbuf = (exch_buf_t*)b; 
    if(exbuf->state != STAT_READY)
        return -EAGAIN;    
    G711_decoder(exbuf->data, (short*)i2s_dec_buf, I2S_MODE_G711U, exbuf->len);
    //mono2stereo(i2s_dec_buf, i2s_chl_buf, 320, 16);
    mono2stereo(i2s_dec_buf, cache_ptr, exbuf->len*2, 16);
    cache_byte += exbuf->len*4;
    cache_ptr += exbuf->len*4;
    if(cache_ptr >= i2s_chl_buf + BUF_BLK*I2S_DMA_PAGE_SIZE)
    {
        memcpy(i2s_chl_buf, i2s_chl_buf + BUF_BLK*I2S_DMA_PAGE_SIZE, cache_ptr - (i2s_chl_buf + BUF_BLK*I2S_DMA_PAGE_SIZE));
        cache_ptr -= BUF_BLK*I2S_DMA_PAGE_SIZE;
    }
    if(cache_byte >= I2S_DMA_PAGE_SIZE)
    {
        cache_byte %= I2S_DMA_PAGE_SIZE;
        //printf("\n\rcache:%dB", cache_byte);
    update_token++;
    }
    //printf("\n\r%d", update_token);
    while(update_token >= prev_token + BUF_BLK - 1)//MINUS 1 TO AVOID RING BUFFER OUT RACE
        vTaskDelay(1);
    exbuf->state = STAT_USED;    
    return 0;
}

msink_module_t i2s_sink_module = 
{
	.create = i2s_sink_mod_open,
	.destroy = i2s_sink_mod_close,
	.set_param = i2s_sink_mod_set_param,
	.handle = i2s_sink_mod_handle,
};
