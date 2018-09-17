/*
 * Copyright (c) 2010-2012, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "obds.h"
#include "registers/regsiomuxc.h"
#define SPI_NOR_XFER_SZ     512

#define SPI_INIT_PARAM { \
    .channel = 1, \
    .mode = 1, \
    .ss_pol = 0, \
    .sclk_pol = 0, \
    .sclk_pha = 0, \
    .pre_div = 0, \
    .post_div = 3 \
    }
    
//#define  SPI_INIT_PARAM  0x00000084

const char g_spi_nor_test_name[] = "SPI NOR FLASH Test";

extern int spi_nor_query_atmel(uint32_t * data);
extern int spi_nor_write_atmel(uint32_t addr, uint8_t * data, uint32_t length);
extern int spi_nor_read_atmel(void *src, void *dest, int len);
extern int spi_nor_erase_atmel(uint32_t addr, uint32_t length);
extern int spi_nor_query_numonyx(uint32_t * data);
extern int spi_nor_write_numonyx(uint32_t addr, uint8_t* data, uint32_t length);
extern int spi_nor_read_numonyx(void *src, void *dest, int len);
extern int spi_nor_erase_numonyx(uint32_t addr, uint32_t length);

uint32_t spi_nor_flash_type;
dev_ecspi_e dev_spi_nor;

uint8_t __attribute__ ((aligned(4))) spi_nor_tx_buf[SPI_NOR_XFER_SZ];
uint8_t __attribute__ ((aligned(4))) spi_nor_rx_buf[SPI_NOR_XFER_SZ];

struct chip_id {
    char id0;
    char id1;
    char id2;
};

struct chip_id AT45DB321D_id = {
    .id0 = 0x01,                // Atmel AT45DB321D
    .id1 = 0x27,
    .id2 = 0x1f
};

struct chip_id M25P32_id = {
    .id0 = 0x16,
    .id1 = 0x20,
    .id2 = 0x20
};

int spi_nor_query(uint32_t * data, uint32_t spi_nor_flash_type)
{
    /* query id */
    if (spi_nor_flash_type == AT45DB321D)
        return spi_nor_query_atmel(data);
    else if (spi_nor_flash_type == M25P32)
        return spi_nor_query_numonyx(data);

    return 0;
}

/* spi nor write */
int spi_nor_write(uint32_t addr, uint8_t * data, uint32_t length, uint32_t spi_nor_flash_type)
{
    if (spi_nor_flash_type == AT45DB321D)
        return spi_nor_write_atmel(addr, data, length);
    else if (spi_nor_flash_type == M25P32)
        return spi_nor_write_numonyx(addr, data, length);

    return 0;
}

int spi_nor_read(void *src, void *dest, int len, uint32_t spi_nor_flash_type)
{
    if (spi_nor_flash_type == AT45DB321D)
        return spi_nor_read_atmel(src, dest, len);
    else if (spi_nor_flash_type == M25P32)
        return spi_nor_read_numonyx(src, dest, len);

    return 0;
}

int spi_nor_erase(uint32_t addr, uint32_t length, uint32_t spi_nor_flash_type)
{
    if (spi_nor_flash_type == AT45DB321D)
        return spi_nor_erase_atmel(addr, length);
    else if (spi_nor_flash_type == M25P32)
        return spi_nor_erase_numonyx(addr, length);

    return 0;
}

test_return_t spi_nor_test(void)
{
    uint32_t src[128];
    uint32_t dst[128];
    uint32_t i;
    uint8_t id[4];
    const char* indent = menu_get_indent();
    param_ecspi_t  spiParams = SPI_INIT_PARAM;

#if defined(BOARD_SABRE_AI) || defined(BOARD_SMART_DEVICE) || defined(BOARD_EVB) || defined(BOARD_EVK)
    spi_nor_flash_type = M25P32;
#endif

#if defined(BOARD_SABRE_AI)
    printf("%sConfigure J3 to 2-3 position when running the test,\n", indent);
    printf("%s and when test is over configure J3 to 1-2 position.\n", indent);
    printf("%sHas jumper J3 been properly configured?\n", indent);

    if (!is_input_char('y', indent)) 
    	return TEST_BYPASSED;

	 HW_IOMUXC_SW_MUX_CTL_PAD_EIM_ADDR24_WR(
			               BF_IOMUXC_SW_MUX_CTL_PAD_EIM_ADDR24_MUX_MODE_V(ALT5));
     gpio_set_direction(GPIO_PORT5, 4, GPIO_GDIR_OUTPUT);
	 gpio_set_level(GPIO_PORT5, 4, GPIO_LOW_LEVEL);
#endif

    dev_spi_nor = DEV_ECSPI1;
    //init eCSPI
    ecspi_open(dev_spi_nor, &spiParams);
//    ecspi_open(dev_spi_nor, (param_ecspi_t)SPI_INIT_PARAM);

    // Look through table for device data
    spi_nor_query((uint32_t *) id, spi_nor_flash_type);

    // Do we find the device? If not, return error.
    printf("%sGet chip id: 0x%02X, 0x%02X, 0x%02X, 0x%02X\n", indent, id[0], id[1], id[2], id[3]);

    if (spi_nor_flash_type == AT45DB321D) {
        if ((id[0] == AT45DB321D_id.id0) && (id[1] == AT45DB321D_id.id1)
            && (id[2] == AT45DB321D_id.id2)) {
            printf("%sSPI nor flash chip AT45DBxx found.\n", indent);
            printf("%sChip id check PASS.\n", indent);

        } else {
            printf("%sChip id check FAIL.\n", indent);

            return TEST_FAILED;
        }
    } else if (spi_nor_flash_type == M25P32) {
        if ((id[0] == M25P32_id.id0) && (id[1] == M25P32_id.id1)
            && (id[2] == M25P32_id.id2)) {
            printf("%sSPI nor flash chip M25P32xx found.\n", indent);
            printf("%sChip id check PASS.\n", indent);

        } else {
            printf("%sChip id check FAIL.\n", indent);

            return TEST_FAILED;
        }
    }

    src[0] = 0x12345678;
    src[1] = 0x22222222;

    for (i = 2; i < 128; i++) {
        src[i] = src[i - 1] + src[i - 2];
    }

    printf("%sErase spi-nor flash...\n", indent);
    spi_nor_erase(0, 64 * 1024, spi_nor_flash_type);

    printf("%sProgram spi-nor flash... \n", indent);
    if (spi_nor_write(0, (void *)src, 512, spi_nor_flash_type) == 1) {
        /* Not tested as we need a power cycle */
        printf("%s SPI NOR test has been bypassed \n", indent);

        return TEST_FAILED;
    }
    printf("%sRead back data from spi-nor flash \n", indent);
    memset((void *)dst, 0x0, 512);
    spi_nor_read(0, (void *)dst, 512, spi_nor_flash_type);
    printf("%sCompare data...\n", indent);

    // compare data...
    for (i = 0; i < 128; i++) {
        if (dst[i] != src[i]) {
            printf("%sSPI NOR verify failed. \n", indent);
            printf("%s [%d] src: 0x%08X, dst: 0x%08X\n", indent, i, src[i], dst[i]);

            return TEST_FAILED;
        }
    }
    
    return TEST_PASSED;
}

