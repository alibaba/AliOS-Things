/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <aos/kernel.h>
#include "aos/aos.h"
#include "hal/soc/soc.h"

#include "ff.h"
#include "audio/manager/audio_manager.h"
#include "audio/pcm/audio_pcm.h"


#define SOUND_CARD_ID_IN 	AUDIO_CARD0

#define SOUND_CARD_ID 0
#define READ_CNT      25 
/* PCM_BUF_SIZE>=(channel*(16bit[simple rates]/8)*period_count*period_size)/2 */
//#define PCM_BUF_SIZE 		(1024 * 4)
#define PCM_BUF_CNT         4
#define PCM_PERIOD_SIZE     640
#define PCM_BUF_SIZE 		(PCM_PERIOD_SIZE*PCM_BUF_CNT)
//#define SAVE_TO_SD          1
static uint8_t pcm_data_buf[PCM_BUF_SIZE];

#define MMC_DISK_TAG  "0:/"
#define MMC_MUSIC_DIR "0:/music/1.pcm"

struct pcm_config pal_pcm_cfg;
void pcm_cfg_init(void)
{
	pal_pcm_cfg.channels = 1;
	pal_pcm_cfg.format = PCM_FORMAT_S16_LE;
	pal_pcm_cfg.period_count = PCM_BUF_CNT;
	
	pal_pcm_cfg.period_size = PCM_PERIOD_SIZE;
	//config.period_size = 320 * 2;
	
	pal_pcm_cfg.rate = 16000;
}

typedef void (*get_recode_stream_fn)(char * date,int size);

typedef struct{
	char *data;
	int size;
}t_fn_data;

volatile int call_cnt=0;

#ifdef SAVE_TO_SD
 	FIL pcm_fp;
#endif

void async_run(void * paras)
{
	t_fn_data *data=paras;
	if(data==NULL){
		return;
	}
#ifdef SAVE_TO_SD
       
		int f_sta = f_write(&pcm_fp, data->data, data->size, &write_l);
		if((f_sta != FR_OK) && (write_l != data->size))
		{
			LOG("!!!!!fat write error!!!!!!!\n");
			return -1;
		}
#endif		
}


int pal_pcm_recode_with_cb(get_recode_stream_fn fn)
{
	int ret = 0;
	int snd_read_num = 0;


    LOGI("palapp","pal_pcm_recode_with_cb\n");
	ret = snd_pcm_open(&pal_pcm_cfg, SOUND_CARD_ID_IN, PCM_IN);
	if (ret != 0) {
		LOGE("palapp","pcm open error\n");
		ret = -1;
	}
#ifdef SAVE_TO_SD
    char file_name[64];
	sprintf(file_name,"0:/%d.wav",aos_now());
	int f_sta = f_open(&pcm_fp, file_name, FA_CREATE_NEW | FA_WRITE | FA_READ);
	if(f_sta != FR_OK) {
		LOG("!!!!!fail to open fat!!!!!\n");
		return -1;
	}
#endif	

	int recd_count = READ_CNT;
	unsigned int write_l = 0;
	LOG("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
	LOG("~~~~~~~~~~~~~~~~~~~~~~~~Please say some thing to me~~~~~~~~~~~~~~~~~~~~~~~~~~~");
	draw_text(0,0,1,"Please say some thing to me");
	aos_msleep(500);
	while(recd_count --) {

		snd_read_num = snd_pcm_read(&pal_pcm_cfg, SOUND_CARD_ID_IN, pcm_data_buf, PCM_BUF_SIZE);
		if(snd_read_num == -1) {
			LOG("!!!!!!!!pcm read error!!!!!!!!\n");
			return -1;
		}
#ifdef SAVE_TO_SD

        
#endif
        
		int size=PCM_BUF_SIZE/PCM_BUF_CNT;
		for(int i=0;i<PCM_BUF_CNT;i++){
		    fn(pcm_data_buf+i*size,size);
			//aos_msleep(5);
		}
	}
	draw_text(0,0,1,"In the processing...");
	snd_pcm_close(SOUND_CARD_ID_IN, PCM_IN);
	LOG("recoder end!\n");
#ifdef SAVE_TO_SD
  	f_close(&pcm_fp);
#endif
	return ret;
} 

// #define USE_SDMMC_SAVE	1
// int pal_pcm_recode()
// {
// 	int ret = 0;
// 	int snd_read_num = 0;

// 	ret = snd_pcm_open(&pal_pcm_cfg, SOUND_CARD_ID_IN, PCM_IN);
// 	if (ret != 0) {
// 		LOG("pcm open error\n");
// 		ret = -1;
// 	}

// 	LOG("recoder start\n");
// /*
// 	snd_read_num = snd_pcm_read(&config, SOUND_CARD_ID_IN, pcm_data_buf, PCM_BUF_SIZE);
// 	if(snd_read_num == -1) {
// 		LOG("pcm read error\n");
// 		return -1;
// 	}
// */	
// #if USE_SDMMC_SAVE
// 	FIL fp;
// 	FRESULT f_sta;
// 	FATFS fs;
	
// 	f_sta = f_mount(&fs, MMC_DISK_TAG, 1);
// 	if(f_sta != FR_OK) {
// 		LOG("!!!!!fail to mount\n");
// 		return -1;
// 	}

// 	f_unlink(MMC_MUSIC_DIR);
// 	f_sta = f_open(&fp, MMC_MUSIC_DIR, FA_CREATE_NEW | FA_WRITE | FA_READ);
// 	if(f_sta != FR_OK) {
// 		LOG("!!!!!fail to open fat\n");
// 		return -1;
// 	}

// 	int recd_count = READ_CNT;
// 	unsigned int write_l = 0;
// 	LOGI("PALAPP","~~~~~~~~~~~~~~~~~~~please say some thing to me~~~~~~~~~~~~~~~~~~~~~~~~~~~");
// 	aos_msleep(1000);
// 	while(recd_count --) {
// 		snd_read_num = snd_pcm_read(&pal_pcm_cfg, SOUND_CARD_ID_IN, pcm_data_buf, PCM_BUF_SIZE);
// 		if(snd_read_num == -1) {
// 			LOG("!!!!!pcm read error\n");
// 			return -1;
// 		}
		
// 		f_sta = f_write(&fp, pcm_data_buf, snd_read_num, &write_l);
// 		if((f_sta != FR_OK) && (write_l != snd_read_num))
// 		{
// 			LOG("!!!!!fat write error\n");
// 			return -1;
// 		}
// 	}

// 	f_close(&fp);
// #endif

// 	snd_pcm_close(SOUND_CARD_ID_IN, PCM_IN);
// 	LOG("recoder end!\n");

// 	return ret;
// } 
// int pal_pcm_read_from_mmc(get_recode_stream_fn fn)
// {
// 	FRESULT f_sta;
// 	FIL fp;
// 	FATFS fs;

// 	int ret = 0;
// 	uint32_t read_len = 0;

// 	f_sta = f_mount(&fs, MMC_DISK_TAG, 1);
// 	if(f_sta != FR_OK) {
// 		LOG("fail to mount\n");
// 		return -1;
// 	}

// 	ret = snd_pcm_open(&pal_pcm_cfg, SOUND_CARD_ID, PCM_OUT);
// 	if (ret != 0) {
// 		LOG("pcm open error\n");
// 		return -1;
// 	}

// 	aud_mgr_handler(AUDIO_DEVICE_MANAGER_VOLUME, 10);
// 	f_open (&fp, MMC_MUSIC_DIR, FA_READ);
// 	LOG("play voice start!\n");

// 	while(1) {
// 		f_sta = f_read (&fp, pcm_data_buf, PCM_BUF_SIZE, &read_len);
// 		if(f_sta != FR_OK) {
// 			LOG("fs read error\n");
// 			ret = -1;
// 			goto over;
// 		}
// 		if(read_len != PCM_BUF_SIZE)
// 		break;
// 		//fn(pcm_data_buf,PCM_BUF_SIZE);
// 	    int size=PCM_BUF_SIZE/PCM_BUF_CNT;
// 		for(int i=0;i<PCM_BUF_CNT;i++){
// 		    fn(pcm_data_buf+i*size,size);

// 		}	
// 		// ret = snd_pcm_write(&pal_pcm_cfg, SOUND_CARD_ID, pcm_data_buf, PCM_BUF_SIZE);

// 		// if(ret != PCM_BUF_SIZE) {
// 		// 	LOG("pcm write error, %d\n", ret);
// 		// 	ret = -1;
// 		// 	goto over;
// 		// }
// 	}
// 	ret = snd_pcm_flush(&pal_pcm_cfg, SOUND_CARD_ID);
// 	if (ret != 0) 
// 		printf("pcm flush error\n");

// over:	
// 	LOG("play voice over!\n");
// 	snd_pcm_close(SOUND_CARD_ID, PCM_OUT);
// 	f_close(&fp);

// 	return ret;
// }



int pal_pcm_play(void)
{
	FRESULT f_sta;
	FIL fp;
	FATFS fs;

	int ret = 0;
	uint32_t read_len = 0;

	f_sta = f_mount(&fs, MMC_DISK_TAG, 1);
	if(f_sta != FR_OK) {
		LOG("fail to mount\n");
		return -1;
	}

	ret = snd_pcm_open(&pal_pcm_cfg, SOUND_CARD_ID, PCM_OUT);
	if (ret != 0) {
		LOG("pcm open error\n");
		return -1;
	}

	//aud_mgr_handler(AUDIO_DEVICE_MANAGER_VOLUME, 25);
	f_open (&fp, MMC_MUSIC_DIR, FA_READ);
	LOG("play voice start!\n");

	while(1) {
		f_sta = f_read (&fp, pcm_data_buf, PCM_BUF_SIZE, &read_len);
		if(f_sta != FR_OK) {
			LOG("fs read error\n");
			ret = -1;
			goto over;
		}
		if(read_len != PCM_BUF_SIZE)
			break;
		ret = snd_pcm_write(&pal_pcm_cfg, SOUND_CARD_ID, pcm_data_buf, PCM_BUF_SIZE);
		if(ret != PCM_BUF_SIZE) {
			LOG("pcm write error, %d\n", ret);
			ret = -1;
			goto over;
		}
	}
	ret = snd_pcm_flush(&pal_pcm_cfg, SOUND_CARD_ID);
	if (ret != 0) 
		printf("pcm flush error\n");

over:	
	LOG("play voice over!\n");
	snd_pcm_close(SOUND_CARD_ID, PCM_OUT);
	f_close(&fp);

	return ret;
}

// static int pcm_recode_play_demo(void)
// {
// 	int ret = 0;
	
// 	pcm_cfg_init();
	
// 	ret = pal_pcm_recode();
// 	if(ret == -1) {
// 		LOG("pcm recode error!\n");
// 		return -1;
// 	}

// 	ret = pal_pcm_play();
// 	if(ret == -1) {
// 		LOG("pcm play error!\n");
// 		return -1;
// 	}

// 	return 0;
// }

