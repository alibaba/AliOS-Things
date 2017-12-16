#include "hal/soc/soc.h"
#include "stm32l4xx.h"
#include "stm32l4xx_hal_flash.h"
#include "flash.h"

#define ROUND_DOWN(a,b) (((a) / (b)) * (b))

extern const hal_logic_partition_t hal_partitions[];
extern int FLASH_read_at(uint32_t address, uint64_t *pData, uint32_t len_bytes);
extern int FLASH_bank1_enabled(void);

hal_logic_partition_t *hal_flash_get_info(hal_partition_t pno)
{
    hal_logic_partition_t *logic_partition;

    logic_partition = (hal_logic_partition_t *)&hal_partitions[ pno ];

    return logic_partition;
}

int32_t hal_flash_write(hal_partition_t pno, uint32_t* poff, const void* buf ,uint32_t buf_size)
{
    uint32_t start_addr;
    hal_logic_partition_t *partition_info;
    hal_partition_t real_pno;

#ifdef CONFIG_AOS_KV_MULTIPTN_MODE
    if (pno == CONFIG_AOS_KV_PTN) {
        if ((*poff) >= CONFIG_AOS_KV_PTN_SIZE) {
            pno = (hal_partition_t)CONFIG_AOS_KV_SECOND_PTN;
            *poff = (*poff) - CONFIG_AOS_KV_PTN_SIZE;
        }
    }
#endif
    real_pno = pno;

    if (!FLASH_bank1_enabled()) {
        if (pno == CONFIG_AOS_KV_PTN) {
            real_pno = HAL_PARTITION_PARAMETER_4;
        } else if (pno == CONFIG_AOS_KV_SECOND_PTN) {
            real_pno = HAL_PARTITION_PARAMETER_1;
        }
    }

    partition_info = hal_flash_get_info( real_pno );
    start_addr = partition_info->partition_start_addr + *poff;
    if (0 != FLASH_update(start_addr, buf, buf_size)) {
        printf("FLASH_update failed!\n");
    }
    *poff += buf_size;
    return 0;
}

int32_t hal_flash_read(hal_partition_t pno, uint32_t* poff, void* buf, uint32_t buf_size)
{
    uint32_t start_addr;
    hal_logic_partition_t *partition_info;
    hal_partition_t real_pno;

#ifdef CONFIG_AOS_KV_MULTIPTN_MODE
    if (pno == CONFIG_AOS_KV_PTN) {
        if ((*poff) >=  CONFIG_AOS_KV_PTN_SIZE) {
            pno = (hal_partition_t)CONFIG_AOS_KV_SECOND_PTN;
            *poff = (*poff) - CONFIG_AOS_KV_PTN_SIZE;
        }
    }
#endif
    real_pno = pno;

    if (!FLASH_bank1_enabled()) {
        if (pno == CONFIG_AOS_KV_PTN) {
            real_pno = HAL_PARTITION_PARAMETER_4;
        } else if (pno == CONFIG_AOS_KV_SECOND_PTN) {
            real_pno = HAL_PARTITION_PARAMETER_1;
        }
    }

    partition_info = hal_flash_get_info( real_pno );

    if(poff == NULL || buf == NULL || *poff + buf_size > partition_info->partition_length)
        return -1;
    start_addr = partition_info->partition_start_addr + *poff;
    FLASH_read_at(start_addr, buf, buf_size);
    *poff += buf_size;

    return 0;
}

int32_t hal_flash_erase(hal_partition_t pno, uint32_t off_set,
                        uint32_t size)
{
    uint32_t start_addr;
    hal_logic_partition_t *partition_info;
    hal_partition_t real_pno;

#ifdef CONFIG_AOS_KV_MULTIPTN_MODE
    if (pno == CONFIG_AOS_KV_PTN) {
        if (off_set >= CONFIG_AOS_KV_PTN_SIZE) {
            pno = (hal_partition_t)CONFIG_AOS_KV_SECOND_PTN;
            off_set -= CONFIG_AOS_KV_PTN_SIZE;
        }
    }
#endif
    real_pno = pno;

    if (!FLASH_bank1_enabled()) {
        if (pno == CONFIG_AOS_KV_PTN) {
            real_pno = HAL_PARTITION_PARAMETER_4;
        } else if (pno == CONFIG_AOS_KV_SECOND_PTN) {
            real_pno = HAL_PARTITION_PARAMETER_1;
        }
    }

    partition_info = hal_flash_get_info( real_pno );
    if(size + off_set > partition_info->partition_length)
        return -1;

    start_addr = ROUND_DOWN((partition_info->partition_start_addr + off_set), FLASH_PAGE_SIZE);

    FLASH_unlock_erase(start_addr, size);
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


