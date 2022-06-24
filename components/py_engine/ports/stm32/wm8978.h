/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef MICROPY_INCLUDED_STM32_WM8978_H
#define MICROPY_INCLUDED_STM32_WM8978_H
#if MICROPY_HW_WM8978

#include "lib/oofatfs/ff.h"
#include "extmod/vfs.h"
#include "extmod/vfs_fat.h"

//======================================================================
#if defined(STM32F4)
#define WAV_I2S_TX_DMA_BUFSIZE		8192
#define I2S_RX_DMA_BUF_SIZE    		8192*10
#elif defined(STM32F7)
#define WAV_I2S_TX_DMA_BUFSIZE		8192
#define I2S_RX_DMA_BUF_SIZE    		8192*10
#elif defined(STM32H7)
#define WAV_I2S_TX_DMA_BUFSIZE		1024*100
#define I2S_RX_DMA_BUF_SIZE    		1024*32
#endif
	
//======================================================================
//音乐播放控制器
typedef  struct
{  
#if defined(STM32F4) || defined(STM32F7)
	uint8_t *i2sbuf1;
	uint8_t *i2sbuf2; 
#elif defined(STM32H7)
	 uint8_t i2sbuf1[WAV_I2S_TX_DMA_BUFSIZE];
	 uint8_t i2sbuf2[WAV_I2S_TX_DMA_BUFSIZE];
#endif
	uint8_t *tbuf; 
	FIL *file;
	char *audioName;
	volatile uint8_t status; 			//bit0:0,暂停播放;1,继续播放
							//bit1:0,结束播放;1,开启播放 
}__attribute__((packed)) __audiodev;

extern __audiodev audiodev;	//音乐播放控制器

//-----------------------------------------

//RIFF块
typedef struct
{
    uint32_t ChunkID;
    uint32_t ChunkSize ;
    uint32_t Format;
}__attribute__((packed)) ChunkRIFF ;
//fmt块
typedef struct
{
	uint32_t ChunkID;
	uint32_t ChunkSize ;
	uint16_t AudioFormat;
	uint16_t NumOfChannels;
	uint32_t SampleRate;
	uint32_t ByteRate;
	uint16_t BlockAlign;
	uint16_t BitsPerSample;
}__attribute__((packed)) ChunkFMT;	   
//fact块 
typedef struct 
{
    uint32_t ChunkID;
    uint32_t ChunkSize ;
    uint32_t NumOfSamples;
}__attribute__((packed)) ChunkFACT;
//LIST块 
typedef struct 
{
    uint32_t ChunkID;		
    uint32_t ChunkSize ;
}__attribute__((packed)) ChunkLIST;

//data块 
typedef struct 
{
    uint32_t ChunkID;
    uint32_t ChunkSize ;
}__attribute__((packed)) ChunkDATA;

//wav头
typedef struct
{ 
	ChunkRIFF riff;
	ChunkFMT fmt;
	ChunkDATA data;
}__attribute__((packed)) __WaveHeader; 

typedef struct
{ 
	uint16_t audioformat;
	uint16_t nchannels;
	uint16_t blockalign;
	uint32_t datasize;
	uint32_t totsec ;
	uint32_t cursec ;	
	uint32_t bitrate;	
	uint32_t samplerate;
	uint16_t bps;
	uint32_t datastart;
}__attribute__((packed)) __wavctrl; 
//====================================================================================================

extern void (*i2s_tx_callback)(void);
extern void (*i2s_rx_callback)(void);
extern DMA_HandleTypeDef I2S2_TXDMA_Handler;
extern DMA_HandleTypeDef I2S2_RXDMA_Handler;

//======================================================================
 extern const mp_obj_type_t audio_wm8978_type;

 extern void wm8978_init(void);
 extern void wm8978_spk_vol(uint8_t volx);
 extern void wm8978_adda_cfg(uint8_t dacen,uint8_t adcen);
 extern void wm8978_input_cfg(uint8_t micen,uint8_t lineinen,uint8_t auxen);
 extern void wm8978_output_cfg(uint8_t dacen,uint8_t bpsen);
 extern void wm8978_hspk_vol(uint8_t voll,uint8_t volr);
 extern void WM8978_I2S_CFG(uint8_t fmt,uint8_t len);

#if defined(STM32F4)
extern void audio_init(uint32_t I2S_Standard,uint32_t I2S_Mode,uint32_t I2S_Clock_Polarity,uint32_t I2S_DataFormat);
#elif defined(STM32F7)
void I2Sxext_RX_DMA_STREAM_IRQFUN(void);
extern void audio_init(uint32_t I2S_Standard,uint32_t I2S_Mode,uint32_t I2S_Clock_Polarity,uint32_t I2S_DataFormat);
#elif defined(STM32H7)
extern void audio_init(uint32_t I2S_Standard,uint32_t I2S_Mode,uint32_t I2S_Clock_Polarity,uint32_t I2S_DataFormat,uint32_t samplerate);
#endif
 
 extern uint8_t I2S2_SampleRate_Set(uint32_t samplerate);

 extern void I2S2_TX_DMA_Init(uint8_t* buf0,uint8_t *buf1,uint32_t num);

#endif
#endif // MICROPY_INCLUDED_STM32_WM8978_H
