/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdlib.h>
#include <aos/kernel.h>
#include "aos/init.h"
#include "aos/hal/gpio.h"
#include "aos/hal/spi.h"
#include "board.h"
#include <k_api.h>

#if (AOS_HAL_SPI_ENABLED > 0)
#ifndef HALAPP_SPI_TEST
#define HALAPP_SPI_TEST             PORT_SPI_1
#endif

/*W25X series ID*/
#define W25Q80  0XEF13
#define W25Q16  0XEF14
#define W25Q32  0XEF15
#define W25Q64  0XEF16
#define W25Q128 0XEF17

#define W25X_CMD_WRITE_ENABLE           0x06
#define W25X_CMD_WRITE_DISABLE          0x04
#define W25X_CMD_READ_STATUS            0x05
#define W25X_CMD_WRITE_STATUS           0x01
#define W25X_CMD_READDATA               0x03
#define W25X_CMD_FAST_READDATA          0x0B
#define W25X_CMD_FAST_READDUAL          0x3B
#define W25X_CMD_PAGEPROGRAM            0x02
#define W25X_CMD_BLOCKERASE             0xD8
#define W25X_CMD_SECTORERASE            0x20
#define W25X_CMD_CHIPERASE              0xC7
#define W25X_CMD_POWERDOWN              0xB9
#define W25X_CMD_RELEASE_POWERDOWN      0xAB
#define W25X_CMD_DEVICEID               0xAB

#define W25X_SECTOR_SIZE                4096

static spi_dev_t spi_w25 = {0};
uint8_t gcmd_data = 0;

static int32_t w25qxx_init(void)
{
    int32_t ret;
    spi_w25.port = PORT_SPI_1;
    spi_w25.config.mode = HAL_SPI_MODE_MASTER;
    spi_w25.config.freq = 6000000;
    ret = hal_spi_init(&spi_w25);
    return ret;
}

void w25qxx_write_enable(void)
{
    gcmd_data = W25X_CMD_WRITE_ENABLE;
    hal_spi_send(&spi_w25, &gcmd_data, 1, 10);
}

void w25qxx_write_disable(void)
{
    gcmd_data = W25X_CMD_WRITE_DISABLE;
    hal_spi_send(&spi_w25, &gcmd_data, 1, 10);
}

//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
uint8_t w25qxx_read_sr(void)
{
    int32_t ret;
    uint8_t byte = 0;
    gcmd_data = W25X_CMD_READ_STATUS;
    hal_spi_send(&spi_w25, &gcmd_data, 1, 10);

    ret = hal_spi_recv(&spi_w25, &byte, 1, 10);
    if(ret){
        printf("w25qxx_readsr,hal_spi_recv fail!\r\n");
        return (uint8_t)-1;
    }

    return byte;
}

//only SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2) can be written.
void w25qxx_write_sr(uint8_t sr)
{
    uint8_t data_send = sr;
    gcmd_data = W25X_CMD_WRITE_STATUS;
    hal_spi_send(&spi_w25, &gcmd_data, 1, 10);

    hal_spi_send(&spi_w25, &data_send, 1, 10);
}

void w25qxx_wait_busy(void)
{
    while((w25qxx_read_sr()&0x01)==0x01);
}

int32_t w25qxx_read(uint8_t* pbuffer,uint32_t readaddr,uint16_t num)
{
    uint8_t data;
    int32_t ret;
    gcmd_data = W25X_CMD_READDATA;
    hal_spi_send(&spi_w25, &gcmd_data, 1, 10);

    /* send addr */
    data = (uint8_t)(readaddr >> 16);
    hal_spi_send(&spi_w25, &data, 1, 10);

    data = (uint8_t)(readaddr >> 8);
    hal_spi_send(&spi_w25, &data, 1, 10);

    data = (uint8_t)(readaddr);
    hal_spi_send(&spi_w25, &data, 1, 10);

    ret = hal_spi_recv(&spi_w25, pbuffer, num, 20);
    if(ret){
        printf("w25qxx_read,hal_spi_recv fail!\r\n");
        return ret;
    }
    return 0;

}

//Totally in one page, less than 256 bytes write
void w25qxx_write_page(uint8_t* pbuffer,uint32_t writeaddr,uint16_t num)
{
    uint8_t data;
    w25qxx_write_enable();
    gcmd_data = W25X_CMD_PAGEPROGRAM;
    hal_spi_send(&spi_w25, &gcmd_data, 1, 10);

    /* send addr */
    data = (uint8_t)(writeaddr >> 16);
    hal_spi_send(&spi_w25, &data, 1, 10);

    data = (uint8_t)(writeaddr >> 8);
    hal_spi_send(&spi_w25, &data, 1, 10);

    data = (uint8_t)(writeaddr);
    hal_spi_send(&spi_w25, &data, 1, 10);

    hal_spi_send(&spi_w25,pbuffer,num,20);

    w25qxx_wait_busy();
}

void w25qxx_erase_sector(uint32_t sector_index)
{
    uint8_t data;
    uint32_t eraseaddr;
    w25qxx_write_enable();
    w25qxx_wait_busy();

    gcmd_data = W25X_CMD_SECTORERASE;
    hal_spi_send(&spi_w25, &gcmd_data, 1, 10);

    eraseaddr = sector_index * W25X_SECTOR_SIZE;

    /* send addr */
    data = (uint8_t)(eraseaddr >> 16);
    hal_spi_send(&spi_w25, &data, 1, 10);

    data = (uint8_t)(eraseaddr >> 8);
    hal_spi_send(&spi_w25, &data, 1, 10);

    data = (uint8_t)(eraseaddr);
    hal_spi_send(&spi_w25, &data, 1, 10);

    w25qxx_wait_busy();
}


/* toggle gpio_out periodically in timer handler */
void hal_spi_app_run(void)
{
    int32_t ret;
    uint32_t addr = 20;
    char *pbuffer = "hal spi test w25qxxx ok!";
    char pbuffer_read[40] = {0};
    printf("hal_i2c_app_run in\r\n");
    w25qxx_erase_sector(0);

    w25qxx_write_page(pbuffer,addr,strlen(pbuffer));

    ret = w25qxx_read(pbuffer_read,addr,strlen(pbuffer));
    if(ret){
        printf("hal_spi_app_run:w25qxx_read fail,ret:%d\r\n",ret);
        return;
    }
    if(strncmp(pbuffer,pbuffer_read,strlen(pbuffer))){
        printf("hal_spi_app_run:read data error!\r\n");
        return;
    }

    printf("hal_i2c_app_run end\r\n");
}

#endif
