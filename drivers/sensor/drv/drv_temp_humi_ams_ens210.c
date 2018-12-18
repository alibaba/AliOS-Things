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


#define ENS210_I2C_ADDR1            (0x43)
#define ENS210_I2C_ADDR_TRANS(n)    ((n)<<1)  
#define ENS210_I2C_ADDR             ENS210_I2C_ADDR_TRANS(ENS210_I2C_ADDR1)

/**
* @brief ENS210 API Constants
*/
/* Register address */
#define ENS210_REG_PART_ID                      0x00
#define ENS210_REG_UID                          0x04
#define ENS210_REG_SYS_CTRL                     0x10
#define ENS210_REG_SYS_STAT                     0x11
#define ENS210_REG_SENS_RUN                     0x21
#define ENS210_REG_SENS_START                   0x22
#define ENS210_REG_SENS_STOP                    0x23
#define ENS210_REG_SENS_STAT                    0x24
#define ENS210_REG_T_VAL                        0x30
#define ENS210_REG_H_VAL                        0x33

/** Mask to extract 16-bit data from raw T and H values */
#define ENS210_T_H_MASK                         0xFFFFU

/** Simplification macro, implementing integer division with simple rounding to closest number
 *  It supports both positive and negative numbers, but ONLY positive divisors */
#define IDIV(n,d)                               ((n)>0 ? ((n)+(d)/2)/(d) : ((n)-(d)/2)/(d))

#define CRC7WIDTH                               7       //7 bits CRC has polynomial of 7th order (has 8 terms)
#define CRC7POLY                                0x89    //The 8 coefficients of the polynomial
#define CRC7IVEC                                0x7F    //Initial vector has all 7 bits high

#define DATA7WIDTH                              17
#define DATA7MASK                               ((1UL << DATA7WIDTH) - 1)     //0b 1 1111 1111 1111 1111
#define DATA7MSB                                (1UL << (DATA7WIDTH - 1))     //0b 1 0000 0000 0000 0000
#define I2C_RESULT_OK          					0
#define ENS210_RESET_WAIT_TIME_MS               2
#define ENS210_ADDRESS 							67 //Address of the ENS210
#define WaitMsec 								delay

/*****************************************************************************
 * Types/enumerations/variables
 ****************************************************************************/

/** ENS210 T and H conversion time in milliseconds. Refer to ENS210 data sheet for timing information. */
#define ENS210_T_H_CONVERSION_TIME_MS           130

/** ENS210 T conversion time in milliseconds */
#define ENS210_T_CONVERSION_TIME_MS             105

/** ENS210 Booting time in milliseconds. */
#define ENS210_BOOTING_TIME_MS                  1


/** ENS210 SysCtrl register: Low power enable */
#define ENS210_SYSCTRL_LOWPOWER_ENABLE          (1 << 0)
/** ENS210 SysCtrl register: Low power disable */
#define ENS210_SYSCTRL_LOWPOWER_DISABLE         (0 << 0)
/** ENS210 SysCtrl register: Reset enable */
#define ENS210_SYSCTRL_RESET_ENABLE             (1 << 7)
/** ENS210 SysCtrl register: Reset disable */
#define ENS210_SYSCTRL_RESET_DISABLE            (0 << 7)

/** ENS210 SysStat register: Standby or Booting mode */
#define ENS210_SYSSTAT_MODE_STANDBY             (0 << 0)
/** ENS210 SysStat register: Active mode */
#define ENS210_SYSSTAT_MODE_ACTIVE              (1 << 0)


/** ENS210 SensRun register: temperature single shot mode */
#define ENS210_SENSRUN_T_MODE_SINGLE_SHOT       (0 << 0)
/** ENS210 SensRun register: temperature continuous mode */
#define ENS210_SENSRUN_T_MODE_CONTINUOUS        (1 << 0)
/** ENS210 SensRun register: relative humidity single shot mode */
#define ENS210_SENSRUN_H_MODE_SINGLE_SHOT       (0 << 1)
/** ENS210 SensRun register: relative humidity continuous mode */
#define ENS210_SENSRUN_H_MODE_CONTINUOUS        (1 << 1)

/** ENS210  SensStart register: T sensor start */
#define ENS210_SENSSTART_T_START                (1 << 0)
/** ENS210  SensStart register: H sensor start */
#define ENS210_SENSSTART_H_START                (1 << 1)

/** ENS210  SensStop register: T sensor stop */
#define ENS210_SENSSTOP_T_STOP                  (1 << 0)
/** ENS210  SensStop register: H sensor stop */
#define ENS210_SENSSTOP_H_STOP                  (1 << 1)

/** ENS210  SensStat register: T sensor idle */
#define ENS210_SENSSTAT_T_STAT_IDLE             (0 << 0)
/** ENS210  SensStat register: T sensor active */
#define ENS210_SENSSTAT_T_STAT_ACTIVE           (1 << 0)
/** ENS210  SensStat register: H sensor idle */
#define ENS210_SENSSTAT_H_STAT_IDLE             (0 << 1)
/** ENS210  SensStat register: H sensor active */
#define ENS210_SENSSTAT_H_STAT_ACTIVE           (1 << 1)

#define ENS210_CMD_LENGTH                            2
#define ENS210_RESPONSE_LENGTH                       6
#define ENS210_DATA_READ_MIN_INTERVAL              1000



/**
 * @brief    ENS210 ID block structure
 */
typedef struct ENS210_Ids_s
{
    uint16_t    partId;             /*!< Part ID */
    uint8_t     uId[8];             /*!< Unique Identifier 8 bytes */
} ENS210_Ids_t;

typedef struct {
    int32_t temperature;
    uint32_t humidity;
} ens210_sensor_data;

ens210_sensor_data g_ens210_data = {0};

typedef enum {
    FLAG_INIT_TEMP = 0,
    FLAG_INIT_HUMI,
} FLAG_INIT_BIT;

static uint8_t g_init_bitwise = 0;

i2c_dev_t ens210_ctx = {
    .port = 3,
    .config.address_width = 8,
    .config.freq = 100000,
    .config.dev_addr = ENS210_I2C_ADDR,
};
extern long long aos_now_ms(void);

// Compute the CRC-7 of 'val' (should only have 17 bits)
uint32_t crc7(uint32_t val) 
{
    // Setup polynomial
    uint32_t pol= CRC7POLY;
    // Align polynomial with data
    pol = pol << (DATA7WIDTH-CRC7WIDTH-1);
    // Loop variable (indicates which bit to test, start with highest)
    uint32_t bit = DATA7MSB;
    // Make room for CRC value
    val = val << CRC7WIDTH;
    bit = bit << CRC7WIDTH;
    pol = pol << CRC7WIDTH;
    // Insert initial vector
    val |= CRC7IVEC;
    // Apply division until all bits done
    while( bit & (DATA7MASK<<CRC7WIDTH) ) {
    if( bit & val ) val ^= pol;
    bit >>= 1;
    pol >>= 1;
    }
  return val;
}

static int drv_ens210_read_temp_and_humi(i2c_dev_t* drv, int32_t *temperature, uint32_t *humidity)
{
    int ret = 0;
    uint8_t temp_data[3] = {0};
    uint8_t humi_data[3] = {0};
    uint8_t cmd;
    
    cmd = ENS210_SYSCTRL_LOWPOWER_DISABLE;
    ret = sensor_i2c_write(&ens210_ctx, ENS210_REG_SYS_CTRL, &cmd, 1, I2C_OP_RETRIES);
    if(unlikely(ret)){
       return ret;
    }  
    
    cmd = (ENS210_SENSRUN_T_MODE_SINGLE_SHOT | ENS210_SENSRUN_H_MODE_SINGLE_SHOT);
    ret = sensor_i2c_write(&ens210_ctx, ENS210_REG_SENS_RUN, &cmd, 1, I2C_OP_RETRIES);
    if(unlikely(ret)){
       return ret;
    }  

    cmd = (ENS210_SENSSTART_T_START | ENS210_SENSSTART_H_START);
    ret = sensor_i2c_write(&ens210_ctx, ENS210_REG_SENS_START, &cmd, 1, I2C_OP_RETRIES);
    if(unlikely(ret)){
       return ret;
    }  

    if (temperature == NULL && humidity == NULL)
        return -1;
    
    ret  = sensor_i2c_read(&ens210_ctx, ENS210_REG_T_VAL, temp_data, 3, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }
    
    uint32_t t_val = (temp_data[2]<<16) + (temp_data[1]<<8) + (temp_data[0]<<0);

    // Extract (and print) the fields
    uint32_t t_data = (t_val>>0 ) & 0xffff;
    uint32_t t_valid= (t_val>>16) & 0x1;
    uint32_t t_crc = (t_val>>17) & 0x7f;

    // Check the CRC
    uint32_t t_payl = (t_val>>0 ) & 0x1ffff;
    bool t_crc_ok= crc7(t_payl)==t_crc;
    // Convert to float (and print)
    float TinK = (float)t_data / 64; // Temperature in Kelvin
    float TinC = TinK - 273.15; // Temperature in Celsius
    float TinF = TinC * 1.8 + 32.0; // Temperature in Fahrenheit
	
    ret  = sensor_i2c_read(&ens210_ctx, ENS210_REG_H_VAL, humi_data, 3, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    uint32_t h_val = (humi_data[2]<<16) + (humi_data[1]<<8) + (humi_data[0]<<0);
    
    // Extract (and print) the fields
    uint32_t h_data = (h_val>>0 ) & 0xffff;
    uint32_t h_valid= (h_val>>16) & 0x1;
    uint32_t h_crc = (h_val>>17) & 0x7f;

    // Check the CRC
    uint32_t h_payl = (h_val>>0 ) & 0x1ffff;
    bool h_crc_ok= (crc7(h_payl)==h_crc);
    // Convert to float (and print)
    float H = (float)h_data/512; // relative humidity (in %)
    
    *temperature = (int32_t)(TinC*10); // Temperature in Celsius
    *humidity = (uint32_t)(H*10);  // relative humidity (in ‰)

    (void)h_crc_ok;
    (void)h_valid;
    (void)TinF;
    (void)t_crc_ok;
    (void)t_valid;

    return 0;
}

UNUSED static int drv_ens210_update_data(i2c_dev_t* drv)
{
    static uint32_t prev_update_tick = 0;
    uint32_t now_tick = 0;
    int ret = 0;

     now_tick = aos_now_ms();
    if (now_tick - prev_update_tick > ENS210_DATA_READ_MIN_INTERVAL) {
        ret = drv_ens210_read_temp_and_humi(drv, &g_ens210_data.temperature, &g_ens210_data.humidity);
        if (ret != 0) {
            return ret;
        }
        prev_update_tick = now_tick;
    }

    return 0;
}

static int drv_temp_humi_ams_ens210_validate_id(i2c_dev_t* drv, uint8_t id_value)
{
    int     ret = 0;
    uint8_t value, value2;
    ENS210_Ids_t ids;

    if(drv == NULL){
        return -1;
    }
    
    value = 0x00;
    ret = sensor_i2c_write(drv, ENS210_REG_SYS_CTRL, &value, 1, I2C_OP_RETRIES);
    if(unlikely(ret)){
       return ret;
    }

    ret = sensor_i2c_read(drv, ENS210_REG_SYS_STAT, &value2, 1, I2C_OP_RETRIES);
    if(unlikely(ret)){
       return ret;
    }

    ret = sensor_i2c_read(drv, ENS210_REG_PART_ID, (uint8_t*)&ids.partId, 2, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

    if (id_value != value){
        return -1;
    }
    return 0;
}


UNUSED static int  drv_temp_humi_ams_ens210_soft_reset(i2c_dev_t* drv)
{
    int     ret = 0;
    uint8_t value, value2;

    if(drv == NULL){
        return -1;
    }
    
    value = ENS210_SYSCTRL_RESET_ENABLE;
    ret = sensor_i2c_write(drv, ENS210_REG_SYS_CTRL, &value, 1, I2C_OP_RETRIES);
    if(unlikely(ret)){
       return ret;
    }

    ret = sensor_i2c_read(drv, ENS210_REG_SYS_STAT, &value2, 1, I2C_OP_RETRIES);
    if(unlikely(ret)){
       return ret;
    }

    return 0;
}

static int drv_temp_humi_ams_ens210_set_power_mode(i2c_dev_t* drv, dev_power_mode_e mode)
{
    return 0;
}

UNUSED static int drv_temp_humi_ams_ens210_set_default_config(i2c_dev_t* drv)
{
     int     ret = 0;
    uint8_t value = 0x00;  

    ret = sensor_i2c_read(&ens210_ctx, ENS210_REG_SYS_STAT, &value, 1, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

    ret = drv_temp_humi_ams_ens210_set_power_mode(&ens210_ctx, DEV_SLEEP);
    if(unlikely(ret)){
        return ret;
    }

    return 0;
}



UNUSED static int  drv_temp_humi_ams_ens210_get_calib_param(i2c_dev_t* drv)
{
    return 0;
}

UNUSED static int drv_temp_humi_ams_ens210_set_work_mode(i2c_dev_t* drv,uint8_t mode)
{
    return 0;
}


static void drv_temp_ams_ens210_irq_handle(void)
{
    /* no handle so far */
}

static int drv_temp_ams_ens210_open(void)
{
    LOG("%s %s successfully \n", SENSOR_STR, __func__);

    return 0;
}

static int drv_temp_ams_ens210_close(void)
{
    LOG("%s %s successfully \n", SENSOR_STR, __func__);

    return 0;
}

static int drv_temp_ams_ens210_read(void *buf, size_t len)
{
    int ret = 0;
    size_t size;
    temperature_data_t* pdata = (temperature_data_t*)buf;

    if(buf == NULL){
        return -1;
    }

    size = sizeof(temperature_data_t);
    if(len < size){
        return -1;
    }

    //ret = drv_ens210_update_data(&ens210_ctx);
    ret = drv_ens210_read_temp_and_humi(&ens210_ctx, &g_ens210_data.temperature, &g_ens210_data.humidity);
    if (ret != 0)
        return -1;

    pdata->t = g_ens210_data.temperature;
    pdata->timestamp = aos_now_ms();

    return (int)size;
}

static int drv_temp_ams_ens210_write(const void *buf, size_t len)
{
    (void)buf;
    (void)len;
    return 0;
}

static int drv_temp_ams_ens210_ioctl(int cmd, unsigned long arg)
{
    switch (cmd) {
        case SENSOR_IOCTL_GET_INFO:{
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
            info->model = "ENS210";
            info->unit = dCelsius;
        }break;
        default:
            return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

static void drv_humi_ams_ens210_irq_handle(void)
{
    /* no handle so far */
}

static int drv_humi_ams_ens210_open(void)
{
    LOG("%s %s successfully \n", SENSOR_STR, __func__);

    return 0;
}

static int drv_humi_ams_ens210_close(void)
{
    LOG("%s %s successfully \n", SENSOR_STR, __func__);

    return 0;
}

static int drv_humi_ams_ens210_read(void *buf, size_t len)
{
    int ret = 0;
    size_t size;
    humidity_data_t* pdata = (humidity_data_t*)buf;

    if(buf == NULL){
        return -1;
    }

    size = sizeof(humidity_data_t);
    if(len < size){
        return -1;
    }

    ret = drv_ens210_read_temp_and_humi(&ens210_ctx, &g_ens210_data.temperature, &g_ens210_data.humidity);
    if (ret != 0)
        return -1;

    pdata->h = g_ens210_data.humidity;

    pdata->timestamp = aos_now_ms();

    return (int)size;
}

static int drv_humi_ams_ens210_write(const void *buf, size_t len)
{
    (void)buf;
    (void)len;
    return 0;
}

static int drv_humi_ams_ens210_ioctl(int cmd, unsigned long arg)
{
    switch (cmd) {
        case SENSOR_IOCTL_GET_INFO:{
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
            info->model = "ENS210";
            info->unit = permillage;
        }break;
        default:
            return -1;
    }
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

int drv_temp_ams_ens210_init(void)
{
    int ret = 0;
    sensor_obj_t sensor_temp;

    memset(&sensor_temp, 0, sizeof(sensor_temp));
    if (!g_init_bitwise) {

        ret = drv_temp_humi_ams_ens210_validate_id(&ens210_ctx, ENS210_REG_PART_ID);
        if (unlikely(ret)) {
            return -1;
        }
    }

    if (!(g_init_bitwise & (1 << FLAG_INIT_TEMP))) {
        /* fill the sensor_temp obj parameters here */
        sensor_temp.tag = TAG_DEV_TEMP;
        sensor_temp.path = dev_temp_path;
        sensor_temp.io_port = I2C_PORT;
        sensor_temp.open = drv_temp_ams_ens210_open;
        sensor_temp.close = drv_temp_ams_ens210_close;
        sensor_temp.read = drv_temp_ams_ens210_read;
        sensor_temp.write = drv_temp_ams_ens210_write;
        sensor_temp.ioctl = drv_temp_ams_ens210_ioctl;
        sensor_temp.irq_handle = drv_temp_ams_ens210_irq_handle;

        ret = sensor_create_obj(&sensor_temp);
        if (unlikely(ret)) {
            return -1;
        }

        g_init_bitwise |= 1 << FLAG_INIT_TEMP;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}


int drv_humi_ams_ens210_init(void)
{
    int ret = 0;
    sensor_obj_t sensor_humi;
    memset(&sensor_humi, 0, sizeof(sensor_humi));
    if (!g_init_bitwise) {

        ret = drv_temp_humi_ams_ens210_validate_id(&ens210_ctx, ENS210_REG_PART_ID);
        if (unlikely(ret)) {
            return -1;
        }
     }

   if (!(g_init_bitwise & (1 << FLAG_INIT_HUMI))) {
        /* fill the sensor_humi obj parameters here */
        sensor_humi.tag = TAG_DEV_HUMI;
        sensor_humi.path = dev_humi_path;
        sensor_humi.io_port = I2C_PORT;
        sensor_humi.open = drv_humi_ams_ens210_open;
        sensor_humi.close = drv_humi_ams_ens210_close;
        sensor_humi.read = drv_humi_ams_ens210_read;
        sensor_humi.write = drv_humi_ams_ens210_write;
        sensor_humi.ioctl = drv_humi_ams_ens210_ioctl;
        sensor_humi.irq_handle = drv_humi_ams_ens210_irq_handle;

        ret = sensor_create_obj(&sensor_humi);
        if (unlikely(ret)) {
            return -1;
        }

       g_init_bitwise |= 1 << FLAG_INIT_HUMI;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

SENSOR_DRV_ADD(drv_temp_ams_ens210_init);
SENSOR_DRV_ADD(drv_humi_ams_ens210_init);