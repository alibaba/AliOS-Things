#include <k_api.h>
#include <string.h>
#include <CheckSumUtils.h>

#include "aos/hal/flash.h"

#define KV_HAL_OTA_CRC16                "hal_ota_crc16"
#define OTA_FLAG_MAGIC                  0xA5A5A5A5
#define OTA_FLAG_PART                   HAL_PARTITION_PARAMETER_2

typedef struct
{
	uint32_t ota_len;
	uint32_t ota_crc;
} ota_reboot_info_t;

typedef enum {
	UPG_NORMAL = 0,
	UPG_SWITCH_APP,
	UPG_CHECK_APP,
	UPG_BOOT_FAILED,
} Upgrade_flag_enum;

#pragma pack(1)
typedef struct {
	uint32_t magic;
	uint32_t upgrade_flag;
	ota_reboot_info_t m_ota_info;
} Ota_flag;
#pragma pack()

#if defined( USE_OTA_SPIM_FLASH)
void spim_flash_init (void);
int spim_flash_erase_write (volatile uint32_t* off_set, uint8_t* in_buf ,uint32_t in_buf_len);
int spim_flash_read (volatile uint32_t* off_set, uint8_t* out_buf, uint32_t out_buf_len);
#endif

static ota_reboot_info_t ota_info;
static CRC16_Context contex;
static unsigned int _off_set = 0;

static uint32_t g_now_bank = 0;

void platform_flash_lock(void);
void platform_flash_unlock(void);

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

static int dump_ota_flag(Ota_flag *data)
{
	if (data == NULL) {
		LOG("get_ota_flag parameters is invalid.\n");
		return -1;
	}
	printf("[%s] %08X %08X %08X %08X.\r\n", __func__, \
											data->magic, \
											data->upgrade_flag, \
											data->m_ota_info.ota_crc, \
											data->m_ota_info.ota_len );
}

static int get_ota_flag(Ota_flag *data)
{
	uint32_t offset = 0;
	int len = sizeof(Ota_flag);

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

	flag.magic 	= OTA_FLAG_MAGIC;
	flag.upgrade_flag = UPG_NORMAL;
	memcpy( &flag.m_ota_info, &ota_info, sizeof(ota_reboot_info_t) );
	if (set_ota_flag(&flag) != 0) {
		LOG("init_ota_flag set_ota_flag return failed.\n");
	}
}

/**
 * To initialize flash hardwares and related variables
 *
 * @param[in]  m  interface pointer. There's no need to use in functions.
 *
 * @param[in]  something  current breakpoint address, use : _off_set = (uint32_t)something。
 * 
 * If it is 0, you need to erase the flash partition in FOTA and prepare for a new download. 
 * For the sake of safety, we suggest you to execute CRC16 Check, and initiate CRC16 at the same time.
 * If it is not 0, which means the last download is interrupted,you don't need to erase the flash partition.
 * If CRC16 check was used,  the backup CRC16 check at last breakpoint should be read from flash.
 * 
 * @return  0 : on success, EIO : if an error occurred with any step
 */
static int numicro_ota_init(hal_ota_module_t *m, void *something)
{
	LOG("ota module init\n");
	_off_set = *(uint32_t*)something;
	ota_info.ota_len=_off_set;

	if(_off_set==0) {
		CRC16_Init( &contex );
	} else {
		contex.crc=hal_ota_get_crc16();
		LOG("--------get crc16 context.crc=%d!--------\n",contex.crc);
	}

#if defined( USE_OTA_SPIM_FLASH)
    spim_flash_init ();
#endif

	return 0;
}

/**
 * The interface is used to save firmwares when downloading FOTA
 *
 * @param[in]  m	interface pointer. There's no need to use in functions
 *
 * @param[in]  off_set	  Write offset address. Since the upper level is currently set 0, there's no need for use.
 * 
 * @param[in]  in_buf	  The data you need to write this time.
 * 
 * @param[in]  in_buf_len	The length of data you need to write this time.
 * 
 * Since off_set is always 0 when it is called, the write offset address should be implemented by functions. 
 * We suggest you to use the offset address obtained in function init, and add your write length after each write. 
 * This function needs to call flash write function to write data.
 * 
 * @return  0 : on success, EIO : if an error occurred with any step
 */
static int numicro_ota_write(hal_ota_module_t *m, volatile uint32_t* off_set, uint8_t* in_buf ,uint32_t in_buf_len)
{
	hal_partition_t pno = HAL_PARTITION_OTA_TEMP;
	int ret;

	if (ota_info.ota_len == 0) {
		_off_set = 0;
		CRC16_Init( &contex );
		memset(&ota_info, 0 , sizeof ota_info);
	}
	CRC16_Update( &contex, in_buf, in_buf_len);

#if defined( USE_OTA_SPIM_FLASH)
    ret = spim_flash_erase_write (&_off_set, in_buf, in_buf_len);
#else
    ret = hal_flash_write(pno, &_off_set, in_buf, in_buf_len);
#endif

	ota_info.ota_len += in_buf_len;

	return ret;
}

/**
 * This interface hasn't been used in FOTA currently, so you don't necessarily need to implement it. 
 *
 * @param[in]  m	interface pointer. There's no need to use in functions
 *
 * @param[in]  off_set	  Write offset address. Since the upper level is currently set 0, there's no need for use.
 * 
 * @param[in]  in_buf	  The data you need to write this time.
 * 
 * @param[in]  in_buf_len	The length of data you need to write this time.
 * 
 * You can encapsulate flash read function with this function.
 * 
 * @return  0 : on success, EIO : if an error occurred with any step
 */
static int numicro_ota_read(hal_ota_module_t *m,  volatile uint32_t* off_set, uint8_t* out_buf, uint32_t out_buf_len)
{
#if defined( USE_OTA_SPIM_FLASH)
    spim_flash_read ((uint32_t*)off_set, out_buf, out_buf_len);
#else
    hal_partition_t pno = HAL_PARTITION_OTA_TEMP;
    hal_flash_read(pno, (uint32_t*)off_set, out_buf, out_buf_len);
#endif

	return 0;
}

/**
 * This interface is used to update the system after download, or save the scene if download is interrupted.
 *
 * @param[in]  m  interface pointer. There's no need to use in functions.
 *
 * @param[in]  something  the pass-in parameter. The actual pass-in type is : ota_finish_param_t *。
 * 
 * If it is 0, you need to erase the flash partition in FOTA and prepare for a new download. 
 * For the sake of safety, we suggest you to execute CRC16 Check, and initiate CRC16 at the same time.
 * If it is not 0, which means the last download is interrupted,you don't need to erase the flash partition.
 * If CRC16 check was used,  the backup CRC16 check at last breakpoint should be read from flash.
 * 
 * @return  0 : on success, EIO : if an error occurred with any step
 */

static int numicro_ota_set_boot(hal_ota_module_t *m, void *something)
{
	Ota_flag flag;
	ota_finish_param_t *param = (ota_finish_param_t *)something;
	int ret = 0;

	if (param==NULL)
		return -1;

	if (param->result_type==OTA_FINISH)
	{
		printf("?????????????????\r\n");
		CRC16_Final( &contex, (uint16_t *)&ota_info.ota_crc );

		flag.magic = OTA_FLAG_MAGIC;
		flag.upgrade_flag = UPG_SWITCH_APP;
		memcpy( &flag.m_ota_info, &ota_info, sizeof(ota_reboot_info_t) );
		ret = set_ota_flag(&flag);
		
		aos_reboot();

		memset(&ota_info, 0 , sizeof ota_info);
	} else if (param->result_type==OTA_BREAKPOINT) {
		LOG("OTA package is incomplete!\n");
		hal_ota_save_crc16(contex.crc);
	}

	return 0;
}

int test_numicro_ota_set_boot(void)
{
	ota_finish_param_t something;
	Ota_flag data;
	memset(&data, 0, sizeof(Ota_flag) );
	get_ota_flag(&data);
	dump_ota_flag(&data);
	something.result_type=OTA_FINISH;
	numicro_ota_set_boot(NULL, (void*)&something);
}

static int numicro_ota_rollback(hal_ota_module_t *m)
{
	Ota_flag flag;
	int ret = 0;

	ret = get_ota_flag(&flag);
	if (ret != 0) {
		LOG("numicro_ota_rollback get_ota_flag return failed.\n");
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

			LOG("magic: 0x%x, upgrade_flag: 0x%x.\n", flag.magic, flag.upgrade_flag);

			ret = -1;
			break;
		}
	} while (0);

	init_ota_flag();

	return ret;
}

struct hal_ota_module_s numicro_ota_module = {
	.init = numicro_ota_init,
	.ota_write = numicro_ota_write,
	.ota_read = numicro_ota_read,
	.ota_set_boot = numicro_ota_set_boot,
	.ota_rollback = numicro_ota_rollback,
};
