/*
* Copyright (C) 2015-2017 Alibaba Group Holding Limited
*
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aos/kernel.h"
#include "network/hal/base.h"
#include "sensor_drv_api.h"
#include "sensor_hal.h"
 
/* I2C settings for I2C library */
#ifndef DS1307_I2C
#define DS1307_I2C				    1	
//#define DS1307_I2C_PINSPACK			TM_I2C_PinsPack_2
#endif

/* DS1307 I2C clock */
#ifndef DS1307_I2C_CLOCK
#define DS1307_I2C_CLOCK			100000
#endif

/* I2C slave address for DS1307 */
#define DS1307_I2C_ADDR				0xD0

/* Registers location */
#define DS1307_SECONDS				0x00
#define DS1307_MINUTES				0x01
#define DS1307_HOURS				0x02
#define DS1307_DAY					0x03
#define DS1307_DATE					0x04
#define DS1307_MONTH				0x05
#define DS1307_YEAR					0x06
#define DS1307_CONTROL				0x07

/* Bits in control register */
#define DS1307_CONTROL_OUT			7
#define DS1307_CONTROL_SQWE			4
#define DS1307_CONTROL_RS1			1
#define DS1307_CONTROL_RS0			0


/**
 * @brief  Result enumeration
 */
typedef enum {
	TM_DS1307_Result_Ok = 0x00,         /*!< Everything OK */
	TM_DS1307_Result_Error,             /*!< An error occurred */
	TM_DS1307_Result_DeviceNotConnected /*!< Device is not connected */
} TM_DS1307_Result_t;

/**
 * @brief  Structure for date/time
 */
typedef struct {
	uint8_t seconds; /*!< Seconds parameter, from 00 to 59 */
	uint8_t minutes; /*!< Minutes parameter, from 00 to 59 */
	uint8_t hours;   /*!< Hours parameter, 24Hour mode, 00 to 23 */
	uint8_t day;     /*!< Day in a week, from 1 to 7 */
	uint8_t date;    /*!< Date in a month, 1 to 31 */
	uint8_t month;   /*!< Month in a year, 1 to 12 */
	uint8_t year;    /*!< Year parameter, 00 to 99, 00 is 2000 and 99 is 2099 */
} TM_DS1307_Time_t;

/**
 * @brief  Enumeration for SQW/OUT pin
 */
typedef enum {
	TM_DS1307_OutputFrequency_1Hz = 0x00, /*!< Set SQW/OUT pin to 1Hz output frequency */
	TM_DS1307_OutputFrequency_4096Hz,     /*!< Set SQW/OUT pin to 4096Hz output frequency */
	TM_DS1307_OutputFrequency_8192Hz,     /*!< Set SQW/OUT pin to 8192Hz output frequency */
	TM_DS1307_OutputFrequency_32768Hz,    /*!< Set SQW/OUT pin to 32768Hz output frequency */
	TM_DS1307_OutputFrequency_High,       /*!< Set SQW/OUT pin high. Because this pin is open-drain, you will need external pull up resistor */
	TM_DS1307_OutputFrequency_Low         /*!< Set SQW/OUT pin low */
} TM_DS1307_OutputFrequency_t;

/**
 * @}
 */


i2c_dev_t ds1307_i2c = {
    .port = DS1307_I2C,
    .config.address_width = 8,
    .config.freq = DS1307_I2C_CLOCK,
    .config.dev_addr = DS1307_I2C_ADDR,
};

uint8_t TM_DS1307_Bcd2Bin(uint8_t bcd) {
	uint8_t dec = 10 * (bcd >> 4);
	dec += bcd & 0x0F;
	return dec;
}

uint8_t TM_DS1307_Bin2Bcd(uint8_t bin) {
	uint8_t low = 0;
	uint8_t high = 0;
	
	/* High nibble */
	high = bin / 10;
	/* Low nibble */
	low = bin - (high * 10);
	
	/* Return */
	return high << 4 | low;
}

uint8_t TM_DS1307_CheckMinMax(uint8_t val, uint8_t min, uint8_t max) {
	if (val < min) {
		return min;
	} else if (val > max) {
		return max;
	}
	return val;
}


static void drv_rtc_maxim_ds1307_irq_handle(void)
{
    /* no handle so far */
}

static int drv_rtc_maxim_ds1307_open(void)
{
    return 0;
    /* no handle so far */
}

static int drv_rtc_maxim_ds1307_close(void)
{
    /* no handle so far */
    return 0;
}


static int drv_rtc_maxim_ds1307_ioctl(int cmd, unsigned long arg)
{
    /* no handle so far */
    return 0;
}


static int drv_rtc_maxim_ds1307_write(const void *buf, size_t len)
{
    int ret = 0;
    size_t size;
    uint8_t data[7];

    const rtc_data_t *time = (rtc_data_t *)buf;

    if (buf == NULL)
    {
        return -1;
    }

    size = sizeof(rtc_data_t);

    if (len < size)
    {
        return -1;
    }

    /* Format data */ 	
    data[DS1307_SECONDS] = TM_DS1307_Bin2Bcd(TM_DS1307_CheckMinMax(time->seconds, 0, 59)); 	
    data[DS1307_MINUTES] = TM_DS1307_Bin2Bcd(TM_DS1307_CheckMinMax(time->minutes, 0, 59)); 	
    data[DS1307_HOURS] = TM_DS1307_Bin2Bcd(TM_DS1307_CheckMinMax(time->hours, 0, 23)); 	
    data[DS1307_DAY] = TM_DS1307_Bin2Bcd(TM_DS1307_CheckMinMax(time->day, 1, 7)); 	
    data[DS1307_DATE] = TM_DS1307_Bin2Bcd(TM_DS1307_CheckMinMax(time->date, 1, 31)); 	
    data[DS1307_MONTH] = TM_DS1307_Bin2Bcd(TM_DS1307_CheckMinMax(time->month, 1, 12)); 	
    data[DS1307_YEAR] = TM_DS1307_Bin2Bcd(TM_DS1307_CheckMinMax(time->year, 0, 99));

    ret  = sensor_i2c_write(&ds1307_i2c, DS1307_I2C_ADDR, data, 7, I2C_OP_RETRIES);

    if (unlikely(ret))
    {
        return -1;
    }

    return (int)size;
}


static int drv_rtc_maxim_ds1307_read(void *buf, size_t len)
{
    int ret = 0;
    size_t size;
    uint8_t data[7];

    rtc_data_t *time = (rtc_data_t *)buf;

    if (buf == NULL)
    {
        return -1;
    }

    size = sizeof(rtc_data_t);

    if (len < size)
    {
        return -1;
    }
    
    ret  = sensor_i2c_read(&ds1307_i2c, DS1307_SECONDS, data, 7, I2C_OP_RETRIES);

    if (unlikely(ret))
    {
        return -1;
    }
     
    /* Fill data */
    time->seconds = TM_DS1307_Bcd2Bin(data[DS1307_SECONDS]);
    time->minutes = TM_DS1307_Bcd2Bin(data[DS1307_MINUTES]); 	
    time->hours = TM_DS1307_Bcd2Bin(data[DS1307_HOURS]); 	
    time->day = TM_DS1307_Bcd2Bin(data[DS1307_DAY]); 	
    time->date = TM_DS1307_Bcd2Bin(data[DS1307_DATE]); 	
    time->month = TM_DS1307_Bcd2Bin(data[DS1307_MONTH]); 	
    time->year = TM_DS1307_Bcd2Bin(data[DS1307_YEAR]);
    
    time->timestamp = aos_now_ms();

    return (int)size;

}


int drv_rtc_maxim_ds1307_init(void)
{
    int ret = 0;
    sensor_obj_t sensor;
    memset(&sensor, 0, sizeof(sensor));
    /* fill the sensor obj parameters here */
    sensor.io_port    = I2C_PORT;
    sensor.tag        = TAG_DEV_RTC;
    sensor.path       = dev_rtc_path;
    sensor.open       = drv_rtc_maxim_ds1307_open;
    sensor.close      = drv_rtc_maxim_ds1307_close;
    sensor.read       = drv_rtc_maxim_ds1307_read;
    sensor.write      = drv_rtc_maxim_ds1307_write;
    sensor.ioctl      = drv_rtc_maxim_ds1307_ioctl;
    sensor.irq_handle = drv_rtc_maxim_ds1307_irq_handle;

    ret = sensor_create_obj(&sensor);
    if(unlikely(ret)){
        return -1;
    }

    ret = hal_i2c_init(&ds1307_i2c);
    if(unlikely(ret)){
        printf("i2c init failed!\n");
    }

    if(unlikely(ret)){
        return -1;
    }

    //set odr is 100hz, and will update   
    if(unlikely(ret)){
        return -1;
    }

    /* update the phy sensor info to sensor hal */
    LOG("%s %s successfully \n", RTC_STR, __func__);
    return 0;
}

SENSOR_DRV_ADD(drv_rtc_maxim_ds1307_init);


