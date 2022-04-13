/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stddef.h>
#include "aos/hal/flash.h"

extern const hal_logic_partition_t hal_partitions[];
extern const size_t hal_partitions_amount;

int32_t hal_flash_info_get(hal_partition_t pno, hal_logic_partition_t *partition)
{
    memset(partition, 0, sizeof(hal_logic_partition_t));
    return 0;
}

#if 0
int32_t hal_flash_mtdpart_info_get(hal_mtdpart_info_t **result_array, int *cnt)
{
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
        printf("Failed to malloc!\r\n");
        return -1;
    }

    /* fetch result */
    int idx = 0;
    arr = *result_array;
    for (int i = 0; i < total; i++) {
        p = &hal_partitions[i];
        if (p->partition_description == NULL) continue;
        arr[idx].p = i;
        arr[idx].descr = p->partition_description;
        arr[idx].offset = p->partition_start_addr;
        arr[idx].siz = p->partition_length;
        idx++;
    }

    return 0;
}
#endif

int32_t hal_flash_write(hal_partition_t pno, uint32_t *poff, const void *buf, uint32_t buf_size)
{
#if 0
    int size = 0;
    unsigned int chipno = 0;
    if(pno == HAL_PARTITION_LITTLEFS) {
    } else if(pno == HAL_PARTITION_LITTLEFS2) {
        chipno = 1;             // second flash
    } else if(pno == HAL_PARTITION_ENV) {
        chipno = 1;             // second flash
    } else {
        return -1;
    }
    size = nor_flash_write(chipno, poff, buf, buf_size);

    if (size <= 0)
        printf("nor_flash_read returns %d\r\n", size);
    else
        *poff += size;
#else
    *poff += buf_size;
#endif

    return 0;
}

int32_t hal_flash_read(hal_partition_t pno, uint32_t *poff, void *buf, uint32_t buf_size)
{
#if 0
    int size = 0;
    unsigned int chipno = 0;
    if(pno == HAL_PARTITION_LITTLEFS) {
    } else if(pno == HAL_PARTITION_LITTLEFS2) {
        chipno = 1;             // second flash
    } else if(pno == HAL_PARTITION_ENV) {
        //*poff += 0x3000000;
        chipno = 1;             // second flash
    } else {
        return -1;
    }

    size = nor_flash_read(chipno, poff, buf, buf_size);
    if (size <= 0)
        printf("nor_flash_read returns %d\r\n", size);
    else
        *poff += size;
#else
    *poff += buf_size;
#endif

    return 0;
}

int32_t hal_flash_erase(hal_partition_t pno, uint32_t off_set, uint32_t size)
{
#if 0
    unsigned int chipno = 0;
    if(pno == HAL_PARTITION_LITTLEFS) {
    } else if(pno == HAL_PARTITION_LITTLEFS2) {
        chipno = 1;             // second flash
    } else if(pno == HAL_PARTITION_ENV) {
        chipno = 1;             // second flash
    } else {
        return -1;
    }
    nor_flash_erase(chipno, off_set, size);
#endif
    return 0;
}

int hal_flash_isbad (hal_partition_t in_partition, uint32_t off_set)
{
    return 0;
}

int hal_flash_markbad(hal_partition_t in_partition, uint32_t off_set)
{
    return 0;
}

int32_t hal_flash_enable_secure(hal_partition_t partition, uint32_t off_set, uint32_t size)
{
    return 0;
}

int32_t hal_flash_dis_secure(hal_partition_t partition, uint32_t off_set, uint32_t size)
{
    return 0;
}
