#include "nbpatch.h"

extern void * nbpatch_buffer_get();

int nbpatch_eof(unsigned long src)
{
    return 0;
}

#if !(defined RECOVERY_FLASH_COPY)
int nbpatch_read(unsigned long src, const unsigned char *buffer, unsigned long pos, size_t size, u_char is_old)
{
    unsigned long base = pos;
    size_t read_size = 0;
    int ret = 0;
 
    while(size > 0) {
        read_size = size > SECTOR_SIZE ? SECTOR_SIZE : size;
        ret = patch_flash_read(src, buffer+pos-base, pos, read_size);
        if(ret < 0) {
            LOG("nbpatch_read %0x error\r\n", src);
            return -1;
        }
        pos += read_size;
        size -= read_size;
    }
    return pos - base;
}
#else
int nbpatch_read(unsigned long src, const unsigned char *buffer, unsigned long pos, size_t size, u_char is_old)
{
	unsigned long base = pos;
	size_t read_size = 0;
	int ret = 0;
        unsigned long new_src = src;

	while (size > 0) {
		read_size = size > SECTOR_SIZE ? SECTOR_SIZE : size;
                if(is_old) {
			if (nbpatch_flash_status_check(pos) == 2){
                                if (SPLICT_SIZE - pos % SPLICT_SIZE < read_size){
                                        read_size = SPLICT_SIZE - pos % SPLICT_SIZE;
                                }
				new_src = PARTITION_OTA;
			}else{
				new_src = src;
			}
		} else {
                        if (SPLICT_SIZE - pos % SPLICT_SIZE < read_size){
                                read_size = SPLICT_SIZE - pos % SPLICT_SIZE;
                        }
			new_src = src;
		}

		ret = patch_flash_read(new_src, buffer + pos - base, (is_old==0)?nbpatch_ota_addr_get(pos):pos, read_size);
		if (ret < 0) {
			LOG("r %d err\r\n", src);
			return -1;
		}
		pos += read_size;
		size -= read_size;
	}
	return pos - base;
}
#endif
int nbpatch_write(unsigned long dst, const unsigned char *buffer, unsigned long pos, size_t size)
{
    unsigned long base = pos;
    size_t write_size = 0;
    int ret = 0;

    while(size > 0) {
        write_size = size > SECTOR_SIZE? SECTOR_SIZE:size;
        ret = patch_flash_write(dst, buffer+pos-base, pos, write_size);
        if(ret < 0) {
            LOG("w %d err\r\n", dst);
            return -1;
        }
        pos += write_size;
        size -= write_size;
    }
    return pos - base;
}

uint16_t cal_crc(void *addr, off_t len)
{
    uint16_t patch_crc = 0;
    CRC16_Context crc_context;
    CRC16_Init(&crc_context);
    CRC16_Update(&crc_context, addr, len);
    CRC16_Final(&crc_context, &patch_crc);
    return patch_crc;
}

int save_patch_status(PatchStatus *status)
{
    int ret;
    uint16_t patch_crc = 0;
    if (!status) {
        LOG("st err\r\n");
        return -1;
    }

    patch_crc = cal_crc(status, sizeof(PatchStatus) - sizeof(uint16_t));
    status->patch_crc = patch_crc;

    ret = patch_flash_erase(PARTITION_PARAM, DIFF_CONF_OFFSET, sizeof(PatchStatus));
    if(ret < 0) {
        LOG("er err\r\n");
        return ret;
    }

    ret = patch_flash_write(PARTITION_PARAM, (unsigned char *) status,
            DIFF_CONF_OFFSET, sizeof(PatchStatus));
    if (ret < 0) {
        LOG("wr err\r\n");
        return ret;
    }

    //bakeup patchstatus
    ret = patch_flash_erase(PARTITION_OTA, DIFF_CONF2_OFFSET, sizeof(PatchStatus));
    if (ret < 0) {
        LOG("er err\r\n");
        return ret;
    }

    ret = patch_flash_write(PARTITION_OTA, (unsigned char *) status,
            DIFF_CONF2_OFFSET, sizeof(PatchStatus));
    if (ret < 0) {
        LOG("w err\r\n");
        return ret;
    }

    return 0;
}

int read_patch_status(PatchStatus *status)
{
    uint16_t patch_crc = 0;
    if (!status) {
        LOG("st err\r\n");
        return -1;
    }
    int ret = patch_flash_read(PARTITION_PARAM,
            (unsigned char *) status, DIFF_CONF_OFFSET, sizeof(PatchStatus));
    if(ret < 0) {
        LOG("read err\r\n");
        return ret;
    }

    uint16_t patch_crc2  = status->patch_crc;
    patch_crc = cal_crc(status, sizeof(PatchStatus) - sizeof(uint16_t));
    if(patch_crc == patch_crc2) {
        return 0;
    }
    ret = patch_flash_read(PARTITION_OTA, (unsigned char *) status,
            DIFF_CONF2_OFFSET, sizeof(PatchStatus));
    if (ret < 0) {
        LOG("read2 err\r\n");
        return ret;
    }

    patch_crc2 = status->patch_crc;
    patch_crc = cal_crc(status, sizeof(PatchStatus) - sizeof(uint16_t));
    if (patch_crc == patch_crc2) {
        return 0;
    }

    LOG("crc %d, %d b err\r\n", patch_crc, patch_crc2);
    return -1;
}

off_t save_bakeup_data(unsigned long src, off_t size)
{
    u_char buffer[SECTOR_SIZE];
    unsigned long pos = 0;
    size_t bsiz = 0;
    uint16_t crc = 0;
    int ret = 0;

    memset(buffer, 0, sizeof(buffer));
    CRC16_Context crc_context;
    CRC16_Init(&crc_context);

    ret = patch_flash_erase(PARTITION_OTA, DIFF_BACKUP_OFFSET, size);
    if(ret < 0) {
        LOG("erase err\r\n");
        return ret;
    }

    while (size > 0) {
        bsiz = size > SECTOR_SIZE ? SECTOR_SIZE : size;
        ret = patch_flash_write(PARTITION_OTA, (const unsigned char *)(src + pos), DIFF_BACKUP_OFFSET + pos, bsiz);
        if(ret < 0) {
            LOG("write err\r\n");
            return ret;
        }
        ret = patch_flash_read(PARTITION_OTA, buffer, DIFF_BACKUP_OFFSET + pos, bsiz);
        if(ret < 0) {
            LOG("read err\r\n");
            return ret;
        }
        if(memcmp((const void *)buffer, (const void *)(src+pos), bsiz) != 0) {
            LOG("crc %d err\r\n", crc_context.crc);
            return -1;
        }
        CRC16_Update(&crc_context, buffer, bsiz);
        pos += bsiz;
        size -= bsiz;
    }

    CRC16_Final(&crc_context, &crc);
    return 0;
}

#if !(defined RECOVERY_FLASH_COPY)
off_t load_bakeup_data(unsigned long dst, off_t size, off_t offset) {
    u_char buffer[SECTOR_SIZE];
    unsigned long pos = offset;
    size_t bsiz = 0;
    int ret = 0;

    size -= offset;
    memset(buffer, 0, sizeof(buffer));

    uint16_t crc = 0;
    CRC16_Context crc_context;
    CRC16_Init(&crc_context);

    patch_flash_erase(HAL_PARTITION_APPLICATION, dst + pos, size);

    while (size > 0) {
        bsiz = size > SECTOR_SIZE ? SECTOR_SIZE : size;
        memset(buffer, 0, SECTOR_SIZE);
        ret = patch_flash_read(HAL_PARTITION_OTA_TEMP, buffer, DIFF_BACKUP_OFFSET + pos, bsiz);
        if(ret < 0) {
            LOG("load_bakeup_data read  error\r\n");
            return ret;
        }

        ret = patch_flash_write(HAL_PARTITION_APPLICATION, buffer, dst + pos, bsiz);
        if(ret < 0) {
            LOG("load_bakeup_data write error\r\n");
            return ret;
        }
        CRC16_Update(&crc_context, buffer, bsiz);
        pos += bsiz;
        size -= bsiz;
    }
    CRC16_Final(&crc_context, &crc);
    return pos;
}
#else
off_t load_bakeup_data(unsigned long dst, off_t size, off_t offset) {
    u_char buffer[SECTOR_SIZE];
    unsigned long pos = offset;
    size_t bsiz = 0;
    int ret = 0;
    off_t free_offset = 0xFFFFFFFF;
    void *buf = nbpatch_buffer_get();
	
    size -= offset;
    memset(buffer, 0, sizeof(buffer));

    uint16_t crc = 0;
    CRC16_Context crc_context;
    CRC16_Init(&crc_context);

    //判断目的区间是否有被使用
    if(nbpatch_flash_status_check(dst) != 0) {
        // 如果已经被使用了，查找一个空闲区间
        free_offset = nbpatch_find_free_space();
        if(free_offset == 0xFFFFFFFF) {
            LOG("free fail!!!\n");
            return -1;
        }
        patch_flash_copy(PARTITION_OTA, free_offset, dst, SPLICT_SIZE);
        nbpatch_ota_status_set(free_offset, 1);
        nbpatch_ota_addr_set(dst, free_offset);
    }

    patch_flash_erase(PARTITION_OTA, dst + pos, size);
    //从内存中将解压数据拷贝到目的区间
    while (size > 0) {
        bsiz = size > SECTOR_SIZE ? SECTOR_SIZE : size;
        ret = patch_flash_write(PARTITION_OTA, (char *)((uint32_t)buf + pos), dst + pos, bsiz);
        if(ret < 0) {
            LOG("write err\r\n");
            return ret;
        }
        CRC16_Update(&crc_context, buffer, bsiz);
        pos += bsiz;
        size -= bsiz;
    }
    CRC16_Final(&crc_context, &crc);
    nbpatch_ota_status_set(dst, 2);
    LOG("load data, off:0x%x, len:0x%x pos:0x%x \n", offset, size, pos);
    return pos;
}
#endif
