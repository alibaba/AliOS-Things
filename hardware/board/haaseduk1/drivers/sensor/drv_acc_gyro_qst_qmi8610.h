/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef FIS210X_H
#define FIS210X_H
#include "k_api.h"
#include <math.h>
#include <stdbool.h>
#include "aos/hal/i2c.h"
#include "hal_iomux_haas1000.h"

#ifndef M_PI
#define M_PI    (3.14159265358979323846f)
#endif
#ifndef ONE_G
#define ONE_G   (9.80665f)
#endif

#define FISIMU_CTRL7_DISABLE_ALL        (0x0)
#define FISIMU_CTRL7_ACC_ENABLE         (0x1)
#define FISIMU_CTRL7_GYR_ENABLE         (0x2)
#define FISIMU_CTRL7_MAG_ENABLE         (0x4)
#define FISIMU_CTRL7_AE_ENABLE          (0x8)
#define FISIMU_CTRL7_ENABLE_MASK        (0xF)

#define FISIMU_CONFIG_ACC_ENABLE        FISIMU_CTRL7_ACC_ENABLE
#define FISIMU_CONFIG_GYR_ENABLE        FISIMU_CTRL7_GYR_ENABLE
#define FISIMU_CONFIG_MAG_ENABLE        FISIMU_CTRL7_MAG_ENABLE
#define FISIMU_CONFIG_AE_ENABLE         FISIMU_CTRL7_AE_ENABLE
#define FISIMU_CONFIG_ACCGYR_ENABLE     (FISIMU_CONFIG_ACC_ENABLE | FISIMU_CONFIG_GYR_ENABLE)
#define FISIMU_CONFIG_ACCGYRMAG_ENABLE  (FISIMU_CONFIG_ACC_ENABLE | FISIMU_CONFIG_GYR_ENABLE | FISIMU_CONFIG_MAG_ENABLE)
#define FISIMU_CONFIG_AEMAG_ENABLE      (FISIMU_CONFIG_AE_ENABLE | FISIMU_CONFIG_MAG_ENABLE)

#define FISIMU_STATUS1_CMD_DONE         (0x01)
#define FISIMU_STATUS1_WAKEUP_EVENT     (0x04)

enum FIS210xRegister {
    /*! \brief FIS device identifier register. */
    FisRegister_WhoAmI = 0, // 0
    /*! \brief FIS hardware revision register. */
    FisRegister_Revision, // 1
    /*! \brief General and power management modes. */
    FisRegister_Ctrl1, // 2
    /*! \brief Accelerometer control. */
    FisRegister_Ctrl2, // 3
    /*! \brief Gyroscope control. */
    FisRegister_Ctrl3, // 4
    /*! \brief Magnetometer control. */
    FisRegister_Ctrl4, // 5
    /*! \brief Data processing settings. */
    FisRegister_Ctrl5, // 6
    /*! \brief AttitudeEngine control. */
    FisRegister_Ctrl6, // 7
    /*! \brief Sensor enabled status. */
    FisRegister_Ctrl7, // 8
    /*! \brief Reserved - do not write. */
    FisRegister_Ctrl8, // 9
    /*! \brief Host command register. */
    FisRegister_Ctrl9,
    /*! \brief Calibration register 1 least significant byte. */
    FisRegister_Cal1_L,
    /*! \brief Calibration register 1 most significant byte. */
    FisRegister_Cal1_H,
    /*! \brief Calibration register 2 least significant byte. */
    FisRegister_Cal2_L,
    /*! \brief Calibration register 2 most significant byte. */
    FisRegister_Cal2_H,
    /*! \brief Calibration register 3 least significant byte. */
    FisRegister_Cal3_L,
    /*! \brief Calibration register 3 most significant byte. */
    FisRegister_Cal3_H,
    /*! \brief Calibration register 4 least significant byte. */
    FisRegister_Cal4_L,
    /*! \brief Calibration register 4 most significant byte. */
    FisRegister_Cal4_H,
    /*! \brief FIFO control register. */
    FisRegister_FifoCtrl,
    /*! \brief FIFO data register. */
    FisRegister_FifoData,
    /*! \brief FIFO status register. */
    FisRegister_FifoStatus,
    /*! \brief Output data overrun and availability. */
    FisRegister_Status0,
    /*! \brief Miscellaneous status register. */
    FisRegister_Status1,
    /*! \brief Sample counter. */
    FisRegister_CountOut,
    /*! \brief Accelerometer X axis least significant byte. */
    FisRegister_Ax_L,
    /*! \brief Accelerometer X axis most significant byte. */
    FisRegister_Ax_H,
    /*! \brief Accelerometer Y axis least significant byte. */
    FisRegister_Ay_L,
    /*! \brief Accelerometer Y axis most significant byte. */
    FisRegister_Ay_H,
    /*! \brief Accelerometer Z axis least significant byte. */
    FisRegister_Az_L,
    /*! \brief Accelerometer Z axis most significant byte. */
    FisRegister_Az_H,
    /*! \brief Gyroscope X axis least significant byte. */
    FisRegister_Gx_L,
    /*! \brief Gyroscope X axis most significant byte. */
    FisRegister_Gx_H,
    /*! \brief Gyroscope Y axis least significant byte. */
    FisRegister_Gy_L,
    /*! \brief Gyroscope Y axis most significant byte. */
    FisRegister_Gy_H,
    /*! \brief Gyroscope Z axis least significant byte. */
    FisRegister_Gz_L,
    /*! \brief Gyroscope Z axis most significant byte. */
    FisRegister_Gz_H,
    /*! \brief Magnetometer X axis least significant byte. */
    FisRegister_Mx_L,
    /*! \brief Magnetometer X axis most significant byte. */
    FisRegister_Mx_H,
    /*! \brief Magnetometer Y axis least significant byte. */
    FisRegister_My_L,
    /*! \brief Magnetometer Y axis most significant byte. */
    FisRegister_My_H,
    /*! \brief Magnetometer Z axis least significant byte. */
    FisRegister_Mz_L,
    /*! \brief Magnetometer Z axis most significant byte. */
    FisRegister_Mz_H,
    /*! \brief Quaternion increment W least significant byte. */
    FisRegister_Q1_L = 45,
    /*! \brief Quaternion increment W most significant byte. */
    FisRegister_Q1_H,
    /*! \brief Quaternion increment X least significant byte. */
    FisRegister_Q2_L,
    /*! \brief Quaternion increment X most significant byte. */
    FisRegister_Q2_H,
    /*! \brief Quaternion increment Y least significant byte. */
    FisRegister_Q3_L,
    /*! \brief Quaternion increment Y most significant byte. */
    FisRegister_Q3_H,
    /*! \brief Quaternion increment Z least significant byte. */
    FisRegister_Q4_L,
    /*! \brief Quaternion increment Z most significant byte. */
    FisRegister_Q4_H,
    /*! \brief Velocity increment X least significant byte. */
    FisRegister_Dvx_L,
    /*! \brief Velocity increment X most significant byte. */
    FisRegister_Dvx_H,
    /*! \brief Velocity increment Y least significant byte. */
    FisRegister_Dvy_L,
    /*! \brief Velocity increment Y most significant byte. */
    FisRegister_Dvy_H,
    /*! \brief Velocity increment Z least significant byte. */
    FisRegister_Dvz_L,
    /*! \brief Velocity increment Z most significant byte. */
    FisRegister_Dvz_H,
    /*! \brief Temperature output. */
    FisRegister_Temperature,
    /*! \brief AttitudeEngine clipping flags. */
    FisRegister_AeClipping,
    /*! \brief AttitudeEngine overflow flags. */
    FisRegister_AeOverflow,
};

enum FisImu_Ctrl9Command {
    /*! \brief No operation. */
    Ctrl9_Nop = 0,
    /*! \brief Reset FIFO. */
    Ctrl9_ResetFifo = 0x2,
    /*! \brief Set magnetometer X calibration values. */
    Ctrl9_SetMagXCalibration = 0x6,
    /*! \brief Set magnetometer Y calibration values. */
    Ctrl9_SetMagYCalibration = 0x7,
    /*! \brief Set magnetometer Z calibration values. */
    Ctrl9_SetMagZCalibration = 0x8,
    /*! \brief Set accelerometer offset correction value. */
    Ctrl9_SetAccelOffset = 0x12,
    /*! \brief Set gyroscope offset correction value. */
    Ctrl9_SetGyroOffset = 0x13,
    /*! \brief Set accelerometer sensitivity. */
    Ctrl9_SetAccelSensitivity = 0x14,
    /*! \brief Set gyroscope sensitivity. */
    Ctrl9_SetGyroSensitivity = 0x15,
    /*! \brief Update magnemoter bias compensation. */
    Ctrl9_UpdateMagBias = 0xB,
    /*! \brief Trigger motion on demand sample. */
    Ctrl9_TriggerMotionOnDemand = 0x0c,
    /*! \brief Update gyroscope bias compensation. */
    Ctrl9_UpdateAttitudeEngineGyroBias = 0xE,
    /*! \brief Read frequency correction value. */
    Ctrl9_ReadTrimmedFrequencyValue = 0x18,
    /*! \brief Prepare for FIFO read sequence. */
    Ctrl9_ReadFifo = 0x0D,
    /*! \brief Set wake on motion parameters. */
    Ctrl9_ConfigureWakeOnMotion = 0x19,
};

enum FisImu_LpfConfig {
    Lpf_Disable, /*!< \brief Disable low pass filter. */
    Lpf_Enable   /*!< \brief Enable low pass filter. */
};

enum FisImu_HpfConfig {
    Hpf_Disable, /*!< \brief Disable high pass filter. */
    Hpf_Enable   /*!< \brief Enable high pass filter. */
};

enum FisImu_AccRange {
    AccRange_2g = 0 << 3, /*!< \brief +/- 2g range */
    AccRange_4g = 1 << 3, /*!< \brief +/- 4g range */
    AccRange_8g = 2 << 3, /*!< \brief +/- 8g range */
    AccRange_16g = 3 << 3 /*!< \brief +/- 16g range */
};


enum FisImu_AccOdr {
    AccOdr_1024Hz = 0,  /*!< \brief High resolution 1024Hz output rate. */
    AccOdr_256Hz = 1, /*!< \brief High resolution 256Hz output rate. */
    AccOdr_128Hz = 2, /*!< \brief High resolution 128Hz output rate. */
    AccOdr_32Hz = 3,  /*!< \brief High resolution 32Hz output rate. */
    AccOdr_LowPower_128Hz = 4, /*!< \brief Low power 128Hz output rate. */
    AccOdr_LowPower_64Hz = 5,  /*!< \brief Low power 64Hz output rate. */
    AccOdr_LowPower_25Hz = 6,  /*!< \brief Low power 25Hz output rate. */
    AccOdr_LowPower_3Hz = 7    /*!< \brief Low power 3Hz output rate. */
};

enum FisImu_GyrRange {
    GyrRange_32dps = 0 << 3,   /*!< \brief +-32 degrees per second. */
    GyrRange_64dps = 1 << 3,   /*!< \brief +-64 degrees per second. */
    GyrRange_128dps = 2 << 3,  /*!< \brief +-128 degrees per second. */
    GyrRange_256dps = 3 << 3,  /*!< \brief +-256 degrees per second. */
    GyrRange_512dps = 4 << 3,  /*!< \brief +-512 degrees per second. */
    GyrRange_1024dps = 5 << 3, /*!< \brief +-1024 degrees per second. */
    GyrRange_2048dps = 6 << 3, /*!< \brief +-2048 degrees per second. */
    GyrRange_2560dps = 7 << 3  /*!< \brief +-2560 degrees per second. */
};

/*!
 * \brief Gyroscope output rate configuration.
 */
enum FisImu_GyrOdr {
    GyrOdr_1024Hz           = 0,    /*!< \brief High resolution 1024Hz output rate. */
    GyrOdr_256Hz            = 1,    /*!< \brief High resolution 256Hz output rate. */
    GyrOdr_128Hz            = 2,    /*!< \brief High resolution 128Hz output rate. */
    GyrOdr_32Hz             = 3,    /*!< \brief High resolution 32Hz output rate. */
    GyrOdr_OIS_8192Hz       = 6,    /*!< \brief OIS Mode 8192Hz output rate. */
    GyrOdr_OIS_LL_8192Hz    = 7     /*!< \brief OIS LL Mode 8192Hz output rate. */
};

enum FisImu_AeOdr {
    AeOdr_1Hz = 0,  /*!< \brief 1Hz output rate. */
    AeOdr_2Hz = 1,  /*!< \brief 2Hz output rate. */
    AeOdr_4Hz = 2,  /*!< \brief 4Hz output rate. */
    AeOdr_8Hz = 3,  /*!< \brief 8Hz output rate. */
    AeOdr_16Hz = 4, /*!< \brief 16Hz output rate. */
    AeOdr_32Hz = 5, /*!< \brief 32Hz output rate. */
    AeOdr_64Hz = 6,  /*!< \brief 64Hz output rate. */
    /*!
     * \brief Motion on demand mode.
     *
     * In motion on demand mode the application can trigger AttitudeEngine
     * output samples as necessary. This allows the AttitudeEngine to be
     * synchronized with external data sources.
     *
     * When in Motion on Demand mode the application should request new data
     * by calling the FisImu_requestAttitudeEngineData() function. The
     * AttitudeEngine will respond with a data ready event (INT2) when the
     * data is available to be read.
     */
    AeOdr_motionOnDemand = 128
};

enum FisImu_MagOdr {
    MagOdr_32Hz = 2   /*!< \brief 32Hz output rate. */
};

enum FisImu_MagDev {
    MagDev_AK8975 = (0 << 4), /*!< \brief AKM AK8975. */
    MagDev_AK8963 = (1 << 4) /*!< \brief AKM AK8963. */
};

enum FisImu_AccUnit {
    AccUnit_g,  /*!< \brief Accelerometer output in terms of g (9.81m/s^2). */
    AccUnit_ms2 /*!< \brief Accelerometer output in terms of m/s^2. */
};

enum FisImu_GyrUnit {
    GyrUnit_dps, /*!< \brief Gyroscope output in degrees/s. */
    GyrUnit_rads /*!< \brief Gyroscope output in rad/s. */
};

struct FisImuConfig {
    /*! \brief Sensor fusion input selection. */
    uint8_t inputSelection;
    /*! \brief Accelerometer dynamic range configuration. */
    enum FisImu_AccRange accRange;
    /*! \brief Accelerometer output rate. */
    enum FisImu_AccOdr accOdr;
    /*! \brief Gyroscope dynamic range configuration. */
    enum FisImu_GyrRange gyrRange;
    /*! \brief Gyroscope output rate. */
    enum FisImu_GyrOdr gyrOdr;
    /*! \brief AttitudeEngine output rate. */
    enum FisImu_AeOdr aeOdr;
    /*!
     * \brief Magnetometer output data rate.
     *
     * \remark This parameter is not used when using an external magnetometer.
     * In this case the external magnetometer is sampled at the FIS output
     * data rate, or at an integer divisor thereof such that the maximum
     * sample rate is not exceeded.
     */
    enum FisImu_MagOdr magOdr;

    /*!
     * \brief Magnetometer device to use.
     *
     * \remark This parameter is not used when using an external magnetometer.
     */
    enum FisImu_MagDev magDev;
};


#define FISIMU_SAMPLE_SIZE (3 * sizeof(int16_t))
#define FISIMU_AE_SAMPLE_SIZE ((4 + 3 + 1) * sizeof(int16_t) + sizeof(uint8_t))
struct FisImuRawSample {
    /*! \brief The sample counter of the sample. */
    uint8_t sampleCounter;
    /*!
     * \brief Pointer to accelerometer data in the sample buffer.
     *
     * \c NULL if no accelerometer data is available in the buffer.
     */
    uint8_t const *accelerometerData;
    /*!
     * \brief Pointer to gyroscope data in the sample buffer.
     *
     * \c NULL if no gyroscope data is available in the buffer.
     */
    uint8_t const *gyroscopeData;
    /*!
     * \brief Pointer to magnetometer data in the sample buffer.
     *
     * \c NULL if no magnetometer data is available in the buffer.
     */
    uint8_t const *magnetometerData;
    /*!
     * \brief Pointer to AttitudeEngine data in the sample buffer.
     *
     * \c NULL if no AttitudeEngine data is available in the buffer.
     */
    uint8_t const *attitudeEngineData;
    /*! \brief Raw sample buffer. */
    uint8_t sampleBuffer[FISIMU_SAMPLE_SIZE + FISIMU_AE_SAMPLE_SIZE];
    /*! \brief Contents of the FIS status 1 register. */
    uint8_t status1;
    // uint8_t status0;
    // uint32_t durT;
};

struct FisImu_offsetCalibration {
    enum FisImu_AccUnit accUnit;
    float accOffset[3];
    enum FisImu_GyrUnit gyrUnit;
    float gyrOffset[3];
};

struct FisImu_sensitivityCalibration {
    float accSensitivity[3];
    float gyrSensitivity[3];
};

enum FisImu_Interrupt {
    /*! \brief FIS INT1 line. */
    Fis_Int1 = (0 << 6),
    /*! \brief FIS INT2 line. */
    Fis_Int2 = (1 << 6)
};

enum FisImu_InterruptInitialState {
    InterruptInitialState_high = (1 << 7), /*!< Interrupt high. */
    InterruptInitialState_low  = (0 << 7)  /*!< Interrupt low. */
};

enum FisImu_WakeOnMotionThreshold {
    WomThreshold_high = 128, /*!< High threshold - large motion needed to wake. */
    WomThreshold_low  = 32   /*!< Low threshold - small motion needed to wake. */
};


extern uint8_t FisImu_write_reg(uint8_t reg, uint8_t value);
extern uint8_t FisImu_read_reg(uint8_t reg, uint8_t *buf, uint16_t len);
extern uint8_t FisImu_init(void);
extern void FisImu_deinit(void);
extern void FisImu_Config_apply(struct FisImuConfig const *config);
extern void FisImu_enableSensors(uint8_t enableFlags);
extern void FisImu_read_acc_xyz(float acc_xyz[3]);
extern void FisImu_read_gyro_xyz(float gyro_xyz[3]);
extern void FisImu_read_xyz(float acc[3], float gyro[3]);
extern uint8_t FisImu_readStatus1(void);
extern int8_t FisImu_readTemp(void);
extern void FisImu_enableWakeOnMotion(void);
extern void FisImu_disableWakeOnMotion(void);
// for XKF3
extern void FisImu_processAccelerometerData(uint8_t const *rawData, float *calibratedData);
extern void FisImu_processGyroscopeData(uint8_t const *rawData, float *calibratedData);
extern void FisImu_read_rawsample(struct FisImuRawSample *sample);
extern void FisImu_applyOffsetCalibration(struct FisImu_offsetCalibration const *cal);
// for XKF3
// fis210x
#endif
