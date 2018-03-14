#ifndef __DRV_TEMP_ST_LSM6DSL_H
#define __DRV_TEMP_ST_LSM6DSL_H

typedef enum {
	TEMP_ST_LSM6DSL_ADDR_LOW = 0,
	TEMP_ST_LSM6DSL_ADDR_HIGH
} TEMP_ST_LSM6DSL_ADDR_SEL;

int drv_temp_st_lsm6dsl_init(i2c_dev_t *i2c_dev, TEMP_ST_LSM6DSL_ADDR_SEL i2c_addr_sel);

#endif /* __DRV_TEMP_ST_LSM6DSL_H */