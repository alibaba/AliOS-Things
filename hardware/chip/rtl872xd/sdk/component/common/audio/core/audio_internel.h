#ifndef _AUDIO_RL6548_H
#define _AUDIO_RL6548_H

#include "ameba_soc.h"
#include "rl6548.h"

#define SP_DMA_PAGE_SIZE	1600
#define SP_DMA_PAGE_NUM    	4
#define SP_FULL_BUF_SIZE	128
#define SP_ZERO_BUF_SIZE	128

#define SET_VOLUME	CODEC_SetVolume
#define GET_VOLUME	CODEC_GetVolume
#define VOLUME_MAX (0xAF)
#define VOLUME_MIX (0x00)

//#define ENABLE_PA
typedef struct {
    GDMA_InitTypeDef       	SpTxGdmaInitStruct;              //Pointer to GDA_InitTypeDef
    GDMA_InitTypeDef       	SpRxGdmaInitStruct;              //Pointer to GDA_InitTypeDef		
}SP_GDMA_STRUCT, *pSP_GDMA_STRUCT;

typedef struct {
    u8 rx_gdma_own;
    u32 rx_addr;
    u32 rx_length;

}RX_BLOCK, *pRX_BLOCK;

typedef struct {
    RX_BLOCK rx_block[SP_DMA_PAGE_NUM];
    RX_BLOCK rx_full_block;
    u8 rx_gdma_cnt;
    u8 rx_usr_cnt;
    u8 rx_full_flag;

}SP_RX_INFO, *pSP_RX_INFO;

typedef struct {
    u8 tx_gdma_own;
    u32 tx_addr;
    u32 tx_length;
}TX_BLOCK, *pTX_BLOCK;

typedef struct {
    TX_BLOCK tx_block[SP_DMA_PAGE_NUM];
    TX_BLOCK tx_zero_block;
    u8 tx_gdma_cnt;
    u8 tx_usr_cnt;
    u8 tx_empty_flag;

}SP_TX_INFO, *pSP_TX_INFO;

typedef struct {
    u32 sample_rate;
    u32 word_len;
    u32 mono_stereo;
    u32 direction;
}SP_OBJ, *pSP_OBJ;


void *rl6548_audio_init(int sampleRate, int channels, int wordLen);
void rl6548_audio_deinit();

void rl6548_capture_start();
void rl6548_playback_start();
void rl6548_capture_stop();
void rl6548_playback_stop();

int rl6548_audio_write(void *h, uint8_t *buf, uint32_t size);
int rl6548_data_read(void *handle, void *buf, unsigned int size);


int rl6548_volume_get();
void rl6548_volume_set(int index);
int rl6548_get_mute();
void rl6548_set_mute(int mute);

#endif
