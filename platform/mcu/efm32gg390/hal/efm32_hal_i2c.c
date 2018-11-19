
#include "BCDS_I2CTransceiver.h"
#include "BCDS_MCU_I2C_Handle.h"
//#include "BSP_BoardSetting.h"
#include "BCDS_HAL.h"
#include "hal/hal.h"
#include "em_i2c.h"
#include "em_cmu.h"
#include "em_gpio.h"


#define I2C0_SCL_PIN                                                            (15)
#define I2C0_SCL_PORT                                                           (gpioPortD)
#define I2C0_SCL_MODE                                                           (gpioModeWiredAnd)

#define I2C0_SDA_PIN                                                            (14)
#define I2C0_SDA_PORT                                                           (gpioPortD)
#define I2C0_SDA_MODE                                                           (gpioModeWiredAnd)


#define I2C1_SCL_PIN                                                            (12)
#define I2C1_SCL_PORT                                                           (gpioPortB)
#define I2C1_SCL_MODE                                                           (gpioModeWiredAndPullUpFilter)

#define I2C1_SDA_PIN                                                            (11)
#define I2C1_SDA_PORT                                                           (gpioPortB)
#define I2C1_SDA_MODE                                                           (gpioModeWiredAndPullUpFilter)


static I2cTranceiverHandle_T I2C0Transceiver = { false, NULL, NULL, NULL, INT8_C(-1) };
static struct MCU_I2C_Handle_S I2C0_Handle;
bool i2c0enable = false;
static I2cTranceiverHandle_T I2C1Transceiver = { false, NULL, NULL, NULL, INT8_C(-1) };
static struct MCU_I2C_Handle_S I2C1_Handle;
bool i2c1enable = false;

static inline I2cTranceiverHandle_T *get_priv(uint8_t port)
{
    I2cTranceiverHandle_T *hi2c = NULL;

    switch (port) {
    case 0:
        hi2c = &I2C0Transceiver;
        break;
    case 1:
        hi2c = &I2C1Transceiver;
        break;
    default:
        return NULL;
    }

    return hi2c;
}

static void I2c0InterruptCallback(I2C_T i2c, struct MCU_I2C_Event_S event)
{
    BCDS_UNUSED(i2c);
    I2CTransceiver_LoopCallback(&I2C0Transceiver, event);
}

static void I2c1InterruptCallback(I2C_T i2c, struct MCU_I2C_Event_S event)
{
    BCDS_UNUSED(i2c);
    I2CTransceiver_LoopCallback(&I2C1Transceiver, event);
}

void I2C0_IRQHandler(void)
{
    
    if(i2c0enable == true)
        I2C0_Handle.IRQCallback((I2C_T)&I2C0_Handle);
}

void I2C1_IRQHandler(void)
{
    
    if(i2c1enable == true)
        I2C1_Handle.IRQCallback((I2C_T)&I2C1_Handle);
}

void I2C0_Init(void)
{
    I2C_T i2chandle;
    int ret =0;
    I2C_Init_TypeDef i2cInit =
            { false, /* Enable when init done */
            true, /* Set to master mode */
            0, /* Use currently configured reference clock */
            I2C_FREQ_FASTPLUS_MAX, /* Set to fast+ rate assuring being */
            /* within I2C spec */
            i2cClockHLRStandard /* Set to use 4:4 low/high duty cycle */
            };

    CMU_ClockEnable(cmuClock_I2C0, true);

    I2C_Init(I2C0, &i2cInit);

    CMU_ClockEnable(cmuClock_I2C0, false);
    
    GPIO_PinModeSet(I2C0_SCL_PORT, I2C0_SCL_PIN, I2C0_SCL_MODE, 1);
    GPIO_PinModeSet(I2C0_SDA_PORT, I2C0_SDA_PIN, I2C0_SDA_MODE, 1);
    // Configure the Route for the I2C
    CMU_ClockEnable(cmuClock_I2C0, true);
    I2C0->ROUTE = I2C_ROUTE_SDAPEN | I2C_ROUTE_SCLPEN | I2C_ROUTE_LOCATION_LOC3;
    CMU_ClockEnable(cmuClock_I2C0, false);
    
    
    I2C0_Handle.instance = I2C0;
    I2C0_Handle.TransferMode = BCDS_HAL_TRANSFER_MODE_INTERRUPT;
    ret = MCU_I2C_Initialize((I2C_T)&I2C0_Handle,I2c0InterruptCallback);
    printf("MCU_I2C_Initialize ret=%d\n",ret);
    ret = I2CTransceiver_Init(&I2C0Transceiver,(I2C_T)&I2C0_Handle);
    printf("I2CTransceiver_Init ret=%d\n",ret);

    
    
    CMU_ClockEnable(cmuClock_I2C0, true);
    
    NVIC_SetPriority(I2C0_IRQn, 5);
    NVIC_EnableIRQ(I2C0_IRQn);
    
    I2C_Enable(I2C0, true);
    i2c0enable = true;
    //SensorNode_EnableBus();

}

void I2C1_Init(void)
{
    I2C_T i2chandle;
    int ret =0;
    I2C_Init_TypeDef i2cInit =
            { false, /* Enable when init done */
            true, /* Set to master mode */
            0, /* Use currently configured reference clock */
            I2C_FREQ_STANDARD_MAX, /* Set to fast+ rate assuring being */
            /* within I2C spec */
            i2cClockHLRStandard /* Set to use 4:4 low/high duty cycle */
            };
    CMU_ClockEnable(cmuClock_GPIO, true);

    CMU_ClockEnable(cmuClock_I2C1, true);

    I2C_Init(I2C1, &i2cInit);

    CMU_ClockEnable(cmuClock_I2C1, false);
    
    GPIO_PinModeSet(I2C1_SCL_PORT, I2C1_SCL_PIN, I2C1_SCL_MODE, 1);
    GPIO_PinModeSet(I2C1_SDA_PORT, I2C1_SDA_PIN, I2C1_SDA_MODE, 1);
    // Configure the Route for the I2C
    CMU_ClockEnable(cmuClock_I2C1, true);
    I2C1->ROUTE = I2C_ROUTE_SDAPEN | I2C_ROUTE_SCLPEN | I2C_ROUTE_LOCATION_LOC1;
    CMU_ClockEnable(cmuClock_I2C1, false);
    
    
    I2C1_Handle.instance = I2C1;
    I2C1_Handle.TransferMode = BCDS_HAL_TRANSFER_MODE_INTERRUPT;
    ret = MCU_I2C_Initialize((I2C_T)&I2C1_Handle,I2c1InterruptCallback);
    printf("MCU_I2C_Initialize ret=%d\n",ret);
    ret = I2CTransceiver_Init(&I2C1Transceiver,(I2C_T)&I2C1_Handle);
    printf("I2CTransceiver_Init ret=%d\n",ret);

    
    
    CMU_ClockEnable(cmuClock_I2C1, true);
    
    NVIC_SetPriority(I2C1_IRQn, 5);
    NVIC_EnableIRQ(I2C1_IRQn);
    
    I2C_Enable(I2C1, true);
    i2c1enable = true;
    //SensorNode_EnableBus();

}

int32_t hal_i2c_init(i2c_dev_t *i2c)
{
    printf("hal_i2c_init i2cport=%d\n",i2c->port);
    int32_t ret = -1;

    if (i2c == NULL) {
       return -1;
    }

    switch(i2c->port){
        case 0:
            I2C0_Init();
            ret = 0;
            break;
        case 1:
            I2C1_Init();
            ret = 0;
            break;
        default:
            break;
        }
    return ret;

}


int32_t hal_i2c_master_send(i2c_dev_t *i2c, uint16_t dev_addr, const uint8_t *data,
                            uint16_t size, uint32_t timeout)
{
    int ret = -1;
    I2cTranceiverHandle_T * handle = NULL;

    if (NULL == i2c || NULL == data){
        return ret;
    }
    handle = get_priv(i2c->port);
    if(handle == NULL)
    {
        return ret;
    }
    ret = MCU_I2C_Send(handle->I2CHandle,dev_addr,data,size);
    return ret;
}


int32_t hal_i2c_master_recv(i2c_dev_t *i2c, uint16_t dev_addr, uint8_t *data,
                            uint16_t size, uint32_t timeout)
{
    
    int ret = -1;
    I2cTranceiverHandle_T * handle = NULL;

    if (NULL == i2c || NULL == data){
        return ret;
    }
    handle = get_priv(i2c->port);
    if(handle == NULL)
    {
        return ret;
    }
    ret = MCU_I2C_Receive(handle->I2CHandle,dev_addr,data,size);
    return ret;
}

int32_t hal_i2c_slave_send(i2c_dev_t *i2c, const uint8_t *data, uint16_t size, uint32_t timeout)
{
}

int32_t hal_i2c_slave_recv(i2c_dev_t *i2c, uint8_t *data, uint16_t size, uint32_t timeout)
{
}

int32_t hal_i2c_mem_write(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr,
                          uint16_t mem_addr_size, const uint8_t *data, uint16_t size,
                          uint32_t timeout)
{
    int ret = -1;
    I2cTranceiverHandle_T *psti2c = NULL;
    if (NULL == i2c || NULL == data){
        return ret;
    }
    psti2c = get_priv(i2c->port);
    if(RETCODE_OK == I2CTransceiver_Write(psti2c,dev_addr,mem_addr,data,size))
    {
        ret = 0;
    }
    
    return ret;
}


int32_t hal_i2c_mem_read(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr,
                         uint16_t mem_addr_size, uint8_t *data, uint16_t size,
                         uint32_t timeout)
{
    int ret = -1;
    I2cTranceiverHandle_T *psti2c = NULL;
    if (NULL == i2c || NULL == data){
        return ret;
    }
    psti2c = get_priv(i2c->port);
    if(RETCODE_OK == I2CTransceiver_Read(psti2c,dev_addr,mem_addr,data,size))
    {
        ret = 0;
    }
    
    return ret;

}

