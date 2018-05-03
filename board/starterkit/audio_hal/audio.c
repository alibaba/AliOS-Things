#include "audio.h"
#include "stm32l4xx_hal.h"

/* for l433 emb flash 256k */
#define MCU_FLASH_START_ADDR      0x8000000
#define MCU_FLASH_SIZE            0x40000

#define FLASH_MONO_DATA

typedef enum {
	SAI_dir_tx_m2p,
	SAI_dir_rx_p2m
} SAI_DIRECTION;

#define SAI_DATASIZE              8
#define SAI_DATA_BYTES            (SAI_DATASIZE / 8)
#define DATA_BUFF_LEN             (4096 / SAI_DATA_BYTES)

/* in second */
#define AUDIO_MAX_TIME						6

#ifdef FLASH_MONO_DATA
#define AUDIO_MAX_SIZE            (AUDIO_MAX_TIME * 8000 * SAI_DATA_BYTES)
#else
#define AUDIO_MAX_SIZE            (AUDIO_MAX_TIME * 8000 * SAI_DATA_BYTES * 2)
#endif

#define AUDIO_ADDRESS             (MCU_FLASH_START_ADDR + MCU_FLASH_SIZE - AUDIO_MAX_SIZE)

/* in millisecond */
#define DMA_WAIT_TIMEOUT          10000
#define SAI_WAIT_TIMEOUT          1000

#if (AUDIO_MAX_SIZE > MCU_FLASH_SIZE / 2)
#error "Be careful audio data override the code section!"
#endif

#if (SAI_DATASIZE == 8)
typedef uint8_t SAI_datatype;
#elif (SAI_DATASIZE == 16)
typedef uint16_t SAI_datatype;
#elif (SAI_DATASIZE == 32)
typedef uint32_t SAI_datatype;
#else
#error "Invalid sai datasize!"
#endif

static SAI_datatype data_buff[DATA_BUFF_LEN] = {0};
static __IO int16_t UpdatePointer = -1;
static aos_mutex_t sai_mutex;
static aos_sem_t audio_sem;

extern SAI_HandleTypeDef hsai_BlockA1;
extern DMA_HandleTypeDef hdma_sai1_a;

static uint32_t GetPage(uint32_t Addr)
{
  uint32_t page = 0;
  
  if (Addr < (FLASH_BASE + FLASH_BANK_SIZE))
  {
    /* Bank 1 */
    page = (Addr - FLASH_BASE) / FLASH_PAGE_SIZE;
  }
  else
  {
    /* Bank 2 */
    page = (Addr - (FLASH_BASE + FLASH_BANK_SIZE)) / FLASH_PAGE_SIZE;
  }
  
  return page;
}

int flash_write(uint32_t flash_addr, void *src, int size)
{
	FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t FirstPage = 0, NbOfPages = 0;
	uint32_t PAGEError = 0;
	uint64_t double_word = 0;
	uint32_t dst_addr = flash_addr;
	uint64_t *src_ptr = (uint64_t *)src;
	static uint32_t last_page = 0xffffffff;
	static uint32_t last_addr = 0xffffffff;
	int last_bytes = size;
	int ret = 0;
	
	ret = HAL_FLASH_Unlock();
	if (ret != 0) {
		KIDS_A10_PRT("HAL_FLASH_Unlock return %d\n", ret);
		return ret;
	}
	
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);
	
	FirstPage = GetPage(flash_addr);
	NbOfPages = GetPage(flash_addr + size - 1) - FirstPage + 1;
	printf("FirstPage = 0x%x, NbOfPages = 0x%x\n", FirstPage, NbOfPages);
	
	if (!(last_page == FirstPage && flash_addr >= last_addr && NbOfPages == 1)) {
		EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
		EraseInitStruct.Banks       = 1;
		EraseInitStruct.Page        = FirstPage;
		EraseInitStruct.NbPages     = NbOfPages;
		if (last_page == FirstPage && flash_addr >= last_addr && NbOfPages != 1) {
				EraseInitStruct.Page += 1;
				EraseInitStruct.NbPages -= 1;
		}
		ret = HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError);
		if (ret != 0) {
			KIDS_A10_PRT("HAL_FLASHEx_Erase return %d, PAGEError: 0x%x\n", ret, PAGEError);
			return ret;
		}
		last_page = FirstPage + NbOfPages - 1;
		last_addr = flash_addr + size - 1;
		printf("last_page = 0x%x, last_addr = 0x%x\n", last_page, last_addr);
	}

	while (last_bytes >= 8) {
		ret = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, dst_addr, *src_ptr);
		if (ret != 0) {
			KIDS_A10_PRT("HAL_FLASH_Program return %d\n", ret);
			return ret;
		}
		dst_addr += 8;
		++src_ptr;
		last_bytes -= 8;
	}
 	if (last_bytes != 0) {
		memcpy((void *)&double_word, (void *)src_ptr, last_bytes);
		ret = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, dst_addr, double_word);
		if (ret != 0) {
			KIDS_A10_PRT("HAL_FLASH_Program return %d\n", ret);
			return ret;
		}
	}
	ret = HAL_FLASH_Lock();
	if (ret != 0) {
		KIDS_A10_PRT("HAL_FLASH_Lock return %d\n", ret);
		return ret;
	}
	
	return 0;
}

static int reinit_sai_and_dma(SAI_DIRECTION dir)
{
	static SAI_DIRECTION sai_dma_status = SAI_dir_tx_m2p;

	if (dir == sai_dma_status)
		return 0;

	switch (dir) {
		case SAI_dir_tx_m2p:
			hal_gpio_output_high(&brd_gpio_table[GPIO_AUDIO_EN]);
			break;
		case SAI_dir_rx_p2m:
			hal_gpio_output_low(&brd_gpio_table[GPIO_AUDIO_EN]);
			break;
		default:
			KIDS_A10_PRT("Parameters is invalied, dir: %d\n", dir);
			return -1;
	}

	hsai_BlockA1.Init.AudioMode = (dir == SAI_dir_tx_m2p ? SAI_MODEMASTER_TX : SAI_MODEMASTER_RX);
	hsai_BlockA1.Init.ClockStrobing = (dir == SAI_dir_tx_m2p ? SAI_CLOCKSTROBING_FALLINGEDGE : SAI_CLOCKSTROBING_RISINGEDGE);
	if (HAL_SAI_Init(&hsai_BlockA1) != HAL_OK)
	{
		KIDS_A10_PRT("HAL_SAI_Init return failed.\n");
		return -1;
	}

	hdma_sai1_a.Init.Direction = (dir == SAI_dir_tx_m2p ? DMA_MEMORY_TO_PERIPH : DMA_PERIPH_TO_MEMORY);
	if (HAL_DMA_Init(&hdma_sai1_a) != HAL_OK)
	{
		KIDS_A10_PRT("HAL_DMA_Init return failed.\n");
		return -1;
	}
	sai_dma_status = dir;

	return 0;
}

/* stereo to mono */
static void ready_to_save(SAI_datatype *data, uint32_t data_num)
{
	uint32_t i;
	uint32_t frame_num = data_num / 2;
	
	if (data == NULL || data_num < 2) {
		KIDS_A10_PRT("Parameters is invalid.\n");
		return;
	}
	
	for (i = 1; i < frame_num; ++i)
		data[i] = data[i << 1];

	memset(&data[frame_num], 0x00, frame_num * SAI_DATA_BYTES);
}

/* mono to stereo */
static void ready_to_send(SAI_datatype *data, uint32_t data_num)
{
	uint32_t i;
	
	if (data == NULL || data_num < 1) {
		KIDS_A10_PRT("Parameters is invalid.\n");
		return;
	}
	
	for (i = data_num - 1; i > 0; --i) {
		data[i << 1] = data[i];
		data[(i << 1) + 1] = 0x00;
	}
	
	data[1] = 0x00;
}

void HAL_SAI_TxCpltCallback(SAI_HandleTypeDef *hsai)
{
  UpdatePointer = DATA_BUFF_LEN / 2;
	aos_sem_signal(&audio_sem);
}

void HAL_SAI_TxHalfCpltCallback(SAI_HandleTypeDef *hsai)
{
  UpdatePointer = 0;
	aos_sem_signal(&audio_sem);
}

void HAL_SAI_RxCpltCallback(SAI_HandleTypeDef *hsai)
{
  UpdatePointer = DATA_BUFF_LEN / 2;
	aos_sem_signal(&audio_sem);
}

void HAL_SAI_RxHalfCpltCallback(SAI_HandleTypeDef *hsai)
{
  UpdatePointer = 0;
	aos_sem_signal(&audio_sem);
}

int record_to_flash(void)
{
	int ret = 0;
	int pos_buff = 0;
	uint32_t pos_flash = 0;
	uint32_t part_len = DATA_BUFF_LEN / 2;
	uint32_t part_bytes = part_len * SAI_DATA_BYTES;
	
	if (!aos_mutex_is_valid(&sai_mutex)) {
		KIDS_A10_PRT("aos_mutex_is_valid return false.\n");
		return -1;
	}
	ret = aos_mutex_lock(&sai_mutex, SAI_WAIT_TIMEOUT);
	if (ret != 0) {
		KIDS_A10_PRT("SAI is very busy now.\n");
		return -1;
	}
	
	if (!aos_sem_is_valid(&audio_sem)) {
		KIDS_A10_PRT("aos_sem_is_valid return false.\n");
		return -1;
	}
	ret = reinit_sai_and_dma(SAI_dir_rx_p2m);
	if (ret != 0)
		return -1;

	ret = HAL_SAI_Receive_DMA(&hsai_BlockA1, (uint8_t *)data_buff, DATA_BUFF_LEN);
	if (ret != 0)	{
		KIDS_A10_PRT("HAL_SAI_Receive_DMA return failed.\n");
		return -1;
	}

#ifdef FLASH_MONO_DATA
	while (1) {
		/* Wait a callback event */
		while (UpdatePointer == -1) {
			ret = aos_sem_wait(&audio_sem, DMA_WAIT_TIMEOUT);
			if (ret != 0)
				KIDS_A10_PRT("DMA timeout.\n");
		}

		pos_buff = UpdatePointer;
		UpdatePointer = -1;

		/* Upate the first or the second part of the buffer */
		ready_to_save(&data_buff[pos_buff], part_len);
		ret = flash_write(AUDIO_ADDRESS + pos_flash, &data_buff[pos_buff], part_bytes / 2);
		if (ret != 0)
			break;

		pos_flash += part_bytes / 2;

		/* check the end of the file */
		if ((pos_flash + part_bytes / 2) > AUDIO_MAX_SIZE) {
			ret = HAL_SAI_DMAStop(&hsai_BlockA1);
			if (ret != 0) {
				KIDS_A10_PRT("HAL_SAI_DMAStop return failed.\n");
				ret = -1;
			}
			break;
		}

		if (UpdatePointer != -1) {
			/* Buffer update time is too long compare to the data transfer time */
			KIDS_A10_PRT("UpdatePointer error.\n");
			ret = -1;
			break;
		}
	}
#else
	while (1) {
		/* Wait a callback event */
		while (UpdatePointer == -1) {
			aos_sem_wait(&audio_sem, DMA_WAIT_TIMEOUT);
			if (ret != 0)
				KIDS_A10_PRT("DMA timeout.\n");
		}

		pos_buff = UpdatePointer;
		UpdatePointer = -1;

		/* Upate the first or the second part of the buffer */
		ret = flash_write(AUDIO_ADDRESS + pos_flash, &data_buff[pos_buff], part_bytes);
		if (ret != 0)
			break;

		pos_flash += part_bytes;

		/* check the end of the file */
		if ((pos_flash + part_bytes) > AUDIO_MAX_SIZE) {
			ret = HAL_SAI_DMAStop(&hsai_BlockA1);
			if (ret != 0) {
				KIDS_A10_PRT("HAL_SAI_DMAStop return failed.\n");
				ret = -1;
			}
			break;
		}

		if (UpdatePointer != -1) {
			/* Buffer update time is too long compare to the data transfer time */
			KIDS_A10_PRT("UpdatePointer error.\n");
			ret = -1;
			break;
		}
	}
#endif
	
	ret = aos_mutex_unlock(&sai_mutex);
	if (ret != 0) {
		KIDS_A10_PRT("SAI release failed.\n");
	}
	
	return ret;
}

int playback_from_flash(void)
{
	int ret = 0;
	int i;
	int pos_buff = 0;
	uint32_t pos_flash = 0;
	uint32_t part_len = DATA_BUFF_LEN / 2;
	uint32_t part_bytes = part_len * SAI_DATA_BYTES;
	
	if (!aos_mutex_is_valid(&sai_mutex)) {
		KIDS_A10_PRT("aos_mutex_is_valid return false.\n");
		return -1;
	}
	ret = aos_mutex_lock(&sai_mutex, SAI_WAIT_TIMEOUT);
	if (ret != 0) {
		KIDS_A10_PRT("SAI is very busy now.\n");
		return -1;
	}
	
	if (!aos_sem_is_valid(&audio_sem)) {
		KIDS_A10_PRT("aos_sem_is_valid return false.\n");
		return -1;
	}
	ret = reinit_sai_and_dma(SAI_dir_tx_m2p);
	if (ret != 0)
		return -1;
	
#ifdef FLASH_MONO_DATA
	for(i = 0; i < DATA_BUFF_LEN / 2; ++i) {
		data_buff[i] = *(__IO SAI_datatype *)(AUDIO_ADDRESS + pos_flash);
		pos_flash += SAI_DATA_BYTES;
	}
	
	ready_to_send(data_buff, DATA_BUFF_LEN / 2);
	
	ret = HAL_SAI_Transmit_DMA(&hsai_BlockA1, (uint8_t *)data_buff, DATA_BUFF_LEN);
	if (ret != 0) {
		KIDS_A10_PRT("HAL_SAI_Transmit_DMA return failed.\n");
		return -1;
	}

	while (1) {
		/* Wait a callback event */
		while (UpdatePointer == -1) {
			aos_sem_wait(&audio_sem, DMA_WAIT_TIMEOUT);
			if (ret != 0)
				KIDS_A10_PRT("DMA timeout.\n");
		}

		pos_buff = UpdatePointer;
		UpdatePointer = -1;

		/* Upate the first or the second part of the buffer */
		for (i = 0; i < part_len / 2; ++i) {
			data_buff[i + pos_buff] = *(__IO SAI_datatype *)(AUDIO_ADDRESS + pos_flash);
			pos_flash += SAI_DATA_BYTES; 
		}
		
		ready_to_send(&data_buff[pos_buff], part_len / 2);

		/* check the end of the file */
		if ((pos_flash + part_bytes / 2) > AUDIO_MAX_SIZE) {
			ret = HAL_SAI_DMAStop(&hsai_BlockA1);
			if (ret != 0) {
				KIDS_A10_PRT("HAL_SAI_DMAStop return failed.\n");
				ret = -1;
			}
			break;
		}

		if (UpdatePointer != -1) {
			/* Buffer update time is too long compare to the data transfer time */
			KIDS_A10_PRT("UpdatePointer error.\n");
			ret = -1;
			break;
		}
	}
#else
	for(i = 0; i < DATA_BUFF_LEN; ++i) {
		data_buff[i] = *(__IO SAI_datatype *)(AUDIO_ADDRESS + pos_flash);
		pos_flash += SAI_DATA_BYTES;
	}
	
	ret = HAL_SAI_Transmit_DMA(&hsai_BlockA1, (uint8_t *)data_buff, DATA_BUFF_LEN);
	if (ret != 0) {
		KIDS_A10_PRT("HAL_SAI_Transmit_DMA return failed.\n");
		return -1;
	}

	while (1) {
		/* Wait a callback event */
		while (UpdatePointer == -1) {
			aos_sem_wait(&audio_sem, DMA_WAIT_TIMEOUT);
			if (ret != 0)
				KIDS_A10_PRT("DMA timeout.\n");
		}

		pos_buff = UpdatePointer;
		UpdatePointer = -1;

		/* Upate the first or the second part of the buffer */
		for (i = 0; i < part_len; ++i) {
			data_buff[i + pos_buff] = *(__IO SAI_datatype *)(AUDIO_ADDRESS + pos_flash);
			pos_flash += SAI_DATA_BYTES; 
		}

		/* check the end of the file */
		if ((pos_flash + part_bytes) > AUDIO_MAX_SIZE) {
			ret = HAL_SAI_DMAStop(&hsai_BlockA1);
			if (ret != 0) {
				KIDS_A10_PRT("HAL_SAI_DMAStop return failed.\n");
				ret = -1;
			}
			break;
		}

		if (UpdatePointer != -1) {
			/* Buffer update time is too long compare to the data transfer time */
			KIDS_A10_PRT("UpdatePointer error.\n");
			ret = -1;
			break;
		}
	}
#endif
	
	ret = aos_mutex_unlock(&sai_mutex);
	if (ret != 0) {
		KIDS_A10_PRT("SAI release failed.\n");
	}
	
	return ret;
}

int audio_init(void)
{
	int ret = 0;
	
	if (aos_mutex_is_valid(&sai_mutex) || aos_sem_is_valid(&audio_sem)) {
		KIDS_A10_PRT("Audio module initialization had completed before now.\n");
		return -1;
	}
	
	ret = aos_mutex_new(&sai_mutex);
	if (ret != 0) {
		KIDS_A10_PRT("aos_mutex_new return failed.\n");
		return -1;
	}
	ret = aos_sem_new(&audio_sem, 0);
	if (ret != 0) {
		KIDS_A10_PRT("aos_sem_new return failed.\n");
		return -1;
	}
	
	return 0;
}
