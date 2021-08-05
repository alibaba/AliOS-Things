/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef __QMC6310_H
#define __QMC6310_H
#include "k_api.h"
#include <math.h>
#include <stdbool.h>
#include "aos/hal/i2c.h"
#include "hal_iomux_haas1000.h"

/* vendor chip id*/
#define QMC6308_IIC_ADDR                0x2c
#define QMC6310U_IIC_ADDR               0x1c
#define QMC6310N_IIC_ADDR               0x3c
#define QMC6310_CHIP_ID_REG             0x00

/*data output register*/
#define QMC6310_DATA_OUT_X_LSB_REG      0x01
#define QMC6310_DATA_OUT_X_MSB_REG      0x02
#define QMC6310_DATA_OUT_Y_LSB_REG      0x03
#define QMC6310_DATA_OUT_Y_MSB_REG      0x04
#define QMC6310_DATA_OUT_Z_LSB_REG      0x05
#define QMC6310_DATA_OUT_Z_MSB_REG      0x06
/*Status registers */
#define QMC6310_STATUS_REG              0x09
/* configuration registers */
#define QMC6310_CTL_REG_ONE             0x0A  /* Contrl register one */
#define QMC6310_CTL_REG_TWO             0x0B  /* Contrl register two */

/* Magnetic Sensor Operating Mode MODE[1:0]*/
#define QMC6310_SUSPEND_MODE            0x00
#define QMC6310_NORMAL_MODE             0x01
#define QMC6310_SINGLE_MODE             0x02
#define QMC6310_H_PFM_MODE              0x03

/*data output rate OSR2[2:0]*/
#define OUTPUT_DATA_RATE_800HZ          0x00
#define OUTPUT_DATA_RATE_400HZ          0x01
#define OUTPUT_DATA_RATE_200HZ          0x02
#define OUTPUT_DATA_RATE_100HZ          0x03

/*oversample Ratio  OSR[1]*/
#define OVERSAMPLE_RATE_256             0x01
#define OVERSAMPLE_RATE_128             0x00

#define SET_RESET_ON                    0x00
#define SET_ONLY_ON                     0x01
#define SET_RESET_OFF                   0x02

#define QMC6310_DEFAULT_DELAY           200

enum {
    AXIS_X = 0,
    AXIS_Y = 1,
    AXIS_Z = 2,

    AXIS_TOTAL
};

typedef struct {
    signed char sign[3];
    unsigned char map[3];
} QMC6310_map;

extern int qmc6310_init(void);
extern uint8_t qmc6310_read_mag_xyz(float *data);
extern int qmc6310_write_reg(uint8_t addr, uint8_t data);
extern int qmc6310_read_block(uint8_t addr, uint8_t *data, uint8_t len);
extern int qmc6310_readHeading();
extern void qmc6310_deinit(void);
#endif

