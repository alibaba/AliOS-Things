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

/************ define parameter for register ************/
#define REG_SP_INT_RESET (0xE1)
#define REG_SP_IMM_STOP (0xE2)
#define REG_SP_IMM_START (0xE3)
#define REG_SP_SW_RESET (0xE4)

/* REG_ALSCONTROL(0x00) */
#define MEAS_SERIAL (0 << 3)
#define MEAS_UNIT (1 << 3)
#define TYPE_BOTH (0 << 2)
#define TYPE_ONLY (1 << 2)
#define CTL_STANDBY (0)
#define CTL_STANDALONE (3)
#define CONTROL_MAX (16)

/* REG_INTERRUPT(0x02) */
#define INT_NONSTOP (0 << 6)
#define INT_STOP (1 << 6)
#define INT_DISABLE (0 << 4)
#define INT_ENABLE (1 << 4)
#define INT_PER_MAX (16)
#define INTRRUPT_MASK (0x5F)

/* REG_ALSGAIN(0x07) */
#define GAIN_X001 (0)
#define GAIN_X002 (1)
#define GAIN_X064 (2)
#define GAIN_X128 (3)
#define GAIN_MAX (4)


#define ALS_SET_CONTROL (MEAS_SERIAL)
#define ALS_SET_TIMING (218)
#define ALS_SET_INTR_PERSIST (1)
#define ALS_SET_INTR (INT_NONSTOP | INT_DISABLE | ALS_SET_INTR_PERSIST)
#define ALS_SET_ALSTH_UP_L (0xFF)
#define ALS_SET_ALSTH_UP_H (0xFF)

#define ALS_SET_ALSTH_LOW_L (0x00)
#define ALS_SET_ALSTH_LOW_H (0x00)

#define ALS_SET_GAIN (0x00)


/************ definition to dependent on sensor IC ************/
#define BH1730_I2C_NAME "bh1730_i2c"
#define BH1730_I2C_ADDRESS1 (0x29) // 7 bits slave address 010 1001
#define BH1730_ADDR_TRANS(n) ((n) << 1)
#define BH1730_I2C_ADDRESS BH1730_ADDR_TRANS(BH1730_I2C_ADDRESS1)
#define BH1730_PART_ID_NUMBER \
    0x07 // PART_ID:  part number 3:0   revision id
         // 7:4

/************ define register for IC ************/
/* BH1730 REGSTER */
#define REG_ALSCONTROL (0x80)
#define REG_ALSTIMING (0x81)
#define REG_ALSINTERRUPT (0x82)
#define REG_ALSTHLOW (0x83)
#define REG_ALSTHLLOW (0x83)
#define REG_ALSTHLHIGH (0x84)
#define REG_ALSTHHIGH (0x85)
#define REG_ALSTHHLOW (0x85)
#define REG_ALSTHHHIGH (0x86)
#define REG_ALSGAIN (0x87)
#define REG_ID (0x92)
#define REG_ALSDATA0 (0x94)
#define REG_ALSDATA0_LOW (0x94)
#define REG_ALSDATA0_HIGH (0x95)
#define REG_ALSDATA1 (0x96)
#define REG_ALSDATA1_LOW (0x96)
#define REG_ALSDATA1_HIGH (0x97)

/************ command definition of ioctl ************/
#define IOCTL_APP_SET_TIMER (10)
#define IOCTL_APP_SET_PWRSET_ALS (11)
#define IOCTL_APP_SET_ALS_TH_UP (15)
#define IOCTL_APP_SET_ALS_TH_LOW (16)
#define IOCTL_APP_SET_INT_OUTRST (20)
#define IOCTL_APP_SET_SW_RST (21)
#define IOCTL_APP_SET_TIMING (22)
#define IOCTL_APP_SET_GAIN (23)
#define IOCTL_APP_SET_GENERAL (253)
#define IOCTL_APP_READ_GENERAL (254)
#define IOCTL_APP_READ_DRIVER_VER (255)

#define COEFFICIENT (4)

const unsigned long data0_coefficient[COEFFICIENT] = { 1290, 795, 510, 276 };
const unsigned long data1_coefficient[COEFFICIENT] = { 2733, 859, 345, 130 };
const unsigned long judge_coefficient[COEFFICIENT] = { 26, 55, 109, 213 };

/* value to calcrate  */
#define DECIMAL_BIT (14)


i2c_dev_t bh1730_ctx = {
    .config.dev_addr = BH1730_I2C_ADDRESS,
};

typedef struct
{
    uint8_t als_data0_low;  /* data value of ALS data0 low from sensor */
    uint8_t als_data0_high; /* data value of ALS data0 high from sensor */
    uint8_t als_data1_low;  /* data value of ALS data1 low from sensor */
    uint8_t als_data1_high; /* data value of ALS data1 high from sensor */
} READ_DATA_BUF;

/************************************************************
 *                      logic function                      *
 ***********************************************************/
/******************************************************************************
 * NAME       : measure_calc_als
 * FUNCTION   : calculate illuminance data for BH1730
 * REMARKS    : final_data format
 *            :+-----------+-------------------------+------------------------+
 *            :| sign 1bit | positive values : 17bit | decimal values : 14bit |
 *            :+-----------+-------------------------+------------------------+
 *****************************************************************************/
static int measure_calc_als(uint32_t *final_data)
{

#define ITIME_CYCLE_BASE (256)
#define ITIME_CYCLE_UNIT (27)
#define JUDGE_FIXED_COEF (100)
#define MAX_OUTRANGE (100000)
#define DEVICE_OUTMAX (0xFFFF)

    READ_DATA_BUF data;
    int           ret;
    uint16_t      als_data0;
    uint16_t      als_data1;
    uint8_t       gain, read_time;
    uint32_t      calc_judge, calc_data, gain_time, timing;
    uint32_t      positive, decimal;
    const uint8_t gain_table[GAIN_MAX] = { 1, 2, 64, 128 };
    (void)decimal;
    ret = sensor_i2c_read(&bh1730_ctx, REG_ALSGAIN, &gain, sizeof(gain),
                          I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }
    ret = sensor_i2c_read(&bh1730_ctx, REG_ALSTIMING, &read_time,
                          sizeof(read_time), I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }
    ret = sensor_i2c_read(&bh1730_ctx, REG_ALSDATA0, (uint8_t*)&data, sizeof(data),
                          I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }
    als_data0 = data.als_data0_low | (data.als_data0_high << 8);
    als_data1 = data.als_data1_low | (data.als_data1_high << 8);

    /* calculate data */
    if (als_data0 == DEVICE_OUTMAX) {
        positive = MAX_OUTRANGE;
        decimal  = 0;
    } else {
        timing     = (ITIME_CYCLE_BASE - read_time) * ITIME_CYCLE_UNIT;
        gain_time  = timing * gain_table[gain & GAIN_X128];
        calc_judge = als_data1 * JUDGE_FIXED_COEF;
        if (calc_judge < (als_data0 * judge_coefficient[0])) {
            calc_data = (data0_coefficient[0] * als_data0) -
                        (data1_coefficient[0] * als_data1);
        } else if (calc_judge < (als_data0 * judge_coefficient[1])) {
            calc_data = (data0_coefficient[1] * als_data0) -
                        (data1_coefficient[1] * als_data1);
        } else if (calc_judge < (als_data0 * judge_coefficient[2])) {
            calc_data = (data0_coefficient[2] * als_data0) -
                        (data1_coefficient[2] * als_data1);
        } else if (calc_judge < (als_data0 * judge_coefficient[3])) {
            calc_data = (data0_coefficient[3] * als_data0) -
                        (data1_coefficient[3] * als_data1);
        } else {
            calc_data = 0;
        }

        /* calculate a positive number */
        positive = calc_data / gain_time;
        decimal  = 0;

        if (positive < MAX_OUTRANGE) {
            /* calculate over plus and shift 16bit */
            calc_data = calc_data - (positive * gain_time);
            if (calc_data != 0) {
                calc_data = calc_data << DECIMAL_BIT;

                /* calculate a decimal number */
                decimal = calc_data / gain_time;
            }
        } else {
            positive = MAX_OUTRANGE;
        }
    }
    //*final_data = (uint32_t)(positive << DECIMAL_BIT) + decimal;
    *final_data = positive;
    return (0);

#undef ITIME_CYCLE_BASE
#undef ITIME_CYCLE_UNIT
#undef JUDGE_FIXED_COEF
#undef MAX_OUTRANGE
}


/******************************************************************************
 * NAME       : als_driver_read_power_state
 * FUNCTION   : read the value of ALS status in BH1730
 * REMARKS    :
 *****************************************************************************/
static int als_driver_read_control_state(uint8_t *con_st)
{
    uint8_t result;
    int     ret;
    /* read register to BH1730 via i2c */
    ret = sensor_i2c_read(&bh1730_ctx, REG_ALSCONTROL, &result, sizeof(result),
                          I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }
    *con_st = result;
    return (ret);
}


static int drv_als_rohm_bh1730_set_power_mode(i2c_dev_t *      drv,
                                              dev_power_mode_e mode)
{
    int     ret = 0;
    uint8_t dev_con;
    ret = als_driver_read_control_state(&dev_con);
    if (unlikely(ret)) {
        return ret;
    }
    switch (mode) {
        case DEV_POWER_OFF:
            dev_con = dev_con | CTL_STANDBY;
            break;
        case DEV_POWER_ON:
            dev_con = dev_con | CTL_STANDALONE;
            break;
        default:
            return -1;
    }
    ret = sensor_i2c_write(drv, REG_ALSCONTROL, &dev_con, sizeof(dev_con),
                           I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }
    return 0;
}

static int drv_als_rohm_bh1730_open(void)
{

    int ret = 0;
    ret     = drv_als_rohm_bh1730_set_power_mode(&bh1730_ctx, DEV_POWER_ON);
    if (unlikely(ret)) {
        return -1;
    }
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

static int drv_als_rohm_bh1730_close(void)
{

    int ret = 0;
    ret     = drv_als_rohm_bh1730_set_power_mode(&bh1730_ctx, DEV_POWER_OFF);
    if (unlikely(ret)) {
        return -1;
    }
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;

    return 0;
}

static int drv_als_rohm_bh1730_read(void *buf, size_t len)
{

    als_data_t *pdata = (als_data_t *)buf;
    if (buf == NULL) {
        return -1;
    }
    measure_calc_als(&(pdata->lux));
    pdata->timestamp = aos_now_ms();
    return sizeof(humidity_data_t);
}


static int drv_als_rohm_bh1730_write(const void *buf, size_t len)
{
    return 0;
}

static int drv_als_rohm_bh1730_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;

    switch (cmd) {
        case SENSOR_IOCTL_SET_POWER: {
            ret = drv_als_rohm_bh1730_set_power_mode(&bh1730_ctx, arg);
            if (unlikely(ret)) {
                return -1;
            }
        } break;
        case SENSOR_IOCTL_GET_INFO: {
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
            info->model             = "BH1730";
            info->unit              = lux;
        } break;
        default:
            break;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

static void drv_als_rohm_bh1730_handle(void)
{
    /* no handle so far */
}

static int drv_als_rohm_bh1730_validate_id(i2c_dev_t *drv, uint8_t id_val)
{
    int     ret           = 0;
    uint8_t part_id_value = 0;

    if (drv == NULL) {
        return -1;
    }

    ret = sensor_i2c_read(drv, REG_ID, &part_id_value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }
    if ((part_id_value >> 4) != id_val) {
        return -1;
    }

    return 0;
}

int drv_als_rohm_bh1730_init(void)
{
    int          ret = 0;
    sensor_obj_t sensor;

    memset(&sensor, 0, sizeof(sensor));

    ret = drv_als_rohm_bh1730_validate_id(&bh1730_ctx, BH1730_PART_ID_NUMBER);

    struct init_func_write_data
    {
        uint8_t control;
        uint8_t timing;
        uint8_t intr;
        uint8_t alsth_ll;
        uint8_t alsth_lh;
        uint8_t alsth_hl;
        uint8_t alsth_hh;
        uint8_t als_gain;
    } write_data;

    write_data.control  = ALS_SET_CONTROL;
    write_data.timing   = ALS_SET_TIMING;
    write_data.intr     = ALS_SET_INTR;
    write_data.alsth_hl = ALS_SET_ALSTH_UP_L;
    write_data.alsth_hh = ALS_SET_ALSTH_UP_H;
    write_data.alsth_ll = ALS_SET_ALSTH_LOW_L;
    write_data.alsth_lh = ALS_SET_ALSTH_LOW_H;
    write_data.als_gain = ALS_SET_GAIN;
    ret = sensor_i2c_write(&bh1730_ctx, REG_ALSCONTROL, (uint8_t *)&write_data,
                           sizeof(write_data), I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }

    /* fill the sensor obj parameters here */
    sensor.tag        = TAG_DEV_ALS;
    sensor.path       = dev_als_path;
    sensor.io_port    = I2C_PORT;
    sensor.open       = drv_als_rohm_bh1730_open;
    sensor.close      = drv_als_rohm_bh1730_close;
    sensor.read       = drv_als_rohm_bh1730_read;
    sensor.write      = drv_als_rohm_bh1730_write;
    sensor.ioctl      = drv_als_rohm_bh1730_ioctl;
    sensor.irq_handle = drv_als_rohm_bh1730_handle;


    ret = sensor_create_obj(&sensor);
    if (unlikely(ret)) {
        return -1;
    }


    LOG("%s %s successfully \n", SENSOR_STR, __func__);

    return 0;
}



SENSOR_DRV_ADD(drv_als_rohm_bh1730_init);

