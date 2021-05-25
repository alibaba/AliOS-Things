/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "aos/kernel.h"
#include "aos/hal/spi.h"
#include "spi_test.h"
#include "board.h"
#include "stm32f4xx_hal.h"

/* Private define ------------------------------------------------------------*/
#define W25X_WriteEnable              0x06 
#define W25X_WriteDisable             0x04 
#define W25X_ReadStatusReg          0x05 
#define W25X_WriteStatusReg         0x01 
#define W25X_ReadData                   0x03 
#define W25X_FastReadData             0x0B 
#define W25X_FastReadDual             0x3B 
#define W25X_PageProgram              0x02 
#define W25X_BlockErase               0xD8 
#define W25X_SectorErase              0x20 
#define W25X_ChipErase                0xC7 
#define W25X_PowerDown                0xB9 
#define W25X_ReleasePowerDown       0xAB 
#define W25X_DeviceID                   0xAB 
#define W25X_ManufactDeviceID       0x90 
#define W25X_JedecDeviceID          0x9F 

#define WIP_Flag                  0x01  /* Write In Progress (WIP) flag */

#define Dummy_Byte                0xFF

#define TEST_DATA_BYTES 128

static spi_dev_t spi_dev;

static uint8_t write_buf[TEST_DATA_BYTES];
static uint8_t read_buf[TEST_DATA_BYTES];

static int32_t flash_spi_init(void);
static int32_t flash_spi_read(void *buf, uint32_t bytes);
static int32_t flash_spi_write(void *buf, uint32_t bytes);
static int32_t flash_spi_erase(void);

SPI_HandleTypeDef hspi1;
/* SPI1 init function */
static void MX_SPI1_Init(void)
{

  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;

  hspi1.Init.Mode = SPI_MODE_MASTER;
  
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

uint8_t flash_spi_read_device_id(void)
{
   uint8_t data = 0;
   uint8_t data_r = 0;
   int ret = -1;

   /* Select the FLASH: Chip Select low */
   //SPI_FLASH_CS_LOW();

   /* Send "RDID " instruction */
    data = W25X_DeviceID;
    ret = HAL_SPI_Transmit(&hspi1, &data, 1, 100);
    printf("ret %d\n", ret);

    data = Dummy_Byte;
    HAL_SPI_TransmitReceive(&hspi1, &data, &data_r, 1, 1000);
    HAL_SPI_TransmitReceive(&hspi1, &data, &data_r, 1, 1000);
    HAL_SPI_TransmitReceive(&hspi1, &data, &data_r, 1, 1000);
    HAL_SPI_TransmitReceive(&hspi1, &data, &data_r, 1, 1000);

    while (HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY)
    {
    } 

   /* Read a byte from the FLASH */
    data = Dummy_Byte;
    ret = HAL_SPI_TransmitReceive(&hspi1, &data, &data_r, 1, 1000);
    printf("ret %d\n", ret);
   /* Deselect the FLASH: Chip Select high */
   //SPI_FLASH_CS_HIGH();

   return data_r;
}

uint32_t flash_spi_read_id(void)
{
   uint32_t data1 = 0;
   uint32_t data2 = 0;
   uint32_t data3 = 0;
   uint32_t data4 = 0;
   uint8_t data = 0;
   uint8_t data_r = 0;
   int ret = -1;

   /* Select the FLASH: Chip Select low */
   //SPI_FLASH_CS_LOW();

   /* Send "RDID " instruction */
    data = 0x90;
    HAL_SPI_TransmitReceive(&hspi1, &data, &data_r, 1, 1000);
    data = 0x00;
    HAL_SPI_TransmitReceive(&hspi1, &data, &data_r, 1, 1000);
    data = 0x00;
    HAL_SPI_TransmitReceive(&hspi1, &data, &data_r, 1, 1000);
    data = 0x00;
    HAL_SPI_TransmitReceive(&hspi1, &data, &data_r, 1, 1000);

    data = 0xFF;
    HAL_SPI_TransmitReceive(&hspi1, &data, &data1, 1, 1000);
    data = 0xFF;
    HAL_SPI_TransmitReceive(&hspi1, &data, &data2, 1, 1000);


    data4 = (data1 << 8) | data2;


   return data4;
}

#if 0
uint8_t flash_spi_read_device_id(void)
{
   uint8_t data = 0;
   int ret = -1;

   /* Select the FLASH: Chip Select low */
   //SPI_FLASH_CS_LOW();

   /* Send "RDID " instruction */
    data = W25X_DeviceID;
    ret = hal_spi_send(&spi_dev, &data, 1, AOS_WAIT_FOREVER);
    printf("ret %d\n", ret);

    data = Dummy_Byte;
    hal_spi_send(&spi_dev, &data, 1, AOS_WAIT_FOREVER);
    hal_spi_send(&spi_dev, &data, 1, AOS_WAIT_FOREVER);
    hal_spi_send(&spi_dev, &data, 1, AOS_WAIT_FOREVER);
    hal_spi_send(&spi_dev, &data, 1, AOS_WAIT_FOREVER);

   /* Read a byte from the FLASH */
    hal_spi_recv(&spi_dev, &data, 1, AOS_WAIT_FOREVER);

   /* Deselect the FLASH: Chip Select high */
   //SPI_FLASH_CS_HIGH();

   return data;
}
#endif

/* a spi interface rtc(ds1307) must be connetted to the mcu before testing */
void hal_spi_test(void)
{
    int ret = -1;
    int i   = 0;

    printf("*********** spi test start ! ***********\n");

    memset(read_buf, 0, sizeof(read_buf));

    //ret = flash_spi_init();
    //if (ret != 0) {
    //    printf("flash_spi_init error ! test failed !\n");
    //    return;
    //}
    MX_SPI1_Init();

    //uint8_t data = flash_spi_read_device_id();
    //printf("flash_spi_read_device_id: %x\n", data);

    uint32_t data32 = flash_spi_read_id();
    printf("flash_spi_read_id: %x\n", data32);
    return;

    ret = flash_spi_erase();
    if (ret != 0) {
        printf("flash_spi_erase error ! test failed !\n");
        return;
    }

    for (i = 0; i < TEST_DATA_BYTES; i++)
    {
        write_buf[i] = i;
    }

    ret = flash_spi_write(write_buf, TEST_DATA_BYTES);
    if (ret != 0) {
        printf("flash_spi_write error ! test failed !\n");
        return;
    }

    ret = flash_spi_read(read_buf, TEST_DATA_BYTES);
    if (ret != 0) {
        printf("flash_spi_read error ! test failed !\n");
        return;
    }

    for (i = 0; i < TEST_DATA_BYTES; i++)
    {
        if (write_buf[i] != read_buf[i]) {
            printf("error ! read data diff with write data ! test failed !\n");
            return;
        }
    }

    ret = hal_spi_finalize(&spi_dev);
    if (ret != 0) {
        printf("hal_spi_finalize error ! test failed !\n");
        return;
    }

    printf("spi test result: succeed !\n");

    printf("*********** spi test end ! ***********\n");
}

int32_t flash_spi_init(void)
{
    int ret = -1;

    spi_dev.port = PORT_SPI_1;
    spi_dev.config.mode = HAL_SPI_MODE_MASTER;
    spi_dev.config.freq = 15000000;

    ret = hal_spi_init(&spi_dev);
    if (ret != 0) {
        printf("hal_spi_init error !\n");
    }

    return ret;
}

int32_t flash_spi_read(void *buf, uint32_t bytes)
{
    int ret = -1;



    return ret;
}

int32_t flash_spi_write(void *buf, uint32_t bytes)
{
    int ret = -1;



    return ret;
}

int32_t flash_spi_erase(void)
{
    int ret = -1;



    return ret;
}

