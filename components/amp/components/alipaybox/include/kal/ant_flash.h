/**
 * ant_flash.h
 *
 * NOR/SPI FLASH接口
 */
#ifndef __ANT_FLASH_H__
#define __ANT_FLASH_H__
#include "ant_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif
    
/**
 * 向spi flash地址写入数据
 * 
 * @param addr spiflash地址
 * @param data 数据缓存指针
 * @param len 数据缓存长度
 * 
 * @return  0： 写入成功
 *         -1： 写入失败
 */
ant_s32 ant_flash_write(ant_u32 addr, ant_u8 *data, ant_u32 len);

/**
 * 从spi flash地址读取数据
 * 
 * @param addr spiflash地址
 * @param data 数据缓存指针
 * @param len 数据缓存长度
 * 
 * @return  0： 读取成功
 *         -1： 读取失败
 */
ant_s32 ant_flash_read (ant_u32 addr, ant_u8 *data, ant_u32 len);

/**
 * 擦除spi flash块
 * 
 * @param block_index spiflash块序号
 * 
 * @return  0： 擦除成功
 *         -1： 擦除失败
 */
ant_s32 ant_flash_erase_block(ant_u32 block_index);

/**
 * 擦除spi flash用户区域
 * 
 * @return  0： 擦除成功
 *         -1： 擦除失败
 */
ant_s32 ant_flash_erase(void);

/**
 * 获取spi flash地址对应的块序号
 * 
 * @param addr spiflash地址
 * 
 * @return  对应 @addr 属于的spiflash块序号
 */
ant_u32 ant_flash_get_block_index(ant_u32 addr);

/**
 * 获取spi flash用户区域的起始地址
 * 
 * @return  0： 擦除成功
 *         -1： 擦除失败
 */
ant_u32 ant_flash_get_start_addr(void);

ant_u32 ant_flash_get_total_size(void);

   
#ifdef __cplusplus
} // extern "C"
#endif

#endif

