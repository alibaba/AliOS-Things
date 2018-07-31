NAME := sensor

$(NAME)_TYPE := kernel

$(NAME)_SOURCES += \
        hal/sensor_hal.c \
        hal/sensor_drv_api.c \
        drv/drv_acc_bosch_bma253.c \
        drv/drv_baro_bosch_bmp280.c \
        drv/drv_acc_gyro_st_lsm6dsl.c\
        drv/drv_acc_gyro_st_lsm6ds3tr_c.c\
        drv/drv_acc_gyro_st_lsm6ds3.c\
        drv/drv_acc_gyro_st_lsm6dsoq.c\
        drv/drv_acc_gyro_st_lsm6dsm.c\
        drv/drv_acc_gyro_st_lsm6dsr.c\
        drv/drv_baro_st_lps22hb.c \
        drv/drv_baro_st_lps33hb.c \
        drv/drv_baro_st_lps35hb.c \
        drv/drv_acc_mir3_da217.c \
        drv/drv_als_ps_liteon_ltr553.c \
        drv/drv_temp_humi_sensirion_shtc1.c \
        drv/drv_temp_humi_st_hts221.c \
        drv/drv_mag_st_lis3mdl.c \
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
		

GLOBAL_INCLUDES += .
GLOBAL_DEFINES      += AOS_SENSOR

#GLOBAL_DEFINES      += AOS_SENSOR_HUMI_BOSCH_BME280
#GLOBAL_DEFINES      += AOS_SENSOR_ACC_BOSCH_BMA253
#GLOBAL_DEFINES      += AOS_SENSOR_BARO_BOSCH_BMP280
#GLOBAL_DEFINES      += AOS_SENSOR_ACC_ST_LSM6DSL
#GLOBAL_DEFINES      += AOS_SENSOR_GYRO_ST_LSM6DSL
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
#GLOBAL_DEFINES      += AOS_SENSOR_BARO_ST_LPS22HB
#GLOBAL_DEFINES      += AOS_SENSOR_BARO_ST_LPS33HB
#GLOBAL_DEFINES      += AOS_SENSOR_BARO_ST_LPS35HB
#GLOBAL_DEFINES      += AOS_SENSOR_ACC_SUPPORT_STEP
#GLOBAL_DEFINES      += AOS_SENSOR_MAG_ST_LIS3MDL  
#GLOBAL_DEFINES      += AOS_SENSOR_MAG_ST_LIS2MDL  
#GLOBAL_DEFINES      += AOS_SENSOR_ACC_ST_LSM303AGR  
#GLOBAL_DEFINES      += AOS_SENSOR_MAG_ST_LSM303AGR  
#GLOBAL_DEFINES      += AOS_SENSOR_TEMP_ST_HTS221
#GLOBAL_DEFINES      += AOS_SENSOR_HUMI_ST_HTS221
#GLOBAL_DEFINES      += AOS_SENSOR_ACC_ST_LIS2DH12
#GLOBAL_DEFINES      += AOS_SENSOR_ACC_ST_LIS2DW12
#GLOBAL_DEFINES      += AOS_SENSOR_ACC_ST_LIS3DH
#GLOBAL_DEFINES      += AOS_SENSOR_ACC_ST_LIS2HH12
#GLOBAL_DEFINES      += AOS_SENSOR_ACC_ST_N2DM
#GLOBAL_DEFINES      += AOS_SENSOR_ACC_ST_AIS328DQ
#GLOBAL_DEFINES      += AOS_SENSOR_ACC_ST_LIS331HH
#GLOBAL_DEFINES      += AOS_SENSOR_ACC_ST_H3LIS331DL
#GLOBAL_DEFINES      += AOS_SENSOR_ACC_ST_H3LIS100DL
#GLOBAL_DEFINES      += AOS_SENSOR_GYRO_ST_L3GD20H
#GLOBAL_DEFINES      += AOS_SENSOR_GYRO_ST_I3G4250D
#GLOBAL_DEFINES      += AOS_SENSOR_GYRO_ST_A3G4250D


#GLOBAL_DEFINES       += AOS_SENSOR_ACC_MIR3_DA217
#GLOBAL_DEFINES       += AOS_SENSOR_ALS_LITEON_LTR553
#GLOBAL_DEFINES       += AOS_SENSOR_PS_LITEON_LTR553
#GLOBAL_DEFINES      += AOS_SENSOR_TEMP_SENSIRION_SHTC1
#GLOBAL_DEFINES      += AOS_SENSOR_HUMI_SENSIRION_SHTC1


