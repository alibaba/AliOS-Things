/*
* Copyright (C) 2015-2017 Alibaba Group Holding Limited
*
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <aos/aos.h>
#include <vfs_conf.h>
#include <vfs_err.h>
#include <vfs_register.h>
#include <hal/base.h>
#include "common.h"
#include "hal/sensor.h"

#define	BMG160_I2C_ADDR1				           (0x68<<1)
#define	BMG160_I2C_ADDR2				           (0x69<<1)

#define BMG160_CHIP_ID_ADDR						   (0x00)
#define BMG160_CHIP_ID_VALUE					   (0x0F)

#define BMG160_RATE_X_LSB_ADDR                     (0x02)
#define BMG160_RATE_X_MSB_ADDR                     (0x03)
#define BMG160_RATE_Y_LSB_ADDR                     (0x04)
#define BMG160_RATE_Y_MSB_ADDR                     (0x05)
#define BMG160_RATE_Z_LSB_ADDR                     (0x06)
#define BMG160_RATE_Z_MSB_ADDR                     (0x07)
#define BMG160_TEMP_ADDR                           (0x08)
#define BMG160_INTR_STAT0_ADDR                     (0x09)
#define BMG160_INTR_STAT1_ADDR                     (0x0A)
#define BMG160_INTR_STAT2_ADDR                     (0x0B)
#define BMG160_INTR_STAT3_ADDR                     (0x0C)
#define BMG160_FIFO_STAT_ADDR                      (0x0E)
#define BMG160_RANGE_ADDR                          (0x0F)
#define BMG160_BW_ADDR                             (0x10)
#define BMG160_MODE_LPM1_ADDR                      (0x11)
#define BMG160_MODE_LPM2_ADDR                      (0x12)
#define BMG160_HIGH_BW_ADDR                        (0x13)
#define BMG160_BGW_SOFT_RST_ADDR                   (0x14)
#define BMG160_INTR_ENABLE0_ADDR                   (0x15)
#define BMG160_INTR_ENABLE1_ADDR                   (0x16)
#define BMG160_INTR_MAP_ZERO_ADDR                  (0x17)
#define BMG160_INTR_MAP_ONE_ADDR                   (0x18)
#define BMG160_INTR_MAP_TWO_ADDR                   (0x19)
#define BMG160_INTR_ZERO_ADDR                      (0x1A)
#define BMG160_INTR_ONE_ADDR                       (0x1B)
#define BMG160_INTR_TWO_ADDR                       (0x1C)
#define BMG160_INTR_4_ADDR                         (0x1E)
#define BMG160_RST_LATCH_ADDR                      (0x21)
#define BMG160_HIGHRATE_THRES_X_ADDR               (0x22)
#define BMG160_HIGHRATE_DURN_X_ADDR                (0x23)
#define BMG160_HIGHRATE_THRES_Y_ADDR               (0x24)
#define BMG160_HIGHRATE_DURN_Y_ADDR                (0x25)
#define BMG160_HIGHRATE_THRES_Z_ADDR               (0x26)
#define BMG160_HIGHRATE_DURN_Z_ADDR                (0x27)
#define BMG160_SOC_ADDR                            (0x31)
#define BMG160_A_FOC_ADDR                          (0x32)
#define BMG160_TRIM_NVM_CTRL_ADDR                  (0x33)
#define BMG160_BGW_SPI3_WDT_ADDR                   (0x34)

#define BMG160_OFFSET_OFC1_ADDR                    (0x36)
#define BMG160_OFC2_ADDR                           (0x37)
#define BMG160_OFC3_ADDR                           (0x38)
#define BMG160_OFC4_ADDR                           (0x39)
#define BMG160_TRIM_GP0_ADDR                       (0x3A)
#define BMG160_TRIM_GP1_ADDR                       (0x3B)
#define BMG160_SELFTEST_ADDR                       (0x3C)
#define BMG160_FIFO_CGF1_ADDR                      (0x3D)
#define BMG160_FIFO_CGF0_ADDR                      (0x3E)

#define BMG160_FIFO_DATA_ADDR                      (0x3F)

#define BMG160_RATE_X_LSB_BIT_POS        (0)
#define BMG160_RATE_X_LSB_BIT_LEN        (8)
#define BMG160_RATE_X_LSB_BIT_MSK        (0xFF)
#define BMG160_RATE_X_LSB_BIT_REG        (BMG160_RATE_X_LSB_ADDR)

#define BMG160_RATE_Y_LSB_BIT_POS        (0)
#define BMG160_RATE_Y_LSB_BIT_LEN        (8)
#define BMG160_RATE_Y_LSB_BIT_MSK        (0xFF)
#define BMG160_RATE_Y_LSB_BIT_REG        (BMG160_RATE_Y_LSB_ADDR)
#define BMG160_RATE_Z_LSB_BIT_POS        (0)
#define BMG160_RATE_Z_LSB_BIT_LEN        (8)
#define BMG160_RATE_Z_LSB_BIT_MSK        (0xFF)
#define BMG160_RATE_Z_LSB_BIT_REG        (BMG160_RATE_Z_LSB_ADDR)
#define BMG160_INTR_STAT0_ANY_MOTION_INTR_POS     (2)
#define BMG160_INTR_STAT0_ANY_MOTION_INTR_LEN     (1)
#define BMG160_INTR_STAT0_ANY_MOTION_INTR_MSK     (0x04)
#define BMG160_INTR_STAT0_ANY_MOTION_INTR_REG     (BMG160_INTR_STAT0_ADDR)

#define BMG160_INTR_STAT0_HIGHRATE_INTR_POS    (1)
#define BMG160_INTR_STAT0_HIGHRATE_INTR_LEN    (1)
#define BMG160_INTR_STAT0_HIGHRATE_INTR_MSK    (0x02)
#define BMG160_INTR_STAT0_HIGHRATE_INTR_REG    (BMG160_INTR_STAT0_ADDR)

#define BMG160_INTR_STAT_ZERO_POS    (1)
#define BMG160_INTR_STAT_ZERO_LEN    (2)
#define BMG160_INTR_STAT_ZERO_MSK    (0x06)
#define BMG160_INTR_STAT_ZERO_REG    (BMG160_INTR_STAT0_ADDR)

#define BMG160_INTR_STAT1_DATA_INTR_POS           (7)
#define BMG160_INTR_STAT1_DATA_INTR_LEN           (1)
#define BMG160_INTR_STAT1_DATA_INTR_MSK           (0x80)
#define BMG160_INTR_STAT1_DATA_INTR_REG           (BMG160_INTR_STAT1_ADDR)

#define BMG160_INTR_STAT1_AUTO_OFFSET_INTR_POS    (6)
#define BMG160_INTR_STAT1_AUTO_OFFSET_INTR_LEN    (1)
#define BMG160_INTR_STAT1_AUTO_OFFSET_INTR_MSK    (0x40)
#define BMG160_INTR_STAT1_AUTO_OFFSET_INTR_REG    (BMG160_INTR_STAT1_ADDR)

#define BMG160_INTR_STAT1_FAST_OFFSET_INTR_POS    (5)
#define BMG160_INTR_STAT1_FAST_OFFSET_INTR_LEN    (1)
#define BMG160_INTR_STAT1_FAST_OFFSET_INTR_MSK    (0x20)
#define BMG160_INTR_STAT1_FAST_OFFSET_INTR_REG    (BMG160_INTR_STAT1_ADDR)

#define BMG160_INTR_STAT1_FIFO_INTR_POS           (4)
#define BMG160_INTR_STAT1_FIFO_INTR_LEN           (1)
#define BMG160_INTR_STAT1_FIFO_INTR_MSK           (0x10)
#define BMG160_INTR_STAT1_FIFO_INTR_REG           (BMG160_INTR_STAT1_ADDR)

#define BMG160_INTR_STAT_ONE_POS           (4)
#define BMG160_INTR_STAT_ONE_LEN           (4)
#define BMG160_INTR_STAT_ONE_MSK           (0xF0)
#define BMG160_INTR_STAT_ONE_REG           (BMG160_INTR_STAT1_ADDR)

#define BMG160_INTR_STAT2_ANY_MOTION_SIGN_INTR_POS     (3)
#define BMG160_INTR_STAT2_ANY_MOTION_SIGN_INTR_LEN     (1)
#define BMG160_INTR_STAT2_ANY_MOTION_SIGN_INTR_MSK     (0x08)
#define BMG160_INTR_STAT2_ANY_MOTION_SIGN_INTR_REG     (BMG160_INTR_STAT2_ADDR)

#define BMG160_INTR_STAT2_ANY_MOTION_FIRSTZ_INTR_POS   (2)
#define BMG160_INTR_STAT2_ANY_MOTION_FIRSTZ_INTR_LEN   (1)
#define BMG160_INTR_STAT2_ANY_MOTION_FIRSTZ_INTR_MSK   (0x04)
#define BMG160_INTR_STAT2_ANY_MOTION_FIRSTZ_INTR_REG   (BMG160_INTR_STAT2_ADDR)

#define BMG160_INTR_STAT2_ANY_MOTION_FIRSTY_INTR_POS   (1)
#define BMG160_INTR_STAT2_ANY_MOTION_FIRSTY_INTR_LEN   (1)
#define BMG160_INTR_STAT2_ANY_MOTION_FIRSTY_INTR_MSK   (0x02)
#define BMG160_INTR_STAT2_ANY_MOTION_FIRSTY_INTR_REG   (BMG160_INTR_STAT2_ADDR)

#define BMG160_INTR_STAT2_ANY_MOTION_FIRSTX_INTR_POS   (0)
#define BMG160_INTR_STAT2_ANY_MOTION_FIRSTX_INTR_LEN   (1)
#define BMG160_INTR_STAT2_ANY_MOTION_FIRSTX_INTR_MSK   (0x01)
#define BMG160_INTR_STAT2_ANY_MOTION_FIRSTX_INTR_REG   (BMG160_INTR_STAT2_ADDR)

#define BMG160_INTR_STAT_TWO_POS   (0)
#define BMG160_INTR_STAT_TWO_LEN   (4)
#define BMG160_INTR_STAT_TWO_MSK   (0x0F)
#define BMG160_INTR_STAT_TWO_REG   (BMG160_INTR_STAT2_ADDR)

#define BMG160_INTR_STAT3_HIGHRATE_SIGN_INTR_POS     (3)
#define BMG160_INTR_STAT3_HIGHRATE_SIGN_INTR_LEN     (1)
#define BMG160_INTR_STAT3_HIGHRATE_SIGN_INTR_MSK     (0x08)
#define BMG160_INTR_STAT3_HIGHRATE_SIGN_INTR_REG     (BMG160_INTR_STAT3_ADDR)

#define BMG160_INTR_STAT3_HIGHRATE_FIRSTZ_INTR_POS   (2)
#define BMG160_INTR_STAT3_HIGHRATE_FIRSTZ_INTR_LEN   (1)
#define BMG160_INTR_STAT3_HIGHRATE_FIRSTZ_INTR_MSK   (0x04)
#define BMG160_INTR_STAT3_HIGHRATE_FIRSTZ_INTR_REG  (BMG160_INTR_STAT3_ADDR)

#define BMG160_INTR_STAT3_HIGHRATE_FIRSTY_INTR_POS   (1)
#define BMG160_INTR_STAT3_HIGHRATE_FIRSTY_INTR_LEN   (1)
#define BMG160_INTR_STAT3_HIGHRATE_FIRSTY_INTR_MSK   (0x02)
#define BMG160_INTR_STAT3_HIGHRATE_FIRSTY_INTR_REG   (BMG160_INTR_STAT3_ADDR)

#define BMG160_INTR_STAT3_HIGHRATE_FIRSTX_INTR_POS   (0)
#define BMG160_INTR_STAT3_HIGHRATE_FIRSTX_INTR_LEN   (1)
#define BMG160_INTR_STAT3_HIGHRATE_FIRSTX_INTR_MSK   (0x01)
#define BMG160_INTR_STAT3_HIGHRATE_FIRSTX_INTR_REG   (BMG160_INTR_STAT3_ADDR)

#define BMG160_INTR_STAT_THREE_POS   (0)
#define BMG160_INTR_STAT_THREE_LEN   (4)
#define BMG160_INTR_STAT_THREE_MSK   (0x0F)
#define BMG160_INTR_STAT_THREE_REG   (BMG160_INTR_STAT3_ADDR)

#define BMG160_FIFO_STAT_OVERRUN_POS         (7)
#define BMG160_FIFO_STAT_OVERRUN_LEN         (1)
#define BMG160_FIFO_STAT_OVERRUN_MSK         (0x80)
#define BMG160_FIFO_STAT_OVERRUN_REG         (BMG160_FIFO_STAT_ADDR)

#define BMG160_FIFO_STAT_FRAME_COUNTER_POS   (0)
#define BMG160_FIFO_STAT_FRAME_COUNTER_LEN   (7)
#define BMG160_FIFO_STAT_FRAME_COUNTER_MSK   (0x7F)
#define BMG160_FIFO_STAT_FRAME_COUNTER_REG   (BMG160_FIFO_STAT_ADDR)

#define BMG160_RANGE_RANGE_2000DPS            (0x00)
#define BMG160_RANGE_RANGE_1000DPS            (0x01)
#define BMG160_RANGE_RANGE_500DPS             (0x02)
#define BMG160_RANGE_RANGE_250DPS             (0x03)
#define BMG160_RANGE_RANGE_125DPS             (0x04)
#define BMG160_RANGE_ADDR_RANGE_POS           (0)
#define BMG160_RANGE_ADDR_RANGE_LEN           (3)
#define BMG160_RANGE_ADDR_RANGE_MSK           (0x07)
#define BMG160_RANGE_ADDR_RANGE_REG           (BMG160_RANGE_ADDR)

#define BMG160_BW_ADDR_POS                    (0)
#define BMG160_BW_ADDR_LEN                    (3)
#define BMG160_BW_ADDR_MSK                    (0x07)
#define BMG160_BW_ADDR_REG                    (BMG160_BW_ADDR)

#define BMG160_MODE_LPM1_POS             (5)
#define BMG160_MODE_LPM1_LEN             (3)
#define BMG160_MODE_LPM1_MSK             (0xA0)
#define BMG160_MODE_LPM1_REG             (BMG160_MODE_LPM1_ADDR)

#define BMG160_MODELPM1_ADDR_SLEEP_DURN_POS              (1)
#define BMG160_MODELPM1_ADDR_SLEEP_DURN_LEN              (3)
#define BMG160_MODELPM1_ADDR_SLEEP_DURN_MSK              (0x0E)
#define BMG160_MODELPM1_ADDR_SLEEP_DURN_REG              (BMG160_MODE_LPM1_ADDR)

#define BMG160_MODE_LPM2_ADDR_FAST_POWERUP_POS           (7)
#define BMG160_MODE_LPM2_ADDR_FAST_POWERUP_LEN           (1)
#define BMG160_MODE_LPM2_ADDR_FAST_POWERUP_MSK           (0x80)
#define BMG160_MODE_LPM2_ADDR_FAST_POWERUP_REG           (BMG160_MODE_LPM2_ADDR)

#define BMG160_MODE_LPM2_ADDR_ADV_POWERSAVING_POS        (6)
#define BMG160_MODE_LPM2_ADDR_ADV_POWERSAVING_LEN        (1)
#define BMG160_MODE_LPM2_ADDR_ADV_POWERSAVING_MSK        (0x40)
#define BMG160_MODE_LPM2_ADDR_ADV_POWERSAVING_REG        (BMG160_MODE_LPM2_ADDR)

#define BMG160_MODE_LPM2_ADDR_EXT_TRI_SELECT_POS         (4)
#define BMG160_MODE_LPM2_ADDR_EXT_TRI_SELECT_LEN         (2)
#define BMG160_MODE_LPM2_ADDR_EXT_TRI_SELECT_MSK         (0x30)
#define BMG160_MODE_LPM2_ADDR_EXT_TRI_SELECT_REG         (BMG160_MODE_LPM2_ADDR)

#define BMG160_MODE_LPM2_ADDR_AUTO_SLEEP_DURN_POS  (0)
#define BMG160_MODE_LPM2_ADDR_AUTO_SLEEP_DURN_LEN  (3)
#define BMG160_MODE_LPM2_ADDR_AUTO_SLEEP_DURN_MSK  (0x07)
#define BMG160_MODE_LPM2_ADDR_AUTO_SLEEP_DURN_REG  (BMG160_MODE_LPM2_ADDR)

#define BMG160_HIGH_BW_POS         (7)
#define BMG160_HIGH_BW_LEN         (1)
#define BMG160_HIGH_BW_MSK         (0x80)
#define BMG160_HIGH_BW_REG         (BMG160_HIGH_BW_ADDR)

#define BMG160_SHADOW_DIS_POS          (6)
#define BMG160_SHADOW_DIS_LEN          (1)
#define BMG160_SHADOW_DIS_MSK          (0x40)
#define BMG160_SHADOW_DIS_REG          (BMG160_HIGH_BW_ADDR)

#define BMG160_INTR_ENABLE0_DATA_POS               (7)
#define BMG160_INTR_ENABLE0_DATA_LEN               (1)
#define BMG160_INTR_ENABLE0_DATA_MSK               (0x80)
#define BMG160_INTR_ENABLE0_DATA_REG               (BMG160_INTR_ENABLE0_ADDR)

#define BMG160_INTR_ENABLE0_FIFO_POS               (6)
#define BMG160_INTR_ENABLE0_FIFO_LEN               (1)
#define BMG160_INTR_ENABLE0_FIFO_MSK               (0x40)
#define BMG160_INTR_ENABLE0_FIFO_REG               (BMG160_INTR_ENABLE0_ADDR)

#define BMG160_INTR_ENABLE0_AUTO_OFFSET_POS        (2)
#define BMG160_INTR_ENABLE0_AUTO_OFFSET_LEN        (1)
#define BMG160_INTR_ENABLE0_AUTO_OFFSET_MSK        (0x04)
#define BMG160_INTR_ENABLE0_AUTO_OFFSET_REG        (BMG160_INTR_ENABLE0_ADDR)

#define BMG160_INTR_ENABLE1_IT2_OUTPUT_TYPE_POS               (3)
#define BMG160_INTR_ENABLE1_IT2_OUTPUT_TYPE_LEN               (1)
#define BMG160_INTR_ENABLE1_IT2_OUTPUT_TYPE_MSK               (0x08)
#define BMG160_INTR_ENABLE1_IT2_OUTPUT_TYPE_REG               (BMG160_INTR_ENABLE1_ADDR)

#define BMG160_INTR_ENABLE1_IT2_LEVEL_POS              (2)
#define BMG160_INTR_ENABLE1_IT2_LEVEL_LEN              (1)
#define BMG160_INTR_ENABLE1_IT2_LEVEL_MSK              (0x04)
#define BMG160_INTR_ENABLE1_IT2_LEVEL_REG              (BMG160_INTR_ENABLE1_ADDR)

#define BMG160_INTR_ENABLE1_IT1_OUTPUT_TYPE_POS               (1)
#define BMG160_INTR_ENABLE1_IT1_OUTPUT_TYPE_LEN               (1)
#define BMG160_INTR_ENABLE1_IT1_OUTPUT_TYPE_MSK               (0x02)
#define BMG160_INTR_ENABLE1_IT1_OUTPUT_TYPE_REG               (BMG160_INTR_ENABLE1_ADDR)

#define BMG160_INTR_ENABLE1_IT1_LEVEL_POS              (0)
#define BMG160_INTR_ENABLE1_IT1_LEVEL_LEN              (1)
#define BMG160_INTR_ENABLE1_IT1_LEVEL_MSK              (0x01)
#define BMG160_INTR_ENABLE1_IT1_LEVEL_REG              (BMG160_INTR_ENABLE1_ADDR)

#define BMG160_INTR_MAP_ZERO_INTR1_HIGHRATE_POS            (3)
#define BMG160_INTR_MAP_ZERO_INTR1_HIGHRATE_LEN            (1)
#define BMG160_INTR_MAP_ZERO_INTR1_HIGHRATE_MSK            (0x08)
#define BMG160_INTR_MAP_ZERO_INTR1_HIGHRATE_REG            (BMG160_INTR_MAP_ZERO_ADDR)

#define BMG160_INTR_MAP_ZERO_INTR1_ANY_MOTION_POS             (1)
#define BMG160_INTR_MAP_ZERO_INTR1_ANY_MOTION_LEN             (1)
#define BMG160_INTR_MAP_ZERO_INTR1_ANY_MOTION_MSK             (0x02)
#define BMG160_INTR_MAP_ZERO_INTR1_ANY_MOTION_REG             (BMG160_INTR_MAP_ZERO_ADDR)

#define BMG160_MAP_ONE_INTR2_DATA_POS                  (7)
#define BMG160_MAP_ONE_INTR2_DATA_LEN                  (1)
#define BMG160_MAP_ONE_INTR2_DATA_MSK                  (0x80)
#define BMG160_MAP_ONE_INTR2_DATA_REG                  (BMG160_INTR_MAP_ONE_ADDR)

#define BMG160_MAP_ONE_INTR2_FAST_OFFSET_POS           (6)
#define BMG160_MAP_ONE_INTR2_FAST_OFFSET_LEN           (1)
#define BMG160_MAP_ONE_INTR2_FAST_OFFSET_MSK           (0x40)
#define BMG160_MAP_ONE_INTR2_FAST_OFFSET_REG           (BMG160_INTR_MAP_ONE_ADDR)

#define BMG160_MAP_ONE_INTR2_FIFO_POS                  (5)
#define BMG160_MAP_ONE_INTR2_FIFO_LEN                  (1)
#define BMG160_MAP_ONE_INTR2_FIFO_MSK                  (0x20)
#define BMG160_MAP_ONE_INTR2_FIFO_REG                  (BMG160_INTR_MAP_ONE_ADDR)

#define BMG160_MAP_ONE_INTR2_AUTO_OFFSET_POS           (4)
#define BMG160_MAP_ONE_INTR2_AUTO_OFFSET_LEN           (1)
#define BMG160_MAP_ONE_INTR2_AUTO_OFFSET_MSK           (0x10)
#define BMG160_MAP_ONE_INTR2_AUTO_OFFSET_REG           (BMG160_INTR_MAP_ONE_ADDR)

#define BMG160_MAP_ONE_INTR1_AUTO_OFFSET_POS           (3)
#define BMG160_MAP_ONE_INTR1_AUTO_OFFSET_LEN           (1)
#define BMG160_MAP_ONE_INTR1_AUTO_OFFSET_MSK           (0x08)
#define BMG160_MAP_ONE_INTR1_AUTO_OFFSET_REG           (BMG160_INTR_MAP_ONE_ADDR)

#define BMG160_MAP_ONE_INTR1_FIFO_POS                  (2)
#define BMG160_MAP_ONE_INTR1_FIFO_LEN                  (1)
#define BMG160_MAP_ONE_INTR1_FIFO_MSK                  (0x04)
#define BMG160_MAP_ONE_INTR1_FIFO_REG                  (BMG160_INTR_MAP_ONE_ADDR)

#define BMG160_MAP_ONE_INTR1_FAST_OFFSET_POS           (1)
#define BMG160_MAP_ONE_INTR1_FAST_OFFSET_LEN           (1)
#define BMG160_MAP_ONE_INTR1_FAST_OFFSET_MSK           (0x02)
#define BMG160_MAP_ONE_INTR1_FAST_OFFSET_REG           (BMG160_INTR_MAP_ONE_ADDR)

#define BMG160_MAP_ONE_INTR1_DATA_POS                  (0)
#define BMG160_MAP_ONE_INTR1_DATA_LEN                  (1)
#define BMG160_MAP_ONE_INTR1_DATA_MSK                  (0x01)
#define BMG160_MAP_ONE_INTR1_DATA_REG                  (BMG160_INTR_MAP_ONE_ADDR)

#define BMG160_INTR_MAP_TWO_INTR2_HIGHRATE_POS            (3)
#define BMG160_INTR_MAP_TWO_INTR2_HIGHRATE_LEN            (1)
#define BMG160_INTR_MAP_TWO_INTR2_HIGHRATE_MSK            (0x08)
#define BMG160_INTR_MAP_TWO_INTR2_HIGHRATE_REG            (BMG160_INTR_MAP_TWO_ADDR)

#define BMG160_INTR_MAP_TWO_INTR2_ANY_MOTION_POS             (1)
#define BMG160_INTR_MAP_TWO_INTR2_ANY_MOTION_LEN             (1)
#define BMG160_INTR_MAP_TWO_INTR2_ANY_MOTION_MSK             (0x02)
#define BMG160_INTR_MAP_TWO_INTR2_ANY_MOTION_REG             (BMG160_INTR_MAP_TWO_ADDR)

#define BMG160_INTR_ZERO_ADDR_SLOW_OFFSET_UNFILT_POS          (5)
#define BMG160_INTR_ZERO_ADDR_SLOW_OFFSET_UNFILT_LEN          (1)
#define BMG160_INTR_ZERO_ADDR_SLOW_OFFSET_UNFILT_MSK          (0x20)
#define BMG160_INTR_ZERO_ADDR_SLOW_OFFSET_UNFILT_REG          (BMG160_INTR_ZERO_ADDR)

#define BMG160_INTR_ZERO_ADDR_HIGHRATE_UNFILT_DATA_POS            (3)
#define BMG160_INTR_ZERO_ADDR_HIGHRATE_UNFILT_DATA_LEN            (1)
#define BMG160_INTR_ZERO_ADDR_HIGHRATE_UNFILT_DATA_MSK            (0x08)
#define BMG160_INTR_ZERO_ADDR_HIGHRATE_UNFILT_DATA_REG            (BMG160_INTR_ZERO_ADDR)

#define BMG160_INTR_ZERO_ADDR_ANY_MOTION_UNFILT_DATA_POS             (1)
#define BMG160_INTR_ZERO_ADDR_ANY_MOTION_UNFILT_DATA_LEN             (1)
#define BMG160_INTR_ZERO_ADDR_ANY_MOTION_UNFILT_DATA_MSK             (0x02)
#define BMG160_INTR_ZERO_ADDR_ANY_MOTION_UNFILT_DATA_REG             (BMG160_INTR_ZERO_ADDR)

#define BMG160_INTR_ONE_ADDR_FAST_OFFSET_UNFILT_POS            (7)
#define BMG160_INTR_ONE_ADDR_FAST_OFFSET_UNFILT_LEN            (1)
#define BMG160_INTR_ONE_ADDR_FAST_OFFSET_UNFILT_MSK            (0x80)
#define BMG160_INTR_ONE_ADDR_FAST_OFFSET_UNFILT_REG            (BMG160_INTR_ONE_ADDR)

#define BMG160_INTR_ONE_ADDR_ANY_MOTION_THRES_POS                       (0)
#define BMG160_INTR_ONE_ADDR_ANY_MOTION_THRES_LEN                       (7)
#define BMG160_INTR_ONE_ADDR_ANY_MOTION_THRES_MSK                       (0x7F)
#define BMG160_INTR_ONE_ADDR_ANY_MOTION_THRES_REG                       (BMG160_INTR_ONE_ADDR)

#define BMG160_INTR_TWO_ADDR_AWAKE_DURN_POS          (6)
#define BMG160_INTR_TWO_ADDR_AWAKE_DURN_LEN          (2)
#define BMG160_INTR_TWO_ADDR_AWAKE_DURN_MSK          (0xC0)
#define BMG160_INTR_TWO_ADDR_AWAKE_DURN_REG          (BMG160_INTR_TWO_ADDR)

#define BMG160_INTR_TWO_ADDR_ANY_MOTION_DURN_SAMPLE_POS      (4)
#define BMG160_INTR_TWO_ADDR_ANY_MOTION_DURN_SAMPLE_LEN      (2)
#define BMG160_INTR_TWO_ADDR_ANY_MOTION_DURN_SAMPLE_MSK      (0x30)
#define BMG160_INTR_TWO_ADDR_ANY_MOTION_DURN_SAMPLE_REG      (BMG160_INTR_TWO_ADDR)

#define BMG160_INTR_TWO_ADDR_ANY_MOTION_ENABLE_Z_POS           (2)
#define BMG160_INTR_TWO_ADDR_ANY_MOTION_ENABLE_Z_LEN           (1)
#define BMG160_INTR_TWO_ADDR_ANY_MOTION_ENABLE_Z_MSK           (0x04)
#define BMG160_INTR_TWO_ADDR_ANY_MOTION_ENABLE_Z_REG           (BMG160_INTR_TWO_ADDR)

#define BMG160_INTR_TWO_ADDR_ANY_MOTION_ENABLE_Y_POS           (1)
#define BMG160_INTR_TWO_ADDR_ANY_MOTION_ENABLE_Y_LEN           (1)
#define BMG160_INTR_TWO_ADDR_ANY_MOTION_ENABLE_Y_MSK           (0x02)
#define BMG160_INTR_TWO_ADDR_ANY_MOTION_ENABLE_Y_REG           (BMG160_INTR_TWO_ADDR)

#define BMG160_INTR_TWO_ADDR_ANY_MOTION_ENABLE_X_POS           (0)
#define BMG160_INTR_TWO_ADDR_ANY_MOTION_ENABLE_X_LEN           (1)
#define BMG160_INTR_TWO_ADDR_ANY_MOTION_ENABLE_X_MSK           (0x01)
#define BMG160_INTR_TWO_ADDR_ANY_MOTION_ENABLE_X_REG           (BMG160_INTR_TWO_ADDR)

#define BMG160_INTR_4_FIFO_WM_ENABLE_POS           (7)
#define BMG160_INTR_4_FIFO_WM_ENABLE_LEN           (1)
#define BMG160_INTR_4_FIFO_WM_ENABLE_MSK           (0x80)
#define BMG160_INTR_4_FIFO_WM_ENABLE_REG           (BMG160_INTR_4_ADDR)

#define BMG160_RST_LATCH_ADDR_RST_INTR_POS           (7)
#define BMG160_RST_LATCH_ADDR_RST_INTR_LEN           (1)
#define BMG160_RST_LATCH_ADDR_RST_INTR_MSK           (0x80)
#define BMG160_RST_LATCH_ADDR_RST_INTR_REG           (BMG160_RST_LATCH_ADDR)

#define BMG160_RST_LATCH_ADDR_OFFSET_RST_POS        (6)
#define BMG160_RST_LATCH_ADDR_OFFSET_RST_LEN        (1)
#define BMG160_RST_LATCH_ADDR_OFFSET_RST_MSK        (0x40)
#define BMG160_RST_LATCH_ADDR_OFFSET_RST_REG        (BMG160_RST_LATCH_ADDR)

#define BMG160_RST_LATCH_ADDR_LATCH_STAT_POS        (4)
#define BMG160_RST_LATCH_ADDR_LATCH_STAT_LEN        (1)
#define BMG160_RST_LATCH_ADDR_LATCH_STAT_MSK        (0x10)
#define BMG160_RST_LATCH_ADDR_LATCH_STAT_REG        (BMG160_RST_LATCH_ADDR)

#define BMG160_RST_LATCH_ADDR_LATCH_INTR_POS           (0)
#define BMG160_RST_LATCH_ADDR_LATCH_INTR_LEN           (4)
#define BMG160_RST_LATCH_ADDR_LATCH_INTR_MSK           (0x0F)
#define BMG160_RST_LATCH_ADDR_LATCH_INTR_REG           (BMG160_RST_LATCH_ADDR)

#define BMG160_HIGHRATE_HYST_X_POS        (6)
#define BMG160_HIGHRATE_HYST_X_LEN        (2)
#define BMG160_HIGHRATE_HYST_X_MSK        (0xC0)
#define BMG160_HIGHRATE_HYST_X_REG        (BMG160_HIGHRATE_THRES_X_ADDR)

#define BMG160_HIGHRATE_THRES_X_POS        (1)
#define BMG160_HIGHRATE_THRES_X_LEN        (5)
#define BMG160_HIGHRATE_THRES_X_MSK        (0x3E)
#define BMG160_HIGHRATE_THRES_X_REG        (BMG160_HIGHRATE_THRES_X_ADDR)

#define BMG160_HIGHRATE_ENABLE_X_POS        (0)
#define BMG160_HIGHRATE_ENABLE_X_LEN        (1)
#define BMG160_HIGHRATE_ENABLE_X_MSK        (0x01)
#define BMG160_HIGHRATE_ENABLE_X_REG        (BMG160_HIGHRATE_THRES_X_ADDR)

#define BMG160_HIGHRATE_HYST_Y_POS        (6)
#define BMG160_HIGHRATE_HYST_Y_LEN        (2)
#define BMG160_HIGHRATE_HYST_Y_MSK        (0xC0)
#define BMG160_HIGHRATE_HYST_Y_REG        (BMG160_HIGHRATE_THRES_Y_ADDR)

#define BMG160_HIGHRATE_THRES_Y_POS        (1)
#define BMG160_HIGHRATE_THRES_Y_LEN        (5)
#define BMG160_HIGHRATE_THRES_Y_MSK        (0x3E)
#define BMG160_HIGHRATE_THRES_Y_REG        (BMG160_HIGHRATE_THRES_Y_ADDR)

#define BMG160_HIGHRATE_ENABLE_Y_POS        (0)
#define BMG160_HIGHRATE_ENABLE_Y_LEN        (1)
#define BMG160_HIGHRATE_ENABLE_Y_MSK        (0x01)
#define BMG160_HIGHRATE_ENABLE_Y_REG        (BMG160_HIGHRATE_THRES_Y_ADDR)

#define BMG160_HIGHRATE_HYST_Z_POS        (6)
#define BMG160_HIGHRATE_HYST_Z_LEN        (2)
#define BMG160_HIGHRATE_HYST_Z_MSK        (0xC0)
#define BMG160_HIGHRATE_HYST_Z_REG        (BMG160_HIGHRATE_THRES_Z_ADDR)

#define BMG160_HIGHRATE_THRES_Z_POS        (1)
#define BMG160_HIGHRATE_THRES_Z_LEN        (5)
#define BMG160_HIGHRATE_THRES_Z_MSK        (0x3E)
#define BMG160_HIGHRATE_THRES_Z_REG        (BMG160_HIGHRATE_THRES_Z_ADDR)

#define BMG160_HIGHRATE_ENABLE_Z_POS        (0)
#define BMG160_HIGHRATE_ENABLE_Z_LEN        (1)
#define BMG160_HIGHRATE_ENABLE_Z_MSK        (0x01)
#define BMG160_HIGHRATE_ENABLE_Z_REG        (BMG160_HIGHRATE_THRES_Z_ADDR)

#define BMG160_SLOW_OFFSET_THRES_POS          (6)
#define BMG160_SLOW_OFFSET_THRES_LEN          (2)
#define BMG160_SLOW_OFFSET_THRES_MSK          (0xC0)
#define BMG160_SLOW_OFFSET_THRES_REG          (BMG160_SOC_ADDR)

#define BMG160_SLOW_OFFSET_DURN_POS         (3)
#define BMG160_SLOW_OFFSET_DURN_LEN         (3)
#define BMG160_SLOW_OFFSET_DURN_MSK         (0x38)
#define BMG160_SLOW_OFFSET_DURN_REG         (BMG160_SOC_ADDR)

#define BMG160_SLOW_OFFSET_ENABLE_Z_POS        (2)
#define BMG160_SLOW_OFFSET_ENABLE_Z_LEN        (1)
#define BMG160_SLOW_OFFSET_ENABLE_Z_MSK        (0x04)
#define BMG160_SLOW_OFFSET_ENABLE_Z_REG        (BMG160_SOC_ADDR)

#define BMG160_SLOW_OFFSET_ENABLE_Y_POS        (1)
#define BMG160_SLOW_OFFSET_ENABLE_Y_LEN        (1)
#define BMG160_SLOW_OFFSET_ENABLE_Y_MSK        (0x02)
#define BMG160_SLOW_OFFSET_ENABLE_Y_REG        (BMG160_SOC_ADDR)

#define BMG160_SLOW_OFFSET_ENABLE_X_POS        (0)
#define BMG160_SLOW_OFFSET_ENABLE_X_LEN        (1)
#define BMG160_SLOW_OFFSET_ENABLE_X_MSK        (0x01)
#define BMG160_SLOW_OFFSET_ENABLE_X_REG        (BMG160_SOC_ADDR)

#define BMG160_AUTO_OFFSET_WORD_LENGHTH_POS        (6)
#define BMG160_AUTO_OFFSET_WORD_LENGHTH_LEN        (2)
#define BMG160_AUTO_OFFSET_WORD_LENGHTH_MSK        (0xC0)
#define BMG160_AUTO_OFFSET_WORD_LENGHTH_REG        (BMG160_A_FOC_ADDR)

#define BMG160_FAST_OFFSET_WORD_LENGHTH_POS        (4)
#define BMG160_FAST_OFFSET_WORD_LENGHTH_LEN        (2)
#define BMG160_FAST_OFFSET_WORD_LENGHTH_MSK        (0x30)
#define BMG160_FAST_OFFSET_WORD_LENGHTH_REG        (BMG160_A_FOC_ADDR)

#define BMG160_FAST_OFFSET_ENABLE_POS        (3)
#define BMG160_FAST_OFFSET_ENABLE_LEN        (1)
#define BMG160_FAST_OFFSET_ENABLE_MSK        (0x08)
#define BMG160_FAST_OFFSET_ENABLE_REG        (BMG160_A_FOC_ADDR)

#define BMG160_FAST_OFFSET_ENABLE_Z_POS      (2)
#define BMG160_FAST_OFFSET_ENABLE_Z_LEN      (1)
#define BMG160_FAST_OFFSET_ENABLE_Z_MSK      (0x04)
#define BMG160_FAST_OFFSET_ENABLE_Z_REG      (BMG160_A_FOC_ADDR)

#define BMG160_FAST_OFFSET_ENABLE_Y_POS      (1)
#define BMG160_FAST_OFFSET_ENABLE_Y_LEN      (1)
#define BMG160_FAST_OFFSET_ENABLE_Y_MSK      (0x02)
#define BMG160_FAST_OFFSET_ENABLE_Y_REG      (BMG160_A_FOC_ADDR)

#define BMG160_FAST_OFFSET_ENABLE_X_POS      (0)
#define BMG160_FAST_OFFSET_ENABLE_X_LEN      (1)
#define BMG160_FAST_OFFSET_ENABLE_X_MSK      (0x01)
#define BMG160_FAST_OFFSET_ENABLE_X_REG      (BMG160_A_FOC_ADDR)

#define BMG160_FAST_OFFSET_ENABLE_XYZ_POS      (0)
#define BMG160_FAST_OFFSET_ENABLE_XYZ_LEN      (3)
#define BMG160_FAST_OFFSET_ENABLE_XYZ_MSK      (0x07)
#define BMG160_FAST_OFFSET_ENABLE_XYZ_REG      (BMG160_A_FOC_ADDR)

#define BMG160_TRIM_NVM_CTRL_ADDR_NVM_REMAIN_POS        (4)
#define BMG160_TRIM_NVM_CTRL_ADDR_NVM_REMAIN_LEN        (4)
#define BMG160_TRIM_NVM_CTRL_ADDR_NVM_REMAIN_MSK        (0xF0)
#define BMG160_TRIM_NVM_CTRL_ADDR_NVM_REMAIN_REG        (BMG160_TRIM_NVM_CTRL_ADDR)

#define BMG160_TRIM_NVM_CTRL_ADDR_NVM_LOAD_POS          (3)
#define BMG160_TRIM_NVM_CTRL_ADDR_NVM_LOAD_LEN          (1)
#define BMG160_TRIM_NVM_CTRL_ADDR_NVM_LOAD_MSK          (0x08)
#define BMG160_TRIM_NVM_CTRL_ADDR_NVM_LOAD_REG          (BMG160_TRIM_NVM_CTRL_ADDR)

#define BMG160_TRIM_NVM_CTRL_ADDR_NVM_RDY_POS           (2)
#define BMG160_TRIM_NVM_CTRL_ADDR_NVM_RDY_LEN           (1)
#define BMG160_TRIM_NVM_CTRL_ADDR_NVM_RDY_MSK           (0x04)
#define BMG160_TRIM_NVM_CTRL_ADDR_NVM_RDY_REG           (BMG160_TRIM_NVM_CTRL_ADDR)

#define BMG160_TRIM_NVM_CTRL_ADDR_NVM_PROG_TRIG_POS     (1)
#define BMG160_TRIM_NVM_CTRL_ADDR_NVM_PROG_TRIG_LEN     (1)
#define BMG160_TRIM_NVM_CTRL_ADDR_NVM_PROG_TRIG_MSK     (0x02)
#define BMG160_TRIM_NVM_CTRL_ADDR_NVM_PROG_TRIG_REG     (BMG160_TRIM_NVM_CTRL_ADDR)

#define BMG160_TRIM_NVM_CTRL_ADDR_NVM_PROG_MODE_POS     (0)
#define BMG160_TRIM_NVM_CTRL_ADDR_NVM_PROG_MODE_LEN     (1)
#define BMG160_TRIM_NVM_CTRL_ADDR_NVM_PROG_MODE_MSK     (0x01)
#define BMG160_TRIM_NVM_CTRL_ADDR_NVM_PROG_MODE_REG     (BMG160_TRIM_NVM_CTRL_ADDR)

#define BMG160_BGW_SPI3_WDT_ADDR_I2C_WDT_ENABLE_POS      (2)
#define BMG160_BGW_SPI3_WDT_ADDR_I2C_WDT_ENABLE_LEN      (1)
#define BMG160_BGW_SPI3_WDT_ADDR_I2C_WDT_ENABLE_MSK      (0x04)
#define BMG160_BGW_SPI3_WDT_ADDR_I2C_WDT_ENABLE_REG      (BMG160_BGW_SPI3_WDT_ADDR)

#define BMG160_BGW_SPI3_WDT_ADDR_I2C_WDT_SELECT_POS     (1)
#define BMG160_BGW_SPI3_WDT_ADDR_I2C_WDT_SELECT_LEN     (1)
#define BMG160_BGW_SPI3_WDT_ADDR_I2C_WDT_SELECT_MSK     (0x02)
#define BMG160_BGW_SPI3_WDT_ADDR_I2C_WDT_SELECT_REG     (BMG160_BGW_SPI3_WDT_ADDR)

#define BMG160_BGW_SPI3_WDT_ADDR_SPI3_POS            (0)
#define BMG160_BGW_SPI3_WDT_ADDR_SPI3_LEN            (1)
#define BMG160_BGW_SPI3_WDT_ADDR_SPI3_MSK            (0x01)
#define BMG160_BGW_SPI3_WDT_ADDR_SPI3_REG            (BMG160_BGW_SPI3_WDT_ADDR)

#define BMG160_SELFTEST_ADDR_RATEOK_POS            (4)
#define BMG160_SELFTEST_ADDR_RATEOK_LEN            (1)
#define BMG160_SELFTEST_ADDR_RATEOK_MSK            (0x10)
#define BMG160_SELFTEST_ADDR_RATEOK_REG            (BMG160_SELFTEST_ADDR)

#define BMG160_SELFTEST_ADDR_BISTFAIL_POS          (2)
#define BMG160_SELFTEST_ADDR_BISTFAIL_LEN          (1)
#define BMG160_SELFTEST_ADDR_BISTFAIL_MSK          (0x04)
#define BMG160_SELFTEST_ADDR_BISTFAIL_REG          (BMG160_SELFTEST_ADDR)

#define BMG160_SELFTEST_ADDR_BISTRDY_POS           (1)
#define BMG160_SELFTEST_ADDR_BISTRDY_LEN           (1)
#define BMG160_SELFTEST_ADDR_BISTRDY_MSK           (0x02)
#define BMG160_SELFTEST_ADDR_BISTRDY_REG           (BMG160_SELFTEST_ADDR)

#define BMG160_SELFTEST_ADDR_TRIGBIST_POS          (0)
#define BMG160_SELFTEST_ADDR_TRIGBIST_LEN          (1)
#define BMG160_SELFTEST_ADDR_TRIGBIST_MSK          (0x01)
#define BMG160_SELFTEST_ADDR_TRIGBIST_REG          (BMG160_SELFTEST_ADDR)

#define BMG160_FIFO_CGF1_ADDR_TAG_POS     (7)
#define BMG160_FIFO_CGF1_ADDR_TAG_LEN     (1)
#define BMG160_FIFO_CGF1_ADDR_TAG_MSK     (0x80)
#define BMG160_FIFO_CGF1_ADDR_TAG_REG     (BMG160_FIFO_CGF1_ADDR)

#define BMG160_FIFO_CGF1_ADDR_WML_POS     (0)
#define BMG160_FIFO_CGF1_ADDR_WML_LEN     (7)
#define BMG160_FIFO_CGF1_ADDR_WML_MSK     (0x7F)
#define BMG160_FIFO_CGF1_ADDR_WML_REG     (BMG160_FIFO_CGF1_ADDR)

#define BMG160_FIFO_CGF0_ADDR_MODE_POS         (6)
#define BMG160_FIFO_CGF0_ADDR_MODE_LEN         (2)
#define BMG160_FIFO_CGF0_ADDR_MODE_MSK         (0xC0)
#define BMG160_FIFO_CGF0_ADDR_MODE_REG         (BMG160_FIFO_CGF0_ADDR)

#define BMG160_FIFO_CGF0_ADDR_DATA_SELECT_POS     (0)
#define BMG160_FIFO_CGF0_ADDR_DATA_SELECT_LEN     (2)
#define BMG160_FIFO_CGF0_ADDR_DATA_SELECT_MSK     (0x03)
#define BMG160_FIFO_CGF0_ADDR_DATA_SELECT_REG     (BMG160_FIFO_CGF0_ADDR)

#define BMG160_OFC1_ADDR_OFFSET_X_POS       (6)
#define BMG160_OFC1_ADDR_OFFSET_X_LEN       (2)
#define BMG160_OFC1_ADDR_OFFSET_X_MSK       (0xC0)
#define BMG160_OFC1_ADDR_OFFSET_X_REG       (BMG160_OFFSET_OFC1_ADDR)

#define BMG160_OFC1_ADDR_OFFSET_Y_POS       (3)
#define BMG160_OFC1_ADDR_OFFSET_Y_LEN       (3)
#define BMG160_OFC1_ADDR_OFFSET_Y_MSK       (0x38)
#define BMG160_OFC1_ADDR_OFFSET_Y_REG       (BMG160_OFFSET_OFC1_ADDR)

#define BMG160_OFC1_ADDR_OFFSET_Z_POS       (0)
#define BMG160_OFC1_ADDR_OFFSET_Z_LEN       (3)
#define BMG160_OFC1_ADDR_OFFSET_Z_MSK       (0x07)
#define BMG160_OFC1_ADDR_OFFSET_Z_REG       (BMG160_OFFSET_OFC1_ADDR)

#define BMG160_TRIM_GP0_ADDR_GP0_POS            (4)
#define BMG160_TRIM_GP0_ADDR_GP0_LEN            (4)
#define BMG160_TRIM_GP0_ADDR_GP0_MSK            (0xF0)
#define BMG160_TRIM_GP0_ADDR_GP0_REG            (BMG160_TRIM_GP0_ADDR)

#define BMG160_TRIM_GP0_ADDR_OFFSET_X_POS       (2)
#define BMG160_TRIM_GP0_ADDR_OFFSET_X_LEN       (2)
#define BMG160_TRIM_GP0_ADDR_OFFSET_X_MSK       (0x0C)
#define BMG160_TRIM_GP0_ADDR_OFFSET_X_REG       (BMG160_TRIM_GP0_ADDR)

#define BMG160_TRIM_GP0_ADDR_OFFSET_Y_POS       (1)
#define BMG160_TRIM_GP0_ADDR_OFFSET_Y_LEN       (1)
#define BMG160_TRIM_GP0_ADDR_OFFSET_Y_MSK       (0x02)
#define BMG160_TRIM_GP0_ADDR_OFFSET_Y_REG       (BMG160_TRIM_GP0_ADDR)

#define BMG160_TRIM_GP0_ADDR_OFFSET_Z_POS       (0)
#define BMG160_TRIM_GP0_ADDR_OFFSET_Z_LEN       (1)
#define BMG160_TRIM_GP0_ADDR_OFFSET_Z_MSK       (0x01)
#define BMG160_TRIM_GP0_ADDR_OFFSET_Z_REG       (BMG160_TRIM_GP0_ADDR)

#define BMG160_MODE_NORMAL              (0)
#define BMG160_MODE_DEEPSUSPEND         (1)
#define BMG160_MODE_SUSPEND             (2)
#define BMG160_MODE_FASTPOWERUP			(3)
#define BMG160_MODE_ADVANCEDPOWERSAVING (4)

#define BMG160_GET_BITSLICE(regvar, bitname) ((regvar & bitname##_MSK) >> bitname##_POS)
#define BMG160_SET_BITSLICE(regvar, bitname, val) ((regvar&~bitname##_MSK)|((val<<bitname##_POS)&bitname##_MSK))

#define	BMG160_X_DATA_SIZE			(2)
#define	BMG160_Y_DATA_SIZE			(2)
#define	BMG160_Z_DATA_SIZE			(2)
#define	BMG160_XYZ_DATA_SIZE		(6)
#define	BMG160_XYZ_INTR_DATA_SIZE	(12)

#define	BMG160_X_LSB_DATA	(0)
#define	BMG160_X_MSB_DATA	(1)
#define	BMG160_Y_LSB_DATA	(0)
#define	BMG160_Y_MSB_DATA	(1)
#define	BMG160_Z_LSB_DATA	(0)
#define	BMG160_Z_MSB_DATA	(1)
#define	BMG160_INTR0_STAT	(0)
#define	BMG160_INTR1_STAT	(1)
#define	BMG160_INTR2_STAT	(2)
#define	BMG160_INTR3_STAT	(3)
#define	BMG160_INTR4_STAT	(4)

#define	BMG160_DATA_FRAME_X_LSB_BYTE		(0)
#define	BMG160_DATA_FRAME_X_MSB_BYTE		(1)
#define	BMG160_DATA_FRAME_Y_LSB_BYTE		(2)
#define	BMG160_DATA_FRAME_Y_MSB_BYTE		(3)
#define	BMG160_DATA_FRAME_Z_LSB_BYTE		(4)
#define	BMG160_DATA_FRAME_Z_MSB_BYTE		(5)
#define	BMG160_DATA_FRAME_INTR0_BYTE	(7)
#define	BMG160_DATA_FRAME_INTR1_BYTE	(8)
#define	BMG160_DATA_FRAME_INTR2_BYTE	(9)
#define	BMG160_DATA_FRAME_INTR3_BYTE	(10)
#define	BMG160_DATA_FRAME_INTR4_BYTE	(11)

#define BMG160_ODR_100   (100)
#define BMG160_ODR_200   (200)
#define BMG160_ODR_400   (400)
#define BMG160_ODR_1000  (1000)
#define BMG160_ODR_2000  (2000)

#define BMG160_ODR_100_REG   (0x07)
#define BMG160_ODR_200_REG   (0x06)
#define BMG160_ODR_400_REG   (0x03)
#define BMG160_ODR_1000_REG  (0x02)
#define BMG160_ODR_2000_REG  (0x01)

#define BMG160_POWER_BIT_MASK           (0xA0)
#define BMG160_POWER_NORMAL_REG         (0x00)
#define BMG160_POWER_SLEEP_REG          (0x80)
#define BMG160_POWER_DEEP_SUSPEND_REG   (0x20)

#define BMG160_SHIFT_EIGHT_BITS         (8)

//bmg160 sensitivity factor table, the unit is LSB/°/s
static uint32_t bmg160_factor[5] = { 2624, 1312, 656, 328, 164 }; //(LSB/°/s) * 10
static uint32_t current_factor = 0;

i2c_dev_t bmg160_ctx = {
    .port = 1,
    .config.address_width = 8,
    .config.freq = 400000,
    .config.dev_addr = BMG160_I2C_ADDR1,
};

static int drv_gyro_bosch_bmg160_validate_id(i2c_dev_t* drv, uint8_t id_value)
{
    uint8_t value = 0x00;
    int ret = 0;

    if(drv == NULL){
        return -1;
    }
    
    ret = sensor_i2c_read(drv,  BMG160_CHIP_ID_ADDR, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    
    if (id_value != value){
        return -1;
    }
    
    return 0;
}

static int drv_gyro_bosch_bmg160_set_power_mode(i2c_dev_t* drv, dev_power_mode_e mode)
{
    uint8_t value,value1 = 0x00;
    int ret = 0;
    uint8_t buf[4];
    
    ret = sensor_i2c_read(drv, BMG160_MODE_LPM1_ADDR, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    switch(mode){
        case DEV_POWER_ON:{
            if((value & BMG160_POWER_BIT_MASK) == BMG160_POWER_NORMAL_REG){
                return 0;
            }
            value &= (~BMG160_POWER_BIT_MASK);
            value |= BMG160_POWER_NORMAL_REG;
            ret = sensor_i2c_write(drv, BMG160_MODE_LPM1_ADDR, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
            if(unlikely(ret)){
                return ret;
            }
        }break;
        
        case DEV_POWER_OFF:{
            if((value & BMG160_POWER_BIT_MASK) == BMG160_POWER_DEEP_SUSPEND_REG){
                return 0;
            }
            
            value &= (~BMG160_POWER_BIT_MASK);
            value |= BMG160_POWER_DEEP_SUSPEND_REG;
            ret = sensor_i2c_write(drv, BMG160_MODE_LPM1_ADDR, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
            if(unlikely(ret)){
                return ret;
            }
        }break;
            
        case DEV_SLEEP:{
            if((value & BMG160_POWER_BIT_MASK) == BMG160_POWER_SLEEP_REG){
                return 0;
            }
            
            value &= (~BMG160_POWER_BIT_MASK);
            value |= BMG160_POWER_SLEEP_REG;
            ret = sensor_i2c_write(drv, BMG160_MODE_LPM1_ADDR, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
            if(unlikely(ret)){
                return ret;
            }

        }break;
        
       default:break;
    }
    return 0;
}

static int drv_gyro_bosch_bmg160_set_odr(i2c_dev_t* drv, uint32_t odr)
{
    int ret = 0;
    uint8_t value = 0x00;

    ret = sensor_i2c_read(drv, BMG160_BW_ADDR, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

    if(odr >= BMG160_ODR_2000){
        value &= BMG160_BW_ADDR_MSK; 
        value |= BMG160_ODR_2000_REG;
    }
    else if(odr >= BMG160_ODR_1000){
        value &= BMG160_BW_ADDR_MSK; 
        value |= BMG160_ODR_1000_REG;
    }
    else if(odr >= BMG160_ODR_400){
        value &= BMG160_BW_ADDR_MSK; 
        value |= BMG160_ODR_400_REG;
    }
    else if(odr >= BMG160_ODR_200){
        value &= BMG160_BW_ADDR_MSK; 
        value |= BMG160_ODR_200_REG;
    }
    else{
        value &= BMG160_BW_ADDR_MSK; 
        value |= BMG160_ODR_100_REG;
    }

    /* Write the range register 0x0F*/
    ret = sensor_i2c_write(drv, BMG160_BW_ADDR, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    return 0;
}

static int drv_gyro_bosch_bmg160_set_range(i2c_dev_t* drv, uint32_t range)
{
    int ret = 0;
    uint8_t value = 0x00;

    ret = sensor_i2c_read(drv, BMG160_RANGE_ADDR, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    
    switch (range){
        case GYRO_RANGE_125DPS:{
            value &= BMG160_RANGE_ADDR_RANGE_MSK;
            value |= BMG160_RANGE_RANGE_125DPS;
        }break;
        
        case GYRO_RANGE_250DPS:{
            value &= BMG160_RANGE_ADDR_RANGE_MSK;
            value |= BMG160_RANGE_RANGE_250DPS;
        }break;
        
        case GYRO_RANGE_500DPS:{
            value &= BMG160_RANGE_ADDR_RANGE_MSK;
            value |= BMG160_RANGE_RANGE_500DPS;
        }break;
            
        case GYRO_RANGE_1000DPS:{
            value &= BMG160_RANGE_ADDR_RANGE_MSK;
            value |= BMG160_RANGE_RANGE_1000DPS;
        }break;
        
        case GYRO_RANGE_2000DPS:{
            value &= BMG160_RANGE_ADDR_RANGE_MSK;
            value |= BMG160_RANGE_RANGE_2000DPS;
        }break;
        
        default:break;
    }

    /* Write the range register 0x0F*/
    ret = sensor_i2c_write(drv, BMG160_RANGE_ADDR, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    
    if((range >= GYRO_RANGE_125DPS)&&(range <= GYRO_RANGE_2000DPS)){
        current_factor = bmg160_factor[range];
    }
    return 0;    
}

static void drv_gyro_bosch_bmg160_irq_handle(void)
{
    /* no handle so far */
}

static int drv_gyro_bosch_bmg160_open(void)
{
    int ret = 0;
    ret  = drv_gyro_bosch_bmg160_set_power_mode(&bmg160_ctx, DEV_POWER_ON);
    if(unlikely(ret)){
        return -1;
    }
    
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;

}

static int drv_gyro_bosch_bmg160_close(void)
{
    int ret = 0;
    ret  = drv_gyro_bosch_bmg160_set_power_mode(&bmg160_ctx, DEV_POWER_OFF);
    if(unlikely(ret)){
        return -1;
    }
    return 0;
}

static int drv_gyro_bosch_bmg160_read(void *buf, size_t len)
{
    int ret = 0;
    size_t size = 0;
    uint8_t reg[6];
    gyro_data_t *gyro = (gyro_data_t *)buf;
    if(buf == NULL){
        return -1;
    }

    size = sizeof(gyro_data_t);
    if(len < size){
        return -1;
    }

    ret  = sensor_i2c_read(&bmg160_ctx, BMG160_RATE_X_LSB_ADDR,  &reg[0], I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&bmg160_ctx, BMG160_RATE_X_MSB_ADDR,  &reg[1], I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&bmg160_ctx, BMG160_RATE_Y_LSB_ADDR,  &reg[2], I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&bmg160_ctx, BMG160_RATE_Y_MSB_ADDR,  &reg[3], I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&bmg160_ctx, BMG160_RATE_Z_LSB_ADDR,  &reg[4], I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&bmg160_ctx, BMG160_RATE_Z_MSB_ADDR,  &reg[5], I2C_REG_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return -1;
    }
    gyro->data[DATA_AXIS_X] = (int32_t)((((int32_t)((int8_t)reg[1]))<< BMG160_SHIFT_EIGHT_BITS)|reg[0]);
    gyro->data[DATA_AXIS_Y] = (int32_t)((((int32_t)((int8_t)reg[3]))<< BMG160_SHIFT_EIGHT_BITS)|reg[2]);
    gyro->data[DATA_AXIS_Z] = (int32_t)((((int32_t)((int8_t)reg[5]))<< BMG160_SHIFT_EIGHT_BITS)|reg[4]);

    if(current_factor != 0){
        //the unit of gyro is uDPS, 1000 000 uDPS = 1 DPS
        gyro->data[DATA_AXIS_X] = gyro->data[DATA_AXIS_X] * GYROSCOPE_UNIT_FACTOR / current_factor;
        gyro->data[DATA_AXIS_Y] = gyro->data[DATA_AXIS_Y] * GYROSCOPE_UNIT_FACTOR / current_factor;
        gyro->data[DATA_AXIS_Z] = gyro->data[DATA_AXIS_Z] * GYROSCOPE_UNIT_FACTOR / current_factor;
    }

    gyro->timestamp = aos_now_ms();
    return (int)size;
}

static int drv_gyro_bosch_bmg160_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;
    
    switch(cmd){
        case SENSOR_IOCTL_ODR_SET:{
            ret = drv_gyro_bosch_bmg160_set_odr(&bmg160_ctx, arg);
            if(unlikely(ret)){
                return -1;
            }
        }break;
        case SENSOR_IOCTL_RANGE_SET:{
            ret = drv_gyro_bosch_bmg160_set_range(&bmg160_ctx, arg);
            if(unlikely(ret)){
                return -1;
            }
        }break;
        case SENSOR_IOCTL_SET_POWER:{
            ret = drv_gyro_bosch_bmg160_set_power_mode(&bmg160_ctx, arg);
            if(unlikely(ret)){
                return -1;
            }
        }break;
        case SENSOR_IOCTL_GET_INFO:{ 
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
            info->model = "BMG160";
            info->range_max = 2000;
            info->range_min = 125;
            info->unit = udps;
        }break;
       
       default:break;
    }

    return 0;
}

int drv_gyro_bosch_bmg160_init(void)
{
    int ret = 0;
    sensor_obj_t sensor;

    /* fill the sensor obj parameters here */
    sensor.io_port    = I2C_PORT;
    sensor.tag        = TAG_DEV_GYRO;
    sensor.path       = dev_gyro_path;
    sensor.open       = drv_gyro_bosch_bmg160_open;
    sensor.close      = drv_gyro_bosch_bmg160_close;
    sensor.read       = drv_gyro_bosch_bmg160_read;
    sensor.write      = NULL;
    sensor.ioctl      = drv_gyro_bosch_bmg160_ioctl;
    sensor.irq_handle = drv_gyro_bosch_bmg160_irq_handle;

    ret = sensor_create_obj(&sensor);
    if(unlikely(ret)){
        return -1;
    }

    ret = drv_gyro_bosch_bmg160_validate_id(&bmg160_ctx, BMG160_CHIP_ID_VALUE);
    if(unlikely(ret)){
        return -1;
    }

    ret = drv_gyro_bosch_bmg160_set_range(&bmg160_ctx, GYRO_RANGE_2000DPS);
    if(unlikely(ret)){
        return -1;
    }
    
    ret = drv_gyro_bosch_bmg160_set_odr(&bmg160_ctx, BMG160_ODR_200);
    if(unlikely(ret)){
        return -1;
    }

    ret = drv_gyro_bosch_bmg160_set_power_mode(&bmg160_ctx, DEV_POWER_OFF);
    if(unlikely(ret)){
        return -1;
    }

    /* update the phy sensor info to sensor hal */
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

