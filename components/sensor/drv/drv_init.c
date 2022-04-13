
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aos/kernel.h"
#include "sensor_hal.h"

void sensor_drv_init()
{
#if AOS_SENSOR_TEMP_SI_SI7006
    drv_temp_si_si7006_init();
#endif

#if AOS_SENSOR_HUMI_SI_SI7006
    drv_humi_si_si7006_init();
#endif

#if AOS_SENSOR_ACC_ADI_ADXL372
    drv_acc_adi_adxl372_init();
#endif

#if AOS_SENSOR_ACC_ADI_ADXL345
    drv_acc_adi_adxl345_init();
#endif

#if AOS_SENSOR_ACC_ADI_ADXL355
    drv_acc_adi_adxl355_init();
#endif

#if AOS_SENSOR_ACC_BOSCH_BMA253
    drv_acc_bosch_bma253_init();
#endif

#if AOS_SENSOR_ACC_BOSCH_BMA280
    drv_acc_bosch_bma280_init();
#endif

#if AOS_SENSOR_ACC_BOSCH_BMA421
    drv_acc_bosch_bma421_init();
#endif

#if AOS_SENSOR_ACC_BOSCH_BMA422
    drv_acc_bosch_bma422_init();
#endif

#if AOS_SENSOR_ACC_BOSCH_BMA455
    drv_acc_bosch_bma455_init();
#endif

#if AOS_SENSOR_ACC_BOSCH_BMA456
    drv_acc_bosch_bma456_init();
#endif

#if AOS_SENSOR_ACC_BOSCH_BMI055
    drv_acc_bosch_bmi055_init();
#endif
#if AOS_SENSOR_ACC_BOSCH_BMI088
    drv_acc_bosch_bmi088_init();
#endif
#if AOS_SENSOR_ACC_BOSCH_BMI120
    drv_acc_bosch_bmi120_init();
#endif
#if AOS_SENSOR_ACC_BOSCH_BMI160
    drv_acc_bosch_bmi160_init();
#endif
#if AOS_SENSOR_ACC_BOSCH_BMI260
    drv_acc_bosch_bmi260_init();
#endif
#if AOS_SENSOR_ACC_SENODIA_SH200L
    drv_acc_senodia_sh200l_init();
#endif
#if AOS_SENSOR_ACC_SENODIA_SH200Q
    drv_acc_senodia_sh200q_init();
#endif
#if AOS_SENSOR_ACC_ST_LSM6DS3
    drv_acc_st_lsm6ds3_init();
#endif
#if AOS_SENSOR_ACC_ST_LSM6DS3TR_C
    drv_acc_st_lsm6ds3tr_c_init();
#endif
#if AOS_SENSOR_ACC_ST_LSM6DSL
    drv_acc_st_lsm6dsl_init();
#endif

#if AOS_SENSOR_ACC_ST_LSM6DSM
    drv_acc_st_lsm6dsm_init();
#endif

#if AOS_SENSOR_ACC_ST_LSM6DSOQ
    drv_acc_st_lsm6dsoq_init();
#endif

#if AOS_SENSOR_ACC_ST_LSM6DSR
    drv_acc_st_lsm6dsr_init();
#endif

#if AOS_SENSOR_ACC_ST_LSM303AGR
    drv_acc_st_lsm303agr_init();
#endif

#if AOS_SENSOR_ACC_MIR3_DA213B
    drv_acc_mir3_da213B_init();
#endif

#if AOS_SENSOR_ACC_MIR3_DA215
    drv_acc_mir3_da215_init();
#endif

#if AOS_SENSOR_ACC_MIR3_DA217
    drv_acc_mir3_da217_init();
#endif

#if AOS_SENSOR_ACC_MIR3_DA270
    drv_acc_mir3_da270_init();
#endif

#if AOS_SENSOR_ACC_MIR3_DA312B
    drv_acc_mir3_da213B_init();
#endif

#if AOS_SENSOR_ACC_MIR3_DA380B
    drv_acc_mir3_da380B_init();
#endif

#if AOS_SENSOR_ACC_ST_AIS328DQ
    drv_acc_st_ais328dq_init();
#endif

#if AOS_SENSOR_ACC_ST_H3LIS100DL
    drv_acc_st_h3lis100dl_init();
#endif

#if AOS_SENSOR_ACC_ST_H3LIS331DL
    drv_acc_st_h3lis331dl_init();
#endif

#if AOS_SENSOR_ACC_ST_LIS2DH12
    drv_acc_st_lis2dh12_init();
#endif

#if AOS_SENSOR_ACC_ST_LIS2DW12
    drv_acc_st_lis2dw12_init();
#endif

#if AOS_SENSOR_ACC_ST_LIS2HH12
    drv_acc_st_lis2hh12_init();
#endif

#if AOS_SENSOR_ACC_ST_LIS3DH
    drv_acc_st_lis3dh_init();
#endif

#if AOS_SENSOR_ACC_ST_LIS331HH
    drv_acc_st_lis331hh_init();
#endif

#if AOS_SENSOR_ACC_ST_N2DM
    drv_acc_st_n2dm_init();
#endif

#if AOS_SENSOR_GYRO_BOSCH_BMI055
    drv_gyro_bosch_bmi055_init();
#endif

#if AOS_SENSOR_GYRO_BOSCH_BMI088
    drv_gyro_bosch_bmi088_init();
#endif

#if AOS_SENSOR_GYRO_BOSCH_BMI120
    drv_gyro_bosch_bmi120_init();
#endif

#if AOS_SENSOR_GYRO_BOSCH_BMI160
    drv_gyro_bosch_bmi160_init();
#endif

#if AOS_SENSOR_GYRO_BOSCH_BMI260
    drv_gyro_bosch_bmi260_init();
#endif

#if AOS_SENSOR_GYRO_SENODIA_SH200L
    drv_gyro_senodia_sh200l_init();
#endif

#if AOS_SENSOR_GYRO_SENODIA_SH200Q
    drv_gyro_senodia_sh200q_init();
#endif

#if AOS_SENSOR_GYRO_ST_LSM6DS3
    drv_gyro_st_lsm6ds3_init();
#endif

#if AOS_SENSOR_GYRO_ST_LSM6DS3TR_C
    drv_gyro_st_lsm6ds3tr_c_init();
#endif

#if AOS_SENSOR_GYRO_ST_LSM6DSL
    drv_gyro_st_lsm6dsl_init();
#endif

#if AOS_SENSOR_GYRO_ST_LSM6DSM
    drv_gyro_st_lsm6dsm_init();
#endif

#if AOS_SENSOR_GYRO_ST_LSM6DSOQ
    drv_gyro_st_lsm6dsoq_init();
#endif
#if AOS_SENSOR_GYRO_ST_LSM6DSR
    drv_gyro_st_lsm6dsr_init();
#endif
#if AOS_SENSOR_GYRO_BOSCH_BMG160
    drv_gyro_bosch_bmg160_init();
#endif
#if AOS_SENSOR_GYRO_ST_A3G4250D
    drv_gyro_st_a3g4250d_init();
#endif
#if AOS_SENSOR_GYRO_ST_I3G4250D
    drv_gyro_st_i3g4250d_init();
#endif
#if AOS_SENSOR_GYRO_ST_L3GD20H
    drv_gyro_st_l3gd20h_init();
#endif
#if AOS_SENSOR_MAG_BOSCH_BMM150
    drv_mag_bosch_bmm150_init();
#endif
#if AOS_SENSOR_MAG_ST_LSM303AGR
    drv_mag_st_lsm303agr_init();
#endif
#if AOS_SENSOR_MAG_AKM_AK09917
    drv_mag_akm_ak09917_init();
#endif
#if AOS_SENSOR_MAG_AKM_AK09918
    drv_mag_akm_ak09918_init();
#endif
#if AOS_SENSOR_MAG_AKM_AK09940
    drv_mag_akm_ak09940_init();
#endif
#if AOS_SENSOR_MAG_ROHM_BM1422A
    drv_mag_rohm_bm1422a_init();
#endif
#if AOS_SENSOR_MAG_SENODIA_ST350
    drv_mag_sen_st350_init();
#endif
#if AOS_SENSOR_MAG_SENODIA_ST480
    drv_mag_sen_st480_init();
#endif
#if AOS_SENSOR_MAG_ST_LIS2MDL
    drv_mag_st_lis2mdl_init();
#endif
#if AOS_SENSOR_MAG_ST_LIS3MDL
    drv_mag_st_lis3mdl_init();
#endif
#if AOS_SENSOR_MAG_MEMSIC_MMC3680KJ
    drv_mag_memsic_mmc3680kj_init();
#endif
#if AOS_SENSOR_ALS_AMS_TCS3400
    drv_als_ams_tcs3400_init();
#endif
#if AOS_SENSOR_ALS_AMS_TMD2725
    drv_als_ams_tmd2725_init();
#endif
#if AOS_SENSOR_ALS_LITEON_LTR303
    drv_als_liteon_ltr303_init();
#endif
#if AOS_SENSOR_ALS_LITEON_LTR568
    drv_als_liteon_ltr568_init();
#endif
#if AOS_SENSOR_ALS_LITEON_LTR507
    drv_als_liteon_ltr507_init();
#endif
#if AOS_SENSOR_ALS_LITEON_LTR553
    drv_als_liteon_ltr553_init();
#endif
#if AOS_SENSOR_ALS_LITEON_LTR559
    drv_als_liteon_ltr559_init();
#endif
#if AOS_SENSOR_ALS_ROHM_BH1730
    drv_als_rohm_bh1730_init();
#endif
#if AOS_SENSOR_PS_AMS_TMD2725
    drv_ps_ams_tmd2725_init();
#endif
#if AOS_SENSOR_PS_LITEON_LTR507
    drv_ps_liteon_ltr507_init();
#endif
#if AOS_SENSOR_PS_LITEON_LTR553
    drv_ps_liteon_ltr553_init();
#endif
#if AOS_SENSOR_PS_LITEON_LTR559
    drv_ps_liteon_ltr559_init();
#endif
#if AOS_SENSOR_PS_LITEON_LTR659
    drv_ps_liteon_ltr659_init();
#endif

#if AOS_SENSOR_PS_LITEON_LTR690
    drv_ps_liteon_ltr690_init();
#endif
#if AOS_SENSOR_PS_LITEON_LTR706
    drv_ps_liteon_ltr706_init();
#endif
#if AOS_SENSOR_BARO_IFX_DSP310
    drv_baro_ifx_dps310_init();
#endif
#if AOS_SENSOR_BARO_BOSCH_BMP280
    drv_baro_bosch_bmp280_init();
#endif
#if AOS_SENSOR_BARO_BOSCH_BMP380
    drv_baro_bosch_bmp380_init();
#endif
#if AOS_SENSOR_BARO_ROHM_BM1383A
    drv_baro_rohm_bm1383a_init();
#endif
#if AOS_SENSOR_BARO_ST_LPS22HB
    drv_baro_st_lps22hb_init();
#endif
#if AOS_SENSOR_BARO_ST_LPS33HB
    drv_baro_st_lps33hb_init();
#endif
#if AOS_SENSOR_BARO_ST_LPS35HB
    drv_baro_st_lps35hb_init();
#endif
#if AOS_SENSOR_BARO_BOSCH_BME280
    drv_humi_bosch_bme280_init();
#endif
#if AOS_SENSOR_GESTURE_PIXART_PAJ7620
    drv_gs_pixart_paj7620_init();
#endif
#if AOS_SENSOR_IR_AKM_AK9754
    drv_ir_akm_ak9754_init();
#endif
#if AOS_SENSOR_RGB_LITEON_LTR381
    drv_rgb_liteon_ltr381_init();
#endif
#if AOS_SENSOR_RTC_MAXIM_DS1307
    drv_rtc_maxim_ds1307_init();
#endif
#if AOS_SENSOR_UV_LITEON_LTR390
    drv_uv_liteon_ltr390_init();
#endif
#if AOS_SENSOR_TEMP_AMS_ENS210
    drv_temp_ams_ens210_init();
#endif
#if AOS_SENSOR_TEMP_ADI_ADT7410
    drv_temp_adi_adt7410_init();
#endif
#if AOS_SENSOR_TEMP_BOSCH_BME280
    drv_temp_bosch_bme280_init();
#endif
#if AOS_SENSOR_TEMP_SENSIRION_SHT30
    drv_temp_sensirion_sht30_init();
#endif
#if AOS_SENSOR_TEMP_SENSIRION_SHT31
    drv_temp_sensirion_sht31_init();
#endif
#if AOS_SENSOR_TEMP_SENSIRION_SHTC1
    drv_temp_sensirion_shtc1_init();
#endif
#if AOS_SENSOR_TEMP_ST_HTS221
    drv_temp_st_hts221_init();
#endif
#if AOS_SENSOR_HUMI_AMS_ENS210
    drv_humi_ams_ens210_init();
#endif
#if AOS_SENSOR_HUMI_BOSCH_BME280
    drv_baro_bosch_bme280_init();
#endif
#if AOS_SENSOR_HUMI_SENSIRION_SHT30
    drv_humi_sensirion_sht30_init();
#endif
#if AOS_SENSOR_HUMI_SENSIRION_SHT31
    drv_humi_sensirion_sht31_init();
#endif
#if AOS_SENSOR_HUMI_SENSIRION_SHTC1
    drv_humi_sensirion_shtc1_init();
#endif
#if AOS_SENSOR_HUMI_ST_HTS221
    drv_humi_st_hts221_init();
#endif
#if AOS_SENSOR_CO2_SENSIRION_SCD30
    drv_co2_sensirion_scd30_init();
#endif
#if AOS_SENSOR_ECG_ADI_ADPD188GG
    drv_ecg_adi_adpd188gg_init();
#endif

#if AOS_SENSOR_TVOC_SENSIRION_SGP30
    drv_tvoc_sensirion_sgp30_init();
#endif
#if AOS_SENSOR_TVOC_AMS_CCS811
    drv_voc_ams_ccs811_init();
#endif
#if AOS_SENSOR_HUMI_BME280_SPI
    drv_humi_bosch_bme280_init();
#endif
#if AOS_SENSOR_BARO_BME280_SPI
    drv_baro_bosch_bme280_init();
#endif
#if AOS_SENSOR_TEMP_BME280_SPI
    drv_temp_bosch_bme280_init();
#endif
#if AOS_SENSOR_CANBUS_INV_MPU9250
    drv_canbus_acc_inv_mpu9250_init();
#endif
#if AOS_SENSOR_PS_ST_VL53L0X
    drv_ps_st_vl53l0x_init();
#endif
}
