#include <k_api.h>
#include "audio.h"
#include "stm32l4xx_hal.h"
#include "soc_init.h"

#define FLASH_MONO_DATA

#define PART_FOR_AUDIO            HAL_PARTITION_CUSTOM_1

typedef enum {
	SAI_dir_tx_m2p,
	SAI_dir_rx_p2m
} SAI_DIRECTION;

#define SAMPLE_RATE               8000
#define SAI_DATASIZE              8
#define SAI_DATA_BYTES            (SAI_DATASIZE / 8)
#define DATA_BUFF_LEN             (4096 / SAI_DATA_BYTES)

/* in millisecond */
#define DMA_WAIT_TIMEOUT          10000
#define SAI_WAIT_TIMEOUT          1000

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

static uint32_t get_audio_part_len(void)
{
    hal_logic_partition_t audio_part;
    int32_t ret = 0;

    ret = hal_flash_info_get(PART_FOR_AUDIO, &audio_part);

    if (ret != 0) {
        return 0;
    }

    return audio_part.partition_length;
}

static float get_run_time(void)
{
	float time_in_sec = 0.0;

#ifdef FLASH_MONO_DATA
	time_in_sec = get_audio_part_len() / (SAMPLE_RATE * SAI_DATA_BYTES);
#else
	time_in_sec = get_audio_part_len() / (SAMPLE_RATE * SAI_DATA_BYTES * 2);
#endif

	return time_in_sec;
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

/**
* @brief This function handles DMA2 channel1 global interrupt.
*/
void DMA2_Channel1_IRQHandler(void)
{
	krhino_intrpt_enter();
	HAL_DMA_IRQHandler(&hdma_sai1_a);
	krhino_intrpt_exit();
}

/**
* @brief This function handles SAI1 global interrupt.
*/
void SAI1_IRQHandler(void)
{
	krhino_intrpt_enter();
	HAL_SAI_IRQHandler(&hsai_BlockA1);
	krhino_intrpt_exit();
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
	uint32_t total_size = get_audio_part_len();

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
		ret = -1;
		goto REC_EXIT;
	}
	ret = reinit_sai_and_dma(SAI_dir_rx_p2m);
	if (ret != 0) {
		ret = -1;
		goto REC_EXIT;
	}

	printf("Record time is %f seconds!\n", get_run_time());

	ret = HAL_SAI_Receive_DMA(&hsai_BlockA1, (uint8_t *)data_buff, DATA_BUFF_LEN);
	if (ret != 0) {
		KIDS_A10_PRT("HAL_SAI_Receive_DMA return failed.\n");
		ret = -1;
		goto REC_EXIT;
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
		ret = hal_flash_write(PART_FOR_AUDIO, &pos_flash, &data_buff[pos_buff], part_bytes / 2);
		if (ret != 0) {
			ret = -1;
			break;
		}

		/* check the end of the file */
		if ((pos_flash + part_bytes / 2) > total_size) {
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
		ret = hal_flash_write(PART_FOR_AUDIO, &pos_flash, &data_buff[pos_buff], part_bytes);
		if (ret != 0) {
			ret = -1;
			break;
		}

		/* check the end of the file */
		if ((pos_flash + part_bytes) > total_size) {
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

REC_EXIT:
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
	uint32_t total_size = get_audio_part_len();

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
		ret = -1;
		goto PB_EXIT;
	}
	ret = reinit_sai_and_dma(SAI_dir_tx_m2p);
	if (ret != 0) {
		ret = -1;
		goto PB_EXIT;
	}

	printf("Playback time is %f seconds!\n", get_run_time());

#ifdef FLASH_MONO_DATA
	ret = hal_flash_read(PART_FOR_AUDIO, &pos_flash, &data_buff[pos_buff], part_bytes);
	if (ret != 0) {
		ret = -1;
		goto PB_EXIT;
	}

	ready_to_send(data_buff, part_len);

	ret = HAL_SAI_Transmit_DMA(&hsai_BlockA1, (uint8_t *)data_buff, DATA_BUFF_LEN);
	if (ret != 0) {
		KIDS_A10_PRT("HAL_SAI_Transmit_DMA return failed.\n");
		ret = -1;
		goto PB_EXIT;
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
		ret = hal_flash_read(PART_FOR_AUDIO, &pos_flash, &data_buff[pos_buff], part_bytes / 2);
		if (ret != 0) {
			ret = -1;
			break;
		}

		ready_to_send(&data_buff[pos_buff], part_len / 2);

		/* check the end of the file */
		if ((pos_flash + part_bytes / 2) > total_size) {
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
	ret = hal_flash_read(PART_FOR_AUDIO, &pos_flash, &data_buff[pos_buff], DATA_BUFF_LEN * SAI_DATA_BYTES);
	if (ret != 0) {
		ret = -1;
		goto PB_EXIT;
	}

	ret = HAL_SAI_Transmit_DMA(&hsai_BlockA1, (uint8_t *)data_buff, DATA_BUFF_LEN);
	if (ret != 0) {
		KIDS_A10_PRT("HAL_SAI_Transmit_DMA return failed.\n");
		ret = -1;
		goto PB_EXIT;
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
		ret = hal_flash_read(PART_FOR_AUDIO, &pos_flash, &data_buff[pos_buff], part_bytes);
		if (ret != 0) {
			ret = -1;
			break;
		}

		/* check the end of the file */
		if ((pos_flash + part_bytes) > total_size) {
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

PB_EXIT:
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
