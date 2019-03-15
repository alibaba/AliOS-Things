/*
* Copyright (C) 2015-2017 Alibaba Group Holding Limited
*****   only for 1422a ,not for 1422 ***********
*	Edit wanjiang-yan 2018-6-26
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aos/kernel.h"
#include "network/hal/base.h"
#include "sensor_drv_api.h"
#include "sensor_hal.h"

// ********** ADDR = L ,addr=0x0E,ADDR = H,addr=0x0f ************
//#define ADDR_HIGH 1
#ifdef ADDR_HIGH
#define BM1422AGMV_I2C_ADDR1                   0x0F			//ADDR=H
#else
#define BM1422AGMV_I2C_ADDR1                   0x0E			//ADDR=L
#endif

#define BM1422AGMV_I2C_ADDR_TRANS(n)           ((n)<<1)  
#define BM1422AGMV_I2C_ADDR                    BM1422AGMV_I2C_ADDR_TRANS(BM1422AGMV_I2C_ADDR1)
#define BM1422AGMV_BIT(x)                      (x)

#define I_AM_BM1422AGMV                        0x41

#define BM1422AGMV_MAG_INFO1_REG            	0x0D
#define BM1422AGMV_MAG_INFO2_REG            	0x0E
#define BM1422AGMV_MAG_WHO_AM_I_REG            	0x0F
#define BM1422AGMV_MAG_OUTX_L                  0x10
#define BM1422AGMV_MAG_OUTX_H                  0x11
#define BM1422AGMV_MAG_OUTY_L                  0x12
#define BM1422AGMV_MAG_OUTY_H                  0x13
#define BM1422AGMV_MAG_OUTZ_L                  0x14
#define BM1422AGMV_MAG_OUTZ_H                  0x15
#define BM1422AGMV_MAG_STATUS_REG              0x18
#define BM1422AGMV_MAG_CNTL1_REG               0x1B
#define BM1422AGMV_MAG_CNTL2_REG               0x1C
#define BM1422AGMV_MAG_CNTL3_REG               0x1D
#define BM1422AGMV_MAG_PRET_REG                0x30
#define BM1422AGMV_MAG_AVE_A_REG               0x40
#define BM1422AGMV_MAG_CNTL4_REG               0x5C
#define BM1422AGMV_MAG_CNTL4_REG2               0x5D
#define BM1422AGMV_MAG_TEMP_OUT_L              0x60
#define BM1422AGMV_MAG_TEMP_OUT_H              0x61
#define BM1422AGMV_MAG_OFF_X_L                  0x6C
#define BM1422AGMV_MAG_OFF_X_H                  0x6D
#define BM1422AGMV_MAG_OFF_Y_L                  0x72
#define BM1422AGMV_MAG_OFF_Y_H                  0x73
#define BM1422AGMV_MAG_OFF_Z_L                  0x78
#define BM1422AGMV_MAG_OFF_Z_H                  0x79

#define BM1422AGMV_MAG_FINE_OUT_X_L             0x90
#define BM1422AGMV_MAG_FINE_OUT_X_H             0x91
#define BM1422AGMV_MAG_FINE_OUT_Y_L             0x92
#define BM1422AGMV_MAG_FINE_OUT_Y_H             0x93
#define BM1422AGMV_MAG_FINE_OUT_Z_L             0x94
#define BM1422AGMV_MAG_FINE_OUT_Z_H             0x95

#define BM1422AGMV_MAG_GAIN_PARA_X_L            0x9C
#define BM1422AGMV_MAG_GAIN_PARA_X_H            0x9D
#define BM1422AGMV_MAG_GAIN_PARA_Y_L            0x9E
#define BM1422AGMV_MAG_GAIN_PARA_Y_H            0x9F
#define BM1422AGMV_MAG_GAIN_PARA_Z_L            0xA0
#define BM1422AGMV_MAG_GAIN_PARA_Z_H            0xA1

#define INFORMATION_BM1422AGMV                  0x0101

#define I_AM_BM1422AGMV                         0x41


//0X18 STA1 BIT 6
#define BM1422AGMV_MAG_STATUS_RD_DRDY_BIT              0x40

//CNTL1 0X1B
#define BM1422AGMV_MAG_CNTL1_ODR_BIT                 BM1422AGMV_BIT(0x18)
#define BM1422AGMV_MAG_CNTL1_ODR_10_HZ               0x00 
#define BM1422AGMV_MAG_CNTL1_ODR_20_HZ               0x10
#define BM1422AGMV_MAG_CNTL1_ODR_100_HZ              0x08
#define BM1422AGMV_MAG_CNTL1_ODR_1000_HZ             0x18  

#define BM1422AGMV_MAG_CNTL1_FS1_BIT                 BM1422AGMV_BIT(0x02)
#define BM1422AGMV_MAG_CNTL1_FS1_CONTINUOUS          0x00
#define BM1422AGMV_MAG_CNTL1_FS1_SINGLE              0x02

#define BM1422AGMV_MAG_CNTL1_PC1_BIT                 BM1422AGMV_BIT(0x80)
#define BM1422AGMV_MAG_CNTL1_PC1_POWERDOWN           0x00
#define BM1422AGMV_MAG_CNTL1_PC1_ACTIVE              0x80
#define BM1422AGMV_MAG_CNTL1_OUT_BIT                 BM1422AGMV_BIT(0x40)
#define BM1422AGMV_MAG_CNTL1_OUT12_BIT               0x40
#define BM1422AGMV_MAG_CNTL1_OUT14_BIT               0x00


#define BM1422AGMV_MAG_CNTL1_RST_LV_BIT              BM1422AGMV_BIT(0x20)



//CNTL2 0X1C
#define BM1422AGMV_MAG_CNTL2_DRP_BIT              BM1422AGMV_BIT(0x04)
#define BM1422AGMV_MAG_CNTL2_DREN_BIT             BM1422AGMV_BIT(0x08)

//CNTL3 0X1D
#define BM1422AGMV_MAG_CNTL3_FORCE_BIT            BM1422AGMV_BIT(0x40)

//PRET 0X30
#define BM1422AGMV_MAG_PRET_PS_BIT   		      BM1422AGMV_BIT(0x01)

//AVE 0X40
#define BM1422AGMV_MAG_AVE_A_BIT   		      	  BM1422AGMV_BIT(0x1C)

//CNTL4 0X5D
#define BM1422AGMV_MAG_CNTL4_RSTB_LV_BIT          BM1422AGMV_BIT(0x3F)

#define DEV_POWER_ON	1
#define DEV_POWER_OFF	0



i2c_dev_t bm1422a_ctx = {
    .port = 3,
    .config.address_width = 8,
    .config.freq = 400000,
    .config.dev_addr = BM1422AGMV_I2C_ADDR,
};
uint16_t offx_dat,offy_dat,offz_dat;


static int drv_mag_rohm_bm1422a_soft_reset(i2c_dev_t* drv)
{
    int ret = 0;
    uint8_t value = BM1422AGMV_MAG_CNTL1_PC1_ACTIVE;
    ret = sensor_i2c_write(drv, BM1422AGMV_MAG_CNTL1_REG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return -1;
    }
    return 0;
}

UNUSED static int drv_mag_rohm_bm1422a_selftest(i2c_dev_t* drv)
{
    /*
    int ret = 0;
    ret = sensor_i2c_write(drv, BM1422AGMV_MAG_CTRL_REG1, LMS303C_MAG_SELFTEST_ENABLE, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return -1;
    }
    */
    return 0;
}

UNUSED static int drv_mag_rohm_bm1422a_reboot(i2c_dev_t* drv)
{
    /*
    int ret = 0;
    ret = sensor_i2c_write(drv, BM1422AGMV_MAG_CNTL2_REG, BM1422AGMV_MAG_REBOOT_ENABLE, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return -1;
    }*/
    return 0;
}

static int drv_mag_rohm_bm1422a_validate_id(i2c_dev_t* drv, uint8_t id_value)
{
    uint8_t value = 0x00;
    int ret = 0;

    if(drv == NULL){
        return -1;
    }
    
    ret = sensor_i2c_read(drv, BM1422AGMV_MAG_WHO_AM_I_REG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    
    if (id_value != value){
        return -1;
    }
    //ok
    return 0;
}

//
static int drv_mag_rohm_bm1422a_set_power_mode(i2c_dev_t* drv, dev_power_mode_e mode)
{
    uint8_t value = 0x00;
    int ret = 0;
    
    ret = sensor_i2c_read(drv, BM1422AGMV_MAG_CNTL1_REG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

    if (mode == DEV_POWER_ON)
    {
        value &= ~BM1422AGMV_MAG_CNTL1_PC1_BIT;
        value |= BM1422AGMV_MAG_CNTL1_PC1_ACTIVE;
    }
    else{
        value &= ~BM1422AGMV_MAG_CNTL1_PC1_BIT;
        value |= BM1422AGMV_MAG_CNTL1_PC1_POWERDOWN;
    }
    
    ret = sensor_i2c_write(drv, BM1422AGMV_MAG_CNTL1_REG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

    return 0;
}

//
static int drv_mag_rohm_bm1422a_set_odr(i2c_dev_t* drv, uint8_t odr)
{
    uint8_t value = 0x00;
    int ret = 0;
    
    ret = sensor_i2c_read(drv, BM1422AGMV_MAG_CNTL1_REG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    value &= ~BM1422AGMV_MAG_CNTL1_ODR_BIT;
    value |= (uint8_t)odr;
    ret = sensor_i2c_write(drv, BM1422AGMV_MAG_CNTL1_REG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    return 0;
}

//single mode or continuous mode
UNUSED static int drv_mag_rohm_bm1422a_set_mode(i2c_dev_t* drv, uint8_t mode)
{
    uint8_t value = 0x00;
    int ret = 0;
    
    ret = sensor_i2c_read(drv, BM1422AGMV_MAG_CNTL1_REG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    value &= ~BM1422AGMV_MAG_CNTL1_FS1_BIT;
    value |= (uint8_t)mode;
    ret = sensor_i2c_write(drv, BM1422AGMV_MAG_CNTL1_REG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    return 0;
}

//Output bit 12bit 14bit
UNUSED static int drv_mag_rohm_bm1422a_set_outmode(i2c_dev_t* drv, uint8_t bit_wide)
{
    uint8_t value = 0x00;
    int ret = 0;
    
    ret = sensor_i2c_read(drv, BM1422AGMV_MAG_CNTL1_REG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    value &= ~BM1422AGMV_MAG_CNTL1_OUT_BIT;
    value |= (uint8_t)bit_wide;
    ret = sensor_i2c_write(drv, BM1422AGMV_MAG_CNTL1_REG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    return 0;
}

//
static int drv_mag_rohm_bm1422a_set_cntl1(i2c_dev_t* drv, uint8_t value)
{
    int ret = 0;

    ret = sensor_i2c_write(drv, BM1422AGMV_MAG_CNTL1_REG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    return 0;
}


static int drv_mag_rohm_bm1422a_set_step1(i2c_dev_t* drv, uint16_t value)
{
	//uint8_t value = 0x00;
	int ret = 0;
	
	//5C //5D
	//value = 0X00;
	ret = sensor_i2c_write(drv, BM1422AGMV_MAG_CNTL4_REG, (uint8_t*)&value, 2, I2C_OP_RETRIES);
	if(unlikely(ret)){
		return ret;
	}
	
	
	return 0;


}

static int drv_mag_rohm_bm1422a_set_step2(i2c_dev_t* drv, uint8_t value)
{
	//uint8_t value = 0x00;
	int ret = 0;

	//value = BM1422AGMV_MAG_CNTL2_DRP_BIT | BM1422AGMV_MAG_CNTL2_DREN_BIT;
	ret = sensor_i2c_write(drv, BM1422AGMV_MAG_CNTL2_REG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
	if(unlikely(ret)){
	   return ret;
	}
	return 0;

}

static int drv_mag_rohm_bm1422a_set_step4(i2c_dev_t* drv, uint8_t value)
{
	//uint8_t value = 0x00;
	int ret = 0;

	//value = BM1422AGMV_MAG_CNTL3_FORCE_BIT;
	ret = sensor_i2c_write(drv, BM1422AGMV_MAG_CNTL3_REG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
	if(unlikely(ret)){
	   return ret;
	}
	return 0;

}


// offset x,y,z step 3
static int drv_mag_rohm_bm1422a_set_x_offset(i2c_dev_t* drv, uint16_t value)
{
    //uint8_t value = 0x00;
    int ret = 0;
 
    ret = sensor_i2c_write(drv, BM1422AGMV_MAG_OFF_X_L, (uint8_t*)&value, 2, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    return 0;
}

static int drv_mag_rohm_bm1422a_set_y_offset(i2c_dev_t* drv, uint16_t value)
{
    //uint8_t value = 0x00;
    int ret = 0;
    
    ret = sensor_i2c_write(drv, BM1422AGMV_MAG_OFF_Y_L, (uint8_t*)&value, 2, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    return 0;
}

static int drv_mag_rohm_bm1422a_set_z_offset(i2c_dev_t* drv, uint16_t value)
{
    //uint8_t value = 0x00;
    int ret = 0;
    
     ret = sensor_i2c_write(drv, BM1422AGMV_MAG_OFF_Z_L, (uint8_t*)&value, 2, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    return 0;
}

//select x,y,z
UNUSED static int drv_mag_rohm_bm1422a_offset_adjustment(i2c_dev_t* drv, uint8_t xyz_wide)
{
    int ret = 0;
//	int diff_x = 9999;
	uint8_t wk_dat = 1,off_dat=1,drdy=0,buffer[2];
	uint16_t data_temp,diff_x=9999;

	drv_mag_rohm_bm1422a_set_step1(drv,0x82);
    drv_mag_rohm_bm1422a_set_step2(drv,0x0C);
	switch(xyz_wide)
	{
	case 1:
		while(wk_dat<96)
		{
			drv_mag_rohm_bm1422a_set_x_offset(drv,wk_dat);
			drv_mag_rohm_bm1422a_set_step4(drv,0x40);
			do
			{
				
		  		ret = sensor_i2c_read(&bm1422a_ctx, BM1422AGMV_MAG_STATUS_REG, &drdy, I2C_DATA_LEN, I2C_OP_RETRIES);
			
			}while((drdy & BM1422AGMV_MAG_STATUS_RD_DRDY_BIT) != BM1422AGMV_MAG_STATUS_RD_DRDY_BIT);
	
			// read x,y,z data
			ret = sensor_i2c_read(&bm1422a_ctx, BM1422AGMV_MAG_OUTX_L , buffer, 2, I2C_OP_RETRIES);
            if(unlikely(ret)){
                return -1;
            }
			data_temp=((((uint16_t)buffer[1]) << 8) | (uint16_t)buffer[0]);
			if(diff_x > abs(data_temp))
			{
				off_dat = wk_dat;
				diff_x = abs(data_temp);
				offx_dat = off_dat;
			}
			wk_dat ++;
		}
		drv_mag_rohm_bm1422a_set_x_offset(drv,off_dat);
		break;
	case 2:
		while(wk_dat<96)
		{
			drv_mag_rohm_bm1422a_set_y_offset(drv,wk_dat);
			drv_mag_rohm_bm1422a_set_step4(drv,0x40);
			do
			{
				
		  		ret = sensor_i2c_read(&bm1422a_ctx, BM1422AGMV_MAG_STATUS_REG, &drdy, I2C_DATA_LEN, I2C_OP_RETRIES);
			
			}while((drdy & BM1422AGMV_MAG_STATUS_RD_DRDY_BIT) != BM1422AGMV_MAG_STATUS_RD_DRDY_BIT);
	
			// read x,y,z data
			ret = sensor_i2c_read(&bm1422a_ctx, BM1422AGMV_MAG_OUTY_L, buffer, 2, I2C_OP_RETRIES);
			data_temp=((((uint16_t)buffer[1]) << 8) | (uint16_t)buffer[0]);
			if(diff_x > abs(data_temp))
			{
				off_dat = wk_dat;
				diff_x = abs(data_temp);
				offy_dat = off_dat;
			}
			wk_dat ++;
		}
		drv_mag_rohm_bm1422a_set_y_offset(drv,off_dat);
		break;	
	case 3:
		while(wk_dat<96)
		{
			drv_mag_rohm_bm1422a_set_z_offset(drv,wk_dat);
			drv_mag_rohm_bm1422a_set_step4(drv,0x40);
			do
			{
				
		  		ret = sensor_i2c_read(&bm1422a_ctx, BM1422AGMV_MAG_STATUS_REG, &drdy, I2C_DATA_LEN, I2C_OP_RETRIES);
			
			}while((drdy & BM1422AGMV_MAG_STATUS_RD_DRDY_BIT) != BM1422AGMV_MAG_STATUS_RD_DRDY_BIT);
			// read x,y,z data
			
			ret = sensor_i2c_read(&bm1422a_ctx, BM1422AGMV_MAG_OUTZ_L, buffer, 2, I2C_OP_RETRIES);
			data_temp=((((uint16_t)buffer[1]) << 8) | (uint16_t)buffer[0]);
			if(diff_x > abs(data_temp))
			{
				off_dat = wk_dat;
				diff_x = abs(data_temp);
				offz_dat = off_dat;
			}
			wk_dat ++;
		}
		drv_mag_rohm_bm1422a_set_z_offset(drv,off_dat);
		break;
	default:
		break;
	}
	
    return 0;
}


static int drv_mag_rohm_bm1422a_set_default_config(i2c_dev_t* drv)
{
    /*
    uint8_t value = 0x00;
    ret = drv_mag_rohm_bm1422a_set_power_mode(drv, DEV_POWER_ON);
    if(unlikely(ret)){
        return ret;
    }
    ret = drv_mag_rohm_bm1422a_set_odr(drv, BM1422AGMV_MAG_CNTL1_ODR_10_HZ);
    if(unlikely(ret)){
        return ret;
    }
	//single or connutional
	ret = drv_mag_rohm_bm1422a_set_mode(drv, BM1422AGMV_MAG_CNTL1_FS1_CONTINUOUS);
	if(unlikely(ret)){
		return ret;
	}

	//single or connutional
	ret = drv_mag_rohm_bm1422a_set_mode(drv, BM1422AGMV_MAG_CNTL1_FS1_CONTINUOUS);
	if(unlikely(ret)){
		return ret;
	}

	ret = drv_mag_rohm_bm1422a_set_outmode(drv, BM1422AGMV_MAG_CNTL1_OUT12_BIT);
	if(unlikely(ret)){
		return ret;
	}
	*/
    //step 1
    drv_mag_rohm_bm1422a_set_cntl1(drv,0x80);
	drv_mag_rohm_bm1422a_set_step1(drv,0x0000);
    aos_msleep(10);
    //step 2
	drv_mag_rohm_bm1422a_set_step2(drv,0x0c);
    aos_msleep(10);
    //step 3

    //step 4
	drv_mag_rohm_bm1422a_set_step4(drv,0x40);
    aos_msleep(10);
    return 0;
}

static void drv_mag_rohm_bm1422a_irq_handle(void)
{
    /* no handle so far */
}

static int drv_mag_rohm_bm1422a_open(void)
{
    int ret = 0;
    ret  = drv_mag_rohm_bm1422a_set_power_mode(&bm1422a_ctx, DEV_POWER_ON);
    if(unlikely(ret)){
        return -1;
    }
    return 0;

}

static int drv_mag_rohm_bm1422a_close(void)
{
    int ret = 0;
    ret  = drv_mag_rohm_bm1422a_set_power_mode(&bm1422a_ctx, DEV_POWER_OFF);
    if(unlikely(ret)){
        return -1;
    }
    return 0;
}


///////////////////////////////////////////

static int drv_mag_rohm_bm1422a_read(void* buf, size_t len)
{
  int ret = 0;
  size_t size;
  int16_t data_temp[3];
  uint8_t drdy= 0;
  uint8_t buffer[6];
  uint8_t i = 0;
  
  mag_data_t* pdata = (mag_data_t*)buf;
    if(buf == NULL){
        return -1;
    }

    size = sizeof(mag_data_t);
    if(len < size){
        return -1;
    }
	
	drdy = 0;
	do{
		
  		ret = sensor_i2c_read(&bm1422a_ctx, BM1422AGMV_MAG_STATUS_REG, &drdy, I2C_DATA_LEN, I2C_OP_RETRIES);
	
	}while(drdy == 0);
	
    ret = 0;
    ret |= sensor_i2c_read(&bm1422a_ctx, BM1422AGMV_MAG_OUTX_L, &buffer[0], 1, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&bm1422a_ctx, BM1422AGMV_MAG_OUTX_H, &buffer[1], 1, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&bm1422a_ctx, BM1422AGMV_MAG_OUTY_L, &buffer[2], 1, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&bm1422a_ctx, BM1422AGMV_MAG_OUTY_H, &buffer[3], 1, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&bm1422a_ctx, BM1422AGMV_MAG_OUTZ_L, &buffer[4], 1, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&bm1422a_ctx, BM1422AGMV_MAG_OUTZ_H, &buffer[5], 1, I2C_OP_RETRIES);
    
    if(unlikely(ret)){
        return -1;
    }
    //ret = sensor_i2c_read(&bm1422a_ctx, BM1422AGMV_MAG_OUTX_L, buffer, 6, I2C_OP_RETRIES);

  
    for(i=0; i<3; i++)
    {
        data_temp[i]=(((buffer[2*i+1]) << 8) | buffer[2*i]);
    }
  
  //check x,y,z value is ok
  //
  for(i=0; i<3; i++)
  {
    //pdata->data[i] = ((data_temp[i] * sensitivity) >> 10);
    pdata->data[i] = data_temp[i] /6000;
  }
  pdata->timestamp = aos_now_ms();

  return (int)size;
}


static int drv_mag_rohm_bm1422a_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;
    
    switch(cmd){
        case SENSOR_IOCTL_ODR_SET:{
            ret = drv_mag_rohm_bm1422a_set_odr(&bm1422a_ctx, arg);
            if(unlikely(ret)){
                return -1;
            }
        }break;
        /*
        case SENSOR_IOCTL_RANGE_SET:{
            ret = drv_mag_rohm_bm1422a_set_range(&bm1422a_ctx, arg);
            if(unlikely(ret)){
                return -1;
            }
        }break;
        */
        case SENSOR_IOCTL_SET_POWER:{
            ret = drv_mag_rohm_bm1422a_set_power_mode(&bm1422a_ctx, arg);
            if(unlikely(ret)){
                return -1;
            }
        }break;
        case SENSOR_IOCTL_GET_INFO:{ 
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
            info->model = "BM1422AGMV";
            info->range_max = 8191;
            info->range_min = -8192;
            info->unit = mGauss;
        }break;
       
       default:break;
    }

    return 0;
}

int drv_mag_rohm_bm1422a_init(void){
    int ret = 0;
    sensor_obj_t sensor;
    memset(&sensor, 0, sizeof(sensor));
    /* fill the sensor obj parameters here */
    sensor.io_port    = I2C_PORT;
    sensor.tag        = TAG_DEV_MAG;
    sensor.path       = dev_mag_path;
    sensor.open       = drv_mag_rohm_bm1422a_open;
    sensor.close      = drv_mag_rohm_bm1422a_close;
    sensor.read       = drv_mag_rohm_bm1422a_read;
    sensor.write      = NULL;
    sensor.ioctl      = drv_mag_rohm_bm1422a_ioctl;
    sensor.irq_handle = drv_mag_rohm_bm1422a_irq_handle;

    ret = sensor_create_obj(&sensor);
    if(unlikely(ret)){
        return -1;
    }

    ret = drv_mag_rohm_bm1422a_validate_id(&bm1422a_ctx, I_AM_BM1422AGMV);
    if(unlikely(ret)){
        return -1;
    }
    
    ret = drv_mag_rohm_bm1422a_soft_reset(&bm1422a_ctx);
    if(unlikely(ret)){
        return -1;
    }
    
    ret = drv_mag_rohm_bm1422a_set_default_config(&bm1422a_ctx);
    if(unlikely(ret)){
        return -1;
    }
    
    /* update the phy sensor info to sensor hal */
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

SENSOR_DRV_ADD(drv_mag_rohm_bm1422a_init);


