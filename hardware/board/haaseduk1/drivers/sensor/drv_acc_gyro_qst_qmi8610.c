
/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */
#include "drv_acc_gyro_qst_qmi8610.h"
#include "aos/kernel.h"
#include "ulog/ulog.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#define FIS210X_SLAVE_ADDR          0x6a
#define FISIMU_UINT_MG_DPS

enum {
    AXIS_X = 0,
    AXIS_Y = 1,
    AXIS_Z = 2,

    AXIS_TOTAL
};

typedef struct {
    short               sign[AXIS_TOTAL];
    unsigned short      map[AXIS_TOTAL];
} qst_imu_layout;

static uint16_t acc_lsb_div = 0;
static uint16_t gyro_lsb_div = 0;
static struct FisImuConfig fisimu_config;

uint8_t FisImu_write_reg(uint8_t reg, uint8_t value)
{
    uint8_t ret = 0;

#if defined(QST_USE_SPI)
    ret = qst_fis210x_spi_write(reg, value);
#elif defined(QST_USE_SW_I2C)
    ret = qst_sw_writereg(FIS210X_SLAVE_ADDR, reg, value);
#else
    uint8_t write_buf[2];
    write_buf[0] = reg;
    write_buf[1] = value;
    sensor_i2c_master_send(1, FIS210X_SLAVE_ADDR, write_buf, 2, 1000);
#endif
    return ret;
}

uint8_t FisImu_write_regs(uint8_t reg, uint8_t *value, uint8_t len)
{
    uint8_t ret = 0;
#if defined(QST_USE_SPI)
    ret = qst_fis210x_spi_write_bytes(reg, value, len);
#elif defined(QST_USE_SW_I2C)
    ret = qst_sw_writeregs(FIS210X_SLAVE_ADDR, reg, value, len);
#else
    sensor_i2c_mem_write(1, FIS210X_SLAVE_ADDR, reg, 1, &value, len, 100);
#endif
    return ret;
}

uint8_t FisImu_read_reg(uint8_t reg, uint8_t *buf, uint16_t len)
{
    uint8_t ret = 0;

#if defined(QST_USE_SPI)
    ret = qst_fis210x_spi_read(reg, buf, len);
#elif defined(QST_USE_SW_I2C)
    ret = qst_sw_readreg(FIS210X_SLAVE_ADDR, reg, buf, len);
#else
    sensor_i2c_mem_read(1, FIS210X_SLAVE_ADDR, reg, 1, buf, len, 100);
#endif
    return ret;
}


static qst_imu_layout imu_map;

void FisImu_set_layout(short layout)
{
    if (layout == 0) {
        imu_map.sign[AXIS_X] = 1;
        imu_map.sign[AXIS_Y] = 1;
        imu_map.sign[AXIS_Z] = 1;
        imu_map.map[AXIS_X] = AXIS_X;
        imu_map.map[AXIS_Y] = AXIS_Y;
        imu_map.map[AXIS_Z] = AXIS_Z;
    } else if (layout == 1) {
        imu_map.sign[AXIS_X] = -1;
        imu_map.sign[AXIS_Y] = 1;
        imu_map.sign[AXIS_Z] = 1;
        imu_map.map[AXIS_X] = AXIS_Y;
        imu_map.map[AXIS_Y] = AXIS_X;
        imu_map.map[AXIS_Z] = AXIS_Z;
    } else if (layout == 2) {
        imu_map.sign[AXIS_X] = -1;
        imu_map.sign[AXIS_Y] = -1;
        imu_map.sign[AXIS_Z] = 1;
        imu_map.map[AXIS_X] = AXIS_X;
        imu_map.map[AXIS_Y] = AXIS_Y;
        imu_map.map[AXIS_Z] = AXIS_Z;
    } else if (layout == 3) {
        imu_map.sign[AXIS_X] = 1;
        imu_map.sign[AXIS_Y] = -1;
        imu_map.sign[AXIS_Z] = 1;
        imu_map.map[AXIS_X] = AXIS_Y;
        imu_map.map[AXIS_Y] = AXIS_X;
        imu_map.map[AXIS_Z] = AXIS_Z;
    } else if (layout == 4) {
        imu_map.sign[AXIS_X] = -1;
        imu_map.sign[AXIS_Y] = 1;
        imu_map.sign[AXIS_Z] = -1;
        imu_map.map[AXIS_X] = AXIS_X;
        imu_map.map[AXIS_Y] = AXIS_Y;
        imu_map.map[AXIS_Z] = AXIS_Z;
    } else if (layout == 5) {
        imu_map.sign[AXIS_X] = 1;
        imu_map.sign[AXIS_Y] = 1;
        imu_map.sign[AXIS_Z] = -1;
        imu_map.map[AXIS_X] = AXIS_Y;
        imu_map.map[AXIS_Y] = AXIS_X;
        imu_map.map[AXIS_Z] = AXIS_Z;
    } else if (layout == 6) {
        imu_map.sign[AXIS_X] = 1;
        imu_map.sign[AXIS_Y] = -1;
        imu_map.sign[AXIS_Z] = -1;
        imu_map.map[AXIS_X] = AXIS_X;
        imu_map.map[AXIS_Y] = AXIS_Y;
        imu_map.map[AXIS_Z] = AXIS_Z;
    } else if (layout == 7) {
        imu_map.sign[AXIS_X] = -1;
        imu_map.sign[AXIS_Y] = -1;
        imu_map.sign[AXIS_Z] = -1;
        imu_map.map[AXIS_X] = AXIS_Y;
        imu_map.map[AXIS_Y] = AXIS_X;
        imu_map.map[AXIS_Z] = AXIS_Z;
    } else {
        imu_map.sign[AXIS_X] = 1;
        imu_map.sign[AXIS_Y] = 1;
        imu_map.sign[AXIS_Z] = 1;
        imu_map.map[AXIS_X] = AXIS_X;
        imu_map.map[AXIS_Y] = AXIS_Y;
        imu_map.map[AXIS_Z] = AXIS_Z;
    }
}


void FisImu_config_acc(enum FisImu_AccRange range, enum FisImu_AccOdr odr, enum FisImu_LpfConfig lpfEnable, enum FisImu_HpfConfig hpfEnable)
{
    unsigned char ctl_dada;
    unsigned char range_set;

    switch (range) {
        case AccRange_2g:
            range_set = 0 << 3;
            acc_lsb_div = (1 << 14);
            break;
        case AccRange_4g:
            range_set = 1 << 3;
            acc_lsb_div = (1 << 13);
            break;
        case AccRange_8g:
            range_set = 2 << 3;
            acc_lsb_div = (1 << 12);
            break;
        default:
            range_set = 2 << 3;
            acc_lsb_div = (1 << 12);
    }
    ctl_dada = (unsigned char)range_set | (unsigned char)odr;
    FisImu_write_reg(FisRegister_Ctrl2, ctl_dada);
    //  set LPF & HPF
    FisImu_read_reg(FisRegister_Ctrl5, &ctl_dada, 1);
    ctl_dada &= 0xfc;
    if (lpfEnable == Lpf_Enable)
        ctl_dada |= 0x02;
    if (hpfEnable == Hpf_Enable)
        ctl_dada |= 0x01;
    FisImu_write_reg(FisRegister_Ctrl5, ctl_dada);
    //  set LPF & HPF
}

void FisImu_config_gyro(enum FisImu_GyrRange range, enum FisImu_GyrOdr odr, enum FisImu_LpfConfig lpfEnable, enum FisImu_HpfConfig hpfEnable)
{
    //  Set the CTRL3 register to configure dynamic range and ODR
    unsigned char ctl_dada;
    ctl_dada = (unsigned char)range | (unsigned char)odr;
    FisImu_write_reg(FisRegister_Ctrl3, ctl_dada);

    // Store the scale factor for use when processing raw data
    switch (range) {
        case GyrRange_32dps:
            gyro_lsb_div = 1024;
            break;
        case GyrRange_64dps:
            gyro_lsb_div = 512;
            break;
        case GyrRange_128dps:
            gyro_lsb_div = 256;
            break;
        case GyrRange_256dps:
            gyro_lsb_div = 128;
            break;
        case GyrRange_512dps:
            gyro_lsb_div = 64;
            break;
        case GyrRange_1024dps:
            gyro_lsb_div = 32;
            break;
        case GyrRange_2048dps:
            gyro_lsb_div = 16;
            break;
        case GyrRange_2560dps:
            // gyro_lsb_div = 8;
            break;
        default:
            gyro_lsb_div = 32;
            break;
    }

    //  Conversion from degrees/s to rad/s if necessary
    //  set LPF & HPF
    FisImu_read_reg(FisRegister_Ctrl5, &ctl_dada, 1);
    ctl_dada &= 0xf3;
    if (lpfEnable == Lpf_Enable)
        ctl_dada |= 0x08;
    if (hpfEnable == Hpf_Enable)
        ctl_dada |= 0x04;
    FisImu_write_reg(FisRegister_Ctrl5, ctl_dada);
    //  set LPF & HPF
}

void FisImu_config_ae(enum FisImu_AeOdr odr)
{
    // Configure Accelerometer and Gyroscope settings
    FisImu_config_acc(AccRange_8g, AccOdr_1024Hz, Lpf_Enable, Hpf_Disable);
    FisImu_config_gyro(GyrRange_2048dps, GyrOdr_1024Hz, Lpf_Enable, Hpf_Disable);
    FisImu_write_reg(FisRegister_Ctrl6, odr);
}

void FisImu_config_mag(enum FisImu_MagDev device, enum FisImu_MagOdr odr)
{

}

uint8_t FisImu_readStatus0(void)
{
    uint8_t status;
    FisImu_read_reg(FisRegister_Status0, &status, sizeof(status));
    return status;
}
/*!
 * \brief Blocking read of data status register 1 (::FisRegister_Status1).
 * \returns Status byte \see STATUS1 for flag definitions.
 */
uint8_t FisImu_readStatus1(void)
{
    uint8_t status;
    FisImu_read_reg(FisRegister_Status1, &status, sizeof(status));
    return status;
}

int8_t FisImu_readTemp(void)
{
    int8_t temp;
    FisImu_read_reg(FisRegister_Temperature, (uint8_t *)&temp, sizeof(temp));
    return temp;
}

void FisImu_read_acc_xyz(float acc_xyz[3])
{
    unsigned char   buf_reg[6];
    short           raw_acc_xyz[3];

#if defined(QST_USE_SPI)
    FisImu_read_reg(FisRegister_Ax_L, &buf_reg[0], 6);  // 0x19, 25
#else
    FisImu_read_reg(FisRegister_Ax_L, &buf_reg[0], 1);      // 0x19, 25
    FisImu_read_reg(FisRegister_Ax_H, &buf_reg[1], 1);
    FisImu_read_reg(FisRegister_Ay_L, &buf_reg[2], 1);
    FisImu_read_reg(FisRegister_Ay_H, &buf_reg[3], 1);
    FisImu_read_reg(FisRegister_Az_L, &buf_reg[4], 1);
    FisImu_read_reg(FisRegister_Az_H, &buf_reg[5], 1);
#endif
    raw_acc_xyz[0] = (short)((buf_reg[1] << 8) | (buf_reg[0]));
    raw_acc_xyz[1] = (short)((buf_reg[3] << 8) | (buf_reg[2]));
    raw_acc_xyz[2] = (short)((buf_reg[5] << 8) | (buf_reg[4]));
    //  if(cali_flag == FALSE)
    //  {
    //      raw_acc_xyz[0]+=offset[0];
    //      raw_acc_xyz[1]+=offset[1];
    //      raw_acc_xyz[2]+=offset[2];
    //  }

    acc_xyz[0] = (raw_acc_xyz[0] * ONE_G) / acc_lsb_div;
    acc_xyz[1] = (raw_acc_xyz[1] * ONE_G) / acc_lsb_div;
    acc_xyz[2] = (raw_acc_xyz[2] * ONE_G) / acc_lsb_div;
    //  acc_layout = 0;
    //  acc_xyz[qst_map[acc_layout].map[0]] = qst_map[acc_layout].sign[0]*acc_xyz[0];
    //  acc_xyz[qst_map[acc_layout].map[1]] = qst_map[acc_layout].sign[1]*acc_xyz[1];
    //  acc_xyz[qst_map[acc_layout].map[2]] = qst_map[acc_layout].sign[2]*acc_xyz[2];
    //  LOGD("SENSOR", "fis210x acc:    %f  %f  %f\n", acc_xyz[0], acc_xyz[1], acc_xyz[2]);
}

void FisImu_read_gyro_xyz(float gyro_xyz[3])
{
    unsigned char   buf_reg[6];
    short           raw_gyro_xyz[3];

#if defined(QST_USE_SPI)
    FisImu_read_reg(FisRegister_Gx_L, &buf_reg[0], 6);  // 0x19, 25
#else
    FisImu_read_reg(FisRegister_Gx_L, &buf_reg[0], 1);
    FisImu_read_reg(FisRegister_Gx_H, &buf_reg[1], 1);
    FisImu_read_reg(FisRegister_Gy_L, &buf_reg[2], 1);
    FisImu_read_reg(FisRegister_Gy_H, &buf_reg[3], 1);
    FisImu_read_reg(FisRegister_Gz_L, &buf_reg[4], 1);
    FisImu_read_reg(FisRegister_Gz_H, &buf_reg[5], 1);
#endif
    raw_gyro_xyz[0] = (short)((buf_reg[1] << 8) | (buf_reg[0]));
    raw_gyro_xyz[1] = (short)((buf_reg[3] << 8) | (buf_reg[2]));
    raw_gyro_xyz[2] = (short)((buf_reg[5] << 8) | (buf_reg[4]));
    //  if (cali_flag == FALSE)
    //  {
    //      raw_gyro_xyz[0] += offset_gyro[0];
    //      raw_gyro_xyz[1] += offset_gyro[1];
    //      raw_gyro_xyz[2] += offset_gyro[2];
    //  }

    gyro_xyz[0] = (raw_gyro_xyz[0] * 1.0f) / gyro_lsb_div;
    gyro_xyz[1] = (raw_gyro_xyz[1] * 1.0f) / gyro_lsb_div;
    gyro_xyz[2] = (raw_gyro_xyz[2] * 1.0f) / gyro_lsb_div;
    //  acc_layout = 0;
    //  gyro_xyz[qst_map[acc_layout].map[0]] = qst_map[acc_layout].sign[0]*gyro_xyz[0];
    //  gyro_xyz[qst_map[acc_layout].map[1]] = qst_map[acc_layout].sign[1]*gyro_xyz[1];
    //  gyro_xyz[qst_map[acc_layout].map[2]] = qst_map[acc_layout].sign[2]*gyro_xyz[2];
    LOGD("SENSOR", "fis210x gyro: %f %f %f\n", gyro_xyz[0], gyro_xyz[1], gyro_xyz[2]);
}

void FisImu_read_xyz(float acc[3], float gyro[3])
{
    unsigned char   buf_reg[12];
    short           raw_acc_xyz[3];
    short           raw_gyro_xyz[3];
    float acc_t[3];
    float gyro_t[3];

#if defined(QST_USE_SPI)
    if (tim_count) {
        FisImu_read_reg(FisRegister_CountOut, tim_count, 1);    // 0x18 24
    }
    FisImu_read_reg(FisRegister_Ax_L, &buf_reg[0], 12);     // 0x19, 25
#else
    FisImu_read_reg(FisRegister_Ax_L | 0x80, buf_reg, 12); // 0x19, 25
#endif
    raw_acc_xyz[0] = (short)((buf_reg[1] << 8) | (buf_reg[0]));
    raw_acc_xyz[1] = (short)((buf_reg[3] << 8) | (buf_reg[2]));
    raw_acc_xyz[2] = (short)((buf_reg[5] << 8) | (buf_reg[4]));


    raw_gyro_xyz[0] = (short)((buf_reg[7] << 8) | (buf_reg[6]));
    raw_gyro_xyz[1] = (short)((buf_reg[9] << 8) | (buf_reg[8]));
    raw_gyro_xyz[2] = (short)((buf_reg[11] << 8) | (buf_reg[10]));

    // m/s2
    acc_t[AXIS_X] = (float)(raw_acc_xyz[AXIS_X] * ONE_G) / acc_lsb_div;
    acc_t[AXIS_Y] = (float)(raw_acc_xyz[AXIS_Y] * ONE_G) / acc_lsb_div;
    acc_t[AXIS_Z] = (float)(raw_acc_xyz[AXIS_Z] * ONE_G) / acc_lsb_div;

    acc[AXIS_X] = -acc_t[AXIS_X]; // imu_map.sign[AXIS_X]*acc_t[imu_map.map[AXIS_X]];
    acc[AXIS_Y] = -acc_t[AXIS_Y]; // imu_map.sign[AXIS_Y]*acc_t[imu_map.map[AXIS_Y]];
    acc[AXIS_Z] = acc_t[AXIS_Z]; // imu_map.sign[AXIS_Z]*acc_t[imu_map.map[AXIS_Z]];

    //  rad/s
    gyro_t[AXIS_X] = (float)(raw_gyro_xyz[AXIS_X] * M_PI / 180) / gyro_lsb_div; //  *pi/180
    gyro_t[AXIS_Y] = (float)(raw_gyro_xyz[AXIS_Y] * M_PI / 180) / gyro_lsb_div;
    gyro_t[AXIS_Z] = (float)(raw_gyro_xyz[AXIS_Z] * M_PI / 180) / gyro_lsb_div;

    gyro[AXIS_X] = gyro_t[AXIS_X]; // imu_map.sign[AXIS_X]*gyro_t[imu_map.map[AXIS_X]];
    gyro[AXIS_Y] = gyro_t[AXIS_Y]; // imu_map.sign[AXIS_Y]*gyro_t[imu_map.map[AXIS_Y]];
    gyro[AXIS_Z] = -gyro_t[AXIS_Z]; // imu_map.sign[AXIS_Z]*gyro_t[imu_map.map[AXIS_Z]];
}

// for XKF3
static inline void applyScaleFactor(float scaleFactor, uint8_t nElements, uint8_t const *rawData, float *calibratedData)
{
    for (int i = 0; i < nElements; ++i) {
        calibratedData[i] = scaleFactor * (int16_t)((uint16_t)rawData[2 * i] | ((uint16_t)rawData[2 * i + 1] << 8));
    }
}

void FisImu_processAccelerometerData(uint8_t const *rawData, float *calibratedData)
{
    applyScaleFactor((float)(ONE_G / acc_lsb_div), 3, rawData, calibratedData);
}

void FisImu_processGyroscopeData(uint8_t const *rawData, float *calibratedData)
{
    applyScaleFactor((float)(M_PI / (gyro_lsb_div * 180)), 3, rawData, calibratedData);
}

void FisImu_read_rawsample(struct FisImuRawSample *sample)
{
    FisImu_read_reg(FisRegister_CountOut, &(sample->sampleCounter), sizeof(uint8_t));
    sample->accelerometerData = sample->sampleBuffer;
    sample->gyroscopeData = sample->sampleBuffer + FISIMU_SAMPLE_SIZE;
    FisImu_read_reg(FisRegister_Ax_L, (uint8_t *)sample->accelerometerData, FISIMU_SAMPLE_SIZE);
    FisImu_read_reg(FisRegister_Gx_L, (uint8_t *)sample->gyroscopeData, FISIMU_SAMPLE_SIZE);
}

static void writeCalibrationVectorBuffer(float const *calVector, float conversionFactor, uint8_t fractionalBits)
{
    int i;
    int16_t o;
    uint8_t calCmd[6];
    // calCmd[0] = FisRegister_Cal1_L;

    for (i = 0; i < 3; ++i) {
        o = (int16_t)roundf(calVector[i] * conversionFactor * (1 << fractionalBits));
        calCmd[(2 * i)] = o & 0xFF;
        calCmd[(2 * i) + 1] = o >> 8;
    }
    FisImu_write_regs(FisRegister_Cal1_L, calCmd, sizeof(calCmd));
}

void FisImu_doCtrl9Command(enum FisImu_Ctrl9Command cmd)
{
    uint8_t gyroConfig;
    const uint8_t oisModeBits = 0x06;
    unsigned char oisEnabled;
    uint8_t status = 0;
    uint32_t count = 0;

    FisImu_read_reg(FisRegister_Ctrl3, &gyroConfig, sizeof(gyroConfig));
    oisEnabled = ((gyroConfig & oisModeBits) == oisModeBits);
    if (oisEnabled) {
        FisImu_write_reg(FisRegister_Ctrl3, (gyroConfig & ~oisModeBits));
    }

    // g_fisDriverHal->waitForEvent(Fis_Int1, FisInt_low);
    // aos_msleep(300);

    FisImu_write_reg(FisRegister_Ctrl9, cmd);
    // g_fisDriverHal->waitForEvent(Fis_Int1, FisInt_high);
    // aos_msleep(300);

    // Check that command has been executed
    while (((status & FISIMU_STATUS1_CMD_DONE) == 0) && (count < 10000)) {
        FisImu_read_reg(FisRegister_Status1, &status, sizeof(status));
        count++;
    }
    // assert(status & FISIMU_STATUS1_CMD_DONE);

    // g_fisDriverHal->waitForEvent(Fis_Int1, FisInt_low);
    // aos_msleep(300);

    if (oisEnabled) {
        // Re-enable OIS mode configuration if necessary
        FisImu_write_reg(FisRegister_Ctrl3, gyroConfig);
    }
}

void FisImu_applyAccelerometerOffset(float const *offset, enum FisImu_AccUnit unit)
{
    const float conversionFactor = (unit == AccUnit_ms2) ? (1 / ONE_G) : 1;
    writeCalibrationVectorBuffer(offset, conversionFactor, 11);
    FisImu_doCtrl9Command(Ctrl9_SetAccelOffset);
}

void FisImu_applyGyroscopeOffset(float const *offset, enum FisImu_GyrUnit unit)
{
    const float conversionFactor = (unit == GyrUnit_rads) ? 180 / M_PI : 1;
    writeCalibrationVectorBuffer(offset, conversionFactor, 6);
    FisImu_doCtrl9Command(Ctrl9_SetGyroOffset);
}

void FisImu_applyOffsetCalibration(struct FisImu_offsetCalibration const *cal)
{
    FisImu_applyAccelerometerOffset(cal->accOffset, cal->accUnit);
    FisImu_applyGyroscopeOffset(cal->gyrOffset, cal->gyrUnit);
}

// for XKF3

void FisImu_enableWakeOnMotion(void)
{
    uint8_t womCmd[3];
    enum FisImu_Interrupt interrupt = Fis_Int1;
    enum FisImu_InterruptInitialState initialState = InterruptInitialState_low;
    enum FisImu_WakeOnMotionThreshold threshold = WomThreshold_low;
    uint8_t blankingTime = 0x00;
    const uint8_t blankingTimeMask = 0x3F;

    FisImu_enableSensors(FISIMU_CTRL7_DISABLE_ALL);
    // FisImu_config_acc(AccRange_2g, AccOdr_LowPower_3Hz, Lpf_Disable, Hpf_Disable);
    FisImu_config_acc(AccRange_2g, AccOdr_LowPower_25Hz, Lpf_Disable, Hpf_Disable);

    womCmd[0] = FisRegister_Cal1_L;     // WoM Threshold: absolute value in mg (with 1mg/LSB resolution)
    womCmd[1] = threshold;
    womCmd[2] = (uint8_t)interrupt | (uint8_t)initialState | (blankingTime & blankingTimeMask);
    FisImu_write_reg(FisRegister_Cal1_L, womCmd[1]);
    FisImu_write_reg(FisRegister_Cal1_H, womCmd[2]);

    FisImu_doCtrl9Command(Ctrl9_ConfigureWakeOnMotion);
    FisImu_enableSensors(FISIMU_CTRL7_ACC_ENABLE);
}

void FisImu_disableWakeOnMotion(void)
{
    FisImu_enableSensors(FISIMU_CTRL7_DISABLE_ALL);
    FisImu_write_reg(FisRegister_Cal1_L, 0);
    FisImu_doCtrl9Command(Ctrl9_ConfigureWakeOnMotion);
}

void FisImu_enableSensors(uint8_t enableFlags)
{
    if (enableFlags & FISIMU_CONFIG_AE_ENABLE) {
        enableFlags |= FISIMU_CTRL7_ACC_ENABLE | FISIMU_CTRL7_GYR_ENABLE;
    }

    FisImu_write_reg(FisRegister_Ctrl7, enableFlags & FISIMU_CTRL7_ENABLE_MASK);
}


void FisImu_Config_apply(struct FisImuConfig const *config)
{
    uint8_t fisSensors = config->inputSelection;

    if (fisSensors & FISIMU_CONFIG_AE_ENABLE) {
        FisImu_config_ae(config->aeOdr);
    } else {
        if (config->inputSelection & FISIMU_CONFIG_ACC_ENABLE) {
            // FisImu_config_acc(config->accRange, config->accOdr, Lpf_Disable, Hpf_Disable);
            FisImu_config_acc(config->accRange, config->accOdr, Lpf_Enable, Hpf_Disable);
        }
        if (config->inputSelection & FISIMU_CONFIG_GYR_ENABLE) {
            // FisImu_config_gyro(config->gyrRange, config->gyrOdr, Lpf_Disable, Hpf_Disable);
            FisImu_config_gyro(config->gyrRange, config->gyrOdr, Lpf_Enable, Hpf_Disable);
        }
    }

    if (config->inputSelection & FISIMU_CONFIG_MAG_ENABLE) {
        FisImu_config_mag(config->magDev, config->magOdr);
    }
    FisImu_enableSensors(fisSensors);
}

uint8_t FisImu_init(void)
{
    uint8_t chip_id = 0x00;

    int32_t ret = sensor_i2c_open(1, FIS210X_SLAVE_ADDR, I2C_BUS_BIT_RATES_100K, 0);
    if (ret) {
        LOGE("SENSOR", "sensor i2c open 0x%x:0x%x failed, ret:%d\n", 0x1, FIS210X_SLAVE_ADDR, ret);
        return;
    }

    FisImu_read_reg(FisRegister_WhoAmI, &chip_id, 1);
    aos_msleep(100);
    if (chip_id == 0xfc) {
        fisimu_config.inputSelection = FISIMU_CONFIG_ACCGYR_ENABLE;
        fisimu_config.accRange = AccRange_4g;
        fisimu_config.accOdr = AccOdr_128Hz;
        fisimu_config.gyrRange = GyrRange_1024dps; // GyrRange_1024dps;
        fisimu_config.gyrOdr = GyrOdr_256Hz; // GyrOdr_1024Hz;
        fisimu_config.magOdr = MagOdr_32Hz;
        fisimu_config.magDev = MagDev_AK8963;
        fisimu_config.aeOdr = AeOdr_32Hz;
        aos_msleep(100);

        FisImu_Config_apply(&fisimu_config);
        aos_msleep(100);
        FisImu_set_layout(2);
    } else {
        chip_id = 0;
    }

    return chip_id;
}

void FisImu_deinit(void)
{
    int32_t ret = sensor_i2c_close(1);
    if (ret) {
        LOGE("SENSOR", "sensor i2c close failed, ret:%d\n", ret);
    }

    return;
}
