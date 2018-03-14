#ifndef __DRV_GYRO_ST_LSM6DSL_H
#define __DRV_GYRO_ST_LSM6DSL_H

typedef enum {
	GYRO_ST_LSM6DSL_ADDR_LOW = 0,
	GYRO_ST_LSM6DSL_ADDR_HIGH
} GYRO_ST_LSM6DSL_ADDR_SEL;

int drv_gyro_st_lsm6dsl_init(i2c_dev_t *i2c_dev, GYRO_ST_LSM6DSL_ADDR_SEL i2c_addr_sel);

#endif /* __DRV_GYRO_ST_LSM6DSL_H */