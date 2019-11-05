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

//#include "vl53l0x_proximity.h"
#include "vl53l0x/vl53l0x_def.h"
#include "vl53l0x/vl53l0x_api.h"
#include "vl53l0x_platform.h"

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/

#define PROXIMITY_I2C_ADDRESS ((uint16_t)0x0052)
#define VL53L0X_ID ((uint16_t)0xEEAA)
#define VL53L0X_XSHUT_Pin GPIO_PIN_6
#define VL53L0X_XSHUT_GPIO_Port GPIOC
#define VL53L0X_IRQ_PIN (62)

static gpio_irq_trigger_t vl53l0x_irq_mode_rising = IRQ_TRIGGER_RISING_EDGE;
bool                      g_interrupt_received    = false;

/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
VL53L0X_Dev_t Dev = { .I2cDevAddr = PROXIMITY_I2C_ADDRESS };

i2c_dev_t vl53l0x_ctx_i2c = {
    .port                 = 3,
    .config.address_width = 8,
    .config.freq          = 400000,
    .config.dev_addr      = PROXIMITY_I2C_ADDRESS,
};

static sensor_obj_t sensor;

/* Private function prototypes -----------------------------------------------*/
void SENSOR_IO_Init(void);
void SetupSingleShot(VL53L0X_Dev_t Dev);


void VL53L0X_INTERRRUPT_INIT()
{
    int status;
    (void)status;
    if (sensor.mode == DEV_INT) {
        status = VL53L0X_SetGpioConfig(
          &Dev, 0, 0, VL53L0X_REG_SYSTEM_INTERRUPT_GPIO_LEVEL_HIGH,
          VL53L0X_INTERRUPTPOLARITY_HIGH);
        status = VL53L0X_SetInterruptThresholds(
          &Dev, 0, (FixPoint1616_t)100 * 65536, (FixPoint1616_t)200 * 65536);
        status =
          VL53L0X_SetDeviceMode(&Dev, VL53L0X_DEVICEMODE_CONTINUOUS_RANGING);
        VL53L0X_ClearInterruptMask(&Dev, 0);
    } else if (sensor.mode == DEV_DATA_READY) {
        status = VL53L0X_SetGpioConfig(
          &Dev, 0, 0, VL53L0X_REG_SYSTEM_INTERRUPT_GPIO_NEW_SAMPLE_READY,
          VL53L0X_INTERRUPTPOLARITY_HIGH);
        status =
          VL53L0X_SetDeviceMode(&Dev, VL53L0X_DEVICEMODE_CONTINUOUS_RANGING);
        VL53L0X_ClearInterruptMask(&Dev, 0);
    } else {
    }
}


/**
 * @brief  VL53L0X proximity sensor Initialization.
 */
void VL53L0X_PROXIMITY_Init(void)
{
    uint16_t             vl53l0x_id = 0;
    VL53L0X_DeviceInfo_t VL53L0X_DeviceInfo;

    memset(&VL53L0X_DeviceInfo, 0, sizeof(VL53L0X_DeviceInfo_t));

    if (VL53L0X_ERROR_NONE ==
        VL53L0X_GetDeviceInfo(&Dev, &VL53L0X_DeviceInfo)) {
        if (VL53L0X_ERROR_NONE ==
            VL53L0X_RdWord(&Dev, VL53L0X_REG_IDENTIFICATION_MODEL_ID,
                           (uint16_t *)&vl53l0x_id)) {
            if (vl53l0x_id == VL53L0X_ID) {
                if (VL53L0X_ERROR_NONE == VL53L0X_DataInit(&Dev)) {
                    SetupSingleShot(Dev);
                }
            }
        } else {
            LOG("VL53L0X Time of Flight Failed to Initialize!\n");
        }
    } else {
        LOG("VL53L0X Time of Flight Failed to get infos!\n");
    }
    VL53L0X_SetInterMeasurementPeriodMilliSeconds(&Dev, 30);
    VL53L0X_INTERRRUPT_INIT();
}

/**
 * @brief  Get distance from VL53L0X proximity sensor.
 * @retval Distance in mm
 */
uint16_t VL53L0X_PROXIMITY_GetDistance(void)
{
    VL53L0X_RangingMeasurementData_t RangingMeasurementData;

    VL53L0X_PerformSingleRangingMeasurement(&Dev, &RangingMeasurementData);

    return RangingMeasurementData.RangeMilliMeter;
}

/**
 * @brief  VL53L0X proximity sensor Msp Initialization.
 */
void VL53L0X_PROXIMITY_MspInit(void) {}

/**
 *  Setup all detected sensors for single shot mode and setup ranging
 * configuration
 */
void SetupSingleShot(VL53L0X_Dev_t Dev)
{
    int            status;
    uint8_t        VhvSettings;
    uint8_t        PhaseCal;
    uint32_t       refSpadCount;
    uint8_t        isApertureSpads;
    FixPoint1616_t signalLimit           = (FixPoint1616_t)(0.25 * 65536);
    FixPoint1616_t sigmaLimit            = (FixPoint1616_t)(18 * 65536);
    uint32_t       timingBudget          = 33000;
    uint8_t        preRangeVcselPeriod   = 14;
    uint8_t        finalRangeVcselPeriod = 10;


    if (Dev.Present) {
        status = VL53L0X_StaticInit(&Dev);
        if (status) {
            LOG("VL53L0X_StaticInit failed\n");
        }


        status = VL53L0X_PerformRefCalibration(&Dev, &VhvSettings, &PhaseCal);
        if (status) {
            LOG("VL53L0X_PerformRefCalibration failed\n");
        }

        status = VL53L0X_PerformRefSpadManagement(&Dev, &refSpadCount,
                                                  &isApertureSpads);
        if (status) {
            LOG("VL53L0X_PerformRefSpadManagement failed\n");
        }

        status = VL53L0X_SetDeviceMode(
          &Dev,
          VL53L0X_DEVICEMODE_SINGLE_RANGING); // Setup in single ranging mode
        if (status) {
            LOG("VL53L0X_SetDeviceMode failed\n");
        }

        status = VL53L0X_SetLimitCheckEnable(
          &Dev, VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE, 1); // Enable Sigma limit
        if (status) {
            LOG("VL53L0X_SetLimitCheckEnable failed\n");
        }

        status = VL53L0X_SetLimitCheckEnable(
          &Dev, VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE,
          1); // Enable Signa limit
        if (status) {
            LOG("VL53L0X_SetLimitCheckEnable failed\n");
        }

        /* Ranging configuration */
        signalLimit = (FixPoint1616_t)(0.1 * 65536);
        sigmaLimit  = (FixPoint1616_t)(60 * 65536);
        // signalLimit = (FixPoint1616_t)(0.25*65536);
        // sigmaLimit = (FixPoint1616_t)(18*65536);
        timingBudget = 33000;
        // timingBudget = 200000;
        preRangeVcselPeriod   = 18;
        finalRangeVcselPeriod = 14;

        status = VL53L0X_SetLimitCheckValue(
          &Dev, VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE, signalLimit);

        if (status) {
            LOG("VL53L0X_SetLimitCheckValue failed\n");
        }

        status = VL53L0X_SetLimitCheckValue(
          &Dev, VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE, sigmaLimit);
        if (status) {
            LOG("VL53L0X_SetLimitCheckValue failed\n");
        }

        status =
          VL53L0X_SetMeasurementTimingBudgetMicroSeconds(&Dev, timingBudget);
        if (status) {
            LOG("VL53L0X_SetMeasurementTimingBudgetMicroSeconds failed\n");
        }

        status = VL53L0X_SetVcselPulsePeriod(
          &Dev, VL53L0X_VCSEL_PERIOD_PRE_RANGE, preRangeVcselPeriod);
        if (status) {
            LOG("VL53L0X_SetVcselPulsePeriod failed\n");
        }

        status = VL53L0X_SetVcselPulsePeriod(
          &Dev, VL53L0X_VCSEL_PERIOD_FINAL_RANGE, finalRangeVcselPeriod);
        if (status) {
            LOG("VL53L0X_SetVcselPulsePeriod failed\n");
        }

        Dev.LeakyFirst = 1;
    }
}


static void drv_ps_st_vl53l0x_irq_handle(void)
{
    VL53L0X_ClearInterruptMask(&Dev, 0);
    g_interrupt_received = true;
}

static int drv_ps_st_vl53l0x_open(void)
{
    VL53L0X_StartMeasurement(&Dev);
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

static int drv_ps_st_vl53l0x_close(void)
{
    VL53L0X_StopMeasurement(&Dev);
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

static int drv_ps_st_vl53l0x_read(void *buf, size_t len)
{
    size_t                           size;
    VL53L0X_RangingMeasurementData_t RangingMeasurementData;
    VL53L0X_Error                    Status = VL53L0X_ERROR_NONE;
    proximity_data_t *               pdata  = (proximity_data_t *)buf;
    if (buf == NULL) {
        return -1;
    }
    size = sizeof(proximity_data_t);
    if (len < size) {
        return -1;
    }
    if (g_interrupt_received == false) {
        pdata->present = VL53L0X_PROXIMITY_GetDistance();
    } else {
        g_interrupt_received = false;
        VL53L0X_ClearInterruptMask(&Dev, 0);
        Status =
          VL53L0X_GetRangingMeasurementData(&Dev, &RangingMeasurementData);
        if (Status != VL53L0X_ERROR_NONE)
            return -1;
        pdata->present = RangingMeasurementData.RangeMilliMeter;
    }
    LOG("drv_ps_st_vl53l0x_read DATA=%d\n ", pdata->present);
    return (int)size;
}

static int drv_ps_st_vl53l0x_write(const void *buf, size_t len)
{
    return 0;
}

static int drv_ps_st_vl53l0x_ioctl(int cmd, unsigned long arg)
{
    VL53L0X_Error Status = VL53L0X_ERROR_NONE;
    switch (cmd) {
        case SENSOR_IOCTL_ODR_SET: {
        } break;
        case SENSOR_IOCTL_SET_POWER: {
            Status = VL53L0X_SetPowerMode(&Dev, arg);
            if (Status != VL53L0X_ERROR_NONE)
                return -1;
        } break;
        case SENSOR_IOCTL_GET_INFO: {
        } break;

        default:
            break;
    }

    return 0;
}


int drv_ps_st_vl53l0x_init(void)
{

    int ret = 0;

    /* fill the sensor obj parameters here */
    sensor.tag         = TAG_DEV_PS;
    sensor.path        = dev_ps_path;
    sensor.io_port     = I2C_PORT;
    sensor.open        = drv_ps_st_vl53l0x_open;
    sensor.close       = drv_ps_st_vl53l0x_close;
    sensor.read        = drv_ps_st_vl53l0x_read;
    sensor.write       = drv_ps_st_vl53l0x_write;
    sensor.ioctl       = drv_ps_st_vl53l0x_ioctl;
    sensor.irq_handle  = drv_ps_st_vl53l0x_irq_handle;
    sensor.mode        = DEV_POLLING;
    sensor.gpio.port   = VL53L0X_IRQ_PIN;
    sensor.gpio.config = IRQ_MODE;
    sensor.gpio.priv   = &vl53l0x_irq_mode_rising;
    sensor.data_len    = sizeof(proximity_data_t);


    ret = sensor_create_obj(&sensor);
    if (unlikely(ret)) {
        return -1;
    }
    VL53L0X_PROXIMITY_Init();

    LOG("%s %s successfully \n", SENSOR_STR, __func__);

    return 0;
}

SENSOR_DRV_ADD(drv_ps_st_vl53l0x_init);

