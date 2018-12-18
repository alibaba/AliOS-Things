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
//#include "soc_init.h"


#define PAJ7620_SLAVE_ADDRESS    (0x73 << 1)

#define BIT(x)  1 << x

// REGISTER DESCRIPTION
#define PAJ7620_VAL(val, maskbit)        ( val << maskbit )
#define PAJ7620_ADDR_BASE                0x00

// REGISTER BANK SELECT
#define PAJ7620_REGITER_BANK_SEL        (PAJ7620_ADDR_BASE + 0xEF)    //W

// REGISTER BANK 0
#define PAJ7620_ADDR_SUSPEND_CMD        (PAJ7620_ADDR_BASE + 0x3)    //W
#define PAJ7620_ADDR_GES_PS_DET_MASK_0        (PAJ7620_ADDR_BASE + 0x41)    //RW
#define PAJ7620_ADDR_GES_PS_DET_MASK_1        (PAJ7620_ADDR_BASE + 0x42)    //RW
#define PAJ7620_ADDR_GES_PS_DET_FLAG_0        (PAJ7620_ADDR_BASE + 0x43)    //R
#define PAJ7620_ADDR_GES_PS_DET_FLAG_1        (PAJ7620_ADDR_BASE + 0x44)    //R
#define PAJ7620_ADDR_STATE_INDICATOR    (PAJ7620_ADDR_BASE + 0x45)    //R
#define PAJ7620_ADDR_PS_HIGH_THRESHOLD    (PAJ7620_ADDR_BASE + 0x69)    //RW
#define PAJ7620_ADDR_PS_LOW_THRESHOLD    (PAJ7620_ADDR_BASE + 0x6A)    //RW
#define PAJ7620_ADDR_PS_APPROACH_STATE    (PAJ7620_ADDR_BASE + 0x6B)    //R
#define PAJ7620_ADDR_PS_RAW_DATA        (PAJ7620_ADDR_BASE + 0x6C)    //R

// REGISTER BANK 1
#define PAJ7620_ADDR_PS_GAIN            (PAJ7620_ADDR_BASE + 0x44)    //RW
#define PAJ7620_ADDR_IDLE_S1_STEP_0        (PAJ7620_ADDR_BASE + 0x67)    //RW
#define PAJ7620_ADDR_IDLE_S1_STEP_1        (PAJ7620_ADDR_BASE + 0x68)    //RW
#define PAJ7620_ADDR_IDLE_S2_STEP_0        (PAJ7620_ADDR_BASE + 0x69)    //RW
#define PAJ7620_ADDR_IDLE_S2_STEP_1        (PAJ7620_ADDR_BASE + 0x6A)    //RW
#define PAJ7620_ADDR_OP_TO_S1_STEP_0    (PAJ7620_ADDR_BASE + 0x6B)    //RW
#define PAJ7620_ADDR_OP_TO_S1_STEP_1    (PAJ7620_ADDR_BASE + 0x6C)    //RW
#define PAJ7620_ADDR_OP_TO_S2_STEP_0    (PAJ7620_ADDR_BASE + 0x6D)    //RW
#define PAJ7620_ADDR_OP_TO_S2_STEP_1    (PAJ7620_ADDR_BASE + 0x6E)    //RW
#define PAJ7620_ADDR_OPERATION_ENABLE    (PAJ7620_ADDR_BASE + 0x72)    //RW

// PAJ7620_REGITER_BANK_SEL
#define PAJ7620_BANK0        PAJ7620_VAL(0,0)
#define PAJ7620_BANK1        PAJ7620_VAL(1,0)

// PAJ7620_ADDR_SUSPEND_CMD
#define PAJ7620_I2C_WAKEUP    PAJ7620_VAL(1,0)
#define PAJ7620_I2C_SUSPEND    PAJ7620_VAL(0,0)

// PAJ7620_ADDR_OPERATION_ENABLE
#define PAJ7620_ENABLE        PAJ7620_VAL(1,0)
#define PAJ7620_DISABLE        PAJ7620_VAL(0,0)

enum {
    BANK0 = 0,
    BANK1,        
};

#define GES_RIGHT_FLAG                PAJ7620_VAL(1,0)
#define GES_LEFT_FLAG                PAJ7620_VAL(1,1)
#define GES_UP_FLAG                    PAJ7620_VAL(1,2)
#define GES_DOWN_FLAG                PAJ7620_VAL(1,3)
#define GES_FORWARD_FLAG            PAJ7620_VAL(1,4)
#define GES_BACKWARD_FLAG            PAJ7620_VAL(1,5)
#define GES_CLOCKWISE_FLAG            PAJ7620_VAL(1,6)
#define GES_COUNT_CLOCKWISE_FLAG    PAJ7620_VAL(1,7)
#define GES_WAVE_FLAG                PAJ7620_VAL(1,0)


enum {
    GESTURE_INVALID = 0,
    GESTURE_UP,
    GESTURE_DOWN,
    GESTURE_LEFT,
    GESTURE_RIGHT,
    GESTURE_FORWARD,
    GESTURE_BACKWARD,
    GESTURE_CLOCKWISE,
    GESTURE_ANTICLOCKWISE,
    GESTURE_WAVE,
};

static unsigned char paj7620_init_regs[][2] = {
    {0xEF,0x00},
    {0x32,0x29},
    {0x33,0x01},
    {0x34,0x00},
    {0x35,0x01},
    {0x36,0x00},
    {0x37,0x07},
    {0x38,0x17},
    {0x39,0x06},
    {0x3A,0x12},
    {0x3F,0x00},
    {0x40,0x02},
    {0x41,0xFF},
    {0x42,0x01},
    {0x46,0x2D},
    {0x47,0x0F},
    {0x48,0x3C},
    {0x49,0x00},
    {0x4A,0x1E},
    {0x4B,0x00},
    {0x4C,0x20},
    {0x4D,0x00},
    {0x4E,0x1A},
    {0x4F,0x14},
    {0x50,0x00},
    {0x51,0x10},
    {0x52,0x00},
    {0x5C,0x02},
    {0x5D,0x00},
    {0x5E,0x10},
    {0x5F,0x3F},
    {0x60,0x27},
    {0x61,0x28},
    {0x62,0x00},
    {0x63,0x03},
    {0x64,0xF7},
    {0x65,0x03},
    {0x66,0xD9},
    {0x67,0x03},
    {0x68,0x01},
    {0x69,0xC8},
    {0x6A,0x40},
    {0x6D,0x04},
    {0x6E,0x00},
    {0x6F,0x00},
    {0x70,0x80},
    {0x71,0x00},
    {0x72,0x00},
    {0x73,0x00},
    {0x74,0xF0},
    {0x75,0x00},
    {0x80,0x42},
    {0x81,0x44},
    {0x82,0x04},
    {0x83,0x20},
    {0x84,0x20},
    {0x85,0x00},
    {0x86,0x10},
    {0x87,0x00},
    {0x88,0x05},
    {0x89,0x18},
    {0x8A,0x10},
    {0x8B,0x01},
    {0x8C,0x37},
    {0x8D,0x00},
    {0x8E,0xF0},
    {0x8F,0x81},
    {0x90,0x06},
    {0x91,0x06},
    {0x92,0x1E},
    {0x93,0x0D},
    {0x94,0x0A},
    {0x95,0x0A},
    {0x96,0x0C},
    {0x97,0x05},
    {0x98,0x0A},
    {0x99,0x41},
    {0x9A,0x14},
    {0x9B,0x0A},
    {0x9C,0x3F},
    {0x9D,0x33},
    {0x9E,0xAE},
    {0x9F,0xF9},
    {0xA0,0x48},
    {0xA1,0x13},
    {0xA2,0x10},
    {0xA3,0x08},
    {0xA4,0x30},
    {0xA5,0x19},
    {0xA6,0x10},
    {0xA7,0x08},
    {0xA8,0x24},
    {0xA9,0x04},
    {0xAA,0x1E},
    {0xAB,0x1E},
    {0xCC,0x19},
    {0xCD,0x0B},
    {0xCE,0x13},
    {0xCF,0x64},
    {0xD0,0x21},
    {0xD1,0x0F},
    {0xD2,0x88},
    {0xE0,0x01},
    {0xE1,0x04},
    {0xE2,0x41},
    {0xE3,0xD6},
    {0xE4,0x00},
    {0xE5,0x0C},
    {0xE6,0x0A},
    {0xE7,0x00},
    {0xE8,0x00},
    {0xE9,0x00},
    {0xEE,0x07},
    {0xEF,0x01},
    {0x00,0x1E},
    {0x01,0x1E},
    {0x02,0x0F},
    {0x03,0x10},
    {0x04,0x02},
    {0x05,0x00},
    {0x06,0xB0},
    {0x07,0x04},
    {0x08,0x0D},
    {0x09,0x0E},
    {0x0A,0x9C},
    {0x0B,0x04},
    {0x0C,0x05},
    {0x0D,0x0F},
    {0x0E,0x02},
    {0x0F,0x12},
    {0x10,0x02},
    {0x11,0x02},
    {0x12,0x00},
    {0x13,0x01},
    {0x14,0x05},
    {0x15,0x07},
    {0x16,0x05},
    {0x17,0x07},
    {0x18,0x01},
    {0x19,0x04},
    {0x1A,0x05},
    {0x1B,0x0C},
    {0x1C,0x2A},
    {0x1D,0x01},
    {0x1E,0x00},
    {0x21,0x00},
    {0x22,0x00},
    {0x23,0x00},
    {0x25,0x01},
    {0x26,0x00},
    {0x27,0x39},
    {0x28,0x7F},
    {0x29,0x08},
    {0x30,0x03},
    {0x31,0x00},
    {0x32,0x1A},
    {0x33,0x1A},
    {0x34,0x07},
    {0x35,0x07},
    {0x36,0x01},
    {0x37,0xFF},
    {0x38,0x36},
    {0x39,0x07},
    {0x3A,0x00},
    {0x3E,0xFF},
    {0x3F,0x00},
    {0x40,0x77},
    {0x41,0x40},
    {0x42,0x00},
    {0x43,0x30},
    {0x44,0xA0},
    {0x45,0x5C},
    {0x46,0x00},
    {0x47,0x00},
    {0x48,0x58},
    {0x4A,0x1E},
    {0x4B,0x1E},
    {0x4C,0x00},
    {0x4D,0x00},
    {0x4E,0xA0},
    {0x4F,0x80},
    {0x50,0x00},
    {0x51,0x00},
    {0x52,0x00},
    {0x53,0x00},
    {0x54,0x00},
    {0x57,0x80},
    {0x59,0x10},
    {0x5A,0x08},
    {0x5B,0x94},
    {0x5C,0xE8},
    {0x5D,0x08},
    {0x5E,0x3D},
    {0x5F,0x99},
    {0x60,0x45},
    {0x61,0x40},
    {0x63,0x2D},
    {0x64,0x02},
    {0x65,0x96},
    {0x66,0x00},
    {0x67,0x97},
    {0x68,0x01},
    {0x69,0xCD},
    {0x6A,0x01},
    {0x6B,0xB0},
    {0x6C,0x04},
    {0x6D,0x2C},
    {0x6E,0x01},
    {0x6F,0x32},
    {0x71,0x00},
    {0x72,0x01},
    {0x73,0x35},
    {0x74,0x00},
    {0x75,0x33},
    {0x76,0x31},
    {0x77,0x01},
    {0x7C,0x84},
    {0x7D,0x03},
    {0x7E,0x01},
};

static i2c_dev_t paj7620_client = {
    .port = 3,
    .config.dev_addr = PAJ7620_SLAVE_ADDRESS,
};

uint8_t gesture_action;

static int paj7620_reg_write(uint8_t reg, uint8_t val)
{
    if (sensor_i2c_write(&paj7620_client, reg, &val, 1, 200)) {
        printf("%s: write failed !\n", __func__);
        return 1;
    }
    return 0;
}

static int paj7620_reg_read(uint8_t reg, uint8_t *val)
{
    if (sensor_i2c_read(&paj7620_client, reg, val, 1, 200)) {
        //printf("%s: read failed !\n", __func__);
        return 1;
    }
    return 0;
}

static int paj7620_bank_select(int bank)
{
    switch (bank) {
    case BANK0:
        paj7620_reg_write(PAJ7620_REGITER_BANK_SEL, PAJ7620_BANK0);
        break;
    case BANK1:
        paj7620_reg_write(PAJ7620_REGITER_BANK_SEL, PAJ7620_BANK1);
        break;
    default:
        break;
    }
    return 0;
}

static uint8_t paj7620_gesture(uint8_t gesture)
    {
    uint8_t data = 0;
    uint8_t data1 = 0;
    uint8_t rc = 0;

    switch (gesture) {
    case GES_RIGHT_FLAG:
        aos_msleep(1);
        paj7620_reg_read(PAJ7620_ADDR_GES_PS_DET_FLAG_0, &data);
        if (data == GES_FORWARD_FLAG)  {
            rc = GESTURE_FORWARD;
            aos_msleep(1);
        } else if (data == GES_BACKWARD_FLAG)  {
            rc = GESTURE_BACKWARD;
            aos_msleep(1);
        } else {
            rc = GESTURE_RIGHT;
        }          
        break;
    case GES_LEFT_FLAG: 
        aos_msleep(1);
        paj7620_reg_read(PAJ7620_ADDR_GES_PS_DET_FLAG_0, &data);
        if (data == GES_FORWARD_FLAG) {
            rc = GESTURE_FORWARD;
            aos_msleep(1);
        } else if(data == GES_BACKWARD_FLAG) {
            rc = GESTURE_BACKWARD;
            aos_msleep(1);
        } else {
            rc = GESTURE_LEFT;
        }          
        break;
    case GES_UP_FLAG:
        aos_msleep(1);
        paj7620_reg_read(PAJ7620_ADDR_GES_PS_DET_FLAG_0, &data);
        if (data == GES_FORWARD_FLAG) {
            rc = GESTURE_FORWARD;
            aos_msleep(1);
        } else if(data == GES_BACKWARD_FLAG) {
            rc = GESTURE_BACKWARD;
            aos_msleep(1);
        } else {
            rc = GESTURE_UP;
        }          
        break;
    case GES_DOWN_FLAG:
        aos_msleep(1);
        paj7620_reg_read(PAJ7620_ADDR_GES_PS_DET_FLAG_0, &data);
        if(data == GES_FORWARD_FLAG) {
            rc = GESTURE_FORWARD;
            aos_msleep(1);
        } else if(data == GES_BACKWARD_FLAG) {
            rc = GESTURE_BACKWARD;
            aos_msleep(1);
        } else {
            rc = GESTURE_DOWN;
        }          
        break;
    case GES_FORWARD_FLAG:
        rc = GESTURE_FORWARD;
        aos_msleep(1);
        break;
    case GES_BACKWARD_FLAG:
        rc = GESTURE_BACKWARD;
        aos_msleep(1);
        break;
    case GES_CLOCKWISE_FLAG:
        rc = GESTURE_CLOCKWISE;
        break;
    case GES_COUNT_CLOCKWISE_FLAG:
        rc = GESTURE_ANTICLOCKWISE;
        break;  
    default:
        paj7620_reg_read(PAJ7620_ADDR_GES_PS_DET_FLAG_1, &data1);
        if (data1 == GES_WAVE_FLAG)
            rc = GESTURE_WAVE;
        break;
    }
    return rc;
}

int paj7620_poll(void)
{
    uint8_t gesture = 0;
    while (1) {
        aos_msleep(5);
        paj7620_reg_read(PAJ7620_ADDR_GES_PS_DET_FLAG_0, &gesture);
        paj7620_gesture(gesture);
    }
    return 0;
}

static uint8_t paj7620_gesture_get(uint8_t gesture)
{
    uint8_t data = 0;
    uint8_t data1 = 0;
    uint8_t rc = 0;

    switch (gesture) {
    case GES_RIGHT_FLAG:
        aos_msleep(1);
        paj7620_reg_read(PAJ7620_ADDR_GES_PS_DET_FLAG_0, &data);
        if (data == GES_FORWARD_FLAG)  {
            rc = GS_SENSOR_INVALID;
            aos_msleep(1);
        } else if (data == GES_BACKWARD_FLAG)  {
            rc = GS_SENSOR_INVALID;
            aos_msleep(1);
        } else {
            rc = GS_SENSOR_RIGHT;
        }          
        break;
    case GES_LEFT_FLAG: 
        aos_msleep(1);
        paj7620_reg_read(PAJ7620_ADDR_GES_PS_DET_FLAG_0, &data);
        if (data == GES_FORWARD_FLAG) {
            rc = GS_SENSOR_INVALID;
            aos_msleep(1);
        } else if(data == GES_BACKWARD_FLAG) {
            rc = GS_SENSOR_INVALID;
            aos_msleep(1);
        } else {
            rc = GS_SENSOR_LEFT;
        }          
        break;
    case GES_UP_FLAG:
        aos_msleep(1);
        paj7620_reg_read(PAJ7620_ADDR_GES_PS_DET_FLAG_0, &data);
        if (data == GES_FORWARD_FLAG) {
            rc = GS_SENSOR_INVALID;
            aos_msleep(1);
        } else if(data == GES_BACKWARD_FLAG) {
            rc = GS_SENSOR_INVALID;
            aos_msleep(1);
        } else {
            rc = GS_SENSOR_UP;
        }          
        break;
    case GES_DOWN_FLAG:
        aos_msleep(1);
        paj7620_reg_read(PAJ7620_ADDR_GES_PS_DET_FLAG_0, &data);
        if(data == GES_FORWARD_FLAG) {
            rc = GS_SENSOR_INVALID;
            aos_msleep(1);
        } else if(data == GES_BACKWARD_FLAG) {
            rc = GS_SENSOR_INVALID;
            aos_msleep(1);
        } else {
            rc = GS_SENSOR_DOWN;
        }          
        break;
    case GES_FORWARD_FLAG:
        rc = GS_SENSOR_INVALID;
        aos_msleep(1);
        break;
    case GES_BACKWARD_FLAG:
        rc = GS_SENSOR_INVALID;
        aos_msleep(1);
        break;
    case GES_CLOCKWISE_FLAG:
        rc = GS_SENSOR_CLOCKWISE;
        break;
    case GES_COUNT_CLOCKWISE_FLAG:
        rc = GS_SENSOR_ANTICLOCKWISE;
        break;  
    default:
        paj7620_reg_read(PAJ7620_ADDR_GES_PS_DET_FLAG_1, &data1);
        if (data1 == GES_WAVE_FLAG)
            rc = GS_SENSOR_WAVE;
        break;
    }
    return rc;
}


static int drv_gs_pixart_paj7620_open(void)
{
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

static int drv_gs_pixart_paj7620_close(void)
{
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

static int drv_gs_pixart_paj7620_read(void *buf, size_t len)
{
    uint8_t gesture = 0;
    size_t size = sizeof(gs_data_t);
    gs_data_t* gs_data = (gs_data_t*)buf;
    if(NULL == buf){
        return -1;
    }

    if(len <= size){
        return -1;
    }

    paj7620_reg_read(PAJ7620_ADDR_GES_PS_DET_FLAG_0, &gesture);
    gs_data->gs_type = (gs_type_e)paj7620_gesture_get(gesture);
    gs_data->timestamp = aos_now_ms();

    return (int) size;
}

static int drv_gs_pixart_paj7620_write(const void *buf, size_t len)
{
    (void) buf;
    (void) len;

    return 0;
}

static int drv_gs_pixart_paj7620_ioctl(int cmd, unsigned long arg)
{

    switch (cmd) {
        case SENSOR_IOCTL_SET_POWER: {
        } break;
        case SENSOR_IOCTL_GET_INFO: {
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *) arg;
            info->model = "paj7620";
        } break;
        default:
            return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}


int drv_gs_pixart_paj7620_init(void)
{
    int ret;
    uint8_t data0 = 1;
    uint8_t data1 = 2;
    int i;

    sensor_obj_t sensor;
    memset(&sensor, 0, sizeof(sensor));

    /* fill the sensor_gs obj parameters here */
    sensor.tag = TAG_DEV_GS;
    sensor.path = dev_gs_path;
    sensor.io_port = I2C_PORT;
    sensor.mode = DEV_POLLING;
    sensor.power = DEV_POWER_OFF;
    sensor.open = drv_gs_pixart_paj7620_open;
    sensor.close = drv_gs_pixart_paj7620_close;
    sensor.read = drv_gs_pixart_paj7620_read;
    sensor.write = drv_gs_pixart_paj7620_write;
    sensor.ioctl = drv_gs_pixart_paj7620_ioctl;
    sensor.irq_handle = NULL;

    ret = sensor_create_obj(&sensor);
    if (unlikely(ret)) {
        return -1;
    }

    aos_msleep(10);
    paj7620_bank_select(BANK0);
    paj7620_bank_select(BANK0);

    paj7620_reg_read(0, &data0);
    paj7620_reg_read(1, &data1);

    if (data0 != 0x20 || data1 != 0x76) {
        printf("%s: paj7620 check failed!\n", __func__);
        return 1;
    }
    if (data0 == 0x20)
        printf("%s: paj7620 wakeup\n", __func__);

    for (i = 0; i < sizeof(paj7620_init_regs)/sizeof(paj7620_init_regs[0]); i++) {
        paj7620_reg_write(paj7620_init_regs[i][0], paj7620_init_regs[i][1]);
    }

    paj7620_bank_select(BANK0);
    aos_msleep(100);
    uint8_t gesture;
    paj7620_reg_read(PAJ7620_ADDR_GES_PS_DET_FLAG_0, &gesture);
    paj7620_reg_read(PAJ7620_ADDR_GES_PS_DET_FLAG_1, &gesture);
    printf("%s: paj7620 init success\n", __func__);

    return 0;
}

SENSOR_DRV_ADD(drv_gs_pixart_paj7620_init);

