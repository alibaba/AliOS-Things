#include <string.h>
#include "ota_port.h"
#include "hal_trace.h"
#include "hal_norflash.h"
#include "cmsis_os.h"
#include "cmsis.h"
#include "pmu.h"

extern const hal_logic_partition_t hal_partitions[];
extern osMutexId FlashMutex;
extern osMutexDef_t os_mutex_def_flash;
extern enum ota_link ota_current_link;

#define EXCEPTION_REBOOT_COUNT_MAX		5

ota_bin_partition ota_bin[] =
{
    {"ota_boot2a.bin",  HAL_PARTITION_BOOT1},
    {"ota_boot2b.bin",  HAL_PARTITION_BOOT1_REDUND},
    {"ota_rtos.bin",    HAL_PARTITION_APPLICATION},
    {"littlefs.bin",    HAL_PARTITION_LITTLEFS},
};

static void FlashosMutexWait(void)
{
    if(FlashMutex == NULL)
    {
        FlashMutex = osMutexCreate(&os_mutex_def_flash);
    }

    osMutexWait(FlashMutex, osWaitForever);
}

int ota_adapt_flash_read(const uint32_t addr, uint8_t *dst, const uint32_t size)
{
    int ret = 0;
    uint32_t lock = 0;
    volatile char *flashPointer = NULL;

	if(NULL == dst) {
        ret = -1;
        goto RETURN;
    }

    FlashosMutexWait();
    lock = int_lock();
    flashPointer = (volatile char *)(FLASH_NC_BASE + addr);
    memcpy(dst, (void *)flashPointer, size);
    int_unlock(lock);
    osMutexRelease(FlashMutex);

RETURN:
    return ret;
}

int ota_erase_norflash(hal_partition_t partition, unsigned int erase_addr, unsigned int erase_len)
{
    int ret = 0;

    if (partition == HAL_PARTITION_RTOSA) {
        hal_norflash_disable_remap(HAL_NORFLASH_ID_0);
    }

    ret = hal_norflash_erase(HAL_NORFLASH_ID_0, erase_addr, erase_len);

	if (partition == HAL_PARTITION_RTOSA) {
        hal_norflash_re_enable_remap(HAL_NORFLASH_ID_0);
    }

    return ret;
}

int ota_write_norflash(hal_partition_t partition, unsigned int addr, const unsigned char *data, unsigned int len)
{
    int ret = 0;

    if (partition == HAL_PARTITION_RTOSA) {
        hal_norflash_disable_remap(HAL_NORFLASH_ID_0);
    }

    ret = hal_norflash_write(HAL_NORFLASH_ID_0, addr, data, len);

	if (partition == HAL_PARTITION_RTOSA) {
        hal_norflash_re_enable_remap(HAL_NORFLASH_ID_0);
    }

    return ret;
}

int ota_adapt_flash_write(const hal_partition_t partition, const uint32_t addr, const uint8_t *src, const uint32_t size)
{
    int ret = 0;
    uint32_t lock;
    uint32_t num = 0;
    uint32_t left_len = 0;
    uint32_t align_len = 0;
    uint32_t fill_len = 0;
    uint32_t flash_offset = addr;
    uint32_t lengthToBurn = size;
    uint8_t *ptrSource = NULL;
    volatile char *flashPointer = NULL;
    uint8_t *buf = NULL;

    ptrSource = (uint8_t *)src;
    if (NULL == ptrSource) {
        ret = -2;
        goto RETURN;
    }

    FlashosMutexWait();
    if (addr % FLASH_BLOCK_SIZE_IN_BYTES != 0) {
        buf = (uint8_t *)aos_malloc(FLASH_BLOCK_SIZE_IN_BYTES);
        if (!buf) {
            TRACE("%s %d, rt_malloc error", __func__, __LINE__);
            ret = -1;
            goto end;
        }
    } else {
        if (lengthToBurn % FLASH_BLOCK_SIZE_IN_BYTES != 0) {
            buf = (uint8_t *)aos_malloc(FLASH_BLOCK_SIZE_IN_BYTES);
            if (!buf) {
                TRACE("%s %d, rt_malloc error", __func__, __LINE__);
                ret = -1;
                goto end;
            }
        }
    }

    pmu_flash_write_config();
    if (flash_offset % FLASH_BLOCK_SIZE_IN_BYTES != 0) {
        left_len = FLASH_BLOCK_SIZE_IN_BYTES - flash_offset % FLASH_BLOCK_SIZE_IN_BYTES;
        fill_len = (left_len >= lengthToBurn) ? lengthToBurn : left_len;

        align_len = flash_offset;
        align_len -= align_len % FLASH_BLOCK_SIZE_IN_BYTES;
        memset(buf, 0, FLASH_BLOCK_SIZE_IN_BYTES);
        // read first
        lock = int_lock();
        flashPointer = (volatile char *)(FLASH_NC_BASE + align_len);
        memcpy(buf, (void *)flashPointer, FLASH_BLOCK_SIZE_IN_BYTES);

        ret = ota_erase_norflash(partition, align_len, FLASH_BLOCK_SIZE_IN_BYTES);
        if (ret != HAL_NORFLASH_OK) {
            TRACE("error %s %d, hal_norflash_erase ret:%d", __func__, __LINE__, ret);
            pmu_flash_read_config();
            int_unlock(lock);
            goto end;
        }

        memcpy((buf + flash_offset % FLASH_BLOCK_SIZE_IN_BYTES), ptrSource, fill_len);
        ret = ota_write_norflash(partition, align_len, buf, FLASH_BLOCK_SIZE_IN_BYTES);
        if (ret != HAL_NORFLASH_OK) {
            TRACE("error %s %d, hal_norflash_write ret:%d", __func__, __LINE__, ret);
            pmu_flash_read_config();
            int_unlock(lock);
            goto end;
        }
        int_unlock(lock);

#ifdef __WATCHER_DOG_RESET__
        app_wdt_ping();
#endif

        lengthToBurn -= fill_len;
        flash_offset += fill_len;
        ptrSource += fill_len;
    }

    if (lengthToBurn > 0) {
        for (num = 0; num < lengthToBurn / FLASH_BLOCK_SIZE_IN_BYTES; num ++) {
            lock = int_lock();
            ret = ota_erase_norflash(partition, flash_offset, FLASH_BLOCK_SIZE_IN_BYTES);
            if (ret != HAL_NORFLASH_OK) {
                TRACE("error %s %d, hal_norflash_erase ret:%d", __func__, __LINE__, ret);
                pmu_flash_read_config();
                int_unlock(lock);
                goto end;
            }
            ret = ota_write_norflash(partition, flash_offset, ptrSource, FLASH_BLOCK_SIZE_IN_BYTES);
            if (ret != HAL_NORFLASH_OK) {
                TRACE("error %s %d, hal_norflash_write ret:%d", __func__, __LINE__, ret);
                pmu_flash_read_config();
                int_unlock(lock);
                goto end;
            }
            int_unlock(lock);

#ifdef __WATCHER_DOG_RESET__
            app_wdt_ping();
#endif
            flash_offset += FLASH_BLOCK_SIZE_IN_BYTES;
            ptrSource += FLASH_BLOCK_SIZE_IN_BYTES;
        }

        left_len = lengthToBurn % FLASH_BLOCK_SIZE_IN_BYTES;
        if (left_len) {
            memset(buf, 0, FLASH_BLOCK_SIZE_IN_BYTES);
            // read first
            lock = int_lock();
            flashPointer = (volatile char *)(FLASH_NC_BASE + flash_offset);
            memcpy(buf, (void *)flashPointer, FLASH_BLOCK_SIZE_IN_BYTES);

            ret = ota_erase_norflash(partition, flash_offset, FLASH_BLOCK_SIZE_IN_BYTES);
            if (ret != HAL_NORFLASH_OK) {
                TRACE("error %s %d, hal_norflash_erase ret:%d", __func__, __LINE__, ret);
                pmu_flash_read_config();
                int_unlock(lock);
                goto end;
            }

            memcpy(buf, ptrSource, left_len);
            ret = ota_write_norflash(partition, flash_offset, buf, FLASH_BLOCK_SIZE_IN_BYTES);
            if (ret != HAL_NORFLASH_OK) {
                TRACE("error %s %d, hal_norflash_write ret:%d", __func__, __LINE__, ret);
                pmu_flash_read_config();
                int_unlock(lock);
                goto end;
            }
            int_unlock(lock);

#ifdef __WATCHER_DOG_RESET__
            app_wdt_ping();
#endif
        }
    }
    pmu_flash_read_config();

end:
    if (addr % FLASH_BLOCK_SIZE_IN_BYTES != 0) {
        aos_free(buf);
        buf = NULL;
    } else {
        if (lengthToBurn % FLASH_BLOCK_SIZE_IN_BYTES != 0) {
            aos_free(buf);
            buf = NULL;
        }
    }
    osMutexRelease(FlashMutex);

RETURN:
    return ret;
}

int ota_get_bootinfo(struct ota_boot_info *info, enum bootinfo_zone zone)
{
    uint32_t lock = 0;
    uint32_t start_addr = 0;
    hal_logic_partition_t* partition_info;
    volatile char *flashPointer = NULL;

    if (zone >= OTA_BOOTINFO_ZONEMAX) {
        TRACE("error %s %d, zone:%d", __func__, __LINE__, zone);
        return OTA_FAILE;
    }
    partition_info = (hal_logic_partition_t *)&hal_partitions[HAL_PARTITION_PARAMETER_3];
    if(partition_info == NULL) {
        return OTA_FAILE;
    }
    if (zone == OTA_BOOTINFO_ZONEA) {
        start_addr = partition_info->partition_start_addr;
    } else if (zone == OTA_BOOTINFO_ZONEB) {
        start_addr = partition_info->partition_start_addr + OTA_BOOT_INFO_SIZE;
    }

    lock = int_lock();
    flashPointer = (volatile char *)(FLASH_NC_BASE + start_addr);
    memcpy((uint8_t*)info, (void *)flashPointer, sizeof(struct ota_boot_info));
    int_unlock(lock);

    return 0;
}

int ota_set_bootinfo_crc32value(struct ota_boot_info *info)
{
    uint32_t crc32_value = 0;
    uint8_t *flash_pointer = NULL;

    flash_pointer = (uint8_t *)(info);
    crc32_value = crc32(crc32_value, (uint8_t *)(flash_pointer + OTA_BOOT_INFO_HEAD_LEN), info->info_len);
    info->crc32 = crc32_value;

    return 0;
}

int ota_set_bootinfo(struct ota_boot_info *info, enum bootinfo_zone zone)
{
    int ret = 0;
    uint32_t lock = 0;
    uint32_t start_addr = 0;
    uint8_t buffer[FLASH_SECTOR_SIZE_IN_BYTES] = {0};
    hal_logic_partition_t* partition_info = NULL;

    if (zone >= OTA_BOOTINFO_ZONEMAX) {
        TRACE("error %s %d, zone:%d", __func__, __LINE__, zone);
        return OTA_FAILE;
    }

    ret = ota_set_bootinfo_crc32value(info);
    if (ret) {
        return OTA_FAILE;
    }
    partition_info = (hal_logic_partition_t *)&hal_partitions[HAL_PARTITION_PARAMETER_3];
    if(partition_info == NULL) {
        return OTA_FAILE;
    }
    if (zone == OTA_BOOTINFO_ZONEA) {
        start_addr = partition_info->partition_start_addr;
    } else if (zone == OTA_BOOTINFO_ZONEB) {
        start_addr = partition_info->partition_start_addr + OTA_BOOT_INFO_SIZE;
    }

    lock = int_lock();
    pmu_flash_write_config();

    memcpy(buffer, (uint8_t *)info, sizeof(struct ota_boot_info));
    ret = hal_norflash_erase(HAL_NORFLASH_ID_0, start_addr, FLASH_SECTOR_SIZE_IN_BYTES);
    if (ret != HAL_NORFLASH_OK) {
        TRACE("error %s %d, ret:%d", __func__, __LINE__, ret);
        goto end;
    }

    ret = hal_norflash_write(HAL_NORFLASH_ID_0, start_addr, buffer, FLASH_SECTOR_SIZE_IN_BYTES);
    if (ret != HAL_NORFLASH_OK) {
        TRACE("error %s %d, ret:%d", __func__, __LINE__, ret);
        goto end;
    }

end:
    pmu_flash_read_config();
    int_unlock(lock);

    return ret;
}

int ota_set_bootinfo_to_zoneAB(struct ota_boot_info *info)
{
    int ret = 0;

    ret = ota_set_bootinfo(info, OTA_BOOTINFO_ZONEA);
    if (ret) {
        TRACE("error %s %d, ota_set_bootinfo return %d", __func__, __LINE__, ret);
    }

    ret = ota_set_bootinfo(info, OTA_BOOTINFO_ZONEB);
    if (ret) {
        TRACE("error %s %d, ota_set_bootinfo return %d", __func__, __LINE__, ret);
    }

    return ret;
}

int ota_check_bootinfo(enum bootinfo_zone zone)
{
    uint32_t crc32_value = 0;
    uint32_t flash_offset = 0;
    uint8_t *flash_pointer = NULL;
    hal_logic_partition_t *partition_info = NULL;
    const struct ota_boot_info *info;

    if (zone >= OTA_BOOTINFO_ZONEMAX) {
        TRACE("%s %d, error zone:%d", __func__, __LINE__, zone);
        return OTA_FAILE;
    }

    partition_info = (hal_logic_partition_t *)&hal_partitions[HAL_PARTITION_PARAMETER_3];
    if(partition_info == NULL) {
        return OTA_FAILE;
    }

    //get boot info to choose linkA or linkB.
    if (zone == OTA_BOOTINFO_ZONEA) {
		flash_offset = partition_info->partition_start_addr;
    } else if (zone == OTA_BOOTINFO_ZONEB) {
		flash_offset = partition_info->partition_start_addr + OTA_BOOT_INFO_SIZE; //boot info zoneB start address
    }

	info = (const struct ota_boot_info *)(FLASH_NC_BASE + flash_offset);
    if (info->update_link >= OTA_LINK_MAX) {
        TRACE("%s %d, error info->update_link:%d", __func__, __LINE__, info->update_link);
        return OTA_FAILE;
    } else if ((info->update_link == OTA_LINK_A) && (info->linkA_used_flag != OTA_LINK_USED)) {
        TRACE("%s %d, error info->linkA_used_flag:%d", __func__, __LINE__, info->linkA_used_flag);
        return OTA_FAILE;
    } else if ((info->update_link == OTA_LINK_B) && (info->linkB_used_flag != OTA_LINK_USED)) {
        TRACE("%s %d, error info->linkB_used_flag:%d", __func__, __LINE__, info->linkB_used_flag);
        return OTA_FAILE;
    }

    flash_pointer = (uint8_t *)(info);
    crc32_value = crc32(crc32_value, (uint8_t *)(flash_pointer + OTA_BOOT_INFO_HEAD_LEN), info->info_len);
    if (crc32_value != info->crc32) {
        TRACE("%s %d, error info->crc32:0x%x, crc32_value:0x%x", __func__, __LINE__, info->crc32, crc32_value);
        return OTA_FAILE;
    }

    return OTA_OK;
}

void ota_set_zoneAB_bootinfo_to_default(void)
{
    struct ota_boot_info info;
    int ret = 0;

    TRACE("%s %d", __func__, __LINE__);

    //set boot info to A and B.
    memset(&info, 0, sizeof(struct ota_boot_info));
    info.linkA_used_flag  		 = OTA_LINK_USED;
    info.linkB_used_flag  		 = OTA_LINK_USED; //if RTOSB not write, set OTA_LINK_NOTUSED.
    info.info_len         		 = OTA_BOOT_INFO_BODY_LEN;
	info.odm_type         		 = 0;
	info.reserved         		 = 0;
    info.update_link      		 = OTA_LINK_A; //default:OTA_LINK_A
    info.reboot_reason           = 0;
    info.crash_reboot_count      = 0;
	info.secureERR_reboot_count  = 0;
    info.reboot_count_max 		 = EXCEPTION_REBOOT_COUNT_MAX;
    info.fallback_disable 		 = 1;

    ret = ota_set_bootinfo_to_zoneAB(&info);
    if (ret) {
        TRACE("error %s %d, ret:%d", __func__, __LINE__, ret);
        return;
    }
}

enum bootinfo_zone ota_get_valid_bootinfo_zone(void)
{
    enum bootinfo_zone ret = OTA_BOOTINFO_ZONEA;

    //get boot info to choose linkA or linkB.
    ret = ota_check_bootinfo(OTA_BOOTINFO_ZONEA);
    if (!ret) {
        return OTA_BOOTINFO_ZONEA;
    } else {
        ret = ota_check_bootinfo(OTA_BOOTINFO_ZONEB);
        if (!ret) {
            return OTA_BOOTINFO_ZONEB;
        } else { //first boot or both boot info bad, set both to default.
            ota_set_zoneAB_bootinfo_to_default();
            return OTA_BOOTINFO_ZONEA;
        }
    }
}

ODM_TYPE ota_adapt_get_odm_type(void)
{
    int32_t ret = 0;
    struct ota_boot_info boot_info;
    enum bootinfo_zone zone;

    zone = ota_get_valid_bootinfo_zone();
    ret = ota_get_bootinfo(&boot_info, zone);
    if (ret) {
		TRACE("error %s %d, ret:%d", __func__, __LINE__, ret);
        return ALI_ODM_MAX;
    }

    return boot_info.odm_type;
}

int ota_adapt_set_odm_type(ODM_TYPE type)
{
    int32_t ret = 0;
    struct ota_boot_info boot_info;
	enum bootinfo_zone zone;

	if (type >= ALI_ODM_MAX) {
		return -1;
	}

	zone = ota_get_valid_bootinfo_zone();
    ret = ota_get_bootinfo(&boot_info, zone);
    if (ret) {
		TRACE("error %s %d, ret:%d", __func__, __LINE__, ret);
        return ret;
    }

    boot_info.odm_type = type;
	ret = ota_set_bootinfo_to_zoneAB(&boot_info);
    if (ret) {
        TRACE("error %s %d, ret:%d", __func__, __LINE__, ret);
    }

	return ret;
}

int ota_copy_bootinfo_fromone_toanother(enum bootinfo_zone to_zone, enum bootinfo_zone from_zone)
{
    struct ota_boot_info info;
    int32_t ret = 0;

    if ((to_zone >= OTA_BOOTINFO_ZONEMAX) || (from_zone >= OTA_BOOTINFO_ZONEMAX)) {
        //TRACE("%s %d, error zone:%d", __func__, __LINE__, zone);
        return -1;
    }

    //get boot info from from_zone.
    ret = ota_get_bootinfo(&info, from_zone);
    if (ret) {
        TRACE("%s %d, ota_get_bootinfo return fail", __func__, __LINE__);
        return -1;
    }

    //set boot info to to_zone.
    ret = ota_set_bootinfo(&info, to_zone);
    if (ret) {
        TRACE("%s %d, ota_get_bootinfo return fail", __func__, __LINE__);
        return -1;
    }

    return ret;
}

/**
 * @brief Get current active A/B slot
 *
 * @returns "a" if current slot is A, or "b" if current slot is B.
 */
const char* tg_ota_get_current_ab(void)
{
    int32_t ret = 0;
    struct ota_boot_info boot_info;
    enum bootinfo_zone zone;

    zone = ota_get_valid_bootinfo_zone();
    ret = ota_get_bootinfo(&boot_info, zone);
    if (ret) {
        return NULL;
    }

    if (boot_info.update_link == OTA_LINK_A) {
        return "a";
    } else if (boot_info.update_link == OTA_LINK_B) {
        return "b";
    }
    return NULL;
}

/**
 * @return: - 0: success; - -1: fail
 */
int ota_upgrade_link()
{
    int32_t ret = -1;
    struct ota_boot_info info;
	enum bootinfo_zone zone;

	if (ota_current_link == OTA_LINK_ERR) {
		zone = ota_get_valid_bootinfo_zone();
	    ret = ota_get_bootinfo(&info, zone);
        if (ret != 0) {
            return -1;
	    }
		ota_current_link = info.update_link;
	}

    if ((ota_current_link == OTA_LINK_A) || (ota_current_link == OTA_LINK_B)) {
		zone = ota_get_valid_bootinfo_zone();
	    ret = ota_get_bootinfo(&info, zone);
        if (ret != 0) {
            return -1;
        }

	    if (ota_current_link == OTA_LINK_B) {
	        info.linkA_used_flag = OTA_LINK_USED;
			info.update_link = OTA_LINK_A;
	    } if (ota_current_link == OTA_LINK_A) {
	        info.linkB_used_flag = OTA_LINK_USED;
			info.update_link = OTA_LINK_B;
	    }

		info.crash_reboot_count = 0; //reset to 0
		info.secureERR_reboot_count = 0; //reset to 0
		info.reboot_reason = 0;
		info.fallback_disable = 0; //enable fallback

	    ret = ota_set_bootinfo_to_zoneAB(&info); //sync bootinfo A with B
	    if (ret) {
			TRACE("error %s %d, return:%d", __func__, __LINE__, ret);
            return -1;
	    } else {
			ota_current_link = info.update_link;
        }
		TRACE("%s %d, ota_current_link:%d, zone:%d", __func__, __LINE__, ota_current_link, zone);
    }

    return ret;
}

int ota_clear_reboot_count(void)
{
    int32_t ret = 0;
    struct ota_boot_info info;
	enum bootinfo_zone zone;

    zone = ota_get_valid_bootinfo_zone();
    ret = ota_get_bootinfo(&info, zone);
    if (ret) {
        return OTA_FAILE;
    }

	if ((info.crash_reboot_count != 0) ||(info.secureERR_reboot_count != 0) ) {
	    info.crash_reboot_count = 0; //reset to 0
	    info.secureERR_reboot_count = 0; //reset to 0
	    info.reboot_reason = 0;
	    info.fallback_disable = 0; //enable fallback

	    ret = ota_set_bootinfo_to_zoneAB(&info); //sync bootinfo A with B
	    if (ret) {
			TRACE("error %s %d, return:%d", __func__, __LINE__, ret);
            return ret;
	    }
	}

    return ret;
}

int ota_get_boot_type()
{
    int boot_type = 1;
    return boot_type;
}

int ota_set_user_bootinfo(void *param)
{
    return ota_upgrade_link();
}

int ota_adapt_copy_partitionA_to_partitionB(hal_partition_t dst_partition, hal_partition_t src_partition)
{
	uint8_t *buf = NULL;
	uint8_t *read_buf = NULL;
	int32_t ret = 0;
	int32_t num = 0;
	uint32_t addrA = 0;
	uint32_t addrB = 0;
    hal_partition_t partitionA_index = src_partition;
    hal_logic_partition_t partitionA_info;
    hal_partition_t partitionB_index = dst_partition;
    hal_logic_partition_t partitionB_info;

    buf = (uint8_t *)aos_malloc(FLASH_BLOCK_SIZE_IN_BYTES);
	if (!buf) {
		TRACE("%s %d, malloc fail", __func__, __LINE__);
		return OTA_FAILE;
	}

    read_buf = (uint8_t *)aos_malloc(FLASH_BLOCK_SIZE_IN_BYTES);
    if (!read_buf) {
        TRACE("%s %d, malloc fail", __func__, __LINE__);
        return OTA_FAILE;
    }

	memset(buf, 0, FLASH_BLOCK_SIZE_IN_BYTES);
	memset(read_buf, 0, FLASH_BLOCK_SIZE_IN_BYTES);

	ret = hal_flash_info_get(partitionA_index, &partitionA_info);
	if (ret) {
		TRACE("error %s %d, hal_flash_info_get return != 0", __func__, __LINE__);
		ret = OTA_FAILE;
		goto end;
	}

	ret = hal_flash_info_get(partitionB_index, &partitionB_info);
	if (ret) {
		TRACE("error %s %d, hal_flash_info_get return != 0", __func__, __LINE__);
		ret = OTA_FAILE;
		goto end;
	}

	addrA = partitionA_info.partition_start_addr;
	addrB = partitionB_info.partition_start_addr;
	TRACE("%s %d, src addr:0x%x, length:0x%x", __func__, __LINE__, addrA, partitionA_info.partition_length);
	TRACE("%s %d, dst addr:0x%x, length:0x%x", __func__, __LINE__, addrB, partitionB_info.partition_length);
    for (num = 0; num < partitionA_info.partition_length / FLASH_BLOCK_SIZE_IN_BYTES; num++) {
        ota_adapt_flash_read(addrA, buf, FLASH_BLOCK_SIZE_IN_BYTES);

        ret = ota_adapt_flash_write(partitionB_index, addrB, buf, FLASH_BLOCK_SIZE_IN_BYTES);
		if (ret) {
			TRACE("error %s %d, return %d", __func__, __LINE__, ret);
			ret = OTA_FAILE;
			goto end;
		}

        ota_adapt_flash_read(addrB, read_buf, FLASH_BLOCK_SIZE_IN_BYTES);
        if (memcmp(read_buf, buf, FLASH_BLOCK_SIZE_IN_BYTES)) {
            TRACE("error %s %d, ota_adapt_flash_write fail", __func__, __LINE__);
			ret = OTA_FAILE;
			goto end;
		}

		addrA += FLASH_BLOCK_SIZE_IN_BYTES;
		addrB += FLASH_BLOCK_SIZE_IN_BYTES;
		memset(buf, 0, FLASH_BLOCK_SIZE_IN_BYTES);
		memset(read_buf, 0, FLASH_BLOCK_SIZE_IN_BYTES);

		TRACE("%s %d, src addr:0x%x", __func__, __LINE__, addrA);
		TRACE("%s %d, dst addr:0x%x", __func__, __LINE__, addrB);
	}

end:
    aos_free(buf);
	buf = NULL;

    aos_free(read_buf);
	read_buf = NULL;

	return ret;
}
