/*
 * Copyright (C) 2019 X-Cite SA (http://www.x-cite.io)
 * Written by Lemmer El Assal (lemmer@x-cite.io)
*/
#include "bme680.h"
#include "sensor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <aos/aos.h>
#include <vfs_conf.h>
#include <vfs_err.h>
#include <vfs_register.h>
#include <hal/base.h>
#include "common.h"
#include "sensor_drv_api.h"
#include "sensor_hal.h"

#ifndef BME680_I2C_PORT
	#define BME680_I2C_PORT 1
#endif

#ifndef BME680_DATA_READ_MIN_INTERVAL
	#define BME680_DATA_READ_MIN_INTERVAL 1000
#endif

#ifndef BME680_I2C_FREQ
	#define BME680_I2C_FREQ 100000
#endif

#ifndef REDTEXT
#define REDTEXT   "\x1B[31m"
#endif
#ifndef GREENTEXT
#define GREENTEXT   "\x1B[32m"
#endif
#ifndef RESETTEXT
#define RESETTEXT "\x1B[0m"
#endif

#ifndef TRACE_COLOR
#define TRACE_COLOR(color, type, fmt, ...)  \
    do { \
        HAL_Printf("%s%s|%03d :: [%s] ", color, __func__, __LINE__, type); \
        HAL_Printf(fmt, ##__VA_ARGS__); \
        HAL_Printf("%s" RESETTEXT, "\r\n"); \
    } while(0)
#endif

#ifndef TRACE_ERROR
#define TRACE_ERROR(fmt, ...) TRACE_COLOR(REDTEXT, "ERROR", fmt, ##__VA_ARGS__)
#endif

#ifndef TRACE_INFO
#define TRACE_INFO(fmt, ...) TRACE_COLOR(GREENTEXT, "INFO", fmt, ##__VA_ARGS__)
#endif

static struct bme680_dev g_bme680_dev;
static struct bme680_field_data g_bme680_data_new;
uint16_t g_bme680_meas_period;
uint32_t g_bme680_timeout = 1000;
sensor_obj_t sensor_temp, sensor_humi, sensor_gr, sensor_baro;
static uint32_t prev_update_tick = 0;
uint32_t now_tick = 0;


i2c_dev_t bme680_ctx = {
    .port = BME680_I2C_PORT,
	.config.freq = BME680_I2C_FREQ,
	.config.address_width = I2C_MEM_ADDR_SIZE_8BIT,
    .config.dev_addr = BME680_I2C_ADDR_PRIMARY,
}; 

static int drv_bme680_update_data()
{

    int ret = 0;

    now_tick = aos_now_ms();
    if ((now_tick - prev_update_tick >= BME680_DATA_READ_MIN_INTERVAL) || (prev_update_tick == 0)) {
		ret = bme680_set_sensor_mode(&g_bme680_dev);
        if(ret != BME680_OK)
            TRACE_ERROR("[BME680] Set sensor mode failed (returned %d)", ret);
        aos_msleep(g_bme680_meas_period * 2);
        ret = bme680_get_sensor_data(&g_bme680_data_new, &g_bme680_dev);

		if (unlikely(ret != 0))
            return ret;
        prev_update_tick = now_tick;
    }

    return 0;
}


static void drv_gr_bosch_bme680_irq_handle(void)
{
    /* no handle so far */
}

static int drv_gr_bosch_bme680_open(void)
{
    LOG("%s %s successfully \n", SENSOR_STR, __func__);

    return 0;
}

static int drv_gr_bosch_bme680_close(void)
{
    LOG("%s %s successfully \n", SENSOR_STR, __func__);

    return 0;
}

static int drv_gr_bosch_bme680_read(void *buf, size_t len)
{
    int ret = 0;
    const size_t size = sizeof(gr_data_t);
    gr_data_t *pdata = (gr_data_t*)buf;

    if (buf == NULL){
        return -1;
    }

    if (len < size){
        return -1;
    }
    
    ret = drv_bme680_update_data();
    if (ret != 0)
        return -1;

    pdata->gr = g_bme680_data_new.gas_resistance;

    pdata->timestamp = aos_now_ms();

    return (int)size;
}

static int drv_gr_bosch_bme680_write(const void *buf, size_t len)
{
    (void)buf;
    (void)len;
    return 0;
}


static int drv_gr_bosch_bme680_ioctl(int cmd, unsigned long arg)
{
    switch (cmd) {
        case SENSOR_IOCTL_GET_INFO:
		{
			/* fill the dev info here */
			dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
			info->model = "BME680";
			info->vendor = DEV_SENSOR_VENDOR_BOSCH;
			info->unit = ohm;
			break;
		}
        default:
            return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

static void drv_baro_bosch_bme680_irq_handle(void)
{
    /* no handle so far */
}

static int drv_baro_bosch_bme680_open(void)
{
    LOG("%s %s successfully \n", SENSOR_STR, __func__);

    return 0;
}

static int drv_baro_bosch_bme680_close(void)
{
    LOG("%s %s successfully \n", SENSOR_STR, __func__);

    return 0;
}

static int drv_baro_bosch_bme680_read(void *buf, size_t len)
{
    int ret = 0;
    const size_t size = sizeof(barometer_data_t);
    barometer_data_t *pdata = (barometer_data_t*)buf;

    if (buf == NULL){
        return -1;
    }

    if (len < size){
        return -1;
    }
    
    ret = drv_bme680_update_data();
    if (ret != 0)
        return -1;

    pdata->p = g_bme680_data_new.pressure;

    pdata->timestamp = aos_now_ms();

    return (int)size;
}

static int drv_baro_bosch_bme680_write(const void *buf, size_t len)
{
    (void)buf;
    (void)len;
    return 0;
}


static int drv_baro_bosch_bme680_ioctl(int cmd, unsigned long arg)
{
    switch (cmd) {
        case SENSOR_IOCTL_GET_INFO: {
			dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
			info->model = "BME680";
			info->vendor = DEV_SENSOR_VENDOR_BOSCH;
			info->unit = pascal;
			break;
		}	
        default:
            return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

static void drv_humi_bosch_bme680_irq_handle(void)
{
    /* no handle so far */
}

static int drv_humi_bosch_bme680_open(void)
{
    LOG("%s %s successfully \n", SENSOR_STR, __func__);

    return 0;
}

static int drv_humi_bosch_bme680_close(void)
{
    LOG("%s %s successfully \n", SENSOR_STR, __func__);

    return 0;
}

static int drv_humi_bosch_bme680_read(void *buf, size_t len)
{
    int ret = 0;
    const size_t size = sizeof(humidity_data_t);
    humidity_data_t *pdata = (humidity_data_t*)buf;

    if (buf == NULL){
        return -1;
    }

    if (len < size){
        return -1;
    }
    
    ret = drv_bme680_update_data();
    if (ret != 0)
        return -1;

    pdata->h = g_bme680_data_new.humidity / 1000;

    pdata->timestamp = aos_now_ms();

    return (int)size;
}

static int drv_humi_bosch_bme680_write(const void *buf, size_t len)
{
    (void)buf;
    (void)len;
    return 0;
}


static int drv_humi_bosch_bme680_ioctl(int cmd, unsigned long arg)
{
		switch (cmd) {
			case SENSOR_IOCTL_GET_INFO: {
				/* fill the dev info here */
				dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
				info->model = "BME680";
				info->vendor = DEV_SENSOR_VENDOR_BOSCH;
				info->unit = percentage;
				break;
			}
			default:
				return -1;
    	}

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

static void drv_temp_bosch_bme680_irq_handle(void)
{
    /* no handle so far */
}

static int drv_temp_bosch_bme680_open(void)
{
    LOG("%s %s successfully \n", SENSOR_STR, __func__);

    return 0;
}

static int drv_temp_bosch_bme680_close(void)
{
    LOG("%s %s successfully \n", SENSOR_STR, __func__);

    return 0;
}

static int drv_temp_bosch_bme680_read(void *buf, size_t len)
{
    int ret = 0;
    const size_t size = sizeof(temperature_data_t);
    temperature_data_t *pdata = (temperature_data_t*)buf;
    if ((buf == NULL) || (len < size))
        return -1;
    ret = drv_bme680_update_data();
    if (ret != 0)
        return -1;

    pdata->t = g_bme680_data_new.temperature / 100;
	TRACE_INFO("Temperature raw: %d", g_bme680_data_new.temperature);
	TRACE_INFO("Temperature: %d", pdata->t);
	

    pdata->timestamp = aos_now_ms();

    return (int)size;
}

static int drv_temp_bosch_bme680_write(const void *buf, size_t len)
{
    (void)buf;
    (void)len;
    return 0;
}


static int drv_temp_bosch_bme680_ioctl(int cmd, unsigned long arg)
{
    switch (cmd) {
        case SENSOR_IOCTL_GET_INFO: {
			dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
            info->model = "BME680";
			info->vendor = DEV_SENSOR_VENDOR_BOSCH;
            info->unit = dCelsius;
        	break;
		}
        default:
            return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

int8_t user_i2c_write(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len)
{
    return hal_i2c_mem_write(&bme680_ctx, bme680_ctx.config.dev_addr, (uint16_t) reg_addr, 1, reg_data, len, g_bme680_timeout);
}

int8_t user_i2c_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len)
{
    int8_t result = user_i2c_write(dev_id, reg_addr, 0, 0);
    if(result)
        return result;
    result = hal_i2c_master_recv(&bme680_ctx, bme680_ctx.config.dev_addr, reg_data, len, g_bme680_timeout);
    return result;
}

void user_delay_ms(uint32_t period)
{
    aos_msleep(period);
}


int8_t user_spi_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len)
{
    return -1; //return hal_spi_recv(brd_spi_dev, reg_data, len, timeout);
}

int8_t user_spi_write(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len)
{
    return -1; //return hal_spi_send(brd_spi_dev, reg_data, len, timeout);
}

int32_t drv_bme680_init(int8_t bSPInI2C){
	int32_t rslt = 0;

    if(bSPInI2C) {
        g_bme680_dev.read = user_spi_read;
        g_bme680_dev.write = user_spi_write;
        g_bme680_dev.intf = BME680_SPI_INTF;
    } else {
        g_bme680_dev.read = user_i2c_read;
        g_bme680_dev.write = user_i2c_write;
        g_bme680_dev.intf = BME680_I2C_INTF;
		rslt = hal_i2c_init(&bme680_ctx);
    }
    g_bme680_dev.delay_ms = user_delay_ms;
    g_bme680_dev.amb_temp = 25;
    g_bme680_timeout = 1000;

    rslt = bme680_init(&g_bme680_dev);
    if (BME680_OK != rslt) {
        TRACE_ERROR("BME680 device not found (returned %d)", rslt);
        return rslt;
    }
    //
    // Configuring the sensor
    //
    uint8_t set_required_settings;

    /* Set the temperature, pressure and humidity settings */
    g_bme680_dev.tph_sett.os_hum = BME680_OS_2X;
    g_bme680_dev.tph_sett.os_pres = BME680_OS_4X;
    g_bme680_dev.tph_sett.os_temp = BME680_OS_8X;
    g_bme680_dev.tph_sett.filter = BME680_FILTER_SIZE_3;

    /* Set the remaining gas sensor settings and link the heating profile */
    g_bme680_dev.gas_sett.run_gas = BME680_ENABLE_GAS_MEAS;
    /* Create a ramp heat waveform in 3 steps */
    g_bme680_dev.gas_sett.heatr_temp = 320; /* degree Celsius */
    g_bme680_dev.gas_sett.heatr_dur = 150;  /* milliseconds */

    /* Select the power mode */
    /* Must be set before writing the sensor configuration */
    g_bme680_dev.power_mode = BME680_FORCED_MODE;

    /* Set the required sensor settings needed */
    set_required_settings = BME680_OST_SEL | BME680_OSP_SEL | BME680_OSH_SEL | BME680_FILTER_SEL | BME680_GAS_SENSOR_SEL;
    /* Set the desired sensor configuration */
    rslt = bme680_set_sensor_settings(set_required_settings, &g_bme680_dev);
    if (BME680_OK != rslt) {
        TRACE_ERROR("[BME680] Could not set sensor settings (returned %d)", rslt);
        return rslt;
    }
    rslt = bme680_set_sensor_mode(&g_bme680_dev);
    if (BME680_OK != rslt) {
        TRACE_ERROR("[BME680] Could not set sensor mode (returned %d)", rslt);
        return rslt;
    }
    bme680_get_profile_dur(&g_bme680_meas_period, &g_bme680_dev);

    memset(&sensor_temp, 0, sizeof(sensor_temp));
	/* fill the sensor_temp obj parameters here */
	sensor_temp.tag        = TAG_DEV_TEMP;
	sensor_temp.path       = dev_temp_path;
	sensor_temp.io_port    = BME680_I2C_PORT;
	sensor_temp.open       = drv_temp_bosch_bme680_open;
	sensor_temp.close      = drv_temp_bosch_bme680_close;
	sensor_temp.read       = drv_temp_bosch_bme680_read;
	sensor_temp.write      = drv_temp_bosch_bme680_write;
	sensor_temp.ioctl      = drv_temp_bosch_bme680_ioctl;
	sensor_temp.irq_handle = drv_temp_bosch_bme680_irq_handle;
	rslt = sensor_create_obj(&sensor_temp);
	if (unlikely(rslt)) {
		return -1;
	}

	memset(&sensor_humi, 0, sizeof(sensor_humi));
	/* fill the sensor_humi obj parameters here */
	sensor_humi.tag        = TAG_DEV_HUMI;
	sensor_humi.path       = dev_humi_path;
	sensor_humi.io_port    = BME680_I2C_PORT;
	sensor_humi.open       = drv_humi_bosch_bme680_open;
	sensor_humi.close      = drv_humi_bosch_bme680_close;
	sensor_humi.read       = drv_humi_bosch_bme680_read;
	sensor_humi.write      = drv_humi_bosch_bme680_write;
	sensor_humi.ioctl      = drv_humi_bosch_bme680_ioctl;
	sensor_humi.irq_handle = drv_humi_bosch_bme680_irq_handle;
	rslt = sensor_create_obj(&sensor_humi);
	if (unlikely(rslt)) {
		return -1;
	}

	memset(&sensor_baro, 0, sizeof(sensor_baro));
	/* fill the sensor_baro obj parameters here */
	sensor_baro.tag        = TAG_DEV_BARO;
	sensor_baro.path       = dev_baro_path;
	sensor_baro.io_port    = BME680_I2C_PORT;
	sensor_baro.open       = drv_baro_bosch_bme680_open;
	sensor_baro.close      = drv_baro_bosch_bme680_close;
	sensor_baro.read       = drv_baro_bosch_bme680_read;
	sensor_baro.write      = drv_baro_bosch_bme680_write;
	sensor_baro.ioctl      = drv_baro_bosch_bme680_ioctl;
	sensor_baro.irq_handle = drv_baro_bosch_bme680_irq_handle;
	rslt = sensor_create_obj(&sensor_baro);
	if (unlikely(rslt)) {
		return -1;
	}

	memset(&sensor_gr, 0, sizeof(sensor_gr));
	/* fill the sensor_gr obj parameters here */
	sensor_gr.tag        = TAG_DEV_GR;
	sensor_gr.path       = dev_gas_resistance_path;
	sensor_gr.io_port    = BME680_I2C_PORT;
	sensor_gr.open       = drv_gr_bosch_bme680_open;
	sensor_gr.close      = drv_gr_bosch_bme680_close;
	sensor_gr.read       = drv_gr_bosch_bme680_read;
	sensor_gr.write      = drv_gr_bosch_bme680_write;
	sensor_gr.ioctl      = drv_gr_bosch_bme680_ioctl;
	sensor_gr.irq_handle = drv_gr_bosch_bme680_irq_handle;
	rslt = sensor_create_obj(&sensor_gr);
	if (unlikely(rslt)) {
		return -1;
	}

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return BME680_OK;

}
	
/*!
 * @brief This internal API is used to read the calibrated data from the sensor.
 *
 * This function is used to retrieve the calibration
 * data from the image registers of the sensor.
 *
 * @note Registers 89h  to A1h for calibration data 1 to 24
 *        from bit 0 to 7
 * @note Registers E1h to F0h for calibration data 25 to 40
 *        from bit 0 to 7
 * @param[in] dev	:Structure instance of bme680_dev.
 *
 * @return Result of API execution status.
 * @retval zero -> Success / +ve value -> Warning / -ve value -> Error
 */
static int8_t get_calib_data(struct bme680_dev *dev);

/*!
 * @brief This internal API is used to set the gas configuration of the sensor.
 *
 * @param[in] dev	:Structure instance of bme680_dev.
 *
 * @return Result of API execution status.
 * @retval zero -> Success / +ve value -> Warning / -ve value -> Error
 */
static int8_t set_gas_config(struct bme680_dev *dev);

/*!
 * @brief This internal API is used to get the gas configuration of the sensor.
 * @note heatr_temp and heatr_dur values are currently register data
 * and not the actual values set
 *
 * @param[in] dev	:Structure instance of bme680_dev.
 *
 * @return Result of API execution status.
 * @retval zero -> Success / +ve value -> Warning / -ve value -> Error
 */
static int8_t get_gas_config(struct bme680_dev *dev);

/*!
 * @brief This internal API is used to calculate the Heat duration value.
 *
 * @param[in] dur	:Value of the duration to be shared.
 *
 * @return uint8_t threshold duration after calculation.
 */
static uint8_t calc_heater_dur(uint16_t dur);

#ifndef BME680_FLOAT_POINT_COMPENSATION

/*!
 * @brief This internal API is used to calculate the temperature value.
 *
 * @param[in] dev	:Structure instance of bme680_dev.
 * @param[in] temp_adc	:Contains the temperature ADC value .
 *
 * @return uint32_t calculated temperature.
 */
static int16_t calc_temperature(uint32_t temp_adc, struct bme680_dev *dev);

/*!
 * @brief This internal API is used to calculate the pressure value.
 *
 * @param[in] dev	:Structure instance of bme680_dev.
 * @param[in] pres_adc	:Contains the pressure ADC value .
 *
 * @return uint32_t calculated pressure.
 */
static uint32_t calc_pressure(uint32_t pres_adc, const struct bme680_dev *dev);

/*!
 * @brief This internal API is used to calculate the humidity value.
 *
 * @param[in] dev	:Structure instance of bme680_dev.
 * @param[in] hum_adc	:Contains the humidity ADC value.
 *
 * @return uint32_t calculated humidity.
 */
static uint32_t calc_humidity(uint16_t hum_adc, const struct bme680_dev *dev);

/*!
 * @brief This internal API is used to calculate the Gas Resistance value.
 *
 * @param[in] dev		:Structure instance of bme680_dev.
 * @param[in] gas_res_adc	:Contains the Gas Resistance ADC value.
 * @param[in] gas_range		:Contains the range of gas values.
 *
 * @return uint32_t calculated gas resistance.
 */
static uint32_t calc_gas_resistance(uint16_t gas_res_adc, uint8_t gas_range, const struct bme680_dev *dev);

/*!
 * @brief This internal API is used to calculate the Heat Resistance value.
 *
 * @param[in] dev	: Structure instance of bme680_dev
 * @param[in] temp	: Contains the target temperature value.
 *
 * @return uint8_t calculated heater resistance.
 */
static uint8_t calc_heater_res(uint16_t temp, const struct bme680_dev *dev);

#else
/*!
 * @brief This internal API is used to calculate the
 * temperature value value in float format
 *
 * @param[in] dev	:Structure instance of bme680_dev.
 * @param[in] temp_adc	:Contains the temperature ADC value .
 *
 * @return Calculated temperature in float
 */
static float calc_temperature(uint32_t temp_adc, struct bme680_dev *dev);

/*!
 * @brief This internal API is used to calculate the
 * pressure value value in float format
 *
 * @param[in] dev	:Structure instance of bme680_dev.
 * @param[in] pres_adc	:Contains the pressure ADC value .
 *
 * @return Calculated pressure in float.
 */
static float calc_pressure(uint32_t pres_adc, const struct bme680_dev *dev);

/*!
 * @brief This internal API is used to calculate the
 * humidity value value in float format
 *
 * @param[in] dev	:Structure instance of bme680_dev.
 * @param[in] hum_adc	:Contains the humidity ADC value.
 *
 * @return Calculated humidity in float.
 */
static float calc_humidity(uint16_t hum_adc, const struct bme680_dev *dev);

/*!
 * @brief This internal API is used to calculate the
 * gas resistance value value in float format
 *
 * @param[in] dev		:Structure instance of bme680_dev.
 * @param[in] gas_res_adc	:Contains the Gas Resistance ADC value.
 * @param[in] gas_range		:Contains the range of gas values.
 *
 * @return Calculated gas resistance in float.
 */
static float calc_gas_resistance(uint16_t gas_res_adc, uint8_t gas_range, const struct bme680_dev *dev);

/*!
 * @brief This internal API is used to calculate the
 * heater resistance value in float format
 *
 * @param[in] temp	: Contains the target temperature value.
 * @param[in] dev	: Structure instance of bme680_dev.
 *
 * @return Calculated heater resistance in float.
 */
static float calc_heater_res(uint16_t temp, const struct bme680_dev *dev);

#endif

/*!
 * @brief This internal API is used to calculate the field data of sensor.
 *
 * @param[out] data :Structure instance to hold the data
 * @param[in] dev	:Structure instance of bme680_dev.
 *
 *  @return int8_t result of the field data from sensor.
 */
static int8_t read_field_data(struct bme680_field_data *data, struct bme680_dev *dev);

/*!
 * @brief This internal API is used to set the memory page
 * based on register address.
 *
 * The value of memory page
 *  value  | Description
 * --------|--------------
 *   0     | BME680_PAGE0_SPI
 *   1     | BME680_PAGE1_SPI
 *
 * @param[in] dev	:Structure instance of bme680_dev.
 * @param[in] reg_addr	:Contains the register address array.
 *
 * @return Result of API execution status
 * @retval zero -> Success / +ve value -> Warning / -ve value -> Error
 */
static int8_t set_mem_page(uint8_t reg_addr, struct bme680_dev *dev);

/*!
 * @brief This internal API is used to get the memory page based
 * on register address.
 *
 * The value of memory page
 *  value  | Description
 * --------|--------------
 *   0     | BME680_PAGE0_SPI
 *   1     | BME680_PAGE1_SPI
 *
 * @param[in] dev	:Structure instance of bme680_dev.
 *
 * @return Result of API execution status
 * @retval zero -> Success / +ve value -> Warning / -ve value -> Error
 */
static int8_t get_mem_page(struct bme680_dev *dev);

/*!
 * @brief This internal API is used to validate the device pointer for
 * null conditions.
 *
 * @param[in] dev	:Structure instance of bme680_dev.
 *
 * @return Result of API execution status
 * @retval zero -> Success / +ve value -> Warning / -ve value -> Error
 */
static int8_t null_ptr_check(const struct bme680_dev *dev);

/*!
 * @brief This internal API is used to check the boundary
 * conditions.
 *
 * @param[in] value	:pointer to the value.
 * @param[in] min	:minimum value.
 * @param[in] max	:maximum value.
 * @param[in] dev	:Structure instance of bme680_dev.
 *
 * @return Result of API execution status
 * @retval zero -> Success / +ve value -> Warning / -ve value -> Error
 */
static int8_t boundary_check(uint8_t *value, uint8_t min, uint8_t max, struct bme680_dev *dev);

/****************** Global Function Definitions *******************************/
/*!
 *@brief This API is the entry point.
 *It reads the chip-id and calibration data from the sensor.
 */
int8_t bme680_init(struct bme680_dev *dev)
{
	int8_t rslt;
	/* Check for null pointer in the device structure*/
	rslt = null_ptr_check(dev);
	if (rslt == BME680_OK) {
		/* Soft reset to restore it to default values*/
		rslt = bme680_soft_reset(dev);
		if (rslt == BME680_OK) {
			rslt = bme680_get_regs(BME680_CHIP_ID_ADDR, &dev->chip_id, 1, dev);
			if (rslt == BME680_OK) {
				TRACE_INFO("[BME680] Chip id: %02x", dev->chip_id);
				if (dev->chip_id == BME680_CHIP_ID) {
					rslt = get_calib_data(dev);
				} else {
					rslt = BME680_E_DEV_NOT_FOUND;
				}
			}
		} else {
			TRACE_ERROR("[BME680] Soft reset unsuccessful (returned %d)",rslt);
		}
	}

	return rslt;
}

/*!
 * @brief This API reads the data from the given register address of the sensor.
 */
int8_t bme680_get_regs(uint8_t reg_addr, uint8_t *reg_data, uint16_t len, struct bme680_dev *dev)
{
	int8_t rslt;

	/* Check for null pointer in the device structure*/
	rslt = null_ptr_check(dev);
	if (rslt == BME680_OK) {
		if (dev->intf == BME680_SPI_INTF) {
			/* Set the memory page */
			rslt = set_mem_page(reg_addr, dev);
			if (rslt == BME680_OK)
				reg_addr = reg_addr | BME680_SPI_RD_MSK;
		}
		dev->com_rslt = dev->read(dev->dev_id, reg_addr, reg_data, len);
		if (dev->com_rslt != 0)
			rslt = BME680_E_COM_FAIL;
	}
	if(rslt != 0)
		TRACE_ERROR("bme680_get_regs failed (addr = 0x%02x)",reg_addr);

	return rslt;
}

/*!
 * @brief This API writes the given data to the register address
 * of the sensor.
 */
int8_t bme680_set_regs(const uint8_t *reg_addr, const uint8_t *reg_data, uint8_t len, struct bme680_dev *dev)
{
	int8_t rslt;
	/* Length of the temporary buffer is 2*(length of register)*/
	uint8_t tmp_buff[BME680_TMP_BUFFER_LENGTH] = { 0 };
	uint16_t index;

	/* Check for null pointer in the device structure*/
	rslt = null_ptr_check(dev);
	if (rslt == BME680_OK) {
		if ((len > 0) && (len < BME680_TMP_BUFFER_LENGTH / 2)) {
			/* Interleave the 2 arrays */
			for (index = 0; index < len; index++) {
				if (dev->intf == BME680_SPI_INTF) {
					/* Set the memory page */
					rslt = set_mem_page(reg_addr[index], dev);
					tmp_buff[(2 * index)] = reg_addr[index] & BME680_SPI_WR_MSK;
				} else {
					tmp_buff[(2 * index)] = reg_addr[index];
				}
				tmp_buff[(2 * index) + 1] = reg_data[index];
			}
			/* Write the interleaved array */
			if (rslt == BME680_OK) {
				dev->com_rslt = dev->write(dev->dev_id, tmp_buff[0], &tmp_buff[1], (2 * len) - 1);
				if (dev->com_rslt != 0)
					rslt = BME680_E_COM_FAIL;
			}
		} else {
			rslt = BME680_E_INVALID_LENGTH;
		}
	}

	if(rslt != 0)
		TRACE_ERROR("bme680_set_regs failed (addr = 0x%02x)",*reg_addr);

	return rslt;
}

/*!
 * @brief This API performs the soft reset of the sensor.
 */
int8_t bme680_soft_reset(struct bme680_dev *dev)
{
	int8_t rslt;
	uint8_t reg_addr = BME680_SOFT_RESET_ADDR;
	/* 0xb6 is the soft reset command */
	uint8_t soft_rst_cmd = BME680_SOFT_RESET_CMD;

	/* Check for null pointer in the device structure*/
	rslt = null_ptr_check(dev);
	if (rslt == BME680_OK) {
		if (dev->intf == BME680_SPI_INTF)
			rslt = get_mem_page(dev);

		/* Reset the device */
		if (rslt == BME680_OK) {
			rslt = bme680_set_regs(&reg_addr, &soft_rst_cmd, 1, dev);
			/* Wait for 5ms */
			dev->delay_ms(BME680_RESET_PERIOD);

			if (rslt == BME680_OK) {
				/* After reset get the memory page */
				if (dev->intf == BME680_SPI_INTF)
					rslt = get_mem_page(dev);
			}
		}
	}

	return rslt;
}

/*!
 * @brief This API is used to set the oversampling, filter and T,P,H, gas selection
 * settings in the sensor.
 */
int8_t bme680_set_sensor_settings(uint16_t desired_settings, struct bme680_dev *dev)
{
	int8_t rslt;
	uint8_t reg_addr;
	uint8_t data = 0;
	uint8_t count = 0;
	uint8_t reg_array[BME680_REG_BUFFER_LENGTH] = { 0 };
	uint8_t data_array[BME680_REG_BUFFER_LENGTH] = { 0 };
	uint8_t intended_power_mode = dev->power_mode; /* Save intended power mode */

	/* Check for null pointer in the device structure*/
	rslt = null_ptr_check(dev);
	if (rslt == BME680_OK) {
		if (desired_settings & BME680_GAS_MEAS_SEL) {
			rslt = set_gas_config(dev);
			if(rslt != BME680_OK) {
				TRACE_ERROR("[BME680] set_gas_config failed");
				return rslt;
			}
				
		}
			
		dev->power_mode = BME680_SLEEP_MODE;
		if (rslt == BME680_OK)
			rslt = bme680_set_sensor_mode(dev);
		else {
			TRACE_ERROR("[BME680] set_sensor_mode failed");
			return rslt;
		}

		/* Selecting the filter */
		if (desired_settings & BME680_FILTER_SEL) {
			rslt = boundary_check(&dev->tph_sett.filter, BME680_FILTER_SIZE_0, BME680_FILTER_SIZE_127, dev);
			reg_addr = BME680_CONF_ODR_FILT_ADDR;

			if (rslt == BME680_OK)
				rslt = bme680_get_regs(reg_addr, &data, 1, dev);
			else {
				TRACE_ERROR("[BME680] get_regs failed");
				return rslt;
			}
				
			if (desired_settings & BME680_FILTER_SEL)
				data = BME680_SET_BITS(data, BME680_FILTER, dev->tph_sett.filter);

			reg_array[count] = reg_addr; /* Append configuration */
			data_array[count] = data;
			count++;
		}

		/* Selecting heater control for the sensor */
		if (desired_settings & BME680_HCNTRL_SEL) {
			rslt = boundary_check(&dev->gas_sett.heatr_ctrl, BME680_ENABLE_HEATER,
				BME680_DISABLE_HEATER, dev);
			if(rslt != BME680_OK) {
				TRACE_ERROR("[BME680] boundary_check failed");
				return rslt;
			}
			reg_addr = BME680_CONF_HEAT_CTRL_ADDR;

			if (rslt == BME680_OK)
				rslt = bme680_get_regs(reg_addr, &data, 1, dev);
			else {
				TRACE_ERROR("[BME680] get_regs failed");
				return rslt;
			}
			data = BME680_SET_BITS_POS_0(data, BME680_HCTRL, dev->gas_sett.heatr_ctrl);

			reg_array[count] = reg_addr; /* Append configuration */
			data_array[count] = data;
			count++;
		}

		/* Selecting heater T,P oversampling for the sensor */
		if (desired_settings & (BME680_OST_SEL | BME680_OSP_SEL)) {
			rslt = boundary_check(&dev->tph_sett.os_temp, BME680_OS_NONE, BME680_OS_16X, dev);
			reg_addr = BME680_CONF_T_P_MODE_ADDR;

			if (rslt == BME680_OK)
				rslt = bme680_get_regs(reg_addr, &data, 1, dev);
			else {
				TRACE_ERROR("[BME680] get_regs failed");
				return rslt;

			}
				
			if (desired_settings & BME680_OST_SEL)
				data = BME680_SET_BITS(data, BME680_OST, dev->tph_sett.os_temp);

			if (desired_settings & BME680_OSP_SEL)
				data = BME680_SET_BITS(data, BME680_OSP, dev->tph_sett.os_pres);

			reg_array[count] = reg_addr;
			data_array[count] = data;
			count++;
		}

		/* Selecting humidity oversampling for the sensor */
		if (desired_settings & BME680_OSH_SEL) {
			rslt = boundary_check(&dev->tph_sett.os_hum, BME680_OS_NONE, BME680_OS_16X, dev);
			reg_addr = BME680_CONF_OS_H_ADDR;

			if (rslt == BME680_OK)
				rslt = bme680_get_regs(reg_addr, &data, 1, dev);
			else {
				TRACE_ERROR("[BME680] boundary_check failed");
				return rslt;
			}
			data = BME680_SET_BITS_POS_0(data, BME680_OSH, dev->tph_sett.os_hum);

			reg_array[count] = reg_addr; /* Append configuration */
			data_array[count] = data;
			count++;
		}

		/* Selecting the runGas and NB conversion settings for the sensor */
		if (desired_settings & (BME680_RUN_GAS_SEL | BME680_NBCONV_SEL)) {
			rslt = boundary_check(&dev->gas_sett.run_gas, BME680_RUN_GAS_DISABLE,
				BME680_RUN_GAS_ENABLE, dev);
			if (rslt == BME680_OK) {
				/* Validate boundary conditions */
				rslt = boundary_check(&dev->gas_sett.nb_conv, BME680_NBCONV_MIN,
					BME680_NBCONV_MAX, dev);
				if(rslt != BME680_OK) {
					TRACE_ERROR("[BME680] boundary_check failed");
					return rslt;
				}
			} else {
				TRACE_ERROR("[BME680] boundary_check failed");
				return rslt;
			}
				
			reg_addr = BME680_CONF_ODR_RUN_GAS_NBC_ADDR;

			if (rslt == BME680_OK)
				rslt = bme680_get_regs(reg_addr, &data, 1, dev);
			else {
				TRACE_ERROR("[BME680] get_regs failed");
				return rslt;
			}

			if (desired_settings & BME680_RUN_GAS_SEL)
				data = BME680_SET_BITS(data, BME680_RUN_GAS, dev->gas_sett.run_gas);

			if (desired_settings & BME680_NBCONV_SEL)
				data = BME680_SET_BITS_POS_0(data, BME680_NBCONV, dev->gas_sett.nb_conv);

			reg_array[count] = reg_addr; /* Append configuration */
			data_array[count] = data;
			count++;
		}

		if (rslt == BME680_OK)
			rslt = bme680_set_regs(reg_array, data_array, count, dev);
		else {
			TRACE_ERROR("[BME680] set_regs failed");
			return rslt;
		}

		/* Restore previous intended power mode */
		dev->power_mode = intended_power_mode;
	}

	return rslt;
}

/*!
 * @brief This API is used to get the oversampling, filter and T,P,H, gas selection
 * settings in the sensor.
 */
int8_t bme680_get_sensor_settings(uint16_t desired_settings, struct bme680_dev *dev)
{
	int8_t rslt;
	/* starting address of the register array for burst read*/
	uint8_t reg_addr = BME680_CONF_HEAT_CTRL_ADDR;
	uint8_t data_array[BME680_REG_BUFFER_LENGTH] = { 0 };

	/* Check for null pointer in the device structure*/
	rslt = null_ptr_check(dev);
	if (rslt == BME680_OK) {
		rslt = bme680_get_regs(reg_addr, data_array, BME680_REG_BUFFER_LENGTH, dev);

		if (rslt == BME680_OK) {
			if (desired_settings & BME680_GAS_MEAS_SEL)
				rslt = get_gas_config(dev);

			/* get the T,P,H ,Filter,ODR settings here */
			if (desired_settings & BME680_FILTER_SEL)
				dev->tph_sett.filter = BME680_GET_BITS(data_array[BME680_REG_FILTER_INDEX],
					BME680_FILTER);

			if (desired_settings & (BME680_OST_SEL | BME680_OSP_SEL)) {
				dev->tph_sett.os_temp = BME680_GET_BITS(data_array[BME680_REG_TEMP_INDEX], BME680_OST);
				dev->tph_sett.os_pres = BME680_GET_BITS(data_array[BME680_REG_PRES_INDEX], BME680_OSP);
			}

			if (desired_settings & BME680_OSH_SEL)
				dev->tph_sett.os_hum = BME680_GET_BITS_POS_0(data_array[BME680_REG_HUM_INDEX],
					BME680_OSH);

			/* get the gas related settings */
			if (desired_settings & BME680_HCNTRL_SEL)
				dev->gas_sett.heatr_ctrl = BME680_GET_BITS_POS_0(data_array[BME680_REG_HCTRL_INDEX],
					BME680_HCTRL);

			if (desired_settings & (BME680_RUN_GAS_SEL | BME680_NBCONV_SEL)) {
				dev->gas_sett.nb_conv = BME680_GET_BITS_POS_0(data_array[BME680_REG_NBCONV_INDEX],
					BME680_NBCONV);
				dev->gas_sett.run_gas = BME680_GET_BITS(data_array[BME680_REG_RUN_GAS_INDEX],
					BME680_RUN_GAS);
			}
		}
	} else {
		rslt = BME680_E_NULL_PTR;
	}

	return rslt;
}

/*!
 * @brief This API is used to set the power mode of the sensor.
 */
int8_t bme680_set_sensor_mode(struct bme680_dev *dev)
{
	int8_t rslt;
	uint8_t tmp_pow_mode;
	uint8_t pow_mode = 0;
	uint8_t reg_addr = BME680_CONF_T_P_MODE_ADDR;

	/* Check for null pointer in the device structure*/
	rslt = null_ptr_check(dev);
	if (rslt == BME680_OK) {
		/* Call repeatedly until in sleep */
		do {
			rslt = bme680_get_regs(BME680_CONF_T_P_MODE_ADDR, &tmp_pow_mode, 1, dev);
			if (rslt == BME680_OK) {
				/* Put to sleep before changing mode */
				pow_mode = (tmp_pow_mode & BME680_MODE_MSK);

				if (pow_mode != BME680_SLEEP_MODE) {
					tmp_pow_mode = tmp_pow_mode & (~BME680_MODE_MSK); /* Set to sleep */
					rslt = bme680_set_regs(&reg_addr, &tmp_pow_mode, 1, dev);
					dev->delay_ms(BME680_POLL_PERIOD_MS);
				}
			}
		} while (pow_mode != BME680_SLEEP_MODE);

		/* Already in sleep */
		if (dev->power_mode != BME680_SLEEP_MODE) {
			tmp_pow_mode = (tmp_pow_mode & ~BME680_MODE_MSK) | (dev->power_mode & BME680_MODE_MSK);
			if (rslt == BME680_OK)
				rslt = bme680_set_regs(&reg_addr, &tmp_pow_mode, 1, dev);
		}
	} else {
		TRACE_ERROR("Nullpointer exception.");
	}

	return rslt;
}

/*!
 * @brief This API is used to get the power mode of the sensor.
 */
int8_t bme680_get_sensor_mode(struct bme680_dev *dev)
{
	int8_t rslt;
	uint8_t mode;

	/* Check for null pointer in the device structure*/
	rslt = null_ptr_check(dev);
	if (rslt == BME680_OK) {
		rslt = bme680_get_regs(BME680_CONF_T_P_MODE_ADDR, &mode, 1, dev);
		/* Masking the other register bit info*/
		dev->power_mode = mode & BME680_MODE_MSK;
	}

	return rslt;
}

/*!
 * @brief This API is used to set the profile duration of the sensor.
 */
void bme680_set_profile_dur(uint16_t duration, struct bme680_dev *dev)
{
	uint32_t tph_dur; /* Calculate in us */
	uint32_t meas_cycles;
	uint8_t os_to_meas_cycles[6] = {0, 1, 2, 4, 8, 16};

	meas_cycles = os_to_meas_cycles[dev->tph_sett.os_temp];
	meas_cycles += os_to_meas_cycles[dev->tph_sett.os_pres];
	meas_cycles += os_to_meas_cycles[dev->tph_sett.os_hum];

	/* TPH measurement duration */
	tph_dur = meas_cycles * UINT32_C(1963);
	tph_dur += UINT32_C(477 * 4); /* TPH switching duration */
	tph_dur += UINT32_C(477 * 5); /* Gas measurement duration */
	tph_dur += UINT32_C(500); /* Get it to the closest whole number.*/
	tph_dur /= UINT32_C(1000); /* Convert to ms */

	tph_dur += UINT32_C(1); /* Wake up duration of 1ms */
	/* The remaining time should be used for heating */
	dev->gas_sett.heatr_dur = duration - (uint16_t) tph_dur;
}

/*!
 * @brief This API is used to get the profile duration of the sensor.
 */
void bme680_get_profile_dur(uint16_t *duration, const struct bme680_dev *dev)
{
	uint32_t tph_dur; /* Calculate in us */
	uint32_t meas_cycles;
	uint8_t os_to_meas_cycles[6] = {0, 1, 2, 4, 8, 16};

	meas_cycles = os_to_meas_cycles[dev->tph_sett.os_temp];
	meas_cycles += os_to_meas_cycles[dev->tph_sett.os_pres];
	meas_cycles += os_to_meas_cycles[dev->tph_sett.os_hum];

	/* TPH measurement duration */
	tph_dur = meas_cycles * UINT32_C(1963);
	tph_dur += UINT32_C(477 * 4); /* TPH switching duration */
	tph_dur += UINT32_C(477 * 5); /* Gas measurement duration */
	tph_dur += UINT32_C(500); /* Get it to the closest whole number.*/
	tph_dur /= UINT32_C(1000); /* Convert to ms */

	tph_dur += UINT32_C(1); /* Wake up duration of 1ms */

	*duration = (uint16_t) tph_dur;

	/* Get the gas duration only when the run gas is enabled */
	if (dev->gas_sett.run_gas) {
		/* The remaining time should be used for heating */
		*duration += dev->gas_sett.heatr_dur;
	}
}

/*!
 * @brief This API reads the pressure, temperature and humidity and gas data
 * from the sensor, compensates the data and store it in the bme680_data
 * structure instance passed by the user.
 */
int8_t bme680_get_sensor_data(struct bme680_field_data *data, struct bme680_dev *dev)
{
	int8_t rslt;

	/* Check for null pointer in the device structure*/
	rslt = null_ptr_check(dev);
	if (rslt == BME680_OK) {
		/* Reading the sensor data in forced mode only */
		rslt = read_field_data(data, dev);
		if (rslt == BME680_OK) {
			if (data->status & BME680_NEW_DATA_MSK)
				dev->new_fields = 1;
			else
				dev->new_fields = 0;
		}
	}

	return rslt;
}

/*!
 * @brief This internal API is used to read the calibrated data from the sensor.
 */
static int8_t get_calib_data(struct bme680_dev *dev)
{
	int8_t rslt;
	uint8_t coeff_array[BME680_COEFF_SIZE] = { 0 };
	uint8_t temp_var = 0; /* Temporary variable */

	/* Check for null pointer in the device structure*/
	rslt = null_ptr_check(dev);
	if (rslt == BME680_OK) {
		rslt = bme680_get_regs(BME680_COEFF_ADDR1, coeff_array, BME680_COEFF_ADDR1_LEN, dev);
		/* Append the second half in the same array */
		if (rslt == BME680_OK)
			rslt = bme680_get_regs(BME680_COEFF_ADDR2, &coeff_array[BME680_COEFF_ADDR1_LEN]
			, BME680_COEFF_ADDR2_LEN, dev);

		/* Temperature related coefficients */
		dev->calib.par_t1 = (uint16_t) (BME680_CONCAT_BYTES(coeff_array[BME680_T1_MSB_REG],
			coeff_array[BME680_T1_LSB_REG]));
		dev->calib.par_t2 = (int16_t) (BME680_CONCAT_BYTES(coeff_array[BME680_T2_MSB_REG],
			coeff_array[BME680_T2_LSB_REG]));
		dev->calib.par_t3 = (int8_t) (coeff_array[BME680_T3_REG]);

		/* Pressure related coefficients */
		dev->calib.par_p1 = (uint16_t) (BME680_CONCAT_BYTES(coeff_array[BME680_P1_MSB_REG],
			coeff_array[BME680_P1_LSB_REG]));
		dev->calib.par_p2 = (int16_t) (BME680_CONCAT_BYTES(coeff_array[BME680_P2_MSB_REG],
			coeff_array[BME680_P2_LSB_REG]));
		dev->calib.par_p3 = (int8_t) coeff_array[BME680_P3_REG];
		dev->calib.par_p4 = (int16_t) (BME680_CONCAT_BYTES(coeff_array[BME680_P4_MSB_REG],
			coeff_array[BME680_P4_LSB_REG]));
		dev->calib.par_p5 = (int16_t) (BME680_CONCAT_BYTES(coeff_array[BME680_P5_MSB_REG],
			coeff_array[BME680_P5_LSB_REG]));
		dev->calib.par_p6 = (int8_t) (coeff_array[BME680_P6_REG]);
		dev->calib.par_p7 = (int8_t) (coeff_array[BME680_P7_REG]);
		dev->calib.par_p8 = (int16_t) (BME680_CONCAT_BYTES(coeff_array[BME680_P8_MSB_REG],
			coeff_array[BME680_P8_LSB_REG]));
		dev->calib.par_p9 = (int16_t) (BME680_CONCAT_BYTES(coeff_array[BME680_P9_MSB_REG],
			coeff_array[BME680_P9_LSB_REG]));
		dev->calib.par_p10 = (uint8_t) (coeff_array[BME680_P10_REG]);

		/* Humidity related coefficients */
		dev->calib.par_h1 = (uint16_t) (((uint16_t) coeff_array[BME680_H1_MSB_REG] << BME680_HUM_REG_SHIFT_VAL)
			| (coeff_array[BME680_H1_LSB_REG] & BME680_BIT_H1_DATA_MSK));
		dev->calib.par_h2 = (uint16_t) (((uint16_t) coeff_array[BME680_H2_MSB_REG] << BME680_HUM_REG_SHIFT_VAL)
			| ((coeff_array[BME680_H2_LSB_REG]) >> BME680_HUM_REG_SHIFT_VAL));
		dev->calib.par_h3 = (int8_t) coeff_array[BME680_H3_REG];
		dev->calib.par_h4 = (int8_t) coeff_array[BME680_H4_REG];
		dev->calib.par_h5 = (int8_t) coeff_array[BME680_H5_REG];
		dev->calib.par_h6 = (uint8_t) coeff_array[BME680_H6_REG];
		dev->calib.par_h7 = (int8_t) coeff_array[BME680_H7_REG];

		/* Gas heater related coefficients */
		dev->calib.par_gh1 = (int8_t) coeff_array[BME680_GH1_REG];
		dev->calib.par_gh2 = (int16_t) (BME680_CONCAT_BYTES(coeff_array[BME680_GH2_MSB_REG],
			coeff_array[BME680_GH2_LSB_REG]));
		dev->calib.par_gh3 = (int8_t) coeff_array[BME680_GH3_REG];

		/* Other coefficients */
		if (rslt == BME680_OK) {
			rslt = bme680_get_regs(BME680_ADDR_RES_HEAT_RANGE_ADDR, &temp_var, 1, dev);

			dev->calib.res_heat_range = ((temp_var & BME680_RHRANGE_MSK) / 16);
			if (rslt == BME680_OK) {
				rslt = bme680_get_regs(BME680_ADDR_RES_HEAT_VAL_ADDR, &temp_var, 1, dev);

				dev->calib.res_heat_val = (int8_t) temp_var;
				if (rslt == BME680_OK)
					rslt = bme680_get_regs(BME680_ADDR_RANGE_SW_ERR_ADDR, &temp_var, 1, dev);
			}
		}
		dev->calib.range_sw_err = ((int8_t) temp_var & (int8_t) BME680_RSERROR_MSK) / 16;
	}

	return rslt;
}

/*!
 * @brief This internal API is used to set the gas configuration of the sensor.
 */
static int8_t set_gas_config(struct bme680_dev *dev)
{
	int8_t rslt;

	/* Check for null pointer in the device structure*/
	rslt = null_ptr_check(dev);
	if (rslt == BME680_OK) {

		uint8_t reg_addr[2] = {0};
		uint8_t reg_data[2] = {0};

		if (dev->power_mode == BME680_FORCED_MODE) {
			reg_addr[0] = BME680_RES_HEAT0_ADDR;
			reg_data[0] = calc_heater_res(dev->gas_sett.heatr_temp, dev);
			reg_addr[1] = BME680_GAS_WAIT0_ADDR;
			reg_data[1] = calc_heater_dur(dev->gas_sett.heatr_dur);
			dev->gas_sett.nb_conv = 0;
		} else {
			rslt = BME680_W_DEFINE_PWR_MODE;
		}
		if (rslt == BME680_OK)
			rslt = bme680_set_regs(reg_addr, reg_data, 2, dev);
	}

	return rslt;
}

/*!
 * @brief This internal API is used to get the gas configuration of the sensor.
 * @note heatr_temp and heatr_dur values are currently register data
 * and not the actual values set
 */
static int8_t get_gas_config(struct bme680_dev *dev)
{
	int8_t rslt;
	/* starting address of the register array for burst read*/
	uint8_t reg_addr1 = BME680_ADDR_SENS_CONF_START;
	uint8_t reg_addr2 = BME680_ADDR_GAS_CONF_START;
	uint8_t reg_data = 0;

	/* Check for null pointer in the device structure*/
	rslt = null_ptr_check(dev);
	if (rslt == BME680_OK) {
		if (BME680_SPI_INTF == dev->intf) {
			/* Memory page switch the SPI address*/
			rslt = set_mem_page(reg_addr1, dev);
		}

		if (rslt == BME680_OK) {
			rslt = bme680_get_regs(reg_addr1, &reg_data, 1, dev);
			if (rslt == BME680_OK) {
				dev->gas_sett.heatr_temp = reg_data;
				rslt = bme680_get_regs(reg_addr2, &reg_data, 1, dev);
				if (rslt == BME680_OK) {
					/* Heating duration register value */
					dev->gas_sett.heatr_dur = reg_data;
				}
			}
		}
	}

	return rslt;
}

#ifndef BME680_FLOAT_POINT_COMPENSATION

/*!
 * @brief This internal API is used to calculate the temperature value.
 */
static int16_t calc_temperature(uint32_t temp_adc, struct bme680_dev *dev)
{
	int64_t var1;
	int64_t var2;
	int64_t var3;
	int16_t calc_temp;

	var1 = ((int32_t) temp_adc >> 3) - ((int32_t) dev->calib.par_t1 << 1);
	var2 = (var1 * (int32_t) dev->calib.par_t2) >> 11;
	var3 = ((var1 >> 1) * (var1 >> 1)) >> 12;
	var3 = ((var3) * ((int32_t) dev->calib.par_t3 << 4)) >> 14;
	dev->calib.t_fine = (int32_t) (var2 + var3);
	calc_temp = (int16_t) (((dev->calib.t_fine * 5) + 128) >> 8);

	return calc_temp;
}

/*!
 * @brief This internal API is used to calculate the pressure value.
 */
static uint32_t calc_pressure(uint32_t pres_adc, const struct bme680_dev *dev)
{
	int32_t var1;
	int32_t var2;
	int32_t var3;
	int32_t pressure_comp;

	var1 = (((int32_t)dev->calib.t_fine) >> 1) - 64000;
	var2 = ((((var1 >> 2) * (var1 >> 2)) >> 11) *
		(int32_t)dev->calib.par_p6) >> 2;
	var2 = var2 + ((var1 * (int32_t)dev->calib.par_p5) << 1);
	var2 = (var2 >> 2) + ((int32_t)dev->calib.par_p4 << 16);
	var1 = (((((var1 >> 2) * (var1 >> 2)) >> 13) *
		((int32_t)dev->calib.par_p3 << 5)) >> 3) +
		(((int32_t)dev->calib.par_p2 * var1) >> 1);
	var1 = var1 >> 18;
	var1 = ((32768 + var1) * (int32_t)dev->calib.par_p1) >> 15;
	pressure_comp = 1048576 - pres_adc;
	pressure_comp = (int32_t)((pressure_comp - (var2 >> 12)) * ((uint32_t)3125));
	if (pressure_comp >= BME680_MAX_OVERFLOW_VAL)
		pressure_comp = ((pressure_comp / var1) << 1);
	else
		pressure_comp = ((pressure_comp << 1) / var1);
	var1 = ((int32_t)dev->calib.par_p9 * (int32_t)(((pressure_comp >> 3) *
		(pressure_comp >> 3)) >> 13)) >> 12;
	var2 = ((int32_t)(pressure_comp >> 2) *
		(int32_t)dev->calib.par_p8) >> 13;
	var3 = ((int32_t)(pressure_comp >> 8) * (int32_t)(pressure_comp >> 8) *
		(int32_t)(pressure_comp >> 8) *
		(int32_t)dev->calib.par_p10) >> 17;

	pressure_comp = (int32_t)(pressure_comp) + ((var1 + var2 + var3 +
		((int32_t)dev->calib.par_p7 << 7)) >> 4);

	return (uint32_t)pressure_comp;

}

/*!
 * @brief This internal API is used to calculate the humidity value.
 */
static uint32_t calc_humidity(uint16_t hum_adc, const struct bme680_dev *dev)
{
	int32_t var1;
	int32_t var2;
	int32_t var3;
	int32_t var4;
	int32_t var5;
	int32_t var6;
	int32_t temp_scaled;
	int32_t calc_hum;

	temp_scaled = (((int32_t) dev->calib.t_fine * 5) + 128) >> 8;
	var1 = (int32_t) (hum_adc - ((int32_t) ((int32_t) dev->calib.par_h1 * 16)))
		- (((temp_scaled * (int32_t) dev->calib.par_h3) / ((int32_t) 100)) >> 1);
	var2 = ((int32_t) dev->calib.par_h2
		* (((temp_scaled * (int32_t) dev->calib.par_h4) / ((int32_t) 100))
			+ (((temp_scaled * ((temp_scaled * (int32_t) dev->calib.par_h5) / ((int32_t) 100))) >> 6)
				/ ((int32_t) 100)) + (int32_t) (1 << 14))) >> 10;
	var3 = var1 * var2;
	var4 = (int32_t) dev->calib.par_h6 << 7;
	var4 = ((var4) + ((temp_scaled * (int32_t) dev->calib.par_h7) / ((int32_t) 100))) >> 4;
	var5 = ((var3 >> 14) * (var3 >> 14)) >> 10;
	var6 = (var4 * var5) >> 1;
	calc_hum = (((var3 + var6) >> 10) * ((int32_t) 1000)) >> 12;

	if (calc_hum > 100000) /* Cap at 100%rH */
		calc_hum = 100000;
	else if (calc_hum < 0)
		calc_hum = 0;

	return (uint32_t) calc_hum;
}

/*!
 * @brief This internal API is used to calculate the Gas Resistance value.
 */
static uint32_t calc_gas_resistance(uint16_t gas_res_adc, uint8_t gas_range, const struct bme680_dev *dev)
{
	int64_t var1;
	uint64_t var2;
	int64_t var3;
	uint32_t calc_gas_res;
	/**Look up table 1 for the possible gas range values */
	uint32_t lookupTable1[16] = { UINT32_C(2147483647), UINT32_C(2147483647), UINT32_C(2147483647), UINT32_C(2147483647),
		UINT32_C(2147483647), UINT32_C(2126008810), UINT32_C(2147483647), UINT32_C(2130303777),
		UINT32_C(2147483647), UINT32_C(2147483647), UINT32_C(2143188679), UINT32_C(2136746228),
		UINT32_C(2147483647), UINT32_C(2126008810), UINT32_C(2147483647), UINT32_C(2147483647) };
	/**Look up table 2 for the possible gas range values */
	uint32_t lookupTable2[16] = { UINT32_C(4096000000), UINT32_C(2048000000), UINT32_C(1024000000), UINT32_C(512000000),
		UINT32_C(255744255), UINT32_C(127110228), UINT32_C(64000000), UINT32_C(32258064), UINT32_C(16016016),
		UINT32_C(8000000), UINT32_C(4000000), UINT32_C(2000000), UINT32_C(1000000), UINT32_C(500000),
		UINT32_C(250000), UINT32_C(125000) };

	var1 = (int64_t) ((1340 + (5 * (int64_t) dev->calib.range_sw_err)) *
		((int64_t) lookupTable1[gas_range])) >> 16;
	var2 = (((int64_t) ((int64_t) gas_res_adc << 15) - (int64_t) (16777216)) + var1);
	var3 = (((int64_t) lookupTable2[gas_range] * (int64_t) var1) >> 9);
	calc_gas_res = (uint32_t) ((var3 + ((int64_t) var2 >> 1)) / (int64_t) var2);

	return calc_gas_res;
}

/*!
 * @brief This internal API is used to calculate the Heat Resistance value.
 */
static uint8_t calc_heater_res(uint16_t temp, const struct bme680_dev *dev)
{
	uint8_t heatr_res;
	int32_t var1;
	int32_t var2;
	int32_t var3;
	int32_t var4;
	int32_t var5;
	int32_t heatr_res_x100;

	if (temp > 400) /* Cap temperature */
		temp = 400;

	var1 = (((int32_t) dev->amb_temp * dev->calib.par_gh3) / 1000) * 256;
	var2 = (dev->calib.par_gh1 + 784) * (((((dev->calib.par_gh2 + 154009) * temp * 5) / 100) + 3276800) / 10);
	var3 = var1 + (var2 / 2);
	var4 = (var3 / (dev->calib.res_heat_range + 4));
	var5 = (131 * dev->calib.res_heat_val) + 65536;
	heatr_res_x100 = (int32_t) (((var4 / var5) - 250) * 34);
	heatr_res = (uint8_t) ((heatr_res_x100 + 50) / 100);

	return heatr_res;
}

#else


/*!
 * @brief This internal API is used to calculate the
 * temperature value in float format
 */
static float calc_temperature(uint32_t temp_adc, struct bme680_dev *dev)
{
	float var1 = 0;
	float var2 = 0;
	float calc_temp = 0;

	/* calculate var1 data */
	var1  = ((((float)temp_adc / 16384.0f) - ((float)dev->calib.par_t1 / 1024.0f))
			* ((float)dev->calib.par_t2));

	/* calculate var2 data */
	var2  = (((((float)temp_adc / 131072.0f) - ((float)dev->calib.par_t1 / 8192.0f)) *
		(((float)temp_adc / 131072.0f) - ((float)dev->calib.par_t1 / 8192.0f))) *
		((float)dev->calib.par_t3 * 16.0f));

	/* t_fine value*/
	dev->calib.t_fine = (var1 + var2);

	/* compensated temperature data*/
	calc_temp  = ((dev->calib.t_fine) / 5120.0f);

	return calc_temp;
}

/*!
 * @brief This internal API is used to calculate the
 * pressure value in float format
 */
static float calc_pressure(uint32_t pres_adc, const struct bme680_dev *dev)
{
	float var1 = 0;
	float var2 = 0;
	float var3 = 0;
	float calc_pres = 0;

	var1 = (((float)dev->calib.t_fine / 2.0f) - 64000.0f);
	var2 = var1 * var1 * (((float)dev->calib.par_p6) / (131072.0f));
	var2 = var2 + (var1 * ((float)dev->calib.par_p5) * 2.0f);
	var2 = (var2 / 4.0f) + (((float)dev->calib.par_p4) * 65536.0f);
	var1 = (((((float)dev->calib.par_p3 * var1 * var1) / 16384.0f)
		+ ((float)dev->calib.par_p2 * var1)) / 524288.0f);
	var1 = ((1.0f + (var1 / 32768.0f)) * ((float)dev->calib.par_p1));
	calc_pres = (1048576.0f - ((float)pres_adc));

	/* Avoid exception caused by division by zero */
	if ((int)var1 != 0) {
		calc_pres = (((calc_pres - (var2 / 4096.0f)) * 6250.0f) / var1);
		var1 = (((float)dev->calib.par_p9) * calc_pres * calc_pres) / 2147483648.0f;
		var2 = calc_pres * (((float)dev->calib.par_p8) / 32768.0f);
		var3 = ((calc_pres / 256.0f) * (calc_pres / 256.0f) * (calc_pres / 256.0f)
			* (dev->calib.par_p10 / 131072.0f));
		calc_pres = (calc_pres + (var1 + var2 + var3 + ((float)dev->calib.par_p7 * 128.0f)) / 16.0f);
	} else {
		calc_pres = 0;
	}

	return calc_pres;
}

/*!
 * @brief This internal API is used to calculate the
 * humidity value in float format
 */
static float calc_humidity(uint16_t hum_adc, const struct bme680_dev *dev)
{
	float calc_hum = 0;
	float var1 = 0;
	float var2 = 0;
	float var3 = 0;
	float var4 = 0;
	float temp_comp;

	/* compensated temperature data*/
	temp_comp  = ((dev->calib.t_fine) / 5120.0f);

	var1 = (float)((float)hum_adc) - (((float)dev->calib.par_h1 * 16.0f) + (((float)dev->calib.par_h3 / 2.0f)
		* temp_comp));

	var2 = var1 * ((float)(((float) dev->calib.par_h2 / 262144.0f) * (1.0f + (((float)dev->calib.par_h4 / 16384.0f)
		* temp_comp) + (((float)dev->calib.par_h5 / 1048576.0f) * temp_comp * temp_comp))));

	var3 = (float) dev->calib.par_h6 / 16384.0f;

	var4 = (float) dev->calib.par_h7 / 2097152.0f;

	calc_hum = var2 + ((var3 + (var4 * temp_comp)) * var2 * var2);

	if (calc_hum > 100.0f)
		calc_hum = 100.0f;
	else if (calc_hum < 0.0f)
		calc_hum = 0.0f;

	return calc_hum;
}

/*!
 * @brief This internal API is used to calculate the
 * gas resistance value in float format
 */
static float calc_gas_resistance(uint16_t gas_res_adc, uint8_t gas_range, const struct bme680_dev *dev)
{
	float calc_gas_res;
	float var1 = 0;
	float var2 = 0;
	float var3 = 0;

	const float lookup_k1_range[16] = {
	0.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, -0.8,
	0.0, 0.0, -0.2, -0.5, 0.0, -1.0, 0.0, 0.0};
	const float lookup_k2_range[16] = {
	0.0, 0.0, 0.0, 0.0, 0.1, 0.7, 0.0, -0.8,
	-0.1, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

	var1 = (1340.0f + (5.0f * dev->calib.range_sw_err));
	var2 = (var1) * (1.0f + lookup_k1_range[gas_range]/100.0f);
	var3 = 1.0f + (lookup_k2_range[gas_range]/100.0f);

	calc_gas_res = 1.0f / (float)(var3 * (0.000000125f) * (float)(1 << gas_range) * (((((float)gas_res_adc)
		- 512.0f)/var2) + 1.0f));

	return calc_gas_res;
}

/*!
 * @brief This internal API is used to calculate the
 * heater resistance value in float format
 */
static float calc_heater_res(uint16_t temp, const struct bme680_dev *dev)
{
	float var1 = 0;
	float var2 = 0;
	float var3 = 0;
	float var4 = 0;
	float var5 = 0;
	float res_heat = 0;

	if (temp > 400) /* Cap temperature */
		temp = 400;

	var1 = (((float)dev->calib.par_gh1 / (16.0f)) + 49.0f);
	var2 = ((((float)dev->calib.par_gh2 / (32768.0f)) * (0.0005f)) + 0.00235f);
	var3 = ((float)dev->calib.par_gh3 / (1024.0f));
	var4 = (var1 * (1.0f + (var2 * (float)temp)));
	var5 = (var4 + (var3 * (float)dev->amb_temp));
	res_heat = (uint8_t)(3.4f * ((var5 * (4 / (4 + (float)dev->calib.res_heat_range)) *
		(1/(1 + ((float) dev->calib.res_heat_val * 0.002f)))) - 25));

	return res_heat;
}

#endif

/*!
 * @brief This internal API is used to calculate the Heat duration value.
 */
static uint8_t calc_heater_dur(uint16_t dur)
{
	uint8_t factor = 0;
	uint8_t durval;

	if (dur >= 0xfc0) {
		durval = 0xff; /* Max duration*/
	} else {
		while (dur > 0x3F) {
			dur = dur / 4;
			factor += 1;
		}
		durval = (uint8_t) (dur + (factor * 64));
	}

	return durval;
}

/*!
 * @brief This internal API is used to calculate the field data of sensor.
 */
static int8_t read_field_data(struct bme680_field_data *data, struct bme680_dev *dev)
{
	int8_t rslt;
	uint8_t buff[BME680_FIELD_LENGTH] = { 0 };
	uint8_t gas_range;
	uint32_t adc_temp;
	uint32_t adc_pres;
	uint16_t adc_hum;
	uint16_t adc_gas_res;
	uint8_t tries = 10;

	/* Check for null pointer in the device structure*/
	rslt = null_ptr_check(dev);
	do {
		if (rslt == BME680_OK) {
			rslt = bme680_get_regs(((uint8_t) (BME680_FIELD0_ADDR)), buff, (uint16_t) BME680_FIELD_LENGTH,
				dev);

			data->status = buff[0] & BME680_NEW_DATA_MSK;
			data->gas_index = buff[0] & BME680_GAS_INDEX_MSK;
			data->meas_index = buff[1];

			/* read the raw data from the sensor */
			adc_pres = (uint32_t) (((uint32_t) buff[2] * 4096) | ((uint32_t) buff[3] * 16)
				| ((uint32_t) buff[4] / 16));
			adc_temp = (uint32_t) (((uint32_t) buff[5] * 4096) | ((uint32_t) buff[6] * 16)
				| ((uint32_t) buff[7] / 16));
			adc_hum = (uint16_t) (((uint32_t) buff[8] * 256) | (uint32_t) buff[9]);
			adc_gas_res = (uint16_t) ((uint32_t) buff[13] * 4 | (((uint32_t) buff[14]) / 64));
			gas_range = buff[14] & BME680_GAS_RANGE_MSK;

			data->status |= buff[14] & BME680_GASM_VALID_MSK;
			data->status |= buff[14] & BME680_HEAT_STAB_MSK;

			if (data->status & BME680_NEW_DATA_MSK) {
				data->temperature = calc_temperature(adc_temp, dev);
				data->pressure = calc_pressure(adc_pres, dev);
				data->humidity = calc_humidity(adc_hum, dev);
				data->gas_resistance = calc_gas_resistance(adc_gas_res, gas_range, dev);
				break;
			} else {
				TRACE_INFO("[BME680] No new data. Retrying (%d tries left)", tries);
			}
			/* Delay to poll the data */
			dev->delay_ms(BME680_POLL_PERIOD_MS);
		}
		tries--;
	} while (tries);

	if (!tries)
		rslt = BME680_W_NO_NEW_DATA;

	return rslt;
}

/*!
 * @brief This internal API is used to set the memory page based on register address.
 */
static int8_t set_mem_page(uint8_t reg_addr, struct bme680_dev *dev)
{
	int8_t rslt;
	uint8_t reg;
	uint8_t mem_page;

	/* Check for null pointers in the device structure*/
	rslt = null_ptr_check(dev);
	if (rslt == BME680_OK) {
		if (reg_addr > 0x7f)
			mem_page = BME680_MEM_PAGE1;
		else
			mem_page = BME680_MEM_PAGE0;

		if (mem_page != dev->mem_page) {
			dev->mem_page = mem_page;

			dev->com_rslt = dev->read(dev->dev_id, BME680_MEM_PAGE_ADDR | BME680_SPI_RD_MSK, &reg, 1);
			if (dev->com_rslt != 0)
				rslt = BME680_E_COM_FAIL;

			if (rslt == BME680_OK) {
				reg = reg & (~BME680_MEM_PAGE_MSK);
				reg = reg | (dev->mem_page & BME680_MEM_PAGE_MSK);

				dev->com_rslt = dev->write(dev->dev_id, BME680_MEM_PAGE_ADDR & BME680_SPI_WR_MSK,
					&reg, 1);
				if (dev->com_rslt != 0)
					rslt = BME680_E_COM_FAIL;
			}
		}
	}

	return rslt;
}

/*!
 * @brief This internal API is used to get the memory page based on register address.
 */
static int8_t get_mem_page(struct bme680_dev *dev)
{
	int8_t rslt;
	uint8_t reg;

	/* Check for null pointer in the device structure*/
	rslt = null_ptr_check(dev);
	if (rslt == BME680_OK) {
		dev->com_rslt = dev->read(dev->dev_id, BME680_MEM_PAGE_ADDR | BME680_SPI_RD_MSK, &reg, 1);
		if (dev->com_rslt != 0)
			rslt = BME680_E_COM_FAIL;
		else
			dev->mem_page = reg & BME680_MEM_PAGE_MSK;
	}

	return rslt;
}

/*!
 * @brief This internal API is used to validate the boundary
 * conditions.
 */
static int8_t boundary_check(uint8_t *value, uint8_t min, uint8_t max, struct bme680_dev *dev)
{
	int8_t rslt = BME680_OK;

	if (value != NULL) {
		/* Check if value is below minimum value */
		if (*value < min) {
			/* Auto correct the invalid value to minimum value */
			*value = min;
			dev->info_msg |= BME680_I_MIN_CORRECTION;
		}
		/* Check if value is above maximum value */
		if (*value > max) {
			/* Auto correct the invalid value to maximum value */
			*value = max;
			dev->info_msg |= BME680_I_MAX_CORRECTION;
		}
	} else {
		rslt = BME680_E_NULL_PTR;
	}

	return rslt;
}

/*!
 * @brief This internal API is used to validate the device structure pointer for
 * null conditions.
 */
static int8_t null_ptr_check(const struct bme680_dev *dev)
{
	int8_t rslt;

	if ((dev == NULL) || (dev->read == NULL) || (dev->write == NULL) || (dev->delay_ms == NULL)) {
		if(dev == NULL)
			TRACE_ERROR("[BME680] dev == NULL");
		if(dev->read == NULL)
			TRACE_ERROR("[BME680] dev->read == NULL");
		if(dev->write == NULL)
			TRACE_ERROR("[BME680] dev->write == NULL");
		if(dev->delay_ms == NULL)
			TRACE_ERROR("[BME680] dev->delay_ms == NULL");
			
		/* Device structure pointer is not valid */
		rslt = BME680_E_NULL_PTR;
	} else {
		/* Device structure is fine */
		rslt = BME680_OK;
	}

	return rslt;
}
