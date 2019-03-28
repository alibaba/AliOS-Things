#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <errno.h>
#include <hal/ota.h>
#include <aos/aos.h>
#include <hal/soc/soc.h>
#include <CheckSumUtils.h>

#define OTA_TWO_IMAGE_SWITCH_RUN 0
#define KV_HAL_OTA_CRC16  "hal_ota_get_crc16"

typedef struct
{
    uint32_t ota_len;
    uint32_t ota_crc;
} ota_reboot_info_t;

static ota_reboot_info_t ota_info;
static CRC16_Context contex;
static uint32_t _off_set = 0;

static uint16_t hal_ota_get_crc16(void);
static void hal_ota_save_crc16(uint16_t crc16);

#if OTA_TWO_IMAGE_SWITCH_RUN
#define OTA_IMAGE_A    0
#define OTA_IMAGE_B    1
#define KV_HAL_OTA_HDR  "hal_ota_get_hdr"
typedef struct
{
	uint32_t version;
	uint32_t hdr_num;
	struct
	{
		uint32_t img_ID;
		uint32_t hdr_len;
		uint32_t checksum;
		uint32_t img_len;
		uint32_t img_offset;
		uint32_t flash_offset;
	}ota_file[2];
}ota_hdr_t;

static ota_hdr_t ota_hdr_info;
static uint8_t ota_index;
static uint8_t ota_hdr_check;

static int hal_get_ota_hdr(void)
{
    int len, ret;

	len = sizeof(ota_hdr_t);
    ret = aos_kv_get(KV_HAL_OTA_HDR, &ota_hdr_info, &len);

	return ret;
}

static int hal_ota_save_hdr(void)
{
    int len, ret;

	len = sizeof(ota_hdr_t);
	ret = aos_kv_set(KV_HAL_OTA_HDR, &ota_hdr_info, len, 1);

	return ret;
}

static void hal_get_ota_index(void)
{
	uint32_t offset, boot_addr;
	hal_logic_partition_t *partition_info;

	offset = 0x00;
	hal_flash_read(HAL_PARTITION_PARAMETER_1, &offset, (void *)&boot_addr, sizeof(boot_addr));

	partition_info = hal_flash_get_info(HAL_PARTITION_OTA_TEMP);

	if(boot_addr == partition_info->partition_start_addr)
	{
		ota_index = OTA_IMAGE_A; //update to image A
	}
	else
	{
		ota_index = OTA_IMAGE_B; //update to image B
	}

	return;
}

static int hal_verify_ota_head(void)
{
	char imgid[5];
	
	if(~ota_hdr_info.version)
	{
		LOG("check ota version error %x.\r\n", ota_hdr_info.version);
		return 1;
	}

	if(ota_hdr_info.hdr_num != 2)
	{
		LOG("check ota number error %d.\r\n", ota_hdr_info.hdr_num);
		return 1;
	}

	memset(imgid, 0, sizeof(imgid));
	memcpy(imgid, (char *)&ota_hdr_info.ota_file[0].img_ID, 4);
	if(strcmp(imgid, "OTA1"))
	{
		LOG("OTA1 image id error.\r\n");
		return 1;
	}
	memcpy(imgid, (char *)&ota_hdr_info.ota_file[1].img_ID, 4);
	if(strcmp(imgid, "OTA2"))
	{
		LOG("OTA2 image id error.\r\n");
		return 1;
	}

	if(ota_hdr_info.ota_file[0].checksum == 0)
	{
		LOG("OTA1 checksum is zaro.\r\n");
		return 1;
	}
	if(ota_hdr_info.ota_file[1].checksum == 0)
	{
		LOG("OTA2 checksum is zaro.\r\n");
		return 1;
	}

	return 0;
}

static int hal_verify_ota_checksum(uint8_t crc_partition_idx, uint32_t crc_len)
{
	uint8_t *f_data;
	CRC16_Context ctx;
	uint16_t ota_crc;
	uint32_t i = 0, j = 0, len = 0, checksum = 0;

	CRC16_Final(&contex, &ota_info.ota_crc);//download crc
	if(ota_info.ota_crc == 0)
	{
		LOG("crc is zaro.\r\n");
		return -1;
	}
	
	f_data = (uint8_t *)aos_malloc(0x1000);
	if(f_data == NULL)
	{
		LOG("hal_ota_crc16_compute malloc fail.\r\n");
		return -1;
	}
	
	_off_set = 0;
	CRC16_Init(&ctx);
	
	while(i < crc_len)
	{
		if((0x1000 + i) < crc_len)
		{
			len = 0x1000;
		}
		else
		{
			len = crc_len - i;
		}
		
		hal_flash_read(crc_partition_idx, &_off_set, f_data, len);
		
		/*update checksum*/
		for(j = 0; j < len; j++)
		{
			checksum += f_data[j];
		}
		
		/*update crc16*/
		CRC16_Update(&ctx, f_data, len);
		
		i += len;
	}
	
	CRC16_Final(&ctx, &ota_crc);

	if(f_data)
	{
		aos_free(f_data);
	}

	/*verify crc*/
	if(ota_info.ota_crc != ota_crc)
	{
		LOG("crc check error: download_crc=0x%x, comput_crc=0x%x.\r\n", ota_info.ota_crc, ota_crc);
		return -1;
	}

	/*verify checksum*/
	if(ota_hdr_info.ota_file[ota_index].checksum != checksum)
	{
		LOG("checksum error: gen_chcksum=0x%x, comput_checksum=0x%x.\r\n", ota_hdr_info.ota_file[ota_index].checksum, checksum);
		return -1;
	}

	return 0;
}

static int bk7231_ota_init(hal_ota_module_t *m, void *something)
{
	uint32_t offset, delta;
    hal_logic_partition_t *partition_info;
	
    LOG("!!!!!!ota start!!!!!!\n");
	hal_flash_dis_secure(0, 0, 0);//disable flash protect
	hal_get_ota_index();
    offset = *(uint32_t*)something;
	if(offset == 0)
	{
		if(ota_index == OTA_IMAGE_A)
		{
        	partition_info = hal_flash_get_info(HAL_PARTITION_APPLICATION);
			hal_flash_erase(HAL_PARTITION_APPLICATION, 0, partition_info->partition_length);
		}
		else
		{
			partition_info = hal_flash_get_info(HAL_PARTITION_OTA_TEMP);
			hal_flash_erase(HAL_PARTITION_OTA_TEMP, 0, partition_info->partition_length);
		}
		ota_hdr_check = 0;
		memset(&ota_info, 0, sizeof(ota_info));
		memset(&ota_hdr_info, 0, sizeof(ota_hdr_info));
		CRC16_Init(&contex);
	}
	else
	{
		ota_info.ota_len = offset;
		ota_info.ota_crc = 0;
		if(hal_get_ota_hdr())
		{
			LOG("get ota header fail.\r\n");
			return -1;
		}
		if(offset < sizeof(ota_hdr_t))
		{
			ota_hdr_check = 0;
		}
		else
		{
			ota_hdr_check = hal_verify_ota_head();
			if(ota_hdr_check == 0)
			{
				if(offset < ota_hdr_info.ota_file[1].img_offset)
				{
					delta = offset - sizeof(ota_hdr_t);
					if(ota_index == OTA_IMAGE_A)
					{
						_off_set = delta;
					}
				}
				else
				{
					delta = offset - ota_hdr_info.ota_file[1].img_offset;
					if(ota_index == OTA_IMAGE_B)
					{
						_off_set = delta;
					}
				}
			}
			else
			{
				LOG("check ota header error.\r\n");
				return -1;
			}
		}
		contex.crc = hal_ota_get_crc16();
	}
	
	return 0;
}

static int bk7231_ota_read(hal_ota_module_t *m,  volatile uint32_t* off_set, uint8_t* out_buf, uint32_t out_buf_len)
{
	if(ota_index == OTA_IMAGE_A)
	{
    	hal_flash_read(HAL_PARTITION_APPLICATION, off_set, out_buf, out_buf_len);
	}
	else
	{
    	hal_flash_read(HAL_PARTITION_OTA_TEMP, off_set, out_buf, out_buf_len);
	}
	return 0;
}

static int bk7231_ota_write(hal_ota_module_t *m, volatile uint32_t* off_set, uint8_t* in_buf ,uint32_t in_buf_len)
{
	uint32_t offset;

	if(ota_hdr_check)
	{
		LOG("check ota header fail.\r\n");
		return -1;
	}
	else
	{
		bk_printf(".");
	}

	if(ota_info.ota_len < sizeof(ota_hdr_t))
	{
		offset = sizeof(ota_hdr_t) - ota_info.ota_len;
		if(in_buf_len < offset)
		{
			memcpy((uint8_t *)&ota_hdr_info + ota_info.ota_len, in_buf, in_buf_len);
		}
		else
		{
			memcpy((uint8_t *)&ota_hdr_info + ota_info.ota_len, in_buf, offset);
			ota_hdr_check = hal_verify_ota_head();
			if(ota_hdr_check == 0 && ota_index == OTA_IMAGE_A)
			{
				CRC16_Update(&contex, in_buf + offset, in_buf_len - offset);
				hal_flash_write(HAL_PARTITION_APPLICATION, &_off_set, in_buf + offset, in_buf_len - offset);
			}
		}
	}
	else
	{
		if(ota_index == OTA_IMAGE_A)
		{
			if(ota_info.ota_len + in_buf_len < ota_hdr_info.ota_file[1].img_offset)
			{
				CRC16_Update(&contex, in_buf, in_buf_len);
				hal_flash_write(HAL_PARTITION_APPLICATION, &_off_set, in_buf, in_buf_len);
			}
			else
			{
				if(ota_info.ota_len < ota_hdr_info.ota_file[1].img_offset)
				{
					offset = ota_hdr_info.ota_file[1].img_offset - ota_info.ota_len;
					CRC16_Update(&contex, in_buf, offset);
					hal_flash_write(HAL_PARTITION_APPLICATION, &_off_set, in_buf, offset);
				}
			}
		}
		else
		{
			if(ota_info.ota_len > ota_hdr_info.ota_file[1].img_offset)
			{
				CRC16_Update(&contex, in_buf, in_buf_len);
				hal_flash_write(HAL_PARTITION_OTA_TEMP, &_off_set, in_buf, in_buf_len);
			}
			else
			{
				if(ota_info.ota_len + in_buf_len > ota_hdr_info.ota_file[1].img_offset)
				{
					offset = ota_hdr_info.ota_file[1].img_offset - ota_info.ota_len;
					CRC16_Update(&contex, in_buf + offset, in_buf_len - offset);
					hal_flash_write(HAL_PARTITION_OTA_TEMP, &_off_set, in_buf + offset, in_buf_len - offset);
				}
			}
		}
	}
	
	ota_info.ota_len += in_buf_len;
	
	return 0;
}

static int bk7231_ota_set_boot(hal_ota_module_t *m, void *something)
{
	int ret = 0;
    uint32_t offset, addr_rb, boot_addr;
	uint32_t crc_partition_idx, crc_len;
	hal_logic_partition_t *partition_info;
    ota_finish_param_t *param;

	param = (ota_finish_param_t *)something;
    if (param==NULL)
	{
		LOG("finish type is null.\r\n");
		return -1;
    }

	if(ota_hdr_check)
	{
		LOG("check ota head fail.\r\n");
		return -1;
	}

	if(param->result_type == OTA_FINISH)
	{
		if(ota_index == OTA_IMAGE_A)
		{
			crc_partition_idx = HAL_PARTITION_APPLICATION;
			crc_len = ota_hdr_info.ota_file[0].img_len;
			partition_info = hal_flash_get_info(HAL_PARTITION_APPLICATION);
		}
		else
		{
			crc_partition_idx = HAL_PARTITION_OTA_TEMP;
			crc_len = ota_hdr_info.ota_file[1].img_len;
			partition_info = hal_flash_get_info(HAL_PARTITION_OTA_TEMP);
		}
		boot_addr = partition_info->partition_start_addr;
		
		ret = hal_verify_ota_checksum(crc_partition_idx, crc_len);
		if(ret)
		{
			return -1;
		}
		
		offset = 0x00;
		hal_flash_erase(HAL_PARTITION_PARAMETER_1, offset, sizeof(boot_addr));
		
		offset = 0x00;
		hal_flash_write(HAL_PARTITION_PARAMETER_1, &offset, (const void *)&boot_addr, sizeof(boot_addr));
		
		offset = 0x00;
		hal_flash_read(HAL_PARTITION_PARAMETER_1, &offset, &addr_rb, sizeof(addr_rb));

		if(addr_rb != boot_addr)
		{
			LOG("write boot address fail.\r\n");
			return -1;
		}
	}
	else
	{
		ret = hal_ota_save_hdr();
		if(ret)
		{
			LOG("save ota header fail.\r\n");
			return -1;
		}
        hal_ota_save_crc16(contex.crc);
	}

	hal_reboot();
	return ret;
}
#else
typedef struct
{
    uint32_t dst_adr;
    uint32_t src_adr;
    uint32_t siz;
    uint16_t crc;
} __attribute__((packed)) ota_hdl_t;

extern char app_download_addr;
extern char kernel_download_addr;

int hal_ota_switch_to_new_fw(uint8_t parti, int ota_data_len, uint16_t ota_data_crc )
{
    uint32_t offset;
    ota_hdl_t ota_hdl,ota_hdl_rb;
    hal_logic_partition_t* ota_partition;

    ota_partition = hal_flash_get_info( HAL_PARTITION_OTA_TEMP );

   
    memset( &ota_hdl, 0, sizeof(ota_hdl_t) );
    ota_hdl.dst_adr = parti == OTA_PARTITION_KERNEL ? &kernel_download_addr :
                                   parti == OTA_PARTITION_APP ? &app_download_addr : &kernel_download_addr;
    
    if(ota_hdl.dst_adr < 0x11000 || ota_hdl.dst_adr > 0x100000) {
        LOG("download_addr 0x%08x invalid", ota_hdl.dst_adr);
        hal_reboot();
        return;
    }
    ota_hdl.src_adr = ota_partition->partition_start_addr;
    ota_hdl.siz = ota_data_len;
    ota_hdl.crc = ota_data_crc;

    LOG("OTA destination = 0x%08x, source address = 0x%08x, size = 0x%08x, CRC = 0x%04x\r\n",
    ota_hdl.dst_adr, ota_hdl.src_adr, ota_hdl.siz, ota_hdl.crc);

    offset = 0x00;
    hal_flash_erase( HAL_PARTITION_PARAMETER_1, offset, sizeof(ota_hdl_t) );

    offset = 0x00;
    hal_flash_write( HAL_PARTITION_PARAMETER_1, &offset, (const void *)&ota_hdl, sizeof(ota_hdl_t));

    offset = 0x00;
    memset(&ota_hdl_rb, 0, sizeof(ota_hdl_t));
    hal_flash_read( HAL_PARTITION_PARAMETER_1, &offset, &ota_hdl_rb, sizeof(ota_hdl_t));

    if(memcmp(&ota_hdl, &ota_hdl_rb, sizeof(ota_hdl_t)) != 0)
    {
        LOG("OTA header compare failed, OTA destination = 0x%08x, source address = 0x%08x, size = 0x%08x, CRC = 0x%04x\r\n",
        ota_hdl_rb.dst_adr, ota_hdl_rb.src_adr, ota_hdl_rb.siz, ota_hdl_rb.crc);
        return -1;
    }

    /* reboot */
    hal_reboot();

    return 0;
}

static int bk7231_ota_init(hal_ota_module_t *m, void *something)
{
    hal_logic_partition_t *partition_info;

    LOG("bk7231 ota init\n");
    _off_set = *(uint32_t*)something;
    ota_info.ota_len=_off_set;
    if(_off_set==0)
    {
        partition_info = hal_flash_get_info( HAL_PARTITION_OTA_TEMP );
        hal_flash_erase(HAL_PARTITION_OTA_TEMP, 0 ,partition_info->partition_length);
        CRC16_Init( &contex );
    }
    else
    {
        contex.crc=hal_ota_get_crc16();
        LOG("--------get crc16 context.crc=%d!--------\n",contex.crc);
    }

    return 0;
}

static int bk7231_ota_write(hal_ota_module_t *m, volatile uint32_t* off_set, uint8_t* in_buf ,uint32_t in_buf_len)
{
    if(ota_info.ota_len == 0) {
        _off_set = 0;
        CRC16_Init( &contex );
        memset(&ota_info, 0 , sizeof ota_info);
    }
    CRC16_Update( &contex, in_buf, in_buf_len);
    int ret = hal_flash_write(HAL_PARTITION_OTA_TEMP, &_off_set, in_buf, in_buf_len);
    ota_info.ota_len += in_buf_len;
    return ret;
}

static int bk7231_ota_read(hal_ota_module_t *m,  volatile uint32_t* off_set, uint8_t* out_buf, uint32_t out_buf_len)
{
    hal_flash_read(HAL_PARTITION_OTA_TEMP, off_set, out_buf, out_buf_len);
    return 0;
}

static int bk7231_ota_set_boot(hal_ota_module_t *m, void *something)
{

    ota_finish_param_t *param = (ota_finish_param_t *)something;
    if (param==NULL){
        return -1;
    }
    if (param->result_type==OTA_FINISH)
    {
        CRC16_Final( &contex, &ota_info.ota_crc );
        LOG("bk7231 set boot\n");
        hal_ota_switch_to_new_fw(param->update_type, ota_info.ota_len, ota_info.ota_crc);
        memset(&ota_info, 0 , sizeof ota_info);
    }
    else if (param->result_type==OTA_BREAKPOINT)
    {
        unsigned int save_env_addr;
        hal_logic_partition_t *partition_info;
        LOG("-------save breakpoint to flash- crc=%d--------------\n",contex.crc);
        partition_info = hal_flash_get_info( HAL_PARTITION_OTA_TEMP );
        hal_ota_save_crc16(contex.crc);
    }
    return 0;
}
#endif

struct hal_ota_module_s bk7231_ota_module = {
    .init = bk7231_ota_init,
    .ota_write = bk7231_ota_write,
    .ota_read = bk7231_ota_read,
    .ota_set_boot = bk7231_ota_set_boot,
};

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

