/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include "nbpatch.h"

extern void * nbpatch_buffer_get();

int nbpatch_eof(unsigned long src)
{
    return 0;
}

#if (AOS_OTA_RECOVERY_TYPE == OTA_RECOVERY_TYPE_DIRECT)
int nbpatch_read(unsigned long src, const unsigned char *buffer, unsigned long pos, size_t size, u_char is_old)
{
    unsigned long base = pos;
    size_t read_size = 0;
    int ret = 0;

    while(size > 0) {
        read_size = size > SECTOR_SIZE ? SECTOR_SIZE : size;
        ret = patch_flash_read(src, buffer+pos-base, pos, read_size);
        if(ret < 0) {
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
		    return ret;
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
            return ret;
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
    int ret = 0;
    uint16_t patch_crc = 0;
    if (!status) {
        return -1;
    }

    patch_crc = cal_crc(status, sizeof(PatchStatus) - sizeof(uint16_t));
    status->patch_crc = patch_crc;
    ret = patch_flash_erase(PARTITION_PARAM, DIFF_CONF_OFFSET, sizeof(PatchStatus));
    if(ret < 0) {
        goto ERR;
    }

    ret = patch_flash_write(PARTITION_PARAM, (unsigned char *) status,
            DIFF_CONF_OFFSET, sizeof(PatchStatus));
    if (ret < 0) {
        goto ERR;
    }

    //bakeup patchstatus
    ret = patch_flash_erase(PARTITION_BACKUP_PARAM, DIFF_CONF_OFFSET, sizeof(PatchStatus));
    if (ret < 0) {
        goto ERR;
    }

    ret = patch_flash_write(PARTITION_BACKUP_PARAM, (unsigned char *) status,
            DIFF_CONF_OFFSET, sizeof(PatchStatus));
    if (ret < 0) {
        goto ERR;
    }

    return 0;

ERR:
    LOG("st err:%d",ret);
    return ret;
}

int read_patch_status(PatchStatus *status)
{
    uint16_t patch_crc = 0;
    if (!status) {
        return -1;
    }

    int ret = patch_flash_read(PARTITION_PARAM,
            (unsigned char *) status, DIFF_CONF_OFFSET, sizeof(PatchStatus));
    if(ret < 0) {
        return ret;
    }

    uint16_t patch_crc2  = status->patch_crc;
    patch_crc = cal_crc(status, sizeof(PatchStatus) - sizeof(uint16_t));
    if(patch_crc == patch_crc2) {
        return 0;
    }

    ret = patch_flash_read(PARTITION_BACKUP_PARAM, (unsigned char *) status,
            DIFF_CONF_OFFSET, sizeof(PatchStatus));
    if (ret < 0) {
        return ret;
    }

    patch_crc2 = status->patch_crc;
    patch_crc = cal_crc(status, sizeof(PatchStatus) - sizeof(uint16_t));
    if (patch_crc == patch_crc2) {
        return 0;
    }

    LOG("crc err %d, %d", patch_crc, patch_crc2);
    return -1;
}

#if (AOS_OTA_RECOVERY_TYPE == OTA_RECOVERY_TYPE_DIRECT)
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
        return ret;
    }

    while (size > 0) {
        bsiz = size > SECTOR_SIZE ? SECTOR_SIZE : size;
        ret = patch_flash_write(PARTITION_OTA, (const unsigned char *)(src + pos), DIFF_BACKUP_OFFSET + pos, bsiz);
        if(ret < 0) {
            return ret;
        }
        ret = patch_flash_read(PARTITION_OTA, buffer, DIFF_BACKUP_OFFSET + pos, bsiz);
        if(ret < 0) {
            return ret;
        }
        if(memcmp((const void *)buffer, (const void *)(src+pos), bsiz) != 0) {
            LOG("crc %d err", crc_context.crc);
            return -1;
        }
        CRC16_Update(&crc_context, buffer, bsiz);
        pos += bsiz;
        size -= bsiz;
    }

    CRC16_Final(&crc_context, &crc);
    return 0;
}

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
            return ret;
        }

        ret = patch_flash_write(HAL_PARTITION_APPLICATION, buffer, dst + pos, bsiz);
        if(ret < 0) {
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

    if(nbpatch_flash_status_check(dst) != 0) {
        free_offset = nbpatch_find_free_space();
        if(free_offset == 0xFFFFFFFF) {
            LOG("find space err\n");
            return -1;
        }
        patch_flash_copy(PARTITION_OTA, free_offset, dst, SPLICT_SIZE);
        nbpatch_ota_status_set(free_offset, OTA_FLASH_STATUS_USED);
        nbpatch_ota_addr_set(dst, free_offset);
    }

    patch_flash_erase(PARTITION_OTA, dst + pos, size);
    //copy decompressed data from memory to destination flash partion
    while (size > 0) {
        bsiz = size > SECTOR_SIZE ? SECTOR_SIZE : size;
        ret = patch_flash_write(PARTITION_OTA, (const unsigned char *)((uint32_t)buf + pos), dst + pos, bsiz);
        if(ret < 0) {
            return ret;
        }
        CRC16_Update(&crc_context, buffer, bsiz);
        pos += bsiz;
        size -= bsiz;
    }
    CRC16_Final(&crc_context, &crc);
    nbpatch_ota_status_set(dst, OTA_FLASH_STATUS_REVY);
    return pos;
}
#endif
