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

#define TMD2725_REG_ENABLE 0x80
#define TMD2725_REG_ATIME 0x81
#define TMD2725_REG_PTIME 0x82
#define TMD2725_REG_WTIME 0x83
#define TMD2725_REG_AILT 0x84
#define TMD2725_REG_AILT_HI 0x85
#define TMD2725_REG_AIHT 0x86
#define TMD2725_REG_AIHT_HI 0x87
#define TMD2725_REG_PILT 0x88
#define TMD2725_REG_PIHT 0x8A
#define TMD2725_REG_PERS 0x8C
#define TMD2725_REG_CFG0 0x8D
#define TMD2725_REG_PGCFG0 0x8E
#define TMD2725_REG_PGCFG1 0x8F

#define TMD2725_REG_CFG1 0x90
#define TMD2725_REG_REVID 0x91
#define TMD2725_REG_CHIPID 0x92
#define TMD2725_REG_STATUS 0x93
#define TMD2725_REG_CH0DATA 0x94
#define TMD2725_REG_CH0DATA_HI 0x95
#define TMD2725_REG_CH1DATA 0x96
#define TMD2725_REG_CH1DATA_HI 0x97
#define TMD2725_REG_PDATA 0x9C

#define TMD2725_REG_ADCDATA_L 0x9D
#define TMD2725_REG_AUXID 0x9E
#define TMD2725_REG_CFG2 0x9F

#define TMD2725_REG_CFG3 0xAB
#define TMD2725_REG_CFG4 0xAC
#define TMD2725_REG_CFG5 0xAD

#define TMD2725_REG_POFFSET_L 0xC0
#define TMD2725_REG_POFFSET_H 0xC1

#define TMD2725_REG_AZ_CONFIG 0xD6
#define TMD2725_REG_CALIB 0xD7
#define TMD2725_REG_CALIBCFG 0xD9
#define TMD2725_REG_CALIBSTAT 0xDC
#define TMD2725_REG_INTENAB 0xDD

#define TMD2725_CHIPID_VALUE 0xE4
#define TMD2725_I2C_SLAVE_ADDR 0x39
#define TMD2725_ADDR_TRANS(n) ((n) << 1)
#define TMD2725_I2C_ADDR TMD2725_ADDR_TRANS(TMD2725_I2C_SLAVE_ADDR)


enum tmd2725_reg
{
    TMD2725_MASK_BINSRCH_TARGET  = 0x70,
    TMD2725_SHIFT_BINSRCH_TARGET = 4,

    TMD2725_MASK_START_OFFSET_CALIB = 0x01,

    TMD2725_MASK_PROX_PERS = 0xf0,

    TMD2725_MASK_PDRIVE = 0x1f,

    TMD2725_MASK_PGAIN  = 0xC0,
    TMD2725_SHIFT_PGAIN = 6,

    TMD2725_MASK_AGAIN  = 0x03,
    TMD2725_SHIFT_AGAIN = 0,

    TMD2725_MASK_APERS = 0x0f,

    TMD2725_MASK_POFFSET_H  = 0x01,
    TMD2725_SHIFT_POFFSET_H = 0,

    TMD2725_MASK_PROX_DATA_AVG  = 0x07, // AVG 8counts
    TMD2725_SHIFT_PROX_DATA_AVG = 0,

    TMD2725_MASK_PROX_AUTO_OFFSET_ADJUST  = 0x08,
    TMD2725_SHIFT_PROX_AUTO_OFFSET_ADJUST = 3,
};

enum tmd2725_en_reg
{
    TMD2725_PON    = (1 << 0),
    TMD2725_AEN    = (1 << 1),
    TMD2725_PEN    = (1 << 2),
    TMD2725_WEN    = (1 << 3),
    TMD2725_EN_ALL = (TMD2725_AEN | TMD2725_PEN | TMD2725_WEN),
};

static uint8_t const regs[] = {
    TMD2725_REG_PILT,   TMD2725_REG_PIHT, TMD2725_REG_PERS,  TMD2725_REG_PGCFG0,
    TMD2725_REG_PGCFG1, TMD2725_REG_CFG1, TMD2725_REG_PTIME, TMD2725_REG_ATIME,
};

static uint8_t const als_regs[] = {
    TMD2725_REG_ATIME, TMD2725_REG_WTIME, TMD2725_REG_PERS,
    TMD2725_REG_CFG0,  TMD2725_REG_CFG1,
};

enum tmd2725_status
{
    TMD2725_ST_PGSAT_AMBIENT  = (1 << 0),
    TMD2725_ST_PGSAT_RELFLECT = (1 << 1),
    TMD2725_ST_ZERODET        = (1 << 2),
    TMD2725_ST_CAL_IRQ        = (1 << 3),
    TMD2725_ST_ALS_IRQ        = (1 << 4),
    TMD2725_ST_PRX_IRQ        = (1 << 5),
    TMD2725_ST_PRX_SAT        = (1 << 6),
    TMD2725_ST_ALS_SAT        = (1 << 7),
};

enum tmd2725_intenab_reg
{
    TMD2725_CIEN  = (1 << 3),
    TMD2725_AIEN  = (1 << 4),
    TMD2725_PIEN  = (1 << 5),
    TMD2725_PSIEN = (1 << 6),
    TMD2725_ASIEN = (1 << 7),
};

enum tmd2725_pwr_state
{
    POWER_ON,
    POWER_OFF,
    POWER_STANDBY,
};

enum tmd2725_prox_state
{
    PROX_STATE_NONE = 0,
    PROX_STATE_INIT,
    PROX_STATE_CALIB,
    PROX_STATE_WAIT_AND_CALIB
};

enum tmd2725_ctrl_reg
{
    AGAIN_1       = (0 << 0),
    AGAIN_4       = (1 << 0),
    AGAIN_16      = (2 << 0),
    AGAIN_64      = (3 << 0),
    PGAIN_1       = (0 << TMD2725_SHIFT_PGAIN),
    PGAIN_2       = (1 << TMD2725_SHIFT_PGAIN),
    PGAIN_4       = (2 << TMD2725_SHIFT_PGAIN),
    PGAIN_8       = (3 << TMD2725_SHIFT_PGAIN),
    PG_PULSE_4US  = (0 << 6),
    PG_PULSE_8US  = (1 << 6),
    PG_PULSE_16US = (2 << 6),
    PG_PULSE_32US = (3 << 6),
};

static uint8_t const als_gains[] = { 1, 4, 16, 64 };
// pldriver
#define PDRIVE_MA(p) (((uint8_t)((p) / 6) - 1) & 0x1f)
#define P_TIME_US(p) ((((p) / 88) - 1.0) + 0.5)
#define PRX_PERSIST(p) (((p)&0xf) << 4)

#define INTEGRATION_CYCLE 2816
#define AW_TIME_MS(p) \
    ((((p)*1000) + (INTEGRATION_CYCLE - 1)) / INTEGRATION_CYCLE)
#define ALS_PERSIST(p) (((p)&0xf) << 0)

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
// lux
#define INDOOR_LUX_TRIGGER 6000
#define OUTDOOR_LUX_TRIGGER 10000
#define TMD2725_MAX_LUX 0xffff
#define TMD2725_MAX_ALS_VALUE 0xffff
#define TMD2725_MIN_ALS_VALUE 10

/* Default LUX coefficients */
#define DFG 56
#define CoefA 1000
#define CoefB 82
#define CoefC 788
#define CoefD 41
#define MAX_REGS 256

#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))

struct tmd2725_lux_segment
{
    uint32_t ch0_coef;
    uint32_t ch1_coef;
};

struct tmd2725_als_inf
{
    uint16_t als_ch0;
    uint16_t als_ch1;
    uint32_t CPL;
    uint32_t lux1_ch0_coef;
    uint32_t lux1_ch1_coef;
    uint32_t lux2_ch0_coef;
    uint32_t lux2_ch1_coef;
    uint32_t sat;
    uint16_t lux;
};

struct tmd2725_prox_inf
{
    uint16_t raw;
    int      detected;
};

struct tmd2725_chips
{
    bool    als_enabled;
    uint8_t atime;
    uint8_t again;
    uint8_t persist;

    bool    prox_enabled;
    int16_t prox_th_min;
    int16_t prox_th_max;
    uint8_t prox_gain;
    uint8_t prox_drive;
    uint8_t prox_pulse_cnt;
    uint8_t prox_pulse_len;
    int16_t prox_offset;

    uint8_t                    shadow[MAX_REGS];
    struct tmd2725_lux_segment lux_segment[2];
    struct tmd2725_prox_inf    prx_inf;
    struct tmd2725_als_inf     als_inf;
};

i2c_dev_t tmd2725_ctx = {
    .port                 = 3,
    .config.address_width = 8,
    .config.freq          = 400000,
    .config.dev_addr      = TMD2725_I2C_ADDR,
};

static struct tmd2725_chips tmd2725_chip;
static uint8_t              init_flag = 0;

static int drv_als_ps_ams_tmd2725_validate_id(i2c_dev_t *drv, uint8_t id_value)
{
    int     ret = 0;
    uint8_t chipid_value;
    ret = sensor_i2c_read(drv, TMD2725_REG_CHIPID, &chipid_value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if (unlikely(ret)) {
        LOG("%s %s Sensor_i2c_read failure \n", SENSOR_STR, __func__);
        return ret;
    }
    if (chipid_value != id_value)
        return -1;

    return 0;
}

static int tmd2725_reset_als_regs(i2c_dev_t *drv)
{
    int     i;
    int     ret = 0;
    uint8_t reg;
    for (i = 0; i < ARRAY_SIZE(als_regs); i++) {
        reg = als_regs[i];
        ret = sensor_i2c_write(drv, reg, &(tmd2725_chip.shadow[reg]),
                               I2C_DATA_LEN, I2C_OP_RETRIES);
        if (unlikely(ret)) {
            LOG("%s %s tmd2725 reset_als_regs failure \n", SENSOR_STR,
                __func__);
            return ret;
        }
    }
    return 0;
}

static int32_t sensor_i2c_modify(i2c_dev_t *drv, uint8_t *shadow, uint16_t reg,
                                 uint8_t mask, uint8_t value)
{
    int     ret;
    uint8_t temp;
    ret = sensor_i2c_read(drv, reg, &temp, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        LOG("%s %s tmd2725 sensor_i2c_modify \n", SENSOR_STR, __func__);
        return ret;
    }
    temp &= ~mask;
    temp |= value;
    sensor_i2c_write(drv, reg, &temp, I2C_DATA_LEN, I2C_OP_RETRIES);
    shadow[reg] = temp;
    return 0;
}

static int drv_als_ams_tmd2725_set_power_mode(i2c_dev_t *      drv,
                                              dev_power_mode_e mode)
{
    switch (mode) {
        case DEV_POWER_ON: {
            if (!(tmd2725_chip.als_enabled)) {
                tmd2725_chip.shadow[TMD2725_REG_ATIME] = tmd2725_chip.atime;
                tmd2725_chip.shadow[TMD2725_REG_PERS] &= (~TMD2725_MASK_APERS);
                tmd2725_chip.shadow[TMD2725_REG_PERS] |= 0x02;
                tmd2725_reset_als_regs(drv);
                sensor_i2c_modify(drv, tmd2725_chip.shadow, TMD2725_REG_INTENAB,
                                  TMD2725_AIEN, TMD2725_AIEN);
                sensor_i2c_modify(drv, tmd2725_chip.shadow, TMD2725_REG_ENABLE,
                                  TMD2725_WEN | TMD2725_AEN | TMD2725_PON,
                                  TMD2725_WEN | TMD2725_AEN | TMD2725_PON);
                tmd2725_chip.als_enabled = true;
            }
        } break;
        case DEV_POWER_OFF:
        case DEV_SLEEP: {
            sensor_i2c_modify(drv, tmd2725_chip.shadow, TMD2725_REG_INTENAB,
                              TMD2725_AIEN, 0);
            sensor_i2c_modify(drv, tmd2725_chip.shadow, TMD2725_REG_ENABLE,
                              TMD2725_WEN | TMD2725_AEN, 0);
            tmd2725_chip.als_enabled = false;
            if (!(tmd2725_chip.shadow[TMD2725_REG_ENABLE] & TMD2725_EN_ALL))
                sensor_i2c_modify(drv, tmd2725_chip.shadow, TMD2725_REG_ENABLE,
                                  TMD2725_PON, 0);
        } break;
        default:
            break;
    }
    return 0;
}

static int tmd2725_reset_regs(i2c_dev_t *drv)
{
    int     i;
    int     ret = 0;
    uint8_t reg;
    for (i = 0; i < ARRAY_SIZE(regs); i++) {
        reg = regs[i];
        ret = sensor_i2c_write(drv, reg, &(tmd2725_chip.shadow[reg]),
                               I2C_DATA_LEN, I2C_OP_RETRIES);
        if (unlikely(ret)) {
            LOG("%s %s tmd2725 reset_regs failure  %d \n", SENSOR_STR,
                __func__);
            return ret;
        }
    }
    return 0;
}

static int drv_als_ps_ams_tmd2725_set_default_config(i2c_dev_t *drv)
{
    int ret = 0;
    if (!init_flag) {
        tmd2725_chip.prox_th_min    = 50;
        tmd2725_chip.prox_th_max    = 80;
        tmd2725_chip.persist        = PRX_PERSIST(0) | ALS_PERSIST(0);
        tmd2725_chip.prox_pulse_cnt = 4;
        tmd2725_chip.prox_gain      = PGAIN_4;
        tmd2725_chip.prox_drive     = PDRIVE_MA(75);
        tmd2725_chip.prox_offset    = 0;
        tmd2725_chip.prox_pulse_len = PG_PULSE_16US;
        tmd2725_chip.again          = AGAIN_16;
        tmd2725_chip.atime          = AW_TIME_MS(200);

        tmd2725_chip.lux_segment[0].ch0_coef = CoefA;
        tmd2725_chip.lux_segment[0].ch1_coef = CoefB;
        tmd2725_chip.lux_segment[1].ch0_coef = CoefC;
        tmd2725_chip.lux_segment[1].ch1_coef = CoefD;

        tmd2725_chip.als_enabled  = false;
        tmd2725_chip.prox_enabled = false;

        /* initial some config register */
        tmd2725_chip.shadow[TMD2725_REG_PILT] = tmd2725_chip.prox_th_min & 0xff;
        tmd2725_chip.shadow[TMD2725_REG_PIHT] = tmd2725_chip.prox_th_max & 0xff;
        tmd2725_chip.shadow[TMD2725_REG_PERS] = tmd2725_chip.persist;
        tmd2725_chip.shadow[TMD2725_REG_PGCFG0] =
          tmd2725_chip.prox_pulse_cnt | tmd2725_chip.prox_pulse_len;
        tmd2725_chip.shadow[TMD2725_REG_ATIME] = tmd2725_chip.atime;
        tmd2725_chip.shadow[TMD2725_REG_CFG1]  = tmd2725_chip.again;
        tmd2725_chip.shadow[TMD2725_REG_PTIME] = P_TIME_US(2816);
        tmd2725_chip.shadow[TMD2725_REG_PGCFG1] =
          (tmd2725_chip.prox_gain & TMD2725_MASK_PGAIN) |
          tmd2725_chip.prox_drive;

        struct tmd2725_lux_segment *als    = &(tmd2725_chip.lux_segment[0]);
        tmd2725_chip.als_inf.lux1_ch0_coef = DFG * (als[0].ch0_coef);
        tmd2725_chip.als_inf.lux1_ch1_coef = DFG * (als[0].ch1_coef);
        tmd2725_chip.als_inf.lux2_ch0_coef = DFG * (als[1].ch0_coef);
        tmd2725_chip.als_inf.lux2_ch1_coef = DFG * (als[1].ch1_coef);

        ret = tmd2725_reset_regs(&tmd2725_ctx);
        if (unlikely(ret)) {
            LOG("%s  %s , tmd2725 reset regs failure \n", SENSOR_STR, __func__);
            return ret;
        }
        init_flag = 1;
    }
    return 0;
}

static void drv_als_ams_tmd2725_irq_handle(void)
{
    /* no handle so far */
}

static int drv_als_ams_tmd2725_open(void)
{
    int ret = 0;
    ret     = drv_als_ams_tmd2725_set_power_mode(&tmd2725_ctx, DEV_POWER_ON);
    if (unlikely(ret)) {
        return -1;
    }
    return 0;
}

static int drv_als_ams_tmd2725_close(void)
{
    int ret = 0;
    ret     = drv_als_ams_tmd2725_set_power_mode(&tmd2725_ctx, DEV_POWER_OFF);
    if (unlikely(ret)) {
        return -1;
    }
    return 0;
}

static int tmd2725_read_als_data()
{
    int      ret;
    uint8_t *buf;
    ret = sensor_i2c_read(&tmd2725_ctx, TMD2725_REG_CH0DATA,
                          &(tmd2725_chip.shadow[TMD2725_REG_CH0DATA]), 4,
                          I2C_OP_RETRIES);
    if (unlikely(ret)) {
        LOG("%s %s tmd2725_read_als_data failure \n", SENSOR_STR, __func__);
        return ret;
    }
    buf                          = &(tmd2725_chip.shadow[TMD2725_REG_CH0DATA]);
    tmd2725_chip.als_inf.als_ch0 = (uint16_t)((buf[1] << 8) | buf[0]);
    tmd2725_chip.als_inf.als_ch1 = (uint16_t)((buf[3] << 8) | buf[2]);

    //   LOG("%s  %s als_ch0 is %d als_ch1 is %d \n",
    //   SENSOR_STR,__func__,tmd2725_chip.als_inf.als_ch0,tmd2725_chip.als_inf.als_ch1);

    return 0;
}

static int tmd2725_als_cal_cpl()
{
    uint32_t cpl;
    uint32_t sat;
    uint8_t  atime;
    atime = tmd2725_chip.shadow[TMD2725_REG_ATIME];
    cpl   = atime;
    cpl *= INTEGRATION_CYCLE;
    cpl *=
      als_gains[tmd2725_chip.shadow[TMD2725_REG_CFG1] & TMD2725_MASK_AGAIN];

    sat = (int32_t)(atime << 10);
    if (sat > TMD2725_MAX_ALS_VALUE)
        sat = TMD2725_MAX_ALS_VALUE;

    sat                      = sat * 8 / 10;
    tmd2725_chip.als_inf.CPL = cpl;
    tmd2725_chip.als_inf.sat = sat;

    //   LOG("CPL is %d  sat is %d, atime is  %d  again is
    //   %d\n",tmd2725_chip.als_inf.CPL,tmd2725_chip.als_inf.sat,
    //   tmd2725_chip.atime, tmd2725_chip.again);
    return 0;
}

static int tmd2725_set_als_gain(uint8_t gain)
{
    uint8_t ctrl_reg;
    uint8_t temp, temp_zero = 0;
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
            LOG("set als_gain wrong Gain data \n");
            return -1;
    }
    sensor_i2c_read(&tmd2725_ctx, TMD2725_REG_ENABLE, &temp, I2C_DATA_LEN,
                    I2C_OP_RETRIES);
    sensor_i2c_write(&tmd2725_ctx, TMD2725_REG_ENABLE, &temp_zero, I2C_DATA_LEN,
                     I2C_OP_RETRIES);
    sensor_i2c_modify(&tmd2725_ctx, tmd2725_chip.shadow, TMD2725_REG_CFG1,
                      TMD2725_MASK_AGAIN, ctrl_reg);
    sensor_i2c_write(&tmd2725_ctx, TMD2725_REG_ENABLE, &temp, I2C_DATA_LEN,
                     I2C_OP_RETRIES);

    tmd2725_chip.again = tmd2725_chip.shadow[TMD2725_REG_CFG1];
    return 0;
}


static int tmd2725_als_inc_gain()
{
    int     ret;
    uint8_t gain = (tmd2725_chip.shadow[TMD2725_REG_CFG1] & TMD2725_MASK_AGAIN);
    if (gain > AGAIN_16)
        return 1;
    else if (gain < AGAIN_4)
        gain = als_gains[AGAIN_4];
    else if (gain < AGAIN_16)
        gain = als_gains[AGAIN_16];
    else
        gain = als_gains[AGAIN_64];

    ret = tmd2725_set_als_gain(gain);
    if (unlikely(ret)) {
        LOG("%s %s tmd2725_set_als_gain false\n", SENSOR_STR, __func__);
        return ret;
    }
    tmd2725_als_cal_cpl();
    return 0;
}

static int tmd2725_als_dec_gain()
{
    int     ret;
    uint8_t gain = (tmd2725_chip.shadow[TMD2725_REG_CFG1] & TMD2725_MASK_AGAIN);
    if (gain == AGAIN_1)
        return 1;
    else if (gain > AGAIN_16)
        gain = als_gains[AGAIN_16];
    else if (gain > AGAIN_4)
        gain = als_gains[AGAIN_4];
    else
        gain = als_gains[AGAIN_1];

    ret = tmd2725_set_als_gain(gain);
    if (unlikely(ret)) {
        LOG("%s %s tmd2725_set_als_gain false in dec_gain\n", SENSOR_STR,
            __func__);
        return ret;
    }
    tmd2725_als_cal_cpl();
    return 0;
}

static int tmd2725_max_als_value()
{
    int val;
    val = tmd2725_chip.shadow[TMD2725_REG_ATIME];

    if (val > 63)
        val = 0xffff;
    else
        val = ((val * 1024) - 1);

    return val;
}
static int tmd2725_get_lux()
{
    int ch0, ch1, lux1, lux2, lux;
    ch0 = tmd2725_chip.als_inf.als_ch0;
    ch1 = tmd2725_chip.als_inf.als_ch1;

    tmd2725_als_cal_cpl();
    lux1 = ((tmd2725_chip.als_inf.lux1_ch0_coef * ch0) -
            (tmd2725_chip.als_inf.lux1_ch1_coef * ch1)) /
           tmd2725_chip.als_inf.CPL;
    lux2 = ((tmd2725_chip.als_inf.lux2_ch0_coef * ch0) -
            (tmd2725_chip.als_inf.lux2_ch1_coef * ch1)) /
           tmd2725_chip.als_inf.CPL;
    lux = max(lux1, lux2);
    lux = min(TMD2725_MAX_LUX, max(0, lux));

    tmd2725_chip.als_inf.lux = lux;

    if (ch0 < 100) {
        tmd2725_als_inc_gain();
        tmd2725_reset_als_regs(&tmd2725_ctx);
    } else if (ch0 >= tmd2725_max_als_value()) {
        tmd2725_als_dec_gain();
        tmd2725_reset_als_regs(&tmd2725_ctx);
    }
    return 0;
}

static int drv_als_ams_tmd2725_read(void *buf, size_t len)
{
    int         ret;
    size_t      size;
    uint8_t     status     = 0;
    als_data_t *sensordata = (als_data_t *)buf;

    if (buf == NULL) {
        return -1;
    }
    size = sizeof(als_data_t);
    if (len < size) {
        return -1;
    }

    ret = sensor_i2c_read(&tmd2725_ctx, TMD2725_REG_STATUS,
                          &(tmd2725_chip.shadow[TMD2725_REG_STATUS]),
                          I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        LOG("%s %s TMD2725_READ_STATUS failure \n", SENSOR_STR, __func__);
        return ret;
    }
    status = tmd2725_chip.shadow[TMD2725_REG_STATUS];
    if (status & TMD2725_ST_ALS_IRQ) {
        // clear AINT
        sensor_i2c_modify(&tmd2725_ctx, tmd2725_chip.shadow, TMD2725_REG_STATUS,
                          TMD2725_ST_ALS_IRQ, 0);
        tmd2725_read_als_data();
        tmd2725_get_lux();
    }

    sensordata->lux       = (uint32_t)(tmd2725_chip.als_inf.lux);
    sensordata->timestamp = aos_now_ms();
    return (int)size;
}

static int drv_als_ams_tmd2725_write(const void *buf, size_t len)
{

    // no handle so far
    return 0;
}

static int drv_als_ams_tmd2725_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;
    switch (cmd) {
        case SENSOR_IOCTL_SET_POWER: {
            ret = drv_als_ams_tmd2725_set_power_mode(&tmd2725_ctx, arg);
            if (unlikely(ret)) {
                return -1;
            }
        } break;
        case SENSOR_IOCTL_GET_INFO: {
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
            info->model             = "TMD2725_ALS";
            info->unit              = lux;
        } break;
        default:
            break;
    }
    return 0;
}

int drv_als_ams_tmd2725_init(void)
{
    int          ret = 0;
    sensor_obj_t sensor_als;
    memset(&sensor_als, 0, sizeof(sensor_als));
    /* fill the sensor obj parameters here */
    sensor_als.tag        = TAG_DEV_ALS;
    sensor_als.path       = dev_als_path;
    sensor_als.io_port    = I2C_PORT;
    sensor_als.open       = drv_als_ams_tmd2725_open;
    sensor_als.close      = drv_als_ams_tmd2725_close;
    sensor_als.read       = drv_als_ams_tmd2725_read;
    sensor_als.write      = drv_als_ams_tmd2725_write;
    sensor_als.ioctl      = drv_als_ams_tmd2725_ioctl;
    sensor_als.irq_handle = drv_als_ams_tmd2725_irq_handle;

    ret = sensor_create_obj(&sensor_als);
    if (unlikely(ret)) {
        return -1;
    }
    ret =
      drv_als_ps_ams_tmd2725_validate_id(&tmd2725_ctx, TMD2725_CHIPID_VALUE);
    if (unlikely(ret)) {
        return -1;
    }

    ret = drv_als_ps_ams_tmd2725_set_default_config(&tmd2725_ctx);
    if (unlikely(ret)) {
        return -1;
    }
    return 0;
}

static uint32_t ams_sensor_i2c_write(i2c_dev_t *drv, uint8_t *shadow,
                                     uint16_t reg, uint8_t data)
{
    sensor_i2c_write(drv, reg, &data, I2C_DATA_LEN, I2C_OP_RETRIES);
    tmd2725_chip.shadow[reg] = data;
    return 0;
}

int tmd2725_offset_calibration()
{
    uint8_t temp;
    sensor_i2c_read(&tmd2725_ctx, TMD2725_REG_ENABLE, &temp, I2C_DATA_LEN,
                    I2C_OP_RETRIES);
    // enable prox enb bit
    sensor_i2c_modify(&tmd2725_ctx, tmd2725_chip.shadow, TMD2725_REG_ENABLE,
                      TMD2725_PEN | TMD2725_PON, TMD2725_PON);
    sensor_i2c_modify(&tmd2725_ctx, tmd2725_chip.shadow, TMD2725_REG_INTENAB,
                      TMD2725_PIEN | TMD2725_CIEN, TMD2725_CIEN);
    sensor_i2c_modify(&tmd2725_ctx, tmd2725_chip.shadow, TMD2725_REG_CALIBCFG,
                      TMD2725_MASK_BINSRCH_TARGET | TMD2725_MASK_PROX_DATA_AVG |
                        TMD2725_MASK_PROX_AUTO_OFFSET_ADJUST,
                      (0x03 << TMD2725_SHIFT_BINSRCH_TARGET) |
                        (1 << TMD2725_SHIFT_PROX_DATA_AVG) |
                        (1 << TMD2725_SHIFT_PROX_AUTO_OFFSET_ADJUST));

    sensor_i2c_modify(&tmd2725_ctx, tmd2725_chip.shadow, TMD2725_REG_CALIB,
                      TMD2725_MASK_START_OFFSET_CALIB, 0x01);
    // set calibration default time 100ms
    aos_msleep(100);

    sensor_i2c_read(&tmd2725_ctx, TMD2725_REG_POFFSET_L,
                    &tmd2725_chip.shadow[TMD2725_REG_POFFSET_L], 2,
                    I2C_OP_RETRIES);
    tmd2725_chip.prox_offset = tmd2725_chip.shadow[TMD2725_REG_POFFSET_L];
    if (tmd2725_chip.shadow[TMD2725_REG_POFFSET_H] & TMD2725_MASK_POFFSET_H) {
        tmd2725_chip.prox_offset *= -1;
    }
    sensor_i2c_modify(&tmd2725_ctx, tmd2725_chip.shadow, TMD2725_REG_ENABLE,
                      TMD2725_PEN, temp);
    return 0;
}

static int drv_ps_ams_tmd2725_set_power_mode(i2c_dev_t *      drv,
                                             dev_power_mode_e mode)
{
    switch (mode) {
        case DEV_POWER_ON: {
            if (!(tmd2725_chip.prox_enabled)) {
                tmd2725_chip.prox_th_min = 50;
                tmd2725_chip.prox_th_max = 80;
                ams_sensor_i2c_write(&tmd2725_ctx, tmd2725_chip.shadow,
                                     TMD2725_REG_PILT,
                                     tmd2725_chip.prox_th_min);
                ams_sensor_i2c_write(&tmd2725_ctx, tmd2725_chip.shadow,
                                     TMD2725_REG_PIHT,
                                     tmd2725_chip.prox_th_max);
                tmd2725_offset_calibration();

                sensor_i2c_modify(&tmd2725_ctx, tmd2725_chip.shadow,
                                  TMD2725_REG_PERS, TMD2725_MASK_PROX_PERS,
                                  tmd2725_chip.persist &
                                    TMD2725_MASK_PROX_PERS);

                ams_sensor_i2c_write(
                  &tmd2725_ctx, tmd2725_chip.shadow, TMD2725_REG_PGCFG0,
                  tmd2725_chip.prox_pulse_cnt | tmd2725_chip.prox_pulse_len);

                sensor_i2c_modify(
                  &tmd2725_ctx, tmd2725_chip.shadow, TMD2725_REG_PGCFG1,
                  (tmd2725_chip.prox_gain & TMD2725_MASK_PGAIN) |
                    (tmd2725_chip.prox_drive & TMD2725_MASK_PDRIVE),
                  TMD2725_MASK_PGAIN | TMD2725_MASK_PDRIVE);
                ams_sensor_i2c_write(&tmd2725_ctx, tmd2725_chip.shadow,
                                     TMD2725_REG_PTIME, P_TIME_US(2816));

                // enable proximity and interrupt
                sensor_i2c_modify(&tmd2725_ctx, tmd2725_chip.shadow,
                                  TMD2725_REG_ENABLE, TMD2725_PEN | TMD2725_PON,
                                  TMD2725_PEN | TMD2725_PON);
                sensor_i2c_modify(&tmd2725_ctx, tmd2725_chip.shadow,
                                  TMD2725_REG_INTENAB, TMD2725_PIEN,
                                  TMD2725_PIEN);
                tmd2725_chip.prox_enabled     = true;
                tmd2725_chip.prx_inf.detected = false;
            }
        } break;

        case DEV_POWER_OFF:

        case DEV_SLEEP: {
            if (tmd2725_chip.prox_enabled) {
                sensor_i2c_modify(&tmd2725_ctx, tmd2725_chip.shadow,
                                  TMD2725_REG_ENABLE, TMD2725_PEN, 0);
                sensor_i2c_modify(&tmd2725_ctx, tmd2725_chip.shadow,
                                  TMD2725_REG_INTENAB, TMD2725_PIEN, 0);
                tmd2725_chip.prox_enabled = false;
                if (!(tmd2725_chip.shadow[TMD2725_REG_ENABLE] & TMD2725_EN_ALL))
                    sensor_i2c_modify(drv, tmd2725_chip.shadow,
                                      TMD2725_REG_ENABLE, TMD2725_PON, 0);
            }
        } break;

        default:
            break;
    }
    return 0;
}

static void drv_ps_ams_tmd2725_irq_handle(void)
{
    /* no handle so far */
}

static int drv_ps_ams_tmd2725_open(void)
{
    int ret = 0;
    ret     = drv_ps_ams_tmd2725_set_power_mode(&tmd2725_ctx, DEV_POWER_ON);
    if (unlikely(ret)) {
        return -1;
    }
    return 0;
}

static int drv_ps_ams_tmd2725_close(void)
{
    int ret = 0;
    ret     = drv_ps_ams_tmd2725_set_power_mode(&tmd2725_ctx, DEV_POWER_OFF);
    if (unlikely(ret)) {
        return -1;
    }
    return 0;
}

static void tmd2725_read_prox_data()
{
    sensor_i2c_read(&tmd2725_ctx, TMD2725_REG_PDATA,
                    &tmd2725_chip.shadow[TMD2725_REG_PDATA], I2C_DATA_LEN,
                    I2C_OP_RETRIES);
    tmd2725_chip.prx_inf.raw = tmd2725_chip.shadow[TMD2725_REG_PDATA];
    //    LOG("%s  %s  read_prox_data raw data is
    //    %d\n",SENSOR_STR,__func__,tmd2725_chip.prx_inf.raw);
}

static void tmd2725_get_prox()
{
    if (tmd2725_chip.prx_inf.detected == false) {
        if (tmd2725_chip.prx_inf.raw > tmd2725_chip.prox_th_max) {
            tmd2725_chip.prx_inf.detected = true;
        }
    } else {
        if (tmd2725_chip.prx_inf.raw < tmd2725_chip.prox_th_min) {
            tmd2725_chip.prx_inf.detected = false;
        }
    }
}

static int drv_ps_ams_tmd2725_read(void *buf, size_t len)
{
    size_t size;

    proximity_data_t *sensordata = (proximity_data_t *)buf;

    if (buf == NULL) {
        return -1;
    }
    size = sizeof(proximity_data_t);
    if (len < size) {
        return -1;
    }

    tmd2725_read_prox_data();
    tmd2725_get_prox();
    //prx_inf.detected describe the near or far, and prx_inf.raw is pdata
    //sensordata->present   = tmd2725_chip.prx_inf.detected;
    sensordata->present = tmd2725_chip.prx_inf.raw;
    sensordata->timestamp = aos_now_ms();
    return (int)size;
}

static int drv_ps_ams_tmd2725_write(const void *buf, size_t len)
{
    // no handle so far
    return 0;
}

static int drv_ps_ams_tmd2725_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;
    switch (cmd) {
        case SENSOR_IOCTL_SET_POWER: {
            ret = drv_ps_ams_tmd2725_set_power_mode(&tmd2725_ctx, arg);
            if (unlikely(ret)) {
                return -1;
            }
        } break;
        case SENSOR_IOCTL_GET_INFO: {
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
            info->model             = "TMD2725_PS";
            info->unit              = cm;
        } break;
        default:
            break;
    }
    return 0;
}

int drv_ps_ams_tmd2725_init(void)
{
    int          ret = 0;
    sensor_obj_t sensor_ps;
    memset(&sensor_ps, 0, sizeof(sensor_ps));
    /* fill the sensor obj parameters here */
    sensor_ps.tag        = TAG_DEV_PS;
    sensor_ps.path       = dev_ps_path;
    sensor_ps.io_port    = I2C_PORT;
    sensor_ps.open       = drv_ps_ams_tmd2725_open;
    sensor_ps.close      = drv_ps_ams_tmd2725_close;
    sensor_ps.read       = drv_ps_ams_tmd2725_read;
    sensor_ps.write      = drv_ps_ams_tmd2725_write;
    sensor_ps.ioctl      = drv_ps_ams_tmd2725_ioctl;
    sensor_ps.irq_handle = drv_ps_ams_tmd2725_irq_handle;

    ret = sensor_create_obj(&sensor_ps);
    if (unlikely(ret)) {
        return -1;
    }
    ret =
      drv_als_ps_ams_tmd2725_validate_id(&tmd2725_ctx, TMD2725_CHIPID_VALUE);
    if (unlikely(ret)) {
        return -1;
    }
    ret = drv_als_ps_ams_tmd2725_set_default_config(&tmd2725_ctx);
    if (unlikely(ret)) {
        return -1;
    }
    return 0;
}


SENSOR_DRV_ADD(drv_als_ams_tmd2725_init);
SENSOR_DRV_ADD(drv_ps_ams_tmd2725_init);

