#ifndef __LSD_SPI_FLASH_H
#define __LSD_SPI_FLASH_H




		#include "hal_pinmux_define.h"
		#include "hal_gpio.h"
//		#include "type_def.h"
typedef char                s8;
typedef short               s16;
typedef int                 s32;

//typedef long long           INT64;
typedef unsigned char       u8;
typedef unsigned short      u16;
typedef unsigned int	      u32;

		



#define FLASH_PAGE_SIZE     256
#define FLASH_SECTOR_SIZE   4096
//#define FLASH_SECTOR_COUNT  512
#define FLASH_BLOCK_SIZE    65536
#define FLASH_SECTOR_COUNT  128     // 0~512Kä½œä¸ºæ–‡ä»¶ç³»ç»Ÿï¼Œ512K~1024Kä½œä¸ºæ™®é€šflashï¼Œ
#define FLASH_PAGES_PER_SECTOR  FLASH_SECTOR_SIZE/FLASH_PAGE_SIZE

////////////////////////////////////////////////////////////////////////////
//W25X16?¨¢D¡ä
#define FLASH_ID 0XEF14
//??¨¢?¡À¨ª
#define W25X_WriteEnable        0x06
#define W25X_WriteDisable       0x04
#define W25X_ReadStatusReg      0x05
#define W25X_WriteStatusReg     0x01
#define W25X_ReadData           0x03
#define W25X_FastReadData       0x0B
#define W25X_FastReadDual       0x3B
#define W25X_PageProgram        0x02
#define W25X_BlockErase         0xD8
#define W25X_SectorErase        0x20
#define W25X_ChipErase          0xC7
#define W25X_PowerDown          0xB9
#define W25X_ReleasePowerDown   0xAB
#define W25X_DeviceID           0xAB
#define W25X_ManufactDeviceID   0x90
#define W25X_JedecDeviceID      0x9F

uint8_t SPI_Flash_ReadID(uint8_t *id);
void W25X_Read_Sector(uint32_t nSector, uint8_t* pBuffer);	
void W25X_Write_Sector(uint32_t nSector, uint8_t* pBuffer);


// return 0 ok , -1 fail
//int lsdspi_take_lock(void);

// return 0 ok , -1 fail
//int lsdspi_give_lock(void);

/*
 * lsdspi_flash_init
 *
 * must call this function before call lsdfile_xxx api or lsdspi_xxx api 
 *
 * return NumByteToRead success, -1 fail
 */
void lsdspi_flash_init(void);

void lsdspi_flash_deinit(void);

// return 0 not ready, 1 ok
//char lsdspi_init_is_ok();

/*
 * lsdspi_flash_read
 *
 * return NumByteToRead success, -1 fail
 */
int lsdspi_flash_read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead);

/*
 * lsdspi_flash_write
 * no need to earse before write.
 *
 * return NumByteToWrite success, -1 fail
 */
//int lsdspi_flash_write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);

/*
 * lsdspi_flash_write
 * need to earse before write.
 *
 * return NumByteToWrite success, -1 fail
 */
//int lsdspi_flash_write_nocheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);

/*
 * lsdspi_flash_erase_sector
 * Dst_Addr: range 0~511 for w25x16
 *
 * cost 150ms to erase a sector.
 *
 * return 0 success, -1 fail
 */
//int lsdspi_flash_erase_sector(u32 Dst_Addr);

/**
 * lsdspi_flash_addr_write
 *
 * @param FlashAddress
 * @param Data
 * @param DataLength
 * @param inc   if FlashAddress increase by DataLength.
 *
 * @return int -1 error, DataLength success
 */
//int lsdspi_flash_addr_write(__IO uint32_t* FlashAddress, uint8_t* Data ,uint16_t DataLength, uint8_t inc);

   
#define SPIM_PIN_NUMBER_CS      HAL_GPIO_17
#define SPIM_PIN_FUNC_CS        HAL_GPIO_17_SPIMST_B_CS
#define SPIM_PIN_NUMBER_CLK     HAL_GPIO_16
#define SPIM_PIN_FUNC_CLK       HAL_GPIO_16_SPIMST_B_SCK
#define SPIM_PIN_NUMBER_SIO_0   HAL_GPIO_15
#define SPIM_PIN_FUNC_SIO_0     HAL_GPIO_15_SPIMST_B_SIO0
#define SPIM_PIN_NUMBER_SIO_1   HAL_GPIO_14
#define SPIM_PIN_FUNC_SIO_1     HAL_GPIO_14_SPIMST_B_SIO1
#define SPIM_PIN_NUMBER_SIO_2   HAL_GPIO_13
#define SPIM_PIN_FUNC_SIO_2     HAL_GPIO_13_SPIMST_B_SIO2
#define SPIM_PIN_NUMBER_SIO_3   HAL_GPIO_12
#define SPIM_PIN_FUNC_SIO_3     HAL_GPIO_12_SPIMST_B_SIO3
   
//#define USE_SPI_NSS_HARD
#ifdef USE_SPI_NSS_HARD
    #define Select_Flash()
    #define NotSelect_Flash()
#else
#if defined(STM32F2XX)
    /* Select SPI FLASH: ChipSelect pin low  */
    #define Select_Flash()     GPIO_ResetBits(GPIOB, GPIO_Pin_12)
    /* Deselect SPI FLASH: ChipSelect pin high */
    #define NotSelect_Flash()    GPIO_SetBits(GPIOB, GPIO_Pin_12)
#elif defined(STM32F4XX)
    /* Select SPI FLASH: ChipSelect pin low  */
    #define Select_Flash()     GPIO_ResetBits(GPIOB, GPIO_Pin_9)
    /* Deselect SPI FLASH: ChipSelect pin high */
    #define NotSelect_Flash()    GPIO_SetBits(GPIOB, GPIO_Pin_9)
#else
//extern hal_gpio_pin_t g_spi_master_cs_n;
    /* Select SPI FLASH: ChipSelect pin low  */
    #define Select_Flash()     hal_gpio_set_output(SPIM_PIN_NUMBER_CS, HAL_GPIO_DATA_LOW)
    /* Deselect SPI FLASH: ChipSelect pin high */
    #define NotSelect_Flash()    hal_gpio_set_output(SPIM_PIN_NUMBER_CS, HAL_GPIO_DATA_HIGH)
#endif
#endif

#endif
















