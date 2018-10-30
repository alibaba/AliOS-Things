#include "gd32f30x_fmc_ext.h"
#include <stdio.h>
#include <string.h>

#define FLASH_OPER_RESERVE_ORIGINAL_DATA        1
#define FMC_PAGE_SIZE                           0x800U         /*!< main flash page size */
#define FMC_BASE_ADDRESS                        0x08000000U     /*!< main flash start address*/
#define FMC_END_ADDRESS                         0x08040000U     /*!< main flash max end address*/
#define FMC_PAGE_SIZE                           0x800U         /*!< main flash page size */
#define FMC_SECTOR_SIZE                         0x1000U        /*!< main flash sector size */

#define XXX_DEBUGE      printf
/* define array if it is necessary to reserve original data located in out of targeted scope */
#if (1 == FLASH_OPER_RESERVE_ORIGINAL_DATA)
static uint8_t _g_fmc_cache[FMC_PAGE_SIZE];
#endif

/* reserve original data located in out of targeted scope */
static void _fmc_page_erase_update(uint8_t *p_buffer, uint32_t size, uint32_t page_addr, \
                             uint32_t page_offset);
static void _fmc_program_align_word(uint32_t dst_addr, uint8_t *data, uint32_t size);

/*!
    \brief      read flash target region
    \param[in]  start_addr: target region start address 
    \param[in]  data: pointer to read result
    \param[in]  size: target region size
    \param[out] none
    \retval     none
*/
void xxx_fmc_region_read(uint32_t start_addr, uint8_t *data, uint32_t size)
{
    uint32_t counter;
    /* check the parameters */
    if((start_addr < FMC_BASE_ADDRESS)||(start_addr > FMC_END_ADDRESS)){
        XXX_DEBUGE("parameter [start_addr] is a invalid address.");
    }
    if(size > (FMC_END_ADDRESS - start_addr)){
        XXX_DEBUGE("target data size exceed flash atual address scope.");
    }
    /* assign flash specific data to pointer */
    for(counter = 0U; counter < size; counter++){
        *(data + counter) = REG8(start_addr + counter);
    }
}

/*!
    \brief      write flash target region with amounts of data
    \param[in]  start_addr: target region start address 
    \param[in]  size: target region size
    \param[out] none
    \retval     ErrStatus
*/
ErrStatus xxx_fmc_region_write(uint32_t start_addr, uint8_t *data, uint32_t size)
{
    uint32_t counter = 0U;
    uint32_t remaining = size;
    uint32_t update_offset = 0U;
    uint8_t *src_addr = data;
    uint32_t page_offset = 0U;
    uint32_t write_addr_limit = ((start_addr + size) / FMC_PAGE_SIZE + 1U) * FMC_PAGE_SIZE;
    uint16_t write_buffer_size;
    uint32_t page_addr;
    uint8_t  non_ff_flag = 0U;
    uint32_t length, left_align,right_align;
    uint8_t right_align_complement = 0U;
    uint32_t addr_size_temp = (start_addr / FMC_PAGE_SIZE)* FMC_PAGE_SIZE ;
    uint8_t single_page_flag; 
    page_offset = start_addr - (start_addr / FMC_PAGE_SIZE) * FMC_PAGE_SIZE;
    
    /* check whether all data is at the same page */
    if((write_addr_limit - addr_size_temp) > FMC_PAGE_SIZE){
        single_page_flag = 0U;
    }else{
        single_page_flag = 1U;
    }
    left_align = page_offset - page_offset % 4U;
    
    if((page_offset + size) % 4U){
        right_align_complement = 4U - (page_offset + size) % 4U;
    }else{
        right_align_complement = 0U;
    }
    right_align = (page_offset + size + right_align_complement) % FMC_PAGE_SIZE;
    if(0 == right_align){
        right_align = FMC_PAGE_SIZE;
    }
    /* check the parameters */
    if((start_addr < FMC_BASE_ADDRESS)||(start_addr > FMC_END_ADDRESS)){
        XXX_DEBUGE("parameter [start_addr] is a invalid address.");
        return ERROR;
    }
    if(NULL == data){
        XXX_DEBUGE("parameter [data] is a NULL pointer.");
        return ERROR;
    }
    if(size > (FMC_END_ADDRESS - start_addr)){
        XXX_DEBUGE("target data size exceed flash atual address scope.");
        return ERROR;
    }
    /* save the original data , erase responding page */
    /* save the original data in the processing of writing data */
#if (1 == FLASH_OPER_RESERVE_ORIGINAL_DATA)
    do{
        for(counter = 0; counter < FMC_PAGE_SIZE; counter++){
            _g_fmc_cache[counter] = 0xFFU;
        }
        page_addr = (start_addr / FMC_PAGE_SIZE) * FMC_PAGE_SIZE;
        page_offset = start_addr - page_addr;
 
        /* get the currently transmitting data length */
        if(page_offset + remaining < FMC_PAGE_SIZE){
            length = remaining;
        }else{
            length = FMC_PAGE_SIZE - page_offset;
        }
        
        if(1U == single_page_flag){
            /* process the condition that all data is at the same page */
            update_offset = left_align;
            write_buffer_size = right_align - left_align;
            /* check whether target region content is 0xFF */
            for(counter = left_align; counter < right_align; counter++){
                if(0xFF != REG8(page_addr + counter)){
                    non_ff_flag = 1U;
                    break;
                }
            }
            /* backup the orignal data and erase the page */
            if(1U == non_ff_flag){
                for(counter = 0; counter < left_align; counter++){
                    if(0xFF != REG8(page_addr + counter)){
                        update_offset = counter;
                        write_buffer_size = right_align - counter;
                        break;
                    }
                }

                for(counter = FMC_PAGE_SIZE - 1U; counter >= right_align; counter--){
                    if(0xFF != REG8(page_addr + counter)){
                        write_buffer_size = counter - update_offset + 1U;
                        break;
                    }
                }
                memcpy((uint8_t *)(_g_fmc_cache + update_offset), (uint8_t *)(page_addr + update_offset), write_buffer_size);
                fmc_page_erase(page_addr);
                non_ff_flag = 0U;
            }
        }else{
            /* process the condition that all data cover more than one page */
            if(page_addr == (write_addr_limit - FMC_PAGE_SIZE)){
                /* process the data in last page */
                update_offset = page_offset;  
                write_buffer_size = right_align;
                /* check whether target region content is 0xFF */
                for(counter = 0; counter < right_align; counter++){
                    if(0xFF != REG8(page_addr + counter)){
                        non_ff_flag = 1U;
                        break;
                    }
                } 
                /* backup the orignal data and erase the page */
                if(1U == non_ff_flag){
                    for(counter = FMC_PAGE_SIZE - 1U; counter >= right_align; counter--){
                        if(0xFF != REG8(page_addr + counter)){
                            write_buffer_size = counter + 1U;
                            break;
                        }
                    }
                    /* load from the flash into the cache */
                    memcpy((uint8_t *)(_g_fmc_cache), (uint8_t *)(page_addr), write_buffer_size);
                    /* erase the page, and write the cache */
                    fmc_page_erase(page_addr);
                    non_ff_flag = 0U;
                }
            }else{
                /* process the data in middle pages */
                if(page_addr == start_addr){
                    update_offset = 0U;
                    write_buffer_size = FMC_PAGE_SIZE;
                    /* check whether target region content is 0xFF */
                    for(counter = 0U; counter <  FMC_PAGE_SIZE; counter++){
                        if(0xFF != REG8(page_addr + counter)){
                            non_ff_flag = 1U;
                            break;
                        }
                    }
                    /* erase the page if it is necessary */
                    if(1U == non_ff_flag){
                        fmc_page_erase(page_addr);
                        non_ff_flag = 0U;
                    }
                }else{ 
                    update_offset = left_align;
                    write_buffer_size = FMC_PAGE_SIZE - update_offset;
                    /* process the data in first page */
                    /* check whether target region content is 0xFF */
                    for(counter = left_align; counter < FMC_PAGE_SIZE; counter++){
                        if(0xFF != REG8(page_addr + counter)){
                            non_ff_flag = 1U;
                            break;
                        }
                    }  
                    /* backup the orignal data and erase the page */
                    if(1U == non_ff_flag){
                        for(counter = 0; counter < left_align; counter++){
                            if(0xFF != REG8(page_addr + counter)){
                                update_offset = counter;
                                write_buffer_size = FMC_PAGE_SIZE - update_offset;
                                break;
                            }
                        }
                        /* load from the flash into the cache */
                        memcpy((uint8_t *)(_g_fmc_cache + update_offset), (uint8_t *)(page_addr + update_offset), \
                                           write_buffer_size);
                        /* erase the page, and write the cache */
                        fmc_page_erase(page_addr);
                        non_ff_flag = 0U;
                    }
                }
            }
        }

        /* update the cache from the source */
        memcpy((uint8_t *)_g_fmc_cache + page_offset, src_addr, length);

        _fmc_program_align_word(page_addr + update_offset , (uint8_t *)(_g_fmc_cache + update_offset), write_buffer_size);
        start_addr += length;
        src_addr += length;
        remaining -= length;

    }while(remaining > 0);
#else
    while(counter < size){   
        if(0xffU == (REG8(start_addr + counter))){
            counter++;
        }else{
            /* erase the pgae unless all the original data is 0xFF */
            page_addr = ((start_addr + counter)/FMC_PAGE_SIZE) * FMC_PAGE_SIZE;
            fmc_page_erase(page_addr);
            counter++;
        }
    }
    /* save the original data which is outside the writing target range */
    _fmc_program_align_word(start_addr, data, size);
#endif /* save the original data which is outside the writing target range */
    return SUCCESS;
}

/*!
    \brief      erase flash target region
    \param[in]  start_addr: target region start address 
    \param[in]  size: target region size
    \param[out] none
    \retval     ErrStatus
*/
ErrStatus xxx_fmc_region_erase(uint32_t start_addr, uint32_t size)
{
    uint32_t page_offset, page_number, counter = 0U;
    uint32_t page_addr;
    /* check the parameters */
    if((start_addr < FMC_BASE_ADDRESS)||(start_addr > FMC_END_ADDRESS)){
        XXX_DEBUGE("parameter [start_addr] is a invalid address.");
        return ERROR;
    }
    if(size > (FMC_END_ADDRESS - start_addr)){
        XXX_DEBUGE("target data size exceed flash atual address scope.");
        return ERROR;
    }

    page_offset = start_addr % FMC_PAGE_SIZE;
    if((size + page_offset) % FMC_PAGE_SIZE ) {
        page_number =(size + page_offset) / FMC_PAGE_SIZE + 1U;
    }else{
        page_number =(size + page_offset) / FMC_PAGE_SIZE;
    }
    page_addr = start_addr - page_offset;
#if (1 == FLASH_OPER_RESERVE_ORIGINAL_DATA)
    /* page nubmer which buffer refer is equal to one */
    if(1U == page_number){
        _fmc_page_erase_update(_g_fmc_cache, size, page_addr, page_offset);
    }else if(2U == page_number){
        /* page nubmer which buffer refer is equal to two */
        _fmc_page_erase_update(_g_fmc_cache, FMC_PAGE_SIZE - page_offset, \
                         page_addr, page_offset);

        _fmc_page_erase_update(_g_fmc_cache, page_offset + size - FMC_PAGE_SIZE, \
                         page_addr + FMC_PAGE_SIZE, 0);

    }else{
        /* page nubmer which buffer refer is more than two */
        _fmc_page_erase_update(_g_fmc_cache, FMC_PAGE_SIZE - page_offset, \
                         page_addr, page_offset);

        /* erase middle pages */
        for (counter = 1; counter < (page_number - 1U); counter++){
            fmc_page_erase(page_addr + (FMC_PAGE_SIZE * counter));
        }
        counter = (page_offset + size) % FMC_PAGE_SIZE;
        if(0U == counter){
            counter = FMC_PAGE_SIZE;
        }
        _fmc_page_erase_update(_g_fmc_cache, counter, \
                        page_addr + (FMC_PAGE_SIZE * (page_number - 1U)), 0);
    }
#else
    /* erase all the pages which buffer refer */
    for(counter =0U; counter < page_number; counter++){
        fmc_page_erase(page_addr + (counter * FMC_PAGE_SIZE));
    }
#endif /* save the original data which is outside the erasing target range */
    return SUCCESS;
}

/*!
    \brief      write target data aligned in word
    \param[in]  p_buffer: pointer to data buffer
    \param[in]  size: target region size
    \param[in]  page_addr: page head address
    \param[in]  page_offset: the offset with respect to page head address
    \param[out] none
    \retval     none
*/
static void _fmc_page_erase_update(uint8_t *p_buffer, uint32_t size, uint32_t page_addr, \
                             uint32_t page_offset)
{
    uint32_t temp;
    uint32_t counter;
    uint32_t update_index;
    
    temp = (uint32_t)p_buffer;

    for(counter = 0U; counter < FMC_PAGE_SIZE; counter += 4){
        if(0xFFFFFFFF != REG32(page_addr + counter)){
            update_index = counter;
            break;
        }
    }
    
    if(FMC_PAGE_SIZE != counter){
        for(counter = update_index; counter < FMC_PAGE_SIZE; counter += 4){
            REG32(temp + counter) = REG32(page_addr + counter);
        }

        /* configure the target region to 0xFF */
        for(counter = 0U;counter < size; counter++){
            p_buffer[page_offset + counter] = 0xFFU;
        }
        
        fmc_page_erase(page_addr);
        
        /* write the orignal data to address outside the target range */
        temp = (uint32_t)p_buffer;
        for(counter = update_index; counter < FMC_PAGE_SIZE; counter += 4){
            fmc_word_program(page_addr + counter, REG32(temp + counter));
        }
    }
}

/*!
    \brief      write target data aligned in word
    \param[in]  start_addr: target region start address 
    \param[in]  data: pointer to target data
    \param[in]  data_size: target region size
    \param[out] none
    \retval     none
*/
static void _fmc_program_align_word(uint32_t dst_addr, uint8_t *data, uint32_t data_size)
{
    uint32_t head_offset, tail_offset, data_offset;
    uint32_t data_addr_align, data_size_align;
    uint32_t counter;
    uint32_t temp_addr;
    uint8_t align_buffer[4] = {0xFF, 0xFF, 0xFF, 0xFF};
    
    head_offset = dst_addr % 4U;
    tail_offset = (data_size + head_offset) % 4U;
    
    if(0 != head_offset){
        /* write the target region if front address is non-aligned */
        data_addr_align = dst_addr + (4 - head_offset);
        data_size_align = data_size - (4 - head_offset);
        
        temp_addr = (uint32_t)data;
        for(counter = head_offset; counter < 4; counter++){
            align_buffer[counter] = REG8(temp_addr);
            temp_addr++;
        }
        data_offset = 4 - head_offset;
        fmc_word_program(dst_addr - head_offset, REG32(align_buffer));
    }else{
        /* write the target region aligned address */
        data_addr_align = dst_addr;
        data_size_align = data_size;
        data_offset = 0;
    }
    /* write the target region if back address is non-aligned */
    if(0 != tail_offset){
        data_size_align = data_size - tail_offset;
        
        temp_addr = (uint32_t)data;
        temp_addr = temp_addr + data_size - tail_offset;
        for(counter = 0; counter < tail_offset; counter++){
            align_buffer[counter] = REG8(temp_addr);
            temp_addr++;
        }
        
        fmc_word_program(dst_addr + data_size - tail_offset, REG32(align_buffer));
    }
    /* write the target region middle aligned address */
    temp_addr = (uint32_t)data + data_offset;
    for(counter = 0; counter < data_size_align; counter += 4){
        fmc_word_program(data_addr_align + counter, REG32(temp_addr + counter));
    }
}
