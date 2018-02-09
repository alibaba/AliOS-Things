#include "example_audio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "diag.h"
#include "i2s_api.h"
#include "analogin_api.h"
#include <platform/platform_stdlib.h>

#include "sgtl5000.h"

#define CONFIG_PLAY_SD_WAV		0	// 1: play wav audio file store on SD card
									//0: play audio file store on memory
#define CONFIG_TUNE_VOLUME		0	// tune volume by trimmer resistor and ADC


#define I2S_DMA_PAGE_SIZE	512   // 2 ~ 4096
#define I2S_DMA_PAGE_NUM    4   // Vaild number is 2~4

/* play wav file on SD card */
#if CONFIG_PLAY_SD_WAV
#include "ff.h"
#include <fatfs_ext/inc/ff_driver.h>
#include <disk_if/inc/sdcard.h>
#include "wav.h"

SRAM_BF_DATA_SECTION u8 WAV_Buf[I2S_DMA_PAGE_SIZE];
#define SAMPLE_BLOCKS	(I2S_DMA_PAGE_SIZE/2)
#else
#include "birds_44100_2ch_16b.c"
#endif


u8 i2s_tx_buf[I2S_DMA_PAGE_SIZE*I2S_DMA_PAGE_NUM];
u8 i2s_rx_buf[I2S_DMA_PAGE_SIZE*I2S_DMA_PAGE_NUM];

#if defined(CONFIG_PLATFORM_8195A)
	#define I2S_SCLK_PIN            PC_1
	#define I2S_WS_PIN              PC_0
	#define I2S_SD_TX_PIN         PC_2
	#define I2S_SD_RX_PIN	         PC_4
	#define I2S_MCK_PIN              PC_3
#elif defined(CONFIG_PLATFORM_8711B)
	#define I2S_SCLK_PIN	         PA_21
	#define I2S_WS_PIN               PA_22
	#define I2S_SD_TX_PIN	         PA_19
	#define I2S_SD_RX_PIN	         PA_20
	#define I2S_MCK_PIN              PA_18
#endif 

i2s_t i2s_obj;

float volume_ratio = 0; // codec volume ration. 0.0 ~1.0


void i2s_tx_complete(void *data, char *pbuf)
{   
	//
}

void i2s_rx_complete(void *data, char* pbuf)
{
	//
}

#if CONFIG_PLAY_SD_WAV

void print_wav_header(WavHeader *pwavHeader){
	u8 buf[5];
	printf("================wav header===============\n");
	memcpy(buf, &pwavHeader->chunk_id[0], 4);
	buf[4] = 0;
	printf(" chunk_id: %s\n", buf);
	printf(" chunk_size: 0x%08X\n",pwavHeader->chunk_size);
	memcpy(buf, &pwavHeader->format[0], 4);
	buf[4] = 0;
	printf(" format: %s\n", buf);
	memcpy(buf, &pwavHeader->fmtchunk_id[0], 4);
	buf[4] = 0;
	printf(" fmtchunk_id: %s\n", buf);
	printf(" fmtchunk_size: 0x%08X\n",pwavHeader->fmtchunk_size);
	printf(" audio_format: 0x%04X\n",pwavHeader->audio_format);
	printf(" num_channels: 0x%04X\n",pwavHeader->num_channels);
	printf(" sample_rate: 0x%08X\n",pwavHeader->sample_rate);
	printf(" byte_rate: 0x%08X\n",pwavHeader->byte_rate);
	printf(" block_align: 0x%04X\n",pwavHeader->block_align);
	printf(" bps: 0x%04X\n",pwavHeader->bps);
	memcpy(buf, &pwavHeader->datachunk_id[0], 4);
	buf[4] = 0;
	printf(" datachunk_id: %s\n", buf);
	printf(" datachunk_size: 0x%08X\n",pwavHeader->datachunk_size);
	printf("==================end====================\n");
}


void audio_play_sd_wav(u8* filename){
	int drv_num = 0;
	u32 read_length = 0;
	FRESULT res; 
	FATFS 	m_fs;
	FIL		m_file;
	char	logical_drv[4]; /* root diretor */
	char abs_path[64];

	WavHeader pwavHeader;
    u32 wav_length = 0;
    u32 wav_offset = 0;
	int *ptx_buf;
	
	drv_num = FATFS_RegisterDiskDriver(&SD_disk_Driver);
	if(drv_num < 0){
		printf("Rigester disk driver to FATFS fail.\n");
		return;
	}else{
		logical_drv[0] = drv_num + '0';
		logical_drv[1] = ':';
		logical_drv[2] = '/';
		logical_drv[3] = 0;
	}

	if(f_mount(&m_fs, logical_drv, 1)!= FR_OK){
		printf("FATFS mount logical drive fail, please format DISK to FAT16/32.\n");
		goto unreg;
	}
	memset(abs_path, 0x00, sizeof(abs_path));
	strcpy(abs_path, logical_drv);
	sprintf(&abs_path[strlen(abs_path)],"%s", filename);

	//Open source file
	res = f_open(&m_file, abs_path, FA_OPEN_EXISTING | FA_READ); // open read only file
	if(res != FR_OK){
		printf("Open source file %(s) fail.\n", filename);
		goto umount;
	}
	
	/* Read WAV header */
	res = f_read(&m_file, WAV_Buf, sizeof(WavHeader),(UINT*)&read_length);	
	if((res != FR_OK) || (read_length == 0)){
		printf("Read wav header fail!\n");
	}

	memcpy(&pwavHeader, WAV_Buf, sizeof(WavHeader));
	
	print_wav_header(&pwavHeader);
	
	if (strncmp(pwavHeader.chunk_id, "RIFF", 4) ||
		strncmp(pwavHeader.format, "WAVE", 4)){
			printf("Not a wav file\n");
			goto exit;
	}
	
	/* only support PCM, 44.1K stereo wav file for this demo */
	if (pwavHeader.audio_format != 1 ||
		pwavHeader.sample_rate != 44100 ||
		pwavHeader.num_channels != 2){
		printf("Only PCM encoding 44.1K stereo wav file\n");
		goto exit;
	}
	
	/* Read wav data length */
	res = f_lseek(&m_file, m_file.fptr + pwavHeader.datachunk_size);
	res = f_read(&m_file, WAV_Buf, 8,(UINT*)&read_length);	
	if((res != FR_OK) || (read_length == 0)){
		printf("Read wav data info!\n");
	}
	if(strncmp(WAV_Buf, "data", 4)){
		printf("Have not found invalid data chunk\n");
		goto exit;
	}
 
	wav_length = *(u32*)&WAV_Buf[4];
	printf("Audio data total length 0x%08X\n", wav_length);

	do{
		/* Read a block */
		res = f_read(&m_file, WAV_Buf, I2S_DMA_PAGE_SIZE,(UINT*)&read_length);	
		if((res != FR_OK) || (read_length == 0)){
			printf("Wav play done !\n");
			break;
		}

		retry:
		ptx_buf = i2s_get_tx_page(&i2s_obj);
		if(ptx_buf){
			if(read_length < I2S_DMA_PAGE_SIZE){
				/* if valid audio data short than a page, make sure the rest of the page set to 0*/
				memset((void*)ptx_buf, 0x00, I2S_DMA_PAGE_SIZE);
				memcpy((void*)ptx_buf, (void*)&WAV_Buf[0], read_length);
			}else
				memcpy((void*)ptx_buf, (void*)&WAV_Buf[0], I2S_DMA_PAGE_SIZE);
			i2s_send_page(&i2s_obj, (uint32_t*)ptx_buf);
		}else{
			vTaskDelay(1);
			goto retry;
		}
		
		wav_offset += read_length;
	}while(1);

	printf("I2S write data length 0x%08X\n", wav_offset);
	
exit:	
	// close source file
	res = f_close(&m_file);
	if(res){
		printf("close file (%s) fail.\n", filename);
	}

umount:
	if(f_mount(NULL, logical_drv, 1) != FR_OK){
		printf("FATFS unmount logical drive fail.\n");
	}

unreg:	
	if(FATFS_UnRegisterDiskDriver(drv_num))	
		printf("Unregister disk driver from FATFS fail.\n");
	
}
#else
/* only for 16bit sampled audio data */
void audio_play_memory(void *audio, u32 length){
	int *ptx_buf;
	s16 *p = audio;
	u32 offset = 0;
	u32 sampleBlocks = length*2; // 2 channel 
	u32 MaxBlocksPerCycle = I2S_DMA_PAGE_SIZE/sizeof(short);
	u32 dataLen = 0;
again:	
	ptx_buf = i2s_get_tx_page(&i2s_obj);
	if (ptx_buf) {
		if(sampleBlocks-offset >= MaxBlocksPerCycle)
			dataLen = MaxBlocksPerCycle*sizeof(short);
		else{
			/* if valid audio data short than a page, make sure the rest of the page set to 0*/
			memset((void*)ptx_buf, 0x00, I2S_DMA_PAGE_SIZE);
			dataLen = (sampleBlocks-offset)*sizeof(short);
		}
			
		_memcpy((void*)ptx_buf, (void*)(p+offset), dataLen);
		i2s_send_page(&i2s_obj, (uint32_t*)ptx_buf);
		offset += dataLen/sizeof(short);
		if(offset >= sampleBlocks) {
			offset = 0;
			return;
		}
		goto again;
	}else{
		vTaskDelay(1);
		goto again;
	}
}

#endif


#if	CONFIG_TUNE_VOLUME
#define MBED_ADC_PIN_3    AD_3	// HDK, A2

/*
 * OFFSET:   value of measuring at 0.000v, value(0.000v)
 * GAIN_DIV: value(1.000v)-value(0.000v) or value(2.000v)-value(1.000v) or value(3.000v)-value(2.000v)
 *
 * MSB 12bit of value is valid, need to truncate LSB 4bit (0xABCD -> 0xABC). OFFSET and GAIN_DIV are truncated values.
 */
#define OFFSET 		0x298							
#define GAIN_DIV	0x34C							
#define AD2MV(ad,offset,gain) (((ad/16)-offset)*1000/gain)	
analogin_t   adc2;

void audio_tune_volume_thread(void* param){
	uint16_t offset, gain;
	uint16_t adcdat2    = 0;
	
	int32_t v_mv2;
	float ratio = 0;
	analogin_init(&adc2, MBED_ADC_PIN_3);
	offset = OFFSET;
	gain = GAIN_DIV;
	
	while(1){
		adcdat2 = analogin_read_u16(&adc2);
		v_mv2 = AD2MV(adcdat2, offset, gain);
		ratio = (float)v_mv2/(float)3300;
		ratio = (float)((int)(ratio*10))/10.0;
		if(ratio > 1.0)ratio = 1.0;
		if(ratio < 0.0)ratio = 0.0;
		if(ratio != volume_ratio){
			volume_ratio = ratio;
#if CONFIG_EXAMPLE_CODEC_SGTL5000
			sgtl5000_setVolume(volume_ratio);
#endif
		}
		vTaskDelay(100);
	}
exit:
	analogin_deinit(&adc2);
	vTaskDelete(NULL);
}
#endif

void example_audio_thread(void* param){
	printf("Audio codec demo begin......\n");

	//Init I2S first, generate MCLK to drive SGTL5000
	i2s_obj.channel_num = CH_STEREO;
	i2s_obj.sampling_rate = SR_44p1KHZ;
	i2s_obj.word_length = WL_16b;
	i2s_obj.direction = I2S_DIR_TXRX;	 
	i2s_init(&i2s_obj, I2S_SCLK_PIN, I2S_WS_PIN, I2S_SD_TX_PIN, I2S_SD_RX_PIN, I2S_MCK_PIN);
	i2s_set_dma_buffer(&i2s_obj, (char*)i2s_tx_buf, (char*)i2s_rx_buf, \
		I2S_DMA_PAGE_NUM, I2S_DMA_PAGE_SIZE);
	i2s_tx_irq_handler(&i2s_obj, (i2s_irq_handler)i2s_tx_complete, (uint32_t)&i2s_obj);
	i2s_rx_irq_handler(&i2s_obj, (i2s_irq_handler)i2s_rx_complete, (uint32_t)&i2s_obj);

//	i2s_recv_page(&i2s_obj); // sumbit a page to receive
#if CONFIG_EXAMPLE_CODEC_SGTL5000
	sgtl5000_enable();
#endif

#if CONFIG_EXAMPLE_CODEC_ALC5651
	alc5651_init();
	alc5651_init_interface2();
#endif

#if	CONFIG_TUNE_VOLUME
	/*Create a task to tune codec volume*/
	if(xTaskCreate(audio_tune_volume_thread, ((const char*)"audio_tune_volume_thread"), 2048, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
		printf("\n\r%s xTaskCreate(audio_tune_volume_thread) failed", __FUNCTION__);
#endif

#if CONFIG_PLAY_SD_WAV	
	char wav[20] = "AudioSDTest.wav";
	volume_ratio = 0.3;
#if CONFIG_EXAMPLE_CODEC_SGTL5000
	sgtl5000_setVolume(volume_ratio);
#endif
	printf("\nPlay %s on SD card.\n", wav);
	audio_play_sd_wav(wav);
#else
	volume_ratio = 0.7;
#if CONFIG_EXAMPLE_CODEC_SGTL5000
	sgtl5000_setVolume(volume_ratio);
#endif
	printf("\nPlay bird sing on memory.\n");
	while(1){
		audio_play_memory(birds_sing, birds_sing_size);
		wait_ms(1000);
	}
#endif	

exit:
	vTaskDelete(NULL);
}

void example_audio(void)
{
	if(xTaskCreate(example_audio_thread, ((const char*)"example_audio_thread"), 2048, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
		printf("\n\r%s xTaskCreate(example_audio_thread) failed", __FUNCTION__);
}

