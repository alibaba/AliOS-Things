
ifeq ($(AOS_SENSOR_ACC_ADI_ADXL372),y)
$(NAME)_SOURCES += drv/drv_acc_adi_adxl372.c
endif
ifeq ($(AOS_SENSOR_ACC_ADI_ADXL345),y)
$(NAME)_SOURCES += drv/drv_acc_adi_adxl345.c
endif
ifeq ($(AOS_SENSOR_ACC_ADI_ADXL355),y)
$(NAME)_SOURCES += drv/drv_acc_adi_adxl355.c
endif
ifeq ($(AOS_SENSOR_ACC_BOSCH_BMA253),y)
$(NAME)_SOURCES += drv/drv_acc_bosch_bma253.c
endif
ifeq ($(AOS_SENSOR_ACC_BOSCH_BMA280),y)
$(NAME)_SOURCES += drv/drv_acc_bosch_bma280.c
endif
ifeq ($(AOS_SENSOR_ACC_BOSCH_BMA421),y)
$(NAME)_SOURCES += drv/drv_acc_bosch_bma421.c
endif
ifeq ($(AOS_SENSOR_ACC_BOSCH_BMA422),y)
$(NAME)_SOURCES += drv/drv_acc_bosch_bma422.c
endif
ifeq ($(AOS_SENSOR_ACC_BOSCH_BMA455),y)
$(NAME)_SOURCES += drv/drv_acc_bosch_bma455.c
endif
ifeq ($(AOS_SENSOR_ACC_BOSCH_BMA456),y)
$(NAME)_SOURCES += drv/drv_acc_bosch_bma456.c
endif
ifeq ($(AOS_SENSOR_ACC_BOSCH_BMI055),y)
$(NAME)_SOURCES += drv/drv_acc_gyro_bosch_bmi055.c
endif
ifeq ($(AOS_SENSOR_ACC_BOSCH_BMI088),y)
$(NAME)_SOURCES += drv/drv_acc_gyro_bosch_bmi088.c
endif
ifeq ($(AOS_SENSOR_ACC_BOSCH_BMI120),y)
$(NAME)_SOURCES += drv/drv_acc_gyro_bosch_bmi120.c
endif
ifeq ($(AOS_SENSOR_ACC_BOSCH_BMI160),y)
$(NAME)_SOURCES += drv/drv_acc_gyro_bosch_bmi160.c
endif
ifeq ($(AOS_SENSOR_ACC_BOSCH_BMI260),y)
$(NAME)_SOURCES += drv/drv_acc_gyro_bosch_bmi260.c
endif
ifeq ($(AOS_SENSOR_ACC_SENODIA_SH200L),y)
$(NAME)_SOURCES += drv/drv_acc_gyro_senodia_sh200l.c
endif
ifeq ($(AOS_SENSOR_ACC_SENODIA_SH200Q),y)
$(NAME)_SOURCES += drv/drv_acc_gyro_senodia_sh200q.c
endif
ifeq ($(AOS_SENSOR_ACC_ST_LSM6DS3),y)
$(NAME)_SOURCES += drv/drv_acc_gyro_st_lsm6ds3.c
endif
ifeq ($(AOS_SENSOR_ACC_ST_LSM6DS3TR_C),y)
$(NAME)_SOURCES += drv/drv_acc_gyro_st_lsm6ds3tr_c.c
endif
ifeq ($(AOS_SENSOR_ACC_ST_LSM6DSL),y)
$(NAME)_SOURCES += drv/drv_acc_gyro_st_lsm6dsl.c
endif
ifeq ($(AOS_SENSOR_ACC_ST_LSM6DSM),y)
$(NAME)_SOURCES += drv/drv_acc_gyro_st_lsm6dsm.c
endif
ifeq ($(AOS_SENSOR_ACC_ST_LSM6DSOQ),y)
$(NAME)_SOURCES += drv/drv_acc_gyro_st_lsm6dsoq.c
endif
ifeq ($(AOS_SENSOR_ACC_ST_LSM6DSR),y)
$(NAME)_SOURCES += drv/drv_acc_gyro_st_lsm6dsr.c
endif
ifeq ($(AOS_SENSOR_ACC_ST_LSM303AGR),y)
$(NAME)_SOURCES += drv/drv_acc_mag_st_lsm303agr.c
endif
ifeq ($(AOS_SENSOR_ACC_MIR3_DA213B),y)
$(NAME)_SOURCES += drv/drv_acc_mir3_da213B.c
endif
ifeq ($(AOS_SENSOR_ACC_MIR3_DA215),y)
$(NAME)_SOURCES += drv/drv_acc_mir3_da215.c
endif
ifeq ($(AOS_SENSOR_ACC_MIR3_DA217),y)
$(NAME)_SOURCES += drv/drv_acc_mir3_da217.c
endif
ifeq ($(AOS_SENSOR_ACC_MIR3_DA270),y)
$(NAME)_SOURCES += drv/drv_acc_mir3_da270.c
endif
ifeq ($(AOS_SENSOR_ACC_MIR3_DA312B),y)
$(NAME)_SOURCES += drv/drv_acc_mir3_da312B.c
endif
ifeq ($(AOS_SENSOR_ACC_MIR3_DA380B),y)
$(NAME)_SOURCES += drv/drv_acc_mir3_da380B.c
endif
ifeq ($(AOS_SENSOR_ACC_ST_AIS328DQ),y)
$(NAME)_SOURCES += drv/drv_acc_st_ais328dq.c
endif
ifeq ($(AOS_SENSOR_ACC_ST_H3LIS100DL),y)
$(NAME)_SOURCES += drv/drv_acc_st_h3lis100dl.c
endif
ifeq ($(AOS_SENSOR_ACC_ST_H3LIS331DL),y)
$(NAME)_SOURCES += drv/drv_acc_st_h3lis331dl.c
endif
ifeq ($(AOS_SENSOR_ACC_ST_LIS2DH12),y)
$(NAME)_SOURCES += drv/drv_acc_st_lis2dh12.c
endif
ifeq ($(AOS_SENSOR_ACC_ST_LIS2DW12),y)
$(NAME)_SOURCES += drv/drv_acc_st_lis2dw12.c
endif
ifeq ($(AOS_SENSOR_ACC_ST_LIS2HH12),y)
$(NAME)_SOURCES += drv/drv_acc_st_lis2hh12.c
endif
ifeq ($(AOS_SENSOR_ACC_ST_LIS3DH),y)
$(NAME)_SOURCES += drv/drv_acc_st_lis3dh.c
endif
ifeq ($(AOS_SENSOR_ACC_ST_LIS331HH),y)
$(NAME)_SOURCES += drv/drv_acc_st_lis331hh.c
endif
ifeq ($(AOS_SENSOR_ACC_ST_N2DM),y)
$(NAME)_SOURCES += drv/drv_acc_st_n2dm.c
endif
ifeq ($(AOS_SENSOR_GYRO_BOSCH_BMI055),y)
$(NAME)_SOURCES += drv/drv_acc_gyro_bosch_bmi055.c
endif
ifeq ($(AOS_SENSOR_GYRO_BOSCH_BMI088),y)
$(NAME)_SOURCES += drv/drv_acc_gyro_bosch_bmi088.c
endif
ifeq ($(AOS_SENSOR_GYRO_BOSCH_BMI120),y)
$(NAME)_SOURCES += drv/drv_acc_gyro_bosch_bmi120.c
endif
ifeq ($(AOS_SENSOR_GYRO_BOSCH_BMI160),y)
$(NAME)_SOURCES += drv/drv_acc_gyro_bosch_bmi160.c
endif
ifeq ($(AOS_SENSOR_GYRO_BOSCH_BMI260),y)
$(NAME)_SOURCES += drv/drv_acc_gyro_bosch_bmi260.c
endif
ifeq ($(AOS_SENSOR_GYRO_SENODIA_SH200L),y)
$(NAME)_SOURCES += drv/drv_acc_gyro_senodia_sh200l.c
endif
ifeq ($(AOS_SENSOR_GYRO_SENODIA_SH200Q),y)
$(NAME)_SOURCES += drv/drv_acc_gyro_senodia_sh200q.c
endif
ifeq ($(AOS_SENSOR_GYRO_ST_LSM6DS3),y)
$(NAME)_SOURCES += drv/drv_acc_gyro_st_lsm6ds3.c
endif
ifeq ($(AOS_SENSOR_GYRO_ST_LSM6DS3TR_C),y)
$(NAME)_SOURCES += drv/drv_acc_gyro_st_lsm6ds3tr_c.c
endif
ifeq ($(AOS_SENSOR_GYRO_ST_LSM6DSL),y)
$(NAME)_SOURCES += drv/drv_acc_gyro_st_lsm6dsl.c
endif
ifeq ($(AOS_SENSOR_GYRO_ST_LSM6DSM),y)
$(NAME)_SOURCES += drv/drv_acc_gyro_st_lsm6dsm.c
endif
ifeq ($(AOS_SENSOR_GYRO_ST_LSM6DSOQ),y)
$(NAME)_SOURCES += drv/drv_acc_gyro_st_lsm6dsoq.c
endif
ifeq ($(AOS_SENSOR_GYRO_ST_LSM6DSR),y)
$(NAME)_SOURCES += drv/drv_acc_gyro_st_lsm6dsr.c
endif
ifeq ($(AOS_SENSOR_GYRO_BOSCH_BMG160),y)
$(NAME)_SOURCES += drv/drv_gyro_bosch_bmg160.c
endif
ifeq ($(AOS_SENSOR_GYRO_ST_A3G4250D),y)
$(NAME)_SOURCES += drv/drv_gyro_st_a3g4250d.c
endif
ifeq ($(AOS_SENSOR_GYRO_ST_I3G4250D),y)
$(NAME)_SOURCES += drv/drv_gyro_st_i3g4250d.c
endif
ifeq ($(AOS_SENSOR_GYRO_ST_L3GD20H),y)
$(NAME)_SOURCES += drv/drv_gyro_st_l3gd20h.c
endif

ifeq ($(AOS_SENSOR_MAG_BOSCH_BMM150),y)
$(NAME)_SOURCES += drv/drv_mag_bosch_bmm150.c
endif
ifeq ($(AOS_SENSOR_MAG_ST_LSM303AGR),y)
$(NAME)_SOURCES += drv/drv_acc_mag_st_lsm303agr.c
endif
ifeq ($(AOS_SENSOR_MAG_AKM_AK09917),y)
$(NAME)_SOURCES += drv/drv_mag_akm_ak09917.c
endif
ifeq ($(AOS_SENSOR_MAG_AKM_AK09918),y)
$(NAME)_SOURCES += drv/drv_mag_akm_ak09918.c
endif
ifeq ($(AOS_SENSOR_MAG_AKM_AK09940),y)
$(NAME)_SOURCES += drv/drv_mag_akm_ak09940.c
endif
ifeq ($(AOS_SENSOR_MAG_ROHM_BM1422A),y)
$(NAME)_SOURCES += drv/drv_mag_rohm_bm1422a.c
endif
ifeq ($(AOS_SENSOR_MAG_SENODIA_ST350),y)
$(NAME)_SOURCES += drv/drv_mag_senodia_st350.c
endif
ifeq ($(AOS_SENSOR_MAG_SENODIA_ST480),y)
$(NAME)_SOURCES += drv/drv_mag_senodia_st480.c
endif
ifeq ($(AOS_SENSOR_MAG_ST_LIS2MDL),y)
$(NAME)_SOURCES += drv/drv_mag_st_lis2mdl.c
endif
ifeq ($(AOS_SENSOR_MAG_ST_LIS3MDL),y)
$(NAME)_SOURCES += drv/drv_mag_st_lis3mdl.c
endif
ifeq ($(AOS_SENSOR_MAG_MEMSIC_MMC3680KJ),y)
$(NAME)_SOURCES += drv/drv_mag_temp_memsic_mmc3680kj.c
endif
ifeq ($(AOS_SENSOR_ALS_AMS_TCS3400),y)
$(NAME)_SOURCES += drv/drv_als_ams_tcs3400.c
endif
ifeq ($(AOS_SENSOR_ALS_AMS_TMD2725),y)
$(NAME)_SOURCES += drv/drv_als_ps_ams_tmd2725.c
endif
ifeq ($(AOS_SENSOR_ALS_LITEON_LTR303),y)
$(NAME)_SOURCES += drv/drv_als_liteon_ltr303.c
endif
ifeq ($(AOS_SENSOR_ALS_LITEON_LTR568),y)
$(NAME)_SOURCES += drv/drv_als_liteon_ltr568.c
endif
ifeq ($(AOS_SENSOR_ALS_LITEON_LTR507),y)
$(NAME)_SOURCES += drv/drv_als_ps_liteon_ltr507.c
endif
ifeq ($(AOS_SENSOR_ALS_LITEON_LTR553),y)
$(NAME)_SOURCES += drv/drv_als_ps_liteon_ltr553.c
endif
ifeq ($(AOS_SENSOR_ALS_LITEON_LTR559),y)
$(NAME)_SOURCES += drv/drv_als_ps_liteon_ltr559.c
endif
ifeq ($(AOS_SENSOR_ALS_ROHM_BH1730),y)
$(NAME)_SOURCES += drv/drv_als_rohm_bh1730.c
endif
ifeq ($(AOS_SENSOR_PS_AMS_TMD2725),y)
$(NAME)_SOURCES += drv/drv_als_ps_ams_tmd2725.c
endif
ifeq ($(AOS_SENSOR_PS_LITEON_LTR507),y)
$(NAME)_SOURCES += drv/drv_als_ps_liteon_ltr507.c
endif
ifeq ($(AOS_SENSOR_PS_LITEON_LTR553),y)
$(NAME)_SOURCES += drv/drv_als_ps_liteon_ltr553.c
endif
ifeq ($(AOS_SENSOR_PS_LITEON_LTR559),y)
$(NAME)_SOURCES += drv/drv_als_ps_liteon_ltr559.c
endif
ifeq ($(AOS_SENSOR_PS_LITEON_LTR659),y)
$(NAME)_SOURCES += drv/drv_ps_liteon_ltr659.c
endif
ifeq ($(AOS_SENSOR_PS_LITEON_LTR690),y)
$(NAME)_SOURCES += drv/drv_ps_liteon_ltr690.c
endif
ifeq ($(AOS_SENSOR_PS_LITEON_LTR706),y)
$(NAME)_SOURCES += drv/drv_ps_liteon_ltr706.c
endif

ifeq ($(AOS_SENSOR_BARO_IFX_DSP310),y)
$(NAME)_SOURCES += drv/drv_baro_ifx_dps310.c
endif
ifeq ($(AOS_SENSOR_BARO_BOSCH_BMP280),y)
$(NAME)_SOURCES += drv/drv_baro_bosch_bmp280.c
endif
ifeq ($(AOS_SENSOR_BARO_BOSCH_BMP380),y)
$(NAME)_SOURCES += drv/drv_baro_bosch_bmp38x.c
endif
ifeq ($(AOS_SENSOR_BARO_ROHM_BM1383A),y)
$(NAME)_SOURCES += drv/drv_baro_rohm_bm1383a.c
endif
ifeq ($(AOS_SENSOR_BARO_ST_LPS22HB),y)
$(NAME)_SOURCES += drv/drv_baro_st_lps22hb.c
endif
ifeq ($(AOS_SENSOR_BARO_ST_LPS33HB),y)
$(NAME)_SOURCES += drv/drv_baro_st_lps33hb.c
endif
ifeq ($(AOS_SENSOR_BARO_ST_LPS35HB),y)
$(NAME)_SOURCES += drv/drv_baro_st_lps35hb.c
endif
ifeq ($(AOS_SENSOR_BARO_BOSCH_BME280),y)
$(NAME)_SOURCES += drv/drv_temp_humi_baro_bosch_bme280.c
endif
ifeq ($(AOS_SENSOR_GESTURE_PIXART_PAJ7620),y)
$(NAME)_SOURCES += drv/drv_gs_pixart_paj7620.c
endif
ifeq ($(AOS_SENSOR_IR_AKM_AK9754),y)
$(NAME)_SOURCES += drv/drv_ir_akm_ak9754.c
endif
ifeq ($(AOS_SENSOR_RGB_LITEON_LTR381),y)
$(NAME)_SOURCES += drv/drv_rgb_liteon_ltr381.c
endif
ifeq ($(AOS_SENSOR_RTC_MAXIM_DS1307),y)
$(NAME)_SOURCES += drv/drv_rtc_maxim_ds1307.c
endif
ifeq ($(AOS_SENSOR_UV_LITEON_LTR390),y)
$(NAME)_SOURCES += drv/drv_uv_liteon_ltr390.c
endif

ifeq ($(AOS_SENSOR_TEMP_AMS_ENS210),y)
$(NAME)_SOURCES += drv/drv_temp_humi_ams_ens210.c
endif
ifeq ($(AOS_SENSOR_TEMP_ADI_ADT7410),y)
$(NAME)_SOURCES += drv/drv_temp_adi_adt7410.c
endif
ifeq ($(AOS_SENSOR_TEMP_BOSCH_BME280),y)
$(NAME)_SOURCES += drv/drv_temp_humi_baro_bosch_bme280.c
endif
ifeq ($(AOS_SENSOR_TEMP_SENSIRION_SHT30),y)
$(NAME)_SOURCES += drv/drv_temp_humi_sensirion_sht30.c
endif
ifeq ($(AOS_SENSOR_TEMP_SENSIRION_SHT31),y)
$(NAME)_SOURCES += drv/drv_temp_humi_sensirion_sht31.c
endif
ifeq ($(AOS_SENSOR_TEMP_SENSIRION_SHTC1),y)
$(NAME)_SOURCES += drv/drv_temp_humi_sensirion_shtc1.c
endif
ifeq ($(AOS_SENSOR_TEMP_ST_HTS221),y)
$(NAME)_SOURCES += drv/drv_temp_humi_st_hts221.c
endif

ifeq ($(AOS_SENSOR_HUMI_AMS_ENS210),y)
$(NAME)_SOURCES += drv/drv_temp_humi_ams_ens210.c
endif
ifeq ($(AOS_SENSOR_HUMI_BOSCH_BME280),y)
$(NAME)_SOURCES += drv/drv_temp_humi_baro_bosch_bme280.c
endif
ifeq ($(AOS_SENSOR_HUMI_SENSIRION_SHT30),y)
$(NAME)_SOURCES += drv/drv_temp_humi_sensirion_sht30.c
endif
ifeq ($(AOS_SENSOR_HUMI_SENSIRION_SHT31),y)
$(NAME)_SOURCES += drv/drv_temp_humi_sensirion_sht31.c
endif
ifeq ($(AOS_SENSOR_HUMI_SENSIRION_SHTC1),y)
$(NAME)_SOURCES += drv/drv_temp_humi_sensirion_shtc1.c
endif
ifeq ($(AOS_SENSOR_HUMI_ST_HTS221),y)
$(NAME)_SOURCES += drv/drv_temp_humi_st_hts221.c
endif

ifeq ($(AOS_SENSOR_CO2_SENSIRION_SCD30),y)
$(NAME)_SOURCES += drv/drv_co2_sensirion_scd30.c
endif

ifeq ($(AOS_SENSOR_ECG_ADI_ADPD188GG),y)
$(NAME)_SOURCES += drv/drv_ecg_adi_adpd188gg.c
endif

ifeq ($(AOS_SENSOR_TVOC_SENSIRION_SGP30),y)
$(NAME)_SOURCES += drv/drv_tvoc_sensirion_sgp30.c
endif

ifeq ($(AOS_SENSOR_TVOC_AMS_CCS811),y)
$(NAME)_SOURCES += drv/drv_tvoc_ams_ccs811.c
endif


ifeq ($(AOS_SENSOR_HUMI_BME280_SPI),y)
$(NAME)_SOURCES += drv/drv_temp_humi_baro_bosch_bme280.c
endif

ifeq ($(AOS_SENSOR_BARO_BME280_SPI),y)
$(NAME)_SOURCES += drv/drv_temp_humi_baro_bosch_bme280.c
endif

ifeq ($(AOS_SENSOR_TEMP_BME280_SPI),y)
$(NAME)_SOURCES += drv/drv_temp_humi_baro_bosch_bme280.c
endif

ifeq ($(AOS_SENSOR_CANBUS_INV_MPU9250),y)
$(NAME)_SOURCES += drv/drv_canbus_inv_mpu9250.c
endif

ifeq ($(AOS_SENSOR_PS_ST_VL53L0X),y)
$(NAME)_SOURCES += drv/drv_ps_st_vl53l0x/drv_ps_st_vl53l0x.c
$(NAME)_SOURCES += drv/drv_ps_st_vl53l0x/vl53l0x_platform.c
$(NAME)_SOURCES += drv/drv_ps_st_vl53l0x/vl53l0x/vl53l0x_api.c
$(NAME)_SOURCES += drv/drv_ps_st_vl53l0x/vl53l0x/vl53l0x_api_calibration.c
$(NAME)_SOURCES += drv/drv_ps_st_vl53l0x/vl53l0x/vl53l0x_api_core.c
$(NAME)_SOURCES += drv/drv_ps_st_vl53l0x/vl53l0x/vl53l0x_api_ranging.c
$(NAME)_SOURCES += drv/drv_ps_st_vl53l0x/vl53l0x/vl53l0x_api_strings.c
$(NAME)_SOURCES += drv/drv_ps_st_vl53l0x/vl53l0x/vl53l0x_platform_log.c
endif

