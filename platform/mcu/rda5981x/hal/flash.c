#include <string.h>

#include "aos/hal/flash.h"

#include "flash_api.h"
#include "flash_data.h"
#include "aos/kernel.h"
#include "board.h"
#include "rda_flash.h"

#define PARTITION_TABLE_END_ADDR 0x18001000

static flash_t flash_obj;
static uint8_t flash_inited = 0;
static aos_mutex_t *flash_mutex;

#if 0
static const hal_logic_partition_t hal_partitions[] = {
    [HAL_PARTITION_APPLICATION] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "Application",
        .partition_start_addr       = 0x18000000,
        .partition_length           = 0x400000,    //4MB
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
};
#else
extern const hal_logic_partition_t hal_partitions[];
#endif

static inline uint8_t is_aligned(uint32_t number, uint32_t alignment)
{
    //printf("is_aligned number 0x%x,  alignment 0x%x \r\n", number, alignment);
    if ((number % alignment) != 0) {
        return 0;
    } else {
        return 1;
    }
}

uint8_t is_aligned_to_sector(uint32_t addr, uint32_t size)
{
    uint32_t current_sector_size = flash_get_sector_size(&flash_obj, addr);
    if (!is_aligned(size, current_sector_size) ||
        !is_aligned(addr, current_sector_size)) {
        return 0;
    } else {
        return 1;
    }
}

uint32_t get_page_size()
{
    if (0 == flash_inited) {
        flash_init(&flash_obj);
        aos_mutex_new(flash_mutex);
        flash_inited = 1;
    }
    
    return flash_get_page_size(&flash_obj);
}

uint32_t get_sector_size(uint32_t addr)
{
    if (0 == flash_inited) {
        flash_init(&flash_obj);
        aos_mutex_new(flash_mutex);
        flash_inited = 1;
    }

    return flash_get_sector_size(&flash_obj, addr);
}

uint32_t get_flash_start()
{
    if (0 == flash_inited) {
        flash_init(&flash_obj);
        aos_mutex_new(flash_mutex);
        flash_inited = 1;
    }
    
    return flash_get_start_address(&flash_obj);
}

uint32_t get_flash_size()
{
    if (0 == flash_inited) {
        flash_init(&flash_obj);
        aos_mutex_new(flash_mutex);
        flash_inited = 1;
    }
    
    return flash_get_size(&flash_obj);
}

hal_logic_partition_t *hal_flash_get_info(hal_partition_t in_partition)
{
    hal_logic_partition_t *logic_partition;

   // if (in_partition != HAL_PARTITION_APPLICATION)
     //   return NULL;
    
    logic_partition = (hal_logic_partition_t *)&hal_partitions[in_partition];

    return logic_partition;
}

int32_t hal_flash_erase(hal_partition_t in_partition, uint32_t off_set, uint32_t size)
{
    hal_logic_partition_t *partition_info;
    uint32_t start_address, end_addr;

    if (0 == flash_inited) {
        flash_init(&flash_obj);
        aos_mutex_new(flash_mutex);
        flash_inited = 1;
    }

    partition_info = hal_flash_get_info( in_partition );
    start_address = partition_info->partition_start_addr + off_set;
#if 0
    //uint32_t start_address = flash_get_start_address(&flash_obj);
    uint32_t current_sector_size = flash_get_sector_size(&flash_obj, off_set + start_address);
    if ((off_set + start_address < PARTITION_TABLE_END_ADDR) || 
        (off_set + size > flash_get_size(&flash_obj))) {
        printf("size err \r\n");
        return -1;
    }

    if (!is_aligned_to_sector(off_set + start_address, size)) {
        printf("is_aligned_to_sector err \r\n");
        return -1;
    }

    int32_t ret = 0;
    aos_mutex_lock(flash_mutex, 0xFFFFFFFF);
    while (size) {
        ret = flash_erase_sector(&flash_obj, off_set + start_address);
        //printf("flash erase sector ret %d \r\n", ret);
        if (ret != 0) {
            ret = -1;
            break;
        }
        size -= current_sector_size;
        off_set += current_sector_size;
    }
    aos_mutex_unlock(flash_mutex);
    return ret;
#else
    //printf("hal_flash_erase start_address is 0x%x, size is %d \r\n", start_address, size);
    return rda5981_erase_flash(start_address, size);
#endif
}

int flash_write(unsigned int addr, char *buf, unsigned int len)
{
    int ret;
    int left;
    unsigned int addr_t, len_t;
    char *temp_buf = NULL;
    addr_t = addr & 0xffffff00;
    len_t = addr - addr_t + len;
    if(len_t%256)
            len_t += 256-len_t%256;
    //printf("addr %x addr_t %x\r\n", addr, addr_t);
    //printf("len %d len_t %d\r\n", len, len_t);
    temp_buf = (char*)malloc(256);
    if(temp_buf == NULL)
        return -1;

    ret = rda5981_read_flash(addr_t, temp_buf, 256);
    if(ret){
        free(temp_buf);
        return -1;
    }
    left = 256 - (addr - addr_t);
    if (len < left)
        memcpy(temp_buf+addr-addr_t, buf, len);//256-(addr-addr_t));
    else
        memcpy(temp_buf+addr-addr_t, buf, left);
    ret = rda5981_write_flash(addr_t, temp_buf, 256);
    if(ret){
        free(temp_buf);
        return -1;
    }

    len_t -= 256;
    buf += 256-(addr-addr_t);
    len -= 256-(addr-addr_t);
    addr_t += 256;
    
    while(len_t != 0){
        //printf("len_t %d buf %x len %d addr_t %x\r\n", len_t, buf, len, addr_t);
        if(len >= 256){
           memcpy(temp_buf, buf, 256);
        }else{
            ret = rda5981_read_flash(addr_t, temp_buf, 256);
            if(ret){
                free(temp_buf);
                return -1;
            }
           memcpy(temp_buf, buf, len); 
        }
        ret = rda5981_write_flash(addr_t, temp_buf, 256);
        if(ret){
            free(temp_buf);
            return -1;
        }
        len_t -= 256;
        buf += 256;
        len -= 256;
        addr_t += 256;
    }
    free(temp_buf);
    return 0;
}


int32_t hal_flash_write(hal_partition_t in_partition, uint32_t *off_set,
                        const void *in_buf, uint32_t in_buf_len)
{
    int32_t ret = 0;
    hal_logic_partition_t *partition_info;

    uint32_t page_size = get_page_size();
    //uint32_t start_address = flash_get_start_address(&flash_obj);
    partition_info = hal_flash_get_info( in_partition );
    uint32_t start_address = partition_info->partition_start_addr + *off_set;
    uint32_t partition_end = partition_info->partition_start_addr + partition_info->partition_length;
    if(start_address >= partition_end){
        printf("flash over write\r\n");
        return -1;
    }
    if((start_address + in_buf_len) > partition_end){
        in_buf_len = partition_end - start_address;
        printf("flash over write, new len is %d\r\n", in_buf_len);
    }
#if 0
    uint32_t current_sector_size = flash_get_sector_size(&flash_obj, (uint32_t)off_set + start_address);

    if (((uint32_t)*off_set + start_address < PARTITION_TABLE_END_ADDR) ||
        ((uint32_t)*off_set + in_buf_len > flash_get_size(&flash_obj))){
        return -1;
    }

    if (0 == is_aligned((uint32_t)*off_set + start_address, page_size)) {
        printf("hal_flash_write not aligned\r\n");
        return -1;
    }

    int32_t ret = 0;
    aos_mutex_lock(flash_mutex, 0xFFFFFFFF);
    if (flash_program_page(&flash_obj, (uint32_t)*off_set + start_address, (const uint8_t *)in_buf, in_buf_len)) {
        ret = -1;
    }
    aos_mutex_unlock(flash_mutex);
    return ret;
#else
    if ((start_address < PARTITION_TABLE_END_ADDR) ||
        ((uint32_t)*off_set + in_buf_len > flash_get_size(&flash_obj))){
        return -1;
    }
    ret = flash_write(start_address, (const uint8_t *)in_buf, in_buf_len);
    *off_set += in_buf_len;
    return ret;
#endif
}

int32_t hal_flash_erase_write(hal_partition_t in_partition, uint32_t *off_set,
                              const void *in_buf, uint32_t in_buf_len)
{
    int32_t ret = 0;

    ret = hal_flash_erase(in_partition, (uint32_t)*off_set, in_buf_len);
    if (ret != 0)
        return -1;
    ret = hal_flash_write(in_partition, *off_set, in_buf, in_buf_len);
    if (ret != 0)
        return -1;

    return 0;
}

int32_t hal_flash_read(hal_partition_t in_partition, uint32_t *off_set,
                       void *out_buf, uint32_t in_buf_len)
{
    hal_logic_partition_t *partition_info;

    if (0 == flash_inited) {
        flash_init(&flash_obj);
        flash_inited = 1;
    }

    partition_info = hal_flash_get_info( in_partition );
    uint32_t start_address = partition_info->partition_start_addr + *off_set;

    if ((start_address < PARTITION_TABLE_END_ADDR) ||
        ((uint32_t)*off_set + in_buf_len > flash_get_size(&flash_obj))){
        return -1;
    }

    *off_set += in_buf_len;

    return rda5981_read_flash(start_address, (const uint8_t *)out_buf, in_buf_len);
    #if 0
    //uint32_t start_address = flash_get_start_address(&flash_obj);
    if (((uint32_t)*off_set + start_address < PARTITION_TABLE_END_ADDR) ||
        ((uint32_t)*off_set + in_buf_len > flash_get_size(&flash_obj))){
        return -1;
    }
    aos_mutex_lock(flash_mutex, 0xFFFFFFFF);
    memcpy(out_buf, (const void *)((uint32_t)*off_set + start_address), in_buf_len);
    aos_mutex_unlock(flash_mutex);
    #endif
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

