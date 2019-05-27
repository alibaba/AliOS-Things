#ifndef __AUDIO_PUB_H__
#define __AUDIO_PUB_H__

#define AUD_FAILURE                  (1)
#define AUD_SUCCESS                  (0)

#define AUD_DAC_DEV_NAME             "aud_dac"
#define AUD_DAC_CMD_MAGIC            (0x1EBC0000)

typedef struct aud_dac_cfg_st
{
    UINT8 *buf;
    UINT16 buf_len;
    UINT16 freq;
    UINT16 channels;     
    UINT16 dma_mode;
    UINT16 mute_pin;
    UINT16 def_volume;    
} AUD_DAC_CFG_ST, *AUD_DAC_CFG_PTR;

enum
{
    AUD_DAC_CMD_GET_FREE_BUF_SIZE = AUD_DAC_CMD_MAGIC + 1,
    AUD_DAC_CMD_PLAY,    
    AUD_DAC_CMD_PAUSE,
    AUD_DAC_CMD_SET_SAMPLE_RATE,
    AUD_DAC_CMD_SET_VOLUME,    
};

#include "gpio_pub.h"
#if AUD_USE_EXT_PA
#define AUD_DAC_MUTE_PIN            GPIO9
#define AUD_DAC_MUTE_ENA_LEVEL      GPIO_INT_LEVEL_LOW
#endif


///////////////////////////////////////////////////////////////////////////////


#define AUD_ADC_DEV_NAME             "aud_adc"
#define AUD_ADC_CMD_MAGIC            (0x2EBC0000)
typedef struct aud_adc_cfg_st
{
    UINT8 *buf;
    UINT16 buf_len;
    UINT16 freq;
    UINT16 channels;
    UINT16 mode;
    UINT32 linein_detect_pin;
} AUD_ADC_CFG_ST, *AUD_ADC_CFG_PTR;

enum
{
    AUD_ADC_CMD_GET_FILL_BUF_SIZE = AUD_ADC_CMD_MAGIC + 1,
    AUD_ADC_CMD_PLAY,    
    AUD_ADC_CMD_PAUSE,
    AUD_ADC_CMD_DO_LINEIN_DETECT,
    AUD_ADC_CMD_SET_SAMPLE_RATE,
    AUD_ADC_CMD_SET_VOLUME
};


#define AUD_ADC_LINEIN_DETECT_PIN           GPIO8
#define AUD_ADC_LINEIN_ENABLE_LEVEL         0
#define AUD_ADC_DAC_HARDWARD_LOOPBACK       0

#define AUD_ADC_MODE_DMA_BIT                (1 << 0)  // 1: DMA MODE, 0: ISR MODE
#define AUD_ADC_MODE_LINEIN                 (1 << 1)  // 1: LINEIN, 0: MIC 
#define AUD_ADC_DEF_WR_THRED                (8)
#define AUD_ADC_DEF_GAIN                    (0x2D)   // 0dm
#define AUD_ADC_MAX_THRED                   (0x10)
#define AUD_ADC_MAX_VOLUME                  (124)

void audio_init(void);
void audio_exit(void);



#endif // __AUDIO_PUB_H__