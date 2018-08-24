NAME := sensor

$(NAME)_TYPE := kernel

$(NAME)_SOURCES += \
        hal/sensor_hal.c \
        hal/sensor_drv_api.c \
        drv/drv_humi_bosch_bme280.c \
        drv/drv_acc_bosch_bma253.c \
        drv/drv_baro_bosch_bmp280.c \
        drv/drv_acc_gyro_st_lsm6dsl.c\
        drv/drv_baro_st_lps22hb.c \
        drv/drv_acc_mir3_da217.c \
        drv/drv_als_ps_liteon_ltr553.c \
        drv/drv_temp_humi_sensirion_shtc1.c \
        drv/drv_temp_humi_st_hts221.c \
        drv/drv_mag_st_lis3mdl.c \
        drv/drv_mag_temp_memsic_mmc3680kj.c \
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
        drv/drv_als_ams_tcs3400.c  \
        drv/drv_als_ps_ams_tmd2725.c
=======
        drv/drv_temp_adi_adt7410.c \
        drv/drv_acc_adi_adxl355.c \
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
        drv/drv_baro_rohm_bm1383a.c \
	drv/drv_mag_rohm_bm1422a.c 	
>>>>>>> d378d15eb (rohm sensor)

GLOBAL_INCLUDES += .
GLOBAL_DEFINES      += AOS_SENSOR
#GLOBAL_DEFINES      += AOS_SENSOR_MAG_ROHM_BM1422A
#GLOBAL_DEFINES      += AOS_SENSOR_BARO_ROHM_BM1383A

#GLOBAL_DEFINES      += AOS_SENSOR_HUMI_BOSCH_BME280
#GLOBAL_DEFINES      += AOS_SENSOR_ACC_BOSCH_BMA253
#GLOBAL_DEFINES      += AOS_SENSOR_BARO_BOSCH_BMP280
#GLOBAL_DEFINES      += AOS_SENSOR_ACC_ST_LSM6DSL
#GLOBAL_DEFINES      += AOS_SENSOR_GYRO_ST_LSM6DSL
#GLOBAL_DEFINES      += AOS_SENSOR_BARO_ST_LPS22HB
#GLOBAL_DEFINES      += AOS_SENSOR_ACC_SUPPORT_STEP
#GLOBAL_DEFINES      += AOS_SENSOR_MAG_ST_LIS3MDL  

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

#GLOBAL_DEFINES       += AOS_SENSOR_ACC_MIR3_DA217
#GLOBAL_DEFINES       += AOS_SENSOR_ALS_LITEON_LTR553
#GLOBAL_DEFINES       += AOS_SENSOR_PS_LITEON_LTR553
#GLOBAL_DEFINES      += AOS_SENSOR_TEMP_SENSIRION_SHTC1
#GLOBAL_DEFINES      += AOS_SENSOR_HUMI_SENSIRION_SHTC1
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

>>>>>>> e9139ca4c (add driver for adt7410 adxl345 adxl355)
