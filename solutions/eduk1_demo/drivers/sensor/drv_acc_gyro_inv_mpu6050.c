/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "drv_acc_gyro_inv_mpu6050.h"
#include "aos/hal/i2c.h"
#include "aos/kernel.h"
#include "hal_iomux_haas1000.h"
#include "ulog/ulog.h"
#include <stdio.h>
#include <stdlib.h>

static i2c_dev_t i2c_dev;

// wtite one byte to register
uint8_t MPU_Write_Byte(uint8_t reg, uint8_t data)
{
    uint8_t *write_buf[2];

    write_buf[0] = reg;
    write_buf[1] = data;
    sensor_i2c_master_send(MPU_I2C_PORT, MPU_ADDR, write_buf, 2, 1000);
    return 0;
}

// read byte from register
uint8_t MPU_Read_Byte(uint8_t reg)
{
    uint8_t data;

    sensor_i2c_master_send(MPU_I2C_PORT, MPU_ADDR, &reg, 1, 1000);
    aos_msleep(2);
    sensor_i2c_master_recv(MPU_I2C_PORT, MPU_ADDR, &data, 1, 1000);
    return data;
}

// write nbytes to register
void MPU_Write_Len(uint8_t reg, uint8_t len, uint8_t *buf)
{
    uint8_t *write_buf[100] = {0};

    if ((len > 100) || (buf == NULL))
        return;

    write_buf[0] = reg;
    memcpy(&write_buf[1], buf, len);
    sensor_i2c_master_send(MPU_I2C_PORT, MPU_ADDR, &write_buf, len + 1, 1000);
}

// read nbytes from register
void MPU_Read_Len(uint8_t reg, uint8_t len, uint8_t *buf)
{
    sensor_i2c_master_send(MPU_I2C_PORT, MPU_ADDR, &reg, 1, 1000);
    aos_msleep(2);
    sensor_i2c_master_recv(MPU_I2C_PORT, MPU_ADDR, buf, len, 1000);
}

// 设置MPU6050陀螺仪传感器满量程范围
// fsr:0,±250dps;1,±500dps;2,±1000dps;3,±2000dps
// 返回值:0,设置成功
// 其他,设置失败
uint8_t MPU_Set_Gyro_Fsr(uint8_t fsr)
{
    return MPU_Write_Byte(MPU_GYRO_CFG_REG, fsr << 3); // 设置陀螺仪满量程范围
}
// 设置MPU6050加速度传感器满量程范围
// fsr:0,±2g;1,±4g;2,±8g;3,±16g
// 返回值:0,设置成功
// 其他,设置失败
uint8_t MPU_Set_Accel_Fsr(uint8_t fsr)
{
    return MPU_Write_Byte(MPU_ACCEL_CFG_REG,
                          fsr << 3); // 设置加速度传感器满量程范围
}
// 设置MPU6050的数字低通滤波器
// lpf:数字低通滤波频率(Hz)
// 返回值:0,设置成功
// 其他,设置失败
uint8_t MPU_Set_LPF(u16 lpf)
{
    uint8_t data = 0;
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
    return MPU_Write_Byte(MPU_CFG_REG, data); // 设置数字低通滤波器
}
// 设置MPU6050的采样率(假定Fs=1KHz)
// rate:4~1000(Hz)
// 返回值:0,设置成功
// 其他,设置失败
uint8_t MPU_Set_Rate(u16 rate)
{
    uint8_t data;
    if (rate > 1000)
        rate = 1000;
    if (rate < 4)
        rate = 4;
    data = 1000 / rate - 1;
    MPU_Write_Byte(MPU_SAMPLE_RATE_REG, data); // 设置数字低通滤波器
    return MPU_Set_LPF(rate / 2); // 自动设置LPF为采样率的一半
}

// 初始化MPU6050
// 返回值:0,成功
// 其他,错误代码
uint8_t MPU_Init(void)
{
    uint8_t device_id = 0;

    int32_t ret = sensor_i2c_open (MPU_I2C_PORT, MPU_ADDR, I2C_BUS_BIT_RATES_100K, 0);
    if (ret) {
        LOGE("SENSOR", "sensor i2c open failed, ret:%d\n", ret);
        return -EIO;
    }

    MPU_Write_Byte(MPU_PWR_MGMT1_REG, 0X80); // 复位MPU6050
    aos_msleep(100);
    MPU_Write_Byte(MPU_PWR_MGMT1_REG, 0X00); // 唤醒MPU6050
    MPU_Set_Gyro_Fsr(3);                     // 陀螺仪传感器,±2000dps
    MPU_Set_Accel_Fsr(0);                    // 加速度传感器,±2g
    MPU_Set_Rate(50);                        // 设置采样率50Hz
    MPU_Write_Byte(MPU_INT_EN_REG, 0X00);    // 关闭所有中断
    MPU_Write_Byte(MPU_USER_CTRL_REG, 0X00); // I2C主模式关闭
    MPU_Write_Byte(MPU_FIFO_EN_REG, 0X00);   // 关闭FIFO
    MPU_Write_Byte(MPU_INTBP_CFG_REG, 0X80); // INT引脚低电平有效
    device_id = MPU_Read_Byte(MPU_DEVICE_ID_REG);
    if (device_id == MPU_DEV_ID) {
        // 器件ID正确
        LOGI("SENSOR", "MPU init OK\n");
        MPU_Write_Byte(MPU_PWR_MGMT1_REG, 0X01); // 设置CLKSEL,PLL X轴为参考
        MPU_Write_Byte(MPU_PWR_MGMT2_REG, 0X00); // 加速度与陀螺仪都工作
        MPU_Set_Rate(50);                        // 设置采样率为50Hz
    } else {
        LOGE("SENSOR", "MPU init Error -- %x\n", device_id);
        return 1;
    }
    return 0;
}

// 得到温度值
// 返回值:温度值(扩大了100倍)
short MPU_Get_Temperature(void)
{
    uint8_t buf[2];
    short   raw;
    float   temp;
    MPU_Read_Len(MPU_TEMP_OUTH_REG, 2, buf);
    raw  = ((u16)buf[0] << 8) | buf[1];
    temp = 36.53 + ((double)raw) / 340;
    return temp * 100;
    ;
}
// 得到陀螺仪值(原始值)
// gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
// 返回值:0,成功
// 其他,错误代码
void MPU_Get_Gyroscope(short *gx, short *gy, short *gz)
{
    uint8_t buf[6];

    MPU_Read_Len(MPU_GYRO_XOUTH_REG, 6, buf);
    *gx = ((u16)buf[0] << 8) | buf[1];
    *gy = ((u16)buf[2] << 8) | buf[3];
    *gz = ((u16)buf[4] << 8) | buf[5];
}
// 得到加速度值(原始值)
// gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
// 返回值:0,成功
// 其他,错误代码
void MPU_Get_Accelerometer(short *ax, short *ay, short *az)
{
    uint8_t buf[6];

    MPU_Read_Len(MPU_ACCEL_XOUTH_REG, 6, buf);
    *ax = ((u16)buf[0] << 8) | buf[1];
    *ay = ((u16)buf[2] << 8) | buf[3];
    *az = ((u16)buf[4] << 8) | buf[5];
}


void MPU_Deinit(void) {
    int32_t ret = sensor_i2c_close(MPU_I2C_PORT);
    if (ret) {
        LOGE("SENSOR", "sensor i2c close failed, ret:%d\n", ret);
    }

    return;
}