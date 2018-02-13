#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <errno.h>
#include <hal/ota.h>
#include <aos/aos.h>
#include <hal/soc/soc.h>
#include <CheckSumUtils.h>
#include "sys/image.h"
#include "sys/ota.h"

#define OTA_DEBUG_ON	0
#define OTA_ERROR_ON	0

#if OTA_DEBUG_ON
#define OTA_DEBUG(...) do { \
                            printf("[ota debug]: "); \
                            printf(__VA_ARGS__); \
                        } while (0)
#else
#define OTA_DEBUG(...)
#endif

#if OTA_ERROR_ON
#define OTA_ERROR(...) do { \
                            printf("[ota error]: "); \
                            printf(__VA_ARGS__); \
                        } while (0)
#else
#define OTA_ERROR(...)
#endif

typedef struct
{
    uint32_t dst_adr;
    uint32_t src_adr;
    uint32_t siz;
    uint16_t crc;
} __attribute__((packed)) ota_hdl_t;

typedef struct
{
    uint32_t ota_len;
    uint32_t ota_crc;
} ota_reboot_info_t;

static ota_reboot_info_t ota_info;

static  CRC16_Context contex;
hal_partition_t ota_partition = HAL_PARTITION_OTA_TEMP;

static int xr871_ota_init(hal_ota_module_t *m, void *something)
{
    hal_logic_partition_t *partition_info;

	image_seq_t	seq;
	ota_cfg_t	cfg;
 
	OTA_DEBUG("xr871_ota_init enter\n");

	if (ota_read_cfg(&cfg) != OTA_STATUS_OK)
		OTA_ERROR("ota read cfg failed\n");

	if (((cfg.image == OTA_IMAGE_1ST) && (cfg.state == OTA_STATE_VERIFIED))
		|| ((cfg.image == OTA_IMAGE_2ND) && (cfg.state == OTA_STATE_UNVERIFIED))) {
		seq = IMAGE_SEQ_1ST;
	} else if (((cfg.image == OTA_IMAGE_2ND) && (cfg.state == OTA_STATE_VERIFIED))
			   || ((cfg.image == OTA_IMAGE_1ST) && (cfg.state == OTA_STATE_UNVERIFIED))) {
		seq = IMAGE_SEQ_2ND;
	} else {
		OTA_ERROR("invalid image %d, state %d\n", cfg.image, cfg.state);
		seq = IMAGE_SEQ_1ST;
	}

	OTA_DEBUG("image seq %d\n", seq);

	if (seq == IMAGE_SEQ_1ST) {
		ota_partition = HAL_PARTITION_OTA_TEMP;
    	partition_info = hal_flash_get_info( HAL_PARTITION_OTA_TEMP );
    	hal_flash_erase(HAL_PARTITION_OTA_TEMP, 0 ,partition_info->partition_length);
	}
	else if (seq == OTA_IMAGE_2ND) {
		ota_partition = HAL_PARTITION_APPLICATION;
    	partition_info = hal_flash_get_info( HAL_PARTITION_APPLICATION);
    	hal_flash_erase(HAL_PARTITION_APPLICATION, 0 ,partition_info->partition_length);
	}
	ota_info.ota_len = 0;

    //CRC16_Init( &contex );
	memset(&ota_info, 0 , sizeof ota_info);

	OTA_DEBUG("xr871_ota_init exit\n");
    return 0;
}


static int xr871_ota_write(hal_ota_module_t *m, volatile uint32_t* off_set, uint8_t* in_buf ,uint32_t in_buf_len)
{
	int ret = 0;
	unsigned int _off_set = 0;

	OTA_DEBUG("xr871_ota_write enter\n");

	if (ota_info.ota_len + in_buf_len < 0x8000) {
		//do nothing
	}
	else if(ota_info.ota_len >= 0x8000)
	{
		_off_set = ota_info.ota_len -0x8000;
	    ret = hal_flash_write(ota_partition, &_off_set, in_buf, in_buf_len);
	}
	else {
		int rem;

		rem = ota_info.ota_len + in_buf_len - 0x8000;
		_off_set = 0;

		ret = hal_flash_write(ota_partition, &_off_set, &in_buf[in_buf_len-rem], rem);
	}

	ota_info.ota_len += in_buf_len;

    //CRC16_Update( &contex, in_buf, in_buf_len);

    OTA_DEBUG(" &_off_set 0x %08x, %d\n", _off_set ,ota_info.ota_len);

	OTA_DEBUG("xr871_ota_write exit\n");

    return ret;
}

static int xr871_ota_read(hal_ota_module_t *m,  volatile uint32_t* off_set, uint8_t* out_buf, uint32_t out_buf_len)
{
	uint32_t temp;

	OTA_DEBUG("xr871_ota_read enter\n");

	temp = *off_set;

	if (ota_partition == HAL_PARTITION_OTA_TEMP) {
		temp += 0x8000;
	}

    hal_flash_read(ota_partition, &temp, out_buf, out_buf_len);

	*off_set += out_buf_len;
	OTA_DEBUG("xr871_ota_read exit\n");

    return 0;
}

static int xr871_ota_set_boot(hal_ota_module_t *m, void *something)
{
	ota_cfg_t ota_cfg;
    uint8_t parti = *(uint8_t *)something;
    //CRC16_Final( &contex, &ota_info.ota_crc );
    OTA_DEBUG("xr871 set boot\n");

	if (ota_partition == HAL_PARTITION_OTA_TEMP) {
		ota_cfg.image = 2;
	}
	else {
		ota_cfg.image = 1;
	}

	ota_cfg.state = OTA_STATE_UNVERIFIED;
	ota_write_cfg(&ota_cfg);
	ota_set_size(ota_info.ota_len);
	if (ota_verify_image(OTA_VERIFY_NONE, NULL)  != OTA_STATUS_OK) {
		OTA_DEBUG("xr871 set boot fail\n");
	}

    /* reboot */
    hal_reboot();

    return 0;
}

struct hal_ota_module_s xr871_ota_module = {
    .init = xr871_ota_init,
    .ota_write = xr871_ota_write,
    .ota_read = xr871_ota_read,
    .ota_set_boot = xr871_ota_set_boot,
};
