/*
    Copyright (C) 2017 GigaDevice

    2017-06-28, V1.0.0, demo for GD32F3x0
*/

#include <stdio.h>
#include <string.h>

#include "aos/kernel.h"
#include "aos/hal/flash.h"

#include "gd32f4xx_fmc.h"

/* base address of the FMC sectors */
#define ADDR_FMC_SECTOR_0     ((uint32_t)0x08000000) /*!< base address of sector 0, 16 kbytes */
#define ADDR_FMC_SECTOR_1     ((uint32_t)0x08004000) /*!< base address of sector 1, 16 kbytes */
#define ADDR_FMC_SECTOR_2     ((uint32_t)0x08008000) /*!< base address of sector 2, 16 kbytes */
#define ADDR_FMC_SECTOR_3     ((uint32_t)0x0800C000) /*!< base address of sector 3, 16 kbytes */
#define ADDR_FMC_SECTOR_4     ((uint32_t)0x08010000) /*!< base address of sector 4, 64 kbytes */
#define ADDR_FMC_SECTOR_5     ((uint32_t)0x08020000) /*!< base address of sector 5, 64 kbytes */
#define ADDR_FMC_SECTOR_6     ((uint32_t)0x08040000) /*!< base address of sector 6, 64 kbytes */
#define ADDR_FMC_SECTOR_7     ((uint32_t)0x08060000) /*!< base address of sector 7, 64 kbytes */
#define ADDR_FMC_SECTOR_8     ((uint32_t)0x08080000) /*!< base address of sector 8, 64 kbytes */
#define ADDR_FMC_SECTOR_9     ((uint32_t)0x080A0000) /*!< base address of sector 9, 64 kbytes */
#define ADDR_FMC_SECTOR_10    ((uint32_t)0x080C0000) /*!< base address of sector 10, 64 kbytes */
#define ADDR_FMC_SECTOR_11    ((uint32_t)0x080E0000) /*!< base address of sector 11, 64 kbytes */

#define ROUND_DOWN(a,b) (((a) / (b)) * (b))
#define FMC_CACHE_SIZE  0x2000

extern const hal_logic_partition_t hal_partitions[];

int32_t hal_flash_info_get(hal_partition_t pno, hal_logic_partition_t *partition)
{
    hal_logic_partition_t *logic_partition;

    logic_partition = (hal_logic_partition_t *)&hal_partitions[ pno ];
    memcpy(partition, logic_partition, sizeof(hal_logic_partition_t));

    return 0;
}

static uint32_t dev_fmc_sector_get(uint32_t address)
{
    uint32_t sector = 0;
    
    if((address < ADDR_FMC_SECTOR_1) && (address >= ADDR_FMC_SECTOR_0)){
        sector = CTL_SECTOR_NUMBER_0;
    }else if((address < ADDR_FMC_SECTOR_2) && (address >= ADDR_FMC_SECTOR_1)){
        sector = CTL_SECTOR_NUMBER_1;
    }else if((address < ADDR_FMC_SECTOR_3) && (address >= ADDR_FMC_SECTOR_2)){
        sector = CTL_SECTOR_NUMBER_2;
    }else if((address < ADDR_FMC_SECTOR_4) && (address >= ADDR_FMC_SECTOR_3)){
        sector = CTL_SECTOR_NUMBER_3;  
    }else if((address < ADDR_FMC_SECTOR_5) && (address >= ADDR_FMC_SECTOR_4)){
        sector = CTL_SECTOR_NUMBER_4;
    }else if((address < ADDR_FMC_SECTOR_6) && (address >= ADDR_FMC_SECTOR_5)){
        sector = CTL_SECTOR_NUMBER_5;
    }else if((address < ADDR_FMC_SECTOR_7) && (address >= ADDR_FMC_SECTOR_6)){
        sector = CTL_SECTOR_NUMBER_6;
    }else if((address < ADDR_FMC_SECTOR_8) && (address >= ADDR_FMC_SECTOR_7)){
        sector = CTL_SECTOR_NUMBER_7;  
    }else if((address < ADDR_FMC_SECTOR_9) && (address >= ADDR_FMC_SECTOR_8)){
        sector = CTL_SECTOR_NUMBER_8;
    }else if((address < ADDR_FMC_SECTOR_10) && (address >= ADDR_FMC_SECTOR_9)){
        sector = CTL_SECTOR_NUMBER_9;
    }else if((address < ADDR_FMC_SECTOR_11) && (address >= ADDR_FMC_SECTOR_10)){
        sector = CTL_SECTOR_NUMBER_10;
    }else if(address >= ADDR_FMC_SECTOR_11){
        sector = CTL_SECTOR_NUMBER_11;
    }else{
        while(1);
    }
    
    return sector;
}

static int dev_fmc_write(uint32_t start_addr, uint8_t* buf ,uint32_t buf_size)
{
    int i;
    uint8_t *p_data;
    uint32_t address;
    
    p_data = buf;
    fmc_unlock();
    /* clear pending flags */
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR | FMC_FLAG_PGMERR | FMC_FLAG_PGSERR);
    address = start_addr;
    /* write data_8 to the corresponding address */
    for(i=0; i < buf_size; i++){
        if(FMC_READY == fmc_byte_program(address, buf[i])){
            address++;
//            fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR | FMC_FLAG_PGMERR | FMC_FLAG_PGSERR);
        }else{ 
            while(1);
        }
    }

    /* lock the flash program erase controller */
    fmc_lock();
    /* Memory check */
    for (i = 0; i < buf_size; i++) {
        uint8_t *dst = (uint8_t *)(start_addr + i);
        uint8_t *src = (p_data + i);

        if ( *dst != *src ) {
            printf("Write failed @0x%08lx, read value=0x%08lx, expected=0x%08lx\n", (uint32_t) dst, *dst, *src);
            return -1;
        }
    }

    return 0;
}

static void fmc_erase_sector(uint32_t fmc_sector)
{
    /* unlock the flash program erase controller */
    fmc_unlock(); 
    /* clear pending flags */
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR | FMC_FLAG_PGMERR | FMC_FLAG_PGSERR);
    /* wait the erase operation complete*/
    if(FMC_READY != fmc_sector_erase(fmc_sector)){
        while(1);
    }
    /* lock the flash program erase controller */
    fmc_lock();
}

static ErrStatus dev_fmc_bank0_update(uint32_t dst_addr, void *data, uint32_t size)
{
    int reval;
    uint32_t sector;
    uint8_t *p_cache;
    int fl_offset;
    
    if((dst_addr < 0x08082000) || (dst_addr > 0x08084000)){
        while(1);
    }
    
    if((dst_addr + size) > 0x08084000)
    {
        while(1);
    }
    
    if((0xFFFFFFFF == *(uint32_t*)dst_addr) || (0xFFFFFFFF == *(uint32_t*)(dst_addr + 4))){
        reval = dev_fmc_write(dst_addr, data, size);
        if(reval != 0) {
            printf("Error writing %lu bytes at 0x%08lx\n", size, dst_addr);
            while(1);
        }
    }else{
        p_cache = (uint8_t*)aos_malloc(FMC_CACHE_SIZE);
        if(p_cache == NULL){
            while(1);
        }
        
        if(size < FMC_CACHE_SIZE){
            fl_offset = dst_addr - 0x08082000;
            /* Load from the flash into the cache */
            memcpy(p_cache, (uint8_t *)0x08082000, FMC_CACHE_SIZE);
            /* Update the cache from the source */
            memcpy(p_cache + fl_offset, (uint8_t *) data, size);
            sector = dev_fmc_sector_get(dst_addr);
            fmc_erase_sector(sector);
            reval = dev_fmc_write(0x08082000, p_cache, FMC_CACHE_SIZE);
            if(reval != 0) {
                printf("Error writing %lu bytes at 0x%08lx\n", FMC_CACHE_SIZE, dst_addr);
                while(1);
            }
        }else{
            while(1);
        }
        
        aos_free(p_cache);
    }
    
    return SUCCESS;
}

static void dev_fmc_read(uint32_t address, uint32_t *pData, uint32_t len_bytes)
{
    int i;
    uint32_t *src = (uint32_t *)(address);
    uint32_t *dst = ((uint32_t *) pData);

    for (i = 0; i < len_bytes; i += 4) {
        *(dst++) = *(src++);
    }
}

void dev_fmc_bank0_erase(uint32_t address, uint32_t len_bytes)
{
    uint32_t start_sector;
    uint32_t end_sector;
    uint32_t addr_offset;
    uint8_t *p_cache;
    int reval;
    
    if((address < 0x08082000) || (address > 0x08084000)){
        while(1);
    }
//    printf("===============Flash erase =============\r\n start address: %08x \r\n", address);
    start_sector = dev_fmc_sector_get(address);
    end_sector = dev_fmc_sector_get(address + len_bytes - 1);

    if(start_sector != end_sector){
        while(1);
    }
    
    p_cache = (uint8_t*)aos_malloc(FMC_CACHE_SIZE);
    if(p_cache == NULL){
        while(1);
    }
    
    memcpy(p_cache, (uint8_t *)0x08082000, FMC_CACHE_SIZE);
    
    fmc_erase_sector(start_sector);
    addr_offset = address - 0x08082000;
    if(0 == addr_offset){
        reval = dev_fmc_write(address + len_bytes, p_cache + len_bytes, FMC_CACHE_SIZE - len_bytes);
    }else{
        reval = dev_fmc_write(0x08082000, p_cache, addr_offset);
        reval = dev_fmc_write(address + len_bytes, p_cache + addr_offset + len_bytes, FMC_CACHE_SIZE - len_bytes - addr_offset);
    }
    
    if(reval != 0) {
        printf("Error writing %lu bytes at 0x%08lx\n", FMC_CACHE_SIZE, address + len_bytes);
        while(1);
    }
    
    aos_free(p_cache);
}

int32_t hal_flash_write(hal_partition_t pno, uint32_t* poff, const void* buf ,uint32_t buf_size)
{
    ErrStatus reval;
    uint32_t start_addr;
    hal_logic_partition_t info;
    hal_logic_partition_t *partition_info = &info;

    if (hal_flash_info_get(pno, partition_info) != 0) {
        return -1;
    }

    start_addr = partition_info->partition_start_addr + *poff;

    reval = dev_fmc_bank0_update(start_addr, (void*)buf, buf_size);

    if (SUCCESS != reval) {
        printf("HAL FLASH update failed! \r\n");
        return -1;
    }

    *poff += buf_size;

    return 0;
}

int32_t hal_flash_read(hal_partition_t pno, uint32_t* poff, void* buf, uint32_t buf_size)
{
    uint32_t start_addr;
    hal_logic_partition_t info;
    hal_logic_partition_t *partition_info = &info;

    if (hal_flash_info_get(pno, partition_info) != 0) {
        return -1;
    }

    if(poff == NULL || buf == NULL || *poff + buf_size > partition_info->partition_length)
        return -1;
    start_addr = partition_info->partition_start_addr + *poff;
    dev_fmc_read(start_addr, buf, buf_size);
    *poff += buf_size;

    return 0;
}

int32_t hal_flash_erase(hal_partition_t pno, uint32_t off_set,
                        uint32_t size)
{
    uint32_t start_addr;
    hal_logic_partition_t info;
    hal_logic_partition_t *partition_info = &info;

    if (hal_flash_info_get(pno, partition_info) != 0) {
        return -1;
    }

    if(size + off_set > partition_info->partition_length)
        return -1;

    start_addr = partition_info->partition_start_addr + off_set;

    dev_fmc_bank0_erase(start_addr, size);

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

