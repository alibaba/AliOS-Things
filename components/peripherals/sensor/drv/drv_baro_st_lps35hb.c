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

/* ST BARO SENSOR REGISTER MAP */
#define LPS35HB_BIT(x)               ((uint8_t)x)
#define LPS35HB_ADDRESS              (uint8_t)0xB8
#define LPS35HB_DriverVersion_Major  (uint8_t)1
#define LPS35HB_DriverVersion_Minor  (uint8_t)0
#define LPS35HB_DriverVersion_Point  (uint8_t)0
#define LPS35HB_WHO_AM_I_REG         (uint8_t)0x0F
#define LPS35HB_WHO_AM_I_VAL         (uint8_t)0xB1
#define LPS35HB_REF_P_XL_REG         (uint8_t)0x15
#define LPS35HB_REF_P_L_REG          (uint8_t)0x16
#define LPS35HB_REF_P_H_REG          (uint8_t)0x17
#define LPS35HB_RES_CONF_REG         (uint8_t)0x1A
#define LPS35HB_RES_CONF_REG         (uint8_t)0x1A
#define LPS35HB_RES_CONF_REG         (uint8_t)0x1A
#define LPS35HB_LCEN_MASK            (uint8_t)0x01
#define LPS35HB_LCEN_POWERON         (uint8_t)0x00
#define LPS35HB_LCEN_LOWPOWER        (uint8_t)0x01
#define LPS35HB_CTRL_REG1            (uint8_t)0x10
#define LPS35HB_ODR_MASK             (uint8_t)0x70
#define LPS35HB_LPFP_MASK            (uint8_t)0x08
#define LPS35HB_LPFP_CUTOFF_MASK     (uint8_t)0x04
#define LPS35HB_BDU_MASK             (uint8_t)0x02
#define LPS35HB_SIM_MASK             (uint8_t)0x01
#define LPS35HB_LPFP_BIT             LPS35HB_BIT(3)
#define LPS35HB_CTRL_REG2            (uint8_t)0x11
#define LPS35HB_BOOT_BIT             LPS35HB_BIT(7)
#define LPS35HB_FIFO_EN_BIT          LPS35HB_BIT(6)
#define LPS35HB_WTM_EN_BIT           LPS35HB_BIT(5)
#define LPS35HB_ADD_INC_BIT          LPS35HB_BIT(4)
#define LPS35HB_I2C_BIT              LPS35HB_BIT(3)
#define LPS35HB_SW_RESET_BIT         LPS35HB_BIT(2)
#define LPS35HB_FIFO_EN_MASK         (uint8_t)0x40
#define LPS35HB_WTM_EN_MASK          (uint8_t)0x20
#define LPS35HB_ADD_INC_MASK         (uint8_t)0x10
#define LPS35HB_I2C_MASK             (uint8_t)0x08
#define LPS35HB_ONE_SHOT_MASK        (uint8_t)0x01
#define LPS35HB_CTRL_REG3            (uint8_t)0x12
#define LPS35HB_PP_OD_BIT            LPS35HB_BIT(6)
#define LPS35HB_FIFO_FULL_BIT        LPS35HB_BIT(5)
#define LPS35HB_FIFO_FTH_BIT         LPS35HB_BIT(4)
#define LPS35HB_FIFO_OVR_BIT         LPS35HB_BIT(3)
#define LPS35HB_DRDY_BIT             LPS35HB_BIT(2)
#define LPS35HB_INT_H_L_MASK         (uint8_t)0x80
#define LPS35HB_PP_OD_MASK           (uint8_t)0x40
#define LPS35HB_FIFO_FULL_MASK       (uint8_t)0x20
#define LPS35HB_FIFO_FTH_MASK        (uint8_t)0x10
#define LPS35HB_FIFO_OVR_MASK        (uint8_t)0x08
#define LPS35HB_DRDY_MASK            (uint8_t)0x04
#define LPS35HB_INT_S12_MASK         (uint8_t)0x03
#define LPS35HB_INTERRUPT_CFG_REG    (uint8_t)0x0B
#define LPS35HB_DIFF_EN_BIT          LPS35HB_BIT(3)
#define LPS35HB_LIR_BIT              LPS35HB_BIT(2)
#define LPS35HB_PLE_BIT              LPS35HB_BIT(1)
#define LPS35HB_PHE_BIT              LPS35HB_BIT(0)
#define LPS35HB_AUTORIFP_MASK        (uint8_t)0x80
#define LPS35HB_RESET_ARP_MASK       (uint8_t)0x40
#define LPS35HB_AUTOZERO_MASK        (uint8_t)0x20
#define LPS35HB_RESET_AZ_MASK        (uint8_t)0x10
#define LPS35HB_DIFF_EN_MASK         (uint8_t)0x08
#define LPS35HB_LIR_MASK             (uint8_t)0x04
#define LPS35HB_PLE_MASK             (uint8_t)0x02
#define LPS35HB_PHE_MASK             (uint8_t)0x01
#define LPS35HB_INTERRUPT_SOURCE_REG (uint8_t)0x25
#define LPS35HB_BOOT_STATUS_BIT      LPS35HB_BIT(7)
#define LPS35HB_IA_BIT               LPS35HB_BIT(2)
#define LPS35HB_PL_BIT               LPS35HB_BIT(1)
#define LPS35HB_PH_BIT               LPS35HB_BIT(0)
#define LPS35HB_BOOT_STATUS_MASK     (uint8_t)0x80
#define LPS35HB_IA_MASK              (uint8_t)0x04
#define LPS35HB_PL_MASK              (uint8_t)0x02
#define LPS35HB_PH_MASK              (uint8_t)0x01
#define LPS35HB_STATUS_REG           (uint8_t)0x27
#define LPS35HB_TOR_BIT              LPS35HB_BIT(5)
#define LPS35HB_POR_BIT              LPS35HB_BIT(4)
#define LPS35HB_TDA_BIT              LPS35HB_BIT(1)
#define LPS35HB_PDA_BIT              LPS35HB_BIT(0)
#define LPS35HB_TOR_MASK             (uint8_t)0x20
#define LPS35HB_POR_MASK             (uint8_t)0x10
#define LPS35HB_TDA_MASK             (uint8_t)0x02
#define LPS35HB_PDA_MASK             (uint8_t)0x01
#define LPS35HB_PRESS_OUT_XL_REG     (uint8_t)0x28
#define LPS35HB_PRESS_OUT_L_REG      (uint8_t)0x29
#define LPS35HB_PRESS_OUT_H_REG      (uint8_t)0x2A
#define LPS35HB_TEMP_OUT_L_REG       (uint8_t)0x2B
#define LPS35HBH_TEMP_OUT_H_REG      (uint8_t)0x2C
#define LPS35HB_THS_P_LOW_REG        (uint8_t)0x0C
#define LPS35HB_THS_P_HIGH_REG       (uint8_t)0x0D
#define LPS35HB_CTRL_FIFO_REG        (uint8_t)0x14
#define LPS35HB_FIFO_MODE_MASK       (uint8_t)0xE0
#define LPS35HB_WTM_POINT_MASK       (uint8_t)0x1F
#define LPS35HB_STATUS_FIFO_REG      (uint8_t)0x26
#define LPS35HB_FTH_FIFO_BIT         LPS35HB_BIT(7)
#define LPS35HB_OVR_FIFO_BIT         LPS35HB_BIT(6)
#define LPS35HB_FTH_FIFO_MASK        (uint8_t)0x80
#define LPS35HB_OVR_FIFO_MASK        (uint8_t)0x40
#define LPS35HB_LEVEL_FIFO_MASK      (uint8_t)0x3F
#define LPS35HB_FIFO_EMPTY           (uint8_t)0x00
#define LPS35HB_FIFO_FULL            (uint8_t)0x18
#define LPS35HB_RPDS_H_REG           (uint8_t)0x19
#define LPS35HB_CLOCK_TREE_CONFIG    (uint8_t)0x43
#define LPS35HB_CTE_MASK             (uint8_t)0x20

#define LPS35HB_I2C_ADDR1_R          (uint8_t)0xB9
#define LPS35HB_I2C_ADDR1_W          (uint8_t)0xB8
#define LPS35HB_I2C_ADDR2_R          (uint8_t)0xBB
#define LPS35HB_I2C_ADDR2_W          (uint8_t)0xBZ

#define LPS35HB_I2C_ADDR1            (0x5C)
#define LPS35HB_I2C_ADDR2            (0x5D)
#define LPS35HB_I2C_ADDR_TRANS(n)    ((n)<<1)  
#define LPS35HB_I2C_ADDR             LPS35HB_I2C_ADDR_TRANS(LPS35HB_I2C_ADDR2)


typedef enum {
  LPS35HB_ODR_ONE_SHOT        = (uint8_t)0x00,         /*!< Output Data Rate: one shot */
  LPS35HB_ODR_1HZ             = (uint8_t)0x10,         /*!< Output Data Rate: 1Hz */
  LPS35HB_ODR_10HZ            = (uint8_t)0x20,         /*!< Output Data Rate: 10Hz */
  LPS35HB_ODR_25HZ            = (uint8_t)0x30,         /*!< Output Data Rate: 25Hz */
  LPS35HB_ODR_50HZ            = (uint8_t)0x40,         /*!< Output Data Rate: 50Hz */
  LPS35HB_ODR_75HZ            = (uint8_t)0x50          /*!< Output Data Rate: 75Hz */
} lps35hb_odr_e;

typedef enum {
  LPS35HB_BDU_CONTINUOUS_UPDATE     =  (uint8_t)0x00,  /*!< Data updated continuously */
  LPS35HB_BDU_NO_UPDATE             =  (uint8_t)0x02   /*!< Data updated after a read operation */
} lps35hb_bdu_e;

i2c_dev_t lps35hb_ctx = {
    .port = 3,
    .config.address_width = 8,
    .config.freq = 400000,
    .config.dev_addr = LPS35HB_I2C_ADDR,
};

static lps35hb_odr_e drv_baro_st_lps35hb_hz2odr(int hz)
{
    if(hz > 50)
        return LPS35HB_ODR_75HZ;
    else if(hz > 25)
        return LPS35HB_ODR_50HZ;
    else if(hz > 10)
        return LPS35HB_ODR_25HZ;
    else if(hz > 1)
        return LPS35HB_ODR_10HZ;
    else
        return LPS35HB_ODR_1HZ;
}

static int drv_baro_st_lps35hb_validate_id(i2c_dev_t* drv, uint8_t id_value)
{
    uint8_t value = 0x00;
    int ret = 0;

    if(drv == NULL){
        return -1;
    }
    
    ret = sensor_i2c_read(drv, LPS35HB_WHO_AM_I_REG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    
    if (id_value != value){
        return -1;
    }
    return 0;
}

static int drv_baro_st_lps35hb_set_power_mode(i2c_dev_t* drv, dev_power_mode_e mode)
{
    uint8_t value = 0x00;
    int ret = 0;
    
    ret = sensor_i2c_read(drv, LPS35HB_RES_CONF_REG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

    switch(mode){
        case DEV_POWER_ON:{
            value &= ~LPS35HB_LCEN_MASK;
            value |= LPS35HB_LCEN_POWERON;
            ret = sensor_i2c_write(drv, LPS35HB_RES_CONF_REG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
            if(unlikely(ret)){
                return ret;
            }
        }break;
        
        case DEV_POWER_OFF:{
            value |= LPS35HB_LCEN_LOWPOWER;
            ret = sensor_i2c_write(drv, LPS35HB_RES_CONF_REG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
            if(unlikely(ret)){
                return ret;
            }
        }break;
        
       default:break;
    }
    return 0;
}

static int drv_baro_st_lps35hb_set_odr(i2c_dev_t* drv, lps35hb_odr_e odr)
{
    uint8_t value = 0x00;
    int ret = 0;
    
    ret = sensor_i2c_read(drv, LPS35HB_CTRL_REG1, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    value &= ~LPS35HB_ODR_MASK;
    value |= (uint8_t)odr;
    
    ret = sensor_i2c_write(drv, LPS35HB_CTRL_REG1, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    return 0;
}


static int drv_baro_st_lps35hb_enable_lpf(i2c_dev_t* drv)
{
    uint8_t value = 0x00;
    int ret = 0;
    
    ret = sensor_i2c_read(drv, LPS35HB_CTRL_REG1, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    value |= LPS35HB_LPFP_MASK | LPS35HB_LPFP_CUTOFF_MASK;

    ret = sensor_i2c_write(drv, LPS35HB_CTRL_REG1, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    return 0;
}

static int drv_baro_st_lps35hb_set_bdu(i2c_dev_t* drv, lps35hb_bdu_e bdu)
{
    uint8_t value = 0x00;
    int ret = 0;
    
    ret = sensor_i2c_read(drv, LPS35HB_CTRL_REG1, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    value &= ~LPS35HB_BDU_MASK;
    value |= (uint8_t)bdu;
    
    ret = sensor_i2c_write(drv, LPS35HB_CTRL_REG1, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    return 0;
}

static int drv_baro_st_lps35hb_set_default_config(i2c_dev_t* drv)
{
    int ret = 0;
    ret = drv_baro_st_lps35hb_set_power_mode(drv, DEV_POWER_OFF);
    if(unlikely(ret)){
        return ret;
    }
    ret = drv_baro_st_lps35hb_set_odr(drv, LPS35HB_ODR_25HZ);
    if(unlikely(ret)){
        return ret;
    }
    ret = drv_baro_st_lps35hb_enable_lpf(drv);
    if(unlikely(ret)){
        return ret;
    }
	
    ret = drv_baro_st_lps35hb_set_bdu(drv, LPS35HB_BDU_NO_UPDATE);
    if(unlikely(ret)){
        return ret;
    }
    /* you also can set the low-pass filter and cut off config here */
    return 0;
}


static void drv_baro_st_lps35hb_irq_handle(void)
{
    /* no handle so far */
}

static int drv_baro_st_lps35hb_open(void)
{
    int ret = 0;
    ret  = drv_baro_st_lps35hb_set_power_mode(&lps35hb_ctx, DEV_POWER_ON);
    if(unlikely(ret)){
        return -1;
    }
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;

}

static int drv_baro_st_lps35hb_close(void)
{
    int ret = 0;
    ret  = drv_baro_st_lps35hb_set_power_mode(&lps35hb_ctx, DEV_POWER_OFF);
    if(unlikely(ret)){
        return -1;
    }
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}


static int drv_baro_st_lps35hb_read(void *buf, size_t len)
{
    int ret = 0;
    size_t size;
    uint8_t data[3];
    barometer_data_t* pdata = (barometer_data_t*)buf;
    if(buf == NULL){
        return -1;
    }

    size = sizeof(barometer_data_t);
    if(len < size){
        return -1;
    }
    
    ret  = sensor_i2c_read(&lps35hb_ctx, LPS35HB_PRESS_OUT_XL_REG,  &data[0], I2C_DATA_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&lps35hb_ctx, LPS35HB_PRESS_OUT_L_REG,   &data[1], I2C_DATA_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&lps35hb_ctx, LPS35HB_PRESS_OUT_H_REG,   &data[2], I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return -1;
    }

   /* hatch the baro data here*/
    for(int i=0; i<3; i++){
      pdata->p |= (((uint32_t)data[i]) << (8*i));
    }
    
    /* convert the 2's complement 24 bit to 2's complement 32 bit */
    if((pdata->p & 0x00800000) != 0){
        pdata->p |= 0xFF000000;
    }
    pdata->p = ((pdata->p)*100)/4096;
    //pdata->p = pdata->p/100;
    pdata->timestamp = aos_now_ms();
    
    return (int)size;
}

static int drv_baro_st_lps35hb_write(const void *buf, size_t len)
{
    return 0;
}

static int drv_baro_st_lps35hb_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;
    
    switch(cmd){
        case SENSOR_IOCTL_ODR_SET:{
            lps35hb_odr_e odr = drv_baro_st_lps35hb_hz2odr(arg);
            ret = drv_baro_st_lps35hb_set_odr(&lps35hb_ctx, odr);
            if(unlikely(ret)){
                return -1;
            }
        }break;
        case SENSOR_IOCTL_SET_POWER:{
            ret = drv_baro_st_lps35hb_set_power_mode(&lps35hb_ctx, arg);
            if(unlikely(ret)){
                return -1;
            }
        }break;
        case SENSOR_IOCTL_GET_INFO:{ 
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
            info->model = "LPS35HB";
            info->range_max = 1260;
            info->range_min = 260;
            info->unit = pa;

        }break;
       
       default:break;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

int drv_baro_st_lps35hb_init(void){
    int ret = 0;
    sensor_obj_t sensor;
    memset(&sensor, 0, sizeof(sensor));

    /* fill the sensor obj parameters here */
    sensor.tag = TAG_DEV_BARO;
    sensor.path = dev_baro_path;
    sensor.io_port = I2C_PORT;
    sensor.open = drv_baro_st_lps35hb_open;
    sensor.close = drv_baro_st_lps35hb_close;
    sensor.read = drv_baro_st_lps35hb_read;
    sensor.write = drv_baro_st_lps35hb_write;
    sensor.ioctl = drv_baro_st_lps35hb_ioctl;
    sensor.irq_handle = drv_baro_st_lps35hb_irq_handle;


    ret = sensor_create_obj(&sensor);
    if(unlikely(ret)){
        return -1;
    }

    ret = drv_baro_st_lps35hb_validate_id(&lps35hb_ctx, LPS35HB_WHO_AM_I_VAL);
    if(unlikely(ret)){
        return -1;
    }
    /* set the default config for the sensor here */
    ret = drv_baro_st_lps35hb_set_default_config(&lps35hb_ctx);
    if(unlikely(ret)){
        return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

SENSOR_DRV_ADD(drv_baro_st_lps35hb_init);

