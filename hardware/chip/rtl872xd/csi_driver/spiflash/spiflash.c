/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "objects.h"
#include "PinNames.h"
#include "pinmap.h"
#include "ameba_soc.h"
#include "rtl8721dlp_rcc.h"
#include <drv/spiflash.h>
#include <aos/mtd.h>
#include <aos/mtdpart.h>
#include <aos/hal/flash.h>

csi_error_t csi_spiflash_spi_init(csi_spiflash_t *spiflash, uint32_t spi_idx, void *spi_cs_callback)
{
    spiflash->spi_qspi.spi.dev.idx = spi_idx;
    spiflash->spi_qspi.spi.dev.reg_base = SPIC;
    spiflash->spi_cs_callback = spi_cs_callback;
    return CSI_OK;
}

void csi_spiflash_spi_uninit(csi_spiflash_t *spiflash)
{
    if(spiflash->spi_cs_callback != NULL)
        spiflash->spi_cs_callback = NULL;
}

csi_error_t csi_spiflash_get_flash_info(csi_spiflash_t *spiflash, csi_spiflash_info_t *flash_info)
{
    /* To avoid gcc warnings */
    ( void ) spiflash;

    uint8_t flash_id[4];

    FLASH_Write_Lock();
    FLASH_RxCmd(flash_init_para.FLASH_cmd_rd_id, 3, flash_id);
    FLASH_Write_Unlock();

    flash_info->flash_id = (flash_id[2] << 16) |(flash_id[1] << 8) |flash_id[0];
    /* get flash vender by flash id */
    flash_info->xip_addr = SPI_FLASH_BASE;
    flash_info->sector_size = 0x1000;
    flash_info->page_size = 256;
    /* get flash size by flash vendor */
    flash_info->flash_size = 0x200000;
    return CSI_OK;
}


int32_t csi_spiflash_read(csi_spiflash_t *spiflash, uint32_t offset, void *data, uint32_t size)
{
    /* To avoid gcc warnings */
    ( void ) spiflash;

    //assert_param(data != NULL);
    if(data == NULL)
        return CSI_ERROR;

    uint32_t offset_to_align;
    uint32_t i;
    uint32_t read_word;
    uint8_t *ptr;
    uint8_t *pbuf;
    int32_t read_len = 0;

    FLASH_Write_Lock();

    offset_to_align = offset & 0x03;
    pbuf = (uint8_t *)data;
    if (offset_to_align != 0) {
        /* the start address is not 4-bytes aligned */
        read_word = HAL_READ32(SPI_FLASH_BASE, (offset - offset_to_align));
        ptr = (uint8_t*)&read_word + offset_to_align;
        offset_to_align = 4 - offset_to_align;
        for (i=0;i<offset_to_align;i++) {
            *pbuf = *(ptr+i);
            pbuf++;
            size--;
            if (size == 0) {
                break;
            }
        }
    }

    /* address = next 4-bytes aligned */
    offset = (((offset-1) >> 2) + 1) << 2;

    ptr = (uint8_t*)&read_word;
    if ((uint32_t)pbuf & 0x03) {
        while (size >= 4) {
            read_word = HAL_READ32(SPI_FLASH_BASE, offset);
            for (i=0;i<4;i++) {
                *pbuf = *(ptr+i);
                pbuf++;
            }
            offset += 4;
            size -= 4;
            read_len += 4;
        }
    } else {
        while (size >= 4) {
            *((uint32_t *)pbuf) = HAL_READ32(SPI_FLASH_BASE, offset);
            pbuf += 4;
            offset += 4;
            size -= 4;
            read_len += 4;
        }
    }

    if (size > 0) {
        read_word = HAL_READ32(SPI_FLASH_BASE, offset);
        for (i=0;i<size;i++) {
            *pbuf = *(ptr+i);
            pbuf++;
        }
    }
    read_len += size;

    FLASH_Write_Unlock();
    return read_len;
}

int32_t csi_spiflash_program(csi_spiflash_t *spiflash, uint32_t offset, const void *data, uint32_t size)
{
    /* To avoid gcc warnings */
    ( void ) spiflash;

    // Check address: 4byte aligned & page(256bytes) aligned
    uint32_t page_begin = offset &  (~0xff);
    uint32_t page_end = (offset + size) & (~0xff);
    uint32_t page_cnt = ((page_end - page_begin) >> 8) + 1;

    uint32_t addr_begin = offset;
    uint32_t addr_end = (page_cnt == 1) ? (offset + size) : (page_begin + 0x100);
    uint32_t length = addr_end - addr_begin;
    uint8_t *buffer = (uint8_t *)data;
    uint8_t write_data[12];

    uint32_t offset_to_align;
    uint32_t read_word;
    uint32_t i;
    int32_t write_len = 0;

    FLASH_Write_Lock();
    while(page_cnt){
        offset_to_align = addr_begin & 0x3;

        write_len += length;

        if(offset_to_align != 0){
            read_word = HAL_READ32(SPI_FLASH_BASE, addr_begin - offset_to_align);
            for(i = offset_to_align;i < 4;i++){
                read_word = (read_word &  (~(0xff << (8*i)))) |( (*buffer) <<(8*i));
                length--;
                buffer++;
                if(length == 0)
                    break;
            }
            FLASH_TxData12B(addr_begin - offset_to_align, 4, (uint8_t*)&read_word);
        }

        addr_begin = (((addr_begin-1) >> 2) + 1) << 2;
        for(;length >= 12 ;length -= 12){
            _memcpy(write_data, buffer, 12);
            FLASH_TxData12B(addr_begin, 12, write_data);
            buffer += 12;
            addr_begin += 12;
        }

        for(;length >= 4; length -=4){
            _memcpy(write_data, buffer, 4);
            FLASH_TxData12B(addr_begin, 4, write_data);
            buffer += 4;
            addr_begin += 4;
        }

        if(length > 0){
            read_word = HAL_READ32(SPI_FLASH_BASE, addr_begin);
            for( i = 0;i < length;i++){
                read_word = (read_word & (~(0xff << (8*i)))) | ((*buffer) <<(8*i));
                buffer++;
            }
            FLASH_TxData12B(addr_begin, 4, (uint8_t*)&read_word);
        }
        page_cnt--;
        addr_begin = addr_end;
        addr_end = (page_cnt == 1) ? (offset + size) : (((addr_begin>>8) + 1)<<8);
        length = addr_end - addr_begin;
    }

    DCache_Invalidate(SPI_FLASH_BASE + offset, size);
    FLASH_Write_Unlock();

    return write_len;
}

csi_error_t csi_spiflash_erase(csi_spiflash_t *spiflash, uint32_t offset, uint32_t size)
{
    /* To avoid gcc warnings */
    ( void ) spiflash;

    /* Sector alignment */
    size = (((size - 1) >> 12 ) + 1) << 12;

    FLASH_Write_Lock();
    while(size)
    {
        FLASH_Erase(EraseSector, offset);
        DCache_Invalidate(SPI_FLASH_BASE + offset, 0x1000);
        offset += 0x1000;
        size -= 0x1000;
    }
    FLASH_Write_Unlock();
    return CSI_OK;
}

static aos_mtd_t g_mtd_nor_dev;

struct part_info {
    uint32_t cnt;
    struct mtd_part *part;
};

static int get_mtd_part(struct part_info *info)
{
    mtd_partition_t *p;
    struct mtd_part *parts;

    parts = calloc(mtd_partitions_amount, sizeof(struct mtd_part));
    if (parts == NULL) {
        return -ENOMEM;
    }

    for (int i = 0, j = 0; i < mtd_partitions_amount; i++) {
        p = &mtd_partitions[i];
        parts[j].name_std = p->partition_name_std;
        parts[j].name = p->partition_name;
        parts[j].offset = p->partition_start_addr;
        parts[j].size = p->partition_length;
        j++;
    }

    info->cnt = mtd_partitions_amount;
    info->part = parts;
    return 0;
}

int csi_flash_init()
{
    struct part_info info = {0};
    uint32_t blk_size = 4096;
    uint32_t page_size = 256;
    int ret;

    printf("%s:%d begin..\n", __func__, __LINE__);
    if (get_mtd_part(&info)) {
        printf("%s:%d get_mtd_part failed.\n", __func__, __LINE__);
        return -1;
    }

    ret = aos_mtd_nor_init(&g_mtd_nor_dev, blk_size, page_size);
    if (ret != 0) {
        printf("%s:%d aos_mtd_nor_init failed, ret:\n", __func__, __LINE__, ret);
        return -2;
    }

    ret = aos_mtd_register(&g_mtd_nor_dev, info.part, info.cnt);
    if (ret < 0) {
        printf("%s:%d aos_mtd_register failed, ret:\n", __func__, __LINE__, ret);
    } else {
        printf("%s:%d done\n", __func__, __LINE__);
    }

    return ret;
}
