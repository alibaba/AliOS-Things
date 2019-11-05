/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aos/kernel.h"
#include "ulog/ulog.h"
#include "network/hal/base.h"
#include "sensor_drv_api.h"
#include "sensor_hal.h"

#define TCS3400_ENABLE 0x80
#define TCS3400_ALS_TIME 0x81
#define TCS3400_WAIT_TIME 0x83
#define TCS3400_ALS_MINTHRESHLO 0x84
#define TCS3400_ALS_MINTHRESHHI 0x85
#define TCS3400_ALS_MAXTHRESHLO 0x86
#define TCS3400_ALS_MAXTHRESHHI 0x87
#define TCS3400_PERSISTENCE 0x8C
#define TCS3400_CONFIG 0x8D
#define TCS3400_CONTROL 0x8F
#define TCS3400_REG_AUX 0x90
#define TCS3400_REVID 0x91
#define TCS3400_CHIPID 0x92
#define TCS3400_STATUS 0x93
#define TCS3400_CLR_CHANLO 0x94
#define TCS3400_CLR_CHANHI 0x95
#define TCS3400_RED_CHANLO 0x96
#define TCS3400_RED_CHANHI 0x97
#define TCS3400_GRN_CHANLO 0x98
#define TCS3400_GRN_CHANHI 0x99
#define TCS3400_BLU_CHANLO 0x9A
#define TCS3400_BLU_CHANHI 0x9B
#define TCS3400_IR_TOGGLE 0xC0
#define TCS3400_IFORCE 0xE4
#define TCS3400_CLCLEAR 0xE6
#define TCS3400_AICLEAR 0xE7

#define TCS3400_I2C_SLAVE_ADDR 0x29
#define TCS3400_ADDR_TRANS(n) ((n) << 1)
#define TCS3400_I2C_ADDR TCS3400_ADDR_TRANS(TCS3400_I2C_SLAVE_ADDR)
#define TCS3400_CHIPID_VALUE 0x90 // 0x90 or 0x93

#define MAX_REGS 256
#define INTEGRATION_CYCLE 2780

#define TCS3400_MASK_AGAIN 0x03
#define TCS3400_MAX_LUX 0xffff
#define TCS3400_MAX_ALS_VALUE 0xffff
#define TCS3400_MIN_ALS_VALUE 3

#define DGF 783
#define R_Coef 50
#define G_Coef 530
#define B_Coef -230

#define CT_Coef 3647
#define CT_Offset 1319

#define TCS3400_CMD_ALS_INT_CLR 0xE6
#define TCS3400_CMD_ALL_INT_CLR 0xE7

#define GAIN1 0
#define GAIN4 1
#define GAIN16 2
#define GAIN64 3

#define ALS_PERSIST(p) (((p)&0xf) << 3)
#define AW_TIME_MS(p) \
    ((((p)*1000) + (INTEGRATION_CYCLE - 1)) / INTEGRATION_CYCLE)
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

struct tcs3400_als_info
{
    uint32_t sat;
    uint32_t cpl;
    uint16_t clear;
    uint16_t red;
    uint16_t green;
    uint16_t blue;
    int16_t  ir_data;
    uint16_t lux;
    uint16_t cct;
};

enum tcs3400_pwr_state
{
    POWER_ON,
    POWER_OFF,
    POWER_STANDBY,
};

enum tcs3400_ctrl_reg
{
    AGAIN_1  = (0 << 0),
    AGAIN_4  = (1 << 0),
    AGAIN_16 = (2 << 0),
    AGAIN_64 = (3 << 0),
};

enum tcs3400_en_reg
{
    TCS3400_EN_PWR_ON      = (1 << 0),
    TCS3400_EN_ALS         = (1 << 1),
    TCS3400_EN_WAIT        = (1 << 3),
    TCS3400_EN_ALS_IRQ     = (1 << 4),
    TCS3400_EN_ALS_SAT_IRQ = (1 << 5),
    TCS3400_EN_IRQ_PWRDN   = (1 << 6),
};

// 0x93 Register
enum tcs3400_status
{
    TCS3400_ST_ALS_VALID = (1 << 0),
    TCS3400_ST_ALS_IRQ   = (1 << 4),
    TCS3400_ST_ALS_SAT   = (1 << 7),
};

enum
{
    TCS3400_ALS_GAIN_MASK    = (3 << 0),
    TCS3400_ATIME_DEFAULT_MS = 50,
    MAX_ALS_VALUE            = 0xffff,
    MIN_ALS_VALUE            = 1,
};

static uint8_t const regs[] = {
    TCS3400_ENABLE,          TCS3400_ALS_TIME,        TCS3400_ALS_MINTHRESHLO,
    TCS3400_ALS_MINTHRESHHI, TCS3400_ALS_MAXTHRESHLO, TCS3400_ALS_MAXTHRESHHI,
    TCS3400_PERSISTENCE,     TCS3400_CONFIG,          TCS3400_CONTROL,
    TCS3400_REG_AUX,
};
static uint8_t const als_gains[] = { 1, 4, 16, 64 };

struct tcs3400_chips
{
    bool                    als_enabled;
    uint8_t                 atime;
    uint8_t                 again;
    uint8_t                 persist;
    uint8_t                 in_asat;
    uint8_t                 shadow[MAX_REGS];
    struct tcs3400_als_info als_inf;
};

i2c_dev_t tcs3400_ctx = {
    .port                 = 3,
    .config.address_width = 8,
    .config.freq          = 400000,
    .config.dev_addr      = TCS3400_I2C_ADDR,
};

static struct tcs3400_chips tcs3400_chip;

static int drv_als_ams_tcs3400_validate_id(i2c_dev_t *drv, uint8_t id_value)
{
    int     ret = 0;
    uint8_t chipid_value;
    ret = sensor_i2c_read(drv, TCS3400_CHIPID, &chipid_value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if (unlikely(ret)) {
        LOG("%s %s Sensor_i2c_read failure \n", SENSOR_STR, __func__);
        return ret;
    }

    if (!((chipid_value == 0x90) || (chipid_value == 0x93)))
        return -1;

    return 0;
}

static int drv_als_ams_tcs3400_set_power_mode(i2c_dev_t *      drv,
                                              dev_power_mode_e mode)
{
    int     ret       = 0;
    uint8_t temp_zero = 0;
    switch (mode) {
        case DEV_POWER_ON: {
            if (!(tcs3400_chip.als_enabled)) {
                ret = sensor_i2c_write(drv, TCS3400_CMD_ALL_INT_CLR, &temp_zero,
                                       I2C_DATA_LEN, I2C_OP_RETRIES);
                if (unlikely(ret)) {
                    return ret;
                }
                tcs3400_chip.shadow[TCS3400_ENABLE] |=
                  (TCS3400_EN_PWR_ON | TCS3400_EN_ALS | TCS3400_EN_ALS_IRQ);
                ret = sensor_i2c_write(drv, TCS3400_ENABLE,
                                       &(tcs3400_chip.shadow[TCS3400_ENABLE]),
                                       I2C_DATA_LEN, I2C_OP_RETRIES);
                if (unlikely(ret)) {
                    LOG("%s %s PowerEnable failure \n", SENSOR_STR, __func__);
                    return ret;
                }
                tcs3400_chip.shadow[TCS3400_REG_AUX] |= TCS3400_EN_ALS_SAT_IRQ;
                sensor_i2c_write(drv, TCS3400_REG_AUX,
                                 &(tcs3400_chip.shadow[TCS3400_REG_AUX]),
                                 I2C_DATA_LEN, I2C_OP_RETRIES);
                tcs3400_chip.als_enabled = 1;
            }
        } break;

        case DEV_POWER_OFF:

        case DEV_SLEEP: {
            if (tcs3400_chip.als_enabled) {
                ret = sensor_i2c_write(drv, TCS3400_CMD_ALL_INT_CLR, &temp_zero,
                                       I2C_DATA_LEN, I2C_OP_RETRIES);
                if (unlikely(ret)) {
                    return ret;
                }
                tcs3400_chip.shadow[TCS3400_ENABLE] = 0x00;
                ret = sensor_i2c_write(drv, TCS3400_ENABLE,
                                       &(tcs3400_chip.shadow[TCS3400_ENABLE]),
                                       I2C_DATA_LEN, I2C_OP_RETRIES);
                if (unlikely(ret)) {
                    LOG("%s %s PowerOFF or SLEEP failure \n", SENSOR_STR,
                        __func__);
                    return ret;
                }
                tcs3400_chip.als_enabled = 0;
            }
        } break;

        default:
            break;
    }
    return 0;
}

static int tcs3400_reset_regs(i2c_dev_t *drv)
{
    int     i;
    int     ret = 0;
    uint8_t reg;
    uint8_t temp      = 0;
    uint8_t temp_zero = 0;
    // clear interrupt
    ret = sensor_i2c_write(drv, TCS3400_CMD_ALL_INT_CLR, &temp_zero,
                           I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        LOG("%s %s reset_regs INT_CLR failure \n", SENSOR_STR, __func__);
        return -1;
    }
    sensor_i2c_read(drv, TCS3400_ENABLE, &temp, I2C_DATA_LEN, I2C_OP_RETRIES);
    sensor_i2c_write(drv, TCS3400_ENABLE, &temp_zero, I2C_DATA_LEN,
                     I2C_OP_RETRIES);
    for (i = 0; i < ARRAY_SIZE(regs); i++) {
        reg = regs[i];
        ret = sensor_i2c_write(drv, reg, &(tcs3400_chip.shadow[reg]),
                               I2C_DATA_LEN, I2C_OP_RETRIES);
        if (unlikely(ret)) {
            LOG("%s %s reset_regs failure  \n", SENSOR_STR, __func__);
            return ret;
        }
    }
    sensor_i2c_write(drv, TCS3400_ENABLE, &temp, I2C_DATA_LEN, I2C_OP_RETRIES);
    return ret;
}

static int drv_als_ams_tcs3400_set_default_config(i2c_dev_t *drv)
{
    int ret;
    tcs3400_chip.again       = AGAIN_16;
    tcs3400_chip.atime       = 0xee;
    tcs3400_chip.als_enabled = 0;
    tcs3400_chip.persist     = ALS_PERSIST(0);
    tcs3400_chip.shadow[TCS3400_CONTROL] &= ~0x3;
    tcs3400_chip.shadow[TCS3400_CONTROL] |= tcs3400_chip.again;
    tcs3400_chip.shadow[TCS3400_ALS_TIME] |= tcs3400_chip.atime;
    tcs3400_chip.shadow[TCS3400_PERSISTENCE] = tcs3400_chip.persist;
    tcs3400_chip.shadow[TCS3400_CONFIG]      = 0x40;
    ret                                      = tcs3400_reset_regs(&tcs3400_ctx);
    if (unlikely(ret)) {
        LOG("%s %s set_default_config failure \n", SENSOR_STR, __func__);
        return ret;
    }
    return 0;
}

static void drv_als_ams_tcs3400_irq_handle(void)
{
    /* no handle so far */
}

static int drv_als_ams_tcs3400_open(void)
{
    int ret = 0;
    ret     = drv_als_ams_tcs3400_set_power_mode(&tcs3400_ctx, DEV_POWER_ON);
    if (unlikely(ret)) {
        return -1;
    }
    return 0;
}

static int drv_als_ams_tcs3400_close(void)
{
    int ret = 0;
    ret     = drv_als_ams_tcs3400_set_power_mode(&tcs3400_ctx, DEV_POWER_OFF);
    if (unlikely(ret)) {
        return -1;
    }
    return 0;
}

static int tcs3400_read_als_data()
{
    int      ret;
    uint8_t *buf;
    int16_t  ir_data;
    ret = sensor_i2c_read(&tcs3400_ctx, TCS3400_CLR_CHANLO,
                          &tcs3400_chip.shadow[TCS3400_CLR_CHANLO], 8,
                          I2C_OP_RETRIES);
    if (unlikely(ret)) {
        LOG("%s %s TCS3400_READ_all_channel data failure \n", SENSOR_STR,
            __func__);
        return ret;
    }
    buf                        = &tcs3400_chip.shadow[TCS3400_CLR_CHANLO];
    tcs3400_chip.als_inf.clear = (uint16_t)((buf[1] << 8) | buf[0]);
    tcs3400_chip.als_inf.red   = (uint16_t)((buf[3] << 8) | buf[2]);
    tcs3400_chip.als_inf.green = (uint16_t)((buf[5] << 8) | buf[4]);
    tcs3400_chip.als_inf.blue  = (uint16_t)((buf[7] << 8) | buf[6]);

    ir_data = (tcs3400_chip.als_inf.red + tcs3400_chip.als_inf.green +
               tcs3400_chip.als_inf.blue - tcs3400_chip.als_inf.clear + 1) /
              2;
    if (ir_data < 0)
        ir_data = 0;
    tcs3400_chip.als_inf.ir_data = ir_data;
    // LOG("clear is %d ,red is %d, green is %d, blue is %d, ir_data is %d,atime
    // is %d, again is %d \n",
    // tcs3400_chip.als_inf.clear,tcs3400_chip.als_inf.red,tcs3400_chip.als_inf.green,tcs3400_chip.als_inf.blue,tcs3400_chip.als_inf.ir_data,tcs3400_chip.atime,tcs3400_chip.again);
    return 0;
}

static int tcs3400_max_als_value()
{
    int val;
    val = 256 - tcs3400_chip.shadow[TCS3400_ALS_TIME];
    val = ((val)*1024);
    val = val > MAX_ALS_VALUE ? MAX_ALS_VALUE : val;
    return val;
}

static int tcs3400_cal_cpl()
{
    uint32_t CPL   = 0;
    uint8_t  atime = 256 - tcs3400_chip.shadow[TCS3400_ALS_TIME];
    int32_t  dgf   = DGF;
    uint32_t sat;
    uint8_t  gain = als_gains[(tcs3400_chip.shadow[TCS3400_CONTROL] & 0x3)];

    CPL = (atime * gain);
    CPL *= INTEGRATION_CYCLE;

    CPL /= dgf;
    if (CPL < 1)
        CPL = 1;

    sat = (int32_t)(atime << 10);
    if (sat > TCS3400_MAX_ALS_VALUE)
        sat = TCS3400_MAX_ALS_VALUE;

    sat                      = sat * 8 / 10;
    tcs3400_chip.als_inf.sat = sat;
    tcs3400_chip.als_inf.cpl = (uint32_t)CPL;
    return 1;
}

static int tcs3400_als_enable(int on)
{
    int     ret       = 0;
    uint8_t temp_zero = 0;
    if (on) {
        tcs3400_cal_cpl();
        ret = sensor_i2c_write(&tcs3400_ctx, TCS3400_CMD_ALL_INT_CLR,
                               &temp_zero, I2C_DATA_LEN, I2C_OP_RETRIES);
        if (unlikely(ret)) {
            return ret;
        }
        tcs3400_chip.shadow[TCS3400_ENABLE] |=
          (TCS3400_EN_PWR_ON | TCS3400_EN_ALS | TCS3400_EN_ALS_IRQ);
        ret = sensor_i2c_write(&tcs3400_ctx, TCS3400_ENABLE,
                               &(tcs3400_chip.shadow[TCS3400_ENABLE]),
                               I2C_DATA_LEN, I2C_OP_RETRIES);
        if (unlikely(ret)) {
            LOG("%s %s als_enable failure \n", SENSOR_STR, __func__);
            return ret;
        }
        tcs3400_chip.shadow[TCS3400_REG_AUX] |= TCS3400_EN_ALS_SAT_IRQ;
        sensor_i2c_write(&tcs3400_ctx, TCS3400_REG_AUX,
                         &(tcs3400_chip.shadow[TCS3400_REG_AUX]), I2C_DATA_LEN,
                         I2C_OP_RETRIES);
        tcs3400_chip.als_enabled = 1;
    } else {
        ret = sensor_i2c_write(&tcs3400_ctx, TCS3400_CMD_ALL_INT_CLR,
                               &temp_zero, I2C_DATA_LEN, I2C_OP_RETRIES);
        if (unlikely(ret)) {
            return ret;
        }
        tcs3400_chip.shadow[TCS3400_ENABLE] = 0x00;
        ret = sensor_i2c_write(&tcs3400_ctx, TCS3400_ENABLE,
                               &(tcs3400_chip.shadow[TCS3400_ENABLE]),
                               I2C_DATA_LEN, I2C_OP_RETRIES);
        if (unlikely(ret)) {
            LOG("%s %s als_enable failure \n", SENSOR_STR, __func__);
            return ret;
        }
        tcs3400_chip.als_enabled = 0;
    }
    return 0;
}

static int tcs3400_set_als_gain(int gain)
{
    int     ret;
    uint8_t ctrl_reg;
    bool    current_state = tcs3400_chip.als_enabled;
    tcs3400_als_enable(0);
    switch (gain) {
        case 1:
            ctrl_reg = AGAIN_1;
            break;
        case 4:
            ctrl_reg = AGAIN_4;
            break;
        case 16:
            ctrl_reg = AGAIN_16;
            break;
        case 64:
            ctrl_reg = AGAIN_64;
            break;
        default:
            LOG("set als_gain wrong gain data \n");
            return -1;
    }
    tcs3400_chip.again = ctrl_reg;
    tcs3400_chip.shadow[TCS3400_CONTROL] &= ~0x3;
    tcs3400_chip.shadow[TCS3400_CONTROL] |= ctrl_reg;
    ret = tcs3400_reset_regs(&tcs3400_ctx);
    if (unlikely(ret)) {
        LOG("%s %s tcs3400_reset_regs failure \n", SENSOR_STR, __func__);
        return ret;
    }
    tcs3400_cal_cpl();
    if (current_state)
        tcs3400_als_enable(1);

    return 0;
}

static void tcs3400_dec_gain()
{
    int     ret;
    uint8_t gain = (tcs3400_chip.shadow[TCS3400_CONTROL]) & 0x3;
    uint8_t idx;

    if ((gain <= 0))
        return;
    for (idx = (ARRAY_SIZE(als_gains) - 1); idx >= 0; idx--) {
        if ((als_gains[idx] == 0) || (idx >= gain))
            continue;
        else if (idx < gain) {
            gain = idx;
            break;
        }
    }
    LOG("%s %s in the dec_gain the alg_gain value is %d \n", SENSOR_STR,
        __func__, als_gains[gain]);
    ret = tcs3400_set_als_gain(als_gains[gain]);
    if (ret == 0)
        tcs3400_cal_cpl();
}

static void tcs3400_inc_gain()
{
    int     ret;
    uint8_t gain = (tcs3400_chip.shadow[TCS3400_CONTROL]) & 0x3;
    uint8_t idx;

    LOG("kavin print gain value is %d\n", gain);
    if ((gain >= (ARRAY_SIZE(als_gains) - 1)))
        return;
    for (idx = 0; idx <= (ARRAY_SIZE(als_gains) - 1); idx++) {
        if ((als_gains[idx] == 0) || (idx <= gain))
            continue;
        else if (idx > gain) {
            gain = idx;
            break;
        }
    }

    LOG("%s %s in the inc_gain the alg_gain value is %d \n", SENSOR_STR,
        __func__, als_gains[gain]);
    ret = tcs3400_set_als_gain(als_gains[gain]);
    if (ret == 0)
        tcs3400_cal_cpl();
}


static int tcs3400_get_lux_cct()
{
    int32_t  rp1 = 0, gp1 = 0, bp1 = 0, cp1 = 0;
    int32_t  rp2 = 0, gp2 = 0, bp2 = 0;
    uint32_t lux, temp_lux, CPL    = 0;
    int32_t  ir, cct = 0, r_coef = R_Coef, g_coef = G_Coef,
                b_coef = B_Coef;

    int32_t quintile = (tcs3400_max_als_value() / 5);
    tcs3400_cal_cpl();

    rp1 = (tcs3400_chip.als_inf.red);
    gp1 = (tcs3400_chip.als_inf.green);
    bp1 = (tcs3400_chip.als_inf.blue);
    cp1 = (tcs3400_chip.als_inf.clear);
    ir  = (tcs3400_chip.als_inf.ir_data);
    CPL = (tcs3400_chip.als_inf.cpl);
    /*if(tcs3400_chip.shadow[TCS3400_CONTROL]==0x03)
        ir=0; */
    /* remove ir from counts */
    rp1 -= ir;
    gp1 -= ir;
    bp1 -= ir;
    cp1 -= ir;

    rp2 = r_coef * rp1;
    gp2 = g_coef * gp1;
    bp2 = b_coef * bp1;

    lux = (rp2 + gp2 + bp2);

    temp_lux = lux / CPL;
    // avoid div err;
    if (rp1 == 0)
        rp1 = 1;
    cct = ((CT_Coef * (uint32_t)bp1) / (uint32_t)rp1) + CT_Offset;

    if ((tcs3400_chip.als_inf.clear >= tcs3400_chip.als_inf.sat) ||
        tcs3400_chip.in_asat) {
        LOG("%s %s enter dec_gain", SENSOR_STR, __func__);
        tcs3400_dec_gain();
        tcs3400_reset_regs(&tcs3400_ctx);
    } else if (tcs3400_chip.als_inf.clear < quintile) {
        LOG("%s %s enter inc_gain", SENSOR_STR, __func__);
        tcs3400_inc_gain();
        tcs3400_reset_regs(&tcs3400_ctx);
    }

    if (temp_lux > 0) {
        tcs3400_chip.als_inf.lux = (uint16_t)temp_lux;
        tcs3400_chip.als_inf.cct = (uint16_t)cct;
    }

//    LOG("%s %s tcs3400_get_lux_cct lux is  %d, cct is %d \n", SENSOR_STR,
//        __func__, temp_lux, cct);
      return 0;
}

static int drv_als_ams_tcs3400_read(void *buf, size_t len)
{
  //  LOG("%s %s drv_als_ams_tcs3400_read enter \n", SENSOR_STR, __func__);
    int         ret;
    size_t      size;
    uint8_t     temp_zero  = 0;
    uint8_t     status     = 0;
    als_data_t *sensordata = (als_data_t *)buf;

    if (buf == NULL) {
        return -1;
    }
    size = sizeof(als_data_t);
    if (len < size) {
        return -1;
    }
    // LOG("%s %s drv_als_ams_tcs3400_read begin \n", SENSOR_STR, __func__);
    ret = sensor_i2c_read(&tcs3400_ctx, TCS3400_STATUS,
                          &(tcs3400_chip.shadow[TCS3400_STATUS]), I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if (unlikely(ret)) {
        LOG("%s %s TCS3400_READ_STATUS failure \n", SENSOR_STR, __func__);
        return ret;
    } else {
        status = tcs3400_chip.shadow[TCS3400_STATUS];
        if (status & TCS3400_ST_ALS_SAT)
            tcs3400_chip.in_asat = true;
        else
            tcs3400_chip.in_asat = false;
        if (((status & TCS3400_ST_ALS_IRQ) &&
             (status & TCS3400_ST_ALS_VALID)) ||
            (tcs3400_chip.in_asat)) {
            // clear interrupt
            sensor_i2c_write(&tcs3400_ctx, TCS3400_CMD_ALL_INT_CLR, &temp_zero,
                             I2C_DATA_LEN, I2C_OP_RETRIES);
            LOG("%s %s drv_als_ams_tcs3400_read enter read_als_data \n",
                SENSOR_STR, __func__);
            tcs3400_read_als_data();
            tcs3400_get_lux_cct();
        }
        sensordata->lux       = (uint32_t)(tcs3400_chip.als_inf.lux);
        sensordata->timestamp = aos_now_ms();
        return (int)size;
    }
    return 0;
}

static int drv_als_ams_tcs3400_write(const void *buf, size_t len)
{
    // no handle so far
    return 0;
}

static int drv_als_ams_tcs3400_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;

    switch (cmd) {
        case SENSOR_IOCTL_SET_POWER: {
            ret = drv_als_ams_tcs3400_set_power_mode(&tcs3400_ctx, arg);
            if (unlikely(ret)) {
                return -1;
            }
        } break;

        case SENSOR_IOCTL_GET_INFO: {
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
            info->model             = "TCS3400";
            info->unit              = lux;
        } break;

        default:
            break;
    }

    return 0;
}

int drv_als_ams_tcs3400_init(void)
{
    int          ret = 0;
    sensor_obj_t sensor_als;
    memset(&sensor_als, 0, sizeof(sensor_als));

    /* fill the sensor obj parameters here */
    sensor_als.tag        = TAG_DEV_ALS;
    sensor_als.path       = dev_als_path;
    sensor_als.io_port    = I2C_PORT;
    sensor_als.open       = drv_als_ams_tcs3400_open;
    sensor_als.close      = drv_als_ams_tcs3400_close;
    sensor_als.read       = drv_als_ams_tcs3400_read;
    sensor_als.write      = drv_als_ams_tcs3400_write;
    sensor_als.ioctl      = drv_als_ams_tcs3400_ioctl;
    sensor_als.irq_handle = drv_als_ams_tcs3400_irq_handle;

    ret = sensor_create_obj(&sensor_als);
    if (unlikely(ret)) {
        return -1;
    }

    ret = drv_als_ams_tcs3400_validate_id(&tcs3400_ctx, TCS3400_CHIPID_VALUE);
    if (unlikely(ret)) {
        return -1;
    }

    ret = drv_als_ams_tcs3400_set_default_config(&tcs3400_ctx);
    if (unlikely(ret)) {
        return -1;
    }
    return 0;
}

SENSOR_DRV_ADD(drv_als_ams_tcs3400_init);

