/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 *
 */

#include "rx8130ce.h"
#include "aos/kernel.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static i2c_dev_t rx8130ce_i2c = {
    .port = RX8130CE_I2C,
    .config.address_width = 8,
    .config.freq = RX8130CE_I2C_CLOCK,
    .config.dev_addr = RX8130CE_I2C_ADDR,
};

static uint8_t rx8130ce_bcd2bin(uint8_t bcd)
{
    uint8_t dec;

    dec = (10 * (bcd >> 4)) + (bcd & 0x0F);

    return dec;
}

static uint8_t rx8130ce_bin2bcd(uint8_t bin)
{
    uint8_t bcd;

    bcd = ((bin / 10) << 4) | (bin % 10);

    return bcd;
}

static uint8_t rx8130ce_checkminmax(uint8_t val, uint8_t min, uint8_t max)
{
    if (val < min) {
        return min;
    } else if (val > max) {
        return max;
    }
    return val;
}

/*********************************************************
 * @fun    rx8130ce_init
 * @breif  rx8130ce initialization
 * @param  none
 * @rtn    0 : on success, EIO : error
 *********************************************************/
int rx8130ce_init(void)
{
    int ret = 0;

    ret = rtc_i2c_init(&rx8130ce_i2c);
    if (ret) {
        printf("rtc i2c init fail\r\n");
        return -1;
    }

    return 0;
}

/*********************************************************
 * @fun    rx8130ce_set_time
 * @param[in]  buf      the pointer for rx8130ce time
 * @param[in]  len      the length of time
 * @rtn    0 : on success, EIO : error
 *********************************************************/
int rx8130ce_set_time(const void *buf, uint8_t len)
{
    int ret = 0;
    uint8_t size;
    uint8_t data[7];
    uint16_t reg_addr = RX8130CE_SECONDS;

    const RX8130CE_TIME_T *time = (RX8130CE_TIME_T *)buf;

    if (buf == NULL) {
        return -1;
    }

    size = sizeof(RX8130CE_TIME_T);

    if (len < size) {
        return -1;
    }

    /* Format data */
    data[0] = rx8130ce_bin2bcd(rx8130ce_checkminmax(time->seconds, 0, 59)) & 0x7F;
    data[1] = rx8130ce_bin2bcd(rx8130ce_checkminmax(time->minutes, 0, 59)) & 0x7F;
    data[2] = rx8130ce_bin2bcd(rx8130ce_checkminmax(time->hours, 0, 23)) & 0x3F;
    data[3] = rx8130ce_checkminmax(time->week, 1, 7) & 0x7F;
    data[4] = rx8130ce_bin2bcd(rx8130ce_checkminmax(time->day, 1, 31)) & 0x3F;
    data[5] = rx8130ce_bin2bcd(rx8130ce_checkminmax(time->month, 1, 12)) & 0x1F;
    data[6] = rx8130ce_bin2bcd(rx8130ce_checkminmax(time->year, 0, 99)) & 0xFF;

    for (uint8_t i = 0; i < 7; i++) {
        ret = rtc_i2c_write(&rx8130ce_i2c, RX8130CE_I2C_ADDR, reg_addr++, 1,
                &data[i], 1, 1000);
        if (ret) {
            printf("RX8310CE set time fail\r\n");
            return -1;
        }
    }

    return 0;
}

/*********************************************************
 * @fun    rx8130ce_get_time
 * @param[in]  buf      the pointer for rx8130ce time
 * @param[in]  len      the length of time
 * @rtn    0 : on success, EIO : error
 *********************************************************/
int rx8130ce_get_time(void *buf, uint8_t len)
{
    int ret = 0;
    uint8_t size;
    uint8_t data[7];
    uint16_t reg_addr = RX8130CE_SECONDS;

    RX8130CE_TIME_T *time = (RX8130CE_TIME_T *)buf;

    if (buf == NULL) {
        return -1;
    }

    size = sizeof(RX8130CE_TIME_T);

    if (len < size) {
        return -1;
    }

    for (uint8_t i = 0; i < 7; i++) {
        ret = rtc_i2c_read(&rx8130ce_i2c, RX8130CE_I2C_ADDR, reg_addr++, 1,
                &data[i], 1, 1000);
        if (ret) {
            printf("RX8310CE read time fail\r\n");
            return -1;
        }
    }

    /* Fill data */
    time->seconds = rx8130ce_bcd2bin(data[0]);
    time->minutes = rx8130ce_bcd2bin(data[1]);
    time->hours = rx8130ce_bcd2bin(data[2]);
    time->week = (data[3]) & 0x7F;
    time->day = rx8130ce_bcd2bin(data[4]);
    time->month = rx8130ce_bcd2bin(data[5]);
    time->year = rx8130ce_bcd2bin(data[6]);

    return 0;
}
