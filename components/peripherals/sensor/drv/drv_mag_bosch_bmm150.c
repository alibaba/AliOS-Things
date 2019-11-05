/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
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


#define BMM150_I2C_ADDR1                            (0x10<<1)
#define BMM150_I2C_ADDR2                            (0x11 << 1)
#define BMM150_I2C_ADDR3                            (0x12 << 1)
#define BMM150_I2C_ADDR4                            (0x13 << 1)

#define BMM150_CHIP_ID_ADDR                         (0X40)
#define BMM150_CHIP_ID_VALUE                        (0X32)

#define BMM150_OP_MODE_ADDR                         (0x4C)
#define BMM150_DATA_X_LSB                           (0x42)

#define BMM150_ODR_ADDR_POS                         (3)
#define BMM150_ODR_ADDR_LEN                         (3)
#define BMM150_ODR_ADDR_MSK                         (0x31)
#define BMM150_ODR_ADDR_REG                         (BMM150_OP_MODE_ADDR)

#define BMM150_POWER_MODE_POS                       (1)
#define BMM150_POWER_MODE_LEN                       (2)
#define BMM150_POWER_MODE_MSK                       (0xf9)
#define BMM150_POWER_MODE_REG                       (BMM150_OP_MODE_ADDR)

#define BMM150_POWER_MODE_NORMAL (0X0)
#define BMM150_POWER_MODE_SLEEP                     (0X3)
#define BMM150_POWER_MODE_OFF                       (0X3)

#define BMM150_ODR_2                        (2)
#define BMM150_ODR_6                        (6)
#define BMM150_ODR_8                        (8)
#define BMM150_ODR_10                       (10)
#define BMM150_ODR_15                       (15)
#define BMM150_ODR_20                       (20)
#define BMM150_ODR_25                       (25)
#define BMM150_ODR_30                       (30)


#define BMM150_ODR_2_REG                    (0x1)
#define BMM150_ODR_6_REG                    (0x2)
#define BMM150_ODR_8_REG                    (0x3)
#define BMM150_ODR_10_REG                   (0x0)
#define BMM150_ODR_15_REG                   (0x4)
#define BMM150_ODR_20_REG                   (0x5)
#define BMM150_ODR_25_REG                   (0x6)
#define BMM150_ODR_30_REG                   (0x7)

#define BMM150_DIG_X1                       (0x5D)
#define BMM150_DIG_Y1                       (0x5E)
#define BMM150_DIG_Z4_LSB                   (0x62)
#define BMM150_DIG_Z4_MSB                   (0x63)
#define BMM150_DIG_X2                       (0x64)
#define BMM150_DIG_Y2                       (0x65)
#define BMM150_DIG_Z2_LSB                   (0x68)
#define BMM150_DIG_Z2_MSB                   (0x69)
#define BMM150_DIG_Z1_LSB                   (0x6A)
#define BMM150_DIG_Z1_MSB                   (0x6B)
#define BMM150_DIG_XYZ1_LSB                 (0x6C)
#define BMM150_DIG_XYZ1_MSB                 (0x6D)
#define BMM150_DIG_Z3_LSB                   (0x6E)
#define BMM150_DIG_Z3_MSB                   (0x6F)
#define BMM150_DIG_XY2                      (0x70)
#define BMM150_DIG_XY1                      (0x71)


#define BMM150_DATA_X_MSK                       (0xF8)
#define BMM150_DATA_X_POS                       (0x03)

#define BMM150_DATA_Y_MSK                       (0xF8)
#define BMM150_DATA_Y_POS                       (0x03)

#define BMM150_DATA_Z_MSK                       (0xFE)
#define BMM150_DATA_Z_POS                       (0x01)

#define BMM150_DATA_RHALL_MSK                   (0xFC)
#define BMM150_DATA_RHALL_POS                   (0x02)

#define BMM150_DATA_UT_2_MG                     (10)


#define BMM150_OVERFLOW_OUTPUT                                      (-32768)
#define BMM150_NEGATIVE_SATURATION_Z                                (-32767)
#define BMM150_POSITIVE_SATURATION_Z                                (32767)

#define BMM150_XYAXES_FLIP_OVERFLOW_ADCVAL                          (-4096)
#define BMM150_ZAXIS_HALL_OVERFLOW_ADCVAL                           (-16384)

#define BMM150_GET_BITSLICE(regvar, bitname) \
    ((regvar & bitname##_MSK) >> bitname##_POS)
#define BMM150_SET_BITSLICE(regvar, bitname, val) \
    ((regvar & (~bitname##_MSK)) | ((val << bitname##_POS) & bitname##_MSK))

typedef struct bmm150_trim_registers {
    int8_t dig_x1;
    int8_t dig_y1;
    int8_t dig_x2;
    int8_t dig_y2;
    uint16_t dig_z1;
    int16_t dig_z2;
    int16_t dig_z3;
    int16_t dig_z4;
    uint8_t dig_xy1;
    int8_t dig_xy2;
    uint16_t dig_xyz1;
}bmm150_trim_reg_st;

bmm150_trim_reg_st      g_bmm150_trim_reg = {0};

i2c_dev_t bmm150_ctx = {
    .port                 = 3,
    .config.address_width = 8,
    .config.freq          = 400000,
    .config.dev_addr      = BMM150_I2C_ADDR1,
};

int drv_mag_bosch_bmm150_validate_id(i2c_dev_t *drv, uint8_t id_value)
{
    uint8_t value = 0x00;
    int     ret   = 0;

    if (drv == NULL) {
        return -1;
    }

    ret = sensor_i2c_read(drv, BMM150_CHIP_ID_ADDR, &value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    if (id_value != value) {
        return -1;
    }

    return 0;
}

int drv_mag_bosch_bmm150_set_power_mode(i2c_dev_t *      drv,
                                                dev_power_mode_e mode)
{
    uint8_t value = 0x00;
    
    uint8_t value1 = 0x00;
    int     ret = 0;

    ret = sensor_i2c_read(drv, BMM150_POWER_MODE_REG, &value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }
    switch (mode) {
        case DEV_POWER_ON: {
            ret = sensor_i2c_read(drv, 0x4b, &value1, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
            value1 |= 1;

            ret = sensor_i2c_write(drv, 0x4b, &value1,
                           I2C_DATA_LEN, I2C_OP_RETRIES);

                           aos_msleep(1);
            value = BMM150_SET_BITSLICE(value,BMM150_POWER_MODE,BMM150_POWER_MODE_NORMAL);
        } break;

        case DEV_POWER_OFF: {
            value = BMM150_SET_BITSLICE(value,BMM150_POWER_MODE,BMM150_POWER_MODE_OFF);

        } break;

        case DEV_SLEEP: {
            value = BMM150_SET_BITSLICE(value,BMM150_POWER_MODE,BMM150_POWER_MODE_OFF);
        } break;

        default:
            return -1;
    }
    
    ret = sensor_i2c_write(drv, BMM150_POWER_MODE_REG, &value,
                           I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }
    
    return 0;
}

int drv_mag_bosch_bmm150_set_odr(i2c_dev_t *drv, uint32_t odr)
{
    int     ret   = 0;
    uint8_t value = 0x00;

    ret = sensor_i2c_read(drv, BMM150_ODR_ADDR_REG, &value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    if (odr >= BMM150_ODR_30) {
        value = BMM150_SET_BITSLICE(value,BMM150_ODR_ADDR,BMM150_ODR_30_REG);
    } else if (odr >= BMM150_ODR_25) {
        value = BMM150_SET_BITSLICE(value,BMM150_ODR_ADDR,BMM150_ODR_25_REG);
    } else if (odr >= BMM150_ODR_20) {
        value = BMM150_SET_BITSLICE(value,BMM150_ODR_ADDR,BMM150_ODR_20_REG);
    } else if (odr >= BMM150_ODR_15) {
        value = BMM150_SET_BITSLICE(value,BMM150_ODR_ADDR,BMM150_ODR_15_REG);
    } else if (odr >= BMM150_ODR_10) {
        value = BMM150_SET_BITSLICE(value,BMM150_ODR_ADDR,BMM150_ODR_10_REG);
    } else if (odr >= BMM150_ODR_8) {
        value = BMM150_SET_BITSLICE(value,BMM150_ODR_ADDR,BMM150_ODR_8_REG);
    } else if (odr >= BMM150_ODR_6) {
        value = BMM150_SET_BITSLICE(value,BMM150_ODR_ADDR,BMM150_ODR_6_REG);
    } else {
        value = BMM150_SET_BITSLICE(value,BMM150_ODR_ADDR,BMM150_ODR_2_REG);
    }

    ret = sensor_i2c_write(drv, BMM150_ODR_ADDR_REG, &value, I2C_DATA_LEN,
                           I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }
    return 0;
}

void drv_mag_bosch_bmm150_irq_handle(void)
{
    /* no handle so far */
}

int drv_mag_bosch_bmm150_open(void)
{
    int ret = 0;
    ret     = drv_mag_bosch_bmm150_set_power_mode(&bmm150_ctx, DEV_POWER_ON);
    if (unlikely(ret)) {
        return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

int drv_mag_bosch_bmm150_close(void)
{
    int ret = 0;
    ret     = drv_mag_bosch_bmm150_set_power_mode(&bmm150_ctx, DEV_POWER_OFF);
    if (unlikely(ret)) {
        return -1;
    }
    return 0;
}



static int16_t drv_mag_bosch_bmm150_compensate_x(int16_t mag_data_x, uint16_t data_rhall)
{
    int16_t retval;
    uint16_t process_comp_x0 = 0;
    int32_t process_comp_x1;
    uint16_t process_comp_x2;
    int32_t process_comp_x3;
    int32_t process_comp_x4;
    int32_t process_comp_x5;
    int32_t process_comp_x6;
    int32_t process_comp_x7;
    int32_t process_comp_x8;
    int32_t process_comp_x9;
    int32_t process_comp_x10;

	/* Overflow condition check */
	if (mag_data_x != BMM150_XYAXES_FLIP_OVERFLOW_ADCVAL) {
        if (data_rhall != 0) {
            process_comp_x0 = data_rhall;
        } else if (g_bmm150_trim_reg.dig_xyz1 != 0) {
            process_comp_x0 = g_bmm150_trim_reg.dig_xyz1;
        } else {
            process_comp_x0 = 0;
        }
        if (process_comp_x0 != 0) {
            process_comp_x1 = ((int32_t)g_bmm150_trim_reg.dig_xyz1) * 16384;
            process_comp_x2 = ((uint16_t)(process_comp_x1 / process_comp_x0)) - ((uint16_t)0x4000);
            retval = ((int16_t)process_comp_x2);
            process_comp_x3 = (((int32_t)retval) * ((int32_t)retval));
            process_comp_x4 = (((int32_t)g_bmm150_trim_reg.dig_xy2) * (process_comp_x3 / 128));
            process_comp_x5 = (int32_t)(((int16_t)g_bmm150_trim_reg.dig_xy1) * 128);
            process_comp_x6 = ((int32_t)retval) * process_comp_x5;
            process_comp_x7 = (((process_comp_x4 + process_comp_x6) / 512) + ((int32_t)0x100000));
            process_comp_x8 = ((int32_t)(((int16_t)g_bmm150_trim_reg.dig_x2) + ((int16_t)0xA0)));
            process_comp_x9 = ((process_comp_x7 * process_comp_x8) / 4096);
            process_comp_x10 = ((int32_t)mag_data_x) * process_comp_x9;
            retval = ((int16_t)(process_comp_x10 / 8192));
            retval = (retval + (((int16_t)g_bmm150_trim_reg.dig_x1) * 8)) / 16;
        } else {
            retval = BMM150_OVERFLOW_OUTPUT;
        }
    } else {
        retval = BMM150_OVERFLOW_OUTPUT;
    }

    return retval;
}


static int16_t drv_mag_bosch_bmm150_compensate_y(int16_t mag_data_y, uint16_t data_rhall)
{
    int16_t retval;
    uint16_t process_comp_y0 = 0;
    int32_t process_comp_y1;
    uint16_t process_comp_y2;
    int32_t process_comp_y3;
    int32_t process_comp_y4;
    int32_t process_comp_y5;
    int32_t process_comp_y6;
    int32_t process_comp_y7;
    int32_t process_comp_y8;
    int32_t process_comp_y9;


    if (mag_data_y != BMM150_XYAXES_FLIP_OVERFLOW_ADCVAL) {
        if (data_rhall != 0) {
            process_comp_y0 = data_rhall;
        } else if (g_bmm150_trim_reg.dig_xyz1 != 0) {
            process_comp_y0 = g_bmm150_trim_reg.dig_xyz1;
        } else {
            process_comp_y0 = 0;
        }
        if (process_comp_y0 != 0) {
            process_comp_y1 = (((int32_t)g_bmm150_trim_reg.dig_xyz1) * 16384) / process_comp_y0;
            process_comp_y2 = ((uint16_t)process_comp_y1) - ((uint16_t)0x4000);
            retval = ((int16_t)process_comp_y2);
            process_comp_y3 = ((int32_t) retval) * ((int32_t)retval);
            process_comp_y4 = ((int32_t)g_bmm150_trim_reg.dig_xy2) * (process_comp_y3 / 128);
            process_comp_y5 = ((int32_t)(((int16_t)g_bmm150_trim_reg.dig_xy1) * 128));
            process_comp_y6 = ((process_comp_y4 + (((int32_t)retval) * process_comp_y5)) / 512);
            process_comp_y7 = ((int32_t)(((int16_t)g_bmm150_trim_reg.dig_y2) + ((int16_t)0xA0)));
            process_comp_y8 = (((process_comp_y6 + ((int32_t)0x100000)) * process_comp_y7) / 4096);
            process_comp_y9 = (((int32_t)mag_data_y) * process_comp_y8);
            retval = (int16_t)(process_comp_y9 / 8192);
            retval = (retval + (((int16_t)g_bmm150_trim_reg.dig_y1) * 8)) / 16;
        } else {
            retval = BMM150_OVERFLOW_OUTPUT;
        }
    } else {

        retval = BMM150_OVERFLOW_OUTPUT;
    }

    return retval;
}

/*!
 * @brief This internal API is used to obtain the compensated
 * magnetometer Z axis data(micro-tesla) in int16_t.
 */
static int16_t drv_mag_bosch_bmm150_compensate_z(int16_t mag_data_z, uint16_t data_rhall)
{
	int32_t retval;
	int16_t process_comp_z0;
	int32_t process_comp_z1;
	int32_t process_comp_z2;
	int32_t process_comp_z3;
	int16_t process_comp_z4;

	if (mag_data_z != BMM150_ZAXIS_HALL_OVERFLOW_ADCVAL) {
		if ((g_bmm150_trim_reg.dig_z2 != 0) && (g_bmm150_trim_reg.dig_z1 != 0)
		&& (data_rhall != 0) && (g_bmm150_trim_reg.dig_xyz1 != 0)) {
			/*Processing compensation equations*/
			process_comp_z0 = ((int16_t)data_rhall) - ((int16_t) g_bmm150_trim_reg.dig_xyz1);
			process_comp_z1 = (((int32_t)g_bmm150_trim_reg.dig_z3) * ((int32_t)(process_comp_z0))) / 4;
			process_comp_z2 = (((int32_t)(mag_data_z - g_bmm150_trim_reg.dig_z4)) * 32768);
			process_comp_z3 = ((int32_t)g_bmm150_trim_reg.dig_z1) * (((int16_t)data_rhall) * 2);
			process_comp_z4 = (int16_t)((process_comp_z3 + (32768)) / 65536);
			retval = ((process_comp_z2 - process_comp_z1) / (g_bmm150_trim_reg.dig_z2 + process_comp_z4));

			/* saturate result to +/- 2 micro-tesla */
			if (retval > BMM150_POSITIVE_SATURATION_Z) {
				retval =  BMM150_POSITIVE_SATURATION_Z;
			} else {
				if (retval < BMM150_NEGATIVE_SATURATION_Z)
					retval = BMM150_NEGATIVE_SATURATION_Z;
			}
			/* Conversion of LSB to micro-tesla*/
			retval = retval / 16;
		} else {
			retval = BMM150_OVERFLOW_OUTPUT;

		}
	} else {
		/* Overflow condition*/
		retval = BMM150_OVERFLOW_OUTPUT;
	}

	return (int16_t)retval;
}



static int8_t drv_mag_bosch_bmm150_read_calib_reg(i2c_dev_t *drv)
{
    int8_t ret;
    uint8_t trim_x1y1[2] = {0};
    uint8_t trim_xyz_data[4] = {0};
    uint8_t trim_xy1xy2[10] = {0};
    uint16_t temp_msb = 0;

    ret = sensor_i2c_read(drv, BMM150_DIG_X1, trim_x1y1, 2,
                          I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }
    
    ret = sensor_i2c_read(drv, BMM150_DIG_Z4_LSB, trim_xyz_data, 4,
                  I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }
                  
    ret = sensor_i2c_read(drv, BMM150_DIG_Z2_LSB, trim_xy1xy2, 10,
      I2C_OP_RETRIES);

    if (unlikely(ret)) {
        return -1;
    }

    g_bmm150_trim_reg.dig_x1 = (int8_t)trim_x1y1[0];
    g_bmm150_trim_reg.dig_y1 = (int8_t)trim_x1y1[1];
    g_bmm150_trim_reg.dig_x2 = (int8_t)trim_xyz_data[2];
    g_bmm150_trim_reg.dig_y2 = (int8_t)trim_xyz_data[3];
    temp_msb = ((uint16_t)trim_xy1xy2[3]) << 8;
    g_bmm150_trim_reg.dig_z1 = (uint16_t)(temp_msb | trim_xy1xy2[2]);
    temp_msb = ((uint16_t)trim_xy1xy2[1]) << 8;
    g_bmm150_trim_reg.dig_z2 = (int16_t)(temp_msb | trim_xy1xy2[0]);
    temp_msb = ((uint16_t)trim_xy1xy2[7]) << 8;
    g_bmm150_trim_reg.dig_z3 = (int16_t)(temp_msb | trim_xy1xy2[6]);
    temp_msb = ((uint16_t)trim_xyz_data[1]) << 8;
    g_bmm150_trim_reg.dig_z4 = (int16_t)(temp_msb | trim_xyz_data[0]);
    g_bmm150_trim_reg.dig_xy1 = trim_xy1xy2[9];
    g_bmm150_trim_reg.dig_xy2 = (int8_t)trim_xy1xy2[8];
    temp_msb = ((uint16_t)(trim_xy1xy2[5] & 0x7F)) << 8;
    g_bmm150_trim_reg.dig_xyz1 = (uint16_t)(temp_msb | trim_xy1xy2[4]);

    return ret;
}


int drv_mag_bosch_bmm150_read(void *buf, size_t len)
{
    int          ret  = 0;
    size_t       size = 0;
    uint8_t      reg[8];
    int16_t     msb_data;
    int16_t     raw_datax;
    int16_t     raw_datay;
    int16_t     raw_dataz;
    uint16_t    raw_data_r;
    mag_data_t *mag = (mag_data_t *)buf;
    if (mag == NULL) {
        return -1;
    }

    size = sizeof(mag_data_t);
    if (len < size) {
        return -1;
    }

    ret = sensor_i2c_read(&bmm150_ctx, BMM150_DATA_X_LSB, &reg[0],
                          8, I2C_OP_RETRIES);

    if (unlikely(ret)) {
        return -1;
    }

    reg[0] = BMM150_GET_BITSLICE(reg[0], BMM150_DATA_X);
    msb_data = ((int16_t)((int8_t)reg[1])) * 32;
    raw_datax = (int16_t)(msb_data | reg[0]);
    
    reg[2] = BMM150_GET_BITSLICE(reg[2], BMM150_DATA_Y);
    msb_data = ((int16_t)((int8_t)reg[3])) * 32;
    raw_datay = (int16_t)(msb_data | reg[2]);

    reg[4] = BMM150_GET_BITSLICE(reg[4], BMM150_DATA_Z);
    msb_data = ((int16_t)((int8_t)reg[5])) * 128;
    raw_dataz = (int16_t)(msb_data | reg[4]);

    reg[6] = BMM150_GET_BITSLICE(reg[6], BMM150_DATA_RHALL);
    raw_data_r = (uint16_t)(((uint16_t)reg[7] << 6) | reg[6]);

    mag->data[0] = (int32_t)drv_mag_bosch_bmm150_compensate_x(raw_datax, raw_data_r);

    mag->data[1] = (int32_t)drv_mag_bosch_bmm150_compensate_y(raw_datay, raw_data_r);

    mag->data[2] = (int32_t)drv_mag_bosch_bmm150_compensate_z(raw_dataz, raw_data_r);

    mag->data[0] = mag->data[0] * BMM150_DATA_UT_2_MG;
    mag->data[1] = mag->data[1] * BMM150_DATA_UT_2_MG;
    mag->data[2] = mag->data[2] * BMM150_DATA_UT_2_MG;
    
    mag->timestamp = aos_now_ms();
    return (int)size;
}

static int drv_mag_bosch_bmm150_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;

    switch (cmd) {
        case SENSOR_IOCTL_ODR_SET: {
            ret = drv_mag_bosch_bmm150_set_odr(&bmm150_ctx, arg);
            if (unlikely(ret)) {
                return -1;
            }
        } break;
        case SENSOR_IOCTL_RANGE_SET: {
  
        } break;
        case SENSOR_IOCTL_SET_POWER: {
            ret = drv_mag_bosch_bmm150_set_power_mode(&bmm150_ctx, arg);
            if (unlikely(ret)) {
                return -1;
            }
        } break;
        case SENSOR_IOCTL_GET_INFO: {
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
            info->model             = "BMM150";
            info->range_max         = 2500;
            info->range_min         = 2500;
            info->unit              = udps;
        } break;

        default:
            break;
    }

    return 0;
}

int drv_mag_bosch_bmm150_init(void)
{
    int          ret = 0;
    sensor_obj_t sensor;
    memset(&sensor, 0, sizeof(sensor));
    /* fill the sensor obj parameters here */
    sensor.io_port    = I2C_PORT;
    sensor.tag        = TAG_DEV_MAG;
    sensor.path       = dev_mag_path;
    sensor.open       = drv_mag_bosch_bmm150_open;
    sensor.close      = drv_mag_bosch_bmm150_close;
    sensor.read       = drv_mag_bosch_bmm150_read;
    sensor.write      = NULL;
    sensor.ioctl      = drv_mag_bosch_bmm150_ioctl;
    sensor.irq_handle = drv_mag_bosch_bmm150_irq_handle;

    ret = sensor_create_obj(&sensor);
    if (unlikely(ret)) {
        return -1;
    }

    ret = drv_mag_bosch_bmm150_set_power_mode(&bmm150_ctx, DEV_POWER_ON);
    if (unlikely(ret)) {
        return -1;
    }
    aos_msleep(10);
    ret = drv_mag_bosch_bmm150_validate_id(&bmm150_ctx, BMM150_CHIP_ID_VALUE);
    if (unlikely(ret)) {
        return -1;
    }

    ret = drv_mag_bosch_bmm150_set_odr(&bmm150_ctx, 2);
    if (unlikely(ret)) {
        return -1;
    }
    
    memset(&g_bmm150_trim_reg, 0, sizeof(g_bmm150_trim_reg));
    ret = drv_mag_bosch_bmm150_read_calib_reg(&bmm150_ctx);
    if (unlikely(ret)) {
        return -1;
    }

    /* update the phy sensor info to sensor hal */
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

SENSOR_DRV_ADD(drv_mag_bosch_bmm150_init);