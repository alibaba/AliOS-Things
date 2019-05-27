#include "driver_audio_if.h"
#include "mem_pub.h"
#include "i2s_pub.h"
#include "gpio_pub.h"
#include "uart_pub.h"
#include "drv_model_pub.h"
#include <string.h>
#include "driver_codec_es8374_pub.h"
#if (CONFIG_APP_MP3PLAYER==1)

typedef struct _driver_audio_codec_option_s
{
    void (*codec_init)();
    void (*codec_configure)(unsigned int, unsigned char);
    void (*codec_close)();
    void (*codec_volume_control)(unsigned char volume);
    void (*codec_mute_control)(BOOL enable);
}driver_audio_codec_option;


typedef struct driver_iis_global_setting_s
{
    UINT8 datawidth; 
    UINT8 volume;
    driver_audio_codec_option codec_ops; 
}driver_iis_global_setting_t;

volatile driver_iis_global_setting_t iis_global_setting;


static driver_audio_codec_option audio_codec_ops = 
{
    es8374_codec_init,
    es8374_codec_configure,
    es8374_codec_close,
    es8374_codec_volume_control,
    es8374_codec_mute_control
};

AUDIO_CTRL_BLK  audio_ctrl_blk = {0};

static uint16_t rb_get_fill_size(driver_ringbuff_t *rb)
{
    if (rb->buffp == NULL)
    {
        DRIVER_AUDIO_PRT("Buffer_null_1\n");
        return -1;
    }

    return rb->buffer_fill;
}


static uint16_t rb_get_buffer_size(driver_ringbuff_t *rb)
{

    if (rb->buffp == NULL)
    {
        DRIVER_AUDIO_PRT("Buffer_null_1\n");
        return -1;
    }

    return (rb->buffer_len - rb->buffer_fill);
}


static uint16_t  rb_fill_buffer( driver_ringbuff_t *rb, uint8_t *buff, uint16_t size)
{
    uint16_t buff_space, cpy_size, differ;

    GLOBAL_INT_DECLARATION();

    if (rb->buffp == NULL)
    {
        DRIVER_AUDIO_PRT("Buffer_null_3\n");
        return -1;
    }
    buff_space = rb_get_buffer_size(rb);

    if( buff_space >= size )
        cpy_size = size;
    else
    {
        cpy_size = buff_space;
    }

    if( rb->wptr + cpy_size > rb->buffer_len )
    {
        differ = rb->buffer_len - rb->wptr;
        memcpy( (uint8_t *)&rb->buffp[rb->wptr],
                buff, differ);
        memcpy( (uint8_t *)&rb->buffp[0], buff + differ, cpy_size - differ);
        rb->wptr = cpy_size - differ;
    }
    else
    {
        memcpy( (uint8_t *)&rb->buffp[rb->wptr],
                buff, cpy_size);
        rb->wptr += cpy_size;
    }

    if( rb->wptr >= rb->buffer_len)
        rb->wptr -= rb->buffer_len;

    GLOBAL_INT_DISABLE();
    rb->buffer_fill += cpy_size;
    GLOBAL_INT_RESTORE();

    return cpy_size;
}

static void rb_init( driver_ringbuff_t *rb, uint8_t *pbuff, uint16_t len )
{
    memset( (uint8_t *)rb, 0, sizeof(driver_ringbuff_t));

    rb->buffp = pbuff;
    rb->buffer_len = len;
    rb->buffer_fill = 0;

    return;
}
static int16_t rb_get_buffer_with_length( driver_ringbuff_t *rb, uint8_t *buff, uint16_t len )
{
    int32_t ret = 0;
    int32_t size1, size2;
    GLOBAL_INT_DECLARATION();

    if (rb->buffp == NULL)
    {
        DRIVER_AUDIO_PRT("Buffer_null_4\r\n");
        return 0;
     }

    if (!rb->buffer_fill)
        goto exit;

    if( rb->rptr >= rb->wptr )
    {
        size1 = rb->buffer_len - rb->rptr;
        size2 = rb->wptr;

        if( len < size1 )
        {
            memcpy( buff, &rb->buffp[rb->rptr], len );
            ret = len;
            rb->rptr += len;
        }
        else
        {
            memcpy( buff, &rb->buffp[rb->rptr], size1 );
            if( len - size1 < size2 )
            {
                memcpy( buff + size1, &rb->buffp[0], (len - size1) );
                ret = len;
                rb->rptr = len - size1;
            }
            else
            {
                memcpy( buff + size1, &rb->buffp[0], size2 );
                ret = size1 + size2;
                rb->rptr = size2;
            }
        }
    }
    else
    {
        size1 = rb->wptr - rb->rptr;

        if( len < size1 )
        {
            memcpy( buff, &rb->buffp[rb->rptr], len );
            ret = len;
            rb->rptr += len;
        }
        else
        {
            memcpy( buff, &rb->buffp[rb->rptr], size1 );
            ret = size1;
            rb->rptr += size1;

        }
    }

    GLOBAL_INT_DISABLE();
    rb->buffer_fill -= ret;
    GLOBAL_INT_RESTORE();

exit:
    return ret;
}


void aud_initial(uint32_t freq, uint32_t channels, uint32_t bits_per_sample)
{
	i2s_rate_t rate;
	rate.datawidth = bits_per_sample;
	rate.freq  = freq;

    sddev_control(I2S_DEV_NAME,I2S_CMD_SET_FREQ_DATAWIDTH,&rate);
	iis_global_setting.codec_ops.codec_configure(rate.freq,rate.datawidth);
	audio_ctrl_blk.channels = channels;
}

int32_t aud_hw_init(void)
{
//output 26M clk to dac MCLK
	UINT32 	param = GFUNC_MODE_CLK26M;
	sddev_control(GPIO_DEV_NAME, CMD_GPIO_ENABLE_SECOND, &param);
	
	vTaskDelay(10);
    sddev_control(I2S_DEV_NAME, I2S_HW_SET, (void *)0);
	
	memset((void *)&iis_global_setting, 0, sizeof(driver_iis_global_setting_t));
    iis_global_setting.codec_ops = audio_codec_ops;
	
	if(audio_ctrl_blk.data_buff == NULL)
    {
        audio_ctrl_blk.data_buff = os_malloc(AUDIO_BUFF_LEN);
        if(audio_ctrl_blk.data_buff == NULL)
        {
            DRIVER_AUDIO_PRT("===audio malloc error==\r\n");
            return -1;
        }
    }
	
	rb_init( &audio_ctrl_blk.aud_rb, (uint8_t *)audio_ctrl_blk.data_buff, AUDIO_BUFF_LEN );
	iis_global_setting.codec_ops.codec_init();
	aud_initial(44100,2,16);
	return 0;
}



void aud_open( void )
{
 	UINT8 enable = 1;
 	DRIVER_AUDIO_PRT("aud open\r\n");
    sddev_control(I2S_DEV_NAME, I2S_CMD_ACTIVE, (void *)&enable);
	iis_global_setting.codec_ops.codec_mute_control(0);
}

void aud_close(void)
{
	UINT8 enable = 0;
	DRIVER_AUDIO_PRT("aud close\r\n");
	iis_global_setting.codec_ops.codec_mute_control(1);
    sddev_control(I2S_DEV_NAME, I2S_CMD_ACTIVE, (void *)&enable);
}

uint8_t is_aud_opened(void)
{
	return is_i2s_active();
}

uint16_t aud_get_buffer_size(void)
{
    return rb_get_buffer_size( &audio_ctrl_blk.aud_rb );
}

void  aud_fill_buffer( uint8_t *buff, uint16_t len )
{
    rb_fill_buffer(&audio_ctrl_blk.aud_rb, buff, len);
}

uint16_t aud_get_buffer_length(uint8_t *buff, uint16_t len)
{
    return rb_get_buffer_with_length(&audio_ctrl_blk.aud_rb, buff, len);
}

uint8_t aud_get_channel(void)
{
    uint8_t ret;
    
    if(audio_ctrl_blk.data_buff)
    {
        ret = audio_ctrl_blk.channels;
    }
    else
    {
        ret = 0;
    }
    
    return ret;
}

uint16_t aud_get_fill_size(void)
{
    if(audio_ctrl_blk.data_buff)
    {
        return rb_get_fill_size(&audio_ctrl_blk.aud_rb);
    }

    return 0;
}
// eof
#endif
