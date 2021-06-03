/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

/*
The driver for AP3216C chip, The AP3216C is an integrated ALS & PS module
that includes a digital ambient light sensor [ALS], a proximity sensor [PS],
and an IR LED in a single package.
*/
/*
添加如下配置到app.json中：
        "ap3216c": {
            "type": "I2C",
            "port": 1,
            "addrWidth": 7,
            "freq": 100000,
            "mode": "master",
            "devAddr": 30
        }
*/
// Require libjs/lib/i2c.js module.

const AP3216C_I2C_PORT = 1
const AP3216C_ADDR      = 0x1e

// System Register
const AP3216C_SYS_CONFIGURATION_REG    = 0x00
const AP3216C_SYS_INT_STATUS_REG       = 0x01
const AP3216C_SYS_INT_CLEAR_MANNER_REG = 0x02
const AP3216C_IR_DATA_L_REG            = 0x0A
const AP3216C_IR_DATA_H_REG            = 0x0B
const AP3216C_ALS_DATA_L_REG           = 0x0C
const AP3216C_ALS_DATA_H_REG           = 0x0D
const AP3216C_PS_DATA_L_REG            = 0x0E
const AP3216C_PS_DATA_H_REG            = 0x0F

// ALS Register
const AP3216C_ALS_CONFIGURATION_REG    = 0x10
const AP3216C_ALS_CALIBRATION_REG      = 0x19
const AP3216C_ALS_THRESHOLD_LOW_L_REG  = 0x1A
const AP3216C_ALS_THRESHOLD_LOW_H_REG  = 0x1B
const AP3216C_ALS_THRESHOLD_HIGH_L_REG = 0x1C
const AP3216C_ALS_THRESHOLD_HIGH_H_REG = 0x1D

// PS Register
const AP3216C_PS_CONFIGURATION_REG    = 0x20
const AP3216C_PS_LED_DRIVER_REG       = 0x21
const AP3216C_PS_INT_FORM_REG         = 0x22
const AP3216C_PS_MEAN_TIME_REG        = 0x23
const AP3216C_PS_LED_WAITING_TIME_REG = 0x24
const AP3216C_PS_CALIBRATION_L_REG    = 0x28
const AP3216C_PS_CALIBRATION_H_REG    = 0x29
const AP3216C_PS_THRESHOLD_LOW_L_REG  = 0x2A
const AP3216C_PS_THRESHOLD_LOW_H_REG  = 0x2B
const AP3216C_PS_THRESHOLD_HIGH_L_REG = 0x2C
const AP3216C_PS_THRESHOLD_HIGH_H_REG = 0x2D

//mode value
const AP3216C_MODE_POWER_DOWN    = 0x0
const AP3216C_MODE_ALS           = 0x1
const AP3216C_MODE_PS            = 0x2
const AP3216C_MODE_ALS_AND_PS    = 0x3
const AP3216C_MODE_SW_RESET      = 0x4
const AP3216C_MODE_ALS_ONCE      = 0x5
const AP3216C_MODE_PS_ONCE       = 0x6
const AP3216C_MODE_ALS_AND_PS_ONCE  = 0x7

//ap3216c_int_clear_manner
const AP3216C_INT_CLEAR_MANNER_BY_READING    = 0x0
const AP3216C_ALS_CLEAR_MANNER_BY_SOFTWARE   = 0x1

//als_range
const AP3216C_ALS_RANGE_20661    = 0x0
const AP3216C_ALS_RANGE_5162     = 0x1
const AP3216C_ALS_RANGE_1291     = 0x2
const AP3216C_ALS_RANGE_323    = 0x3

//als_range
const AP3216C_PS_GAIN1    = 0x0
const AP3216C_PS_GAIN2     = 0x1
const AP3216C_PS_GAIN4     = 0x2
const AP3216C_PS_GAIN8    = 0x3

const AP3216C_SYSTEM_MODE          = 0x0
const AP3216C_INT_PARAM            = 0x1
const AP3216C_ALS_RANGE            = 0x2
const AP3216C_ALS_PERSIST          = 0x3
const AP3216C_ALS_CALIBRATION      = 0x4
const AP3216C_ALS_LOW_THRESHOLD_L  = 0x5
const AP3216C_ALS_LOW_THRESHOLD_H  = 0x6
const AP3216C_ALS_HIGH_THRESHOLD_L = 0x7
const AP3216C_ALS_HIGH_THRESHOLD_H = 0x8
const AP3216C_PS_INTEGRATED_TIME   = 0x9
const AP3216C_PS_GAIN              = 0xa
const AP3216C_PS_PERSIST           = 0xb
const AP3216C_PS_LED_CONTROL       = 0xc
const AP3216C_PS_LED_DRIVER_RATIO  = 0xd
const AP3216C_PS_INT_MODE          = 0xe
const AP3216C_PS_MEAN_TIME         = 0xf
const AP3216C_PS_WAITING_TIME      = 0x10
const AP3216C_PS_CALIBRATION_L     = 0x11
const AP3216C_PS_CALIBRATION_H     = 0x12
const AP3216C_PS_LOW_THRESHOLD_L   = 0x13
const AP3216C_PS_LOW_THRESHOLD_H   = 0x14
const AP3216C_PS_HIGH_THRESHOLD_L  = 0x15
const AP3216C_PS_HIGH_THRESHOLD_H  = 0x16

var i2c = require('i2c');

var ap3216cDev;

function fakeSleep(ms) {
    var date = new Date();
    var curDate;
    do {
        curDate = new Date();
    } while((curDate - date) < ms);
}

/* 写寄存器的值 */
function write_reg(addr, data)
{
    var msgbuf = [data];
    ap3216cDev.writeMem(addr, msgbuf);
    console.log("--> write addr " + addr + ", value = " + msgbuf);
}

/* 读寄存器的值 */
function read_regs(addr, len)
{
    buf = ap3216cDev.readMem(addr, len);
    console.log("--> read addr " + addr + ", value = " + buf);
    return buf;
}

/* 软件复位传感器 */
function reset_sensor()
{
    write_reg(AP3216C_SYS_CONFIGURATION_REG, AP3216C_MODE_SW_RESET); // reset
}

/**
 * This function is convenient to getting data except including high and low
 * data for this sensor. note:after reading lower register first,reading higher
 * add one.
 */
function read_low_and_high(reg, len)
{
    var data;
    var buf0;
    var buf1;

    buf0 = read_regs(reg, len); // 读低字节
    buf1 = read_regs(reg + 1, len);  // 读高字节
    data = buf0 | (buf1 << len * 8); // 合并数据
    data = data >>> 0

    return data;
}

/**
 * This function reads status register by ap3216c sensor measurement
 *
 * @param no
 *
 * @return status register value.
 */

function ap3216c_get_IntStatus()
{
    var IntStatus;

    /* 读中断状态寄存器 */

    IntStatus = read_regs(AP3216C_SYS_INT_STATUS_REG, 1);
    // IntStatus 第 0 位表示 ALS 中断，第 1 位表示 PS 中断。

    return IntStatus; // 返回状态
}

function ap3216c_int_init() { ; }

/**
 * @brief  配置 中断输入引脚
 * @param  无
 * @retval 无
 */
function ap3216c_int_Config() { ; }

/**
 * This function initializes ap3216c registered device driver
 *
 * @param no
 *
 * @return the ap3216c device.
 */
function init(i2cid)
{
    ap3216cDev = i2c.open({id: i2cid});
    fakeSleep(30);

    /* reset ap3216c */
    reset_sensor();
    fakeSleep(100);
    ap3216c_set_param(AP3216C_SYSTEM_MODE, AP3216C_MODE_ALS_AND_PS);
    fakeSleep(150); // delay at least 112.5ms

    ap3216c_int_Config();
    ap3216c_int_init();
}

function ap3216c_deinit() {
    ap3216cDev.close();
}

/**
 * This function reads light by ap3216c sensor measurement
 *
 * @param no
 *
 * @return the ambient light converted to float data.
 */
function ap3216c_read_ambient_light()
{
    var brightness; // default error data
    var read_data;
    var range;

    read_data = read_low_and_high(AP3216C_ALS_DATA_L_REG, 1);
    range = ap3216c_get_param(AP3216C_ALS_RANGE);
    //console.log("ap3216c_read_ambient_light read_data is " , read_data, range);
    if (range == AP3216C_ALS_RANGE_20661) {
        brightness =
            0.35 * read_data; // sensor ambient light converse to reality
    } else if (range == AP3216C_ALS_RANGE_5162) {
        brightness =
            0.0788 * read_data; // sensor ambient light converse to reality
    } else if (range == AP3216C_ALS_RANGE_1291) {
        brightness =
            0.0197 * read_data; // sensor ambient light converse to reality
    } else if (range == AP3216C_ALS_RANGE_323) {
        brightness =
            0.0049 * read_data; // sensor ambient light converse to reality
    }
    return brightness;
}

/**
 * This function reads proximity by ap3216c sensor measurement
 *
 * @param no
 *
 * @return the proximity data.
 */
function ap3216c_read_ps_data()
{
    var proximity = 0;

    var read_data;
    read_data = read_low_and_high(AP3216C_PS_DATA_L_REG, 1); // read two data
    //console.log("ap3216c_read_ps_data read_data is " , read_data);
    if (1 == ((read_data >> 6) & 0x01 || (read_data >> 14) & 0x01)) {
        return proximity = 55555; // 红外过高（IR），PS无效 返回一个 55555 的无效数据
    }

    proximity = (read_data & 0x000f) + (((read_data >> 8) & 0x3f) << 4); // sensor proximity converse to reality

    proximity = proximity >>> 0

    proximity |= read_data & 0x8000; // 取最高位，0 表示物体远离，1 表示物体靠近

    return proximity; // proximity 后十位是数据位，最高位为状态位
}

/**
 * This function reads ir by ap3216c sensor measurement
 *
 * @param no
 *
 * @return the ir data.
 */
function ap3216c_read_ir_data()
{
    var proximity = 0;

    var read_data;
    read_data = read_low_and_high(AP3216C_IR_DATA_L_REG, 1); // read two data
    //console.log("ap3216c_read_ir_data read_data is" , read_data);
    proximity =
        (read_data & 0x0003) +
        ((read_data >> 8) & 0xFF); // sensor proximity converse to reality

    proximity = proximity >>> 0

    return proximity;
}

/**
 * This function sets parameter of ap3216c sensor
 *
 * @param cmd the parameter cmd of device
 * @param value for setting value in cmd register
 *
 * @return the setting parameter status,RT_EOK reprensents setting successfully.
 */
function ap3216c_set_param(cmd, value)
{
    switch (cmd) {
        case AP3216C_SYSTEM_MODE:
            {
                /* default 000,power down */
                write_reg(AP3216C_SYS_CONFIGURATION_REG, value);
                break;
            }
        case AP3216C_INT_PARAM:
            {
                write_reg(AP3216C_SYS_INT_CLEAR_MANNER_REG, value);

                break;
            }

        case AP3216C_ALS_RANGE:
            {
                var args = read_regs(AP3216C_ALS_CONFIGURATION_REG, 1);
                args &= 0xcf;
                args |= value << 4;
                write_reg(AP3216C_ALS_CONFIGURATION_REG, args);

                break;
            }
        case AP3216C_ALS_PERSIST:
            {
                var args = read_regs(AP3216C_ALS_CONFIGURATION_REG, 1);
                args &= 0xf0;
                args |= value;
                write_reg(AP3216C_ALS_CONFIGURATION_REG, args);

                break;
            }
        case AP3216C_ALS_LOW_THRESHOLD_L:
            {
                write_reg(AP3216C_ALS_THRESHOLD_LOW_L_REG, value);

                break;
            }
        case AP3216C_ALS_LOW_THRESHOLD_H:
            {
                write_reg(AP3216C_ALS_THRESHOLD_LOW_H_REG, value);

                break;
            }
        case AP3216C_ALS_HIGH_THRESHOLD_L:
            {
                write_reg(AP3216C_ALS_THRESHOLD_HIGH_L_REG, value);

                break;
            }
        case AP3216C_ALS_HIGH_THRESHOLD_H:
            {
                write_reg(AP3216C_ALS_THRESHOLD_HIGH_H_REG, value);

                break;
            }
        case AP3216C_PS_GAIN:
            {
                var args = read_regs(AP3216C_PS_CONFIGURATION_REG, 1);
                args &= 0xf3;
                args |= value;
                write_reg(AP3216C_PS_CONFIGURATION_REG, args);

                break;
            }
        case AP3216C_PS_PERSIST:
            {
                var args = read_regs(AP3216C_PS_CONFIGURATION_REG, 1);
                args &= 0xfc;
                args |= value;
                write_reg(AP3216C_PS_CONFIGURATION_REG, args);

                break;
            }
        case AP3216C_PS_LOW_THRESHOLD_L:
            {
                write_reg(AP3216C_PS_THRESHOLD_LOW_L_REG, value);

                break;
            }
        case AP3216C_PS_LOW_THRESHOLD_H:
            {
                write_reg(AP3216C_PS_THRESHOLD_LOW_H_REG, value);

                break;
            }
        case AP3216C_PS_HIGH_THRESHOLD_L:
            {
                write_reg(AP3216C_PS_THRESHOLD_HIGH_L_REG, value);

                break;
            }
        case AP3216C_PS_HIGH_THRESHOLD_H:
            {
                write_reg(AP3216C_PS_THRESHOLD_HIGH_H_REG, value);

                break;
            }

        default:
            {
            }
    }
}

/**
 * This function gets parameter of ap3216c sensor
 *
 * @param cmd the parameter cmd of device
 * @param value to get value in cmd register
 *
 * @return the getting parameter status,RT_EOK reprensents getting successfully.
 */
function ap3216c_get_param(cmd)
{
    switch (cmd) {
        case AP3216C_SYSTEM_MODE:
            {
                value = read_regs(AP3216C_SYS_CONFIGURATION_REG, 1);

                break;
            }
        case AP3216C_INT_PARAM:
            {
                value = read_regs(AP3216C_SYS_INT_CLEAR_MANNER_REG, 1);

                break;
            }
        case AP3216C_ALS_RANGE:
            {
                var value = read_regs(AP3216C_ALS_CONFIGURATION_REG, 1);
                temp = (value & 0xff) >> 4;

                value = temp;

                break;
            }
        case AP3216C_ALS_PERSIST:
            {
                var temp = read_regs(AP3216C_ALS_CONFIGURATION_REG, 1);
                temp = value & 0x0f;

                value = temp;

                break;
            }
        case AP3216C_ALS_LOW_THRESHOLD_L:
            {
                value = read_regs(AP3216C_ALS_THRESHOLD_LOW_L_REG, 1);

                break;
            }
        case AP3216C_ALS_LOW_THRESHOLD_H:
            {
                value = read_regs(AP3216C_ALS_THRESHOLD_LOW_H_REG, 1);

                break;
            }
        case AP3216C_ALS_HIGH_THRESHOLD_L:
            {
                value = read_regs(AP3216C_ALS_THRESHOLD_HIGH_L_REG, 1);

                break;
            }
        case AP3216C_ALS_HIGH_THRESHOLD_H:
            {
                value = read_regs(AP3216C_ALS_THRESHOLD_HIGH_H_REG, 1);

                break;
            }
        case AP3216C_PS_GAIN:
            {
                var temp = read_regs(AP3216C_PS_CONFIGURATION_REG, 1);

                value = (temp & 0xc) >> 2;

                break;
            }
        case AP3216C_PS_PERSIST:
            {
                var temp = read_regs(AP3216C_PS_CONFIGURATION_REG, 1);

                value = temp & 0x3;

                break;
            }
        case AP3216C_PS_LOW_THRESHOLD_L:
            {
                value = read_regs(AP3216C_PS_THRESHOLD_LOW_L_REG, 1);

                break;
            }
        case AP3216C_PS_LOW_THRESHOLD_H:
            {
                value = read_regs(AP3216C_PS_THRESHOLD_LOW_H_REG, 1);
                break;
            }
        case AP3216C_PS_HIGH_THRESHOLD_L:
            {
               value = read_regs(AP3216C_PS_THRESHOLD_HIGH_L_REG, 1);

                break;
            }
        case AP3216C_PS_HIGH_THRESHOLD_H:
            {
                value = read_regs(AP3216C_PS_THRESHOLD_HIGH_H_REG, 1);

                break;
            }

        default:
            {
            }
    }
    return value;
}

 // 初始化MPU6050
// 返回值:0,成功
// 其他,错误代码
function init(i2cid) {

    ap3216cDev = i2c.open({id: i2cid});
    fakeSleep(100);

    /* reset ap3216c */
    reset_sensor();
    fakeSleep(100);
    ap3216c_set_param(AP3216C_SYSTEM_MODE, AP3216C_MODE_ALS_AND_PS);
    fakeSleep(150); // delay at least 112.5ms

    ap3216c_int_Config();
    ap3216c_int_init();

}

// De-init qlc5883
function deinit() {
    ap3216cDev.close();
}

module.exports = {
    init,
    deinit,
    ap3216c_read_ambient_light,
    ap3216c_read_ps_data,
    ap3216c_read_ir_data,
    deinit,
}