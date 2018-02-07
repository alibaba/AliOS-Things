
#include "driver/chip/hal_def.h"
#include "driver/chip/hal_adc.h"
#include "aos/hal/adc_priv.h"
#include "hal/soc/soc.h"
#include <aos/kernel.h>
#include <aos/aos.h>

/*
  0x0                        0x100000
 |--------------------------|--------------------------|
 |bootloader(32K)           |ota cfg(32K)              |
 |--------------------------|--------------------------|
 |application               |ota                       |
 |                          |                          |
 |                          |                          |
 |                          |                          |
 |                          |                          |
 |                          |                          |
 |                          |                          |
 |                          |                          |
 |                          |                          |
 |--------------------------|--------------------------|
 |                          |PARAM1(4K)                |
 |--------------------------|--------------------------|
 |                          |PARAM2(4K)                |
 |--------------------------|--------------------------|
 |                          |PARAM3(4K)                |
 |--------------------------|--------------------------|
 |SYSINFO(4K)               |PARAM4(4K)                |
 |--------------------------|--------------------------|
 */

#define SPI_FLASH_CAPACITY	0x200000UL //total capacity
#define OTA_2ND_SEQ_START	0x100000UL //according to image-xip-xz.cfg file

/* Logic partition on flash devices */
const hal_logic_partition_t hal_partitions[] =
{
	[HAL_PARTITION_BOOTLOADER] =
	{
		.partition_owner          = HAL_FLASH_EMBEDDED,
		.partition_description    = "Bootloader",
		.partition_start_addr     = 0x0,
		.partition_length         = 0x8000,	//32k bytes
		.partition_options        = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
	},
	[HAL_PARTITION_PARAMETER_1] =
	{
		.partition_owner          = HAL_FLASH_EMBEDDED,
		.partition_description    = "PARAMETER1",
		.partition_start_addr     = SPI_FLASH_CAPACITY - 4 * 0x1000,
		.partition_length         = 0x1000, // 4k bytes
		.partition_options        = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
	},
	[HAL_PARTITION_PARAMETER_2] =
	{
		.partition_owner          = HAL_FLASH_EMBEDDED,
		.partition_description    = "PARAMETER2",
		.partition_start_addr     = SPI_FLASH_CAPACITY - 3 * 0x1000,
		.partition_length         = 0x1000, //4k bytes
		.partition_options        = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
	},
	[HAL_PARTITION_APPLICATION] =
	{
		.partition_owner          = HAL_FLASH_EMBEDDED,
		.partition_description    = "Application",
		.partition_start_addr     = 0x8000,   // from 32k bytes
		.partition_length         = (SPI_FLASH_CAPACITY/2) - 0x8000 - 4 * 0x1000,
		.partition_options        = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
	},
	[HAL_PARTITION_OTA_TEMP] =
	{
		.partition_owner          = HAL_FLASH_EMBEDDED,
		.partition_description    = "OTA Storage",
		.partition_start_addr     = OTA_2ND_SEQ_START + 0x8000,
		.partition_length         = (SPI_FLASH_CAPACITY/2) - 0x8000 - 4 * 0x1000,
		.partition_options        = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
	},
	[HAL_PARTITION_PARAMETER_3] =
	{
		.partition_owner          = HAL_FLASH_EMBEDDED,
		.partition_description    = "PARAMETER3",
		.partition_start_addr     = SPI_FLASH_CAPACITY - 2 * 0x1000,
		.partition_length         = 0x1000, //4k bytes
		.partition_options        = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
	},
	[HAL_PARTITION_PARAMETER_4] =
	{
		.partition_owner          = HAL_FLASH_EMBEDDED,
		.partition_description    = "PARAMETER4",
		.partition_start_addr     = SPI_FLASH_CAPACITY - 1 * 0x1000,
		.partition_length         = 0x1000, //4k bytes
		.partition_options        = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
	},
};

#define BOARD_AKEY   (1) //adc key ch 1
#define BOARD_DKEY0  (6) //dkey 0
#define BOARD_DKEY1  (7) //dkey 1

struct board_dkey {
	gpio_dev_t gpio;
	uint64_t   time;
};
static struct board_dkey board_dkeys[2];

static void board_dkey_irq_callback(void* arg)
{
	struct board_dkey *key = arg;
	uint32_t level;
	uint64_t diff;
	uint16_t key_code;

	hal_gpio_input_get(key, &level);
	//printf("key[%d] event: %d\n", key->gpio.port, level);

	if (level == 0 && key->time == 0) {
		key->time = aos_now_ms();
	} else {
		diff = aos_now_ms() - key->time;
		if (key->gpio.port == BOARD_DKEY0)
			key_code = CODE_BOOT;
		if (key->gpio.port == BOARD_DKEY1)
			key_code = CODE_VOLUME;

		if (diff > 6000) { /*long long press */
			key->time = 0;
			aos_post_event(EV_KEY, key_code, VALUE_KEY_LLTCLICK);
		} else if (diff > 2000) { /* long press */
			key->time = 0;
			aos_post_event(EV_KEY, key_code, VALUE_KEY_LTCLICK);
		} else if (diff > 40) { /* short press */
			key->time = 0;
			aos_post_event(EV_KEY, key_code, VALUE_KEY_CLICK);
		}
	}
}

void dkey_init(void)
{
	printf("gpio key init...\n");
	memset(&board_dkeys[0], 0, sizeof(struct board_dkey));
	board_dkeys[0].gpio.port = BOARD_DKEY0;
	hal_gpio_enable_irq(&board_dkeys[0].gpio, IRQ_TRIGGER_BOTH_EDGES, board_dkey_irq_callback, &board_dkeys[0]);

	memset(&board_dkeys[1], 0, sizeof(struct board_dkey));
	board_dkeys[1].gpio.port = BOARD_DKEY1;
	hal_gpio_enable_irq(&board_dkeys[1].gpio, IRQ_TRIGGER_BOTH_EDGES, board_dkey_irq_callback, &board_dkeys[1]);
}

struct board_akey {
	adc_dev_t adc;
	uint64_t   time;
	uint32_t   sample_count;
	uint32_t   avg_val;
};
static struct board_akey board_adc_key;
#define AKEY12_DOWN_TOGETHER_VOL    (300)
#define AKEY1_DOWN_VOL    (1000)  // 0.5V
#define AKEY2_DOWN_VOL    (2000) // 1.05V
#define AKEY3_DOWN_VOL    (3000) // 1.65V
#define AKEY_UP_VOL       (3800)

static void board_akey_irq_callback(void* arg)
{
	struct board_akey *akey = arg;
	adc_priv_config_t *adc_priv_cfg = (adc_priv_config_t *)akey->adc.priv;
	uint32_t level;
	uint64_t diff;
	uint16_t key_code;

	hal_adc_value_get(&akey->adc, &level, 0xffffffff);

	if (level < AKEY3_DOWN_VOL) {
		if (akey->sample_count == 0 && akey->time == 0) {
			akey->avg_val = 0;
			akey->time = aos_now_ms();
		}
		akey->avg_val += level;
		akey->sample_count++;
		if (akey->sample_count == 10) {
			HAL_ADC_ConfigChannel(akey->adc.port, ADC_SELECT_ENABLE,
						ADC_IRQMODE_HIGH, adc_priv_cfg->irqlow, adc_priv_cfg->irqhigh);
		}
	} else if (level > AKEY_UP_VOL) {
		HAL_ADC_ConfigChannel(akey->adc.port, ADC_SELECT_ENABLE,
						ADC_IRQMODE_LOW, adc_priv_cfg->irqlow, adc_priv_cfg->irqhigh);
		diff = aos_now_ms() - akey->time;
		// akey up, calc the average voltage
		akey->avg_val = akey->avg_val / akey->sample_count;
		printf("avg %d, count %d\n", akey->avg_val, akey->sample_count);
		if (akey->avg_val < AKEY12_DOWN_TOGETHER_VOL) {
			key_code = CODE_ELINK;
		} else if (akey->avg_val < AKEY1_DOWN_VOL) {
			key_code = CODE_NEXT;
		} else if (akey->avg_val < AKEY2_DOWN_VOL) {
			key_code = CODE_RECORD_PRE;
		} else if (akey->avg_val < AKEY3_DOWN_VOL) {
			key_code = CODE_PLAY_PAUSE;
		}

		if (diff > 6000) { /*long long press */
			akey->time = 0;
			akey->sample_count = 0;
			aos_post_event(EV_KEY, key_code, VALUE_KEY_LLTCLICK);
		} else if (diff > 2000) { /* long press */
			akey->time = 0;
			akey->sample_count = 0;
			aos_post_event(EV_KEY, key_code, VALUE_KEY_LTCLICK);
		} else if (diff > 40) { /* short press */
			akey->time = 0;
			akey->sample_count = 0;
			aos_post_event(EV_KEY, key_code, VALUE_KEY_CLICK);
		}
	}
}

int adc_key_init(void)
{
	HAL_Status status = HAL_ERROR;
	ADC_InitParam initParam;
	uint32_t akey_low_vol = 1650 + 100;  // 1650mV
	uint32_t akey_high_vol = 3000; // 3000mV
	static adc_priv_config_t adc_priv_cfg = {0};

	initParam.delay = 10;
	initParam.freq = 500000; // default 500K clock for adc controller
	status = HAL_ADC_Init(&initParam);
	if (status != HAL_OK) {
		printf("[adc_init]: adc init error\n");
		return -1;
	}

	memset(&board_adc_key, 0, sizeof(struct board_akey));
	board_adc_key.adc.port = BOARD_AKEY;
	board_adc_key.adc.config.sampling_cycle = 500000; // nothing
	board_adc_key.adc.priv = &adc_priv_cfg;
	adc_priv_cfg.irqmode = ADC_IRQMODE_LOW;
	adc_priv_cfg.irqlow = AKEY3_DOWN_VOL; // x/4096 == vol / 2.5
	adc_priv_cfg.irqhigh = AKEY_UP_VOL;
	adc_priv_cfg.irq_callback = board_akey_irq_callback;
	adc_priv_cfg.arg = &board_adc_key;
	hal_adc_init(&board_adc_key.adc);

	status = HAL_ADC_Start_Conv_IT();
	if (status != HAL_OK) {
		printf("[adc_init]: ADC it mode start error %d\n", status);
		return;
	}
	return 0;
}

void board_init(void)
{
	printf("xr871evb board init...\n");
	dkey_init();
	adc_key_init();
	return;
}
