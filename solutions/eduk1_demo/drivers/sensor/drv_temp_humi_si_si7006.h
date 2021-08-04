
/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "aos/hal/i2c.h"
#include <math.h>
#include <stdbool.h>

#ifndef SI7006_h
#define SI7006_h

#define SI7006_ADDRESS 0x40
#define SI7006_TAG     0x06

#define SI7006_MEAS_REL_HUMIDITY_MASTER_MODE    0xE5
#define SI7006_MEAS_REL_HUMIDITY_NO_MASTER_MODE 0xF5
#define SI7006_MEAS_TEMP_MASTER_MODE            0xE3
#define SI7006_MEAS_TEMP_NO_MASTER_MODE         0xF3
#define SI7006_READ_OLD_TEMP                    0xE0
#define SI7006_RESET                            0xFE
#define SI7006_READ_ID_LOW_0                    0xFA
#define SI7006_READ_ID_LOW_1                    0x0F
#define SI7006_READ_ID_HIGH_0                   0xFC
#define SI7006_READ_ID_HIGH_1                   0xC9
#define SI7006_READ_Firmware_Revision_0         0x84
#define SI7006_READ_Firmware_Revision_1         0xB8

extern void si7006_init(void);
extern uint8_t si7006_getVer(void);
extern void si7006_getID(uint8_t *id_buf);
extern bool si7006_getTemperature(float *temperature);
extern bool si7006_getHumidity(float *humidity);
extern void si7006_getTempHumidity(float *humidity, float *temperature);
extern void si7006_deinit(void);
#endif
