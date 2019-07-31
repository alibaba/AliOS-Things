NAME := sensor

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := Hardware abstract layer for sensors

$(NAME)_SOURCES += \
        hal/sensor_hal.c \
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> b7b8639a0 (BugID: 17115493:add sensor compile file)
        hal/sensor_drv_api.c \
<<<<<<< HEAD
        drv/drv_temp_humi_baro_bosch_bme280.c \
        drv/drv_acc_bosch_bma253.c \
        drv/drv_baro_bosch_bmp280.c \
        drv/drv_acc_gyro_st_lsm6dsl.c\
        drv/drv_baro_st_lps22hb.c \
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
        drv/drv_acc_mir3_da213B.c \
        drv/drv_acc_mir3_da215.c \
        drv/drv_baro_st_lps33hb.c \
        drv/drv_baro_st_lps35hb.c \
>>>>>>> 056ff03e6 (updata sensitivity definition for sensor I3G4250D and L3GD20H)
=======
>>>>>>> 08bc8fba1 (release aos2.0.0)
=======
>>>>>>> b7b8639a0 (BugID: 17115493:add sensor compile file)
        drv/drv_acc_mir3_da217.c \
        drv/drv_als_ps_liteon_ltr553.c \
        drv/drv_temp_humi_sensirion_shtc1.c \
        drv/drv_temp_humi_st_hts221.c \
        drv/drv_mag_st_lis3mdl.c \
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
        drv/drv_mag_temp_memsic_mmc3680kj.c \
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
        drv/drv_baro_rohm_bm1383a.c \
        drv/drv_mag_rohm_bm1422a.c \
        drv/drv_als_liteon_ltr303.c \
        drv/drv_uv_liteon_ltr390.c \
        drv/drv_als_ps_liteon_ltr507.c \
        drv/drv_als_ps_liteon_ltr559.c \
        drv/drv_als_liteon_ltr568.c \
        drv/drv_ps_liteon_ltr659.c \
        drv/drv_ps_liteon_ltr690.c \
        drv/drv_ps_liteon_ltr706.c \
>>>>>>> 537f234b5 (BugID:16841468: sensor Marco definition typo (#549))
        drv/drv_als_ams_tcs3400.c  \
        drv/drv_als_ps_ams_tmd2725.c
=======
        drv/drv_temp_adi_adt7410.c \
        drv/drv_acc_adi_adxl355.c \
<<<<<<< HEAD
        drv/drv_acc_adi_adxl345.c
>>>>>>> e9139ca4c (add driver for adt7410 adxl345 adxl355)
=======
		drv/drv_als_liteon_ltr303.c \
		drv/drv_uv_liteon_ltr390.c \
		drv/drv_als_ps_liteon_ltr507.c \
		drv/drv_als_ps_liteon_ltr559.c \
		drv/drv_als_liteon_ltr568.c \
        drv/drv_ps_liteon_ltr659.c \
        drv/drv_ps_liteon_ltr690.c \
        drv/drv_ps_liteon_ltr706.c \
        #drv/drv_rgb_liteon_ltr381.c \
        #drv/drv_gs_liteon_ltr91100.c 
        
>>>>>>> 8cae6d447 (Added drivers for Lite-On sensors LTR303, LTR381, LTR390, LTR507, LTR559, LTR568, LTR659, LTR690, LTR706, LTR91100, consists of ambient light sensors, proximity sensors, UV sensors, RGB sensors, and gesture sensors.)
=======
=======
>>>>>>> ccbf6d929 (add driver for sensor BMP38x BMA4xy BMI160 BMI260)
        drv/drv_acc_gyro_bosch_bmi160.c \
        drv/drv_acc_gyro_bosch_bmi260.c \
        drv/drv_baro_bosch_bmp38x.c \
        drv/drv_acc_bosch_bma4xy.c
<<<<<<< HEAD
>>>>>>> f85ffb2ab (add driver for sensor BMP38x BMA4xy BMI160 BMI260)
=======
>>>>>>> ccbf6d929 (add driver for sensor BMP38x BMA4xy BMI160 BMI260)
		
=======
        drv/drv_baro_rohm_bm1383a.c \
	drv/drv_mag_rohm_bm1422a.c 	
>>>>>>> d378d15eb (rohm sensor)
=======
        drv/drv_acc_adi_adxl345.c \
        drv/drv_mag_st_lis2mdl.c \
        drv/drv_acc_mag_st_lsm303agr.c\
        drv/drv_acc_st_lis3dh.c \
        drv/drv_acc_st_lis2dh12.c \
        drv/drv_acc_st_lis2dw12.c \
        drv/drv_acc_st_lis2hh12.c \
        drv/drv_acc_st_n2dm.c \
        drv/drv_acc_st_ais328dq.c \
        drv/drv_acc_st_lis331hh.c \
        drv/drv_acc_st_h3lis331dl.c \
        drv/drv_acc_st_h3lis100dl.c \
        drv/drv_gyro_st_l3gd20h.c \
        drv/drv_gyro_st_i3g4250d.c \
        drv/drv_gyro_st_a3g4250d.c \
        drv/drv_mag_temp_memsic_mmc3680kj.c 
>>>>>>> 056ff03e6 (updata sensitivity definition for sensor I3G4250D and L3GD20H)
=======
        drv/drv_mag_temp_memsic_mmc3680kj.c
<<<<<<< HEAD
>>>>>>> 08bc8fba1 (release aos2.0.0)
=======
#		drv/drv_gr_baro_temp_hum_bosch_bme680/drv_gr_baro_temp_hum_bosch_bme680.c \
#        drv/drv_pm_sensirion_sps30/sensirion_common.c \
#        drv/drv_pm_sensirion_sps30/sps30.c \
#        drv/drv_pm_sensirion_sps30/drv_pm_sensirion_sps30.c 
>>>>>>> 6ad7c776d (I2C HAL for ESP32 fixed, native sensor support for Sensirion SPS30 and Bosch BME680 added. (#840))
=======
        drv/drv_mag_temp_memsic_mmc3680kj.c 
>>>>>>> 24c274278 (BugID:16179547: optimize industry gateway canbus sensor receive data mode)

        
=======
        hal/sensor_drv_api.c 
=======
        drv/drv_mag_temp_memsic_mmc3680kj.c 
>>>>>>> b7b8639a0 (BugID: 17115493:add sensor compile file)

=======
        hal/sensor_config.c
>>>>>>> 44f3c13a5 ( BugID:17532990: sensor automatic init)
=======
        hal/sensor_drv_api.c
>>>>>>> 178b10fab ( BugID:18506933:change config path)

CONFIG_SENSOR =  $(addsuffix .c,$(CONFIG_SENSOR_DRV_NAME))
SENSOR_ALL_FILE=$(notdir $(wildcard drivers/sensor/drv/*.c))

FILTER_FILE=$(foreach defines_var,$(1),$(filter $(defines_var),$(2)))
SENSOR_SRC_FILE=$(call FILTER_FILE,$(CONFIG_SENSOR),$(SENSOR_ALL_FILE))

$(NAME)_SOURCES += $(addprefix drv/,$(SENSOR_SRC_FILE))

CONFIG_DRV_SET = $(word 1,$(CONFIG_SENSOR_DRV_NAME))

$(NAME)_COMPONENTS += vfs

ifneq ($(CONFIG_DRV_SET),)
GLOBAL_DEFINES += SENSOR_DRV_AUTO_INIT
endif

<<<<<<< HEAD
>>>>>>> 0cac3b6d8 (BugID:17115375: sensor driver initialize dynamically)
ifeq ($(modbus_sensor_enable),1)
=======
ifeq ($(AOS_SENSOR_MODBUS_ENABLE),y)
<<<<<<< HEAD
>>>>>>> 44f3c13a5 ( BugID:17532990: sensor automatic init)
$(NAME)_SOURCES += drv/drv_modbus_sensors.c
=======
$(NAME)_SOURCES += drv/modbus_sensors/drv_modbus_sensors.c
<<<<<<< HEAD
>>>>>>> 4ab0e73c5 (BugID:16833564:fix sensor error return bug,adapt modbus sensor to new modbus lib)
$(NAME)_COMPONENTS  += kernel.bus.mbmaster
=======
$(NAME)_COMPONENTS  += mbmaster
>>>>>>> e5d87d740 (BugID: 18138458: header file included check)
GLOBAL_DEFINES += UDATA_MODBUS
endif

ifeq ($(AOS_SENSOR_INT_ENABLE),y)
GLOBAL_DEFINES += SENSOR_INT_ENABLE
endif

ifeq ($(AOS_SENSOR_I2C_ENABLE),y)
GLOBAL_DEFINES += UDATA_MEMS
GLOBAL_DEFINES += SENSOR_I2C_ENABLE
endif

ifeq ($(AOS_SENSOR_SPI_ENABLE),y)
GLOBAL_DEFINES += UDATA_MEMS
GLOBAL_DEFINES += SENSOR_SPI_ENABLE
endif

ifeq ($(AOS_SENSOR_GPS_SIMCOM_SIM868),y)
$(NAME)_COMPONENTS += atparser device_sal_sim800
$(NAME)_SOURCES += drv/drv_gps_simcom_sim868.c
GLOBAL_DEFINES += AOS_SENSOR_GPS_SIMCON_SIM868
endif

ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif

$(NAME)_INCLUDES +=  ./include
GLOBAL_DEFINES      += AOS_SENSOR
include $(SOURCE_ROOT)/drivers/sensor/drv.mk
<<<<<<< HEAD

<<<<<<< HEAD
<<<<<<< HEAD
#GLOBAL_DEFINES      += AOS_SENSOR_HUMI_BOSCH_BME280
#GLOBAL_DEFINES      += AOS_SENSOR_ACC_BOSCH_BMA253
#GLOBAL_DEFINES      += AOS_SENSOR_BARO_BOSCH_BMP280
#GLOBAL_DEFINES      += AOS_SENSOR_ACC_ST_LSM6DSL
#GLOBAL_DEFINES      += AOS_SENSOR_GYRO_ST_LSM6DSL
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
#GLOBAL_DEFINES      += AOS_SENSOR_ACC_ST_LSM6DS3TR_C
#GLOBAL_DEFINES      += AOS_SENSOR_GYRO_ST_LSM6DS3TR_C
#GLOBAL_DEFINES      += AOS_SENSOR_ACC_ST_LSM6DS3
#GLOBAL_DEFINES      += AOS_SENSOR_GYRO_ST_LSM6DS3
#GLOBAL_DEFINES      += AOS_SENSOR_ACC_ST_LSM6DSOQ
#GLOBAL_DEFINES      += AOS_SENSOR_GYRO_ST_LSM6DSOQ
#GLOBAL_DEFINES      += AOS_SENSOR_ACC_ST_LSM6DSM
#GLOBAL_DEFINES      += AOS_SENSOR_GYRO_ST_LSM6DSM
#GLOBAL_DEFINES      += AOS_SENSOR_ACC_ST_LSM6DSR
#GLOBAL_DEFINES      += AOS_SENSOR_GYRO_ST_LSM6DSR
=======
=======
>>>>>>> ccbf6d929 (add driver for sensor BMP38x BMA4xy BMI160 BMI260)
GLOBAL_DEFINES      += AOS_SENSOR_ACC_BOSCH_BMI160
GLOBAL_DEFINES      += AOS_SENSOR_GYRO_BOSCH_BMI160
GLOBAL_DEFINES      += AOS_SENSOR_ACC_BOSCH_BMI260
GLOBAL_DEFINES      += AOS_SENSOR_GYRO_BOSCH_BMI260
<<<<<<< HEAD
>>>>>>> f85ffb2ab (add driver for sensor BMP38x BMA4xy BMI160 BMI260)
=======
>>>>>>> ccbf6d929 (add driver for sensor BMP38x BMA4xy BMI160 BMI260)
=======
>>>>>>> 08bc8fba1 (release aos2.0.0)
#GLOBAL_DEFINES      += AOS_SENSOR_BARO_ST_LPS22HB
#GLOBAL_DEFINES      += AOS_SENSOR_ACC_SUPPORT_STEP
#GLOBAL_DEFINES      += AOS_SENSOR_MAG_ST_LIS3MDL

<<<<<<< HEAD
#GLOBAL_DEFINES      += AOS_SENSOR_ALS_LITEON_LTR553
#GLOBAL_DEFINES      += AOS_SENSOR_PS_LITEON_LTR553
#GLOBAL_DEFINES      += AOS_SENSOR_ALS_LITEON_LTR507
#GLOBAL_DEFINES      += AOS_SENSOR_PS_LITEON_LTR507
#GLOBAL_DEFINES      += AOS_SENSOR_ALS_LITEON_LTR559
#GLOBAL_DEFINES      += AOS_SENSOR_PS_LITEON_LTR559
#GLOBAL_DEFINES      += AOS_SENSOR_ALS_LITEON_LTR568
#GLOBAL_DEFINES      += AOS_SENSOR_PS_LITEON_LTR568
#GLOBAL_DEFINES      += AOS_SENSOR_ALS_LITEON_LTR303
#GLOBAL_DEFINES      += AOS_SENSOR_PS_LITEON_LTR690
#GLOBAL_DEFINES      += AOS_SENSOR_PS_LITEON_LTR659
#GLOBAL_DEFINES      += AOS_SENSOR_PS_LITEON_LTR706
#GLOBAL_DEFINES      += AOS_SENSOR_UV_LITEON_LTR390
#GLOBAL_DEFINES      += AOS_SENSOR_GS_LITEON_LTR91100
#GLOBAL_DEFINES      += AOS_SENSOR_RGB_LITEON_LTR381
=======
>>>>>>> 08bc8fba1 (release aos2.0.0)

#GLOBAL_DEFINES       += AOS_SENSOR_ACC_MIR3_DA217
#GLOBAL_DEFINES       += AOS_SENSOR_ALS_LITEON_LTR553
#GLOBAL_DEFINES       += AOS_SENSOR_PS_LITEON_LTR553
#GLOBAL_DEFINES      += AOS_SENSOR_TEMP_SENSIRION_SHTC1
#GLOBAL_DEFINES      += AOS_SENSOR_HUMI_SENSIRION_SHTC1
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
#GLOBAL_DEFINES       += AOS_SENSOR_ALS_AMS_TCS3400
#GLOBAL_DEFINES       += AOS_SENSOR_ALS_AMS_TMD2725
#GLOBAL_DEFINES       += AOS_SENSOR_PS_AMS_TMD2725
=======
=======
#GLOBAL_DEFINES      += AOS_SENSOR_TEMP_SENSIRION_SHT3X
#GLOBAL_DEFINES      += AOS_SENSOR_HUMI_SENSIRION_SHT3X
>>>>>>> 63ce5941e (add support for Sensirion SHT3x humidity/temp sensor)

#GLOBAL_DEFINES      += AOS_SENSOR_ACC_ADI_ADXL345
#GLOBAL_DEFINES      += AOS_SENSOR_ACC_ADI_ADXL355
#GLOBAL_DEFINES      += AOS_SENSOR_TEMP_ADI_ADT7410
=======

>>>>>>> d378d15eb (rohm sensor)
=======
GLOBAL_DEFINES      += AOS_SENSOR_BARO_BOSCH_BMP38X
GLOBAL_DEFINES      += AOS_SENSOR_ACC_BOSCH_BMA4XY
>>>>>>> f85ffb2ab (add driver for sensor BMP38x BMA4xy BMI160 BMI260)
=======
GLOBAL_DEFINES      += AOS_SENSOR_BARO_BOSCH_BMP38X
GLOBAL_DEFINES      += AOS_SENSOR_ACC_BOSCH_BMA4XY
>>>>>>> ccbf6d929 (add driver for sensor BMP38x BMA4xy BMI160 BMI260)

>>>>>>> e9139ca4c (add driver for adt7410 adxl345 adxl355)
=======

<<<<<<< HEAD
<<<<<<< HEAD
>>>>>>> 08bc8fba1 (release aos2.0.0)
=======
#  * Copyright (C) 2019 X-Cite SA (http://www.x-cite.io)
#  * Written by Lemmer El Assal (lemmer@x-cite.io)
#GLOBAL_DEFINES      += AOS_SENSOR_GR_BARO_TEMP_HUM_BOSCH_BME680
#GLOBAL_DEFINES      += AOS_SENSOR_PM_SENSIRION_SPS30
>>>>>>> 6ad7c776d (I2C HAL for ESP32 fixed, native sensor support for Sensirion SPS30 and Bosch BME680 added. (#840))
=======
=======
>>>>>>> 0cac3b6d8 (BugID:17115375: sensor driver initialize dynamically)

>>>>>>> 24c274278 (BugID:16179547: optimize industry gateway canbus sensor receive data mode)
=======
>>>>>>> cce10f460 (BugID:19137880:change driver init config)
=======
>>>>>>> 17aa01077 (BugID:19137880:change the config name)
