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
#define W25X_CMD_MANUFACTDEVICEID       0x90
#define W25X_CMD_DEVICEID               0xAB

#define W25X_SECTOR_SIZE                4096

static spi_dev_t spi_w25 = {0};
static uint8_t gcmd_data = 0;

static int32_t w25qxx_init(void)
{
    int32_t ret;

    spi_w25.port = PORT_SPI_DEMO;
    spi_w25.config.mode = HAL_SPI_MODE_MASTER;
    spi_w25.config.freq = 6000000;

    ret = hal_spi_init(&spi_w25);
    if (ret) {
        printf("w25qxx_init fail\r\n");
        return ret;
    }

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

    ret = hal_spi_send_and_recv(&spi_w25, &gcmd_data, 1, &byte, 1, 10);
    if (ret) {
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

    hal_spi_send_and_send(&spi_w25, &gcmd_data, 1, &sr, 1, 10);
}

void w25qxx_wait_busy(void)
{
    /* wait untill the device is not busy */
    while((w25qxx_read_sr()&0x01) == 0x01);
}

int32_t w25qxx_read(uint8_t* pbuffer,uint32_t readaddr,uint16_t num)
{
    uint8_t data[4];
    int32_t ret;

    data[0] = W25X_CMD_READDATA;
    data[1] = (uint8_t)(readaddr >> 16);
    data[2] = (uint8_t)(readaddr >> 8);
    data[3] = (uint8_t)(readaddr);
    ret = hal_spi_send_and_recv(&spi_w25, data, 4, pbuffer, num, 10);
    if(ret){
        printf("w25qxx_read,hal_spi_recv fail!\r\n");
        return ret;
    }
    return 0;
}

//Totally in one page, less than 256 bytes write
void w25qxx_write_page(uint8_t* pbuffer,uint32_t writeaddr,uint16_t num)
{
    uint8_t data[4];
    int32_t ret;

    w25qxx_write_enable();

    w25qxx_wait_busy();

    data[0] = W25X_CMD_PAGEPROGRAM;
    data[1] = (uint8_t)(writeaddr >> 16);
    data[2] = (uint8_t)(writeaddr >> 8);
    data[3] = (uint8_t)(writeaddr);

    ret = hal_spi_send_and_send(&spi_w25, data, 4, pbuffer, num, 10);
    if (ret) {
        printf("write page addr 0x%x failed\r\n", writeaddr);
    }

    w25qxx_wait_busy();

    w25qxx_write_disable();

}

void w25qxx_erase_sector(uint32_t sector_index)
{
    uint8_t  data[4];
    uint32_t eraseaddr;
    int      ret;

    w25qxx_write_enable();

    w25qxx_wait_busy();

    eraseaddr = sector_index * W25X_SECTOR_SIZE;

    data[0] = W25X_CMD_BLOCKERASE;
    data[1] = (uint8_t)(eraseaddr >> 16);
    data[2] = (uint8_t)(eraseaddr >> 8);
    data[3] = (uint8_t)(eraseaddr);

    ret = hal_spi_send(&spi_w25, data, 4, 10);
    if (ret) {
        printf("sector %d erase failed\r\n", sector_index);
    }

    w25qxx_wait_busy();

    w25qxx_write_disable();
}

void w25qxx_chip_erase(void)
{
    uint8_t data;
    int     ret;

    w25qxx_write_enable();

    w25qxx_wait_busy();

    data = W25X_CMD_CHIPERASE;

    ret = hal_spi_send(&spi_w25, &data, 1, 10);
    if (ret) {
        printf("chip erase failed\r\n");
    }

    w25qxx_wait_busy();

    w25qxx_write_disable();
}

uint16_t w25qxx_read_manufactureID(void)
{
    uint8_t  send_buf[4];
    uint16_t manufactureID = 0;

    send_buf[0] = W25X_CMD_MANUFACTDEVICEID;
    send_buf[1] = 0xff;
    send_buf[2] = 0xff;
    send_buf[3] = 0x00;

    hal_spi_send_and_recv(&spi_w25, send_buf, 4, &manufactureID, 2, 10);

    return manufactureID;
}

/* toggle gpio_out periodically in timer handler */
void hal_spi_app_run(void)
{
    uint32_t addr = 20;
    char     pbuffer_read[40] = {0};
    uint16_t id;
    int32_t  ret;

    char *pbuffer = "hal spi test w25qxxx ok!";

    printf("hal_spi_app_run in\r\n");

    w25qxx_init();

    id = w25qxx_read_manufactureID();
    printf("manufacture id 0x%x\r\n", id);

    w25qxx_erase_sector(0);

    w25qxx_write_page(pbuffer, addr, strlen(pbuffer));

    memset(pbuffer_read, 0, sizeof(pbuffer_read));
    ret = w25qxx_read(pbuffer_read, addr, strlen(pbuffer));
    if (ret) {
        printf("hal_spi_app_run:w25qxx_read fail,ret:%d\r\n",ret);
        return;
    }

    if (strncmp(pbuffer,pbuffer_read,strlen(pbuffer))) {
        printf("hal_spi_app_run:read data error!\r\n");
        return;
    } else {
        printf("read data: %s\r\n", pbuffer_read);
    }

    printf("hal_spi_app_run end\r\n");
}

#endif
