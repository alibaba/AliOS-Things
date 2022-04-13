/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 *
 *
 */

/* Includes ------------------------------------------------------------------*/
#include <string.h>

#include "vl53l0x/vl53l0x_def.h"
#include "vl53l0x/vl53l0x_api.h"
#include "vl53l0x_platform.h"
#include "sensor_drv_api.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define I2C_TIME_OUT_BASE 30
#define I2C_TIME_OUT_BYTE 5
#define AOS_WAIT_TIMEOUT 100
//#define VL53L0X_OsDelay(...) HAL_Delay(2)
#define VL53L0X_OsDelay(...) aos_msleep(2)
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
extern i2c_dev_t vl53l0x_ctx_i2c;

VL53L0X_Error VL53L0X_RdByte(VL53L0X_DEV Dev, uint8_t index, uint8_t *data)
{
    VL53L0X_Error Status = VL53L0X_ERROR_NONE;
    int32_t       status_int;
    int           i2c_time_out = I2C_TIME_OUT_BASE + I2C_TIME_OUT_BYTE;
    status_int =
      sensor_i2c_read(&vl53l0x_ctx_i2c, index, data, 1, i2c_time_out);
    if (status_int < 0) {
        Status = VL53L0X_ERROR_CONTROL_INTERFACE;
        goto done;
    }
done:
    return Status;
}


// the ranging_sensor_comms.dll will take care of the page selection
VL53L0X_Error VL53L0X_WriteMulti(VL53L0X_DEV Dev, uint8_t index, uint8_t *pdata,
                                 uint32_t count)
{
    int           status_int;
    VL53L0X_Error Status       = VL53L0X_ERROR_NONE;
    int           i2c_time_out = I2C_TIME_OUT_BASE + count * I2C_TIME_OUT_BYTE;
    status_int =
      sensor_i2c_write(&vl53l0x_ctx_i2c, index, pdata, count, i2c_time_out);
    if (status_int < 0) {
        Status = VL53L0X_ERROR_CONTROL_INTERFACE;
    }

    return Status;
}

// the ranging_sensor_comms.dll will take care of the page selection
VL53L0X_Error VL53L0X_ReadMulti(VL53L0X_DEV Dev, uint8_t index, uint8_t *pdata,
                                uint32_t count)
{
    VL53L0X_Error Status = VL53L0X_ERROR_NONE;
    int32_t       status_int;
    int           i2c_time_out = I2C_TIME_OUT_BASE + count * I2C_TIME_OUT_BYTE;
    status_int =
      sensor_i2c_read(&vl53l0x_ctx_i2c, index, pdata, count, i2c_time_out);
    if (status_int < 0) {
        Status = VL53L0X_ERROR_CONTROL_INTERFACE;
        goto done;
    }
done:
    return Status;
}


VL53L0X_Error VL53L0X_RdWord(VL53L0X_DEV Dev, uint8_t index, uint16_t *data)
{
    VL53L0X_Error Status = VL53L0X_ERROR_NONE;
    int32_t       status_int;
    uint8_t       mdata[2]     = { 0 };
    int           i2c_time_out = I2C_TIME_OUT_BASE + 2 * I2C_TIME_OUT_BYTE;
    status_int =
      sensor_i2c_read(&vl53l0x_ctx_i2c, index, &mdata[0], 2, i2c_time_out);
    if (status_int < 0) {
        Status = VL53L0X_ERROR_CONTROL_INTERFACE;
        goto done;
    }

    *data = ((uint16_t)mdata[0] << 8) + (uint16_t)mdata[1];
done:
    return Status;
}

VL53L0X_Error VL53L0X_RdDWord(VL53L0X_DEV Dev, uint8_t index, uint32_t *data)
{
    VL53L0X_Error Status = VL53L0X_ERROR_NONE;
    int32_t       status_int;
    uint8_t       mdata[4]     = { 0 };
    int           i2c_time_out = I2C_TIME_OUT_BASE + 4 * I2C_TIME_OUT_BYTE;
    status_int =
      sensor_i2c_read(&vl53l0x_ctx_i2c, index, &mdata[0], 4, i2c_time_out);
    if (status_int < 0) {
        Status = VL53L0X_ERROR_CONTROL_INTERFACE;
        goto done;
    }

    *data = ((uint32_t)mdata[0] << 24) + ((uint32_t)mdata[1] << 16) +
            ((uint32_t)mdata[2] << 8) + (uint32_t)mdata[3];

done:
    return Status;
}

VL53L0X_Error VL53L0X_WrByte(VL53L0X_DEV Dev, uint8_t index, uint8_t data)
{
    VL53L0X_Error Status = VL53L0X_ERROR_NONE;
    int32_t       status_int;
    uint8_t       mdata        = data;
    int           i2c_time_out = I2C_TIME_OUT_BASE + I2C_TIME_OUT_BYTE;
    status_int =
      sensor_i2c_write(&vl53l0x_ctx_i2c, index, &mdata, 1, i2c_time_out);
    if (status_int < 0) {
        Status = VL53L0X_ERROR_CONTROL_INTERFACE;
    }

    return Status;
}

VL53L0X_Error VL53L0X_WrWord(VL53L0X_DEV Dev, uint8_t index, uint16_t data)
{
    VL53L0X_Error Status = VL53L0X_ERROR_NONE;
    int32_t       status_int;
    int           i2c_time_out = I2C_TIME_OUT_BASE + 2 * I2C_TIME_OUT_BYTE;
    uint8_t       mdata[2]     = { 0 };
    mdata[0]                   = data >> 8;
    mdata[1]                   = data & 0x00FF;
    status_int =
      sensor_i2c_write(&vl53l0x_ctx_i2c, index, &mdata[0], 2, i2c_time_out);
    if (status_int < 0) {
        Status = VL53L0X_ERROR_CONTROL_INTERFACE;
    }

    return Status;
}

VL53L0X_Error VL53L0X_WrDWord(VL53L0X_DEV Dev, uint8_t index, uint32_t data)
{
    VL53L0X_Error Status = VL53L0X_ERROR_NONE;
    int32_t       status_int;
    int           i2c_time_out = I2C_TIME_OUT_BASE + 4 * I2C_TIME_OUT_BYTE;
    uint8_t       mdata[4]     = { 0 };
    mdata[0]                   = (data >> 24) & 0xFF;
    mdata[1]                   = (data >> 16) & 0xFF;
    mdata[2]                   = (data >> 8) & 0xFF;
    mdata[3]                   = (data >> 0) & 0xFF;
    status_int =
      sensor_i2c_write(&vl53l0x_ctx_i2c, index, &mdata[0], 4, i2c_time_out);
    if (status_int < 0) {
        Status = VL53L0X_ERROR_CONTROL_INTERFACE;
    }

    return Status;
}

VL53L0X_Error VL53L0X_UpdateByte(VL53L0X_DEV Dev, uint8_t index,
                                 uint8_t AndData, uint8_t OrData)
{
    VL53L0X_Error Status = VL53L0X_ERROR_NONE;
    uint8_t       data;

    Status = VL53L0X_RdByte(Dev, index, &data);

    if (Status) {
        goto done;
    }

    data   = (data & AndData) | OrData;
    Status = VL53L0X_WrByte(Dev, index, data);
done:
    return Status;
}

VL53L0X_Error VL53L0X_PollingDelay(VL53L0X_DEV Dev)
{
    VL53L0X_Error status = VL53L0X_ERROR_NONE;

    // do nothing
    VL53L0X_OsDelay();
    return status;
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
