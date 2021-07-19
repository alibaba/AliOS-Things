#include <string.h>
#include "ota_port.h"
#include "hal_trace.h"
#include "hal_norflash.h"
#include "cmsis_os.h"
#include "cmsis.h"
#include "pmu.h"
#include "aos/hal/flash.h"
#include "aos/mtd.h"
#include <vfsdev/flash_dev.h>
#include "aos/mtdpart.h"

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

int ota_get_bootinfo(struct ota_boot_info *info, enum bootinfo_zone zone)
{
    uint32_t lock = 0;
    uint32_t start_addr = 0;
    mtd_partition_t* partition_info;
    volatile char *flashPointer = NULL;

    if (zone >= OTA_BOOTINFO_ZONEMAX) {
        TRACE("error %s %d, zone:%d", __func__, __LINE__, zone);
        return OTA_FAILE;
    }
    partition_info = (mtd_partition_t *)&mtd_partitions[MTD_PART_ID_ENV2];
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
    mtd_partition_t* partition_info = NULL;

    if (zone >= OTA_BOOTINFO_ZONEMAX) {
        TRACE("error %s %d, zone:%d", __func__, __LINE__, zone);
        return OTA_FAILE;
    }

    ret = ota_set_bootinfo_crc32value(info);
    if (ret) {
        return OTA_FAILE;
    }
    partition_info = (mtd_partition_t *)&mtd_partitions[MTD_PART_ID_ENV2];
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
    mtd_partition_t *partition_info = NULL;
    const struct ota_boot_info *info;

    if (zone >= OTA_BOOTINFO_ZONEMAX) {
        TRACE("%s %d, error zone:%d", __func__, __LINE__, zone);
        return OTA_FAILE;
    }
    partition_info = (mtd_partition_t *)&mtd_partitions[MTD_PART_ID_ENV2];
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
    (void *)param;
    return ota_upgrade_link();
}

int ota_hal_rollback_platform_hook(void)
{
     return ota_clear_reboot_count();
}

int ota_hal_platform_boot_type(void)
{
    return ota_get_boot_type();
}

int ota_get_running_index(void)
{
    int32_t ret = 0;
    struct ota_boot_info info;
    enum bootinfo_zone zone;

    zone = ota_get_valid_bootinfo_zone();
    ret = ota_get_bootinfo(&info, zone);
    if (ret) {
        printf("%s err \r\n", __FUNCTION__);
        return -1;
    }
    return info.update_link;
}

int ota_hal_final(void)
{
    return ota_set_user_bootinfo(NULL);
}
