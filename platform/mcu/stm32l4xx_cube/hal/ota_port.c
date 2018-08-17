/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <k_api.h>
#include <string.h>
#include "stm32l4xx_hal.h"
#include "CheckSumUtils.h"
#include "hal/hal.h"

#define KV_HAL_OTA_CRC16                "hal_ota_get_crc16"
#define OTA_FLAG_MAGIC                  0x20180712
#define OTA_FLAG_PART                   HAL_PARTITION_PARAMETER_3

typedef struct
{
	uint32_t ota_len;
	uint32_t ota_crc;
} ota_reboot_info_t;

typedef enum {
	UPG_NORMAL = 0,
#ifdef SYSCFG_MEMRMP_FB_MODE
	UPG_SWITCH_BANK,
#else
	UPG_SWITCH_APP,
#endif
	UPG_CHECK_APP,
	UPG_BOOT_FAILED,
} Upgrade_flag_enum;

#pragma pack(1)
typedef struct {
	uint32_t magic;
	uint32_t upgrade_flag;
#ifdef SYSCFG_MEMRMP_FB_MODE
	uint32_t switch_bank;
#endif
} Ota_flag;
#pragma pack()

static ota_reboot_info_t ota_info;
static CRC16_Context contex;
static unsigned int _off_set = 0;

static uint32_t g_now_bank = 0;

static uint16_t hal_ota_get_crc16(void);
static void  hal_ota_save_crc16(uint16_t crc16);
static int stm32l4xx_ota_write(hal_ota_module_t *m, volatile uint32_t* off_set, uint8_t* in_buf ,uint32_t in_buf_len);
static int stm32l4xx_ota_read(hal_ota_module_t *m,  volatile uint32_t* off_set, uint8_t* out_buf, uint32_t out_buf_len);
static int stm32l4xx_ota_set_boot(hal_ota_module_t *m, void *something);

#ifdef SYSCFG_MEMRMP_FB_MODE
static void init_boot_bank(void)
{
	uint32_t memrmp = 0;

	memrmp = READ_BIT(SYSCFG->MEMRMP, SYSCFG_MEMRMP_FB_MODE);
	g_now_bank = memrmp ? FLASH_BANK_2 : FLASH_BANK_1;
}
#endif

static int get_ota_flag(Ota_flag *data)
{
	uint32_t offset = 0;

	if (data == NULL) {
		LOG("get_ota_flag parameters is invalid.\n");
		return -1;
	}

	return hal_flash_read(OTA_FLAG_PART, &offset, data, sizeof(Ota_flag));
}

static int set_ota_flag(Ota_flag *data)
{
	uint32_t offset = 0;

	if (data == NULL) {
		LOG("set_ota_flag parameters is invalid.\n");
		return -1;
	}

	return hal_flash_write(OTA_FLAG_PART, &offset, data, sizeof(Ota_flag));
}

static void init_ota_flag(void)
{
	Ota_flag flag;

	flag.magic = OTA_FLAG_MAGIC;
	flag.upgrade_flag = UPG_NORMAL;
#ifdef SYSCFG_MEMRMP_FB_MODE
	flag.switch_bank = 0;
#endif
	if (set_ota_flag(&flag) != 0) {
		LOG("init_ota_flag set_ota_flag return failed.\n");
	}
}

#ifdef SYSCFG_MEMRMP_FB_MODE
static int flash_sw_bank(void)
{
	FLASH_OBProgramInitTypeDef OBInit;
	int ret = 0;

	HAL_FLASH_Unlock();

	/* Clear OPTVERR bit set on virgin samples */
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR); 

	/* Allow Access to option bytes sector */ 
	HAL_FLASH_OB_Unlock();

	/* Get the Dual boot configuration status */
	HAL_FLASHEx_OBGetConfig(&OBInit);

	/* Enable/Disable dual boot feature */
	OBInit.OptionType = OPTIONBYTE_USER;
	OBInit.USERType   = OB_USER_BFB2;

	do {
		if (((OBInit.USERConfig) & (OB_BFB2_ENABLE)) == OB_BFB2_ENABLE)
		{
			if (g_now_bank == FLASH_BANK_1) {
				LOG("OBConfig conflicts with boot bank. Now running on bank1.\n");
				ret = -1;
				break;
			}
			OBInit.USERConfig = OB_BFB2_DISABLE;
		}
		else
		{
			if (g_now_bank == FLASH_BANK_2) {
				LOG("OBConfig conflicts with boot bank. Now running on bank2.\n");
				ret = -1;
				break;
			}
			OBInit.USERConfig = OB_BFB2_ENABLE;
		}

		if(HAL_FLASHEx_OBProgram (&OBInit) != HAL_OK) {
			/*
			Error occurred while setting option bytes configuration.
			User can add here some code to deal with this error.
			To know the code error, user can call function 'HAL_FLASH_GetError()'
			*/
			LOG("HAL_FLASHEx_OBProgram failed\n");
			ret = -1;
			break;
		}

		/* Start the Option Bytes programming process */  
		if (HAL_FLASH_OB_Launch() != HAL_OK) {
			/*
			Error occurred while reloading option bytes configuration.
			User can add here some code to deal with this error.
			To know the code error, user can call function 'HAL_FLASH_GetError()'
			*/
			LOG("HAL_FLASH_OB_Launch failed\n");
			ret = -1;
			break;
		}
	} while (0);

	HAL_FLASH_OB_Lock();
	HAL_FLASH_Lock();

	return ret;
}

static int reboot_sw_bank(void)
{
	Ota_flag flag;
	int ret = 0;

	flag.magic = OTA_FLAG_MAGIC;
	flag.upgrade_flag = UPG_SWITCH_BANK;
	flag.switch_bank = g_now_bank == FLASH_BANK_1 ? FLASH_BANK_2 : FLASH_BANK_1;
	ret = set_ota_flag(&flag);
	if (ret != 0) {
		LOG("stm32l4xx_ota_set_boot set_ota_flag return failed.\n");
		return -1;
	}
	LOG("Switch boot bank.\n");
	ret = flash_sw_bank();
	if (ret != 0) {
		init_ota_flag();
	}
	LOG("reboot_sw_bank unknow error.\n");

	return -1;
}
#endif

static int stm32l4xx_ota_init(hal_ota_module_t *m, void *something)
{
	LOG("ota module init\n");
	_off_set = *(uint32_t*)something;
	ota_info.ota_len=_off_set;

#ifdef SYSCFG_MEMRMP_FB_MODE
	init_boot_bank();
#endif

	if(_off_set==0) {
		CRC16_Init( &contex );
	} else {
		contex.crc=hal_ota_get_crc16();
		LOG("--------get crc16 context.crc=%d!--------\n",contex.crc);
	}

	return 0;
}

static int stm32l4xx_ota_write(hal_ota_module_t *m, volatile uint32_t* off_set, uint8_t* in_buf ,uint32_t in_buf_len)
{
	hal_partition_t pno = HAL_PARTITION_OTA_TEMP;
	int ret;

	if (ota_info.ota_len == 0) {
		_off_set = 0;
		CRC16_Init( &contex );
		memset(&ota_info, 0 , sizeof ota_info);
	}
	CRC16_Update( &contex, in_buf, in_buf_len);

#ifdef SYSCFG_MEMRMP_FB_MODE
	pno = g_now_bank == FLASH_BANK_1 ? HAL_PARTITION_OTA_TEMP : HAL_PARTITION_APPLICATION;
#endif
	ret = hal_flash_write(pno, &_off_set, in_buf, in_buf_len);
	ota_info.ota_len += in_buf_len;

	return ret;
}

static int stm32l4xx_ota_read(hal_ota_module_t *m,  volatile uint32_t* off_set, uint8_t* out_buf, uint32_t out_buf_len)
{
	hal_partition_t pno = HAL_PARTITION_OTA_TEMP;

#ifdef SYSCFG_MEMRMP_FB_MODE
	pno = g_now_bank == FLASH_BANK_1 ? HAL_PARTITION_OTA_TEMP : HAL_PARTITION_APPLICATION;
#endif
	hal_flash_read(pno, (uint32_t*)off_set, out_buf, out_buf_len);

	return 0;
}

static int stm32l4xx_ota_set_boot(hal_ota_module_t *m, void *something)
{
#ifndef SYSCFG_MEMRMP_FB_MODE
	Ota_flag flag;
#endif
	ota_finish_param_t *param = (ota_finish_param_t *)something;
	int ret = 0;

	if (param==NULL){
		return -1;
	}
	if (param->result_type==OTA_FINISH)
	{
		CRC16_Final( &contex, (uint16_t *)&ota_info.ota_crc );
#ifdef SYSCFG_MEMRMP_FB_MODE
		reboot_sw_bank();
#else
		flag.magic = OTA_FLAG_MAGIC;
		flag.upgrade_flag = UPG_SWITCH_APP;
		ret = set_ota_flag(&flag);
		aos_reboot();
#endif
		memset(&ota_info, 0 , sizeof ota_info);
	} else if (param->result_type==OTA_BREAKPOINT) {
		LOG("OTA package is incomplete!\n");
		hal_ota_save_crc16(contex.crc);
	}

	return 0;
}

static int stm32l4xx_ota_rollback(hal_ota_module_t *m)
{
	Ota_flag flag;
	int ret = 0;

	ret = get_ota_flag(&flag);
	if (ret != 0) {
		LOG("stm32l4xx_ota_rollback get_ota_flag return failed.\n");
		return -1;
	}

	do {
		if (flag.magic != OTA_FLAG_MAGIC) {
			LOG("First init OTA flag data.\n");
			break;
		}
		if (flag.upgrade_flag == UPG_NORMAL) {
			return 0;
		} else if (flag.upgrade_flag == UPG_CHECK_APP) {
			LOG("OTA finished.\n");
			break;
		} else if (flag.upgrade_flag == UPG_BOOT_FAILED) {
			LOG("OTA failed. Rollback finished\n");
			ret = -1;
			break;
		} else {
			LOG("OTA unknow state.\n");
#ifdef SYSCFG_MEMRMP_FB_MODE
			LOG("magic: 0x%x, upgrade_flag: 0x%x, switch_bank: 0x%x.\n", flag.magic, flag.upgrade_flag, flag.switch_bank);
#else
			LOG("magic: 0x%x, upgrade_flag: 0x%x.\n", flag.magic, flag.upgrade_flag);
#endif
			ret = -1;
			break;
		}
	} while (0);

	init_ota_flag();

	return ret;
}

static uint16_t hal_ota_get_crc16(void)
{
	int len = 2;
	uint16_t crc16=0;
	aos_kv_get(KV_HAL_OTA_CRC16, &crc16, &len);
	return crc16;
}

static void  hal_ota_save_crc16(uint16_t crc16)
{
	aos_kv_set(KV_HAL_OTA_CRC16, &crc16, 2, 1);
}

struct hal_ota_module_s stm32l4xx_ota_module = {
	.init = stm32l4xx_ota_init,
	.ota_write = stm32l4xx_ota_write,
	.ota_read = stm32l4xx_ota_read,
	.ota_set_boot = stm32l4xx_ota_set_boot,
	.ota_rollback = stm32l4xx_ota_rollback,
};
