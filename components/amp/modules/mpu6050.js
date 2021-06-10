// The driver for mpu6050 chip, it is a temperature and humidity sensor.

/*
添加如下配置到app.json中：
        "mpu6050": {
            "type": "I2C",
            "port": 1,
            "addrWidth": 7,
            "freq": 100000,
            "mode": "master",
            "devAddr": 105
        }
*/
// Require libjs/lib/i2c.js module.

const MPU_SELF_TESTX_REG = 0X0D
// 自检寄存器X
const MPU_SELF_TESTY_REG = 0X0E
// 自检寄存器Y
const MPU_SELF_TESTZ_REG = 0X0F
// 自检寄存器Z
const MPU_SELF_TESTA_REG = 0X10
// 自检寄存器A
const MPU_SAMPLE_RATE_REG = 0X19
// 采样频率分频器
const MPU_CFG_REG         = 0X1A
// 配置寄存器
const MPU_GYRO_CFG_REG    = 0X1B
// 陀螺仪配置寄存器
const MPU_ACCEL_CFG_REG   = 0X1C
// 加速度计配置寄存器
const MPU_MOTION_DET_REG  = 0X1F
// 运动检测阀值设置寄存器
const MPU_FIFO_EN_REG     = 0X23
// FIFO使能寄存器
const MPU_I2CMST_CTRL_REG = 0X24
// IIC主机控制寄存器
const MPU_I2CSLV0_ADDR_REG = 0X25
// IIC从机0器件地址寄存器
const MPU_I2CSLV0_REG      = 0X26
// IIC从机0数据地址寄存器
const MPU_I2CSLV0_CTRL_REG = 0X27
// IIC从机0控制寄存器
const MPU_I2CSLV1_ADDR_REG = 0X28
// IIC从机1器件地址寄存器
const MPU_I2CSLV1_REG      = 0X29
// IIC从机1数据地址寄存器
const MPU_I2CSLV1_CTRL_REG = 0X2A
// IIC从机1控制寄存器
const MPU_I2CSLV2_ADDR_REG = 0X2B
// IIC从机2器件地址寄存器
const MPU_I2CSLV2_REG      = 0X2C
// IIC从机2数据地址寄存器
const MPU_I2CSLV2_CTRL_REG = 0X2D
// IIC从机2控制寄存器
const MPU_I2CSLV3_ADDR_REG = 0X2E
// IIC从机3器件地址寄存器
const MPU_I2CSLV3_REG      = 0X2F
// IIC从机3数据地址寄存器
const MPU_I2CSLV3_CTRL_REG = 0X30
// IIC从机3控制寄存器
const MPU_I2CSLV4_ADDR_REG = 0X31
// IIC从机4器件地址寄存器
const MPU_I2CSLV4_REG      = 0X32
// IIC从机4数据地址寄存器
const MPU_I2CSLV4_DO_REG   = 0X33
// IIC从机4写数据寄存器
const MPU_I2CSLV4_CTRL_REG = 0X34
// IIC从机4控制寄存器
const MPU_I2CSLV4_DI_REG   = 0X35
// IIC从机4读数据寄存器

const MPU_I2CMST_STA_REG = 0X36
// IIC主机状态寄存器
const MPU_INTBP_CFG_REG  = 0X37
// 中断/旁路设置寄存器
const MPU_INT_EN_REG     = 0X38
// 中断使能寄存器
const MPU_INT_STA_REG    = 0X3A
// 中断状态寄存器

const MPU_ACCEL_XOUTH_REG = 0X3B
// 加速度值,X轴高8位寄存器
const MPU_ACCEL_XOUTL_REG = 0X3C
// 加速度值,X轴低8位寄存器
const MPU_ACCEL_YOUTH_REG = 0X3D
// 加速度值,Y轴高8位寄存器
const MPU_ACCEL_YOUTL_REG = 0X3E
// 加速度值,Y轴低8位寄存器
const MPU_ACCEL_ZOUTH_REG = 0X3F
// 加速度值,Z轴高8位寄存器
const MPU_ACCEL_ZOUTL_REG = 0X40
// 加速度值,Z轴低8位寄存器

const MPU_TEMP_OUTH_REG = 0X41
// 温度值高八位寄存器
const MPU_TEMP_OUTL_REG = 0X42
// 温度值低8位寄存器

const MPU_GYRO_XOUTH_REG = 0X43
// 陀螺仪值,X轴高8位寄存器
const MPU_GYRO_XOUTL_REG = 0X44
// 陀螺仪值,X轴低8位寄存器
const MPU_GYRO_YOUTH_REG = 0X45
// 陀螺仪值,Y轴高8位寄存器
const MPU_GYRO_YOUTL_REG = 0X46
// 陀螺仪值,Y轴低8位寄存器
const MPU_GYRO_ZOUTH_REG = 0X47
// 陀螺仪值,Z轴高8位寄存器
const MPU_GYRO_ZOUTL_REG = 0X48
// 陀螺仪值,Z轴低8位寄存器

const MPU_I2CSLV0_DO_REG = 0X63
// IIC从机0数据寄存器
const MPU_I2CSLV1_DO_REG = 0X64
// IIC从机1数据寄存器
const MPU_I2CSLV2_DO_REG = 0X65
// IIC从机2数据寄存器
const MPU_I2CSLV3_DO_REG = 0X66
// IIC从机3数据寄存器

const MPU_I2CMST_DELAY_REG = 0X67
// IIC主机延时管理寄存器
const MPU_SIGPATH_RST_REG  = 0X68
// 信号通道复位寄存器
const MPU_MDETECT_CTRL_REG = 0X69
// 运动检测控制寄存器
const MPU_USER_CTRL_REG    = 0X6A
// 用户控制寄存器
const MPU_PWR_MGMT1_REG    = 0X6B
// 电源管理寄存器1
const MPU_PWR_MGMT2_REG    = 0X6C
// 电源管理寄存器2
const MPU_FIFO_CNTH_REG    = 0X72
// FIFO计数寄存器高八位
const MPU_FIFO_CNTL_REG    = 0X73
// FIFO计数寄存器低八位
const MPU_FIFO_RW_REG      = 0X74
// FIFO读写寄存器
const MPU_DEVICE_ID_REG    = 0X75
// 器件ID寄存器

// 如果AD0脚(9脚)接地,IIC地址为0X68(不包含最低位).
// 如果接V3.3,则IIC地址为0X69(不包含最低位).
const MPU_I2C_PORT =  0x1
const MPU_ADDR     = 0X69
const MPU_DEV_ID   = 0x68

var i2c = require('i2c');

var mpu6050Dev;

/*
duktape engine doesn't support async/await to sleep.
function sleep(ms) {
    return new Promise(function (resolve, reject) {
        setTimeout(function () {
            resolve();
        }, ms);
    });
}
*/

function fakeSleep(ms) {
    var date = new Date();
    var curDate;
    do {
        curDate = new Date();
    } while((curDate - date) < ms);
}

function i2c_eeprom_write_byte(addr, value) {
    var Reg = [addr, value];
    mpu6050Dev.write(Reg);
    console.log("--> write addr " + addr + ", value = " + Reg);
}

function i2c_eeprom_read_byte(addr) {
    var Reg = [];
    Reg[0] = addr;
    mpu6050Dev.write(Reg);
    value = mpu6050Dev.read(1);
    console.log("<-- read addr " + addr + ", value = " + value);
    return value;
}

function i2c_eeprom_write_len(addr, len, data) {
    var data = [];
    var reg = [addr];
    mpu6050Dev.write(reg);

    for (i = 0; i < len; i++) {
        mpu6050Dev.write(data[i]);
    }
    console.log("--> write addr " + addr + ", " + len + " bytes value = " + data);
}

function i2c_eeprom_read_len(addr, len) {
    var data = [];
    var reg = [addr];
    mpu6050Dev.write(reg);

    for (i = 0; i < len; i++) {
        data[i] = mpu6050Dev.read(1);
    }
    console.log("--> read addr " + addr + ", " + len + " bytes value = " + data);
    return data;
}

// 设置MPU6050陀螺仪传感器满量程范围
// fsr:0,±250dps;1,±500dps;2,±1000dps;3,±2000dps
// 返回值:0,设置成功
// 其他,设置失败
function MPU_Set_Gyro_Fsr(fsr)
{
    return i2c_eeprom_write_byte(MPU_GYRO_CFG_REG, fsr << 3); // 设置陀螺仪满量程范围
}

// 设置MPU6050加速度传感器满量程范围
// fsr:0,±2g;1,±4g;2,±8g;3,±16g
// 返回值:0,设置成功
// 其他,设置失败
function MPU_Set_Accel_Fsr(fsr)
{
    return i2c_eeprom_write_byte(MPU_ACCEL_CFG_REG, fsr << 3); // 设置加速度传感器满量程范围
}

// 设置MPU6050的数字低通滤波器
// lpf:数字低通滤波频率(Hz)
// 返回值:0,设置成功
// 其他,设置失败
function MPU_Set_LPF(lpf)
{
    var data = 0;
    if (lpf >= 188)
        data = 1;
    else if (lpf >= 98)
        data = 2;
    else if (lpf >= 42)
        data = 3;
    else if (lpf >= 20)
        data = 4;
    else if (lpf >= 10)
        data = 5;
    else
        data = 6;
    return i2c_eeprom_write_byte(MPU_CFG_REG, data); // 设置数字低通滤波器
}

// 设置MPU6050的采样率(假定Fs=1KHz)
// rate:4~1000(Hz)
// 返回值:0,设置成功
// 其他,设置失败
function MPU_Set_Rate(rate)
{
    var data;
    if (rate > 1000)
        rate = 1000;
    if (rate < 4)
        rate = 4;
    data = 1000 / rate - 1;
    i2c_eeprom_write_byte(MPU_SAMPLE_RATE_REG, data); // 设置数字低通滤波器
    return MPU_Set_LPF(rate / 2); // 自动设置LPF为采样率的一半
}


// 得到温度值
// 返回值:温度值(扩大了100倍)
function get_Temperature()
{
    var buf = [];
    var raw;
    var temp;

    buf = i2c_eeprom_read_len(MPU_TEMP_OUTH_REG, 2);
    raw  = (buf[0] << 8) | buf[1];
    if (raw > (1 << 15)) {
        raw = raw - (1<<16)
    }
    temp = 36.53 + (raw) / 340;
    return temp * 100;
}

// 得到陀螺仪值(原始值)
// gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
// 返回值:0,成功
// 其他,错误代码
function get_Gyroscope(gx, gy, gz)
{
    var buf = [];
    var arr = [];

    buf = i2c_eeprom_read_len(MPU_GYRO_XOUTH_REG, 6);
    gx = (buf[0] << 8) | buf[1];
    gy = (buf[2] << 8) | buf[3];
    gz = (buf[4] << 8) | buf[5];
    if (gx > (1 << 15)) {
        gx = gx - (1<<16)
    }
    if (gy > (1 << 15)) {
        gy = gy - (1<<16)
    }
    if (gz > (1 << 15)) {
        gz = gz - (1<<16)
    }
    arr[0] = gx;
    arr[1] = gy;
    arr[2] = gz;

    return arr;
}
// 得到加速度值(原始值)
// gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
// 返回值:0,成功
// 其他,错误代码
function get_Accelerometer(ax, ay, az)
{
    var buf = [];
    var arr = [];

    buf = i2c_eeprom_read_len(MPU_ACCEL_XOUTH_REG, 6);
    ax = (buf[0] << 8) | buf[1];
    ay = (buf[2] << 8) | buf[3];
    az = (buf[4] << 8) | buf[5];
    if (ax > (1 << 15)) {
        ax = ax - (1<<16)
    }
    if (ay > (1 << 15)) {
        ay = ay - (1<<16)
    }
    if (az > (1 << 15)) {
        az = az - (1<<16)
    }

    arr[0] = ax;
    arr[1] = ay;
    arr[2] = az;

    return arr;
}

 // 初始化MPU6050
// 返回值:0,成功
// 其他,错误代码
function init(i2cid) {

    var device_id = 0;

    mpu6050Dev = i2c.open({id: i2cid});
    fakeSleep(30);

    i2c_eeprom_write_byte(MPU_PWR_MGMT1_REG, 0X80); // 复位MPU6050
    fakeSleep(200);
    i2c_eeprom_write_byte(MPU_PWR_MGMT1_REG, 0X00); // 唤醒MPU6050
    MPU_Set_Gyro_Fsr(3);                     // 陀螺仪传感器,±2000dps
    MPU_Set_Accel_Fsr(0);                    // 加速度传感器,±2g
    MPU_Set_Rate(50);                        // 设置采样率50Hz
    i2c_eeprom_write_byte(MPU_INT_EN_REG, 0X00);    // 关闭所有中断
    i2c_eeprom_write_byte(MPU_USER_CTRL_REG, 0X00); // I2C主模式关闭
    i2c_eeprom_write_byte(MPU_FIFO_EN_REG, 0X00);   // 关闭FIFO
    i2c_eeprom_write_byte(MPU_INTBP_CFG_REG, 0X80); // INT引脚低电平有效
    device_id = i2c_eeprom_read_byte(MPU_DEVICE_ID_REG);
    if (device_id == MPU_DEV_ID) {
        // 器件ID正确
        i2c_eeprom_write_byte(MPU_PWR_MGMT1_REG, 0X01); // 设置CLKSEL,PLL X轴为参考
        i2c_eeprom_write_byte(MPU_PWR_MGMT2_REG, 0X00); // 加速度与陀螺仪都工作
        MPU_Set_Rate(50);                        // 设置采样率为50Hz
    } else {
        return 1;
    }
    return 0;
}

// De-init qlc5883
function deinit() {
    mpu6050Dev.close();
}

module.exports = {
    init,
    deinit,
    get_Temperature,
    get_Gyroscope,
    get_Accelerometer,
    deinit,
}
