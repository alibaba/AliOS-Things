#include "hal/soc/soc.h"
#include "stm32l0xx.h"
#include "stm32l0xx_hal_flash.h"


int32_t hal_flash_write(hal_partition_t pno, uint32_t* poff, const void* buf ,uint32_t buf_size)
{
	//Todo...
	return -1;
}

int32_t hal_flash_read(hal_partition_t pno, uint32_t* poff, void* buf, uint32_t buf_size)
{
	//Todo...
	return -1;
}

int32_t hal_flash_erase(hal_partition_t pno, uint32_t off_set,
                        uint32_t size)
{
	//Todo...
	return -1;
}

int32_t hal_flash_enable_secure(hal_partition_t partition, uint32_t off_set, uint32_t size)
{
    return 0;
}

int32_t hal_flash_dis_secure(hal_partition_t partition, uint32_t off_set, uint32_t size)
{
    return 0;
}


