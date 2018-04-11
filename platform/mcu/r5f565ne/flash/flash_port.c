
/* Start user code for include. Do not edit comment generated here */
#include "platform.h"
#include "r_flash_rx_if.h"
#include "r_flash_rx_config.h"
/* End user code. Do not edit comment generated here */
#include "flash.h"

static uint8_t DF_Program_buf[1024];

static void R_FLASH_Read(uint32_t address, uint32_t *pData, uint32_t len_bytes);
static flash_block_address_t R_FLASH_BlockGet(uint32_t address);
static uint8_t R_FLASH_BlockNumGet(uint32_t address,uint32_t size);
static void R_FLASH_COPY(uint32_t address,uint32_t size,uint8_t *buf);

extern const hal_logic_partition_t hal_partitions[];

hal_logic_partition_t *hal_flash_get_info(hal_partition_t pno)
{
    hal_logic_partition_t *logic_partition;

    logic_partition = (hal_logic_partition_t *)&hal_partitions[ pno ];

    return logic_partition;
}

int32_t hal_flash_write(hal_partition_t pno, uint32_t* poff, const void* buf ,uint32_t buf_size)
{
	flash_err_t reval;
	flash_res_t result;

    uint32_t start_addr;
    uint32_t end_addr;
    uint32_t f_start;
    uint32_t f_end;
    uint32_t f_size;

    hal_logic_partition_t *partition_info;
    uint16_t i;

    uint8_t *dst;
    uint8_t *src;

#ifdef CONFIG_AOS_KV_MULTIPTN_MODE
    if (pno == CONFIG_AOS_KV_PTN) {
        if ((*poff) >= CONFIG_AOS_KV_PTN_SIZE) {
            pno = CONFIG_AOS_KV_SECOND_PTN;
            *poff = (*poff) - CONFIG_AOS_KV_PTN_SIZE;
        }
    }
#endif

    partition_info = hal_flash_get_info( pno );
    start_addr = partition_info->partition_start_addr + *poff;
    end_addr = start_addr + buf_size;
    if(start_addr < 0xFFE00000)																		//if start address < 0xFFF00000,the target Flash area is DataFlash
    {
    	f_start = start_addr - (start_addr&0xFFFF)%FLASH_DF_MIN_PGM_SIZE;							//Programming start address must be divisible by the minimum programming size
    	if((end_addr&0xFFFF)%FLASH_DF_MIN_PGM_SIZE != 0)
    	{
    		f_end = end_addr + FLASH_DF_MIN_PGM_SIZE - (end_addr&0xFFFF)%FLASH_DF_MIN_PGM_SIZE;		//end address not included
    	}
    	else
    	{
    		f_end = end_addr;
    	}
    	f_size = f_end - f_start;
    	R_FLASH_BlankCheck(f_start,f_size,&result);
    	if(result != FLASH_RES_BLANK)																//if target area is not blank,refresh all covered blocks
    	{
    		f_start = (f_start/FLASH_DF_BLOCK_SIZE)*FLASH_DF_BLOCK_SIZE;
    		f_end = (f_end/FLASH_DF_BLOCK_SIZE+1)*FLASH_DF_BLOCK_SIZE;
    		f_size = f_end - f_start;
    	}
    	R_FLASH_COPY(f_start,f_size,&DF_Program_buf[0]);											//Copy old flash data in blocks to be refreshed

		dst = &DF_Program_buf[0]+(start_addr - f_start);											//only update the data to be programmed this time
		src = buf;
		for(i=0;i<buf_size;i++)
		{
			 *(dst++) = *(src++);
		}
    	if(result != FLASH_RES_BLANK)																//if target area is not blank,erase the target block(s)
    	{
			R_FLASH_Erase(f_start,f_size/FLASH_DF_BLOCK_SIZE);
    	}
		reval = R_FLASH_Write(&DF_Program_buf[0],f_start,f_size);									//program new data as long as old data in releated blocks

		if (FLASH_SUCCESS != reval) {
			printf("HAL Data Flash operation error! \r\n");
			return -5;
		}
		*poff += buf_size;
    }
    else
    {
		buf_size = ((buf_size/(FLASH_CF_MIN_PGM_SIZE+1))+1)*FLASH_CF_MIN_PGM_SIZE;
		reval = R_FLASH_Write(buf,start_addr,buf_size);

		if (FLASH_SUCCESS != reval) {
			printf("HAL FLASH update failed! \r\n");
			return -5;
		}

		*poff += buf_size;
    }
    return 0;
}

int32_t hal_flash_read(hal_partition_t pno, uint32_t* poff, void* buf, uint32_t buf_size)
{
    uint32_t start_addr;
    hal_logic_partition_t *partition_info;

#ifdef CONFIG_AOS_KV_MULTIPTN_MODE
    if (pno == CONFIG_AOS_KV_PTN) {
        if ((*poff) >=  CONFIG_AOS_KV_PTN_SIZE) {
            pno = CONFIG_AOS_KV_SECOND_PTN;
            *poff = (*poff) - CONFIG_AOS_KV_PTN_SIZE;
        }
    }
#endif

    partition_info = hal_flash_get_info( pno );

//    if(poff == NULL || buf == NULL || *poff + buf_size > partition_info->partition_length)
//        return -1;
    start_addr = partition_info->partition_start_addr + *poff;
    R_FLASH_Read(start_addr, buf, buf_size);
    *poff += buf_size;

    return 0;
}

int32_t hal_flash_erase(hal_partition_t pno, uint32_t off_set,
                        uint32_t size)
{
	flash_err_t reval;
    uint32_t start_addr;
    hal_logic_partition_t *partition_info;
    flash_block_address_t block;
	uint8_t	blockNum;
	flash_access_window_config_t  accessInfo;

#ifdef CONFIG_AOS_KV_MULTIPTN_MODE
    if (pno == CONFIG_AOS_KV_PTN) {
        if (off_set >= CONFIG_AOS_KV_PTN_SIZE) {
            pno = CONFIG_AOS_KV_SECOND_PTN;
            off_set -= CONFIG_AOS_KV_PTN_SIZE;
        }
    }
#endif

    partition_info = hal_flash_get_info( pno );
//    if(size + off_set > partition_info->partition_length)
//        return -1;

    start_addr = partition_info->partition_start_addr + off_set;

    block = R_FLASH_BlockGet(start_addr);
    blockNum = R_FLASH_BlockNumGet(start_addr,size);
        if(start_addr>=0xFFFF0000)
        {
            accessInfo.start_addr = (uint32_t)block;
        	accessInfo.end_addr = (uint32_t)block+blockNum*FLASH_CF_SMALL_BLOCK_SIZE;
            reval = R_FLASH_Control(FLASH_CMD_ACCESSWINDOW_SET, (void *)&accessInfo);
            if (reval != FLASH_SUCCESS)
            {
                while (1) ;
            }
        }
        else if(start_addr<0xFFFF0000 && start_addr>= 0xFFE00000)
        {
        	accessInfo.start_addr = (uint32_t)block;
        	accessInfo.end_addr = (uint32_t)block+blockNum*FLASH_CF_MEDIUM_BLOCK_SIZE;
            reval = R_FLASH_Control(FLASH_CMD_ACCESSWINDOW_SET, (void *)&accessInfo);
            if (reval != FLASH_SUCCESS)
            {
                while (1) ;
            }
        }
    reval = R_FLASH_Erase(block,blockNum);

    
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


static void R_FLASH_Read(uint32_t address, uint32_t *pData, uint32_t len_bytes)
{
    int i;
    uint32_t *src = (uint32_t *)(address);
    uint32_t *dst = ((uint32_t *) pData);

    for (i = 0; i < len_bytes; i += 4) {
        *(dst++) = *(src++);
    }
}



static flash_block_address_t R_FLASH_BlockGet(uint32_t address)
{
	flash_block_address_t block_address;
	if(address>=0xFFFF0000)
	{
		block_address = address & 0xFFFFE000;

	}
	else if(address<0xFFFF0000 && address >FLASH_CF_BLOCK_INVALID)
	{
		block_address = address & 0xFFFF8000;
	}
	else if(address<= FLASH_CF_BLOCK_INVALID)
	{
		block_address = (((address&0xFFFF)/FLASH_DF_BLOCK_SIZE)*FLASH_DF_BLOCK_SIZE)|0x00100000;
	}

	return (block_address);
}

static uint8_t R_FLASH_BlockNumGet(uint32_t address,uint32_t size)
{
	uint8_t	block_number;

	if(address>=0xFFFF0000)
	{
		block_number = ((size-1)/0x2000)+1;
	}
	else if(address<0xFFFF0000 && address >FLASH_CF_BLOCK_INVALID)
	{
		block_number = ((size-1)/0x8000)+1;
	}
	else if(address<= FLASH_CF_BLOCK_INVALID)
	{
		block_number =(size-1)/FLASH_DF_BLOCK_SIZE+1 ;
	}

	return (block_number);

}


static void R_FLASH_COPY(uint32_t address,uint32_t size,uint8_t *buf)
{
	uint16_t i;
    uint8_t *src = (uint8_t *)(address);
    uint8_t *dst = ((uint8_t *)buf);

    for (i = 0; i <size; i++) {
        *(dst++) = *(src++);
    }
}
