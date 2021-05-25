/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include "aos/hal/flash.h"
#include "cmsis_os.h"
#include "cmsis.h"
#include "hal_trace.h"
#include "hal_norflash.h"
#define BES_HAL_DEBUG 0
#include "app_hal.h"
#include "ota_port.h"

#define FLASH_PAGE_SIZE_IN_BYTES 256

extern const size_t hal_partitions_amount;
extern const hal_logic_partition_t hal_partitions[];
enum ota_link ota_current_link = OTA_LINK_ERR;
osMutexId FlashMutex = NULL;
osMutexDef_t os_mutex_def_flash;
static void FlashosMutexWait(void)
{
    if(FlashMutex == NULL)
    {
        FlashMutex = osMutexCreate(&os_mutex_def_flash);
    }

    osMutexWait(FlashMutex, osWaitForever);
}

void * my_memcpy(void *dst, void *src, size_t num)
{
	int offset1 = (4 - ((uint32)dst & 3)) & 3;
	int offset2 = (4 - ((uint32)src & 3)) & 3;

	if(offset1 != offset2) {
		return memcpy(dst, src, num);
	}

    int wordnum = num > offset1 ? (num - offset1) /8 : 0;
    int slice = num > offset1 ? (num-offset1) % 8 : 0;
	char *pdst = (char *)dst;
	char *psrc = (char *)src;
    long long * pintsrc;
    long long * pintdst;

	while(offset1--) {
		*pdst++ = *psrc++;
	}

	pintdst = (long long*)pdst;
	pintsrc = (long long*)psrc;
    while(wordnum--) {
		*pintdst++ = *pintsrc++;
    }

	pdst = (char*)pintdst;
	psrc = (char*)pintsrc;
    while(slice--) {
		*pdst++ = *psrc++;
    }

    return dst;
}

int bes_hal_flash_read(const hal_partition_t partition, const uint32_t addr, uint8_t *dst, const uint32_t size)
{
    int ret = 0;
    uint32_t lock = 0;
    volatile char *flashPointer = NULL;

	if (NULL == dst) {
        ret = -1;
        goto RETURN;
    }
    watchdog_feeddog();
    FlashosMutexWait();
	if (ota_current_link == OTA_LINK_B) {
		if (partition == HAL_PARTITION_APPLICATION) {
			hal_norflash_disable_remap(HAL_NORFLASH_ID_0);
		}
	}

    flashPointer = (volatile char *)(FLASH_NC_BASE + addr);
    //memcpy(dst, (void *)flashPointer, size);
    my_memcpy(dst, (void *)flashPointer, size);

	if (ota_current_link == OTA_LINK_B) {
		if (partition == HAL_PARTITION_APPLICATION) {
			hal_norflash_re_enable_remap(HAL_NORFLASH_ID_0);
        }
	}
    osMutexRelease(FlashMutex);
RETURN:
    return ret;
}

#if 0
int bes_hal_flash_write(const uint32_t addr, const uint8_t *src, const uint32_t size)
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
        //buf = (uint8_t *)rt_malloc(FLASH_BLOCK_SIZE_IN_BYTES);
        buf = (uint8_t *)malloc(FLASH_BLOCK_SIZE_IN_BYTES);
        if (!buf) {
            TRACE("%s %d, rt_malloc error", __func__, __LINE__);
            ret = -1;
            goto end;
        }
    } else {
        if (lengthToBurn % FLASH_BLOCK_SIZE_IN_BYTES != 0) {
            //buf = (uint8_t *)rt_malloc(FLASH_BLOCK_SIZE_IN_BYTES);
            buf = (uint8_t *)malloc(FLASH_BLOCK_SIZE_IN_BYTES);
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

        ret = hal_norflash_erase(HAL_NORFLASH_ID_0, align_len, FLASH_BLOCK_SIZE_IN_BYTES);
        if (ret != HAL_NORFLASH_OK) {
            TRACE("error %s %d, hal_norflash_erase ret:%d", __func__, __LINE__, ret);
            pmu_flash_read_config();
            int_unlock(lock);
            goto end;
        }

        memcpy((buf + flash_offset % FLASH_BLOCK_SIZE_IN_BYTES), ptrSource, fill_len);
        ret = hal_norflash_write(HAL_NORFLASH_ID_0, align_len, buf, FLASH_BLOCK_SIZE_IN_BYTES);
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
            ret = hal_norflash_erase(HAL_NORFLASH_ID_0, flash_offset, FLASH_BLOCK_SIZE_IN_BYTES);
            if (ret != HAL_NORFLASH_OK) {
                TRACE("error %s %d, hal_norflash_erase ret:%d", __func__, __LINE__, ret);
                pmu_flash_read_config();
                int_unlock(lock);
                goto end;
            }
            ret = hal_norflash_write(HAL_NORFLASH_ID_0, flash_offset, ptrSource, FLASH_BLOCK_SIZE_IN_BYTES);
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

            ret = hal_norflash_erase(HAL_NORFLASH_ID_0, flash_offset, FLASH_BLOCK_SIZE_IN_BYTES);
            if (ret != HAL_NORFLASH_OK) {
                TRACE("error %s %d, hal_norflash_erase ret:%d", __func__, __LINE__, ret);
                pmu_flash_read_config();
                int_unlock(lock);
                goto end;
            }

            memcpy(buf, ptrSource, left_len);
            ret = hal_norflash_write(HAL_NORFLASH_ID_0, flash_offset, buf, FLASH_BLOCK_SIZE_IN_BYTES);
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
        //rt_free(buf);
        free(buf);
        buf = NULL;
    } else {
        if (lengthToBurn % FLASH_BLOCK_SIZE_IN_BYTES != 0) {
            //rt_free(buf);
            free(buf);
            buf = NULL;
        }
    }
    osMutexRelease(FlashMutex);
    
RETURN:
    return ret;
}
#else
int bes_hal_flash_write(const hal_partition_t partition, const uint32_t addr, const uint8_t *src, const uint32_t size)
{
    int ret = 0;
    uint32_t lock;
    
    watchdog_feeddog();
    FlashosMutexWait();
    lock = int_lock();
    pmu_flash_write_config();
	if (partition == HAL_PARTITION_APPLICATION) {
        hal_norflash_disable_remap(HAL_NORFLASH_ID_0);
    }

    ret = hal_norflash_write(HAL_NORFLASH_ID_0, addr, src, size);

	if (partition == HAL_PARTITION_APPLICATION) {
        hal_norflash_re_enable_remap(HAL_NORFLASH_ID_0);
    }

    pmu_flash_read_config();
    int_unlock(lock);
    osMutexRelease(FlashMutex);

    if (ret != HAL_NORFLASH_OK) {
        TRACE("error %s %d, hal_norflash_write ret:%d", __func__, __LINE__, ret);
    }
    return ret;
}
#endif

#if 0
int bes_hal_flash_erase(const uint32_t addr, const uint32_t size)
{
    int i = 0;
    int ret = 0;
    uint32_t lock = 0;
    uint32_t start_addr = addr;
    uint16_t start_sec = (addr / FLASH_SECTOR_SIZE_IN_BYTES);
    uint16_t end_sec = ((addr + size - 1) / FLASH_SECTOR_SIZE_IN_BYTES);

    if (addr % FLASH_SECTOR_SIZE_IN_BYTES != 0) {
        TRACE("%s %d, unaligned addr:%d", __func__, __LINE__, addr);
        //return -2;  // allow to erase from unaligned addr
    }

    FlashosMutexWait();
    lock = int_lock();
    pmu_flash_write_config();

    for (i = start_sec; i <= end_sec; i++) {
        ret = hal_norflash_erase(HAL_NORFLASH_ID_0, start_addr, FLASH_SECTOR_SIZE_IN_BYTES);
        if (ret != HAL_NORFLASH_OK) {
            TRACE("error %s %d, ret:%d", __func__, __LINE__, ret);
            goto end;
        }
        start_addr += FLASH_SECTOR_SIZE_IN_BYTES;
    }

end:
    pmu_flash_read_config();
    int_unlock(lock);
    osMutexRelease(FlashMutex);

    return ret;
}
#else
int bes_hal_flash_erase(const hal_partition_t partition, const uint32_t addr, const uint32_t size)
{
    int i = 0;
    int ret = 0;
    uint32_t lock = 0;

    FlashosMutexWait();
    lock = int_lock();
    pmu_flash_write_config();

	if (partition == HAL_PARTITION_APPLICATION) {
        hal_norflash_disable_remap(HAL_NORFLASH_ID_0);
    }

    ret = hal_norflash_erase(HAL_NORFLASH_ID_0, addr, size);
    if (ret != HAL_NORFLASH_OK) {
        TRACE("error %s %d, ret:%d", __func__, __LINE__, ret);
    }

	if (partition == HAL_PARTITION_APPLICATION) {
        hal_norflash_re_enable_remap(HAL_NORFLASH_ID_0);
    }

    pmu_flash_read_config();
    int_unlock(lock);
    osMutexRelease(FlashMutex);

    return ret;
}
#endif

enum ota_link bes_get_current_link(void)
{
	int32_t ret = 0;
    struct ota_boot_info info;
	enum bootinfo_zone zone;

	zone = ota_get_valid_bootinfo_zone();
	ret = ota_get_bootinfo(&info, zone);
	if (ret) {
		return -1;
	}

	return info.update_link;
}

hal_partition_t bes_get_operate_partition(const hal_partition_t in_partition)
{
	hal_partition_t out_partition = 0;
	int32_t ret = 0;
    hal_partition_t partition_index;
    hal_logic_partition_t partition_info;
    struct ota_boot_info info;
	enum bootinfo_zone zone;

	if (in_partition >= HAL_PARTITION_MAX) {
		return HAL_PARTITION_ERROR;
	}

	if (ota_current_link == OTA_LINK_ERR) {
		ota_current_link = bes_get_current_link();
	}

	if (in_partition == HAL_PARTITION_OTA_TEMP) {
		if (ota_current_link == OTA_LINK_A) { //Now run RTOS zoneA, update zoneB.
			out_partition = HAL_PARTITION_OTA_TEMP;
		} else if (ota_current_link == OTA_LINK_B) { //Now run RTOS zoneB, update zoneA.
			out_partition = HAL_PARTITION_APPLICATION;
		}
	} else {
		out_partition = in_partition;
	}

	return out_partition;
}

int32_t hal_flash_mtdpart_info_get(hal_mtdpart_info_t **result_array, int *cnt)
{
#if CONFIG_U_FLASH_CORE
    return -1;
#else
    hal_logic_partition_t *p;
    hal_mtdpart_info_t *arr;
    size_t total = hal_partitions_amount;

    /* get the actual cnt */
    *cnt = 0;
    for (int i = 0; i < total; i++) {
        p = &hal_partitions[i];
        if (p->partition_description == NULL) continue;
        *cnt += 1;
    }

    /* memory alloc */
    *result_array = (hal_mtdpart_info_t *)malloc(sizeof(hal_mtdpart_info_t) * (*cnt));
    if (*result_array == NULL) {
        return -1;
    }

    /* fetch result */
    int idx = 0;
    arr = *result_array;
    for (int i = 0; i < total; i++) {
        p = &hal_partitions[i];
        if (p->partition_description == NULL) continue;
        arr[idx].p = i;
        arr[idx].descr = (char *)p->partition_description;
        arr[idx].offset = p->partition_start_addr;
        arr[idx].siz = p->partition_length;
        arr[idx].ssiz = CONFIG_LFS_PROG_SIZE;
        arr[idx].bsiz = CONFIG_LFS_PROG_SIZE * CONFIG_LFS_PAGE_NUM_PER_BLOCK;
        idx++;
    }

    return 0;
#endif
}

/**
 * Get the information of the specified flash area
 *
 * @param[in]  in_partition     The target flash logical partition
 * @param[in]  partition        The buffer to store partition info
 *
 * @return  0: On success， otherwise is error
 */
int32_t hal_flash_info_get(hal_partition_t in_partition, hal_logic_partition_t *partition)
{
    int32_t ret = -1;
    hal_logic_partition_t *logic_partition;
    hal_partition_t partition_id = 0;

    if ((partition != NULL) && (in_partition < hal_partitions_amount)) {
        ENTER_FUNCTION();
        partition_id = bes_get_operate_partition(in_partition);
        if (partition_id < 0) {
            TRACE("bes_get_operate_partition fail\n");
        }
        else {
            logic_partition = (hal_logic_partition_t *)&hal_partitions[partition_id];
            memcpy(partition, logic_partition, sizeof(hal_logic_partition_t));
            ret = 0;
        }
        LEAVE_FUNCTION();
    }
	return ret;
}

/**
 * Erase an area on a Flash logical partition
 *
 * @note  Erase on an address will erase all data on a sector that the
 *        address is belonged to, this function does not save data that
 *        beyond the address area but in the affected sector, the data
 *        will be lost.
 *
 * @param[in]  in_partition  The target flash logical partition which should be erased
 * @param[in]  off_set       Start address of the erased flash area
 * @param[in]  size          Size of the erased flash area
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t hal_flash_erase(hal_partition_t in_partition, uint32_t off_set, uint32_t size)
{
    uint32_t start_addr;
    hal_logic_partition_t info;
    hal_partition_t operate_partition_id = 0;
    int32_t ret = 0;
    watchdog_feeddog();
    ENTER_FUNCTION();
    if (hal_flash_info_get(in_partition, &info) != 0) {
        TRACEME("hal_flash_info_get fail\n");
        ret = -1;
        goto RETURN;
    }
    start_addr = info.partition_start_addr + off_set;
    operate_partition_id = bes_get_operate_partition(in_partition);
    TRACEME("hal_flash_erase part start0x%x offset 0x%x  size 0x%x\n", info.partition_start_addr, off_set, size);
    ret = bes_hal_flash_erase(operate_partition_id, start_addr, size);

RETURN:
    LEAVE_FUNCTION();
    return ret;
}

/**
 * Write data to an area on a flash logical partition without erase
 *
 * @param[in]  in_partition    The target flash logical partition which should be read which should be written
 * @param[in]  off_set         Point to the start address that the data is written to, and
 *                             point to the last unwritten address after this function is
 *                             returned, so you can call this function serval times without
 *                             update this start address.
 * @param[in]  inBuffer        point to the data buffer that will be written to flash
 * @param[in]  inBufferLength  The length of the buffer
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t hal_flash_write(hal_partition_t in_partition, uint32_t *off_set,
                        const void *in_buf, uint32_t in_buf_len)
{
    int32_t ret = 0;
    uint32_t start_addr;
    uint32_t partition_end;
    hal_logic_partition_t info;
    hal_partition_t operate_partition_id = 0;

    ENTER_FUNCTION();
    if (hal_flash_info_get(in_partition, &info) != 0) {
        TRACEME("hal_flash_info_get fail\n");
        ret = -1;
        goto RETURN;
    }

    start_addr = info.partition_start_addr + *off_set;
    partition_end = info.partition_start_addr + info.partition_length;
    TRACEME("hal_flash_write partstart 0x%x offset 0x%x len 0x%x\n", info.partition_start_addr, *off_set, in_buf_len);
    if(start_addr >= partition_end){
        TRACEME("flash over write\r\n");
        ret = -1;
        goto RETURN;
    }

    if((start_addr + in_buf_len) > partition_end){
        in_buf_len = partition_end - start_addr;
        TRACEME("flash over write, new len is %d\r\n", in_buf_len);
    }
    operate_partition_id = bes_get_operate_partition(in_partition);
    ret = bes_hal_flash_write(operate_partition_id, start_addr, in_buf, in_buf_len);
    if (!ret) {
        *off_set += in_buf_len;
    }

RETURN:
    LEAVE_FUNCTION();
	return ret;
}

/**
 * Write data to an area on a flash logical partition with erase first
 *
 * @param[in]  in_partition    The target flash logical partition which should be read which should be written
 * @param[in]  off_set         Point to the start address that the data is written to, and
 *                             point to the last unwritten address after this function is
 *                             returned, so you can call this function serval times without
 *                             update this start address.
 * @param[in]  inBuffer        point to the data buffer that will be written to flash
 * @param[in]  inBufferLength  The length of the buffer
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t hal_flash_erase_write(hal_partition_t in_partition, uint32_t *off_set,
                              const void *in_buf, uint32_t in_buf_len)
{
    int32_t ret = 0;
    uint32_t start_addr;
    uint32_t partition_end;
    hal_logic_partition_t info;
    hal_partition_t operate_partition_id = 0;
    ENTER_FUNCTION();
    if (hal_flash_info_get(in_partition, &info) != 0) {
        TRACEME("hal_flash_info_get fail\n");
        ret = -1;
        goto RETURN;
    }

    start_addr = info.partition_start_addr + *off_set;
    partition_end = info.partition_start_addr + info.partition_length;

    if(start_addr >= partition_end){
        TRACEME("flash over write\r\n");
        ret = -1;
        goto RETURN;
    }

    if((start_addr + in_buf_len) > partition_end){
        in_buf_len = partition_end - start_addr;
        TRACEME("flash over write, new len is %d\r\n", in_buf_len);
    }
    operate_partition_id = bes_get_operate_partition(in_partition);
    ret = bes_hal_flash_erase(operate_partition_id, start_addr, in_buf_len);
    if (ret) {
        TRACEME("flash erase fail\r\n");
        ret = -1;
        goto RETURN;
    }

    ret = bes_hal_flash_write(operate_partition_id, start_addr, in_buf, in_buf_len);
    if (!ret) {
        *off_set += in_buf_len;
    }

RETURN:
    LEAVE_FUNCTION();
    return ret;
}

/**
 * Read data from an area on a Flash to data buffer in RAM
 *
 * @param[in]  in_partition    The target flash logical partition which should be read
 * @param[in]  off_set         Point to the start address that the data is read, and
 *                             point to the last unread address after this function is
 *                             returned, so you can call this function serval times without
 *                             update this start address.
 * @param[in]  outBuffer       Point to the data buffer that stores the data read from flash
 * @param[in]  inBufferLength  The length of the buffer
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t hal_flash_read(hal_partition_t in_partition, uint32_t *off_set,
                       void *out_buf, uint32_t in_buf_len)
{
    int32_t ret = 0;
    uint32_t start_addr;
    hal_logic_partition_t info;
    hal_partition_t operate_partition_id = 0;
    ENTER_FUNCTION();
    if (hal_flash_info_get(in_partition, &info) != 0) {
        TRACEME("hal_flash_info_get fail\n");
        ret = -1;
        goto RETURN;
    }
    start_addr = info.partition_start_addr + *off_set;
    operate_partition_id = bes_get_operate_partition(in_partition);
    ret = bes_hal_flash_read(operate_partition_id, start_addr, out_buf, in_buf_len);
    if(ret == 0) {
        *off_set += in_buf_len;
    }
RETURN:
    LEAVE_FUNCTION();
    return ret;
}
/**
 * Set security options on a logical partition
 *
 * @param[in]  partition  The target flash logical partition
 * @param[in]  offset     Point to the start address that the data is read, and
 *                        point to the last unread address after this function is
 *                        returned, so you can call this function serval times without
 *                        update this start address.
 * @param[in]  size       Size of enabled flash area
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t hal_flash_enable_secure(hal_partition_t partition, uint32_t off_set, uint32_t size)
{
    //FAIL_FUNCTION();
    return 0;
}

/**
 * Disable security options on a logical partition
 *
 * @param[in]  partition  The target flash logical partition
 * @param[in]  offset     Point to the start address that the data is read, and
 *                        point to the last unread address after this function is
 *                        returned, so you can call this function serval times without
 *                        update this start address.
 * @param[in]  size       Size of disabled flash area
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t hal_flash_dis_secure(hal_partition_t partition, uint32_t off_set, uint32_t size)
{
    //FAIL_FUNCTION();
    return 0;
}

/**
 * Convert physical address to logic partition id and offset in partition
 *
 * @param[out]  in_partition Point to the logic partition id
 * @param[out]  off_set      Point to the offset in logic partition
 * @param[in]   addr         The physical address
 *
 * @return 0 : On success, EIO : If an error occurred with any step
 */
int32_t hal_flash_addr2offset(hal_partition_t *in_partition, uint32_t *off_set, uint32_t addr)
{
    //FAIL_FUNCTION();
    return 0;
}