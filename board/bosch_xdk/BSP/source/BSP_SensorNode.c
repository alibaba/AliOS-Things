/*----------------------------------------------------------------------------*/
/*
 * Copyright (C) Bosch Connected Devices and Solutions GmbH. 
 * All Rights Reserved. Confidential.
 *
 * Distribution only to people who need to know this information in
 * order to do their job.(Need-to-know principle).
 * Distribution to persons outside the company, only if these persons
 * signed a non-disclosure agreement.
 * Electronic transmission, e.g. via electronic mail, must be made in
 * encrypted form.
 */
/*----------------------------------------------------------------------------*/

/**
 * @file
 * @brief  Implementation of I2C Sensor Node BSP functions
 */

/* This is used to define the correct module ID for generation of module
 * error codes
 */
#include "BCDS_BSP.h"
#undef BCDS_MODULE_ID
#define BCDS_MODULE_ID BCDS_BSP_MODULE_ID_SENSOR_NODE

#include "BCDS_HAL.h"

#if BCDS_FEATURE_BSP_SENSOR_NODE

#include "BSP_BoardType.h"
#include "BSP_BoardSettings.h"
#include "BSP_BoardShared.h"
#include "BSP_SensorNode.h"
#include "BCDS_BSP_SensorNode.h"
#include "BCDS_MCU_I2C_Handle.h"

#define BMA280_STARTUP_DELAY           UINT32_C(3)      /**< macro used to represent the delay required for BMA280 after the power-up */
#define BME280_STARTUP_DELAY           UINT32_C(10)     /**< macro used to represent the delay required for BME280 after the power-up */
#define BMG160_STARTUP_DELAY           UINT32_C(30)     /**< macro used to represent the delay required for BMG160 after the power-up */
#define BMI160_STARTUP_DELAY           UINT32_C(10)     /**< macro used to represent the delay required for BMI160 after the power-up */
#define BMM150_STARTUP_DELAY           UINT32_C(4)      /**< macro used to represent the delay required for BMM150 after the power-up */
#define MAX44009_STARTUP_DELAY         UINT32_C(10)     /**< macro used to represent the delay required for MAX4409 after the power-up */

enum Retcode_SensorNode_E
{
    RETCODE_BSP_SENSOR_NODE_BMA280_INT2_BUSY = RETCODE_FIRST_CUSTOM_CODE,
    RETCODE_BSP_SENSOR_NODE_BMM150_INT_BUSY,
    RETCODE_BSP_SENSOR_NODE_BMI160_INT2_BUSY,
    RETCODE_SENSOR_NODE_BMA280_ENABLED,
    RETCODE_SENSOR_NODE_BMG160_ENABLED,
    RETCODE_SENSOR_NODE_BME280_ENABLED,
    RETCODE_SENSOR_NODE_BMM150_ENABLED,
    RETCODE_SENSOR_NODE_MAX44009_ENABLED,
    RETCODE_SENSOR_NODE_BMI160_ENABLED,
};

enum
{
    BMA280_INTERRUPT_1 = BSP_INTERRUPT_INDEX_1,
    BMA280_INTERRUPT_2 = BSP_INTERRUPT_INDEX_2
};

enum
{
    MAX44009_INTN = BSP_INTERRUPT_INDEX_1,
};

enum
{
    BMG160_INTERRUPT_1 = BSP_INTERRUPT_INDEX_1,
    BMG160_INTERRUPT_2 = BSP_INTERRUPT_INDEX_2
};

enum
{
    BMM150_INTERRUPT_1 = BSP_INTERRUPT_INDEX_1,
    BMM150_DRDY = BSP_INTERRUPT_INDEX_2
};

enum
{
    BMI160_INTERRUPT_1 = BSP_INTERRUPT_INDEX_1,
    BMI160_INTERRUPT_2 = BSP_INTERRUPT_INDEX_2
};

static void BMA280_INT2_InterruptCallback(void);
static void BMA280_INT_InterruptCallback(void);

static void MAX44009_INTN_InterruptCallback(void);

static void BMG160_INT2_InterruptCallback(void);
static void BMG160_INT1_InterruptCallback(void);

static void BMM150_INT_InterruptCallback(void);
static void BMM150_DRDY_InterruptCallback(void);

static void BMI160_INT2_InterruptCallback(void);
static void BMI160_INT1_InterruptCallback(void);

I2C0Register_T BSP_I2C0;

static BSP_Sensor_InterruptCallback_T SensorNode_BMI160ISRCallback;
static BSP_Sensor_InterruptCallback_T SensorNode_BMM150ISRCallback;
static BSP_Sensor_InterruptCallback_T SensorNode_BMA280ISRCallback;
static BSP_Sensor_InterruptCallback_T SensorNode_BMG160ISRCallback;
static BSP_Sensor_InterruptCallback_T SensorNode_MAX44009ISRCallback;
static struct MCU_I2C_Handle_S BSP_SensorNode_I2C_Handle;
static I2C_IRQ_Callback_T SensorNode_I2CISRCallback;

static struct
{
    bool BMA280UsesINT1 :1;
    bool BMA280UsesINT2 :1;
    bool BMM150UsesINT :1;
    bool BMM150UsesDRDY :1;
    bool BMI160UsesINT1 :1;
    bool BMI160UsesINT2 :1;
    bool BMG160UsesINT1 :1;
    bool BMG160UsesINT2 :1;
    bool MAX44009UsesINTN :1;

} SensorsFlags;
/**
 * @brief Enable the I2C bus
 * @details This function enables the bus and registers the sensor in the
 * bus shared register
 * @param Sensor the sensor requesting  to be enabled
 */
static void SensorNode_EnableBus(void);
/**
 * @brief Disables the I2C bus
 * @details This function disables the bus if it is not in use by any sensor
 * @param Sensor the sensor requesting  to be enabled
 */
static void SensorNode_DisableBus(void);

/*
 * Refer to interface header for description
 */
Retcode_T BSP_SensorNode_Connect(void)
{
    GPIO_PinModeSet(VDD_BMA280_PORT, VDD_BMA280_PIN, VDD_BMA280_MODE, 0);

    GPIO_PinModeSet(VDD_BME280_PORT, VDD_BME280_PIN, VDD_BME280_MODE, 0);

    GPIO_PinModeSet(VDD_BMG160_PORT, VDD_BMG160_PIN, VDD_BMG160_MODE, 0);

    GPIO_PinModeSet(VDD_BMI160_PORT, VDD_BMI160_PIN, VDD_BMI160_MODE, 0);

    GPIO_PinModeSet(VDD_BMM150_PORT, VDD_BMM150_PIN, VDD_BMM150_MODE, 0);

    GPIO_PinModeSet(VDD_MAX44009_PORT, VDD_MAX44009_PIN, VDD_MAX44009_MODE, 0);

    GPIO_PinModeSet(BMA280_INT1_PORT, BMA280_INT1_PIN, BMA280_INT1_MODE, 0);
    GPIO_PinModeSet(BMA280_INT2_PORT, BMA280_INT2_PIN, BMA280_INT2_MODE, 0);

    GPIO_PinModeSet(BMG160_INT1_PORT, BMG160_INT1_PIN, BMG160_INT1_MODE, 0);
    GPIO_PinModeSet(BMG160_INT2_PORT, BMG160_INT2_PIN, BMG160_INT2_MODE, 0);

    GPIO_PinModeSet(BMI160_INT1_PORT, BMI160_INT1_PIN, BMI160_INT1_MODE, 0);
    GPIO_PinModeSet(BMI160_INT2_PORT, BMI160_INT2_PIN, BMI160_INT2_MODE, 0);

    GPIO_PinModeSet(BMM150_DRDY_PORT, BMM150_DRDY_PIN, BMM150_DRDY_MODE, 0);
    GPIO_PinModeSet(BMM150_INT_PORT, BMM150_INT_PIN, BMM150_INT_MODE, 0);

    GPIO_PinModeSet(MAX44009_INTN_PORT, MAX44009_INTN_PIN, MAX44009_INTN_MODE, 0);

    GPIO_PinModeSet(I2C0_SCL_PORT, I2C0_SCL_PIN, I2C0_SCL_MODE, 1);
    GPIO_PinModeSet(I2C0_SDA_PORT, I2C0_SDA_PIN, I2C0_SDA_MODE, 1);

    // Configure the Route for the I2C
    CMU_ClockEnable(SENSOR_NODE_I2C_CLOCK, true);
    SENSOR_NODE_I2C_BUS->ROUTE = I2C_ROUTE_SDAPEN | I2C_ROUTE_SCLPEN | I2C_ROUTE_LOCATION_LOC3;
    CMU_ClockEnable(SENSOR_NODE_I2C_CLOCK, false);

    return RETCODE_OK;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_SensorNode_Enable(uint32_t id, BSP_Sensor_InterruptCallback_T callback)
{
    Retcode_T retcode = RETCODE_OK;
    switch (id)
    {
        case (BSP_XDK_SENSOR_BMA280):
            if (BSP_I2C0.Peripheral.Sensor_BMA280 == 0)
            {
                /* Enable VDD for BMA280 sensor */
                GPIO_PinModeSet(VDD_BMA280_PORT, VDD_BMA280_PIN, VDD_BMA280_MODE, 1);
                Board_Delay(BMA280_STARTUP_DELAY);

                SensorNode_EnableBus();
                BSP_I2C0.Peripheral.Sensor_BMA280 = 1;
                /*lint -e506 wrong config results in runtime error reported*/
                if (BSP_XDK_SENSOR_BMA280_ENABLE_INTERRUPT)
                {
                    if (NULL == callback)
                    {
                        retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
                    }
                    else if ((0 == BSP_XDK_SENSOR_BMA280_FORCE_INTERRUPT) && (NULL != Board_GPIOInterruptGetCallback(BMA280_INT2_PIN)))
                    {
                        /* Enable interrupt1 pin for BMA280 sensor */
                        GPIO_PinOutSet(BMA280_INT1_PORT, BMA280_INT1_PIN);

                        NVIC_DisableIRQ(GPIO_EVEN_IRQn);

                        SensorNode_BMA280ISRCallback = callback;

                        Board_GPIOInterruptRegisterCallback(BMA280_INT1_PIN, BMA280_INT_InterruptCallback);

                        GPIO_IntConfig(BMA280_INT1_PORT, BMA280_INT1_PIN, BMA280_INT1_EDGE_RISING, BMA280_INT1_EDGE_FALLING, BSP_ENABLE);

                        SensorsFlags.BMA280UsesINT1 = true;

                        NVIC_EnableIRQ(GPIO_EVEN_IRQn);

                        retcode = RETCODE(RETCODE_SEVERITY_INFO, RETCODE_BSP_SENSOR_NODE_BMA280_INT2_BUSY);
                    }
                    else
                    {
                        /* Enable interrupt pins for BMA280 sensor */
                        GPIO_PinOutSet(BMA280_INT1_PORT, BMA280_INT1_PIN);
                        GPIO_PinOutSet(BMA280_INT2_PORT, BMA280_INT2_PIN);

                        NVIC_DisableIRQ(GPIO_ODD_IRQn);
                        NVIC_DisableIRQ(GPIO_EVEN_IRQn);

                        SensorNode_BMA280ISRCallback = callback;

                        Board_GPIOInterruptRegisterCallback(BMA280_INT1_PIN, BMA280_INT_InterruptCallback);
                        Board_GPIOInterruptRegisterCallback(BMA280_INT2_PIN, BMA280_INT2_InterruptCallback);

                        GPIO_IntConfig(BMA280_INT1_PORT, BMA280_INT1_PIN, BMA280_INT1_EDGE_RISING, BMA280_INT1_EDGE_FALLING, BSP_ENABLE);
                        GPIO_IntConfig(BMA280_INT2_PORT, BMA280_INT2_PIN, BMA280_INT2_EDGE_RISING, BMA280_INT2_EDGE_FALLING, BSP_ENABLE);
                        SensorsFlags.BMA280UsesINT1 = true;
                        SensorsFlags.BMA280UsesINT2 = true;

                        NVIC_EnableIRQ(GPIO_ODD_IRQn);
                        NVIC_EnableIRQ(GPIO_EVEN_IRQn);

                        retcode = RETCODE_OK;
                    }
                }
            }
            else
            {
                retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_SENSOR_NODE_BMA280_ENABLED);
            }
            break;
        case (BSP_XDK_SENSOR_BMG160):
            if (BSP_I2C0.Peripheral.Sensor_BMG160 == 0)
            {
                /* Enable VDD for BMG160 sensor */
                GPIO_PinModeSet(VDD_BMG160_PORT, VDD_BMG160_PIN, VDD_BMG160_MODE, 1);
                Board_Delay(BMG160_STARTUP_DELAY);

                SensorNode_EnableBus();
                BSP_I2C0.Peripheral.Sensor_BMG160 = 1;
                /*lint -e506 wrong config results in runtime error reported*/
                if (BSP_XDK_SENSOR_BMG160_ENABLE_INTERRUPT)
                {
                    if (NULL == callback)
                    {
                        retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
                    }
                    else
                    {
                        /* Enable interrupt pins for BMG160 sensor */
                        GPIO_PinOutSet(BMG160_INT1_PORT, BMG160_INT1_PIN);
                        GPIO_PinOutSet(BMG160_INT2_PORT, BMG160_INT2_PIN);

                        NVIC_DisableIRQ(GPIO_ODD_IRQn);
                        NVIC_DisableIRQ(GPIO_EVEN_IRQn);
                        SensorNode_BMG160ISRCallback = callback;
                        Board_GPIOInterruptRegisterCallback(BMG160_INT1_PIN, BMG160_INT1_InterruptCallback);
                        Board_GPIOInterruptRegisterCallback(BMG160_INT2_PIN, BMG160_INT2_InterruptCallback);

                        GPIO_IntConfig(BMG160_INT1_PORT, BMG160_INT1_PIN, BMG160_INT1_EDGE_RISING, BMG160_INT1_EDGE_FALLING, BSP_ENABLE);
                        GPIO_IntConfig(BMG160_INT2_PORT, BMG160_INT2_PIN, BMG160_INT2_EDGE_RISING, BMG160_INT2_EDGE_FALLING, BSP_ENABLE);

                        SensorsFlags.BMG160UsesINT1 = true;
                        SensorsFlags.BMG160UsesINT2 = true;

                        NVIC_EnableIRQ(GPIO_ODD_IRQn);
                        NVIC_EnableIRQ(GPIO_EVEN_IRQn);
                        retcode = RETCODE_OK;
                    }
                }
            }
            else
            {
                retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_SENSOR_NODE_BMG160_ENABLED);
            }
            break;
        case (BSP_XDK_SENSOR_BME280):
            if (BSP_I2C0.Peripheral.Sensor_BME280 == 0)
            {
                /* Enable VDD for BME280 sensor */
                GPIO_PinModeSet(VDD_BME280_PORT, VDD_BME280_PIN, VDD_BME280_MODE, 1);
                Board_Delay(BME280_STARTUP_DELAY);

                SensorNode_EnableBus();
                BSP_I2C0.Peripheral.Sensor_BME280 = 1;
            }
            else
            {
                retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_SENSOR_NODE_BME280_ENABLED);
            }
            break;
        case (BSP_XDK_SENSOR_BMM150):
            if (BSP_I2C0.Peripheral.Sensor_BMM150 == 0)
            {
                /* Enable VDD for BMM150 sensor */
                GPIO_PinModeSet(VDD_BMM150_PORT, VDD_BMM150_PIN, VDD_BMM150_MODE, 1);
                Board_Delay(BMM150_STARTUP_DELAY);

                SensorNode_EnableBus();
                BSP_I2C0.Peripheral.Sensor_BMM150 = 1;
                /*lint -e506 wrong config results in runtime error reported*/
                if (BSP_XDK_SENSOR_BMM150_ENABLE_INTERRUPT)
                {
                    if (NULL == callback)
                    {
                        retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
                    }
                    else if ((0 == BSP_XDK_SENSOR_BMM150_FORCE_INTERRUPT) && (NULL != Board_GPIOInterruptGetCallback(BMM150_INT_PIN)))
                    {
                        /* Enable Data Ready pin for BMM150 sensor */
                        GPIO_PinOutSet(BMM150_DRDY_PORT, BMM150_DRDY_PIN);

                        NVIC_DisableIRQ(GPIO_ODD_IRQn);

                        SensorNode_BMM150ISRCallback = callback;

                        Board_GPIOInterruptRegisterCallback(BMM150_DRDY_PIN, BMM150_DRDY_InterruptCallback);

                        GPIO_IntConfig(BMM150_DRDY_PORT, BMM150_DRDY_PIN, BMM150_DRDY_EDGE_RISING, BMM150_DRDY_EDGE_FALLING, BSP_ENABLE);

                        SensorsFlags.BMM150UsesDRDY = true;

                        NVIC_EnableIRQ(GPIO_ODD_IRQn);
                        retcode = RETCODE(RETCODE_SEVERITY_INFO, RETCODE_BSP_SENSOR_NODE_BMM150_INT_BUSY);
                    }
                    else
                    {
                        /* Enable Data Ready pin and interrupt pin for BMM150 sensor */
                        GPIO_PinOutSet(BMM150_DRDY_PORT, BMM150_DRDY_PIN);
                        GPIO_PinOutSet(BMM150_INT_PORT, BMM150_INT_PIN);

                        NVIC_DisableIRQ(GPIO_ODD_IRQn);
                        NVIC_DisableIRQ(GPIO_EVEN_IRQn);

                        SensorNode_BMM150ISRCallback = callback;

                        Board_GPIOInterruptRegisterCallback(BMM150_INT_PIN, BMM150_INT_InterruptCallback);
                        Board_GPIOInterruptRegisterCallback(BMM150_DRDY_PIN, BMM150_DRDY_InterruptCallback);

                        GPIO_IntConfig(BMM150_INT_PORT, BMM150_INT_PIN, BMM150_INT_EDGE_RISING, BMM150_INT_EDGE_FALLING, BSP_ENABLE);
                        GPIO_IntConfig(BMM150_DRDY_PORT, BMM150_DRDY_PIN, BMM150_DRDY_EDGE_RISING, BMM150_DRDY_EDGE_FALLING, BSP_ENABLE);

                        SensorsFlags.BMM150UsesINT = true;
                        SensorsFlags.BMM150UsesDRDY = true;

                        NVIC_EnableIRQ(GPIO_ODD_IRQn);
                        NVIC_EnableIRQ(GPIO_EVEN_IRQn);

                        retcode = RETCODE_OK;
                    }
                }
            }
            else
            {
                retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_SENSOR_NODE_BMM150_ENABLED);
            }

            break;
        case (BSP_XDK_SENSOR_MAX44009):
            if (BSP_I2C0.Peripheral.Sensor_MAX44009 == 0)
            {
                /* Enable VDD for MAX44009 sensor */
                GPIO_PinModeSet(VDD_MAX44009_PORT, VDD_MAX44009_PIN, VDD_MAX44009_MODE, 1);
                Board_Delay(MAX44009_STARTUP_DELAY);

                SensorNode_EnableBus();
                BSP_I2C0.Peripheral.Sensor_MAX44009 = 1;
                /*lint -e506 wrong config results in runtime error reported*/
                if (BSP_XDK_SENSOR_MAX44009_ENABLE_INTERRUPT)
                {
                    if (NULL == callback)
                    {
                        retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
                    }
                    else
                    {
                        /* Enable interrupt pin for MAX44009 sensor */
                        GPIO_PinOutSet(MAX44009_INTN_PORT, MAX44009_INTN_PIN);

                        NVIC_DisableIRQ(GPIO_EVEN_IRQn);

                        SensorNode_MAX44009ISRCallback = callback;

                        Board_GPIOInterruptRegisterCallback(MAX44009_INTN_PIN, MAX44009_INTN_InterruptCallback);

                        GPIO_IntConfig(MAX44009_INTN_PORT, MAX44009_INTN_PIN, MAX44009_INTN_EDGE_RISING, MAX44009_INTN_EDGE_FALLING, BSP_ENABLE);

                        SensorsFlags.MAX44009UsesINTN = true;

                        NVIC_EnableIRQ(GPIO_EVEN_IRQn);

                        retcode = RETCODE_OK;
                    }
                }
            }
            else
            {
                retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_SENSOR_NODE_MAX44009_ENABLED);
            }

            break;
        case (BSP_XDK_SENSOR_BMI160):
            if (BSP_I2C0.Peripheral.Sensor_BMI160 == 0)
            {
                /* Enable VDD for BMI160 sensor */
                GPIO_PinModeSet(VDD_BMI160_PORT, VDD_BMI160_PIN, VDD_BMI160_MODE, 1);
                Board_Delay(BMI160_STARTUP_DELAY);

                SensorNode_EnableBus();
                BSP_I2C0.Peripheral.Sensor_BMI160 = 1;
                /*lint -e506 wrong config results in runtime error reported*/
                if (BSP_XDK_SENSOR_BMI160_ENABLE_INTERRUPT)
                {
                    if (NULL == callback)
                    {
                        retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
                    }
                    else if ((false == BSP_XDK_SENSOR_BMI160_FORCE_INTERRUPT) && (NULL != Board_GPIOInterruptGetCallback(BMI160_INT2_PIN)))
                    {
                        /* Enable interrupt1 pin for BMI160 sensor */
                        GPIO_PinOutSet(BMI160_INT1_PORT, BMI160_INT1_PIN);

                        NVIC_DisableIRQ(GPIO_ODD_IRQn);

                        SensorNode_BMI160ISRCallback = callback;

                        Board_GPIOInterruptRegisterCallback(BMI160_INT1_PIN, BMI160_INT1_InterruptCallback);

                        GPIO_IntConfig(BMI160_INT1_PORT, BMI160_INT1_PIN, BMI160_INT1_EDGE_RISING, BMI160_INT1_EDGE_FALLING, BSP_ENABLE);

                        SensorsFlags.BMI160UsesINT1 = true;

                        NVIC_EnableIRQ(GPIO_ODD_IRQn);

                        retcode = RETCODE(RETCODE_SEVERITY_INFO, RETCODE_BSP_SENSOR_NODE_BMI160_INT2_BUSY);

                    }
                    else
                    {
                        /* Enable interrupt pins for BMI160 sensor */
                        GPIO_PinOutSet(BMI160_INT1_PORT, BMI160_INT1_PIN);
                        GPIO_PinOutSet(BMI160_INT2_PORT, BMI160_INT2_PIN);

                        NVIC_DisableIRQ(GPIO_ODD_IRQn);
                        NVIC_DisableIRQ(GPIO_EVEN_IRQn);

                        SensorNode_BMI160ISRCallback = callback;

                        Board_GPIOInterruptRegisterCallback(BMI160_INT1_PIN, BMI160_INT1_InterruptCallback);
                        Board_GPIOInterruptRegisterCallback(BMI160_INT2_PIN, BMI160_INT2_InterruptCallback);

                        GPIO_IntConfig(BMI160_INT1_PORT, BMI160_INT1_PIN, BMI160_INT1_EDGE_RISING, BMI160_INT1_EDGE_FALLING, BSP_ENABLE);
                        GPIO_IntConfig(BMI160_INT2_PORT, BMI160_INT2_PIN, BMI160_INT2_EDGE_RISING, BMI160_INT2_EDGE_FALLING, BSP_ENABLE);

                        SensorsFlags.BMI160UsesINT1 = true;
                        SensorsFlags.BMI160UsesINT2 = true;

                        NVIC_EnableIRQ(GPIO_ODD_IRQn);
                        NVIC_EnableIRQ(GPIO_EVEN_IRQn);

                        retcode = RETCODE_OK;
                    }
                }
            }
            else
            {
                retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_SENSOR_NODE_BMI160_ENABLED);
            }
            break;
        default:
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    }
    return retcode;
}
/*
 * Refer to interface header for description
 */
Retcode_T BSP_SensorNode_Disable(uint32_t id)
{

    Retcode_T retcode = RETCODE_OK;
    switch (id)
    {
        case (BSP_XDK_SENSOR_BMA280):

            BSP_I2C0.Peripheral.Sensor_BMA280 = 0;
            SensorNode_DisableBus();
            if (SensorsFlags.BMA280UsesINT1)
            {
                /* Disable BMA280_INT1 HW Pin Interrupt */
                GPIO_IntConfig(  BMA280_INT1_PORT, BMA280_INT1_PIN, BMA280_INT1_EDGE_RISING, BMA280_INT1_EDGE_FALLING, BSP_DISABLE);
          	   	/* Disable HW input filter */
                GPIO_PinOutClear(BMA280_INT1_PORT, BMA280_INT1_PIN);

                SensorNode_BMA280ISRCallback = NULL;
                Board_GPIOInterruptUnregisterCallback(BMA280_INT1_PIN);
                SensorsFlags.BMA280UsesINT1 = false;
            }
            if (SensorsFlags.BMA280UsesINT2)
            {
                /* Disable BMA280_INT2 HW Pin Interrupt */
                GPIO_IntConfig(  BMA280_INT1_PORT, BMA280_INT2_PIN, BMA280_INT2_EDGE_RISING, BMA280_INT2_EDGE_FALLING, BSP_DISABLE);
                /* Disable BMA280_INT2 HW Pin Interrupt */
                GPIO_PinOutClear(BMA280_INT2_PORT, BMA280_INT1_PIN);

                SensorNode_BMA280ISRCallback = NULL;
                Board_GPIOInterruptUnregisterCallback(BMA280_INT2_PIN);
                SensorsFlags.BMA280UsesINT2 = false;
            }

            /* Disable VDD for BMA280 sensor */
            GPIO_PinOutClear(VDD_BMA280_PORT, VDD_BMA280_PIN);

            retcode = RETCODE_OK;
            break;
        case (BSP_XDK_SENSOR_BMG160):
            BSP_I2C0.Peripheral.Sensor_BMG160 = 0;
            SensorNode_DisableBus();
            if (SensorsFlags.BMG160UsesINT1)
            {
                /* Disable BMG160_INT1 HW Pin Interrupt */
                GPIO_IntConfig(  BMG160_INT1_PORT, BMG160_INT1_PIN, BMG160_INT1_EDGE_RISING, BMG160_INT1_EDGE_FALLING, BSP_DISABLE);
                /* Disable HW input filter */
                GPIO_PinOutClear(BMG160_INT1_PORT, BMG160_INT1_PIN);

            	SensorNode_BMG160ISRCallback = NULL;
                Board_GPIOInterruptUnregisterCallback(BMG160_INT1_PIN);
                SensorsFlags.BMG160UsesINT1 = false;
            }
            if (SensorsFlags.BMG160UsesINT2)
            {
                /* Disable BMG160_INT1 HW Pin Interrupt */
                GPIO_IntConfig(  BMG160_INT2_PORT, BMG160_INT2_PIN, BMG160_INT2_EDGE_RISING, BMG160_INT2_EDGE_FALLING, BSP_DISABLE);
                /* Disable HW input filter */
                GPIO_PinOutClear(BMG160_INT2_PORT, BMG160_INT2_PIN);

                SensorNode_BMG160ISRCallback = NULL;
                Board_GPIOInterruptUnregisterCallback(BMG160_INT2_PIN);
                SensorsFlags.BMG160UsesINT2 = false;
            }

            /* Disable VDD for BMG160 sensor */
            GPIO_PinOutClear(VDD_BMG160_PORT, VDD_BMG160_PIN);

            retcode = RETCODE_OK;
            break;
        case (BSP_XDK_SENSOR_BME280):
            BSP_I2C0.Peripheral.Sensor_BME280 = 0;
            SensorNode_DisableBus();
            /* Disable VDD for BME280 sensor */
            GPIO_PinOutClear(VDD_BME280_PORT, VDD_BME280_PIN);
            retcode = RETCODE_OK;
            break;

        case (BSP_XDK_SENSOR_BMM150):
            BSP_I2C0.Peripheral.Sensor_BMM150 = 0;
            SensorNode_DisableBus();
            if (SensorsFlags.BMM150UsesINT)
            {
                /* Disable BMG160_INT1 HW Pin Interrupt */
                GPIO_IntConfig(  BMM150_INT_PORT, BMM150_INT_PIN, BMM150_INT_EDGE_RISING, BMM150_INT_EDGE_FALLING, BSP_DISABLE);
                /* Disable HW input filter */
                GPIO_PinOutClear(BMM150_INT_PORT, BMM150_INT_PIN);

                SensorNode_BMM150ISRCallback = NULL;
                Board_GPIOInterruptUnregisterCallback(BMM150_INT_PIN);
                SensorsFlags.BMM150UsesINT = false;
            }
            if (SensorsFlags.BMM150UsesDRDY)
            {
                /* Disable BMG160_INT1 HW Pin Interrupt */
                GPIO_IntConfig(  BMM150_DRDY_PORT, BMM150_DRDY_PIN, BMM150_DRDY_EDGE_RISING, BMM150_DRDY_EDGE_FALLING, BSP_DISABLE);
                 /* Disable HW input filter */
                GPIO_PinOutClear(BMM150_DRDY_PORT, BMM150_DRDY_PIN);

                SensorNode_BMM150ISRCallback = NULL;
                Board_GPIOInterruptUnregisterCallback(BMM150_DRDY_PIN);
                SensorsFlags.BMM150UsesDRDY = false;
            }

            /* Disable VDD for BMM150 sensor */
            GPIO_PinOutClear(VDD_BMM150_PORT, VDD_BMM150_PIN);

            retcode = RETCODE_OK;
            break;
        case (BSP_XDK_SENSOR_MAX44009):
            BSP_I2C0.Peripheral.Sensor_MAX44009 = 0;
            SensorNode_DisableBus();
            if (SensorsFlags.MAX44009UsesINTN)
            {
                /* Disable interrupt pin for MAX44009 sensor */
            	GPIO_IntConfig(  MAX44009_INTN_PORT, MAX44009_INTN_PIN, MAX44009_INTN_EDGE_RISING, MAX44009_INTN_EDGE_FALLING, BSP_DISABLE);
            	/* Disable HW input filter */
                GPIO_PinOutClear(MAX44009_INTN_PORT, MAX44009_INTN_PIN);

                SensorNode_MAX44009ISRCallback = NULL;
                Board_GPIOInterruptUnregisterCallback(MAX44009_INTN_PIN);
                SensorsFlags.MAX44009UsesINTN = false;

            }

            /* Disable VDD for MAX44009 sensor */
            GPIO_PinOutClear(VDD_MAX44009_PORT, VDD_MAX44009_PIN);

            retcode = RETCODE_OK;
            break;
        case (BSP_XDK_SENSOR_BMI160):
            BSP_I2C0.Peripheral.Sensor_BMI160 = 0;
            SensorNode_DisableBus();
            if (SensorsFlags.BMI160UsesINT1)
            {
                /* Disable interrupt pin for BMI160 sensor */
              	GPIO_IntConfig(  BMI160_INT1_PORT, BMI160_INT1_PIN, BMI160_INT1_EDGE_RISING, BMI160_INT1_EDGE_FALLING, BSP_DISABLE);
              	/* Disable HW input filter */
                GPIO_PinOutClear(BMI160_INT1_PORT, BMI160_INT1_PIN);

                SensorNode_BMI160ISRCallback = NULL;
                Board_GPIOInterruptUnregisterCallback(BMG160_INT1_PIN);
                SensorsFlags.BMI160UsesINT1 = false;
            }
            else if (SensorsFlags.BMI160UsesINT2)
            {
                SensorNode_BMI160ISRCallback = NULL;
                Board_GPIOInterruptUnregisterCallback(BMI160_INT2_PIN);
                SensorsFlags.BMI160UsesINT2 = false;
                /* Disable interrupt2 pin for MAX44009 sensor */
                GPIO_PinOutClear(BMI160_INT2_PORT, BMI160_INT2_PIN);
            }

            /* Disable VDD for BMI160 sensor */
            GPIO_PinOutClear(VDD_BMI160_PORT, VDD_BMI160_PIN);

            retcode = RETCODE_OK;
            break;
        default:
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    }
    return retcode;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_SensorNode_Disconnect(void)
{
    GPIO_PinModeSet(VDD_BMA280_PORT, VDD_BMA280_PIN, gpioModeDisabled, 0);
    GPIO_PinModeSet(VDD_BME280_PORT, VDD_BME280_PIN, gpioModeDisabled, 0);
    GPIO_PinModeSet(VDD_BMG160_PORT, VDD_BMG160_PIN, gpioModeDisabled, 0);
    GPIO_PinModeSet(VDD_BMI160_PORT, VDD_BMI160_PIN, gpioModeDisabled, 0);
    GPIO_PinModeSet(VDD_BMM150_PORT, VDD_BMM150_PIN, gpioModeDisabled, 0);
    GPIO_PinModeSet(VDD_MAX44009_PORT, VDD_MAX44009_PIN, gpioModeDisabled, 0);

    GPIO_PinModeSet(BMA280_INT1_PORT, BMA280_INT1_PIN, gpioModeDisabled, 0);
    GPIO_PinModeSet(BMA280_INT2_PORT, BMA280_INT2_PIN, gpioModeDisabled, 0);
    GPIO_PinModeSet(BMG160_INT1_PORT, BMG160_INT1_PIN, gpioModeDisabled, 0);
    GPIO_PinModeSet(BMG160_INT2_PORT, BMG160_INT2_PIN, gpioModeDisabled, 0);
    GPIO_PinModeSet(BMI160_INT1_PORT, BMI160_INT1_PIN, gpioModeDisabled, 0);
    GPIO_PinModeSet(BMI160_INT2_PORT, BMI160_INT2_PIN, gpioModeDisabled, 0);
    GPIO_PinModeSet(BMM150_DRDY_PORT, BMM150_DRDY_PIN, gpioModeDisabled, 0);
    GPIO_PinModeSet(BMM150_INT_PORT, BMM150_INT_PIN, gpioModeDisabled, 0);
    GPIO_PinModeSet(MAX44009_INTN_PORT, MAX44009_INTN_PIN, gpioModeDisabled, 0);

    GPIO_PinModeSet(I2C0_SCL_PORT, I2C0_SCL_PIN, gpioModeDisabled, 0);
    GPIO_PinModeSet(I2C0_SDA_PORT, I2C0_SDA_PIN, gpioModeDisabled, 0);

    // Configure the Route for the I2C
    CMU_ClockEnable(SENSOR_NODE_I2C_CLOCK, true);
    SENSOR_NODE_I2C_BUS->ROUTE = _I2C_ROUTE_RESETVALUE;
    CMU_ClockEnable(SENSOR_NODE_I2C_CLOCK, false);
    return RETCODE_OK;
}

/*
 * Refer to interface header for description
 */
HWHandle_T BSP_SensorNode_GetI2CHandle(void)
{
    BSP_SensorNode_I2C_Handle.instance = SENSOR_NODE_I2C_BUS;
    BSP_SensorNode_I2C_Handle.TransferMode = BCDS_HAL_TRANSFER_MODE_INTERRUPT;
    return (HWHandle_T) &BSP_SensorNode_I2C_Handle;
}

/*
 * Refer to interface header for description
 */
bool BSP_SensorNode_IsAllNodesDisabled(void)
{
    bool SensorDisable = true;
    if(BSP_I2C0.Register)
    {
        SensorDisable =  false;
    }
    return SensorDisable;
}

/*
 * Refer to interface header for description
 */
static void SensorNode_EnableBus(void)
{
    if (0 == BSP_I2C0.Register)
    {
        CMU_ClockEnable(SENSOR_NODE_I2C_CLOCK, true);

        if (NULL != BSP_SensorNode_I2C_Handle.IRQCallback)
        {
            SensorNode_I2CISRCallback = BSP_SensorNode_I2C_Handle.IRQCallback;
            NVIC_SetPriority(SENSOR_NODE_I2C_IRQn, SENSOR_NODE_I2C_PRIORITY);
            NVIC_EnableIRQ(SENSOR_NODE_I2C_IRQn);
        }
        I2C_Enable(SENSOR_NODE_I2C_BUS, true);
    }
}

/*
 * See declaration for description
 */
static void SensorNode_DisableBus(void)
{

    if (BSP_I2C0.Register == 0)
    {
        NVIC_DisableIRQ(SENSOR_NODE_I2C_IRQn);
        SensorNode_I2CISRCallback = NULL;
        I2C_Enable(SENSOR_NODE_I2C_BUS, false);
        CMU_ClockEnable(SENSOR_NODE_I2C_CLOCK, false);
    }
}
static void BMA280_INT_InterruptCallback(void)
{
    uint32_t index;
    uint32_t edge;

    /*lint -e{64} assignment for code clarity purposes*/
    index = (uint32_t) BMA280_INTERRUPT_1;
    if (BSP_IO_PIN_LOW == GPIO_PinInGet(BMA280_INT1_PORT, BMA280_INT1_PIN))
    {
        edge = (uint32_t) BSP_FALLING_EDGE;
    }
    else
    {
        edge = (uint32_t) BSP_RISING_EDGE;
    }
    SensorNode_BMA280ISRCallback(index, edge);

}
/*
 * Refer to prototype for description
 */
static void BMA280_INT2_InterruptCallback(void)
{
    uint32_t index;
    uint32_t edge;

    index = (uint32_t) BMA280_INTERRUPT_2;
    if (BSP_IO_PIN_LOW == GPIO_PinInGet(BMA280_INT2_PORT, BMA280_INT2_PIN))
    {
        edge = (uint32_t) BSP_FALLING_EDGE;
    }
    else
    {
        edge = (uint32_t) BSP_RISING_EDGE;
    }
    SensorNode_BMA280ISRCallback(index, edge);
}
/*
 * Refer to prototype for description
 */
static void MAX44009_INTN_InterruptCallback(void)
{
    uint32_t index;
    uint32_t edge;

    index = (uint32_t) MAX44009_INTN;
    if (BSP_IO_PIN_LOW == GPIO_PinInGet(MAX44009_INTN_PORT, MAX44009_INTN_PIN))
    {
        edge = (uint32_t) BSP_FALLING_EDGE;
    }
    else
    {
        edge = (uint32_t) BSP_RISING_EDGE;
    }
    SensorNode_MAX44009ISRCallback(index, edge);
}
/*
 * Refer to prototype for description
 */
static void BMG160_INT2_InterruptCallback(void)
{
    uint32_t index;
    uint32_t edge;

    index = (uint32_t) BMG160_INTERRUPT_2;
    if (BSP_IO_PIN_LOW == GPIO_PinInGet(BMG160_INT2_PORT, BMG160_INT2_PIN))
    {
        edge = (uint32_t) BSP_FALLING_EDGE;
    }
    else
    {
        edge = (uint32_t) BSP_RISING_EDGE;
    }
    SensorNode_BMG160ISRCallback(index, edge);
}
/*
 * Refer to prototype for description
 */
static void BMG160_INT1_InterruptCallback(void)
{
    uint32_t index;
    uint32_t edge;

    index = (uint32_t) BMG160_INTERRUPT_1;
    if (BSP_IO_PIN_LOW == GPIO_PinInGet(BMG160_INT1_PORT, BMG160_INT1_PIN))
    {
        edge = (uint32_t) BSP_FALLING_EDGE;
    }
    else
    {
        edge = (uint32_t) BSP_RISING_EDGE;
    }
    SensorNode_BMG160ISRCallback(index, edge);
}
/*
 * Refer to prototype for description
 */
static void BMM150_INT_InterruptCallback(void)
{
    uint32_t index;
    uint32_t edge;

    index = BMM150_INTERRUPT_1;
    if (BSP_IO_PIN_LOW == GPIO_PinInGet(BMM150_INT_PORT, BMM150_INT_PIN))
    {
        edge = (uint32_t) BSP_FALLING_EDGE;
    }
    else
    {
        edge = (uint32_t) BSP_RISING_EDGE;
    }
    SensorNode_BMM150ISRCallback(index, edge);
}
/*
 * Refer to prototype for description
 */
static void BMM150_DRDY_InterruptCallback(void)
{
    uint32_t index;
    uint32_t edge;

    index = (uint32_t) BMM150_DRDY;
    if (BSP_IO_PIN_LOW == GPIO_PinInGet(BMM150_DRDY_PORT, BMM150_DRDY_PIN))
    {
        edge = (uint32_t) BSP_FALLING_EDGE;
    }
    else
    {
        edge = (uint32_t) BSP_RISING_EDGE;
    }
    SensorNode_BMM150ISRCallback(index, edge);
}
/*
 * Refer to prototype for description
 */
static void BMI160_INT2_InterruptCallback(void)
{
    uint32_t index;
    uint32_t edge;

    index = (uint32_t) BMI160_INTERRUPT_2;
    if (BSP_IO_PIN_LOW == GPIO_PinInGet(BMI160_INT2_PORT, BMI160_INT2_PIN))
    {
        edge = (uint32_t) BSP_FALLING_EDGE;
    }
    else
    {
        edge = (uint32_t) BSP_RISING_EDGE;
    }
    SensorNode_BMI160ISRCallback(index, edge);
}
/*
 * Refer to prototype for description
 */
static void BMI160_INT1_InterruptCallback(void)
{
    uint32_t index;
    uint32_t edge;

    index = (uint32_t) BMI160_INTERRUPT_1;
    if (BSP_IO_PIN_LOW == GPIO_PinInGet(BMI160_INT1_PORT, BMI160_INT1_PIN))
    {
        edge = (uint32_t) BSP_FALLING_EDGE;
    }
    else
    {
        edge = (uint32_t) BSP_RISING_EDGE;
    }
    SensorNode_BMI160ISRCallback(index, edge);
}

void SensorNode_I2CISR(void)
{
    if (NULL != SensorNode_I2CISRCallback)
    {
        /* All I2C interrupt processing is to be done at the MCU level */
        SensorNode_I2CISRCallback((I2C_T) &BSP_SensorNode_I2C_Handle);
    }
}

#endif /* BCDS_FEATURE_BSP_SENSOR_NODE */

