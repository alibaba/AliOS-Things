/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef SENSOR_HAL_H
#define SENSOR_HAL_H

extern int sensor_hal_init(void);
extern int sensor_create_obj(sensor_obj_t *sensor);

#ifdef UDATA_MODBUS
extern int find_ModbusSensors(char *path);
extern int modbus_sensor_read(void *buffer, size_t length, int index);
extern void modbus_sensor_drv_init();
#endif


#ifdef AOS_SENSOR_HUMI_BOSCH_BME280
extern int drv_humi_bosch_bme280_init(void);
#endif /* AOS_SENSOR_HUMI_BOSCH_BME280 */

#ifdef AOS_SENSOR_ACC_BOSCH_BMA253
extern   int drv_acc_bosch_bma253_init(void);
#endif /* AOS_SENSOR_ACC_BOSCH_BMA253 */

#ifdef AOS_SENSOR_BARO_BOSCH_BMP280
extern   int drv_baro_bosch_bmp280_init(void);
#endif /* AOS_SENSOR_BARO_BOSCH_BMP280 */

#ifdef AOS_SENSOR_ACC_ST_LSM6DSL
extern   int drv_acc_st_lsm6dsl_init(void);
#endif /* AOS_SENSOR_ACC_ST_LSM6DSL */

#ifdef AOS_SENSOR_GYRO_ST_LSM6DSL
extern   int drv_gyro_st_lsm6dsl_init(void);
#endif /* AOS_SENSOR_GYRO_ST_LSM6DSL */

#ifdef AOS_SENSOR_BARO_ST_LPS22HB
extern   int drv_baro_st_lps22hb_init(void);
#endif /* AOS_SENSOR_BARO_ST_LPS22HB */


#ifdef AOS_SENSOR_ACC_MIR3_DA217
extern   int drv_acc_mir3_da217_init(void);
#endif /* AOS_SENSOR_ACC_MIR3_DA217 */

#ifdef AOS_SENSOR_ALS_LITEON_LTR553
extern   int drv_als_liteon_ltr553_init(void);
#endif /* AOS_SENSOR_ALS_LITEON_LTR553 */

#ifdef AOS_SENSOR_PS_LITEON_LTR553
extern   int drv_ps_liteon_ltr553_init(void);
#endif /* AOS_SENSOR_PS_LITEON_LTR553 */

#ifdef AOS_SENSOR_TEMP_SENSIRION_SHTC1
extern   int drv_temp_sensirion_shtc1_init(void);
#endif /* AOS_SENSOR_TEMP_SENSIRION_SHTC1 */

#ifdef AOS_SENSOR_HUMI_SENSIRION_SHTC1
extern   int drv_humi_sensirion_shtc1_init(void);
#endif /* AOS_SENSOR_HUMI_SENSIRION_SHTC1 */

#ifdef AOS_SENSOR_TEMP_SENSIRION_HTS221
extern   int drv_temp_st_hts221_init(void);
#endif /* AOS_SENSOR_TEMP_SENSIRION_HTS221 */

#ifdef AOS_SENSOR_HUMI_SENSIRION_HTS221
extern   int drv_humi_st_hts221_init(void);
#endif /* AOS_SENSOR_HUMI_SENSIRION_HTS221 */


#ifdef AOS_SENSOR_MAG_ST_LIS3MDL
extern   int drv_mag_st_lis3mdl_init(void);
#endif /*AOS_SENSOR_MAG_ST_LIS3MDL*/

#ifdef AOS_SENSOR_MAG_MEMSIC_MMC3680KJ
extern   int drv_mag_memsic_mmc3680kj_init(void);
#endif /* AOS_SENSOR_MAG_MEMSIC_MMC3680KJ */

#ifdef AOS_SENSOR_TEMP_MEMSIC_MMC3680KJ
extern   int drv_temp_memsic_mmc3680kj_init(void);
#endif /* AOS_SENSOR_TEMP_MEMSIC_MMC3680KJ */



#endif /* SENSOR_HAL_H */
