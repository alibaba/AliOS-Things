#include "nbpatch_io.h"
#include "nbpatch.h"

extern void * nbpatch_buffer_get();

int nbpatch_eof(unsigned long src)
{
    return 0;
}

int nbpatch_read(unsigned long src, const unsigned char *buffer, unsigned long pos, size_t size)
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

#if (defined IS_ESP8266)
// 说明： nbpatch_ota_read仅能用于读取ota数据
int nbpatch_ota_read(unsigned long src, const unsigned char *buffer, unsigned long pos, size_t size)
{
	unsigned long base = pos;
	size_t read_size = 0;
	int ret = 0;
	unsigned long new_pos;

	while (size > 0) {
		read_size = size > FLASH_SECTOR_SIZE ? FLASH_SECTOR_SIZE : size;

		if (SPLICT_SIZE - pos % SPLICT_SIZE < read_size)
		{
			read_size = SPLICT_SIZE - pos % SPLICT_SIZE;
		}

		new_pos = nbpatch_ota_addr_get(pos);
		ret = patch_flash_read(src, buffer + pos - base, new_pos, read_size);
		if (ret < 0) {
			printf("nbpatch_read %0x error\r\n", src);
			return -1;
		}

		pos += read_size;
		size -= read_size;
	}
	return pos - base;
}
#else
int nbpatch_ota_read(unsigned long src, const unsigned char *buffer, unsigned long pos, size_t size)
{
    return nbpatch_read(src, buffer, pos, size);
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
            LOG("nbpatch_write %0x error\r\n", dst);
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
		LOG("status error\r\n");
        return -1;
    }

    patch_crc = cal_crc(status, sizeof(PatchStatus) - sizeof(uint16_t));
    status->patch_crc = patch_crc;

    ret = patch_flash_erase(PARTITION_PARAM, DIFF_CONF_OFFSET, sizeof(PatchStatus));
    if(ret < 0) {
        LOG("save_patch_status  erase error\r\n");
        return ret;
    }

    ret = patch_flash_write(PARTITION_PARAM, (unsigned char *) status,
            DIFF_CONF_OFFSET, sizeof(PatchStatus));
    if (ret < 0) {
        LOG("save_patch_status write  error\r\n");
        return ret;
    }

    //bakeup patchstatus
    ret = patch_flash_erase(HAL_PARTITION_OTA_TEMP, DIFF_CONF2_OFFSET, sizeof(PatchStatus));
    if (ret < 0) {
        LOG("save_patch_status2  erase error\r\n");
        return ret;
    }

    ret = patch_flash_write(HAL_PARTITION_OTA_TEMP, (unsigned char *) status,
            DIFF_CONF2_OFFSET, sizeof(PatchStatus));
    if (ret < 0) {
        LOG("save_patch_status write  error\r\n");
        return ret;
    }

    return 0;
}

int read_patch_status(PatchStatus *status)
{
	uint16_t patch_crc = 0;
    if (!status) {
		LOG("status error\r\n");
        return -1;
    }
    int ret = patch_flash_read(PARTITION_PARAM,
            (unsigned char *) status, DIFF_CONF_OFFSET, sizeof(PatchStatus));
    if(ret < 0) {
        LOG("read_patch_status read error\r\n");
        return ret;
    }

    uint16_t patch_crc2  = status->patch_crc;
    patch_crc = cal_crc(status, sizeof(PatchStatus) - sizeof(uint16_t));

    if(patch_crc == patch_crc2) {
        return 0;
    }
    LOG("read_patch_status crc %d, %d error, now read bakeup\r\n", patch_crc, patch_crc2);

    ret = patch_flash_read(HAL_PARTITION_OTA_TEMP, (unsigned char *) status,
            DIFF_CONF2_OFFSET, sizeof(PatchStatus));
    if (ret < 0) {
        LOG("read_patch_status read2 error\r\n");
        return ret;
    }

    patch_crc2 = status->patch_crc;
    patch_crc = cal_crc(status, sizeof(PatchStatus) - sizeof(uint16_t));
    if (patch_crc == patch_crc2) {
        return 0;
    }

    LOG("read_patch_status crc %d, %d both error\r\n", patch_crc, patch_crc2);
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

    ret = patch_flash_erase(HAL_PARTITION_OTA_TEMP, DIFF_BACKUP_OFFSET, size);
    if(ret < 0) {
        LOG("save_bakeup_data erase error\r\n");
        return ret;
    }

    while (size > 0) {
        bsiz = size > SECTOR_SIZE ? SECTOR_SIZE : size;
        ret = patch_flash_write(HAL_PARTITION_OTA_TEMP, (const unsigned char *)(src + pos), DIFF_BACKUP_OFFSET + pos, bsiz);
        if(ret < 0) {
            LOG("save_bakeup_data write error\r\n");
            return ret;
        }
        ret = patch_flash_read(HAL_PARTITION_OTA_TEMP, buffer, DIFF_BACKUP_OFFSET + pos, bsiz);
        if(ret < 0) {
            LOG("save_bakeup_data read error\r\n");
            return ret;
        }
        if(memcmp((const void *)buffer, (const void *)(src+pos), bsiz) != 0) {
            LOG("bsiz crc %d\r\n", crc_context.crc);
            return -1;
        }
        CRC16_Update(&crc_context, buffer, bsiz);
        pos += bsiz;
        size -= bsiz;
    }

    CRC16_Final(&crc_context, &crc);
    return 0;
}

#if (defined IS_ESP8266)
off_t load_bakeup_data_to_ota(unsigned long dst, off_t size, off_t offset) {
    u_char buffer[SECTOR_SIZE];
    unsigned long pos = offset;
    size_t bsiz = 0;
    int ret = 0;
    off_t free_offset = 0xFFFFFFFF;
    void *buf = nbpatch_buffer_get();

    printf("begin load xz data, offset 0x%x, size 0x%x\n", offset, size);

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
            printf("find free flash space fail!!!\n");
            return -1;
        }
        patch_flash_copy(HAL_PARTITION_OTA_TEMP, free_offset, dst, SPLICT_SIZE);
        nbpatch_ota_status_set(free_offset, 1);
        nbpatch_ota_addr_set(dst, free_offset);
    }

    patch_flash_erase(HAL_PARTITION_OTA_TEMP, dst + pos, size);

    //从内存中将解压数据拷贝到目的区间
    while (size > 0) {
        bsiz = size > SECTOR_SIZE ? SECTOR_SIZE : size;
        ret = patch_flash_write(HAL_PARTITION_OTA_TEMP, (char *)((uint32_t)buf + pos), dst + pos, bsiz);
        if(ret < 0) {
            printf("load_bakeup_data write error\r\n");
            return ret;
        }
        CRC16_Update(&crc_context, buffer, bsiz);
        pos += bsiz;
        size -= bsiz;
    }
    CRC16_Final(&crc_context, &crc);
    nbpatch_ota_status_set(dst, 2);

    printf("end load xz data, pos 0x%x\n", pos);

    return pos;
}
#endif

off_t load_bakeup_data(unsigned long dst, off_t size, off_t offset) {
    u_char buffer[SECTOR_SIZE];
    unsigned long pos = offset;
    size_t bsiz = 0;
    int ret = 0;

#if (defined IS_ESP8266)
    if(rec_get_ota_mode() == 2)
    {
        return load_bakeup_data_to_ota(dst, size, offset);
    }
#endif

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

